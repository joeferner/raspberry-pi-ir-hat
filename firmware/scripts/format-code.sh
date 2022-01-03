#!/bin/bash

clang-format --verbose -i --style=file \
  src/*.h \
  src/*.cpp src/*.hpp \
  src/hal/*.cpp src/hal/*.hpp \
  src/peripheral/*.hpp
