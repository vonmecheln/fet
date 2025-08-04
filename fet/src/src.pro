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
	engine/import.cpp \
	engine/export.cpp \
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
	engine/statisticsexport.cpp \
	engine/lockunlock.cpp \
	\
	interface/messageboxes.cpp \
	\
	interface/addormodifytimeconstraint.cpp \
	interface/modifytimeconstraints.cpp \
	interface/listtimeconstraints.cpp \
	\
	interface/addormodifyspaceconstraint.cpp \
	interface/listspaceconstraints.cpp \
	\
	interface/fet.cpp \
	interface/helpaboutform.cpp \
	interface/helpblockplanningform.cpp \
	interface/helptermsform.cpp \
	interface/helpaboutlibrariesform.cpp \
	interface/helpfaqform.cpp \
	interface/helptipsform.cpp \
	interface/helpinstructionsform.cpp \
	interface/activitiesform.cpp \
	interface/activitiestagsform.cpp \
	interface/addactivityform.cpp \
	interface/modifyactivityform.cpp \
	\
	interface/subactivitiesform.cpp \
	interface/modifysubactivityform.cpp \
	\
	interface/addroomform.cpp \
	interface/modifyroomform.cpp \
	interface/addbuildingform.cpp \
	interface/modifybuildingform.cpp \
	interface/addstudentsyearform.cpp \
	interface/modifystudentsyearform.cpp \
	interface/addstudentsgroupform.cpp \
	interface/addstudentssubgroupform.cpp \
	interface/addexistingstudentssubgroupsform.cpp \
	interface/addexistingstudentsgroupsform.cpp \
	interface/modifystudentsgroupform.cpp \
	interface/modifystudentssubgroupform.cpp \
	\
	interface/alltimeconstraintsform.cpp \
	interface/allspaceconstraintsform.cpp \
	interface/groupsform.cpp \
	interface/subgroupsform.cpp \
	interface/subjectsform.cpp \
	interface/activitytagsform.cpp \
	interface/teachersform.cpp \
	interface/addteacherform.cpp \
	interface/modifyteacherform.cpp \
	interface/timetablegenerateform.cpp \
	interface/timetablegeneratemultipleform.cpp \
	interface/fetmainform.cpp \
	interface/utilities.cpp \
	interface/helponstudentsminhoursdaily.cpp \
	interface/settingstimetablehtmllevelform.cpp \
	interface/timetableviewstudentsdayshorizontalform.cpp \
	interface/timetableviewstudentsdaysverticalform.cpp \
	interface/timetableviewstudentstimehorizontalform.cpp \
	interface/timetableviewroomsdayshorizontalform.cpp \
	interface/timetableviewroomsdaysverticalform.cpp \
	interface/timetableviewroomstimehorizontalform.cpp \
	interface/yearsform.cpp \
	interface/timetableshowconflictsform.cpp \
	interface/timetableviewteachersdayshorizontalform.cpp \
	interface/timetableviewteachersdaysverticalform.cpp \
	interface/timetableviewteacherstimehorizontalform.cpp \
	interface/hoursform.cpp \
	interface/daysform.cpp \
	\
	interface/modifydayform.cpp \
	interface/modifyrealdayform.cpp \
	interface/modifyhourform.cpp \
	interface/modifyrealhourform.cpp \
	\
	interface/roomsform.cpp \
	interface/randomseedform.cpp \
	interface/getmodefornewfileform.cpp \
	interface/roommakeeditvirtualform.cpp \
	interface/buildingsform.cpp \
	\
	interface/groupactivitiesininitialorderitemsform.cpp \
	interface/addgroupactivitiesininitialorderitemform.cpp \
	interface/modifygroupactivitiesininitialorderitemform.cpp \
	\
	interface/institutionnameform.cpp \
	interface/commentsform.cpp \
	interface/teachersstatisticsform.cpp \
	interface/subjectsstatisticsform.cpp \
	interface/studentsstatisticsform.cpp \
	interface/activitiesroomsstatisticsform.cpp \
	interface/splityearform.cpp \
	\
	interface/changemindaysselectivelyform.cpp \
	interface/changeminhalfdaysselectivelyform.cpp \
	\
	interface/spreadmindaysconstraintsfivedaysform.cpp \
	interface/spreadconfirmationform.cpp \
	\
	interface/removeredundantform.cpp \
	interface/removeredundantconfirmationform.cpp \
	\
	interface/savetimetableconfirmationform.cpp \
	\
	interface/advancedlockunlockform.cpp \
	\
	interface/longtextmessagebox.cpp \
	interface/advancedfilterform.cpp \
	\
	interface/activityplanningform.cpp \
	interface/activityplanningconfirmationform.cpp \
	\
	interface/sparsetableview.cpp \
	interface/sparseitemmodel.cpp \
	\
	interface/timetableprintform.cpp \
	interface/statisticsprintform.cpp \
	\
	interface/timetablestowriteondiskform.cpp \
	\
	interface/teachersubjectsqualificationsform.cpp \
	\
	interface/studentscomboboxesstyleform.cpp \
	\
	interface/notificationcommandform.cpp \
	\
	interface/helpmoroccoform.cpp \
	interface/helpalgeriaform.cpp \
	\
	interface/termsform.cpp \
	\
	interface/addremovemultipleconstraintsactivitiessamestartinghourform.cpp \
	interface/addremovemultipleconstraintsactivitiesoccupymaxdifferentroomsform.cpp \
	\
	interface/settingsdatatoprintintimetablesform.cpp \
	\
	interface/teachersnotavailabletimestimehorizontalform.cpp \
	interface/studentsnotavailabletimestimehorizontalform.cpp \
	\
	interface/restoredatastateform.cpp \
	\
	interface/settingsrestoredatafrommemoryform.cpp \
	interface/settingsrestoredatafromdiskform.cpp \
	\
	interface/settingsautosaveform.cpp \
	\
	interface/settingsstyleandcolorschemeform.cpp

