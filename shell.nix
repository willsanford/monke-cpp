{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell rec {
  SHELL_NAME = "cpp";
  
  buildInputs = with pkgs; [
    jetbrains.clion
    gcc
    cmake
    ninja
    bear
    gdb
    valgrind
    clang-tools
  ];

  shellHook = ''
    export PS1="\n\[\033[1;32m\][nix-shell:$name]\$\[\033[0m\] "
    echo "C++ Development Environment"
    echo "GCC Version: $(gcc --version | head -n1)"
    echo "CMake Version: $(cmake --version | head -n1)"
  '';
}

