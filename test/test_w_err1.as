.define sz = 3
MAIN: mov r3, LIST[sz]
LOOP: jmp L1  , r3
mcr m_mcr
cmp r3, #sz
 bne END
endmcr
 prn 
 mov STR[5], STR[2]
  
 sub r1, r4
 m_mcr
L1: inc K
 bne LOOP
END: hlt 
define len = 4
STR: .string "abcdef
LIST: .data 6, -9, len
K: .data 22 
.data 6 ,, 10
lea r3 , #-6
jsr , r2
LOOP: jmp L1  , r1
mov: jmp L1  , r3
sub r2, r6 ,
.data





