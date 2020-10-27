# minic
Mini C Compiler

Work-in-progress C compiler inspired by [this](https://github.com/ayazhafiz/ccc/tree/l2/parsers/L2) subset of the C grammar, implemented in C using [mpc](https://github.com/orangeduck/mpc) for parsing and grammar specification.

## Building

`nix-shell` and then `make` to build. 

For C++ auto-completion such as with [ccls](https://github.com/MaskRay/ccls) or [clangd](https://github.com/clangd/clangd), install [Bear](https://github.com/rizsotto/Bear) and then run `nix-shell` and `bear make` to produce a `compile_commands.json` file.
