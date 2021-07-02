QT -= gui
QT += axcontainer

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#win32: LIBS += -L$$PWD/include/ -lfmtd

#INCLUDEPATH += $$PWD/include
#DEPENDPATH += $$PWD/include

#win32: LIBS += -L$$PWD/include/ -lboost_locale-vc140-mt-gd

#INCLUDEPATH += $$PWD/include
#DEPENDPATH += $$PWD/include

win32: LIBS += -L$$PWD/include/ -lfmt

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include

win32: LIBS += -L$$PWD/include/ -lboost_locale-vc140-mt

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include
