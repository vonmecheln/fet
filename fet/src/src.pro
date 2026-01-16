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
	interface/activitiesform.cpp \
	interface/activitiesroomsstatisticsform.cpp \
	interface/activitiestagsform.cpp \
	interface/activityplanningconfirmationform.cpp \
	interface/activityplanningform.cpp \
	interface/activitytagsform.cpp \
	interface/addactivityform.cpp \
	interface/addbuildingform.cpp \
	interface/addexistingstudentsgroupsform.cpp \
	interface/addexistingstudentssubgroupsform.cpp \
	interface/addgroupactivitiesininitialorderitemform.cpp \
	interface/addormodifyspaceconstraint.cpp \
	interface/addormodifytimeconstraint.cpp \
	interface/addremovemultipleconstraintsactivitiesoccupymaxdifferentroomsform.cpp \
	interface/addremovemultipleconstraintsactivitiessamestartinghourform.cpp \
	interface/addroomform.cpp \
	interface/addstudentsgroupform.cpp \
	interface/addstudentssubgroupform.cpp \
	interface/addstudentsyearform.cpp \
	interface/addteacherform.cpp \
	interface/advancedfilterform.cpp \
	interface/advancedlockunlockform.cpp \
	interface/allspaceconstraintsform.cpp \
	interface/alltimeconstraintsform.cpp \
	interface/buildingsform.cpp \
	interface/changemindaysselectivelyform.cpp \
	interface/changeminhalfdaysselectivelyform.cpp \
	interface/commentsform.cpp \
	interface/daysform.cpp \
	interface/exportinterface.cpp \
	interface/fet.cpp \
	interface/fetmainform.cpp \
	interface/getmodefornewfileform.cpp \
	interface/groupactivitiesininitialorderitemsform.cpp \
	interface/groupsform.cpp \
	interface/helpaboutform.cpp \
	interface/helpaboutlibrariesform.cpp \
	interface/helpalgeriaform.cpp \
	interface/helpblockplanningform.cpp \
	interface/helpfaqform.cpp \
	interface/helpinstructionsform.cpp \
	interface/helpmoroccoform.cpp \
	interface/helponstudentsminhoursdaily.cpp \
	interface/helptermsform.cpp \
	interface/helptipsform.cpp \
	interface/hoursform.cpp \
	interface/importinterface.cpp \
	interface/institutionnameform.cpp \
	interface/listofrelatedspaceconstraintsform.cpp \
	interface/listofrelatedtimeconstraintsform.cpp \
	interface/listspaceconstraints.cpp \
	interface/listtimeconstraints.cpp \
	interface/longtextmessagebox.cpp \
	interface/messageboxesinterface.cpp \
	interface/modifyactivityform.cpp \
	interface/modifybuildingform.cpp \
	interface/modifydayform.cpp \
	interface/modifygroupactivitiesininitialorderitemform.cpp \
	interface/modifyhourform.cpp \
	interface/modifyrealdayform.cpp \
	interface/modifyrealhourform.cpp \
	interface/modifyroomform.cpp \
	interface/modifystudentsgroupform.cpp \
	interface/modifystudentssubgroupform.cpp \
	interface/modifystudentsyearform.cpp \
	interface/modifysubactivityform.cpp \
	interface/modifyteacherform.cpp \
	interface/modifytimeconstraints.cpp \
	interface/notificationcommandform.cpp \
	interface/randomseedform.cpp \
	interface/removeredundantconfirmationform.cpp \
	interface/removeredundantform.cpp \
	interface/restoredatastateform.cpp \
	interface/roommakeeditvirtualform.cpp \
	interface/roomsform.cpp \
	interface/savetimetableconfirmationform.cpp \
	interface/settingsautosaveform.cpp \
	interface/settingsdatatoprintintimetablesform.cpp \
	interface/settingsrestoredatafromdiskform.cpp \
	interface/settingsrestoredatafrommemoryform.cpp \
	interface/settingsstyleandcolorschemeform.cpp \
	interface/settingstimetablehtmllevelform.cpp \
	interface/sparseitemmodel.cpp \
	interface/sparsetableview.cpp \
	interface/splityearform.cpp \
	interface/spreadconfirmationform.cpp \
	interface/spreadmindaysconstraintsfivedaysform.cpp \
	interface/statisticsexport.cpp \
	interface/statisticsprintform.cpp \
	interface/studentscomboboxesstyleform.cpp \
	interface/studentsnotavailabletimestimehorizontalform.cpp \
	interface/studentsstatisticsform.cpp \
	interface/subactivitiesform.cpp \
	interface/subgroupsform.cpp \
	interface/subjectsform.cpp \
	interface/subjectsstatisticsform.cpp \
	interface/teachersform.cpp \
	interface/teachersnotavailabletimestimehorizontalform.cpp \
	interface/teachersstatisticsform.cpp \
	interface/teachersubjectsqualificationsform.cpp \
	interface/termsform.cpp \
	interface/timetablegenerateform.cpp \
	interface/timetablegeneratemultipleform.cpp \
	interface/timetableprintform.cpp \
	interface/timetableshowconflictsform.cpp \
	interface/timetablestowriteondiskform.cpp \
	interface/timetableviewroomsdayshorizontalform.cpp \
	interface/timetableviewroomsdaysverticalform.cpp \
	interface/timetableviewroomstimehorizontalform.cpp \
	interface/timetableviewstudentsdayshorizontalform.cpp \
	interface/timetableviewstudentsdaysverticalform.cpp \
	interface/timetableviewstudentstimehorizontalform.cpp \
	interface/timetableviewteachersdayshorizontalform.cpp \
	interface/timetableviewteachersdaysverticalform.cpp \
	interface/timetableviewteacherstimehorizontalform.cpp \
	interface/utilities.cpp \
	interface/yearsform.cpp

