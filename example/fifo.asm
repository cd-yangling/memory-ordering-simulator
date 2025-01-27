; 5 12 25 38 51
; fifo 简易实现: 生产者每次写入一个32位数值,消费者也按照32位数值消费(队列固定1024)

; static unsigned int p = 0;
; static unsigned int c = 0;
; static unsigned int d[1024];
; p 0x10000000 
; c 0x10000004
; d 0xa0000000

; CPU0(生产者)                                   CPU1(消费者)
; --------------------------------------------------------------------

; void producer(void)                            void consumer(void)
; {                                              {
;     for (int i = 0; i < 100000; i++)               for (;c != 100000;)
;     {                                              {
;         if ((p + 1) % 1024 != c % 1024)                while (p % 1024 != c % 1024)
;         {                                              {
;             d[p % 1024] = p;                               assert(d[c % 1024] == c);
;             p++;                                           c++;
;         } else i--;                                    }
;     }                                              }
; }                                               }

    MOV    R3           100000
    PID    R2                                     ; get processor id into R2
    JEQ    cpu0_start   R2           0            ; CPU0 goto cpu0_start label (if R2 == 0)
    JEQ    cpu1_start   R2           1            ; CPU1 goto cpu1_start label (if R2 == 1)
    END                                           ; CPU3 and CPU2 goto done(直接休息)
; ------------------------------------------ cpu0 proc
cpu0_start:
    MOV    R2           0                         ; R2 = 0
cpu0_for:
    JEQ    cpu0_done    R2           R3           ; if (R2 == R3) goto cpu0_done
    LDR    R0           [0x10000000]              ; R0 = p
    LDR    R1           [0x10000004]              ; R1 = c
    ADD    R0           R0           1            ; R0 = R0 + 1
    MOD    R0           R0           1024         ; R0 = R0 % 1024
    MOD    R1           R1           1024         ; R1 = R1 % 1024
    JEQ    cpu0_for_rb  R0           R1           ; if ((p + 1) % 1024 == c % 1024) i--
    LDR    R0           [0x10000000]              ; R0 = p
    MOV    R1           R0                        ; R1 = R0
    MOD    R1           R1           1024         ; R1 = R1 % 1024
    MUL    R1           R1           4            ; R1 = R1 * 4
    ADD    R1           R1           0xa0000000   ; R1 = R1 + 0xa0000000
    STR    [R1]         R0                        ; d[p % 1024] = p
    ADD    R0           R0           1            ; R0 = R0 + 1
;mb ; 屏障的地方
    STR    [0x10000000] R0                        ; p++;
    ADD    R2           R2           1            ; R2 = R2 + 1
cpu0_for_rb:
    JMP    cpu0_for                               ; goto cpu0_for

cpu0_done:
    END                                           ; 任务结束

; ------------------------------------------ cpu1 proc
cpu1_start:
    MOV    R2           0                         ; R2 = 0
cpu1_for:
    JEQ    cpu1_done    R2           R3           ; if (R2 == R3) goto cpu1_done

cpu1_while:
    LDR    R0           [0x10000000]              ; R0 = p
    LDR    R1           [0x10000004]              ; R1 = c
    MOD    R0           R0           1024         ; R0 = R0 % 1024
    MOD    R1           R1           1024         ; R1 = R1 % 1024
    JEQ    cpu1_while   R1           R0           ; if (R0 == R1) goto cpu1_while
    LDR    R1           [0x10000004]              ; R1 = c
    MOV    R0           R1                        ; R0 = R1
    MOD    R0           R0           1024         ; R0 = R0 % 1024
    MUL    R0           R0           4            ; R0 = R0 * 4
    ADD    R0           R0           0xa0000000   ; R0 = R0 + 0xa0000000
    LDR    R0           [R0]                      ; R0 = d[c % 1024]
    JNE    cpu1_failed  R0           R1           ; if (R0 != R1) goto cpu1_failed
    ADD    R1           R1           1            ; R1 = R1 + 1
;mb ; 屏障的地方
    STR    [0x10000004] R1                        ; c++
    MOV    R2           R1                        ; R2 = R1
    JMP    cpu1_for                               ; goto cpu1_for
cpu1_done:
    END                                           ; 任务结束
cpu1_failed:
    BUG

