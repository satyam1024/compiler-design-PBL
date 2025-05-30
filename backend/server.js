import express from "express";
import { writeFile, unlink, mkdir, rm, readFile } from "fs/promises";
import { execFile } from "child_process";
import path from "path";
import cors from "cors";
import { promisify } from "util";

const app = express();
app.use(express.json());
app.use(cors());

const COMPILER_PATH = path.resolve("./compiler/compiler.exe");
const execFileAsync = promisify(execFile);

app.post("/compile", async (req, res) => {
  const { code } = req.body;
  console.log(code);
  // if (!code) return res.status(400).json({ error: "No code provided" });

  const inputPath = "./compiler/temp_input.code";
  const outputDir = "./compiler/temp_output";

  try {
    await writeFile(inputPath, code, "utf-8");

    await mkdir(outputDir, { recursive: true });

    await execFileAsync(COMPILER_PATH, [inputPath, outputDir]);

    // Read all output files concurrently, fallback to empty string if missing
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

    console.log(tokens, errors, ir, optimizedIR, cCode, output);
    // Cleanup temp files and directory
    await unlink(inputPath);
    await rm(outputDir, { recursive: true, force: true });

    // Send JSON response
    res.json({
      tokens: tokens.split("\n").filter(Boolean),
      errors: errors.split("\n").filter(Boolean),
      ir,
      optimized_ir: optimizedIR,
      c_code: cCode,
      output,
    });
  } catch (e) {
    console.error(e);
    res
      .status(500)
      .json({ error: "Compiler execution failed", details: e.message });
  }
});

app.listen(5000, () => {
  console.log("Compiler backend listening on http://localhost:5000");
});
