@echo off
rem *
rem * Link object modules with libraries into ".COM" file
rem *
rem * Copyright 1988-1995 Dave Dunfield
rem *
rem * See CC.C for comments reguarding modifications for different linkers
rem * 
echo DDS MICRO-C 8086 Link Command by Dave Dunfield.
if .%1 == . goto help
if .%MCDIR% == . set MCDIR=\mc
if .%1 == .-s goto small
:tiny
echo Linking TINY model...
rem *
rem * Uncomment ONLY the linker that you are using
rem *
link/noi        %MCDIR%\PC86RL_T %1 %2 %3 %4 %5 %6 %7 %8 %9,%1,NUL,%MCDIR%\MCLIB;
rem tlink/c     %MCDIR%\PC86RL_T %1 %2 %3 %4 %5 %6 %7 %8 %9,%1,NUL,%MCDIR%\MCLIB;
rem val/NCI/COM %MCDIR%\PC86RL_T %1 %2 %3 %4 %5 %6 %7 %8 %9,%1,NUL,%MCDIR%\MCLIB;
rem *** End of Linkers ***
if errorlevel 1 goto fail
rem *
rem * Using VAL, comment out this EXE2BIN section
rem *
echo Converting...
exe2bin %1 %1.COM
del %1.EXE
rem *** End of EXE2BIN ***
goto stop
:small
if .%2 == . goto help
echo Linking SMALL model...
shift
rem *
rem * Uncomment ONLY the linker that you are using
rem *
link/noi     %MCDIR%\PC86RL_S %1 %2 %3 %4 %5 %6 %7 %8 %9,%1,NUL,%MCDIR%\MCLIB;
rem tlink/c  %MCDIR%\PC86RL_S %1 %2 %3 %4 %5 %6 %7 %8 %9,%1,NUL,%MCDIR%\MCLIB;
rem val /NCI %MCDIR%\PC86RL_S %1 %2 %3 %4 %5 %6 %7 %8 %9,%1,NUL,%MCDIR%\MCLIB;
rem *** End of Linkers ***
if errorlevel 1 goto fail
goto stop
:help
echo.
echo Use: "%0 [-s] <filename(s)> ..."
echo.
echo Copyright 1988-1995 Dave Dunfield
echo All rights reserved.
goto stop
:fail
echo Link aborted due to errors!!!
:stop
