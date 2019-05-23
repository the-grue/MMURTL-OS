@ECHO OFF
CLS
ECHO.
ECHO ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ECHO ณ You have chosen to install the CM 32 Compiler and DASM  ณ
ECHO ณ Assembler files from this disc to your C: hard drive.   ณ
ECHO ณ                                                         ณ
ECHO ณ                                                         ณ
ECHO ณ The files will take up about 1 megabyte of space.       ณ
ECHO ณ                                                         ณ
ECHO ณ                                                         ณ
ECHO ณ If this is NOT what you want to do, press the Ctrl + C  ณ
ECHO ณ keys to stop this batch program.                        ณ
ECHO ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
ECHO.
ECHO.
ECHO.
PAUSE Paused... press any key to continue the installation
CLS
ECHO.
ECHO ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ECHO ณ The software will now be installed to your hard drive.                ณ
ECHO ณ You'll see a message telling you when the installation is finished.   ณ
ECHO ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
rem xcopy /s ..\mmsys\*.*     c:\mmsys\*.* >nul
xcopy /s ..\cm32\*.*      c:\cm32\*.* >nul
xcopy /s ..\dasm\*.*      c:\dasm\*.* >nul
rem xcopy /s ..\msamples\*.*  c:\msamples\*.* >nul
rem xcopy /s ..\mscode16\*.*  c:\mscode16\*.* >nul
rem xcopy /s ..\ossource\*.*  c:\ossource\*.* >nul
CLS
ECHO 
ECHO.
ECHO ษออออออออออออออออออออออออออออออออออออออออออออออออออออป
ECHO บ The installation is complete!                      บ
ECHO บ                                                    บ
ECHO บ You will now be returned to the installation menu. บ
ECHO ศออออออออออออออออออออออออออออออออออออออออออออออออออออผ
ECHO.
ECHO.
PAUSE