HEADERS += \
	engine/activity.h      engine/lockunlock.h    engine/solution.h         engine/timeconstraint.h \
	engine/activitytag.h   engine/matrix.h        engine/spaceconstraint.h  engine/timetable_defs.h \
	engine/building.h      engine/messageboxes.h  engine/studentsset.h      engine/timetableexport.h \
	engine/generate.h      engine/room.h          engine/subject.h          engine/timetable.h \
	engine/generate_pre.h  engine/rules.h         engine/teacher.h \
	\
	interface/activitiesform.h \
	interface/activitiesroomsstatisticsform.h \
	interface/activitiestagsform.h \
	interface/activityplanningconfirmationform.h \
	interface/activityplanningform.h \
	interface/activitytagsform.h \
	interface/addactivityform.h \
	interface/addbuildingform.h \
	interface/addexistingstudentsgroupsform.h \
	interface/addexistingstudentssubgroupsform.h \
	interface/addgroupactivitiesininitialorderitemform.h \
	interface/addormodifyspaceconstraint.h \
	interface/addormodifytimeconstraint.h \
	interface/addremovemultipleconstraintsactivitiesoccupymaxdifferentroomsform.h \
	interface/addremovemultipleconstraintsactivitiessamestartinghourform.h \
	interface/addroomform.h \
	interface/addstudentsgroupform.h \
	interface/addstudentssubgroupform.h \
	interface/addstudentsyearform.h \
	interface/addteacherform.h \
	interface/advancedfilterform.h \
	interface/advancedlockunlockform.h \
	interface/allspaceconstraintsform.h \
	interface/alltimeconstraintsform.h \
	interface/buildingsform.h \
	interface/changemindaysselectivelyform.h \
	interface/changeminhalfdaysselectivelyform.h \
	interface/commentsform.h \
	interface/daysform.h \
	interface/exportinterface.h \
	interface/fet.h \
	interface/fetmainform.h \
	interface/getmodefornewfileform.h \
	interface/groupactivitiesininitialorderitemsform.h \
	interface/groupsform.h \
	interface/helpaboutform.h \
	interface/helpaboutlibrariesform.h \
	interface/helpalgeriaform.h \
	interface/helpblockplanningform.h \
	interface/helpfaqform.h \
	interface/helpinstructionsform.h \
	interface/helpmoroccoform.h \
	interface/helponstudentsminhoursdaily.h \
	interface/helptermsform.h \
	interface/helptipsform.h \
	interface/hoursform.h \
	interface/importinterface.h \
	interface/institutionnameform.h \
	interface/listofrelatedspaceconstraintsform.h \
	interface/listofrelatedtimeconstraintsform.h \
	interface/listspaceconstraints.h \
	interface/listtimeconstraints.h \
	interface/longtextmessagebox_auxiliary.h \
	interface/longtextmessagebox.h \
	interface/modifyactivityform.h \
	interface/modifybuildingform.h \
	interface/modifydayform.h \
	interface/modifygroupactivitiesininitialorderitemform.h \
	interface/modifyhourform.h \
	interface/modifyrealdayform.h \
	interface/modifyrealhourform.h \
	interface/modifyroomform.h \
	interface/modifystudentsgroupform.h \
	interface/modifystudentssubgroupform.h \
	interface/modifystudentsyearform.h \
	interface/modifysubactivityform.h \
	interface/modifyteacherform.h \
	interface/modifytimeconstraints.h \
	interface/notificationcommandform.h \
	interface/randomseedform.h \
	interface/removeredundantconfirmationform.h \
	interface/removeredundantform.h \
	interface/restoredatastateform.h \
	interface/roommakeeditvirtualform.h \
	interface/roomsform.h \
	interface/savetimetableconfirmationform.h \
	interface/settingsautosaveform.h \
	interface/settingsdatatoprintintimetablesform.h \
	interface/settingsrestoredatafromdiskform.h \
	interface/settingsrestoredatafrommemoryform.h \
	interface/settingsstyleandcolorschemeform.h \
	interface/settingstimetablehtmllevelform.h \
	interface/sparseitemmodel.h \
	interface/sparsetableview.h \
	interface/splityearform.h \
	interface/spreadconfirmationform.h \
	interface/spreadmindaysconstraintsfivedaysform.h \
	interface/statisticsexport.h \
	interface/statisticsprintform.h \
	interface/studentscomboboxesstyleform.h \
	interface/studentsnotavailabletimestimehorizontalform.h \
	interface/studentsstatisticsform.h \
	interface/subactivitiesform.h \
	interface/subgroupsform.h \
	interface/subjectsform.h \
	interface/subjectsstatisticsform.h \
	interface/teachersform.h \
	interface/teachersnotavailabletimestimehorizontalform.h \
	interface/teachersstatisticsform.h \
	interface/teachersubjectsqualificationsform.h \
	interface/termsform.h \
	interface/timetablegenerateform.h \
	interface/timetablegeneratemultipleform.h \
	interface/timetableprintform.h \
	interface/timetableshowconflictsform.h \
	interface/timetablestowriteondiskform.h \
	interface/timetableviewroomsdayshorizontalform.h \
	interface/timetableviewroomsdaysverticalform.h \
	interface/timetableviewroomstimehorizontalform.h \
	interface/timetableviewstudentsdayshorizontalform.h \
	interface/timetableviewstudentsdaysverticalform.h \
	interface/timetableviewstudentstimehorizontalform.h \
	interface/timetableviewteachersdayshorizontalform.h \
	interface/timetableviewteachersdaysverticalform.h \
	interface/timetableviewteacherstimehorizontalform.h \
	interface/utilities.h \
	interface/yearsform.h

