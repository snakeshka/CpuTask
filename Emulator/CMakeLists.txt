#создаем internal object для тестов
add_library(EmulatorInternal OBJECT
    src/Executor.cpp
    src/Emulator.cpp
    src/Tracer.cpp
    src/Ram.cpp
    src/BusRouter.cpp
    src/Devices/OutputDevice.cpp
    src/Devices/PicDevice.cpp
    src/Devices/TimerDevice.cpp
)

target_include_directories(EmulatorInternal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(EmulatorInternal PUBLIC Core)


file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

add_executable(Emulator
    ${SRC_FILES}
    ${HEADER_FILES}
)

target_include_directories(Emulator PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(Emulator PRIVATE Core)

set_target_properties(Emulator PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Emulator/bin"
)