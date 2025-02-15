QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bloodrecognize.cpp \
    cvutils.cpp \
    dxgicapture.cpp \
    globalconfig.cpp \
    globalstatic.cpp \
    loginet.cpp \
    logitech_driver.cpp \
    macroconfigview.cpp \
    main.cpp \
    mainwindow.cpp \
    recognizeobject.cpp \
    recognizer.cpp \
    recognizeview.cpp \
    testview.cpp \
    utils.cpp \
    weaponconfig.cpp \
    weaponconfigview.cpp

HEADERS += \
    bloodrecognize.h \
    cvutils.h \
    dxgicapture.h \
    globalconfig.h \
    globalstatic.h \
    loginet.h \
    logitech_driver.h \
    macroconfigview.h \
    mainwindow.h \
    recognizeobject.h \
    recognizer.h \
    recognizeview.h \
    testview.h \
    utils.h \
    weaponconfig.h \
    weaponconfigview.h

FORMS += \
    macroconfigview.ui \
    mainwindow.ui \
    recognizeview.ui \
    testview.ui \
    weaponconfigview.ui

CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += F:\MySource\opencv\build-msvc-static\include

CONFIG(debug, debug|release) {
LIBS += F:\MySource\opencv\build-msvc-static\staticlib\*d.lib
} else {
LIBS += F:\MySource\opencv\build-msvc-static\staticlib\*.lib
}

LIBS += gdi32.lib dxgi.lib d3d11.lib dxguid.lib

RC_ICONS = pubg.ico

DISTFILES +=

RESOURCES += \
    pubg.qrc

win32-msvc*: {
    QMAKE_CFLAGS *= /utf-8
    QMAKE_CXXFLAGS *= /utf-8
}
