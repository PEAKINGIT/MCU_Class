
;����1����                       
USART1_BASE         EQU    0x40013800 
USART1_SR           EQU    (USART1_BASE + 0x00) 
USART1_DR           EQU    (USART1_BASE + 0x04) 

	AREA UsartSend, CODE, READONLY
    EXPORT usart_send

; Function: usart_send
; params:
;   R0 = �����ֽ� (byte to send)
usart_send

    ; �ȴ� USART1 �ķ������ݼĴ����� (TXE)
Wait_TXE
    LDR r1, =USART1_BASE             ; USART1 �Ļ���ַ
    LDR r2, =USART1_SR             ; ��ȡ USART1_SR 
	LDR r2,[r2]
    TST r2, #0X40                   ; 
    BEQ Wait_TXE                    ; ��� TXE Ϊ 0��������ȴ�

    ; ��������
    LDR r3, =USART1_DR
    STRB r0, [r3] 					; return ch

    BX LR
	NOP
    END
