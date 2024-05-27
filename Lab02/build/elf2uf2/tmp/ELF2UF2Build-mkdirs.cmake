# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/yelps/OneDrive - James Cook University/Uni/2023/Semester 2/CC2511 - Embedded Systems Design/Subject Software/pico-sdk-master/tools/elf2uf2"
  "C:/Users/yelps/OneDrive/Documents/GitHub/labs-Peachy97/Lab2/build/elf2uf2"
  "C:/Users/yelps/OneDrive/Documents/GitHub/labs-Peachy97/Lab2/build/elf2uf2"
  "C:/Users/yelps/OneDrive/Documents/GitHub/labs-Peachy97/Lab2/build/elf2uf2/tmp"
  "C:/Users/yelps/OneDrive/Documents/GitHub/labs-Peachy97/Lab2/build/elf2uf2/src/ELF2UF2Build-stamp"
  "C:/Users/yelps/OneDrive/Documents/GitHub/labs-Peachy97/Lab2/build/elf2uf2/src"
  "C:/Users/yelps/OneDrive/Documents/GitHub/labs-Peachy97/Lab2/build/elf2uf2/src/ELF2UF2Build-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/yelps/OneDrive/Documents/GitHub/labs-Peachy97/Lab2/build/elf2uf2/src/ELF2UF2Build-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/yelps/OneDrive/Documents/GitHub/labs-Peachy97/Lab2/build/elf2uf2/src/ELF2UF2Build-stamp${cfgdir}") # cfgdir has leading slash
endif()
