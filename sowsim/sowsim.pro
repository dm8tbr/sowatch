#-------------------------------------------------
#
# Project created by QtCreator 2011-09-16T23:52:50
#
#-------------------------------------------------

TEMPLATE = app

TARGET = sowsim

QT       += core

SOURCES += main.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libsowatch/release/ -llibsowatch
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libsowatch/debug/ -llibsowatch
else:symbian: LIBS += -llibsowatch
else:unix: LIBS += -L$$OUT_PWD/../libsowatch/ -llibsowatch

INCLUDEPATH += $$PWD/../libsowatch
DEPENDPATH += $$PWD/../libsowatch

unix {
	maemo5 {
		target.path = /opt/sowatch
	} else {
		target.path = /usr/bin
	}
	INSTALLS += target
}
