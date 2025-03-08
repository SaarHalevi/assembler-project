K: .data 22 
END: hlt 
.define len = 4
STR: .string "abcdef"
LIST: .data 6, -9, len

mcr .data
hlt
endmcr
