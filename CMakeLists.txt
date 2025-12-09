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
find_package(OpenCV REQUIRED)
pkg_check_modules(TESSERACT REQUIRED tesseract)
pkg_check_modules(LEPTONICA REQUIRED lept)

qt_standard_project_setup(REQUIRES 6.8)

set(SOURCES
    src/main.cpp
    src/Application.cpp
    src/SettingsManager.cpp
    src/TimerController.cpp
    src/TTSManager.cpp
    src/NotificationManager.cpp
    src/input/Keyhook.cpp
    src/input/InputManager.cpp
    src/gamestate/GameStateTracker.cpp
    src/gamestate/Regions.cpp
    src/gamestate/OCRManager.cpp
    src/gamestate/CVManager.cpp
)

qt_add_executable(deadclock ${SOURCES})

set_source_files_properties(src/ui/components/Constants.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)
qt_add_qml_module(deadclock
    URI Deadclock.Ui
    VERSION 1.0
    QML_FILES
        src/ui/components/Constants.qml
        src/ui/components/Main.qml
        src/ui/components/Settings.qml
        src/ui/components/TitleBar.qml
        src/ui/components/DCButton.qml
        src/ui/components/TimerBar.qml
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
        resources/images/minimap_highlight.png
        resources/images/gear_solid_full.svg
        resources/images/xmark_solid_full.svg
)

qt_add_resources(deadclock "opencv_images"
    PREFIX "/images/opencv"
    BASE "resources/images/opencv"
    FILES
        resources/images/opencv/dl_esc_icon.png
        resources/images/opencv/dl_rejuv_icon.png
)

qt_add_resources(deadclock "fonts"
    PREFIX "/fonts"
    BASE "resources/fonts"
    FILES
        resources/fonts/Lato-Regular.ttf
        resources/fonts/Lato-Bold.ttf
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_target_properties(deadclock PROPERTIES WIN32_EXECUTABLE TRUE)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(deadclock PRIVATE QT_QML_DEBUG)
endif()

target_include_directories(deadclock PRIVATE 
    src
    ${TESSERACT_INCLUDE_DIRS}
    ${LEPTONICA_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
)

target_link_directories(deadclock PRIVATE
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
    ${OpenCV_LIBS}
)

if(WIN32)
    target_link_libraries(deadclock PRIVATE Dwmapi)
    target_compile_definitions(deadclock PRIVATE PLATFORM_WINDOWS)
endif()

set(ocr_data_path "${CMAKE_SOURCE_DIR}/resources/ocr/retaildemo.traineddata")

file(COPY "${ocr_data_path}"
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

install(FILES "${ocr_data_path}"
        DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

set(MAIN_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(test/computer_vision)
