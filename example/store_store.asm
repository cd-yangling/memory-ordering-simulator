; 5 12 25 38 51
    MOV    R2           0                         ; get processor id into R1
    PID    R3                                     ; get processor id into R1
    JEQ    cpu0_start   R3           0            ; CPU0 goto cpu0_start label
    JEQ    cpu1_start   R3           1            ; CPU1 goto cpu1_start label
    JEQ    cpu2_start   R3           2            ; CPU2 goto cpu2_start label (观察者)
    JMP    done                                   ; CPU3 goto done(直接休息)

cpu0_start:
    SDW    0                                      ; 等待观察者通过信号量 0 发号施令开始运行的信号
    MOV    R2           0
    STR    [0x00000020] R2                        ; 因为这个测试场景需要 cpu0 首先独占 b,
wmb
;;mb????
    LDR    R2           [0xa0000000]              ; R2 = *(0xa0000000)
    JEQ    done         R2           1000000      ; if (R2 == 1000000) goto done 结束测试
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 下面的代码是正文测试代码 (CPU0 执行的代码片段)
; 下面的汇编模拟的代码用 C 语言描述如下, &a == 0x00000010 &b == 0x00000020
; a = 1
; b = 1

    MOV    R0           1		                  ; R0 = 1
    STR    [0x00000010] R0                        ; *(0x00000010) = R0
;没有下面的这个指令将导致 BUG 指令被执行
;wmb
    STR    [0x00000020] R0                        ; *(0x00000020) = R0
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    SUP    2                                      ; 告诉观察者自己正常结束本轮测试
    JMP    cpu0_start                             ; goto cpu0_start label

cpu1_start:
    SDW    1                                      ; 等待观察者通过信号量 1 发号施令开始运行的信号
    LDR    R2           [0xa0000000]              ; R2 = *(0xa0000000)
    JEQ    done         R2           1000000      ; if (R2 == 1000000) goto done 结束测试
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 下面的代码是正文测试代码 (CPU1 执行的代码片段)
; 下面的汇编模拟的代码用 C 语言描述如下, &a == 0x00000010 &b == 0x00000020
; while (0 == b) continue;
; assert(1 == a);
cpu1_retry:
    LDR    R0           [0x00000020]              ; R0 = *(0x00000020)
    JEQ    cpu1_retry   R0           0            ; if (R0 == 0) goto cpu1_retry
    LDR    R0           [0x00000010]              ; R0 = *(0x00000010)
    JNE    cpu1_failed  R0           1            ; if (R0 != 1) goto cpu1_failed
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    SUP    2                                      ; 告诉观察者自己正常结束本轮测试
    JMP    cpu1_start                             ; goto cpu1_start label
cpu1_failed:
    BUG
cpu2_start:
    MOV    R2           0                         ; R2 = 0
cpu2_retry:
    JEQ    cpu2_done    R2           1000000      ; 测试了 1000000 次 可以结束测试了
    SUP    1                                      ; 告诉 CPU1 可以开始运行了
    SUP    0                                      ; 告诉 CPU0 可以开始运行了
    ADD    R2           R2           1            ; R2 = R2 + 1
    SDW    2                                      ; 等待 CPU 0/CPU 1 结束
    SDW    2                                      ; 等待 CPU 0/CPU 1 结束
	JMP    cpu2_retry                             ; 继续测试
cpu2_done:
    STR    [0xa0000000] R2                        ; *(0xa0000000) = R2
;??? mb
    SUP    0                                      ; 告诉 CPU0 可以开始运行了
    SUP    1                                      ; 告诉 CPU1 可以开始运行了
done:
    END
