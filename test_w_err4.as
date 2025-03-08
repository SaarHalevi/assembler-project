LIST: .data 6, -9, len 
K: .data 22 
.extern L3
mcr 
jmp L1
endmcr
END: hlt
.define len = 4
STR: .string "abcdef" 

