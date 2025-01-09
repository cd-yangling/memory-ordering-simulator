; test add instruction
  MOV R0 1                 ;R0 = 1
  MOV R1 R0                ;R1 = R0
  ADD R2 R1 2              ;R2 = R1 + 2
  ADD R3 R1 R0             ;R3 = R1 + R0
  JNE failed R2 3          ;if (R2 != 3) goto failed
  JNE failed R3 2          ;if (R3 != 2) goto failed

; test sub instruction
  MOV R0 10                ;R0 = 10
  MOV R1 R0                ;R1 = R0
  SUB R2 R1 3              ;R2 = R1 - 3
  SUB R3 R1 R0             ;R3 = R1 - R0
  JNE failed R2 7          ;if (R2 != 7) goto failed
  JNE failed R3 0          ;if (R3 != 0) goto failed

; test mul instruction
  MOV R0 8                 ;R0 = 8
  MOV R1 R0                ;R1 = R0
  MUL R2 R1 2              ;R2 = R1 * 2
  MUL R3 R1 R0             ;R3 = R1 * R0
  JNE failed R2 16         ;if (R2 != 16) goto failed
  JNE failed R3 64         ;if (R3 != 64) goto failed

; test div instruction
  MOV R0 6                 ;R0 = 6
  MOV R1 R0                ;R1 = R0
  DIV R2 R1 3              ;R2 = R1 / 3
  DIV R3 R1 R0             ;R3 = R1 / R0
  JNE failed R2 2          ;if (R2 != 2) goto failed
  JNE failed R3 1          ;if (R3 != 1) goto failed

; test mod instruction
  MOV R0 4                 ;R0 = 4
  MOV R1 R0                ;R1 = R0
  MOD R2 R1 3              ;R2 = R1 % 2
  MOD R3 R1 R0             ;R3 = R1 % R0
  JNE failed R2 1          ;if (R2 != 1) goto failed
  JNE failed R3 0          ;if (R3 != 0) goto failed

; test and instruction
  MOV R0 7                 ;R0 = 7
  MOV R1 R0                ;R1 = R0
  AND R2 R1 6              ;R2 = R1 & 6
  AND R3 R1 R0             ;R3 = R1 & R0
  JNE failed R2 6          ;if (R2 != 6) goto failed
  JNE failed R3 7          ;if (R3 != 7) goto failed

; test or instruction
  MOV R0 3                 ;R0 = 3
  MOV R1 R0                ;R1 = R0
  OR  R2 R1 4              ;R2 = R1 | 4
  OR  R3 R1 R0             ;R3 = R1 | R0
  JNE failed R2 7          ;if (R2 != 7) goto failed
  JNE failed R3 3          ;if (R3 != 3) goto failed

; test xor instruction
  MOV R0 1                 ;R0 = 1
  MOV R1 R0                ;R1 = R0
  XOR R2 R1 2              ;R2 = R1 ^ 2
  XOR R3 R1 R0             ;R3 = R1 ^ R0
  JNE failed R2 3          ;if (R2 != 3) goto failed
  JNE failed R3 0          ;if (R3 != 0) goto failed

; test not instruction
  MOV R0 21                ;R0 = 21
  MOV R1 R0                ;R1 = R0
  NOT R2 8                 ;R2 = ~8
  NOT R3 R1                ;R3 = ~R1
  JNE failed R2 0xFFFFFFF7 ;if (R2 != 0xFFFFFFF7) goto failed
  JNE failed R3 0xFFFFFFEA ;if (R3 != 10) goto failed

  END
failed:
  BUG
