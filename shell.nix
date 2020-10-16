# Tip: direnv to keep dependencies for a specific project in Nix
# Run: nix-shell

{ pkgs ? import <nixpkgs>{} }:
with pkgs;
mkShell {
  buildInputs = [llvm clang
                 llvmPackages.libstdcxxClang
                 ncurses
                ];
}
