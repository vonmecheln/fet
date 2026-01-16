lessThan(QT_MAJOR_VERSION, 5) {
	error(Qt version $$QT_VERSION is not supported. The minimum supported Qt version is 5.15.0.)
}

equals(QT_MAJOR_VERSION, 5) {
	lessThan(QT_MINOR_VERSION, 15){
		error(Qt version $$QT_VERSION is not supported. The minimum supported Qt version is 5.15.0.)
	}
}

SOURCES += \
	engine/activity.cpp      engine/lockunlock.cpp       engine/studentsset.cpp     engine/timetable_defs.cpp \
	engine/activitytag.cpp   engine/room.cpp             engine/subject.cpp         engine/timetableexport.cpp \
	engine/building.cpp      engine/rules.cpp            engine/teacher.cpp \
	engine/generate.cpp      engine/solution.cpp         engine/timeconstraint.cpp \
	engine/generate_pre.cpp  engine/spaceconstraint.cpp  engine/timetable.cpp \
	\
	cl/clsettings.cpp  cl/fetcl.cpp                 cl/messageboxescl.cpp \
	cl/exportcl.cpp    cl/longtextmessageboxcl.cpp

HEADERS += \
	engine/activity.h      engine/lockunlock.h       engine/studentsset.h     engine/timetable_defs.h \
	engine/activitytag.h   engine/room.h             engine/subject.h         engine/timetableexport.h \
	engine/building.h      engine/rules.h            engine/teacher.h \
	engine/generate.h      engine/solution.h         engine/timeconstraint.h \
	engine/generate_pre.h  engine/spaceconstraint.h  engine/timetable.h \
	\
	cl/fetcl.h cl/exportcl.h    cl/longtextmessageboxcl.h

TEMPLATE = app

DEFINES += \
	FET_COMMAND_LINE \
	QT_NO_FOREACH \
	QT_NO_JAVA_STYLE_ITERATORS \
	QT_NO_LINKED_LIST \
	QT_STRICT_ITERATORS

CONFIG += release warn_on c++17 cmdline no_keywords

QT -= gui

DESTDIR = ..
TARGET = fet-cl

OBJECTS_DIR = ../tmp/cl
UI_DIR = ../tmp/cl
MOC_DIR = ../tmp/cl
RCC_DIR = ../tmp/cl

INCLUDEPATH += cl engine interface
DEPENDPATH += cl engine interface

unix {
	target.path = /usr/bin
	INSTALLS += target
}
