import { Terminal as TerminalIcon } from "lucide-react";

export const Terminal = ({ output }) => {
  return (
    <div className="h-full bg-black border-t border-gray-700 flex flex-col">
      {/* Header */}
      <div className="bg-gray-800 px-4 py-2 border-b border-gray-700 flex items-center space-x-2">
        <TerminalIcon className="w-4 h-4 text-gray-400" />
        <span className="text-sm text-gray-400">Terminal</span>
      </div>

      {/* Scrollable Output */}
      <div className="flex-1 overflow-auto">
        <div className="p-4">
          <pre className="text-green-400 font-mono text-sm whitespace-pre-wrap leading-relaxed">
            {output || "Ready to compile and run your code..."}
          </pre>
        </div>
      </div>
    </div>
  );
};
