;; Makros

makro Test(A, B, C) {
    mov A $1000
    inc B
    mov B A
}

anfang:
ende:
    %Test r1 r2 acu