HEADERS += \
	engine/timetableexport.h \
	engine/activity.h \
	engine/solution.h \
	engine/timetable.h \
	engine/rules.h \
	engine/import.h \
	engine/export.h \
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
	engine/statisticsexport.h \
	engine/lockunlock.h \
	engine/matrix.h \
	\
	interface/messageboxes.h \
	\
	interface/addormodifytimeconstraint.h \
	interface/modifytimeconstraints.h \
	interface/listtimeconstraints.h \
	\
	interface/addormodifyspaceconstraint.h \
	interface/listspaceconstraints.h \
	\
	interface/fet.h \
	interface/helpaboutform.h \
	interface/helpblockplanningform.h \
	interface/helptermsform.h \
	interface/helpaboutlibrariesform.h \
	interface/helpfaqform.h \
	interface/helptipsform.h \
	interface/helpinstructionsform.h \
	interface/activitiesform.h \
	interface/activitiestagsform.h \
	interface/addactivityform.h \
	interface/modifyactivityform.h \
	\
	interface/subactivitiesform.h \
	interface/modifysubactivityform.h \
	\
	interface/addroomform.h \
	interface/modifyroomform.h \
	interface/addbuildingform.h \
	interface/modifybuildingform.h \
	interface/addstudentsyearform.h \
	interface/modifystudentsyearform.h \
	interface/addstudentsgroupform.h \
	interface/addstudentssubgroupform.h \
	interface/addexistingstudentssubgroupsform.h \
	interface/addexistingstudentsgroupsform.h \
	interface/modifystudentsgroupform.h \
	interface/modifystudentssubgroupform.h \
	\
	interface/alltimeconstraintsform.h \
	interface/allspaceconstraintsform.h \
	interface/groupsform.h \
	interface/subgroupsform.h \
	interface/subjectsform.h \
	interface/activitytagsform.h \
	interface/teachersform.h \
	interface/addteacherform.h \
	interface/modifyteacherform.h \
	interface/timetablegenerateform.h \
	interface/timetablegeneratemultipleform.h \
	interface/fetmainform.h \
	interface/utilities.h \
	interface/helponstudentsminhoursdaily.h \
	interface/settingstimetablehtmllevelform.h \
	interface/timetableviewstudentsdayshorizontalform.h \
	interface/timetableviewstudentsdaysverticalform.h \
	interface/timetableviewstudentstimehorizontalform.h \
	interface/timetableviewroomsdayshorizontalform.h \
	interface/timetableviewroomsdaysverticalform.h \
	interface/timetableviewroomstimehorizontalform.h \
	interface/yearsform.h \
	interface/timetableshowconflictsform.h \
	interface/timetableviewteachersdayshorizontalform.h \
	interface/timetableviewteachersdaysverticalform.h \
	interface/timetableviewteacherstimehorizontalform.h \
	interface/hoursform.h \
	interface/daysform.h \
	\
	interface/modifydayform.h \
	interface/modifyrealdayform.h \
	interface/modifyhourform.h \
	interface/modifyrealhourform.h \
	\
	interface/roomsform.h \
	interface/randomseedform.h \
	interface/getmodefornewfileform.h \
	interface/roommakeeditvirtualform.h \
	interface/buildingsform.h \
	\
	interface/groupactivitiesininitialorderitemsform.h \
	interface/addgroupactivitiesininitialorderitemform.h \
	interface/modifygroupactivitiesininitialorderitemform.h \
	\
	interface/institutionnameform.h \
	interface/commentsform.h \
	interface/teachersstatisticsform.h \
	interface/subjectsstatisticsform.h \
	interface/studentsstatisticsform.h \
	interface/activitiesroomsstatisticsform.h \
	interface/splityearform.h\
	\
	interface/changemindaysselectivelyform.h \
	interface/changeminhalfdaysselectivelyform.h \
	\
	interface/spreadmindaysconstraintsfivedaysform.h \
	interface/spreadconfirmationform.h \
	\
	interface/removeredundantform.h \
	interface/removeredundantconfirmationform.h \
	\
	interface/savetimetableconfirmationform.h \
	\
	interface/advancedlockunlockform.h \
	\
	interface/longtextmessagebox.h \
	interface/longtextmessagebox_auxiliary.h \
	interface/advancedfilterform.h \
	\
	interface/activityplanningform.h \
	interface/activityplanningconfirmationform.h \
	\
	interface/sparsetableview.h \
	interface/sparseitemmodel.h \
	\
	interface/timetableprintform.h \
	interface/statisticsprintform.h \
	\
	interface/timetablestowriteondiskform.h \
	\
	interface/teachersubjectsqualificationsform.h \
	\
	interface/studentscomboboxesstyleform.h \
	\
	interface/notificationcommandform.h \
	\
	interface/helpmoroccoform.h \
	interface/helpalgeriaform.h \
	\
	interface/termsform.h \
	\
	interface/addremovemultipleconstraintsactivitiessamestartinghourform.h \
	interface/addremovemultipleconstraintsactivitiesoccupymaxdifferentroomsform.h \
	\
	interface/settingsdatatoprintintimetablesform.h \
	\
	interface/teachersnotavailabletimestimehorizontalform.h \
	interface/studentsnotavailabletimestimehorizontalform.h \
	\
	interface/restoredatastateform.h \
	\
	interface/settingsrestoredatafrommemoryform.h \
	interface/settingsrestoredatafromdiskform.h \
	\
	interface/settingsautosaveform.h \
	\
	interface/settingsstyleandcolorschemeform.h

