;并行全局变量 ++, 测试 MESI 协议的正确性
;4 个 CPU 分别对内存地址是 0xa0000000 这个变量进行++,
;1: CPU0 处理 if (v % 0 == 0) v++
;2: CPU1 处理 if (v % 1 == 1) v++
;3: CPU2 处理 if (v % 2 == 2) v++
;4: CPU3 处理 if (v % 3 == 3) v++
;for (;v < 1000000;)
;{
;	if (pid == v % 4)
;		v = v + 1;
;}

; 5 12 25 38 51
    PID    R1                                     ; get processor id into R1
retry:
    LDR    R3           [0xa0000000]              ; R3 = *(0xa0000000)
    JEQ    done         R3           1000000      ; if (R3 == 1000000) goto done
    MOD    R2           R3           4            ; R2 = R3 % 4
    JNE    retry        R2           R1           ; if (R2 != R1) goto retry
    ADD    R3           R3           1            ; R3 = R3 + 1
    STR    [0xa0000000] R3                        ; *(0xa0000000) = R3
    JMP    retry                                  ; unconditional goto retry

done:
    END
