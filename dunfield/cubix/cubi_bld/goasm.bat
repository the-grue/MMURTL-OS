@echo off
rem * Use this file to assemble the various utilities and applications
rem * that run under CUBIX. DO NOT try and use this file to assemble
rem * the operating system itself... See the READ.ME file in the CUBIX
rem * directory for instruction on how to do that.
rem *
rem * Before using this file, modify it if necessary to place the temporary
rem * files in an appriopriate area. In this example, it uses my D: RAMDRIVE
rem * for the temporary file.
rem *
rem * Use "GOASM <name>" to assemble a given program. Do not try to assemble
rem * any files which have an EXTENSION to the filename, since these are
rem * other support files.
rem *
macro \cubix\macros %1 %2 %3 %4 %5 >D:%1.ASM
asm09 D:%1 -t
del D:%1.ASM
