cmake_minimum_required(VERSION 3.15)
project(2245_TP3 C)

set(CMAKE_C_STANDARD 99)

add_executable(2245_TP3 src/common.c
        src/common.h src/conf.h src/parse.tab.c src/parse.tab.h
        src/pm.c src/pm.h src/pt.c src/pt.h src/tlb.c src/tlb.h
        src/tokens.c src/vmm.c src/vmm.h )