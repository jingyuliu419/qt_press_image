cmake_minimum_required(VERSION 3.10)

project(AppDesign)

set(CMAKE_CXX_STANDARD 17)

# Enable debug information
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
    
# Find Qt libraries
find_package(Qt5 COMPONENTS Core Gui Widgets Multimedia Network Charts Sql REQUIRED)

# Define sources and headers
set(SOURCES
    src/audioplayer.cpp
    src/databaseworker.cpp
    src/findpeaks.cpp
    src/grid_layout_panel.cpp
    src/main.cpp
    src/mainwindow.cpp
    src/mysql_test.cpp
    src/packet_transmission.cpp
    src/picture_process.cpp
    src/pinterwave.cpp
    src/pinterwave1.cpp
    src/presssensor.cpp
    src/worker.cpp
)

set(HEADERS
    include/audioplayer.h
    include/databaseworker.h
    include/findpeaks.h
    include/grid_layout_panel.h
    include/mainwindow.h
    include/mysql_test.h
    include/packet_transmission.h
    include/picture_process.h
    include/pinterwave.h
    include/pinterwave1.h
    include/presssensor.h
    include/worker.h
)

set(FORMS
    forms/mainwindow.ui
)

# Define executable before setting directories or linking libraries
add_executable(AppDesign ${SOURCES} ${HEADERS} ${FORMS})

# Include directories (configure after target definition)
target_include_directories(AppDesign PRIVATE
    ${CMAKE_BINARY_DIR}
    /usr/local/include
    /usr/local/include/opencv
    /usr/local/include/opencv2
    /home/ljy/software/onnxruntime/include/onnxruntime/core/session
    /home/ljy/software/onnxruntime/include/
    /home/ljy/project/qt_press_image/pressprocess/appdesign_4/include
)

# Link directories (configure after target definition)
target_link_directories(AppDesign PRIVATE
    /home/ljy/software/opencv-3.4.14/build/lib
    /home/ljy/software/onnxruntime/build/Linux/Release
)

# Link libraries
target_link_libraries(AppDesign
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Multimedia
    Qt5::Network
    Qt5::Charts
    Qt5::Sql
    /home/ljy/software/opencv-3.4.14/build/lib/libopencv_core.so
    /home/ljy/software/opencv-3.4.14/build/lib/libopencv_imgproc.so
    /home/ljy/software/opencv-3.4.14/build/lib/libopencv_highgui.so
    /home/ljy/software/opencv-3.4.14/build/lib/libopencv_imgcodecs.so
    /home/ljy/software/onnxruntime/build/Linux/Release/libonnxruntime.so
    /home/ljy/software/onnxruntime/build/Linux/Release/libonnxruntime_providers_shared.so
    /home/ljy/software/onnxruntime/build/Linux/Release/libonnxruntime_providers_cuda.so
    /home/ljy/software/onnxruntime/build/Linux/Release/libcustom_op_library.so
    /home/ljy/software/onnxruntime/build/Linux/Release/libonnxruntime_providers_tensorrt.so
    /home/ljy/software/onnxruntime/build/Linux/Release/libnvonnxparser.so.8.4.1
    /home/ljy/software/onnxruntime/build/Linux/Release/libmsc.so
)

# Set RPATH for runtime library paths
set_target_properties(AppDesign PROPERTIES
    BUILD_RPATH "/home/ljy/software/opencv-3.4.14/build/lib;/home/ljy/software/onnxruntime/build/Linux/Release"
    INSTALL_RPATH "/home/ljy/software/opencv-3.4.14/build/lib;/home/ljy/software/onnxruntime/build/Linux/Release"
    CXX_STANDARD 17
)

# Process UI files
qt5_wrap_ui(UI_HEADERS ${FORMS})

# Process MOC files
qt5_wrap_cpp(MOC_SOURCES ${HEADERS})

# Add MOC sources to the executable
target_sources(AppDesign PRIVATE ${MOC_SOURCES})

# Define deployment rules
install(TARGETS AppDesign DESTINATION /opt/${PROJECT_NAME}/bin)
