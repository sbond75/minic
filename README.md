# minic
Mini C Compiler

Work-in-progress C compiler inspired by [this](https://github.com/ayazhafiz/ccc/tree/l2/parsers/L2) subset of the C grammar, implemented in C using [mpc](https://github.com/orangeduck/mpc) for parsing and grammar specification.

## Building

Run `nix-shell` and then `make` to build. 

### Notes
- `nix-shell` requires [Nix](https://nixos.org/download.html) to build with [LLVM](https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl03.html); if needed, you can change the Makefile to avoid requiring LLVM for testing purposes.
- For C++ auto-completion such as with [ccls](https://github.com/MaskRay/ccls) or [clangd](https://github.com/clangd/clangd), install [Bear](https://github.com/rizsotto/Bear) and then run `nix-shell` and `bear make` to produce a `compile_commands.json` file.
