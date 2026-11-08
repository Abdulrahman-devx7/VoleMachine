cmake_minimum_required(VERSION 3.16)
project(VoleMachineSimulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(VoleMachineSimulator
    main.cpp
    MainWindow.cpp
    MainWindow.h
    VoleMachine.h
    CPU.h
    ControlUnit.h
    Instruction.h
    ALU.h
    OpCodes.h
    Memory.h
    Registers.h
    Screen.h
    CellsBasic.h
)

target_link_libraries(VoleMachineSimulator PRIVATE Qt6::Widgets)

# Copy Qt's runtime DLLs and platform plugins beside the executable on Windows.
if(WIN32)
    get_filename_component(_QT6_INSTALL_PREFIX "${Qt6Core_DIR}/../../../" ABSOLUTE)
    find_program(QT_WINDEPLOYQT_EXECUTABLE
        NAMES windeployqt
        HINTS "${_QT6_INSTALL_PREFIX}/bin"
    )

    if(NOT QT_WINDEPLOYQT_EXECUTABLE)
        message(FATAL_ERROR "windeployqt was not found. Select a Qt 6 Windows kit that includes the Qt deployment tools.")
    endif()

    add_custom_command(TARGET VoleMachineSimulator POST_BUILD
        COMMAND "${QT_WINDEPLOYQT_EXECUTABLE}"
            --no-translations
            --dir "$<TARGET_FILE_DIR:VoleMachineSimulator>"
            "$<TARGET_FILE:VoleMachineSimulator>"
        COMMENT "Deploying Qt runtime files beside VoleMachineSimulator"
        VERBATIM
    )
endif()
