	; Program for Arithmatic Operations
	EXPORT __main
	AREA	PROG_1, CODE, READONLY
__main	
			LDR R1, =OUR_FIXED_DATA	   ;memory location for condition LDR R2, [R1]
			LDR R2, =RESULT
			LDR R3, [R1]
			LDR R4, [R1,#4]	;Operand 1
			LDR R5, [R1,#8]	;Operand 2
			SUB R3,#2
			CBZ R3, LABEL
			ADDS R7, R5, R4	;R7=R3+R4
			B LAST

LABEL 		SUBS R7, R5, R4	;R7=R5-R4
			B LAST
 
; CHK_MUL 	CMP R2, #3	; 3: Multiplication BNE CHK_DIV
			;UMULL R7, R8, R3, R4	; [RdHi,RdLow]=Rn*Rm
			;B LAST	;UMULL:Unsigned multiply long

;CHK_DIV 	CMP R2, #4	; 4: Division BNE LAST
			;UDIV R7, R3, R4	;R7=R3/R4*/

LAST		STR R7, [R2]	;store result in memory location 0x20000014 STR R8, [R1, #24]	;store the RdLow in memory location
HERE		B	HERE		; stay here forever
		
				
		AREA	tempdata, DATA, READONLY,ALIGN=4
OUR_FIXED_DATA
	DCD	0x00000002,0x00000020,0x00000030,0x00000000
	DCW	0x4540, 0x50
	
		AREA	result, DATA, READWRITE, ALIGN=4	
RESULT	SPACE 4	; 32 words allocated for Array 1
	
	END