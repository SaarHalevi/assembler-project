.define sz = 3
MAIN: mov r3, LIST[sz]
LOOP: jmp L1
mcr m_mcr
cmp r3, #sz
 bne END
endmcr
 prn #-5
 mov STR[5], STR[2]
 
 mcr m_mcr
prn #-5
endmcr
 







