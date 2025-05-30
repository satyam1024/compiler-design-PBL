import { useState } from "react";

export const CompilationTabs = ({ data }) => {
  const [activeTab, setActiveTab] = useState("tokens");

  if (!data) {
    return (
      <div className="h-full flex items-center justify-center bg-gray-900 text-gray-500">
        <div className="text-center">
          <p className="text-lg mb-2">No compilation data</p>
          <p className="text-sm">
            Click "Compile & Run" to see the compilation pipeline
          </p>
        </div>
      </div>
    );
  }

  const tabs = [
    { id: "tokens", label: "Tokens" },
    { id: "errors", label: "Errors" },
    { id: "ir", label: "IR" },
    { id: "optimized_ir", label: "Optimized IR" },
    { id: "c_code", label: "C Code" },
    { id: "output", label: "Output" },
  ];

  return (
    <div className="h-full bg-gray-900 flex flex-col">
      {/* Tabs List */}
      <div className="flex border-b border-gray-700 bg-gray-800 h-12">
        {tabs.map(({ id, label }) => (
          <button
            key={id}
            onClick={() => setActiveTab(id)}
            className={`flex items-center justify-center px-4 text-sm font-medium select-none ${
              activeTab === id
                ? "bg-gray-700 text-white"
                : "text-gray-400 hover:bg-gray-700 hover:text-white"
            }`}
            type="button"
            role="tab"
            aria-selected={activeTab === id}
            aria-controls={`${id}-tab`}
            id={`${id}-tab-button`}
          >
            {label}
          </button>
        ))}
      </div>

      {/* Tabs Content */}
      <div className="flex-1 overflow-auto p-4">
        {activeTab === "tokens" && (
          <div className="space-y-2">
            {data.tokens.length > 0 ? (
              data.tokens.map((token, i) => (
                <div
                  key={i}
                  className="p-2 bg-gray-800 rounded font-mono text-sm text-blue-400"
                >
                  {token}
                </div>
              ))
            ) : (
              <p className="text-gray-500">No tokens available.</p>
            )}
          </div>
        )}

        {activeTab === "errors" && (
          <div className="space-y-2">
            {data.errors.length > 0 ? (
              data.errors.map((error, i) => (
                <div
                  key={i}
                  className="p-3 bg-red-900 border border-red-700 rounded text-red-300 font-mono text-sm"
                >
                  {error}
                </div>
              ))
            ) : (
              <p className="text-green-400">No errors found.</p>
            )}
          </div>
        )}

        {activeTab === "ir" && (
          <pre className="text-sm text-gray-300 font-mono bg-gray-800 p-4 rounded overflow-x-auto whitespace-pre-wrap">
            {data.ir || "No IR available."}
          </pre>
        )}

        {activeTab === "optimized_ir" && (
          <pre className="text-sm text-gray-300 font-mono bg-gray-800 p-4 rounded overflow-x-auto whitespace-pre-wrap">
            {data.optimized_ir || "No optimized IR available."}
          </pre>
        )}

        {activeTab === "c_code" && (
          <pre className="text-sm text-gray-300 font-mono bg-gray-800 p-4 rounded overflow-x-auto whitespace-pre-wrap">
            {data.c_code || "No C code available."}
          </pre>
        )}

        {activeTab === "output" && (
          <pre className="text-sm text-green-400 font-mono bg-gray-800 p-4 rounded overflow-x-auto whitespace-pre-wrap">
            {data.output || "No output available."}
          </pre>
        )}
      </div>
    </div>
  );
};
