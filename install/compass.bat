@ECHO OFF
CLS
ECHO.
ECHO 旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
ECHO � You have chosen to install the CM 32 Compiler and DASM  �
ECHO � Assembler files from this disc to your C: hard drive.   �
ECHO �                                                         �
ECHO �                                                         �
ECHO � The files will take up about 1 megabyte of space.       �
ECHO �                                                         �
ECHO �                                                         �
ECHO � If this is NOT what you want to do, press the Ctrl + C  �
ECHO � keys to stop this batch program.                        �
ECHO 읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
ECHO.
ECHO.
ECHO.
PAUSE Paused... press any key to continue the installation
CLS
ECHO.
ECHO 旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
ECHO � The software will now be installed to your hard drive.                �
ECHO � You'll see a message telling you when the installation is finished.   �
ECHO 읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
rem xcopy /s ..\mmsys\*.*     c:\mmsys\*.* >nul
xcopy /s ..\cm32\*.*      c:\cm32\*.* >nul
xcopy /s ..\dasm\*.*      c:\dasm\*.* >nul
rem xcopy /s ..\msamples\*.*  c:\msamples\*.* >nul
rem xcopy /s ..\mscode16\*.*  c:\mscode16\*.* >nul
rem xcopy /s ..\ossource\*.*  c:\ossource\*.* >nul
CLS
ECHO 
ECHO.
ECHO �袴袴袴袴袴袴袴袴袴袴袴袴袴袴袴袴袴袴袴袴袴袴袴袴袴袴�
ECHO � The installation is complete!                      �
ECHO �                                                    �
ECHO � You will now be returned to the installation menu. �
ECHO 훤袴袴袴袴袴袴袴袴袴袴袴袴袴袴袴袴袴袴袴袴袴袴袴袴袴暠
ECHO.
ECHO.
PAUSE
