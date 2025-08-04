lessThan(QT_MAJOR_VERSION, 5) {
	error(Qt version $$QT_VERSION is not supported. The minimum supported Qt version is 5.15.0.)
}

equals(QT_MAJOR_VERSION, 5) {
	lessThan(QT_MINOR_VERSION, 15){
		error(Qt version $$QT_VERSION is not supported. The minimum supported Qt version is 5.15.0.)
	}
}

SOURCES += \
	engine/timetableexport.cpp \
	engine/activity.cpp \
	engine/solution.cpp \
	engine/timetable.cpp \
	engine/rules.cpp \
	engine/generate_pre.cpp \
	engine/timeconstraint.cpp \
	engine/spaceconstraint.cpp \
	engine/studentsset.cpp \
	engine/teacher.cpp \
	engine/subject.cpp \
	engine/activitytag.cpp \
	engine/room.cpp \
	engine/building.cpp \
	engine/timetable_defs.cpp \
	engine/generate.cpp \
	engine/lockunlock.cpp \
	engine/export.cpp \
	\
	interface/messageboxes.cpp \
	interface/utilities.cpp \
	interface/fet.cpp \
	interface/fetmainform.cpp \
	interface/longtextmessagebox.cpp

HEADERS += \
	engine/timetableexport.h \
	engine/activity.h \
	engine/solution.h \
	engine/timetable.h \
	engine/rules.h \
	engine/generate_pre.h \
	engine/timeconstraint.h \
	engine/spaceconstraint.h \
	engine/timetable_defs.h \
	engine/studentsset.h \
	engine/teacher.h \
	engine/subject.h \
	engine/activitytag.h \
	engine/room.h \
	engine/building.h \
	engine/generate.h \
	engine/lockunlock.h \
	engine/matrix.h \
	engine/export.h \
	\
	interface/messageboxes.h \
	interface/utilities.h \
	interface/fet.h \
	interface/longtextmessagebox.h

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

OBJECTS_DIR = ../tmp/commandline
UI_DIR = ../tmp/commandline
MOC_DIR = ../tmp/commandline
RCC_DIR = ../tmp/commandline

INCLUDEPATH += engine interface
DEPENDPATH += engine interface

unix {
	target.path = /usr/bin
	INSTALLS += target
}
