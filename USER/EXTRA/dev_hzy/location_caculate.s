        AREA    text, CODE, READONLY
        EXPORT  location_caculate

; Function: location_caculate
; Parameters:
;   R0 = time_data_value
; Returns:
;   R0 = (time_data_value * 180) / 50 + 30 的计算结果

location_caculate
        PUSH    {r4-r7,lr}            ; 保存函数执行过程中可能用到的寄存器以及链接寄存器（lr），以便函数结束后恢复现场

        MOV     r1,r0                ; 将传入的参数 time_data_value（存储在 R0 中）复制到 R1，方便后续对参数值的使用
        MOV     r4,r0                ; 额外备份一份 time_data_value 到 R4，以防后续操作中误修改原始参数，可用于调试或后续可能的检查需求

        MOVS    r0,#0xB4              ; 把立即数 180（十六进制表示为 0xB4）加载到 R0，作为乘法运算的一个操作数

        MULS    r0,r1,r0              ; 执行乘法：R1（time_data_value） * R0（180），结果存放在 R0 中

        MOVS    r2,#0x32              ; 将立即数 50（十六进制表示为 0x32）加载到 R2，用于后续除法运算

        SDIV    r0,r0,r2              ; 执行除法：R0（time_data_value * 180 的结果） / R2（50），商存放在 R0 中

        MOVS    r5,#0x1E              ; 将立即数 30（十六进制表示为 0x1E）加载到 R5，准备用于加法运算

        ADDS    r0,r0,r5              ; 执行加法：R0（(time_data_value * 180) / 50 的结果） + R5（30），得到最终结果存放在 R0 中

        POP     {r4-r7,pc}           ; 恢复之前保存的寄存器，并通过 pc（程序计数器）返回到调用函数的地方继续执行后续代码
        MOVS    r0,r0                 ; （这条指令可视具体情况进一步分析其必要性，当前按原格式保留，它一般对主要功能影响不大，但某些场景下可能有细微作用）

        END