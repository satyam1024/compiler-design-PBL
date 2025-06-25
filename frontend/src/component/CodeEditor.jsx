import React, { useState, useEffect } from "react";

// SyntaxHighlighter component
const rules = [
  {
    regex: /"([^"\\]*(\\.[^"\\]*)*)"|'([^'\\]*(\\.[^'\\]*)*)'/g,
    className: "text-green-400",
  },
  {
    regex: /\b\d+\b/g,
    className: "text-yellow-400",
  },
  {
    regex: /\/\/.*/g,
    className: "text-gray-500 italic",
  },
  {
    regex:
      /\b(function|if|else|return|let|const|var|for|while|import|export|in|and|output)\b/g,
    className: "text-purple-400 font-semibold",
  },
  {
    regex: /\b(add|subtract|multiply|store|divide|be|output)\b/g,
    className: "text-red-400 font-semibold",
  },
  {
    regex: /\b([a-zA-Z_][a-zA-Z0-9_]*)\s*(?=\()/g,
    className: "text-blue-400",
  },
];

// Tokenizer function
function tokenize(text, rules) {
  let tokens = [{ text, className: null }];

  for (const rule of rules) {
    let newTokens = [];
    for (const token of tokens) {
      if (token.className) {
        // Already highlighted, keep as is
        newTokens.push(token);
        continue;
      }
      let lastIndex = 0;
      let match;
      while ((match = rule.regex.exec(token.text)) !== null) {
        if (match.index > lastIndex) {
          newTokens.push({
            text: token.text.slice(lastIndex, match.index),
            className: null,
          });
        }
        newTokens.push({
          text: match[0],
          className: rule.className,
        });
        lastIndex = match.index + match[0].length;
      }
      if (lastIndex < token.text.length) {
        newTokens.push({
          text: token.text.slice(lastIndex),
          className: null,
        });
      }
    }
    tokens = newTokens;
  }
  return tokens;
}

// SyntaxHighlighter component using map approach
const SyntaxHighlighter = ({ code }) => {
  const tokens = tokenize(code, rules);

  return (
    <code className="font-mono text-sm leading-6 whitespace-pre-wrap">
      {tokens.map((token, i) =>
        token.className ? (
          <span key={i} className={token.className}>
            {token.text}
          </span>
        ) : (
          token.text
        )
      )}
    </code>
  );
};

// export default SyntaxHighlighter;

// Main CodeEditor component
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
