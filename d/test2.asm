start:
    mov &0050 $0A

loop:
    mov acc &0050
    dec acc
    mov &0050 acc
    inc r2
    inc r2
    inc r2
    jne $00 &[!loop]

end:
    hlt
