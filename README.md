# AI-NATIVE-DBMS-C

AI-NATIVE-DBMS-C is an experimental project for a C DBMS Core, a Handmade AI Runtime, and first-class LLM semantic operators.

The current stage is **v0.1 skeleton**. This repository is focused on a clean buildable foundation, not a full database engine or AI stack yet.

## What This Is Not

- Not an industrial-grade database.
- Not a C version of LangChain.
- Not a thin ChatGPT shell around SQL text.

## What Works Now

- Configure the project with CMake presets.
- Build the minimal `aidb` executable.
- Run the minimal startup/core path.
- Run the default non-network CTest suite.

## Build

```powershell
cmake --preset mingw-debug
cmake --build --preset mingw-debug-build
ctest --test-dir build/mingw-debug --output-on-failure
```

## Run

```powershell
.\bin\debug\aidb.exe
```

## Road Ahead

The planned path is DBMS Core, Explain, Handmade JSON, AI Runtime, `AI_MATCH`, Cache, CallLog, and a mini optimizer. See [ROADMAP.md](ROADMAP.md).
