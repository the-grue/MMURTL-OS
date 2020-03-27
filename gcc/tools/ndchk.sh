# This will generate a dump of a runfile that skips
# the first 55 header bytes and starts with the .text
# section through the .data, .rodata, and .bss
# B2 will identify the data areas, the call above 
# B2 is the stdentry that calls main and is expected
# to be at the end of the .text section
# 
# For more information on the layout, please refer
# to the linker script

ndisasm -b 32 -e 55 -p intel $1
