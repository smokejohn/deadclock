project(opencv_test VERSION 0.1 LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
)

find_package(OpenCV REQUIRED)

qt_standard_project_setup(REQUIRES 6.8)

set(SOURCES
    src/main.cpp
    src/LabeledSlider.cpp
    ${MAIN_SRC_DIR}/CVManager.cpp
)

qt_add_executable(opencv_test ${SOURCES})


target_include_directories(opencv_test PRIVATE
    ${MAIN_SRC_DIR}
    src
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(opencv_test PRIVATE
    Qt6::Core
    Qt6::Widgets
    ${OpenCV_LIBS}
)
