#-------------------------------------------------
#
# Project created by QtCreator 2013-07-24T17:29:35
#
#-------------------------------------------------

QT       += core gui network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = XmppClient
TEMPLATE = app


SOURCES += main.cpp\
        client_mainwindow.cpp \
    client.cpp \
    logindlg.cpp \
    global.cpp \
    rostermodel.cpp \
    rostertreeitem.cpp \
    Preferences.cpp \
    roster_treeview.cpp

HEADERS  += client_mainwindow.h \
    client.h \
    logindlg.h \
    global.h \
    constances.h \
    rostermodel.h \
    rostertreeitem.h \
    Preferences.h \
    roster_treeview.h

FORMS    += client_mainwindow.ui \
    logindlg.ui


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../Qt/4.8.0/lib/ -lqxmpp0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../Qt/4.8.0/lib/ -lqxmpp_d0

INCLUDEPATH += $$PWD/../../../../../../Qt/4.8.0/include
DEPENDPATH += $$PWD/../../../../../../Qt/4.8.0/include

RESOURCES += \
    res.qrc
