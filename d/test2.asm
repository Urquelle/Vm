start:
    mov &0050 #0A

loop:
    mov acu &0050
    dec acu
    mov &0050 acu
    inc r2
    inc r2
    inc r2
    jne #00 &[!loop]

end:
    hlt
