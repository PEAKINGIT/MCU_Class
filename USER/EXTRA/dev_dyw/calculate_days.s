	AREA |.text|, CODE, READONLY
	EXPORT calculate_days

; 函数：calculate_days
; 参数：
; r0 -> year
; 返回值：
; r0 -> 累计的天数

calculate_days
    PUSH     {r4-r5,lr}       ; 保存寄存器和返回地址
    MOV      r3, r0           ; year 存入 r3
    MOVS     r1, #0x00        ; day = 0
    
    ; 初始化 i = 1900
    MOVW     r0, #1900

loop_start
    ; if i >= year 跳出循环
    CMP      r0, r3
    BGE      loop_end
    
    ; 计算 i 是否是闰年
    ASRS     r4, r0, #31
    ADD      r4, r0, r4, LSR #30
    ASRS     r4, r4, #2
    SUB      r4, r0, r4, LSL #2
    CBNZ     r4, not_leap_year   ; 如果 i % 4 != 0 跳到 not_leap_year
    
    MOVS     r4, #100
    SDIV     r5, r0, r4
    MLS      r4, r4, r5, r0
    CBNZ     r4, leap_year_check ; 如果 i % 100 != 0 跳到 leap_year_check

not_leap_year
    MOVS     r4, #400
    SDIV     r5, r0, r4
    MLS      r4, r4, r5, r0
    CBNZ     r4, not_leap_year_final ; 如果 i % 400 != 0 跳到 not_leap_year_final

leap_year_check
    ADD      r1, r1, #366    ; day += 366
    B        loop_continue

not_leap_year_final
    ADD      r1, r1, #365    ; day += 365

loop_continue
    ; i++
    ADDS     r0, r0, #1
    B        loop_start

loop_end
    ; 返回 day
    MOV      r0, r1
    POP      {r4-r5,pc}      ; 恢复寄存器和返回地址

	END
