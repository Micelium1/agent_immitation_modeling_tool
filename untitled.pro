QT       += core gui printsupport
CONFIG += qscintilla2
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    agent.cpp \
    agentbaseselection.cpp \
    agentcommand.cpp \
    agentfactory.cpp \
    agentlogicinput.cpp \
    main.cpp \
    mainwindow.cpp \
    object.cpp \
    objecteditor.cpp \
    subject.cpp

HEADERS += \
    agent.h \
    agentbaseselection.h \
    agentcommand.h \
    agentfactory.h \
    agentlogicinput.h \
    mainwindow.h \
    object.h \
    objecteditor.h \
    subject.h

FORMS += \
    agentbaseselection.ui \
    agentlogicinput.ui \
    mainwindow.ui \
    objecteditor.ui

#путь к SCINTILLA
QSCINTILLA_PATH = $$[QT_INSTALL_PREFIX]
INCLUDEPATH += $$QSCINTILLA_PATH/include
LIBS += -L$$QSCINTILLA_PATH/lib -lqscintilla2_qt6

#путь к Python
PYTHON_VERSION = 313  # Укажите вашу версию
PYTHON_DIR = "C:/Program Files/Python$$PYTHON_VERSION"
INCLUDEPATH += "$$PYTHON_DIR/include"
LIBS += -L"$$PYTHON_DIR/libs" -lpython$$PYTHON_VERSION

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
