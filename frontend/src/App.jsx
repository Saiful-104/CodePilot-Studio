
import { useState } from "react";
import Editor from "@monaco-editor/react";
import "./App.css";

const API_URL = "http://localhost:5000";

const languages = {
  minilang: {
    label: "MiniLang",
    editorLanguage: "cpp",
    defaultCode: `int age = 24;
float score = 85.5;
bool passed = true;

print(age);
print(score);
print(passed);`,
  },
  c: {
    label: "C",
    editorLanguage: "c",
    defaultCode: `#include <stdio.h>

int main() {
    printf("Hello from C!");
    return 0;
}`,
  },
  cpp: {
    label: "C++",
    editorLanguage: "cpp",
    defaultCode: `#include <iostream>
using namespace std;

int main() {
    cout << "Hello from C++!";
    return 0;
}`,
  },
  java: {
    label: "Java",
    editorLanguage: "java",
    defaultCode: `public class Main {
    public static void main(String[] args) {
        System.out.println("Hello from Java!");
    }
}`,
  },
};

function App() {
  const [language, setLanguage] = useState("minilang");
  const [code, setCode] = useState(
    languages.minilang.defaultCode
  );
  const [input, setInput] = useState("");
  const [output, setOutput] = useState("");
  const [error, setError] = useState("");
  const [loading, setLoading] = useState(false);

  const handleLanguageChange = (event) => {
    const selectedLanguage = event.target.value;

    setLanguage(selectedLanguage);
    setCode(languages[selectedLanguage].defaultCode);
    setOutput("");
    setError("");
  };

  const handleRunCode = async () => {
    setLoading(true);
    setOutput("");
    setError("");

    try {
      const endpoint =
        language === "minilang"
          ? `${API_URL}/api/compiler/run`
          : `${API_URL}/api/execute/run`;

      const response = await fetch(endpoint, {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify({
          language,
          sourceCode: code,
          input,
        }),
      });

      const result = await response.json();

      if (result.output) {
        setOutput(result.output);
      }

      if (!response.ok || !result.success) {
        setError(
          result.error || "Something went wrong."
        );
      }
    } catch (err) {
      setError(
        "Unable to connect to the backend. Make sure the server is running."
      );
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="app">
      <header className="navbar">
        <div className="brand">
          <span className="brand-icon">{"</>"}</span>
          <span>CodePilot Studio</span>
        </div>

        <div className="navbar-status">
          <span className="status-dot"></span>
          Compiler Workspace
        </div>
      </header>

      <main className="workspace">
        <section className="toolbar">
          <div className="language-control">
            <label htmlFor="language">Language</label>

            <select
              id="language"
              value={language}
              onChange={handleLanguageChange}
            >
              {Object.entries(languages).map(
                ([value, item]) => (
                  <option key={value} value={value}>
                    {item.label}
                  </option>
                )
              )}
            </select>
          </div>

          <button
            className="run-button"
            onClick={handleRunCode}
            disabled={loading}
          >
            {loading ? "Running..." : "▶ Run Code"}
          </button>
        </section>

        <section className="editor-section">
          <div className="section-heading">
            <span>Code Editor</span>
            <span>{languages[language].label}</span>
          </div>

          <Editor
            height="450px"
            language={languages[language].editorLanguage}
            theme="vs-dark"
            value={code}
            onChange={(value) => setCode(value || "")}
            options={{
              fontSize: 14,
              minimap: { enabled: false },
              automaticLayout: true,
              scrollBeyondLastLine: false,
              wordWrap: "on",
              tabSize: 4,
              padding: { top: 16 },
            }}
          />
        </section>

        <section className="input-section">
          <div className="section-heading">
            Custom Input
          </div>

          <textarea
            value={input}
            onChange={(event) => setInput(event.target.value)}
            placeholder="Enter input for your program..."
            rows={4}
          />
        </section>

        <section className="output-section">
          <div className="section-heading">
            Execution Output
          </div>

          <div className="output-content">
            {output && (
              <pre className="output-text">{output}</pre>
            )}

            {error && (
              <pre className="error-text">{error}</pre>
            )}

            {!output && !error && !loading && (
              <p className="empty-output">
                Your program output will appear here.
              </p>
            )}

            {loading && (
              <p className="empty-output">
                Executing your code...
              </p>
            )}
          </div>
        </section>
      </main>
    </div>
  );
}

export default App;