FORMS += \
	interface/activitiesform_template.ui \
	interface/activitiesroomsstatisticsform_template.ui \
	interface/activitiestagsform_template.ui \
	interface/activityplanningconfirmationform_template.ui \
	interface/activitytagsform_template.ui \
	interface/addactivityform_template.ui \
	interface/addbuildingform_template.ui \
	interface/addexistingstudentsgroupsform_template.ui \
	interface/addexistingstudentssubgroupsform_template.ui \
	interface/addgroupactivitiesininitialorderitemform_template.ui \
	interface/addremovemultipleconstraintsactivitiesoccupymaxdifferentroomsform_template.ui \
	interface/addremovemultipleconstraintsactivitiessamestartinghourform_template.ui \
	interface/addroomform_template.ui \
	interface/addstudentsgroupform_template.ui \
	interface/addstudentssubgroupform_template.ui \
	interface/addstudentsyearform_template.ui \
	interface/addteacherform_template.ui \
	interface/allspaceconstraintsform_template.ui \
	interface/alltimeconstraintsform_template.ui \
	interface/buildingsform_template.ui \
	interface/changemindaysselectivelyform_template.ui \
	interface/changeminhalfdaysselectivelyform_template.ui \
	interface/commentsform_template.ui \
	interface/daysform_template.ui \
	interface/fetmainform_template.ui \
	interface/getmodefornewfileform_template.ui \
	interface/groupactivitiesininitialorderitemsform_template.ui \
	interface/groupsform_template.ui \
	interface/helpaboutform_template.ui \
	interface/helpaboutlibrariesform_template.ui \
	interface/helpalgeriaform_template.ui \
	interface/helpblockplanningform_template.ui \
	interface/helpfaqform_template.ui \
	interface/helpinstructionsform_template.ui \
	interface/helpmoroccoform_template.ui \
	interface/helptermsform_template.ui \
	interface/helptipsform_template.ui \
	interface/hoursform_template.ui \
	interface/institutionnameform_template.ui \
	interface/listofrelatedspaceconstraintsform_template.ui \
	interface/listofrelatedtimeconstraintsform_template.ui \
	interface/modifyactivityform_template.ui \
	interface/modifybuildingform_template.ui \
	interface/modifydayform_template.ui \
	interface/modifygroupactivitiesininitialorderitemform_template.ui \
	interface/modifyhourform_template.ui \
	interface/modifyrealdayform_template.ui \
	interface/modifyrealhourform_template.ui \
	interface/modifyroomform_template.ui \
	interface/modifystudentsgroupform_template.ui \
	interface/modifystudentssubgroupform_template.ui \
	interface/modifystudentsyearform_template.ui \
	interface/modifysubactivityform_template.ui \
	interface/modifyteacherform_template.ui \
	interface/notificationcommandform_template.ui \
	interface/randomseedform_template.ui \
	interface/removeredundantconfirmationform_template.ui \
	interface/removeredundantform_template.ui \
	interface/restoredatastateform_template.ui \
	interface/roommakeeditvirtualform_template.ui \
	interface/roomsform_template.ui \
	interface/savetimetableconfirmationform_template.ui \
	interface/settingsautosaveform_template.ui \
	interface/settingsdatatoprintintimetablesform_template.ui \
	interface/settingsrestoredatafromdiskform_template.ui \
	interface/settingsrestoredatafrommemoryform_template.ui \
	interface/settingsstyleandcolorschemeform_template.ui \
	interface/settingstimetablehtmllevelform_template.ui \
	interface/splityearform_template.ui \
	interface/spreadconfirmationform_template.ui \
	interface/spreadmindaysconstraintsfivedaysform_template.ui \
	interface/studentscomboboxesstyleform_template.ui \
	interface/studentsnotavailabletimestimehorizontalform_template.ui \
	interface/studentsstatisticsform_template.ui \
	interface/subactivitiesform_template.ui \
	interface/subgroupsform_template.ui \
	interface/subjectsform_template.ui \
	interface/subjectsstatisticsform_template.ui \
	interface/teachersform_template.ui \
	interface/teachersnotavailabletimestimehorizontalform_template.ui \
	interface/teachersstatisticsform_template.ui \
	interface/teachersubjectsqualificationsform_template.ui \
	interface/termsform_template.ui \
	interface/timetablegenerateform_template.ui \
	interface/timetablegeneratemultipleform_template.ui \
	interface/timetableshowconflictsform_template.ui \
	interface/timetablestowriteondiskform_template.ui \
	interface/timetableviewroomsdayshorizontalform_template.ui \
	interface/timetableviewroomsdaysverticalform_template.ui \
	interface/timetableviewroomstimehorizontalform_template.ui \
	interface/timetableviewstudentsdayshorizontalform_template.ui \
	interface/timetableviewstudentsdaysverticalform_template.ui \
	interface/timetableviewstudentstimehorizontalform_template.ui \
	interface/timetableviewteachersdayshorizontalform_template.ui \
	interface/timetableviewteachersdaysverticalform_template.ui \
	interface/timetableviewteacherstimehorizontalform_template.ui \
	interface/yearsform_template.ui

