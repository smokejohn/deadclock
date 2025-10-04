cmake_minimum_required(VERSION 3.16)
project(deadclock VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(thirdparty/libuiohook external_libs)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Multimedia
    Gui
    Qml
    Quick
    QuickControls2
    TextToSpeech
)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup(REQUIRES 6.9)

set(SOURCES
    src/main.cpp
    src/WindowController.cpp
    src/Keyhook.cpp
)

qt_add_executable(deadclock ${SOURCES}
    src/PausableTimer.h src/PausableTimer.cpp
    src/TimerController.h src/TimerController.cpp
    )

qt_add_resources(deadclock "ui"
    PREFIX "/ui"
    BASE "src/ui"
    FILES
        src/ui/MainWindow.qml
        src/ui/OverlayWindow.qml
)

qt_add_resources(deadclock "images"
    PREFIX "/images"
    BASE "resources/images"
    FILES
        resources/images/deadclock_icon.ico
        resources/images/deadclock_icon.png
)

set_target_properties(deadclock PROPERTIES WIN32_EXECUTABLE TRUE)

target_include_directories(deadclock PRIVATE src)

target_link_libraries(deadclock PRIVATE
    Qt6::Core
    Qt6::Multimedia
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::TextToSpeech
    uiohook
)
