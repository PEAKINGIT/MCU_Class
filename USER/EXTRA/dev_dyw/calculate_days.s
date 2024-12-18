	AREA |.text|, CODE, READONLY
	EXPORT calculate_days

; ������calculate_days
; ������
; r0 -> year
; ����ֵ��
; r0 -> �ۼƵ�����

calculate_days
    PUSH     {r4-r5,lr}       ; ����Ĵ����ͷ��ص�ַ
    MOV      r3, r0           ; year ���� r3
    MOVS     r1, #0x00        ; day = 0
    
    ; ��ʼ�� i = 1900
    MOVW     r0, #1900

loop_start
    ; if i >= year ����ѭ��
    CMP      r0, r3
    BGE      loop_end
    
    ; ���� i �Ƿ�������
    ASRS     r4, r0, #31
    ADD      r4, r0, r4, LSR #30
    ASRS     r4, r4, #2
    SUB      r4, r0, r4, LSL #2
    CBNZ     r4, not_leap_year   ; ��� i % 4 != 0 ���� not_leap_year
    
    MOVS     r4, #100
    SDIV     r5, r0, r4
    MLS      r4, r4, r5, r0
    CBNZ     r4, leap_year_check ; ��� i % 100 != 0 ���� leap_year_check

not_leap_year
    MOVS     r4, #400
    SDIV     r5, r0, r4
    MLS      r4, r4, r5, r0
    CBNZ     r4, not_leap_year_final ; ��� i % 400 != 0 ���� not_leap_year_final

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
    ; ���� day
    MOV      r0, r1
    POP      {r4-r5,pc}      ; �ָ��Ĵ����ͷ��ص�ַ

	END
