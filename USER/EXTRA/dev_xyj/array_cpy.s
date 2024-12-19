	AREA ArrayCpy, CODE, READONLY     ; 定义代码区域
    EXPORT array_cpy                  ; 导出 CopyArray 函数

; Function: CopyArray
; Parameters:
;   R0 = src     ; 源数组地址
;   R1 = dest    ; 目标数组地址
;   R2 = length  ; 拷贝的字节数

array_cpy
    PUSH {r4-r6, lr}

    MOV r4, r0                       ; r4 = src (源数组地址)
    MOV r5, r1                       ; r5 = dest (目标数组地址)
    MOV r6, r2                       ; r6 = length (拷贝的字节数)

Loop
    CMP r6, #0                      
    BEQ Done                          ; 如果剩余字节数为 0，跳转到 Done

    LDRB r7, [r4], #1                ; 从 src (r4) 中读取一个字节到 r7，r4 增加 1
    STRB r7, [r5], #1                ; 将 r7 的字节存储到 dest (r5)，r5 增加 1

    SUBS r6, r6, #1                  ; length (r6) 减 1，更新条件标志
    BNE Loop                         ; 如果 length (r6) 不为 0，继续循环

Done
	MOVS r0, r0
    POP {r4-r6, pc}

    END
	