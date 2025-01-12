QT       += core gui widgets multimedia network charts sql

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
    audioplayer.cpp \
    databaseworker.cpp \
    findpeaks.cpp \
    grid_layout_panel.cpp \
    main.cpp \
    mainwindow.cpp \
    mysql_test.cpp \
    packet_transmission.cpp \
    picture_process.cpp \
    pinterwave.cpp \
    pinterwave1.cpp \
    presssensor.cpp \
    worker.cpp

HEADERS += \
    audioplayer.h \
    databaseworker.h \
    findpeaks.h \
    grid_layout_panel.h \
    mainwindow.h \
    mysql_test.h \
    packet_transmission.h \
    picture_process.h \
    pinterwave.h \
    pinterwave1.h \
    presssensor.h \
    worker.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += /usr/local/include
INCLUDEPATH += /usr/local/include/opencv
INCLUDEPATH += /usr/local/include/opencv2
INCLUDEPATH += /home/ljy/software/onnxruntime/include/onnxruntime/core/session
INCLUDEPATH += /home/ljy/software/onnxruntime/include/
#INCLUDEPATH +=/home/liu/Doc/Doc1/mycode/4/appdesign_4/include




LIBS += /home/ljy/software/opencv-3.4.14/build/lib/libopencv_*.so

#LIBS += -L/home/ljy/software/onnxruntime/build/Linux/Release/lib -lonnxruntime



LIBS += /home/ljy/software/onnxruntime/build/Linux/Release/libmsc.so \
        /home/ljy/software/onnxruntime/build/Linux/Release/libonnxruntime.so \
        /home/ljy/software/onnxruntime/build/Linux/Release/libonnxruntime_providers_shared.so \
        /home/ljy/software/onnxruntime/build/Linux/Release/libonnxruntime_providers_cuda.so \
        /home/ljy/software/onnxruntime/build/Linux/Release/libcustom_op_library.so \
        /home/ljy/software/onnxruntime/build/Linux/Release/libonnxruntime_providers_tensorrt.so \
        /home/ljy/software/onnxruntime/build/Linux/Release/libnvonnxparser.so.8.4.1\
        /home/ljy/Doc1/mycode/4/appdesign_4/libs/x64/libmsc.so


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
