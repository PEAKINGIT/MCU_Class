	AREA |.text|, CODE, READONLY
	EXPORT linear_predict

; Function: linear_predict
; Parameters:
;   R0 = x0
;   R1 = y0
;   R2 = x1
;   R3 = y1
;   R4 = x
; Returns:
;   R0 = predicted y value

linear_predict
	PUSH {r4-r7,lr}
	MOV r4,r0
	LDR r5,[sp,#0x14]
	SUBS r0,r3,r1
	SUBS r7,r2,r4
	SDIV r6,r0,r7
	SUBS r0,r5,r4
	MLA r0,r6,r0,r1
	
	POP {r4-r7,pc}
	MOVS r0,r0
	
	END

