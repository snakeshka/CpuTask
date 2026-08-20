RJMP MAIN

INTERRUPT_HANDLER:
    ; Читаем POP из PIC (0xE020)
    LI R5 0xE020
    LDB R6 R5 0

    STB R6 R5 0

    ; счетчик прерываний R10++
    LI R5 1
    ADD R10 R10 R5
    RIH

MAIN:
    LI R10 0

    ; Записываем строку "Hello!" в RAM на 0x1000
    LI R1 0x1000
    LI R2 0x6548
    LUI R2 0x6C6C
    STW R2 R1 0       ; "Hell"

    LI R2 0x216F
    LUI R2 0x0000
    STW R2 R1 4       ; "o!"

    LI R3 INTERRUPT_HANDLER
    SCRW IHR R3

    ; первый вывод Hello
    LI R4 0xE004
    LI R5 6
    STH R5 R4 0       ; SIZE = 6

    LI R4 0xE006
    LI R5 5
    STB R5 R4 0       ; CTRL = 5 (DMA=1, IE=1)

    LI R4 0xE000
    STW R1 R4 0       ; Старт DMA (0x1000)

    WAIT              ; ждем прерывания

    ; Проверяем что 1 прерывание пришло
    LI R5 1
    NE R6 R10 R5
    BRNZ R6 FAIL

    ; второй вывод Hello
    LI R4 0xE004
    LI R5 6
    STH R5 R4 0       ; SIZE = 6

    LI R4 0xE006
    LI R5 5
    STB R5 R4 0       ; CTRL = 5 (DMA=1, IE=1)

    LI R4 0xE000
    STW R1 R4 0       ; DMA (0x1000)

    WAIT              ; ждем прерывания

    ; Проверяем что 2 прерывание пришло
    LI R5 2
    NE R6 R10 R5
    BRNZ R6 FAIL

    _SIM_HALTI 0      ; Успех

FAIL:
    _SIM_HALTI 1      ; Ошибка