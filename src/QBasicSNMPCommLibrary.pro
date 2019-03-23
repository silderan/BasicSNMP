#-------------------------------------------------
#
# Project created by QtCreator 2019-03-23T08:25:52
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = QBasicSNMPCommLibrary
TEMPLATE = lib

DEFINES += QBASICSNMPCOMMLIBRARY_LIBRARY
CONFIG += staticlib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
		lib/snmptable.cpp \
		lib/asn1encoder.cpp \
		lib/snmpencoder.cpp \
		qsnmpconn.cpp \
		qbasicsnmpcommlibrary.cpp

HEADERS += \
		lib/snmplib.h \
		lib/asn1types.h \
		lib/snmptable.h \
		lib/stdcharvector.h \
		lib/stdlist.h \
		lib/oid.h \
		lib/asn1variable.h \
		lib/stdvector.h \
		lib/pduvarbind.h \
		lib/asn1encoder.h \
		lib/snmpencoder.h \
		lib/types.h \
		lib/stdstring.h \
		lib/basic_types.h \
		lib/stddeque.h \
		utils.h \
		qsnmpconn.h \
		qbasicsnmpcommlibrary.h \
		qbasicsnmpcommlibrary_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
