# Git repository template for C/C++ development

The target CMake toolchain is clang/llvm with clangd, clang_format and clang_tidy.
There are various useful utilities available for starting up the project under `utils/`

To start using CMake with the default clang configuration, execute: `./utils/cmake-boot`

To add external dependencies using `FetchContent`, use `./utils/cmake-add-dependency <depname> -u <url> -t <tag>`

Or add from the list of default dependencies using: `./utils/cmake-add-dependency --default <depname>`

The list of defaults can be found under `./utils/cmake-add-dependency-defaults.json`

> NOTE: To use default dependencies you will need `jq` installed (`apt install jq -y`)

