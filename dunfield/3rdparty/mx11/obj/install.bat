rem batch file to copy files to \mc\lib11

copy mxsem.asm        c:\mc\lib11
copy mxtime.asm       c:\mc\lib11
copy mxmsg.asm        c:\mc\lib11
copy mxkernal.asm     c:\mc\lib11
copy mxstack.asm      c:\mc\lib11
copy mxnull.asm       c:\mc\lib11
copy mxnull1.asm      c:\mc\lib11
copy mxcomm.asm       c:\mc\lib11
copy mx.equ           c:\mc\lib11
copy mxvect.equ       c:\mc\lib11
copy mxindex.lib      c:\mc\lib11
copy mx.h             c:\mc\lib11

@echo off

echo Remember to edit the EXTINDEX.LIB file to add the
echo entries in the MXINDEX.LIB

