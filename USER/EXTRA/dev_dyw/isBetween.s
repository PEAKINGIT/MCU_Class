	AREA |.text|, CODE, READONLY
	EXPORT isBetween

; 函数：isBetween
; 参数：
; r0 -> month
; r1 -> day
; r2 -> startMonth
; r3 -> startDay
; r4 -> endMonth
; r5 -> endDay

isBetween
    PUSH     {r4-r6,lr}          ; 保存寄存器和返回地址
    MOV      r4, r2              ; r4 = startMonth
    MOV      r5, r3              ; r5 = startDay
    MOV      r6, r0              ; r6 = month
    MOV      r7, r1              ; r7 = day
    LDRD     r2, r3, [sp, #0x10] ; 加载 endMonth 和 endDay

    ; if (startMonth == endMonth)
    CMP      r4, r2              ; 比较 startMonth 和 endMonth
    BNE      check_startMonth    ; 如果不相等，跳转到 check_startMonth

    ; return (month == startMonth && day >= startDay && day < endDay)
    CMP      r6, r4              ; 比较 month 和 startMonth
    BNE      check_startMonth    ; 如果不相等，跳转到 check_startMonth
    CMP      r7, r5              ; 比较 day 和 startDay
    BCC      check_startMonth    ; 如果小于 startDay，跳转到 check_startMonth
    CMP      r7, r3              ; 比较 day 和 endDay
    BCS      check_startMonth    ; 如果大于等于 endDay，跳转到 check_startMonth
    MOVS     r0, #0x01           ; 返回 1
    POP      {r4-r6,pc}          ; 恢复寄存器和返回地址

check_startMonth
    ; if (month == startMonth && day >= startDay) return 1;
    CMP      r6, r4              ; 比较 month 和 startMonth
    BNE      check_endMonth      ; 如果不相等，跳转到 check_endMonth
    CMP      r7, r5              ; 比较 day 和 startDay
    BCC      check_endMonth      ; 如果小于 startDay，跳转到 check_endMonth
    MOVS     r0, #0x01           ; 返回 1
    POP      {r4-r6,pc}          ; 恢复寄存器和返回地址

check_endMonth
    ; if (month == endMonth && day < endDay) return 1;
    CMP      r6, r2              ; 比较 month 和 endMonth
    BNE      check_between       ; 如果不相等，跳转到 check_between
    CMP      r7, r3              ; 比较 day 和 endDay
    BCS      check_between       ; 如果大于等于 endDay，跳转到 check_between
    MOVS     r0, #0x01           ; 返回 1
    POP      {r4-r6,pc}          ; 恢复寄存器和返回地址

check_between
    ; if (month > startMonth && month < endMonth) return 1;
    CMP      r6, r4              ; 比较 month 和 startMonth
    BLS      default_return      ; 如果小于等于 startMonth，跳转到 default_return
    CMP      r6, r2              ; 比较 month 和 endMonth
    BCS      default_return      ; 如果大于等于 endMonth，跳转到 default_return
    MOVS     r0, #0x01           ; 返回 1
    POP      {r4-r6,pc}          ; 恢复寄存器和返回地址

default_return
    ; return 0;
    MOVS     r0, #0x00           ; 返回 0
    POP      {r4-r6,pc}          ; 恢复寄存器和返回地址

	END