	AREA ArrayCpy, CODE, READONLY     ; �����������
    EXPORT array_cpy                  ; ���� CopyArray ����

; Function: CopyArray
; Parameters:
;   R0 = src     ; Դ�����ַ
;   R1 = dest    ; Ŀ�������ַ
;   R2 = length  ; �������ֽ���

array_cpy
    PUSH {r4-r6, lr}

    MOV r4, r0                       ; r4 = src (Դ�����ַ)
    MOV r5, r1                       ; r5 = dest (Ŀ�������ַ)
    MOV r6, r2                       ; r6 = length (�������ֽ���)

Loop
    CMP r6, #0                      
    BEQ Done                          ; ���ʣ���ֽ���Ϊ 0����ת�� Done

    LDRB r7, [r4], #1                ; �� src (r4) �ж�ȡһ���ֽڵ� r7��r4 ���� 1
    STRB r7, [r5], #1                ; �� r7 ���ֽڴ洢�� dest (r5)��r5 ���� 1

    SUBS r6, r6, #1                  ; length (r6) �� 1������������־
    BNE Loop                         ; ��� length (r6) ��Ϊ 0������ѭ��

Done
	MOVS r0, r0
    POP {r4-r6, pc}

    END
	