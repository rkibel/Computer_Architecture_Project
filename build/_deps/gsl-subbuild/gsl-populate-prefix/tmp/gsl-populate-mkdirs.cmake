# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/ronki/OneDrive/Documents/GitHub/Computer_Architecture_Project/build/_deps/gsl-src"
  "C:/Users/ronki/OneDrive/Documents/GitHub/Computer_Architecture_Project/build/_deps/gsl-build"
  "C:/Users/ronki/OneDrive/Documents/GitHub/Computer_Architecture_Project/build/_deps/gsl-subbuild/gsl-populate-prefix"
  "C:/Users/ronki/OneDrive/Documents/GitHub/Computer_Architecture_Project/build/_deps/gsl-subbuild/gsl-populate-prefix/tmp"
  "C:/Users/ronki/OneDrive/Documents/GitHub/Computer_Architecture_Project/build/_deps/gsl-subbuild/gsl-populate-prefix/src/gsl-populate-stamp"
  "C:/Users/ronki/OneDrive/Documents/GitHub/Computer_Architecture_Project/build/_deps/gsl-subbuild/gsl-populate-prefix/src"
  "C:/Users/ronki/OneDrive/Documents/GitHub/Computer_Architecture_Project/build/_deps/gsl-subbuild/gsl-populate-prefix/src/gsl-populate-stamp"
)

set(configSubDirs Debug)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/ronki/OneDrive/Documents/GitHub/Computer_Architecture_Project/build/_deps/gsl-subbuild/gsl-populate-prefix/src/gsl-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/ronki/OneDrive/Documents/GitHub/Computer_Architecture_Project/build/_deps/gsl-subbuild/gsl-populate-prefix/src/gsl-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
