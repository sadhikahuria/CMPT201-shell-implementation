# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/cmpt201/units/03-memory/CMPT201-shell-implementation/build/_deps/googletest-src"
  "/home/cmpt201/units/03-memory/CMPT201-shell-implementation/build/_deps/googletest-build"
  "/home/cmpt201/units/03-memory/CMPT201-shell-implementation/build/_deps/googletest-subbuild/googletest-populate-prefix"
  "/home/cmpt201/units/03-memory/CMPT201-shell-implementation/build/_deps/googletest-subbuild/googletest-populate-prefix/tmp"
  "/home/cmpt201/units/03-memory/CMPT201-shell-implementation/build/_deps/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp"
  "/home/cmpt201/units/03-memory/CMPT201-shell-implementation/build/_deps/googletest-subbuild/googletest-populate-prefix/src"
  "/home/cmpt201/units/03-memory/CMPT201-shell-implementation/build/_deps/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/cmpt201/units/03-memory/CMPT201-shell-implementation/build/_deps/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/cmpt201/units/03-memory/CMPT201-shell-implementation/build/_deps/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
