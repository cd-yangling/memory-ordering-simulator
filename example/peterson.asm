; 5 12 25 38 51
; https://en.wikipedia.org/wiki/Peterson%27s_algorithm
; peterson 算法, 在没有硬件支持的情况实现临界区的方案的算法
; 缓存行隔离                        缓存行伪共享
; flg1  0x00000000                  flg1  0x00000000
; flg2  0x00000010                  flg2  0x00000004
; turn  0x00000020                  turn  0x00000008
; count 0x00000030                  count 0x0000000c
; CPU0                                           CPU1
; --------------------------------------------------------------------
; flg1 = 1;                                      flg2 = 1;
; turn = 200;                                    turn = 100;
; while ((flg2 == 1) && (turn == 200)) {}        while ((flg1 == 1) && (turn == 100)) {}
; count++                                        count++;
; flg1 = 0;                                      flg2 = 0;
; CPU2 通过信号量来协调 CPU0 和 CPU1 并发测试, 并且观察最终的结果

    MOV    R3           100000
    PID    R2                                     ; get processor id into R2
    JEQ    cpu0_start   R2           0            ; CPU0 goto cpu0_start label (if R2 == 0)
    JEQ    cpu1_start   R2           1            ; CPU1 goto cpu1_start label (if R2 == 1)
    JEQ    cpu2_start   R2           2            ; CPU2 goto cpu2_start label (if R2 == 2) (发号施令)
    END                                           ; CPU3 goto done(直接休息)
; ------------------------------------------ cpu0 proc
cpu0_start:
    SDW    0                                      ; 等待发号施令者通过信号量 0 发送开始信号
    MOV    R2           0                         ; R2 = 0
cpu0_for:
    JEQ    cpu0_done    R2           R3           ; if (R2 == R3) goto cpu0_done
    MOV    R0           1                         ; R0 = 1
    MOV    R1           200                       ; R1 = 200
    STR    [0x00000000] R0                        ; flg1 = R0
    STR    [0x00000020] R1                        ; turn = R1
;mb ; 屏障的地方
cpu0_while:
    LDR    R0           [0x00000010]              ; R0 = flg2
    JNE    cpu0_csenter R0           1            ; if (flg2 !=   1) goto cpu0_csenter
    LDR    R0           [0x00000020]              ; R0 = turn
    JNE    cpu0_csenter R0           200          ; if (turn != 200) goto cpu0_csenter
    JMP    cpu0_while                             ; goto cpu0_while
cpu0_csenter:
; 这里就是临界区了
    LDR    R0           [0x00000030]              ; R0 = count
    ADD    R0           R0           1            ; R0 = R0 + 1
    STR    [0x00000030] R0                        ; count = R0
cpu0_csleave:
    MOV    R0           0                         ; R0 = 0
    STR    [0x00000000] R0                        ; flg1 = R0

    ADD    R2           R2           1            ; R2 = R2 + 1
    JMP    cpu0_for                               ; goto cpu0_for

cpu0_done:
    SUP    5                                      ; 告诉 CPU2 我结束了
    END                                           ; 任务结束

; ------------------------------------------ cpu1 proc
cpu1_start:
    SDW    1                                      ; 等待发号施令者通过信号量 1 发送开始信号
    MOV    R2           0                         ; R2 = 0
cpu1_for:
    JEQ    cpu1_done    R2           R3           ; if (R2 == R3) goto cpu1_done
    MOV    R0           1                         ; R0 = 1
    MOV    R1           100                       ; R1 = 100
    STR    [0x00000010] R0                        ; flg2 = R0
    STR    [0x00000020] R1                        ; turn = R1
;mb ; 屏障的地方
cpu1_while:
    LDR    R0           [0x00000000]              ; R0 = flg1
    JNE    cpu1_csenter R0           1            ; if (flg1 !=   1) goto cpu1_csenter
    LDR    R0           [0x00000020]              ; R0 = turn
    JNE    cpu1_csenter R0           100          ; if (turn != 100) goto cpu1_csenter
    JMP    cpu1_while                             ; goto cpu1_while
cpu1_csenter:
; 这里就是临界区了
    LDR    R0           [0x00000030]              ; R0 = count
    ADD    R0           R0           1            ; R0 = R0 + 1
    STR    [0x00000030] R0                        ; count = R0
cpu1_csleave:
    MOV    R0           0                         ; R0 = 0
    STR    [0x00000010] R0                        ; flg2 = R0

    ADD    R2           R2           1            ; R2 = R2 + 1
    JMP    cpu1_for                               ; goto cpu1_for

cpu1_done:
    SUP    5                                      ; 告诉 CPU2 我结束了
    END                                           ; 任务结束

; ------------------------------------------ cpu2 proc
cpu2_start:
    SUP    0                                      ; 告诉 CPU0 可以开始测试
    SUP    1                                      ; 告诉 CPU1 可以开始测试
    SDW    5                                      ; 等待 CPU0/CPU1结束测试
    SDW    5                                      ; 等待 CPU0/CPU1结束测试
    mb     ;---强制内存同步: 确保观察结果的正确性
    LDR    R0           [0x00000030]              ; R0 = count
    MUL    R3           R3           2            ; R3 = R3 * 2
    JEQ    cpu2_done    R0           R3           ; if (R3 == R0) goto cpu2_done
    BUG

cpu2_done:
    END
