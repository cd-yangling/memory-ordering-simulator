;测试单 CPU 的基本逻辑正确性
;&a == 0xa0000010 &b == 0xa0000020
;a = 1
;b = a + 2
;assert(b==3)

; 5 12 25 38 51
    PID    R1                                     ; get processor id into R1
    JNE    done         R1           0            ; just CPU0 do below test, other CPU goto done
    MOV    R0           0xa0000010                ; R0 = 0xa0000010
    MOV    R1           1                         ; R1 = 1
    STR    [R0]         R1                        ; *(0xa0000010) = R1
    ADD    R2           R1           2            ; R2 = R1 + 2
    STR    [0xa0000020] R2                        ; *(0xa0000020) = R2
    MOV    R0           0xa0000020                ; R0 = 0xa0000020
    LDR    R3           [R0]                      ; R3 = *(0xa0000020)
    JEQ    done         R3           3            ; if (R3 == 3) goto done
	BUG
done:
    END

