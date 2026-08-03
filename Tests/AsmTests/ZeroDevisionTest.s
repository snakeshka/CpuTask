; Регистрируем адрес обработчика IHR в R5
LI R5 HANDLER
LUI R5 HANDLER
SCRW IHR R5        ; IHR = &HANDLER
; Инициализация счетчика прерываний в R10
LI R10 0           ; R10 = 0
; Подготовка операндов для деления на 0
LI R1 10           ; Делимое R1 = 10
LI R2 0            ; Делитель R2 = 0
; Выполнение деления на 0 (ожидаем вызова прерывания)
DIV_LABEL:
UDIV R3 R1 R2      ; 10 / 0 Прерывание Illegal Operand (1)
; Если мы попали сюда без прерывания то тест провален
_SIM_HALTI 1       ; TEST FAILED (код 1)


; Обработчик прерываний
HANDLER:
    ; Проверяем причину прерывания в ICR
    SCRR R4 ICR    ; R4 = ICR
    LI R6 1        ; Ожидаем код прерывания 1 (Illegal Operand)
    EQ R4 R4 R6    ; R4 == 1?
    BRNZ R4 CHECK_COUNT
    _SIM_HALTI 2   ; Если причиной было НЕ деление на 0 то ошибка (код 2)
CHECK_COUNT:
    ; Увеличиваем счетчик вызова прерываний
    LI R6 1
    ADD R10 R10 R6 ; R10 += 1
    ; Если это первое прерывание то возвращаемся с помощью RIH
    EQ R4 R10 R6   ; R10 == 1?
    BRZ R4 CHECK_SECOND
    RIH            ; Возврат из прерывания. IP = IIP (повторный запуск UDIV)
CHECK_SECOND:
    ; Если это второе прерывание то тест успешно пройден
    LI R6 2
    EQ R4 R10 R6   ; R10 == 2?
    BRNZ R4 TEST_SUCCESS
    _SIM_HALTI 3   ; Количество прерываний не равно 2 то ошибка (код 3)
TEST_SUCCESS:
    _SIM_HALTI 0   ; TEST PASSED (код 0)