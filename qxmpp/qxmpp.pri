# Common definitions

QT += network xml
QXMPP_VERSION = 0.7.6
QXMPP_INCLUDEPATH = $$PWD/qxmpp/base $$PWD/qxmpp/client $$PWD/qxmpp/server

# Determine library name
CONFIG(debug, debug|release) {
    QXMPP_LIBRARY_NAME = qxmpp_d
} else {
    QXMPP_LIBRARY_NAME = qxmpp
}

# Determine library type (shared or staticlib)
isEmpty(QXMPP_LIBRARY_TYPE) {
    android {
        QXMPP_LIBRARY_TYPE = staticlib
    } else {
        QXMPP_LIBRARY_TYPE = shared
    }
}


# Libraries for apps which use QXmpp
QXMPP_LIBS = -l$${QXMPP_LIBRARY_NAME}
contains(QXMPP_LIBRARY_TYPE,staticlib) {
    # Symbian needs a .lib extension to recognise the library as static
    symbian: QXMPP_LIBS = -l$${QXMPP_LIBRARY_NAME}.lib

    # FIXME: we should be able to use the link_prl option to automatically pull
    # in the extra libraries which the qxmpp library needs, but this does not
    # seem to work on win32, so we specify the dependencies here:
    QXMPP_LIBS += $$QXMPP_INTERNAL_LIBS
    DEFINES += QXMPP_STATIC
} else {
    # Windows needs the major library version
    win32: QXMPP_LIBS = -l$${QXMPP_LIBRARY_NAME}0
    DEFINES += QXMPP_SHARED
}

# Installation prefix and library directory
isEmpty(PREFIX) {
    contains(MEEGO_EDITION,harmattan) {
        PREFIX = /usr
    } else:unix {
        PREFIX = /usr/local
    } else {
        PREFIX = $$[QT_INSTALL_PREFIX]
    }
}
isEmpty(LIBDIR) {
    LIBDIR = lib
}

# Internal API auto-tests
!isEmpty(QXMPP_AUTOTEST_INTERNAL) {
    DEFINES += QXMPP_AUTOTEST_INTERNAL
}
