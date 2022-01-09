#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd "${SCRIPT_DIR}/.."

clang-format --verbose -i --style=file \
  src/*.h \
  src/*.cpp src/*.hpp \
  src/hal/*.cpp src/hal/*.hpp \
  src/ir/*.cpp src/ir/*.hpp \
  src/test/*.cpp src/test/*.hpp \
  src/peripheral/*.hpp
