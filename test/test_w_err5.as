MAIN: mov r3, LIST[sz]
LOOP: jmp L1
 prn #-5
 mov STR[5], STR[2]
 .define sz = 2
  
 sub r1, r4
cmp r3, #sz
 bne END
L1: inc K
 bne dist
END: hlt 
.define len = 4
STR: .string "abcdef"
LIST: .data 6, -9, x
K: .data 22 



