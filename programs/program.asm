; =========================================================
; Square Calculation Program for 8-bit Tiny CPU
; Computes N^2 where N is loaded into R0
; Example: N = 15 -> Result = 225 (0x19 stored in R1 and memory[50])
; =========================================================

; Byte 0 (len 2): Load N into R0
; Change N as you wish. Squares above 255 will overflow 
; (eg 256 becomes 0)
MOV_IMM R0, 5

; Byte 2 (len 2): Initialize result accumulator R1 = 0
MOV_IMM R1, 0

; Byte 4 (len 1): Copy loop counter N into R2
MOV_REG R2, R0

; Byte 5 (len 2): Load -1 (255 in 2's complement) into R3 for decrementing
MOV_IMM R3, 255

; --- LOOP START (Byte Address 7) ---
; Byte 7 (len 2): If loop counter R2 == 0 (Z flag active), jump to STORE at Byte 13
JZ 13

; Byte 9 (len 1): Accumulate: R1 = R1 + N (R0)
ADD R1, R0

; Byte 10 (len 1): Decrement loop counter: R2 = R2 + 255 (updates Z flag when R2 reaches 0)
ADD R2, R3

; Byte 11 (len 2): Jump back to loop condition check at Byte 7
JUMP 7
; --- LOOP END ---

; Byte 13 (len 2): Store squared result (R1) into memory address 50
STORE R1, 50

; Byte 15 (len 1): Halt CPU execution
HALT
