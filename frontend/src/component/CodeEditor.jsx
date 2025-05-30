import { useState, useEffect } from "react";

export const CodeEditor = ({ code, onChange }) => {
  const [lineNumbers, setLineNumbers] = useState([]);

  useEffect(() => {
    const lines = code.split("\n").length;
    setLineNumbers(Array.from({ length: lines }, (_, i) => i + 1));
  }, [code]);

  const handleChange = (e) => {
    onChange(e.target.value);
  };

  return (
    <div className="h-full flex flex-col">
      <div className="bg-gray-800 px-4 py-2 border-b border-gray-700">
        <span className="text-sm text-gray-400">main.custom</span>
      </div>

      <div className="flex-1 flex bg-gray-900 relative overflow-hidden">
        {/* Line Numbers */}
        <div className="bg-gray-800 text-gray-500 text-sm font-mono py-4 px-2 select-none border-r border-gray-700 min-w-[60px]">
          {lineNumbers.map((num) => (
            <div key={num} className="h-6 flex items-center justify-end pr-2">
              {num}
            </div>
          ))}
        </div>

        {/* Code Area */}
        <div className="flex-1 relative font-mono text-sm leading-6">
          {/* Syntax Highlighting Overlay */}
          <div
            className="absolute inset-0 p-4 whitespace-pre-wrap pointer-events-none z-0"
            aria-hidden="true"
          >
            <SyntaxHighlighter code={code} />
          </div>

          {/* Transparent Textarea */}
          <textarea
            value={code}
            onChange={handleChange}
            className="absolute inset-0 w-full h-full bg-transparent text-transparent caret-white z-10 p-4 border-none outline-none resize-none"
            style={{ tabSize: 2, lineHeight: "1.5rem" }}
            spellCheck={false}
            autoComplete="off"
            autoCorrect="off"
            autoCapitalize="off"
          />
        </div>
      </div>
    </div>
  );
};
const SyntaxHighlighter = ({ code }) => {
  const highlightSyntax = (text) => {
    let escaped = text
      .replace(/&/g, "&amp;")
      .replace(/</g, "&lt;")
      .replace(/>/g, "&gt;");

    const rules = [
      // Strings
      { regex: /("[^"]*"|'[^']*')/g, className: "text-green-400" },
      // Numbers
      { regex: /\b\d+\b/g, className: "text-yellow-400" },
      // Comments
      { regex: /\/\/.*/g, className: "text-gray-500 italic" },
      // Keywords
      {
        regex:
          /\b(function|if|else|return|let|const|var|for|while|import|export)\b/g,
        className: "text-purple-400 font-semibold",
      },
      // Function names (followed by parentheses)
      {
        regex: /\b([a-zA-Z_][a-zA-Z0-9_]*)\s*(?=\()/g,
        className: "text-blue-400",
      },
    ];

    for (const rule of rules) {
      escaped = escaped.replace(rule.regex, (match) => {
        return `<span class="${rule.className}">${match}</span>`;
      });
    }

    return escaped;
  };

  return <code dangerouslySetInnerHTML={{ __html: highlightSyntax(code) }} />;
};
