include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)

FetchContent_MakeAvailable(googletest)

# �������������� ���������� ���������� bin � lib ����� �� �������� ����� :D
foreach(t IN ITEMS gtest gtest_main gmock gmock_main)
  if(TARGET ${t})
    set_target_properties(${t} PROPERTIES
      ARCHIVE_OUTPUT_DIRECTORY     "${CMAKE_BINARY_DIR}/UnitTests/lib"
      LIBRARY_OUTPUT_DIRECTORY     "${CMAKE_BINARY_DIR}/UnitTests/lib"
      RUNTIME_OUTPUT_DIRECTORY     "${CMAKE_BINARY_DIR}/UnitTests/bin"
      PDB_OUTPUT_DIRECTORY         "${CMAKE_BINARY_DIR}/UnitTests/bin"
      COMPILE_PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/UnitTests/lib"
    )
  endif()
endforeach()

add_executable(UnitTests
    Core/InstructionSetTests.cpp
    Core/CodecTests.cpp
    Core/InstructionTests.cpp
    Core/RegisterTests.cpp

    Emulator/EmulatorTests.cpp
    Emulator/ExecutorTests.cpp
    Emulator/CpuStateTests.cpp
)

target_include_directories(UnitTests
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(UnitTests
  PRIVATE
    gtest_main
    Core
    EmulatorInternal
)

set_target_properties(UnitTests PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/UnitTests/bin"
)

include(GoogleTest)
gtest_discover_tests(UnitTests
    PROPERTIES
        LABELS "UnitTests"
)
