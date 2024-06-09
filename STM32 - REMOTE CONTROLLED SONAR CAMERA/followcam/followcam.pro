QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

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
    ccamera.cpp \
    cdetection.cpp \
    cgui.cpp \
    cprediction.cpp \
    cthread.cpp \
    cthreads.cpp \
    followcam.cpp \
    main.cpp \
    mainwindow.cpp \
    settings_menu.cpp \
    tacquire_frames.cpp \
    tactuateservo.cpp \
    tdetect_face.cpp \
    tface_center_control.cpp \
    tgui.cpp \
    trecognize_face.cpp \
    user_menu.cpp

HEADERS += \
    ccamera.h \
    cdetection.h \
    cgui.h \
    cprediction.h \
    cthread.h \
    cthreads.h \
    followcam.h \
    mainwindow.h \
    settings_menu.h \
    tacquire_frames.h \
    tactuateservo.h \
    tdetect_face.h \
    tface_center_control.h \
    tgui.h \
    trecognize_face.h \
    user_menu.h

QMAKE_CXXFLAGS += -std=c++0x -pthread
LIBS += -pthread -lrt -lpthread -lbcm2835
LIBS += -L"/home/joao/Desktop" -lwiringPi

FORMS += \
    mainwindow.ui \
    settings_menu.ui \
    user_menu.ui


CONFIG += link_pkgconfig
PKGCONFIG += opencv

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    imgs.qrc
