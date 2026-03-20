		; Program for Block Move & Exchange
			EXPORT __main
			AREA	PROG_2, CODE, READONLY
__main	
			MOV	R0, #0x08
			LDR	R1, =ROMARRAY1 + 28	; HEX(16) = Length of 2 Arrays
										; Point to last address of Array 2 on ROM 28 = (LEN-1) * 4
			LDR	R2, =ARRAY1 	; Point to last address of Array 2 on RAM28 = (LEN-1) * 4

label1		LDR	R3, [R1],	#-4	; Load data from address pointed by R1, and ;decrement the address in R1 by 0x04
			STR	R3,	[R2],	#4	; Load data from address pointed by R2, and ;increment the address in R2 by 0x04
			SUBS	R0,R0,  #1	; Decrement the length
			BNE  label1	; Loop till length is equal to 0

			MOV	R0, #LEN	; Store the length of single array to R0
			LDR	R1,=ARRAY2	; Point to last address of Array 1 on RAM	28 = (LEN-1) * 4
			LDR	R2,=ARRAY1+28	; Point to last address of Array 2 on RAM	28 = (LEN-1) * 4
label2		LDR	R3, [R2],#-4
			STR	R3, [R1], #4
			SUBS 	R0,R0, #1
	     	BNE label2
				

STOP		B	STOP
				

			AREA tempData, DATA, READONLY, ALIGN=4
ROMARRAY1   DCD	0x00000001, 0x00000002, 0x00000003, 0x00000004, 0x00000005, 0x00000006, 0x00000007, 0x00000008			; Initializing Array1 in the ROM 
;ROMARRAY2   DCD	0x00000009, 0x00000010, 0x00000011, 0x00000012, 0x00000013, 0x00000014, 0x00000015, 0x00000016			; Initializing Array2 in the ROM

			AREA	|.data|, DATA, READWRITE, ALIGN=4	
LEN 	EQU	0x08	; Length of Array
ARRAY1	SPACE 32	; 32 words allocated for Array 1
ARRAY2	SPACE 32	; 32 words allocated for Array 2

				END