TRANSLATIONS += \
	../translations/fet_ar.ts \
	../translations/fet_bg.ts \
	../translations/fet_bs.ts \
	../translations/fet_ca.ts \
	../translations/fet_cs.ts \
	../translations/fet_da.ts \
	../translations/fet_de.ts \
	../translations/fet_el.ts \
	../translations/fet_en_GB.ts \
	../translations/fet_es.ts \
	../translations/fet_eu.ts \
	../translations/fet_fa.ts \
	../translations/fet_fr.ts \
	../translations/fet_gl.ts \
	../translations/fet_he.ts \
	../translations/fet_hu.ts \
	../translations/fet_id.ts \
	../translations/fet_it.ts \
	../translations/fet_ja.ts \
	../translations/fet_lt.ts \
	../translations/fet_mk.ts \
	../translations/fet_ms.ts \
	../translations/fet_nl.ts \
	../translations/fet_pl.ts \
	../translations/fet_pt_BR.ts \
	../translations/fet_ro.ts \
	../translations/fet_ru.ts \
	../translations/fet_si.ts \
	../translations/fet_sk.ts \
	../translations/fet_sq.ts \
	../translations/fet_sr.ts \
	../translations/fet_tr.ts \
	../translations/fet_uk.ts \
	../translations/fet_untranslated.ts \
	../translations/fet_uz.ts \
	../translations/fet_vi.ts \
	../translations/fet_zh_CN.ts \
	../translations/fet_zh_TW.ts

TEMPLATE = app

DEFINES += \
	QT_NO_FOREACH \
	QT_NO_JAVA_STYLE_ITERATORS \
	QT_NO_LINKED_LIST \
	QT_STRICT_ITERATORS

CONFIG += release warn_on c++17 no_keywords

QT += network widgets
!isEmpty(QT.printsupport.name): QT += printsupport

DESTDIR = ..
TARGET = fet

win32 {
	RC_ICONS = ../icons/fet.ico
}
else: macx {
	ICON = ../icons/fet.icns
}

OBJECTS_DIR = ../tmp/interface
UI_DIR = ../tmp/interface
MOC_DIR = ../tmp/interface
RCC_DIR = ../tmp/interface

INCLUDEPATH += cl engine interface
DEPENDPATH += cl engine interface

RESOURCES += interface/interface_images.qrc

unix {
	target.path = /usr/bin
	INSTALLS += target
}
