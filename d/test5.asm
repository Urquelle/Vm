;; Makros

makro Test(A, B, C) {
    mov A #1000
    inc B
    mov B A
}

+dat16 pos_A = { #01, #02, #03 }
dat16 pos_B = { #05, #06, #07 }

anfang:
ende:
    %Test r1 r2 acu
