const CODE_CONSTANT = #C0DE

+dat8 bytes = { #01,   #02,   #03,   #04 }
dat16 words = { #0506, #0708, #090A, #0B0C }

code:
    mov &1234 [!CODE_CONSTANT]
