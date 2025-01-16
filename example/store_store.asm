; 5 12 25 38 51
; &a == 0x00000010 &b == 0x00000020
; 测试代码基本逻辑如下
; 初始状态 a = 0; b = 0;
; 初始条件 CPU0: 独占 b(M/E) CPU1: 共享 a(S) 
; 
; CPU0             CPU1
; a = 1;           while (0 == b) continue;
; <--- [wmb]       <--- [rmb]
; b = 1;           assert(1 == a)
; 
; CPU2 通过信号量来协调 CPU0 和 CPU1 准备环境,并发测试
; 包括 阶段1: 准备环境, 阶段2: 并发测试
; 
    MOV    R2           100000
    PID    R3                                     ; get processor id into R3
    JEQ    cpu0_start   R3           0            ; CPU0 goto cpu0_start label
    JEQ    cpu1_start   R3           1            ; CPU1 goto cpu1_start label
    JEQ    cpu2_start   R3           2            ; CPU2 goto cpu2_start label (发号施令)
    JMP    done                                   ; CPU3 goto done(直接休息)

cpu0_start:
    SDW    0                                      ; 等待发号施令者通过信号量 0 发送 stage 1 信号
cpu0_stage1:                                      ; 准备测试前置要求条件(独占 b(M/E))
    MOV    R0           0                         ; R0 = 0
    STR    [0x00000020] R0                        ; b = R0
    mb     ;---强制内存同步: 确保测试前置条件
    MOV    R0           1                         ; R0 = 1
    SUP    5                                      ; 告诉发号施令者, stage 1 完毕
cpu0_stage2:
    SDW    0                                      ; 等待发号施令者通过信号量 0 发送 stage 2 信号
    STR    [0x00000010] R0                        ; *(0x00000010) = R0 (a = 1)
;wmb ;取消注释,将不会出现 SS 问题
    STR    [0x00000020] R0                        ; *(0x00000020) = R0 (b = 1)
    SUP    5                                      ; 告诉发号施令者, stage 2 完毕
    SUB    R2           R2           1            ; R2 = R2 - 1
    JNE    cpu0_start   R2           0            ; goto cpu0_start label (if R2 != 0)
cpu0_end:
    END

cpu1_start:
    SDW    1                                      ; 等待发号施令者通过信号量 1 发送 stage 1 信号
cpu1_stage1:                                      ; 准备测试前置要求条件(共享 a(S))
    LDR    R0           [0x00000010]              ; R0 = *(0x00000010)
    mb     ;---强制内存同步: 确保测试前置条件
    SUP    5                                      ; 告诉发号施令者, stage 1 完毕
cpu1_stage2:
    SDW    1                                      ; 等待发号施令者通过信号量 1 发送 stage 2 信号
cpu1_retry:
    LDR    R0           [0x00000020]              ; R0 = *(0x00000020)
    JEQ    cpu1_retry   R0           0            ; if (R0 == 0) goto cpu1_retry
;rmb ;取消注释,将不会出现 LL 问题
    LDR    R0           [0x00000010]              ; R0 = *(0x00000010)
    JNE    cpu1_failed  R0           1            ; if (R0 != 1) goto cpu1_failed
    SUP    5                                      ; 告诉发号施令者, stage 2 完毕
    SUB    R2           R2           1            ; R2 = R2 - 1
    JNE    cpu1_start   R2           0            ; goto cpu1_start label (if R2 != 0)
cpu1_end:
    END
cpu1_failed:
    BUG

cpu2_start:
    MOV    R0           0                         ; R0 = 0
    STR    [0x00000010] R0                        ; *(0x00000010) = R0 (a = 0)
    STR    [0x00000020] R0                        ; *(0x00000020) = R0 (b = 0)
    mb     ;---强制内存同步: 确保测试前置条件
    SUP    0                                      ; 告诉 CPU0 可以进入 stage1 阶段
    SUP    1                                      ; 告诉 CPU1 可以进入 stage1 阶段
    SDW    5                                      ; 等待 CPU0/CPU1处理 stage1 完成
    SDW    5                                      ; 等待 CPU0/CPU1处理 stage1 完成
    SUP    0                                      ; 告诉 CPU0 可以进入 stage2 阶段
    SUP    1                                      ; 告诉 CPU1 可以进入 stage2 阶段
    SDW    5                                      ; 等待 CPU0/CPU1处理 stage2 完成
    SDW    5                                      ; 等待 CPU0/CPU1处理 stage2 完成
    SUB    R2           R2           1            ; R2 = R2 - 1
    JNE    cpu2_start   R2           0            ; goto cpu2_start label (if R2 != 0)

done:
    END
