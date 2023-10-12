# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/ubuntu/CLionProjects/Computer_Architecture_Project/_deps/gsl-src"
  "/home/ubuntu/CLionProjects/Computer_Architecture_Project/_deps/gsl-build"
  "/home/ubuntu/CLionProjects/Computer_Architecture_Project/_deps/gsl-subbuild/gsl-populate-prefix"
  "/home/ubuntu/CLionProjects/Computer_Architecture_Project/_deps/gsl-subbuild/gsl-populate-prefix/tmp"
  "/home/ubuntu/CLionProjects/Computer_Architecture_Project/_deps/gsl-subbuild/gsl-populate-prefix/src/gsl-populate-stamp"
  "/home/ubuntu/CLionProjects/Computer_Architecture_Project/_deps/gsl-subbuild/gsl-populate-prefix/src"
  "/home/ubuntu/CLionProjects/Computer_Architecture_Project/_deps/gsl-subbuild/gsl-populate-prefix/src/gsl-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/ubuntu/CLionProjects/Computer_Architecture_Project/_deps/gsl-subbuild/gsl-populate-prefix/src/gsl-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/ubuntu/CLionProjects/Computer_Architecture_Project/_deps/gsl-subbuild/gsl-populate-prefix/src/gsl-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
