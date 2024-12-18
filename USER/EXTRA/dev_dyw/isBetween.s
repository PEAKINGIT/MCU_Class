	AREA |.text|, CODE, READONLY
	EXPORT isBetween

; ������isBetween
; ������
; r0 -> month
; r1 -> day
; r2 -> startMonth
; r3 -> startDay
; r4 -> endMonth
; r5 -> endDay

isBetween
    PUSH     {r4-r6,lr}          ; ����Ĵ����ͷ��ص�ַ
    MOV      r4, r2              ; r4 = startMonth
    MOV      r5, r3              ; r5 = startDay
    MOV      r6, r0              ; r6 = month
    MOV      r7, r1              ; r7 = day
    LDRD     r2, r3, [sp, #0x10] ; ���� endMonth �� endDay

    ; if (startMonth == endMonth)
    CMP      r4, r2              ; �Ƚ� startMonth �� endMonth
    BNE      check_startMonth    ; �������ȣ���ת�� check_startMonth

    ; return (month == startMonth && day >= startDay && day < endDay)
    CMP      r6, r4              ; �Ƚ� month �� startMonth
    BNE      check_startMonth    ; �������ȣ���ת�� check_startMonth
    CMP      r7, r5              ; �Ƚ� day �� startDay
    BCC      check_startMonth    ; ���С�� startDay����ת�� check_startMonth
    CMP      r7, r3              ; �Ƚ� day �� endDay
    BCS      check_startMonth    ; ������ڵ��� endDay����ת�� check_startMonth
    MOVS     r0, #0x01           ; ���� 1
    POP      {r4-r6,pc}          ; �ָ��Ĵ����ͷ��ص�ַ

check_startMonth
    ; if (month == startMonth && day >= startDay) return 1;
    CMP      r6, r4              ; �Ƚ� month �� startMonth
    BNE      check_endMonth      ; �������ȣ���ת�� check_endMonth
    CMP      r7, r5              ; �Ƚ� day �� startDay
    BCC      check_endMonth      ; ���С�� startDay����ת�� check_endMonth
    MOVS     r0, #0x01           ; ���� 1
    POP      {r4-r6,pc}          ; �ָ��Ĵ����ͷ��ص�ַ

check_endMonth
    ; if (month == endMonth && day < endDay) return 1;
    CMP      r6, r2              ; �Ƚ� month �� endMonth
    BNE      check_between       ; �������ȣ���ת�� check_between
    CMP      r7, r3              ; �Ƚ� day �� endDay
    BCS      check_between       ; ������ڵ��� endDay����ת�� check_between
    MOVS     r0, #0x01           ; ���� 1
    POP      {r4-r6,pc}          ; �ָ��Ĵ����ͷ��ص�ַ

check_between
    ; if (month > startMonth && month < endMonth) return 1;
    CMP      r6, r4              ; �Ƚ� month �� startMonth
    BLS      default_return      ; ���С�ڵ��� startMonth����ת�� default_return
    CMP      r6, r2              ; �Ƚ� month �� endMonth
    BCS      default_return      ; ������ڵ��� endMonth����ת�� default_return
    MOVS     r0, #0x01           ; ���� 1
    POP      {r4-r6,pc}          ; �ָ��Ĵ����ͷ��ص�ַ

default_return
    ; return 0;
    MOVS     r0, #0x00           ; ���� 0
    POP      {r4-r6,pc}          ; �ָ��Ĵ����ͷ��ص�ַ

	END