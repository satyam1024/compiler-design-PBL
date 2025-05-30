import { useEffect, useState } from "react";
import { CodeEditor } from "./component/CodeEditor";
import { CompilationTabs } from "./component/CompilationTab";
import { Terminal } from "./component/Terminal";

import { Play, Save } from "lucide-react";
import { useToast } from "./hooks/use-toast";

const Index = () => {
  const [code, setCode] = useState("");
  const [compilationData, setCompilationData] = useState(null);
  const [terminalOutput, setTerminalOutput] = useState("");
  const [isCompiling, setIsCompiling] = useState(false);
  const { toast } = useToast();

  useEffect(() => {
    const savedCode = localStorage.getItem("customLangCode");
    if (savedCode) setCode(savedCode);
    else
      setCode(
        `// Welcome to your custom language IDE\nfunction fibonacci(n) {\n    if (n <= 1) return n;\n    return fibonacci(n - 1) + fibonacci(n - 2);\n}\n\nlet result = fibonacci(10);\nprint("Fibonacci(10) = " + result);`
      );
  }, []);

  // const handleCompile = async () => {
  //   setIsCompiling(true);
  //   setTerminalOutput("Compiling...\n");

  //   try {
  //     await new Promise((resolve) => setTimeout(resolve, 1500));

  //     const mockCompilationData = {
  //       tokens: [
  //         { type: "KEYWORD", value: "function", line: 1, column: 1 },
  //         { type: "IDENTIFIER", value: "fibonacci", line: 1, column: 10 },
  //         { type: "SYMBOL", value: "(", line: 1, column: 19 },
  //       ],
  //       ast: {
  //         type: "Program",
  //         body: [
  //           {
  //             type: "FunctionDeclaration",
  //             name: "fibonacci",
  //             params: ["n"],
  //             body: { type: "BlockStatement", statements: [] },
  //           },
  //         ],
  //       },
  //       semanticErrors: [],
  //       ir: "define i32 @fibonacci(i32 %n) { ... }",
  //       optimizedIr: "define i32 @fibonacci(i32 %n) { ... // Optimized }",
  //       cCode: `#include <stdio.h>\nint fibonacci(int n) { ... }\nint main() { ... }`,
  //       finalOutput: "Fibonacci(10) = 55\nProgram executed successfully.",
  //     };

  //     setCompilationData(mockCompilationData);
  //     setTerminalOutput(
  //       "Compilation successful!\nExecuting program...\n" +
  //         mockCompilationData.finalOutput
  //     );

  //     toast({
  //       title: "Compilation Complete",
  //       description: "Your code has been successfully compiled and executed.",
  //     });
  //   } catch (error) {
  //     setTerminalOutput("Compilation failed with errors.");
  //     toast({
  //       title: "Compilation Failed",
  //       description: "There were errors in your code.",
  //       variant: "destructive",
  //     });
  //   } finally {
  //     setIsCompiling(false);
  //   }
  // };

  const handleCompile = async () => {
    setIsCompiling(true);
    setTerminalOutput("Compiling...\n");

    try {
      const response = await fetch("http://localhost:5000/compile", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify({ code }),
      });

      if (!response.ok) {
        const errorData = await response.json();
        setTerminalOutput(
          `Compilation failed: ${errorData.error || "Unknown error"}`
        );
        toast({
          title: "Compilation Failed",
          description: errorData.error || "Unknown error occurred.",
          variant: "destructive",
        });
        setIsCompiling(false);
        return;
      }

      const compilationData = await response.json();
      setCompilationData(compilationData);

      setTerminalOutput(
        "Compilation successful!\nExecuting program...\n" +
          (compilationData.output || "No output from program.")
      );

      toast({
        title: "Compilation Complete",
        description: "Your code has been successfully compiled and executed.",
      });
    } catch (error) {
      setTerminalOutput("Compilation failed due to network or server error.");
      toast({
        title: "Compilation Failed",
        description: error.message || "An error occurred.",
        variant: "destructive",
      });
    } finally {
      setIsCompiling(false);
    }
  };

  const handleSave = () => {
    localStorage.setItem("customLangCode", code);
    toast({
      title: "Code Saved",
      description: "Your code has been saved locally.",
    });
  };

  return (
    <div className="h-screen bg-gray-900 text-white flex flex-col">
      <header className="bg-gray-800 border-b border-gray-700 p-4 flex justify-between items-center">
        <h1 className="text-xl font-bold text-blue-400">Custom Language IDE</h1>
        <div className="flex space-x-2">
          <button
            onClick={handleCompile}
            disabled={isCompiling}
            className={`px-3 py-1 rounded text-white ${
              isCompiling
                ? "bg-green-800 cursor-not-allowed"
                : "bg-green-600 hover:bg-green-700"
            }`}
          >
            {isCompiling ? "Compiling..." : "Compile & Run"}
          </button>

          <button
            onClick={handleSave}
            className="px-3 py-1 rounded border border-gray-600 text-gray-300 hover:bg-gray-700"
          >
            Save
          </button>
        </div>
      </header>

      <main className="flex-1 flex">
        <section className="w-1/2 border-r border-gray-700">
          <CodeEditor code={code} onChange={setCode} />
        </section>

        <section className="w-1/2 flex flex-col">
          <div className="flex-1 overflow-y-auto">
            <CompilationTabs data={compilationData} />
          </div>
          <div className="h-48 border-t border-gray-700">
            <Terminal output={terminalOutput} />
          </div>
        </section>
      </main>
    </div>
  );
};

export default Index;
