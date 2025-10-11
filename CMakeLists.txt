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
find_package(PkgConfig REQUIRED)
pkg_check_modules(TESSERACT REQUIRED tesseract)
pkg_check_modules(LEPTONICA REQUIRED lept)

qt_standard_project_setup(REQUIRES 6.8)

set(SOURCES
    src/main.cpp
    src/Application.cpp
    src/Keyhook.cpp
    src/ClockReader.cpp
    src/SettingsManager.cpp
    src/TimerController.cpp
    src/TTSManager.cpp
    src/InputManager.cpp
)

qt_add_executable(deadclock ${SOURCES})

qt_add_qml_module(deadclock
    URI Application
    VERSION 1.0
    QML_FILES src/ui/MainWindow.qml src/ui/OverlayWindow.qml
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

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_target_properties(deadclock PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

target_include_directories(deadclock PRIVATE 
    src
    ${TESSERACT_INCLUDE_DIRS}
    ${LEPTONICA_INCLUDE_DIRS}
)

link_directories(
    ${TESSERACT_LIBRARY_DIRS}
    ${LEPTONICA_LIBRARY_DIRS}
)


target_link_libraries(deadclock PRIVATE
    Qt6::Core
    Qt6::Multimedia
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::TextToSpeech
    uiohook
    tesseract
    leptonica
)

file(COPY "${CMAKE_SOURCE_DIR}/resources/eng.traineddata"
     DESTINATION "${CMAKE_BINARY_DIR}"
)

## Install and deployment
install(TARGETS deadclock
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_qml_app_script(
    TARGET deadclock
    OUTPUT_SCRIPT deploy_script
)
install(SCRIPT ${deploy_script})

install(FILES "${CMAKE_SOURCE_DIR}/resources/eng.traineddata"
        DESTINATION "${CMAKE_INSTALL_BINDIR}"
)
