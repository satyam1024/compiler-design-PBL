import http from "http";
import express from "express";
import cors from "cors";
import {
  writeFile,
  unlink,
  mkdir,
  rm,
  readFile,
  readdir,
  stat,
} from "fs/promises";
import { execFile, spawn } from "child_process";
import { promisify } from "util";
import path from "path";
import { fileURLToPath } from "url";
import { Server as SocketServer } from "socket.io";
import chokidar from "chokidar";
import pty from "node-pty";

// Simulate __dirname in ES modules
const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

// === Setup Express and HTTP Server ===
const app = express();
const server = http.createServer(app);
const io = new SocketServer(server, {
  cors: {
    origin: "*",
  },
});

app.use(cors());
app.use(express.json());

// === Terminal Setup with node-pty ===
const userDir = path.join(__dirname, "user");
await mkdir(userDir, { recursive: true });

const ptyProcess = pty.spawn("powershell.exe", [], {
  name: "xterm-color",
  cols: 80,
  rows: 30,
  cwd: userDir,
  env: process.env,
});

ptyProcess.onData((data) => {
  io.emit("terminal:data", data);
});

// === Socket.IO ===
io.on("connection", (socket) => {
  console.log("✅ Socket connected:", socket.id);
  socket.emit("file:refresh");

  // Live terminal input from frontend
  socket.on("terminal:write", (data) => {
    ptyProcess.write(data);
  });

  // Save file content from frontend
  socket.on("file:change", async ({ path: filePath, content }) => {
    const fullPath = path.join(userDir, filePath);
    await mkdir(path.dirname(fullPath), { recursive: true });
    await writeFile(fullPath, content);
  });

  // === C Code Compilation + Execution ===
  socket.on("run:c", async () => {
    const cFilePath = path.join(userDir, "code.c");
    const exePath = path.join(userDir, "a.exe");

    // Step 1: Compile using gcc
    const gcc = spawn("gcc", [cFilePath, "-o", exePath]);

    gcc.stderr.on("data", (data) => {
      socket.emit("terminal:data", `⚠️ gcc error:\n${data.toString()}`);
    });

    gcc.on("close", (code) => {
      if (code !== 0) {
        socket.emit(
          "terminal:data",
          `❌ Compilation failed with code ${code}\n`
        );
        return;
      }

      // Step 2: Run the compiled program
      const runProcess = spawn(exePath);

      runProcess.stdout.on("data", (data) => {
        socket.emit("terminal:data", data.toString());
      });

      runProcess.stderr.on("data", (data) => {
        socket.emit("terminal:data", `stderr: ${data.toString()}`);
      });

      runProcess.on("close", (code) => {
        socket.emit("terminal:data", `\n✅ Program exited with code ${code}`);
      });

      // Step 3: Pipe frontend terminal input to program
      socket.on("terminal:write", (input) => {
        runProcess.stdin.write(input);
      });
    });
  });
});

// === File Watcher ===
chokidar.watch(userDir).on("all", (event, changedPath) => {
  io.emit("file:refresh", changedPath);
});

// === Compiler Integration ===
const COMPILER_PATH = path.resolve("./compiler/compiler.exe");
const execFileAsync = promisify(execFile);

app.post("/compile", async (req, res) => {
  const { code } = req.body;
  console.log("🚧 Compiling via custom compiler...");

  const inputPath = "./compiler/temp_input.code";
  const outputDir = "./compiler/temp_output";

  try {
    await writeFile(inputPath, code, "utf-8");
    await mkdir(outputDir, { recursive: true });

    await execFileAsync(COMPILER_PATH, [inputPath, outputDir]);

    const [tokens, errors, ir, optimizedIR, cCode, output] = await Promise.all([
      readFile(path.join(outputDir, "tokens.txt"), "utf-8").catch(() => ""),
      readFile(path.join(outputDir, "errors.txt"), "utf-8").catch(() => ""),
      readFile(path.join(outputDir, "ir.txt"), "utf-8").catch(() => ""),
      readFile(path.join(outputDir, "optimized_ir.txt"), "utf-8").catch(
        () => ""
      ),
      readFile(path.join(outputDir, "c_code.txt"), "utf-8").catch(() => ""),
      readFile(path.join(outputDir, "output.txt"), "utf-8").catch(() => ""),
    ]);

    // Permanently store generated C code
    const cFilePath = path.join(userDir, "code.c");
    if (cCode.trim()) {
      await writeFile(cFilePath, cCode, "utf-8");
      console.log(`💾 Saved generated C code to: ${cFilePath}`);
    }

    // Cleanup temp files
    await unlink(inputPath);
    await rm(outputDir, { recursive: true, force: true });

    res.json({
      tokens: tokens.split("\n").filter(Boolean),
      errors: errors.split("\n").filter(Boolean),
      ir,
      optimized_ir: optimizedIR,
      c_code: cCode,
      output,
    });
  } catch (e) {
    console.error("❌ Compiler failed:", e);
    res
      .status(500)
      .json({ error: "Compiler execution failed", details: e.message });
  }
});

// === Start Server ===
const PORT = 9900;
server.listen(PORT, () => {
  console.log(`🚀 Server running at http://localhost:${PORT}`);
});
