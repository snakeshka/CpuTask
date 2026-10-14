## Сборка и запуск

### Требования

Для сборки проекта нужны:

- C++20-совместимый компилятор;
- CMake 3.20 или новее;
- система сборки, поддерживаемая CMake;
- доступ к интернету при первой конфигурации для загрузки GoogleTest.

### Сборка проекта

Все команды выполняются из корневой директории проекта:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

После сборки в будут созданы основные исполняемые файлы:

```text
Assembler
Emulator
UnitTests
```

### Запуск ассемблера

```bash
./build/Assembler/bin/Assembler program.s program.o
```

Ассемблер читает текстовый файл `program.s` и создаёт бинарный файл `program.o`.

### Запуск эмулятора

```bash
./build/Emulator/bin/Emulator program.o
```

Эмулятор читает бинарный файл `program.o`, выполняет инструкции и выводит итоговое состояние регистров.

### Запуск тестов

Проект использует два уровня тестирования:

- `UnitTests` — unit-тесты на GTest;
- `E2EFullFlow` — полный flow-тест через запуск `Assembler` и `Emulator`.

Запуск всех тестов:

```bash
ctest --test-dir build --output-on-failure
```

Запуск только unit-тестов:

```bash
ctest --test-dir build -L UnitTests --output-on-failure
```

Запуск только E2E-теста:

```bash
ctest --test-dir build -R E2EFullFlow --output-on-failure
```

Просмотр списка зарегистрированных тестов:

```bash
ctest --test-dir build -N
```
