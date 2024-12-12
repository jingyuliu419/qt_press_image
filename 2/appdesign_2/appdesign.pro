QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    findpeaks.cpp \
    grid_layout_panel.cpp \
    main.cpp \
    mainwindow.cpp \
    picture_process.cpp \
    pinterwave.cpp

HEADERS += \
    findpeaks.h \
    grid_layout_panel.h \
    mainwindow.h \
    picture_process.h \
    pinterwave.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += /home/liu/opencv_build/opencv/build
INCLUDEPATH += /home/liu/opencv_build/opencv/build/opencv
INCLUDEPATH += /home/liu/opencv_build/opencv/build/opencv2
INCLUDEPATH += /home/liu/Doc/software/onnxruntime/include/onnxruntime/core/session


LIBS += /home/liu/opencv_build/opencv/build/lib/libopencv_*.so
INCLUDEPATH += /usr/include/rtfilter
LIBS += -lfftw3
LIBS += -lrtfilter
LIBS += /home/liu/Doc/software/onnxruntime/build/Linux/Release/libmsc.so \
        /home/liu/Doc/software/onnxruntime/build/Linux/Release/libonnxruntime.so \
        /home/liu/Doc/software/onnxruntime/build/Linux/Release/libonnxruntime_providers_shared.so \
        /home/liu/Doc/software/onnxruntime/build/Linux/Release/libonnxruntime_providers_cuda.so \
        /home/liu/Doc/software/onnxruntime/build/Linux/Release/libcustom_op_library.so \
        /home/liu/Doc/software/onnxruntime/build/Linux/Release/libonnxruntime_providers_tensorrt.so \
        /home/liu/Doc/software/onnxruntime/build/Linux/Release/libnvonnxparser.so.8.4.1

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target