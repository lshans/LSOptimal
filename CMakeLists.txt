cmake_minimum_required (VERSION 2.6)
project (LSOptimal)

aux_source_directory(./src DIR_SRCS)
add_executable(LSOptimal ${DIR_SRCS})

