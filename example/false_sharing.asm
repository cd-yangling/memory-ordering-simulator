; 测试 伪共享的情况 (a 和 b) 都在一个 cache line 中
; CPU0 单独写 a, CPU1 单独写 b, 最终观察者检查 a 和 b 的情况
; &a == 0xb0000004 &b == 0xb0000008
; CPU0                           CPU1
; for (i = 0; i < 100000; i++)   for (i = 0; i < 100000; i++)
;      a++;                           b++;
; 最终 CPU2 检查 a 和 b  应该都是100w
; R2 作为循环次数变量初始化成 1000000

; 5 12 25 38 51
    XOR    R0           R0           R0           ; R0 = 0 R0 = R0 ^ R0
    PID    R3                                     ; get processor id into R3
    MOV    R2           1000000                   ; R2 = 1000000(测试100w次)
    JEQ    cpu0_proc    R3           0            ; CPU0 goto cpu0_proc label
    JEQ    cpu1_proc    R3           1            ; CPU1 goto cpu1_proc label
    JEQ    cpu2_proc    R3           2            ; CPU2 goto cpu2_proc label (观察者)
    JMP    done                                   ; CPU3 goto done(直接休息)

cpu0_proc:
    SDW    0                                      ; 等待观察者通过信号量 0 发号施令开始运行的信号
cpu0_retry:
    STR    [0xb0000004] R0                        ; *(0xb0000004) = R0
    JEQ    cpu0_done    R2           0            ; if (R2 == 0) goto cpu0_done 结束测试
    ADD    R0           R0           1            ; R0 = R0 + 1
    SUB    R2           R2           1            ; R2 = R2 - 1
    JMP    cpu0_retry;                            ; goto cpu0_retry
cpu0_done:
    wmb                                           ; 确保观察者能看见最新的 a
    SUP    2                                      ; 告诉观察者自己结束了测试
    END

cpu1_proc:
    SDW    1                                      ; 等待观察者通过信号量 1 发号施令开始运行的信号
cpu1_retry:
    STR    [0xb0000008] R0                        ; *(0xb0000008) = R0
    JEQ    cpu1_done    R2           0            ; if (R2 == 0) goto cpu1_done 结束测试
    ADD    R0           R0           1            ; R0 = R0 + 1
    SUB    R2           R2           1            ; R2 = R2 - 1
    JMP    cpu1_retry;                            ; goto cpu1_retry
cpu1_done:
    wmb                                           ; 确保观察者能看见最新的 b
    SUP    2                                      ; 告诉观察者自己结束了测试
    END

cpu2_proc:
    SUP    0                                      ; 告诉 CPU0 可以开始运行了
    SUP    1                                      ; 告诉 CPU1 可以开始运行了
    SDW    2                                      ; 等待 CPU 0/CPU 1 结束
    SDW    2                                      ; 等待 CPU 0/CPU 1 结束
    rmb                                           ; 确保 Cpu0/Cpu1 能看到正确的 a 和 b
    LDR    R3           [0xb0000004]              ; R3 = a
    JNE    failed       R3           R2           ; if (R3 != R2) goto failed;
    LDR    R3           [0xb0000008]              ; R3 = b
    JNE    failed       R3           R2           ; if (R3 != R2) goto failed;
done:
    END                                           ; 正常结束啦

failed:
    BUG


