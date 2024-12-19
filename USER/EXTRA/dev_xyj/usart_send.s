
;串口1控制                       
USART1_BASE         EQU    0x40013800 
USART1_SR           EQU    (USART1_BASE + 0x00) 
USART1_DR           EQU    (USART1_BASE + 0x04) 

	AREA UsartSend, CODE, READONLY
    EXPORT usart_send

; Function: usart_send
; params:
;   R0 = 数据字节 (byte to send)
usart_send

    ; 等待 USART1 的发送数据寄存器空 (TXE)
Wait_TXE
    LDR r1, =USART1_BASE             ; USART1 的基地址
    LDR r2, =USART1_SR             ; 读取 USART1_SR 
	LDR r2,[r2]
    TST r2, #0X40                   ; 
    BEQ Wait_TXE                    ; 如果 TXE 为 0，则继续等待

    ; 发送数据
    LDR r3, =USART1_DR
    STRB r0, [r3] 					; return ch

    BX LR
	NOP
    END
