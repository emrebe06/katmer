# Katmer Install

This guide is for users who want to use Katmer directly, without N-True,
QuickAPI, or SQLNocturne.

## Requirements

- Git
- CMake 3.16+
- C/C++ compiler
  - Windows: Visual Studio Build Tools
  - Linux: GCC or Clang
  - macOS: Xcode Command Line Tools

## Windows

```bat
cd /d C:\Users\%USERNAME%\Desktop
git clone https://github.com/emrebe06/katmer.git
cd katmer
cmake -S cpp -B build
cmake --build build --config Release
```

Run Katmer:

```bat
build\Release\katmer_bake.exe katmer "<katmer module=\"http\" type=\"request\">GET /health</katmer>"
```

Run from a file:

```bat
echo ^<katmer module="http" type="request"^>GET /health^</katmer^> > test.katmer
build\Release\katmer_bake.exe katmer < test.katmer
```

## Linux/macOS

```bash
git clone https://github.com/emrebe06/katmer.git
cd katmer
cmake -S cpp -B build
cmake --build build --config Release
```

Run Katmer:

```bash
./build/katmer_bake katmer '<katmer module="http" type="request">GET /health</katmer>'
```

Run from a file:

```bash
cat > test.katmer <<'EOF'
<katmer module="http" type="request">GET /health</katmer>
EOF

./build/katmer_bake katmer < test.katmer
```

## Expected Result

Katmer prints JSON:

```json
{
  "ok": true,
  "engine": "katmer",
  "module": "http",
  "type": "request",
  "result": {
    "module": "http",
    "select_kind": "http",
    "guard": "allow"
  }
}
```

The full JSON also includes trace, layers, signals, shape metadata, and arena
stats.
