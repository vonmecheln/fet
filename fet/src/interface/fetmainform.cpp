//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2003 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "timetable_defs.h"
#include "timetable.h"
#include "solution.h"
#include "rules.h"

#include <iostream>
//for std::min
#include <algorithm>
//using namespace std;

#ifndef FET_COMMAND_LINE

#include <Qt>

#include "fetmainform.h"

#include "restoredatastateform.h"
#include "settingsrestoredatafrommemoryform.h"
#include "settingsrestoredatafromdiskform.h"

#include "settingsautosaveform.h"

#include "getmodefornewfileform.h"

#include "timetablegenerateform.h"
#include "timetablegeneratemultipleform.h"

#include "timetableviewstudentsdayshorizontalform.h"
#include "timetableviewstudentstimehorizontalform.h"
#include "timetableviewteachersdayshorizontalform.h"
#include "timetableviewteacherstimehorizontalform.h"
#include "timetableviewroomsdayshorizontalform.h"
#include "timetableviewroomstimehorizontalform.h"
#include "timetableshowconflictsform.h"
#include "timetableprintform.h"
#include "statisticsprintform.h"

#include "export.h"
#include "import.h"

#include "institutionnameform.h"
#include "commentsform.h"
#include "daysform.h"
#include "hoursform.h"
#include "subjectsform.h"
#include "subjectsstatisticsform.h"
#include "activitytagsform.h"
#include "teachersform.h"
#include "teachersstatisticsform.h"
#include "yearsform.h"
#include "splityearform.h"
#include "groupsform.h"
#include "subgroupsform.h"
#include "studentsstatisticsform.h"
#include "activitiesroomsstatisticsform.h"
#include "activitiesform.h"
#include "subactivitiesform.h"
#include "roomsform.h"
#include "buildingsform.h"
#include "alltimeconstraintsform.h"
#include "allspaceconstraintsform.h"
#include "helpaboutform.h"
#include "helpaboutlibrariesform.h"
#include "helpblockplanningform.h"
#include "helptermsform.h"
#include "helpfaqform.h"
#include "helptipsform.h"
#include "helpinstructionsform.h"
#include "helpmoroccoform.h"
#include "helpalgeriaform.h"

#include "randomseedform.h"

#include "fet.h"

#include "groupactivitiesininitialorderitemsform.h"

#include "constraintactivityendsstudentsdayform.h"
#include "constraintactivitiesendstudentsdayform.h"

#include "constraintactivityendsteachersdayform.h"
#include "constraintactivitiesendteachersdayform.h"

#include "constraintactivitybeginsstudentsdayform.h"
#include "constraintactivitiesbeginstudentsdayform.h"

#include "constraintactivitybeginsteachersdayform.h"
#include "constraintactivitiesbeginteachersdayform.h"

#include "constrainttwoactivitiesconsecutiveform.h"
#include "constrainttwoactivitiesgroupedform.h"
#include "constraintthreeactivitiesgroupedform.h"
#include "constrainttwoactivitiesorderedform.h"
#include "constrainttwosetsofactivitiesorderedform.h"
#include "constrainttwoactivitiesorderedifsamedayform.h"
#include "constraintactivitiespreferredtimeslotsform.h"
#include "constraintactivitiespreferredstartingtimesform.h"

#include "constraintsubactivitiespreferredtimeslotsform.h"
#include "constraintsubactivitiespreferredstartingtimesform.h"

#include "constraintactivitiessamestartingtimeform.h"
#include "constraintactivitiessamestartinghourform.h"
#include "constraintactivitiessamestartingdayform.h"

#include "constraintactivitiesoccupymaxtimeslotsfromselectionform.h"
#include "constraintactivitiesoccupymintimeslotsfromselectionform.h"
#include "constraintactivitiesmaxsimultaneousinselectedtimeslotsform.h"
#include "constraintactivitiesminsimultaneousinselectedtimeslotsform.h"

#include "constraintteachernotavailabletimesform.h"
#include "teachersnotavailabletimestimehorizontalform.h"
#include "constraintbasiccompulsorytimeform.h"
#include "constraintbasiccompulsoryspaceform.h"
#include "constraintroomnotavailabletimesform.h"
#include "constraintteacherroomnotavailabletimesform.h"
#include "constraintactivitypreferredroomform.h"
#include "constraintstudentssetnotavailabletimesform.h"
#include "studentsnotavailabletimestimehorizontalform.h"
#include "constraintbreaktimesform.h"
#include "constraintteachermaxdaysperweekform.h"
#include "constraintteachersmaxdaysperweekform.h"

#include "constraintteachermindaysperweekform.h"
#include "constraintteachersmindaysperweekform.h"

#include "constraintteacherintervalmaxdaysperweekform.h"
#include "constraintteachersintervalmaxdaysperweekform.h"
#include "constraintstudentssetintervalmaxdaysperweekform.h"
#include "constraintstudentsintervalmaxdaysperweekform.h"

#include "constraintstudentssetmaxdaysperweekform.h"
#include "constraintstudentsmaxdaysperweekform.h"

#include "constraintteachermaxhoursdailyform.h"
#include "constraintteachersmaxhoursdailyform.h"
#include "constraintteachermaxhourscontinuouslyform.h"
#include "constraintteachersmaxhourscontinuouslyform.h"

#include "constraintteachersmaxhoursdailyinintervalform.h"
#include "constraintteachermaxhoursdailyinintervalform.h"
#include "constraintstudentsmaxhoursdailyinintervalform.h"
#include "constraintstudentssetmaxhoursdailyinintervalform.h"

#include "constraintteacheractivitytagmaxhourscontinuouslyform.h"
#include "constraintteachersactivitytagmaxhourscontinuouslyform.h"

#include "constraintteacheractivitytagmaxhoursdailyform.h"
#include "constraintteachersactivitytagmaxhoursdailyform.h"

#include "constraintteacheractivitytagminhoursdailyform.h"
#include "constraintteachersactivitytagminhoursdailyform.h"

#include "constraintteacherminhoursdailyform.h"
#include "constraintteachersminhoursdailyform.h"
#include "constraintactivitypreferredstartingtimeform.h"
#include "constraintstudentssetmaxgapsperweekform.h"
#include "constraintstudentsmaxgapsperweekform.h"

#include "constraintstudentssetmaxgapsperdayform.h"
#include "constraintstudentsmaxgapsperdayform.h"

#include "constraintteachersmaxgapsperweekform.h"
#include "constraintteachermaxgapsperweekform.h"
#include "constraintteachersmaxgapsperdayform.h"
#include "constraintteachermaxgapsperdayform.h"

#include "constraintteachersmaxgapspermorningandafternoonform.h"
#include "constraintteachermaxgapspermorningandafternoonform.h"

#include "constraintstudentsearlymaxbeginningsatsecondhourform.h"
#include "constraintstudentssetearlymaxbeginningsatsecondhourform.h"
#include "constraintstudentssetmaxhoursdailyform.h"
#include "constraintstudentsmaxhoursdailyform.h"
#include "constraintstudentssetmaxhourscontinuouslyform.h"
#include "constraintstudentsmaxhourscontinuouslyform.h"

#include "constraintstudentssetactivitytagmaxhourscontinuouslyform.h"
#include "constraintstudentsactivitytagmaxhourscontinuouslyform.h"

#include "constraintstudentssetactivitytagmaxhoursdailyform.h"
#include "constraintstudentsactivitytagmaxhoursdailyform.h"

#include "constraintstudentssetactivitytagminhoursdailyform.h"
#include "constraintstudentsactivitytagminhoursdailyform.h"

#include "constraintstudentssetminhoursdailyform.h"
#include "constraintstudentsminhoursdailyform.h"
#include "constraintactivitiesnotoverlappingform.h"
#include "constraintactivitytagsnotoverlappingform.h"
#include "constraintmindaysbetweenactivitiesform.h"
#include "constraintminhalfdaysbetweenactivitiesform.h"
#include "constraintmaxdaysbetweenactivitiesform.h"
#include "constraintactivitiesmaxhourlyspanform.h"
#include "constraintmaxhalfdaysbetweenactivitiesform.h"
#include "constraintmaxtermsbetweenactivitiesform.h"
#include "constraintmingapsbetweenactivitiesform.h"
#include "constraintactivitypreferredtimeslotsform.h"
#include "constraintactivitypreferredstartingtimesform.h"

#include "constraintteachermaxspanperdayform.h"
#include "constraintteachersmaxspanperdayform.h"
#include "constraintstudentssetmaxspanperdayform.h"
#include "constraintstudentsmaxspanperdayform.h"

#include "constraintteacherminrestinghoursform.h"
#include "constraintteachersminrestinghoursform.h"
#include "constraintstudentssetminrestinghoursform.h"
#include "constraintstudentsminrestinghoursform.h"

#include "constraintactivitypreferredroomsform.h"

#include "constraintstudentssethomeroomform.h"
#include "constraintstudentssethomeroomsform.h"
#include "constraintteacherhomeroomform.h"
#include "constraintteacherhomeroomsform.h"

//2024-02-09
#include "constraintstudentssetmaxbuildingchangesperdayinintervalform.h"
#include "constraintstudentsmaxbuildingchangesperdayinintervalform.h"
#include "constraintteachermaxbuildingchangesperdayinintervalform.h"
#include "constraintteachersmaxbuildingchangesperdayinintervalform.h"
#include "constraintstudentssetmaxbuildingchangesperrealdayinintervalform.h"
#include "constraintstudentsmaxbuildingchangesperrealdayinintervalform.h"
#include "constraintteachermaxbuildingchangesperrealdayinintervalform.h"
#include "constraintteachersmaxbuildingchangesperrealdayinintervalform.h"

//2024-02-19
#include "constraintstudentssetmaxroomchangesperdayinintervalform.h"
#include "constraintstudentsmaxroomchangesperdayinintervalform.h"
#include "constraintteachermaxroomchangesperdayinintervalform.h"
#include "constraintteachersmaxroomchangesperdayinintervalform.h"
#include "constraintstudentssetmaxroomchangesperrealdayinintervalform.h"
#include "constraintstudentsmaxroomchangesperrealdayinintervalform.h"
#include "constraintteachermaxroomchangesperrealdayinintervalform.h"
#include "constraintteachersmaxroomchangesperrealdayinintervalform.h"

#include "constraintstudentssetmaxbuildingchangesperdayform.h"
#include "constraintstudentsmaxbuildingchangesperdayform.h"
#include "constraintstudentssetmaxbuildingchangesperweekform.h"
#include "constraintstudentsmaxbuildingchangesperweekform.h"
#include "constraintstudentssetmingapsbetweenbuildingchangesform.h"
#include "constraintstudentsmingapsbetweenbuildingchangesform.h"

#include "constraintteachermaxbuildingchangesperdayform.h"
#include "constraintteachersmaxbuildingchangesperdayform.h"
#include "constraintteachermaxbuildingchangesperweekform.h"
#include "constraintteachersmaxbuildingchangesperweekform.h"
#include "constraintteachermingapsbetweenbuildingchangesform.h"
#include "constraintteachersmingapsbetweenbuildingchangesform.h"

#include "constraintstudentssetmaxroomchangesperrealdayform.h"
#include "constraintstudentsmaxroomchangesperrealdayform.h"

#include "constraintstudentssetmaxbuildingchangesperrealdayform.h"
#include "constraintstudentsmaxbuildingchangesperrealdayform.h"

#include "constraintstudentssetmaxroomchangesperdayform.h"
#include "constraintstudentsmaxroomchangesperdayform.h"
#include "constraintstudentssetmaxroomchangesperweekform.h"
#include "constraintstudentsmaxroomchangesperweekform.h"
#include "constraintstudentssetmingapsbetweenroomchangesform.h"
#include "constraintstudentsmingapsbetweenroomchangesform.h"

#include "constraintteachermaxroomchangesperrealdayform.h"
#include "constraintteachersmaxroomchangesperrealdayform.h"

#include "constraintteachermaxbuildingchangesperrealdayform.h"
#include "constraintteachersmaxbuildingchangesperrealdayform.h"

#include "constraintteachermaxroomchangesperdayform.h"
#include "constraintteachersmaxroomchangesperdayform.h"
#include "constraintteachermaxroomchangesperweekform.h"
#include "constraintteachersmaxroomchangesperweekform.h"
#include "constraintteachermingapsbetweenroomchangesform.h"
#include "constraintteachersmingapsbetweenroomchangesform.h"

#include "constraintsubjectpreferredroomform.h"
#include "constraintsubjectpreferredroomsform.h"
#include "constraintsubjectactivitytagpreferredroomform.h"
#include "constraintsubjectactivitytagpreferredroomsform.h"

#include "constraintactivitytagpreferredroomform.h"
#include "constraintactivitytagpreferredroomsform.h"

#include "constraintactivitiesoccupymaxdifferentroomsform.h"
#include "constraintactivitiessameroomifconsecutiveform.h"

#include "constraintstudentssetmingapsbetweenorderedpairofactivitytagsform.h"
#include "constraintstudentsmingapsbetweenorderedpairofactivitytagsform.h"
#include "constraintteachermingapsbetweenorderedpairofactivitytagsform.h"
#include "constraintteachersmingapsbetweenorderedpairofactivitytagsform.h"

//2021-12-15
#include "constraintstudentssetmingapsbetweenactivitytagform.h"
#include "constraintstudentsmingapsbetweenactivitytagform.h"
#include "constraintteachermingapsbetweenactivitytagform.h"
#include "constraintteachersmingapsbetweenactivitytagform.h"

//2024-03-16
#include "constraintstudentssetmingapsbetweenorderedpairofactivitytagsperrealdayform.h"
#include "constraintstudentsmingapsbetweenorderedpairofactivitytagsperrealdayform.h"
#include "constraintteachermingapsbetweenorderedpairofactivitytagsperrealdayform.h"
#include "constraintteachersmingapsbetweenorderedpairofactivitytagsperrealdayform.h"

#include "constraintstudentssetmingapsbetweenactivitytagperrealdayform.h"
#include "constraintstudentsmingapsbetweenactivitytagperrealdayform.h"
#include "constraintteachermingapsbetweenactivitytagperrealdayform.h"
#include "constraintteachersmingapsbetweenactivitytagperrealdayform.h"

//2024-05-20
#include "constraintstudentssetmingapsbetweenorderedpairofactivitytagsbetweenmorningandafternoonform.h"
#include "constraintstudentsmingapsbetweenorderedpairofactivitytagsbetweenmorningandafternoonform.h"
#include "constraintteachermingapsbetweenorderedpairofactivitytagsbetweenmorningandafternoonform.h"
#include "constraintteachersmingapsbetweenorderedpairofactivitytagsbetweenmorningandafternoonform.h"

#include "constraintstudentssetmingapsbetweenactivitytagbetweenmorningandafternoonform.h"
#include "constraintstudentsmingapsbetweenactivitytagbetweenmorningandafternoonform.h"
#include "constraintteachermingapsbetweenactivitytagbetweenmorningandafternoonform.h"
#include "constraintteachersmingapsbetweenactivitytagbetweenmorningandafternoonform.h"

//mornings-afternoons
#include "constraintteachersmaxzerogapsperafternoonform.h"
#include "constraintteachermaxzerogapsperafternoonform.h"

#include "constraintteachersmaxgapsperrealdayform.h"
#include "constraintteachermaxgapsperrealdayform.h"

#include "constraintteachermaxrealdaysperweekform.h"
#include "constraintteachersmaxrealdaysperweekform.h"
#include "constraintteachermaxafternoonsperweekform.h"
#include "constraintteachersmaxafternoonsperweekform.h"
#include "constraintteachermaxmorningsperweekform.h"
#include "constraintteachersmaxmorningsperweekform.h"

#include "constraintteachermaxtwoconsecutivemorningsform.h"
#include "constraintteachersmaxtwoconsecutivemorningsform.h"
#include "constraintteachermaxtwoconsecutiveafternoonsform.h"
#include "constraintteachersmaxtwoconsecutiveafternoonsform.h"

#include "constraintteachersmaxhoursdailyrealdaysform.h"
#include "constraintteachermaxhoursdailyrealdaysform.h"

#include "constraintteachersminhoursdailyrealdaysform.h"
#include "constraintteacherminhoursdailyrealdaysform.h"

#include "constraintteachermaxhoursperallafternoonsform.h"
#include "constraintteachersmaxhoursperallafternoonsform.h"

#include "constraintstudentssetmaxhoursperallafternoonsform.h"
#include "constraintstudentsmaxhoursperallafternoonsform.h"

#include "constraintteachersminhourspermorningform.h"
#include "constraintteacherminhourspermorningform.h"

#include "constraintteachersminhoursperafternoonform.h"
#include "constraintteacherminhoursperafternoonform.h"

#include "constraintteachersminhoursdailyrealdaysform.h"
#include "constraintteacherminhoursdailyrealdaysform.h"

#include "constraintteacherminrealdaysperweekform.h"
#include "constraintteachersminrealdaysperweekform.h"

#include "constraintteacherminmorningsperweekform.h"
#include "constraintteachersminmorningsperweekform.h"
#include "constraintteacherminafternoonsperweekform.h"
#include "constraintteachersminafternoonsperweekform.h"

#include "constraintteachermaxtwoactivitytagsperdayfromn1n2n3form.h"
#include "constraintteachersmaxtwoactivitytagsperdayfromn1n2n3form.h"

#include "constraintstudentssetmaxtwoactivitytagsperdayfromn1n2n3form.h"
#include "constraintstudentsmaxtwoactivitytagsperdayfromn1n2n3form.h"

#include "constraintteachermaxtwoactivitytagsperrealdayfromn1n2n3form.h"
#include "constraintteachersmaxtwoactivitytagsperrealdayfromn1n2n3form.h"

#include "constraintstudentssetmaxtwoactivitytagsperrealdayfromn1n2n3form.h"
#include "constraintstudentsmaxtwoactivitytagsperrealdayfromn1n2n3form.h"

#include "constraintteachersactivitytagmaxhoursdailyrealdaysform.h"
#include "constraintteacheractivitytagmaxhoursdailyrealdaysform.h"

#include "constraintstudentssetmaxgapsperrealdayform.h"
#include "constraintstudentsmaxgapsperrealdayform.h"

#include "constraintteachersmaxgapsperweekforrealdaysform.h"
#include "constraintteachermaxgapsperweekforrealdaysform.h"
#include "constraintstudentssetmaxgapsperweekforrealdaysform.h"
#include "constraintstudentsmaxgapsperweekforrealdaysform.h"

#include "constraintstudentsafternoonsearlymaxbeginningsatsecondhourform.h"
#include "constraintstudentssetafternoonsearlymaxbeginningsatsecondhourform.h"

#include "constraintteachersafternoonsearlymaxbeginningsatsecondhourform.h"
#include "constraintteacherafternoonsearlymaxbeginningsatsecondhourform.h"

#include "constraintstudentsmorningsearlymaxbeginningsatsecondhourform.h"
#include "constraintstudentssetmorningsearlymaxbeginningsatsecondhourform.h"

#include "constraintteachersmorningsearlymaxbeginningsatsecondhourform.h"
#include "constraintteachermorningsearlymaxbeginningsatsecondhourform.h"

#include "constraintstudentssetmaxhoursdailyrealdaysform.h"
#include "constraintstudentsmaxhoursdailyrealdaysform.h"

#include "constraintstudentsminhourspermorningform.h"
#include "constraintstudentssetminhourspermorningform.h"

#include "constraintstudentsminhoursperafternoonform.h"
#include "constraintstudentssetminhoursperafternoonform.h"

#include "constraintstudentssetactivitytagmaxhoursdailyrealdaysform.h"
#include "constraintstudentsactivitytagmaxhoursdailyrealdaysform.h"

#include "constraintteachermorningintervalmaxdaysperweekform.h"
#include "constraintteachersmorningintervalmaxdaysperweekform.h"

#include "constraintteacherafternoonintervalmaxdaysperweekform.h"
#include "constraintteachersafternoonintervalmaxdaysperweekform.h"

#include "constraintstudentssetmorningintervalmaxdaysperweekform.h"
#include "constraintstudentsmorningintervalmaxdaysperweekform.h"
#include "constraintstudentssetafternoonintervalmaxdaysperweekform.h"
#include "constraintstudentsafternoonintervalmaxdaysperweekform.h"

#include "constraintstudentssetmaxrealdaysperweekform.h"
#include "constraintstudentsmaxrealdaysperweekform.h"

#include "constraintstudentssetmaxafternoonsperweekform.h"
#include "constraintstudentsmaxafternoonsperweekform.h"
#include "constraintstudentssetmaxmorningsperweekform.h"
#include "constraintstudentsmaxmorningsperweekform.h"

#include "constraintstudentssetminafternoonsperweekform.h"
#include "constraintstudentsminafternoonsperweekform.h"
#include "constraintstudentssetminmorningsperweekform.h"
#include "constraintstudentsminmorningsperweekform.h"

#include "constraintteachermaxspanperrealdayform.h"
#include "constraintteachersmaxspanperrealdayform.h"
#include "constraintstudentssetmaxspanperrealdayform.h"
#include "constraintstudentsmaxspanperrealdayform.h"

#include "constraintteacherminrestinghoursbetweenmorningandafternoonform.h"
#include "constraintteachersminrestinghoursbetweenmorningandafternoonform.h"
#include "constraintstudentssetminrestinghoursbetweenmorningandafternoonform.h"
#include "constraintstudentsminrestinghoursbetweenmorningandafternoonform.h"

#include "constraintteachermaxthreeconsecutivedaysform.h"
#include "constraintteachersmaxthreeconsecutivedaysform.h"

#include "constraintstudentssetmaxthreeconsecutivedaysform.h"
#include "constraintstudentsmaxthreeconsecutivedaysform.h"

//block-planning
#include "constraintmaxtotalactivitiesfromsetinselectedtimeslotsform.h"

#include "constraintmaxgapsbetweenactivitiesform.h"

//terms
#include "constraintactivitiesmaxinatermform.h"
#include "constraintactivitiesmininatermform.h"
#include "constraintactivitiesoccupymaxtermsform.h"

#include "termsform.h"

#include "settingstimetablehtmllevelform.h"

#include "settingsdatatoprintintimetablesform.h"

#include "notificationcommandform.h"

#include "activityplanningconfirmationform.h"
#include "activityplanningform.h"

#include "spreadconfirmationform.h"

#include "removeredundantconfirmationform.h"
#include "removeredundantform.h"

#include "savetimetableconfirmationform.h"

#include "timetablestowriteondiskform.h"

#include "studentscomboboxesstyleform.h"

#include "lockunlock.h"
#include "advancedlockunlockform.h"

#include "longtextmessagebox.h"

#include <QMessageBox>

#include <QFileInfo>
#include <QFile>

#include <QFileDialog>
#include <QString>
#include <QDir>
#include <QTranslator>
#include <QCloseEvent>
#include <QStatusBar>
#include <QMap>
#include <QWidget>
#include <QList>
#include <QHash>

#include <QPalette>
#include <QColor>

#include <QDesktopServices>
#include <QUrl>
#include <QApplication>
#include <QCoreApplication>

#include <QSysInfo>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QGuiApplication>
#include <QScreen>
#else
#include <QDesktopWidget>
#endif

#include <QCursor>
#include <QSettings>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSslSocket>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#else
#include <QRegExp>
#endif

//for the icons of not perfect constraints and for the application window icon
#include <QIcon>

#include <QRect>

#include <QFont>
#include <QFontDialog>

#include <list>
#include <iterator>

#include <QByteArray>
#include <QDataStream>

QRect mainFormSettingsRect;
int MAIN_FORM_SHORTCUTS_TAB_POSITION;

#include "spreadmindaysconstraintsfivedaysform.h"

#include "statisticsexport.h"

#endif

bool generation_running; //true if the user started a generation of the timetable

bool generation_running_multi;

bool students_schedule_ready;
bool teachers_schedule_ready;
bool rooms_buildings_schedule_ready;

Solution best_solution;

QString conflictsString; //the string that contains a log of the broken constraints
QString conflictsStringTitle;

bool TIMETABLES_SUBGROUPS_SORTED=false;

bool WRITE_TIMETABLE_CONFLICTS=true;

bool WRITE_TIMETABLES_STATISTICS=true;
bool WRITE_TIMETABLES_XML=true;
bool WRITE_TIMETABLES_DAYS_HORIZONTAL=true;
bool WRITE_TIMETABLES_DAYS_VERTICAL=true;
bool WRITE_TIMETABLES_TIME_HORIZONTAL=true;
bool WRITE_TIMETABLES_TIME_VERTICAL=true;

bool WRITE_TIMETABLES_SUBGROUPS=true;
bool WRITE_TIMETABLES_GROUPS=true;
bool WRITE_TIMETABLES_YEARS=true;
bool WRITE_TIMETABLES_TEACHERS=true;
bool WRITE_TIMETABLES_TEACHERS_FREE_PERIODS=true;
bool WRITE_TIMETABLES_BUILDINGS=true;
bool WRITE_TIMETABLES_ROOMS=true;
bool WRITE_TIMETABLES_SUBJECTS=true;
bool WRITE_TIMETABLES_ACTIVITY_TAGS=true;
bool WRITE_TIMETABLES_ACTIVITIES=true;

bool SHOW_VIRTUAL_ROOMS_IN_TIMETABLES=false;

#ifndef FET_COMMAND_LINE
extern QApplication* pqapplication;
#endif

Rules rules2;

#ifndef FET_COMMAND_LINE
static bool fontIsUserSelectable=false;
static bool userChoseAFont=false;
static QFont originalFont;

static int ORIGINAL_WIDTH;
static int ORIGINAL_HEIGHT;
//static int ORIGINAL_X;
//static int ORIGINAL_Y;

const QString COMPANY=QString("fet");
const QString PROGRAM=QString("fettimetabling");

bool USE_GUI_COLORS=false;

bool SHOW_SUBGROUPS_IN_COMBO_BOXES=true;
bool SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING=true;

bool SHOW_SHORTCUTS_ON_MAIN_WINDOW=true;

bool SHOW_TOOLTIPS_FOR_CONSTRAINTS_WITH_TABLES=false;

bool ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY=false;
bool ENABLE_ACTIVITY_TAG_MIN_HOURS_DAILY=false;
bool ENABLE_STUDENTS_MAX_GAPS_PER_DAY=false;

bool ENABLE_MAX_GAPS_PER_REAL_DAY=false;

bool SHOW_WARNING_FOR_NOT_PERFECT_CONSTRAINTS=true;

bool SHOW_WARNING_FOR_SUBGROUPS_WITH_THE_SAME_ACTIVITIES=true;

bool SHOW_WARNING_FOR_ACTIVITIES_FIXED_SPACE_VIRTUAL_REAL_ROOMS_BUT_NOT_FIXED_TIME=true;

bool SHOW_WARNING_FOR_MAX_HOURS_DAILY_WITH_UNDER_100_WEIGHT=true;

bool ENABLE_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS=false;
bool SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS=true;

bool ENABLE_STUDENTS_MIN_HOURS_PER_MORNING_WITH_ALLOW_EMPTY_MORNINGS=false;
bool SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_PER_MORNING_WITH_ALLOW_EMPTY_MORNINGS=true;

bool ENABLE_STUDENTS_MIN_HOURS_PER_AFTERNOON_WITH_ALLOW_EMPTY_AFTERNOONS=false;
bool SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_PER_AFTERNOON_WITH_ALLOW_EMPTY_AFTERNOONS=true;

bool ENABLE_GROUP_ACTIVITIES_IN_INITIAL_ORDER=false;
bool SHOW_WARNING_FOR_GROUP_ACTIVITIES_IN_INITIAL_ORDER=true;

bool CONFIRM_ACTIVITY_PLANNING=true;
bool CONFIRM_SPREAD_ACTIVITIES=true;
bool CONFIRM_REMOVE_REDUNDANT=true;
bool CONFIRM_SAVE_TIMETABLE=true;

bool CONFIRM_ACTIVATE_DEACTIVATE_ACTIVITIES_CONSTRAINTS=true;

//extern MRG32k3a rng;

const int STATUS_BAR_MILLISECONDS=2500;

//in rules.cpp
extern int cntUndoRedoStackIterator;
extern std::list<QByteArray> oldRulesArchived; //.front() is oldest, .back() is newest
//extern std::list<QString> operationWhichWasDone; //as above
extern std::list<QByteArray> operationWhichWasDoneArchived; //as above
extern std::list<QPair<QDate, QTime>> operationDateTime; //as above
extern std::list<int> unarchivedSizes; //as above
//extern std::list<QString> stateFileName; //as above

extern std::list<QByteArray>::const_iterator crtBAIt;

extern int savedStateIterator;

extern FetMainForm* pFetMainForm;

void showStatusBarAutosaved()
{
	if(pFetMainForm!=nullptr)
		pFetMainForm->statusBar()->showMessage(FetMainForm::tr("File autosaved"), STATUS_BAR_MILLISECONDS);
}

void updateFetMainFormAfterHistoryRestored(int iterationsBackward)
{
	if(pFetMainForm!=nullptr){
		pFetMainForm->setCurrentFile(INPUT_FILENAME_XML);
		/*if(!pFetMainForm->isWindowModified())
			pFetMainForm->setWindowModified(true);*/
		pFetMainForm->setWindowModified(gt.rules.modified);

		if(iterationsBackward>0)
			pFetMainForm->statusBar()->showMessage(FetMainForm::tr("Backward state restored"), STATUS_BAR_MILLISECONDS);
		else if(iterationsBackward<0)
			pFetMainForm->statusBar()->showMessage(FetMainForm::tr("Forward state restored"), STATUS_BAR_MILLISECONDS);
		else
			assert(0);
		
		pFetMainForm->updateMode();
	}
}

void clearHistory()
{
	cntUndoRedoStackIterator=0;

	oldRulesArchived.clear();
	operationWhichWasDoneArchived.clear();
	operationDateTime.clear();
	unarchivedSizes.clear();
	//stateFileName.clear();
	
	savedStateIterator=0;
}

bool FetMainForm::openHistory()
{
	QString filename=INPUT_FILENAME_XML+SUFFIX_FILENAME_SAVE_HISTORY;

	if(USE_UNDO_REDO && USE_UNDO_REDO_SAVE){
		if(!QFile::exists(filename))
			return false;

		QFile file(filename);

#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
		if(!file.open(QIODeviceBase::ReadOnly)){
#else
		if(!file.open(QIODevice::ReadOnly)){
#endif
			QMessageBox::critical(this, tr("FET critical"),
			 tr("Cannot open %1 for reading the history ... please check the reading permissions of the selected directory. Reading of the history file aborted.").arg(QFileInfo(filename).fileName()));

			return false;
		}
		
		QDataStream tos(&file);
		
		//tos<<QString("FET history file")<<QString("\n")
		// <<QString("Data format version=")<<FET_DATA_FORMAT_VERSION<<QString("\n")
		// <<QString("FET version=")<<FET_VERSION<<QString("\n");
		QString s1, s2, s3, s4, s5, s6, s7, s8;
		tos>>s1>>s2>>s3>>s4>>s5>>s6>>s7>>s8;
		if(s1!=QString("FET history file") || s2!=QString("\n")
		 || s3!=QString("Data format version=") || s5!=QString("\n")
		 || s6!=QString("FET version=") || s8!=QString("\n")){
			QMessageBox::information(this, tr("FET information"), tr("The history will not be loaded from the disk for this data file,"
			 " because the history file header is invalid. (Starting with FET version %1 the history file header changed.)")
			 .arg("6.16.0")
			 +QString("\n\n")
			 +tr("There should be nothing else to worry about. Your .fet data file will be safely/correctly opened and the disk history will be"
			 " updated/corrected when you will save your data file."));
			
			file.close();
			
			return false;
		}
		if(s4!=FET_DATA_FORMAT_VERSION){
			QMessageBox::information(this, tr("FET information"), tr("The history for this file cannot be loaded from the disk, because the data structure"
			 " has changed (you are using a different FET version, in which the data format was changed).")+QString(" ")
			 +tr("The history file data format version=%1, your FET data format version=%2.").arg(s4).arg(FET_DATA_FORMAT_VERSION)
			 +QString("\n\n")
			 +tr("This was explained in the disk history settings dialog, and there should be nothing else to worry about. Your .fet data file will be"
			 " safely/correctly opened and the disk history will be updated/corrected when you will save your data file."));
			
			file.close();
			
			return false;
		}
		Q_UNUSED(s7);

		int stepsToRead;
		tos>>stepsToRead;
		
		if(stepsToRead>UNDO_REDO_STEPS)
			stepsToRead=UNDO_REDO_STEPS;

		if(stepsToRead<=0){
			file.close();
			
			return false;
		}
		
		clearHistory(); //so that the "Opened the file..." undo/redo point from the function openFile(...) is removed.
		
		for(int i=0; i<stepsToRead; i++){
			QByteArray oRAba;
			QByteArray oWWDAba;
			QPair<QDate, QTime> oDTp;
			int uSi;
			tos>>oRAba>>oWWDAba>>oDTp>>uSi;

			oldRulesArchived.push_front(oRAba);
			operationWhichWasDoneArchived.push_front(oWWDAba);
			operationDateTime.push_front(oDTp);
			unarchivedSizes.push_front(uSi);
		}

		/*if(stepsToRead==UNDO_REDO_STEPS+1){
			//we have one extra history item, "file was opened..."
			oldRulesArchived.pop_back();
			operationWhichWasDoneArchived.pop_back();
			operationDateTime.pop_back();
			unarchivedSizes.pop_back();
		}*/

		cntUndoRedoStackIterator=int(oldRulesArchived.size());
		savedStateIterator=cntUndoRedoStackIterator;
		assert(!oldRulesArchived.empty());
		crtBAIt=std::prev(oldRulesArchived.cend());

		file.close();

		return true;
	}
	else{
		return false;
	}
}

bool FetMainForm::saveHistory()
{
	QString filename=INPUT_FILENAME_XML+SUFFIX_FILENAME_SAVE_HISTORY;

	if(USE_UNDO_REDO && USE_UNDO_REDO_SAVE){
		std::list<QByteArray>::const_iterator oRAit=oldRulesArchived.cbegin();
		std::list<QByteArray>::const_iterator oWWDAit=operationWhichWasDoneArchived.cbegin();
		std::list<QPair<QDate, QTime>>::const_iterator oDTit=operationDateTime.cbegin();
		std::list<int>::const_iterator uSit=unarchivedSizes.cbegin();

		assert(cntUndoRedoStackIterator==savedStateIterator);
		assert(savedStateIterator>=0);
		assert(savedStateIterator<=int(oldRulesArchived.size()));
		if(savedStateIterator==0){
			if(QFile::exists(filename))
				return QFile::remove(filename);
			else
				return true;
		}
		else{
			for(int i=1; i<savedStateIterator; i++){
				oRAit++;
				oWWDAit++;
				oDTit++;
				uSit++;
			}

			QSaveFile file(filename);

#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
			if(!file.open(QIODeviceBase::WriteOnly | QIODeviceBase::Truncate)){
#else
			if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate)){
#endif
				QMessageBox::critical(this, tr("FET critical"),
				 tr("Cannot open %1 for writing of the history ... please check the write permissions of the selected directory and your disk's free space. Saving of the file aborted.").arg(QFileInfo(filename).fileName()));

				return false;
			}

			QDataStream tos(&file);

			tos<<QString("FET history file")<<QString("\n")
			 <<QString("Data format version=")<<FET_DATA_FORMAT_VERSION<<QString("\n")
			 <<QString("FET version=")<<FET_VERSION<<QString("\n");

			int stepsToSave=std::min(UNDO_REDO_STEPS_SAVE, savedStateIterator);
			tos<<stepsToSave;
			for(int i=stepsToSave-1; i>=0; i--){
				tos<<*oRAit<<*oWWDAit<<*oDTit<<*uSit;

				if(i>=1){
					oRAit--;
					oWWDAit--;
					oDTit--;
					uSit--;
				}
			}

			if(file.error()!=QFileDevice::NoError){
				QMessageBox::critical(this, tr("FET critical"),
				 tr("Saving the history file gave the error message '%1', which means the saving is compromised. Please check your disk's free space.")
				 .arg(file.errorString()));
			}

			bool res=file.commit();
			return res;
		}
	}
	else{
		if(QFile::exists(filename))
			return QFile::remove(filename);
		else
			return true;
	}
}

FetMainForm::FetMainForm()
{
	setupUi(this);

	generation_running=false;
	generation_running_multi=false;

	menuA_teacher_time_constraints=nullptr;
	menuAll_teachers_time_constraints=nullptr;
	menuA_students_set_time_constraints=nullptr;
	menuAll_students_time_constraints=nullptr;

	menuA_teacher_1_time_constraints=nullptr;
	menuA_teacher_2_time_constraints=nullptr;
	menuA_teacher_3_time_constraints=nullptr;
	menuA_teacher_4_time_constraints=nullptr;
	menuAll_teachers_time_constraints=nullptr;
	menuAll_teachers_1_time_constraints=nullptr;
	menuAll_teachers_2_time_constraints=nullptr;
	menuAll_teachers_3_time_constraints=nullptr;
	menuAll_teachers_4_time_constraints=nullptr;
	menuA_students_set_1_time_constraints=nullptr;
	menuA_students_set_2_time_constraints=nullptr;
	menuA_students_set_3_time_constraints=nullptr;
	menuA_students_set_4_time_constraints=nullptr;
	menuAll_students_1_time_constraints=nullptr;
	menuAll_students_2_time_constraints=nullptr;
	menuAll_students_3_time_constraints=nullptr;
	menuAll_students_4_time_constraints=nullptr;

	menuActivities_preferred_times_time_constraints=nullptr;
	menuActivities_others_1_time_constraints=nullptr;
	menuActivities_others_2_time_constraints=nullptr;
	menuActivities_others_3_time_constraints=nullptr;

	createActionsForConstraints();
	
	QIcon appIcon(":/images/appicon.png");
	pqapplication->setWindowIcon(appIcon);
	
	this->statusBar()->addPermanentWidget(&modeLabel);
	dataAvailable=false;
	updateMode(true); //true means force
	
	QSettings settings(COMPANY, PROGRAM);
	
	USE_UNDO_REDO=settings.value(QString("enable-data-states-recording"), QString("true")).toBool();
	UNDO_REDO_STEPS=settings.value(QString("number-of-data-steps-to-record"), QString("100")).toInt();
	if(UNDO_REDO_STEPS>1000)
		UNDO_REDO_STEPS=1000;
	if(UNDO_REDO_STEPS<=0)
		UNDO_REDO_STEPS=1;
	//UNDO_REDO_COMPRESSION_LEVEL=settings.value(QString("data-states-compression-level"), QString("-1")).toInt();

	USE_UNDO_REDO_SAVE=settings.value(QString("enable-data-states-recording-on-disk"), QString("false")).toBool();
	UNDO_REDO_STEPS_SAVE=settings.value(QString("number-of-data-steps-to-record-on-disk"), QString("20")).toInt();
	if(UNDO_REDO_STEPS_SAVE>100)
		UNDO_REDO_STEPS=100;
	if(UNDO_REDO_STEPS_SAVE<=0)
		UNDO_REDO_STEPS_SAVE=1;
	SUFFIX_FILENAME_SAVE_HISTORY=settings.value(QString("filename-suffix-save-history"), QString(".his")).toString();
	if(SUFFIX_FILENAME_SAVE_HISTORY!=QString(".his"))
		SUFFIX_FILENAME_SAVE_HISTORY=QString(".his");

	USE_AUTOSAVE=settings.value(QString("enable-file-autosave"), QString("false")).toBool();
	MINUTES_AUTOSAVE=settings.value(QString("minutes-for-autosave"), QString("3")).toInt();
	if(MINUTES_AUTOSAVE>15)
		MINUTES_AUTOSAVE=15;
	if(MINUTES_AUTOSAVE<1)
		MINUTES_AUTOSAVE=1;
	OPERATIONS_AUTOSAVE=settings.value(QString("operations-for-autosave"), QString("1")).toInt();
	if(OPERATIONS_AUTOSAVE>100)
		OPERATIONS_AUTOSAVE=100;
	if(OPERATIONS_AUTOSAVE<1)
		OPERATIONS_AUTOSAVE=1;
		
	QString das=settings.value(QString("directory-for-autosave"), QString("")).toString();
	if(das.isEmpty()){
		DIRECTORY_AUTOSAVE=QString("");
	}
	else{
		DIRECTORY_AUTOSAVE=QDir(das).canonicalPath();
		if(!QDir(DIRECTORY_AUTOSAVE).exists())
			DIRECTORY_AUTOSAVE=QString("");
	}
	
	SUFFIX_FILENAME_AUTOSAVE=settings.value(QString("filename-suffix-for-autosave"), QString("_AUTOSAVE")).toString();
	if(SUFFIX_FILENAME_AUTOSAVE!=QString("_AUTOSAVE"))
		SUFFIX_FILENAME_AUTOSAVE=QString("_AUTOSAVE");

	originalFont=qApp->font();
	fontIsUserSelectable=settings.value(QString("font-is-user-selectable"), QString("false")).toBool();
	userChoseAFont=false;
	if(fontIsUserSelectable){
		if(settings.contains(QString("font"))){
			QString s=settings.value(QString("font")).toString();
			if(s!=QString("")){
				QFont interfaceFont;
				bool ok=interfaceFont.fromString(s);
				if(ok){
					qApp->setFont(interfaceFont);
					userChoseAFont=true;
				}
			}
		}
	}
	
	int nRec=settings.value(QString("FetMainForm/number-of-recent-files"), 0).toInt();
	if(nRec>MAX_RECENT_FILES)
		nRec=MAX_RECENT_FILES;
	recentFiles.clear();
	for(int i=0; i<nRec; i++)
		if(settings.contains(QString("FetMainForm/recent-file/")+CustomFETString::number(i+1)))
			recentFiles.append(settings.value(QString("FetMainForm/recent-file/")+CustomFETString::number(i+1)).toString());
	
	recentSeparatorAction=fileOpenRecentMenu->insertSeparator(fileClearRecentFilesListAction);
	for(int i=0; i<MAX_RECENT_FILES; i++){
		recentFileActions[i]=new QAction(this);
		recentFileActions[i]->setVisible(false);
		connect(recentFileActions[i], &QAction::triggered, this, &FetMainForm::openRecentFile);
		
		fileOpenRecentMenu->insertAction(recentSeparatorAction, recentFileActions[i]);
	}
	
	updateRecentFileActions();
	
	//statusBar()->showMessage(tr("FET started", "This is a message written in the status bar, saying that FET was started"), STATUS_BAR_MILLISECONDS);
	statusBar()->showMessage("", STATUS_BAR_MILLISECONDS); //to get the correct centralWidget for the logo, so we need status bar existing.
	
	INPUT_FILENAME_XML=QString("");
	setCurrentFile(INPUT_FILENAME_XML);

	//toolBox->setCurrentIndex(0);
	tabWidget->setVisible(SHOW_SHORTCUTS_ON_MAIN_WINDOW);
	tabWidget->setCurrentIndex(MAIN_FORM_SHORTCUTS_TAB_POSITION);
	
	shortcutBasicMenu=new QMenu();
	shortcutBasicMenu->addMenu(menuInstitution_information);
	shortcutBasicMenu->addSeparator(); //added on 19 Dec. 2009
	shortcutBasicMenu->addAction(dataTermsAction);
	shortcutBasicMenu->addMenu(menuDays_and_hours);
	
	shortcutAdvancedTimeMenu=new QMenu();
	shortcutAdvancedTimeMenu->addAction(spreadActivitiesAction);
	shortcutAdvancedTimeMenu->addSeparator();
	shortcutAdvancedTimeMenu->addAction(removeRedundantConstraintsAction);
	//shortcutAdvancedTimeMenu->addSeparator();
	//shortcutAdvancedTimeMenu->addAction(groupActivitiesInInitialOrderAction);
	
	shortcutDataSpaceMenu=new QMenu();
	shortcutDataSpaceMenu->addAction(dataBuildingsAction);
	shortcutDataSpaceMenu->addSeparator();
	shortcutDataSpaceMenu->addAction(dataRoomsAction);
	
	shortcutDataAdvancedMenu=new QMenu();
	shortcutDataAdvancedMenu->addAction(activityPlanningAction);
	shortcutDataAdvancedMenu->addSeparator();
	shortcutDataAdvancedMenu->addAction(dataTeachersStatisticsAction);
	shortcutDataAdvancedMenu->addAction(dataSubjectsStatisticsAction);
	shortcutDataAdvancedMenu->addAction(dataStudentsStatisticsAction);
	shortcutDataAdvancedMenu->addSeparator();
	shortcutDataAdvancedMenu->addAction(dataActivitiesRoomsStatisticsAction);
	shortcutDataAdvancedMenu->addSeparator();
	shortcutDataAdvancedMenu->addAction(dataTeachersSubjectsQualificationsStatisticsAction);
	
	//2014-07-01
	shortcutTimetableLockingMenu=new QMenu();
	shortcutTimetableLockingMenu->addAction(timetableLockAllActivitiesAction);
	shortcutTimetableLockingMenu->addAction(timetableUnlockAllActivitiesAction);
	shortcutTimetableLockingMenu->addSeparator();
	shortcutTimetableLockingMenu->addAction(timetableLockActivitiesDayAction);
	shortcutTimetableLockingMenu->addAction(timetableUnlockActivitiesDayAction);
	shortcutTimetableLockingMenu->addSeparator();
	shortcutTimetableLockingMenu->addAction(timetableLockActivitiesEndStudentsDayAction);
	shortcutTimetableLockingMenu->addAction(timetableUnlockActivitiesEndStudentsDayAction);
	//2018-06-26
	shortcutTimetableLockingMenu->addSeparator();
	shortcutTimetableLockingMenu->addAction(timetableLockActivitiesWithASpecifiedActivityTagAction);
	shortcutTimetableLockingMenu->addAction(timetableUnlockActivitiesWithASpecifiedActivityTagAction);
	//2023-01-13
	shortcutTimetableLockingMenu->addSeparator();
	shortcutTimetableLockingMenu->addAction(timetableLockActivitiesWithAdvancedFilterAction);
	shortcutTimetableLockingMenu->addAction(timetableUnlockActivitiesWithAdvancedFilterAction);
	
	shortcutTimetableAdvancedMenu=new QMenu();
	shortcutTimetableAdvancedMenu->addAction(groupActivitiesInInitialOrderAction);
	
	ORIGINAL_WIDTH=width();
	ORIGINAL_HEIGHT=height();
	//ORIGINAL_X=x();
	//ORIGINAL_Y=y();
	
	QRect rect=mainFormSettingsRect;
	if(rect.isValid()){
		bool ok=false;
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
		for(QScreen* screen : QGuiApplication::screens()){
			if(screen->availableGeometry().intersects(rect)){
#else
		for(int i=0; i<QApplication::desktop()->screenCount(); i++){
			if(QApplication::desktop()->availableGeometry(i).intersects(rect)){
#endif
				ok=true;
				break;
			}
		}
	
		if(ok){
			this->setGeometry(rect);
		}
		else{
			forceCenterWidgetOnScreen(this);
		}
	}
	else{
		forceCenterWidgetOnScreen(this);
	}
	
	//new void data
	if(gt.rules.initialized)
		gt.rules.clear();
	/*gt.rules.init();

	gt.rules.modified=true; //to avoid the flickering of the main form modified flag

	bool tmp=gt.rules.addTimeConstraint(new ConstraintBasicCompulsoryTime(100));
	assert(tmp);
	tmp=gt.rules.addSpaceConstraint(new ConstraintBasicCompulsorySpace(100));
	assert(tmp);*/

	gt.rules.modified=true; //force update of the modified flag of the main window
	setRulesUnmodifiedAndOtherThings(&gt.rules);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_buildings_schedule_ready=false;
	
	assert(!gt.rules.initialized);
	
	settingsFontIsUserSelectableAction->setCheckable(true);
	settingsFontIsUserSelectableAction->setChecked(fontIsUserSelectable);
	
	settingsShowShortcutsOnMainWindowAction->setCheckable(true);
	settingsShowShortcutsOnMainWindowAction->setChecked(SHOW_SHORTCUTS_ON_MAIN_WINDOW);
	
	settingsShowToolTipsForConstraintsWithTablesAction->setCheckable(true);
	settingsShowToolTipsForConstraintsWithTablesAction->setChecked(SHOW_TOOLTIPS_FOR_CONSTRAINTS_WITH_TABLES);
	
	checkForUpdatesAction->setCheckable(true);
	checkForUpdatesAction->setChecked(checkForUpdates);
	
	settingsUseColorsAction->setCheckable(true);
	settingsUseColorsAction->setChecked(USE_GUI_COLORS);
	
	settingsShowSubgroupsInComboBoxesAction->setCheckable(true);
	settingsShowSubgroupsInComboBoxesAction->setChecked(SHOW_SUBGROUPS_IN_COMBO_BOXES);
	settingsShowSubgroupsInActivityPlanningAction->setCheckable(true);
	settingsShowSubgroupsInActivityPlanningAction->setChecked(SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING);
	
	////////confirmations
	settingsConfirmActivityPlanningAction->setCheckable(true);
	settingsConfirmActivityPlanningAction->setChecked(CONFIRM_ACTIVITY_PLANNING);
	
	settingsConfirmSpreadActivitiesAction->setCheckable(true);
	settingsConfirmSpreadActivitiesAction->setChecked(CONFIRM_SPREAD_ACTIVITIES);
	
	settingsConfirmRemoveRedundantAction->setCheckable(true);
	settingsConfirmRemoveRedundantAction->setChecked(CONFIRM_REMOVE_REDUNDANT);
	
	settingsConfirmSaveTimetableAction->setCheckable(true);
	settingsConfirmSaveTimetableAction->setChecked(CONFIRM_SAVE_TIMETABLE);

	settingsConfirmActivateDeactivateActivitiesConstraintsAction->setCheckable(true);
	settingsConfirmActivateDeactivateActivitiesConstraintsAction->setChecked(CONFIRM_ACTIVATE_DEACTIVATE_ACTIVITIES_CONSTRAINTS);
	///////
	
	settingsDivideTimetablesByDaysAction->setCheckable(true);
	settingsDivideTimetablesByDaysAction->setChecked(DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS);
	
	settingsDuplicateVerticalNamesAction->setCheckable(true);
	settingsDuplicateVerticalNamesAction->setChecked(TIMETABLE_HTML_REPEAT_NAMES);
	
	if(checkForUpdates){
		//After setGeometry(rect), so that we know the position of the main form, used for the parent 'this' of the QMessageBox below.
		if(!QSslSocket::supportsSsl()){
			QMessageBox::warning(this, tr("FET warning"), tr("SSL is not available (this might be caused by missing SSL libraries)."
			 " Because of this, FET cannot check for updates on startup (it cannot get the file %1).")
			 .arg("https://lalescu.ro/liviu/fet/crtversion/crtversion.txt"));
		}
		else{
			networkManager=new QNetworkAccessManager(this);
			connect(networkManager, &QNetworkAccessManager::finished, this, &FetMainForm::replyFinished);
			QUrl url("https://lalescu.ro/liviu/fet/crtversion/crtversion.txt");
			if(VERBOSE){
				std::cout<<"New version checking host: "<<qPrintable(url.host())<<std::endl;
				std::cout<<"New version checking path: "<<qPrintable(url.path())<<std::endl;
			}
			QNetworkRequest req=QNetworkRequest(url);
			//As on https://stackoverflow.com/questions/14416786/webpage-returning-http-406-error-only-when-connecting-from-qt
			//and http://amin-ahmadi.com/2016/06/13/fix-modsecurity-issues-in-qt-network-module-download-functionality/ ,
			//to avoid code 406 from the server.
#if QT_VERSION >= QT_VERSION_CHECK(5,4,0)
			req.setHeader(QNetworkRequest::UserAgentHeader, QString("FET")+QString(" ")+FET_VERSION+
			 QString(" (Qt ")+QString(qVersion())+QString("; ")+QSysInfo::prettyProductName()+QString("; ")+QSysInfo::currentCpuArchitecture()+QString(")"));
#elif QT_VERSION >= QT_VERSION_CHECK(5,0,0)
			req.setHeader(QNetworkRequest::UserAgentHeader, QString("FET")+QString(" ")+FET_VERSION+QString(" (Qt ")+QString(qVersion())+QString(")"));
#else
			req.setRawHeader("User-Agent", (QString("FET")+QString(" ")+FET_VERSION+QString(" (Qt ")+QString(qVersion())+QString(")")).toUtf8());
#endif
			networkManager->get(req);
		}
	}
	
	settingsShowVirtualRoomsInTimetablesAction->setCheckable(true);
	settingsShowVirtualRoomsInTimetablesAction->setChecked(SHOW_VIRTUAL_ROOMS_IN_TIMETABLES);

	settingsOrderSubgroupsInTimetablesAction->setCheckable(true);
	settingsOrderSubgroupsInTimetablesAction->setChecked(TIMETABLES_SUBGROUPS_SORTED);

	settingsPrintDetailedTimetablesAction->setCheckable(true);
	settingsPrintDetailedTimetablesAction->setChecked(PRINT_DETAILED_HTML_TIMETABLES);
	settingsPrintDetailedTeachersFreePeriodsTimetablesAction->setCheckable(true);
	settingsPrintDetailedTeachersFreePeriodsTimetablesAction->setChecked(PRINT_DETAILED_HTML_TEACHERS_FREE_PERIODS);

	settingsPrintNotAvailableSlotsAction->setCheckable(true);
	settingsPrintNotAvailableSlotsAction->setChecked(PRINT_NOT_AVAILABLE_TIME_SLOTS);

	settingsPrintBreakSlotsAction->setCheckable(true);
	settingsPrintBreakSlotsAction->setChecked(PRINT_BREAK_TIME_SLOTS);

	settingsPrintActivitiesWithSameStartingTimeAction->setCheckable(true);
	settingsPrintActivitiesWithSameStartingTimeAction->setChecked(PRINT_ACTIVITIES_WITH_SAME_STARTING_TIME);

	//needed to sync the view table forms
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	LockUnlock::increaseCommunicationSpinBox();
	
	showWarningForSubgroupsWithTheSameActivitiesAction->setCheckable(true);
	showWarningForActivitiesNotLockedTimeLockedSpaceVirtualRealRoomsAction->setCheckable(true);
	showWarningForMaxHoursDailyWithUnder100WeightAction->setCheckable(true);
	
	enableActivityTagMaxHoursDailyAction->setCheckable(true);
	enableActivityTagMinHoursDailyAction->setCheckable(true);
	enableStudentsMaxGapsPerDayAction->setCheckable(true);
	enableMaxGapsPerRealDayAction->setCheckable(true);
	showWarningForNotPerfectConstraintsAction->setCheckable(true);

	enableStudentsMinHoursDailyWithAllowEmptyDaysAction->setCheckable(true);
	showWarningForStudentsMinHoursDailyWithAllowEmptyDaysAction->setCheckable(true);

	enableStudentsMinHoursPerMorningWithAllowEmptyMorningsAction->setCheckable(true);
	showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsAction->setCheckable(true);

	enableStudentsMinHoursPerAfternoonWithAllowEmptyAfternoonsAction->setCheckable(true);
	showWarningForStudentsMinHoursPerAfternoonWithAllowEmptyAfternoonsAction->setCheckable(true);

	enableGroupActivitiesInInitialOrderAction->setCheckable(true);
	showWarningForGroupActivitiesInInitialOrderAction->setCheckable(true);
	
	showWarningForSubgroupsWithTheSameActivitiesAction->setChecked(SHOW_WARNING_FOR_SUBGROUPS_WITH_THE_SAME_ACTIVITIES);

	showWarningForActivitiesNotLockedTimeLockedSpaceVirtualRealRoomsAction->setChecked(SHOW_WARNING_FOR_ACTIVITIES_FIXED_SPACE_VIRTUAL_REAL_ROOMS_BUT_NOT_FIXED_TIME);

	showWarningForMaxHoursDailyWithUnder100WeightAction->setChecked(SHOW_WARNING_FOR_MAX_HOURS_DAILY_WITH_UNDER_100_WEIGHT);

	enableActivityTagMaxHoursDailyAction->setChecked(ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
	enableActivityTagMinHoursDailyAction->setChecked(ENABLE_ACTIVITY_TAG_MIN_HOURS_DAILY);
	enableStudentsMaxGapsPerDayAction->setChecked(ENABLE_STUDENTS_MAX_GAPS_PER_DAY);
	enableMaxGapsPerRealDayAction->setChecked(ENABLE_MAX_GAPS_PER_REAL_DAY);
	showWarningForNotPerfectConstraintsAction->setChecked(SHOW_WARNING_FOR_NOT_PERFECT_CONSTRAINTS);

	enableStudentsMinHoursDailyWithAllowEmptyDaysAction->setChecked(ENABLE_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS);
	showWarningForStudentsMinHoursDailyWithAllowEmptyDaysAction->setChecked(SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS);

	enableStudentsMinHoursPerMorningWithAllowEmptyMorningsAction->setChecked(ENABLE_STUDENTS_MIN_HOURS_PER_MORNING_WITH_ALLOW_EMPTY_MORNINGS);
	showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsAction->setChecked(SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_PER_MORNING_WITH_ALLOW_EMPTY_MORNINGS);

	enableStudentsMinHoursPerAfternoonWithAllowEmptyAfternoonsAction->setChecked(ENABLE_STUDENTS_MIN_HOURS_PER_AFTERNOON_WITH_ALLOW_EMPTY_AFTERNOONS);
	showWarningForStudentsMinHoursPerAfternoonWithAllowEmptyAfternoonsAction->setChecked(SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_PER_AFTERNOON_WITH_ALLOW_EMPTY_AFTERNOONS);

	enableGroupActivitiesInInitialOrderAction->setChecked(ENABLE_GROUP_ACTIVITIES_IN_INITIAL_ORDER);
	showWarningForGroupActivitiesInInitialOrderAction->setChecked(SHOW_WARNING_FOR_GROUP_ACTIVITIES_IN_INITIAL_ORDER);
	
	connect(showWarningForSubgroupsWithTheSameActivitiesAction, &QAction::toggled, this, &FetMainForm::showWarningForSubgroupsWithTheSameActivitiesToggled);
	connect(showWarningForActivitiesNotLockedTimeLockedSpaceVirtualRealRoomsAction, &QAction::toggled, this, &FetMainForm::showWarningForActivitiesNotLockedTimeLockedSpaceVirtualRealRoomsToggled);
	connect(showWarningForMaxHoursDailyWithUnder100WeightAction, &QAction::toggled, this, &FetMainForm::showWarningForMaxHoursDailyWithUnder100WeightToggled);

	connect(checkForUpdatesAction, &QAction::toggled, this, &FetMainForm::checkForUpdatesToggled);
	connect(settingsUseColorsAction, &QAction::toggled, this, &FetMainForm::useColorsToggled);
	connect(settingsShowSubgroupsInComboBoxesAction, &QAction::toggled, this, &FetMainForm::showSubgroupsInComboBoxesToggled);
	connect(settingsShowSubgroupsInActivityPlanningAction, &QAction::toggled, this, &FetMainForm::showSubgroupsInActivityPlanningToggled);
	
	connect(enableActivityTagMaxHoursDailyAction, &QAction::toggled, this, &FetMainForm::enableActivityTagMaxHoursDailyToggled);
	connect(enableActivityTagMinHoursDailyAction, &QAction::toggled, this, &FetMainForm::enableActivityTagMinHoursDailyToggled);
	connect(enableStudentsMaxGapsPerDayAction, &QAction::toggled, this, &FetMainForm::enableStudentsMaxGapsPerDayToggled);
	connect(enableMaxGapsPerRealDayAction, &QAction::toggled, this, &FetMainForm::enableMaxGapsPerRealDayToggled);
	connect(showWarningForNotPerfectConstraintsAction, &QAction::toggled, this, &FetMainForm::showWarningForNotPerfectConstraintsToggled);

	connect(enableStudentsMinHoursDailyWithAllowEmptyDaysAction, &QAction::toggled, this, &FetMainForm::enableStudentsMinHoursDailyWithAllowEmptyDaysToggled);
	connect(showWarningForStudentsMinHoursDailyWithAllowEmptyDaysAction, &QAction::toggled, this, &FetMainForm::showWarningForStudentsMinHoursDailyWithAllowEmptyDaysToggled);

	connect(enableStudentsMinHoursPerMorningWithAllowEmptyMorningsAction, &QAction::toggled, this, &FetMainForm::enableStudentsMinHoursPerMorningWithAllowEmptyMorningsToggled);
	connect(showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsAction, &QAction::toggled, this, &FetMainForm::showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsToggled);

	connect(enableStudentsMinHoursPerAfternoonWithAllowEmptyAfternoonsAction, &QAction::toggled, this, &FetMainForm::enableStudentsMinHoursPerAfternoonWithAllowEmptyAfternoonsToggled);
	connect(showWarningForStudentsMinHoursPerAfternoonWithAllowEmptyAfternoonsAction, &QAction::toggled, this, &FetMainForm::showWarningForStudentsMinHoursPerAfternoonWithAllowEmptyAfternoonsToggled);

	connect(enableGroupActivitiesInInitialOrderAction, &QAction::toggled, this, &FetMainForm::enableGroupActivitiesInInitialOrderToggled);
	connect(showWarningForGroupActivitiesInInitialOrderAction, &QAction::toggled, this, &FetMainForm::showWarningForGroupActivitiesInInitialOrderToggled);

	dataTimeConstraintsTeacherActivityTagMaxHoursDailyAction->setIconVisibleInMenu(true);
	dataTimeConstraintsTeachersActivityTagMaxHoursDailyAction->setIconVisibleInMenu(true);
	dataTimeConstraintsStudentsActivityTagMaxHoursDailyAction->setIconVisibleInMenu(true);
	dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyAction->setIconVisibleInMenu(true);

	dataTimeConstraintsTeacherActivityTagMaxHoursDailyRealDaysAction->setIconVisibleInMenu(true);
	dataTimeConstraintsTeachersActivityTagMaxHoursDailyRealDaysAction->setIconVisibleInMenu(true);
	dataTimeConstraintsStudentsActivityTagMaxHoursDailyRealDaysAction->setIconVisibleInMenu(true);
	dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyRealDaysAction->setIconVisibleInMenu(true);

	dataTimeConstraintsTeacherActivityTagMinHoursDailyAction->setIconVisibleInMenu(true);
	dataTimeConstraintsTeachersActivityTagMinHoursDailyAction->setIconVisibleInMenu(true);
	dataTimeConstraintsStudentsActivityTagMinHoursDailyAction->setIconVisibleInMenu(true);
	dataTimeConstraintsStudentsSetActivityTagMinHoursDailyAction->setIconVisibleInMenu(true);

	dataTimeConstraintsStudentsSetMaxGapsPerDayAction->setIconVisibleInMenu(true);
	dataTimeConstraintsStudentsMaxGapsPerDayAction->setIconVisibleInMenu(true);

	dataTimeConstraintsStudentsSetMaxGapsPerRealDayAction->setIconVisibleInMenu(true);
	dataTimeConstraintsStudentsMaxGapsPerRealDayAction->setIconVisibleInMenu(true);
	dataTimeConstraintsTeachersMaxGapsPerRealDayAction->setIconVisibleInMenu(true);
	dataTimeConstraintsTeacherMaxGapsPerRealDayAction->setIconVisibleInMenu(true);

	dataTimeConstraintsStudentsSetMaxGapsPerWeekForRealDaysAction->setIconVisibleInMenu(true);
	dataTimeConstraintsStudentsMaxGapsPerWeekForRealDaysAction->setIconVisibleInMenu(true);
	dataTimeConstraintsTeachersMaxGapsPerWeekForRealDaysAction->setIconVisibleInMenu(true);
	dataTimeConstraintsTeacherMaxGapsPerWeekForRealDaysAction->setIconVisibleInMenu(true);

	groupActivitiesInInitialOrderAction->setIconVisibleInMenu(true);

	setEnabledIcon(dataTimeConstraintsTeacherActivityTagMaxHoursDailyAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsTeachersActivityTagMaxHoursDailyAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsStudentsActivityTagMaxHoursDailyAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);

	setEnabledIcon(dataTimeConstraintsTeacherActivityTagMaxHoursDailyRealDaysAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsTeachersActivityTagMaxHoursDailyRealDaysAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsStudentsActivityTagMaxHoursDailyRealDaysAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyRealDaysAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);

	setEnabledIcon(dataTimeConstraintsTeacherActivityTagMinHoursDailyAction, ENABLE_ACTIVITY_TAG_MIN_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsTeachersActivityTagMinHoursDailyAction, ENABLE_ACTIVITY_TAG_MIN_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsStudentsActivityTagMinHoursDailyAction, ENABLE_ACTIVITY_TAG_MIN_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsStudentsSetActivityTagMinHoursDailyAction, ENABLE_ACTIVITY_TAG_MIN_HOURS_DAILY);

	setEnabledIcon(dataTimeConstraintsStudentsSetMaxGapsPerDayAction, ENABLE_STUDENTS_MAX_GAPS_PER_DAY);
	setEnabledIcon(dataTimeConstraintsStudentsMaxGapsPerDayAction, ENABLE_STUDENTS_MAX_GAPS_PER_DAY);

	setEnabledIcon(dataTimeConstraintsStudentsSetMaxGapsPerRealDayAction, ENABLE_MAX_GAPS_PER_REAL_DAY);
	setEnabledIcon(dataTimeConstraintsStudentsMaxGapsPerRealDayAction, ENABLE_MAX_GAPS_PER_REAL_DAY);
	setEnabledIcon(dataTimeConstraintsTeachersMaxGapsPerRealDayAction, ENABLE_MAX_GAPS_PER_REAL_DAY);
	setEnabledIcon(dataTimeConstraintsTeacherMaxGapsPerRealDayAction, ENABLE_MAX_GAPS_PER_REAL_DAY);

	setEnabledIcon(dataTimeConstraintsStudentsSetMaxGapsPerWeekForRealDaysAction, ENABLE_MAX_GAPS_PER_REAL_DAY);
	setEnabledIcon(dataTimeConstraintsStudentsMaxGapsPerWeekForRealDaysAction, ENABLE_MAX_GAPS_PER_REAL_DAY);
	setEnabledIcon(dataTimeConstraintsTeachersMaxGapsPerWeekForRealDaysAction, ENABLE_MAX_GAPS_PER_REAL_DAY);
	setEnabledIcon(dataTimeConstraintsTeacherMaxGapsPerWeekForRealDaysAction, ENABLE_MAX_GAPS_PER_REAL_DAY);

	setEnabledIcon(groupActivitiesInInitialOrderAction, ENABLE_GROUP_ACTIVITIES_IN_INITIAL_ORDER);

	//2024-01-25
	connect(settingsAutosaveAction, &QAction::triggered, this, &FetMainForm::settingsAutosaveAction_triggered);

	connect(restoreDataStateAction, &QAction::triggered, this, &FetMainForm::restoreDataStateAction_triggered);
	connect(settingsHistoryMemoryAction, &QAction::triggered, this, &FetMainForm::settingsHistoryMemoryAction_triggered);
	connect(settingsHistoryDiskAction, &QAction::triggered, this, &FetMainForm::settingsHistoryDiskAction_triggered);
	
	connect(modeOfficialAction, &QAction::triggered, this, &FetMainForm::modeOfficialAction_triggered);
	connect(modeMorningsAfternoonsAction, &QAction::triggered, this, &FetMainForm::modeMorningsAfternoonsAction_triggered);
	connect(modeBlockPlanningAction, &QAction::triggered, this, &FetMainForm::modeBlockPlanningAction_triggered);
	connect(modeTermsAction, &QAction::triggered, this, &FetMainForm::modeTermsAction_triggered);
	
	connect(dataTermsAction, &QAction::triggered, this, &FetMainForm::dataTermsAction_triggered);

	connect(fileNewAction, &QAction::triggered, this, &FetMainForm::fileNewAction_triggered);
	connect(fileSaveAction, &QAction::triggered, this, &FetMainForm::fileSaveAction_triggered);
	connect(fileSaveAsAction, &QAction::triggered, this, &FetMainForm::fileSaveAsAction_triggered);
	connect(fileQuitAction, &QAction::triggered, this, &FetMainForm::fileQuitAction_triggered);
	connect(fileOpenAction, &QAction::triggered, this, &FetMainForm::fileOpenAction_triggered);
	connect(fileClearRecentFilesListAction, &QAction::triggered, this, &FetMainForm::fileClearRecentFilesListAction_triggered);

	connect(fileImportCSVActivityTagsAction, &QAction::triggered, this, &FetMainForm::fileImportCSVActivityTagsAction_triggered);
	connect(fileImportCSVActivitiesAction, &QAction::triggered, this, &FetMainForm::fileImportCSVActivitiesAction_triggered);
	connect(fileImportCSVRoomsBuildingsAction, &QAction::triggered, this, &FetMainForm::fileImportCSVRoomsBuildingsAction_triggered);
	connect(fileImportCSVSubjectsAction, &QAction::triggered, this, &FetMainForm::fileImportCSVSubjectsAction_triggered);
	connect(fileImportCSVTeachersAction, &QAction::triggered, this, &FetMainForm::fileImportCSVTeachersAction_triggered);
	connect(fileImportCSVYearsGroupsSubgroupsAction, &QAction::triggered, this, &FetMainForm::fileImportCSVYearsGroupsSubgroupsAction_triggered);
	connect(fileExportCSVAction, &QAction::triggered, this, &FetMainForm::fileExportCSVAction_triggered);
	
	connect(dataInstitutionNameAction, &QAction::triggered, this, &FetMainForm::dataInstitutionNameAction_triggered);
	connect(dataCommentsAction, &QAction::triggered, this, &FetMainForm::dataCommentsAction_triggered);
	connect(dataDaysAction, &QAction::triggered, this, &FetMainForm::dataDaysAction_triggered);
	connect(dataHoursAction, &QAction::triggered, this, &FetMainForm::dataHoursAction_triggered);
	connect(dataTeachersAction, &QAction::triggered, this, &FetMainForm::dataTeachersAction_triggered);
	connect(dataTeachersStatisticsAction, &QAction::triggered, this, &FetMainForm::dataTeachersStatisticsAction_triggered);
	connect(dataSubjectsAction, &QAction::triggered, this, &FetMainForm::dataSubjectsAction_triggered);
	connect(dataSubjectsStatisticsAction, &QAction::triggered, this, &FetMainForm::dataSubjectsStatisticsAction_triggered);
	connect(dataActivityTagsAction, &QAction::triggered, this, &FetMainForm::dataActivityTagsAction_triggered);
	connect(dataYearsAction, &QAction::triggered, this, &FetMainForm::dataYearsAction_triggered);
	connect(dataGroupsAction, &QAction::triggered, this, &FetMainForm::dataGroupsAction_triggered);
	connect(dataSubgroupsAction, &QAction::triggered, this, &FetMainForm::dataSubgroupsAction_triggered);
	connect(dataStudentsStatisticsAction, &QAction::triggered, this, &FetMainForm::dataStudentsStatisticsAction_triggered);
	connect(dataActivitiesRoomsStatisticsAction, &QAction::triggered, this, &FetMainForm::dataActivitiesRoomsStatisticsAction_triggered);
	connect(dataTeachersSubjectsQualificationsStatisticsAction, &QAction::triggered, this, &FetMainForm::dataTeachersSubjectsQualificationsStatisticsAction_triggered);
	connect(dataHelpOnStatisticsAction, &QAction::triggered, this, &FetMainForm::dataHelpOnStatisticsAction_triggered);
	
	connect(helpSettingsAction, &QAction::triggered, this, &FetMainForm::helpSettingsAction_triggered);

	connect(settingsFontAction, &QAction::triggered, this, &FetMainForm::settingsFontAction_triggered);

	connect(timetablesToWriteOnDiskAction, &QAction::triggered, this, &FetMainForm::timetablesToWriteOnDiskAction_triggered);
	
	connect(studentsComboBoxesStyleAction, &QAction::triggered, this, &FetMainForm::studentsComboBoxesStyleAction_triggered);
	
	connect(settingsCommandAfterFinishingAction, &QAction::triggered, this, &FetMainForm::settingsCommandAfterFinishingAction_triggered);

	connect(groupActivitiesInInitialOrderAction, &QAction::triggered, this, &FetMainForm::groupActivitiesInInitialOrderAction_triggered);
	
	connect(dataActivitiesAction, &QAction::triggered, this, &FetMainForm::dataActivitiesAction_triggered);
	connect(dataSubactivitiesAction, &QAction::triggered, this, &FetMainForm::dataSubactivitiesAction_triggered);
	connect(dataRoomsAction, &QAction::triggered, this, &FetMainForm::dataRoomsAction_triggered);
	connect(dataBuildingsAction, &QAction::triggered, this, &FetMainForm::dataBuildingsAction_triggered);
	connect(dataAllTimeConstraintsAction, &QAction::triggered, this, &FetMainForm::dataAllTimeConstraintsAction_triggered);
	connect(dataAllSpaceConstraintsAction, &QAction::triggered, this, &FetMainForm::dataAllSpaceConstraintsAction_triggered);

	connect(helpMoroccoAction, &QAction::triggered, this, &FetMainForm::helpMoroccoAction_triggered);
	connect(helpAlgeriaAction, &QAction::triggered, this, &FetMainForm::helpAlgeriaAction_triggered);
	connect(helpBlockPlanningAction, &QAction::triggered, this, &FetMainForm::helpBlockPlanningAction_triggered);
	connect(helpTermsAction, &QAction::triggered, this, &FetMainForm::helpTermsAction_triggered);

	connect(activityPlanningAction, &QAction::triggered, this, &FetMainForm::activityPlanningAction_triggered);
	connect(spreadActivitiesAction, &QAction::triggered, this, &FetMainForm::spreadActivitiesAction_triggered);
	connect(removeRedundantConstraintsAction, &QAction::triggered, this, &FetMainForm::removeRedundantConstraintsAction_triggered);

	//about
	connect(helpAboutFETAction, &QAction::triggered, this, &FetMainForm::helpAboutFETAction_triggered);
	connect(helpAboutQtAction, &QAction::triggered, this, &FetMainForm::helpAboutQtAction_triggered);
	connect(helpAboutLibrariesAction, &QAction::triggered, this, &FetMainForm::helpAboutLibrariesAction_triggered);
	//offline
	connect(helpFAQAction, &QAction::triggered, this, &FetMainForm::helpFAQAction_triggered);
	connect(helpTipsAction, &QAction::triggered, this, &FetMainForm::helpTipsAction_triggered);
	connect(helpInstructionsAction, &QAction::triggered, this, &FetMainForm::helpInstructionsAction_triggered);
	//online
	connect(helpHomepageAction, &QAction::triggered, this, &FetMainForm::helpHomepageAction_triggered);
	connect(helpContentsAction, &QAction::triggered, this, &FetMainForm::helpContentsAction_triggered);
	connect(helpForumAction, &QAction::triggered, this, &FetMainForm::helpForumAction_triggered);
	connect(helpAddressesAction, &QAction::triggered, this, &FetMainForm::helpAddressesAction_triggered);

	connect(timetableGenerateAction, &QAction::triggered, this, &FetMainForm::timetableGenerateAction_triggered);
	connect(timetableViewStudentsDaysHorizontalAction, &QAction::triggered, this, &FetMainForm::timetableViewStudentsDaysHorizontalAction_triggered);
	connect(timetableViewStudentsTimeHorizontalAction, &QAction::triggered, this, &FetMainForm::timetableViewStudentsTimeHorizontalAction_triggered);
	connect(timetableViewTeachersDaysHorizontalAction, &QAction::triggered, this, &FetMainForm::timetableViewTeachersDaysHorizontalAction_triggered);
	connect(timetableViewTeachersTimeHorizontalAction, &QAction::triggered, this, &FetMainForm::timetableViewTeachersTimeHorizontalAction_triggered);
	connect(timetableViewRoomsDaysHorizontalAction, &QAction::triggered, this, &FetMainForm::timetableViewRoomsDaysHorizontalAction_triggered);
	connect(timetableViewRoomsTimeHorizontalAction, &QAction::triggered, this, &FetMainForm::timetableViewRoomsTimeHorizontalAction_triggered);
	connect(timetableShowConflictsAction, &QAction::triggered, this, &FetMainForm::timetableShowConflictsAction_triggered);
	connect(timetablePrintAction, &QAction::triggered, this, &FetMainForm::timetablePrintAction_triggered);
	connect(timetableGenerateMultipleAction, &QAction::triggered, this, &FetMainForm::timetableGenerateMultipleAction_triggered);

	connect(timetableLockAllActivitiesAction, &QAction::triggered, this, &FetMainForm::timetableLockAllActivitiesAction_triggered);
	connect(timetableUnlockAllActivitiesAction, &QAction::triggered, this, &FetMainForm::timetableUnlockAllActivitiesAction_triggered);
	connect(timetableLockActivitiesDayAction, &QAction::triggered, this, &FetMainForm::timetableLockActivitiesDayAction_triggered);
	connect(timetableUnlockActivitiesDayAction, &QAction::triggered, this, &FetMainForm::timetableUnlockActivitiesDayAction_triggered);
	connect(timetableLockActivitiesEndStudentsDayAction, &QAction::triggered, this, &FetMainForm::timetableLockActivitiesEndStudentsDayAction_triggered);
	connect(timetableUnlockActivitiesEndStudentsDayAction, &QAction::triggered, this, &FetMainForm::timetableUnlockActivitiesEndStudentsDayAction_triggered);
	connect(timetableLockActivitiesWithASpecifiedActivityTagAction, &QAction::triggered, this, &FetMainForm::timetableLockActivitiesWithASpecifiedActivityTagAction_triggered);
	connect(timetableUnlockActivitiesWithASpecifiedActivityTagAction, &QAction::triggered, this, &FetMainForm::timetableUnlockActivitiesWithASpecifiedActivityTagAction_triggered);
	///
	connect(timetableLockActivitiesWithAdvancedFilterAction, &QAction::triggered, this, &FetMainForm::timetableLockActivitiesWithAdvancedFilterAction_triggered);
	connect(timetableUnlockActivitiesWithAdvancedFilterAction, &QAction::triggered, this, &FetMainForm::timetableUnlockActivitiesWithAdvancedFilterAction_triggered);

	connect(timetableSaveTimetableAsAction, &QAction::triggered, this, &FetMainForm::timetableSaveTimetableAsAction_triggered);

	connect(randomSeedAction, &QAction::triggered, this, &FetMainForm::randomSeedAction_triggered);
	
	connect(languageAction, &QAction::triggered, this, &FetMainForm::languageAction_triggered);
	
	connect(settingsRestoreDefaultsAction, &QAction::triggered, this, &FetMainForm::settingsRestoreDefaultsAction_triggered);
	
	connect(settingsTimetableHtmlLevelAction, &QAction::triggered, this, &FetMainForm::settingsTimetableHtmlLevelAction_triggered);
	
	connect(settingsDataToPrintInTimetablesAction, &QAction::triggered, this, &FetMainForm::settingsDataToPrintInTimetablesAction_triggered);
	
	connect(selectOutputDirAction, &QAction::triggered, this, &FetMainForm::selectOutputDirAction_triggered);
	
	connect(statisticsExportToDiskAction, &QAction::triggered, this, &FetMainForm::statisticsExportToDiskAction_triggered);
	connect(statisticsPrintAction, &QAction::triggered, this, &FetMainForm::statisticsPrintAction_triggered);
	
	connect(settingsShowShortcutsOnMainWindowAction, &QAction::toggled, this, &FetMainForm::settingsShowShortcutsOnMainWindowAction_toggled);
	connect(settingsFontIsUserSelectableAction, &QAction::toggled, this, &FetMainForm::settingsFontIsUserSelectableAction_toggled);
	connect(settingsShowToolTipsForConstraintsWithTablesAction, &QAction::toggled, this, &FetMainForm::settingsShowToolTipsForConstraintsWithTablesAction_toggled);
	
	connect(settingsShowVirtualRoomsInTimetablesAction, &QAction::toggled, this, &FetMainForm::settingsShowVirtualRoomsInTimetablesAction_toggled);

	//////confirmations
	connect(settingsConfirmActivityPlanningAction, &QAction::toggled, this, &FetMainForm::settingsConfirmActivityPlanningAction_toggled);
	connect(settingsConfirmSpreadActivitiesAction, &QAction::toggled, this, &FetMainForm::settingsConfirmSpreadActivitiesAction_toggled);
	connect(settingsConfirmRemoveRedundantAction, &QAction::toggled, this, &FetMainForm::settingsConfirmRemoveRedundantAction_toggled);
	connect(settingsConfirmSaveTimetableAction, &QAction::toggled, this, &FetMainForm::settingsConfirmSaveTimetableAction_toggled);
	connect(settingsConfirmActivateDeactivateActivitiesConstraintsAction, &QAction::toggled, this, &FetMainForm::settingsConfirmActivateDeactivateActivitiesConstraintsAction_toggled);
	//////

	connect(settingsDivideTimetablesByDaysAction, &QAction::toggled, this, &FetMainForm::settingsDivideTimetablesByDaysAction_toggled);
	connect(settingsDuplicateVerticalNamesAction, &QAction::toggled, this, &FetMainForm::settingsDuplicateVerticalNamesAction_toggled);
	
	connect(settingsOrderSubgroupsInTimetablesAction, &QAction::toggled, this, &FetMainForm::settingsOrderSubgroupsInTimetablesAction_toggled);
	connect(settingsPrintDetailedTimetablesAction, &QAction::toggled, this, &FetMainForm::settingsPrintDetailedTimetablesAction_toggled);
	connect(settingsPrintDetailedTeachersFreePeriodsTimetablesAction, &QAction::toggled, this, &FetMainForm::settingsPrintDetailedTeachersFreePeriodsTimetablesAction_toggled);
	connect(settingsPrintNotAvailableSlotsAction, &QAction::toggled, this, &FetMainForm::settingsPrintNotAvailableSlotsAction_toggled);
	connect(settingsPrintBreakSlotsAction, &QAction::toggled, this, &FetMainForm::settingsPrintBreakSlotsAction_toggled);
	
	connect(settingsPrintActivitiesWithSameStartingTimeAction, &QAction::toggled, this, &FetMainForm::settingsPrintActivitiesWithSameStartingTimeAction_toggled);
	
	connect(shortcutAllTimeConstraintsPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutAllTimeConstraintsPushButton_clicked);
	connect(shortcutBreakTimeConstraintsPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutBreakTimeConstraintsPushButton_clicked);
	connect(shortcutTeachersTimeConstraintsPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutTeachersTimeConstraintsPushButton_clicked);
	connect(shortcutStudentsTimeConstraintsPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutStudentsTimeConstraintsPushButton_clicked);
	connect(shortcutActivitiesTimeConstraintsPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutActivitiesTimeConstraintsPushButton_clicked);
	connect(shortcutAdvancedTimeConstraintsPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutAdvancedTimeConstraintsPushButton_clicked);
	
	connect(shortcutAllSpaceConstraintsPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutAllSpaceConstraintsPushButton_clicked);
	connect(shortcutRoomsSpaceConstraintsPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutRoomsSpaceConstraintsPushButton_clicked);
	connect(shortcutTeachersSpaceConstraintsPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutTeachersSpaceConstraintsPushButton_clicked);
	connect(shortcutStudentsSpaceConstraintsPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutStudentsSpaceConstraintsPushButton_clicked);
	connect(shortcutSubjectsSpaceConstraintsPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutSubjectsSpaceConstraintsPushButton_clicked);
	connect(shortcutActivityTagsSpaceConstraintsPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutActivityTagsSpaceConstraintsPushButton_clicked);
	connect(shortcutSubjectsAndActivityTagsSpaceConstraintsPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutSubjectsAndActivityTagsSpaceConstraintsPushButton_clicked);
	connect(shortcutActivitiesSpaceConstraintsPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutActivitiesSpaceConstraintsPushButton_clicked);
	
	connect(shortcutGeneratePushButton, &QPushButton::clicked, this, &FetMainForm::shortcutGeneratePushButton_clicked);
	connect(shortcutGenerateMultiplePushButton, &QPushButton::clicked, this, &FetMainForm::shortcutGenerateMultiplePushButton_clicked);
	connect(shortcutViewTeachersPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutViewTeachersPushButton_clicked);
	connect(shortcutViewStudentsPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutViewStudentsPushButton_clicked);
	connect(shortcutViewRoomsPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutViewRoomsPushButton_clicked);
	connect(shortcutShowSoftConflictsPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutShowSoftConflictsPushButton_clicked);
	//2014-07-01
	connect(shortcutsTimetableAdvancedPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutsTimetableAdvancedPushButton_clicked);
	connect(shortcutsTimetablePrintPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutsTimetablePrintPushButton_clicked);
	connect(shortcutsTimetableLockingPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutsTimetableLockingPushButton_clicked);
	
	connect(shortcutBasicPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutBasicPushButton_clicked);
	connect(shortcutSubjectsPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutSubjectsPushButton_clicked);
	connect(shortcutActivityTagsPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutActivityTagsPushButton_clicked);
	connect(shortcutTeachersPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutTeachersPushButton_clicked);
	connect(shortcutStudentsPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutStudentsPushButton_clicked);
	connect(shortcutActivitiesPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutActivitiesPushButton_clicked);
	connect(shortcutSubactivitiesPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutSubactivitiesPushButton_clicked);
	connect(shortcutDataAdvancedPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutDataAdvancedPushButton_clicked);
	connect(shortcutDataSpacePushButton, &QPushButton::clicked, this, &FetMainForm::shortcutDataSpacePushButton_clicked);

	connect(shortcutOpenPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutOpenPushButton_clicked);
	connect(shortcutOpenRecentPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutOpenRecentPushButton_clicked);
	connect(shortcutNewPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutNewPushButton_clicked);
	connect(shortcutSavePushButton, &QPushButton::clicked, this, &FetMainForm::shortcutSavePushButton_clicked);
	connect(shortcutSaveAsPushButton, &QPushButton::clicked, this, &FetMainForm::shortcutSaveAsPushButton_clicked);

	/*settingsShowShortcutsOnMainWindowAction_toggled();
	settingsFontIsUserSelectableAction_toggled();
	settingsShowToolTipsForConstraintsWithTablesAction_toggled();
	
	settingsShowVirtualRoomsInTimetablesAction_toggled();

	//////confirmations
	settingsConfirmActivityPlanningAction_toggled();
	settingsConfirmSpreadActivitiesAction_toggled();
	settingsConfirmRemoveRedundantAction_toggled();
	settingsConfirmSaveTimetableAction_toggled();
	settingsConfirmActivateDeactivateActivitiesConstraintsAction_toggled();
	//////

	settingsDivideTimetablesByDaysAction_toggled();
	settingsDuplicateVerticalNamesAction_toggled();
	
	settingsOrderSubgroupsInTimetablesAction_toggled();
	settingsPrintDetailedTimetablesAction_toggled();
	settingsPrintDetailedTeachersFreePeriodsTimetablesAction_toggled();
	settingsPrintNotAvailableSlotsAction_toggled();
	settingsPrintBreakSlotsAction_toggled();
	
	settingsPrintActivitiesWithSameStartingTimeAction_toggled();*/
}

void FetMainForm::retranslateMode()
{
	if(dataAvailable){
		assert(gt.rules.initialized);
		assert(currentMode==gt.rules.mode);
		if(gt.rules.mode==OFFICIAL)
			modeLabel.setText(tr("MODE: Official"));
		else if(gt.rules.mode==MORNINGS_AFTERNOONS)
			modeLabel.setText(tr("MODE: Mornings-Afternoons"));
		else if(gt.rules.mode==BLOCK_PLANNING)
			modeLabel.setText(tr("MODE: Block planning"));
		else if(gt.rules.mode==TERMS)
			modeLabel.setText(tr("MODE: Terms"));
		else
			assert(0);
	}
	else{
		if(currentMode==OFFICIAL)
			modeLabel.setText(tr("MODE: Official (no data)"));
		else if(currentMode==MORNINGS_AFTERNOONS)
			modeLabel.setText(tr("MODE: Mornings-Afternoons (no data)"));
		else if(currentMode==BLOCK_PLANNING)
			modeLabel.setText(tr("MODE: Block planning (no data)"));
		else if(currentMode==TERMS)
			modeLabel.setText(tr("MODE: Terms (no data)"));
		else
			assert(0);
	}
}

void FetMainForm::updateMode(bool forceUpdate)
{
	if(!forceUpdate){
		assert(gt.rules.initialized);
		if(gt.rules.mode==currentMode){
			if(!oldDataAvailable && dataAvailable){
				if(gt.rules.mode==OFFICIAL){
					if(dataAvailable)
						modeLabel.setText(tr("MODE: Official"));
					else
						modeLabel.setText(tr("MODE: Official (no data)"));
				}
				else if(gt.rules.mode==MORNINGS_AFTERNOONS){
					if(dataAvailable)
						modeLabel.setText(tr("MODE: Mornings-Afternoons"));
					else
						modeLabel.setText(tr("MODE: Mornings-Afternoons (no data)"));
				}
				else if(gt.rules.mode==BLOCK_PLANNING){
					if(dataAvailable)
						modeLabel.setText(tr("MODE: Block planning"));
					else
						modeLabel.setText(tr("MODE: Block planning (no data)"));
				}
				else if(gt.rules.mode==TERMS){
					if(dataAvailable)
						modeLabel.setText(tr("MODE: Terms"));
					else
						modeLabel.setText(tr("MODE: Terms (no data)"));
				}
				else
					assert(0);
			}
		
			return;
		}
	}
	else{
		//gt.rules have the correct mode, read in fet.cpp, so we continue
	}
	
	currentMode=gt.rules.mode;

	if(gt.rules.mode==OFFICIAL){
		if(dataAvailable)
			modeLabel.setText(tr("MODE: Official"));
		else
			modeLabel.setText(tr("MODE: Official (no data)"));
		
		modeOfficialAction->setChecked(true);
		modeMorningsAfternoonsAction->setChecked(false);
		modeBlockPlanningAction->setChecked(false);
		modeTermsAction->setChecked(false);
	}
	else if(gt.rules.mode==MORNINGS_AFTERNOONS){
		if(dataAvailable)
			modeLabel.setText(tr("MODE: Mornings-Afternoons"));
		else
			modeLabel.setText(tr("MODE: Mornings-Afternoons (no data)"));

		modeOfficialAction->setChecked(false);
		modeMorningsAfternoonsAction->setChecked(true);
		modeBlockPlanningAction->setChecked(false);
		modeTermsAction->setChecked(false);
	}
	else if(gt.rules.mode==BLOCK_PLANNING){
		if(dataAvailable)
			modeLabel.setText(tr("MODE: Block planning"));
		else
			modeLabel.setText(tr("MODE: Block planning (no data)"));

		modeOfficialAction->setChecked(false);
		modeMorningsAfternoonsAction->setChecked(false);
		modeBlockPlanningAction->setChecked(true);
		modeTermsAction->setChecked(false);
	}
	else if(gt.rules.mode==TERMS){
		if(dataAvailable)
			modeLabel.setText(tr("MODE: Terms"));
		else
			modeLabel.setText(tr("MODE: Terms (no data)"));

		modeOfficialAction->setChecked(false);
		modeMorningsAfternoonsAction->setChecked(false);
		modeBlockPlanningAction->setChecked(false);
		modeTermsAction->setChecked(true);
	}
	else
		assert(0);
	
	if(gt.rules.mode==TERMS)
		dataTermsAction->setEnabled(true);
	else
		dataTermsAction->setEnabled(false);

	//(Yes, the enable and warning for min hours per morning with allow empty mornings are always disabled.)
	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		enableActivityTagMaxHoursDailyAction->setText(QCoreApplication::translate("FetMainForm_template", "Enable activity tag max hours daily", nullptr));
	
		enableActivityTagMinHoursDailyAction->setEnabled(true);

		enableMaxGapsPerRealDayAction->setEnabled(false);

		enableStudentsMinHoursDailyWithAllowEmptyDaysAction->setEnabled(true);
		showWarningForStudentsMinHoursDailyWithAllowEmptyDaysAction->setEnabled(true);

		enableStudentsMinHoursPerMorningWithAllowEmptyMorningsAction->setEnabled(false);
		showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsAction->setEnabled(false);

		enableStudentsMinHoursPerAfternoonWithAllowEmptyAfternoonsAction->setEnabled(false);
		showWarningForStudentsMinHoursPerAfternoonWithAllowEmptyAfternoonsAction->setEnabled(false);
	}
	else{
		enableActivityTagMaxHoursDailyAction->setText(QCoreApplication::translate("FetMainForm_template", "Enable activity tag max hours daily / per real day", nullptr));

		enableActivityTagMinHoursDailyAction->setEnabled(true);

		enableMaxGapsPerRealDayAction->setEnabled(true);

		enableStudentsMinHoursDailyWithAllowEmptyDaysAction->setEnabled(false);
		showWarningForStudentsMinHoursDailyWithAllowEmptyDaysAction->setEnabled(false);

		enableStudentsMinHoursPerMorningWithAllowEmptyMorningsAction->setEnabled(false);
		showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsAction->setEnabled(false);

		enableStudentsMinHoursPerAfternoonWithAllowEmptyAfternoonsAction->setEnabled(false);
		showWarningForStudentsMinHoursPerAfternoonWithAllowEmptyAfternoonsAction->setEnabled(false);
	}

	createMenusOfActionsForConstraints();
}

void FetMainForm::createActionsForConstraints()
{
	dataTimeConstraintsActivitiesPreferredTimeSlotsAction = new QAction(this);
	dataTimeConstraintsActivitiesSameStartingTimeAction = new QAction(this);
	dataTimeConstraintsActivitiesOccupyMaxTimeSlotsFromSelectionAction = new QAction(this);
	dataTimeConstraintsActivitiesOccupyMinTimeSlotsFromSelectionAction = new QAction(this);
	dataTimeConstraintsActivitiesMaxSimultaneousInSelectedTimeSlotsAction = new QAction(this);
	dataTimeConstraintsActivitiesMinSimultaneousInSelectedTimeSlotsAction = new QAction(this);
	dataTimeConstraintsTeacherNotAvailableTimesAction = new QAction(this);
	dataTimeConstraintsTeachersNotAvailableTimesAction = new QAction(this);
	dataTimeConstraintsBasicCompulsoryTimeAction = new QAction(this);
	dataTimeConstraintsStudentsSetNotAvailableTimesAction = new QAction(this);
	dataTimeConstraintsStudentsNotAvailableTimesAction = new QAction(this);
	dataTimeConstraintsBreakTimesAction = new QAction(this);
	dataTimeConstraintsTeacherMaxDaysPerWeekAction = new QAction(this);
	dataTimeConstraintsTeachersMaxHoursDailyAction = new QAction(this);

	dataTimeConstraintsTeachersMaxHoursDailyInIntervalAction = new QAction(this);
	dataTimeConstraintsTeacherMaxHoursDailyInIntervalAction = new QAction(this);
	dataTimeConstraintsStudentsMaxHoursDailyInIntervalAction = new QAction(this);
	dataTimeConstraintsStudentsSetMaxHoursDailyInIntervalAction = new QAction(this);

	dataTimeConstraintsActivityPreferredStartingTimeAction = new QAction(this);
	dataTimeConstraintsStudentsSetMaxGapsPerWeekAction = new QAction(this);
	dataTimeConstraintsStudentsMaxGapsPerWeekAction = new QAction(this);
	dataTimeConstraintsStudentsEarlyMaxBeginningsAtSecondHourAction = new QAction(this);
	dataTimeConstraintsActivitiesNotOverlappingAction = new QAction(this);
	dataTimeConstraintsActivityTagsNotOverlappingAction = new QAction(this);
	dataTimeConstraintsMinDaysBetweenActivitiesAction = new QAction(this);
	dataTimeConstraintsMinHalfDaysBetweenActivitiesAction = new QAction(this);
	dataSpaceConstraintsBasicCompulsorySpaceAction = new QAction(this);
	dataSpaceConstraintsRoomNotAvailableTimesAction = new QAction(this);
	dataSpaceConstraintsTeacherRoomNotAvailableTimesAction = new QAction(this);
	dataSpaceConstraintsActivityPreferredRoomAction = new QAction(this);
	dataTimeConstraintsActivitiesSameStartingHourAction = new QAction(this);
	dataSpaceConstraintsActivityPreferredRoomsAction = new QAction(this);
	dataSpaceConstraintsStudentsSetHomeRoomAction = new QAction(this);
	dataSpaceConstraintsStudentsSetHomeRoomsAction = new QAction(this);
	dataTimeConstraintsTeachersMaxGapsPerWeekAction = new QAction(this);
	dataTimeConstraintsTeacherMaxGapsPerWeekAction = new QAction(this);
	dataTimeConstraintsStudentsSetEarlyMaxBeginningsAtSecondHourAction = new QAction(this);
	dataTimeConstraintsTeacherMaxHoursDailyAction = new QAction(this);
	dataTimeConstraintsStudentsSetMaxHoursDailyAction = new QAction(this);
	dataTimeConstraintsStudentsMaxHoursDailyAction = new QAction(this);
	dataTimeConstraintsStudentsMinHoursDailyAction = new QAction(this);
	dataTimeConstraintsStudentsSetMinHoursDailyAction = new QAction(this);
	dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsAction = new QAction(this);
	dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsAction = new QAction(this);
	dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsAction = new QAction(this);
	dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsAction = new QAction(this);
	//2021-12-15
	dataTimeConstraintsStudentsSetMinGapsBetweenActivityTagAction = new QAction(this);
	dataTimeConstraintsStudentsMinGapsBetweenActivityTagAction = new QAction(this);
	dataTimeConstraintsTeacherMinGapsBetweenActivityTagAction = new QAction(this);
	dataTimeConstraintsTeachersMinGapsBetweenActivityTagAction = new QAction(this);
	//2024-03-16
	dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction = new QAction(this);
	dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction = new QAction(this);
	dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction = new QAction(this);
	dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction = new QAction(this);
	//
	dataTimeConstraintsStudentsSetMinGapsBetweenActivityTagPerRealDayAction = new QAction(this);
	dataTimeConstraintsStudentsMinGapsBetweenActivityTagPerRealDayAction = new QAction(this);
	dataTimeConstraintsTeacherMinGapsBetweenActivityTagPerRealDayAction = new QAction(this);
	dataTimeConstraintsTeachersMinGapsBetweenActivityTagPerRealDayAction = new QAction(this);
	//2024-05-20
	dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction = new QAction(this);
	dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction = new QAction(this);
	dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction = new QAction(this);
	dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction = new QAction(this);
	//
	dataTimeConstraintsStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction = new QAction(this);
	dataTimeConstraintsStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction = new QAction(this);
	dataTimeConstraintsTeacherMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction = new QAction(this);
	dataTimeConstraintsTeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction = new QAction(this);
	//
	dataTimeConstraintsTwoActivitiesConsecutiveAction = new QAction(this);
	dataTimeConstraintsActivityEndsStudentsDayAction = new QAction(this);
	dataTimeConstraintsActivityEndsTeachersDayAction = new QAction(this);

	dataTimeConstraintsActivityBeginsStudentsDayAction = new QAction(this);
	dataTimeConstraintsActivityBeginsTeachersDayAction = new QAction(this);

	dataTimeConstraintsTeachersMinHoursDailyAction = new QAction(this);
	dataTimeConstraintsTeacherMinHoursDailyAction = new QAction(this);
	dataTimeConstraintsTeachersMaxGapsPerDayAction = new QAction(this);
	dataTimeConstraintsTeacherMaxGapsPerDayAction = new QAction(this);
	dataTimeConstraintsTeachersMaxGapsPerMorningAndAfternoonAction = new QAction(this);
	dataTimeConstraintsTeacherMaxGapsPerMorningAndAfternoonAction = new QAction(this);
	dataTimeConstraintsTeacherMaxSpanPerDayAction = new QAction(this);
	dataTimeConstraintsTeachersMaxSpanPerDayAction = new QAction(this);
	dataTimeConstraintsStudentsSetMaxSpanPerDayAction = new QAction(this);
	dataTimeConstraintsStudentsMaxSpanPerDayAction = new QAction(this);
	dataTimeConstraintsTeacherMinRestingHoursAction = new QAction(this);
	dataTimeConstraintsTeachersMinRestingHoursAction = new QAction(this);
	dataTimeConstraintsStudentsSetMinRestingHoursAction = new QAction(this);
	dataTimeConstraintsStudentsMinRestingHoursAction = new QAction(this);
	dataSpaceConstraintsSubjectPreferredRoomAction = new QAction(this);
	dataSpaceConstraintsSubjectPreferredRoomsAction = new QAction(this);
	dataSpaceConstraintsSubjectActivityTagPreferredRoomAction = new QAction(this);
	dataSpaceConstraintsSubjectActivityTagPreferredRoomsAction = new QAction(this);
	dataSpaceConstraintsTeacherHomeRoomAction = new QAction(this);
	dataSpaceConstraintsTeacherHomeRoomsAction = new QAction(this);
	dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayAction = new QAction(this);
	dataSpaceConstraintsStudentsMaxBuildingChangesPerDayAction = new QAction(this);
	dataSpaceConstraintsStudentsSetMaxBuildingChangesPerWeekAction = new QAction(this);
	dataSpaceConstraintsStudentsMaxBuildingChangesPerWeekAction = new QAction(this);
	dataSpaceConstraintsStudentsSetMinGapsBetweenBuildingChangesAction = new QAction(this);
	dataSpaceConstraintsStudentsMinGapsBetweenBuildingChangesAction = new QAction(this);
	dataSpaceConstraintsTeacherMaxBuildingChangesPerDayAction = new QAction(this);
	dataSpaceConstraintsTeachersMaxBuildingChangesPerDayAction = new QAction(this);
	dataSpaceConstraintsTeacherMaxBuildingChangesPerWeekAction = new QAction(this);
	dataSpaceConstraintsTeachersMaxBuildingChangesPerWeekAction = new QAction(this);
	dataSpaceConstraintsTeacherMinGapsBetweenBuildingChangesAction = new QAction(this);
	dataSpaceConstraintsTeachersMinGapsBetweenBuildingChangesAction = new QAction(this);
	dataSpaceConstraintsStudentsSetMaxRoomChangesPerDayAction = new QAction(this);
	dataSpaceConstraintsStudentsMaxRoomChangesPerDayAction = new QAction(this);
	dataSpaceConstraintsStudentsSetMaxRoomChangesPerWeekAction = new QAction(this);
	dataSpaceConstraintsStudentsMaxRoomChangesPerWeekAction = new QAction(this);
	dataSpaceConstraintsStudentsSetMinGapsBetweenRoomChangesAction = new QAction(this);
	dataSpaceConstraintsStudentsMinGapsBetweenRoomChangesAction = new QAction(this);
	dataSpaceConstraintsTeacherMaxRoomChangesPerDayAction = new QAction(this);
	dataSpaceConstraintsTeachersMaxRoomChangesPerDayAction = new QAction(this);
	dataSpaceConstraintsTeacherMaxRoomChangesPerWeekAction = new QAction(this);
	dataSpaceConstraintsTeachersMaxRoomChangesPerWeekAction = new QAction(this);
	dataSpaceConstraintsTeacherMinGapsBetweenRoomChangesAction = new QAction(this);
	dataSpaceConstraintsTeachersMinGapsBetweenRoomChangesAction = new QAction(this);
	dataTimeConstraintsActivitiesSameStartingDayAction = new QAction(this);
	dataTimeConstraintsTwoActivitiesOrderedAction = new QAction(this);
	dataTimeConstraintsTwoSetsOfActivitiesOrderedAction = new QAction(this);
	dataTimeConstraintsTwoActivitiesOrderedIfSameDayAction = new QAction(this);
	dataTimeConstraintsTeachersMaxHoursContinuouslyAction = new QAction(this);
	dataTimeConstraintsTeacherMaxHoursContinuouslyAction = new QAction(this);
	dataTimeConstraintsStudentsSetMaxHoursContinuouslyAction = new QAction(this);
	dataTimeConstraintsStudentsMaxHoursContinuouslyAction = new QAction(this);
	dataTimeConstraintsActivitiesPreferredStartingTimesAction = new QAction(this);
	dataTimeConstraintsActivityPreferredTimeSlotsAction = new QAction(this);
	dataTimeConstraintsActivityPreferredStartingTimesAction = new QAction(this);
	dataTimeConstraintsMinGapsBetweenActivitiesAction = new QAction(this);
	dataTimeConstraintsSubactivitiesPreferredTimeSlotsAction = new QAction(this);
	dataTimeConstraintsSubactivitiesPreferredStartingTimesAction = new QAction(this);
	dataTimeConstraintsTeacherIntervalMaxDaysPerWeekAction = new QAction(this);
	dataTimeConstraintsTeachersIntervalMaxDaysPerWeekAction = new QAction(this);
	dataTimeConstraintsStudentsSetIntervalMaxDaysPerWeekAction = new QAction(this);
	dataTimeConstraintsStudentsIntervalMaxDaysPerWeekAction = new QAction(this);
	dataTimeConstraintsActivitiesEndStudentsDayAction = new QAction(this);
	dataTimeConstraintsActivitiesEndTeachersDayAction = new QAction(this);

	dataTimeConstraintsActivitiesBeginStudentsDayAction = new QAction(this);
	dataTimeConstraintsActivitiesBeginTeachersDayAction = new QAction(this);

	dataTimeConstraintsTwoActivitiesGroupedAction = new QAction(this);
	dataTimeConstraintsStudentsSetActivityTagMaxHoursContinuouslyAction = new QAction(this);
	dataTimeConstraintsStudentsActivityTagMaxHoursContinuouslyAction = new QAction(this);
	dataTimeConstraintsTeacherActivityTagMaxHoursContinuouslyAction = new QAction(this);
	dataTimeConstraintsTeachersActivityTagMaxHoursContinuouslyAction = new QAction(this);
	dataSpaceConstraintsActivityTagPreferredRoomAction = new QAction(this);
	dataSpaceConstraintsActivityTagPreferredRoomsAction = new QAction(this);
	dataTimeConstraintsTeachersMaxDaysPerWeekAction = new QAction(this);
	dataTimeConstraintsThreeActivitiesGroupedAction = new QAction(this);
	dataTimeConstraintsMaxDaysBetweenActivitiesAction = new QAction(this);
	dataTimeConstraintsActivitiesMaxHourlySpanAction = new QAction(this);
	dataTimeConstraintsMaxHalfDaysBetweenActivitiesAction = new QAction(this);
	dataTimeConstraintsMaxTermsBetweenActivitiesAction = new QAction(this);
	dataTimeConstraintsTeacherMinDaysPerWeekAction = new QAction(this);
	dataTimeConstraintsTeachersMinDaysPerWeekAction = new QAction(this);
	dataTimeConstraintsTeacherActivityTagMaxHoursDailyAction = new QAction(this);
	dataTimeConstraintsTeachersActivityTagMaxHoursDailyAction = new QAction(this);
	dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyAction = new QAction(this);
	dataTimeConstraintsStudentsActivityTagMaxHoursDailyAction = new QAction(this);
	dataTimeConstraintsTeacherActivityTagMinHoursDailyAction = new QAction(this);
	dataTimeConstraintsTeachersActivityTagMinHoursDailyAction = new QAction(this);
	dataTimeConstraintsStudentsSetActivityTagMinHoursDailyAction = new QAction(this);
	dataTimeConstraintsStudentsActivityTagMinHoursDailyAction = new QAction(this);
	dataTimeConstraintsStudentsSetMaxGapsPerDayAction = new QAction(this);
	dataTimeConstraintsStudentsMaxGapsPerDayAction = new QAction(this);
	dataSpaceConstraintsActivitiesOccupyMaxDifferentRoomsAction = new QAction(this);
	dataSpaceConstraintsActivitiesSameRoomIfConsecutiveAction = new QAction(this);
	dataTimeConstraintsStudentsSetMaxDaysPerWeekAction = new QAction(this);
	dataTimeConstraintsStudentsMaxDaysPerWeekAction = new QAction(this);

	//mornings-afternoons
	dataTimeConstraintsTeacherMaxRealDaysPerWeekAction = new QAction(this);
	dataTimeConstraintsTeacherMaxMorningsPerWeekAction = new QAction(this);
	dataTimeConstraintsTeacherMaxTwoConsecutiveMorningsAction = new QAction(this);
	dataTimeConstraintsTeachersMaxTwoConsecutiveMorningsAction = new QAction(this);
	dataTimeConstraintsTeacherMaxTwoConsecutiveAfternoonsAction = new QAction(this);
	dataTimeConstraintsTeachersMaxTwoConsecutiveAfternoonsAction = new QAction(this);
	dataTimeConstraintsTeacherMaxAfternoonsPerWeekAction = new QAction(this);
	dataTimeConstraintsTeachersMaxHoursDailyRealDaysAction = new QAction(this);
	dataTimeConstraintsTeachersAfternoonsEarlyMaxBeginningsAtSecondHourAction = new QAction(this);
	dataTimeConstraintsTeacherAfternoonsEarlyMaxBeginningsAtSecondHourAction = new QAction(this);
	dataTimeConstraintsStudentsAfternoonsEarlyMaxBeginningsAtSecondHourAction = new QAction(this);
	dataTimeConstraintsStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHourAction = new QAction(this);

	dataTimeConstraintsTeachersMorningsEarlyMaxBeginningsAtSecondHourAction = new QAction(this);
	dataTimeConstraintsTeacherMorningsEarlyMaxBeginningsAtSecondHourAction = new QAction(this);
	dataTimeConstraintsStudentsMorningsEarlyMaxBeginningsAtSecondHourAction = new QAction(this);
	dataTimeConstraintsStudentsSetMorningsEarlyMaxBeginningsAtSecondHourAction = new QAction(this);

	dataTimeConstraintsTeacherMaxHoursDailyRealDaysAction = new QAction(this);
	dataTimeConstraintsStudentsSetMaxHoursDailyRealDaysAction = new QAction(this);
	dataTimeConstraintsStudentsMaxHoursDailyRealDaysAction = new QAction(this);
	dataTimeConstraintsStudentsMinHoursPerMorningAction = new QAction(this);
	dataTimeConstraintsStudentsSetMinHoursPerMorningAction = new QAction(this);
	dataTimeConstraintsTeachersMinHoursPerMorningAction = new QAction(this);

	//2022-09-10
	dataTimeConstraintsStudentsMinHoursPerAfternoonAction = new QAction(this);
	dataTimeConstraintsStudentsSetMinHoursPerAfternoonAction = new QAction(this);
	dataTimeConstraintsTeachersMinHoursPerAfternoonAction = new QAction(this);
	dataTimeConstraintsTeacherMinHoursPerAfternoonAction = new QAction(this);

	dataTimeConstraintsTeachersMinHoursDailyRealDaysAction = new QAction(this);
	dataTimeConstraintsTeacherMaxHoursPerAllAfternoonsAction = new QAction(this);
	dataTimeConstraintsTeachersMaxHoursPerAllAfternoonsAction = new QAction(this);
	dataTimeConstraintsStudentsSetMaxHoursPerAllAfternoonsAction = new QAction(this);
	dataTimeConstraintsStudentsMaxHoursPerAllAfternoonsAction = new QAction(this);
	dataTimeConstraintsTeacherMinHoursPerMorningAction = new QAction(this);
	dataTimeConstraintsTeacherMinHoursDailyRealDaysAction = new QAction(this);
	dataTimeConstraintsTeachersMaxZeroGapsPerAfternoonAction = new QAction(this);
	dataTimeConstraintsTeacherMaxZeroGapsPerAfternoonAction = new QAction(this);
	dataTimeConstraintsTeacherMaxSpanPerRealDayAction = new QAction(this);
	dataTimeConstraintsTeachersMaxSpanPerRealDayAction = new QAction(this);
	dataTimeConstraintsStudentsSetMaxSpanPerRealDayAction = new QAction(this);
	dataTimeConstraintsStudentsMaxSpanPerRealDayAction = new QAction(this);
	dataTimeConstraintsTeacherMinRestingHoursBetweenMorningAndAfternoonAction = new QAction(this);
	dataTimeConstraintsTeachersMinRestingHoursBetweenMorningAndAfternoonAction = new QAction(this);
	dataTimeConstraintsStudentsSetMinRestingHoursBetweenMorningAndAfternoonAction = new QAction(this);
	dataTimeConstraintsStudentsMinRestingHoursBetweenMorningAndAfternoonAction = new QAction(this);
	dataTimeConstraintsTeacherMorningIntervalMaxDaysPerWeekAction = new QAction(this);
	dataTimeConstraintsTeachersMorningIntervalMaxDaysPerWeekAction = new QAction(this);
	dataTimeConstraintsTeacherAfternoonIntervalMaxDaysPerWeekAction = new QAction(this);
	dataTimeConstraintsTeachersAfternoonIntervalMaxDaysPerWeekAction = new QAction(this);
	dataTimeConstraintsStudentsSetMorningIntervalMaxDaysPerWeekAction = new QAction(this);
	dataTimeConstraintsStudentsMorningIntervalMaxDaysPerWeekAction = new QAction(this);
	dataTimeConstraintsStudentsSetAfternoonIntervalMaxDaysPerWeekAction = new QAction(this);
	dataTimeConstraintsStudentsAfternoonIntervalMaxDaysPerWeekAction = new QAction(this);
	dataTimeConstraintsTeachersMaxRealDaysPerWeekAction = new QAction(this);
	dataTimeConstraintsTeachersMaxMorningsPerWeekAction = new QAction(this);
	dataTimeConstraintsTeachersMaxAfternoonsPerWeekAction = new QAction(this);
	dataTimeConstraintsTeacherMinRealDaysPerWeekAction = new QAction(this);
	dataTimeConstraintsTeachersMinRealDaysPerWeekAction = new QAction(this);
	dataTimeConstraintsTeacherMinMorningsPerWeekAction = new QAction(this);
	dataTimeConstraintsTeachersMinMorningsPerWeekAction = new QAction(this);
	dataTimeConstraintsTeacherMinAfternoonsPerWeekAction = new QAction(this);
	dataTimeConstraintsTeachersMinAfternoonsPerWeekAction = new QAction(this);
	dataTimeConstraintsTeacherActivityTagMaxHoursDailyRealDaysAction = new QAction(this);
	dataTimeConstraintsTeacherMaxTwoActivityTagsPerDayFromN1N2N3Action = new QAction(this);
	dataTimeConstraintsTeachersMaxTwoActivityTagsPerDayFromN1N2N3Action = new QAction(this);
	dataTimeConstraintsStudentsSetMaxTwoActivityTagsPerDayFromN1N2N3Action = new QAction(this);
	dataTimeConstraintsStudentsMaxTwoActivityTagsPerDayFromN1N2N3Action = new QAction(this);
	dataTimeConstraintsTeacherMaxTwoActivityTagsPerRealDayFromN1N2N3Action = new QAction(this);
	dataTimeConstraintsTeachersMaxTwoActivityTagsPerRealDayFromN1N2N3Action = new QAction(this);
	dataTimeConstraintsStudentsSetMaxTwoActivityTagsPerRealDayFromN1N2N3Action = new QAction(this);
	dataTimeConstraintsStudentsMaxTwoActivityTagsPerRealDayFromN1N2N3Action = new QAction(this);
	dataTimeConstraintsTeachersActivityTagMaxHoursDailyRealDaysAction = new QAction(this);
	dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyRealDaysAction = new QAction(this);
	dataTimeConstraintsStudentsActivityTagMaxHoursDailyRealDaysAction = new QAction(this);
	dataTimeConstraintsStudentsSetMaxGapsPerRealDayAction = new QAction(this);
	dataTimeConstraintsStudentsMaxGapsPerRealDayAction = new QAction(this);
	dataTimeConstraintsStudentsSetMaxRealDaysPerWeekAction = new QAction(this);
	dataTimeConstraintsStudentsMaxRealDaysPerWeekAction = new QAction(this);
	dataTimeConstraintsStudentsSetMaxMorningsPerWeekAction = new QAction(this);
	dataTimeConstraintsStudentsMaxMorningsPerWeekAction = new QAction(this);
	dataTimeConstraintsStudentsSetMaxAfternoonsPerWeekAction = new QAction(this);
	dataTimeConstraintsStudentsMaxAfternoonsPerWeekAction = new QAction(this);
	dataTimeConstraintsStudentsSetMinMorningsPerWeekAction = new QAction(this);
	dataTimeConstraintsStudentsMinMorningsPerWeekAction = new QAction(this);
	dataTimeConstraintsStudentsSetMinAfternoonsPerWeekAction = new QAction(this);
	dataTimeConstraintsStudentsMinAfternoonsPerWeekAction = new QAction(this);
	dataTimeConstraintsTeacherMaxGapsPerRealDayAction = new QAction(this);
	dataTimeConstraintsTeachersMaxGapsPerRealDayAction = new QAction(this);
	dataTimeConstraintsStudentsSetMaxGapsPerWeekForRealDaysAction = new QAction(this);
	dataTimeConstraintsStudentsMaxGapsPerWeekForRealDaysAction = new QAction(this);
	dataTimeConstraintsTeacherMaxGapsPerWeekForRealDaysAction = new QAction(this);
	dataTimeConstraintsTeachersMaxGapsPerWeekForRealDaysAction = new QAction(this);
	dataSpaceConstraintsStudentsSetMaxRoomChangesPerRealDayAction = new QAction(this);
	dataSpaceConstraintsStudentsMaxRoomChangesPerRealDayAction = new QAction(this);
	dataSpaceConstraintsTeacherMaxRoomChangesPerRealDayAction = new QAction(this);
	dataSpaceConstraintsTeachersMaxRoomChangesPerRealDayAction = new QAction(this);

	dataSpaceConstraintsStudentsSetMaxBuildingChangesPerRealDayAction = new QAction(this);
	dataSpaceConstraintsStudentsMaxBuildingChangesPerRealDayAction = new QAction(this);
	dataSpaceConstraintsTeacherMaxBuildingChangesPerRealDayAction = new QAction(this);
	dataSpaceConstraintsTeachersMaxBuildingChangesPerRealDayAction = new QAction(this);

	dataTimeConstraintsTeacherMaxThreeConsecutiveDaysAction = new QAction(this);
	dataTimeConstraintsTeachersMaxThreeConsecutiveDaysAction = new QAction(this);
	
	dataTimeConstraintsStudentsSetMaxThreeConsecutiveDaysAction = new QAction(this);
	dataTimeConstraintsStudentsMaxThreeConsecutiveDaysAction = new QAction(this);
	
	//block-planning
	dataTimeConstraintsMaxGapsBetweenActivitiesAction = new QAction(this);
	dataTimeConstraintsMaxTotalActivitiesFromSetInSelectedTimeSlotsAction = new QAction(this);
	
	//terms
	dataTimeConstraintsActivitiesMaxInATermAction = new QAction(this);
	dataTimeConstraintsActivitiesMinInATermAction = new QAction(this);
	dataTimeConstraintsActivitiesOccupyMaxTermsAction = new QAction(this);

	//2024-02-09
	dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayInIntervalAction = new QAction(this);
	dataSpaceConstraintsStudentsMaxBuildingChangesPerDayInIntervalAction = new QAction(this);
	dataSpaceConstraintsTeacherMaxBuildingChangesPerDayInIntervalAction = new QAction(this);
	dataSpaceConstraintsTeachersMaxBuildingChangesPerDayInIntervalAction = new QAction(this);
	dataSpaceConstraintsStudentsSetMaxBuildingChangesPerRealDayInIntervalAction = new QAction(this);
	dataSpaceConstraintsStudentsMaxBuildingChangesPerRealDayInIntervalAction = new QAction(this);
	dataSpaceConstraintsTeacherMaxBuildingChangesPerRealDayInIntervalAction = new QAction(this);
	dataSpaceConstraintsTeachersMaxBuildingChangesPerRealDayInIntervalAction = new QAction(this);

	//2024-02-19
	dataSpaceConstraintsStudentsSetMaxRoomChangesPerDayInIntervalAction = new QAction(this);
	dataSpaceConstraintsStudentsMaxRoomChangesPerDayInIntervalAction = new QAction(this);
	dataSpaceConstraintsTeacherMaxRoomChangesPerDayInIntervalAction = new QAction(this);
	dataSpaceConstraintsTeachersMaxRoomChangesPerDayInIntervalAction = new QAction(this);
	dataSpaceConstraintsStudentsSetMaxRoomChangesPerRealDayInIntervalAction = new QAction(this);
	dataSpaceConstraintsStudentsMaxRoomChangesPerRealDayInIntervalAction = new QAction(this);
	dataSpaceConstraintsTeacherMaxRoomChangesPerRealDayInIntervalAction = new QAction(this);
	dataSpaceConstraintsTeachersMaxRoomChangesPerRealDayInIntervalAction = new QAction(this);

	connect(dataTimeConstraintsActivitiesPreferredTimeSlotsAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsActivitiesPreferredTimeSlotsAction_triggered);
	connect(dataTimeConstraintsActivitiesSameStartingTimeAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsActivitiesSameStartingTimeAction_triggered);
	connect(dataTimeConstraintsActivitiesOccupyMaxTimeSlotsFromSelectionAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsActivitiesOccupyMaxTimeSlotsFromSelectionAction_triggered);
	connect(dataTimeConstraintsActivitiesOccupyMinTimeSlotsFromSelectionAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsActivitiesOccupyMinTimeSlotsFromSelectionAction_triggered);
	connect(dataTimeConstraintsActivitiesMaxSimultaneousInSelectedTimeSlotsAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsActivitiesMaxSimultaneousInSelectedTimeSlotsAction_triggered);
	connect(dataTimeConstraintsActivitiesMinSimultaneousInSelectedTimeSlotsAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsActivitiesMinSimultaneousInSelectedTimeSlotsAction_triggered);
	connect(dataTimeConstraintsTeacherNotAvailableTimesAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherNotAvailableTimesAction_triggered);
	connect(dataTimeConstraintsTeachersNotAvailableTimesAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersNotAvailableTimesAction_triggered);
	connect(dataTimeConstraintsBasicCompulsoryTimeAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsBasicCompulsoryTimeAction_triggered);
	connect(dataTimeConstraintsStudentsSetNotAvailableTimesAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetNotAvailableTimesAction_triggered);
	connect(dataTimeConstraintsStudentsNotAvailableTimesAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsNotAvailableTimesAction_triggered);
	connect(dataTimeConstraintsBreakTimesAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsBreakTimesAction_triggered);
	connect(dataTimeConstraintsTeacherMaxDaysPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMaxDaysPerWeekAction_triggered);
	connect(dataTimeConstraintsTeachersMaxHoursDailyAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMaxHoursDailyAction_triggered);

	connect(dataTimeConstraintsTeachersMaxHoursDailyInIntervalAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMaxHoursDailyInIntervalAction_triggered);
	connect(dataTimeConstraintsTeacherMaxHoursDailyInIntervalAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMaxHoursDailyInIntervalAction_triggered);
	connect(dataTimeConstraintsStudentsMaxHoursDailyInIntervalAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMaxHoursDailyInIntervalAction_triggered);
	connect(dataTimeConstraintsStudentsSetMaxHoursDailyInIntervalAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMaxHoursDailyInIntervalAction_triggered);

	connect(dataTimeConstraintsActivityPreferredStartingTimeAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsActivityPreferredStartingTimeAction_triggered);
	connect(dataTimeConstraintsStudentsSetMaxGapsPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMaxGapsPerWeekAction_triggered);
	connect(dataTimeConstraintsStudentsMaxGapsPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMaxGapsPerWeekAction_triggered);
	connect(dataTimeConstraintsStudentsEarlyMaxBeginningsAtSecondHourAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsEarlyMaxBeginningsAtSecondHourAction_triggered);
	connect(dataTimeConstraintsActivitiesNotOverlappingAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsActivitiesNotOverlappingAction_triggered);
	connect(dataTimeConstraintsActivityTagsNotOverlappingAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsActivityTagsNotOverlappingAction_triggered);
	connect(dataTimeConstraintsMinDaysBetweenActivitiesAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsMinDaysBetweenActivitiesAction_triggered);
	connect(dataTimeConstraintsMinHalfDaysBetweenActivitiesAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsMinHalfDaysBetweenActivitiesAction_triggered);
	connect(dataSpaceConstraintsBasicCompulsorySpaceAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsBasicCompulsorySpaceAction_triggered);
	connect(dataSpaceConstraintsRoomNotAvailableTimesAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsRoomNotAvailableTimesAction_triggered);
	connect(dataSpaceConstraintsTeacherRoomNotAvailableTimesAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsTeacherRoomNotAvailableTimesAction_triggered);
	connect(dataSpaceConstraintsActivityPreferredRoomAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsActivityPreferredRoomAction_triggered);
	connect(dataTimeConstraintsActivitiesSameStartingHourAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsActivitiesSameStartingHourAction_triggered);
	connect(dataSpaceConstraintsActivityPreferredRoomsAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsActivityPreferredRoomsAction_triggered);
	connect(dataSpaceConstraintsStudentsSetHomeRoomAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsStudentsSetHomeRoomAction_triggered);
	connect(dataSpaceConstraintsStudentsSetHomeRoomsAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsStudentsSetHomeRoomsAction_triggered);
	connect(dataTimeConstraintsTeachersMaxGapsPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMaxGapsPerWeekAction_triggered);
	connect(dataTimeConstraintsTeacherMaxGapsPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMaxGapsPerWeekAction_triggered);
	connect(dataTimeConstraintsStudentsSetEarlyMaxBeginningsAtSecondHourAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetEarlyMaxBeginningsAtSecondHourAction_triggered);
	connect(dataTimeConstraintsTeacherMaxHoursDailyAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMaxHoursDailyAction_triggered);
	connect(dataTimeConstraintsStudentsSetMaxHoursDailyAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMaxHoursDailyAction_triggered);
	connect(dataTimeConstraintsStudentsMaxHoursDailyAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMaxHoursDailyAction_triggered);
	connect(dataTimeConstraintsStudentsMinHoursDailyAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMinHoursDailyAction_triggered);
	connect(dataTimeConstraintsStudentsSetMinHoursDailyAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMinHoursDailyAction_triggered);
	connect(dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsAction_triggered);
	connect(dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsAction_triggered);
	connect(dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsAction_triggered);
	connect(dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsAction_triggered);
	//2021-12-15
	connect(dataTimeConstraintsStudentsSetMinGapsBetweenActivityTagAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMinGapsBetweenActivityTagAction_triggered);
	connect(dataTimeConstraintsStudentsMinGapsBetweenActivityTagAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMinGapsBetweenActivityTagAction_triggered);
	connect(dataTimeConstraintsTeacherMinGapsBetweenActivityTagAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMinGapsBetweenActivityTagAction_triggered);
	connect(dataTimeConstraintsTeachersMinGapsBetweenActivityTagAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMinGapsBetweenActivityTagAction_triggered);
	//2024-03-16
	connect(dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction_triggered);
	connect(dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction_triggered);
	connect(dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction_triggered);
	connect(dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction_triggered);
	//
	connect(dataTimeConstraintsStudentsSetMinGapsBetweenActivityTagPerRealDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMinGapsBetweenActivityTagPerRealDayAction_triggered);
	connect(dataTimeConstraintsStudentsMinGapsBetweenActivityTagPerRealDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMinGapsBetweenActivityTagPerRealDayAction_triggered);
	connect(dataTimeConstraintsTeacherMinGapsBetweenActivityTagPerRealDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMinGapsBetweenActivityTagPerRealDayAction_triggered);
	connect(dataTimeConstraintsTeachersMinGapsBetweenActivityTagPerRealDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMinGapsBetweenActivityTagPerRealDayAction_triggered);
	//2024-05-20
	connect(dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction_triggered);
	connect(dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction_triggered);
	connect(dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction_triggered);
	connect(dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction_triggered);
	//
	connect(dataTimeConstraintsStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction_triggered);
	connect(dataTimeConstraintsStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction_triggered);
	connect(dataTimeConstraintsTeacherMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction_triggered);
	connect(dataTimeConstraintsTeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction_triggered);
	//
	connect(dataTimeConstraintsTwoActivitiesConsecutiveAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTwoActivitiesConsecutiveAction_triggered);
	connect(dataTimeConstraintsActivityEndsStudentsDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsActivityEndsStudentsDayAction_triggered);
	connect(dataTimeConstraintsActivityEndsTeachersDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsActivityEndsTeachersDayAction_triggered);

	connect(dataTimeConstraintsActivityBeginsStudentsDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsActivityBeginsStudentsDayAction_triggered);
	connect(dataTimeConstraintsActivityBeginsTeachersDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsActivityBeginsTeachersDayAction_triggered);

	connect(dataTimeConstraintsTeachersMinHoursDailyAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMinHoursDailyAction_triggered);
	connect(dataTimeConstraintsTeacherMinHoursDailyAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMinHoursDailyAction_triggered);
	connect(dataTimeConstraintsTeachersMaxGapsPerDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMaxGapsPerDayAction_triggered);
	connect(dataTimeConstraintsTeacherMaxGapsPerDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMaxGapsPerDayAction_triggered);
	connect(dataTimeConstraintsTeachersMaxGapsPerMorningAndAfternoonAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMaxGapsPerMorningAndAfternoonAction_triggered);
	connect(dataTimeConstraintsTeacherMaxGapsPerMorningAndAfternoonAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMaxGapsPerMorningAndAfternoonAction_triggered);
	connect(dataTimeConstraintsTeacherMaxSpanPerDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMaxSpanPerDayAction_triggered);
	connect(dataTimeConstraintsTeachersMaxSpanPerDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMaxSpanPerDayAction_triggered);
	connect(dataTimeConstraintsStudentsSetMaxSpanPerDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMaxSpanPerDayAction_triggered);
	connect(dataTimeConstraintsStudentsMaxSpanPerDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMaxSpanPerDayAction_triggered);
	connect(dataTimeConstraintsTeacherMinRestingHoursAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMinRestingHoursAction_triggered);
	connect(dataTimeConstraintsTeachersMinRestingHoursAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMinRestingHoursAction_triggered);
	connect(dataTimeConstraintsStudentsSetMinRestingHoursAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMinRestingHoursAction_triggered);
	connect(dataTimeConstraintsStudentsMinRestingHoursAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMinRestingHoursAction_triggered);
	connect(dataSpaceConstraintsSubjectPreferredRoomAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsSubjectPreferredRoomAction_triggered);
	connect(dataSpaceConstraintsSubjectPreferredRoomsAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsSubjectPreferredRoomsAction_triggered);
	connect(dataSpaceConstraintsSubjectActivityTagPreferredRoomAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsSubjectActivityTagPreferredRoomAction_triggered);
	connect(dataSpaceConstraintsSubjectActivityTagPreferredRoomsAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsSubjectActivityTagPreferredRoomsAction_triggered);
	connect(dataSpaceConstraintsTeacherHomeRoomAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsTeacherHomeRoomAction_triggered);
	connect(dataSpaceConstraintsTeacherHomeRoomsAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsTeacherHomeRoomsAction_triggered);
	connect(dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayAction_triggered);
	connect(dataSpaceConstraintsStudentsMaxBuildingChangesPerDayAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsStudentsMaxBuildingChangesPerDayAction_triggered);
	connect(dataSpaceConstraintsStudentsSetMaxBuildingChangesPerWeekAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsStudentsSetMaxBuildingChangesPerWeekAction_triggered);
	connect(dataSpaceConstraintsStudentsMaxBuildingChangesPerWeekAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsStudentsMaxBuildingChangesPerWeekAction_triggered);
	connect(dataSpaceConstraintsStudentsSetMinGapsBetweenBuildingChangesAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsStudentsSetMinGapsBetweenBuildingChangesAction_triggered);
	connect(dataSpaceConstraintsStudentsMinGapsBetweenBuildingChangesAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsStudentsMinGapsBetweenBuildingChangesAction_triggered);
	connect(dataSpaceConstraintsTeacherMaxBuildingChangesPerDayAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsTeacherMaxBuildingChangesPerDayAction_triggered);
	connect(dataSpaceConstraintsTeachersMaxBuildingChangesPerDayAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsTeachersMaxBuildingChangesPerDayAction_triggered);
	connect(dataSpaceConstraintsTeacherMaxBuildingChangesPerWeekAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsTeacherMaxBuildingChangesPerWeekAction_triggered);
	connect(dataSpaceConstraintsTeachersMaxBuildingChangesPerWeekAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsTeachersMaxBuildingChangesPerWeekAction_triggered);
	connect(dataSpaceConstraintsTeacherMinGapsBetweenBuildingChangesAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsTeacherMinGapsBetweenBuildingChangesAction_triggered);
	connect(dataSpaceConstraintsTeachersMinGapsBetweenBuildingChangesAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsTeachersMinGapsBetweenBuildingChangesAction_triggered);
	connect(dataSpaceConstraintsStudentsSetMaxRoomChangesPerDayAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsStudentsSetMaxRoomChangesPerDayAction_triggered);
	connect(dataSpaceConstraintsStudentsMaxRoomChangesPerDayAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsStudentsMaxRoomChangesPerDayAction_triggered);
	connect(dataSpaceConstraintsStudentsSetMaxRoomChangesPerWeekAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsStudentsSetMaxRoomChangesPerWeekAction_triggered);
	connect(dataSpaceConstraintsStudentsMaxRoomChangesPerWeekAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsStudentsMaxRoomChangesPerWeekAction_triggered);
	connect(dataSpaceConstraintsStudentsSetMinGapsBetweenRoomChangesAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsStudentsSetMinGapsBetweenRoomChangesAction_triggered);
	connect(dataSpaceConstraintsStudentsMinGapsBetweenRoomChangesAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsStudentsMinGapsBetweenRoomChangesAction_triggered);
	connect(dataSpaceConstraintsTeacherMaxRoomChangesPerDayAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsTeacherMaxRoomChangesPerDayAction_triggered);
	connect(dataSpaceConstraintsTeachersMaxRoomChangesPerDayAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsTeachersMaxRoomChangesPerDayAction_triggered);
	connect(dataSpaceConstraintsTeacherMaxRoomChangesPerWeekAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsTeacherMaxRoomChangesPerWeekAction_triggered);
	connect(dataSpaceConstraintsTeachersMaxRoomChangesPerWeekAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsTeachersMaxRoomChangesPerWeekAction_triggered);
	connect(dataSpaceConstraintsTeacherMinGapsBetweenRoomChangesAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsTeacherMinGapsBetweenRoomChangesAction_triggered);
	connect(dataSpaceConstraintsTeachersMinGapsBetweenRoomChangesAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsTeachersMinGapsBetweenRoomChangesAction_triggered);
	connect(dataTimeConstraintsActivitiesSameStartingDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsActivitiesSameStartingDayAction_triggered);
	connect(dataTimeConstraintsTwoActivitiesOrderedAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTwoActivitiesOrderedAction_triggered);
	connect(dataTimeConstraintsTwoSetsOfActivitiesOrderedAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTwoSetsOfActivitiesOrderedAction_triggered);
	connect(dataTimeConstraintsTwoActivitiesOrderedIfSameDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTwoActivitiesOrderedIfSameDayAction_triggered);
	connect(dataTimeConstraintsTeachersMaxHoursContinuouslyAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMaxHoursContinuouslyAction_triggered);
	connect(dataTimeConstraintsTeacherMaxHoursContinuouslyAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMaxHoursContinuouslyAction_triggered);
	connect(dataTimeConstraintsStudentsSetMaxHoursContinuouslyAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMaxHoursContinuouslyAction_triggered);
	connect(dataTimeConstraintsStudentsMaxHoursContinuouslyAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMaxHoursContinuouslyAction_triggered);
	connect(dataTimeConstraintsActivitiesPreferredStartingTimesAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsActivitiesPreferredStartingTimesAction_triggered);
	connect(dataTimeConstraintsActivityPreferredTimeSlotsAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsActivityPreferredTimeSlotsAction_triggered);
	connect(dataTimeConstraintsActivityPreferredStartingTimesAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsActivityPreferredStartingTimesAction_triggered);
	connect(dataTimeConstraintsMinGapsBetweenActivitiesAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsMinGapsBetweenActivitiesAction_triggered);
	connect(dataTimeConstraintsSubactivitiesPreferredTimeSlotsAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsSubactivitiesPreferredTimeSlotsAction_triggered);
	connect(dataTimeConstraintsSubactivitiesPreferredStartingTimesAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsSubactivitiesPreferredStartingTimesAction_triggered);
	connect(dataTimeConstraintsTeacherIntervalMaxDaysPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherIntervalMaxDaysPerWeekAction_triggered);
	connect(dataTimeConstraintsTeachersIntervalMaxDaysPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersIntervalMaxDaysPerWeekAction_triggered);
	connect(dataTimeConstraintsStudentsSetIntervalMaxDaysPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetIntervalMaxDaysPerWeekAction_triggered);
	connect(dataTimeConstraintsStudentsIntervalMaxDaysPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsIntervalMaxDaysPerWeekAction_triggered);
	connect(dataTimeConstraintsActivitiesEndStudentsDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsActivitiesEndStudentsDayAction_triggered);
	connect(dataTimeConstraintsActivitiesEndTeachersDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsActivitiesEndTeachersDayAction_triggered);

	connect(dataTimeConstraintsActivitiesBeginStudentsDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsActivitiesBeginStudentsDayAction_triggered);
	connect(dataTimeConstraintsActivitiesBeginTeachersDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsActivitiesBeginTeachersDayAction_triggered);

	connect(dataTimeConstraintsTwoActivitiesGroupedAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTwoActivitiesGroupedAction_triggered);
	connect(dataTimeConstraintsStudentsSetActivityTagMaxHoursContinuouslyAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetActivityTagMaxHoursContinuouslyAction_triggered);
	connect(dataTimeConstraintsStudentsActivityTagMaxHoursContinuouslyAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsActivityTagMaxHoursContinuouslyAction_triggered);
	connect(dataTimeConstraintsTeacherActivityTagMaxHoursContinuouslyAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherActivityTagMaxHoursContinuouslyAction_triggered);
	connect(dataTimeConstraintsTeachersActivityTagMaxHoursContinuouslyAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersActivityTagMaxHoursContinuouslyAction_triggered);
	connect(dataSpaceConstraintsActivityTagPreferredRoomAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsActivityTagPreferredRoomAction_triggered);
	connect(dataSpaceConstraintsActivityTagPreferredRoomsAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsActivityTagPreferredRoomsAction_triggered);
	connect(dataTimeConstraintsTeachersMaxDaysPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMaxDaysPerWeekAction_triggered);
	connect(dataTimeConstraintsThreeActivitiesGroupedAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsThreeActivitiesGroupedAction_triggered);
	connect(dataTimeConstraintsMaxDaysBetweenActivitiesAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsMaxDaysBetweenActivitiesAction_triggered);
	connect(dataTimeConstraintsActivitiesMaxHourlySpanAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsActivitiesMaxHourlySpanAction_triggered);
	connect(dataTimeConstraintsMaxHalfDaysBetweenActivitiesAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsMaxHalfDaysBetweenActivitiesAction_triggered);
	connect(dataTimeConstraintsMaxTermsBetweenActivitiesAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsMaxTermsBetweenActivitiesAction_triggered);
	connect(dataTimeConstraintsTeacherMinDaysPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMinDaysPerWeekAction_triggered);
	connect(dataTimeConstraintsTeachersMinDaysPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMinDaysPerWeekAction_triggered);
	connect(dataTimeConstraintsTeacherActivityTagMaxHoursDailyAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherActivityTagMaxHoursDailyAction_triggered);
	connect(dataTimeConstraintsTeachersActivityTagMaxHoursDailyAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersActivityTagMaxHoursDailyAction_triggered);
	connect(dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyAction_triggered);
	connect(dataTimeConstraintsStudentsActivityTagMaxHoursDailyAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsActivityTagMaxHoursDailyAction_triggered);
	connect(dataTimeConstraintsTeacherActivityTagMinHoursDailyAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherActivityTagMinHoursDailyAction_triggered);
	connect(dataTimeConstraintsTeachersActivityTagMinHoursDailyAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersActivityTagMinHoursDailyAction_triggered);
	connect(dataTimeConstraintsStudentsSetActivityTagMinHoursDailyAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetActivityTagMinHoursDailyAction_triggered);
	connect(dataTimeConstraintsStudentsActivityTagMinHoursDailyAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsActivityTagMinHoursDailyAction_triggered);
	connect(dataTimeConstraintsStudentsSetMaxGapsPerDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMaxGapsPerDayAction_triggered);
	connect(dataTimeConstraintsStudentsMaxGapsPerDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMaxGapsPerDayAction_triggered);
	connect(dataSpaceConstraintsActivitiesOccupyMaxDifferentRoomsAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsActivitiesOccupyMaxDifferentRoomsAction_triggered);
	connect(dataSpaceConstraintsActivitiesSameRoomIfConsecutiveAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsActivitiesSameRoomIfConsecutiveAction_triggered);
	connect(dataTimeConstraintsStudentsSetMaxDaysPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMaxDaysPerWeekAction_triggered);
	connect(dataTimeConstraintsStudentsMaxDaysPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMaxDaysPerWeekAction_triggered);

	//mornings-afternoons
	connect(dataTimeConstraintsTeacherMaxRealDaysPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMaxRealDaysPerWeekAction_triggered);
	connect(dataTimeConstraintsTeacherMaxMorningsPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMaxMorningsPerWeekAction_triggered);
	connect(dataTimeConstraintsTeacherMaxTwoConsecutiveMorningsAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMaxTwoConsecutiveMorningsAction_triggered);
	connect(dataTimeConstraintsTeachersMaxTwoConsecutiveMorningsAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMaxTwoConsecutiveMorningsAction_triggered);
	connect(dataTimeConstraintsTeacherMaxTwoConsecutiveAfternoonsAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMaxTwoConsecutiveAfternoonsAction_triggered);
	connect(dataTimeConstraintsTeachersMaxTwoConsecutiveAfternoonsAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMaxTwoConsecutiveAfternoonsAction_triggered);
	connect(dataTimeConstraintsTeacherMaxAfternoonsPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMaxAfternoonsPerWeekAction_triggered);
	connect(dataTimeConstraintsTeachersMaxHoursDailyRealDaysAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMaxHoursDailyRealDaysAction_triggered);
	connect(dataTimeConstraintsTeachersAfternoonsEarlyMaxBeginningsAtSecondHourAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersAfternoonsEarlyMaxBeginningsAtSecondHourAction_triggered);
	connect(dataTimeConstraintsTeacherAfternoonsEarlyMaxBeginningsAtSecondHourAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherAfternoonsEarlyMaxBeginningsAtSecondHourAction_triggered);
	connect(dataTimeConstraintsStudentsAfternoonsEarlyMaxBeginningsAtSecondHourAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsAfternoonsEarlyMaxBeginningsAtSecondHourAction_triggered);
	connect(dataTimeConstraintsStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHourAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHourAction_triggered);

	connect(dataTimeConstraintsTeachersMorningsEarlyMaxBeginningsAtSecondHourAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMorningsEarlyMaxBeginningsAtSecondHourAction_triggered);
	connect(dataTimeConstraintsTeacherMorningsEarlyMaxBeginningsAtSecondHourAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMorningsEarlyMaxBeginningsAtSecondHourAction_triggered);
	connect(dataTimeConstraintsStudentsMorningsEarlyMaxBeginningsAtSecondHourAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMorningsEarlyMaxBeginningsAtSecondHourAction_triggered);
	connect(dataTimeConstraintsStudentsSetMorningsEarlyMaxBeginningsAtSecondHourAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMorningsEarlyMaxBeginningsAtSecondHourAction_triggered);

	connect(dataTimeConstraintsTeacherMaxHoursDailyRealDaysAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMaxHoursDailyRealDaysAction_triggered);
	connect(dataTimeConstraintsStudentsSetMaxHoursDailyRealDaysAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMaxHoursDailyRealDaysAction_triggered);
	connect(dataTimeConstraintsStudentsMaxHoursDailyRealDaysAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMaxHoursDailyRealDaysAction_triggered);
	connect(dataTimeConstraintsStudentsMinHoursPerMorningAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMinHoursPerMorningAction_triggered);
	connect(dataTimeConstraintsStudentsSetMinHoursPerMorningAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMinHoursPerMorningAction_triggered);
	connect(dataTimeConstraintsTeachersMinHoursPerMorningAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMinHoursPerMorningAction_triggered);

	//2022-09-10
	connect(dataTimeConstraintsStudentsMinHoursPerAfternoonAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMinHoursPerAfternoonAction_triggered);
	connect(dataTimeConstraintsStudentsSetMinHoursPerAfternoonAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMinHoursPerAfternoonAction_triggered);
	connect(dataTimeConstraintsTeachersMinHoursPerAfternoonAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMinHoursPerAfternoonAction_triggered);
	connect(dataTimeConstraintsTeacherMinHoursPerAfternoonAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMinHoursPerAfternoonAction_triggered);

	connect(dataTimeConstraintsTeachersMinHoursDailyRealDaysAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMinHoursDailyRealDaysAction_triggered);
	connect(dataTimeConstraintsTeacherMaxHoursPerAllAfternoonsAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMaxHoursPerAllAfternoonsAction_triggered);
	connect(dataTimeConstraintsTeachersMaxHoursPerAllAfternoonsAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMaxHoursPerAllAfternoonsAction_triggered);
	connect(dataTimeConstraintsStudentsSetMaxHoursPerAllAfternoonsAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMaxHoursPerAllAfternoonsAction_triggered);
	connect(dataTimeConstraintsStudentsMaxHoursPerAllAfternoonsAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMaxHoursPerAllAfternoonsAction_triggered);
	connect(dataTimeConstraintsTeacherMinHoursPerMorningAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMinHoursPerMorningAction_triggered);
	connect(dataTimeConstraintsTeacherMinHoursDailyRealDaysAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMinHoursDailyRealDaysAction_triggered);
	connect(dataTimeConstraintsTeachersMaxZeroGapsPerAfternoonAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMaxZeroGapsPerAfternoonAction_triggered);
	connect(dataTimeConstraintsTeacherMaxZeroGapsPerAfternoonAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMaxZeroGapsPerAfternoonAction_triggered);
	connect(dataTimeConstraintsTeacherMaxSpanPerRealDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMaxSpanPerRealDayAction_triggered);
	connect(dataTimeConstraintsTeachersMaxSpanPerRealDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMaxSpanPerRealDayAction_triggered);
	connect(dataTimeConstraintsStudentsSetMaxSpanPerRealDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMaxSpanPerRealDayAction_triggered);
	connect(dataTimeConstraintsStudentsMaxSpanPerRealDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMaxSpanPerRealDayAction_triggered);
	connect(dataTimeConstraintsTeacherMinRestingHoursBetweenMorningAndAfternoonAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMinRestingHoursBetweenMorningAndAfternoonAction_triggered);
	connect(dataTimeConstraintsTeachersMinRestingHoursBetweenMorningAndAfternoonAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMinRestingHoursBetweenMorningAndAfternoonAction_triggered);
	connect(dataTimeConstraintsStudentsSetMinRestingHoursBetweenMorningAndAfternoonAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMinRestingHoursBetweenMorningAndAfternoonAction_triggered);
	connect(dataTimeConstraintsStudentsMinRestingHoursBetweenMorningAndAfternoonAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMinRestingHoursBetweenMorningAndAfternoonAction_triggered);
	connect(dataTimeConstraintsTeacherMorningIntervalMaxDaysPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMorningIntervalMaxDaysPerWeekAction_triggered);
	connect(dataTimeConstraintsTeachersMorningIntervalMaxDaysPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMorningIntervalMaxDaysPerWeekAction_triggered);
	connect(dataTimeConstraintsTeacherAfternoonIntervalMaxDaysPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherAfternoonIntervalMaxDaysPerWeekAction_triggered);
	connect(dataTimeConstraintsTeachersAfternoonIntervalMaxDaysPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersAfternoonIntervalMaxDaysPerWeekAction_triggered);
	connect(dataTimeConstraintsStudentsSetMorningIntervalMaxDaysPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMorningIntervalMaxDaysPerWeekAction_triggered);
	connect(dataTimeConstraintsStudentsMorningIntervalMaxDaysPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMorningIntervalMaxDaysPerWeekAction_triggered);
	connect(dataTimeConstraintsStudentsSetAfternoonIntervalMaxDaysPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetAfternoonIntervalMaxDaysPerWeekAction_triggered);
	connect(dataTimeConstraintsStudentsAfternoonIntervalMaxDaysPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsAfternoonIntervalMaxDaysPerWeekAction_triggered);
	connect(dataTimeConstraintsTeachersMaxRealDaysPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMaxRealDaysPerWeekAction_triggered);
	connect(dataTimeConstraintsTeachersMaxMorningsPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMaxMorningsPerWeekAction_triggered);
	connect(dataTimeConstraintsTeachersMaxAfternoonsPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMaxAfternoonsPerWeekAction_triggered);
	connect(dataTimeConstraintsTeacherMinRealDaysPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMinRealDaysPerWeekAction_triggered);
	connect(dataTimeConstraintsTeachersMinRealDaysPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMinRealDaysPerWeekAction_triggered);
	connect(dataTimeConstraintsTeacherMinMorningsPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMinMorningsPerWeekAction_triggered);
	connect(dataTimeConstraintsTeachersMinMorningsPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMinMorningsPerWeekAction_triggered);
	connect(dataTimeConstraintsTeacherMinAfternoonsPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMinAfternoonsPerWeekAction_triggered);
	connect(dataTimeConstraintsTeachersMinAfternoonsPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMinAfternoonsPerWeekAction_triggered);
	connect(dataTimeConstraintsTeacherActivityTagMaxHoursDailyRealDaysAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherActivityTagMaxHoursDailyRealDaysAction_triggered);
	connect(dataTimeConstraintsTeacherMaxTwoActivityTagsPerDayFromN1N2N3Action, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMaxTwoActivityTagsPerDayFromN1N2N3Action_triggered);
	connect(dataTimeConstraintsTeachersMaxTwoActivityTagsPerDayFromN1N2N3Action, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMaxTwoActivityTagsPerDayFromN1N2N3Action_triggered);
	connect(dataTimeConstraintsStudentsSetMaxTwoActivityTagsPerDayFromN1N2N3Action, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMaxTwoActivityTagsPerDayFromN1N2N3Action_triggered);
	connect(dataTimeConstraintsStudentsMaxTwoActivityTagsPerDayFromN1N2N3Action, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMaxTwoActivityTagsPerDayFromN1N2N3Action_triggered);
	connect(dataTimeConstraintsTeacherMaxTwoActivityTagsPerRealDayFromN1N2N3Action, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMaxTwoActivityTagsPerRealDayFromN1N2N3Action_triggered);
	connect(dataTimeConstraintsTeachersMaxTwoActivityTagsPerRealDayFromN1N2N3Action, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMaxTwoActivityTagsPerRealDayFromN1N2N3Action_triggered);
	connect(dataTimeConstraintsStudentsSetMaxTwoActivityTagsPerRealDayFromN1N2N3Action, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMaxTwoActivityTagsPerRealDayFromN1N2N3Action_triggered);
	connect(dataTimeConstraintsStudentsMaxTwoActivityTagsPerRealDayFromN1N2N3Action, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMaxTwoActivityTagsPerRealDayFromN1N2N3Action_triggered);
	connect(dataTimeConstraintsTeachersActivityTagMaxHoursDailyRealDaysAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersActivityTagMaxHoursDailyRealDaysAction_triggered);
	connect(dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyRealDaysAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyRealDaysAction_triggered);
	connect(dataTimeConstraintsStudentsActivityTagMaxHoursDailyRealDaysAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsActivityTagMaxHoursDailyRealDaysAction_triggered);
	connect(dataTimeConstraintsStudentsSetMaxGapsPerRealDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMaxGapsPerRealDayAction_triggered);
	connect(dataTimeConstraintsStudentsMaxGapsPerRealDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMaxGapsPerRealDayAction_triggered);
	connect(dataTimeConstraintsStudentsSetMaxRealDaysPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMaxRealDaysPerWeekAction_triggered);
	connect(dataTimeConstraintsStudentsMaxRealDaysPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMaxRealDaysPerWeekAction_triggered);
	connect(dataTimeConstraintsStudentsSetMaxMorningsPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMaxMorningsPerWeekAction_triggered);
	connect(dataTimeConstraintsStudentsMaxMorningsPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMaxMorningsPerWeekAction_triggered);
	connect(dataTimeConstraintsStudentsSetMaxAfternoonsPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMaxAfternoonsPerWeekAction_triggered);
	connect(dataTimeConstraintsStudentsMaxAfternoonsPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMaxAfternoonsPerWeekAction_triggered);
	connect(dataTimeConstraintsStudentsSetMinMorningsPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMinMorningsPerWeekAction_triggered);
	connect(dataTimeConstraintsStudentsMinMorningsPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMinMorningsPerWeekAction_triggered);
	connect(dataTimeConstraintsStudentsSetMinAfternoonsPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMinAfternoonsPerWeekAction_triggered);
	connect(dataTimeConstraintsStudentsMinAfternoonsPerWeekAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMinAfternoonsPerWeekAction_triggered);
	connect(dataTimeConstraintsTeacherMaxGapsPerRealDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMaxGapsPerRealDayAction_triggered);
	connect(dataTimeConstraintsTeachersMaxGapsPerRealDayAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMaxGapsPerRealDayAction_triggered);
	connect(dataTimeConstraintsStudentsSetMaxGapsPerWeekForRealDaysAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMaxGapsPerWeekForRealDaysAction_triggered);
	connect(dataTimeConstraintsStudentsMaxGapsPerWeekForRealDaysAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMaxGapsPerWeekForRealDaysAction_triggered);
	connect(dataTimeConstraintsTeacherMaxGapsPerWeekForRealDaysAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMaxGapsPerWeekForRealDaysAction_triggered);
	connect(dataTimeConstraintsTeachersMaxGapsPerWeekForRealDaysAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMaxGapsPerWeekForRealDaysAction_triggered);
	connect(dataSpaceConstraintsStudentsSetMaxRoomChangesPerRealDayAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsStudentsSetMaxRoomChangesPerRealDayAction_triggered);
	connect(dataSpaceConstraintsStudentsMaxRoomChangesPerRealDayAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsStudentsMaxRoomChangesPerRealDayAction_triggered);
	connect(dataSpaceConstraintsTeacherMaxRoomChangesPerRealDayAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsTeacherMaxRoomChangesPerRealDayAction_triggered);
	connect(dataSpaceConstraintsTeachersMaxRoomChangesPerRealDayAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsTeachersMaxRoomChangesPerRealDayAction_triggered);

	connect(dataSpaceConstraintsStudentsSetMaxBuildingChangesPerRealDayAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsStudentsSetMaxBuildingChangesPerRealDayAction_triggered);
	connect(dataSpaceConstraintsStudentsMaxBuildingChangesPerRealDayAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsStudentsMaxBuildingChangesPerRealDayAction_triggered);
	connect(dataSpaceConstraintsTeacherMaxBuildingChangesPerRealDayAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsTeacherMaxBuildingChangesPerRealDayAction_triggered);
	connect(dataSpaceConstraintsTeachersMaxBuildingChangesPerRealDayAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsTeachersMaxBuildingChangesPerRealDayAction_triggered);

	connect(dataTimeConstraintsTeacherMaxThreeConsecutiveDaysAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeacherMaxThreeConsecutiveDaysAction_triggered);
	connect(dataTimeConstraintsTeachersMaxThreeConsecutiveDaysAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsTeachersMaxThreeConsecutiveDaysAction_triggered);

	connect(dataTimeConstraintsStudentsSetMaxThreeConsecutiveDaysAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsSetMaxThreeConsecutiveDaysAction_triggered);
	connect(dataTimeConstraintsStudentsMaxThreeConsecutiveDaysAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsStudentsMaxThreeConsecutiveDaysAction_triggered);

	//block-planning
	connect(dataTimeConstraintsMaxGapsBetweenActivitiesAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsMaxGapsBetweenActivitiesAction_triggered);
	connect(dataTimeConstraintsMaxTotalActivitiesFromSetInSelectedTimeSlotsAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsMaxTotalActivitiesFromSetInSelectedTimeSlotsAction_triggered);

	//terms
	connect(dataTimeConstraintsActivitiesMaxInATermAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsActivitiesMaxInATermAction_triggered);
	connect(dataTimeConstraintsActivitiesMinInATermAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsActivitiesMinInATermAction_triggered);
	connect(dataTimeConstraintsActivitiesOccupyMaxTermsAction, &QAction::triggered, this, &FetMainForm::dataTimeConstraintsActivitiesOccupyMaxTermsAction_triggered);

	//2024-02-09
	connect(dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayInIntervalAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayInIntervalAction_triggered);
	connect(dataSpaceConstraintsStudentsMaxBuildingChangesPerDayInIntervalAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsStudentsMaxBuildingChangesPerDayInIntervalAction_triggered);
	connect(dataSpaceConstraintsTeacherMaxBuildingChangesPerDayInIntervalAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsTeacherMaxBuildingChangesPerDayInIntervalAction_triggered);
	connect(dataSpaceConstraintsTeachersMaxBuildingChangesPerDayInIntervalAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsTeachersMaxBuildingChangesPerDayInIntervalAction_triggered);
	connect(dataSpaceConstraintsStudentsSetMaxBuildingChangesPerRealDayInIntervalAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsStudentsSetMaxBuildingChangesPerRealDayInIntervalAction_triggered);
	connect(dataSpaceConstraintsStudentsMaxBuildingChangesPerRealDayInIntervalAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsStudentsMaxBuildingChangesPerRealDayInIntervalAction_triggered);
	connect(dataSpaceConstraintsTeacherMaxBuildingChangesPerRealDayInIntervalAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsTeacherMaxBuildingChangesPerRealDayInIntervalAction_triggered);
	connect(dataSpaceConstraintsTeachersMaxBuildingChangesPerRealDayInIntervalAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsTeachersMaxBuildingChangesPerRealDayInIntervalAction_triggered);

	//2024-02-19
	connect(dataSpaceConstraintsStudentsSetMaxRoomChangesPerDayInIntervalAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsStudentsSetMaxRoomChangesPerDayInIntervalAction_triggered);
	connect(dataSpaceConstraintsStudentsMaxRoomChangesPerDayInIntervalAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsStudentsMaxRoomChangesPerDayInIntervalAction_triggered);
	connect(dataSpaceConstraintsTeacherMaxRoomChangesPerDayInIntervalAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsTeacherMaxRoomChangesPerDayInIntervalAction_triggered);
	connect(dataSpaceConstraintsTeachersMaxRoomChangesPerDayInIntervalAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsTeachersMaxRoomChangesPerDayInIntervalAction_triggered);
	connect(dataSpaceConstraintsStudentsSetMaxRoomChangesPerRealDayInIntervalAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsStudentsSetMaxRoomChangesPerRealDayInIntervalAction_triggered);
	connect(dataSpaceConstraintsStudentsMaxRoomChangesPerRealDayInIntervalAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsStudentsMaxRoomChangesPerRealDayInIntervalAction_triggered);
	connect(dataSpaceConstraintsTeacherMaxRoomChangesPerRealDayInIntervalAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsTeacherMaxRoomChangesPerRealDayInIntervalAction_triggered);
	connect(dataSpaceConstraintsTeachersMaxRoomChangesPerRealDayInIntervalAction, &QAction::triggered, this, &FetMainForm::dataSpaceConstraintsTeachersMaxRoomChangesPerRealDayInIntervalAction_triggered);

	retranslateConstraints();
}

void FetMainForm::retranslateConstraints()
{
	if(menuA_teacher_1_time_constraints!=nullptr)
		menuA_teacher_1_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "A teacher (1)", "The menu of a teacher time constraints, part 1"));
	if(menuA_teacher_2_time_constraints!=nullptr)
		menuA_teacher_2_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "A teacher (2)", "The menu of a teacher time constraints, part 2"));
	if(menuA_teacher_3_time_constraints!=nullptr)
		menuA_teacher_3_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "A teacher (3)", "The menu of a teacher time constraints, part 3"));
	if(menuA_teacher_4_time_constraints!=nullptr)
		menuA_teacher_4_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "A teacher (4)", "The menu of a teacher time constraints, part 4"));
	if(menuAll_teachers_1_time_constraints!=nullptr)
		menuAll_teachers_1_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "All teachers (1)", "The menu of all teachers time constraints, part 1"));
	if(menuAll_teachers_2_time_constraints!=nullptr)
		menuAll_teachers_2_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "All teachers (2)", "The menu of all teachers time constraints, part 2"));
	if(menuAll_teachers_3_time_constraints!=nullptr)
		menuAll_teachers_3_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "All teachers (3)", "The menu of all teachers time constraints, part 3"));
	if(menuAll_teachers_4_time_constraints!=nullptr)
		menuAll_teachers_4_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "All teachers (4)", "The menu of all teachers time constraints, part 4"));
	//
	if(menuA_students_set_1_time_constraints!=nullptr)
		menuA_students_set_1_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "A students set (1)", "The menu of a students set time constraints, part 1"));
	if(menuA_students_set_2_time_constraints!=nullptr)
		menuA_students_set_2_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "A students set (2)", "The menu of a students set time constraints, part 2"));
	if(menuA_students_set_3_time_constraints!=nullptr)
		menuA_students_set_3_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "A students set (3)", "The menu of a students set time constraints, part 3"));
	if(menuA_students_set_4_time_constraints!=nullptr)
		menuA_students_set_4_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "A students set (4)", "The menu of a students set time constraints, part 4"));
	if(menuAll_students_1_time_constraints!=nullptr)
		menuAll_students_1_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "All students (1)", "The menu of all students time constraints, part 1"));
	if(menuAll_students_2_time_constraints!=nullptr)
		menuAll_students_2_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "All students (2)", "The menu of all students time constraints, part 2"));
	if(menuAll_students_3_time_constraints!=nullptr)
		menuAll_students_3_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "All students (3)", "The menu of all students time constraints, part 3"));
	if(menuAll_students_4_time_constraints!=nullptr)
		menuAll_students_4_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "All students (4)", "The menu of all students time constraints, part 4"));

	if(menuA_teacher_time_constraints!=nullptr)
		menuA_teacher_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "A teacher", nullptr));
	if(menuAll_teachers_time_constraints!=nullptr)
		menuAll_teachers_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "All teachers", nullptr));
	//
	if(menuA_students_set_time_constraints!=nullptr)
		menuA_students_set_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "A students set", nullptr));
	if(menuAll_students_time_constraints!=nullptr)
		menuAll_students_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "All students", nullptr));

	if(menuActivities_preferred_times_time_constraints!=nullptr)
		menuActivities_preferred_times_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "Preferred times", "The menu of the list of preferred time constraints for activities"));
	if(menuActivities_others_1_time_constraints!=nullptr)
		menuActivities_others_1_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "Others (1)", "The menu of the list of other time constraints for activities (1st part)"));
	if(menuActivities_others_2_time_constraints!=nullptr)
		menuActivities_others_2_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "Others (2)", "The menu of the list of other time constraints for activities (2nd part)"));
	if(menuActivities_others_3_time_constraints!=nullptr)
		menuActivities_others_3_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "Others (3)", "The menu of the list of other time constraints for activities (3rd part)"));

	dataTimeConstraintsActivitiesPreferredTimeSlotsAction->setText(QCoreApplication::translate("FetMainForm_template", "A set of activities has a set of preferred time slots", nullptr));
	dataTimeConstraintsActivitiesSameStartingTimeAction->setText(QCoreApplication::translate("FetMainForm_template", "A set of activities has same starting time (day+hour)", nullptr));
	dataTimeConstraintsActivitiesOccupyMaxTimeSlotsFromSelectionAction->setText(QCoreApplication::translate("FetMainForm_template", "A set of activities occupies max time slots from selection", nullptr));
	dataTimeConstraintsActivitiesOccupyMinTimeSlotsFromSelectionAction->setText(QCoreApplication::translate("FetMainForm_template", "A set of activities occupies min time slots from selection", nullptr));
	dataTimeConstraintsActivitiesMaxSimultaneousInSelectedTimeSlotsAction->setText(QCoreApplication::translate("FetMainForm_template", "Max simultaneous activities from a set in selected time slots", nullptr));
	dataTimeConstraintsActivitiesMinSimultaneousInSelectedTimeSlotsAction->setText(QCoreApplication::translate("FetMainForm_template", "Min simultaneous activities from a set in selected time slots", nullptr));
	dataTimeConstraintsTeacherNotAvailableTimesAction->setText(QCoreApplication::translate("FetMainForm_template", "A teacher's not available times", nullptr));
	dataTimeConstraintsTeachersNotAvailableTimesAction->setText(QCoreApplication::translate("FetMainForm_template", "All teachers' not available times", nullptr));
	dataTimeConstraintsBasicCompulsoryTimeAction->setText(QCoreApplication::translate("FetMainForm_template", "Basic compulsory time constraints", nullptr));
	dataTimeConstraintsStudentsSetNotAvailableTimesAction->setText(QCoreApplication::translate("FetMainForm_template", "A students set's not available times", nullptr));
	dataTimeConstraintsStudentsNotAvailableTimesAction->setText(QCoreApplication::translate("FetMainForm_template", "All students' not available times", nullptr));
	dataTimeConstraintsBreakTimesAction->setText(QCoreApplication::translate("FetMainForm_template", "Break times", nullptr));
	dataTimeConstraintsTeacherMaxDaysPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Max days per week for a teacher", nullptr));
	dataTimeConstraintsTeachersMaxHoursDailyAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours daily for all teachers", nullptr));

	dataTimeConstraintsTeachersMaxHoursDailyInIntervalAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours daily in an hourly interval for all teachers", nullptr));
	dataTimeConstraintsTeacherMaxHoursDailyInIntervalAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours daily in an hourly interval for a teacher", nullptr));
	dataTimeConstraintsStudentsMaxHoursDailyInIntervalAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours daily in an hourly interval for all students", nullptr));
	dataTimeConstraintsStudentsSetMaxHoursDailyInIntervalAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours daily in an hourly interval for a students set", nullptr));

	dataTimeConstraintsActivityPreferredStartingTimeAction->setText(QCoreApplication::translate("FetMainForm_template", "An activity has a preferred starting time", nullptr));
	dataTimeConstraintsStudentsSetMaxGapsPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Max gaps per week for a students set", nullptr));
	dataTimeConstraintsStudentsMaxGapsPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Max gaps per week for all students", nullptr));
	dataTimeConstraintsStudentsEarlyMaxBeginningsAtSecondHourAction->setText(QCoreApplication::translate("FetMainForm_template", "All students begin early (max beginnings at second hour)", nullptr));
	dataTimeConstraintsActivitiesNotOverlappingAction->setText(QCoreApplication::translate("FetMainForm_template", "A set of activities are not overlapping", nullptr));
	dataTimeConstraintsActivityTagsNotOverlappingAction->setText(QCoreApplication::translate("FetMainForm_template", "A set of activity tags are not overlapping", nullptr));
	dataTimeConstraintsMinDaysBetweenActivitiesAction->setText(QCoreApplication::translate("FetMainForm_template", "Min days between a set of activities", nullptr));
	dataTimeConstraintsMinHalfDaysBetweenActivitiesAction->setText(QCoreApplication::translate("FetMainForm_template", "Min half days between a set of activities", nullptr));
	dataSpaceConstraintsBasicCompulsorySpaceAction->setText(QCoreApplication::translate("FetMainForm_template", "Basic compulsory space constraints", nullptr));
	dataSpaceConstraintsRoomNotAvailableTimesAction->setText(QCoreApplication::translate("FetMainForm_template", "A room's not available times", nullptr));
	dataSpaceConstraintsTeacherRoomNotAvailableTimesAction->setText(QCoreApplication::translate("FetMainForm_template", "A teacher+a room's not available times", nullptr));
	dataSpaceConstraintsActivityPreferredRoomAction->setText(QCoreApplication::translate("FetMainForm_template", "An activity has a preferred room", nullptr));
	dataTimeConstraintsActivitiesSameStartingHourAction->setText(QCoreApplication::translate("FetMainForm_template", "A set of activities has same starting hour (any days)", nullptr));
	dataSpaceConstraintsActivityPreferredRoomsAction->setText(QCoreApplication::translate("FetMainForm_template", "An activity has a set of preferred rooms", nullptr));
	dataSpaceConstraintsStudentsSetHomeRoomAction->setText(QCoreApplication::translate("FetMainForm_template", "A set of students has a home room", nullptr));
	dataSpaceConstraintsStudentsSetHomeRoomsAction->setText(QCoreApplication::translate("FetMainForm_template", "A set of students has a set of home rooms", nullptr));
	dataTimeConstraintsTeachersMaxGapsPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Max gaps per week for all teachers", nullptr));
	dataTimeConstraintsTeacherMaxGapsPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Max gaps per week for a teacher", nullptr));
	dataTimeConstraintsStudentsSetEarlyMaxBeginningsAtSecondHourAction->setText(QCoreApplication::translate("FetMainForm_template", "A students set begins early (max beginnings at second hour)", nullptr));
	dataTimeConstraintsTeacherMaxHoursDailyAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours daily for a teacher", nullptr));
	dataTimeConstraintsStudentsSetMaxHoursDailyAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours daily for a students set", nullptr));
	dataTimeConstraintsStudentsMaxHoursDailyAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours daily for all students", nullptr));
	dataTimeConstraintsStudentsMinHoursDailyAction->setText(QCoreApplication::translate("FetMainForm_template", "Min hours daily for all students", nullptr));
	dataTimeConstraintsStudentsSetMinHoursDailyAction->setText(QCoreApplication::translate("FetMainForm_template", "Min hours daily for a students set", nullptr));
	dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between an ordered pair of activity tags for a students set", nullptr));
	dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between an ordered pair of activity tags for all students", nullptr));
	dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between an ordered pair of activity tags for a teacher", nullptr));
	dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between an ordered pair of activity tags for all teachers", nullptr));
	//2021-12-15
	dataTimeConstraintsStudentsSetMinGapsBetweenActivityTagAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between an activity tag for a students set", nullptr));
	dataTimeConstraintsStudentsMinGapsBetweenActivityTagAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between an activity tag for all students", nullptr));
	dataTimeConstraintsTeacherMinGapsBetweenActivityTagAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between an activity tag for a teacher", nullptr));
	dataTimeConstraintsTeachersMinGapsBetweenActivityTagAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between an activity tag for all teachers", nullptr));
	//
	//2024-03-16
	dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between an ordered pair of activity tags per real day for a students set", nullptr));
	dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between an ordered pair of activity tags per real day for all students", nullptr));
	dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between an ordered pair of activity tags per real day for a teacher", nullptr));
	dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between an ordered pair of activity tags per real day for all teachers", nullptr));
	//
	dataTimeConstraintsStudentsSetMinGapsBetweenActivityTagPerRealDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between an activity tag per real day for a students set", nullptr));
	dataTimeConstraintsStudentsMinGapsBetweenActivityTagPerRealDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between an activity tag per real day for all students", nullptr));
	dataTimeConstraintsTeacherMinGapsBetweenActivityTagPerRealDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between an activity tag per real day for a teacher", nullptr));
	dataTimeConstraintsTeachersMinGapsBetweenActivityTagPerRealDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between an activity tag per real day for all teachers", nullptr));
	//2024-05-20
	dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between an ordered pair of activity tags between morning and afternoon for a students set", nullptr));
	dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between an ordered pair of activity tags between morning and afternoon for all students", nullptr));
	dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between an ordered pair of activity tags between morning and afternoon for a teacher", nullptr));
	dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between an ordered pair of activity tags between morning and afternoon for all teachers", nullptr));
	//
	dataTimeConstraintsStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between an activity tag between morning and afternoon for a students set", nullptr));
	dataTimeConstraintsStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between an activity tag between morning and afternoon for all students", nullptr));
	dataTimeConstraintsTeacherMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between an activity tag between morning and afternoon for a teacher", nullptr));
	dataTimeConstraintsTeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between an activity tag between morning and afternoon for all teachers", nullptr));
	//
	dataTimeConstraintsTwoActivitiesConsecutiveAction->setText(QCoreApplication::translate("FetMainForm_template", "Two activities are consecutive", nullptr));
	dataTimeConstraintsActivityEndsStudentsDayAction->setText(QCoreApplication::translate("FetMainForm_template", "An activity ends students day", nullptr));
	dataTimeConstraintsActivityEndsTeachersDayAction->setText(QCoreApplication::translate("FetMainForm_template", "An activity ends teachers day", nullptr));

	dataTimeConstraintsActivityBeginsStudentsDayAction->setText(QCoreApplication::translate("FetMainForm_template", "An activity begins students day", nullptr));
	dataTimeConstraintsActivityBeginsTeachersDayAction->setText(QCoreApplication::translate("FetMainForm_template", "An activity begins teachers day", nullptr));

	dataTimeConstraintsTeachersMinHoursDailyAction->setText(QCoreApplication::translate("FetMainForm_template", "Min hours daily for all teachers", nullptr));
	dataTimeConstraintsTeacherMinHoursDailyAction->setText(QCoreApplication::translate("FetMainForm_template", "Min hours daily for a teacher", nullptr));
	dataTimeConstraintsTeachersMaxGapsPerDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max gaps per day for all teachers", nullptr));
	dataTimeConstraintsTeacherMaxGapsPerDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max gaps per day for a teacher", nullptr));
	dataTimeConstraintsTeachersMaxGapsPerMorningAndAfternoonAction->setText(QCoreApplication::translate("FetMainForm_template", "Max gaps per morning+afternoon for all teachers", nullptr));
	dataTimeConstraintsTeacherMaxGapsPerMorningAndAfternoonAction->setText(QCoreApplication::translate("FetMainForm_template", "Max gaps per morning+afternoon for a teacher", nullptr));
	dataTimeConstraintsTeacherMaxSpanPerDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max span per day for a teacher", nullptr));
	dataTimeConstraintsTeachersMaxSpanPerDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max span per day for all teachers", nullptr));
	dataTimeConstraintsStudentsSetMaxSpanPerDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max span per day for a students set", nullptr));
	dataTimeConstraintsStudentsMaxSpanPerDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max span per day for all students", nullptr));
	dataTimeConstraintsTeacherMinRestingHoursAction->setText(QCoreApplication::translate("FetMainForm_template", "Min resting hours for a teacher", nullptr));
	dataTimeConstraintsTeachersMinRestingHoursAction->setText(QCoreApplication::translate("FetMainForm_template", "Min resting hours for all teachers", nullptr));
	dataTimeConstraintsStudentsSetMinRestingHoursAction->setText(QCoreApplication::translate("FetMainForm_template", "Min resting hours for a students set", nullptr));
	dataTimeConstraintsStudentsMinRestingHoursAction->setText(QCoreApplication::translate("FetMainForm_template", "Min resting hours for all students", nullptr));
	dataSpaceConstraintsSubjectPreferredRoomAction->setText(QCoreApplication::translate("FetMainForm_template", "A subject has a preferred room", nullptr));
	dataSpaceConstraintsSubjectPreferredRoomsAction->setText(QCoreApplication::translate("FetMainForm_template", "A subject has a set of preferred rooms", nullptr));
	dataSpaceConstraintsSubjectActivityTagPreferredRoomAction->setText(QCoreApplication::translate("FetMainForm_template", "A subject+an activity tag have a preferred room", nullptr));
	dataSpaceConstraintsSubjectActivityTagPreferredRoomsAction->setText(QCoreApplication::translate("FetMainForm_template", "A subject+an activity tag have a set of preferred rooms", nullptr));
	dataSpaceConstraintsTeacherHomeRoomAction->setText(QCoreApplication::translate("FetMainForm_template", "A teacher has a home room", nullptr));
	dataSpaceConstraintsTeacherHomeRoomsAction->setText(QCoreApplication::translate("FetMainForm_template", "A teacher has a set of home rooms", nullptr));
	dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max building changes per day for a students set", nullptr));
	dataSpaceConstraintsStudentsMaxBuildingChangesPerDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max building changes per day for all students", nullptr));
	dataSpaceConstraintsStudentsSetMaxBuildingChangesPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Max building changes per week for a students set", nullptr));
	dataSpaceConstraintsStudentsMaxBuildingChangesPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Max building changes per week for all students", nullptr));
	dataSpaceConstraintsStudentsSetMinGapsBetweenBuildingChangesAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between building changes for a students set", nullptr));
	dataSpaceConstraintsStudentsMinGapsBetweenBuildingChangesAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between building changes for all students", nullptr));
	dataSpaceConstraintsTeacherMaxBuildingChangesPerDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max building changes per day for a teacher", nullptr));
	dataSpaceConstraintsTeachersMaxBuildingChangesPerDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max building changes per day for all teachers", nullptr));
	dataSpaceConstraintsTeacherMaxBuildingChangesPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Max building changes per week for a teacher", nullptr));
	dataSpaceConstraintsTeachersMaxBuildingChangesPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Max building changes per week for all teachers", nullptr));
	dataSpaceConstraintsTeacherMinGapsBetweenBuildingChangesAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between building changes for a teacher", nullptr));
	dataSpaceConstraintsTeachersMinGapsBetweenBuildingChangesAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between building changes for all teachers", nullptr));
	dataSpaceConstraintsStudentsSetMaxRoomChangesPerDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max room changes per day for a students set", nullptr));
	dataSpaceConstraintsStudentsMaxRoomChangesPerDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max room changes per day for all students", nullptr));
	dataSpaceConstraintsStudentsSetMaxRoomChangesPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Max room changes per week for a students set", nullptr));
	dataSpaceConstraintsStudentsMaxRoomChangesPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Max room changes per week for all students", nullptr));
	dataSpaceConstraintsStudentsSetMinGapsBetweenRoomChangesAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between room changes for a students set", nullptr));
	dataSpaceConstraintsStudentsMinGapsBetweenRoomChangesAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between room changes for all students", nullptr));
	dataSpaceConstraintsTeacherMaxRoomChangesPerDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max room changes per day for a teacher", nullptr));
	dataSpaceConstraintsTeachersMaxRoomChangesPerDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max room changes per day for all teachers", nullptr));
	dataSpaceConstraintsTeacherMaxRoomChangesPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Max room changes per week for a teacher", nullptr));
	dataSpaceConstraintsTeachersMaxRoomChangesPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Max room changes per week for all teachers", nullptr));
	dataSpaceConstraintsTeacherMinGapsBetweenRoomChangesAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between room changes for a teacher", nullptr));
	dataSpaceConstraintsTeachersMinGapsBetweenRoomChangesAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps between room changes for all teachers", nullptr));
	dataTimeConstraintsActivitiesSameStartingDayAction->setText(QCoreApplication::translate("FetMainForm_template", "A set of activities has same starting day (any hours)", nullptr));
	dataTimeConstraintsTwoActivitiesOrderedAction->setText(QCoreApplication::translate("FetMainForm_template", "Two activities are ordered", nullptr));
	dataTimeConstraintsTwoSetsOfActivitiesOrderedAction->setText(QCoreApplication::translate("FetMainForm_template", "Two sets of activities are ordered", nullptr));
	dataTimeConstraintsTwoActivitiesOrderedIfSameDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Two activities are ordered if they are on the same day", nullptr));
	dataTimeConstraintsTeachersMaxHoursContinuouslyAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours continuously for all teachers", nullptr));
	dataTimeConstraintsTeacherMaxHoursContinuouslyAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours continuously for a teacher", nullptr));
	dataTimeConstraintsStudentsSetMaxHoursContinuouslyAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours continuously for a students set", nullptr));
	dataTimeConstraintsStudentsMaxHoursContinuouslyAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours continuously for all students", nullptr));
	dataTimeConstraintsActivitiesPreferredStartingTimesAction->setText(QCoreApplication::translate("FetMainForm_template", "A set of activities has a set of preferred starting times", nullptr));
	dataTimeConstraintsActivityPreferredTimeSlotsAction->setText(QCoreApplication::translate("FetMainForm_template", "An activity has a set of preferred time slots", nullptr));
	dataTimeConstraintsActivityPreferredStartingTimesAction->setText(QCoreApplication::translate("FetMainForm_template", "An activity has a set of preferred starting times", nullptr));
	dataTimeConstraintsMinGapsBetweenActivitiesAction->setText(QCoreApplication::translate("FetMainForm_template", "Min gaps (hours) between a set of activities", nullptr));
	dataTimeConstraintsSubactivitiesPreferredTimeSlotsAction->setText(QCoreApplication::translate("FetMainForm_template", "A set of subactivities has a set of preferred time slots", nullptr));
	dataTimeConstraintsSubactivitiesPreferredStartingTimesAction->setText(QCoreApplication::translate("FetMainForm_template", "A set of subactivities has a set of preferred starting times", nullptr));
	dataTimeConstraintsTeacherIntervalMaxDaysPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "A teacher works in an hourly interval max days per week", nullptr));
	dataTimeConstraintsTeachersIntervalMaxDaysPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "All teachers work in an hourly interval max days per week", nullptr));
	dataTimeConstraintsStudentsSetIntervalMaxDaysPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "A students set works in an hourly interval max days per week", nullptr));
	dataTimeConstraintsStudentsIntervalMaxDaysPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "All students work in an hourly interval max days per week", nullptr));
	dataTimeConstraintsActivitiesEndStudentsDayAction->setText(QCoreApplication::translate("FetMainForm_template", "A set of activities end students day", nullptr));
	dataTimeConstraintsActivitiesEndTeachersDayAction->setText(QCoreApplication::translate("FetMainForm_template", "A set of activities end teachers day", nullptr));

	dataTimeConstraintsActivitiesBeginStudentsDayAction->setText(QCoreApplication::translate("FetMainForm_template", "A set of activities begin students day", nullptr));
	dataTimeConstraintsActivitiesBeginTeachersDayAction->setText(QCoreApplication::translate("FetMainForm_template", "A set of activities begin teachers day", nullptr));

	dataTimeConstraintsTwoActivitiesGroupedAction->setText(QCoreApplication::translate("FetMainForm_template", "Two activities are grouped", nullptr));
	dataTimeConstraintsStudentsSetActivityTagMaxHoursContinuouslyAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours continuously with an activity tag for a students set", nullptr));
	dataTimeConstraintsStudentsActivityTagMaxHoursContinuouslyAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours continuously with an activity tag for all students", nullptr));
	dataTimeConstraintsTeacherActivityTagMaxHoursContinuouslyAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours continuously with an activity tag for a teacher", nullptr));
	dataTimeConstraintsTeachersActivityTagMaxHoursContinuouslyAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours continuously with an activity tag for all teachers", nullptr));
	dataSpaceConstraintsActivityTagPreferredRoomAction->setText(QCoreApplication::translate("FetMainForm_template", "An activity tag has a preferred room", nullptr));
	dataSpaceConstraintsActivityTagPreferredRoomsAction->setText(QCoreApplication::translate("FetMainForm_template", "An activity tag has a set of preferred rooms", nullptr));
	dataTimeConstraintsTeachersMaxDaysPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Max days per week for all teachers", nullptr));
	dataTimeConstraintsThreeActivitiesGroupedAction->setText(QCoreApplication::translate("FetMainForm_template", "Three activities are grouped", nullptr));
	dataTimeConstraintsMaxDaysBetweenActivitiesAction->setText(QCoreApplication::translate("FetMainForm_template", "Max days between a set of activities", nullptr));
	dataTimeConstraintsActivitiesMaxHourlySpanAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hourly span of a set of activities", nullptr));
	dataTimeConstraintsMaxHalfDaysBetweenActivitiesAction->setText(QCoreApplication::translate("FetMainForm_template", "Max half days between a set of activities", nullptr));
	dataTimeConstraintsMaxTermsBetweenActivitiesAction->setText(QCoreApplication::translate("FetMainForm_template", "Max terms between a set of activities", nullptr));
	dataTimeConstraintsTeacherMinDaysPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Min days per week for a teacher", nullptr));
	dataTimeConstraintsTeachersMinDaysPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Min days per week for all teachers", nullptr));
	dataTimeConstraintsTeacherActivityTagMaxHoursDailyAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours daily with an activity tag for a teacher", nullptr));
	dataTimeConstraintsTeachersActivityTagMaxHoursDailyAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours daily with an activity tag for all teachers", nullptr));
	dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours daily with an activity tag for a students set", nullptr));
	dataTimeConstraintsStudentsActivityTagMaxHoursDailyAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours daily with an activity tag for all students", nullptr));
	dataTimeConstraintsTeacherActivityTagMinHoursDailyAction->setText(QCoreApplication::translate("FetMainForm_template", "Min hours daily with an activity tag for a teacher", nullptr));
	dataTimeConstraintsTeachersActivityTagMinHoursDailyAction->setText(QCoreApplication::translate("FetMainForm_template", "Min hours daily with an activity tag for all teachers", nullptr));
	dataTimeConstraintsStudentsSetActivityTagMinHoursDailyAction->setText(QCoreApplication::translate("FetMainForm_template", "Min hours daily with an activity tag for a students set", nullptr));
	dataTimeConstraintsStudentsActivityTagMinHoursDailyAction->setText(QCoreApplication::translate("FetMainForm_template", "Min hours daily with an activity tag for all students", nullptr));
	dataTimeConstraintsStudentsSetMaxGapsPerDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max gaps per day for a students set", nullptr));
	dataTimeConstraintsStudentsMaxGapsPerDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max gaps per day for all students", nullptr));
	dataSpaceConstraintsActivitiesOccupyMaxDifferentRoomsAction->setText(QCoreApplication::translate("FetMainForm_template", "A set of activities occupies max different rooms", nullptr));
	dataSpaceConstraintsActivitiesSameRoomIfConsecutiveAction->setText(QCoreApplication::translate("FetMainForm_template", "A set of activities are in the same room if they are consecutive", nullptr));
	dataTimeConstraintsStudentsSetMaxDaysPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Max days per week for a students set", nullptr));
	dataTimeConstraintsStudentsMaxDaysPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Max days per week for all students", nullptr));

	//mornings-afternoons
	dataTimeConstraintsTeacherMaxRealDaysPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Max real days per week for a teacher", nullptr));
	dataTimeConstraintsTeacherMaxMorningsPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Max mornings per week for a teacher", nullptr));
	dataTimeConstraintsTeacherMaxTwoConsecutiveMorningsAction->setText(QCoreApplication::translate("FetMainForm_template", "A teacher works max two consecutive mornings", nullptr));
	dataTimeConstraintsTeachersMaxTwoConsecutiveMorningsAction->setText(QCoreApplication::translate("FetMainForm_template", "All teachers work max two consecutive mornings", nullptr));
	dataTimeConstraintsTeacherMaxTwoConsecutiveAfternoonsAction->setText(QCoreApplication::translate("FetMainForm_template", "A teacher works max two consecutive afternoons", nullptr));
	dataTimeConstraintsTeachersMaxTwoConsecutiveAfternoonsAction->setText(QCoreApplication::translate("FetMainForm_template", "All teachers work max two consecutive afternoons", nullptr));
	dataTimeConstraintsTeacherMaxAfternoonsPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Max afternoons per week for a teacher", nullptr));
	dataTimeConstraintsTeachersMaxHoursDailyRealDaysAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours daily per real day for all teachers", nullptr));
	dataTimeConstraintsTeachersAfternoonsEarlyMaxBeginningsAtSecondHourAction->setText(QCoreApplication::translate("FetMainForm_template", "All teachers begin afternoons early (max beginnings at second hour)", nullptr));
	dataTimeConstraintsTeacherAfternoonsEarlyMaxBeginningsAtSecondHourAction->setText(QCoreApplication::translate("FetMainForm_template", "A teacher begins afternoons early (max beginnings at second hour)", nullptr));
	dataTimeConstraintsStudentsAfternoonsEarlyMaxBeginningsAtSecondHourAction->setText(QCoreApplication::translate("FetMainForm_template", "All students begin afternoons early (max beginnings at second hour)", nullptr));
	dataTimeConstraintsStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHourAction->setText(QCoreApplication::translate("FetMainForm_template", "A students set begins afternoons early (max beginnings at second hour)", nullptr));

	dataTimeConstraintsTeachersMorningsEarlyMaxBeginningsAtSecondHourAction->setText(QCoreApplication::translate("FetMainForm_template", "All teachers begin mornings early (max beginnings at second hour)", nullptr));
	dataTimeConstraintsTeacherMorningsEarlyMaxBeginningsAtSecondHourAction->setText(QCoreApplication::translate("FetMainForm_template", "A teacher begins mornings early (max beginnings at second hour)", nullptr));
	dataTimeConstraintsStudentsMorningsEarlyMaxBeginningsAtSecondHourAction->setText(QCoreApplication::translate("FetMainForm_template", "All students begin mornings early (max beginnings at second hour)", nullptr));
	dataTimeConstraintsStudentsSetMorningsEarlyMaxBeginningsAtSecondHourAction->setText(QCoreApplication::translate("FetMainForm_template", "A students set begins mornings early (max beginnings at second hour)", nullptr));

	dataTimeConstraintsTeacherMaxHoursDailyRealDaysAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours daily per real day for a teacher", nullptr));
	dataTimeConstraintsStudentsSetMaxHoursDailyRealDaysAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours daily per real day for a students set", nullptr));
	dataTimeConstraintsStudentsMaxHoursDailyRealDaysAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours daily per real day for all students", nullptr));
	dataTimeConstraintsStudentsMinHoursPerMorningAction->setText(QCoreApplication::translate("FetMainForm_template", "Min hours per morning for all students", nullptr));
	dataTimeConstraintsStudentsSetMinHoursPerMorningAction->setText(QCoreApplication::translate("FetMainForm_template", "Min hours per morning for a students set", nullptr));
	dataTimeConstraintsTeachersMinHoursPerMorningAction->setText(QCoreApplication::translate("FetMainForm_template", "Min hours per morning for all teachers", nullptr));

	//2022-09-10
	dataTimeConstraintsStudentsMinHoursPerAfternoonAction->setText(QCoreApplication::translate("FetMainForm_template", "Min hours per afternoon for all students", nullptr));
	dataTimeConstraintsStudentsSetMinHoursPerAfternoonAction->setText(QCoreApplication::translate("FetMainForm_template", "Min hours per afternoon for a students set", nullptr));
	dataTimeConstraintsTeachersMinHoursPerAfternoonAction->setText(QCoreApplication::translate("FetMainForm_template", "Min hours per afternoon for all teachers", nullptr));
	dataTimeConstraintsTeacherMinHoursPerAfternoonAction->setText(QCoreApplication::translate("FetMainForm_template", "Min hours per afternoon for a teacher", nullptr));

	dataTimeConstraintsTeachersMinHoursDailyRealDaysAction->setText(QCoreApplication::translate("FetMainForm_template", "Min hours daily per real day for all teachers", nullptr));
	dataTimeConstraintsTeacherMaxHoursPerAllAfternoonsAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours per all afternoons for a teacher", nullptr));
	dataTimeConstraintsTeachersMaxHoursPerAllAfternoonsAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours per all afternoons for all teachers", nullptr));
	dataTimeConstraintsStudentsSetMaxHoursPerAllAfternoonsAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours per all afternoons for a students set", nullptr));
	dataTimeConstraintsStudentsMaxHoursPerAllAfternoonsAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours per all afternoons for all students", nullptr));
	dataTimeConstraintsTeacherMinHoursPerMorningAction->setText(QCoreApplication::translate("FetMainForm_template", "Min hours per morning for a teacher", nullptr));
	dataTimeConstraintsTeacherMinHoursDailyRealDaysAction->setText(QCoreApplication::translate("FetMainForm_template", "Min hours daily per real day for a teacher", nullptr));
	dataTimeConstraintsTeachersMaxZeroGapsPerAfternoonAction->setText(QCoreApplication::translate("FetMainForm_template", "Max zero gaps per afternoon for all teachers", nullptr));
	dataTimeConstraintsTeacherMaxZeroGapsPerAfternoonAction->setText(QCoreApplication::translate("FetMainForm_template", "Max zero gaps per afternoon for a teacher", nullptr));
	dataTimeConstraintsTeacherMaxSpanPerRealDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max span per real day for a teacher", nullptr));
	dataTimeConstraintsTeachersMaxSpanPerRealDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max span per real day for all teachers", nullptr));
	dataTimeConstraintsStudentsSetMaxSpanPerRealDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max span per real day for a students set", nullptr));
	dataTimeConstraintsStudentsMaxSpanPerRealDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max span per real day for all students", nullptr));
	dataTimeConstraintsTeacherMinRestingHoursBetweenMorningAndAfternoonAction->setText(QCoreApplication::translate("FetMainForm_template", "Min resting hours between morning and afternoon for a teacher", nullptr));
	dataTimeConstraintsTeachersMinRestingHoursBetweenMorningAndAfternoonAction->setText(QCoreApplication::translate("FetMainForm_template", "Min resting hours between morning and afternoon for all teachers", nullptr));
	dataTimeConstraintsStudentsSetMinRestingHoursBetweenMorningAndAfternoonAction->setText(QCoreApplication::translate("FetMainForm_template", "Min resting hours between morning and afternoon for a students set", nullptr));
	dataTimeConstraintsStudentsMinRestingHoursBetweenMorningAndAfternoonAction->setText(QCoreApplication::translate("FetMainForm_template", "Min resting hours between morning and afternoon for all students", nullptr));
	dataTimeConstraintsTeacherMorningIntervalMaxDaysPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "A teacher works in an hourly morning interval max days per week", nullptr));
	dataTimeConstraintsTeachersMorningIntervalMaxDaysPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "All teachers work in an hourly morning interval max days per week", nullptr));
	dataTimeConstraintsTeacherAfternoonIntervalMaxDaysPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "A teacher works in an hourly afternoon interval max days per week", nullptr));
	dataTimeConstraintsTeachersAfternoonIntervalMaxDaysPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "All teachers work in an hourly afternoon interval max days per week", nullptr));
	dataTimeConstraintsStudentsSetMorningIntervalMaxDaysPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "A students set works in an hourly morning interval max days per week", nullptr));
	dataTimeConstraintsStudentsMorningIntervalMaxDaysPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "All students work in an hourly morning interval max days per week", nullptr));
	dataTimeConstraintsStudentsSetAfternoonIntervalMaxDaysPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "A students set works in an hourly afternoon interval max days per week", nullptr));
	dataTimeConstraintsStudentsAfternoonIntervalMaxDaysPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "All students work in an hourly afternoon interval max days per week", nullptr));
	dataTimeConstraintsTeachersMaxRealDaysPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Max real days per week for all teachers", nullptr));
	dataTimeConstraintsTeachersMaxMorningsPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Max mornings per week for all teachers", nullptr));
	dataTimeConstraintsTeachersMaxAfternoonsPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Max afternoons per week for all teachers", nullptr));
	dataTimeConstraintsTeacherMinRealDaysPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Min real days per week for a teacher", nullptr));
	dataTimeConstraintsTeachersMinRealDaysPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Min real days per week for all teachers", nullptr));
	dataTimeConstraintsTeacherMinMorningsPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Min mornings per week for a teacher", nullptr));
	dataTimeConstraintsTeachersMinMorningsPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Min mornings per week for all teachers", nullptr));
	dataTimeConstraintsTeacherMinAfternoonsPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Min afternoons per week for a teacher", nullptr));
	dataTimeConstraintsTeachersMinAfternoonsPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Min afternoons per week for all teachers", nullptr));
	dataTimeConstraintsTeacherActivityTagMaxHoursDailyRealDaysAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours daily per real day with an activity tag for a teacher", nullptr));
	dataTimeConstraintsTeacherMaxTwoActivityTagsPerDayFromN1N2N3Action->setText(QCoreApplication::translate("FetMainForm_template", "Max two activity tags from N1, N2, N3 per day for a teacher", nullptr));
	dataTimeConstraintsTeachersMaxTwoActivityTagsPerDayFromN1N2N3Action->setText(QCoreApplication::translate("FetMainForm_template", "Max two activity tags from N1, N2, N3 per day for all teachers", nullptr));
	dataTimeConstraintsStudentsSetMaxTwoActivityTagsPerDayFromN1N2N3Action->setText(QCoreApplication::translate("FetMainForm_template", "Max two activity tags from N1, N2, N3 per day for a students set", nullptr));
	dataTimeConstraintsStudentsMaxTwoActivityTagsPerDayFromN1N2N3Action->setText(QCoreApplication::translate("FetMainForm_template", "Max two activity tags from N1, N2, N3 per day for all students", nullptr));
	dataTimeConstraintsTeacherMaxTwoActivityTagsPerRealDayFromN1N2N3Action->setText(QCoreApplication::translate("FetMainForm_template", "Max two activity tags from N1, N2, N3 per real day for a teacher", nullptr));
	dataTimeConstraintsTeachersMaxTwoActivityTagsPerRealDayFromN1N2N3Action->setText(QCoreApplication::translate("FetMainForm_template", "Max two activity tags from N1, N2, N3 per real day for all teachers", nullptr));
	dataTimeConstraintsStudentsSetMaxTwoActivityTagsPerRealDayFromN1N2N3Action->setText(QCoreApplication::translate("FetMainForm_template", "Max two activity tags from N1, N2, N3 per real day for a students set", nullptr));
	dataTimeConstraintsStudentsMaxTwoActivityTagsPerRealDayFromN1N2N3Action->setText(QCoreApplication::translate("FetMainForm_template", "Max two activity tags from N1, N2, N3 per real day for all students", nullptr));
	dataTimeConstraintsTeachersActivityTagMaxHoursDailyRealDaysAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours daily per real day with an activity tag for all teachers", nullptr));
	dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyRealDaysAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours daily per real day with an activity tag for a students set", nullptr));
	dataTimeConstraintsStudentsActivityTagMaxHoursDailyRealDaysAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours daily per real day with an activity tag for all students", nullptr));
	dataTimeConstraintsStudentsSetMaxGapsPerRealDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max gaps per real day for a students set", nullptr));
	dataTimeConstraintsStudentsMaxGapsPerRealDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max gaps per real day for all students", nullptr));
	dataTimeConstraintsStudentsSetMaxRealDaysPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Max real days per week for a students set", nullptr));
	dataTimeConstraintsStudentsMaxRealDaysPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Max real days per week for all students", nullptr));
	dataTimeConstraintsStudentsSetMaxMorningsPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Max mornings per week for a students set", nullptr));
	dataTimeConstraintsStudentsMaxMorningsPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Max mornings per week for all students", nullptr));
	dataTimeConstraintsStudentsSetMaxAfternoonsPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Max afternoons per week for a students set", nullptr));
	dataTimeConstraintsStudentsMaxAfternoonsPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Max afternoons per week for all students", nullptr));
	dataTimeConstraintsStudentsSetMinMorningsPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Min mornings per week for a students set", nullptr));
	dataTimeConstraintsStudentsMinMorningsPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Min mornings per week for all students", nullptr));
	dataTimeConstraintsStudentsSetMinAfternoonsPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Min afternoons per week for a students set", nullptr));
	dataTimeConstraintsStudentsMinAfternoonsPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Min afternoons per week for all students", nullptr));
	dataTimeConstraintsTeacherMaxGapsPerRealDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max gaps per real day for a teacher", nullptr));
	dataTimeConstraintsTeachersMaxGapsPerRealDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max gaps per real day for all teachers", nullptr));
	dataTimeConstraintsStudentsSetMaxGapsPerWeekForRealDaysAction->setText(QCoreApplication::translate("FetMainForm_template", "Max gaps per week for real days for a students set", nullptr));
	dataTimeConstraintsStudentsMaxGapsPerWeekForRealDaysAction->setText(QCoreApplication::translate("FetMainForm_template", "Max gaps per week for real days for all students", nullptr));
	dataTimeConstraintsTeacherMaxGapsPerWeekForRealDaysAction->setText(QCoreApplication::translate("FetMainForm_template", "Max gaps per week for real days for a teacher", nullptr));
	dataTimeConstraintsTeachersMaxGapsPerWeekForRealDaysAction->setText(QCoreApplication::translate("FetMainForm_template", "Max gaps per week for real days for all teachers", nullptr));
	dataSpaceConstraintsStudentsSetMaxRoomChangesPerRealDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max room changes per real day for a students set", nullptr));
	dataSpaceConstraintsStudentsMaxRoomChangesPerRealDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max room changes per real day for all students", nullptr));
	dataSpaceConstraintsTeacherMaxRoomChangesPerRealDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max room changes per real day for a teacher", nullptr));
	dataSpaceConstraintsTeachersMaxRoomChangesPerRealDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max room changes per real day for all teachers", nullptr));

	dataSpaceConstraintsStudentsSetMaxBuildingChangesPerRealDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max building changes per real day for a students set", nullptr));
	dataSpaceConstraintsStudentsMaxBuildingChangesPerRealDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max building changes per real day for all students", nullptr));
	dataSpaceConstraintsTeacherMaxBuildingChangesPerRealDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max building changes per real day for a teacher", nullptr));
	dataSpaceConstraintsTeachersMaxBuildingChangesPerRealDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max building changes per real day for all teachers", nullptr));

	dataTimeConstraintsTeacherMaxThreeConsecutiveDaysAction->setText(QCoreApplication::translate("FetMainForm_template", "A teacher works max three consecutive days", nullptr));
	dataTimeConstraintsTeachersMaxThreeConsecutiveDaysAction->setText(QCoreApplication::translate("FetMainForm_template", "All teachers work max three consecutive days", nullptr));

	dataTimeConstraintsStudentsSetMaxThreeConsecutiveDaysAction->setText(QCoreApplication::translate("FetMainForm_template", "A students set works max three consecutive days", nullptr));
	dataTimeConstraintsStudentsMaxThreeConsecutiveDaysAction->setText(QCoreApplication::translate("FetMainForm_template", "All students work max three consecutive days", nullptr));

	//block-planning
	dataTimeConstraintsMaxGapsBetweenActivitiesAction->setText(QCoreApplication::translate("FetMainForm_template", "Max gaps (hours) between a set of activities", nullptr));
	dataTimeConstraintsMaxTotalActivitiesFromSetInSelectedTimeSlotsAction->setText(QCoreApplication::translate("FetMainForm_template", "Max total activities from a set in selected time slots", nullptr));
	
	//terms
	dataTimeConstraintsActivitiesMaxInATermAction->setText(QCoreApplication::translate("FetMainForm_template", "Max activities from a set in a term", nullptr));
	dataTimeConstraintsActivitiesMinInATermAction->setText(QCoreApplication::translate("FetMainForm_template", "Min activities from a set in a term", nullptr));
	dataTimeConstraintsActivitiesOccupyMaxTermsAction->setText(QCoreApplication::translate("FetMainForm_template", "A set of activities occupies max terms", nullptr));

	//2024-02-09
	dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayInIntervalAction->setText(QCoreApplication::translate("FetMainForm_template", "Max building changes per day in an hourly interval for a students set", nullptr));
	dataSpaceConstraintsStudentsMaxBuildingChangesPerDayInIntervalAction->setText(QCoreApplication::translate("FetMainForm_template", "Max building changes per day in an hourly interval for all students", nullptr));
	dataSpaceConstraintsTeacherMaxBuildingChangesPerDayInIntervalAction->setText(QCoreApplication::translate("FetMainForm_template", "Max building changes per day in an hourly interval for a teacher", nullptr));
	dataSpaceConstraintsTeachersMaxBuildingChangesPerDayInIntervalAction->setText(QCoreApplication::translate("FetMainForm_template", "Max building changes per day in an hourly interval for all teachers", nullptr));
	dataSpaceConstraintsStudentsSetMaxBuildingChangesPerRealDayInIntervalAction->setText(QCoreApplication::translate("FetMainForm_template", "Max building changes per real day in an hourly interval for a students set", nullptr));
	dataSpaceConstraintsStudentsMaxBuildingChangesPerRealDayInIntervalAction->setText(QCoreApplication::translate("FetMainForm_template", "Max building changes per real day in an hourly interval for all students", nullptr));
	dataSpaceConstraintsTeacherMaxBuildingChangesPerRealDayInIntervalAction->setText(QCoreApplication::translate("FetMainForm_template", "Max building changes per real day in an hourly interval for a teacher", nullptr));
	dataSpaceConstraintsTeachersMaxBuildingChangesPerRealDayInIntervalAction->setText(QCoreApplication::translate("FetMainForm_template", "Max building changes per real day in an hourly interval for all teachers", nullptr));

	//2024-02-19
	dataSpaceConstraintsStudentsSetMaxRoomChangesPerDayInIntervalAction->setText(QCoreApplication::translate("FetMainForm_template", "Max room changes per day in an hourly interval for a students set", nullptr));
	dataSpaceConstraintsStudentsMaxRoomChangesPerDayInIntervalAction->setText(QCoreApplication::translate("FetMainForm_template", "Max room changes per day in an hourly interval for all students", nullptr));
	dataSpaceConstraintsTeacherMaxRoomChangesPerDayInIntervalAction->setText(QCoreApplication::translate("FetMainForm_template", "Max room changes per day in an hourly interval for a teacher", nullptr));
	dataSpaceConstraintsTeachersMaxRoomChangesPerDayInIntervalAction->setText(QCoreApplication::translate("FetMainForm_template", "Max room changes per day in an hourly interval for all teachers", nullptr));
	dataSpaceConstraintsStudentsSetMaxRoomChangesPerRealDayInIntervalAction->setText(QCoreApplication::translate("FetMainForm_template", "Max room changes per real day in an hourly interval for a students set", nullptr));
	dataSpaceConstraintsStudentsMaxRoomChangesPerRealDayInIntervalAction->setText(QCoreApplication::translate("FetMainForm_template", "Max room changes per real day in an hourly interval for all students", nullptr));
	dataSpaceConstraintsTeacherMaxRoomChangesPerRealDayInIntervalAction->setText(QCoreApplication::translate("FetMainForm_template", "Max room changes per real day in an hourly interval for a teacher", nullptr));
	dataSpaceConstraintsTeachersMaxRoomChangesPerRealDayInIntervalAction->setText(QCoreApplication::translate("FetMainForm_template", "Max room changes per real day in an hourly interval for all teachers", nullptr));
}

void FetMainForm::createMenusOfActionsForConstraints()
{
	menuMisc_time_constraints->clear();
	menuTeachers_time_constraints->clear();
	menuStudents_time_constraints->clear();
	menuActivities_time_constraints->clear();

	menuA_teacher_time_constraints=nullptr;
	menuAll_teachers_time_constraints=nullptr;
	menuA_students_set_time_constraints=nullptr;
	menuAll_students_time_constraints=nullptr;

	menuA_teacher_1_time_constraints=nullptr;
	menuA_teacher_2_time_constraints=nullptr;
	menuA_teacher_3_time_constraints=nullptr;
	menuA_teacher_4_time_constraints=nullptr;
	menuAll_teachers_time_constraints=nullptr;
	menuAll_teachers_1_time_constraints=nullptr;
	menuAll_teachers_2_time_constraints=nullptr;
	menuAll_teachers_3_time_constraints=nullptr;
	menuAll_teachers_4_time_constraints=nullptr;
	menuA_students_set_1_time_constraints=nullptr;
	menuA_students_set_2_time_constraints=nullptr;
	menuA_students_set_3_time_constraints=nullptr;
	menuA_students_set_4_time_constraints=nullptr;
	menuAll_students_1_time_constraints=nullptr;
	menuAll_students_2_time_constraints=nullptr;
	menuAll_students_3_time_constraints=nullptr;
	menuAll_students_4_time_constraints=nullptr;

	menuActivities_preferred_times_time_constraints=nullptr;
	menuActivities_others_1_time_constraints=nullptr;
	menuActivities_others_2_time_constraints=nullptr;
	menuActivities_others_3_time_constraints=nullptr;

	menuMisc_space_constraints->clear();
	menuRooms_space_constraints->clear();
	menuActivities_space_constraints->clear();
	menuSubjects_space_constraints->clear();
	menuSubjects_and_activity_tags_space_constraints->clear();
	menuA_students_set_space_constraints->clear();
	menuAll_students_space_constraints->clear();
	menuA_teacher_space_constraints->clear();
	menuAll_teachers_space_constraints->clear();
	menuActivity_tags_space_constraints->clear();
	
	if(gt.rules.mode==OFFICIAL){
		menuMisc_time_constraints->addAction(dataTimeConstraintsBasicCompulsoryTimeAction);
		menuMisc_time_constraints->addAction(dataTimeConstraintsBreakTimesAction);

		menuA_teacher_time_constraints=new QMenu(menuTeachers_time_constraints);
		menuAll_teachers_time_constraints=new QMenu(menuTeachers_time_constraints);
		//
		menuA_students_set_time_constraints=new QMenu(menuStudents_time_constraints);
		menuAll_students_time_constraints=new QMenu(menuStudents_time_constraints);
		//
		menuActivities_preferred_times_time_constraints=new QMenu(menuActivities_time_constraints);
		menuActivities_others_1_time_constraints=new QMenu(menuActivities_time_constraints);
		menuActivities_others_2_time_constraints=new QMenu(menuActivities_time_constraints);
		//
		menuTeachers_time_constraints->addMenu(menuA_teacher_time_constraints);
		menuTeachers_time_constraints->addMenu(menuAll_teachers_time_constraints);
		//
		menuStudents_time_constraints->addMenu(menuA_students_set_time_constraints);
		menuStudents_time_constraints->addMenu(menuAll_students_time_constraints);
		//
		menuActivities_time_constraints->addMenu(menuActivities_preferred_times_time_constraints);
		menuActivities_time_constraints->addMenu(menuActivities_others_1_time_constraints);
		menuActivities_time_constraints->addMenu(menuActivities_others_2_time_constraints);
		//
		menuA_teacher_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "A teacher", nullptr));
		menuAll_teachers_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "All teachers", nullptr));
		//
		menuA_students_set_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "A students set", nullptr));
		menuAll_students_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "All students", nullptr));
		//
		menuActivities_preferred_times_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "Preferred times", "The menu of the list of preferred time constraints for activities"));
		menuActivities_others_1_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "Others (1)", "The menu of the list of other time constraints for activities (1st part)"));
		menuActivities_others_2_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "Others (2)", "The menu of the list of other time constraints for activities (2nd part)"));

		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherNotAvailableTimesAction);
		menuA_teacher_time_constraints->addSeparator();
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxDaysPerWeekAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinDaysPerWeekAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxGapsPerDayAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxGapsPerWeekAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxHoursDailyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxHoursDailyInIntervalAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxSpanPerDayAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherActivityTagMaxHoursDailyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinHoursDailyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherActivityTagMinHoursDailyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxHoursContinuouslyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherActivityTagMaxHoursContinuouslyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinGapsBetweenActivityTagAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherIntervalMaxDaysPerWeekAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinRestingHoursAction);

		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersNotAvailableTimesAction);
		menuAll_teachers_time_constraints->addSeparator();
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxDaysPerWeekAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMinDaysPerWeekAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxGapsPerDayAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxGapsPerWeekAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxHoursDailyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxHoursDailyInIntervalAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxSpanPerDayAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersActivityTagMaxHoursDailyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMinHoursDailyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersActivityTagMinHoursDailyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxHoursContinuouslyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersActivityTagMaxHoursContinuouslyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMinGapsBetweenActivityTagAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersIntervalMaxDaysPerWeekAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMinRestingHoursAction);

		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetNotAvailableTimesAction);
		menuA_students_set_time_constraints->addSeparator();
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxDaysPerWeekAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxGapsPerDayAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxGapsPerWeekAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetEarlyMaxBeginningsAtSecondHourAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxHoursDailyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxHoursDailyInIntervalAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxSpanPerDayAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMinHoursDailyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetActivityTagMinHoursDailyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxHoursContinuouslyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetActivityTagMaxHoursContinuouslyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMinGapsBetweenActivityTagAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetIntervalMaxDaysPerWeekAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMinRestingHoursAction);

		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsNotAvailableTimesAction);
		menuAll_students_time_constraints->addSeparator();
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxDaysPerWeekAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxGapsPerDayAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxGapsPerWeekAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsEarlyMaxBeginningsAtSecondHourAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxHoursDailyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxHoursDailyInIntervalAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxSpanPerDayAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsActivityTagMaxHoursDailyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMinHoursDailyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsActivityTagMinHoursDailyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxHoursContinuouslyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsActivityTagMaxHoursContinuouslyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMinGapsBetweenActivityTagAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsIntervalMaxDaysPerWeekAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMinRestingHoursAction);

		menuActivities_preferred_times_time_constraints->addAction(dataTimeConstraintsActivityPreferredStartingTimeAction);
		menuActivities_preferred_times_time_constraints->addAction(dataTimeConstraintsActivityPreferredStartingTimesAction);
		menuActivities_preferred_times_time_constraints->addAction(dataTimeConstraintsActivityPreferredTimeSlotsAction);
		menuActivities_preferred_times_time_constraints->addSeparator();
		menuActivities_preferred_times_time_constraints->addAction(dataTimeConstraintsActivitiesPreferredStartingTimesAction);
		menuActivities_preferred_times_time_constraints->addAction(dataTimeConstraintsActivitiesPreferredTimeSlotsAction);
		menuActivities_preferred_times_time_constraints->addSeparator();
		menuActivities_preferred_times_time_constraints->addAction(dataTimeConstraintsSubactivitiesPreferredStartingTimesAction);
		menuActivities_preferred_times_time_constraints->addAction(dataTimeConstraintsSubactivitiesPreferredTimeSlotsAction);

		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsMinDaysBetweenActivitiesAction);
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsMaxDaysBetweenActivitiesAction);

		menuActivities_others_1_time_constraints->addSeparator();
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivityBeginsStudentsDayAction);
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivitiesBeginStudentsDayAction);
		menuActivities_others_1_time_constraints->addSeparator();
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivityBeginsTeachersDayAction);
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivitiesBeginTeachersDayAction);

		menuActivities_others_1_time_constraints->addSeparator();
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivityEndsStudentsDayAction);
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivitiesEndStudentsDayAction);
		menuActivities_others_1_time_constraints->addSeparator();
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivityEndsTeachersDayAction);
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivitiesEndTeachersDayAction);

		menuActivities_others_1_time_constraints->addSeparator();
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivitiesSameStartingTimeAction);
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivitiesSameStartingDayAction);
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivitiesSameStartingHourAction);

		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsActivitiesOccupyMaxTimeSlotsFromSelectionAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsActivitiesOccupyMinTimeSlotsFromSelectionAction);
		menuActivities_others_2_time_constraints->addSeparator();

		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsTwoActivitiesOrderedAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsTwoSetsOfActivitiesOrderedAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsTwoActivitiesOrderedIfSameDayAction);
		menuActivities_others_2_time_constraints->addSeparator();
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsTwoActivitiesConsecutiveAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsTwoActivitiesGroupedAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsThreeActivitiesGroupedAction);
		menuActivities_others_2_time_constraints->addSeparator();
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsActivitiesMaxHourlySpanAction);
		menuActivities_others_2_time_constraints->addSeparator();
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsActivitiesNotOverlappingAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsActivityTagsNotOverlappingAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsActivitiesMaxSimultaneousInSelectedTimeSlotsAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsActivitiesMinSimultaneousInSelectedTimeSlotsAction);
		menuActivities_others_2_time_constraints->addSeparator();
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsMinGapsBetweenActivitiesAction);

		menuMisc_space_constraints->addAction(dataSpaceConstraintsBasicCompulsorySpaceAction);

		menuRooms_space_constraints->addAction(dataSpaceConstraintsRoomNotAvailableTimesAction);
		menuRooms_space_constraints->addAction(dataSpaceConstraintsTeacherRoomNotAvailableTimesAction);

		menuActivities_space_constraints->addAction(dataSpaceConstraintsActivityPreferredRoomAction);
		menuActivities_space_constraints->addAction(dataSpaceConstraintsActivityPreferredRoomsAction);
		menuActivities_space_constraints->addSeparator();
		menuActivities_space_constraints->addAction(dataSpaceConstraintsActivitiesSameRoomIfConsecutiveAction);
		menuActivities_space_constraints->addAction(dataSpaceConstraintsActivitiesOccupyMaxDifferentRoomsAction);

		menuSubjects_space_constraints->addAction(dataSpaceConstraintsSubjectPreferredRoomAction);
		menuSubjects_space_constraints->addAction(dataSpaceConstraintsSubjectPreferredRoomsAction);

		menuSubjects_and_activity_tags_space_constraints->addAction(dataSpaceConstraintsSubjectActivityTagPreferredRoomAction);
		menuSubjects_and_activity_tags_space_constraints->addAction(dataSpaceConstraintsSubjectActivityTagPreferredRoomsAction);

		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetHomeRoomAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetHomeRoomsAction);
		menuA_students_set_space_constraints->addSeparator();
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxRoomChangesPerDayAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxRoomChangesPerDayInIntervalAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxRoomChangesPerWeekAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMinGapsBetweenRoomChangesAction);
		menuA_students_set_space_constraints->addSeparator();
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayInIntervalAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxBuildingChangesPerWeekAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMinGapsBetweenBuildingChangesAction);
		//menuStudents_space_constraints->addSeparator();
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMaxRoomChangesPerDayAction);
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMaxRoomChangesPerDayInIntervalAction);
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMaxRoomChangesPerWeekAction);
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMinGapsBetweenRoomChangesAction);
		menuAll_students_space_constraints->addSeparator();
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMaxBuildingChangesPerDayAction);
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMaxBuildingChangesPerDayInIntervalAction);
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMaxBuildingChangesPerWeekAction);
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMinGapsBetweenBuildingChangesAction);

		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherHomeRoomAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherHomeRoomsAction);
		menuA_teacher_space_constraints->addSeparator();
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMaxRoomChangesPerDayAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMaxRoomChangesPerDayInIntervalAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMaxRoomChangesPerWeekAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMinGapsBetweenRoomChangesAction);
		menuA_teacher_space_constraints->addSeparator();
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMaxBuildingChangesPerDayAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMaxBuildingChangesPerDayInIntervalAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMaxBuildingChangesPerWeekAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMinGapsBetweenBuildingChangesAction);
		//menuTeachers_space_constraints->addSeparator();
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxRoomChangesPerDayAction);
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxRoomChangesPerDayInIntervalAction);
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxRoomChangesPerWeekAction);
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMinGapsBetweenRoomChangesAction);
		menuAll_teachers_space_constraints->addSeparator();
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxBuildingChangesPerDayAction);
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxBuildingChangesPerDayInIntervalAction);
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxBuildingChangesPerWeekAction);
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMinGapsBetweenBuildingChangesAction);

		menuActivity_tags_space_constraints->addAction(dataSpaceConstraintsActivityTagPreferredRoomAction);
		menuActivity_tags_space_constraints->addAction(dataSpaceConstraintsActivityTagPreferredRoomsAction);
	}
	else if(gt.rules.mode==MORNINGS_AFTERNOONS){
		menuMisc_time_constraints->addAction(dataTimeConstraintsBasicCompulsoryTimeAction);
		menuMisc_time_constraints->addAction(dataTimeConstraintsBreakTimesAction);

		menuA_teacher_1_time_constraints=new QMenu(menuTeachers_time_constraints);
		menuA_teacher_2_time_constraints=new QMenu(menuTeachers_time_constraints);
		menuA_teacher_3_time_constraints=new QMenu(menuTeachers_time_constraints);
		menuA_teacher_4_time_constraints=new QMenu(menuTeachers_time_constraints);
		menuAll_teachers_1_time_constraints=new QMenu(menuTeachers_time_constraints);
		menuAll_teachers_2_time_constraints=new QMenu(menuTeachers_time_constraints);
		menuAll_teachers_3_time_constraints=new QMenu(menuTeachers_time_constraints);
		menuAll_teachers_4_time_constraints=new QMenu(menuTeachers_time_constraints);
		//
		menuA_students_set_1_time_constraints=new QMenu(menuStudents_time_constraints);
		menuA_students_set_2_time_constraints=new QMenu(menuStudents_time_constraints);
		menuA_students_set_3_time_constraints=new QMenu(menuStudents_time_constraints);
		menuA_students_set_4_time_constraints=new QMenu(menuStudents_time_constraints);
		menuAll_students_1_time_constraints=new QMenu(menuStudents_time_constraints);
		menuAll_students_2_time_constraints=new QMenu(menuStudents_time_constraints);
		menuAll_students_3_time_constraints=new QMenu(menuStudents_time_constraints);
		menuAll_students_4_time_constraints=new QMenu(menuStudents_time_constraints);
		//
		menuActivities_preferred_times_time_constraints=new QMenu(menuActivities_time_constraints);
		menuActivities_others_1_time_constraints=new QMenu(menuActivities_time_constraints);
		menuActivities_others_2_time_constraints=new QMenu(menuActivities_time_constraints);
		//
		menuTeachers_time_constraints->addMenu(menuA_teacher_1_time_constraints);
		menuTeachers_time_constraints->addMenu(menuA_teacher_2_time_constraints);
		menuTeachers_time_constraints->addMenu(menuA_teacher_3_time_constraints);
		menuTeachers_time_constraints->addMenu(menuA_teacher_4_time_constraints);
		menuTeachers_time_constraints->addMenu(menuAll_teachers_1_time_constraints);
		menuTeachers_time_constraints->addMenu(menuAll_teachers_2_time_constraints);
		menuTeachers_time_constraints->addMenu(menuAll_teachers_3_time_constraints);
		menuTeachers_time_constraints->addMenu(menuAll_teachers_4_time_constraints);
		//
		menuStudents_time_constraints->addMenu(menuA_students_set_1_time_constraints);
		menuStudents_time_constraints->addMenu(menuA_students_set_2_time_constraints);
		menuStudents_time_constraints->addMenu(menuA_students_set_3_time_constraints);
		menuStudents_time_constraints->addMenu(menuA_students_set_4_time_constraints);
		menuStudents_time_constraints->addMenu(menuAll_students_1_time_constraints);
		menuStudents_time_constraints->addMenu(menuAll_students_2_time_constraints);
		menuStudents_time_constraints->addMenu(menuAll_students_3_time_constraints);
		menuStudents_time_constraints->addMenu(menuAll_students_4_time_constraints);
		//
		menuActivities_time_constraints->addMenu(menuActivities_preferred_times_time_constraints);
		menuActivities_time_constraints->addMenu(menuActivities_others_1_time_constraints);
		menuActivities_time_constraints->addMenu(menuActivities_others_2_time_constraints);
		//
		menuA_teacher_1_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "A teacher (1)", "The menu of a teacher time constraints, part 1"));
		menuA_teacher_2_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "A teacher (2)", "The menu of a teacher time constraints, part 2"));
		menuA_teacher_3_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "A teacher (3)", "The menu of a teacher time constraints, part 3"));
		menuA_teacher_4_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "A teacher (4)", "The menu of a teacher time constraints, part 4"));
		menuAll_teachers_1_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "All teachers (1)", "The menu of all teachers time constraints, part 1"));
		menuAll_teachers_2_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "All teachers (2)", "The menu of all teachers time constraints, part 2"));
		menuAll_teachers_3_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "All teachers (3)", "The menu of all teachers time constraints, part 3"));
		menuAll_teachers_4_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "All teachers (4)", "The menu of all teachers time constraints, part 4"));
		//
		menuA_students_set_1_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "A students set (1)", "The menu of a students set time constraints, part 1"));
		menuA_students_set_2_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "A students set (2)", "The menu of a students set time constraints, part 2"));
		menuA_students_set_3_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "A students set (3)", "The menu of a students set time constraints, part 3"));
		menuA_students_set_4_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "A students set (4)", "The menu of a students set time constraints, part 4"));
		menuAll_students_1_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "All students (1)", "The menu of all students time constraints, part 1"));
		menuAll_students_2_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "All students (2)", "The menu of all students time constraints, part 2"));
		menuAll_students_3_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "All students (3)", "The menu of all students time constraints, part 3"));
		menuAll_students_4_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "All students (4)", "The menu of all students time constraints, part 4"));
		//
		menuActivities_preferred_times_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "Preferred times", "The menu of the list of preferred time constraints for activities"));
		menuActivities_others_1_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "Others (1)", "The menu of the list of other time constraints for activities (1st part)"));
		menuActivities_others_2_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "Others (2)", "The menu of the list of other time constraints for activities (2nd part)"));

		menuA_teacher_1_time_constraints->addAction(dataTimeConstraintsTeacherNotAvailableTimesAction);
		menuA_teacher_1_time_constraints->addSeparator();

		menuA_teacher_1_time_constraints->addAction(dataTimeConstraintsTeacherMaxDaysPerWeekAction);
		menuA_teacher_1_time_constraints->addAction(dataTimeConstraintsTeacherMaxRealDaysPerWeekAction);
		menuA_teacher_1_time_constraints->addAction(dataTimeConstraintsTeacherMaxMorningsPerWeekAction);
		menuA_teacher_1_time_constraints->addAction(dataTimeConstraintsTeacherMaxAfternoonsPerWeekAction);

		menuA_teacher_1_time_constraints->addAction(dataTimeConstraintsTeacherMaxThreeConsecutiveDaysAction);
		
		menuA_teacher_1_time_constraints->addAction(dataTimeConstraintsTeacherMaxTwoConsecutiveMorningsAction);
		menuA_teacher_1_time_constraints->addAction(dataTimeConstraintsTeacherMaxTwoConsecutiveAfternoonsAction);

		menuA_teacher_1_time_constraints->addAction(dataTimeConstraintsTeacherMinDaysPerWeekAction);
		menuA_teacher_1_time_constraints->addAction(dataTimeConstraintsTeacherMinRealDaysPerWeekAction);
		menuA_teacher_1_time_constraints->addAction(dataTimeConstraintsTeacherMinMorningsPerWeekAction);
		menuA_teacher_1_time_constraints->addAction(dataTimeConstraintsTeacherMinAfternoonsPerWeekAction);
		
		menuA_teacher_2_time_constraints->addAction(dataTimeConstraintsTeacherMaxGapsPerDayAction);
		menuA_teacher_2_time_constraints->addAction(dataTimeConstraintsTeacherMaxGapsPerMorningAndAfternoonAction);
		menuA_teacher_2_time_constraints->addAction(dataTimeConstraintsTeacherMaxGapsPerWeekAction);

		menuA_teacher_2_time_constraints->addAction(dataTimeConstraintsTeacherMaxGapsPerRealDayAction);
		menuA_teacher_2_time_constraints->addAction(dataTimeConstraintsTeacherMaxGapsPerWeekForRealDaysAction);
		
		menuA_teacher_2_time_constraints->addAction(dataTimeConstraintsTeacherMaxZeroGapsPerAfternoonAction);

		menuA_teacher_2_time_constraints->addAction(dataTimeConstraintsTeacherMorningsEarlyMaxBeginningsAtSecondHourAction);
		menuA_teacher_2_time_constraints->addAction(dataTimeConstraintsTeacherAfternoonsEarlyMaxBeginningsAtSecondHourAction);

		menuA_teacher_2_time_constraints->addAction(dataTimeConstraintsTeacherMaxHoursDailyAction);
		menuA_teacher_2_time_constraints->addAction(dataTimeConstraintsTeacherMaxHoursDailyInIntervalAction);
		menuA_teacher_2_time_constraints->addAction(dataTimeConstraintsTeacherMaxHoursDailyRealDaysAction);
		menuA_teacher_2_time_constraints->addAction(dataTimeConstraintsTeacherMaxSpanPerDayAction);
		menuA_teacher_2_time_constraints->addAction(dataTimeConstraintsTeacherMaxSpanPerRealDayAction);
		
		menuA_teacher_2_time_constraints->addAction(dataTimeConstraintsTeacherActivityTagMaxHoursDailyRealDaysAction);

		menuA_teacher_3_time_constraints->addAction(dataTimeConstraintsTeacherMaxTwoActivityTagsPerDayFromN1N2N3Action);
		menuA_teacher_3_time_constraints->addAction(dataTimeConstraintsTeacherMaxTwoActivityTagsPerRealDayFromN1N2N3Action);
		
		menuA_teacher_3_time_constraints->addAction(dataTimeConstraintsTeacherMinHoursDailyAction);
		menuA_teacher_3_time_constraints->addAction(dataTimeConstraintsTeacherMinHoursDailyRealDaysAction);
		menuA_teacher_3_time_constraints->addAction(dataTimeConstraintsTeacherMinHoursPerMorningAction);
		menuA_teacher_3_time_constraints->addAction(dataTimeConstraintsTeacherMinHoursPerAfternoonAction);

		menuA_teacher_3_time_constraints->addAction(dataTimeConstraintsTeacherActivityTagMinHoursDailyAction);
		
		menuA_teacher_3_time_constraints->addAction(dataTimeConstraintsTeacherMaxHoursContinuouslyAction);
		menuA_teacher_3_time_constraints->addAction(dataTimeConstraintsTeacherActivityTagMaxHoursContinuouslyAction);

		menuA_teacher_3_time_constraints->addAction(dataTimeConstraintsTeacherMinGapsBetweenActivityTagAction);
		menuA_teacher_3_time_constraints->addAction(dataTimeConstraintsTeacherMinGapsBetweenActivityTagPerRealDayAction);
		menuA_teacher_3_time_constraints->addAction(dataTimeConstraintsTeacherMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction);
		menuA_teacher_3_time_constraints->addAction(dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsAction);
		menuA_teacher_3_time_constraints->addAction(dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction);
		menuA_teacher_3_time_constraints->addAction(dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction);

		menuA_teacher_4_time_constraints->addAction(dataTimeConstraintsTeacherIntervalMaxDaysPerWeekAction);
		menuA_teacher_4_time_constraints->addAction(dataTimeConstraintsTeacherMorningIntervalMaxDaysPerWeekAction);
		menuA_teacher_4_time_constraints->addAction(dataTimeConstraintsTeacherAfternoonIntervalMaxDaysPerWeekAction);

		menuA_teacher_4_time_constraints->addAction(dataTimeConstraintsTeacherMaxHoursPerAllAfternoonsAction);

		menuA_teacher_4_time_constraints->addAction(dataTimeConstraintsTeacherMinRestingHoursBetweenMorningAndAfternoonAction);

		//all teachers
		menuAll_teachers_1_time_constraints->addAction(dataTimeConstraintsTeachersNotAvailableTimesAction);
		menuAll_teachers_1_time_constraints->addSeparator();
		
		menuAll_teachers_1_time_constraints->addAction(dataTimeConstraintsTeachersMaxDaysPerWeekAction);
		menuAll_teachers_1_time_constraints->addAction(dataTimeConstraintsTeachersMaxRealDaysPerWeekAction);
		menuAll_teachers_1_time_constraints->addAction(dataTimeConstraintsTeachersMaxMorningsPerWeekAction);
		menuAll_teachers_1_time_constraints->addAction(dataTimeConstraintsTeachersMaxAfternoonsPerWeekAction);

		menuAll_teachers_1_time_constraints->addAction(dataTimeConstraintsTeachersMaxThreeConsecutiveDaysAction);

		menuAll_teachers_1_time_constraints->addAction(dataTimeConstraintsTeachersMaxTwoConsecutiveMorningsAction);
		menuAll_teachers_1_time_constraints->addAction(dataTimeConstraintsTeachersMaxTwoConsecutiveAfternoonsAction);

		menuAll_teachers_1_time_constraints->addAction(dataTimeConstraintsTeachersMinDaysPerWeekAction);
		menuAll_teachers_1_time_constraints->addAction(dataTimeConstraintsTeachersMinRealDaysPerWeekAction);
		menuAll_teachers_1_time_constraints->addAction(dataTimeConstraintsTeachersMinMorningsPerWeekAction);
		menuAll_teachers_1_time_constraints->addAction(dataTimeConstraintsTeachersMinAfternoonsPerWeekAction);
		
		menuAll_teachers_2_time_constraints->addAction(dataTimeConstraintsTeachersMaxGapsPerDayAction);
		menuAll_teachers_2_time_constraints->addAction(dataTimeConstraintsTeachersMaxGapsPerMorningAndAfternoonAction);
		menuAll_teachers_2_time_constraints->addAction(dataTimeConstraintsTeachersMaxGapsPerWeekAction);

		menuAll_teachers_2_time_constraints->addAction(dataTimeConstraintsTeachersMaxGapsPerRealDayAction);
		menuAll_teachers_2_time_constraints->addAction(dataTimeConstraintsTeachersMaxGapsPerWeekForRealDaysAction);
		
		menuAll_teachers_2_time_constraints->addAction(dataTimeConstraintsTeachersMaxZeroGapsPerAfternoonAction);

		menuAll_teachers_2_time_constraints->addAction(dataTimeConstraintsTeachersMorningsEarlyMaxBeginningsAtSecondHourAction);
		menuAll_teachers_2_time_constraints->addAction(dataTimeConstraintsTeachersAfternoonsEarlyMaxBeginningsAtSecondHourAction);

		menuAll_teachers_2_time_constraints->addAction(dataTimeConstraintsTeachersMaxHoursDailyAction);
		menuAll_teachers_2_time_constraints->addAction(dataTimeConstraintsTeachersMaxHoursDailyInIntervalAction);
		menuAll_teachers_2_time_constraints->addAction(dataTimeConstraintsTeachersMaxHoursDailyRealDaysAction);
		menuAll_teachers_2_time_constraints->addAction(dataTimeConstraintsTeachersMaxSpanPerDayAction);
		menuAll_teachers_2_time_constraints->addAction(dataTimeConstraintsTeachersMaxSpanPerRealDayAction);
		
		menuAll_teachers_2_time_constraints->addAction(dataTimeConstraintsTeachersActivityTagMaxHoursDailyRealDaysAction);

		menuAll_teachers_3_time_constraints->addAction(dataTimeConstraintsTeachersMaxTwoActivityTagsPerDayFromN1N2N3Action);
		menuAll_teachers_3_time_constraints->addAction(dataTimeConstraintsTeachersMaxTwoActivityTagsPerRealDayFromN1N2N3Action);
		
		menuAll_teachers_3_time_constraints->addAction(dataTimeConstraintsTeachersMinHoursDailyAction);
		menuAll_teachers_3_time_constraints->addAction(dataTimeConstraintsTeachersMinHoursDailyRealDaysAction);
		menuAll_teachers_3_time_constraints->addAction(dataTimeConstraintsTeachersMinHoursPerMorningAction);
		menuAll_teachers_3_time_constraints->addAction(dataTimeConstraintsTeachersMinHoursPerAfternoonAction);

		menuAll_teachers_3_time_constraints->addAction(dataTimeConstraintsTeachersActivityTagMinHoursDailyAction);
		
		menuAll_teachers_3_time_constraints->addAction(dataTimeConstraintsTeachersMaxHoursContinuouslyAction);
		menuAll_teachers_3_time_constraints->addAction(dataTimeConstraintsTeachersActivityTagMaxHoursContinuouslyAction);

		menuAll_teachers_3_time_constraints->addAction(dataTimeConstraintsTeachersMinGapsBetweenActivityTagAction);
		menuAll_teachers_3_time_constraints->addAction(dataTimeConstraintsTeachersMinGapsBetweenActivityTagPerRealDayAction);
		menuAll_teachers_3_time_constraints->addAction(dataTimeConstraintsTeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction);
		menuAll_teachers_3_time_constraints->addAction(dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsAction);
		menuAll_teachers_3_time_constraints->addAction(dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction);
		menuAll_teachers_3_time_constraints->addAction(dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction);

		menuAll_teachers_4_time_constraints->addAction(dataTimeConstraintsTeachersIntervalMaxDaysPerWeekAction);
		menuAll_teachers_4_time_constraints->addAction(dataTimeConstraintsTeachersMorningIntervalMaxDaysPerWeekAction);
		menuAll_teachers_4_time_constraints->addAction(dataTimeConstraintsTeachersAfternoonIntervalMaxDaysPerWeekAction);

		menuAll_teachers_4_time_constraints->addAction(dataTimeConstraintsTeachersMaxHoursPerAllAfternoonsAction);

		menuAll_teachers_4_time_constraints->addAction(dataTimeConstraintsTeachersMinRestingHoursBetweenMorningAndAfternoonAction);
		/////////

		menuA_students_set_1_time_constraints->addAction(dataTimeConstraintsStudentsSetNotAvailableTimesAction);
		menuA_students_set_1_time_constraints->addSeparator();

		menuA_students_set_1_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxDaysPerWeekAction);
		menuA_students_set_1_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxRealDaysPerWeekAction);
		menuA_students_set_1_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxMorningsPerWeekAction);
		menuA_students_set_1_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxAfternoonsPerWeekAction);

		menuA_students_set_1_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxThreeConsecutiveDaysAction);

		menuA_students_set_1_time_constraints->addAction(dataTimeConstraintsStudentsSetMinMorningsPerWeekAction);
		menuA_students_set_1_time_constraints->addAction(dataTimeConstraintsStudentsSetMinAfternoonsPerWeekAction);

		menuA_students_set_2_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxGapsPerDayAction);
		menuA_students_set_2_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxGapsPerWeekAction);

		menuA_students_set_2_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxGapsPerRealDayAction);
		menuA_students_set_2_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxGapsPerWeekForRealDaysAction);

		menuA_students_set_2_time_constraints->addAction(dataTimeConstraintsStudentsSetEarlyMaxBeginningsAtSecondHourAction);
		menuA_students_set_2_time_constraints->addAction(dataTimeConstraintsStudentsSetMorningsEarlyMaxBeginningsAtSecondHourAction);
		menuA_students_set_2_time_constraints->addAction(dataTimeConstraintsStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHourAction);

		menuA_students_set_2_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxHoursDailyInIntervalAction);
		menuA_students_set_2_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxHoursDailyRealDaysAction);
		menuA_students_set_2_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxSpanPerRealDayAction);
		menuA_students_set_2_time_constraints->addAction(dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyRealDaysAction);

		menuA_students_set_3_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxTwoActivityTagsPerDayFromN1N2N3Action);
		menuA_students_set_3_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxTwoActivityTagsPerRealDayFromN1N2N3Action);

		menuA_students_set_3_time_constraints->addAction(dataTimeConstraintsStudentsSetMinHoursDailyAction);
		menuA_students_set_3_time_constraints->addAction(dataTimeConstraintsStudentsSetMinHoursPerMorningAction);
		menuA_students_set_3_time_constraints->addAction(dataTimeConstraintsStudentsSetMinHoursPerAfternoonAction);

		menuA_students_set_3_time_constraints->addAction(dataTimeConstraintsStudentsSetActivityTagMinHoursDailyAction);
		
		menuA_students_set_3_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxHoursContinuouslyAction);
		menuA_students_set_3_time_constraints->addAction(dataTimeConstraintsStudentsSetActivityTagMaxHoursContinuouslyAction);

		menuA_students_set_3_time_constraints->addAction(dataTimeConstraintsStudentsSetMinGapsBetweenActivityTagAction);
		menuA_students_set_3_time_constraints->addAction(dataTimeConstraintsStudentsSetMinGapsBetweenActivityTagPerRealDayAction);
		menuA_students_set_3_time_constraints->addAction(dataTimeConstraintsStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction);
		menuA_students_set_3_time_constraints->addAction(dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsAction);
		menuA_students_set_3_time_constraints->addAction(dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction);
		menuA_students_set_3_time_constraints->addAction(dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction);

		menuA_students_set_4_time_constraints->addAction(dataTimeConstraintsStudentsSetIntervalMaxDaysPerWeekAction);
		menuA_students_set_4_time_constraints->addAction(dataTimeConstraintsStudentsSetMorningIntervalMaxDaysPerWeekAction);
		menuA_students_set_4_time_constraints->addAction(dataTimeConstraintsStudentsSetAfternoonIntervalMaxDaysPerWeekAction);

		menuA_students_set_4_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxHoursPerAllAfternoonsAction);

		menuA_students_set_4_time_constraints->addAction(dataTimeConstraintsStudentsSetMinRestingHoursBetweenMorningAndAfternoonAction);
		
		///////
		menuAll_students_1_time_constraints->addAction(dataTimeConstraintsStudentsNotAvailableTimesAction);
		menuAll_students_1_time_constraints->addSeparator();

		menuAll_students_1_time_constraints->addAction(dataTimeConstraintsStudentsMaxDaysPerWeekAction);
		menuAll_students_1_time_constraints->addAction(dataTimeConstraintsStudentsMaxRealDaysPerWeekAction);
		menuAll_students_1_time_constraints->addAction(dataTimeConstraintsStudentsMaxMorningsPerWeekAction);
		menuAll_students_1_time_constraints->addAction(dataTimeConstraintsStudentsMaxAfternoonsPerWeekAction);

		menuAll_students_1_time_constraints->addAction(dataTimeConstraintsStudentsMaxThreeConsecutiveDaysAction);

		menuAll_students_1_time_constraints->addAction(dataTimeConstraintsStudentsMinMorningsPerWeekAction);
		menuAll_students_1_time_constraints->addAction(dataTimeConstraintsStudentsMinAfternoonsPerWeekAction);

		menuAll_students_2_time_constraints->addAction(dataTimeConstraintsStudentsMaxGapsPerDayAction);
		menuAll_students_2_time_constraints->addAction(dataTimeConstraintsStudentsMaxGapsPerWeekAction);

		menuAll_students_2_time_constraints->addAction(dataTimeConstraintsStudentsMaxGapsPerRealDayAction);
		menuAll_students_2_time_constraints->addAction(dataTimeConstraintsStudentsMaxGapsPerWeekForRealDaysAction);
		
		menuAll_students_2_time_constraints->addAction(dataTimeConstraintsStudentsEarlyMaxBeginningsAtSecondHourAction);
		menuAll_students_2_time_constraints->addAction(dataTimeConstraintsStudentsMorningsEarlyMaxBeginningsAtSecondHourAction);
		menuAll_students_2_time_constraints->addAction(dataTimeConstraintsStudentsAfternoonsEarlyMaxBeginningsAtSecondHourAction);

		menuAll_students_2_time_constraints->addAction(dataTimeConstraintsStudentsMaxHoursDailyInIntervalAction);
		menuAll_students_2_time_constraints->addAction(dataTimeConstraintsStudentsMaxHoursDailyRealDaysAction);
		menuAll_students_2_time_constraints->addAction(dataTimeConstraintsStudentsMaxSpanPerRealDayAction);
		menuAll_students_2_time_constraints->addAction(dataTimeConstraintsStudentsActivityTagMaxHoursDailyRealDaysAction);

		menuAll_students_3_time_constraints->addAction(dataTimeConstraintsStudentsMaxTwoActivityTagsPerDayFromN1N2N3Action);
		menuAll_students_3_time_constraints->addAction(dataTimeConstraintsStudentsMaxTwoActivityTagsPerRealDayFromN1N2N3Action);
		
		menuAll_students_3_time_constraints->addAction(dataTimeConstraintsStudentsMinHoursDailyAction);
		menuAll_students_3_time_constraints->addAction(dataTimeConstraintsStudentsMinHoursPerMorningAction);
		menuAll_students_3_time_constraints->addAction(dataTimeConstraintsStudentsMinHoursPerAfternoonAction);

		menuAll_students_3_time_constraints->addAction(dataTimeConstraintsStudentsActivityTagMinHoursDailyAction);
		
		menuAll_students_3_time_constraints->addAction(dataTimeConstraintsStudentsMaxHoursContinuouslyAction);
		menuAll_students_3_time_constraints->addAction(dataTimeConstraintsStudentsActivityTagMaxHoursContinuouslyAction);

		menuAll_students_3_time_constraints->addAction(dataTimeConstraintsStudentsMinGapsBetweenActivityTagAction);
		menuAll_students_3_time_constraints->addAction(dataTimeConstraintsStudentsMinGapsBetweenActivityTagPerRealDayAction);
		menuAll_students_3_time_constraints->addAction(dataTimeConstraintsStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction);
		menuAll_students_3_time_constraints->addAction(dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsAction);
		menuAll_students_3_time_constraints->addAction(dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction);
		menuAll_students_3_time_constraints->addAction(dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction);

		menuAll_students_4_time_constraints->addAction(dataTimeConstraintsStudentsIntervalMaxDaysPerWeekAction);
		menuAll_students_4_time_constraints->addAction(dataTimeConstraintsStudentsMorningIntervalMaxDaysPerWeekAction);
		menuAll_students_4_time_constraints->addAction(dataTimeConstraintsStudentsAfternoonIntervalMaxDaysPerWeekAction);

		menuAll_students_4_time_constraints->addAction(dataTimeConstraintsStudentsMaxHoursPerAllAfternoonsAction);

		menuAll_students_4_time_constraints->addAction(dataTimeConstraintsStudentsMinRestingHoursBetweenMorningAndAfternoonAction);
		///////

		menuActivities_preferred_times_time_constraints->addAction(dataTimeConstraintsActivityPreferredStartingTimeAction);
		menuActivities_preferred_times_time_constraints->addAction(dataTimeConstraintsActivityPreferredStartingTimesAction);
		menuActivities_preferred_times_time_constraints->addAction(dataTimeConstraintsActivityPreferredTimeSlotsAction);
		menuActivities_preferred_times_time_constraints->addSeparator();
		menuActivities_preferred_times_time_constraints->addAction(dataTimeConstraintsActivitiesPreferredStartingTimesAction);
		menuActivities_preferred_times_time_constraints->addAction(dataTimeConstraintsActivitiesPreferredTimeSlotsAction);
		menuActivities_preferred_times_time_constraints->addSeparator();
		menuActivities_preferred_times_time_constraints->addAction(dataTimeConstraintsSubactivitiesPreferredStartingTimesAction);
		menuActivities_preferred_times_time_constraints->addAction(dataTimeConstraintsSubactivitiesPreferredTimeSlotsAction);

		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsMinDaysBetweenActivitiesAction);
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsMinHalfDaysBetweenActivitiesAction);
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsMaxDaysBetweenActivitiesAction);
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsMaxHalfDaysBetweenActivitiesAction);

		menuActivities_others_1_time_constraints->addSeparator();
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivityBeginsStudentsDayAction);
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivitiesBeginStudentsDayAction);
		menuActivities_others_1_time_constraints->addSeparator();
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivityBeginsTeachersDayAction);
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivitiesBeginTeachersDayAction);

		menuActivities_others_1_time_constraints->addSeparator();
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivityEndsStudentsDayAction);
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivitiesEndStudentsDayAction);
		menuActivities_others_1_time_constraints->addSeparator();
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivityEndsTeachersDayAction);
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivitiesEndTeachersDayAction);

		menuActivities_others_1_time_constraints->addSeparator();
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivitiesSameStartingTimeAction);
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivitiesSameStartingDayAction);
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivitiesSameStartingHourAction);

		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsActivitiesOccupyMaxTimeSlotsFromSelectionAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsActivitiesOccupyMinTimeSlotsFromSelectionAction);
		menuActivities_others_2_time_constraints->addSeparator();

		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsTwoActivitiesOrderedAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsTwoSetsOfActivitiesOrderedAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsTwoActivitiesOrderedIfSameDayAction);
		menuActivities_others_2_time_constraints->addSeparator();
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsTwoActivitiesConsecutiveAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsTwoActivitiesGroupedAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsThreeActivitiesGroupedAction);
		menuActivities_others_2_time_constraints->addSeparator();
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsActivitiesMaxHourlySpanAction);
		menuActivities_others_2_time_constraints->addSeparator();
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsActivitiesNotOverlappingAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsActivityTagsNotOverlappingAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsActivitiesMaxSimultaneousInSelectedTimeSlotsAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsActivitiesMinSimultaneousInSelectedTimeSlotsAction);
		menuActivities_others_2_time_constraints->addSeparator();
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsMinGapsBetweenActivitiesAction);

		menuMisc_space_constraints->addAction(dataSpaceConstraintsBasicCompulsorySpaceAction);

		menuRooms_space_constraints->addAction(dataSpaceConstraintsRoomNotAvailableTimesAction);
		menuRooms_space_constraints->addAction(dataSpaceConstraintsTeacherRoomNotAvailableTimesAction);

		menuActivities_space_constraints->addAction(dataSpaceConstraintsActivityPreferredRoomAction);
		menuActivities_space_constraints->addAction(dataSpaceConstraintsActivityPreferredRoomsAction);
		menuActivities_space_constraints->addSeparator();
		menuActivities_space_constraints->addAction(dataSpaceConstraintsActivitiesSameRoomIfConsecutiveAction);
		menuActivities_space_constraints->addAction(dataSpaceConstraintsActivitiesOccupyMaxDifferentRoomsAction);

		menuSubjects_space_constraints->addAction(dataSpaceConstraintsSubjectPreferredRoomAction);
		menuSubjects_space_constraints->addAction(dataSpaceConstraintsSubjectPreferredRoomsAction);

		menuSubjects_and_activity_tags_space_constraints->addAction(dataSpaceConstraintsSubjectActivityTagPreferredRoomAction);
		menuSubjects_and_activity_tags_space_constraints->addAction(dataSpaceConstraintsSubjectActivityTagPreferredRoomsAction);

		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetHomeRoomAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetHomeRoomsAction);
		menuA_students_set_space_constraints->addSeparator();
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxRoomChangesPerDayAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxRoomChangesPerDayInIntervalAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxRoomChangesPerWeekAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMinGapsBetweenRoomChangesAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxRoomChangesPerRealDayAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxRoomChangesPerRealDayInIntervalAction);
		menuA_students_set_space_constraints->addSeparator();
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayInIntervalAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxBuildingChangesPerWeekAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMinGapsBetweenBuildingChangesAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxBuildingChangesPerRealDayAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxBuildingChangesPerRealDayInIntervalAction);
		//menuStudents_space_constraints->addSeparator();
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMaxRoomChangesPerDayAction);
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMaxRoomChangesPerDayInIntervalAction);
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMaxRoomChangesPerWeekAction);
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMinGapsBetweenRoomChangesAction);
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMaxRoomChangesPerRealDayAction);
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMaxRoomChangesPerRealDayInIntervalAction);
		menuAll_students_space_constraints->addSeparator();
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMaxBuildingChangesPerDayAction);
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMaxBuildingChangesPerDayInIntervalAction);
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMaxBuildingChangesPerWeekAction);
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMinGapsBetweenBuildingChangesAction);
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMaxBuildingChangesPerRealDayAction);
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMaxBuildingChangesPerRealDayInIntervalAction);

		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherHomeRoomAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherHomeRoomsAction);
		menuA_teacher_space_constraints->addSeparator();
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMaxRoomChangesPerDayAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMaxRoomChangesPerDayInIntervalAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMaxRoomChangesPerWeekAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMinGapsBetweenRoomChangesAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMaxRoomChangesPerRealDayAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMaxRoomChangesPerRealDayInIntervalAction);
		menuA_teacher_space_constraints->addSeparator();
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMaxBuildingChangesPerDayAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMaxBuildingChangesPerDayInIntervalAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMaxBuildingChangesPerWeekAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMinGapsBetweenBuildingChangesAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMaxBuildingChangesPerRealDayAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMaxBuildingChangesPerRealDayInIntervalAction);
		//menuTeachers_space_constraints->addSeparator();
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxRoomChangesPerDayAction);
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxRoomChangesPerDayInIntervalAction);
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxRoomChangesPerWeekAction);
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMinGapsBetweenRoomChangesAction);
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxRoomChangesPerRealDayAction);
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxRoomChangesPerRealDayInIntervalAction);
		menuAll_teachers_space_constraints->addSeparator();
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxBuildingChangesPerDayAction);
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxBuildingChangesPerDayInIntervalAction);
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxBuildingChangesPerWeekAction);
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMinGapsBetweenBuildingChangesAction);
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxBuildingChangesPerRealDayAction);
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxBuildingChangesPerRealDayInIntervalAction);

		menuActivity_tags_space_constraints->addAction(dataSpaceConstraintsActivityTagPreferredRoomAction);
		menuActivity_tags_space_constraints->addAction(dataSpaceConstraintsActivityTagPreferredRoomsAction);
	}
	else if(gt.rules.mode==BLOCK_PLANNING){
		menuMisc_time_constraints->addAction(dataTimeConstraintsBasicCompulsoryTimeAction);
		menuMisc_time_constraints->addAction(dataTimeConstraintsBreakTimesAction);

		menuA_teacher_time_constraints=new QMenu(menuTeachers_time_constraints);
		menuAll_teachers_time_constraints=new QMenu(menuTeachers_time_constraints);
		//
		menuA_students_set_time_constraints=new QMenu(menuStudents_time_constraints);
		menuAll_students_time_constraints=new QMenu(menuStudents_time_constraints);
		//
		menuActivities_preferred_times_time_constraints=new QMenu(menuActivities_time_constraints);
		menuActivities_others_1_time_constraints=new QMenu(menuActivities_time_constraints);
		menuActivities_others_2_time_constraints=new QMenu(menuActivities_time_constraints);
		//
		menuTeachers_time_constraints->addMenu(menuA_teacher_time_constraints);
		menuTeachers_time_constraints->addMenu(menuAll_teachers_time_constraints);
		//
		menuStudents_time_constraints->addMenu(menuA_students_set_time_constraints);
		menuStudents_time_constraints->addMenu(menuAll_students_time_constraints);
		//
		menuActivities_time_constraints->addMenu(menuActivities_preferred_times_time_constraints);
		menuActivities_time_constraints->addMenu(menuActivities_others_1_time_constraints);
		menuActivities_time_constraints->addMenu(menuActivities_others_2_time_constraints);
		//
		menuA_teacher_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "A teacher", nullptr));
		menuAll_teachers_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "All teachers", nullptr));
		//
		menuA_students_set_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "A students set", nullptr));
		menuAll_students_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "All students", nullptr));
		//
		menuActivities_preferred_times_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "Preferred times", "The menu of the list of preferred time constraints for activities"));
		menuActivities_others_1_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "Others (1)", "The menu of the list of other time constraints for activities (1st part)"));
		menuActivities_others_2_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "Others (2)", "The menu of the list of other time constraints for activities (2nd part)"));

		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherNotAvailableTimesAction);
		menuA_teacher_time_constraints->addSeparator();
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxDaysPerWeekAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinDaysPerWeekAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxGapsPerDayAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxGapsPerWeekAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxHoursDailyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxHoursDailyInIntervalAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxSpanPerDayAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherActivityTagMaxHoursDailyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinHoursDailyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherActivityTagMinHoursDailyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxHoursContinuouslyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherActivityTagMaxHoursContinuouslyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinGapsBetweenActivityTagAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherIntervalMaxDaysPerWeekAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinRestingHoursAction);

		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersNotAvailableTimesAction);
		menuAll_teachers_time_constraints->addSeparator();
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxDaysPerWeekAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMinDaysPerWeekAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxGapsPerDayAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxGapsPerWeekAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxHoursDailyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxHoursDailyInIntervalAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxSpanPerDayAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersActivityTagMaxHoursDailyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMinHoursDailyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersActivityTagMinHoursDailyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxHoursContinuouslyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersActivityTagMaxHoursContinuouslyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMinGapsBetweenActivityTagAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersIntervalMaxDaysPerWeekAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMinRestingHoursAction);

		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetNotAvailableTimesAction);
		menuA_students_set_time_constraints->addSeparator();
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxDaysPerWeekAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxGapsPerDayAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxGapsPerWeekAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetEarlyMaxBeginningsAtSecondHourAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxHoursDailyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxHoursDailyInIntervalAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxSpanPerDayAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMinHoursDailyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetActivityTagMinHoursDailyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxHoursContinuouslyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetActivityTagMaxHoursContinuouslyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMinGapsBetweenActivityTagAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetIntervalMaxDaysPerWeekAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMinRestingHoursAction);

		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsNotAvailableTimesAction);
		menuAll_students_time_constraints->addSeparator();
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxDaysPerWeekAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxGapsPerDayAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxGapsPerWeekAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsEarlyMaxBeginningsAtSecondHourAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxHoursDailyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxHoursDailyInIntervalAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxSpanPerDayAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsActivityTagMaxHoursDailyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMinHoursDailyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsActivityTagMinHoursDailyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxHoursContinuouslyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsActivityTagMaxHoursContinuouslyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMinGapsBetweenActivityTagAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsIntervalMaxDaysPerWeekAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMinRestingHoursAction);

		menuActivities_preferred_times_time_constraints->addAction(dataTimeConstraintsActivityPreferredStartingTimeAction);
		menuActivities_preferred_times_time_constraints->addAction(dataTimeConstraintsActivityPreferredStartingTimesAction);
		menuActivities_preferred_times_time_constraints->addAction(dataTimeConstraintsActivityPreferredTimeSlotsAction);
		menuActivities_preferred_times_time_constraints->addSeparator();
		menuActivities_preferred_times_time_constraints->addAction(dataTimeConstraintsActivitiesPreferredStartingTimesAction);
		menuActivities_preferred_times_time_constraints->addAction(dataTimeConstraintsActivitiesPreferredTimeSlotsAction);
		menuActivities_preferred_times_time_constraints->addSeparator();
		menuActivities_preferred_times_time_constraints->addAction(dataTimeConstraintsSubactivitiesPreferredStartingTimesAction);
		menuActivities_preferred_times_time_constraints->addAction(dataTimeConstraintsSubactivitiesPreferredTimeSlotsAction);

		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsMinDaysBetweenActivitiesAction);
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsMaxDaysBetweenActivitiesAction);

		menuActivities_others_1_time_constraints->addSeparator();
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivityBeginsStudentsDayAction);
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivitiesBeginStudentsDayAction);
		menuActivities_others_1_time_constraints->addSeparator();
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivityBeginsTeachersDayAction);
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivitiesBeginTeachersDayAction);

		menuActivities_others_1_time_constraints->addSeparator();
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivityEndsStudentsDayAction);
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivitiesEndStudentsDayAction);
		menuActivities_others_1_time_constraints->addSeparator();
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivityEndsTeachersDayAction);
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivitiesEndTeachersDayAction);

		menuActivities_others_1_time_constraints->addSeparator();
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivitiesSameStartingTimeAction);
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivitiesSameStartingDayAction);
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivitiesSameStartingHourAction);

		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsActivitiesOccupyMaxTimeSlotsFromSelectionAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsActivitiesOccupyMinTimeSlotsFromSelectionAction);
		menuActivities_others_2_time_constraints->addSeparator();

		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsMaxTotalActivitiesFromSetInSelectedTimeSlotsAction);
		menuActivities_others_2_time_constraints->addSeparator();
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsTwoActivitiesOrderedAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsTwoSetsOfActivitiesOrderedAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsTwoActivitiesOrderedIfSameDayAction);
		menuActivities_others_2_time_constraints->addSeparator();
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsTwoActivitiesConsecutiveAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsTwoActivitiesGroupedAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsThreeActivitiesGroupedAction);
		menuActivities_others_2_time_constraints->addSeparator();
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsActivitiesMaxHourlySpanAction);
		menuActivities_others_2_time_constraints->addSeparator();
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsActivitiesNotOverlappingAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsActivityTagsNotOverlappingAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsActivitiesMaxSimultaneousInSelectedTimeSlotsAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsActivitiesMinSimultaneousInSelectedTimeSlotsAction);
		menuActivities_others_2_time_constraints->addSeparator();
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsMinGapsBetweenActivitiesAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsMaxGapsBetweenActivitiesAction);

		menuMisc_space_constraints->addAction(dataSpaceConstraintsBasicCompulsorySpaceAction);

		menuRooms_space_constraints->addAction(dataSpaceConstraintsRoomNotAvailableTimesAction);
		menuRooms_space_constraints->addAction(dataSpaceConstraintsTeacherRoomNotAvailableTimesAction);

		menuActivities_space_constraints->addAction(dataSpaceConstraintsActivityPreferredRoomAction);
		menuActivities_space_constraints->addAction(dataSpaceConstraintsActivityPreferredRoomsAction);
		menuActivities_space_constraints->addSeparator();
		menuActivities_space_constraints->addAction(dataSpaceConstraintsActivitiesSameRoomIfConsecutiveAction);
		menuActivities_space_constraints->addAction(dataSpaceConstraintsActivitiesOccupyMaxDifferentRoomsAction);

		menuSubjects_space_constraints->addAction(dataSpaceConstraintsSubjectPreferredRoomAction);
		menuSubjects_space_constraints->addAction(dataSpaceConstraintsSubjectPreferredRoomsAction);

		menuSubjects_and_activity_tags_space_constraints->addAction(dataSpaceConstraintsSubjectActivityTagPreferredRoomAction);
		menuSubjects_and_activity_tags_space_constraints->addAction(dataSpaceConstraintsSubjectActivityTagPreferredRoomsAction);

		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetHomeRoomAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetHomeRoomsAction);
		menuA_students_set_space_constraints->addSeparator();
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxRoomChangesPerDayAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxRoomChangesPerDayInIntervalAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxRoomChangesPerWeekAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMinGapsBetweenRoomChangesAction);
		menuA_students_set_space_constraints->addSeparator();
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayInIntervalAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxBuildingChangesPerWeekAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMinGapsBetweenBuildingChangesAction);
		//menuStudents_space_constraints->addSeparator();
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMaxRoomChangesPerDayAction);
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMaxRoomChangesPerDayInIntervalAction);
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMaxRoomChangesPerWeekAction);
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMinGapsBetweenRoomChangesAction);
		menuAll_students_space_constraints->addSeparator();
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMaxBuildingChangesPerDayAction);
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMaxBuildingChangesPerDayInIntervalAction);
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMaxBuildingChangesPerWeekAction);
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMinGapsBetweenBuildingChangesAction);

		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherHomeRoomAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherHomeRoomsAction);
		menuA_teacher_space_constraints->addSeparator();
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMaxRoomChangesPerDayAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMaxRoomChangesPerDayInIntervalAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMaxRoomChangesPerWeekAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMinGapsBetweenRoomChangesAction);
		menuA_teacher_space_constraints->addSeparator();
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMaxBuildingChangesPerDayAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMaxBuildingChangesPerDayInIntervalAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMaxBuildingChangesPerWeekAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMinGapsBetweenBuildingChangesAction);
		//menuTeachers_space_constraints->addSeparator();
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxRoomChangesPerDayAction);
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxRoomChangesPerDayInIntervalAction);
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxRoomChangesPerWeekAction);
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMinGapsBetweenRoomChangesAction);
		menuAll_teachers_space_constraints->addSeparator();
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxBuildingChangesPerDayAction);
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxBuildingChangesPerDayInIntervalAction);
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxBuildingChangesPerWeekAction);
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMinGapsBetweenBuildingChangesAction);

		menuActivity_tags_space_constraints->addAction(dataSpaceConstraintsActivityTagPreferredRoomAction);
		menuActivity_tags_space_constraints->addAction(dataSpaceConstraintsActivityTagPreferredRoomsAction);
	}
	else if(gt.rules.mode==TERMS){
		menuMisc_time_constraints->addAction(dataTimeConstraintsBasicCompulsoryTimeAction);
		menuMisc_time_constraints->addAction(dataTimeConstraintsBreakTimesAction);

		menuA_teacher_time_constraints=new QMenu(menuTeachers_time_constraints);
		menuAll_teachers_time_constraints=new QMenu(menuTeachers_time_constraints);
		//
		menuA_students_set_time_constraints=new QMenu(menuStudents_time_constraints);
		menuAll_students_time_constraints=new QMenu(menuStudents_time_constraints);
		//
		menuActivities_preferred_times_time_constraints=new QMenu(menuActivities_time_constraints);
		menuActivities_others_1_time_constraints=new QMenu(menuActivities_time_constraints);
		menuActivities_others_2_time_constraints=new QMenu(menuActivities_time_constraints);
		menuActivities_others_3_time_constraints=new QMenu(menuActivities_time_constraints);
		//
		menuTeachers_time_constraints->addMenu(menuA_teacher_time_constraints);
		menuTeachers_time_constraints->addMenu(menuAll_teachers_time_constraints);
		//
		menuStudents_time_constraints->addMenu(menuA_students_set_time_constraints);
		menuStudents_time_constraints->addMenu(menuAll_students_time_constraints);
		//
		menuActivities_time_constraints->addMenu(menuActivities_preferred_times_time_constraints);
		menuActivities_time_constraints->addMenu(menuActivities_others_1_time_constraints);
		menuActivities_time_constraints->addMenu(menuActivities_others_2_time_constraints);
		menuActivities_time_constraints->addMenu(menuActivities_others_3_time_constraints);
		//
		menuA_teacher_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "A teacher", nullptr));
		menuAll_teachers_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "All teachers", nullptr));
		//
		menuA_students_set_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "A students set", nullptr));
		menuAll_students_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "All students", nullptr));
		//
		menuActivities_preferred_times_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "Preferred times", "The menu of the list of preferred time constraints for activities"));
		menuActivities_others_1_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "Others (1)", "The menu of the list of other time constraints for activities (1st part)"));
		menuActivities_others_2_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "Others (2)", "The menu of the list of other time constraints for activities (2nd part)"));
		menuActivities_others_3_time_constraints->setTitle(QCoreApplication::translate("FetMainForm_template", "Others (3)", "The menu of the list of other time constraints for activities (3rd part)"));

		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherNotAvailableTimesAction);
		menuA_teacher_time_constraints->addSeparator();
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxDaysPerWeekAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinDaysPerWeekAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxGapsPerDayAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxGapsPerWeekAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxHoursDailyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxHoursDailyInIntervalAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxSpanPerDayAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherActivityTagMaxHoursDailyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinHoursDailyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherActivityTagMinHoursDailyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxHoursContinuouslyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherActivityTagMaxHoursContinuouslyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinGapsBetweenActivityTagAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherIntervalMaxDaysPerWeekAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinRestingHoursAction);

		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersNotAvailableTimesAction);
		menuAll_teachers_time_constraints->addSeparator();
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxDaysPerWeekAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMinDaysPerWeekAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxGapsPerDayAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxGapsPerWeekAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxHoursDailyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxHoursDailyInIntervalAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxSpanPerDayAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersActivityTagMaxHoursDailyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMinHoursDailyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersActivityTagMinHoursDailyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxHoursContinuouslyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersActivityTagMaxHoursContinuouslyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMinGapsBetweenActivityTagAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersIntervalMaxDaysPerWeekAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMinRestingHoursAction);

		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetNotAvailableTimesAction);
		menuA_students_set_time_constraints->addSeparator();
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxDaysPerWeekAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxGapsPerDayAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxGapsPerWeekAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetEarlyMaxBeginningsAtSecondHourAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxHoursDailyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxHoursDailyInIntervalAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxSpanPerDayAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMinHoursDailyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetActivityTagMinHoursDailyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxHoursContinuouslyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetActivityTagMaxHoursContinuouslyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMinGapsBetweenActivityTagAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetIntervalMaxDaysPerWeekAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMinRestingHoursAction);

		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsNotAvailableTimesAction);
		menuAll_students_time_constraints->addSeparator();
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxDaysPerWeekAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxGapsPerDayAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxGapsPerWeekAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsEarlyMaxBeginningsAtSecondHourAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxHoursDailyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxHoursDailyInIntervalAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxSpanPerDayAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsActivityTagMaxHoursDailyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMinHoursDailyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsActivityTagMinHoursDailyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxHoursContinuouslyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsActivityTagMaxHoursContinuouslyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMinGapsBetweenActivityTagAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsIntervalMaxDaysPerWeekAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMinRestingHoursAction);

		menuActivities_preferred_times_time_constraints->addAction(dataTimeConstraintsActivityPreferredStartingTimeAction);
		menuActivities_preferred_times_time_constraints->addAction(dataTimeConstraintsActivityPreferredStartingTimesAction);
		menuActivities_preferred_times_time_constraints->addAction(dataTimeConstraintsActivityPreferredTimeSlotsAction);
		menuActivities_preferred_times_time_constraints->addSeparator();
		menuActivities_preferred_times_time_constraints->addAction(dataTimeConstraintsActivitiesPreferredStartingTimesAction);
		menuActivities_preferred_times_time_constraints->addAction(dataTimeConstraintsActivitiesPreferredTimeSlotsAction);
		menuActivities_preferred_times_time_constraints->addSeparator();
		menuActivities_preferred_times_time_constraints->addAction(dataTimeConstraintsSubactivitiesPreferredStartingTimesAction);
		menuActivities_preferred_times_time_constraints->addAction(dataTimeConstraintsSubactivitiesPreferredTimeSlotsAction);

		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsMinDaysBetweenActivitiesAction);
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsMaxDaysBetweenActivitiesAction);

		menuActivities_others_1_time_constraints->addSeparator();
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivityBeginsStudentsDayAction);
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivitiesBeginStudentsDayAction);
		menuActivities_others_1_time_constraints->addSeparator();
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivityBeginsTeachersDayAction);
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivitiesBeginTeachersDayAction);

		menuActivities_others_1_time_constraints->addSeparator();
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivityEndsStudentsDayAction);
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivitiesEndStudentsDayAction);
		menuActivities_others_1_time_constraints->addSeparator();
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivityEndsTeachersDayAction);
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivitiesEndTeachersDayAction);

		menuActivities_others_1_time_constraints->addSeparator();
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivitiesSameStartingTimeAction);
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivitiesSameStartingDayAction);
		menuActivities_others_1_time_constraints->addAction(dataTimeConstraintsActivitiesSameStartingHourAction);

		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsActivitiesOccupyMaxTimeSlotsFromSelectionAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsActivitiesOccupyMinTimeSlotsFromSelectionAction);
		menuActivities_others_2_time_constraints->addSeparator();

		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsTwoActivitiesOrderedAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsTwoSetsOfActivitiesOrderedAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsTwoActivitiesOrderedIfSameDayAction);
		menuActivities_others_2_time_constraints->addSeparator();
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsTwoActivitiesConsecutiveAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsTwoActivitiesGroupedAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsThreeActivitiesGroupedAction);
		menuActivities_others_2_time_constraints->addSeparator();
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsActivitiesMaxHourlySpanAction);
		menuActivities_others_2_time_constraints->addSeparator();
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsActivitiesNotOverlappingAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsActivityTagsNotOverlappingAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsActivitiesMaxSimultaneousInSelectedTimeSlotsAction);
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsActivitiesMinSimultaneousInSelectedTimeSlotsAction);
		menuActivities_others_2_time_constraints->addSeparator();
		menuActivities_others_2_time_constraints->addAction(dataTimeConstraintsMinGapsBetweenActivitiesAction);

		//menuActivities_others_2_time_constraints->addSeparator();

		menuActivities_others_3_time_constraints->addAction(dataTimeConstraintsMaxTermsBetweenActivitiesAction);
		menuActivities_others_3_time_constraints->addAction(dataTimeConstraintsActivitiesMaxInATermAction);
		menuActivities_others_3_time_constraints->addAction(dataTimeConstraintsActivitiesMinInATermAction);
		menuActivities_others_3_time_constraints->addAction(dataTimeConstraintsActivitiesOccupyMaxTermsAction);

		menuMisc_space_constraints->addAction(dataSpaceConstraintsBasicCompulsorySpaceAction);

		menuRooms_space_constraints->addAction(dataSpaceConstraintsRoomNotAvailableTimesAction);
		menuRooms_space_constraints->addAction(dataSpaceConstraintsTeacherRoomNotAvailableTimesAction);

		menuActivities_space_constraints->addAction(dataSpaceConstraintsActivityPreferredRoomAction);
		menuActivities_space_constraints->addAction(dataSpaceConstraintsActivityPreferredRoomsAction);
		menuActivities_space_constraints->addSeparator();
		menuActivities_space_constraints->addAction(dataSpaceConstraintsActivitiesSameRoomIfConsecutiveAction);
		menuActivities_space_constraints->addAction(dataSpaceConstraintsActivitiesOccupyMaxDifferentRoomsAction);

		menuSubjects_space_constraints->addAction(dataSpaceConstraintsSubjectPreferredRoomAction);
		menuSubjects_space_constraints->addAction(dataSpaceConstraintsSubjectPreferredRoomsAction);

		menuSubjects_and_activity_tags_space_constraints->addAction(dataSpaceConstraintsSubjectActivityTagPreferredRoomAction);
		menuSubjects_and_activity_tags_space_constraints->addAction(dataSpaceConstraintsSubjectActivityTagPreferredRoomsAction);

		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetHomeRoomAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetHomeRoomsAction);
		menuA_students_set_space_constraints->addSeparator();
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxRoomChangesPerDayAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxRoomChangesPerDayInIntervalAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxRoomChangesPerWeekAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMinGapsBetweenRoomChangesAction);
		menuA_students_set_space_constraints->addSeparator();
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayInIntervalAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxBuildingChangesPerWeekAction);
		menuA_students_set_space_constraints->addAction(dataSpaceConstraintsStudentsSetMinGapsBetweenBuildingChangesAction);
		//menuStudents_space_constraints->addSeparator();
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMaxRoomChangesPerDayAction);
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMaxRoomChangesPerDayInIntervalAction);
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMaxRoomChangesPerWeekAction);
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMinGapsBetweenRoomChangesAction);
		menuAll_students_space_constraints->addSeparator();
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMaxBuildingChangesPerDayAction);
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMaxBuildingChangesPerDayInIntervalAction);
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMaxBuildingChangesPerWeekAction);
		menuAll_students_space_constraints->addAction(dataSpaceConstraintsStudentsMinGapsBetweenBuildingChangesAction);

		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherHomeRoomAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherHomeRoomsAction);
		menuA_teacher_space_constraints->addSeparator();
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMaxRoomChangesPerDayAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMaxRoomChangesPerDayInIntervalAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMaxRoomChangesPerWeekAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMinGapsBetweenRoomChangesAction);
		menuA_teacher_space_constraints->addSeparator();
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMaxBuildingChangesPerDayAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMaxBuildingChangesPerDayInIntervalAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMaxBuildingChangesPerWeekAction);
		menuA_teacher_space_constraints->addAction(dataSpaceConstraintsTeacherMinGapsBetweenBuildingChangesAction);
		//menuTeachers_space_constraints->addSeparator();
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxRoomChangesPerDayAction);
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxRoomChangesPerDayInIntervalAction);
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxRoomChangesPerWeekAction);
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMinGapsBetweenRoomChangesAction);
		menuAll_teachers_space_constraints->addSeparator();
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxBuildingChangesPerDayAction);
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxBuildingChangesPerDayInIntervalAction);
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxBuildingChangesPerWeekAction);
		menuAll_teachers_space_constraints->addAction(dataSpaceConstraintsTeachersMinGapsBetweenBuildingChangesAction);

		menuActivity_tags_space_constraints->addAction(dataSpaceConstraintsActivityTagPreferredRoomAction);
		menuActivity_tags_space_constraints->addAction(dataSpaceConstraintsActivityTagPreferredRoomsAction);
	}
	else{
		assert(0);
	}
}

void FetMainForm::setEnabledIcon(QAction* action, bool enabled)
{
	assert(action!=nullptr);

	static QIcon locked(":/images/locked.png");
	static QIcon unlocked(":/images/unlocked.png");
	
	if(enabled)
		action->setIcon(unlocked);
	else
		action->setIcon(locked);
}

//this is another place (out of two) in which you need to add a new language. The other one is in the file fet.cpp
void FetMainForm::populateLanguagesMap(QMap<QString, QString>& languagesMap)
{
	languagesMap.clear();

	languagesMap.insert("en_US", QString("US English"));
	languagesMap.insert("en_GB", QString("British English"));

	languagesMap.insert("ar", QString("عربي"));
	languagesMap.insert("ca", QString("Català"));
	languagesMap.insert("de", QString("Deutsch"));
	languagesMap.insert("el", QString("Ελληνικά"));
	languagesMap.insert("es", QString("Español"));
	languagesMap.insert("fr", QString("Français"));
	languagesMap.insert("id", QString("Indonesia"));
	languagesMap.insert("it", QString("Italiano"));
	languagesMap.insert("ro", QString("Română"));
	languagesMap.insert("uk", QString("Українська"));
	languagesMap.insert("pt_BR", QString("Português Brasil"));
	languagesMap.insert("da", QString("Dansk"));
	languagesMap.insert("sr", QString("Српски"));
	languagesMap.insert("gl", QString("Galego"));
	languagesMap.insert("vi", QString("Tiếng Việt"));
	languagesMap.insert("sq", QString("Shqip"));
	languagesMap.insert("zh_TW", QString("正體字型"));
	languagesMap.insert("cs", QString("Český"));
	languagesMap.insert("ja", QString("日本語"));
	languagesMap.insert("ru", QString("Русский"));
	languagesMap.insert("bg", QString("Български"));

	//We need to write the following language names in their native language, as above:
	languagesMap.insert("hu", QString("Hungarian"));
	languagesMap.insert("lt", QString("Lithuanian"));
	languagesMap.insert("mk", QString("Macedonian"));
	languagesMap.insert("ms", QString("Malay"));
	languagesMap.insert("nl", QString("Dutch"));
	languagesMap.insert("pl", QString("Polish"));
	languagesMap.insert("tr", QString("Turkish"));
	languagesMap.insert("fa", QString("Persian"));
	languagesMap.insert("si", QString("Sinhala"));
	languagesMap.insert("sk", QString("Slovak"));
	languagesMap.insert("he", QString("Hebrew"));
	languagesMap.insert("uz", QString("Uzbek"));
	languagesMap.insert("zh_CN", QString("Chinese Simplified"));
	languagesMap.insert("eu", QString("Basque"));
}

void FetMainForm::enableNotPerfectMessage()
{
	QString s=tr("Constraint is not enabled. To use this type of constraint you must enable it from the Settings->Advanced menu.");
	s+="\n\n";
	s+=tr("Explanation:");
	s+=" ";
	s+=tr("Constraints of this type are good, working, but they are not perfectly optimized.");
	s+=" ";
	s+=tr("For some situations, the generation of the timetable may take too long or be impossible.");
	s+="\n\n";
	s+=tr("Use with caution.");

	QMessageBox::information(this, tr("FET information"), s);
}

void FetMainForm::checkForUpdatesToggled(bool checked)
{
	if(checked==true){
		QString s;
		s+=tr("Please note that, by enabling this option, each time you start FET it will get the file %1 from the FET homepage, so the "
			"request for this file will be visible on the server, along with your IP address and access time.")
			.arg("https://lalescu.ro/liviu/fet/crtversion/crtversion.txt");
		s+=" ";
		s+=tr("Also, there will be visible on the server your current FET version, your current Qt version (the C++ toolkit used by FET), "
			"your operating system name and version, and your processor architecture type.");
		s+=" ";
		s+=tr("Thus, it could be deduced if and when you use FET.");
		s+="\n\n";
		s+=tr("Do you agree?");

		QMessageBox::StandardButton b=QMessageBox::question(this, tr("FET question"), s, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

		if(b!=QMessageBox::Yes){
			disconnect(checkForUpdatesAction, &QAction::toggled, this, &FetMainForm::checkForUpdatesToggled);
			checkForUpdatesAction->setChecked(false);
			connect(checkForUpdatesAction, &QAction::toggled, this, &FetMainForm::checkForUpdatesToggled);
			return;
		}
	}

	checkForUpdates=checked;
}

void FetMainForm::useColorsToggled(bool checked)
{
	Q_UNUSED(checked);
	
	USE_GUI_COLORS=settingsUseColorsAction->isChecked();
	
	LockUnlock::increaseCommunicationSpinBox();
}

void FetMainForm::showSubgroupsInComboBoxesToggled(bool checked)
{
	if(checked==false){
		QString s=tr("Note: if you disable this option, you will not be able to add/modify constraints for subgroups,"
		 " and you will not be able to make filters using a particular subgroup, when selecting activities.");
		//s+="\n\n";
		//s+=tr("Continue only if you know what you are doing.");
	
		QMessageBox::StandardButton b=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
	
		if(b!=QMessageBox::Ok){
			disconnect(settingsShowSubgroupsInComboBoxesAction, &QAction::toggled, this, &FetMainForm::showSubgroupsInComboBoxesToggled);
			settingsShowSubgroupsInComboBoxesAction->setChecked(true);
			connect(settingsShowSubgroupsInComboBoxesAction, &QAction::toggled, this, &FetMainForm::showSubgroupsInComboBoxesToggled);
			return;
		}
	}
	
	SHOW_SUBGROUPS_IN_COMBO_BOXES=checked;
}

void FetMainForm::showSubgroupsInActivityPlanningToggled(bool checked)
{
	if(checked==false){
		QString s=tr("Note: if you disable this option, you will not be able to view activities for subgroups"
		 " in the activity planning dialog.");
		//s+="\n\n";
		//s+=tr("Continue only if you know what you are doing.");
	
		QMessageBox::StandardButton b=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
	
		if(b!=QMessageBox::Ok){
			disconnect(settingsShowSubgroupsInActivityPlanningAction, &QAction::toggled, this, &FetMainForm::showSubgroupsInActivityPlanningToggled);
			settingsShowSubgroupsInActivityPlanningAction->setChecked(true);
			connect(settingsShowSubgroupsInActivityPlanningAction, &QAction::toggled, this, &FetMainForm::showSubgroupsInActivityPlanningToggled);
			return;
		}
	}
	
	SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING=checked;
}

void FetMainForm::settingsFontIsUserSelectableAction_toggled()
{
	fontIsUserSelectable=settingsFontIsUserSelectableAction->isChecked();
	if(!fontIsUserSelectable){
		qApp->setFont(originalFont);
		userChoseAFont=false;
	}
}

void FetMainForm::settingsFontAction_triggered()
{
	if(!fontIsUserSelectable){
		QMessageBox::warning(this, tr("FET warning"), tr("You are not allowed to select the font, because the check box 'The font is user selectable' is unchecked."
		 " Please select that option, firstly. You can find that option immediately above the 'Font' entry."));
		return;
	}

	bool ok;
	QFont newFont=QFontDialog::getFont(&ok, qApp->font(), this, tr("Please choose the new font"));
	if(ok){
		qApp->setFont(newFont);
		userChoseAFont=true;
	}
}

void FetMainForm::settingsAutosaveAction_triggered()
{
	SettingsAutosaveForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::modeOfficialAction_triggered()
{
	if(!gt.rules.initialized){
		modeOfficialAction->setChecked(currentMode==OFFICIAL);
		modeMorningsAfternoonsAction->setChecked(currentMode==MORNINGS_AFTERNOONS);
		modeBlockPlanningAction->setChecked(currentMode==BLOCK_PLANNING);
		modeTermsAction->setChecked(currentMode==TERMS);
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(gt.rules.mode==OFFICIAL){
		modeOfficialAction->setChecked(true);
		
		return;
	}
	
	QString s;
	if(gt.rules.mode==MORNINGS_AFTERNOONS){
		s=tr("Are you sure you want to convert your file to the official mode? This might involve data loss."
		 " All the teachers will have uninitialized mornings-afternoons behavior. Also, the incompatible constraints will be removed.");
	}
	else if(gt.rules.mode==BLOCK_PLANNING){
		s=tr("Are you sure you want to convert your file to the official mode? This might involve data loss."
		 " The incompatible constraints will be removed.");
	}
	else if(gt.rules.mode==TERMS){
		s=tr("Are you sure you want to convert your file to the official mode? This might involve data loss."
		 " The incompatible constraints will be removed.");
	}
	else
		assert(0);
	int ok=QMessageBox::question(this, tr("FET confirmation"), s, QMessageBox::Cancel | QMessageBox::Ok);
	if(ok==QMessageBox::Cancel){
		modeOfficialAction->setChecked(false);
		return;
	}

	int removedTime=0;
	int removedSpace=0;
	bool ok2=getLastConfirmation(OFFICIAL, removedTime, removedSpace);
	if(!ok2){
		modeOfficialAction->setChecked(false);
		return;
	}

	gt.rules.setMode(OFFICIAL);
	updateMode();

	gt.rules.addUndoPoint(tr("Changed the mode to %1.").arg(tr("Official"))+QString(" ")+tr("There were removed %1 time constraints and"
	 " %2 space constraints.").arg(removedTime).arg(removedSpace));
}

void FetMainForm::modeMorningsAfternoonsAction_triggered()
{
	if(!gt.rules.initialized){
		modeOfficialAction->setChecked(currentMode==OFFICIAL);
		modeMorningsAfternoonsAction->setChecked(currentMode==MORNINGS_AFTERNOONS);
		modeBlockPlanningAction->setChecked(currentMode==BLOCK_PLANNING);
		modeTermsAction->setChecked(currentMode==TERMS);
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(gt.rules.mode==MORNINGS_AFTERNOONS){
		modeMorningsAfternoonsAction->setChecked(true);
		
		return;
	}

	QString s;
	if(gt.rules.mode==OFFICIAL){
		s=tr("Are you sure you want to convert your file to the mornings-afternoons mode? This might involve data loss."
		 " All the teachers with uninitialized mornings-afternoons behavior will have unrestricted mornings-afternoons behavior."
		 " Also, the incompatible constraints will be removed, and some constraints of type min/max days between activities might be modified.");
	}
	else if(gt.rules.mode==BLOCK_PLANNING){
		s=tr("Are you sure you want to convert your file to the mornings-afternoons mode? This might involve data loss."
		 " All the teachers with uninitialized mornings-afternoons behavior will have unrestricted mornings-afternoons behavior."
		 " Also, the incompatible constraints will be removed, and some constraints of type min/max days between activities might be modified.");
	}
	else if(gt.rules.mode==TERMS){
		s=tr("Are you sure you want to convert your file to the mornings-afternoons mode? This might involve data loss."
		 " All the teachers with uninitialized mornings-afternoons behavior will have unrestricted mornings-afternoons behavior."
		 " Also, the incompatible constraints will be removed, and some constraints of type min/max days between activities might be modified.");
	}
	else
		assert(0);
	int ok=QMessageBox::question(this, tr("FET confirmation"), s, QMessageBox::Cancel | QMessageBox::Ok);
	if(ok==QMessageBox::Cancel){
		modeMorningsAfternoonsAction->setChecked(false);
		return;
	}

	int removedTime=0;
	int removedSpace=0;
	int modifiedMinMaxDays=0;
	bool ok2=getLastConfirmation(MORNINGS_AFTERNOONS, removedTime, removedSpace, &modifiedMinMaxDays);
	if(!ok2){
		modeMorningsAfternoonsAction->setChecked(false);
		return;
	}

	gt.rules.setMode(MORNINGS_AFTERNOONS);
	updateMode();

	gt.rules.addUndoPoint(tr("Changed the mode to %1.").arg(tr("Mornings-Afternoons"))+QString(" ")+tr("There were removed %1 time constraints and"
	 " %2 space constraints.").arg(removedTime).arg(removedSpace)+QString(" ")+
	 tr("There were modified %1 time constraints of type min/max days between activities.").arg(modifiedMinMaxDays));
}

void FetMainForm::modeBlockPlanningAction_triggered()
{
	if(!gt.rules.initialized){
		modeOfficialAction->setChecked(currentMode==OFFICIAL);
		modeMorningsAfternoonsAction->setChecked(currentMode==MORNINGS_AFTERNOONS);
		modeBlockPlanningAction->setChecked(currentMode==BLOCK_PLANNING);
		modeTermsAction->setChecked(currentMode==TERMS);
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(gt.rules.mode==BLOCK_PLANNING){
		modeBlockPlanningAction->setChecked(true);
		
		return;
	}
	
	QString s;
	if(gt.rules.mode==MORNINGS_AFTERNOONS){
		s=tr("Are you sure you want to convert your file to the block-planning mode? This might involve data loss."
		 " All the teachers will have uninitialized mornings-afternoons behavior. Also, the incompatible constraints will be removed.");
	}
	else if(gt.rules.mode==OFFICIAL){
		s=tr("Are you sure you want to convert your file to the block-planning mode? This might involve data loss."
		 " The incompatible constraints will be removed.");
	}
	else if(gt.rules.mode==TERMS){
		s=tr("Are you sure you want to convert your file to the block-planning mode? This might involve data loss."
		 " The incompatible constraints will be removed.");
	}
	else
		assert(0);
	s+="\n\n";
	
	s+=tr("WARNING: In the block-planning mode, the basic time and space constraints are modified to consider that the FET hours are real-life time slots."
	  " Consequently, they are much stronger and each row (FET hour, real-life time slot) for a students subgroup / room may have a single occupied slot out of all"
	  " the FET days (real-life teachers). This might result in an impossible timetable if you intend to do usual timetabling.");

	int ok=QMessageBox::question(this, tr("FET confirmation"), s, QMessageBox::Cancel | QMessageBox::Ok);
	if(ok==QMessageBox::Cancel){
		modeBlockPlanningAction->setChecked(false);
		return;
	}

	int removedTime=0;
	int removedSpace=0;
	bool ok2=getLastConfirmation(BLOCK_PLANNING, removedTime, removedSpace);
	if(!ok2){
		modeBlockPlanningAction->setChecked(false);
		return;
	}

	gt.rules.setMode(BLOCK_PLANNING);
	updateMode();

	gt.rules.addUndoPoint(tr("Changed the mode to %1.").arg(tr("Block planning"))+QString(" ")+tr("There were removed %1 time constraints and"
	 " %2 space constraints.").arg(removedTime).arg(removedSpace));
}

void FetMainForm::modeTermsAction_triggered()
{
	if(!gt.rules.initialized){
		modeOfficialAction->setChecked(currentMode==OFFICIAL);
		modeMorningsAfternoonsAction->setChecked(currentMode==MORNINGS_AFTERNOONS);
		modeBlockPlanningAction->setChecked(currentMode==BLOCK_PLANNING);
		modeTermsAction->setChecked(currentMode==TERMS);
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(gt.rules.mode==TERMS){
		modeTermsAction->setChecked(true);
		
		return;
	}
	
	QString s;
	if(gt.rules.mode==OFFICIAL){
		s=tr("Are you sure you want to convert your file to the terms mode? This might involve data loss."
		 " The incompatible constraints will be removed.");
	}
	else if(gt.rules.mode==MORNINGS_AFTERNOONS){
		s=tr("Are you sure you want to convert your file to the terms mode? This might involve data loss."
		 " All the teachers will have uninitialized mornings-afternoons behavior. Also, the incompatible constraints will be removed.");
	}
	else if(gt.rules.mode==BLOCK_PLANNING){
		s=tr("Are you sure you want to convert your file to the terms mode? This might involve data loss."
		 " The incompatible constraints will be removed.");
	}
	else
		assert(0);
	int ok=QMessageBox::question(this, tr("FET confirmation"), s, QMessageBox::Cancel | QMessageBox::Ok);
	if(ok==QMessageBox::Cancel){
		modeTermsAction->setChecked(false);
		return;
	}

	int removedTime=0;
	int removedSpace=0;
	bool ok2=getLastConfirmation(TERMS, removedTime, removedSpace);
	if(!ok2){
		modeTermsAction->setChecked(false);
		return;
	}

	gt.rules.setMode(TERMS);
	updateMode();

	gt.rules.addUndoPoint(tr("Changed the mode to %1.").arg(tr("Terms"))+QString(" ")+tr("There were removed %1 time constraints and"
	 " %2 space constraints.").arg(removedTime).arg(removedSpace));
}

void FetMainForm::dataTermsAction_triggered()
{
	assert(gt.rules.mode==TERMS);
	
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	TermsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

bool FetMainForm::getLastConfirmation(int newMode, int &ntm, int& nsm, int* nMinMaxDaysModified)
{
	QString removedTimeConstraintsString;
	QString removedSpaceConstraintsString;
	QString modifiedMinMaxDaysString;

	QList<TimeConstraint*> removedTimeConstraintsList;
	for(TimeConstraint* tc : std::as_const(gt.rules.timeConstraintsList))
		if((newMode==OFFICIAL && !tc->canBeUsedInOfficialMode())
		 || (newMode==MORNINGS_AFTERNOONS && !tc->canBeUsedInMorningsAfternoonsMode())
		 || (newMode==BLOCK_PLANNING && !tc->canBeUsedInBlockPlanningMode())
		 || (newMode==TERMS && !tc->canBeUsedInTermsMode())){
			removedTimeConstraintsList.append(tc);
			removedTimeConstraintsString+=tc->getDetailedDescription(gt.rules)+"\n";
		}

	QList<SpaceConstraint*> removedSpaceConstraintsList;
	for(SpaceConstraint* sc : std::as_const(gt.rules.spaceConstraintsList))
		if((newMode==OFFICIAL && !sc->canBeUsedInOfficialMode())
		 || (newMode==MORNINGS_AFTERNOONS && !sc->canBeUsedInMorningsAfternoonsMode())
		 || (newMode==BLOCK_PLANNING && !sc->canBeUsedInBlockPlanningMode())
		 || (newMode==TERMS && !sc->canBeUsedInTermsMode())){
			removedSpaceConstraintsList.append(sc);
			removedSpaceConstraintsString+=sc->getDetailedDescription(gt.rules)+"\n";
		}

	QList<TimeConstraint*> modifiedTimeConstraintsList;
	if(newMode==MORNINGS_AFTERNOONS)
		for(TimeConstraint* tc : std::as_const(gt.rules.timeConstraintsList)){
			if(tc->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES){
				ConstraintMinDaysBetweenActivities* mdc=(ConstraintMinDaysBetweenActivities*)tc;
				if(mdc->minDays>=gt.rules.nDaysPerWeek/2){
					modifiedTimeConstraintsList.append(tc);
					modifiedMinMaxDaysString+=tc->getDetailedDescription(gt.rules)+"\n";
				}
			}
			else if(tc->type==CONSTRAINT_MAX_DAYS_BETWEEN_ACTIVITIES){
				ConstraintMaxDaysBetweenActivities* mdc=(ConstraintMaxDaysBetweenActivities*)tc;
				if(mdc->maxDays>=gt.rules.nDaysPerWeek/2){
					modifiedTimeConstraintsList.append(tc);
					modifiedMinMaxDaysString+=tc->getDetailedDescription(gt.rules)+"\n";
				}
			}
		}

	////////////
	//last confirmation dialog
	QDialog lastConfirmationDialog(this);
	lastConfirmationDialog.setWindowTitle(tr("Last confirmation needed"));

	QVBoxLayout* lastMainLayout=new QVBoxLayout(&lastConfirmationDialog);

	QPushButton* lastpb1=new QPushButton(tr("Cancel"));
	QPushButton* lastpb2=new QPushButton(tr("OK"));
	QHBoxLayout* lastButtons=new QHBoxLayout();
	lastButtons->addStretch();
	lastButtons->addWidget(lastpb1);
	lastButtons->addWidget(lastpb2);

	////texts
	QVBoxLayout* left=new QVBoxLayout();
	QVBoxLayout* middle=nullptr;
	if(newMode==MORNINGS_AFTERNOONS)
		middle=new QVBoxLayout();
	QVBoxLayout* right=new QVBoxLayout();
	QHBoxLayout* all=new QHBoxLayout();
	all->addLayout(left);
	if(newMode==MORNINGS_AFTERNOONS)
		all->addLayout(middle);
	all->addLayout(right);

	QPlainTextEdit* remTim=new QPlainTextEdit();
	remTim->setReadOnly(true);
	remTim->setPlainText(removedTimeConstraintsString);
	QLabel* labRemTim=new QLabel(tr("These %1 time constraints will be removed:").arg(removedTimeConstraintsList.count()));
	labRemTim->setWordWrap(true);
	labRemTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labRemTim);
	left->addWidget(remTim);

	if(newMode==MORNINGS_AFTERNOONS){
		QPlainTextEdit* modTim=new QPlainTextEdit();
		modTim->setReadOnly(true);
		modTim->setPlainText(modifiedMinMaxDaysString);
		QLabel* labModTim=new QLabel(tr("These %1 time constraints will be modified:").arg(modifiedTimeConstraintsList.count()));
		labModTim->setWordWrap(true);
		labModTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
		middle->addWidget(labModTim);
		middle->addWidget(modTim);
	}

	QPlainTextEdit* remSpa=new QPlainTextEdit();
	remSpa->setReadOnly(true);
	remSpa->setPlainText(removedSpaceConstraintsString);
	QLabel* labRemSpa=new QLabel(tr("These %1 space constraints will be removed:").arg(removedSpaceConstraintsList.count()));
	labRemSpa->setWordWrap(true);
	labRemSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labRemSpa);
	right->addWidget(remSpa);
	////end texts

	////////
	lastMainLayout->addLayout(all);
	//lastMainLayout->addStretch();
	lastMainLayout->addLayout(lastButtons);

	connect(lastpb2, &QPushButton::clicked, &lastConfirmationDialog, &QDialog::accept);
	connect(lastpb1, &QPushButton::clicked, &lastConfirmationDialog, &QDialog::reject);

	lastpb2->setDefault(true);
	lastpb2->setFocus();

	int lw=lastConfirmationDialog.sizeHint().width();
	int lh=lastConfirmationDialog.sizeHint().height();
	lastConfirmationDialog.resize(lw,lh);
	
	QString newModeString;
	if(newMode==OFFICIAL)
		newModeString=QString("Official");
	else if(newMode==MORNINGS_AFTERNOONS)
		newModeString=QString("MorningsAfternoons");
	else if(newMode==BLOCK_PLANNING)
		newModeString=QString("BlockPlanning");
	else if(newMode==TERMS)
		newModeString=QString("Terms");
	else
		assert(0);
	
	centerWidgetOnScreen(&lastConfirmationDialog);
	restoreFETDialogGeometry(&lastConfirmationDialog, QString("LastConfirmationChangeMode")+newModeString);
	int ok=lastConfirmationDialog.exec();
	saveFETDialogGeometry(&lastConfirmationDialog, QString("LastConfirmationChangeMode")+newModeString);
	if(ok==QDialog::Rejected)
		return false;
	assert(ok==QDialog::Accepted);
	////////////

	int removedTime=removedTimeConstraintsList.count();
	int removedSpace=removedSpaceConstraintsList.count();
	if(nMinMaxDaysModified!=nullptr)
		*nMinMaxDaysModified=modifiedTimeConstraintsList.count();

	bool t=gt.rules.removeTimeConstraints(removedTimeConstraintsList);
	assert(t);

	removedTimeConstraintsList.clear();

	t=gt.rules.removeSpaceConstraints(removedSpaceConstraintsList);
	assert(t);

	removedSpaceConstraintsList.clear();

	for(TimeConstraint* tc : std::as_const(modifiedTimeConstraintsList)){
		if(tc->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES){
			ConstraintMinDaysBetweenActivities* mdc=(ConstraintMinDaysBetweenActivities*)tc;
			if(mdc->minDays>=gt.rules.nDaysPerWeek/2)
				mdc->minDays=gt.rules.nDaysPerWeek/2-1;
		}
		else if(tc->type==CONSTRAINT_MAX_DAYS_BETWEEN_ACTIVITIES){
			ConstraintMaxDaysBetweenActivities* mdc=(ConstraintMaxDaysBetweenActivities*)tc;
			if(mdc->maxDays>=gt.rules.nDaysPerWeek/2)
				mdc->maxDays=gt.rules.nDaysPerWeek/2-1;
		}
	}

	if(nMinMaxDaysModified!=nullptr)
		QMessageBox::information(&lastConfirmationDialog, tr("FET information"), tr("There were removed %1 time constraints and"
		 " %2 space constraints.").arg(removedTime).arg(removedSpace)+QString(" ")+
		 tr("There were modified %1 time constraints of type min/max days between activities.").arg(*nMinMaxDaysModified));
	else
		QMessageBox::information(&lastConfirmationDialog, tr("FET information"), tr("There were removed %1 time constraints and"
		 " %2 space constraints.").arg(removedTime).arg(removedSpace));

	QString ms;
	if(newMode==OFFICIAL)
		ms=tr("Official");
	else if(newMode==MORNINGS_AFTERNOONS)
		ms=tr("Mornings-Afternoons");
	else if(newMode==BLOCK_PLANNING)
		ms=tr("Block planning");
	else if(newMode==TERMS)
		ms=tr("Terms");
	
	ntm=removedTime;
	nsm=removedSpace;
	
	//gt.rules.addUndoPoint(tr("Changed the mode to %1.").arg(ms)+QString(" ")+tr("There were removed %1 time constraints and"
	// " %2 space constraints.").arg(removedTime).arg(removedSpace));

	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	LockUnlock::increaseCommunicationSpinBox();

	return true;
}

/////////confirmations
void FetMainForm::settingsConfirmActivityPlanningAction_toggled()
{
	CONFIRM_ACTIVITY_PLANNING=settingsConfirmActivityPlanningAction->isChecked();
}

void FetMainForm::settingsConfirmSpreadActivitiesAction_toggled()
{
	CONFIRM_SPREAD_ACTIVITIES=settingsConfirmSpreadActivitiesAction->isChecked();
}

void FetMainForm::settingsConfirmRemoveRedundantAction_toggled()
{
	CONFIRM_REMOVE_REDUNDANT=settingsConfirmRemoveRedundantAction->isChecked();
}

void FetMainForm::settingsConfirmSaveTimetableAction_toggled()
{
	CONFIRM_SAVE_TIMETABLE=settingsConfirmSaveTimetableAction->isChecked();
}

void FetMainForm::settingsConfirmActivateDeactivateActivitiesConstraintsAction_toggled()
{
	CONFIRM_ACTIVATE_DEACTIVATE_ACTIVITIES_CONSTRAINTS=settingsConfirmActivateDeactivateActivitiesConstraintsAction->isChecked();
}

/////////

void FetMainForm::settingsShowShortcutsOnMainWindowAction_toggled()
{
	SHOW_SHORTCUTS_ON_MAIN_WINDOW=settingsShowShortcutsOnMainWindowAction->isChecked();
	tabWidget->setVisible(SHOW_SHORTCUTS_ON_MAIN_WINDOW);
}

void FetMainForm::settingsShowToolTipsForConstraintsWithTablesAction_toggled()
{
	SHOW_TOOLTIPS_FOR_CONSTRAINTS_WITH_TABLES=settingsShowToolTipsForConstraintsWithTablesAction->isChecked();
}

void FetMainForm::settingsDivideTimetablesByDaysAction_toggled()
{
	DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS=settingsDivideTimetablesByDaysAction->isChecked();
}

void FetMainForm::settingsDuplicateVerticalNamesAction_toggled()
{
	TIMETABLE_HTML_REPEAT_NAMES=settingsDuplicateVerticalNamesAction->isChecked();
}

void FetMainForm::timetablesToWriteOnDiskAction_triggered()
{
	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	TimetablesToWriteOnDiskForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::studentsComboBoxesStyleAction_triggered()
{
	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	StudentsComboBoxesStyleForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::replyFinished(QNetworkReply* networkReply)
{
	if(networkReply->error()!=QNetworkReply::NoError){
		QString s=QString("");
		s+=tr("Could not search for possible updates on the internet - the error message is: %1.").arg(networkReply->errorString());
		s+=QString("\n\n");
		s+=tr("Searching for file %1.").arg("https://lalescu.ro/liviu/fet/crtversion/crtversion.txt");
		s+=QString("\n\n");
		s+=tr("Possible actions: check your network connection, try again later, try to visit FET homepage: %1, or"
		 " try to search for the new FET page on the internet (maybe it has changed).").arg("https://lalescu.ro/liviu/fet/");

		QMessageBox::warning(this, tr("FET warning"), s);
	}
	else{
		QString internetVersion;
		QString additionalComments;
		
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
		QRegularExpression regExp("^\\s*(\\S+)(.*)$");
		QRegularExpressionMatch match=regExp.match(QString(networkReply->readAll()));
		if(!match.hasMatch()){
			QString s=QString("");
			s+=tr("The file %1 from the FET homepage, indicating the current FET version, is incorrect.").arg("https://lalescu.ro/liviu/fet/crtversion/crtversion.txt");
			s+=QString("\n\n");
			s+=tr("Maybe the FET homepage has some temporary problems, so try again later."
			 " Or maybe the current structure on FET homepage was changed. You may visit FET homepage: %1, and get latest version or,"
			 " if it does not work, try to search for the new FET page on the internet (maybe it has changed).")
			  .arg("https://lalescu.ro/liviu/fet/");

			QMessageBox::warning(this, tr("FET warning"), s);
		}
		else{
			internetVersion=match.captured(1);
			additionalComments=match.captured(2).trimmed();
#else
		QRegExp regExp("^\\s*(\\S+)(.*)$");
		int t=regExp.indexIn(QString(networkReply->readAll()));
		if(t!=0){
			QString s=QString("");
			s+=tr("The file %1 from the FET homepage, indicating the current FET version, is incorrect.").arg("https://lalescu.ro/liviu/fet/crtversion/crtversion.txt");
			s+=QString("\n\n");
			s+=tr("Maybe the FET homepage has some temporary problems, so try again later."
			 " Or maybe the current structure on FET homepage was changed. You may visit FET homepage: %1, and get latest version or,"
			 " if it does not work, try to search for the new FET page on the internet (maybe it has changed).")
			  .arg("https://lalescu.ro/liviu/fet/");

			QMessageBox::warning(this, tr("FET warning"), s);
		}
		else{
			internetVersion=regExp.cap(1);
			additionalComments=regExp.cap(2).trimmed();
#endif

			if(VERBOSE){
				std::cout<<"Your current version: '";
				std::cout<<qPrintable(FET_VERSION)<<"'"<<std::endl;
				std::cout<<"Latest version: '";
				std::cout<<qPrintable(internetVersion)<<"'"<<std::endl;
			}
			if(internetVersion!=FET_VERSION){
				QString s=tr("Another version: %1, is available on the FET homepage: %2", "%1 is new version, %2 is FET homepage").arg(internetVersion).arg("https://lalescu.ro/liviu/fet/");
				s+=QString("\n\n");
				s+=tr("You have to manually download and install.")+QString(" ")+tr("You may need to hit Refresh in your web browser.")+QString("\n\n")+tr("Would you like to open the FET homepage now?");
				if(!additionalComments.isEmpty()){
					s+=QString("\n\n");
					s+=tr("Additional comments: %1").arg(additionalComments);
				}
			
				QMessageBox::StandardButton button=QMessageBox::information(this, tr("FET information"), s, QMessageBox::Yes|QMessageBox::No);
				
				if(button==QMessageBox::Yes){
					bool tds=QDesktopServices::openUrl(QUrl("https://lalescu.ro/liviu/fet/"));
					if(!tds){
						QMessageBox::warning(this, tr("FET warning"), tr("Could not start the default internet browser (trying to open the link %1)."
							" Maybe you can try to manually start your browser and open this link.").arg("https://lalescu.ro/liviu/fet/"));
					}
				}
			}
		}
	}

	networkReply->deleteLater();
}

void FetMainForm::closeOtherWindows()
{
	QList<QWidget*> tlwl=qApp->topLevelWidgets();
	
	for(QWidget* wi : std::as_const(tlwl))
		if(wi->isVisible() && wi!=this)
			wi->close();
}

void FetMainForm::closeEvent(QCloseEvent* event)
{
	//QRect rect(x(), y(), width(), height());
	QRect rect=this->geometry();
	mainFormSettingsRect=rect;
	
	MAIN_FORM_SHORTCUTS_TAB_POSITION=tabWidget->currentIndex();
	
	if(gt.rules.initialized && gt.rules.modified){
		QMessageBox::StandardButton res=QMessageBox::question( this, tr("FET question"),
		 tr("Your data file has been modified - do you want to save it?"), QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);

		if(res==QMessageBox::Yes){
			bool t=this->fileSave();
			if(!t){
				event->ignore();
			}
			else{
				closeOtherWindows();
				event->accept();
			}
		}
		else if(res==QMessageBox::No){
			closeOtherWindows();
			event->accept();
		}
		else{
			assert(res==QMessageBox::Cancel);
			event->ignore();
		}
	}
	else{
		closeOtherWindows();
		event->accept();
	}
}

FetMainForm::~FetMainForm()
{
	QSettings settings(COMPANY, PROGRAM);
	
	settings.setValue(QString("enable-data-states-recording"), USE_UNDO_REDO);
	settings.setValue(QString("number-of-data-steps-to-record"), UNDO_REDO_STEPS);
	//settings.setValue(QString("data-states-compression-level"), UNDO_REDO_COMPRESSION_LEVEL);

	settings.setValue(QString("enable-data-states-recording-on-disk"), USE_UNDO_REDO_SAVE);
	settings.setValue(QString("number-of-data-steps-to-record-on-disk"), UNDO_REDO_STEPS_SAVE);
	settings.setValue(QString("filename-suffix-save-history"), SUFFIX_FILENAME_SAVE_HISTORY);

	settings.setValue(QString("enable-file-autosave"), USE_AUTOSAVE);
	settings.setValue(QString("minutes-for-autosave"), MINUTES_AUTOSAVE);
	settings.setValue(QString("operations-for-autosave"), OPERATIONS_AUTOSAVE);
	settings.setValue(QString("directory-for-autosave"), DIRECTORY_AUTOSAVE);
	settings.setValue(QString("filename-suffix-for-autosave"), SUFFIX_FILENAME_AUTOSAVE);

	QFont interfaceFont=qApp->font();
	settings.setValue(QString("font-is-user-selectable"), fontIsUserSelectable);
	if(fontIsUserSelectable && userChoseAFont)
		settings.setValue(QString("font"), interfaceFont.toString());
	else
		settings.setValue(QString("font"), QString(""));
	
	settings.setValue(QString("FetMainForm/number-of-recent-files"), recentFiles.count());
	settings.remove(QString("FetMainForm/recent-file"));
	for(int i=0; i<recentFiles.count(); i++)
		settings.setValue(QString("FetMainForm/recent-file/")+CustomFETString::number(i+1), recentFiles.at(i));

	shortcutBasicMenu->clear();
	delete shortcutBasicMenu;

	shortcutAdvancedTimeMenu->clear();
	delete shortcutAdvancedTimeMenu;

	shortcutDataSpaceMenu->clear();
	delete shortcutDataSpaceMenu;

	shortcutDataAdvancedMenu->clear();
	delete shortcutDataAdvancedMenu;
	
	//2014-07-01
	shortcutTimetableLockingMenu->clear();
	delete shortcutTimetableLockingMenu;
	
	shortcutTimetableAdvancedMenu->clear();
	delete shortcutTimetableAdvancedMenu;
}

void FetMainForm::fileQuitAction_triggered()
{
	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	close();
}

QString FetMainForm::strippedName(const QString& fullFileName)
{
	return QFileInfo(fullFileName).fileName();
}

void FetMainForm::setCurrentFile(const QString& fileName)
{
	QString currentFile=fileName;
	QString shownName=QString("Untitled");
	
	if(!currentFile.isEmpty()){
		shownName=strippedName(currentFile);
		recentFiles.removeAll(currentFile);
		recentFiles.prepend(currentFile);
		if(recentFiles.count()>MAX_RECENT_FILES){
			assert(recentFiles.count()==MAX_RECENT_FILES+1);
			assert(!recentFiles.isEmpty());
			recentFiles.removeLast();
		}
		updateRecentFileActions();
	}
	
	setWindowTitle(tr("%1[*] - FET", "The title of the main window, %1 is the name of the current file. "
	 "Please keep the string [*] unmodified (three characters) - it is used to make the difference between modified files and unmodified files.")
	 .arg(shownName));
}

void FetMainForm::updateRecentFileActions()
{
	QStringList existingNames;
	for(const QString& tn : std::as_const(recentFiles))
		if(QFile::exists(tn))
			existingNames.append(tn);
	recentFiles=existingNames;
	
	for(int j=0; j<MAX_RECENT_FILES; j++){
		if(j<recentFiles.count()){
			QString text=strippedName(recentFiles[j]);
			recentFileActions[j]->setText(text);
			recentFileActions[j]->setData(recentFiles[j]);
			recentFileActions[j]->setVisible(true);
		}
		else{
			recentFileActions[j]->setVisible(false);
		}
	}
	
	recentSeparatorAction->setVisible(!recentFiles.isEmpty());
}

void FetMainForm::fileClearRecentFilesListAction_triggered()
{
	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	recentFiles.clear();
	updateRecentFileActions();
}

void FetMainForm::fileNewAction_triggered()
{
	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	bool confirm=false;
	
	if(gt.rules.initialized && gt.rules.modified){
		switch( QMessageBox::question(
		 this,
		 tr("FET application"),
		 tr("Your current data file has been modified. Are you sure you want to reset to new empty data?"),
		 QMessageBox::Yes|QMessageBox::No
		 ) ){
		case QMessageBox::Yes: // Yes
			confirm=true;
			break;
		case QMessageBox::No: // No
			confirm=false;
			break;
		default:
			assert(0);
			break;
		}
	}
	else
		confirm=true;
	
	int tm=-1;
	if(confirm){
		GetModeForNewFileForm form(this);
		int result=form.exec();
		if(result==QDialog::Accepted){
			assert(form.mode==OFFICIAL || form.mode==MORNINGS_AFTERNOONS || form.mode==BLOCK_PLANNING || form.mode==TERMS);
			tm=form.mode;
		}
		else{
			confirm=false;
		}
	}

	if(confirm){
		INPUT_FILENAME_XML=QString("");
		setCurrentFile(INPUT_FILENAME_XML);
	
		if(gt.rules.initialized)
			gt.rules.clear();
		gt.rules.init();
		
		gt.rules.mode=tm;
		
		gt.rules.modified=true; //to avoid the flickering of the main form modified flag

		bool tmp=gt.rules.addTimeConstraint(new ConstraintBasicCompulsoryTime(100));
		assert(tmp);
		tmp=gt.rules.addSpaceConstraint(new ConstraintBasicCompulsorySpace(100));
		assert(tmp);
		
		QString ms;
		if(gt.rules.mode==OFFICIAL)
			ms=tr("Official");
		else if(gt.rules.mode==MORNINGS_AFTERNOONS)
			ms=tr("Mornings-Afternoons");
		else if(gt.rules.mode==BLOCK_PLANNING)
			ms=tr("Block planning");
		else if(gt.rules.mode==TERMS)
			ms=tr("Terms");
		clearHistory();
		gt.rules.addUndoPoint(tr("Created a new file with the mode %1.").arg(ms), false, true);
		savedStateIterator=cntUndoRedoStackIterator;
		
		gt.rules.modified=true; //force update of the modified flag of the main window
		setRulesUnmodifiedAndOtherThings(&gt.rules);

		teachers_schedule_ready=false;
		students_schedule_ready=false;
		rooms_buildings_schedule_ready=false;

		LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
		LockUnlock::increaseCommunicationSpinBox();

		statusBar()->showMessage(tr("New file generated"), STATUS_BAR_MILLISECONDS);
		oldDataAvailable=dataAvailable;
		dataAvailable=true;
		updateMode();
	}
}

void FetMainForm::openRecentFile()
{
	QAction* action=qobject_cast<QAction*>(sender());
	if(action)
		openFile(action->data().toString());
}

void FetMainForm::fileOpenAction_triggered()
{
	openFile(QString());
}

void FetMainForm::openFile(const QString& fileName)
{
	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	bool confirm=false;
	
	if(gt.rules.initialized && gt.rules.modified){
		switch( QMessageBox::question(
		 this,
		 tr("FET application"),
		 tr("Your current data file has been modified. Are you sure you want to open another data file?"),
		 QMessageBox::Yes|QMessageBox::No
		 ) ){
		case QMessageBox::Yes: // Yes
			confirm=true;
			break;
		case QMessageBox::No: // No
			confirm=false;
			break;
		default:
			assert(0);
			break;
		}
	}
	else
		confirm=true;
	
	if(confirm){
		QString s=fileName;
		
		if(s.isNull()){
			s = QFileDialog::getOpenFileName(this, tr("Choose a file to open"),
			 WORKING_DIRECTORY,
			 tr("FET XML files", "Instructions for translators: FET XML is a type of file format (using text mode). "
			 "So this field means files in the FET XML format")+" (*.fet)"+";;"+tr("All files")+" (*)");

			if(s.isNull())
				return;
		}

		int tmp2=s.lastIndexOf(FILE_SEP);
		QString s2=s.right(s.length()-tmp2-1);
		
		if(s2.length()>=1){
			if(s2.at(0).isSpace()){
				QMessageBox::warning(this, tr("FET information"),
				 tr("Please do not use a filename starting with white space(s), the HTML CSS code does not work."
				  " File was not loaded. Please rename it, removing the white space(s) from the beginning and open it after that with FET."));
				return;
			}
		}
		QString s3;
		if(s2.endsWith(".fet"))
			s3=s2.left(s2.length()-4);
		else
			s3=s2;
		if(s3.length()>=1){
			if(s3.at(s3.length()-1).isSpace()){
				QMessageBox::warning(this, tr("FET information"),
				 tr("Please do not use a filename ending with white space(s) before the '.fet' termination or at the end of the filename,"
				 " problems might arise. File was not loaded. Please rename it, removing the white space(s) at the end and open it after"
				 " that with FET."));
				return;
			}
		}
		if(s2.indexOf("\"") >= 0){
			QMessageBox::warning(this, tr("FET information"),
			 tr("Please do not use quotation marks \" in filename, the HTML CSS code does not work."
			  " File was not loaded. Please rename it, removing not allowed characters and open it after that with FET."));
			return;
		}
		if(s2.indexOf(";") >= 0){
			QMessageBox::warning(this, tr("FET information"),
			 tr("Please do not use semicolon ; in filename, the HTML CSS code does not work."
			  " File was not loaded. Please rename it, removing not allowed characters and open it after that with FET."));
			return;
		}
		if(s2.indexOf("#") >= 0){
			QMessageBox::warning(this, tr("FET information"),
			 tr("Please do not use # in filename, the HTML CSS code does not work."
			  " File was not loaded. Please rename it, removing not allowed characters and open it after that with FET."));
			return;
		}
		/*if(s2.indexOf("(") >= 0 || s2.indexOf(")")>=0){
			QMessageBox::information(this, tr("FET information"), tr("Please do not use parentheses () in filename, the HTML CSS code does not work"));
			return;
		}*/
		else{
			//QCursor orig=this->cursor();
			//this->setCursor(Qt::WaitCursor);
			statusBar()->showMessage(tr("Loading...", "This is a message in the status bar, that we are loading the file"), 0);
			//pqapplication->processEvents();
		
			//bool before=gt.rules.modified;
			gt.rules.modified=true; //to avoid the flickering of the main form modified flag

			int oldMode=gt.rules.mode;
			
			if(gt.rules.read(this, s)){
				teachers_schedule_ready=false;
				students_schedule_ready=false;
				rooms_buildings_schedule_ready=false;

				INPUT_FILENAME_XML = s;
				
				clearHistory();
				gt.rules.addUndoPoint(tr("Opened the file %1").arg(QDir::toNativeSeparators(s)), false, true);
				savedStateIterator=cntUndoRedoStackIterator;

				openHistory();
				
				LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
				LockUnlock::increaseCommunicationSpinBox();

				statusBar()->showMessage(tr("File opened"), STATUS_BAR_MILLISECONDS);
				oldDataAvailable=dataAvailable;
				dataAvailable=true;
				updateMode();
				
				gt.rules.modified=true; //force update of the modified flag of the main window
				setRulesUnmodifiedAndOtherThings(&gt.rules);
				
				setCurrentFile(INPUT_FILENAME_XML);
			}
			else{
				QString s2=tr("Critical error: Your data file could not be opened correctly/completely. Would you like to keep the partial data that might "
				 "have been read correctly?");
				s2+=QString(" ");
				s2+=tr("This might help you recover at least some of the inputted data.");
				s2+=QString(" ");
				s2+=tr("(In case of further errors, you can always choose to create a new file, from the FET menu.)");
				s2+=QString("\n\n");
				s2+=tr("Note: If you enabled autosave, you could try to open the filename_AUTOSAVE.fet file.", "Keep the words 'filename_AUTOSAVE.fet' untranslated");
				s2+=QString("\n\n");
				s2+=tr("Note: If you enabled save/restore history to/from disk, you might have success, after accepting this partial data, to restore your data "
				 "to a correct and complete previous state, recorded in the history.");
				
				QMessageBox::StandardButton b=QMessageBox::critical(this, tr("FET critical"), s2, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
				if(b==QMessageBox::Yes){
					teachers_schedule_ready=false;
					students_schedule_ready=false;
					rooms_buildings_schedule_ready=false;
					
					clearHistory();
					INPUT_FILENAME_XML=s;
					openHistory();
					INPUT_FILENAME_XML=QString("");
					gt.rules.addUndoPoint(tr("Opened the file %1, incorrectly/incompletely").arg(QDir::toNativeSeparators(s)), false, true);
					savedStateIterator=cntUndoRedoStackIterator;
					
					LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
					LockUnlock::increaseCommunicationSpinBox();

					statusBar()->showMessage(tr("File opened incorrectly/incompletely"), STATUS_BAR_MILLISECONDS);
					oldDataAvailable=dataAvailable;
					dataAvailable=true;
					updateMode();
					
					gt.rules.modified=true; //force update of the modified flag of the main window
					setRulesUnmodifiedAndOtherThings(&gt.rules);
					
					setCurrentFile(INPUT_FILENAME_XML);
				}
				else{
					assert(b==QMessageBox::No);
					
					teachers_schedule_ready=false;
					students_schedule_ready=false;
					rooms_buildings_schedule_ready=false;
					
					gt.rules.mode=oldMode;
					
					//incorrect code - the old file may be broken - so we generate a new file.
					/*gt.rules.modified=before;
					
					statusBar()->showMessage("", STATUS_BAR_MILLISECONDS);
					
					setCurrentFile(INPUT_FILENAME_XML);*/

					//not needed, because if the file cannot be read we keep the old mode
					//updateMode();
					
					assert(!generation_running);
					//gt.rules.modified=false;
					statusBar()->showMessage(tr("Loading file failed...", "This is a message in the status bar, that opening the chosen file failed"), STATUS_BAR_MILLISECONDS);
					
					//fileNewAction_triggered();
					
					//new void data
					if(gt.rules.initialized)
						gt.rules.clear();
					
					dataAvailable=false;
					updateMode(true); //true means force

					INPUT_FILENAME_XML=QString("");
					setCurrentFile(INPUT_FILENAME_XML);

					gt.rules.modified=true; //force update of the modified flag of the main window
					setRulesUnmodifiedAndOtherThings(&gt.rules);

					assert(!gt.rules.initialized);
				}
			}
			
			//this->setCursor(orig);
		}
		//get the directory
		int tmp=s.lastIndexOf(FILE_SEP);
		WORKING_DIRECTORY=s.left(tmp);
	}
}

bool FetMainForm::fileSaveAs()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return false;
	}

	QString predefFileName=INPUT_FILENAME_XML;
	if(predefFileName.isEmpty())
		predefFileName=WORKING_DIRECTORY+FILE_SEP+QString("untitled.fet");

	QString s = QFileDialog::getSaveFileName(this, tr("Choose a filename to save under"),
		predefFileName, tr("FET XML files", "Instructions for translators: FET XML is a type of file format (using text mode). "
		"So this field means files in the FET XML format")+" (*.fet)"+";;"+tr("All files")+" (*)",
		0, QFileDialog::DontConfirmOverwrite);
	if(s==QString())
		return false;

	int tmp2=s.lastIndexOf(FILE_SEP);
	QString s2=s.right(s.length()-tmp2-1);
	
	if(s2.length()>=1){
		if(s2.at(0).isSpace()){
			QMessageBox::warning(this, tr("FET information"),
			 tr("Please do not use a filename starting with white space(s), the HTML CSS code does not work.")
			 +"\n\n"+tr("File was not saved."));
			return false;
		}
	}
	QString s3;
	bool ewf;
	if(s2.endsWith(".fet")){
		s3=s2.left(s2.length()-4);
		ewf=true;
	}
	else{
		s3=s2;
		ewf=false;
	}
	if(s3.length()>=1){
		if(s3.at(s3.length()-1).isSpace()){
			if(ewf)
				QMessageBox::warning(this, tr("FET information"),
				 tr("Please do not use a filename ending with white space(s) before the '.fet' termination, problems might arise.")
				 +"\n\n"+tr("File was not saved."));
			else
				QMessageBox::warning(this, tr("FET information"),
				 tr("Please do not use a filename ending with white space(s), problems might arise.")
				 +"\n\n"+tr("File was not saved."));
			return false;
		}
	}
	if(s2.indexOf("\"") >= 0){
		QMessageBox::warning(this, tr("FET information"), tr("Please do not use quotation marks \" in filename, the HTML CSS code does not work")
		 +"\n\n"+tr("File was not saved."));
		return false;
	}
	if(s2.indexOf(";") >= 0){
		QMessageBox::warning(this, tr("FET information"), tr("Please do not use semicolon ; in filename, the HTML CSS code does not work")
		 +"\n\n"+tr("File was not saved."));
		return false;
	}
	if(s2.indexOf("#") >= 0){
		QMessageBox::warning(this, tr("FET information"), tr("Please do not use # in filename, the HTML CSS code does not work")
		 +"\n\n"+tr("File was not saved."));
		return false;
	}
	
	if(s.right(4)!=".fet")
		s+=".fet";

	int tmp=s.lastIndexOf(FILE_SEP);
	WORKING_DIRECTORY=s.left(tmp);

	if(QFile::exists(s))
		if(QMessageBox::warning( this, tr("FET"),
		 tr("File %1 exists - are you sure you want to overwrite it?").arg(s),
		 QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)
		 	return false;
			
	bool t=gt.rules.write(this, s);
	if(t){
		INPUT_FILENAME_XML = s;
		
		//gt.rules.addUndoPoint(tr("Saved the file as %1.").arg(QDir::toNativeSeparators(INPUT_FILENAME_XML)));
		savedStateIterator=cntUndoRedoStackIterator;

		saveHistory();
	
		gt.rules.modified=true; //force update of the modified flag of the main window
		setRulesUnmodifiedAndOtherThings(&gt.rules);
	
		setCurrentFile(INPUT_FILENAME_XML);
	
		statusBar()->showMessage(tr("File saved"), STATUS_BAR_MILLISECONDS);
		
		return true;
	}
	else{
		return false;
	}
}

void FetMainForm::fileSaveAsAction_triggered()
{
	fileSaveAs();
}

void FetMainForm::settingsHistoryMemoryAction_triggered()
{
	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	SettingsRestoreDataFromMemoryForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::settingsHistoryDiskAction_triggered()
{
	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	if(!USE_UNDO_REDO){
		QMessageBox::information(this, tr("FET information"),
			tr("Saving/restoring history to/from the memory is disabled. Please enable memory history before enabling disk history.",
			 "Memory history is the history saved in the computer memory, and disk history is the history saved on the computer disk."));
		return;
	}

	SettingsRestoreDataFromDiskForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::restoreDataStateAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing the history."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	if(!USE_UNDO_REDO){
		QMessageBox::information(this, tr("FET information"),
			tr("History saving and restoring is disabled from the History settings. Please enable it to proceed."));
		return;
	}

	RestoreDataStateForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

// Start of code contributed by Volker Dirr
void FetMainForm::fileImportCSVRoomsBuildingsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}
	Import::importCSVRoomsAndBuildings(this);
}

void FetMainForm::fileImportCSVSubjectsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}
	Import::importCSVSubjects(this);
}

void FetMainForm::fileImportCSVTeachersAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}
	Import::importCSVTeachers(this);
}

void FetMainForm::fileImportCSVActivitiesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}
	Import::importCSVActivities(this);

	//TODO: if the import takes care of locked activities, then we need
	//to do:
	//LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	//LockUnlock::increaseCommunicationSpinBox();
	//after the importing
}

void FetMainForm::fileImportCSVActivityTagsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}
	Import::importCSVActivityTags(this);
}

void FetMainForm::fileImportCSVYearsGroupsSubgroupsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}
	Import::importCSVStudents(this);
}

void FetMainForm::fileExportCSVAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}
	Export::exportCSV(this);
}
// End of code contributed by Volker Dirr

void FetMainForm::timetableSaveTimetableAsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!students_schedule_ready || !teachers_schedule_ready || !rooms_buildings_schedule_ready){
		QMessageBox::warning(this, tr("FET - Warning"), tr("You have not yet generated a timetable - please generate firstly"));
		return;
	}

	Solution* tc=&best_solution;
	
	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		//Activity* act=&gt.rules.internalActivitiesList[ai];
		int time=tc->times[ai];
		if(time==UNALLOCATED_TIME){
			QMessageBox::warning(this, tr("FET - Warning"), tr("It seems that you have an incomplete timetable."
			 " Saving of timetable does not work for incomplete timetables. Please generate a complete timetable"));
			 //.arg(act->id));
			return;
		}
		
		int ri=tc->rooms[ai];
		if(ri==UNALLOCATED_SPACE){
			QMessageBox::warning(this, tr("FET - Warning"), tr("It seems that you have an incomplete timetable."
			 " Saving of timetable does not work for incomplete timetables. Please generate a complete timetable"));
			 //.arg(act->id));
			return;
		}
	}

	bool ok_to_continue;
	SaveTimetableConfirmationForm* pc_form=nullptr;
	if(CONFIRM_SAVE_TIMETABLE){
		int confirm;
		
		pc_form=new SaveTimetableConfirmationForm(this);
		setParentAndOtherThings(pc_form, this);
		confirm=pc_form->exec();
		
		if(confirm==QDialog::Accepted){
			if(pc_form->dontShowAgain)
				settingsConfirmSaveTimetableAction->setChecked(false);
			
			ok_to_continue=true;
		}
		else
			ok_to_continue=false;
	}
	else
		ok_to_continue=true;
		
	if(ok_to_continue){
		QWidget* parent=pc_form;
		if(parent==nullptr)
			parent=this;

		QString s;

		for(;;){
			s = QFileDialog::getSaveFileName(parent, tr("Choose a filename for data and timetable"),
				INPUT_FILENAME_XML, tr("FET XML files", "Instructions for translators: FET XML is a type of file format (using text mode). "
				"So this field means files in the FET XML format")+" (*.fet)"+";;"+tr("All files")+" (*)",
				0, QFileDialog::DontConfirmOverwrite);
			if(s==QString())
				return;

			int tmp2=s.lastIndexOf(FILE_SEP);
			QString s2=s.right(s.length()-tmp2-1);
				
			if(s2.length()>=1){
				if(s2.at(0).isSpace()){
					QMessageBox::warning(this, tr("FET information"),
					 tr("Please do not use a filename starting with white space(s), the HTML CSS code does not work.")
					 +"\n\n"+tr("File was not saved."));
					return;
				}
			}
			QString s3;
			bool ewf;
			if(s2.endsWith(".fet")){
				s3=s2.left(s2.length()-4);
				ewf=true;
			}
			else{
				s3=s2;
				ewf=false;
			}
			if(s3.length()>=1){
				if(s3.at(s3.length()-1).isSpace()){
					if(ewf)
						QMessageBox::warning(this, tr("FET information"),
						 tr("Please do not use a filename ending with white space(s) before the '.fet' termination, problems might arise.")
						 +"\n\n"+tr("File was not saved."));
					else
						QMessageBox::warning(this, tr("FET information"),
						 tr("Please do not use a filename ending with white space(s), problems might arise.")
						 +"\n\n"+tr("File was not saved."));
					return;
				}
			}
			if(s2.indexOf("\"") >= 0){
				QMessageBox::warning(parent, tr("FET information"), tr("Please do not use quotation marks \" in filename, the HTML CSS code does not work")
				 +"\n\n"+tr("File was not saved."));
				return;
			}
			if(s2.indexOf(";") >= 0){
				QMessageBox::warning(parent, tr("FET information"), tr("Please do not use semicolon ; in filename, the HTML CSS code does not work")
				 +"\n\n"+tr("File was not saved."));
				return;
			}
			if(s2.indexOf("#") >= 0){
				QMessageBox::warning(parent, tr("FET information"), tr("Please do not use # in filename, the HTML CSS code does not work")
				 +"\n\n"+tr("File was not saved."));
				return;
			}
			
			if(s.right(4)!=".fet")
				s+=".fet";

			int tmp=s.lastIndexOf(FILE_SEP);
			WORKING_DIRECTORY=s.left(tmp);

			if(QFile::exists(s)){
				QString t=tr("File exists");
				t+="\n\n";
				t+=tr("For safety (so you don't lose work), it is not allowed to overwrite an existing file with"
					" locking and saving a current data+timetable");
				t+="\n\n";
				t+=tr("Please choose a non-existing name");
		
				QMessageBox::warning( parent, tr("FET warning"), t);
			}
			else
				break;
		}
		
		rules2.initialized=true;
		
		rules2.mode=gt.rules.mode;
		
		rules2.institutionName=gt.rules.institutionName;
		rules2.comments=gt.rules.comments;
		
		rules2.nTerms=gt.rules.nTerms;
		rules2.nDaysPerTerm=gt.rules.nDaysPerTerm;
		
		rules2.nHoursPerDay=gt.rules.nHoursPerDay;
		rules2.hoursOfTheDay=gt.rules.hoursOfTheDay;
		//for(int i=0; i<gt.rules.nHoursPerDay; i++)
		//	rules2.hoursOfTheDay[i]=gt.rules.hoursOfTheDay[i];

		rules2.nDaysPerWeek=gt.rules.nDaysPerWeek;
		rules2.daysOfTheWeek=gt.rules.daysOfTheWeek;
		//for(int i=0; i<gt.rules.nDaysPerWeek; i++)
		//	rules2.daysOfTheWeek[i]=gt.rules.daysOfTheWeek[i];

		rules2.yearsList=gt.rules.yearsList;
		
		rules2.teachersList=gt.rules.teachersList;
		
		rules2.subjectsList=gt.rules.subjectsList;
		
		rules2.activityTagsList=gt.rules.activityTagsList;

		rules2.activitiesList=gt.rules.activitiesList;

		rules2.buildingsList=gt.rules.buildingsList;

		rules2.roomsList=gt.rules.roomsList;

		rules2.timeConstraintsList=gt.rules.timeConstraintsList;
		
		rules2.spaceConstraintsList=gt.rules.spaceConstraintsList;

		rules2.apstHash=gt.rules.apstHash;
		rules2.aprHash=gt.rules.aprHash;

		rules2.groupActivitiesInInitialOrderList=gt.rules.groupActivitiesInInitialOrderList;

		//add locking constraints
		TimeConstraintsList lockTimeConstraintsList;
		SpaceConstraintsList lockSpaceConstraintsList;

		//bool report=true;
		
		int addedTime=0, duplicatesTime=0;
		int addedSpace=0, duplicatesSpace=0;
		
		QString constraintsString=QString("");

		//lock selected activities
		for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
			Activity* act=&gt.rules.internalActivitiesList[ai];
			int time=tc->times[ai];
			if(time>=0 && time<gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay){
				int hour=time/gt.rules.nDaysPerWeek;
				int day=time%gt.rules.nDaysPerWeek;

				ConstraintActivityPreferredStartingTime* ctr=new ConstraintActivityPreferredStartingTime(100.0, act->id, day, hour, false); //permanently locked is false
				bool t=rules2.addTimeConstraint(ctr);
				
				if(t){
					addedTime++;
					lockTimeConstraintsList.append(ctr);
				}
				else
					duplicatesTime++;

				QString s;

				if(t)
					s=tr("Added to the saved file:", "It refers to a constraint")+QString("\n")+ctr->getDetailedDescription(gt.rules);
				else{
					s=tr("NOT added to the saved file (already existing):", "It refers to a constraint")+QString("\n")+ctr->getDetailedDescription(gt.rules);
					delete ctr;
				}
				
				constraintsString+=QString("\n");
				constraintsString+=s;
			}
			
			int ri=tc->rooms[ai];
			if(ri!=UNALLOCATED_SPACE && ri!=UNSPECIFIED_ROOM && ri>=0 && ri<gt.rules.nInternalRooms){
				QStringList tl;
				if(gt.rules.internalRoomsList[ri]->isVirtual==false)
					assert(tc->realRoomsList[ai].isEmpty());
				else
					for(int rr : std::as_const(tc->realRoomsList[ai]))
						tl.append(gt.rules.internalRoomsList[rr]->name);
				
				ConstraintActivityPreferredRoom* ctr=new ConstraintActivityPreferredRoom(100, act->id, (gt.rules.internalRoomsList[ri])->name, tl, false); //false means not permanently locked
				bool t=rules2.addSpaceConstraint(ctr);

				QString s;
				
				if(t){
					addedSpace++;
					lockSpaceConstraintsList.append(ctr);
				}
				else
					duplicatesSpace++;

				if(t)
					s=tr("Added to the saved file:", "It refers to a constraint")+QString("\n")+ctr->getDetailedDescription(gt.rules);
				else{
					s=tr("NOT added to the saved file (already existing):", "It refers to a constraint")+QString("\n")+ctr->getDetailedDescription(gt.rules);
					delete ctr;
				}
				
				constraintsString+=QString("\n");
				constraintsString+=s;
			}
		}

		LongTextMessageBox::largeInformation(parent, tr("FET information"), tr("Added %1 locking time constraints and %2 locking space constraints to saved file,"
		" ignored %3 activities which were already fixed in time and %4 activities which were already fixed in space.").arg(addedTime).arg(addedSpace).arg(duplicatesTime).arg(duplicatesSpace)
		+QString("\n\n")+tr("Detailed information about each locking constraint which was added or not (if already existing) to the saved file:")+QString("\n")+constraintsString
		+QString("\n")+tr("Your current data file remained untouched (no locking constraints were added), so you can save it also, or generate different timetables."));
			
		bool result=rules2.write(parent, s);
		
		Q_UNUSED(result);
		
		for(TimeConstraint* tc : std::as_const(lockTimeConstraintsList))
			delete tc;
		lockTimeConstraintsList.clear();
		for(SpaceConstraint* sc : std::as_const(lockSpaceConstraintsList))
			delete sc;
		lockSpaceConstraintsList.clear();

		rules2.nHoursPerDay=0;
		rules2.hoursOfTheDay.clear();
		rules2.nDaysPerWeek=0;
		rules2.daysOfTheWeek.clear();

		rules2.yearsList.clear();
		
		rules2.teachersList.clear();
		
		rules2.subjectsList.clear();
		
		rules2.activityTagsList.clear();

		rules2.activitiesList.clear();

		rules2.buildingsList.clear();

		rules2.roomsList.clear();

		rules2.timeConstraintsList.clear();
		
		rules2.spaceConstraintsList.clear();

		rules2.apstHash.clear();
		rules2.aprHash.clear();

		rules2.groupActivitiesInInitialOrderList.clear();
	}
	
	if(pc_form!=nullptr)
		delete pc_form;
}

bool FetMainForm::fileSave()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return false;
	}

	if(INPUT_FILENAME_XML.isEmpty())
		return fileSaveAs();
	else{
		bool t=gt.rules.write(this, INPUT_FILENAME_XML);
		
		if(t){
			//gt.rules.addUndoPoint(tr("Saved the file (the name was %1).").arg(QDir::toNativeSeparators(INPUT_FILENAME_XML)));
			savedStateIterator=cntUndoRedoStackIterator;

			saveHistory();

			gt.rules.modified=true; //force update of the modified flag of the main window
			setRulesUnmodifiedAndOtherThings(&gt.rules);
		
			setCurrentFile(INPUT_FILENAME_XML);
	
			statusBar()->showMessage(tr("File saved"), STATUS_BAR_MILLISECONDS);
			return true;
		}
		else{
			return false;
		}
	}
}

void FetMainForm::fileSaveAction_triggered()
{
	fileSave();
}

void FetMainForm::dataInstitutionNameAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	InstitutionNameForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataCommentsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	CommentsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataDaysAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	DaysForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataHoursAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	HoursForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTeachersAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	TeachersForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTeachersStatisticsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	TeachersStatisticsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSubjectsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	SubjectsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSubjectsStatisticsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	SubjectsStatisticsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataActivityTagsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ActivityTagsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataYearsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	YearsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataGroupsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	GroupsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSubgroupsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	SubgroupsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataStudentsStatisticsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	StudentsStatisticsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataActivitiesRoomsStatisticsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	ActivitiesRoomsStatisticsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTeachersSubjectsQualificationsStatisticsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	QHash<QString, Teacher*> teachersHash;
	
	for(Teacher* tch : std::as_const(gt.rules.teachersList))
		teachersHash.insert(tch->name, tch);
		
	bool unqualifiedExist=false;

	QString s=tr("The teachers who are not qualified to teach a certain activity (in activities order):");
	s+="\n\n";
	
	bool begin=true;

	for(Activity* act : std::as_const(gt.rules.activitiesList)){
		bool alreadyAdded=false;
		QString subject=act->subjectName;
		for(const QString& teacher : std::as_const(act->teachersNames)){
			Teacher* tch=teachersHash.value(teacher, nullptr);
			assert(tch!=nullptr);
			if(!tch->qualifiedSubjectsHash.contains(subject)){
				unqualifiedExist=true;
				if(!alreadyAdded){
					if(!begin)
						s+="\n";
					else
						begin=false;
					s+=tr("For activity: %1").arg(act->getDescription(gt.rules));
					s+="\n";
					alreadyAdded=true;
				}
				s+=QString(4, ' ');
				s+=tr("Teacher %1 is not qualified to teach subject %2.").arg(teacher).arg(subject);
				s+="\n";
			}
		}
	}

	if(!unqualifiedExist)
		s=tr("All the teachers are qualified to teach their activities.");
	s+="\n";

	LongTextMessageBox::largeInformation(this, tr("FET information"), s);
}

void FetMainForm::helpSettingsAction_triggered()
{
	QString s;
	
	s+=tr("Probably some settings which are more difficult to understand are these ones:");
	s+="\n\n";
	s+=tr("Option 'Divide HTML timetables with time axis by days':"
	" This means simply that the HTML timetables of type 'time horizontal' or 'time vertical' (see the generated HTML timetables)"
	" should be or not divided according to the days.");
	s+=" ";
	s+=tr("If the 'time horizontal' or 'time vertical' HTML timetables are too large for you, then you might need to select this option");
	
	s+="\n\n";
	s+=tr("Option 'Print activities with same starting time in timetables': selecting it means that the HTML timetables will contain for"
	 " each slot all the activities which have the same starting time (fact specified by your constraints) as the activity(ies) which are normally shown in this slot."
	 " If you don't use constraints activities same starting time, this option has no effect for you.");
	
	s+="\n\n";
	s+=tr("Seed of random number generator: please read the help in the dialog of this option");
	
	s+="\n\n";
	s+=tr("Interface - use colors: the places with colors in FET interface are in:");
	s+="\n";
	s+=" -";
	s+=tr("add/modify constraints break, not available, preferred starting times or time slots, occupy max time slots from selection or"
		" max simultaneous in selected time slots (the table cells will have green or red colors).");
	s+="\n";
	/*s+=" -";
	s+=tr("activities and subactivities dialogs, the inactive activities will have a distinctive background color");
	s+="\n";
	s+=" -";
	s+=tr("all time constraints and all space constraints dialogs, the inactive constraints will have a distinctive background color");
	s+="\n";
	s+=" -";
	s+=tr("group activities in the initial order items, the inactive items will have a distinctive background color");
	s+="\n";*/
	s+=" -";
	s+=tr("the timetable view dialogs");
	
	s+="\n\n";
	s+=tr("Enable activity tag max hours daily:");
	s+="\n";
	s+=tr("This will enable the menu for the constraints: teacher(s) or students (set) activity tag max hours daily / per real day (where applicable)."
		" These constraints are good, but not perfect and may bring slow down of generation or impossible timetables if improperly used. Select only"
		" if you know what you're doing.");
	s+="\n\n";
	s+=tr("Enable students max gaps per day:");
	s+="\n";
	s+=tr("This will enable the menu for 2 constraints: students (set) max gaps per day. These 2 constraints are good, but not perfect and"
		" may bring slow down of generation or impossible timetables if improperly used. Select only if you know what you're doing.");
		
	s+="\n\n";
	s+=tr("Warn if using not perfect constraints:", "this is a warning if user uses not perfect constraints");
	s+="\n";
	s+=tr("If you use a not perfect constraint (activity tag max hours daily / per real day or students max gaps per day / per real day / per week for real days - where applicable),"
		" you'll get a warning before generating. Uncheck this option to get rid of that warning (it is recommended to keep the warning).");
	
	s+="\n\n";
	s+=tr("Enable students min hours daily with empty days:");
	s+="\n";
	s+=tr("This will enable you to modify the students min hours daily constraints, to allow empty days. It is IMPERATIVE (for performance reasons) to allow empty days for students min hours daily only"
		" if your institution allows empty days for students and if a solution with empty days is possible. Select only if you know what you're doing.");
		
	s+="\n\n";
	s+=tr("Warn if using students min hours daily with empty days:", "this is a warning if user has constraints min hours daily for students with allowed empty days");
	s+="\n";
	s+=tr("If you use constraints students min hours daily with allowed empty days, you'll get a warning before generating"
		". Uncheck this option to get rid of that warning (it is recommended to keep the warning).");
		
	s+="\n\n";
	
	s+=tr("Confirmations: unselect the corresponding check boxes if you want to skip introduction and confirmation to various advanced dialogs.");
	
	s+="\n\n";
	
	s+=tr("Duplicate vertical headers to the right (in timetable settings) - select this if you want the timetables to duplicate the table left vertical headers to the right"
		" part of the tables");
		
	s+="\n\n";
	
	s+=tr("If you have many subgroups and you don't explicitly use them, it is recommended to use the three global settings: hide subgroups"
		" in combo boxes, hide subgroups in activity planning, and do not write subgroups timetables on hard disk.");
	s+="\n";
	s+=tr("Note that using the global menu setting to hide subgroups in activity planning is a different thing from the check box in the activity"
		" planning dialog, and the global setting works better and faster.");
	s+="\n";
	s+=tr("If you hide subgroups in combo boxes, the affected dialogs (like the activities dialog or the add constraint students set not available times dialog)"
		" will load much faster if you have many subgroups.");
		
	s+="\n\n";
	
	s+=tr("If you are only working on a timetable, and you do not need to publish it, you may want to disable writing some categories of timetables"
		" on the hard disk, for efficiency (the generation speed is not affected, only the overhead to write the partial/complete timetables"
		" when stopping/finishing the generation). The timetables taking the longest time are the subgroups, groups AND years ones.");
	s+=" ";
	s+=tr("(Also the conflicts timetable might take long to write, if the file is large.)");
	s+=" ";
	s+=tr("After that, you can enable the writing of the timetables and regenerate.");

	s+="\n\n";
	s+=tr("Show tool tips for constraints with tables: in the add/modify constraint dialogs which use tables, like the 'not available times' ones,"
		" each table cell will have a tool tip to show the current day/hour (column/row name).");
	
	LongTextMessageBox::largeInformation(this, tr("FET information"), s);
}

void FetMainForm::dataHelpOnStatisticsAction_triggered()
{
	QString s;
	
	s+=tr("You will find in the statistics only active activities count. The inactive ones are not counted.");
	
	s+="\n\n";
	
	s+=tr("Statistics for students might be the most difficult to understand."
	 " If you are using divisions of years: probably the most relevant statistics"
	 " are the ones for each subgroup (so you may check only subgroups check box)."
	 " You may see more hours for the years or groups, but these are not significant, please ignore them,"
	 " because each year or group will count also activities of all contained subgroups."
	 "\n\n"
	 "Each subgroup should have a number of hours per week close to the average of"
	 " all subgroups and close to the normal number of working hours of each students set."
	 " If a subgroup has a much lower value, maybe you used incorrectly"
	 " the years/groups/subgroups for activities."
	 "\n\n"
	 "Please read FAQ for detailed description"
	 " on how divisions work. The key is that the subgroups are independent and represent the smallest unit of students."
	 " Each subgroup receives the activities of the parent year and parent group and of itself."
	 "\n\n"
	 "Having a subgroup with too little working hours per week means that you inputted activities in a wrong manner,"
	 " and also that some constraints like no gaps, early or min hours daily for this subgroup"
	 " are interpreted in a wrong manner (if subgroup has only 2 activities, then these must"
	 " be placed in the first hours, which is too hard and wrong)."
	 );
	
	s+="\n\n";
	s+=tr("Students' statistics form contains a check box named '%1'"
	 ". This has effect only if you have overlapping groups/years, and means that FET will show the complete tree structure"
	 ", even if that means that some subgroups/groups will appear twice or more in the table, with the same information."
	 " For instance, if you have year Y1, groups G1 and G2, subgroups S1, S2, S3, with structure: Y1 (G1 (S1, S2), G2 (S1, S3)),"
	 " S1 will appear twice in the table").arg(tr("Show duplicates"));

	s+="\n\n";
	s+=tr("Activities rooms statistics: this menu will show the activities which may be scheduled in an unspecified room,"
	 " if they are referred to by space constraints with weight under 100.0%, and the activities which will certainly be scheduled in an"
	 " unspecified room, if they are not referred to by any space constraints. Remember that home rooms constraints are effective only"
	 " on activities which have only the exact specified teacher or students set (activities with more teachers or students"
	 " sets are not affected by home rooms constraints, you need to have preferred rooms constraints for such activities to ensure"
	 " they will not end up in an unspecified room).");
	
	LongTextMessageBox::largeInformation(this, tr("FET - information about statistics"), s);
}

void FetMainForm::dataActivitiesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ActivitiesForm form(this, "", "", "", "");
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSubactivitiesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	SubactivitiesForm form(this, "", "", "", "");
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataRoomsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	RoomsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataBuildingsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	BuildingsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataAllTimeConstraintsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	AllTimeConstraintsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataAllSpaceConstraintsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	AllSpaceConstraintsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTwoActivitiesConsecutiveAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTwoActivitiesConsecutiveForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTwoActivitiesGroupedAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTwoActivitiesGroupedForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsThreeActivitiesGroupedAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintThreeActivitiesGroupedForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTwoActivitiesOrderedAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTwoActivitiesOrderedForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTwoSetsOfActivitiesOrderedAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTwoSetsOfActivitiesOrderedForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTwoActivitiesOrderedIfSameDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTwoActivitiesOrderedIfSameDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsActivitiesPreferredTimeSlotsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivitiesPreferredTimeSlotsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsActivitiesPreferredStartingTimesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivitiesPreferredStartingTimesForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsSubactivitiesPreferredTimeSlotsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintSubactivitiesPreferredTimeSlotsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsSubactivitiesPreferredStartingTimesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintSubactivitiesPreferredStartingTimesForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsActivityEndsStudentsDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivityEndsStudentsDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsActivitiesEndStudentsDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivitiesEndStudentsDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsActivityEndsTeachersDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivityEndsTeachersDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsActivitiesEndTeachersDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivitiesEndTeachersDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsActivityBeginsStudentsDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivityBeginsStudentsDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsActivitiesBeginStudentsDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivitiesBeginStudentsDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsActivityBeginsTeachersDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivityBeginsTeachersDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsActivitiesBeginTeachersDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivitiesBeginTeachersDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsActivitiesSameStartingTimeAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivitiesSameStartingTimeForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsActivitiesSameStartingHourAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivitiesSameStartingHourForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsActivitiesSameStartingDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivitiesSameStartingDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsActivitiesOccupyMaxTimeSlotsFromSelectionAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivitiesOccupyMaxTimeSlotsFromSelectionForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsActivitiesOccupyMinTimeSlotsFromSelectionAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivitiesOccupyMinTimeSlotsFromSelectionForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsActivitiesMaxSimultaneousInSelectedTimeSlotsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivitiesMaxSimultaneousInSelectedTimeSlotsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsActivitiesMinSimultaneousInSelectedTimeSlotsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivitiesMinSimultaneousInSelectedTimeSlotsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsMaxTotalActivitiesFromSetInSelectedTimeSlotsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintMaxTotalActivitiesFromSetInSelectedTimeSlotsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherNotAvailableTimesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherNotAvailableTimesForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersNotAvailableTimesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	TeachersNotAvailableTimesTimeHorizontalForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsNotAvailableTimesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	StudentsNotAvailableTimesTimeHorizontalForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsBasicCompulsoryTimeAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintBasicCompulsoryTimeForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsBasicCompulsorySpaceAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintBasicCompulsorySpaceForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsRoomNotAvailableTimesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintRoomNotAvailableTimesForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsTeacherRoomNotAvailableTimesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}
	
	ConstraintTeacherRoomNotAvailableTimesForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsActivityPreferredRoomAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivityPreferredRoomForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsActivityPreferredRoomsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivityPreferredRoomsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsSubjectPreferredRoomAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintSubjectPreferredRoomForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsSubjectPreferredRoomsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintSubjectPreferredRoomsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsSubjectActivityTagPreferredRoomAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintSubjectActivityTagPreferredRoomForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsSubjectActivityTagPreferredRoomsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintSubjectActivityTagPreferredRoomsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

///added 6 apr 2009
void FetMainForm::dataSpaceConstraintsActivityTagPreferredRoomAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivityTagPreferredRoomForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsActivityTagPreferredRoomsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivityTagPreferredRoomsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}
///

void FetMainForm::dataSpaceConstraintsStudentsSetHomeRoomAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetHomeRoomForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsStudentsSetHomeRoomsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetHomeRoomsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}


void FetMainForm::dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMaxBuildingChangesPerDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsStudentsMaxBuildingChangesPerDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMaxBuildingChangesPerDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsStudentsSetMaxBuildingChangesPerWeekAction_triggered()
{
	if(generation_running || generation_running_multi){
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMaxBuildingChangesPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsStudentsMaxBuildingChangesPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMaxBuildingChangesPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsStudentsSetMinGapsBetweenBuildingChangesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMinGapsBetweenBuildingChangesForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsStudentsMinGapsBetweenBuildingChangesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMinGapsBetweenBuildingChangesForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsTeacherMaxBuildingChangesPerDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMaxBuildingChangesPerDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}
void FetMainForm::dataSpaceConstraintsTeachersMaxBuildingChangesPerDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMaxBuildingChangesPerDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsTeacherMaxBuildingChangesPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMaxBuildingChangesPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsTeachersMaxBuildingChangesPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMaxBuildingChangesPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsTeacherMinGapsBetweenBuildingChangesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMinGapsBetweenBuildingChangesForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsTeachersMinGapsBetweenBuildingChangesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMinGapsBetweenBuildingChangesForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsStudentsSetMaxRoomChangesPerDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMaxRoomChangesPerDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsStudentsMaxRoomChangesPerDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMaxRoomChangesPerDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsStudentsSetMaxRoomChangesPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMaxRoomChangesPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsStudentsMaxRoomChangesPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMaxRoomChangesPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsStudentsSetMinGapsBetweenRoomChangesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMinGapsBetweenRoomChangesForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsStudentsMinGapsBetweenRoomChangesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMinGapsBetweenRoomChangesForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsTeacherMaxRoomChangesPerDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMaxRoomChangesPerDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}
void FetMainForm::dataSpaceConstraintsTeachersMaxRoomChangesPerDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMaxRoomChangesPerDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsTeacherMaxRoomChangesPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMaxRoomChangesPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsTeachersMaxRoomChangesPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMaxRoomChangesPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsTeacherMinGapsBetweenRoomChangesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMinGapsBetweenRoomChangesForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsTeachersMinGapsBetweenRoomChangesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMinGapsBetweenRoomChangesForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsTeacherHomeRoomAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherHomeRoomForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsTeacherHomeRoomsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherHomeRoomsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetNotAvailableTimesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetNotAvailableTimesForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsBreakTimesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintBreakTimesForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMaxDaysPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMaxDaysPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMaxDaysPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMaxDaysPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMinDaysPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMinDaysPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMinDaysPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMinDaysPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherIntervalMaxDaysPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherIntervalMaxDaysPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersIntervalMaxDaysPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersIntervalMaxDaysPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetMaxDaysPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMaxDaysPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMaxDaysPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMaxDaysPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetIntervalMaxDaysPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetIntervalMaxDaysPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsIntervalMaxDaysPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsIntervalMaxDaysPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMaxHoursDailyAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMaxHoursDailyForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMaxHoursDailyInIntervalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMaxHoursDailyInIntervalForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMaxHoursDailyInIntervalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMaxHoursDailyInIntervalForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetMaxHoursDailyInIntervalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMaxHoursDailyInIntervalForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMaxHoursDailyInIntervalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMaxHoursDailyInIntervalForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMaxHoursDailyAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMaxHoursDailyForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMaxHoursContinuouslyAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMaxHoursContinuouslyForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMaxHoursContinuouslyAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMaxHoursContinuouslyForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersActivityTagMaxHoursContinuouslyAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersActivityTagMaxHoursContinuouslyForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherActivityTagMaxHoursContinuouslyAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherActivityTagMaxHoursContinuouslyForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersActivityTagMaxHoursDailyAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}
	
	if(!ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY){
		enableNotPerfectMessage();
		return;
	}

	ConstraintTeachersActivityTagMaxHoursDailyForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherActivityTagMaxHoursDailyAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	if(!ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY){
		enableNotPerfectMessage();
		return;
	}

	ConstraintTeacherActivityTagMaxHoursDailyForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersActivityTagMinHoursDailyAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}
	
	if(!ENABLE_ACTIVITY_TAG_MIN_HOURS_DAILY){
		enableNotPerfectMessage();
		return;
	}

	ConstraintTeachersActivityTagMinHoursDailyForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherActivityTagMinHoursDailyAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	if(!ENABLE_ACTIVITY_TAG_MIN_HOURS_DAILY){
		enableNotPerfectMessage();
		return;
	}

	ConstraintTeacherActivityTagMinHoursDailyForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMinHoursDailyAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMinHoursDailyForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMinHoursDailyAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMinHoursDailyForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsActivityPreferredStartingTimeAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivityPreferredStartingTimeForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetMaxGapsPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMaxGapsPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMaxGapsPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMaxGapsPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

//terms
void FetMainForm::dataTimeConstraintsActivitiesMaxInATermAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivitiesMaxInATermForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsActivitiesMinInATermAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivitiesMinInATermForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsActivitiesOccupyMaxTermsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivitiesOccupyMaxTermsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

//2024-02-09
void FetMainForm::dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayInIntervalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMaxBuildingChangesPerDayInIntervalForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsStudentsMaxBuildingChangesPerDayInIntervalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMaxBuildingChangesPerDayInIntervalForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsTeacherMaxBuildingChangesPerDayInIntervalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMaxBuildingChangesPerDayInIntervalForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsTeachersMaxBuildingChangesPerDayInIntervalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMaxBuildingChangesPerDayInIntervalForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsStudentsSetMaxBuildingChangesPerRealDayInIntervalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMaxBuildingChangesPerRealDayInIntervalForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsStudentsMaxBuildingChangesPerRealDayInIntervalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMaxBuildingChangesPerRealDayInIntervalForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsTeacherMaxBuildingChangesPerRealDayInIntervalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMaxBuildingChangesPerRealDayInIntervalForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsTeachersMaxBuildingChangesPerRealDayInIntervalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMaxBuildingChangesPerRealDayInIntervalForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

//2024-02-19
void FetMainForm::dataSpaceConstraintsStudentsSetMaxRoomChangesPerDayInIntervalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMaxRoomChangesPerDayInIntervalForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsStudentsMaxRoomChangesPerDayInIntervalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMaxRoomChangesPerDayInIntervalForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsTeacherMaxRoomChangesPerDayInIntervalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMaxRoomChangesPerDayInIntervalForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsTeachersMaxRoomChangesPerDayInIntervalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMaxRoomChangesPerDayInIntervalForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsStudentsSetMaxRoomChangesPerRealDayInIntervalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMaxRoomChangesPerRealDayInIntervalForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsStudentsMaxRoomChangesPerRealDayInIntervalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMaxRoomChangesPerRealDayInIntervalForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsTeacherMaxRoomChangesPerRealDayInIntervalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMaxRoomChangesPerRealDayInIntervalForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsTeachersMaxRoomChangesPerRealDayInIntervalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMaxRoomChangesPerRealDayInIntervalForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::groupActivitiesInInitialOrderAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	if(!ENABLE_GROUP_ACTIVITIES_IN_INITIAL_ORDER){
		QString s=tr("Feature is not enabled. To use this type of feature you must enable it from the Settings->Advanced menu.");
		s+="\n\n";
		s+=tr("Explanation:");
		s+=" ";
		s+=tr("The feature must be used with caution, by experienced users.");

		QMessageBox::information(this, tr("FET information"), s);

		return;
	}

	GroupActivitiesInInitialOrderItemsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetMaxGapsPerDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	if(!ENABLE_STUDENTS_MAX_GAPS_PER_DAY){
		enableNotPerfectMessage();
		return;
	}

	ConstraintStudentsSetMaxGapsPerDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMaxGapsPerDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}
	
	if(!ENABLE_STUDENTS_MAX_GAPS_PER_DAY){
		enableNotPerfectMessage();
		return;
	}

	ConstraintStudentsMaxGapsPerDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMaxGapsPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMaxGapsPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMaxGapsPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMaxGapsPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMaxGapsPerDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMaxGapsPerDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMaxGapsPerDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMaxGapsPerDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMaxGapsPerMorningAndAfternoonAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMaxGapsPerMorningAndAfternoonForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMaxGapsPerMorningAndAfternoonAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMaxGapsPerMorningAndAfternoonForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsEarlyMaxBeginningsAtSecondHourAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsEarlyMaxBeginningsAtSecondHourForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetEarlyMaxBeginningsAtSecondHourAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetMaxHoursDailyAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMaxHoursDailyForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMaxHoursDailyAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMaxHoursDailyForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetMaxHoursContinuouslyAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMaxHoursContinuouslyForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMaxHoursContinuouslyAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMaxHoursContinuouslyForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetActivityTagMaxHoursContinuouslyAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetActivityTagMaxHoursContinuouslyForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsActivityTagMaxHoursContinuouslyAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsActivityTagMaxHoursContinuouslyForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	if(!ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY){
		enableNotPerfectMessage();
		return;
	}

	ConstraintStudentsSetActivityTagMaxHoursDailyForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsActivityTagMaxHoursDailyAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	if(!ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY){
		enableNotPerfectMessage();
		return;
	}

	ConstraintStudentsActivityTagMaxHoursDailyForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetActivityTagMinHoursDailyAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	if(!ENABLE_ACTIVITY_TAG_MIN_HOURS_DAILY){
		enableNotPerfectMessage();
		return;
	}

	ConstraintStudentsSetActivityTagMinHoursDailyForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsActivityTagMinHoursDailyAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	if(!ENABLE_ACTIVITY_TAG_MIN_HOURS_DAILY){
		enableNotPerfectMessage();
		return;
	}

	ConstraintStudentsActivityTagMinHoursDailyForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetMinHoursDailyAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMinHoursDailyForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMinHoursDailyAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMinHoursDailyForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

//begin 2021-12-15
void FetMainForm::dataTimeConstraintsStudentsSetMinGapsBetweenActivityTagAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMinGapsBetweenActivityTagForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMinGapsBetweenActivityTagAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMinGapsBetweenActivityTagForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMinGapsBetweenActivityTagAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMinGapsBetweenActivityTagForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMinGapsBetweenActivityTagAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMinGapsBetweenActivityTagForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}
//end 2021-12-15

//begin 2024-03-16
void FetMainForm::dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsPerRealDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsPerRealDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetMinGapsBetweenActivityTagPerRealDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMinGapsBetweenActivityTagPerRealDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMinGapsBetweenActivityTagPerRealDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMinGapsBetweenActivityTagPerRealDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMinGapsBetweenActivityTagPerRealDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMinGapsBetweenActivityTagPerRealDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMinGapsBetweenActivityTagPerRealDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMinGapsBetweenActivityTagPerRealDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}
//end 2024-03-16

//begin 2024-05-20
void FetMainForm::dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoonForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoonForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMinGapsBetweenActivityTagBetweenMorningAndAfternoonForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoonForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}
//end 2024-03-16

void FetMainForm::dataTimeConstraintsActivitiesNotOverlappingAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivitiesNotOverlappingForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsActivityTagsNotOverlappingAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivityTagsNotOverlappingForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsMinDaysBetweenActivitiesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintMinDaysBetweenActivitiesForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsMinHalfDaysBetweenActivitiesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintMinHalfDaysBetweenActivitiesForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsMaxDaysBetweenActivitiesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintMaxDaysBetweenActivitiesForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsActivitiesMaxHourlySpanAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivitiesMaxHourlySpanForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsMaxHalfDaysBetweenActivitiesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintMaxHalfDaysBetweenActivitiesForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsMaxTermsBetweenActivitiesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintMaxTermsBetweenActivitiesForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsMinGapsBetweenActivitiesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintMinGapsBetweenActivitiesForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsMaxGapsBetweenActivitiesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintMaxGapsBetweenActivitiesForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsActivityPreferredTimeSlotsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivityPreferredTimeSlotsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsActivityPreferredStartingTimesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivityPreferredStartingTimesForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMaxSpanPerDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMaxSpanPerDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMaxSpanPerDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMaxSpanPerDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetMaxSpanPerDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMaxSpanPerDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMaxSpanPerDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMaxSpanPerDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMinRestingHoursAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMinRestingHoursForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMinRestingHoursAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMinRestingHoursForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetMinRestingHoursAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMinRestingHoursForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMinRestingHoursAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMinRestingHoursForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

//2012-04-29
void FetMainForm::dataSpaceConstraintsActivitiesOccupyMaxDifferentRoomsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivitiesOccupyMaxDifferentRoomsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

//2013-09-14
void FetMainForm::dataSpaceConstraintsActivitiesSameRoomIfConsecutiveAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintActivitiesSameRoomIfConsecutiveForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

//mornings-afternoons space
void FetMainForm::dataSpaceConstraintsStudentsSetMaxRoomChangesPerRealDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMaxRoomChangesPerRealDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsStudentsMaxRoomChangesPerRealDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMaxRoomChangesPerRealDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsTeacherMaxRoomChangesPerRealDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMaxRoomChangesPerRealDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsTeachersMaxRoomChangesPerRealDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMaxRoomChangesPerRealDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsStudentsSetMaxBuildingChangesPerRealDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMaxBuildingChangesPerRealDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsStudentsMaxBuildingChangesPerRealDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMaxBuildingChangesPerRealDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsTeacherMaxBuildingChangesPerRealDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMaxBuildingChangesPerRealDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsTeachersMaxBuildingChangesPerRealDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMaxBuildingChangesPerRealDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

//mornings-afternoons time
void FetMainForm::dataTimeConstraintsTeacherMaxRealDaysPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMaxRealDaysPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMaxRealDaysPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMaxRealDaysPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMaxAfternoonsPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMaxAfternoonsPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMaxAfternoonsPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMaxAfternoonsPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMaxMorningsPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMaxMorningsPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMaxMorningsPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMaxMorningsPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

////////2021-09-26
void FetMainForm::dataTimeConstraintsTeacherMaxThreeConsecutiveDaysAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMaxThreeConsecutiveDaysForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMaxThreeConsecutiveDaysAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMaxThreeConsecutiveDaysForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

////////2022-02-15
void FetMainForm::dataTimeConstraintsStudentsSetMaxThreeConsecutiveDaysAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMaxThreeConsecutiveDaysForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMaxThreeConsecutiveDaysAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMaxThreeConsecutiveDaysForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}
//////////

void FetMainForm::dataTimeConstraintsTeacherMaxTwoConsecutiveMorningsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMaxTwoConsecutiveMorningsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMaxTwoConsecutiveMorningsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMaxTwoConsecutiveMorningsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMaxTwoConsecutiveAfternoonsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMaxTwoConsecutiveAfternoonsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMaxTwoConsecutiveAfternoonsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMaxTwoConsecutiveAfternoonsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMinRealDaysPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMinRealDaysPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMinMorningsPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMinMorningsPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMinMorningsPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMinMorningsPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMinAfternoonsPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMinAfternoonsPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMinAfternoonsPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMinAfternoonsPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMinRealDaysPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMinRealDaysPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMorningIntervalMaxDaysPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMorningIntervalMaxDaysPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMorningIntervalMaxDaysPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMorningIntervalMaxDaysPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherAfternoonIntervalMaxDaysPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherAfternoonIntervalMaxDaysPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersAfternoonIntervalMaxDaysPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersAfternoonIntervalMaxDaysPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetMaxRealDaysPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMaxRealDaysPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMaxRealDaysPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMaxRealDaysPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetMaxMorningsPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMaxMorningsPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMaxMorningsPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMaxMorningsPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetMaxAfternoonsPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMaxAfternoonsPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMaxAfternoonsPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMaxAfternoonsPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetMinMorningsPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMinMorningsPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMinMorningsPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMinMorningsPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetMinAfternoonsPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMinAfternoonsPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMinAfternoonsPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMinAfternoonsPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetMorningIntervalMaxDaysPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMorningIntervalMaxDaysPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMorningIntervalMaxDaysPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMorningIntervalMaxDaysPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetAfternoonIntervalMaxDaysPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetAfternoonIntervalMaxDaysPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsAfternoonIntervalMaxDaysPerWeekAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsAfternoonIntervalMaxDaysPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMaxHoursDailyRealDaysAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMaxHoursDailyRealDaysForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMaxHoursDailyRealDaysAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMaxHoursDailyRealDaysForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMaxTwoActivityTagsPerDayFromN1N2N3Action_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMaxTwoActivityTagsPerDayFromN1N2N3Form form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMaxTwoActivityTagsPerDayFromN1N2N3Action_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMaxTwoActivityTagsPerDayFromN1N2N3Form form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetMaxTwoActivityTagsPerDayFromN1N2N3Action_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMaxTwoActivityTagsPerDayFromN1N2N3Form form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMaxTwoActivityTagsPerDayFromN1N2N3Action_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMaxTwoActivityTagsPerDayFromN1N2N3Form form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMaxTwoActivityTagsPerRealDayFromN1N2N3Action_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMaxTwoActivityTagsPerRealDayFromN1N2N3Form form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMaxTwoActivityTagsPerRealDayFromN1N2N3Action_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMaxTwoActivityTagsPerRealDayFromN1N2N3Form form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetMaxTwoActivityTagsPerRealDayFromN1N2N3Action_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMaxTwoActivityTagsPerRealDayFromN1N2N3Form form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMaxTwoActivityTagsPerRealDayFromN1N2N3Action_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMaxTwoActivityTagsPerRealDayFromN1N2N3Form form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersActivityTagMaxHoursDailyRealDaysAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	if(!ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY){
		enableNotPerfectMessage();
		return;
	}

	ConstraintTeachersActivityTagMaxHoursDailyRealDaysForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherActivityTagMaxHoursDailyRealDaysAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	if(!ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY){
		enableNotPerfectMessage();
		return;
	}

	ConstraintTeacherActivityTagMaxHoursDailyRealDaysForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMaxHoursPerAllAfternoonsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMaxHoursPerAllAfternoonsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMaxHoursPerAllAfternoonsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMaxHoursPerAllAfternoonsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetMaxHoursPerAllAfternoonsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMaxHoursPerAllAfternoonsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMaxHoursPerAllAfternoonsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMaxHoursPerAllAfternoonsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMinHoursPerMorningAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMinHoursPerMorningForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMinHoursPerMorningAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMinHoursPerMorningForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMinHoursPerAfternoonAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMinHoursPerAfternoonForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMinHoursPerAfternoonAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMinHoursPerAfternoonForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMinHoursDailyRealDaysAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMinHoursDailyRealDaysForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMinHoursDailyRealDaysAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMinHoursDailyRealDaysForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMaxSpanPerRealDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMaxSpanPerRealDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMaxSpanPerRealDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMaxSpanPerRealDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetMaxSpanPerRealDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMaxSpanPerRealDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMaxSpanPerRealDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMaxSpanPerRealDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMinRestingHoursBetweenMorningAndAfternoonAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMinRestingHoursBetweenMorningAndAfternoonForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMinRestingHoursBetweenMorningAndAfternoonAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMinRestingHoursBetweenMorningAndAfternoonForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetMinRestingHoursBetweenMorningAndAfternoonAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoonForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMinRestingHoursBetweenMorningAndAfternoonAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMinRestingHoursBetweenMorningAndAfternoonForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetMaxGapsPerRealDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	if(!ENABLE_MAX_GAPS_PER_REAL_DAY){
		enableNotPerfectMessage();
		return;
	}

	ConstraintStudentsSetMaxGapsPerRealDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMaxGapsPerRealDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	if(!ENABLE_MAX_GAPS_PER_REAL_DAY){
		enableNotPerfectMessage();
		return;
	}

	ConstraintStudentsMaxGapsPerRealDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMaxGapsPerRealDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	if(!ENABLE_MAX_GAPS_PER_REAL_DAY){
		enableNotPerfectMessage();
		return;
	}

	ConstraintTeachersMaxGapsPerRealDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMaxGapsPerRealDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	if(!ENABLE_MAX_GAPS_PER_REAL_DAY){
		enableNotPerfectMessage();
		return;
	}

	ConstraintTeacherMaxGapsPerRealDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetMaxGapsPerWeekForRealDaysAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	if(!ENABLE_MAX_GAPS_PER_REAL_DAY){
		enableNotPerfectMessage();
		return;
	}

	ConstraintStudentsSetMaxGapsPerWeekForRealDaysForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMaxGapsPerWeekForRealDaysAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	if(!ENABLE_MAX_GAPS_PER_REAL_DAY){
		enableNotPerfectMessage();
		return;
	}

	ConstraintStudentsMaxGapsPerWeekForRealDaysForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMaxGapsPerWeekForRealDaysAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	if(!ENABLE_MAX_GAPS_PER_REAL_DAY){
		enableNotPerfectMessage();
		return;
	}

	ConstraintTeachersMaxGapsPerWeekForRealDaysForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMaxGapsPerWeekForRealDaysAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	if(!ENABLE_MAX_GAPS_PER_REAL_DAY){
		enableNotPerfectMessage();
		return;
	}

	ConstraintTeacherMaxGapsPerWeekForRealDaysForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMaxZeroGapsPerAfternoonAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMaxZeroGapsPerAfternoonForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMaxZeroGapsPerAfternoonAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMaxZeroGapsPerAfternoonForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersAfternoonsEarlyMaxBeginningsAtSecondHourAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersAfternoonsEarlyMaxBeginningsAtSecondHourForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherAfternoonsEarlyMaxBeginningsAtSecondHourAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherAfternoonsEarlyMaxBeginningsAtSecondHourForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsAfternoonsEarlyMaxBeginningsAtSecondHourAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsAfternoonsEarlyMaxBeginningsAtSecondHourForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHourAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHourForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeachersMorningsEarlyMaxBeginningsAtSecondHourAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeachersMorningsEarlyMaxBeginningsAtSecondHourForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMorningsEarlyMaxBeginningsAtSecondHourAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHourForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMorningsEarlyMaxBeginningsAtSecondHourAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHourForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetMorningsEarlyMaxBeginningsAtSecondHourAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMorningsEarlyMaxBeginningsAtSecondHourForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetMaxHoursDailyRealDaysAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMaxHoursDailyRealDaysForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMaxHoursDailyRealDaysAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMaxHoursDailyRealDaysForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyRealDaysAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	if(!ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY){
		enableNotPerfectMessage();
		return;
	}

	ConstraintStudentsSetActivityTagMaxHoursDailyRealDaysForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsActivityTagMaxHoursDailyRealDaysAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	if(!ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY){
		enableNotPerfectMessage();
		return;
	}

	ConstraintStudentsActivityTagMaxHoursDailyRealDaysForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetMinHoursPerMorningAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMinHoursPerMorningForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMinHoursPerMorningAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMinHoursPerMorningForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsSetMinHoursPerAfternoonAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsSetMinHoursPerAfternoonForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsStudentsMinHoursPerAfternoonAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	ConstraintStudentsMinHoursPerAfternoonForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::helpMoroccoAction_triggered()
{
	HelpMoroccoForm* form=new HelpMoroccoForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
}

void FetMainForm::helpAlgeriaAction_triggered()
{
	HelpAlgeriaForm* form=new HelpAlgeriaForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
}

void FetMainForm::helpAboutFETAction_triggered()
{
	HelpAboutForm* form=new HelpAboutForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
}

void FetMainForm::helpAboutQtAction_triggered()
{
	QMessageBox::aboutQt(this);
}

void FetMainForm::helpAboutLibrariesAction_triggered()
{
	HelpAboutLibrariesForm* form=new HelpAboutLibrariesForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
}

void FetMainForm::helpHomepageAction_triggered()
{
	bool tds=QDesktopServices::openUrl(QUrl("https://lalescu.ro/liviu/fet/"));

	if(!tds){
		QMessageBox::warning(this, tr("FET warning"), tr("Could not start the default internet browser (trying to open the link %1)."
		" Maybe you can try to manually start your browser and open this link.").arg("https://lalescu.ro/liviu/fet/"));
	}
}

void FetMainForm::helpContentsAction_triggered()
{
	bool tds=QDesktopServices::openUrl(QUrl("https://lalescu.ro/liviu/fet/doc/"));

	if(!tds){
		QMessageBox::warning(this, tr("FET warning"), tr("Could not start the default internet browser (trying to open the link %1)."
		" Maybe you can try to manually start your browser and open this link.").arg("https://lalescu.ro/liviu/fet/doc/"));
	}
}

void FetMainForm::helpForumAction_triggered()
{
	bool tds=QDesktopServices::openUrl(QUrl("https://lalescu.ro/liviu/fet/forum/"));

	if(!tds){
		QMessageBox::warning(this, tr("FET warning"), tr("Could not start the default internet browser (trying to open the link %1)."
		" Maybe you can try to manually start your browser and open this link.").arg("https://lalescu.ro/liviu/fet/forum/"));
	}
}

void FetMainForm::helpAddressesAction_triggered()
{
	QString s="";
	s+=tr("In case the Help/Online menus do not function, please write down these addresses and open them in an internet browser:");
	s+="\n\n";
	s+=tr("FET homepage: %1", "%1 is FET homepage, begins with https://...").arg("https://lalescu.ro/liviu/fet/");
	s+="\n";
	s+=tr("Documentation (online help contents): %1", "%1 is web page of FET Doc(umentation), which is the contents of the online help, it begins with https://...").arg("https://lalescu.ro/liviu/fet/doc/");
	s+="\n";
	s+=tr("Forum: %1", "%1 is web page of FET forum, begins with https://...").arg("https://lalescu.ro/liviu/fet/forum/");
	s+="\n\n";
	s+=tr("Additionally, you may find on the FET homepage other contact information.");
	s+="\n\n";
	s+=tr("In case these addresses do not function, maybe the FET web page has temporary problems, so try again later. Or maybe the FET web page has changed, so search for the new page on the internet.");

	LongTextMessageBox::largeInformation(this, tr("FET web addresses"), s);
}

void FetMainForm::helpBlockPlanningAction_triggered()
{
	HelpBlockPlanningForm* form=new HelpBlockPlanningForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
}

void FetMainForm::helpTermsAction_triggered()
{
	HelpTermsForm* form=new HelpTermsForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
}

void FetMainForm::helpFAQAction_triggered()
{
	HelpFaqForm* form=new HelpFaqForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
}

void FetMainForm::helpTipsAction_triggered()
{
	HelpTipsForm* form=new HelpTipsForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
}

void FetMainForm::helpInstructionsAction_triggered()
{
	HelpInstructionsForm* form=new HelpInstructionsForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
}

void FetMainForm::timetableGenerateAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	int count=0;
	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		Activity* act=gt.rules.activitiesList[i];
		if(act->active)
			count++;
	}
	if(count<1){
		QMessageBox::information(this, tr("FET information"), tr("Please input at least one active activity before generating."));
		return;
	}
	TimetableGenerateForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
	
//	LockUnlock::increaseCommunicationSpinBox();
}

void FetMainForm::timetableGenerateMultipleAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	if(INPUT_FILENAME_XML.isEmpty()){
		QMessageBox::information(this, tr("FET information"),
			tr("Your current data file is unnamed. Please save it under a chosen name before continuing."));
		return;
	}

	int count=0;
	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		Activity* act=gt.rules.activitiesList[i];
		if(act->active)
			count++;
	}
	if(count<1){
		QMessageBox::information(this, tr("FET information"), tr("Please input at least one active activity before generating multiple."));
		return;
	}
	TimetableGenerateMultipleForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();

//	LockUnlock::increaseCommunicationSpinBox();
}

void FetMainForm::timetableViewStudentsDaysHorizontalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready)){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}

	TimetableViewStudentsDaysHorizontalForm *form=new TimetableViewStudentsDaysHorizontalForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
	form->resizeRowsAfterShow();
}

void FetMainForm::timetableViewStudentsTimeHorizontalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready)){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}

	TimetableViewStudentsTimeHorizontalForm *form=new TimetableViewStudentsTimeHorizontalForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
	form->resizeRowsAfterShow();
}

void FetMainForm::timetableViewTeachersDaysHorizontalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready)){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}
	if(gt.rules.nInternalTeachers!=gt.rules.teachersList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some teachers. Please regenerate the timetable and then view it"));
		return;
	}
	
	TimetableViewTeachersDaysHorizontalForm *form=new TimetableViewTeachersDaysHorizontalForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
	form->resizeRowsAfterShow();
}

void FetMainForm::timetableViewTeachersTimeHorizontalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready)){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}
	if(gt.rules.nInternalTeachers!=gt.rules.teachersList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some teachers. Please regenerate the timetable and then view it"));
		return;
	}
	
	TimetableViewTeachersTimeHorizontalForm *form=new TimetableViewTeachersTimeHorizontalForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
	form->resizeRowsAfterShow();
}

void FetMainForm::timetableShowConflictsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready)){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	TimetableShowConflictsForm *form=new TimetableShowConflictsForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
}

void FetMainForm::timetableViewRoomsDaysHorizontalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready)){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}

	TimetableViewRoomsDaysHorizontalForm* form=new TimetableViewRoomsDaysHorizontalForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
	form->resizeRowsAfterShow();
}

void FetMainForm::timetableViewRoomsTimeHorizontalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready)){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}

	TimetableViewRoomsTimeHorizontalForm* form=new TimetableViewRoomsTimeHorizontalForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
	form->resizeRowsAfterShow();
}

void FetMainForm::timetablePrintAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready)){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}
	if(gt.rules.nInternalTeachers!=gt.rules.teachersList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some teachers. Please regenerate the timetable and then view it"));
		return;
	}
	
	StartTimetablePrint::startTimetablePrint(this);
}

void FetMainForm::statisticsPrintAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	StartStatisticsPrint::startStatisticsPrint(this);
}

void FetMainForm::timetableLockAllActivitiesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready)){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	AdvancedLockUnlockForm::lockAll(this);
}

void FetMainForm::timetableUnlockAllActivitiesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready)){
		//QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		QMessageBox::information(this, tr("FET information"), tr("The timetable is not generated, but anyway FET will proceed now"));

		AdvancedLockUnlockForm::unlockAllWithoutTimetable(this);
	
		return;
	}

	//AdvancedLockUnlockForm::unlockAll(this);
	AdvancedLockUnlockForm::unlockAllWithoutTimetable(this);
}

void FetMainForm::timetableLockActivitiesDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready)){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	AdvancedLockUnlockForm::lockDay(this);
}

void FetMainForm::timetableUnlockActivitiesDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready)){
		//QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		QMessageBox::information(this, tr("FET information"), tr("The timetable is not generated, but anyway FET will proceed now"));
		
		AdvancedLockUnlockForm::unlockDayWithoutTimetable(this);
		
		return;
	}

	//AdvancedLockUnlockForm::unlockDay(this);
	AdvancedLockUnlockForm::unlockDayWithoutTimetable(this);
}

void FetMainForm::timetableLockActivitiesEndStudentsDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready)){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	AdvancedLockUnlockForm::lockEndStudentsDay(this);
}

void FetMainForm::timetableUnlockActivitiesEndStudentsDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready)){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	AdvancedLockUnlockForm::unlockEndStudentsDay(this);
}

void FetMainForm::timetableLockActivitiesWithASpecifiedActivityTagAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(gt.rules.activityTagsList.count()==0){
		QMessageBox::information(this, tr("FET information"), tr("You have no activity tags defined in your data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready)){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	AdvancedLockUnlockForm::lockActivityTag(this);
}

void FetMainForm::timetableUnlockActivitiesWithASpecifiedActivityTagAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(gt.rules.activityTagsList.count()==0){
		QMessageBox::information(this, tr("FET information"), tr("You have no activity tags defined in your data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready)){
		//QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		QMessageBox::information(this, tr("FET information"), tr("The timetable is not generated, but anyway FET will proceed now"));
		
		AdvancedLockUnlockForm::unlockActivityTagWithoutTimetable(this);
		
		return;
	}

	//AdvancedLockUnlockForm::unlockActivityTag(this);
	AdvancedLockUnlockForm::unlockActivityTagWithoutTimetable(this);
}

void FetMainForm::timetableLockActivitiesWithAdvancedFilterAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready)){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	AdvancedLockUnlockForm::lockAdvancedFilter(this);
}

void FetMainForm::timetableUnlockActivitiesWithAdvancedFilterAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready)){
		//QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		QMessageBox::information(this, tr("FET information"), tr("The timetable is not generated, but anyway FET will proceed now"));
		
		AdvancedLockUnlockForm::unlockAdvancedFilterWithoutTimetable(this);
		
		return;
	}

	//AdvancedLockUnlockForm::unlockActivityTag(this);
	AdvancedLockUnlockForm::unlockAdvancedFilterWithoutTimetable(this);
}

void FetMainForm::languageAction_triggered()
{
	QDialog dialog(this);
	dialog.setWindowTitle(tr("Please select FET language"));
	
	QVBoxLayout* taMainLayout=new QVBoxLayout(&dialog);

	QPushButton* tapb1=new QPushButton(tr("Cancel"));
	QPushButton* tapb2=new QPushButton(tr("OK"));
	
	QHBoxLayout* buttons=new QHBoxLayout();
	buttons->addStretch();
	buttons->addWidget(tapb1);
	buttons->addWidget(tapb2);
	
	QComboBox* languagesComboBox=new QComboBox();
	
	QSize tmp=languagesComboBox->minimumSizeHint();
	Q_UNUSED(tmp);
	
	QMap<QString, QString> languagesMap;
	populateLanguagesMap(languagesMap);
	
	//assert(languagesMap.count()==N_LANGUAGES);
	
	QMap<QString, QString>::const_iterator it=languagesMap.constBegin();
	int i=0;
	int j=-1;
	int eng=-1;
	while(it!=languagesMap.constEnd()){
		//languagesComboBox->addItem( it.key() + " (" + it.value() + ")" );
		languagesComboBox->addItem( tr("%1 (%2)", "%1 is the international abbreviation of the language, %2 is the name of the language, untranslated")
		 .arg(it.key())
		 .arg(it.value()) );
		if(it.key()==FET_LANGUAGE)
			j=i;
		if(it.key()=="en_US")
			eng=i;
		i++;
		it++;
	}
	
	assert(eng>=0);
	if(j==-1){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid current language - making it en_US (US English)"));
		FET_LANGUAGE="en_US";
		j=eng;
	}
	languagesComboBox->setCurrentIndex(j);
	
	QLabel* label=new QLabel(tr("Please select FET language"));
	
	QHBoxLayout* languagesLayout=new QHBoxLayout();
	languagesLayout->addWidget(languagesComboBox);
	//languagesLayout->addStretch();
	
	taMainLayout->addStretch();
	taMainLayout->addWidget(label);
	//taMainLayout->addWidget(languagesComboBox);
	taMainLayout->addLayout(languagesLayout);
	taMainLayout->addStretch();
	taMainLayout->addLayout(buttons);

	connect(tapb2, &QPushButton::clicked, &dialog, &QDialog::accept);
	connect(tapb1, &QPushButton::clicked, &dialog, &QDialog::reject);
	
	tapb2->setDefault(true);
	tapb2->setFocus();
	
	const QString settingsName=QString("LanguageSelectionForm");

	int w=dialog.sizeHint().width();
	if(w<350)
		w=350;
	int h=dialog.sizeHint().height();
	if(h<180)
		h=180;
	dialog.resize(w,h);
	centerWidgetOnScreen(&dialog);
	restoreFETDialogGeometry(&dialog, settingsName);
	
	setParentAndOtherThings(&dialog, this);
	bool ok=dialog.exec();
	saveFETDialogGeometry(&dialog, settingsName);
	if(!ok)
		return;
		
	//QString newLang=languagesComboBox->currentText();
	int k=languagesComboBox->currentIndex();
	i=0;
	bool found=false;

	QMap<QString, QString>::const_iterator it2=languagesMap.constBegin();
	while(it2!=languagesMap.constEnd()){
		if(i==k){
			FET_LANGUAGE=it2.key();
			found=true;
		}
		i++;
		it2++;
	}

	if(!found){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid language selected - making it en_US (US English)"));
		FET_LANGUAGE="en_US";
	}
	
	setLanguage(*pqapplication, this);
	setCurrentFile(INPUT_FILENAME_XML);

	//QMessageBox::information(this, tr("FET information"), tr("Language %1 selected").arg( FET_LANGUAGE+" ("+languagesMap.value(FET_LANGUAGE)+")" )+"\n\n"+
	// tr("Please exit and restart FET to activate language change"));
}

void FetMainForm::settingsRestoreDefaultsAction_triggered()
{
	QString default_working_directory="examples";
	QDir d2(default_working_directory);
	if(!d2.exists())
		default_working_directory=QDir::homePath();
	else
		default_working_directory=d2.absolutePath();

	QString s=tr("Are you sure you want to reset all settings to defaults?");
	s+="\n\n";
	
	s+=tr("That means:");
	s+="\n";

	s+=tr("1")+QString(". ")+tr("The geometry and any other saved settings of all the windows and dialogs will be reset to default");
	s+="\n";

	s+=tr("2")+QString(". ")+tr("Show shortcut buttons in main window will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";

	s+=tr("3")+QString(". ")+tr("In the shortcuts tab from the main window, the first section will be selected/shown", "Section refers to the main window tab widget for shortcuts, which currently contains 5 tabs: File, Data, "
		"Time, Space, Timetable (so it will select/show File tab).");
	s+="\n";

	s+=tr("4")+QString(". ")+tr("Check for updates at startup will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

	s+=tr("5")+QString(". ")+tr("Use colors in FET graphical user interface will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

#ifndef USE_SYSTEM_LOCALE
	s+=tr("6")+QString(". ")+tr("Language will be %1", "%1 is the default language").arg(QString("en_US")+QString(" (")+tr("US English")+QString(")"));
#else
	QMap<QString, QString> languagesMap;
	populateLanguagesMap(languagesMap);

	QString NEW_FET_LANGUAGE=QLocale::system().name();
	
	bool ok=false;
	QMap<QString, QString>::const_iterator i=languagesMap.constBegin();
	while(i!=languagesMap.constEnd()){
		if(NEW_FET_LANGUAGE.left(i.key().length())==i.key()){
			NEW_FET_LANGUAGE=i.key();
			ok=true;
			break;
		}
		i++;
	}
	if(!ok)
		NEW_FET_LANGUAGE="en_US";
		
	assert(languagesMap.contains(NEW_FET_LANGUAGE));
	s+=tr("6")+QString(". ")+tr("Language will be %1", "%1 is the default language").arg(NEW_FET_LANGUAGE+QString(" (")+languagesMap.value(NEW_FET_LANGUAGE)+QString(")"));
#endif
	s+="\n";

	s+=tr("7")+QString(". ")+tr("The list of recently used files will be cleared");
	s+="\n";
	
	s+=tr("8")+QString(". ")+tr("Working directory will be %1", "%1 is the directory").arg(QDir::toNativeSeparators(default_working_directory));
	s+="\n";

	s+=tr("9")+QString(". ")+tr("Output directory will be %1", "%1 is the directory").arg(QDir::toNativeSeparators(QDir::homePath()+FILE_SEP+"fet-results"));
	s+="\n";

	s+=tr("10")+QString(". ")+tr("Import directory will be %1", "%1 is the directory").arg(QDir::toNativeSeparators(QDir::homePath()+FILE_SEP+"fet-results"));
	s+="\n";

	s+=tr("11")+QString(". ")+tr("HTML level of the timetables will be %1", "%1 is default HTML level").arg(2);
	s+="\n";

	s+=tr("12")+QString(". ")+tr("Mark not available slots with -x- in timetables will be %1", "%1 is true or false. Lowercase -x-").arg(tr("true"));
	s+="\n";

	s+=tr("13")+QString(". ")+tr("Mark break slots with -X- in timetables will be %1", "%1 is true or false. Uppercase -X-").arg(tr("true"));
	s+="\n";

	s+=tr("14")+QString(". ")+tr("Divide HTML timetables with time axis by days will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

	s+=tr("15")+QString(". ")+tr("Duplicate vertical headers to the right will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

	s+=tr("16")+QString(". ")+tr("Print activities with same starting time will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

	s+=tr("17")+QString(". ")+tr("Print subjects in timetables will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";

	s+=tr("18")+QString(". ")+tr("Print activity tags in timetables will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";

	s+=tr("19")+QString(". ")+tr("Print teachers in timetables will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";

	s+=tr("20")+QString(". ")+tr("Print students in timetables will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";

	s+=tr("21")+QString(". ")+tr("Print rooms in timetables will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";

	s+=tr("22")+QString(". ")+tr("Enable activity tag max hours daily will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

	s+=tr("23")+QString(". ")+tr("Enable activity tag min hours daily will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

	s+=tr("24")+QString(". ")+tr("Enable students max gaps per day will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

	s+=tr("25")+QString(". ")+tr("Warn if using not perfect constraints will be %1", "%1 is true or false. This is a warning if user uses not perfect constraints").arg(tr("true"));
	s+="\n";

	s+=tr("26")+QString(". ")+tr("Enable constraints students min hours daily with empty days will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

	s+=tr("27")+QString(". ")+tr("Warn if using constraints students min hours daily with empty days will be %1", "%1 is true or false. This is a warning if user uses a nonstandard constraint"
		" students min hours daily with allowed empty days").arg(tr("true"));
	s+="\n";

	///////////////confirmations
	s+=tr("28")+QString(". ")+tr("Confirm activity planning will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";
	s+=tr("29")+QString(". ")+tr("Confirm spread activities over the week will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";
	s+=tr("30")+QString(". ")+tr("Confirm remove redundant constraints will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";
	s+=tr("31")+QString(". ")+tr("Confirm save data and timetable as will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";
	///////////////
	
	s+=tr("32")+QString(". ")+tr("Enable group activities in the initial order of generation will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";
	s+=tr("33")+QString(". ")+tr("Warn if using group activities in the initial order of generation will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";
	///////////////

	s+=tr("34")+QString(". ")+tr("Show subgroups in combo boxes will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";
	s+=tr("35")+QString(". ")+tr("Show subgroups in activity planning will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";
	///////////////

	s+=tr("36")+QString(". ")+tr("Write on disk the %1 timetable will be %2", "%1 is a category of timetables, like conflicts, %2 is true or false")
	 .arg(tr("conflicts")).arg(tr("true"));
	s+="\n";

	s+=tr("37")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("statistics")).arg(tr("true"));
	s+="\n";
	s+=tr("38")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("XML")).arg(tr("true"));
	s+="\n";
	s+=tr("39")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("days horizontal")).arg(tr("true"));
	s+="\n";
	s+=tr("40")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("days vertical")).arg(tr("true"));
	s+="\n";
	s+=tr("41")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("time horizontal")).arg(tr("true"));
	s+="\n";
	s+=tr("42")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("time vertical")).arg(tr("true"));
	s+="\n";

	s+=tr("43")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("subgroups")).arg(tr("true"));
	s+="\n";
	s+=tr("44")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("groups")).arg(tr("true"));
	s+="\n";
	s+=tr("45")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("years")).arg(tr("true"));
	s+="\n";
	s+=tr("46")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("teachers")).arg(tr("true"));
	s+="\n";
	s+=tr("47")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("teachers free periods")).arg(tr("true"));
	s+="\n";
	s+=tr("48")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("buildings")).arg(tr("true"));
	s+="\n";
	s+=tr("49")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("rooms")).arg(tr("true"));
	s+="\n";
	s+=tr("50")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("subjects")).arg(tr("true"));
	s+="\n";
	s+=tr("51")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("activity tags")).arg(tr("true"));
	s+="\n";
	s+=tr("52")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("activities")).arg(tr("true"));
	s+="\n";
	s+=tr("53")+QString(". ")+tr("Show tool tips for constraints with tables will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";
	s+=tr("54")+QString(". ")+tr("Show warning for subgroups with the same activities will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";
	s+=tr("55")+QString(". ")+tr("Print detailed timetables will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";
	s+=tr("56")+QString(". ")+tr("Print detailed teachers' free periods timetables will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";
	
	//Just to have them translated, in case I need them.
	QString t1=tr("simple", "It is a style for students' combo boxes");
	Q_UNUSED(t1);
	QString t2=tr("icons", "It is a style for students' combo boxes");
	Q_UNUSED(t2);
	QString t3=tr("categorized", "It is a style for students' combo boxes");
	Q_UNUSED(t3);
	
	s+=tr("57")+QString(". ")+tr("Students' combo boxes style will be %1").arg(tr("simple", "It is a style for students' combo boxes"));
	s+="\n";
	
	s+=tr("58")+QString(". ")+tr("Print virtual rooms in the timetables will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

	s+=tr("59")+QString(". ")+tr("Show warning for activities not locked in time but locked in space in virtual rooms specifying the"
	 " real rooms will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";

	s+=tr("60")+QString(". ")+tr("Beep at the end of the generation will be %1, run external command at the end of generation will be %2,"
	 " and the external command will be empty",
	 "%1 and %2 are true or false").arg(tr("true")).arg(tr("false"));
	s+="\n";

	s+=tr("61")+QString(". ")+tr("Show warning if using constraints of type max hours daily with a weight less than 100%"
	 " will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";

	s+=tr("62")+QString(". ")+tr("Enable max gaps per real day / per week for real days will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

	s+=tr("63")+QString(". ")+tr("Enable constraints students min hours per morning with empty mornings will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

	s+=tr("64")+QString(". ")+tr("Warn if using constraints students min hours per morning with empty mornings will be %1", "%1 is true or false. This is a warning if user uses a nonstandard constraint"
		" students min hours per morning with allowed empty mornings").arg(tr("true"));
	s+="\n";

	s+=tr("65")+QString(". ")+tr("Write HTML timetables for subgroups in sorted order will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";
	
	s+=tr("66")+QString(". ")+tr("The Boolean value 'The font is user selectable' will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

	s+=tr("67")+QString(". ")+tr("The font will be reset to default");
	s+="\n";

	s+=tr("68")+QString(". ")+tr("Enable constraints students min hours per afternoon with empty afternoons will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

	s+=tr("69")+QString(". ")+tr("Warn if using constraints students min hours per afternoon with empty afternoons will be %1", "%1 is true or false. This is a warning if user uses a nonstandard constraint"
		" students min hours per morning with allowed empty mornings").arg(tr("true"));
	s+="\n";

	s+=tr("70")+QString(". ")+tr("Enable save and restore history to/from the memory will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";

	s+=tr("71")+QString(". ")+tr("The number of states to record in history to the memory will be %1", "%1 is a number").arg(100);
	s+="\n";

	s+="(";
	s+=tr("If the history settings will change, the history will be cleared.");
	s+=")\n";

	s+=tr("72")+QString(". ")+tr("Confirm activating/deactivating activities/constraints will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";

	s+=tr("73")+QString(". ")+tr("Enable file autosave will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";
	s+=tr("74")+QString(". ")+tr("The number of minutes before autosave will be %1", "%1 is an integer").arg(3);
	s+="\n";
	s+=tr("75")+QString(". ")+tr("The number of operations before autosave will be %1", "%1 is an integer").arg(1);
	s+="\n";
	s+=tr("76")+QString(". ")+tr("The directory for autosave will be '%1'", "%1 is a directory name").arg(QString(""));
	s+="\n";
	s+=tr("77")+QString(". ")+tr("The file name suffix for autosave will be '%1'", "%1 is a suffix to be added to the file name").arg(QString("_AUTOSAVE"));
	s+="\n";

	s+=tr("78")+QString(". ")+tr("Enable save and restore history to/from the disk will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";
	s+=tr("79")+QString(". ")+tr("The number of states to record in history to the disk will be %1", "%1 is a number").arg(20);
	s+="\n";
	s+=tr("80")+QString(". ")+tr("The file name suffix for saving the history to the disk will be '%1'", "%1 is a suffix to be added to the file name").arg(QString(".his"));
	s+="\n";

	//s+=tr("71")+QString(". ")+tr("The compression level for the states in history will be %1 (the default compression level for zlib)").arg(-1);
	//s+="\n";

	switch( LongTextMessageBox::largeConfirmation( this, tr("FET confirmation"), s,
	 tr("&Yes"), tr("&No"), QString(), 0 , 1 ) ) {
	case 0: // Yes
		break;
	case 1: // No
		return;
	}

	QSettings settings(COMPANY, PROGRAM);
	settings.clear();
	
	fontIsUserSelectable=false;
	userChoseAFont=false;
	qApp->setFont(originalFont);
	
	recentFiles.clear();
	updateRecentFileActions();

	setWindowState(windowState() & (~Qt::WindowMaximized));
	resize(ORIGINAL_WIDTH, ORIGINAL_HEIGHT);
	//move(ORIGINAL_X, ORIGINAL_Y);
	forceCenterWidgetOnScreen(this);
	
#ifndef USE_SYSTEM_LOCALE
	FET_LANGUAGE=QString("en_US");
#else
	FET_LANGUAGE=NEW_FET_LANGUAGE;
#endif
	
	checkForUpdatesAction->setChecked(false);
	checkForUpdates=false;
	
	SHOW_SHORTCUTS_ON_MAIN_WINDOW=true;
	settingsShowShortcutsOnMainWindowAction->setChecked(SHOW_SHORTCUTS_ON_MAIN_WINDOW);
	tabWidget->setVisible(SHOW_SHORTCUTS_ON_MAIN_WINDOW);

	tabWidget->setCurrentIndex(0);
	
	SHOW_TOOLTIPS_FOR_CONSTRAINTS_WITH_TABLES=false;
	settingsShowToolTipsForConstraintsWithTablesAction->setChecked(SHOW_TOOLTIPS_FOR_CONSTRAINTS_WITH_TABLES);
	
	disconnect(settingsUseColorsAction, &QAction::toggled, this, &FetMainForm::useColorsToggled);
	USE_GUI_COLORS=false;
	settingsUseColorsAction->setChecked(USE_GUI_COLORS);
	connect(settingsUseColorsAction, &QAction::toggled, this, &FetMainForm::useColorsToggled);
	
	SHOW_SUBGROUPS_IN_COMBO_BOXES=true;
	settingsShowSubgroupsInComboBoxesAction->setChecked(SHOW_SUBGROUPS_IN_COMBO_BOXES);
	
	SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING=true;
	settingsShowSubgroupsInActivityPlanningAction->setChecked(SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING);

	///
	TIMETABLES_SUBGROUPS_SORTED=false;
	
	WRITE_TIMETABLE_CONFLICTS=true;

	WRITE_TIMETABLES_STATISTICS=true;
	WRITE_TIMETABLES_XML=true;
	WRITE_TIMETABLES_DAYS_HORIZONTAL=true;
	WRITE_TIMETABLES_DAYS_VERTICAL=true;
	WRITE_TIMETABLES_TIME_HORIZONTAL=true;
	WRITE_TIMETABLES_TIME_VERTICAL=true;

	WRITE_TIMETABLES_SUBGROUPS=true;
	WRITE_TIMETABLES_GROUPS=true;
	WRITE_TIMETABLES_YEARS=true;
	WRITE_TIMETABLES_TEACHERS=true;
	WRITE_TIMETABLES_TEACHERS_FREE_PERIODS=true;
	WRITE_TIMETABLES_BUILDINGS=true;
	WRITE_TIMETABLES_ROOMS=true;
	WRITE_TIMETABLES_SUBJECTS=true;
	WRITE_TIMETABLES_ACTIVITY_TAGS=true;
	WRITE_TIMETABLES_ACTIVITIES=true;
	
	STUDENTS_COMBO_BOXES_STYLE=STUDENTS_COMBO_BOXES_STYLE_SIMPLE;
	//
	
	////////confirmations
	CONFIRM_ACTIVITY_PLANNING=true;
	settingsConfirmActivityPlanningAction->setChecked(CONFIRM_ACTIVITY_PLANNING);
	
	CONFIRM_SPREAD_ACTIVITIES=true;
	settingsConfirmSpreadActivitiesAction->setChecked(CONFIRM_SPREAD_ACTIVITIES);
	
	CONFIRM_REMOVE_REDUNDANT=true;
	settingsConfirmRemoveRedundantAction->setChecked(CONFIRM_REMOVE_REDUNDANT);
	
	CONFIRM_SAVE_TIMETABLE=true;
	settingsConfirmSaveTimetableAction->setChecked(CONFIRM_SAVE_TIMETABLE);

	CONFIRM_ACTIVATE_DEACTIVATE_ACTIVITIES_CONSTRAINTS=true;
	settingsConfirmActivateDeactivateActivitiesConstraintsAction->setChecked(CONFIRM_ACTIVATE_DEACTIVATE_ACTIVITIES_CONSTRAINTS);
	///////

	///////////
	SHOW_WARNING_FOR_SUBGROUPS_WITH_THE_SAME_ACTIVITIES=true;
	showWarningForSubgroupsWithTheSameActivitiesAction->setChecked(SHOW_WARNING_FOR_SUBGROUPS_WITH_THE_SAME_ACTIVITIES);
	
	SHOW_WARNING_FOR_ACTIVITIES_FIXED_SPACE_VIRTUAL_REAL_ROOMS_BUT_NOT_FIXED_TIME=true;
	showWarningForActivitiesNotLockedTimeLockedSpaceVirtualRealRoomsAction->setChecked(SHOW_WARNING_FOR_ACTIVITIES_FIXED_SPACE_VIRTUAL_REAL_ROOMS_BUT_NOT_FIXED_TIME);
	
	SHOW_WARNING_FOR_MAX_HOURS_DAILY_WITH_UNDER_100_WEIGHT=true;
	showWarningForMaxHoursDailyWithUnder100WeightAction->setChecked(SHOW_WARNING_FOR_MAX_HOURS_DAILY_WITH_UNDER_100_WEIGHT);
	
	ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY=false;
	enableActivityTagMaxHoursDailyAction->setChecked(ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);

	ENABLE_ACTIVITY_TAG_MIN_HOURS_DAILY=false;
	enableActivityTagMinHoursDailyAction->setChecked(ENABLE_ACTIVITY_TAG_MIN_HOURS_DAILY);

	ENABLE_STUDENTS_MAX_GAPS_PER_DAY=false;
	enableStudentsMaxGapsPerDayAction->setChecked(ENABLE_STUDENTS_MAX_GAPS_PER_DAY);
	
	ENABLE_MAX_GAPS_PER_REAL_DAY=false;
	disconnect(enableMaxGapsPerRealDayAction, &QAction::toggled, this, &FetMainForm::enableMaxGapsPerRealDayToggled);
	enableMaxGapsPerRealDayAction->setChecked(ENABLE_MAX_GAPS_PER_REAL_DAY);
	connect(enableMaxGapsPerRealDayAction, &QAction::toggled, this, &FetMainForm::enableMaxGapsPerRealDayToggled);

	SHOW_WARNING_FOR_NOT_PERFECT_CONSTRAINTS=true;
	showWarningForNotPerfectConstraintsAction->setChecked(SHOW_WARNING_FOR_NOT_PERFECT_CONSTRAINTS);
	
	ENABLE_GROUP_ACTIVITIES_IN_INITIAL_ORDER=false;
	SHOW_WARNING_FOR_GROUP_ACTIVITIES_IN_INITIAL_ORDER=true;
	enableGroupActivitiesInInitialOrderAction->setChecked(ENABLE_GROUP_ACTIVITIES_IN_INITIAL_ORDER);
	showWarningForGroupActivitiesInInitialOrderAction->setChecked(SHOW_WARNING_FOR_GROUP_ACTIVITIES_IN_INITIAL_ORDER);
	
	setEnabledIcon(dataTimeConstraintsTeacherActivityTagMaxHoursDailyAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsTeachersActivityTagMaxHoursDailyAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsStudentsActivityTagMaxHoursDailyAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);

	setEnabledIcon(dataTimeConstraintsTeacherActivityTagMaxHoursDailyRealDaysAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsTeachersActivityTagMaxHoursDailyRealDaysAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsStudentsActivityTagMaxHoursDailyRealDaysAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyRealDaysAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);

	setEnabledIcon(dataTimeConstraintsTeacherActivityTagMinHoursDailyAction, ENABLE_ACTIVITY_TAG_MIN_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsTeachersActivityTagMinHoursDailyAction, ENABLE_ACTIVITY_TAG_MIN_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsStudentsActivityTagMinHoursDailyAction, ENABLE_ACTIVITY_TAG_MIN_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsStudentsSetActivityTagMinHoursDailyAction, ENABLE_ACTIVITY_TAG_MIN_HOURS_DAILY);

	setEnabledIcon(dataTimeConstraintsStudentsSetMaxGapsPerDayAction, ENABLE_STUDENTS_MAX_GAPS_PER_DAY);
	setEnabledIcon(dataTimeConstraintsStudentsMaxGapsPerDayAction, ENABLE_STUDENTS_MAX_GAPS_PER_DAY);

	setEnabledIcon(dataTimeConstraintsStudentsSetMaxGapsPerRealDayAction, ENABLE_MAX_GAPS_PER_REAL_DAY);
	setEnabledIcon(dataTimeConstraintsStudentsMaxGapsPerRealDayAction, ENABLE_MAX_GAPS_PER_REAL_DAY);
	setEnabledIcon(dataTimeConstraintsTeachersMaxGapsPerRealDayAction, ENABLE_MAX_GAPS_PER_REAL_DAY);
	setEnabledIcon(dataTimeConstraintsTeacherMaxGapsPerRealDayAction, ENABLE_MAX_GAPS_PER_REAL_DAY);

	setEnabledIcon(dataTimeConstraintsStudentsSetMaxGapsPerWeekForRealDaysAction, ENABLE_MAX_GAPS_PER_REAL_DAY);
	setEnabledIcon(dataTimeConstraintsStudentsMaxGapsPerWeekForRealDaysAction, ENABLE_MAX_GAPS_PER_REAL_DAY);
	setEnabledIcon(dataTimeConstraintsTeachersMaxGapsPerWeekForRealDaysAction, ENABLE_MAX_GAPS_PER_REAL_DAY);
	setEnabledIcon(dataTimeConstraintsTeacherMaxGapsPerWeekForRealDaysAction, ENABLE_MAX_GAPS_PER_REAL_DAY);

	setEnabledIcon(groupActivitiesInInitialOrderAction, ENABLE_GROUP_ACTIVITIES_IN_INITIAL_ORDER);

	ENABLE_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS=false;
	disconnect(enableStudentsMinHoursDailyWithAllowEmptyDaysAction, &QAction::toggled, this, &FetMainForm::enableStudentsMinHoursDailyWithAllowEmptyDaysToggled);
	enableStudentsMinHoursDailyWithAllowEmptyDaysAction->setChecked(ENABLE_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS);
	connect(enableStudentsMinHoursDailyWithAllowEmptyDaysAction, &QAction::toggled, this, &FetMainForm::enableStudentsMinHoursDailyWithAllowEmptyDaysToggled);

	SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS=true;
	disconnect(showWarningForStudentsMinHoursDailyWithAllowEmptyDaysAction, &QAction::toggled, this, &FetMainForm::showWarningForStudentsMinHoursDailyWithAllowEmptyDaysToggled);
	showWarningForStudentsMinHoursDailyWithAllowEmptyDaysAction->setChecked(SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS);
	connect(showWarningForStudentsMinHoursDailyWithAllowEmptyDaysAction, &QAction::toggled, this, &FetMainForm::showWarningForStudentsMinHoursDailyWithAllowEmptyDaysToggled);

	ENABLE_STUDENTS_MIN_HOURS_PER_MORNING_WITH_ALLOW_EMPTY_MORNINGS=false;
	disconnect(enableStudentsMinHoursPerMorningWithAllowEmptyMorningsAction, &QAction::toggled, this, &FetMainForm::enableStudentsMinHoursPerMorningWithAllowEmptyMorningsToggled);
	enableStudentsMinHoursPerMorningWithAllowEmptyMorningsAction->setChecked(ENABLE_STUDENTS_MIN_HOURS_PER_MORNING_WITH_ALLOW_EMPTY_MORNINGS);
	connect(enableStudentsMinHoursPerMorningWithAllowEmptyMorningsAction, &QAction::toggled, this, &FetMainForm::enableStudentsMinHoursPerMorningWithAllowEmptyMorningsToggled);

	SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_PER_MORNING_WITH_ALLOW_EMPTY_MORNINGS=true;
	disconnect(showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsAction, &QAction::toggled, this, &FetMainForm::showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsToggled);
	showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsAction->setChecked(SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_PER_MORNING_WITH_ALLOW_EMPTY_MORNINGS);
	connect(showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsAction, &QAction::toggled, this, &FetMainForm::showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsToggled);

	ENABLE_STUDENTS_MIN_HOURS_PER_AFTERNOON_WITH_ALLOW_EMPTY_AFTERNOONS=false;
	disconnect(enableStudentsMinHoursPerAfternoonWithAllowEmptyAfternoonsAction, &QAction::toggled, this, &FetMainForm::enableStudentsMinHoursPerAfternoonWithAllowEmptyAfternoonsToggled);
	enableStudentsMinHoursPerAfternoonWithAllowEmptyAfternoonsAction->setChecked(ENABLE_STUDENTS_MIN_HOURS_PER_MORNING_WITH_ALLOW_EMPTY_MORNINGS);
	connect(enableStudentsMinHoursPerAfternoonWithAllowEmptyAfternoonsAction, &QAction::toggled, this, &FetMainForm::enableStudentsMinHoursPerAfternoonWithAllowEmptyAfternoonsToggled);

	SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_PER_AFTERNOON_WITH_ALLOW_EMPTY_AFTERNOONS=true;
	disconnect(showWarningForStudentsMinHoursPerAfternoonWithAllowEmptyAfternoonsAction, &QAction::toggled, this, &FetMainForm::showWarningForStudentsMinHoursPerAfternoonWithAllowEmptyAfternoonsToggled);
	showWarningForStudentsMinHoursPerAfternoonWithAllowEmptyAfternoonsAction->setChecked(SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_PER_AFTERNOON_WITH_ALLOW_EMPTY_AFTERNOONS);
	connect(showWarningForStudentsMinHoursPerAfternoonWithAllowEmptyAfternoonsAction, &QAction::toggled, this, &FetMainForm::showWarningForStudentsMinHoursPerAfternoonWithAllowEmptyAfternoonsToggled);

	///////////
	
	settingsDivideTimetablesByDaysAction->setChecked(false);
	DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS=false;
	
	settingsDuplicateVerticalNamesAction->setChecked(false);
	TIMETABLE_HTML_REPEAT_NAMES=false;
	
	WORKING_DIRECTORY=default_working_directory;
	
	OUTPUT_DIR=QDir::homePath()+FILE_SEP+"fet-results";
	QDir dir;
	if(!dir.exists(OUTPUT_DIR))
		dir.mkpath(OUTPUT_DIR);
	IMPORT_DIRECTORY=OUTPUT_DIR;
	
	TIMETABLE_HTML_LEVEL=2;
	
	TIMETABLE_HTML_PRINT_SUBJECTS=true;
	TIMETABLE_HTML_PRINT_ACTIVITY_TAGS=true;
	TIMETABLE_HTML_PRINT_TEACHERS=true;
	TIMETABLE_HTML_PRINT_STUDENTS=true;
	TIMETABLE_HTML_PRINT_ROOMS=true;
	
	settingsPrintDetailedTimetablesAction->setChecked(true);
	PRINT_DETAILED_HTML_TIMETABLES=true;
	settingsPrintDetailedTeachersFreePeriodsTimetablesAction->setChecked(true);
	PRINT_DETAILED_HTML_TEACHERS_FREE_PERIODS=true;

	settingsPrintNotAvailableSlotsAction->setChecked(true);
	PRINT_NOT_AVAILABLE_TIME_SLOTS=true;

	settingsPrintBreakSlotsAction->setChecked(true);
	PRINT_BREAK_TIME_SLOTS=true;

	settingsPrintActivitiesWithSameStartingTimeAction->setChecked(false);
	PRINT_ACTIVITIES_WITH_SAME_STARTING_TIME=false;
	
	settingsShowVirtualRoomsInTimetablesAction->setChecked(false);
	SHOW_VIRTUAL_ROOMS_IN_TIMETABLES=false;
	
	settingsOrderSubgroupsInTimetablesAction->setChecked(false);
	TIMETABLES_SUBGROUPS_SORTED=false;
	
	BEEP_AT_END_OF_GENERATION=true;
	ENABLE_COMMAND_AT_END_OF_GENERATION=false;
	commandAtEndOfGeneration=QString("");
	//DETACHED_NOTIFICATION=false;
	//terminateCommandAfterSeconds=0;
	//killCommandAfterSeconds=0;

	///////////////////////
	/*if(!USE_UNDO_REDO){
		int cs=oldRulesArchived.size();
		assert(cs==int(operationWhichWasDone.size()));
		assert(cs==int(operationDateTime.size()));
		assert(cs==int(unarchivedSizes.size()));
		//assert(cs==int(stateFileName.size()));

		assert(cs==0);
		assert(cntUndoRedoStackIterator==0);

		USE_UNDO_REDO=true;
	}
	else{
		int cs=oldRulesArchived.size();
		assert(cs==int(operationWhichWasDone.size()));
		assert(cs==int(operationDateTime.size()));
		assert(cs==int(unarchivedSizes.size()));
		//assert(cs==int(stateFileName.size()));

		int ns=100;

		if(ns<cs){
			for(int i=0; i<cs-ns; i++){
				assert(!oldRulesArchived.empty());
				oldRulesArchived.pop_front();

				assert(!operationWhichWasDone.empty());
				operationWhichWasDone.pop_front();

				assert(!operationDateTime.empty());
				operationDateTime.pop_front();

				assert(!unarchivedSizes.empty());
				unarchivedSizes.pop_front();

				//assert(!stateFileName.empty());
				//stateFileName.pop_front();
			}

			cntUndoRedoStackIterator-=(cs-ns);
			if(cntUndoRedoStackIterator<0)
				cntUndoRedoStackIterator=0;

			savedStateIterator-=(cs-ns);
			if(savedStateIterator<0)
				savedStateIterator=0;
		}
	}*/

	if(USE_UNDO_REDO==true && UNDO_REDO_STEPS==100){
		//do nothing
	}
	else{
		USE_UNDO_REDO=true;

		UNDO_REDO_STEPS=100;

		//UNDO_REDO_COMPRESSION_LEVEL=-1;

		clearHistory();
		if(gt.rules.initialized && USE_UNDO_REDO){
			gt.rules.addUndoPoint(tr("Cleared the history, because the history settings were modified when resetting all the settings to default."), false, false);
			if(!gt.rules.modified)
				savedStateIterator=cntUndoRedoStackIterator;
		}
	}
	///////////////////////

	USE_UNDO_REDO_SAVE=false;
	UNDO_REDO_STEPS_SAVE=20;
	SUFFIX_FILENAME_SAVE_HISTORY=QString(".his");

	USE_AUTOSAVE=false;
	MINUTES_AUTOSAVE=3;
	OPERATIONS_AUTOSAVE=1;
	DIRECTORY_AUTOSAVE="";
	SUFFIX_FILENAME_AUTOSAVE="_AUTOSAVE";

	setLanguage(*pqapplication, this);
	setCurrentFile(INPUT_FILENAME_XML);

	if(teachers_schedule_ready && students_schedule_ready && rooms_buildings_schedule_ready)
		LockUnlock::increaseCommunicationSpinBox(); //for GUI colors in timetables
}

void FetMainForm::settingsTimetableHtmlLevelAction_triggered()
{
	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	SettingsTimetableHtmlLevelForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::settingsDataToPrintInTimetablesAction_triggered()
{
	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}
	
	SettingsDataToPrintInTimetablesForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::settingsShowVirtualRoomsInTimetablesAction_toggled()
{
	SHOW_VIRTUAL_ROOMS_IN_TIMETABLES=settingsShowVirtualRoomsInTimetablesAction->isChecked();
}

void FetMainForm::settingsOrderSubgroupsInTimetablesAction_toggled()
{
	TIMETABLES_SUBGROUPS_SORTED=settingsOrderSubgroupsInTimetablesAction->isChecked();
}

void FetMainForm::settingsPrintDetailedTimetablesAction_toggled()
{
	PRINT_DETAILED_HTML_TIMETABLES=settingsPrintDetailedTimetablesAction->isChecked();
}

void FetMainForm::settingsPrintDetailedTeachersFreePeriodsTimetablesAction_toggled()
{
	PRINT_DETAILED_HTML_TEACHERS_FREE_PERIODS=settingsPrintDetailedTeachersFreePeriodsTimetablesAction->isChecked();
}

void FetMainForm::settingsPrintNotAvailableSlotsAction_toggled()
{
	PRINT_NOT_AVAILABLE_TIME_SLOTS=settingsPrintNotAvailableSlotsAction->isChecked();
}

void FetMainForm::settingsPrintBreakSlotsAction_toggled()
{
	PRINT_BREAK_TIME_SLOTS=settingsPrintBreakSlotsAction->isChecked();
}

void FetMainForm::settingsPrintActivitiesWithSameStartingTimeAction_toggled()
{
	PRINT_ACTIVITIES_WITH_SAME_STARTING_TIME=settingsPrintActivitiesWithSameStartingTimeAction->isChecked();
}

void FetMainForm::settingsCommandAfterFinishingAction_triggered()
{
	NotificationCommandForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::activityPlanningAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	if(CONFIRM_ACTIVITY_PLANNING){
		int confirm;
	
		ActivityPlanningConfirmationForm c_form(this);
		setParentAndOtherThings(&c_form, this);
		confirm=c_form.exec();

		if(confirm==QDialog::Accepted){
			if(c_form.dontShowAgain)
				settingsConfirmActivityPlanningAction->setChecked(false);
		
			StartActivityPlanning::startActivityPlanning(&c_form);
		}
	}
	else{
		StartActivityPlanning::startActivityPlanning(this);
	}
}

void FetMainForm::spreadActivitiesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}
	
	if(gt.rules.mode!=MORNINGS_AFTERNOONS && gt.rules.nDaysPerWeek>=7){
		QString s;
		s=tr("You have more than 6 days per week, so probably you won't need this feature. Do you still want to continue?");
		
		/*int cfrm=0;
		switch( QMessageBox::question( this, tr("FET question"),
		 s,
		 tr("&Continue"), tr("&Cancel"), 0 , 1 ) ) {
		case 0: // Yes - continue
			cfrm=1;
			break;
		case 1: // No - cancel
			cfrm=0;
			break;
		}

		if(!cfrm){
			return;
		}*/
		QMessageBox::StandardButton cfrm=QMessageBox::question( this, tr("FET question"), s, QMessageBox::Yes | QMessageBox::Cancel);
		if(cfrm==QMessageBox::Cancel)
			return;
	}
	else if(gt.rules.mode==MORNINGS_AFTERNOONS && gt.rules.nDaysPerWeek/2>=7){
		QString s;
		s=tr("You have more than 6 real days per week, so probably you won't need this feature. Do you still want to continue?");
		
		/*int cfrm=0;
		switch( QMessageBox::question( this, tr("FET question"),
		 s,
		 tr("&Continue"), tr("&Cancel"), 0 , 1 ) ) {
		case 0: // Yes - continue
			cfrm=1;
			break;
		case 1: // No - cancel
			cfrm=0;
			break;
		}

		if(!cfrm){
			return;
		}*/
		QMessageBox::StandardButton cfrm=QMessageBox::question( this, tr("FET question"), s, QMessageBox::Yes | QMessageBox::Cancel);
		if(cfrm==QMessageBox::Cancel)
			return;
	}

	if(gt.rules.mode!=MORNINGS_AFTERNOONS && gt.rules.nDaysPerWeek<=4){
		QString s;
		s=tr("You have less than 5 days per week, so probably you won't need this feature. Do you still want to continue?");

		/*int cfrm=0;
		switch( QMessageBox::question( this, tr("FET question"),
		 s,
		 tr("&Continue"), tr("&Cancel"), 0 , 1 ) ) {
		case 0: // Yes - continue
			cfrm=1;
			break;
		case 1: // No - cancel
			cfrm=0;
			break;
		}

		if(!cfrm){
			return;
		}*/
		QMessageBox::StandardButton cfrm=QMessageBox::question( this, tr("FET question"), s, QMessageBox::Yes | QMessageBox::Cancel);
		if(cfrm==QMessageBox::Cancel)
			return;
	}
	else if(gt.rules.mode==MORNINGS_AFTERNOONS && gt.rules.nDaysPerWeek/2<=4){
		QString s;
		s=tr("You have less than 5 real days per week, so probably you won't need this feature. Do you still want to continue?");

		/*int cfrm=0;
		switch( QMessageBox::question( this, tr("FET question"),
		 s,
		 tr("&Continue"), tr("&Cancel"), 0 , 1 ) ) {
		case 0: // Yes - continue
			cfrm=1;
			break;
		case 1: // No - cancel
			cfrm=0;
			break;
		}

		if(!cfrm){
			return;
		}*/
		QMessageBox::StandardButton cfrm=QMessageBox::question( this, tr("FET question"), s, QMessageBox::Yes | QMessageBox::Cancel);
		if(cfrm==QMessageBox::Cancel)
			return;
	}

	if(CONFIRM_SPREAD_ACTIVITIES){
		int confirm;
	
		SpreadConfirmationForm c_form(this);
		setParentAndOtherThings(&c_form, this);
		confirm=c_form.exec();

		if(confirm==QDialog::Accepted){
			if(c_form.dontShowAgain)
				settingsConfirmSpreadActivitiesAction->setChecked(false);
			
			SpreadMinDaysConstraintsFiveDaysForm form(&c_form);
			setParentAndOtherThings(&form, &c_form);
			form.exec();
		}
	}
	else{
		SpreadMinDaysConstraintsFiveDaysForm form(this);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
}

void FetMainForm::statisticsExportToDiskAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	StatisticsExport::exportStatistics(this);
}

void FetMainForm::removeRedundantConstraintsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}
	
	if(CONFIRM_REMOVE_REDUNDANT){
		int confirm;
	
		RemoveRedundantConfirmationForm c_form(this);
		setParentAndOtherThings(&c_form, this);
		confirm=c_form.exec();

		if(confirm==QDialog::Accepted){
			if(c_form.dontShowAgain)
				settingsConfirmRemoveRedundantAction->setChecked(false);

			RemoveRedundantForm form(&c_form);
			setParentAndOtherThings(&form, &c_form);
			form.exec();
		}
	}
	else{
		RemoveRedundantForm form(this);
		setParentAndOtherThings(&form, this);
		form.exec();
	}
}

void FetMainForm::selectOutputDirAction_triggered()
{
	QString od;
	
	od = QFileDialog::getExistingDirectory(this, tr("Choose results (output) directory"), OUTPUT_DIR);
	
	if(!od.isNull()){
		QFile test(od+FILE_SEP+"test_write_permissions_3.tmp");
		bool existedBefore=test.exists();
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
		bool t=test.open(QIODeviceBase::ReadWrite);
#else
		bool t=test.open(QIODevice::ReadWrite);
#endif
		//if(!test.exists())
		//	t=false;
		if(!t){
			QMessageBox::warning(this, tr("FET warning"), tr("You don't have write permissions in this directory"));
			return;
		}
		test.close();
		if(!existedBefore)
			test.remove();
	
		OUTPUT_DIR=od;
	}
}

void FetMainForm::randomSeedAction_triggered()
{
	RandomSeedForm dialog(this);
	
	setParentAndOtherThings(&dialog, this);
	/*int te=*/
	dialog.exec();
	
	/*if(te==QDialog::Accepted){
		int tx=dialog.lineEditX->text().toInt();
		if(!(tx>=1 && tx<MM)){
			assert(0);
			//QMessageBox::warning(this, tr("FET warning"), tr("The random seed X component must be at least %1 and at most %2").arg(1).arg(MM-1));
			//return;
		}

		int ty=dialog.lineEditY->text().toInt();
		if(!(ty>=1 && ty<MMM)){
			assert(0);
			//QMessageBox::warning(this, tr("FET warning"), tr("The random seed Y component must be at least %1 and at most %2").arg(1).arg(MMM-1));
			//return;
		}

		XX=tx;
		YY=ty;
	}*/
}

void FetMainForm::enableActivityTagMaxHoursDailyToggled(bool checked)
{
	if(checked==true){
		QString s=tr("These kinds of constraints are good, but not perfectly optimized. Adding such constraints may make your"
		 " timetable solve too slow or even impossible.");
		s+=" ";
		s+=tr("It is recommended to use such constraints only at the end, after you generated successfully with all the other constraints.");
		s+="\n\n";
		s+=tr("If your timetable is too difficult, it may be from these kinds of constraints, so you may need to remove them and retry.");
		s+="\n\n";
		s+=tr("Continue only if you know what you are doing.");
	
		QMessageBox::StandardButton b=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
	
		if(b!=QMessageBox::Ok){
			disconnect(enableActivityTagMaxHoursDailyAction, &QAction::toggled, this, &FetMainForm::enableActivityTagMaxHoursDailyToggled);
			enableActivityTagMaxHoursDailyAction->setChecked(false);
			connect(enableActivityTagMaxHoursDailyAction, &QAction::toggled, this, &FetMainForm::enableActivityTagMaxHoursDailyToggled);
			return;
		}
	}
	
	ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY=checked;

	setEnabledIcon(dataTimeConstraintsTeacherActivityTagMaxHoursDailyAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsTeachersActivityTagMaxHoursDailyAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsStudentsActivityTagMaxHoursDailyAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);

	setEnabledIcon(dataTimeConstraintsTeacherActivityTagMaxHoursDailyRealDaysAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsTeachersActivityTagMaxHoursDailyRealDaysAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsStudentsActivityTagMaxHoursDailyRealDaysAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyRealDaysAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
}

void FetMainForm::enableActivityTagMinHoursDailyToggled(bool checked)
{
	if(checked==true){
		QString s=tr("These kinds of constraints are good, but not perfectly optimized. Adding such constraints may make your"
		 " timetable solve too slow or even impossible.");
		s+=" ";
		s+=tr("It is recommended to use such constraints only at the end, after you generated successfully with all the other constraints.");
		s+="\n\n";
		s+=tr("If your timetable is too difficult, it may be from these kinds of constraints, so you may need to remove them and retry.");
		s+="\n\n";
		s+=tr("Continue only if you know what you are doing.");
	
		QMessageBox::StandardButton b=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
	
		if(b!=QMessageBox::Ok){
			disconnect(enableActivityTagMinHoursDailyAction, &QAction::toggled, this, &FetMainForm::enableActivityTagMinHoursDailyToggled);
			enableActivityTagMinHoursDailyAction->setChecked(false);
			connect(enableActivityTagMinHoursDailyAction, &QAction::toggled, this, &FetMainForm::enableActivityTagMinHoursDailyToggled);
			return;
		}
	}
	
	ENABLE_ACTIVITY_TAG_MIN_HOURS_DAILY=checked;

	setEnabledIcon(dataTimeConstraintsTeacherActivityTagMinHoursDailyAction, ENABLE_ACTIVITY_TAG_MIN_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsTeachersActivityTagMinHoursDailyAction, ENABLE_ACTIVITY_TAG_MIN_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsStudentsActivityTagMinHoursDailyAction, ENABLE_ACTIVITY_TAG_MIN_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsStudentsSetActivityTagMinHoursDailyAction, ENABLE_ACTIVITY_TAG_MIN_HOURS_DAILY);
}

void FetMainForm::enableStudentsMaxGapsPerDayToggled(bool checked)
{
	if(checked==true){
		QString s=tr("These kinds of constraints are good, but not perfectly optimized. Adding such constraints may make your"
		 " timetable solve too slow or even impossible.");
		s+=" ";
		s+=tr("It is recommended to use such constraints only at the end, after you generated successfully with all the other constraints.");
		s+="\n\n";
		s+=tr("If your timetable is too difficult, it may be from these kinds of constraints, so you may need to remove them and retry.");
		s+="\n\n";
		s+=tr("Continue only if you know what you are doing.");
	
		QMessageBox::StandardButton b=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
	
		if(b!=QMessageBox::Ok){
			disconnect(enableStudentsMaxGapsPerDayAction, &QAction::toggled, this, &FetMainForm::enableStudentsMaxGapsPerDayToggled);
			enableStudentsMaxGapsPerDayAction->setChecked(false);
			connect(enableStudentsMaxGapsPerDayAction, &QAction::toggled, this, &FetMainForm::enableStudentsMaxGapsPerDayToggled);
			return;
		}
	}
	
	ENABLE_STUDENTS_MAX_GAPS_PER_DAY=checked;
	
	setEnabledIcon(dataTimeConstraintsStudentsSetMaxGapsPerDayAction, ENABLE_STUDENTS_MAX_GAPS_PER_DAY);
	setEnabledIcon(dataTimeConstraintsStudentsMaxGapsPerDayAction, ENABLE_STUDENTS_MAX_GAPS_PER_DAY);
}

void FetMainForm::enableMaxGapsPerRealDayToggled(bool checked)
{
	assert(gt.rules.mode==MORNINGS_AFTERNOONS);
	if(checked==true){
		QString s=tr("These kinds of constraints are good, but not perfectly optimized. Adding such constraints may make your"
		 " timetable solve too slow or even impossible.");
		s+=" ";
		s+=tr("It is recommended to use such constraints only at the end, after you generated successfully with all the other constraints.");
		s+="\n\n";
		s+=tr("If your timetable is too difficult, it may be from these kinds of constraints, so you may need to remove them and retry.");
		s+="\n\n";
		s+=tr("Continue only if you know what you are doing.");

		QMessageBox::StandardButton b=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);

		if(b!=QMessageBox::Ok){
			disconnect(enableMaxGapsPerRealDayAction, &QAction::toggled, this, &FetMainForm::enableMaxGapsPerRealDayToggled);
			enableMaxGapsPerRealDayAction->setChecked(false);
			connect(enableMaxGapsPerRealDayAction, &QAction::toggled, this, &FetMainForm::enableMaxGapsPerRealDayToggled);
			return;
		}
	}

	ENABLE_MAX_GAPS_PER_REAL_DAY=checked;

	setEnabledIcon(dataTimeConstraintsStudentsSetMaxGapsPerRealDayAction, ENABLE_MAX_GAPS_PER_REAL_DAY);
	setEnabledIcon(dataTimeConstraintsStudentsMaxGapsPerRealDayAction, ENABLE_MAX_GAPS_PER_REAL_DAY);
	setEnabledIcon(dataTimeConstraintsTeachersMaxGapsPerRealDayAction, ENABLE_MAX_GAPS_PER_REAL_DAY);
	setEnabledIcon(dataTimeConstraintsTeacherMaxGapsPerRealDayAction, ENABLE_MAX_GAPS_PER_REAL_DAY);

	setEnabledIcon(dataTimeConstraintsStudentsSetMaxGapsPerWeekForRealDaysAction, ENABLE_MAX_GAPS_PER_REAL_DAY);
	setEnabledIcon(dataTimeConstraintsStudentsMaxGapsPerWeekForRealDaysAction, ENABLE_MAX_GAPS_PER_REAL_DAY);
	setEnabledIcon(dataTimeConstraintsTeachersMaxGapsPerWeekForRealDaysAction, ENABLE_MAX_GAPS_PER_REAL_DAY);
	setEnabledIcon(dataTimeConstraintsTeacherMaxGapsPerWeekForRealDaysAction, ENABLE_MAX_GAPS_PER_REAL_DAY);
}

void FetMainForm::showWarningForSubgroupsWithTheSameActivitiesToggled(bool checked)
{
	if(checked==false){
		QString s=tr("It is recommended to keep this warning active, but if you really want, you can disable it.");
		s+="\n\n";
		s+=tr("Disable it only if you know what you are doing.");
		s+="\n\n";
		s+=tr("Are you sure you want to disable it?");
	
		QMessageBox::StandardButton b=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes);
	
		if(b!=QMessageBox::Yes){
			disconnect(showWarningForSubgroupsWithTheSameActivitiesAction, &QAction::toggled, this, &FetMainForm::showWarningForSubgroupsWithTheSameActivitiesToggled);
			showWarningForSubgroupsWithTheSameActivitiesAction->setChecked(true);
			connect(showWarningForSubgroupsWithTheSameActivitiesAction, &QAction::toggled, this, &FetMainForm::showWarningForSubgroupsWithTheSameActivitiesToggled);
			return;
		}
	}
	
	SHOW_WARNING_FOR_SUBGROUPS_WITH_THE_SAME_ACTIVITIES=checked;
}

void FetMainForm::showWarningForActivitiesNotLockedTimeLockedSpaceVirtualRealRoomsToggled(bool checked)
{
	if(checked==false){
		QString s=tr("It is recommended to keep this warning active, but if you really want, you can disable it.");
		s+="\n\n";
		s+=tr("Disable it only if you know what you are doing.");
		s+="\n\n";
		s+=tr("Are you sure you want to disable it?");
	
		QMessageBox::StandardButton b=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes);
	
		if(b!=QMessageBox::Yes){
			disconnect(showWarningForActivitiesNotLockedTimeLockedSpaceVirtualRealRoomsAction, &QAction::toggled, this, &FetMainForm::showWarningForActivitiesNotLockedTimeLockedSpaceVirtualRealRoomsToggled);
			showWarningForActivitiesNotLockedTimeLockedSpaceVirtualRealRoomsAction->setChecked(true);
			connect(showWarningForActivitiesNotLockedTimeLockedSpaceVirtualRealRoomsAction, &QAction::toggled, this, &FetMainForm::showWarningForActivitiesNotLockedTimeLockedSpaceVirtualRealRoomsToggled);
			return;
		}
	}
	
	SHOW_WARNING_FOR_ACTIVITIES_FIXED_SPACE_VIRTUAL_REAL_ROOMS_BUT_NOT_FIXED_TIME=checked;
}

void FetMainForm::showWarningForMaxHoursDailyWithUnder100WeightToggled(bool checked)
{
	if(checked==false){
		QString s=tr("It is recommended to keep this warning active, but if you really want, you can disable it.");
		s+="\n\n";
		s+=tr("Disable it only if you know what you are doing.");
		s+="\n\n";
		s+=tr("Are you sure you want to disable it?");
	
		QMessageBox::StandardButton b=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes);
	
		if(b!=QMessageBox::Yes){
			disconnect(showWarningForMaxHoursDailyWithUnder100WeightAction, &QAction::toggled, this, &FetMainForm::showWarningForMaxHoursDailyWithUnder100WeightToggled);
			showWarningForMaxHoursDailyWithUnder100WeightAction->setChecked(true);
			connect(showWarningForMaxHoursDailyWithUnder100WeightAction, &QAction::toggled, this, &FetMainForm::showWarningForMaxHoursDailyWithUnder100WeightToggled);
			return;
		}
	}
	
	SHOW_WARNING_FOR_MAX_HOURS_DAILY_WITH_UNDER_100_WEIGHT=checked;
}

void FetMainForm::showWarningForNotPerfectConstraintsToggled(bool checked)
{
	if(checked==false){
		QString s=tr("It is recommended to keep this warning active, but if you really want, you can disable it.");
		s+="\n\n";
		s+=tr("Disable it only if you know what you are doing.");
		s+="\n\n";
		s+=tr("Are you sure you want to disable it?");
	
		QMessageBox::StandardButton b=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes);
	
		if(b!=QMessageBox::Yes){
			disconnect(showWarningForNotPerfectConstraintsAction, &QAction::toggled, this, &FetMainForm::showWarningForNotPerfectConstraintsToggled);
			showWarningForNotPerfectConstraintsAction->setChecked(true);
			connect(showWarningForNotPerfectConstraintsAction, &QAction::toggled, this, &FetMainForm::showWarningForNotPerfectConstraintsToggled);
			return;
		}
	}
	
	SHOW_WARNING_FOR_NOT_PERFECT_CONSTRAINTS=checked;
}


void FetMainForm::enableStudentsMinHoursDailyWithAllowEmptyDaysToggled(bool checked)
{
	assert(gt.rules.mode!=MORNINGS_AFTERNOONS);
	if(checked==true){
		QString s=tr("This kind of constraint is good, but only in the right case. Adding such constraints in the wrong circumstance may make your"
		 " timetable solve too slow or even impossible.");
		s+="\n\n";
		s+=tr("Please use such constraints only if you are sure that there exists a timetable with empty days for students. If your institution requires"
			" each day for the students to be not empty, or if there does not exist a solution with empty days for students, "
			"it is IMPERATIVE (for performance reasons) that you use the standard constraint which does not allow empty days."
			" Otherwise, the timetable may be impossible to find.");
		s+="\n\n";
		s+=tr("Continue only if you know what you are doing.");
	
		QMessageBox::StandardButton b=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
	
		if(b!=QMessageBox::Ok){
			disconnect(enableStudentsMinHoursDailyWithAllowEmptyDaysAction, &QAction::toggled, this, &FetMainForm::enableStudentsMinHoursDailyWithAllowEmptyDaysToggled);
			enableStudentsMinHoursDailyWithAllowEmptyDaysAction->setChecked(false);
			connect(enableStudentsMinHoursDailyWithAllowEmptyDaysAction, &QAction::toggled, this, &FetMainForm::enableStudentsMinHoursDailyWithAllowEmptyDaysToggled);
			return;
		}
	}
	
	ENABLE_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS=checked;
}

void FetMainForm::showWarningForStudentsMinHoursDailyWithAllowEmptyDaysToggled(bool checked)
{
	assert(gt.rules.mode!=MORNINGS_AFTERNOONS);
	if(checked==false){
		QString s=tr("It is recommended to keep this warning active, but if you really want, you can disable it.");
		s+="\n\n";
		s+=tr("Disable it only if you know what you are doing.");
		s+="\n\n";
		s+=tr("Are you sure you want to disable it?");
	
		QMessageBox::StandardButton b=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes);
	
		if(b!=QMessageBox::Yes){
			disconnect(showWarningForStudentsMinHoursDailyWithAllowEmptyDaysAction, &QAction::toggled, this, &FetMainForm::showWarningForStudentsMinHoursDailyWithAllowEmptyDaysToggled);
			showWarningForStudentsMinHoursDailyWithAllowEmptyDaysAction->setChecked(true);
			connect(showWarningForStudentsMinHoursDailyWithAllowEmptyDaysAction, &QAction::toggled, this, &FetMainForm::showWarningForStudentsMinHoursDailyWithAllowEmptyDaysToggled);
			return;
		}
	}
	
	SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS=checked;
}

void FetMainForm::enableStudentsMinHoursPerMorningWithAllowEmptyMorningsToggled(bool checked)
{
	Q_UNUSED(checked);

	assert(0); //Yes, this option is always disabled

	/*QMessageBox::information(this, tr("FET information"), tr("This option must remain selected in the custom FET version MA"));

	disconnect(enableStudentsMinHoursPerMorningWithAllowEmptyMorningsAction, SIG NAL(toggled(bool)), this, SL OT(enableStudentsMinHoursPerMorningWithAllowEmptyMorningsToggled(bool)));
	enableStudentsMinHoursPerMorningWithAllowEmptyMorningsAction->setChecked(true);
	connect(enableStudentsMinHoursPerMorningWithAllowEmptyMorningsAction, SIG NAL(toggled(bool)), this, SL OT(enableStudentsMinHoursPerMorningWithAllowEmptyMorningsToggled(bool)));*/

	return;
}

void FetMainForm::showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsToggled(bool checked)
{
	Q_UNUSED(checked);

	assert(0); //Yes, this option is always disabled

	/*QMessageBox::information(this, tr("FET information"), tr("This option must remain unselected in the custom FET version MA"));

	disconnect(showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsAction, SIG NAL(toggled(bool)), this, SL OT(showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsToggled(bool)));
	showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsAction->setChecked(false);
	connect(showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsAction, SIG NAL(toggled(bool)), this, SL OT(showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsToggled(bool)));*/

	return;
}

void FetMainForm::enableStudentsMinHoursPerAfternoonWithAllowEmptyAfternoonsToggled(bool checked)
{
	Q_UNUSED(checked);

	assert(0); //Yes, this option is always disabled

	/*QMessageBox::information(this, tr("FET information"), tr("This option must remain selected in the custom FET version MA"));

	disconnect(enableStudentsMinHoursPerMorningWithAllowEmptyMorningsAction, SIG NAL(toggled(bool)), this, SL OT(enableStudentsMinHoursPerMorningWithAllowEmptyMorningsToggled(bool)));
	enableStudentsMinHoursPerMorningWithAllowEmptyMorningsAction->setChecked(true);
	connect(enableStudentsMinHoursPerMorningWithAllowEmptyMorningsAction, SIG NAL(toggled(bool)), this, SL OT(enableStudentsMinHoursPerMorningWithAllowEmptyMorningsToggled(bool)));*/

	return;
}

void FetMainForm::showWarningForStudentsMinHoursPerAfternoonWithAllowEmptyAfternoonsToggled(bool checked)
{
	Q_UNUSED(checked);

	assert(0); //Yes, this option is always disabled

	/*QMessageBox::information(this, tr("FET information"), tr("This option must remain unselected in the custom FET version MA"));

	disconnect(showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsAction, SIG NAL(toggled(bool)), this, SL OT(showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsToggled(bool)));
	showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsAction->setChecked(false);
	connect(showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsAction, SIG NAL(toggled(bool)), this, SL OT(showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsToggled(bool)));*/

	return;
}

void FetMainForm::enableGroupActivitiesInInitialOrderToggled(bool checked)
{
	if(checked==true){
		QString s=tr("This kind of option is good, but only in the right case. Adding such an option in the wrong circumstance may make your"
		 " timetable solve too slow or even impossible.");
		s+="\n\n";
		s+=tr("Continue only if you know what you are doing.");
	
		QMessageBox::StandardButton b=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
	
		if(b!=QMessageBox::Ok){
			disconnect(enableGroupActivitiesInInitialOrderAction, &QAction::toggled, this, &FetMainForm::enableGroupActivitiesInInitialOrderToggled);
			enableGroupActivitiesInInitialOrderAction->setChecked(false);
			connect(enableGroupActivitiesInInitialOrderAction, &QAction::toggled, this, &FetMainForm::enableGroupActivitiesInInitialOrderToggled);
			return;
		}
	}

	ENABLE_GROUP_ACTIVITIES_IN_INITIAL_ORDER=checked;

	setEnabledIcon(groupActivitiesInInitialOrderAction, ENABLE_GROUP_ACTIVITIES_IN_INITIAL_ORDER);
}

void FetMainForm::showWarningForGroupActivitiesInInitialOrderToggled(bool checked)
{
	if(checked==false){
		QString s=tr("It is recommended to keep this warning active, but if you really want, you can disable it.");
		s+="\n\n";
		s+=tr("Disable it only if you know what you are doing.");
		s+="\n\n";
		s+=tr("Are you sure you want to disable it?");
	
		QMessageBox::StandardButton b=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes);
	
		if(b!=QMessageBox::Yes){
			disconnect(showWarningForGroupActivitiesInInitialOrderAction, &QAction::toggled, this, &FetMainForm::showWarningForGroupActivitiesInInitialOrderToggled);
			showWarningForGroupActivitiesInInitialOrderAction->setChecked(true);
			connect(showWarningForGroupActivitiesInInitialOrderAction, &QAction::toggled, this, &FetMainForm::showWarningForGroupActivitiesInInitialOrderToggled);
			return;
		}
	}
	
	SHOW_WARNING_FOR_GROUP_ACTIVITIES_IN_INITIAL_ORDER=checked;
}


//time constraints
void FetMainForm::shortcutAllTimeConstraintsPushButton_clicked()
{
	dataAllTimeConstraintsAction_triggered();
}

void FetMainForm::shortcutBreakTimeConstraintsPushButton_clicked()
{
	dataTimeConstraintsBreakTimesAction_triggered();
}

void FetMainForm::shortcutTeachersTimeConstraintsPushButton_clicked()
{
	menuTeachers_time_constraints->popup(QCursor::pos());
}

void FetMainForm::shortcutStudentsTimeConstraintsPushButton_clicked()
{
	menuStudents_time_constraints->popup(QCursor::pos());
}

void FetMainForm::shortcutActivitiesTimeConstraintsPushButton_clicked()
{
	menuActivities_time_constraints->popup(QCursor::pos());
}

void FetMainForm::shortcutAdvancedTimeConstraintsPushButton_clicked()
{
	shortcutAdvancedTimeMenu->popup(QCursor::pos());
}


//space constraints
void FetMainForm::shortcutAllSpaceConstraintsPushButton_clicked()
{
	dataAllSpaceConstraintsAction_triggered();
}

void FetMainForm::shortcutRoomsSpaceConstraintsPushButton_clicked()
{
	menuRooms_space_constraints->popup(QCursor::pos());
}

void FetMainForm::shortcutTeachersSpaceConstraintsPushButton_clicked()
{
	menuTeachers_space_constraints->popup(QCursor::pos());
}

void FetMainForm::shortcutStudentsSpaceConstraintsPushButton_clicked()
{
	menuStudents_space_constraints->popup(QCursor::pos());
}

void FetMainForm::shortcutSubjectsSpaceConstraintsPushButton_clicked()
{
	menuSubjects_space_constraints->popup(QCursor::pos());
}

void FetMainForm::shortcutActivityTagsSpaceConstraintsPushButton_clicked()
{
	menuActivity_tags_space_constraints->popup(QCursor::pos());
}

void FetMainForm::shortcutSubjectsAndActivityTagsSpaceConstraintsPushButton_clicked()
{
	menuSubjects_and_activity_tags_space_constraints->popup(QCursor::pos());
}

void FetMainForm::shortcutActivitiesSpaceConstraintsPushButton_clicked()
{
	menuActivities_space_constraints->popup(QCursor::pos());
}

//timetable
void FetMainForm::shortcutGeneratePushButton_clicked()
{
	timetableGenerateAction_triggered();
}

void FetMainForm::shortcutGenerateMultiplePushButton_clicked()
{
	timetableGenerateMultipleAction_triggered();
}

void FetMainForm::shortcutViewTeachersPushButton_clicked()
{
	menuView_teachers->popup(QCursor::pos());
	//old
	//timetableViewTeachersAction_triggered();
}

void FetMainForm::shortcutViewStudentsPushButton_clicked()
{
	menuView_students->popup(QCursor::pos());
	//old
	//timetableViewStudentsAction_triggered();
}

void FetMainForm::shortcutViewRoomsPushButton_clicked()
{
	menuView_rooms->popup(QCursor::pos());
	//old
	//timetableViewRoomsAction_triggered();
}

void FetMainForm::shortcutShowSoftConflictsPushButton_clicked()
{
	timetableShowConflictsAction_triggered();
}

//2014-07-01
void FetMainForm::shortcutsTimetableAdvancedPushButton_clicked()
{
	shortcutTimetableAdvancedMenu->popup(QCursor::pos());
}

void FetMainForm::shortcutsTimetablePrintPushButton_clicked()
{
	timetablePrintAction_triggered();
}

void FetMainForm::shortcutsTimetableLockingPushButton_clicked()
{
	shortcutTimetableLockingMenu->popup(QCursor::pos());
}

//data shortcut
void FetMainForm::shortcutBasicPushButton_clicked()
{
	shortcutBasicMenu->popup(QCursor::pos());
}

void FetMainForm::shortcutSubjectsPushButton_clicked()
{
	dataSubjectsAction_triggered();
}

void FetMainForm::shortcutActivityTagsPushButton_clicked()
{
	dataActivityTagsAction_triggered();
}

void FetMainForm::shortcutTeachersPushButton_clicked()
{
	dataTeachersAction_triggered();
}

void FetMainForm::shortcutStudentsPushButton_clicked()
{
	menuStudents->popup(QCursor::pos());
}

void FetMainForm::shortcutActivitiesPushButton_clicked()
{
	dataActivitiesAction_triggered();
}

void FetMainForm::shortcutSubactivitiesPushButton_clicked()
{
	dataSubactivitiesAction_triggered();
}

void FetMainForm::shortcutDataSpacePushButton_clicked()
{
	shortcutDataSpaceMenu->popup(QCursor::pos());
}

void FetMainForm::shortcutDataAdvancedPushButton_clicked()
{
	shortcutDataAdvancedMenu->popup(QCursor::pos());
}

//file shortcut
void FetMainForm::shortcutNewPushButton_clicked()
{
	fileNewAction_triggered();
}

void FetMainForm::shortcutOpenPushButton_clicked()
{
	fileOpenAction_triggered();
}

void FetMainForm::shortcutOpenRecentPushButton_clicked()
{
	fileOpenRecentMenu->popup(QCursor::pos());
}

void FetMainForm::shortcutSavePushButton_clicked()
{
	fileSaveAction_triggered();
}

void FetMainForm::shortcutSaveAsPushButton_clicked()
{
	fileSaveAsAction_triggered();
}

#else
bool SHOW_WARNING_FOR_SUBGROUPS_WITH_THE_SAME_ACTIVITIES=true;

bool SHOW_WARNING_FOR_ACTIVITIES_FIXED_SPACE_VIRTUAL_REAL_ROOMS_BUT_NOT_FIXED_TIME=true;

bool SHOW_WARNING_FOR_MAX_HOURS_DAILY_WITH_UNDER_100_WEIGHT=true;

bool SHOW_WARNING_FOR_NOT_PERFECT_CONSTRAINTS=true;
bool SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS=true;

bool SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_PER_MORNING_WITH_ALLOW_EMPTY_MORNINGS=true;

bool SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_PER_AFTERNOON_WITH_ALLOW_EMPTY_AFTERNOONS=true;

bool SHOW_WARNING_FOR_GROUP_ACTIVITIES_IN_INITIAL_ORDER=true;
#endif
