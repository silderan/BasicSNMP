#-------------------------------------------------
#
# Project created by QtCreator 2019-02-19T22:34:00
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BasicSNMP
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
	main.cpp \
	mainwindow.cpp \
	QIniFile.cpp \
    lib/asn1types.cpp \
    lib/snmptable.cpp \
    lib/pduvarbind.cpp \
    lib/asn1variable.cpp \
    lib/asn1encoder.cpp \
    lib/snmpencoder.cpp \
    snmptests.cpp \
	qsnmpconn.cpp \
    qconstantsstrings.cpp

HEADERS += \
	mainwindow.h \
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
    QIniFile.h \
	utils.h \
    snmptests.h \
	qsnmpconn.h \
    qconstantsstrings.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