FORMS += \
	interface/fetmainform_template.ui \
	interface/helpaboutform_template.ui \
	interface/helpaboutlibrariesform_template.ui \
	interface/teachersstatisticsform_template.ui \
	interface/subjectsstatisticsform_template.ui \
	interface/studentsstatisticsform_template.ui \
	interface/activitiesroomsstatisticsform_template.ui \
	interface/settingstimetablehtmllevelform_template.ui \
	\
	interface/changemindaysselectivelyform_template.ui \
	interface/changeminhalfdaysselectivelyform_template.ui \
	interface/spreadmindaysconstraintsfivedaysform_template.ui \
	interface/spreadconfirmationform_template.ui \
	\
	interface/removeredundantform_template.ui \
	interface/removeredundantconfirmationform_template.ui \
	\
	interface/savetimetableconfirmationform_template.ui \
	\
	interface/activitiesform_template.ui \
	interface/activitiestagsform_template.ui \
	interface/activitytagsform_template.ui \
	interface/addactivityform_template.ui \
	interface/addbuildingform_template.ui \
	\
	interface/helpblockplanningform_template.ui \
	interface/helptermsform_template.ui \
	interface/helpfaqform_template.ui \
	interface/helptipsform_template.ui \
	interface/helpinstructionsform_template.ui \
	interface/modifyactivityform_template.ui \
	\
	interface/subactivitiesform_template.ui \
	interface/modifysubactivityform_template.ui \
	\
	interface/addroomform_template.ui \
	interface/modifyroomform_template.ui \
	interface/modifybuildingform_template.ui \
	interface/addstudentsyearform_template.ui \
	interface/modifystudentsyearform_template.ui \
	interface/addstudentsgroupform_template.ui \
	interface/addstudentssubgroupform_template.ui \
	interface/addexistingstudentssubgroupsform_template.ui \
	interface/addexistingstudentsgroupsform_template.ui \
	interface/modifystudentsgroupform_template.ui \
	interface/modifystudentssubgroupform_template.ui \
	\
	interface/alltimeconstraintsform_template.ui \
	interface/allspaceconstraintsform_template.ui \
	interface/groupsform_template.ui \
	interface/subgroupsform_template.ui \
	interface/subjectsform_template.ui \
	interface/teachersform_template.ui \
	interface/addteacherform_template.ui \
	interface/modifyteacherform_template.ui \
	interface/timetablegenerateform_template.ui \
	interface/timetablegeneratemultipleform_template.ui \
	interface/timetableviewstudentsdayshorizontalform_template.ui \
	interface/timetableviewstudentsdaysverticalform_template.ui \
	interface/timetableviewstudentstimehorizontalform_template.ui \
	interface/timetableviewroomsdayshorizontalform_template.ui \
	interface/timetableviewroomsdaysverticalform_template.ui \
	interface/timetableviewroomstimehorizontalform_template.ui \
	interface/yearsform_template.ui \
	interface/timetableshowconflictsform_template.ui \
	interface/timetableviewteachersdayshorizontalform_template.ui \
	interface/timetableviewteachersdaysverticalform_template.ui \
	interface/timetableviewteacherstimehorizontalform_template.ui \
	interface/daysform_template.ui \
	interface/hoursform_template.ui \
	\
	interface/modifydayform_template.ui \
	interface/modifyrealdayform_template.ui \
	interface/modifyhourform_template.ui \
	interface/modifyrealhourform_template.ui \
	\
	interface/roomsform_template.ui \
	interface/randomseedform_template.ui \
	interface/getmodefornewfileform_template.ui \
	interface/roommakeeditvirtualform_template.ui \
	interface/buildingsform_template.ui \
	\
	interface/groupactivitiesininitialorderitemsform_template.ui \
	interface/addgroupactivitiesininitialorderitemform_template.ui \
	interface/modifygroupactivitiesininitialorderitemform_template.ui \
	\
	interface/institutionnameform_template.ui \
	interface/commentsform_template.ui \
	interface/splityearform_template.ui \
	\
	interface/activityplanningconfirmationform_template.ui \
	\
	interface/timetablestowriteondiskform_template.ui \
	\
	interface/teachersubjectsqualificationsform_template.ui \
	\
	interface/studentscomboboxesstyleform_template.ui \
	\
	interface/notificationcommandform_template.ui \
	\
	interface/helpmoroccoform_template.ui \
	interface/helpalgeriaform_template.ui \
	\
	interface/termsform_template.ui \
	\
	interface/addremovemultipleconstraintsactivitiessamestartinghourform_template.ui \
	interface/addremovemultipleconstraintsactivitiesoccupymaxdifferentroomsform_template.ui \
	\
	interface/settingsdatatoprintintimetablesform_template.ui \
	\
	interface/teachersnotavailabletimestimehorizontalform_template.ui \
	interface/studentsnotavailabletimestimehorizontalform_template.ui \
	\
	interface/restoredatastateform_template.ui \
	\
	interface/settingsrestoredatafrommemoryform_template.ui \
	interface/settingsrestoredatafromdiskform_template.ui \
	\
	interface/settingsautosaveform_template.ui \
	\
	interface/settingsstyleandcolorschemeform_template.ui

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

OBJECTS_DIR = ../tmp/gui
UI_DIR = ../tmp/gui
MOC_DIR = ../tmp/gui
RCC_DIR = ../tmp/gui

INCLUDEPATH += engine interface
DEPENDPATH += engine interface

RESOURCES += interface/interface_images.qrc

unix {
	target.path = /usr/bin
	INSTALLS += target
}
