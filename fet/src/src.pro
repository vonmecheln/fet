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
	engine/messageboxes.cpp \
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
	interface/addconstraintactivitypreferredtimeslotsform.cpp \
	interface/modifyconstraintactivitypreferredtimeslotsform.cpp \
	interface/addconstraintactivitypreferredstartingtimesform.cpp \
	interface/modifyconstraintactivitypreferredstartingtimesform.cpp \
	interface/addconstraintroomnotavailabletimesform.cpp \
	interface/constraintroomnotavailabletimesform.cpp \
	interface/modifyconstraintroomnotavailabletimesform.cpp \
	interface/addconstraintteacherroomnotavailabletimesform.cpp \
	interface/constraintteacherroomnotavailabletimesform.cpp \
	interface/modifyconstraintteacherroomnotavailabletimesform.cpp \
	interface/addconstraintactivitypreferredroomform.cpp \
	interface/constraintactivitypreferredroomform.cpp \
	interface/modifyconstraintactivitypreferredroomform.cpp \
	interface/addconstraintsubjectpreferredroomform.cpp \
	interface/constraintsubjectpreferredroomform.cpp \
	interface/modifyconstraintsubjectpreferredroomform.cpp \
	interface/addconstraintsubjectpreferredroomsform.cpp \
	interface/constraintsubjectpreferredroomsform.cpp \
	interface/modifyconstraintsubjectpreferredroomsform.cpp \
	interface/addconstraintsubjectactivitytagpreferredroomform.cpp \
	interface/constraintsubjectactivitytagpreferredroomform.cpp \
	interface/modifyconstraintsubjectactivitytagpreferredroomform.cpp \
	interface/addconstraintsubjectactivitytagpreferredroomsform.cpp \
	interface/constraintsubjectactivitytagpreferredroomsform.cpp \
	interface/modifyconstraintsubjectactivitytagpreferredroomsform.cpp \
	\
	interface/addconstraintactivitytagpreferredroomform.cpp \
	interface/constraintactivitytagpreferredroomform.cpp \
	interface/modifyconstraintactivitytagpreferredroomform.cpp \
	interface/addconstraintactivitytagpreferredroomsform.cpp \
	interface/constraintactivitytagpreferredroomsform.cpp \
	interface/modifyconstraintactivitytagpreferredroomsform.cpp \
	\
	interface/addconstraintstudentssethomeroomform.cpp \
	\
	interface/addconstraintstudentssetmaxbuildingchangesperdayform.cpp \
	interface/constraintstudentssetmaxbuildingchangesperdayform.cpp \
	interface/modifyconstraintstudentssetmaxbuildingchangesperdayform.cpp \
	interface/addconstraintstudentsmaxbuildingchangesperdayform.cpp \
	interface/constraintstudentsmaxbuildingchangesperdayform.cpp \
	interface/modifyconstraintstudentsmaxbuildingchangesperdayform.cpp \
	interface/addconstraintstudentssetmaxbuildingchangesperweekform.cpp \
	interface/constraintstudentssetmaxbuildingchangesperweekform.cpp \
	interface/modifyconstraintstudentssetmaxbuildingchangesperweekform.cpp \
	interface/addconstraintstudentsmaxbuildingchangesperweekform.cpp \
	interface/constraintstudentsmaxbuildingchangesperweekform.cpp \
	interface/modifyconstraintstudentsmaxbuildingchangesperweekform.cpp \
	interface/addconstraintstudentssetmingapsbetweenbuildingchangesform.cpp \
	interface/constraintstudentssetmingapsbetweenbuildingchangesform.cpp \
	interface/modifyconstraintstudentssetmingapsbetweenbuildingchangesform.cpp \
	interface/addconstraintstudentsmingapsbetweenbuildingchangesform.cpp \
	interface/constraintstudentsmingapsbetweenbuildingchangesform.cpp \
	interface/modifyconstraintstudentsmingapsbetweenbuildingchangesform.cpp \
	\
	interface/addconstraintteachermaxbuildingchangesperdayform.cpp \
	interface/constraintteachermaxbuildingchangesperdayform.cpp \
	interface/modifyconstraintteachermaxbuildingchangesperdayform.cpp \
	interface/addconstraintteachersmaxbuildingchangesperdayform.cpp \
	interface/constraintteachersmaxbuildingchangesperdayform.cpp \
	interface/modifyconstraintteachersmaxbuildingchangesperdayform.cpp \
	interface/addconstraintteachermaxbuildingchangesperweekform.cpp \
	interface/constraintteachermaxbuildingchangesperweekform.cpp \
	interface/modifyconstraintteachermaxbuildingchangesperweekform.cpp \
	interface/addconstraintteachersmaxbuildingchangesperweekform.cpp \
	interface/constraintteachersmaxbuildingchangesperweekform.cpp \
	interface/modifyconstraintteachersmaxbuildingchangesperweekform.cpp \
	interface/addconstraintteachermingapsbetweenbuildingchangesform.cpp \
	interface/constraintteachermingapsbetweenbuildingchangesform.cpp \
	interface/modifyconstraintteachermingapsbetweenbuildingchangesform.cpp \
	interface/addconstraintteachersmingapsbetweenbuildingchangesform.cpp \
	interface/constraintteachersmingapsbetweenbuildingchangesform.cpp \
	interface/modifyconstraintteachersmingapsbetweenbuildingchangesform.cpp \
	\
	interface/addconstraintstudentssetmaxroomchangesperdayform.cpp \
	interface/constraintstudentssetmaxroomchangesperdayform.cpp \
	interface/modifyconstraintstudentssetmaxroomchangesperdayform.cpp \
	interface/addconstraintstudentsmaxroomchangesperdayform.cpp \
	interface/constraintstudentsmaxroomchangesperdayform.cpp \
	interface/modifyconstraintstudentsmaxroomchangesperdayform.cpp \
	interface/addconstraintstudentssetmaxroomchangesperweekform.cpp \
	interface/constraintstudentssetmaxroomchangesperweekform.cpp \
	interface/modifyconstraintstudentssetmaxroomchangesperweekform.cpp \
	interface/addconstraintstudentsmaxroomchangesperweekform.cpp \
	interface/constraintstudentsmaxroomchangesperweekform.cpp \
	interface/modifyconstraintstudentsmaxroomchangesperweekform.cpp \
	interface/addconstraintstudentssetmingapsbetweenroomchangesform.cpp \
	interface/constraintstudentssetmingapsbetweenroomchangesform.cpp \
	interface/modifyconstraintstudentssetmingapsbetweenroomchangesform.cpp \
	interface/addconstraintstudentsmingapsbetweenroomchangesform.cpp \
	interface/constraintstudentsmingapsbetweenroomchangesform.cpp \
	interface/modifyconstraintstudentsmingapsbetweenroomchangesform.cpp \
	\
	interface/addconstraintteachermaxroomchangesperdayform.cpp \
	interface/constraintteachermaxroomchangesperdayform.cpp \
	interface/modifyconstraintteachermaxroomchangesperdayform.cpp \
	interface/addconstraintteachersmaxroomchangesperdayform.cpp \
	interface/constraintteachersmaxroomchangesperdayform.cpp \
	interface/modifyconstraintteachersmaxroomchangesperdayform.cpp \
	interface/addconstraintteachermaxroomchangesperweekform.cpp \
	interface/constraintteachermaxroomchangesperweekform.cpp \
	interface/modifyconstraintteachermaxroomchangesperweekform.cpp \
	interface/addconstraintteachersmaxroomchangesperweekform.cpp \
	interface/constraintteachersmaxroomchangesperweekform.cpp \
	interface/modifyconstraintteachersmaxroomchangesperweekform.cpp \
	interface/addconstraintteachermingapsbetweenroomchangesform.cpp \
	interface/constraintteachermingapsbetweenroomchangesform.cpp \
	interface/modifyconstraintteachermingapsbetweenroomchangesform.cpp \
	interface/addconstraintteachersmingapsbetweenroomchangesform.cpp \
	interface/constraintteachersmingapsbetweenroomchangesform.cpp \
	interface/modifyconstraintteachersmingapsbetweenroomchangesform.cpp \
	\
	interface/constraintstudentssethomeroomform.cpp \
	interface/modifyconstraintstudentssethomeroomform.cpp \
	interface/addconstraintstudentssethomeroomsform.cpp \
	interface/constraintstudentssethomeroomsform.cpp \
	interface/modifyconstraintstudentssethomeroomsform.cpp \
	interface/addconstraintteacherhomeroomform.cpp \
	interface/constraintteacherhomeroomform.cpp \
	interface/modifyconstraintteacherhomeroomform.cpp \
	interface/addconstraintteacherhomeroomsform.cpp \
	interface/constraintteacherhomeroomsform.cpp \
	interface/modifyconstraintteacherhomeroomsform.cpp \
	interface/addconstraintactivitypreferredroomsform.cpp \
	interface/constraintactivitypreferredroomsform.cpp \
	interface/modifyconstraintactivitypreferredroomsform.cpp \
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
	interface/centerwidgetonscreen.cpp \
	interface/helponimperfectconstraints.cpp \
	interface/helponstudentsminhoursdaily.cpp \
	interface/settingstimetablehtmllevelform.cpp \
	interface/timetableviewstudentsdayshorizontalform.cpp \
	interface/timetableviewstudentstimehorizontalform.cpp \
	interface/timetableviewroomsdayshorizontalform.cpp \
	interface/timetableviewroomstimehorizontalform.cpp \
	interface/yearsform.cpp \
	interface/timetableshowconflictsform.cpp \
	interface/timetableviewteachersdayshorizontalform.cpp \
	interface/timetableviewteacherstimehorizontalform.cpp \
	interface/hoursform.cpp \
	interface/daysform.cpp \
	interface/roomsform.cpp \
	interface/randomseedform.cpp \
	interface/getmodefornewfileform.cpp \
	interface/roommakeeditvirtualform.cpp \
	interface/buildingsform.cpp \
	\
	interface/constraintactivitiespreferredtimeslotsform.cpp \
	interface/addconstraintactivitiespreferredtimeslotsform.cpp \
	interface/modifyconstraintactivitiespreferredtimeslotsform.cpp \
	interface/constraintactivitiespreferredstartingtimesform.cpp \
	interface/addconstraintactivitiespreferredstartingtimesform.cpp \
	interface/modifyconstraintactivitiespreferredstartingtimesform.cpp \
	\
	interface/constraintsubactivitiespreferredtimeslotsform.cpp \
	interface/addconstraintsubactivitiespreferredtimeslotsform.cpp \
	interface/modifyconstraintsubactivitiespreferredtimeslotsform.cpp \
	interface/constraintsubactivitiespreferredstartingtimesform.cpp \
	interface/addconstraintsubactivitiespreferredstartingtimesform.cpp \
	interface/modifyconstraintsubactivitiespreferredstartingtimesform.cpp \
	\
	interface/constraintactivitiessamestartingtimeform.cpp \
	interface/addconstraintactivitiessamestartingtimeform.cpp \
	\
	interface/constraintactivitiessamestartinghourform.cpp \
	interface/addconstraintactivitiessamestartinghourform.cpp \
	\
	interface/constraintactivitiessamestartingdayform.cpp \
	interface/addconstraintactivitiessamestartingdayform.cpp \
	interface/modifyconstraintactivitiessamestartingdayform.cpp \
	\
	interface/constraintteachernotavailabletimesform.cpp \
	interface/addconstraintteachernotavailabletimesform.cpp \
	interface/modifyconstraintteachernotavailabletimesform.cpp \
	interface/constraintbasiccompulsorytimeform.cpp \
	interface/addconstraintbasiccompulsorytimeform.cpp \
	interface/modifyconstraintbasiccompulsorytimeform.cpp \
	interface/constraintbasiccompulsoryspaceform.cpp \
	interface/addconstraintbasiccompulsoryspaceform.cpp \
	interface/modifyconstraintbasiccompulsoryspaceform.cpp \
	interface/constraintstudentssetnotavailabletimesform.cpp \
	interface/addconstraintstudentssetnotavailabletimesform.cpp \
	interface/modifyconstraintstudentssetnotavailabletimesform.cpp \
	interface/constraintbreaktimesform.cpp \
	interface/addconstraintbreaktimesform.cpp \
	interface/modifyconstraintbreaktimesform.cpp \
	interface/constraintteachermaxdaysperweekform.cpp \
	interface/addconstraintteachermaxdaysperweekform.cpp \
	interface/modifyconstraintteachermaxdaysperweekform.cpp \
	\
	interface/constraintteachermaxthreeconsecutivedaysform.cpp \
	interface/addconstraintteachermaxthreeconsecutivedaysform.cpp \
	interface/modifyconstraintteachermaxthreeconsecutivedaysform.cpp \
	interface/constraintteachersmaxthreeconsecutivedaysform.cpp \
	interface/addconstraintteachersmaxthreeconsecutivedaysform.cpp \
	interface/modifyconstraintteachersmaxthreeconsecutivedaysform.cpp \
	\
	interface/constraintstudentssetmaxthreeconsecutivedaysform.cpp \
	interface/addconstraintstudentssetmaxthreeconsecutivedaysform.cpp \
	interface/modifyconstraintstudentssetmaxthreeconsecutivedaysform.cpp \
	interface/constraintstudentsmaxthreeconsecutivedaysform.cpp \
	interface/addconstraintstudentsmaxthreeconsecutivedaysform.cpp \
	interface/modifyconstraintstudentsmaxthreeconsecutivedaysform.cpp \
	\
	interface/constraintteachermindaysperweekform.cpp \
	interface/addconstraintteachermindaysperweekform.cpp \
	interface/modifyconstraintteachermindaysperweekform.cpp \
	\
	interface/constraintteachersmaxdaysperweekform.cpp \
	interface/addconstraintteachersmaxdaysperweekform.cpp \
	interface/modifyconstraintteachersmaxdaysperweekform.cpp \
	\
	interface/constraintteachersmindaysperweekform.cpp \
	interface/addconstraintteachersmindaysperweekform.cpp \
	interface/modifyconstraintteachersmindaysperweekform.cpp \
	\
	interface/constraintteacherintervalmaxdaysperweekform.cpp \
	interface/addconstraintteacherintervalmaxdaysperweekform.cpp \
	interface/modifyconstraintteacherintervalmaxdaysperweekform.cpp \
	\
	interface/constraintstudentssetintervalmaxdaysperweekform.cpp \
	interface/addconstraintstudentssetintervalmaxdaysperweekform.cpp \
	interface/modifyconstraintstudentssetintervalmaxdaysperweekform.cpp \
	\
	interface/constraintstudentsintervalmaxdaysperweekform.cpp \
	interface/addconstraintstudentsintervalmaxdaysperweekform.cpp \
	interface/modifyconstraintstudentsintervalmaxdaysperweekform.cpp \
	\
	interface/constraintstudentssetmaxdaysperweekform.cpp \
	interface/addconstraintstudentssetmaxdaysperweekform.cpp \
	interface/modifyconstraintstudentssetmaxdaysperweekform.cpp \
	\
	interface/constraintstudentsmaxdaysperweekform.cpp \
	interface/addconstraintstudentsmaxdaysperweekform.cpp \
	interface/modifyconstraintstudentsmaxdaysperweekform.cpp \
	\
	interface/constraintteachersintervalmaxdaysperweekform.cpp \
	interface/addconstraintteachersintervalmaxdaysperweekform.cpp \
	interface/modifyconstraintteachersintervalmaxdaysperweekform.cpp \
	\
	interface/constraintteachersmaxhoursdailyform.cpp \
	interface/addconstraintteachersmaxhoursdailyform.cpp \
	interface/modifyconstraintteachersmaxhoursdailyform.cpp \
	interface/constraintteachermaxhoursdailyform.cpp \
	interface/addconstraintteachermaxhoursdailyform.cpp \
	interface/modifyconstraintteachermaxhoursdailyform.cpp \
	\
	interface/constraintteachersmaxhourscontinuouslyform.cpp \
	interface/addconstraintteachersmaxhourscontinuouslyform.cpp \
	interface/modifyconstraintteachersmaxhourscontinuouslyform.cpp \
	interface/constraintteachermaxhourscontinuouslyform.cpp \
	interface/addconstraintteachermaxhourscontinuouslyform.cpp \
	interface/modifyconstraintteachermaxhourscontinuouslyform.cpp \
	\
	interface/constraintteachersactivitytagmaxhourscontinuouslyform.cpp \
	interface/addconstraintteachersactivitytagmaxhourscontinuouslyform.cpp \
	interface/modifyconstraintteachersactivitytagmaxhourscontinuouslyform.cpp \
	interface/constraintteacheractivitytagmaxhourscontinuouslyform.cpp \
	interface/addconstraintteacheractivitytagmaxhourscontinuouslyform.cpp \
	interface/modifyconstraintteacheractivitytagmaxhourscontinuouslyform.cpp \
	\
	interface/constraintteachersactivitytagmaxhoursdailyform.cpp \
	interface/addconstraintteachersactivitytagmaxhoursdailyform.cpp \
	interface/modifyconstraintteachersactivitytagmaxhoursdailyform.cpp \
	interface/constraintteacheractivitytagmaxhoursdailyform.cpp \
	interface/addconstraintteacheractivitytagmaxhoursdailyform.cpp \
	interface/modifyconstraintteacheractivitytagmaxhoursdailyform.cpp \
	\
	interface/constraintteachersactivitytagminhoursdailyform.cpp \
	interface/addconstraintteachersactivitytagminhoursdailyform.cpp \
	interface/modifyconstraintteachersactivitytagminhoursdailyform.cpp \
	interface/constraintteacheractivitytagminhoursdailyform.cpp \
	interface/addconstraintteacheractivitytagminhoursdailyform.cpp \
	interface/modifyconstraintteacheractivitytagminhoursdailyform.cpp \
	\
	interface/constraintteachersminhoursdailyform.cpp \
	interface/addconstraintteachersminhoursdailyform.cpp \
	interface/modifyconstraintteachersminhoursdailyform.cpp \
	interface/constraintteacherminhoursdailyform.cpp \
	interface/addconstraintteacherminhoursdailyform.cpp \
	interface/modifyconstraintteacherminhoursdailyform.cpp \
	interface/constraintactivitypreferredstartingtimeform.cpp \
	interface/addconstraintactivitypreferredstartingtimeform.cpp \
	interface/modifyconstraintactivitypreferredstartingtimeform.cpp \
	\
	interface/constraintactivityendsstudentsdayform.cpp \
	interface/addconstraintactivityendsstudentsdayform.cpp \
	interface/modifyconstraintactivityendsstudentsdayform.cpp \
	\
	interface/constraintactivitiesendstudentsdayform.cpp \
	interface/addconstraintactivitiesendstudentsdayform.cpp \
	interface/modifyconstraintactivitiesendstudentsdayform.cpp \
	\
	interface/constraintactivityendsteachersdayform.cpp \
	interface/addconstraintactivityendsteachersdayform.cpp \
	interface/modifyconstraintactivityendsteachersdayform.cpp \
	\
	interface/constraintactivitiesendteachersdayform.cpp \
	interface/addconstraintactivitiesendteachersdayform.cpp \
	interface/modifyconstraintactivitiesendteachersdayform.cpp \
	\
	interface/constraintactivitybeginsstudentsdayform.cpp \
	interface/addconstraintactivitybeginsstudentsdayform.cpp \
	interface/modifyconstraintactivitybeginsstudentsdayform.cpp \
	\
	interface/constraintactivitiesbeginstudentsdayform.cpp \
	interface/addconstraintactivitiesbeginstudentsdayform.cpp \
	interface/modifyconstraintactivitiesbeginstudentsdayform.cpp \
	\
	interface/constraintactivitybeginsteachersdayform.cpp \
	interface/addconstraintactivitybeginsteachersdayform.cpp \
	interface/modifyconstraintactivitybeginsteachersdayform.cpp \
	\
	interface/constraintactivitiesbeginteachersdayform.cpp \
	interface/addconstraintactivitiesbeginteachersdayform.cpp \
	interface/modifyconstraintactivitiesbeginteachersdayform.cpp \
	\
	interface/constraintstudentssetmaxgapsperweekform.cpp \
	interface/addconstraintstudentssetmaxgapsperweekform.cpp \
	interface/modifyconstraintstudentssetmaxgapsperweekform.cpp \
	interface/constraintstudentsmaxgapsperweekform.cpp \
	interface/addconstraintstudentsmaxgapsperweekform.cpp \
	interface/modifyconstraintstudentsmaxgapsperweekform.cpp \
	\
	interface/constraintstudentssetmaxgapsperdayform.cpp \
	interface/addconstraintstudentssetmaxgapsperdayform.cpp \
	interface/modifyconstraintstudentssetmaxgapsperdayform.cpp \
	interface/constraintstudentsmaxgapsperdayform.cpp \
	interface/addconstraintstudentsmaxgapsperdayform.cpp \
	interface/modifyconstraintstudentsmaxgapsperdayform.cpp \
	\
	interface/constraintteachersmaxgapsperweekform.cpp \
	interface/constraintteachermaxgapsperweekform.cpp \
	interface/addconstraintteachersmaxgapsperweekform.cpp \
	interface/modifyconstraintteachersmaxgapsperweekform.cpp \
	interface/addconstraintteachermaxgapsperweekform.cpp \
	interface/modifyconstraintteachermaxgapsperweekform.cpp \
	interface/constraintteachersmaxgapsperdayform.cpp \
	interface/constraintteachermaxgapsperdayform.cpp \
	interface/addconstraintteachersmaxgapsperdayform.cpp \
	interface/modifyconstraintteachersmaxgapsperdayform.cpp \
	interface/addconstraintteachermaxgapsperdayform.cpp \
	interface/modifyconstraintteachermaxgapsperdayform.cpp \
	\
	interface/constraintteachersmaxgapspermorningandafternoonform.cpp \
	interface/constraintteachermaxgapspermorningandafternoonform.cpp \
	interface/addconstraintteachersmaxgapspermorningandafternoonform.cpp \
	interface/modifyconstraintteachersmaxgapspermorningandafternoonform.cpp \
	interface/addconstraintteachermaxgapspermorningandafternoonform.cpp \
	interface/modifyconstraintteachermaxgapspermorningandafternoonform.cpp \
	\
	interface/constraintstudentsearlymaxbeginningsatsecondhourform.cpp \
	interface/addconstraintstudentsearlymaxbeginningsatsecondhourform.cpp \
	interface/modifyconstraintstudentsearlymaxbeginningsatsecondhourform.cpp \
	interface/constraintstudentssetearlymaxbeginningsatsecondhourform.cpp \
	interface/addconstraintstudentssetearlymaxbeginningsatsecondhourform.cpp \
	interface/modifyconstraintstudentssetearlymaxbeginningsatsecondhourform.cpp \
	\
	interface/constraintstudentssetmaxhoursdailyform.cpp \
	interface/addconstraintstudentssetmaxhoursdailyform.cpp \
	interface/modifyconstraintstudentssetmaxhoursdailyform.cpp \
	interface/constraintstudentsmaxhoursdailyform.cpp \
	interface/addconstraintstudentsmaxhoursdailyform.cpp \
	interface/modifyconstraintstudentsmaxhoursdailyform.cpp \
	\
	interface/constraintstudentssetmaxhourscontinuouslyform.cpp \
	interface/addconstraintstudentssetmaxhourscontinuouslyform.cpp \
	interface/modifyconstraintstudentssetmaxhourscontinuouslyform.cpp \
	interface/constraintstudentsmaxhourscontinuouslyform.cpp \
	interface/addconstraintstudentsmaxhourscontinuouslyform.cpp \
	interface/modifyconstraintstudentsmaxhourscontinuouslyform.cpp \
	\
	interface/constraintstudentssetactivitytagmaxhourscontinuouslyform.cpp \
	interface/addconstraintstudentssetactivitytagmaxhourscontinuouslyform.cpp \
	interface/modifyconstraintstudentssetactivitytagmaxhourscontinuouslyform.cpp \
	interface/constraintstudentsactivitytagmaxhourscontinuouslyform.cpp \
	interface/addconstraintstudentsactivitytagmaxhourscontinuouslyform.cpp \
	interface/modifyconstraintstudentsactivitytagmaxhourscontinuouslyform.cpp \
	\
	interface/constraintstudentssetactivitytagmaxhoursdailyform.cpp \
	interface/addconstraintstudentssetactivitytagmaxhoursdailyform.cpp \
	interface/modifyconstraintstudentssetactivitytagmaxhoursdailyform.cpp \
	interface/constraintstudentsactivitytagmaxhoursdailyform.cpp \
	interface/addconstraintstudentsactivitytagmaxhoursdailyform.cpp \
	interface/modifyconstraintstudentsactivitytagmaxhoursdailyform.cpp \
	\
	interface/constraintstudentssetactivitytagminhoursdailyform.cpp \
	interface/addconstraintstudentssetactivitytagminhoursdailyform.cpp \
	interface/modifyconstraintstudentssetactivitytagminhoursdailyform.cpp \
	interface/constraintstudentsactivitytagminhoursdailyform.cpp \
	interface/addconstraintstudentsactivitytagminhoursdailyform.cpp \
	interface/modifyconstraintstudentsactivitytagminhoursdailyform.cpp \
	\
	interface/constraintstudentssetminhoursdailyform.cpp \
	interface/addconstraintstudentssetminhoursdailyform.cpp \
	interface/modifyconstraintstudentssetminhoursdailyform.cpp \
	interface/constraintstudentsminhoursdailyform.cpp \
	interface/addconstraintstudentsminhoursdailyform.cpp \
	interface/modifyconstraintstudentsminhoursdailyform.cpp \
	interface/constraintactivitiesnotoverlappingform.cpp \
	interface/addconstraintactivitiesnotoverlappingform.cpp \
	interface/modifyconstraintactivitiesnotoverlappingform.cpp \
	\
	interface/constraintactivitytagsnotoverlappingform.cpp \
	interface/addconstraintactivitytagsnotoverlappingform.cpp \
	interface/modifyconstraintactivitytagsnotoverlappingform.cpp \
	\
	interface/constraintactivitiesoccupymaxtimeslotsfromselectionform.cpp \
	interface/addconstraintactivitiesoccupymaxtimeslotsfromselectionform.cpp \
	interface/modifyconstraintactivitiesoccupymaxtimeslotsfromselectionform.cpp \
	\
	interface/constraintactivitiesoccupymintimeslotsfromselectionform.cpp \
	interface/addconstraintactivitiesoccupymintimeslotsfromselectionform.cpp \
	interface/modifyconstraintactivitiesoccupymintimeslotsfromselectionform.cpp \
	\
	interface/constraintactivitiesmaxsimultaneousinselectedtimeslotsform.cpp \
	interface/addconstraintactivitiesmaxsimultaneousinselectedtimeslotsform.cpp \
	interface/modifyconstraintactivitiesmaxsimultaneousinselectedtimeslotsform.cpp \
	\
	interface/constraintactivitiesminsimultaneousinselectedtimeslotsform.cpp \
	interface/addconstraintactivitiesminsimultaneousinselectedtimeslotsform.cpp \
	interface/modifyconstraintactivitiesminsimultaneousinselectedtimeslotsform.cpp \
	\
	interface/constraintactivitiesoccupymaxdifferentroomsform.cpp \
	interface/addconstraintactivitiesoccupymaxdifferentroomsform.cpp \
	interface/modifyconstraintactivitiesoccupymaxdifferentroomsform.cpp \
	\
	interface/constraintactivitiessameroomifconsecutiveform.cpp \
	interface/addconstraintactivitiessameroomifconsecutiveform.cpp \
	interface/modifyconstraintactivitiessameroomifconsecutiveform.cpp \
	\
	interface/constrainttwoactivitiesconsecutiveform.cpp \
	interface/addconstrainttwoactivitiesconsecutiveform.cpp \
	interface/modifyconstrainttwoactivitiesconsecutiveform.cpp \
	\
	interface/constrainttwoactivitiesgroupedform.cpp \
	interface/addconstrainttwoactivitiesgroupedform.cpp \
	interface/modifyconstrainttwoactivitiesgroupedform.cpp \
	\
	interface/constraintthreeactivitiesgroupedform.cpp \
	interface/addconstraintthreeactivitiesgroupedform.cpp \
	interface/modifyconstraintthreeactivitiesgroupedform.cpp \
	\
	interface/constrainttwoactivitiesorderedform.cpp \
	interface/addconstrainttwoactivitiesorderedform.cpp \
	interface/modifyconstrainttwoactivitiesorderedform.cpp \
	\
	interface/constrainttwosetsofactivitiesorderedform.cpp \
	interface/addconstrainttwosetsofactivitiesorderedform.cpp \
	interface/modifyconstrainttwosetsofactivitiesorderedform.cpp \
	\
	interface/constrainttwoactivitiesorderedifsamedayform.cpp \
	interface/addconstrainttwoactivitiesorderedifsamedayform.cpp \
	interface/modifyconstrainttwoactivitiesorderedifsamedayform.cpp \
	\
	interface/constraintmindaysbetweenactivitiesform.cpp \
	interface/addconstraintmindaysbetweenactivitiesform.cpp \
	interface/modifyconstraintmindaysbetweenactivitiesform.cpp \
	\
	interface/constraintminhalfdaysbetweenactivitiesform.cpp \
	interface/addconstraintminhalfdaysbetweenactivitiesform.cpp \
	interface/modifyconstraintminhalfdaysbetweenactivitiesform.cpp \
	\
	interface/groupactivitiesininitialorderitemsform.cpp \
	interface/addgroupactivitiesininitialorderitemform.cpp \
	interface/modifygroupactivitiesininitialorderitemform.cpp \
	\
	interface/constraintmaxdaysbetweenactivitiesform.cpp \
	interface/addconstraintmaxdaysbetweenactivitiesform.cpp \
	interface/modifyconstraintmaxdaysbetweenactivitiesform.cpp \
	\
	interface/constraintactivitiesmaxhourlyspanform.cpp \
	interface/addconstraintactivitiesmaxhourlyspanform.cpp \
	interface/modifyconstraintactivitiesmaxhourlyspanform.cpp \
	\
	interface/constraintmaxhalfdaysbetweenactivitiesform.cpp \
	interface/addconstraintmaxhalfdaysbetweenactivitiesform.cpp \
	interface/modifyconstraintmaxhalfdaysbetweenactivitiesform.cpp \
	\
	interface/constraintmaxtermsbetweenactivitiesform.cpp \
	interface/addconstraintmaxtermsbetweenactivitiesform.cpp \
	interface/modifyconstraintmaxtermsbetweenactivitiesform.cpp \
	\
	interface/constraintmingapsbetweenactivitiesform.cpp \
	interface/addconstraintmingapsbetweenactivitiesform.cpp \
	interface/modifyconstraintmingapsbetweenactivitiesform.cpp \
	\
	interface/constraintactivitypreferredtimeslotsform.cpp \
	interface/constraintactivitypreferredstartingtimesform.cpp \
	interface/modifyconstraintactivitiessamestartingtimeform.cpp \
	interface/modifyconstraintactivitiessamestartinghourform.cpp \
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
	interface/constraintteachermaxspanperdayform.cpp \
	interface/addconstraintteachermaxspanperdayform.cpp \
	interface/modifyconstraintteachermaxspanperdayform.cpp \
	interface/constraintteachersmaxspanperdayform.cpp \
	interface/addconstraintteachersmaxspanperdayform.cpp \
	interface/modifyconstraintteachersmaxspanperdayform.cpp \
	\
	interface/constraintstudentssetmaxspanperdayform.cpp \
	interface/addconstraintstudentssetmaxspanperdayform.cpp \
	interface/modifyconstraintstudentssetmaxspanperdayform.cpp \
	interface/constraintstudentsmaxspanperdayform.cpp \
	interface/addconstraintstudentsmaxspanperdayform.cpp \
	interface/modifyconstraintstudentsmaxspanperdayform.cpp \
	\
	interface/constraintteacherminrestinghoursform.cpp \
	interface/addconstraintteacherminrestinghoursform.cpp \
	interface/modifyconstraintteacherminrestinghoursform.cpp \
	interface/constraintteachersminrestinghoursform.cpp \
	interface/addconstraintteachersminrestinghoursform.cpp \
	interface/modifyconstraintteachersminrestinghoursform.cpp \
	\
	interface/constraintstudentssetminrestinghoursform.cpp \
	interface/addconstraintstudentssetminrestinghoursform.cpp \
	interface/modifyconstraintstudentssetminrestinghoursform.cpp \
	interface/constraintstudentsminrestinghoursform.cpp \
	interface/addconstraintstudentsminrestinghoursform.cpp \
	interface/modifyconstraintstudentsminrestinghoursform.cpp \
	\
	interface/studentscomboboxesstyleform.cpp \
	\
	interface/addconstraintstudentssetmingapsbetweenorderedpairofactivitytagsform.cpp \
	interface/constraintstudentssetmingapsbetweenorderedpairofactivitytagsform.cpp \
	interface/modifyconstraintstudentssetmingapsbetweenorderedpairofactivitytagsform.cpp \
	interface/addconstraintstudentsmingapsbetweenorderedpairofactivitytagsform.cpp \
	interface/constraintstudentsmingapsbetweenorderedpairofactivitytagsform.cpp \
	interface/modifyconstraintstudentsmingapsbetweenorderedpairofactivitytagsform.cpp \
	interface/addconstraintteachermingapsbetweenorderedpairofactivitytagsform.cpp \
	interface/constraintteachermingapsbetweenorderedpairofactivitytagsform.cpp \
	interface/modifyconstraintteachermingapsbetweenorderedpairofactivitytagsform.cpp \
	interface/addconstraintteachersmingapsbetweenorderedpairofactivitytagsform.cpp \
	interface/constraintteachersmingapsbetweenorderedpairofactivitytagsform.cpp \
	interface/modifyconstraintteachersmingapsbetweenorderedpairofactivitytagsform.cpp \
	\
	interface/addconstraintstudentssetmingapsbetweenactivitytagform.cpp \
	interface/constraintstudentssetmingapsbetweenactivitytagform.cpp \
	interface/modifyconstraintstudentssetmingapsbetweenactivitytagform.cpp \
	interface/addconstraintstudentsmingapsbetweenactivitytagform.cpp \
	interface/constraintstudentsmingapsbetweenactivitytagform.cpp \
	interface/modifyconstraintstudentsmingapsbetweenactivitytagform.cpp \
	interface/addconstraintteachermingapsbetweenactivitytagform.cpp \
	interface/constraintteachermingapsbetweenactivitytagform.cpp \
	interface/modifyconstraintteachermingapsbetweenactivitytagform.cpp \
	interface/addconstraintteachersmingapsbetweenactivitytagform.cpp \
	interface/constraintteachersmingapsbetweenactivitytagform.cpp \
	interface/modifyconstraintteachersmingapsbetweenactivitytagform.cpp \
	\
	interface/notificationcommandform.cpp \
	\
	interface/helpmoroccoform.cpp \
	interface/helpalgeriaform.cpp \
	\
	interface/addconstraintstudentssetmaxroomchangesperrealdayform.cpp \
	interface/constraintstudentssetmaxroomchangesperrealdayform.cpp \
	interface/modifyconstraintstudentssetmaxroomchangesperrealdayform.cpp \
	interface/addconstraintstudentsmaxroomchangesperrealdayform.cpp \
	interface/constraintstudentsmaxroomchangesperrealdayform.cpp \
	interface/modifyconstraintstudentsmaxroomchangesperrealdayform.cpp \
	\
	interface/addconstraintteachermaxroomchangesperrealdayform.cpp \
	interface/constraintteachermaxroomchangesperrealdayform.cpp \
	interface/modifyconstraintteachermaxroomchangesperrealdayform.cpp \
	interface/addconstraintteachersmaxroomchangesperrealdayform.cpp \
	interface/constraintteachersmaxroomchangesperrealdayform.cpp \
	interface/modifyconstraintteachersmaxroomchangesperrealdayform.cpp \
	\
	interface/addconstraintstudentssetmaxbuildingchangesperrealdayform.cpp \
	interface/constraintstudentssetmaxbuildingchangesperrealdayform.cpp \
	interface/modifyconstraintstudentssetmaxbuildingchangesperrealdayform.cpp \
	interface/addconstraintstudentsmaxbuildingchangesperrealdayform.cpp \
	interface/constraintstudentsmaxbuildingchangesperrealdayform.cpp \
	interface/modifyconstraintstudentsmaxbuildingchangesperrealdayform.cpp \
	\
	interface/addconstraintteachermaxbuildingchangesperrealdayform.cpp \
	interface/constraintteachermaxbuildingchangesperrealdayform.cpp \
	interface/modifyconstraintteachermaxbuildingchangesperrealdayform.cpp \
	interface/addconstraintteachersmaxbuildingchangesperrealdayform.cpp \
	interface/constraintteachersmaxbuildingchangesperrealdayform.cpp \
	interface/modifyconstraintteachersmaxbuildingchangesperrealdayform.cpp \
	\
	interface/constraintteachermaxafternoonsperweekform.cpp \
	interface/addconstraintteachermaxafternoonsperweekform.cpp \
	interface/modifyconstraintteachermaxafternoonsperweekform.cpp \
	interface/constraintteachersmaxafternoonsperweekform.cpp \
	interface/addconstraintteachersmaxafternoonsperweekform.cpp \
	interface/modifyconstraintteachersmaxafternoonsperweekform.cpp \
	\
	interface/constraintteachermaxmorningsperweekform.cpp \
	interface/addconstraintteachermaxmorningsperweekform.cpp \
	interface/modifyconstraintteachermaxmorningsperweekform.cpp \
	interface/constraintteachersmaxmorningsperweekform.cpp \
	interface/addconstraintteachersmaxmorningsperweekform.cpp \
	interface/modifyconstraintteachersmaxmorningsperweekform.cpp \
	\
	interface/constraintteachermaxtwoconsecutivemorningsform.cpp \
	interface/addconstraintteachermaxtwoconsecutivemorningsform.cpp \
	interface/modifyconstraintteachermaxtwoconsecutivemorningsform.cpp \
	interface/constraintteachersmaxtwoconsecutivemorningsform.cpp \
	interface/addconstraintteachersmaxtwoconsecutivemorningsform.cpp \
	interface/modifyconstraintteachersmaxtwoconsecutivemorningsform.cpp \
	\
	interface/constraintteachermaxtwoconsecutiveafternoonsform.cpp \
	interface/addconstraintteachermaxtwoconsecutiveafternoonsform.cpp \
	interface/modifyconstraintteachermaxtwoconsecutiveafternoonsform.cpp \
	interface/constraintteachersmaxtwoconsecutiveafternoonsform.cpp \
	interface/addconstraintteachersmaxtwoconsecutiveafternoonsform.cpp \
	interface/modifyconstraintteachersmaxtwoconsecutiveafternoonsform.cpp \
	\
	interface/constraintteacherminmorningsperweekform.cpp \
	interface/addconstraintteacherminmorningsperweekform.cpp \
	interface/modifyconstraintteacherminmorningsperweekform.cpp \
	\
	interface/constraintteacherminafternoonsperweekform.cpp \
	interface/addconstraintteacherminafternoonsperweekform.cpp \
	interface/modifyconstraintteacherminafternoonsperweekform.cpp \
	\
	interface/constraintteachersminmorningsperweekform.cpp \
	interface/addconstraintteachersminmorningsperweekform.cpp \
	interface/modifyconstraintteachersminmorningsperweekform.cpp \
	\
	interface/constraintteachersminafternoonsperweekform.cpp \
	interface/addconstraintteachersminafternoonsperweekform.cpp \
	interface/modifyconstraintteachersminafternoonsperweekform.cpp \
	\
	interface/constraintteachermorningintervalmaxdaysperweekform.cpp \
	interface/addconstraintteachermorningintervalmaxdaysperweekform.cpp \
	interface/modifyconstraintteachermorningintervalmaxdaysperweekform.cpp \
	\
	interface/constraintteacherafternoonintervalmaxdaysperweekform.cpp \
	interface/addconstraintteacherafternoonintervalmaxdaysperweekform.cpp \
	interface/modifyconstraintteacherafternoonintervalmaxdaysperweekform.cpp \
	\
	interface/constraintstudentssetmorningintervalmaxdaysperweekform.cpp \
	interface/addconstraintstudentssetmorningintervalmaxdaysperweekform.cpp \
	interface/modifyconstraintstudentssetmorningintervalmaxdaysperweekform.cpp \
	\
	interface/constraintstudentsmorningintervalmaxdaysperweekform.cpp \
	interface/addconstraintstudentsmorningintervalmaxdaysperweekform.cpp \
	interface/modifyconstraintstudentsmorningintervalmaxdaysperweekform.cpp \
	\
	interface/constraintstudentssetafternoonintervalmaxdaysperweekform.cpp \
	interface/addconstraintstudentssetafternoonintervalmaxdaysperweekform.cpp \
	interface/modifyconstraintstudentssetafternoonintervalmaxdaysperweekform.cpp \
	\
	interface/constraintstudentsafternoonintervalmaxdaysperweekform.cpp \
	interface/addconstraintstudentsafternoonintervalmaxdaysperweekform.cpp \
	interface/modifyconstraintstudentsafternoonintervalmaxdaysperweekform.cpp \
	\
	interface/constraintstudentssetmaxafternoonsperweekform.cpp \
	interface/addconstraintstudentssetmaxafternoonsperweekform.cpp \
	interface/modifyconstraintstudentssetmaxafternoonsperweekform.cpp \
	\
	interface/constraintstudentsmaxafternoonsperweekform.cpp \
	interface/addconstraintstudentsmaxafternoonsperweekform.cpp \
	interface/modifyconstraintstudentsmaxafternoonsperweekform.cpp \
	\
	interface/constraintstudentssetmaxmorningsperweekform.cpp \
	interface/addconstraintstudentssetmaxmorningsperweekform.cpp \
	interface/modifyconstraintstudentssetmaxmorningsperweekform.cpp \
	\
	interface/constraintstudentsmaxmorningsperweekform.cpp \
	interface/addconstraintstudentsmaxmorningsperweekform.cpp \
	interface/modifyconstraintstudentsmaxmorningsperweekform.cpp \
	\
	interface/constraintstudentssetminafternoonsperweekform.cpp \
	interface/addconstraintstudentssetminafternoonsperweekform.cpp \
	interface/modifyconstraintstudentssetminafternoonsperweekform.cpp \
	\
	interface/constraintstudentsminafternoonsperweekform.cpp \
	interface/addconstraintstudentsminafternoonsperweekform.cpp \
	interface/modifyconstraintstudentsminafternoonsperweekform.cpp \
	\
	interface/constraintstudentssetminmorningsperweekform.cpp \
	interface/addconstraintstudentssetminmorningsperweekform.cpp \
	interface/modifyconstraintstudentssetminmorningsperweekform.cpp \
	\
	interface/constraintstudentsminmorningsperweekform.cpp \
	interface/addconstraintstudentsminmorningsperweekform.cpp \
	interface/modifyconstraintstudentsminmorningsperweekform.cpp \
	\
	interface/constraintteachersmorningintervalmaxdaysperweekform.cpp \
	interface/addconstraintteachersmorningintervalmaxdaysperweekform.cpp \
	interface/modifyconstraintteachersmorningintervalmaxdaysperweekform.cpp \
	\
	interface/constraintteachersafternoonintervalmaxdaysperweekform.cpp \
	interface/addconstraintteachersafternoonintervalmaxdaysperweekform.cpp \
	interface/modifyconstraintteachersafternoonintervalmaxdaysperweekform.cpp \
	\
	interface/constraintteachermaxhoursperallafternoonsform.cpp \
	interface/addconstraintteachermaxhoursperallafternoonsform.cpp \
	interface/modifyconstraintteachermaxhoursperallafternoonsform.cpp \
	\
	interface/constraintteachersmaxhoursperallafternoonsform.cpp \
	interface/addconstraintteachersmaxhoursperallafternoonsform.cpp \
	interface/modifyconstraintteachersmaxhoursperallafternoonsform.cpp \
	\
	interface/constraintstudentssetmaxhoursperallafternoonsform.cpp \
	interface/addconstraintstudentssetmaxhoursperallafternoonsform.cpp \
	interface/modifyconstraintstudentssetmaxhoursperallafternoonsform.cpp \
	\
	interface/constraintstudentsmaxhoursperallafternoonsform.cpp \
	interface/addconstraintstudentsmaxhoursperallafternoonsform.cpp \
	interface/modifyconstraintstudentsmaxhoursperallafternoonsform.cpp \
	\
	interface/constraintteachersminhourspermorningform.cpp \
	interface/addconstraintteachersminhourspermorningform.cpp \
	interface/modifyconstraintteachersminhourspermorningform.cpp \
	interface/constraintteacherminhourspermorningform.cpp \
	interface/addconstraintteacherminhourspermorningform.cpp \
	interface/modifyconstraintteacherminhourspermorningform.cpp \
	\
	interface/constraintteachersminhoursperafternoonform.cpp \
	interface/addconstraintteachersminhoursperafternoonform.cpp \
	interface/modifyconstraintteachersminhoursperafternoonform.cpp \
	interface/constraintteacherminhoursperafternoonform.cpp \
	interface/addconstraintteacherminhoursperafternoonform.cpp \
	interface/modifyconstraintteacherminhoursperafternoonform.cpp \
	\
	interface/constraintteachersminhoursdailyrealdaysform.cpp \
	interface/addconstraintteachersminhoursdailyrealdaysform.cpp \
	interface/modifyconstraintteachersminhoursdailyrealdaysform.cpp \
	interface/constraintteacherminhoursdailyrealdaysform.cpp \
	interface/addconstraintteacherminhoursdailyrealdaysform.cpp \
	interface/modifyconstraintteacherminhoursdailyrealdaysform.cpp \
	\
	interface/constraintstudentssetmaxgapsperrealdayform.cpp \
	interface/addconstraintstudentssetmaxgapsperrealdayform.cpp \
	interface/modifyconstraintstudentssetmaxgapsperrealdayform.cpp \
	interface/constraintstudentsmaxgapsperrealdayform.cpp \
	interface/addconstraintstudentsmaxgapsperrealdayform.cpp \
	interface/modifyconstraintstudentsmaxgapsperrealdayform.cpp \
	\
	interface/constraintteachersmaxzerogapsperafternoonform.cpp \
	interface/constraintteachermaxzerogapsperafternoonform.cpp \
	interface/addconstraintteachersmaxzerogapsperafternoonform.cpp \
	interface/modifyconstraintteachersmaxzerogapsperafternoonform.cpp \
	interface/addconstraintteachermaxzerogapsperafternoonform.cpp \
	interface/modifyconstraintteachermaxzerogapsperafternoonform.cpp \
	\
	interface/constraintteachersmaxgapsperrealdayform.cpp \
	interface/constraintteachermaxgapsperrealdayform.cpp \
	interface/addconstraintteachersmaxgapsperrealdayform.cpp \
	interface/modifyconstraintteachersmaxgapsperrealdayform.cpp \
	interface/addconstraintteachermaxgapsperrealdayform.cpp \
	interface/modifyconstraintteachermaxgapsperrealdayform.cpp \
	\
	interface/constraintstudentssetmaxgapsperweekforrealdaysform.cpp \
	interface/addconstraintstudentssetmaxgapsperweekforrealdaysform.cpp \
	interface/modifyconstraintstudentssetmaxgapsperweekforrealdaysform.cpp \
	interface/constraintstudentsmaxgapsperweekforrealdaysform.cpp \
	interface/addconstraintstudentsmaxgapsperweekforrealdaysform.cpp \
	interface/modifyconstraintstudentsmaxgapsperweekforrealdaysform.cpp \
	\
	interface/constraintteachersmaxgapsperweekforrealdaysform.cpp \
	interface/constraintteachermaxgapsperweekforrealdaysform.cpp \
	interface/addconstraintteachersmaxgapsperweekforrealdaysform.cpp \
	interface/modifyconstraintteachersmaxgapsperweekforrealdaysform.cpp \
	interface/addconstraintteachermaxgapsperweekforrealdaysform.cpp \
	interface/modifyconstraintteachermaxgapsperweekforrealdaysform.cpp \
	\
	interface/constraintstudentsafternoonsearlymaxbeginningsatsecondhourform.cpp \
	interface/addconstraintstudentsafternoonsearlymaxbeginningsatsecondhourform.cpp \
	interface/modifyconstraintstudentsafternoonsearlymaxbeginningsatsecondhourform.cpp \
	interface/constraintstudentssetafternoonsearlymaxbeginningsatsecondhourform.cpp \
	interface/addconstraintstudentssetafternoonsearlymaxbeginningsatsecondhourform.cpp \
	interface/modifyconstraintstudentssetafternoonsearlymaxbeginningsatsecondhourform.cpp \
	\
	interface/constraintteachersafternoonsearlymaxbeginningsatsecondhourform.cpp \
	interface/addconstraintteachersafternoonsearlymaxbeginningsatsecondhourform.cpp \
	interface/modifyconstraintteachersafternoonsearlymaxbeginningsatsecondhourform.cpp \
	interface/constraintteacherafternoonsearlymaxbeginningsatsecondhourform.cpp \
	interface/addconstraintteacherafternoonsearlymaxbeginningsatsecondhourform.cpp \
	interface/modifyconstraintteacherafternoonsearlymaxbeginningsatsecondhourform.cpp \
	\
	interface/constraintstudentsmorningsearlymaxbeginningsatsecondhourform.cpp \
	interface/addconstraintstudentsmorningsearlymaxbeginningsatsecondhourform.cpp \
	interface/modifyconstraintstudentsmorningsearlymaxbeginningsatsecondhourform.cpp \
	interface/constraintstudentssetmorningsearlymaxbeginningsatsecondhourform.cpp \
	interface/addconstraintstudentssetmorningsearlymaxbeginningsatsecondhourform.cpp \
	interface/modifyconstraintstudentssetmorningsearlymaxbeginningsatsecondhourform.cpp \
	\
	interface/constraintteachersmorningsearlymaxbeginningsatsecondhourform.cpp \
	interface/addconstraintteachersmorningsearlymaxbeginningsatsecondhourform.cpp \
	interface/modifyconstraintteachersmorningsearlymaxbeginningsatsecondhourform.cpp \
	interface/constraintteachermorningsearlymaxbeginningsatsecondhourform.cpp \
	interface/addconstraintteachermorningsearlymaxbeginningsatsecondhourform.cpp \
	interface/modifyconstraintteachermorningsearlymaxbeginningsatsecondhourform.cpp \
	\
	interface/constraintstudentssetminhourspermorningform.cpp \
	interface/addconstraintstudentssetminhourspermorningform.cpp \
	interface/modifyconstraintstudentssetminhourspermorningform.cpp \
	interface/constraintstudentsminhourspermorningform.cpp \
	interface/addconstraintstudentsminhourspermorningform.cpp \
	interface/modifyconstraintstudentsminhourspermorningform.cpp \
	\
	interface/constraintstudentssetminhoursperafternoonform.cpp \
	interface/addconstraintstudentssetminhoursperafternoonform.cpp \
	interface/modifyconstraintstudentssetminhoursperafternoonform.cpp \
	interface/constraintstudentsminhoursperafternoonform.cpp \
	interface/addconstraintstudentsminhoursperafternoonform.cpp \
	interface/modifyconstraintstudentsminhoursperafternoonform.cpp \
	\
	interface/constraintteachermaxtwoactivitytagsperdayfromn1n2n3form.cpp \
	interface/addconstraintteachermaxtwoactivitytagsperdayfromn1n2n3form.cpp \
	interface/modifyconstraintteachermaxtwoactivitytagsperdayfromn1n2n3form.cpp \
	interface/constraintteachersmaxtwoactivitytagsperdayfromn1n2n3form.cpp \
	interface/addconstraintteachersmaxtwoactivitytagsperdayfromn1n2n3form.cpp \
	interface/modifyconstraintteachersmaxtwoactivitytagsperdayfromn1n2n3form.cpp \
	\
	interface/constraintstudentssetmaxtwoactivitytagsperdayfromn1n2n3form.cpp \
	interface/addconstraintstudentssetmaxtwoactivitytagsperdayfromn1n2n3form.cpp \
	interface/modifyconstraintstudentssetmaxtwoactivitytagsperdayfromn1n2n3form.cpp \
	interface/constraintstudentsmaxtwoactivitytagsperdayfromn1n2n3form.cpp \
	interface/addconstraintstudentsmaxtwoactivitytagsperdayfromn1n2n3form.cpp \
	interface/modifyconstraintstudentsmaxtwoactivitytagsperdayfromn1n2n3form.cpp \
	\
	interface/constraintteachermaxtwoactivitytagsperrealdayfromn1n2n3form.cpp \
	interface/addconstraintteachermaxtwoactivitytagsperrealdayfromn1n2n3form.cpp \
	interface/modifyconstraintteachermaxtwoactivitytagsperrealdayfromn1n2n3form.cpp \
	interface/constraintteachersmaxtwoactivitytagsperrealdayfromn1n2n3form.cpp \
	interface/addconstraintteachersmaxtwoactivitytagsperrealdayfromn1n2n3form.cpp \
	interface/modifyconstraintteachersmaxtwoactivitytagsperrealdayfromn1n2n3form.cpp \
	\
	interface/constraintstudentssetmaxtwoactivitytagsperrealdayfromn1n2n3form.cpp \
	interface/addconstraintstudentssetmaxtwoactivitytagsperrealdayfromn1n2n3form.cpp \
	interface/modifyconstraintstudentssetmaxtwoactivitytagsperrealdayfromn1n2n3form.cpp \
	interface/constraintstudentsmaxtwoactivitytagsperrealdayfromn1n2n3form.cpp \
	interface/addconstraintstudentsmaxtwoactivitytagsperrealdayfromn1n2n3form.cpp \
	interface/modifyconstraintstudentsmaxtwoactivitytagsperrealdayfromn1n2n3form.cpp \
	\
	interface/constraintteacherminrestinghoursbetweenmorningandafternoonform.cpp \
	interface/addconstraintteacherminrestinghoursbetweenmorningandafternoonform.cpp \
	interface/modifyconstraintteacherminrestinghoursbetweenmorningandafternoonform.cpp \
	interface/constraintteachersminrestinghoursbetweenmorningandafternoonform.cpp \
	interface/addconstraintteachersminrestinghoursbetweenmorningandafternoonform.cpp \
	interface/modifyconstraintteachersminrestinghoursbetweenmorningandafternoonform.cpp \
	\
	interface/constraintstudentssetminrestinghoursbetweenmorningandafternoonform.cpp \
	interface/addconstraintstudentssetminrestinghoursbetweenmorningandafternoonform.cpp \
	interface/modifyconstraintstudentssetminrestinghoursbetweenmorningandafternoonform.cpp \
	interface/constraintstudentsminrestinghoursbetweenmorningandafternoonform.cpp \
	interface/addconstraintstudentsminrestinghoursbetweenmorningandafternoonform.cpp \
	interface/modifyconstraintstudentsminrestinghoursbetweenmorningandafternoonform.cpp \
	\
	interface/helponn1n2n3.cpp \
	\
	interface/constraintteachersmaxhoursdailyrealdaysform.cpp \
	interface/addconstraintteachersmaxhoursdailyrealdaysform.cpp \
	interface/modifyconstraintteachersmaxhoursdailyrealdaysform.cpp \
	interface/constraintteachermaxhoursdailyrealdaysform.cpp \
	interface/addconstraintteachermaxhoursdailyrealdaysform.cpp \
	interface/modifyconstraintteachermaxhoursdailyrealdaysform.cpp \
	\
	interface/constraintteachersactivitytagmaxhoursdailyrealdaysform.cpp \
	interface/addconstraintteachersactivitytagmaxhoursdailyrealdaysform.cpp \
	interface/modifyconstraintteachersactivitytagmaxhoursdailyrealdaysform.cpp \
	interface/constraintteacheractivitytagmaxhoursdailyrealdaysform.cpp \
	interface/addconstraintteacheractivitytagmaxhoursdailyrealdaysform.cpp \
	interface/modifyconstraintteacheractivitytagmaxhoursdailyrealdaysform.cpp \
	\
	interface/constraintstudentssetmaxhoursdailyrealdaysform.cpp \
	interface/addconstraintstudentssetmaxhoursdailyrealdaysform.cpp \
	interface/modifyconstraintstudentssetmaxhoursdailyrealdaysform.cpp \
	interface/constraintstudentsmaxhoursdailyrealdaysform.cpp \
	interface/addconstraintstudentsmaxhoursdailyrealdaysform.cpp \
	interface/modifyconstraintstudentsmaxhoursdailyrealdaysform.cpp \
	\
	interface/constraintstudentssetactivitytagmaxhoursdailyrealdaysform.cpp \
	interface/addconstraintstudentssetactivitytagmaxhoursdailyrealdaysform.cpp \
	interface/modifyconstraintstudentssetactivitytagmaxhoursdailyrealdaysform.cpp \
	interface/constraintstudentsactivitytagmaxhoursdailyrealdaysform.cpp \
	interface/addconstraintstudentsactivitytagmaxhoursdailyrealdaysform.cpp \
	interface/modifyconstraintstudentsactivitytagmaxhoursdailyrealdaysform.cpp \
	\
	interface/constraintteachermaxrealdaysperweekform.cpp \
	interface/addconstraintteachermaxrealdaysperweekform.cpp \
	interface/modifyconstraintteachermaxrealdaysperweekform.cpp \
	\
	interface/constraintteachersmaxrealdaysperweekform.cpp \
	interface/addconstraintteachersmaxrealdaysperweekform.cpp \
	interface/modifyconstraintteachersmaxrealdaysperweekform.cpp \
	\
	interface/constraintstudentssetmaxrealdaysperweekform.cpp \
	interface/addconstraintstudentssetmaxrealdaysperweekform.cpp \
	interface/modifyconstraintstudentssetmaxrealdaysperweekform.cpp \
	\
	interface/constraintstudentsmaxrealdaysperweekform.cpp \
	interface/addconstraintstudentsmaxrealdaysperweekform.cpp \
	interface/modifyconstraintstudentsmaxrealdaysperweekform.cpp \
	\
	interface/constraintteacherminrealdaysperweekform.cpp \
	interface/addconstraintteacherminrealdaysperweekform.cpp \
	interface/modifyconstraintteacherminrealdaysperweekform.cpp \
	\
	interface/constraintteachersminrealdaysperweekform.cpp \
	interface/addconstraintteachersminrealdaysperweekform.cpp \
	interface/modifyconstraintteachersminrealdaysperweekform.cpp \
	\
	interface/constraintteachermaxspanperrealdayform.cpp \
	interface/addconstraintteachermaxspanperrealdayform.cpp \
	interface/modifyconstraintteachermaxspanperrealdayform.cpp \
	interface/constraintteachersmaxspanperrealdayform.cpp \
	interface/addconstraintteachersmaxspanperrealdayform.cpp \
	interface/modifyconstraintteachersmaxspanperrealdayform.cpp \
	\
	interface/constraintstudentssetmaxspanperrealdayform.cpp \
	interface/addconstraintstudentssetmaxspanperrealdayform.cpp \
	interface/modifyconstraintstudentssetmaxspanperrealdayform.cpp \
	interface/constraintstudentsmaxspanperrealdayform.cpp \
	interface/addconstraintstudentsmaxspanperrealdayform.cpp \
	interface/modifyconstraintstudentsmaxspanperrealdayform.cpp \
	\
	interface/constraintmaxtotalactivitiesfromsetinselectedtimeslotsform.cpp \
	interface/addconstraintmaxtotalactivitiesfromsetinselectedtimeslotsform.cpp \
	interface/modifyconstraintmaxtotalactivitiesfromsetinselectedtimeslotsform.cpp \
	\
	interface/constraintmaxgapsbetweenactivitiesform.cpp \
	interface/addconstraintmaxgapsbetweenactivitiesform.cpp \
	interface/modifyconstraintmaxgapsbetweenactivitiesform.cpp \
	\
	interface/constraintactivitiesmaxinatermform.cpp \
	interface/addconstraintactivitiesmaxinatermform.cpp \
	interface/modifyconstraintactivitiesmaxinatermform.cpp \
	\
	interface/constraintactivitiesmininatermform.cpp \
	interface/addconstraintactivitiesmininatermform.cpp \
	interface/modifyconstraintactivitiesmininatermform.cpp \
	\
	interface/constraintactivitiesoccupymaxtermsform.cpp \
	interface/addconstraintactivitiesoccupymaxtermsform.cpp \
	interface/modifyconstraintactivitiesoccupymaxtermsform.cpp \
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
	interface/settingsautosaveform.cpp

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
	engine/messageboxes.h \
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
	interface/addconstraintactivitypreferredtimeslotsform.h \
	interface/modifyconstraintactivitypreferredtimeslotsform.h \
	interface/addconstraintactivitypreferredstartingtimesform.h \
	interface/modifyconstraintactivitypreferredstartingtimesform.h \
	interface/addconstraintroomnotavailabletimesform.h \
	interface/constraintroomnotavailabletimesform.h \
	interface/modifyconstraintroomnotavailabletimesform.h \
	interface/addconstraintteacherroomnotavailabletimesform.h \
	interface/constraintteacherroomnotavailabletimesform.h \
	interface/modifyconstraintteacherroomnotavailabletimesform.h \
	interface/addconstraintactivitypreferredroomform.h \
	interface/constraintactivitypreferredroomform.h \
	interface/modifyconstraintactivitypreferredroomform.h \
	interface/addconstraintsubjectpreferredroomform.h \
	interface/constraintsubjectpreferredroomform.h \
	interface/modifyconstraintsubjectpreferredroomform.h \
	interface/addconstraintsubjectactivitytagpreferredroomform.h \
	interface/constraintsubjectactivitytagpreferredroomform.h \
	interface/modifyconstraintsubjectactivitytagpreferredroomform.h \
	\
	interface/addconstraintactivitytagpreferredroomform.h \
	interface/constraintactivitytagpreferredroomform.h \
	interface/modifyconstraintactivitytagpreferredroomform.h \
	\
	interface/addconstraintsubjectpreferredroomsform.h \
	interface/constraintsubjectpreferredroomsform.h \
	interface/modifyconstraintsubjectpreferredroomsform.h \
	interface/addconstraintsubjectactivitytagpreferredroomsform.h \
	interface/constraintsubjectactivitytagpreferredroomsform.h \
	interface/modifyconstraintsubjectactivitytagpreferredroomsform.h \
	\
	interface/addconstraintactivitytagpreferredroomsform.h \
	interface/constraintactivitytagpreferredroomsform.h \
	interface/modifyconstraintactivitytagpreferredroomsform.h \
	\
	interface/addconstraintstudentssethomeroomform.h \
	interface/constraintstudentssethomeroomform.h \
	interface/modifyconstraintstudentssethomeroomform.h \
	interface/addconstraintstudentssethomeroomsform.h \
	interface/constraintstudentssethomeroomsform.h \
	interface/modifyconstraintstudentssethomeroomsform.h \
	\
	interface/addconstraintstudentssetmaxbuildingchangesperdayform.h \
	interface/constraintstudentssetmaxbuildingchangesperdayform.h \
	interface/modifyconstraintstudentssetmaxbuildingchangesperdayform.h \
	interface/addconstraintstudentsmaxbuildingchangesperdayform.h \
	interface/constraintstudentsmaxbuildingchangesperdayform.h \
	interface/modifyconstraintstudentsmaxbuildingchangesperdayform.h \
	interface/addconstraintstudentssetmaxbuildingchangesperweekform.h \
	interface/constraintstudentssetmaxbuildingchangesperweekform.h \
	interface/modifyconstraintstudentssetmaxbuildingchangesperweekform.h \
	interface/addconstraintstudentsmaxbuildingchangesperweekform.h \
	interface/constraintstudentsmaxbuildingchangesperweekform.h \
	interface/modifyconstraintstudentsmaxbuildingchangesperweekform.h \
	interface/addconstraintstudentssetmingapsbetweenbuildingchangesform.h \
	interface/constraintstudentssetmingapsbetweenbuildingchangesform.h \
	interface/modifyconstraintstudentssetmingapsbetweenbuildingchangesform.h \
	interface/addconstraintstudentsmingapsbetweenbuildingchangesform.h \
	interface/constraintstudentsmingapsbetweenbuildingchangesform.h \
	interface/modifyconstraintstudentsmingapsbetweenbuildingchangesform.h \
	\
	interface/addconstraintteachermaxbuildingchangesperdayform.h \
	interface/constraintteachermaxbuildingchangesperdayform.h \
	interface/modifyconstraintteachermaxbuildingchangesperdayform.h \
	interface/addconstraintteachersmaxbuildingchangesperdayform.h \
	interface/constraintteachersmaxbuildingchangesperdayform.h \
	interface/modifyconstraintteachersmaxbuildingchangesperdayform.h \
	interface/addconstraintteachermaxbuildingchangesperweekform.h \
	interface/constraintteachermaxbuildingchangesperweekform.h \
	interface/modifyconstraintteachermaxbuildingchangesperweekform.h \
	interface/addconstraintteachersmaxbuildingchangesperweekform.h \
	interface/constraintteachersmaxbuildingchangesperweekform.h \
	interface/modifyconstraintteachersmaxbuildingchangesperweekform.h \
	interface/addconstraintteachermingapsbetweenbuildingchangesform.h \
	interface/constraintteachermingapsbetweenbuildingchangesform.h \
	interface/modifyconstraintteachermingapsbetweenbuildingchangesform.h \
	interface/addconstraintteachersmingapsbetweenbuildingchangesform.h \
	interface/constraintteachersmingapsbetweenbuildingchangesform.h \
	interface/modifyconstraintteachersmingapsbetweenbuildingchangesform.h \
	\
	interface/addconstraintstudentssetmaxroomchangesperdayform.h \
	interface/constraintstudentssetmaxroomchangesperdayform.h \
	interface/modifyconstraintstudentssetmaxroomchangesperdayform.h \
	interface/addconstraintstudentsmaxroomchangesperdayform.h \
	interface/constraintstudentsmaxroomchangesperdayform.h \
	interface/modifyconstraintstudentsmaxroomchangesperdayform.h \
	interface/addconstraintstudentssetmaxroomchangesperweekform.h \
	interface/constraintstudentssetmaxroomchangesperweekform.h \
	interface/modifyconstraintstudentssetmaxroomchangesperweekform.h \
	interface/addconstraintstudentsmaxroomchangesperweekform.h \
	interface/constraintstudentsmaxroomchangesperweekform.h \
	interface/modifyconstraintstudentsmaxroomchangesperweekform.h \
	interface/addconstraintstudentssetmingapsbetweenroomchangesform.h \
	interface/constraintstudentssetmingapsbetweenroomchangesform.h \
	interface/modifyconstraintstudentssetmingapsbetweenroomchangesform.h \
	interface/addconstraintstudentsmingapsbetweenroomchangesform.h \
	interface/constraintstudentsmingapsbetweenroomchangesform.h \
	interface/modifyconstraintstudentsmingapsbetweenroomchangesform.h \
	\
	interface/addconstraintteachermaxroomchangesperdayform.h \
	interface/constraintteachermaxroomchangesperdayform.h \
	interface/modifyconstraintteachermaxroomchangesperdayform.h \
	interface/addconstraintteachersmaxroomchangesperdayform.h \
	interface/constraintteachersmaxroomchangesperdayform.h \
	interface/modifyconstraintteachersmaxroomchangesperdayform.h \
	interface/addconstraintteachermaxroomchangesperweekform.h \
	interface/constraintteachermaxroomchangesperweekform.h \
	interface/modifyconstraintteachermaxroomchangesperweekform.h \
	interface/addconstraintteachersmaxroomchangesperweekform.h \
	interface/constraintteachersmaxroomchangesperweekform.h \
	interface/modifyconstraintteachersmaxroomchangesperweekform.h \
	interface/addconstraintteachermingapsbetweenroomchangesform.h \
	interface/constraintteachermingapsbetweenroomchangesform.h \
	interface/modifyconstraintteachermingapsbetweenroomchangesform.h \
	interface/addconstraintteachersmingapsbetweenroomchangesform.h \
	interface/constraintteachersmingapsbetweenroomchangesform.h \
	interface/modifyconstraintteachersmingapsbetweenroomchangesform.h \
	\
	interface/addconstraintteacherhomeroomform.h \
	interface/constraintteacherhomeroomform.h \
	interface/modifyconstraintteacherhomeroomform.h \
	interface/addconstraintteacherhomeroomsform.h \
	interface/constraintteacherhomeroomsform.h \
	interface/modifyconstraintteacherhomeroomsform.h \
	interface/addconstraintactivitypreferredroomsform.h \
	interface/constraintactivitypreferredroomsform.h \
	interface/modifyconstraintactivitypreferredroomsform.h \
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
	interface/centerwidgetonscreen.h \
	interface/helponimperfectconstraints.h \
	interface/helponstudentsminhoursdaily.h \
	interface/settingstimetablehtmllevelform.h \
	interface/timetableviewstudentsdayshorizontalform.h \
	interface/timetableviewstudentstimehorizontalform.h \
	interface/timetableviewroomsdayshorizontalform.h \
	interface/timetableviewroomstimehorizontalform.h \
	interface/yearsform.h \
	interface/timetableshowconflictsform.h \
	interface/timetableviewteachersdayshorizontalform.h \
	interface/timetableviewteacherstimehorizontalform.h \
	interface/hoursform.h \
	interface/daysform.h \
	interface/roomsform.h \
	interface/randomseedform.h \
	interface/getmodefornewfileform.h \
	interface/roommakeeditvirtualform.h \
	interface/buildingsform.h \
	\
	interface/constraintactivitiespreferredtimeslotsform.h \
	interface/addconstraintactivitiespreferredtimeslotsform.h \
	interface/constraintactivitiespreferredstartingtimesform.h \
	interface/addconstraintactivitiespreferredstartingtimesform.h \
	interface/constraintactivitiessamestartingtimeform.h \
	interface/addconstraintactivitiessamestartingtimeform.h \
	\
	interface/constraintsubactivitiespreferredtimeslotsform.h \
	interface/addconstraintsubactivitiespreferredtimeslotsform.h \
	interface/modifyconstraintsubactivitiespreferredtimeslotsform.h \
	interface/constraintsubactivitiespreferredstartingtimesform.h \
	interface/addconstraintsubactivitiespreferredstartingtimesform.h \
	interface/modifyconstraintsubactivitiespreferredstartingtimesform.h \
	\
	interface/constraintactivitiessamestartinghourform.h \
	interface/addconstraintactivitiessamestartinghourform.h \
	\
	interface/constraintactivitiessamestartingdayform.h \
	interface/addconstraintactivitiessamestartingdayform.h \
	interface/modifyconstraintactivitiessamestartingdayform.h \
	\
	interface/constraintteachernotavailabletimesform.h \
	interface/addconstraintteachernotavailabletimesform.h \
	interface/modifyconstraintteachernotavailabletimesform.h \
	interface/constraintbasiccompulsorytimeform.h \
	interface/addconstraintbasiccompulsorytimeform.h \
	interface/modifyconstraintbasiccompulsorytimeform.h \
	interface/constraintbasiccompulsoryspaceform.h \
	interface/addconstraintbasiccompulsoryspaceform.h \
	interface/modifyconstraintbasiccompulsoryspaceform.h \
	interface/constraintstudentssetnotavailabletimesform.h \
	interface/addconstraintstudentssetnotavailabletimesform.h \
	interface/modifyconstraintstudentssetnotavailabletimesform.h \
	interface/constraintbreaktimesform.h \
	interface/addconstraintbreaktimesform.h \
	interface/modifyconstraintbreaktimesform.h \
	interface/constraintteachermaxdaysperweekform.h \
	interface/addconstraintteachermaxdaysperweekform.h \
	interface/modifyconstraintteachermaxdaysperweekform.h \
	\
	interface/constraintteachermaxthreeconsecutivedaysform.h \
	interface/addconstraintteachermaxthreeconsecutivedaysform.h \
	interface/modifyconstraintteachermaxthreeconsecutivedaysform.h \
	interface/constraintteachersmaxthreeconsecutivedaysform.h \
	interface/addconstraintteachersmaxthreeconsecutivedaysform.h \
	interface/modifyconstraintteachersmaxthreeconsecutivedaysform.h \
	\
	interface/constraintstudentssetmaxthreeconsecutivedaysform.h \
	interface/addconstraintstudentssetmaxthreeconsecutivedaysform.h \
	interface/modifyconstraintstudentssetmaxthreeconsecutivedaysform.h \
	interface/constraintstudentsmaxthreeconsecutivedaysform.h \
	interface/addconstraintstudentsmaxthreeconsecutivedaysform.h \
	interface/modifyconstraintstudentsmaxthreeconsecutivedaysform.h \
	\
	interface/constraintteachermindaysperweekform.h \
	interface/addconstraintteachermindaysperweekform.h \
	interface/modifyconstraintteachermindaysperweekform.h \
	\
	interface/constraintteachersmaxdaysperweekform.h \
	interface/addconstraintteachersmaxdaysperweekform.h \
	interface/modifyconstraintteachersmaxdaysperweekform.h \
	\
	interface/constraintteachersmindaysperweekform.h \
	interface/addconstraintteachersmindaysperweekform.h \
	interface/modifyconstraintteachersmindaysperweekform.h \
	\
	interface/constraintteacherintervalmaxdaysperweekform.h \
	interface/addconstraintteacherintervalmaxdaysperweekform.h \
	interface/modifyconstraintteacherintervalmaxdaysperweekform.h \
	\
	interface/constraintstudentssetintervalmaxdaysperweekform.h \
	interface/addconstraintstudentssetintervalmaxdaysperweekform.h \
	interface/modifyconstraintstudentssetintervalmaxdaysperweekform.h \
	\
	interface/constraintstudentsintervalmaxdaysperweekform.h \
	interface/addconstraintstudentsintervalmaxdaysperweekform.h \
	interface/modifyconstraintstudentsintervalmaxdaysperweekform.h \
	\
	interface/constraintstudentssetmaxdaysperweekform.h \
	interface/addconstraintstudentssetmaxdaysperweekform.h \
	interface/modifyconstraintstudentssetmaxdaysperweekform.h \
	\
	interface/constraintstudentsmaxdaysperweekform.h \
	interface/addconstraintstudentsmaxdaysperweekform.h \
	interface/modifyconstraintstudentsmaxdaysperweekform.h \
	\
	interface/constraintteachersintervalmaxdaysperweekform.h \
	interface/addconstraintteachersintervalmaxdaysperweekform.h \
	interface/modifyconstraintteachersintervalmaxdaysperweekform.h \
	\
	interface/constraintteachersmaxhoursdailyform.h \
	interface/addconstraintteachersmaxhoursdailyform.h \
	interface/modifyconstraintteachersmaxhoursdailyform.h \
	interface/constraintteachermaxhoursdailyform.h \
	interface/addconstraintteachermaxhoursdailyform.h \
	interface/modifyconstraintteachermaxhoursdailyform.h \
	\
	interface/constraintteachersmaxhourscontinuouslyform.h \
	interface/addconstraintteachersmaxhourscontinuouslyform.h \
	interface/modifyconstraintteachersmaxhourscontinuouslyform.h \
	interface/constraintteachermaxhourscontinuouslyform.h \
	interface/addconstraintteachermaxhourscontinuouslyform.h \
	interface/modifyconstraintteachermaxhourscontinuouslyform.h \
	\
	interface/constraintteachersactivitytagmaxhourscontinuouslyform.h \
	interface/addconstraintteachersactivitytagmaxhourscontinuouslyform.h \
	interface/modifyconstraintteachersactivitytagmaxhourscontinuouslyform.h \
	interface/constraintteacheractivitytagmaxhourscontinuouslyform.h \
	interface/addconstraintteacheractivitytagmaxhourscontinuouslyform.h \
	interface/modifyconstraintteacheractivitytagmaxhourscontinuouslyform.h \
	\
	interface/constraintteachersactivitytagmaxhoursdailyform.h \
	interface/addconstraintteachersactivitytagmaxhoursdailyform.h \
	interface/modifyconstraintteachersactivitytagmaxhoursdailyform.h \
	interface/constraintteacheractivitytagmaxhoursdailyform.h \
	interface/addconstraintteacheractivitytagmaxhoursdailyform.h \
	interface/modifyconstraintteacheractivitytagmaxhoursdailyform.h \
	\
	interface/constraintteachersactivitytagminhoursdailyform.h \
	interface/addconstraintteachersactivitytagminhoursdailyform.h \
	interface/modifyconstraintteachersactivitytagminhoursdailyform.h \
	interface/constraintteacheractivitytagminhoursdailyform.h \
	interface/addconstraintteacheractivitytagminhoursdailyform.h \
	interface/modifyconstraintteacheractivitytagminhoursdailyform.h \
	\
	interface/constraintteachersminhoursdailyform.h \
	interface/addconstraintteachersminhoursdailyform.h \
	interface/modifyconstraintteachersminhoursdailyform.h \
	interface/constraintteacherminhoursdailyform.h \
	interface/addconstraintteacherminhoursdailyform.h \
	interface/modifyconstraintteacherminhoursdailyform.h \
	interface/constraintactivitypreferredstartingtimeform.h \
	interface/addconstraintactivitypreferredstartingtimeform.h \
	interface/modifyconstraintactivitypreferredstartingtimeform.h \
	\
	interface/constraintactivityendsstudentsdayform.h \
	interface/addconstraintactivityendsstudentsdayform.h \
	interface/modifyconstraintactivityendsstudentsdayform.h \
	\
	interface/constraintactivitiesendstudentsdayform.h \
	interface/addconstraintactivitiesendstudentsdayform.h \
	interface/modifyconstraintactivitiesendstudentsdayform.h \
	\
	interface/constraintactivityendsteachersdayform.h \
	interface/addconstraintactivityendsteachersdayform.h \
	interface/modifyconstraintactivityendsteachersdayform.h \
	\
	interface/constraintactivitiesendteachersdayform.h \
	interface/addconstraintactivitiesendteachersdayform.h \
	interface/modifyconstraintactivitiesendteachersdayform.h \
	\
	interface/constraintactivitybeginsstudentsdayform.h \
	interface/addconstraintactivitybeginsstudentsdayform.h \
	interface/modifyconstraintactivitybeginsstudentsdayform.h \
	\
	interface/constraintactivitiesbeginstudentsdayform.h \
	interface/addconstraintactivitiesbeginstudentsdayform.h \
	interface/modifyconstraintactivitiesbeginstudentsdayform.h \
	\
	interface/constraintactivitybeginsteachersdayform.h \
	interface/addconstraintactivitybeginsteachersdayform.h \
	interface/modifyconstraintactivitybeginsteachersdayform.h \
	\
	interface/constraintactivitiesbeginteachersdayform.h \
	interface/addconstraintactivitiesbeginteachersdayform.h \
	interface/modifyconstraintactivitiesbeginteachersdayform.h \
	\
	interface/constraintstudentssetmaxgapsperweekform.h \
	interface/addconstraintstudentssetmaxgapsperweekform.h \
	interface/modifyconstraintstudentssetmaxgapsperweekform.h \
	interface/constraintstudentsmaxgapsperweekform.h \
	interface/addconstraintstudentsmaxgapsperweekform.h \
	interface/modifyconstraintstudentsmaxgapsperweekform.h \
	\
	interface/constraintstudentssetmaxgapsperdayform.h \
	interface/addconstraintstudentssetmaxgapsperdayform.h \
	interface/modifyconstraintstudentssetmaxgapsperdayform.h \
	interface/constraintstudentsmaxgapsperdayform.h \
	interface/addconstraintstudentsmaxgapsperdayform.h \
	interface/modifyconstraintstudentsmaxgapsperdayform.h \
	\
	interface/constraintteachersmaxgapsperweekform.h \
	interface/addconstraintteachersmaxgapsperweekform.h \
	interface/modifyconstraintteachersmaxgapsperweekform.h \
	interface/constraintteachermaxgapsperweekform.h \
	interface/addconstraintteachermaxgapsperweekform.h \
	interface/modifyconstraintteachermaxgapsperweekform.h \
	interface/constraintteachersmaxgapsperdayform.h \
	interface/addconstraintteachersmaxgapsperdayform.h \
	interface/modifyconstraintteachersmaxgapsperdayform.h \
	interface/constraintteachermaxgapsperdayform.h \
	interface/addconstraintteachermaxgapsperdayform.h \
	interface/modifyconstraintteachermaxgapsperdayform.h \
	\
	interface/constraintteachersmaxgapspermorningandafternoonform.h \
	interface/constraintteachermaxgapspermorningandafternoonform.h \
	interface/addconstraintteachersmaxgapspermorningandafternoonform.h \
	interface/modifyconstraintteachersmaxgapspermorningandafternoonform.h \
	interface/addconstraintteachermaxgapspermorningandafternoonform.h \
	interface/modifyconstraintteachermaxgapspermorningandafternoonform.h \
	\
	interface/constraintstudentsearlymaxbeginningsatsecondhourform.h \
	interface/addconstraintstudentsearlymaxbeginningsatsecondhourform.h \
	interface/modifyconstraintstudentsearlymaxbeginningsatsecondhourform.h \
	interface/constraintstudentssetearlymaxbeginningsatsecondhourform.h \
	interface/addconstraintstudentssetearlymaxbeginningsatsecondhourform.h \
	interface/modifyconstraintstudentssetearlymaxbeginningsatsecondhourform.h \
	\
	interface/constraintstudentssetmaxhoursdailyform.h \
	interface/addconstraintstudentssetmaxhoursdailyform.h \
	interface/modifyconstraintstudentssetmaxhoursdailyform.h \
	interface/constraintstudentsmaxhoursdailyform.h \
	interface/addconstraintstudentsmaxhoursdailyform.h \
	interface/modifyconstraintstudentsmaxhoursdailyform.h \
	\
	interface/constraintstudentssetmaxhourscontinuouslyform.h \
	interface/addconstraintstudentssetmaxhourscontinuouslyform.h \
	interface/modifyconstraintstudentssetmaxhourscontinuouslyform.h \
	interface/constraintstudentsmaxhourscontinuouslyform.h \
	interface/addconstraintstudentsmaxhourscontinuouslyform.h \
	interface/modifyconstraintstudentsmaxhourscontinuouslyform.h \
	\
	interface/constraintstudentssetactivitytagmaxhourscontinuouslyform.h \
	interface/addconstraintstudentssetactivitytagmaxhourscontinuouslyform.h \
	interface/modifyconstraintstudentssetactivitytagmaxhourscontinuouslyform.h \
	interface/constraintstudentsactivitytagmaxhourscontinuouslyform.h \
	interface/addconstraintstudentsactivitytagmaxhourscontinuouslyform.h \
	interface/modifyconstraintstudentsactivitytagmaxhourscontinuouslyform.h \
	\
	interface/constraintstudentssetactivitytagmaxhoursdailyform.h \
	interface/addconstraintstudentssetactivitytagmaxhoursdailyform.h \
	interface/modifyconstraintstudentssetactivitytagmaxhoursdailyform.h \
	interface/constraintstudentsactivitytagmaxhoursdailyform.h \
	interface/addconstraintstudentsactivitytagmaxhoursdailyform.h \
	interface/modifyconstraintstudentsactivitytagmaxhoursdailyform.h \
	\
	interface/constraintstudentssetactivitytagminhoursdailyform.h \
	interface/addconstraintstudentssetactivitytagminhoursdailyform.h \
	interface/modifyconstraintstudentssetactivitytagminhoursdailyform.h \
	interface/constraintstudentsactivitytagminhoursdailyform.h \
	interface/addconstraintstudentsactivitytagminhoursdailyform.h \
	interface/modifyconstraintstudentsactivitytagminhoursdailyform.h \
	\
	interface/constraintstudentssetminhoursdailyform.h \
	interface/addconstraintstudentssetminhoursdailyform.h \
	interface/modifyconstraintstudentssetminhoursdailyform.h \
	interface/constraintstudentsminhoursdailyform.h \
	interface/addconstraintstudentsminhoursdailyform.h \
	interface/modifyconstraintstudentsminhoursdailyform.h \
	interface/constraintactivitiesnotoverlappingform.h \
	interface/addconstraintactivitiesnotoverlappingform.h \
	interface/modifyconstraintactivitiesnotoverlappingform.h \
	\
	interface/constraintactivitytagsnotoverlappingform.h \
	interface/addconstraintactivitytagsnotoverlappingform.h \
	interface/modifyconstraintactivitytagsnotoverlappingform.h \
	\
	interface/constraintactivitiesoccupymaxtimeslotsfromselectionform.h \
	interface/addconstraintactivitiesoccupymaxtimeslotsfromselectionform.h \
	interface/modifyconstraintactivitiesoccupymaxtimeslotsfromselectionform.h \
	\
	interface/constraintactivitiesoccupymintimeslotsfromselectionform.h \
	interface/addconstraintactivitiesoccupymintimeslotsfromselectionform.h \
	interface/modifyconstraintactivitiesoccupymintimeslotsfromselectionform.h \
	\
	interface/constraintactivitiesmaxsimultaneousinselectedtimeslotsform.h \
	interface/addconstraintactivitiesmaxsimultaneousinselectedtimeslotsform.h \
	interface/modifyconstraintactivitiesmaxsimultaneousinselectedtimeslotsform.h \
	\
	interface/constraintactivitiesminsimultaneousinselectedtimeslotsform.h \
	interface/addconstraintactivitiesminsimultaneousinselectedtimeslotsform.h \
	interface/modifyconstraintactivitiesminsimultaneousinselectedtimeslotsform.h \
	\
	interface/constraintactivitiesoccupymaxdifferentroomsform.h \
	interface/addconstraintactivitiesoccupymaxdifferentroomsform.h \
	interface/modifyconstraintactivitiesoccupymaxdifferentroomsform.h \
	\
	interface/constraintactivitiessameroomifconsecutiveform.h \
	interface/addconstraintactivitiessameroomifconsecutiveform.h \
	interface/modifyconstraintactivitiessameroomifconsecutiveform.h \
	\
	interface/constrainttwoactivitiesconsecutiveform.h \
	interface/addconstrainttwoactivitiesconsecutiveform.h \
	interface/modifyconstrainttwoactivitiesconsecutiveform.h \
	\
	interface/constrainttwoactivitiesgroupedform.h \
	interface/addconstrainttwoactivitiesgroupedform.h \
	interface/modifyconstrainttwoactivitiesgroupedform.h \
	\
	interface/constraintthreeactivitiesgroupedform.h \
	interface/addconstraintthreeactivitiesgroupedform.h \
	interface/modifyconstraintthreeactivitiesgroupedform.h \
	\
	interface/constrainttwoactivitiesorderedform.h \
	interface/addconstrainttwoactivitiesorderedform.h \
	interface/modifyconstrainttwoactivitiesorderedform.h \
	\
	interface/constrainttwosetsofactivitiesorderedform.h \
	interface/addconstrainttwosetsofactivitiesorderedform.h \
	interface/modifyconstrainttwosetsofactivitiesorderedform.h \
	\
	interface/constrainttwoactivitiesorderedifsamedayform.h \
	interface/addconstrainttwoactivitiesorderedifsamedayform.h \
	interface/modifyconstrainttwoactivitiesorderedifsamedayform.h \
	\
	interface/constraintmindaysbetweenactivitiesform.h \
	interface/addconstraintmindaysbetweenactivitiesform.h \
	interface/modifyconstraintmindaysbetweenactivitiesform.h \
	\
	interface/constraintminhalfdaysbetweenactivitiesform.h \
	interface/addconstraintminhalfdaysbetweenactivitiesform.h \
	interface/modifyconstraintminhalfdaysbetweenactivitiesform.h \
	\
	interface/groupactivitiesininitialorderitemsform.h \
	interface/addgroupactivitiesininitialorderitemform.h \
	interface/modifygroupactivitiesininitialorderitemform.h \
	\
	interface/constraintmaxdaysbetweenactivitiesform.h \
	interface/addconstraintmaxdaysbetweenactivitiesform.h \
	interface/modifyconstraintmaxdaysbetweenactivitiesform.h \
	\
	interface/constraintactivitiesmaxhourlyspanform.h \
	interface/addconstraintactivitiesmaxhourlyspanform.h \
	interface/modifyconstraintactivitiesmaxhourlyspanform.h \
	\
	interface/constraintmaxhalfdaysbetweenactivitiesform.h \
	interface/addconstraintmaxhalfdaysbetweenactivitiesform.h \
	interface/modifyconstraintmaxhalfdaysbetweenactivitiesform.h \
	\
	interface/constraintmaxtermsbetweenactivitiesform.h \
	interface/addconstraintmaxtermsbetweenactivitiesform.h \
	interface/modifyconstraintmaxtermsbetweenactivitiesform.h \
	\
	interface/constraintmingapsbetweenactivitiesform.h \
	interface/addconstraintmingapsbetweenactivitiesform.h \
	interface/modifyconstraintmingapsbetweenactivitiesform.h \
	\
	interface/constraintactivitypreferredtimeslotsform.h \
	interface/modifyconstraintactivitiespreferredtimeslotsform.h \
	interface/constraintactivitypreferredstartingtimesform.h \
	interface/modifyconstraintactivitiespreferredstartingtimesform.h \
	interface/modifyconstraintactivitiessamestartingtimeform.h \
	interface/modifyconstraintactivitiessamestartinghourform.h \
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
	interface/constraintteachermaxspanperdayform.h \
	interface/addconstraintteachermaxspanperdayform.h \
	interface/modifyconstraintteachermaxspanperdayform.h \
	interface/constraintteachersmaxspanperdayform.h \
	interface/addconstraintteachersmaxspanperdayform.h \
	interface/modifyconstraintteachersmaxspanperdayform.h \
	\
	interface/constraintstudentssetmaxspanperdayform.h \
	interface/addconstraintstudentssetmaxspanperdayform.h \
	interface/modifyconstraintstudentssetmaxspanperdayform.h \
	interface/constraintstudentsmaxspanperdayform.h \
	interface/addconstraintstudentsmaxspanperdayform.h \
	interface/modifyconstraintstudentsmaxspanperdayform.h \
	\
	interface/constraintteacherminrestinghoursform.h \
	interface/addconstraintteacherminrestinghoursform.h \
	interface/modifyconstraintteacherminrestinghoursform.h \
	interface/constraintteachersminrestinghoursform.h \
	interface/addconstraintteachersminrestinghoursform.h \
	interface/modifyconstraintteachersminrestinghoursform.h \
	\
	interface/constraintstudentssetminrestinghoursform.h \
	interface/addconstraintstudentssetminrestinghoursform.h \
	interface/modifyconstraintstudentssetminrestinghoursform.h \
	interface/constraintstudentsminrestinghoursform.h \
	interface/addconstraintstudentsminrestinghoursform.h \
	interface/modifyconstraintstudentsminrestinghoursform.h \
	\
	interface/studentscomboboxesstyleform.h \
	\
	interface/addconstraintstudentssetmingapsbetweenorderedpairofactivitytagsform.h \
	interface/constraintstudentssetmingapsbetweenorderedpairofactivitytagsform.h \
	interface/modifyconstraintstudentssetmingapsbetweenorderedpairofactivitytagsform.h \
	interface/addconstraintstudentsmingapsbetweenorderedpairofactivitytagsform.h \
	interface/constraintstudentsmingapsbetweenorderedpairofactivitytagsform.h \
	interface/modifyconstraintstudentsmingapsbetweenorderedpairofactivitytagsform.h \
	interface/addconstraintteachermingapsbetweenorderedpairofactivitytagsform.h \
	interface/constraintteachermingapsbetweenorderedpairofactivitytagsform.h \
	interface/modifyconstraintteachermingapsbetweenorderedpairofactivitytagsform.h \
	interface/addconstraintteachersmingapsbetweenorderedpairofactivitytagsform.h \
	interface/constraintteachersmingapsbetweenorderedpairofactivitytagsform.h \
	interface/modifyconstraintteachersmingapsbetweenorderedpairofactivitytagsform.h \
	\
	interface/addconstraintstudentssetmingapsbetweenactivitytagform.h \
	interface/constraintstudentssetmingapsbetweenactivitytagform.h \
	interface/modifyconstraintstudentssetmingapsbetweenactivitytagform.h \
	interface/addconstraintstudentsmingapsbetweenactivitytagform.h \
	interface/constraintstudentsmingapsbetweenactivitytagform.h \
	interface/modifyconstraintstudentsmingapsbetweenactivitytagform.h \
	interface/addconstraintteachermingapsbetweenactivitytagform.h \
	interface/constraintteachermingapsbetweenactivitytagform.h \
	interface/modifyconstraintteachermingapsbetweenactivitytagform.h \
	interface/addconstraintteachersmingapsbetweenactivitytagform.h \
	interface/constraintteachersmingapsbetweenactivitytagform.h \
	interface/modifyconstraintteachersmingapsbetweenactivitytagform.h \
	\
	interface/notificationcommandform.h \
	\
	interface/helpmoroccoform.h \
	interface/helpalgeriaform.h \
	\
	interface/addconstraintstudentssetmaxroomchangesperrealdayform.h \
	interface/constraintstudentssetmaxroomchangesperrealdayform.h \
	interface/modifyconstraintstudentssetmaxroomchangesperrealdayform.h \
	interface/addconstraintstudentsmaxroomchangesperrealdayform.h \
	interface/constraintstudentsmaxroomchangesperrealdayform.h \
	interface/modifyconstraintstudentsmaxroomchangesperrealdayform.h \
	\
	interface/addconstraintteachermaxroomchangesperrealdayform.h \
	interface/constraintteachermaxroomchangesperrealdayform.h \
	interface/modifyconstraintteachermaxroomchangesperrealdayform.h \
	interface/addconstraintteachersmaxroomchangesperrealdayform.h \
	interface/constraintteachersmaxroomchangesperrealdayform.h \
	interface/modifyconstraintteachersmaxroomchangesperrealdayform.h \
	\
	interface/addconstraintstudentssetmaxbuildingchangesperrealdayform.h \
	interface/constraintstudentssetmaxbuildingchangesperrealdayform.h \
	interface/modifyconstraintstudentssetmaxbuildingchangesperrealdayform.h \
	interface/addconstraintstudentsmaxbuildingchangesperrealdayform.h \
	interface/constraintstudentsmaxbuildingchangesperrealdayform.h \
	interface/modifyconstraintstudentsmaxbuildingchangesperrealdayform.h \
	\
	interface/addconstraintteachermaxbuildingchangesperrealdayform.h \
	interface/constraintteachermaxbuildingchangesperrealdayform.h \
	interface/modifyconstraintteachermaxbuildingchangesperrealdayform.h \
	interface/addconstraintteachersmaxbuildingchangesperrealdayform.h \
	interface/constraintteachersmaxbuildingchangesperrealdayform.h \
	interface/modifyconstraintteachersmaxbuildingchangesperrealdayform.h \
	\
	interface/constraintteachermaxafternoonsperweekform.h \
	interface/addconstraintteachermaxafternoonsperweekform.h \
	interface/modifyconstraintteachermaxafternoonsperweekform.h \
	interface/constraintteachersmaxafternoonsperweekform.h \
	interface/addconstraintteachersmaxafternoonsperweekform.h \
	interface/modifyconstraintteachersmaxafternoonsperweekform.h \
	\
	interface/constraintteachermaxmorningsperweekform.h \
	interface/addconstraintteachermaxmorningsperweekform.h \
	interface/modifyconstraintteachermaxmorningsperweekform.h \
	interface/constraintteachersmaxmorningsperweekform.h \
	interface/addconstraintteachersmaxmorningsperweekform.h \
	interface/modifyconstraintteachersmaxmorningsperweekform.h \
	\
	interface/constraintteachermaxtwoconsecutivemorningsform.h \
	interface/addconstraintteachermaxtwoconsecutivemorningsform.h \
	interface/modifyconstraintteachermaxtwoconsecutivemorningsform.h \
	interface/constraintteachersmaxtwoconsecutivemorningsform.h \
	interface/addconstraintteachersmaxtwoconsecutivemorningsform.h \
	interface/modifyconstraintteachersmaxtwoconsecutivemorningsform.h \
	\
	interface/constraintteachermaxtwoconsecutiveafternoonsform.h \
	interface/addconstraintteachermaxtwoconsecutiveafternoonsform.h \
	interface/modifyconstraintteachermaxtwoconsecutiveafternoonsform.h \
	interface/constraintteachersmaxtwoconsecutiveafternoonsform.h \
	interface/addconstraintteachersmaxtwoconsecutiveafternoonsform.h \
	interface/modifyconstraintteachersmaxtwoconsecutiveafternoonsform.h \
	\
	interface/constraintteacherminmorningsperweekform.h \
	interface/addconstraintteacherminmorningsperweekform.h \
	interface/modifyconstraintteacherminmorningsperweekform.h \
	\
	interface/constraintteacherminafternoonsperweekform.h \
	interface/addconstraintteacherminafternoonsperweekform.h \
	interface/modifyconstraintteacherminafternoonsperweekform.h \
	\
	interface/constraintteachersminmorningsperweekform.h \
	interface/addconstraintteachersminmorningsperweekform.h \
	interface/modifyconstraintteachersminmorningsperweekform.h \
	\
	interface/constraintteachersminafternoonsperweekform.h \
	interface/addconstraintteachersminafternoonsperweekform.h \
	interface/modifyconstraintteachersminafternoonsperweekform.h \
	\
	interface/constraintteachermorningintervalmaxdaysperweekform.h \
	interface/addconstraintteachermorningintervalmaxdaysperweekform.h \
	interface/modifyconstraintteachermorningintervalmaxdaysperweekform.h \
	\
	interface/constraintteacherafternoonintervalmaxdaysperweekform.h \
	interface/addconstraintteacherafternoonintervalmaxdaysperweekform.h \
	interface/modifyconstraintteacherafternoonintervalmaxdaysperweekform.h \
	\
	interface/constraintstudentssetmorningintervalmaxdaysperweekform.h \
	interface/addconstraintstudentssetmorningintervalmaxdaysperweekform.h \
	interface/modifyconstraintstudentssetmorningintervalmaxdaysperweekform.h \
	\
	interface/constraintstudentsmorningintervalmaxdaysperweekform.h \
	interface/addconstraintstudentsmorningintervalmaxdaysperweekform.h \
	interface/modifyconstraintstudentsmorningintervalmaxdaysperweekform.h \
	\
	interface/constraintstudentssetafternoonintervalmaxdaysperweekform.h \
	interface/addconstraintstudentssetafternoonintervalmaxdaysperweekform.h \
	interface/modifyconstraintstudentssetafternoonintervalmaxdaysperweekform.h \
	\
	interface/constraintstudentsafternoonintervalmaxdaysperweekform.h \
	interface/addconstraintstudentsafternoonintervalmaxdaysperweekform.h \
	interface/modifyconstraintstudentsafternoonintervalmaxdaysperweekform.h \
	\
	interface/constraintstudentssetmaxafternoonsperweekform.h \
	interface/addconstraintstudentssetmaxafternoonsperweekform.h \
	interface/modifyconstraintstudentssetmaxafternoonsperweekform.h \
	\
	interface/constraintstudentsmaxafternoonsperweekform.h \
	interface/addconstraintstudentsmaxafternoonsperweekform.h \
	interface/modifyconstraintstudentsmaxafternoonsperweekform.h \
	\
	interface/constraintstudentssetmaxmorningsperweekform.h \
	interface/addconstraintstudentssetmaxmorningsperweekform.h \
	interface/modifyconstraintstudentssetmaxmorningsperweekform.h \
	\
	interface/constraintstudentsmaxmorningsperweekform.h \
	interface/addconstraintstudentsmaxmorningsperweekform.h \
	interface/modifyconstraintstudentsmaxmorningsperweekform.h \
	\
	interface/constraintstudentssetminafternoonsperweekform.h \
	interface/addconstraintstudentssetminafternoonsperweekform.h \
	interface/modifyconstraintstudentssetminafternoonsperweekform.h \
	\
	interface/constraintstudentsminafternoonsperweekform.h \
	interface/addconstraintstudentsminafternoonsperweekform.h \
	interface/modifyconstraintstudentsminafternoonsperweekform.h \
	\
	interface/constraintstudentssetminmorningsperweekform.h \
	interface/addconstraintstudentssetminmorningsperweekform.h \
	interface/modifyconstraintstudentssetminmorningsperweekform.h \
	\
	interface/constraintstudentsminmorningsperweekform.h \
	interface/addconstraintstudentsminmorningsperweekform.h \
	interface/modifyconstraintstudentsminmorningsperweekform.h \
	\
	interface/constraintteachersmorningintervalmaxdaysperweekform.h \
	interface/addconstraintteachersmorningintervalmaxdaysperweekform.h \
	interface/modifyconstraintteachersmorningintervalmaxdaysperweekform.h \
	\
	interface/constraintteachersafternoonintervalmaxdaysperweekform.h \
	interface/addconstraintteachersafternoonintervalmaxdaysperweekform.h \
	interface/modifyconstraintteachersafternoonintervalmaxdaysperweekform.h \
	\
	interface/constraintteachermaxhoursperallafternoonsform.h \
	interface/addconstraintteachermaxhoursperallafternoonsform.h \
	interface/modifyconstraintteachermaxhoursperallafternoonsform.h \
	\
	interface/constraintteachersmaxhoursperallafternoonsform.h \
	interface/addconstraintteachersmaxhoursperallafternoonsform.h \
	interface/modifyconstraintteachersmaxhoursperallafternoonsform.h \
	\
	interface/constraintstudentssetmaxhoursperallafternoonsform.h \
	interface/addconstraintstudentssetmaxhoursperallafternoonsform.h \
	interface/modifyconstraintstudentssetmaxhoursperallafternoonsform.h \
	\
	interface/constraintstudentsmaxhoursperallafternoonsform.h \
	interface/addconstraintstudentsmaxhoursperallafternoonsform.h \
	interface/modifyconstraintstudentsmaxhoursperallafternoonsform.h \
	\
	interface/constraintteachersminhourspermorningform.h \
	interface/addconstraintteachersminhourspermorningform.h \
	interface/modifyconstraintteachersminhourspermorningform.h \
	interface/constraintteacherminhourspermorningform.h \
	interface/addconstraintteacherminhourspermorningform.h \
	interface/modifyconstraintteacherminhourspermorningform.h \
	\
	interface/constraintteachersminhoursperafternoonform.h \
	interface/addconstraintteachersminhoursperafternoonform.h \
	interface/modifyconstraintteachersminhoursperafternoonform.h \
	interface/constraintteacherminhoursperafternoonform.h \
	interface/addconstraintteacherminhoursperafternoonform.h \
	interface/modifyconstraintteacherminhoursperafternoonform.h \
	\
	interface/constraintteachersminhoursdailyrealdaysform.h \
	interface/addconstraintteachersminhoursdailyrealdaysform.h \
	interface/modifyconstraintteachersminhoursdailyrealdaysform.h \
	interface/constraintteacherminhoursdailyrealdaysform.h \
	interface/addconstraintteacherminhoursdailyrealdaysform.h \
	interface/modifyconstraintteacherminhoursdailyrealdaysform.h \
	\
	interface/constraintstudentssetmaxgapsperrealdayform.h \
	interface/addconstraintstudentssetmaxgapsperrealdayform.h \
	interface/modifyconstraintstudentssetmaxgapsperrealdayform.h \
	interface/constraintstudentsmaxgapsperrealdayform.h \
	interface/addconstraintstudentsmaxgapsperrealdayform.h \
	interface/modifyconstraintstudentsmaxgapsperrealdayform.h \
	\
	interface/constraintteachersmaxzerogapsperafternoonform.h \
	interface/constraintteachermaxzerogapsperafternoonform.h \
	interface/addconstraintteachersmaxzerogapsperafternoonform.h \
	interface/modifyconstraintteachersmaxzerogapsperafternoonform.h \
	interface/addconstraintteachermaxzerogapsperafternoonform.h \
	interface/modifyconstraintteachermaxzerogapsperafternoonform.h \
	\
	interface/constraintteachersmaxgapsperrealdayform.h \
	interface/constraintteachermaxgapsperrealdayform.h \
	interface/addconstraintteachersmaxgapsperrealdayform.h \
	interface/modifyconstraintteachersmaxgapsperrealdayform.h \
	interface/addconstraintteachermaxgapsperrealdayform.h \
	interface/modifyconstraintteachermaxgapsperrealdayform.h \
	\
	interface/constraintstudentssetmaxgapsperweekforrealdaysform.h \
	interface/addconstraintstudentssetmaxgapsperweekforrealdaysform.h \
	interface/modifyconstraintstudentssetmaxgapsperweekforrealdaysform.h \
	interface/constraintstudentsmaxgapsperweekforrealdaysform.h \
	interface/addconstraintstudentsmaxgapsperweekforrealdaysform.h \
	interface/modifyconstraintstudentsmaxgapsperweekforrealdaysform.h \
	\
	interface/constraintteachersmaxgapsperweekforrealdaysform.h \
	interface/constraintteachermaxgapsperweekforrealdaysform.h \
	interface/addconstraintteachersmaxgapsperweekforrealdaysform.h \
	interface/modifyconstraintteachersmaxgapsperweekforrealdaysform.h \
	interface/addconstraintteachermaxgapsperweekforrealdaysform.h \
	interface/modifyconstraintteachermaxgapsperweekforrealdaysform.h \
	\
	interface/constraintstudentsafternoonsearlymaxbeginningsatsecondhourform.h \
	interface/addconstraintstudentsafternoonsearlymaxbeginningsatsecondhourform.h \
	interface/modifyconstraintstudentsafternoonsearlymaxbeginningsatsecondhourform.h \
	interface/constraintstudentssetafternoonsearlymaxbeginningsatsecondhourform.h \
	interface/addconstraintstudentssetafternoonsearlymaxbeginningsatsecondhourform.h \
	interface/modifyconstraintstudentssetafternoonsearlymaxbeginningsatsecondhourform.h \
	\
	interface/constraintteachersafternoonsearlymaxbeginningsatsecondhourform.h \
	interface/addconstraintteachersafternoonsearlymaxbeginningsatsecondhourform.h \
	interface/modifyconstraintteachersafternoonsearlymaxbeginningsatsecondhourform.h \
	interface/constraintteacherafternoonsearlymaxbeginningsatsecondhourform.h \
	interface/addconstraintteacherafternoonsearlymaxbeginningsatsecondhourform.h \
	interface/modifyconstraintteacherafternoonsearlymaxbeginningsatsecondhourform.h \
	\
	interface/constraintstudentsmorningsearlymaxbeginningsatsecondhourform.h \
	interface/addconstraintstudentsmorningsearlymaxbeginningsatsecondhourform.h \
	interface/modifyconstraintstudentsmorningsearlymaxbeginningsatsecondhourform.h \
	interface/constraintstudentssetmorningsearlymaxbeginningsatsecondhourform.h \
	interface/addconstraintstudentssetmorningsearlymaxbeginningsatsecondhourform.h \
	interface/modifyconstraintstudentssetmorningsearlymaxbeginningsatsecondhourform.h \
	\
	interface/constraintteachersmorningsearlymaxbeginningsatsecondhourform.h \
	interface/addconstraintteachersmorningsearlymaxbeginningsatsecondhourform.h \
	interface/modifyconstraintteachersmorningsearlymaxbeginningsatsecondhourform.h \
	interface/constraintteachermorningsearlymaxbeginningsatsecondhourform.h \
	interface/addconstraintteachermorningsearlymaxbeginningsatsecondhourform.h \
	interface/modifyconstraintteachermorningsearlymaxbeginningsatsecondhourform.h \
	\
	interface/constraintstudentssetminhourspermorningform.h \
	interface/addconstraintstudentssetminhourspermorningform.h \
	interface/modifyconstraintstudentssetminhourspermorningform.h \
	interface/constraintstudentsminhourspermorningform.h \
	interface/addconstraintstudentsminhourspermorningform.h \
	interface/modifyconstraintstudentsminhourspermorningform.h \
	\
	interface/constraintstudentssetminhoursperafternoonform.h \
	interface/addconstraintstudentssetminhoursperafternoonform.h \
	interface/modifyconstraintstudentssetminhoursperafternoonform.h \
	interface/constraintstudentsminhoursperafternoonform.h \
	interface/addconstraintstudentsminhoursperafternoonform.h \
	interface/modifyconstraintstudentsminhoursperafternoonform.h \
	\
	interface/constraintteachermaxtwoactivitytagsperdayfromn1n2n3form.h \
	interface/addconstraintteachermaxtwoactivitytagsperdayfromn1n2n3form.h \
	interface/modifyconstraintteachermaxtwoactivitytagsperdayfromn1n2n3form.h \
	interface/constraintteachersmaxtwoactivitytagsperdayfromn1n2n3form.h \
	interface/addconstraintteachersmaxtwoactivitytagsperdayfromn1n2n3form.h \
	interface/modifyconstraintteachersmaxtwoactivitytagsperdayfromn1n2n3form.h \
	\
	interface/constraintstudentssetmaxtwoactivitytagsperdayfromn1n2n3form.h \
	interface/addconstraintstudentssetmaxtwoactivitytagsperdayfromn1n2n3form.h \
	interface/modifyconstraintstudentssetmaxtwoactivitytagsperdayfromn1n2n3form.h \
	interface/constraintstudentsmaxtwoactivitytagsperdayfromn1n2n3form.h \
	interface/addconstraintstudentsmaxtwoactivitytagsperdayfromn1n2n3form.h \
	interface/modifyconstraintstudentsmaxtwoactivitytagsperdayfromn1n2n3form.h \
	\
	interface/constraintteachermaxtwoactivitytagsperrealdayfromn1n2n3form.h \
	interface/addconstraintteachermaxtwoactivitytagsperrealdayfromn1n2n3form.h \
	interface/modifyconstraintteachermaxtwoactivitytagsperrealdayfromn1n2n3form.h \
	interface/constraintteachersmaxtwoactivitytagsperrealdayfromn1n2n3form.h \
	interface/addconstraintteachersmaxtwoactivitytagsperrealdayfromn1n2n3form.h \
	interface/modifyconstraintteachersmaxtwoactivitytagsperrealdayfromn1n2n3form.h \
	\
	interface/constraintstudentssetmaxtwoactivitytagsperrealdayfromn1n2n3form.h \
	interface/addconstraintstudentssetmaxtwoactivitytagsperrealdayfromn1n2n3form.h \
	interface/modifyconstraintstudentssetmaxtwoactivitytagsperrealdayfromn1n2n3form.h \
	interface/constraintstudentsmaxtwoactivitytagsperrealdayfromn1n2n3form.h \
	interface/addconstraintstudentsmaxtwoactivitytagsperrealdayfromn1n2n3form.h \
	interface/modifyconstraintstudentsmaxtwoactivitytagsperrealdayfromn1n2n3form.h \
	\
	interface/constraintteacherminrestinghoursbetweenmorningandafternoonform.h \
	interface/addconstraintteacherminrestinghoursbetweenmorningandafternoonform.h \
	interface/modifyconstraintteacherminrestinghoursbetweenmorningandafternoonform.h \
	interface/constraintteachersminrestinghoursbetweenmorningandafternoonform.h \
	interface/addconstraintteachersminrestinghoursbetweenmorningandafternoonform.h \
	interface/modifyconstraintteachersminrestinghoursbetweenmorningandafternoonform.h \
	\
	interface/constraintstudentssetminrestinghoursbetweenmorningandafternoonform.h \
	interface/addconstraintstudentssetminrestinghoursbetweenmorningandafternoonform.h \
	interface/modifyconstraintstudentssetminrestinghoursbetweenmorningandafternoonform.h \
	interface/constraintstudentsminrestinghoursbetweenmorningandafternoonform.h \
	interface/addconstraintstudentsminrestinghoursbetweenmorningandafternoonform.h \
	interface/modifyconstraintstudentsminrestinghoursbetweenmorningandafternoonform.h \
	\
	interface/helponn1n2n3.h \
	\
	interface/constraintteachersmaxhoursdailyrealdaysform.h \
	interface/addconstraintteachersmaxhoursdailyrealdaysform.h \
	interface/modifyconstraintteachersmaxhoursdailyrealdaysform.h \
	interface/constraintteachermaxhoursdailyrealdaysform.h \
	interface/addconstraintteachermaxhoursdailyrealdaysform.h \
	interface/modifyconstraintteachermaxhoursdailyrealdaysform.h \
	\
	interface/constraintteachersactivitytagmaxhoursdailyrealdaysform.h \
	interface/addconstraintteachersactivitytagmaxhoursdailyrealdaysform.h \
	interface/modifyconstraintteachersactivitytagmaxhoursdailyrealdaysform.h \
	interface/constraintteacheractivitytagmaxhoursdailyrealdaysform.h \
	interface/addconstraintteacheractivitytagmaxhoursdailyrealdaysform.h \
	interface/modifyconstraintteacheractivitytagmaxhoursdailyrealdaysform.h \
	\
	interface/constraintstudentssetmaxhoursdailyrealdaysform.h \
	interface/addconstraintstudentssetmaxhoursdailyrealdaysform.h \
	interface/modifyconstraintstudentssetmaxhoursdailyrealdaysform.h \
	interface/constraintstudentsmaxhoursdailyrealdaysform.h \
	interface/addconstraintstudentsmaxhoursdailyrealdaysform.h \
	interface/modifyconstraintstudentsmaxhoursdailyrealdaysform.h \
	\
	interface/constraintstudentssetactivitytagmaxhoursdailyrealdaysform.h \
	interface/addconstraintstudentssetactivitytagmaxhoursdailyrealdaysform.h \
	interface/modifyconstraintstudentssetactivitytagmaxhoursdailyrealdaysform.h \
	interface/constraintstudentsactivitytagmaxhoursdailyrealdaysform.h \
	interface/addconstraintstudentsactivitytagmaxhoursdailyrealdaysform.h \
	interface/modifyconstraintstudentsactivitytagmaxhoursdailyrealdaysform.h \
	\
	interface/constraintteachermaxrealdaysperweekform.h \
	interface/addconstraintteachermaxrealdaysperweekform.h \
	interface/modifyconstraintteachermaxrealdaysperweekform.h \
	\
	interface/constraintteachersmaxrealdaysperweekform.h \
	interface/addconstraintteachersmaxrealdaysperweekform.h \
	interface/modifyconstraintteachersmaxrealdaysperweekform.h \
	\
	interface/constraintstudentssetmaxrealdaysperweekform.h \
	interface/addconstraintstudentssetmaxrealdaysperweekform.h \
	interface/modifyconstraintstudentssetmaxrealdaysperweekform.h \
	\
	interface/constraintstudentsmaxrealdaysperweekform.h \
	interface/addconstraintstudentsmaxrealdaysperweekform.h \
	interface/modifyconstraintstudentsmaxrealdaysperweekform.h \
	\
	interface/constraintteacherminrealdaysperweekform.h \
	interface/addconstraintteacherminrealdaysperweekform.h \
	interface/modifyconstraintteacherminrealdaysperweekform.h \
	\
	interface/constraintteachersminrealdaysperweekform.h \
	interface/addconstraintteachersminrealdaysperweekform.h \
	interface/modifyconstraintteachersminrealdaysperweekform.h \
	\
	interface/constraintteachermaxspanperrealdayform.h \
	interface/addconstraintteachermaxspanperrealdayform.h \
	interface/modifyconstraintteachermaxspanperrealdayform.h \
	interface/constraintteachersmaxspanperrealdayform.h \
	interface/addconstraintteachersmaxspanperrealdayform.h \
	interface/modifyconstraintteachersmaxspanperrealdayform.h \
	\
	interface/constraintstudentssetmaxspanperrealdayform.h \
	interface/addconstraintstudentssetmaxspanperrealdayform.h \
	interface/modifyconstraintstudentssetmaxspanperrealdayform.h \
	interface/constraintstudentsmaxspanperrealdayform.h \
	interface/addconstraintstudentsmaxspanperrealdayform.h \
	interface/modifyconstraintstudentsmaxspanperrealdayform.h \
	\
	interface/constraintmaxtotalactivitiesfromsetinselectedtimeslotsform.h \
	interface/addconstraintmaxtotalactivitiesfromsetinselectedtimeslotsform.h \
	interface/modifyconstraintmaxtotalactivitiesfromsetinselectedtimeslotsform.h \
	\
	interface/constraintmaxgapsbetweenactivitiesform.h \
	interface/addconstraintmaxgapsbetweenactivitiesform.h \
	interface/modifyconstraintmaxgapsbetweenactivitiesform.h \
	\
	interface/constraintactivitiesmaxinatermform.h \
	interface/addconstraintactivitiesmaxinatermform.h \
	interface/modifyconstraintactivitiesmaxinatermform.h \
	\
	interface/constraintactivitiesmininatermform.h \
	interface/addconstraintactivitiesmininatermform.h \
	interface/modifyconstraintactivitiesmininatermform.h \
	\
	interface/constraintactivitiesoccupymaxtermsform.h \
	interface/addconstraintactivitiesoccupymaxtermsform.h \
	interface/modifyconstraintactivitiesoccupymaxtermsform.h \
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
	interface/settingsautosaveform.h

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
	interface/addconstraintactivitypreferredtimeslotsform_template.ui \
	interface/modifyconstraintactivitypreferredtimeslotsform_template.ui \
	interface/addconstraintactivitypreferredstartingtimesform_template.ui \
	interface/modifyconstraintactivitypreferredstartingtimesform_template.ui \
	interface/addconstraintroomnotavailabletimesform_template.ui \
	interface/constraintroomnotavailabletimesform_template.ui \
	interface/modifyconstraintroomnotavailabletimesform_template.ui \
	interface/addconstraintteacherroomnotavailabletimesform_template.ui \
	interface/constraintteacherroomnotavailabletimesform_template.ui \
	interface/modifyconstraintteacherroomnotavailabletimesform_template.ui \
	interface/addconstraintactivitypreferredroomform_template.ui \
	interface/constraintactivitypreferredroomform_template.ui \
	interface/modifyconstraintactivitypreferredroomform_template.ui \
	interface/addconstraintsubjectpreferredroomform_template.ui \
	interface/constraintsubjectpreferredroomform_template.ui \
	interface/modifyconstraintsubjectpreferredroomform_template.ui \
	interface/addconstraintsubjectactivitytagpreferredroomform_template.ui \
	interface/constraintsubjectactivitytagpreferredroomform_template.ui \
	interface/modifyconstraintsubjectactivitytagpreferredroomform_template.ui \
	\
	interface/addconstraintactivitytagpreferredroomform_template.ui \
	interface/constraintactivitytagpreferredroomform_template.ui \
	interface/modifyconstraintactivitytagpreferredroomform_template.ui \
	\
	interface/addconstraintsubjectpreferredroomsform_template.ui \
	interface/constraintsubjectpreferredroomsform_template.ui \
	interface/modifyconstraintsubjectpreferredroomsform_template.ui \
	interface/addconstraintsubjectactivitytagpreferredroomsform_template.ui \
	interface/constraintsubjectactivitytagpreferredroomsform_template.ui \
	interface/modifyconstraintsubjectactivitytagpreferredroomsform_template.ui \
	\
	interface/addconstraintactivitytagpreferredroomsform_template.ui \
	interface/constraintactivitytagpreferredroomsform_template.ui \
	interface/modifyconstraintactivitytagpreferredroomsform_template.ui \
	\
	interface/addconstraintstudentssethomeroomform_template.ui \
	interface/constraintstudentssethomeroomform_template.ui \
	interface/modifyconstraintstudentssethomeroomform_template.ui \
	interface/addconstraintstudentssethomeroomsform_template.ui \
	interface/constraintstudentssethomeroomsform_template.ui \
	interface/modifyconstraintstudentssethomeroomsform_template.ui \
	\
	interface/addconstraintstudentssetmaxbuildingchangesperdayform_template.ui \
	interface/constraintstudentssetmaxbuildingchangesperdayform_template.ui \
	interface/modifyconstraintstudentssetmaxbuildingchangesperdayform_template.ui \
	interface/addconstraintstudentsmaxbuildingchangesperdayform_template.ui \
	interface/constraintstudentsmaxbuildingchangesperdayform_template.ui \
	interface/modifyconstraintstudentsmaxbuildingchangesperdayform_template.ui \
	interface/addconstraintstudentssetmaxbuildingchangesperweekform_template.ui \
	interface/constraintstudentssetmaxbuildingchangesperweekform_template.ui \
	interface/modifyconstraintstudentssetmaxbuildingchangesperweekform_template.ui \
	interface/addconstraintstudentsmaxbuildingchangesperweekform_template.ui \
	interface/constraintstudentsmaxbuildingchangesperweekform_template.ui \
	interface/modifyconstraintstudentsmaxbuildingchangesperweekform_template.ui \
	interface/addconstraintstudentssetmingapsbetweenbuildingchangesform_template.ui \
	interface/constraintstudentssetmingapsbetweenbuildingchangesform_template.ui \
	interface/modifyconstraintstudentssetmingapsbetweenbuildingchangesform_template.ui \
	interface/addconstraintstudentsmingapsbetweenbuildingchangesform_template.ui \
	interface/constraintstudentsmingapsbetweenbuildingchangesform_template.ui \
	interface/modifyconstraintstudentsmingapsbetweenbuildingchangesform_template.ui \
	\
	interface/addconstraintteachermaxbuildingchangesperdayform_template.ui \
	interface/constraintteachermaxbuildingchangesperdayform_template.ui \
	interface/modifyconstraintteachermaxbuildingchangesperdayform_template.ui \
	interface/addconstraintteachersmaxbuildingchangesperdayform_template.ui \
	interface/constraintteachersmaxbuildingchangesperdayform_template.ui \
	interface/modifyconstraintteachersmaxbuildingchangesperdayform_template.ui \
	interface/addconstraintteachermaxbuildingchangesperweekform_template.ui \
	interface/constraintteachermaxbuildingchangesperweekform_template.ui \
	interface/modifyconstraintteachermaxbuildingchangesperweekform_template.ui \
	interface/addconstraintteachersmaxbuildingchangesperweekform_template.ui \
	interface/constraintteachersmaxbuildingchangesperweekform_template.ui \
	interface/modifyconstraintteachersmaxbuildingchangesperweekform_template.ui \
	interface/addconstraintteachermingapsbetweenbuildingchangesform_template.ui \
	interface/constraintteachermingapsbetweenbuildingchangesform_template.ui \
	interface/modifyconstraintteachermingapsbetweenbuildingchangesform_template.ui \
	interface/addconstraintteachersmingapsbetweenbuildingchangesform_template.ui \
	interface/constraintteachersmingapsbetweenbuildingchangesform_template.ui \
	interface/modifyconstraintteachersmingapsbetweenbuildingchangesform_template.ui \
	\
	interface/addconstraintstudentssetmaxroomchangesperdayform_template.ui \
	interface/constraintstudentssetmaxroomchangesperdayform_template.ui \
	interface/modifyconstraintstudentssetmaxroomchangesperdayform_template.ui \
	interface/addconstraintstudentsmaxroomchangesperdayform_template.ui \
	interface/constraintstudentsmaxroomchangesperdayform_template.ui \
	interface/modifyconstraintstudentsmaxroomchangesperdayform_template.ui \
	interface/addconstraintstudentssetmaxroomchangesperweekform_template.ui \
	interface/constraintstudentssetmaxroomchangesperweekform_template.ui \
	interface/modifyconstraintstudentssetmaxroomchangesperweekform_template.ui \
	interface/addconstraintstudentsmaxroomchangesperweekform_template.ui \
	interface/constraintstudentsmaxroomchangesperweekform_template.ui \
	interface/modifyconstraintstudentsmaxroomchangesperweekform_template.ui \
	interface/addconstraintstudentssetmingapsbetweenroomchangesform_template.ui \
	interface/constraintstudentssetmingapsbetweenroomchangesform_template.ui \
	interface/modifyconstraintstudentssetmingapsbetweenroomchangesform_template.ui \
	interface/addconstraintstudentsmingapsbetweenroomchangesform_template.ui \
	interface/constraintstudentsmingapsbetweenroomchangesform_template.ui \
	interface/modifyconstraintstudentsmingapsbetweenroomchangesform_template.ui \
	\
	interface/addconstraintteachermaxroomchangesperdayform_template.ui \
	interface/constraintteachermaxroomchangesperdayform_template.ui \
	interface/modifyconstraintteachermaxroomchangesperdayform_template.ui \
	interface/addconstraintteachersmaxroomchangesperdayform_template.ui \
	interface/constraintteachersmaxroomchangesperdayform_template.ui \
	interface/modifyconstraintteachersmaxroomchangesperdayform_template.ui \
	interface/addconstraintteachermaxroomchangesperweekform_template.ui \
	interface/constraintteachermaxroomchangesperweekform_template.ui \
	interface/modifyconstraintteachermaxroomchangesperweekform_template.ui \
	interface/addconstraintteachersmaxroomchangesperweekform_template.ui \
	interface/constraintteachersmaxroomchangesperweekform_template.ui \
	interface/modifyconstraintteachersmaxroomchangesperweekform_template.ui \
	interface/addconstraintteachermingapsbetweenroomchangesform_template.ui \
	interface/constraintteachermingapsbetweenroomchangesform_template.ui \
	interface/modifyconstraintteachermingapsbetweenroomchangesform_template.ui \
	interface/addconstraintteachersmingapsbetweenroomchangesform_template.ui \
	interface/constraintteachersmingapsbetweenroomchangesform_template.ui \
	interface/modifyconstraintteachersmingapsbetweenroomchangesform_template.ui \
	\
	interface/addconstraintteacherhomeroomform_template.ui \
	interface/constraintteacherhomeroomform_template.ui \
	interface/modifyconstraintteacherhomeroomform_template.ui \
	interface/addconstraintteacherhomeroomsform_template.ui \
	interface/constraintteacherhomeroomsform_template.ui \
	interface/modifyconstraintteacherhomeroomsform_template.ui \
	interface/addconstraintactivitypreferredroomsform_template.ui \
	interface/constraintactivitypreferredroomsform_template.ui \
	interface/modifyconstraintactivitypreferredroomsform_template.ui \
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
	interface/timetableviewstudentstimehorizontalform_template.ui \
	interface/timetableviewroomsdayshorizontalform_template.ui \
	interface/timetableviewroomstimehorizontalform_template.ui \
	interface/yearsform_template.ui \
	interface/timetableshowconflictsform_template.ui \
	interface/timetableviewteachersdayshorizontalform_template.ui \
	interface/timetableviewteacherstimehorizontalform_template.ui \
	interface/daysform_template.ui \
	interface/hoursform_template.ui \
	interface/roomsform_template.ui \
	interface/randomseedform_template.ui \
	interface/getmodefornewfileform_template.ui \
	interface/roommakeeditvirtualform_template.ui \
	interface/buildingsform_template.ui \
	\
	interface/constraintactivitiespreferredtimeslotsform_template.ui \
	interface/addconstraintactivitiespreferredtimeslotsform_template.ui \
	interface/constraintactivitiespreferredstartingtimesform_template.ui \
	interface/addconstraintactivitiespreferredstartingtimesform_template.ui \
	\
	interface/constraintsubactivitiespreferredtimeslotsform_template.ui \
	interface/addconstraintsubactivitiespreferredtimeslotsform_template.ui \
	interface/constraintsubactivitiespreferredstartingtimesform_template.ui \
	interface/addconstraintsubactivitiespreferredstartingtimesform_template.ui \
	interface/modifyconstraintsubactivitiespreferredtimeslotsform_template.ui \
	interface/modifyconstraintsubactivitiespreferredstartingtimesform_template.ui \
	\
	interface/constraintactivitiessamestartingtimeform_template.ui \
	interface/addconstraintactivitiessamestartingtimeform_template.ui \
	\
	interface/constraintactivitiessamestartinghourform_template.ui \
	interface/addconstraintactivitiessamestartinghourform_template.ui \
	\
	interface/constraintactivitiessamestartingdayform_template.ui \
	interface/addconstraintactivitiessamestartingdayform_template.ui \
	interface/modifyconstraintactivitiessamestartingdayform_template.ui \
	\
	interface/constraintteachernotavailabletimesform_template.ui \
	interface/addconstraintteachernotavailabletimesform_template.ui \
	interface/modifyconstraintteachernotavailabletimesform_template.ui \
	interface/constraintbasiccompulsorytimeform_template.ui \
	interface/addconstraintbasiccompulsorytimeform_template.ui \
	interface/modifyconstraintbasiccompulsorytimeform_template.ui \
	interface/constraintbasiccompulsoryspaceform_template.ui \
	interface/addconstraintbasiccompulsoryspaceform_template.ui \
	interface/modifyconstraintbasiccompulsoryspaceform_template.ui \
	interface/constraintstudentssetnotavailabletimesform_template.ui \
	interface/addconstraintstudentssetnotavailabletimesform_template.ui \
	interface/modifyconstraintstudentssetnotavailabletimesform_template.ui \
	interface/constraintbreaktimesform_template.ui \
	interface/addconstraintbreaktimesform_template.ui \
	interface/modifyconstraintbreaktimesform_template.ui \
	interface/constraintteachermaxdaysperweekform_template.ui \
	interface/addconstraintteachermaxdaysperweekform_template.ui \
	interface/modifyconstraintteachermaxdaysperweekform_template.ui \
	\
	interface/constraintteachermaxthreeconsecutivedaysform_template.ui \
	interface/addconstraintteachermaxthreeconsecutivedaysform_template.ui \
	interface/modifyconstraintteachermaxthreeconsecutivedaysform_template.ui \
	interface/constraintteachersmaxthreeconsecutivedaysform_template.ui \
	interface/addconstraintteachersmaxthreeconsecutivedaysform_template.ui \
	interface/modifyconstraintteachersmaxthreeconsecutivedaysform_template.ui \
	\
	interface/constraintstudentssetmaxthreeconsecutivedaysform_template.ui \
	interface/addconstraintstudentssetmaxthreeconsecutivedaysform_template.ui \
	interface/modifyconstraintstudentssetmaxthreeconsecutivedaysform_template.ui \
	interface/constraintstudentsmaxthreeconsecutivedaysform_template.ui \
	interface/addconstraintstudentsmaxthreeconsecutivedaysform_template.ui \
	interface/modifyconstraintstudentsmaxthreeconsecutivedaysform_template.ui \
	\
	interface/constraintteachermindaysperweekform_template.ui \
	interface/addconstraintteachermindaysperweekform_template.ui \
	interface/modifyconstraintteachermindaysperweekform_template.ui \
	\
	interface/constraintteachersmaxdaysperweekform_template.ui \
	interface/addconstraintteachersmaxdaysperweekform_template.ui \
	interface/modifyconstraintteachersmaxdaysperweekform_template.ui \
	\
	interface/constraintteachersmindaysperweekform_template.ui \
	interface/addconstraintteachersmindaysperweekform_template.ui \
	interface/modifyconstraintteachersmindaysperweekform_template.ui \
	\
	interface/constraintteacherintervalmaxdaysperweekform_template.ui \
	interface/addconstraintteacherintervalmaxdaysperweekform_template.ui \
	interface/modifyconstraintteacherintervalmaxdaysperweekform_template.ui \
	\
	interface/constraintstudentssetintervalmaxdaysperweekform_template.ui \
	interface/addconstraintstudentssetintervalmaxdaysperweekform_template.ui \
	interface/modifyconstraintstudentssetintervalmaxdaysperweekform_template.ui \
	\
	interface/constraintstudentsintervalmaxdaysperweekform_template.ui \
	interface/addconstraintstudentsintervalmaxdaysperweekform_template.ui \
	interface/modifyconstraintstudentsintervalmaxdaysperweekform_template.ui \
	\
	interface/constraintstudentssetmaxdaysperweekform_template.ui \
	interface/addconstraintstudentssetmaxdaysperweekform_template.ui \
	interface/modifyconstraintstudentssetmaxdaysperweekform_template.ui \
	\
	interface/constraintstudentsmaxdaysperweekform_template.ui \
	interface/addconstraintstudentsmaxdaysperweekform_template.ui \
	interface/modifyconstraintstudentsmaxdaysperweekform_template.ui \
	\
	interface/constraintteachersintervalmaxdaysperweekform_template.ui \
	interface/addconstraintteachersintervalmaxdaysperweekform_template.ui \
	interface/modifyconstraintteachersintervalmaxdaysperweekform_template.ui \
	\
	interface/constraintteachersmaxhoursdailyform_template.ui \
	interface/addconstraintteachersmaxhoursdailyform_template.ui \
	interface/modifyconstraintteachersmaxhoursdailyform_template.ui \
	interface/constraintteachermaxhoursdailyform_template.ui \
	interface/addconstraintteachermaxhoursdailyform_template.ui \
	interface/modifyconstraintteachermaxhoursdailyform_template.ui \
	\
	interface/constraintteachersmaxhourscontinuouslyform_template.ui \
	interface/addconstraintteachersmaxhourscontinuouslyform_template.ui \
	interface/modifyconstraintteachersmaxhourscontinuouslyform_template.ui \
	interface/constraintteachermaxhourscontinuouslyform_template.ui \
	interface/addconstraintteachermaxhourscontinuouslyform_template.ui \
	interface/modifyconstraintteachermaxhourscontinuouslyform_template.ui \
	\
	interface/constraintteachersactivitytagmaxhourscontinuouslyform_template.ui \
	interface/addconstraintteachersactivitytagmaxhourscontinuouslyform_template.ui \
	interface/modifyconstraintteachersactivitytagmaxhourscontinuouslyform_template.ui \
	interface/constraintteacheractivitytagmaxhourscontinuouslyform_template.ui \
	interface/addconstraintteacheractivitytagmaxhourscontinuouslyform_template.ui \
	interface/modifyconstraintteacheractivitytagmaxhourscontinuouslyform_template.ui \
	\
	interface/constraintteachersactivitytagmaxhoursdailyform_template.ui \
	interface/addconstraintteachersactivitytagmaxhoursdailyform_template.ui \
	interface/modifyconstraintteachersactivitytagmaxhoursdailyform_template.ui \
	interface/constraintteacheractivitytagmaxhoursdailyform_template.ui \
	interface/addconstraintteacheractivitytagmaxhoursdailyform_template.ui \
	interface/modifyconstraintteacheractivitytagmaxhoursdailyform_template.ui \
	\
	interface/constraintteachersactivitytagminhoursdailyform_template.ui \
	interface/addconstraintteachersactivitytagminhoursdailyform_template.ui \
	interface/modifyconstraintteachersactivitytagminhoursdailyform_template.ui \
	interface/constraintteacheractivitytagminhoursdailyform_template.ui \
	interface/addconstraintteacheractivitytagminhoursdailyform_template.ui \
	interface/modifyconstraintteacheractivitytagminhoursdailyform_template.ui \
	\
	interface/constraintteachersminhoursdailyform_template.ui \
	interface/addconstraintteachersminhoursdailyform_template.ui \
	interface/modifyconstraintteachersminhoursdailyform_template.ui \
	interface/constraintteacherminhoursdailyform_template.ui \
	interface/addconstraintteacherminhoursdailyform_template.ui \
	interface/modifyconstraintteacherminhoursdailyform_template.ui \
	interface/constraintactivitypreferredstartingtimeform_template.ui \
	interface/addconstraintactivitypreferredstartingtimeform_template.ui \
	interface/modifyconstraintactivitypreferredstartingtimeform_template.ui \
	\
	interface/constraintactivityendsstudentsdayform_template.ui \
	interface/addconstraintactivityendsstudentsdayform_template.ui \
	interface/modifyconstraintactivityendsstudentsdayform_template.ui \
	\
	interface/constraintactivitiesendstudentsdayform_template.ui \
	interface/addconstraintactivitiesendstudentsdayform_template.ui \
	interface/modifyconstraintactivitiesendstudentsdayform_template.ui \
	\
	interface/constraintactivityendsteachersdayform_template.ui \
	interface/addconstraintactivityendsteachersdayform_template.ui \
	interface/modifyconstraintactivityendsteachersdayform_template.ui \
	\
	interface/constraintactivitiesendteachersdayform_template.ui \
	interface/addconstraintactivitiesendteachersdayform_template.ui \
	interface/modifyconstraintactivitiesendteachersdayform_template.ui \
	\
	interface/constraintactivitybeginsstudentsdayform_template.ui \
	interface/addconstraintactivitybeginsstudentsdayform_template.ui \
	interface/modifyconstraintactivitybeginsstudentsdayform_template.ui \
	\
	interface/constraintactivitiesbeginstudentsdayform_template.ui \
	interface/addconstraintactivitiesbeginstudentsdayform_template.ui \
	interface/modifyconstraintactivitiesbeginstudentsdayform_template.ui \
	\
	interface/constraintactivitybeginsteachersdayform_template.ui \
	interface/addconstraintactivitybeginsteachersdayform_template.ui \
	interface/modifyconstraintactivitybeginsteachersdayform_template.ui \
	\
	interface/constraintactivitiesbeginteachersdayform_template.ui \
	interface/addconstraintactivitiesbeginteachersdayform_template.ui \
	interface/modifyconstraintactivitiesbeginteachersdayform_template.ui \
	\
	interface/constraintstudentssetmaxgapsperweekform_template.ui \
	interface/addconstraintstudentssetmaxgapsperweekform_template.ui \
	interface/modifyconstraintstudentssetmaxgapsperweekform_template.ui \
	interface/constraintstudentsmaxgapsperweekform_template.ui \
	interface/addconstraintstudentsmaxgapsperweekform_template.ui \
	interface/modifyconstraintstudentsmaxgapsperweekform_template.ui \
	\
	interface/constraintstudentssetmaxgapsperdayform_template.ui \
	interface/addconstraintstudentssetmaxgapsperdayform_template.ui \
	interface/modifyconstraintstudentssetmaxgapsperdayform_template.ui \
	interface/constraintstudentsmaxgapsperdayform_template.ui \
	interface/addconstraintstudentsmaxgapsperdayform_template.ui \
	interface/modifyconstraintstudentsmaxgapsperdayform_template.ui \
	\
	interface/constraintteachersmaxgapsperweekform_template.ui \
	interface/addconstraintteachersmaxgapsperweekform_template.ui \
	interface/modifyconstraintteachersmaxgapsperweekform_template.ui \
	interface/constraintteachermaxgapsperweekform_template.ui \
	interface/addconstraintteachermaxgapsperweekform_template.ui \
	interface/modifyconstraintteachermaxgapsperweekform_template.ui \
	interface/constraintteachersmaxgapsperdayform_template.ui \
	interface/addconstraintteachersmaxgapsperdayform_template.ui \
	interface/modifyconstraintteachersmaxgapsperdayform_template.ui \
	interface/constraintteachermaxgapsperdayform_template.ui \
	interface/addconstraintteachermaxgapsperdayform_template.ui \
	interface/modifyconstraintteachermaxgapsperdayform_template.ui \
	\
	interface/constraintteachersmaxgapspermorningandafternoonform_template.ui \
	interface/constraintteachermaxgapspermorningandafternoonform_template.ui \
	interface/addconstraintteachersmaxgapspermorningandafternoonform_template.ui \
	interface/modifyconstraintteachersmaxgapspermorningandafternoonform_template.ui \
	interface/addconstraintteachermaxgapspermorningandafternoonform_template.ui \
	interface/modifyconstraintteachermaxgapspermorningandafternoonform_template.ui \
	\
	interface/constraintstudentsearlymaxbeginningsatsecondhourform_template.ui \
	interface/addconstraintstudentsearlymaxbeginningsatsecondhourform_template.ui \
	interface/modifyconstraintstudentsearlymaxbeginningsatsecondhourform_template.ui \
	interface/constraintstudentssetearlymaxbeginningsatsecondhourform_template.ui \
	interface/addconstraintstudentssetearlymaxbeginningsatsecondhourform_template.ui \
	interface/modifyconstraintstudentssetearlymaxbeginningsatsecondhourform_template.ui \
	\
	interface/constraintstudentssetmaxhoursdailyform_template.ui \
	interface/addconstraintstudentssetmaxhoursdailyform_template.ui \
	interface/modifyconstraintstudentssetmaxhoursdailyform_template.ui \
	interface/constraintstudentsmaxhoursdailyform_template.ui \
	interface/addconstraintstudentsmaxhoursdailyform_template.ui \
	interface/modifyconstraintstudentsmaxhoursdailyform_template.ui \
	\
	interface/constraintstudentssetmaxhourscontinuouslyform_template.ui \
	interface/addconstraintstudentssetmaxhourscontinuouslyform_template.ui \
	interface/modifyconstraintstudentssetmaxhourscontinuouslyform_template.ui \
	interface/constraintstudentsmaxhourscontinuouslyform_template.ui \
	interface/addconstraintstudentsmaxhourscontinuouslyform_template.ui \
	interface/modifyconstraintstudentsmaxhourscontinuouslyform_template.ui \
	\
	interface/constraintstudentssetactivitytagmaxhourscontinuouslyform_template.ui \
	interface/addconstraintstudentssetactivitytagmaxhourscontinuouslyform_template.ui \
	interface/modifyconstraintstudentssetactivitytagmaxhourscontinuouslyform_template.ui \
	interface/constraintstudentsactivitytagmaxhourscontinuouslyform_template.ui \
	interface/addconstraintstudentsactivitytagmaxhourscontinuouslyform_template.ui \
	interface/modifyconstraintstudentsactivitytagmaxhourscontinuouslyform_template.ui \
	\
	interface/constraintstudentssetactivitytagmaxhoursdailyform_template.ui \
	interface/addconstraintstudentssetactivitytagmaxhoursdailyform_template.ui \
	interface/modifyconstraintstudentssetactivitytagmaxhoursdailyform_template.ui \
	interface/constraintstudentsactivitytagmaxhoursdailyform_template.ui \
	interface/addconstraintstudentsactivitytagmaxhoursdailyform_template.ui \
	interface/modifyconstraintstudentsactivitytagmaxhoursdailyform_template.ui \
	\
	interface/constraintstudentssetactivitytagminhoursdailyform_template.ui \
	interface/addconstraintstudentssetactivitytagminhoursdailyform_template.ui \
	interface/modifyconstraintstudentssetactivitytagminhoursdailyform_template.ui \
	interface/constraintstudentsactivitytagminhoursdailyform_template.ui \
	interface/addconstraintstudentsactivitytagminhoursdailyform_template.ui \
	interface/modifyconstraintstudentsactivitytagminhoursdailyform_template.ui \
	\
	interface/constraintstudentssetminhoursdailyform_template.ui \
	interface/addconstraintstudentssetminhoursdailyform_template.ui \
	interface/modifyconstraintstudentssetminhoursdailyform_template.ui \
	interface/constraintstudentsminhoursdailyform_template.ui \
	interface/addconstraintstudentsminhoursdailyform_template.ui \
	interface/modifyconstraintstudentsminhoursdailyform_template.ui \
	interface/constraintactivitiesnotoverlappingform_template.ui \
	interface/addconstraintactivitiesnotoverlappingform_template.ui \
	interface/modifyconstraintactivitiesnotoverlappingform_template.ui \
	\
	interface/constraintactivitytagsnotoverlappingform_template.ui \
	interface/addconstraintactivitytagsnotoverlappingform_template.ui \
	interface/modifyconstraintactivitytagsnotoverlappingform_template.ui \
	\
	interface/constraintactivitiesoccupymaxtimeslotsfromselectionform_template.ui \
	interface/addconstraintactivitiesoccupymaxtimeslotsfromselectionform_template.ui \
	interface/modifyconstraintactivitiesoccupymaxtimeslotsfromselectionform_template.ui \
	\
	interface/constraintactivitiesoccupymintimeslotsfromselectionform_template.ui \
	interface/addconstraintactivitiesoccupymintimeslotsfromselectionform_template.ui \
	interface/modifyconstraintactivitiesoccupymintimeslotsfromselectionform_template.ui \
	\
	interface/constraintactivitiesmaxsimultaneousinselectedtimeslotsform_template.ui \
	interface/addconstraintactivitiesmaxsimultaneousinselectedtimeslotsform_template.ui \
	interface/modifyconstraintactivitiesmaxsimultaneousinselectedtimeslotsform_template.ui \
	\
	interface/constraintactivitiesminsimultaneousinselectedtimeslotsform_template.ui \
	interface/addconstraintactivitiesminsimultaneousinselectedtimeslotsform_template.ui \
	interface/modifyconstraintactivitiesminsimultaneousinselectedtimeslotsform_template.ui \
	\
	interface/constraintactivitiesoccupymaxdifferentroomsform_template.ui \
	interface/addconstraintactivitiesoccupymaxdifferentroomsform_template.ui \
	interface/modifyconstraintactivitiesoccupymaxdifferentroomsform_template.ui \
	\
	interface/constraintactivitiessameroomifconsecutiveform_template.ui \
	interface/addconstraintactivitiessameroomifconsecutiveform_template.ui \
	interface/modifyconstraintactivitiessameroomifconsecutiveform_template.ui \
	\
	interface/constrainttwoactivitiesconsecutiveform_template.ui \
	interface/addconstrainttwoactivitiesconsecutiveform_template.ui \
	interface/modifyconstrainttwoactivitiesconsecutiveform_template.ui \
	\
	interface/constrainttwoactivitiesgroupedform_template.ui \
	interface/addconstrainttwoactivitiesgroupedform_template.ui \
	interface/modifyconstrainttwoactivitiesgroupedform_template.ui \
	\
	interface/constraintthreeactivitiesgroupedform_template.ui \
	interface/addconstraintthreeactivitiesgroupedform_template.ui \
	interface/modifyconstraintthreeactivitiesgroupedform_template.ui \
	\
	interface/constrainttwoactivitiesorderedform_template.ui \
	interface/addconstrainttwoactivitiesorderedform_template.ui \
	interface/modifyconstrainttwoactivitiesorderedform_template.ui \
	\
	interface/constrainttwosetsofactivitiesorderedform_template.ui \
	interface/addconstrainttwosetsofactivitiesorderedform_template.ui \
	interface/modifyconstrainttwosetsofactivitiesorderedform_template.ui \
	\
	interface/constrainttwoactivitiesorderedifsamedayform_template.ui \
	interface/addconstrainttwoactivitiesorderedifsamedayform_template.ui \
	interface/modifyconstrainttwoactivitiesorderedifsamedayform_template.ui \
	\
	interface/constraintmindaysbetweenactivitiesform_template.ui \
	interface/addconstraintmindaysbetweenactivitiesform_template.ui \
	interface/modifyconstraintmindaysbetweenactivitiesform_template.ui \
	\
	interface/constraintminhalfdaysbetweenactivitiesform_template.ui \
	interface/addconstraintminhalfdaysbetweenactivitiesform_template.ui \
	interface/modifyconstraintminhalfdaysbetweenactivitiesform_template.ui \
	\
	interface/groupactivitiesininitialorderitemsform_template.ui \
	interface/addgroupactivitiesininitialorderitemform_template.ui \
	interface/modifygroupactivitiesininitialorderitemform_template.ui \
	\
	interface/constraintmaxdaysbetweenactivitiesform_template.ui \
	interface/addconstraintmaxdaysbetweenactivitiesform_template.ui \
	interface/modifyconstraintmaxdaysbetweenactivitiesform_template.ui \
	\
	interface/constraintactivitiesmaxhourlyspanform_template.ui \
	interface/addconstraintactivitiesmaxhourlyspanform_template.ui \
	interface/modifyconstraintactivitiesmaxhourlyspanform_template.ui \
	\
	interface/constraintmaxhalfdaysbetweenactivitiesform_template.ui \
	interface/addconstraintmaxhalfdaysbetweenactivitiesform_template.ui \
	interface/modifyconstraintmaxhalfdaysbetweenactivitiesform_template.ui \
	\
	interface/constraintmaxtermsbetweenactivitiesform_template.ui \
	interface/addconstraintmaxtermsbetweenactivitiesform_template.ui \
	interface/modifyconstraintmaxtermsbetweenactivitiesform_template.ui \
	\
	interface/constraintmingapsbetweenactivitiesform_template.ui \
	interface/addconstraintmingapsbetweenactivitiesform_template.ui \
	interface/modifyconstraintmingapsbetweenactivitiesform_template.ui \
	\
	interface/constraintactivitypreferredtimeslotsform_template.ui \
	interface/modifyconstraintactivitiespreferredtimeslotsform_template.ui \
	interface/constraintactivitypreferredstartingtimesform_template.ui \
	interface/modifyconstraintactivitiespreferredstartingtimesform_template.ui \
	interface/modifyconstraintactivitiessamestartingtimeform_template.ui \
	interface/modifyconstraintactivitiessamestartinghourform_template.ui \
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
	interface/constraintteachermaxspanperdayform_template.ui \
	interface/addconstraintteachermaxspanperdayform_template.ui \
	interface/modifyconstraintteachermaxspanperdayform_template.ui \
	interface/constraintteachersmaxspanperdayform_template.ui \
	interface/addconstraintteachersmaxspanperdayform_template.ui \
	interface/modifyconstraintteachersmaxspanperdayform_template.ui \
	\
	interface/constraintstudentssetmaxspanperdayform_template.ui \
	interface/addconstraintstudentssetmaxspanperdayform_template.ui \
	interface/modifyconstraintstudentssetmaxspanperdayform_template.ui \
	interface/constraintstudentsmaxspanperdayform_template.ui \
	interface/addconstraintstudentsmaxspanperdayform_template.ui \
	interface/modifyconstraintstudentsmaxspanperdayform_template.ui \
	\
	interface/constraintteacherminrestinghoursform_template.ui \
	interface/addconstraintteacherminrestinghoursform_template.ui \
	interface/modifyconstraintteacherminrestinghoursform_template.ui \
	interface/constraintteachersminrestinghoursform_template.ui \
	interface/addconstraintteachersminrestinghoursform_template.ui \
	interface/modifyconstraintteachersminrestinghoursform_template.ui \
	\
	interface/constraintstudentssetminrestinghoursform_template.ui \
	interface/addconstraintstudentssetminrestinghoursform_template.ui \
	interface/modifyconstraintstudentssetminrestinghoursform_template.ui \
	interface/constraintstudentsminrestinghoursform_template.ui \
	interface/addconstraintstudentsminrestinghoursform_template.ui \
	interface/modifyconstraintstudentsminrestinghoursform_template.ui \
	\
	interface/studentscomboboxesstyleform_template.ui \
	\
	interface/addconstraintstudentssetmingapsbetweenorderedpairofactivitytagsform_template.ui \
	interface/constraintstudentssetmingapsbetweenorderedpairofactivitytagsform_template.ui \
	interface/modifyconstraintstudentssetmingapsbetweenorderedpairofactivitytagsform_template.ui \
	interface/addconstraintstudentsmingapsbetweenorderedpairofactivitytagsform_template.ui \
	interface/constraintstudentsmingapsbetweenorderedpairofactivitytagsform_template.ui \
	interface/modifyconstraintstudentsmingapsbetweenorderedpairofactivitytagsform_template.ui \
	interface/addconstraintteachermingapsbetweenorderedpairofactivitytagsform_template.ui \
	interface/constraintteachermingapsbetweenorderedpairofactivitytagsform_template.ui \
	interface/modifyconstraintteachermingapsbetweenorderedpairofactivitytagsform_template.ui \
	interface/addconstraintteachersmingapsbetweenorderedpairofactivitytagsform_template.ui \
	interface/constraintteachersmingapsbetweenorderedpairofactivitytagsform_template.ui \
	interface/modifyconstraintteachersmingapsbetweenorderedpairofactivitytagsform_template.ui \
	\
	interface/addconstraintstudentssetmingapsbetweenactivitytagform_template.ui \
	interface/constraintstudentssetmingapsbetweenactivitytagform_template.ui \
	interface/modifyconstraintstudentssetmingapsbetweenactivitytagform_template.ui \
	interface/addconstraintstudentsmingapsbetweenactivitytagform_template.ui \
	interface/constraintstudentsmingapsbetweenactivitytagform_template.ui \
	interface/modifyconstraintstudentsmingapsbetweenactivitytagform_template.ui \
	interface/addconstraintteachermingapsbetweenactivitytagform_template.ui \
	interface/constraintteachermingapsbetweenactivitytagform_template.ui \
	interface/modifyconstraintteachermingapsbetweenactivitytagform_template.ui \
	interface/addconstraintteachersmingapsbetweenactivitytagform_template.ui \
	interface/constraintteachersmingapsbetweenactivitytagform_template.ui \
	interface/modifyconstraintteachersmingapsbetweenactivitytagform_template.ui \
	\
	interface/notificationcommandform_template.ui \
	\
	interface/helpmoroccoform_template.ui \
	interface/helpalgeriaform_template.ui \
	\
	interface/addconstraintstudentssetmaxroomchangesperrealdayform_template.ui \
	interface/constraintstudentssetmaxroomchangesperrealdayform_template.ui \
	interface/modifyconstraintstudentssetmaxroomchangesperrealdayform_template.ui \
	interface/addconstraintstudentsmaxroomchangesperrealdayform_template.ui \
	interface/constraintstudentsmaxroomchangesperrealdayform_template.ui \
	interface/modifyconstraintstudentsmaxroomchangesperrealdayform_template.ui \
	\
	interface/addconstraintteachermaxroomchangesperrealdayform_template.ui \
	interface/constraintteachermaxroomchangesperrealdayform_template.ui \
	interface/modifyconstraintteachermaxroomchangesperrealdayform_template.ui \
	interface/addconstraintteachersmaxroomchangesperrealdayform_template.ui \
	interface/constraintteachersmaxroomchangesperrealdayform_template.ui \
	interface/modifyconstraintteachersmaxroomchangesperrealdayform_template.ui \
	\
	interface/addconstraintstudentssetmaxbuildingchangesperrealdayform_template.ui \
	interface/constraintstudentssetmaxbuildingchangesperrealdayform_template.ui \
	interface/modifyconstraintstudentssetmaxbuildingchangesperrealdayform_template.ui \
	interface/addconstraintstudentsmaxbuildingchangesperrealdayform_template.ui \
	interface/constraintstudentsmaxbuildingchangesperrealdayform_template.ui \
	interface/modifyconstraintstudentsmaxbuildingchangesperrealdayform_template.ui \
	\
	interface/addconstraintteachermaxbuildingchangesperrealdayform_template.ui \
	interface/constraintteachermaxbuildingchangesperrealdayform_template.ui \
	interface/modifyconstraintteachermaxbuildingchangesperrealdayform_template.ui \
	interface/addconstraintteachersmaxbuildingchangesperrealdayform_template.ui \
	interface/constraintteachersmaxbuildingchangesperrealdayform_template.ui \
	interface/modifyconstraintteachersmaxbuildingchangesperrealdayform_template.ui \
	\
	interface/constraintteachermaxafternoonsperweekform_template.ui \
	interface/addconstraintteachermaxafternoonsperweekform_template.ui \
	interface/modifyconstraintteachermaxafternoonsperweekform_template.ui \
	interface/constraintteachersmaxafternoonsperweekform_template.ui \
	interface/addconstraintteachersmaxafternoonsperweekform_template.ui \
	interface/modifyconstraintteachersmaxafternoonsperweekform_template.ui \
	\
	interface/constraintteachermaxmorningsperweekform_template.ui \
	interface/addconstraintteachermaxmorningsperweekform_template.ui \
	interface/modifyconstraintteachermaxmorningsperweekform_template.ui \
	interface/constraintteachersmaxmorningsperweekform_template.ui \
	interface/addconstraintteachersmaxmorningsperweekform_template.ui \
	interface/modifyconstraintteachersmaxmorningsperweekform_template.ui \
	\
	interface/constraintteachermaxtwoconsecutivemorningsform_template.ui \
	interface/addconstraintteachermaxtwoconsecutivemorningsform_template.ui \
	interface/modifyconstraintteachermaxtwoconsecutivemorningsform_template.ui \
	interface/constraintteachersmaxtwoconsecutivemorningsform_template.ui \
	interface/addconstraintteachersmaxtwoconsecutivemorningsform_template.ui \
	interface/modifyconstraintteachersmaxtwoconsecutivemorningsform_template.ui \
	\
	interface/constraintteachermaxtwoconsecutiveafternoonsform_template.ui \
	interface/addconstraintteachermaxtwoconsecutiveafternoonsform_template.ui \
	interface/modifyconstraintteachermaxtwoconsecutiveafternoonsform_template.ui \
	interface/constraintteachersmaxtwoconsecutiveafternoonsform_template.ui \
	interface/addconstraintteachersmaxtwoconsecutiveafternoonsform_template.ui \
	interface/modifyconstraintteachersmaxtwoconsecutiveafternoonsform_template.ui \
	\
	interface/constraintteacherminmorningsperweekform_template.ui \
	interface/addconstraintteacherminmorningsperweekform_template.ui \
	interface/modifyconstraintteacherminmorningsperweekform_template.ui \
	\
	interface/constraintteacherminafternoonsperweekform_template.ui \
	interface/addconstraintteacherminafternoonsperweekform_template.ui \
	interface/modifyconstraintteacherminafternoonsperweekform_template.ui \
	\
	interface/constraintteachersminmorningsperweekform_template.ui \
	interface/addconstraintteachersminmorningsperweekform_template.ui \
	interface/modifyconstraintteachersminmorningsperweekform_template.ui \
	\
	interface/constraintteachersminafternoonsperweekform_template.ui \
	interface/addconstraintteachersminafternoonsperweekform_template.ui \
	interface/modifyconstraintteachersminafternoonsperweekform_template.ui \
	\
	interface/constraintteachermorningintervalmaxdaysperweekform_template.ui \
	interface/addconstraintteachermorningintervalmaxdaysperweekform_template.ui \
	interface/modifyconstraintteachermorningintervalmaxdaysperweekform_template.ui \
	\
	interface/constraintteacherafternoonintervalmaxdaysperweekform_template.ui \
	interface/addconstraintteacherafternoonintervalmaxdaysperweekform_template.ui \
	interface/modifyconstraintteacherafternoonintervalmaxdaysperweekform_template.ui \
	\
	interface/constraintstudentssetmorningintervalmaxdaysperweekform_template.ui \
	interface/addconstraintstudentssetmorningintervalmaxdaysperweekform_template.ui \
	interface/modifyconstraintstudentssetmorningintervalmaxdaysperweekform_template.ui \
	\
	interface/constraintstudentsmorningintervalmaxdaysperweekform_template.ui \
	interface/addconstraintstudentsmorningintervalmaxdaysperweekform_template.ui \
	interface/modifyconstraintstudentsmorningintervalmaxdaysperweekform_template.ui \
	\
	interface/constraintstudentssetafternoonintervalmaxdaysperweekform_template.ui \
	interface/addconstraintstudentssetafternoonintervalmaxdaysperweekform_template.ui \
	interface/modifyconstraintstudentssetafternoonintervalmaxdaysperweekform_template.ui \
	\
	interface/constraintstudentsafternoonintervalmaxdaysperweekform_template.ui \
	interface/addconstraintstudentsafternoonintervalmaxdaysperweekform_template.ui \
	interface/modifyconstraintstudentsafternoonintervalmaxdaysperweekform_template.ui \
	\
	interface/constraintstudentssetmaxafternoonsperweekform_template.ui \
	interface/addconstraintstudentssetmaxafternoonsperweekform_template.ui \
	interface/modifyconstraintstudentssetmaxafternoonsperweekform_template.ui \
	\
	interface/constraintstudentsmaxafternoonsperweekform_template.ui \
	interface/addconstraintstudentsmaxafternoonsperweekform_template.ui \
	interface/modifyconstraintstudentsmaxafternoonsperweekform_template.ui \
	\
	interface/constraintstudentssetmaxmorningsperweekform_template.ui \
	interface/addconstraintstudentssetmaxmorningsperweekform_template.ui \
	interface/modifyconstraintstudentssetmaxmorningsperweekform_template.ui \
	\
	interface/constraintstudentsmaxmorningsperweekform_template.ui \
	interface/addconstraintstudentsmaxmorningsperweekform_template.ui \
	interface/modifyconstraintstudentsmaxmorningsperweekform_template.ui \
	\
	interface/constraintstudentssetminafternoonsperweekform_template.ui \
	interface/addconstraintstudentssetminafternoonsperweekform_template.ui \
	interface/modifyconstraintstudentssetminafternoonsperweekform_template.ui \
	\
	interface/constraintstudentsminafternoonsperweekform_template.ui \
	interface/addconstraintstudentsminafternoonsperweekform_template.ui \
	interface/modifyconstraintstudentsminafternoonsperweekform_template.ui \
	\
	interface/constraintstudentssetminmorningsperweekform_template.ui \
	interface/addconstraintstudentssetminmorningsperweekform_template.ui \
	interface/modifyconstraintstudentssetminmorningsperweekform_template.ui \
	\
	interface/constraintstudentsminmorningsperweekform_template.ui \
	interface/addconstraintstudentsminmorningsperweekform_template.ui \
	interface/modifyconstraintstudentsminmorningsperweekform_template.ui \
	\
	interface/constraintteachersmorningintervalmaxdaysperweekform_template.ui \
	interface/addconstraintteachersmorningintervalmaxdaysperweekform_template.ui \
	interface/modifyconstraintteachersmorningintervalmaxdaysperweekform_template.ui \
	\
	interface/constraintteachersafternoonintervalmaxdaysperweekform_template.ui \
	interface/addconstraintteachersafternoonintervalmaxdaysperweekform_template.ui \
	interface/modifyconstraintteachersafternoonintervalmaxdaysperweekform_template.ui \
	\
	interface/constraintteachermaxhoursperallafternoonsform_template.ui \
	interface/addconstraintteachermaxhoursperallafternoonsform_template.ui \
	interface/modifyconstraintteachermaxhoursperallafternoonsform_template.ui \
	\
	interface/constraintteachersmaxhoursperallafternoonsform_template.ui \
	interface/addconstraintteachersmaxhoursperallafternoonsform_template.ui \
	interface/modifyconstraintteachersmaxhoursperallafternoonsform_template.ui \
	\
	interface/constraintstudentssetmaxhoursperallafternoonsform_template.ui \
	interface/addconstraintstudentssetmaxhoursperallafternoonsform_template.ui \
	interface/modifyconstraintstudentssetmaxhoursperallafternoonsform_template.ui \
	\
	interface/constraintstudentsmaxhoursperallafternoonsform_template.ui \
	interface/addconstraintstudentsmaxhoursperallafternoonsform_template.ui \
	interface/modifyconstraintstudentsmaxhoursperallafternoonsform_template.ui \
	\
	interface/constraintteachersminhourspermorningform_template.ui \
	interface/addconstraintteachersminhourspermorningform_template.ui \
	interface/modifyconstraintteachersminhourspermorningform_template.ui \
	interface/constraintteacherminhourspermorningform_template.ui \
	interface/addconstraintteacherminhourspermorningform_template.ui \
	interface/modifyconstraintteacherminhourspermorningform_template.ui \
	\
	interface/constraintteachersminhoursperafternoonform_template.ui \
	interface/addconstraintteachersminhoursperafternoonform_template.ui \
	interface/modifyconstraintteachersminhoursperafternoonform_template.ui \
	interface/constraintteacherminhoursperafternoonform_template.ui \
	interface/addconstraintteacherminhoursperafternoonform_template.ui \
	interface/modifyconstraintteacherminhoursperafternoonform_template.ui \
	\
	interface/constraintteachersminhoursdailyrealdaysform_template.ui \
	interface/addconstraintteachersminhoursdailyrealdaysform_template.ui \
	interface/modifyconstraintteachersminhoursdailyrealdaysform_template.ui \
	interface/constraintteacherminhoursdailyrealdaysform_template.ui \
	interface/addconstraintteacherminhoursdailyrealdaysform_template.ui \
	interface/modifyconstraintteacherminhoursdailyrealdaysform_template.ui \
	\
	interface/constraintstudentssetmaxgapsperrealdayform_template.ui \
	interface/addconstraintstudentssetmaxgapsperrealdayform_template.ui \
	interface/modifyconstraintstudentssetmaxgapsperrealdayform_template.ui \
	interface/constraintstudentsmaxgapsperrealdayform_template.ui \
	interface/addconstraintstudentsmaxgapsperrealdayform_template.ui \
	interface/modifyconstraintstudentsmaxgapsperrealdayform_template.ui \
	\
	interface/constraintteachersmaxzerogapsperafternoonform_template.ui \
	interface/constraintteachermaxzerogapsperafternoonform_template.ui \
	interface/addconstraintteachersmaxzerogapsperafternoonform_template.ui \
	interface/modifyconstraintteachersmaxzerogapsperafternoonform_template.ui \
	interface/addconstraintteachermaxzerogapsperafternoonform_template.ui \
	interface/modifyconstraintteachermaxzerogapsperafternoonform_template.ui \
	\
	interface/constraintteachersmaxgapsperrealdayform_template.ui \
	interface/constraintteachermaxgapsperrealdayform_template.ui \
	interface/addconstraintteachersmaxgapsperrealdayform_template.ui \
	interface/modifyconstraintteachersmaxgapsperrealdayform_template.ui \
	interface/addconstraintteachermaxgapsperrealdayform_template.ui \
	interface/modifyconstraintteachermaxgapsperrealdayform_template.ui \
	\
	interface/constraintstudentssetmaxgapsperweekforrealdaysform_template.ui \
	interface/addconstraintstudentssetmaxgapsperweekforrealdaysform_template.ui \
	interface/modifyconstraintstudentssetmaxgapsperweekforrealdaysform_template.ui \
	interface/constraintstudentsmaxgapsperweekforrealdaysform_template.ui \
	interface/addconstraintstudentsmaxgapsperweekforrealdaysform_template.ui \
	interface/modifyconstraintstudentsmaxgapsperweekforrealdaysform_template.ui \
	\
	interface/constraintteachersmaxgapsperweekforrealdaysform_template.ui \
	interface/constraintteachermaxgapsperweekforrealdaysform_template.ui \
	interface/addconstraintteachersmaxgapsperweekforrealdaysform_template.ui \
	interface/modifyconstraintteachersmaxgapsperweekforrealdaysform_template.ui \
	interface/addconstraintteachermaxgapsperweekforrealdaysform_template.ui \
	interface/modifyconstraintteachermaxgapsperweekforrealdaysform_template.ui \
	\
	interface/constraintstudentsafternoonsearlymaxbeginningsatsecondhourform_template.ui \
	interface/addconstraintstudentsafternoonsearlymaxbeginningsatsecondhourform_template.ui \
	interface/modifyconstraintstudentsafternoonsearlymaxbeginningsatsecondhourform_template.ui \
	interface/constraintstudentssetafternoonsearlymaxbeginningsatsecondhourform_template.ui \
	interface/addconstraintstudentssetafternoonsearlymaxbeginningsatsecondhourform_template.ui \
	interface/modifyconstraintstudentssetafternoonsearlymaxbeginningsatsecondhourform_template.ui \
	\
	interface/constraintteachersafternoonsearlymaxbeginningsatsecondhourform_template.ui \
	interface/addconstraintteachersafternoonsearlymaxbeginningsatsecondhourform_template.ui \
	interface/modifyconstraintteachersafternoonsearlymaxbeginningsatsecondhourform_template.ui \
	interface/constraintteacherafternoonsearlymaxbeginningsatsecondhourform_template.ui \
	interface/addconstraintteacherafternoonsearlymaxbeginningsatsecondhourform_template.ui \
	interface/modifyconstraintteacherafternoonsearlymaxbeginningsatsecondhourform_template.ui \
	\
	interface/constraintstudentsmorningsearlymaxbeginningsatsecondhourform_template.ui \
	interface/addconstraintstudentsmorningsearlymaxbeginningsatsecondhourform_template.ui \
	interface/modifyconstraintstudentsmorningsearlymaxbeginningsatsecondhourform_template.ui \
	interface/constraintstudentssetmorningsearlymaxbeginningsatsecondhourform_template.ui \
	interface/addconstraintstudentssetmorningsearlymaxbeginningsatsecondhourform_template.ui \
	interface/modifyconstraintstudentssetmorningsearlymaxbeginningsatsecondhourform_template.ui \
	\
	interface/constraintteachersmorningsearlymaxbeginningsatsecondhourform_template.ui \
	interface/addconstraintteachersmorningsearlymaxbeginningsatsecondhourform_template.ui \
	interface/modifyconstraintteachersmorningsearlymaxbeginningsatsecondhourform_template.ui \
	interface/constraintteachermorningsearlymaxbeginningsatsecondhourform_template.ui \
	interface/addconstraintteachermorningsearlymaxbeginningsatsecondhourform_template.ui \
	interface/modifyconstraintteachermorningsearlymaxbeginningsatsecondhourform_template.ui \
	\
	interface/constraintstudentssetminhourspermorningform_template.ui \
	interface/addconstraintstudentssetminhourspermorningform_template.ui \
	interface/modifyconstraintstudentssetminhourspermorningform_template.ui \
	interface/constraintstudentsminhourspermorningform_template.ui \
	interface/addconstraintstudentsminhourspermorningform_template.ui \
	interface/modifyconstraintstudentsminhourspermorningform_template.ui \
	\
	interface/constraintstudentssetminhoursperafternoonform_template.ui \
	interface/addconstraintstudentssetminhoursperafternoonform_template.ui \
	interface/modifyconstraintstudentssetminhoursperafternoonform_template.ui \
	interface/constraintstudentsminhoursperafternoonform_template.ui \
	interface/addconstraintstudentsminhoursperafternoonform_template.ui \
	interface/modifyconstraintstudentsminhoursperafternoonform_template.ui \
	\
	interface/constraintteachermaxtwoactivitytagsperdayfromn1n2n3form_template.ui \
	interface/addconstraintteachermaxtwoactivitytagsperdayfromn1n2n3form_template.ui \
	interface/modifyconstraintteachermaxtwoactivitytagsperdayfromn1n2n3form_template.ui \
	interface/constraintteachersmaxtwoactivitytagsperdayfromn1n2n3form_template.ui \
	interface/addconstraintteachersmaxtwoactivitytagsperdayfromn1n2n3form_template.ui \
	interface/modifyconstraintteachersmaxtwoactivitytagsperdayfromn1n2n3form_template.ui \
	\
	interface/constraintstudentssetmaxtwoactivitytagsperdayfromn1n2n3form_template.ui \
	interface/addconstraintstudentssetmaxtwoactivitytagsperdayfromn1n2n3form_template.ui \
	interface/modifyconstraintstudentssetmaxtwoactivitytagsperdayfromn1n2n3form_template.ui \
	interface/constraintstudentsmaxtwoactivitytagsperdayfromn1n2n3form_template.ui \
	interface/addconstraintstudentsmaxtwoactivitytagsperdayfromn1n2n3form_template.ui \
	interface/modifyconstraintstudentsmaxtwoactivitytagsperdayfromn1n2n3form_template.ui \
	\
	interface/constraintteachermaxtwoactivitytagsperrealdayfromn1n2n3form_template.ui \
	interface/addconstraintteachermaxtwoactivitytagsperrealdayfromn1n2n3form_template.ui \
	interface/modifyconstraintteachermaxtwoactivitytagsperrealdayfromn1n2n3form_template.ui \
	interface/constraintteachersmaxtwoactivitytagsperrealdayfromn1n2n3form_template.ui \
	interface/addconstraintteachersmaxtwoactivitytagsperrealdayfromn1n2n3form_template.ui \
	interface/modifyconstraintteachersmaxtwoactivitytagsperrealdayfromn1n2n3form_template.ui \
	\
	interface/constraintstudentssetmaxtwoactivitytagsperrealdayfromn1n2n3form_template.ui \
	interface/addconstraintstudentssetmaxtwoactivitytagsperrealdayfromn1n2n3form_template.ui \
	interface/modifyconstraintstudentssetmaxtwoactivitytagsperrealdayfromn1n2n3form_template.ui \
	interface/constraintstudentsmaxtwoactivitytagsperrealdayfromn1n2n3form_template.ui \
	interface/addconstraintstudentsmaxtwoactivitytagsperrealdayfromn1n2n3form_template.ui \
	interface/modifyconstraintstudentsmaxtwoactivitytagsperrealdayfromn1n2n3form_template.ui \
	\
	interface/constraintteacherminrestinghoursbetweenmorningandafternoonform_template.ui \
	interface/addconstraintteacherminrestinghoursbetweenmorningandafternoonform_template.ui \
	interface/modifyconstraintteacherminrestinghoursbetweenmorningandafternoonform_template.ui \
	interface/constraintteachersminrestinghoursbetweenmorningandafternoonform_template.ui \
	interface/addconstraintteachersminrestinghoursbetweenmorningandafternoonform_template.ui \
	interface/modifyconstraintteachersminrestinghoursbetweenmorningandafternoonform_template.ui \
	\
	interface/constraintstudentssetminrestinghoursbetweenmorningandafternoonform_template.ui \
	interface/addconstraintstudentssetminrestinghoursbetweenmorningandafternoonform_template.ui \
	interface/modifyconstraintstudentssetminrestinghoursbetweenmorningandafternoonform_template.ui \
	interface/constraintstudentsminrestinghoursbetweenmorningandafternoonform_template.ui \
	interface/addconstraintstudentsminrestinghoursbetweenmorningandafternoonform_template.ui \
	interface/modifyconstraintstudentsminrestinghoursbetweenmorningandafternoonform_template.ui \
	\
	interface/constraintteachersmaxhoursdailyrealdaysform_template.ui \
	interface/addconstraintteachersmaxhoursdailyrealdaysform_template.ui \
	interface/modifyconstraintteachersmaxhoursdailyrealdaysform_template.ui \
	interface/constraintteachermaxhoursdailyrealdaysform_template.ui \
	interface/addconstraintteachermaxhoursdailyrealdaysform_template.ui \
	interface/modifyconstraintteachermaxhoursdailyrealdaysform_template.ui \
	\
	interface/constraintteachersactivitytagmaxhoursdailyrealdaysform_template.ui \
	interface/addconstraintteachersactivitytagmaxhoursdailyrealdaysform_template.ui \
	interface/modifyconstraintteachersactivitytagmaxhoursdailyrealdaysform_template.ui \
	interface/constraintteacheractivitytagmaxhoursdailyrealdaysform_template.ui \
	interface/addconstraintteacheractivitytagmaxhoursdailyrealdaysform_template.ui \
	interface/modifyconstraintteacheractivitytagmaxhoursdailyrealdaysform_template.ui \
	\
	interface/constraintstudentssetmaxhoursdailyrealdaysform_template.ui \
	interface/addconstraintstudentssetmaxhoursdailyrealdaysform_template.ui \
	interface/modifyconstraintstudentssetmaxhoursdailyrealdaysform_template.ui \
	interface/constraintstudentsmaxhoursdailyrealdaysform_template.ui \
	interface/addconstraintstudentsmaxhoursdailyrealdaysform_template.ui \
	interface/modifyconstraintstudentsmaxhoursdailyrealdaysform_template.ui \
	\
	interface/constraintstudentssetactivitytagmaxhoursdailyrealdaysform_template.ui \
	interface/addconstraintstudentssetactivitytagmaxhoursdailyrealdaysform_template.ui \
	interface/modifyconstraintstudentssetactivitytagmaxhoursdailyrealdaysform_template.ui \
	interface/constraintstudentsactivitytagmaxhoursdailyrealdaysform_template.ui \
	interface/addconstraintstudentsactivitytagmaxhoursdailyrealdaysform_template.ui \
	interface/modifyconstraintstudentsactivitytagmaxhoursdailyrealdaysform_template.ui \
	\
	interface/constraintteachermaxrealdaysperweekform_template.ui \
	interface/addconstraintteachermaxrealdaysperweekform_template.ui \
	interface/modifyconstraintteachermaxrealdaysperweekform_template.ui \
	\
	interface/constraintteachersmaxrealdaysperweekform_template.ui \
	interface/addconstraintteachersmaxrealdaysperweekform_template.ui \
	interface/modifyconstraintteachersmaxrealdaysperweekform_template.ui \
	\
	interface/constraintstudentssetmaxrealdaysperweekform_template.ui \
	interface/addconstraintstudentssetmaxrealdaysperweekform_template.ui \
	interface/modifyconstraintstudentssetmaxrealdaysperweekform_template.ui \
	\
	interface/constraintstudentsmaxrealdaysperweekform_template.ui \
	interface/addconstraintstudentsmaxrealdaysperweekform_template.ui \
	interface/modifyconstraintstudentsmaxrealdaysperweekform_template.ui \
	\
	interface/constraintteacherminrealdaysperweekform_template.ui \
	interface/addconstraintteacherminrealdaysperweekform_template.ui \
	interface/modifyconstraintteacherminrealdaysperweekform_template.ui \
	\
	interface/constraintteachersminrealdaysperweekform_template.ui \
	interface/addconstraintteachersminrealdaysperweekform_template.ui \
	interface/modifyconstraintteachersminrealdaysperweekform_template.ui \
	\
	interface/constraintteachermaxspanperrealdayform_template.ui \
	interface/addconstraintteachermaxspanperrealdayform_template.ui \
	interface/modifyconstraintteachermaxspanperrealdayform_template.ui \
	interface/constraintteachersmaxspanperrealdayform_template.ui \
	interface/addconstraintteachersmaxspanperrealdayform_template.ui \
	interface/modifyconstraintteachersmaxspanperrealdayform_template.ui \
	\
	interface/constraintstudentssetmaxspanperrealdayform_template.ui \
	interface/addconstraintstudentssetmaxspanperrealdayform_template.ui \
	interface/modifyconstraintstudentssetmaxspanperrealdayform_template.ui \
	interface/constraintstudentsmaxspanperrealdayform_template.ui \
	interface/addconstraintstudentsmaxspanperrealdayform_template.ui \
	interface/modifyconstraintstudentsmaxspanperrealdayform_template.ui \
	\
	interface/constraintmaxtotalactivitiesfromsetinselectedtimeslotsform_template.ui \
	interface/addconstraintmaxtotalactivitiesfromsetinselectedtimeslotsform_template.ui \
	interface/modifyconstraintmaxtotalactivitiesfromsetinselectedtimeslotsform_template.ui \
	\
	interface/constraintmaxgapsbetweenactivitiesform_template.ui \
	interface/addconstraintmaxgapsbetweenactivitiesform_template.ui \
	interface/modifyconstraintmaxgapsbetweenactivitiesform_template.ui \
	\
	interface/constraintactivitiesmaxinatermform_template.ui \
	interface/addconstraintactivitiesmaxinatermform_template.ui \
	interface/modifyconstraintactivitiesmaxinatermform_template.ui \
	\
	interface/constraintactivitiesmininatermform_template.ui \
	interface/addconstraintactivitiesmininatermform_template.ui \
	interface/modifyconstraintactivitiesmininatermform_template.ui \
	\
	interface/constraintactivitiesoccupymaxtermsform_template.ui \
	interface/addconstraintactivitiesoccupymaxtermsform_template.ui \
	interface/modifyconstraintactivitiesoccupymaxtermsform_template.ui \
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
	interface/settingsautosaveform_template.ui

TRANSLATIONS += \
	../translations/fet_ar.ts \
	../translations/fet_bg.ts \
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

CONFIG += release warn_on c++17
QT += network

greaterThan(QT_MAJOR_VERSION, 4) {
	QT += widgets
	!isEmpty(QT.printsupport.name): QT += printsupport
}

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
