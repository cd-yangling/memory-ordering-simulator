; 5 12 25 38 51
; &a == 0x00000010 &b == 0x00000020
; &x == 0xa0000010 &y == 0xa0000020
; 初始状态 a = 0; b = 0;
; CPU0       CPU1
; b = 1      a = 1;
; x = a;     y = b;
; CPU2 作为观察者断言
; assert( x==0 ) && assert( y == 0 )
; R0 作为常量一直是0,
; R1 作为常量一直是1,
; R2 作为循环次数变量初始化成 1000000
; R3 作为临时的使用

	XOR    R0           R0           R0           ; R0 = 0 R0 = R0 ^ R0
    MOV    R1           1                         ; R1 = 1
    MOV    R2           100000                    ; R2 = 100000(测试10w次)
    PID    R3                                     ; get processor id into R3
    JEQ    cpu0_proc    R3           0            ; CPU0 goto cpu0_proc label
    JEQ    cpu1_proc    R3           1            ; CPU1 goto cpu1_proc label
    JEQ    cpu2_proc    R3           2            ; CPU2 goto cpu2_proc label (观察者)
    JMP    done                                   ; CPU3 goto done(直接休息)

cpu0_proc:
    JEQ    done         R2           0            ; if (R2 == 0) goto done 结束测试
    rmb    ;--------------------------------------- 确保能看到正确的 b
    SDW    0                                      ; 等待观察者通过信号量 0 发号施令开始运行的信号
    STR    [0x00000020] R1                        ; b = 1   { *(0x00000020) = 1 }
    LDR    R3           [0x00000010]              ; R3 = a
    STR    [0xa0000010] R3                        ; x = R3;
    wmb    ;-------------------------------------- 确保观察者能看到正确的 x
    SUB    R2           R2           1            ; R2 = R2 - 1
    SUP    2                                      ; 告诉观察者自己正常结束本轮测试
    JMP    cpu0_proc                              ; goto cpu0_proc label

cpu1_proc:
    JEQ    done         R2           0            ; if (R2 == 0) goto done 结束测试
    rmb    ;--------------------------------------- 确保能看到正确的 a
    SDW    1                                      ; 等待观察者通过信号量 1 发号施令开始运行的信号
    STR    [0x00000010] R1                        ; a = 1
    LDR    R3           [0x00000020]              ; R3 = b
    STR    [0xa0000020] R3                        ; y = R3;
    wmb    ;--------------------------------------- 确保观察者能看到正确的 y
    SUB    R2           R2           1            ; R2 = R2 - 1
    SUP    2                                      ; 告诉观察者自己正常结束本轮测试
    JMP    cpu1_proc                              ; goto cpu1_proc label

cpu2_proc:
    JEQ    done         R2           0            ; if (R2 == 0) goto done 结束测试
    SUP    0                                      ; 告诉 CPU0 可以开始运行了
    SUP    1                                      ; 告诉 CPU1 可以开始运行了
    SDW    2                                      ; 等待 CPU 0/CPU 1 结束
    SDW    2                                      ; 等待 CPU 0/CPU 1 结束
    rmb    ;--------------------------------------- 确保自己能看到正确的 x 和 y
    LDR    R3           [0xa0000010]              ; R3 = a
    JNE    next_round   R3           0            ; if (R3 != 0) goto next;
    LDR    R3           [0xa0000020]              ; R3 = b
    JNE    next_round   R3           0            ; if (R3 != 0) goto next;
    BUG                                           ; a == 0 && b == 0
next_round:
    STR    [0x00000010] R0                        ; a = 0
    STR    [0x00000020] R0                        ; b = 0
    wmb    ;--------------------------------------- 确保 Cpu0/Cpu1 能看到正确的 a=0 和 b = 0
    SUB    R2           R2           1            ; R2 = R2 - 1
	JMP    cpu2_proc                              ; 继续测试

done:
    END
