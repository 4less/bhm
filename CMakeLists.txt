cmake_minimum_required(VERSION 3.15)
project(bhm)

set(CMAKE_CXX_STANDARD 17)

add_executable(bhm main.cpp BHashMap.cpp BHashMap.h Log.cpp Log.h DLList.cpp DLList.h KmerQueue.cpp KmerQueue.h)