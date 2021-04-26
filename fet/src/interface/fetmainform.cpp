//
//
// Description: This file is part of FET
//
//
// Author: Lalescu Liviu <Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
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

#include <iostream>
//using namespace std;

#ifndef FET_COMMAND_LINE

#include <Qt>

#include "fetmainform.h"

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

#include "constrainttwoactivitiesconsecutiveform.h"
#include "constrainttwoactivitiesgroupedform.h"
#include "constraintthreeactivitiesgroupedform.h"
#include "constrainttwoactivitiesorderedform.h"
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
#include "constraintbasiccompulsorytimeform.h"
#include "constraintbasiccompulsoryspaceform.h"
#include "constraintroomnotavailabletimesform.h"
#include "constraintteacherroomnotavailabletimesform.h"
#include "constraintactivitypreferredroomform.h"
#include "constraintstudentssetnotavailabletimesform.h"
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
#include "constraintmaxdaysbetweenactivitiesform.h"
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

#include "constraintstudentssetmaxroomchangesperdayform.h"
#include "constraintstudentsmaxroomchangesperdayform.h"
#include "constraintstudentssetmaxroomchangesperweekform.h"
#include "constraintstudentsmaxroomchangesperweekform.h"
#include "constraintstudentssetmingapsbetweenroomchangesform.h"
#include "constraintstudentsmingapsbetweenroomchangesform.h"

#include "constraintteachermaxroomchangesperrealdayform.h"
#include "constraintteachersmaxroomchangesperrealdayform.h"

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

#include "constraintstudentssetmaxhoursdailyrealdaysform.h"
#include "constraintstudentsmaxhoursdailyrealdaysform.h"

#include "constraintstudentsminhourspermorningform.h"
#include "constraintstudentssetminhourspermorningform.h"

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

//block-planning
#include "constraintmaxtotalactivitiesfromsetinselectedtimeslotsform.h"

#include "constraintmaxgapsbetweenactivitiesform.h"

//terms
#include "constraintactivitiesmaxinatermform.h"
#include "constraintactivitiesoccupymaxtermsform.h"

#include "termsform.h"

#include "settingstimetablehtmllevelform.h"

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

#include <QMenu>
#include <QCursor>
#include <QSettings>

#include <QFileInfo>
#include <QFile>

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

QRect mainFormSettingsRect;
int MAIN_FORM_SHORTCUTS_TAB_POSITION;

#include "spreadmindaysconstraintsfivedaysform.h"

#include "statisticsexport.h"

#endif

bool simulation_running; //true if the user started an allocation of the timetable

bool simulation_running_multi;

bool students_schedule_ready;
bool teachers_schedule_ready;
bool rooms_schedule_ready;

Solution best_solution;

QString conflictsString; //the string that contains a log of the broken constraints
QString conflictsStringTitle;

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

bool ENABLE_GROUP_ACTIVITIES_IN_INITIAL_ORDER=false;
bool SHOW_WARNING_FOR_GROUP_ACTIVITIES_IN_INITIAL_ORDER=true;

bool CONFIRM_ACTIVITY_PLANNING=true;
bool CONFIRM_SPREAD_ACTIVITIES=true;
bool CONFIRM_REMOVE_REDUNDANT=true;
bool CONFIRM_SAVE_TIMETABLE=true;

//extern MRG32k3a rng;

const int STATUS_BAR_MILLISECONDS=2500;

FetMainForm::FetMainForm()
{
	setupUi(this);

	simulation_running=false;
	simulation_running_multi=false;

	createActionsForConstraints();
	
	QIcon appIcon(":/images/appicon.png");
	pqapplication->setWindowIcon(appIcon);
	
	this->statusBar()->addPermanentWidget(&modeLabel);
	dataAvailable=false;
	updateMode(true); //true means force
	
	QSettings settings(COMPANY, PROGRAM);
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
		connect(recentFileActions[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
		
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
		gt.rules.kill();
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
	rooms_schedule_ready=false;
	
	assert(!gt.rules.initialized);
	
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
			connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
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
			 QString(" (")+QSysInfo::prettyProductName()+QString("; ")+QSysInfo::currentCpuArchitecture()+QString(")"));
#elif QT_VERSION >= QT_VERSION_CHECK(5,0,0)
			req.setHeader(QNetworkRequest::UserAgentHeader, QString("FET")+QString(" ")+FET_VERSION);
#else
			req.setRawHeader("User-Agent", (QString("FET")+QString(" ")+FET_VERSION).toUtf8());
#endif
			networkManager->get(req);
		}
	}
	
	settingsPrintActivityTagsAction->setCheckable(true);
	settingsPrintActivityTagsAction->setChecked(TIMETABLE_HTML_PRINT_ACTIVITY_TAGS);
	
	settingsShowVirtualRoomsInTimetablesAction->setCheckable(true);
	settingsShowVirtualRoomsInTimetablesAction->setChecked(SHOW_VIRTUAL_ROOMS_IN_TIMETABLES);

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

	enableGroupActivitiesInInitialOrderAction->setChecked(ENABLE_GROUP_ACTIVITIES_IN_INITIAL_ORDER);
	showWarningForGroupActivitiesInInitialOrderAction->setChecked(SHOW_WARNING_FOR_GROUP_ACTIVITIES_IN_INITIAL_ORDER);
	
	connect(showWarningForSubgroupsWithTheSameActivitiesAction, SIGNAL(toggled(bool)), this, SLOT(showWarningForSubgroupsWithTheSameActivitiesToggled(bool)));
	connect(showWarningForActivitiesNotLockedTimeLockedSpaceVirtualRealRoomsAction, SIGNAL(toggled(bool)),
	 this, SLOT(showWarningForActivitiesNotLockedTimeLockedSpaceVirtualRealRoomsToggled(bool)));
	connect(showWarningForMaxHoursDailyWithUnder100WeightAction, SIGNAL(toggled(bool)),
	 this, SLOT(showWarningForMaxHoursDailyWithUnder100WeightToggled(bool)));

	connect(checkForUpdatesAction, SIGNAL(toggled(bool)), this, SLOT(checkForUpdatesToggled(bool)));
	connect(settingsUseColorsAction, SIGNAL(toggled(bool)), this, SLOT(useColorsToggled(bool)));
	connect(settingsShowSubgroupsInComboBoxesAction, SIGNAL(toggled(bool)), this, SLOT(showSubgroupsInComboBoxesToggled(bool)));
	connect(settingsShowSubgroupsInActivityPlanningAction, SIGNAL(toggled(bool)), this, SLOT(showSubgroupsInActivityPlanningToggled(bool)));
	
	connect(enableActivityTagMaxHoursDailyAction, SIGNAL(toggled(bool)), this, SLOT(enableActivityTagMaxHoursDailyToggled(bool)));
	connect(enableActivityTagMinHoursDailyAction, SIGNAL(toggled(bool)), this, SLOT(enableActivityTagMinHoursDailyToggled(bool)));
	connect(enableStudentsMaxGapsPerDayAction, SIGNAL(toggled(bool)), this, SLOT(enableStudentsMaxGapsPerDayToggled(bool)));
	connect(enableMaxGapsPerRealDayAction, SIGNAL(toggled(bool)), this, SLOT(enableMaxGapsPerRealDayToggled(bool)));
	connect(showWarningForNotPerfectConstraintsAction, SIGNAL(toggled(bool)), this, SLOT(showWarningForNotPerfectConstraintsToggled(bool)));

	connect(enableStudentsMinHoursDailyWithAllowEmptyDaysAction, SIGNAL(toggled(bool)), this, SLOT(enableStudentsMinHoursDailyWithAllowEmptyDaysToggled(bool)));
	connect(showWarningForStudentsMinHoursDailyWithAllowEmptyDaysAction, SIGNAL(toggled(bool)), this, SLOT(showWarningForStudentsMinHoursDailyWithAllowEmptyDaysToggled(bool)));

	connect(enableStudentsMinHoursPerMorningWithAllowEmptyMorningsAction, SIGNAL(toggled(bool)), this, SLOT(enableStudentsMinHoursPerMorningWithAllowEmptyMorningsToggled(bool)));
	connect(showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsAction, SIGNAL(toggled(bool)), this, SLOT(showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsToggled(bool)));

	connect(enableGroupActivitiesInInitialOrderAction, SIGNAL(toggled(bool)), this, SLOT(enableGroupActivitiesInInitialOrderToggled(bool)));
	connect(showWarningForGroupActivitiesInInitialOrderAction, SIGNAL(toggled(bool)), this, SLOT(showWarningForGroupActivitiesInInitialOrderToggled(bool)));

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
	}
	else{
		enableActivityTagMaxHoursDailyAction->setText(QCoreApplication::translate("FetMainForm_template", "Enable activity tag max hours daily / per real day", nullptr));

		enableActivityTagMinHoursDailyAction->setEnabled(false);

		enableMaxGapsPerRealDayAction->setEnabled(true);

		enableStudentsMinHoursDailyWithAllowEmptyDaysAction->setEnabled(false);
		showWarningForStudentsMinHoursDailyWithAllowEmptyDaysAction->setEnabled(false);

		enableStudentsMinHoursPerMorningWithAllowEmptyMorningsAction->setEnabled(false);
		showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsAction->setEnabled(false);
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
	dataTimeConstraintsBasicCompulsoryTimeAction = new QAction(this);
	dataTimeConstraintsStudentsSetNotAvailableTimesAction = new QAction(this);
	dataTimeConstraintsBreakTimesAction = new QAction(this);
	dataTimeConstraintsTeacherMaxDaysPerWeekAction = new QAction(this);
	dataTimeConstraintsTeachersMaxHoursDailyAction = new QAction(this);
	dataTimeConstraintsActivityPreferredStartingTimeAction = new QAction(this);
	dataTimeConstraintsStudentsSetMaxGapsPerWeekAction = new QAction(this);
	dataTimeConstraintsStudentsMaxGapsPerWeekAction = new QAction(this);
	dataTimeConstraintsStudentsEarlyMaxBeginningsAtSecondHourAction = new QAction(this);
	dataTimeConstraintsActivitiesNotOverlappingAction = new QAction(this);
	dataTimeConstraintsActivityTagsNotOverlappingAction = new QAction(this);
	dataTimeConstraintsMinDaysBetweenActivitiesAction = new QAction(this);
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
	dataTimeConstraintsTwoActivitiesConsecutiveAction = new QAction(this);
	dataTimeConstraintsActivityEndsStudentsDayAction = new QAction(this);
	dataTimeConstraintsActivityEndsTeachersDayAction = new QAction(this);
	dataTimeConstraintsTeachersMinHoursDailyAction = new QAction(this);
	dataTimeConstraintsTeacherMinHoursDailyAction = new QAction(this);
	dataTimeConstraintsTeachersMaxGapsPerDayAction = new QAction(this);
	dataTimeConstraintsTeacherMaxGapsPerDayAction = new QAction(this);
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
	dataTimeConstraintsTeacherMaxHoursDailyRealDaysAction = new QAction(this);
	dataTimeConstraintsStudentsSetMaxHoursDailyRealDaysAction = new QAction(this);
	dataTimeConstraintsStudentsMaxHoursDailyRealDaysAction = new QAction(this);
	dataTimeConstraintsStudentsMinHoursPerMorningAction = new QAction(this);
	dataTimeConstraintsStudentsSetMinHoursPerMorningAction = new QAction(this);
	dataTimeConstraintsTeachersMinHoursPerMorningAction = new QAction(this);
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
	
	//block-planning
	dataTimeConstraintsMaxGapsBetweenActivitiesAction = new QAction(this);
	dataTimeConstraintsMaxTotalActivitiesFromSetInSelectedTimeSlotsAction = new QAction(this);
	
	//terms
	dataTimeConstraintsActivitiesMaxInATermAction = new QAction(this);
	dataTimeConstraintsActivitiesOccupyMaxTermsAction = new QAction(this);

	connect(dataTimeConstraintsActivitiesPreferredTimeSlotsAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsActivitiesPreferredTimeSlotsAction_triggered()));
	connect(dataTimeConstraintsActivitiesSameStartingTimeAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsActivitiesSameStartingTimeAction_triggered()));
	connect(dataTimeConstraintsActivitiesOccupyMaxTimeSlotsFromSelectionAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsActivitiesOccupyMaxTimeSlotsFromSelectionAction_triggered()));
	connect(dataTimeConstraintsActivitiesOccupyMinTimeSlotsFromSelectionAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsActivitiesOccupyMinTimeSlotsFromSelectionAction_triggered()));
	connect(dataTimeConstraintsActivitiesMaxSimultaneousInSelectedTimeSlotsAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsActivitiesMaxSimultaneousInSelectedTimeSlotsAction_triggered()));
	connect(dataTimeConstraintsActivitiesMinSimultaneousInSelectedTimeSlotsAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsActivitiesMinSimultaneousInSelectedTimeSlotsAction_triggered()));
	connect(dataTimeConstraintsTeacherNotAvailableTimesAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherNotAvailableTimesAction_triggered()));
	connect(dataTimeConstraintsBasicCompulsoryTimeAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsBasicCompulsoryTimeAction_triggered()));
	connect(dataTimeConstraintsStudentsSetNotAvailableTimesAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsSetNotAvailableTimesAction_triggered()));
	connect(dataTimeConstraintsBreakTimesAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsBreakTimesAction_triggered()));
	connect(dataTimeConstraintsTeacherMaxDaysPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherMaxDaysPerWeekAction_triggered()));
	connect(dataTimeConstraintsTeachersMaxHoursDailyAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersMaxHoursDailyAction_triggered()));
	connect(dataTimeConstraintsActivityPreferredStartingTimeAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsActivityPreferredStartingTimeAction_triggered()));
	connect(dataTimeConstraintsStudentsSetMaxGapsPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsSetMaxGapsPerWeekAction_triggered()));
	connect(dataTimeConstraintsStudentsMaxGapsPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsMaxGapsPerWeekAction_triggered()));
	connect(dataTimeConstraintsStudentsEarlyMaxBeginningsAtSecondHourAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsEarlyMaxBeginningsAtSecondHourAction_triggered()));
	connect(dataTimeConstraintsActivitiesNotOverlappingAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsActivitiesNotOverlappingAction_triggered()));
	connect(dataTimeConstraintsActivityTagsNotOverlappingAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsActivityTagsNotOverlappingAction_triggered()));
	connect(dataTimeConstraintsMinDaysBetweenActivitiesAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsMinDaysBetweenActivitiesAction_triggered()));
	connect(dataSpaceConstraintsBasicCompulsorySpaceAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsBasicCompulsorySpaceAction_triggered()));
	connect(dataSpaceConstraintsRoomNotAvailableTimesAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsRoomNotAvailableTimesAction_triggered()));
	connect(dataSpaceConstraintsTeacherRoomNotAvailableTimesAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsTeacherRoomNotAvailableTimesAction_triggered()));
	connect(dataSpaceConstraintsActivityPreferredRoomAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsActivityPreferredRoomAction_triggered()));
	connect(dataTimeConstraintsActivitiesSameStartingHourAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsActivitiesSameStartingHourAction_triggered()));
	connect(dataSpaceConstraintsActivityPreferredRoomsAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsActivityPreferredRoomsAction_triggered()));
	connect(dataSpaceConstraintsStudentsSetHomeRoomAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsStudentsSetHomeRoomAction_triggered()));
	connect(dataSpaceConstraintsStudentsSetHomeRoomsAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsStudentsSetHomeRoomsAction_triggered()));
	connect(dataTimeConstraintsTeachersMaxGapsPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersMaxGapsPerWeekAction_triggered()));
	connect(dataTimeConstraintsTeacherMaxGapsPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherMaxGapsPerWeekAction_triggered()));
	connect(dataTimeConstraintsStudentsSetEarlyMaxBeginningsAtSecondHourAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsSetEarlyMaxBeginningsAtSecondHourAction_triggered()));
	connect(dataTimeConstraintsTeacherMaxHoursDailyAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherMaxHoursDailyAction_triggered()));
	connect(dataTimeConstraintsStudentsSetMaxHoursDailyAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsSetMaxHoursDailyAction_triggered()));
	connect(dataTimeConstraintsStudentsMaxHoursDailyAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsMaxHoursDailyAction_triggered()));
	connect(dataTimeConstraintsStudentsMinHoursDailyAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsMinHoursDailyAction_triggered()));
	connect(dataTimeConstraintsStudentsSetMinHoursDailyAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsSetMinHoursDailyAction_triggered()));
	connect(dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsAction_triggered()));
	connect(dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsAction_triggered()));
	connect(dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsAction_triggered()));
	connect(dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsAction_triggered()));
	connect(dataTimeConstraintsTwoActivitiesConsecutiveAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTwoActivitiesConsecutiveAction_triggered()));
	connect(dataTimeConstraintsActivityEndsStudentsDayAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsActivityEndsStudentsDayAction_triggered()));
	connect(dataTimeConstraintsActivityEndsTeachersDayAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsActivityEndsTeachersDayAction_triggered()));
	connect(dataTimeConstraintsTeachersMinHoursDailyAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersMinHoursDailyAction_triggered()));
	connect(dataTimeConstraintsTeacherMinHoursDailyAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherMinHoursDailyAction_triggered()));
	connect(dataTimeConstraintsTeachersMaxGapsPerDayAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersMaxGapsPerDayAction_triggered()));
	connect(dataTimeConstraintsTeacherMaxGapsPerDayAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherMaxGapsPerDayAction_triggered()));
	connect(dataTimeConstraintsTeacherMaxSpanPerDayAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherMaxSpanPerDayAction_triggered()));
	connect(dataTimeConstraintsTeachersMaxSpanPerDayAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersMaxSpanPerDayAction_triggered()));
	connect(dataTimeConstraintsStudentsSetMaxSpanPerDayAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsSetMaxSpanPerDayAction_triggered()));
	connect(dataTimeConstraintsStudentsMaxSpanPerDayAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsMaxSpanPerDayAction_triggered()));
	connect(dataTimeConstraintsTeacherMinRestingHoursAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherMinRestingHoursAction_triggered()));
	connect(dataTimeConstraintsTeachersMinRestingHoursAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersMinRestingHoursAction_triggered()));
	connect(dataTimeConstraintsStudentsSetMinRestingHoursAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsSetMinRestingHoursAction_triggered()));
	connect(dataTimeConstraintsStudentsMinRestingHoursAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsMinRestingHoursAction_triggered()));
	connect(dataSpaceConstraintsSubjectPreferredRoomAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsSubjectPreferredRoomAction_triggered()));
	connect(dataSpaceConstraintsSubjectPreferredRoomsAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsSubjectPreferredRoomsAction_triggered()));
	connect(dataSpaceConstraintsSubjectActivityTagPreferredRoomAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsSubjectActivityTagPreferredRoomAction_triggered()));
	connect(dataSpaceConstraintsSubjectActivityTagPreferredRoomsAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsSubjectActivityTagPreferredRoomsAction_triggered()));
	connect(dataSpaceConstraintsTeacherHomeRoomAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsTeacherHomeRoomAction_triggered()));
	connect(dataSpaceConstraintsTeacherHomeRoomsAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsTeacherHomeRoomsAction_triggered()));
	connect(dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayAction_triggered()));
	connect(dataSpaceConstraintsStudentsMaxBuildingChangesPerDayAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsStudentsMaxBuildingChangesPerDayAction_triggered()));
	connect(dataSpaceConstraintsStudentsSetMaxBuildingChangesPerWeekAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsStudentsSetMaxBuildingChangesPerWeekAction_triggered()));
	connect(dataSpaceConstraintsStudentsMaxBuildingChangesPerWeekAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsStudentsMaxBuildingChangesPerWeekAction_triggered()));
	connect(dataSpaceConstraintsStudentsSetMinGapsBetweenBuildingChangesAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsStudentsSetMinGapsBetweenBuildingChangesAction_triggered()));
	connect(dataSpaceConstraintsStudentsMinGapsBetweenBuildingChangesAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsStudentsMinGapsBetweenBuildingChangesAction_triggered()));
	connect(dataSpaceConstraintsTeacherMaxBuildingChangesPerDayAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsTeacherMaxBuildingChangesPerDayAction_triggered()));
	connect(dataSpaceConstraintsTeachersMaxBuildingChangesPerDayAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsTeachersMaxBuildingChangesPerDayAction_triggered()));
	connect(dataSpaceConstraintsTeacherMaxBuildingChangesPerWeekAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsTeacherMaxBuildingChangesPerWeekAction_triggered()));
	connect(dataSpaceConstraintsTeachersMaxBuildingChangesPerWeekAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsTeachersMaxBuildingChangesPerWeekAction_triggered()));
	connect(dataSpaceConstraintsTeacherMinGapsBetweenBuildingChangesAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsTeacherMinGapsBetweenBuildingChangesAction_triggered()));
	connect(dataSpaceConstraintsTeachersMinGapsBetweenBuildingChangesAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsTeachersMinGapsBetweenBuildingChangesAction_triggered()));
	connect(dataSpaceConstraintsStudentsSetMaxRoomChangesPerDayAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsStudentsSetMaxRoomChangesPerDayAction_triggered()));
	connect(dataSpaceConstraintsStudentsMaxRoomChangesPerDayAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsStudentsMaxRoomChangesPerDayAction_triggered()));
	connect(dataSpaceConstraintsStudentsSetMaxRoomChangesPerWeekAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsStudentsSetMaxRoomChangesPerWeekAction_triggered()));
	connect(dataSpaceConstraintsStudentsMaxRoomChangesPerWeekAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsStudentsMaxRoomChangesPerWeekAction_triggered()));
	connect(dataSpaceConstraintsStudentsSetMinGapsBetweenRoomChangesAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsStudentsSetMinGapsBetweenRoomChangesAction_triggered()));
	connect(dataSpaceConstraintsStudentsMinGapsBetweenRoomChangesAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsStudentsMinGapsBetweenRoomChangesAction_triggered()));
	connect(dataSpaceConstraintsTeacherMaxRoomChangesPerDayAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsTeacherMaxRoomChangesPerDayAction_triggered()));
	connect(dataSpaceConstraintsTeachersMaxRoomChangesPerDayAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsTeachersMaxRoomChangesPerDayAction_triggered()));
	connect(dataSpaceConstraintsTeacherMaxRoomChangesPerWeekAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsTeacherMaxRoomChangesPerWeekAction_triggered()));
	connect(dataSpaceConstraintsTeachersMaxRoomChangesPerWeekAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsTeachersMaxRoomChangesPerWeekAction_triggered()));
	connect(dataSpaceConstraintsTeacherMinGapsBetweenRoomChangesAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsTeacherMinGapsBetweenRoomChangesAction_triggered()));
	connect(dataSpaceConstraintsTeachersMinGapsBetweenRoomChangesAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsTeachersMinGapsBetweenRoomChangesAction_triggered()));
	connect(dataTimeConstraintsActivitiesSameStartingDayAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsActivitiesSameStartingDayAction_triggered()));
	connect(dataTimeConstraintsTwoActivitiesOrderedAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTwoActivitiesOrderedAction_triggered()));
	connect(dataTimeConstraintsTwoActivitiesOrderedIfSameDayAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTwoActivitiesOrderedIfSameDayAction_triggered()));
	connect(dataTimeConstraintsTeachersMaxHoursContinuouslyAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersMaxHoursContinuouslyAction_triggered()));
	connect(dataTimeConstraintsTeacherMaxHoursContinuouslyAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherMaxHoursContinuouslyAction_triggered()));
	connect(dataTimeConstraintsStudentsSetMaxHoursContinuouslyAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsSetMaxHoursContinuouslyAction_triggered()));
	connect(dataTimeConstraintsStudentsMaxHoursContinuouslyAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsMaxHoursContinuouslyAction_triggered()));
	connect(dataTimeConstraintsActivitiesPreferredStartingTimesAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsActivitiesPreferredStartingTimesAction_triggered()));
	connect(dataTimeConstraintsActivityPreferredTimeSlotsAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsActivityPreferredTimeSlotsAction_triggered()));
	connect(dataTimeConstraintsActivityPreferredStartingTimesAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsActivityPreferredStartingTimesAction_triggered()));
	connect(dataTimeConstraintsMinGapsBetweenActivitiesAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsMinGapsBetweenActivitiesAction_triggered()));
	connect(dataTimeConstraintsSubactivitiesPreferredTimeSlotsAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsSubactivitiesPreferredTimeSlotsAction_triggered()));
	connect(dataTimeConstraintsSubactivitiesPreferredStartingTimesAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsSubactivitiesPreferredStartingTimesAction_triggered()));
	connect(dataTimeConstraintsTeacherIntervalMaxDaysPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherIntervalMaxDaysPerWeekAction_triggered()));
	connect(dataTimeConstraintsTeachersIntervalMaxDaysPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersIntervalMaxDaysPerWeekAction_triggered()));
	connect(dataTimeConstraintsStudentsSetIntervalMaxDaysPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsSetIntervalMaxDaysPerWeekAction_triggered()));
	connect(dataTimeConstraintsStudentsIntervalMaxDaysPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsIntervalMaxDaysPerWeekAction_triggered()));
	connect(dataTimeConstraintsActivitiesEndStudentsDayAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsActivitiesEndStudentsDayAction_triggered()));
	connect(dataTimeConstraintsActivitiesEndTeachersDayAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsActivitiesEndTeachersDayAction_triggered()));
	connect(dataTimeConstraintsTwoActivitiesGroupedAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTwoActivitiesGroupedAction_triggered()));
	connect(dataTimeConstraintsStudentsSetActivityTagMaxHoursContinuouslyAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsSetActivityTagMaxHoursContinuouslyAction_triggered()));
	connect(dataTimeConstraintsStudentsActivityTagMaxHoursContinuouslyAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsActivityTagMaxHoursContinuouslyAction_triggered()));
	connect(dataTimeConstraintsTeacherActivityTagMaxHoursContinuouslyAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherActivityTagMaxHoursContinuouslyAction_triggered()));
	connect(dataTimeConstraintsTeachersActivityTagMaxHoursContinuouslyAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersActivityTagMaxHoursContinuouslyAction_triggered()));
	connect(dataSpaceConstraintsActivityTagPreferredRoomAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsActivityTagPreferredRoomAction_triggered()));
	connect(dataSpaceConstraintsActivityTagPreferredRoomsAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsActivityTagPreferredRoomsAction_triggered()));
	connect(dataTimeConstraintsTeachersMaxDaysPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersMaxDaysPerWeekAction_triggered()));
	connect(dataTimeConstraintsThreeActivitiesGroupedAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsThreeActivitiesGroupedAction_triggered()));
	connect(dataTimeConstraintsMaxDaysBetweenActivitiesAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsMaxDaysBetweenActivitiesAction_triggered()));
	connect(dataTimeConstraintsTeacherMinDaysPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherMinDaysPerWeekAction_triggered()));
	connect(dataTimeConstraintsTeachersMinDaysPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersMinDaysPerWeekAction_triggered()));
	connect(dataTimeConstraintsTeacherActivityTagMaxHoursDailyAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherActivityTagMaxHoursDailyAction_triggered()));
	connect(dataTimeConstraintsTeachersActivityTagMaxHoursDailyAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersActivityTagMaxHoursDailyAction_triggered()));
	connect(dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyAction_triggered()));
	connect(dataTimeConstraintsStudentsActivityTagMaxHoursDailyAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsActivityTagMaxHoursDailyAction_triggered()));
	connect(dataTimeConstraintsTeacherActivityTagMinHoursDailyAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherActivityTagMinHoursDailyAction_triggered()));
	connect(dataTimeConstraintsTeachersActivityTagMinHoursDailyAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersActivityTagMinHoursDailyAction_triggered()));
	connect(dataTimeConstraintsStudentsSetActivityTagMinHoursDailyAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsSetActivityTagMinHoursDailyAction_triggered()));
	connect(dataTimeConstraintsStudentsActivityTagMinHoursDailyAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsActivityTagMinHoursDailyAction_triggered()));
	connect(dataTimeConstraintsStudentsSetMaxGapsPerDayAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsSetMaxGapsPerDayAction_triggered()));
	connect(dataTimeConstraintsStudentsMaxGapsPerDayAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsMaxGapsPerDayAction_triggered()));
	connect(dataSpaceConstraintsActivitiesOccupyMaxDifferentRoomsAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsActivitiesOccupyMaxDifferentRoomsAction_triggered()));
	connect(dataSpaceConstraintsActivitiesSameRoomIfConsecutiveAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsActivitiesSameRoomIfConsecutiveAction_triggered()));
	connect(dataTimeConstraintsStudentsSetMaxDaysPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsSetMaxDaysPerWeekAction_triggered()));
	connect(dataTimeConstraintsStudentsMaxDaysPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsMaxDaysPerWeekAction_triggered()));

	//mornings-afternoons
	connect(dataTimeConstraintsTeacherMaxRealDaysPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherMaxRealDaysPerWeekAction_triggered()));
	connect(dataTimeConstraintsTeacherMaxMorningsPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherMaxMorningsPerWeekAction_triggered()));
	connect(dataTimeConstraintsTeacherMaxTwoConsecutiveMorningsAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherMaxTwoConsecutiveMorningsAction_triggered()));
	connect(dataTimeConstraintsTeachersMaxTwoConsecutiveMorningsAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersMaxTwoConsecutiveMorningsAction_triggered()));
	connect(dataTimeConstraintsTeacherMaxTwoConsecutiveAfternoonsAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherMaxTwoConsecutiveAfternoonsAction_triggered()));
	connect(dataTimeConstraintsTeachersMaxTwoConsecutiveAfternoonsAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersMaxTwoConsecutiveAfternoonsAction_triggered()));
	connect(dataTimeConstraintsTeacherMaxAfternoonsPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherMaxAfternoonsPerWeekAction_triggered()));
	connect(dataTimeConstraintsTeachersMaxHoursDailyRealDaysAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersMaxHoursDailyRealDaysAction_triggered()));
	connect(dataTimeConstraintsTeachersAfternoonsEarlyMaxBeginningsAtSecondHourAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersAfternoonsEarlyMaxBeginningsAtSecondHourAction_triggered()));
	connect(dataTimeConstraintsTeacherAfternoonsEarlyMaxBeginningsAtSecondHourAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherAfternoonsEarlyMaxBeginningsAtSecondHourAction_triggered()));
	connect(dataTimeConstraintsStudentsAfternoonsEarlyMaxBeginningsAtSecondHourAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsAfternoonsEarlyMaxBeginningsAtSecondHourAction_triggered()));
	connect(dataTimeConstraintsStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHourAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHourAction_triggered()));
	connect(dataTimeConstraintsTeacherMaxHoursDailyRealDaysAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherMaxHoursDailyRealDaysAction_triggered()));
	connect(dataTimeConstraintsStudentsSetMaxHoursDailyRealDaysAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsSetMaxHoursDailyRealDaysAction_triggered()));
	connect(dataTimeConstraintsStudentsMaxHoursDailyRealDaysAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsMaxHoursDailyRealDaysAction_triggered()));
	connect(dataTimeConstraintsStudentsMinHoursPerMorningAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsMinHoursPerMorningAction_triggered()));
	connect(dataTimeConstraintsStudentsSetMinHoursPerMorningAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsSetMinHoursPerMorningAction_triggered()));
	connect(dataTimeConstraintsTeachersMinHoursPerMorningAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersMinHoursPerMorningAction_triggered()));
	connect(dataTimeConstraintsTeachersMinHoursDailyRealDaysAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersMinHoursDailyRealDaysAction_triggered()));
	connect(dataTimeConstraintsTeacherMaxHoursPerAllAfternoonsAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherMaxHoursPerAllAfternoonsAction_triggered()));
	connect(dataTimeConstraintsTeachersMaxHoursPerAllAfternoonsAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersMaxHoursPerAllAfternoonsAction_triggered()));
	connect(dataTimeConstraintsStudentsSetMaxHoursPerAllAfternoonsAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsSetMaxHoursPerAllAfternoonsAction_triggered()));
	connect(dataTimeConstraintsStudentsMaxHoursPerAllAfternoonsAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsMaxHoursPerAllAfternoonsAction_triggered()));
	connect(dataTimeConstraintsTeacherMinHoursPerMorningAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherMinHoursPerMorningAction_triggered()));
	connect(dataTimeConstraintsTeacherMinHoursDailyRealDaysAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherMinHoursDailyRealDaysAction_triggered()));
	connect(dataTimeConstraintsTeachersMaxZeroGapsPerAfternoonAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersMaxZeroGapsPerAfternoonAction_triggered()));
	connect(dataTimeConstraintsTeacherMaxZeroGapsPerAfternoonAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherMaxZeroGapsPerAfternoonAction_triggered()));
	connect(dataTimeConstraintsTeacherMaxSpanPerRealDayAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherMaxSpanPerRealDayAction_triggered()));
	connect(dataTimeConstraintsTeachersMaxSpanPerRealDayAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersMaxSpanPerRealDayAction_triggered()));
	connect(dataTimeConstraintsStudentsSetMaxSpanPerRealDayAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsSetMaxSpanPerRealDayAction_triggered()));
	connect(dataTimeConstraintsStudentsMaxSpanPerRealDayAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsMaxSpanPerRealDayAction_triggered()));
	connect(dataTimeConstraintsTeacherMinRestingHoursBetweenMorningAndAfternoonAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherMinRestingHoursBetweenMorningAndAfternoonAction_triggered()));
	connect(dataTimeConstraintsTeachersMinRestingHoursBetweenMorningAndAfternoonAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersMinRestingHoursBetweenMorningAndAfternoonAction_triggered()));
	connect(dataTimeConstraintsStudentsSetMinRestingHoursBetweenMorningAndAfternoonAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsSetMinRestingHoursBetweenMorningAndAfternoonAction_triggered()));
	connect(dataTimeConstraintsStudentsMinRestingHoursBetweenMorningAndAfternoonAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsMinRestingHoursBetweenMorningAndAfternoonAction_triggered()));
	connect(dataTimeConstraintsTeacherMorningIntervalMaxDaysPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherMorningIntervalMaxDaysPerWeekAction_triggered()));
	connect(dataTimeConstraintsTeachersMorningIntervalMaxDaysPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersMorningIntervalMaxDaysPerWeekAction_triggered()));
	connect(dataTimeConstraintsTeacherAfternoonIntervalMaxDaysPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherAfternoonIntervalMaxDaysPerWeekAction_triggered()));
	connect(dataTimeConstraintsTeachersAfternoonIntervalMaxDaysPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersAfternoonIntervalMaxDaysPerWeekAction_triggered()));
	connect(dataTimeConstraintsStudentsSetMorningIntervalMaxDaysPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsSetMorningIntervalMaxDaysPerWeekAction_triggered()));
	connect(dataTimeConstraintsStudentsMorningIntervalMaxDaysPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsMorningIntervalMaxDaysPerWeekAction_triggered()));
	connect(dataTimeConstraintsStudentsSetAfternoonIntervalMaxDaysPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsSetAfternoonIntervalMaxDaysPerWeekAction_triggered()));
	connect(dataTimeConstraintsStudentsAfternoonIntervalMaxDaysPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsAfternoonIntervalMaxDaysPerWeekAction_triggered()));
	connect(dataTimeConstraintsTeachersMaxRealDaysPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersMaxRealDaysPerWeekAction_triggered()));
	connect(dataTimeConstraintsTeachersMaxMorningsPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersMaxMorningsPerWeekAction_triggered()));
	connect(dataTimeConstraintsTeachersMaxAfternoonsPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersMaxAfternoonsPerWeekAction_triggered()));
	connect(dataTimeConstraintsTeacherMinRealDaysPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherMinRealDaysPerWeekAction_triggered()));
	connect(dataTimeConstraintsTeachersMinRealDaysPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersMinRealDaysPerWeekAction_triggered()));
	connect(dataTimeConstraintsTeacherMinMorningsPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherMinMorningsPerWeekAction_triggered()));
	connect(dataTimeConstraintsTeachersMinMorningsPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersMinMorningsPerWeekAction_triggered()));
	connect(dataTimeConstraintsTeacherMinAfternoonsPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherMinAfternoonsPerWeekAction_triggered()));
	connect(dataTimeConstraintsTeachersMinAfternoonsPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersMinAfternoonsPerWeekAction_triggered()));
	connect(dataTimeConstraintsTeacherActivityTagMaxHoursDailyRealDaysAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherActivityTagMaxHoursDailyRealDaysAction_triggered()));
	connect(dataTimeConstraintsTeacherMaxTwoActivityTagsPerDayFromN1N2N3Action, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherMaxTwoActivityTagsPerDayFromN1N2N3Action_triggered()));
	connect(dataTimeConstraintsTeachersMaxTwoActivityTagsPerDayFromN1N2N3Action, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersMaxTwoActivityTagsPerDayFromN1N2N3Action_triggered()));
	connect(dataTimeConstraintsTeachersActivityTagMaxHoursDailyRealDaysAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersActivityTagMaxHoursDailyRealDaysAction_triggered()));
	connect(dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyRealDaysAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyRealDaysAction_triggered()));
	connect(dataTimeConstraintsStudentsActivityTagMaxHoursDailyRealDaysAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsActivityTagMaxHoursDailyRealDaysAction_triggered()));
	connect(dataTimeConstraintsStudentsSetMaxGapsPerRealDayAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsSetMaxGapsPerRealDayAction_triggered()));
	connect(dataTimeConstraintsStudentsMaxGapsPerRealDayAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsMaxGapsPerRealDayAction_triggered()));
	connect(dataTimeConstraintsStudentsSetMaxRealDaysPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsSetMaxRealDaysPerWeekAction_triggered()));
	connect(dataTimeConstraintsStudentsMaxRealDaysPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsMaxRealDaysPerWeekAction_triggered()));
	connect(dataTimeConstraintsStudentsSetMaxMorningsPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsSetMaxMorningsPerWeekAction_triggered()));
	connect(dataTimeConstraintsStudentsMaxMorningsPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsMaxMorningsPerWeekAction_triggered()));
	connect(dataTimeConstraintsStudentsSetMaxAfternoonsPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsSetMaxAfternoonsPerWeekAction_triggered()));
	connect(dataTimeConstraintsStudentsMaxAfternoonsPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsMaxAfternoonsPerWeekAction_triggered()));
	connect(dataTimeConstraintsStudentsSetMinMorningsPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsSetMinMorningsPerWeekAction_triggered()));
	connect(dataTimeConstraintsStudentsMinMorningsPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsMinMorningsPerWeekAction_triggered()));
	connect(dataTimeConstraintsStudentsSetMinAfternoonsPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsSetMinAfternoonsPerWeekAction_triggered()));
	connect(dataTimeConstraintsStudentsMinAfternoonsPerWeekAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsMinAfternoonsPerWeekAction_triggered()));
	connect(dataTimeConstraintsTeacherMaxGapsPerRealDayAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherMaxGapsPerRealDayAction_triggered()));
	connect(dataTimeConstraintsTeachersMaxGapsPerRealDayAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersMaxGapsPerRealDayAction_triggered()));
	connect(dataTimeConstraintsStudentsSetMaxGapsPerWeekForRealDaysAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsSetMaxGapsPerWeekForRealDaysAction_triggered()));
	connect(dataTimeConstraintsStudentsMaxGapsPerWeekForRealDaysAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsStudentsMaxGapsPerWeekForRealDaysAction_triggered()));
	connect(dataTimeConstraintsTeacherMaxGapsPerWeekForRealDaysAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeacherMaxGapsPerWeekForRealDaysAction_triggered()));
	connect(dataTimeConstraintsTeachersMaxGapsPerWeekForRealDaysAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsTeachersMaxGapsPerWeekForRealDaysAction_triggered()));
	connect(dataSpaceConstraintsStudentsSetMaxRoomChangesPerRealDayAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsStudentsSetMaxRoomChangesPerRealDayAction_triggered()));
	connect(dataSpaceConstraintsStudentsMaxRoomChangesPerRealDayAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsStudentsMaxRoomChangesPerRealDayAction_triggered()));
	connect(dataSpaceConstraintsTeacherMaxRoomChangesPerRealDayAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsTeacherMaxRoomChangesPerRealDayAction_triggered()));
	connect(dataSpaceConstraintsTeachersMaxRoomChangesPerRealDayAction, SIGNAL(triggered()), this, SLOT(dataSpaceConstraintsTeachersMaxRoomChangesPerRealDayAction_triggered()));

	//block-planning
	connect(dataTimeConstraintsMaxGapsBetweenActivitiesAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsMaxGapsBetweenActivitiesAction_triggered()));
	connect(dataTimeConstraintsMaxTotalActivitiesFromSetInSelectedTimeSlotsAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsMaxTotalActivitiesFromSetInSelectedTimeSlotsAction_triggered()));

	//terms
	connect(dataTimeConstraintsActivitiesMaxInATermAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsActivitiesMaxInATermAction_triggered()));
	connect(dataTimeConstraintsActivitiesOccupyMaxTermsAction, SIGNAL(triggered()), this, SLOT(dataTimeConstraintsActivitiesOccupyMaxTermsAction_triggered()));

	retranslateConstraints();
}

void FetMainForm::retranslateConstraints()
{
	dataTimeConstraintsActivitiesPreferredTimeSlotsAction->setText(QCoreApplication::translate("FetMainForm_template", "A set of activities has a set of preferred time slots", nullptr));
	dataTimeConstraintsActivitiesSameStartingTimeAction->setText(QCoreApplication::translate("FetMainForm_template", "A set of activities has same starting time (day+hour)", nullptr));
	dataTimeConstraintsActivitiesOccupyMaxTimeSlotsFromSelectionAction->setText(QCoreApplication::translate("FetMainForm_template", "A set of activities occupies max time slots from selection", nullptr));
	dataTimeConstraintsActivitiesOccupyMinTimeSlotsFromSelectionAction->setText(QCoreApplication::translate("FetMainForm_template", "A set of activities occupies min time slots from selection", nullptr));
	dataTimeConstraintsActivitiesMaxSimultaneousInSelectedTimeSlotsAction->setText(QCoreApplication::translate("FetMainForm_template", "Max simultaneous activities from a set in selected time slots", nullptr));
	dataTimeConstraintsActivitiesMinSimultaneousInSelectedTimeSlotsAction->setText(QCoreApplication::translate("FetMainForm_template", "Min simultaneous activities from a set in selected time slots", nullptr));
	dataTimeConstraintsTeacherNotAvailableTimesAction->setText(QCoreApplication::translate("FetMainForm_template", "A teacher's not available times", nullptr));
	dataTimeConstraintsBasicCompulsoryTimeAction->setText(QCoreApplication::translate("FetMainForm_template", "Basic compulsory time constraints", nullptr));
	dataTimeConstraintsStudentsSetNotAvailableTimesAction->setText(QCoreApplication::translate("FetMainForm_template", "A students set's not available times", nullptr));
	dataTimeConstraintsBreakTimesAction->setText(QCoreApplication::translate("FetMainForm_template", "Break times", nullptr));
	dataTimeConstraintsTeacherMaxDaysPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Max days per week for a teacher", nullptr));
	dataTimeConstraintsTeachersMaxHoursDailyAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours daily for all teachers", nullptr));
	dataTimeConstraintsActivityPreferredStartingTimeAction->setText(QCoreApplication::translate("FetMainForm_template", "An activity has a preferred starting time", nullptr));
	dataTimeConstraintsStudentsSetMaxGapsPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Max gaps per week for a students set", nullptr));
	dataTimeConstraintsStudentsMaxGapsPerWeekAction->setText(QCoreApplication::translate("FetMainForm_template", "Max gaps per week for all students", nullptr));
	dataTimeConstraintsStudentsEarlyMaxBeginningsAtSecondHourAction->setText(QCoreApplication::translate("FetMainForm_template", "All students begin early (max beginnings at second hour)", nullptr));
	dataTimeConstraintsActivitiesNotOverlappingAction->setText(QCoreApplication::translate("FetMainForm_template", "A set of activities are not overlapping", nullptr));
	dataTimeConstraintsActivityTagsNotOverlappingAction->setText(QCoreApplication::translate("FetMainForm_template", "A set of activity tags are not overlapping", nullptr));
	dataTimeConstraintsMinDaysBetweenActivitiesAction->setText(QCoreApplication::translate("FetMainForm_template", "Min days between a set of activities", nullptr));
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
	dataTimeConstraintsTwoActivitiesConsecutiveAction->setText(QCoreApplication::translate("FetMainForm_template", "Two activities are consecutive", nullptr));
	dataTimeConstraintsActivityEndsStudentsDayAction->setText(QCoreApplication::translate("FetMainForm_template", "An activity ends students day", nullptr));
	dataTimeConstraintsActivityEndsTeachersDayAction->setText(QCoreApplication::translate("FetMainForm_template", "An activity ends teachers day", nullptr));
	dataTimeConstraintsTeachersMinHoursDailyAction->setText(QCoreApplication::translate("FetMainForm_template", "Min hours daily for all teachers", nullptr));
	dataTimeConstraintsTeacherMinHoursDailyAction->setText(QCoreApplication::translate("FetMainForm_template", "Min hours daily for a teacher", nullptr));
	dataTimeConstraintsTeachersMaxGapsPerDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max gaps per day for all teachers", nullptr));
	dataTimeConstraintsTeacherMaxGapsPerDayAction->setText(QCoreApplication::translate("FetMainForm_template", "Max gaps per day for a teacher", nullptr));
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
	dataTimeConstraintsTeacherMaxHoursDailyRealDaysAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours daily per real day for a teacher", nullptr));
	dataTimeConstraintsStudentsSetMaxHoursDailyRealDaysAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours daily per real day for a students set", nullptr));
	dataTimeConstraintsStudentsMaxHoursDailyRealDaysAction->setText(QCoreApplication::translate("FetMainForm_template", "Max hours daily per real day for all students", nullptr));
	dataTimeConstraintsStudentsMinHoursPerMorningAction->setText(QCoreApplication::translate("FetMainForm_template", "Min hours per morning for all students", nullptr));
	dataTimeConstraintsStudentsSetMinHoursPerMorningAction->setText(QCoreApplication::translate("FetMainForm_template", "Min hours per morning for a students set", nullptr));
	dataTimeConstraintsTeachersMinHoursPerMorningAction->setText(QCoreApplication::translate("FetMainForm_template", "Min hours per morning for all teachers", nullptr));
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

	//block-planning
	dataTimeConstraintsMaxGapsBetweenActivitiesAction->setText(QCoreApplication::translate("FetMainForm_template", "Max gaps (hours) between a set of activities", nullptr));
	dataTimeConstraintsMaxTotalActivitiesFromSetInSelectedTimeSlotsAction->setText(QCoreApplication::translate("FetMainForm_template", "Max total activities from a set in selected time slots", nullptr));
	
	//terms
	dataTimeConstraintsActivitiesMaxInATermAction->setText(QCoreApplication::translate("FetMainForm_template", "Max activities from a set in a term", nullptr));
	dataTimeConstraintsActivitiesOccupyMaxTermsAction->setText(QCoreApplication::translate("FetMainForm_template", "A set of activities occupies max terms", nullptr));
}

void FetMainForm::createMenusOfActionsForConstraints()
{
	menuMisc_time_constraints->clear();
	menuA_teacher_time_constraints->clear();
	menuAll_teachers_time_constraints->clear();
	menuA_students_set_time_constraints->clear();
	menuAll_students_time_constraints->clear();
	menuActivities_preferred_times_time_constraints->clear();
	menuActivities_others_time_constraints->clear();
	menuMisc_space_constraints->clear();
	menuRooms_space_constraints->clear();
	menuActivities_space_constraints->clear();
	menuSubjects_space_constraints->clear();
	menuSubjects_and_activity_tags_space_constraints->clear();
	menuStudents_space_constraints->clear();
	menuTeachers_space_constraints->clear();
	menuActivity_tags_space_constraints->clear();
	
	if(gt.rules.mode==OFFICIAL){
		menuMisc_time_constraints->addAction(dataTimeConstraintsBasicCompulsoryTimeAction);
		menuMisc_time_constraints->addAction(dataTimeConstraintsBreakTimesAction);

		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherNotAvailableTimesAction);
		menuA_teacher_time_constraints->addSeparator();
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxDaysPerWeekAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinDaysPerWeekAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxGapsPerDayAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxGapsPerWeekAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxHoursDailyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxSpanPerDayAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherActivityTagMaxHoursDailyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinHoursDailyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherActivityTagMinHoursDailyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxHoursContinuouslyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherActivityTagMaxHoursContinuouslyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherIntervalMaxDaysPerWeekAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinRestingHoursAction);

		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxDaysPerWeekAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMinDaysPerWeekAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxGapsPerDayAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxGapsPerWeekAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxHoursDailyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxSpanPerDayAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersActivityTagMaxHoursDailyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMinHoursDailyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersActivityTagMinHoursDailyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxHoursContinuouslyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersActivityTagMaxHoursContinuouslyAction);
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
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxSpanPerDayAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMinHoursDailyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetActivityTagMinHoursDailyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxHoursContinuouslyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetActivityTagMaxHoursContinuouslyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetIntervalMaxDaysPerWeekAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMinRestingHoursAction);

		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxDaysPerWeekAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxGapsPerDayAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxGapsPerWeekAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsEarlyMaxBeginningsAtSecondHourAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxHoursDailyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxSpanPerDayAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsActivityTagMaxHoursDailyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMinHoursDailyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsActivityTagMinHoursDailyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxHoursContinuouslyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsActivityTagMaxHoursContinuouslyAction);
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

		menuActivities_others_time_constraints->addAction(dataTimeConstraintsMinDaysBetweenActivitiesAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsMaxDaysBetweenActivitiesAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivityEndsStudentsDayAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesEndStudentsDayAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivityEndsTeachersDayAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesEndTeachersDayAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesSameStartingTimeAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesSameStartingDayAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesSameStartingHourAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesOccupyMaxTimeSlotsFromSelectionAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesOccupyMinTimeSlotsFromSelectionAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsTwoActivitiesOrderedAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsTwoActivitiesOrderedIfSameDayAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsTwoActivitiesConsecutiveAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsTwoActivitiesGroupedAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsThreeActivitiesGroupedAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesNotOverlappingAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivityTagsNotOverlappingAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesMaxSimultaneousInSelectedTimeSlotsAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesMinSimultaneousInSelectedTimeSlotsAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsMinGapsBetweenActivitiesAction);

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

		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetHomeRoomAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetHomeRoomsAction);
		menuStudents_space_constraints->addSeparator();
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxRoomChangesPerDayAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxRoomChangesPerWeekAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetMinGapsBetweenRoomChangesAction);
		menuStudents_space_constraints->addSeparator();
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxBuildingChangesPerWeekAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetMinGapsBetweenBuildingChangesAction);
		menuStudents_space_constraints->addSeparator();
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsMaxRoomChangesPerDayAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsMaxRoomChangesPerWeekAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsMinGapsBetweenRoomChangesAction);
		menuStudents_space_constraints->addSeparator();
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsMaxBuildingChangesPerDayAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsMaxBuildingChangesPerWeekAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsMinGapsBetweenBuildingChangesAction);

		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherHomeRoomAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherHomeRoomsAction);
		menuTeachers_space_constraints->addSeparator();
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherMaxRoomChangesPerDayAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherMaxRoomChangesPerWeekAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherMinGapsBetweenRoomChangesAction);
		menuTeachers_space_constraints->addSeparator();
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherMaxBuildingChangesPerDayAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherMaxBuildingChangesPerWeekAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherMinGapsBetweenBuildingChangesAction);
		menuTeachers_space_constraints->addSeparator();
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxRoomChangesPerDayAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxRoomChangesPerWeekAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeachersMinGapsBetweenRoomChangesAction);
		menuTeachers_space_constraints->addSeparator();
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxBuildingChangesPerDayAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxBuildingChangesPerWeekAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeachersMinGapsBetweenBuildingChangesAction);

		menuActivity_tags_space_constraints->addAction(dataSpaceConstraintsActivityTagPreferredRoomAction);
		menuActivity_tags_space_constraints->addAction(dataSpaceConstraintsActivityTagPreferredRoomsAction);
	}
	else if(gt.rules.mode==MORNINGS_AFTERNOONS){
		menuMisc_time_constraints->addAction(dataTimeConstraintsBasicCompulsoryTimeAction);
		menuMisc_time_constraints->addAction(dataTimeConstraintsBreakTimesAction);

		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherNotAvailableTimesAction);
		menuA_teacher_time_constraints->addSeparator();

		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxDaysPerWeekAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxRealDaysPerWeekAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxMorningsPerWeekAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxAfternoonsPerWeekAction);

		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxTwoConsecutiveMorningsAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxTwoConsecutiveAfternoonsAction);

		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinRealDaysPerWeekAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinMorningsPerWeekAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinAfternoonsPerWeekAction);
		
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxGapsPerDayAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxGapsPerWeekAction);

		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxGapsPerRealDayAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxGapsPerWeekForRealDaysAction);
		
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxZeroGapsPerAfternoonAction);

		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherAfternoonsEarlyMaxBeginningsAtSecondHourAction);

		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxHoursDailyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxHoursDailyRealDaysAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxSpanPerRealDayAction);
		
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherActivityTagMaxHoursDailyRealDaysAction);

		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxTwoActivityTagsPerDayFromN1N2N3Action);
		
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinHoursDailyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinHoursDailyRealDaysAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinHoursPerMorningAction);
		
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxHoursContinuouslyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherActivityTagMaxHoursContinuouslyAction);

		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsAction);

		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherIntervalMaxDaysPerWeekAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMorningIntervalMaxDaysPerWeekAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherAfternoonIntervalMaxDaysPerWeekAction);

		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxHoursPerAllAfternoonsAction);

		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinRestingHoursBetweenMorningAndAfternoonAction);

		//all teachers
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxDaysPerWeekAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxRealDaysPerWeekAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxMorningsPerWeekAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxAfternoonsPerWeekAction);

		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxTwoConsecutiveMorningsAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxTwoConsecutiveAfternoonsAction);

		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMinRealDaysPerWeekAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMinMorningsPerWeekAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMinAfternoonsPerWeekAction);
		
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxGapsPerDayAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxGapsPerWeekAction);

		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxGapsPerRealDayAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxGapsPerWeekForRealDaysAction);
		
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxZeroGapsPerAfternoonAction);

		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersAfternoonsEarlyMaxBeginningsAtSecondHourAction);

		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxHoursDailyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxHoursDailyRealDaysAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxSpanPerRealDayAction);
		
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersActivityTagMaxHoursDailyRealDaysAction);

		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxTwoActivityTagsPerDayFromN1N2N3Action);
		
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMinHoursDailyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMinHoursDailyRealDaysAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMinHoursPerMorningAction);
		
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxHoursContinuouslyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersActivityTagMaxHoursContinuouslyAction);

		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsAction);

		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersIntervalMaxDaysPerWeekAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMorningIntervalMaxDaysPerWeekAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersAfternoonIntervalMaxDaysPerWeekAction);

		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxHoursPerAllAfternoonsAction);

		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMinRestingHoursBetweenMorningAndAfternoonAction);
		/////////

		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetNotAvailableTimesAction);
		menuA_students_set_time_constraints->addSeparator();

		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxDaysPerWeekAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxRealDaysPerWeekAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxMorningsPerWeekAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxAfternoonsPerWeekAction);

		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMinMorningsPerWeekAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMinAfternoonsPerWeekAction);

		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxGapsPerDayAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxGapsPerWeekAction);

		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxGapsPerRealDayAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxGapsPerWeekForRealDaysAction);
		
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetEarlyMaxBeginningsAtSecondHourAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHourAction);

		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxHoursDailyRealDaysAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxSpanPerRealDayAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyRealDaysAction);
		
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMinHoursDailyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMinHoursPerMorningAction);
		
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxHoursContinuouslyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetActivityTagMaxHoursContinuouslyAction);

		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsAction);

		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetIntervalMaxDaysPerWeekAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMorningIntervalMaxDaysPerWeekAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetAfternoonIntervalMaxDaysPerWeekAction);

		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxHoursPerAllAfternoonsAction);

		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMinRestingHoursBetweenMorningAndAfternoonAction);
		
		///////
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxDaysPerWeekAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxRealDaysPerWeekAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxMorningsPerWeekAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxAfternoonsPerWeekAction);

		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMinMorningsPerWeekAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMinAfternoonsPerWeekAction);

		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxGapsPerDayAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxGapsPerWeekAction);

		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxGapsPerRealDayAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxGapsPerWeekForRealDaysAction);
		
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsEarlyMaxBeginningsAtSecondHourAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsAfternoonsEarlyMaxBeginningsAtSecondHourAction);

		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxHoursDailyRealDaysAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxSpanPerRealDayAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsActivityTagMaxHoursDailyRealDaysAction);
		
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMinHoursDailyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMinHoursPerMorningAction);
		
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxHoursContinuouslyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsActivityTagMaxHoursContinuouslyAction);

		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsAction);

		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsIntervalMaxDaysPerWeekAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMorningIntervalMaxDaysPerWeekAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsAfternoonIntervalMaxDaysPerWeekAction);

		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxHoursPerAllAfternoonsAction);

		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMinRestingHoursBetweenMorningAndAfternoonAction);
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

		menuActivities_others_time_constraints->addAction(dataTimeConstraintsMinDaysBetweenActivitiesAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsMaxDaysBetweenActivitiesAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivityEndsStudentsDayAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesEndStudentsDayAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivityEndsTeachersDayAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesEndTeachersDayAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesSameStartingTimeAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesSameStartingDayAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesSameStartingHourAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesOccupyMaxTimeSlotsFromSelectionAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesOccupyMinTimeSlotsFromSelectionAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsTwoActivitiesOrderedAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsTwoActivitiesOrderedIfSameDayAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsTwoActivitiesConsecutiveAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsTwoActivitiesGroupedAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsThreeActivitiesGroupedAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesNotOverlappingAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivityTagsNotOverlappingAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesMaxSimultaneousInSelectedTimeSlotsAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesMinSimultaneousInSelectedTimeSlotsAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsMinGapsBetweenActivitiesAction);

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

		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetHomeRoomAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetHomeRoomsAction);
		menuStudents_space_constraints->addSeparator();
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxRoomChangesPerDayAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxRoomChangesPerWeekAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetMinGapsBetweenRoomChangesAction);
		menuStudents_space_constraints->addSeparator();
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxRoomChangesPerRealDayAction);
		menuStudents_space_constraints->addSeparator();
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxBuildingChangesPerWeekAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetMinGapsBetweenBuildingChangesAction);
		menuStudents_space_constraints->addSeparator();
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsMaxRoomChangesPerDayAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsMaxRoomChangesPerWeekAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsMinGapsBetweenRoomChangesAction);
		menuStudents_space_constraints->addSeparator();
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsMaxRoomChangesPerRealDayAction);
		menuStudents_space_constraints->addSeparator();
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsMaxBuildingChangesPerDayAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsMaxBuildingChangesPerWeekAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsMinGapsBetweenBuildingChangesAction);

		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherHomeRoomAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherHomeRoomsAction);
		menuTeachers_space_constraints->addSeparator();
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherMaxRoomChangesPerDayAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherMaxRoomChangesPerWeekAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherMinGapsBetweenRoomChangesAction);
		menuTeachers_space_constraints->addSeparator();
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherMaxRoomChangesPerRealDayAction);
		menuTeachers_space_constraints->addSeparator();
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherMaxBuildingChangesPerDayAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherMaxBuildingChangesPerWeekAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherMinGapsBetweenBuildingChangesAction);
		menuTeachers_space_constraints->addSeparator();
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxRoomChangesPerDayAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxRoomChangesPerWeekAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeachersMinGapsBetweenRoomChangesAction);
		menuTeachers_space_constraints->addSeparator();
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxRoomChangesPerRealDayAction);
		menuTeachers_space_constraints->addSeparator();
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxBuildingChangesPerDayAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxBuildingChangesPerWeekAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeachersMinGapsBetweenBuildingChangesAction);

		menuActivity_tags_space_constraints->addAction(dataSpaceConstraintsActivityTagPreferredRoomAction);
		menuActivity_tags_space_constraints->addAction(dataSpaceConstraintsActivityTagPreferredRoomsAction);
	}
	else if(gt.rules.mode==BLOCK_PLANNING){
		menuMisc_time_constraints->addAction(dataTimeConstraintsBasicCompulsoryTimeAction);
		menuMisc_time_constraints->addAction(dataTimeConstraintsBreakTimesAction);

		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherNotAvailableTimesAction);
		menuA_teacher_time_constraints->addSeparator();
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxDaysPerWeekAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinDaysPerWeekAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxGapsPerDayAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxGapsPerWeekAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxHoursDailyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxSpanPerDayAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherActivityTagMaxHoursDailyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinHoursDailyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherActivityTagMinHoursDailyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxHoursContinuouslyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherActivityTagMaxHoursContinuouslyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherIntervalMaxDaysPerWeekAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinRestingHoursAction);

		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxDaysPerWeekAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMinDaysPerWeekAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxGapsPerDayAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxGapsPerWeekAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxHoursDailyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxSpanPerDayAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersActivityTagMaxHoursDailyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMinHoursDailyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersActivityTagMinHoursDailyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxHoursContinuouslyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersActivityTagMaxHoursContinuouslyAction);
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
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxSpanPerDayAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMinHoursDailyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetActivityTagMinHoursDailyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxHoursContinuouslyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetActivityTagMaxHoursContinuouslyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetIntervalMaxDaysPerWeekAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMinRestingHoursAction);

		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxDaysPerWeekAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxGapsPerDayAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxGapsPerWeekAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsEarlyMaxBeginningsAtSecondHourAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxHoursDailyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxSpanPerDayAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsActivityTagMaxHoursDailyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMinHoursDailyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsActivityTagMinHoursDailyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxHoursContinuouslyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsActivityTagMaxHoursContinuouslyAction);
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

		menuActivities_others_time_constraints->addAction(dataTimeConstraintsMinDaysBetweenActivitiesAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsMaxDaysBetweenActivitiesAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivityEndsStudentsDayAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesEndStudentsDayAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivityEndsTeachersDayAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesEndTeachersDayAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesSameStartingTimeAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesSameStartingDayAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesSameStartingHourAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesOccupyMaxTimeSlotsFromSelectionAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesOccupyMinTimeSlotsFromSelectionAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsMaxTotalActivitiesFromSetInSelectedTimeSlotsAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsTwoActivitiesOrderedAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsTwoActivitiesOrderedIfSameDayAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsTwoActivitiesConsecutiveAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsTwoActivitiesGroupedAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsThreeActivitiesGroupedAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesNotOverlappingAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivityTagsNotOverlappingAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesMaxSimultaneousInSelectedTimeSlotsAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesMinSimultaneousInSelectedTimeSlotsAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsMinGapsBetweenActivitiesAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsMaxGapsBetweenActivitiesAction);

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

		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetHomeRoomAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetHomeRoomsAction);
		menuStudents_space_constraints->addSeparator();
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxRoomChangesPerDayAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxRoomChangesPerWeekAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetMinGapsBetweenRoomChangesAction);
		menuStudents_space_constraints->addSeparator();
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxBuildingChangesPerWeekAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetMinGapsBetweenBuildingChangesAction);
		menuStudents_space_constraints->addSeparator();
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsMaxRoomChangesPerDayAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsMaxRoomChangesPerWeekAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsMinGapsBetweenRoomChangesAction);
		menuStudents_space_constraints->addSeparator();
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsMaxBuildingChangesPerDayAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsMaxBuildingChangesPerWeekAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsMinGapsBetweenBuildingChangesAction);

		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherHomeRoomAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherHomeRoomsAction);
		menuTeachers_space_constraints->addSeparator();
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherMaxRoomChangesPerDayAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherMaxRoomChangesPerWeekAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherMinGapsBetweenRoomChangesAction);
		menuTeachers_space_constraints->addSeparator();
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherMaxBuildingChangesPerDayAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherMaxBuildingChangesPerWeekAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherMinGapsBetweenBuildingChangesAction);
		menuTeachers_space_constraints->addSeparator();
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxRoomChangesPerDayAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxRoomChangesPerWeekAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeachersMinGapsBetweenRoomChangesAction);
		menuTeachers_space_constraints->addSeparator();
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxBuildingChangesPerDayAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxBuildingChangesPerWeekAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeachersMinGapsBetweenBuildingChangesAction);

		menuActivity_tags_space_constraints->addAction(dataSpaceConstraintsActivityTagPreferredRoomAction);
		menuActivity_tags_space_constraints->addAction(dataSpaceConstraintsActivityTagPreferredRoomsAction);
	}
	else if(gt.rules.mode==TERMS){
		menuMisc_time_constraints->addAction(dataTimeConstraintsBasicCompulsoryTimeAction);
		menuMisc_time_constraints->addAction(dataTimeConstraintsBreakTimesAction);

		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherNotAvailableTimesAction);
		menuA_teacher_time_constraints->addSeparator();
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxDaysPerWeekAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinDaysPerWeekAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxGapsPerDayAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxGapsPerWeekAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxHoursDailyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxSpanPerDayAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherActivityTagMaxHoursDailyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinHoursDailyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherActivityTagMinHoursDailyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMaxHoursContinuouslyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherActivityTagMaxHoursContinuouslyAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherIntervalMaxDaysPerWeekAction);
		menuA_teacher_time_constraints->addAction(dataTimeConstraintsTeacherMinRestingHoursAction);

		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxDaysPerWeekAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMinDaysPerWeekAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxGapsPerDayAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxGapsPerWeekAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxHoursDailyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxSpanPerDayAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersActivityTagMaxHoursDailyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMinHoursDailyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersActivityTagMinHoursDailyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersMaxHoursContinuouslyAction);
		menuAll_teachers_time_constraints->addAction(dataTimeConstraintsTeachersActivityTagMaxHoursContinuouslyAction);
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
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxSpanPerDayAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMinHoursDailyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetActivityTagMinHoursDailyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMaxHoursContinuouslyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetActivityTagMaxHoursContinuouslyAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetIntervalMaxDaysPerWeekAction);
		menuA_students_set_time_constraints->addAction(dataTimeConstraintsStudentsSetMinRestingHoursAction);

		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxDaysPerWeekAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxGapsPerDayAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxGapsPerWeekAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsEarlyMaxBeginningsAtSecondHourAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxHoursDailyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxSpanPerDayAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsActivityTagMaxHoursDailyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMinHoursDailyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsActivityTagMinHoursDailyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsMaxHoursContinuouslyAction);
		menuAll_students_time_constraints->addAction(dataTimeConstraintsStudentsActivityTagMaxHoursContinuouslyAction);
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

		menuActivities_others_time_constraints->addAction(dataTimeConstraintsMinDaysBetweenActivitiesAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsMaxDaysBetweenActivitiesAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivityEndsStudentsDayAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesEndStudentsDayAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivityEndsTeachersDayAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesEndTeachersDayAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesSameStartingTimeAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesSameStartingDayAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesSameStartingHourAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesOccupyMaxTimeSlotsFromSelectionAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesOccupyMinTimeSlotsFromSelectionAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsTwoActivitiesOrderedAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsTwoActivitiesOrderedIfSameDayAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsTwoActivitiesConsecutiveAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsTwoActivitiesGroupedAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsThreeActivitiesGroupedAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesNotOverlappingAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivityTagsNotOverlappingAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesMaxSimultaneousInSelectedTimeSlotsAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesMinSimultaneousInSelectedTimeSlotsAction);
		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsMinGapsBetweenActivitiesAction);

		menuActivities_others_time_constraints->addSeparator();
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesMaxInATermAction);
		menuActivities_others_time_constraints->addAction(dataTimeConstraintsActivitiesOccupyMaxTermsAction);

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

		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetHomeRoomAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetHomeRoomsAction);
		menuStudents_space_constraints->addSeparator();
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxRoomChangesPerDayAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxRoomChangesPerWeekAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetMinGapsBetweenRoomChangesAction);
		menuStudents_space_constraints->addSeparator();
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetMaxBuildingChangesPerWeekAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsSetMinGapsBetweenBuildingChangesAction);
		menuStudents_space_constraints->addSeparator();
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsMaxRoomChangesPerDayAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsMaxRoomChangesPerWeekAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsMinGapsBetweenRoomChangesAction);
		menuStudents_space_constraints->addSeparator();
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsMaxBuildingChangesPerDayAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsMaxBuildingChangesPerWeekAction);
		menuStudents_space_constraints->addAction(dataSpaceConstraintsStudentsMinGapsBetweenBuildingChangesAction);

		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherHomeRoomAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherHomeRoomsAction);
		menuTeachers_space_constraints->addSeparator();
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherMaxRoomChangesPerDayAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherMaxRoomChangesPerWeekAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherMinGapsBetweenRoomChangesAction);
		menuTeachers_space_constraints->addSeparator();
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherMaxBuildingChangesPerDayAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherMaxBuildingChangesPerWeekAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeacherMinGapsBetweenBuildingChangesAction);
		menuTeachers_space_constraints->addSeparator();
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxRoomChangesPerDayAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxRoomChangesPerWeekAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeachersMinGapsBetweenRoomChangesAction);
		menuTeachers_space_constraints->addSeparator();
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxBuildingChangesPerDayAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeachersMaxBuildingChangesPerWeekAction);
		menuTeachers_space_constraints->addAction(dataSpaceConstraintsTeachersMinGapsBetweenBuildingChangesAction);

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

	languagesMap.insert("ar", QString::fromUtf8("عربي"));
	languagesMap.insert("ca", QString::fromUtf8("Català"));
	languagesMap.insert("de", QString::fromUtf8("Deutsch"));
	languagesMap.insert("el", QString::fromUtf8("Ελληνικά"));
	languagesMap.insert("es", QString::fromUtf8("Español"));
	languagesMap.insert("fr", QString::fromUtf8("Français"));
	languagesMap.insert("id", QString::fromUtf8("Indonesia"));
	languagesMap.insert("it", QString::fromUtf8("Italiano"));
	languagesMap.insert("ro", QString::fromUtf8("Română"));
	languagesMap.insert("uk", QString::fromUtf8("Українська"));
	languagesMap.insert("pt_BR", QString::fromUtf8("Português Brasil"));
	languagesMap.insert("da", QString::fromUtf8("Dansk"));
	languagesMap.insert("sr", QString::fromUtf8("Српски"));
	languagesMap.insert("gl", QString::fromUtf8("Galego"));
	languagesMap.insert("vi", QString::fromUtf8("Tiếng Việt"));
	languagesMap.insert("sq", QString::fromUtf8("Shqip"));
	languagesMap.insert("zh_TW", QString::fromUtf8("正體字型"));
	languagesMap.insert("cs", QString::fromUtf8("Český"));
	languagesMap.insert("ja", QString::fromUtf8("日本語"));
	languagesMap.insert("ru", QString::fromUtf8("Русский"));

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
		s+=tr("Also, there will be visible on the server your current FET version, your operating system name and version, and your processor architecture type.");
		s+=" ";
		s+=tr("Thus, it could be deduced if and when you use FET.");
		s+="\n\n";
		s+=tr("Do you agree?");

		QMessageBox::StandardButton b=QMessageBox::question(this, tr("FET question"), s, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

		if(b!=QMessageBox::Yes){
			disconnect(checkForUpdatesAction, SIGNAL(toggled(bool)), this, SLOT(checkForUpdatesToggled(bool)));
			checkForUpdatesAction->setChecked(false);
			connect(checkForUpdatesAction, SIGNAL(toggled(bool)), this, SLOT(checkForUpdatesToggled(bool)));
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
			disconnect(settingsShowSubgroupsInComboBoxesAction, SIGNAL(toggled(bool)), this, SLOT(showSubgroupsInComboBoxesToggled(bool)));
			settingsShowSubgroupsInComboBoxesAction->setChecked(true);
			connect(settingsShowSubgroupsInComboBoxesAction, SIGNAL(toggled(bool)), this, SLOT(showSubgroupsInComboBoxesToggled(bool)));
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
			disconnect(settingsShowSubgroupsInActivityPlanningAction, SIGNAL(toggled(bool)), this, SLOT(showSubgroupsInActivityPlanningToggled(bool)));
			settingsShowSubgroupsInActivityPlanningAction->setChecked(true);
			connect(settingsShowSubgroupsInActivityPlanningAction, SIGNAL(toggled(bool)), this, SLOT(showSubgroupsInActivityPlanningToggled(bool)));
			return;
		}
	}
	
	SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING=checked;
}

void FetMainForm::on_modeOfficialAction_triggered()
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

	bool ok2=getLastConfirmation(OFFICIAL);
	if(!ok2){
		modeOfficialAction->setChecked(false);
		return;
	}

	gt.rules.setMode(OFFICIAL);
	updateMode();
}

void FetMainForm::on_modeMorningsAfternoonsAction_triggered()
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
		 " Also, the incompatible constraints will be removed.");
	}
	else if(gt.rules.mode==BLOCK_PLANNING){
		s=tr("Are you sure you want to convert your file to the mornings-afternoons mode? This might involve data loss."
		 " All the teachers with uninitialized mornings-afternoons behavior will have unrestricted mornings-afternoons behavior."
		 " Also, the incompatible constraints will be removed.");
	}
	else if(gt.rules.mode==TERMS){
		s=tr("Are you sure you want to convert your file to the mornings-afternoons mode? This might involve data loss."
		 " All the teachers with uninitialized mornings-afternoons behavior will have unrestricted mornings-afternoons behavior."
		 " Also, the incompatible constraints will be removed.");
	}
	else
		assert(0);
	int ok=QMessageBox::question(this, tr("FET confirmation"), s, QMessageBox::Cancel | QMessageBox::Ok);
	if(ok==QMessageBox::Cancel){
		modeMorningsAfternoonsAction->setChecked(false);
		return;
	}

	bool ok2=getLastConfirmation(MORNINGS_AFTERNOONS);
	if(!ok2){
		modeMorningsAfternoonsAction->setChecked(false);
		return;
	}

	gt.rules.setMode(MORNINGS_AFTERNOONS);
	updateMode();
}

void FetMainForm::on_modeBlockPlanningAction_triggered()
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

	bool ok2=getLastConfirmation(BLOCK_PLANNING);
	if(!ok2){
		modeBlockPlanningAction->setChecked(false);
		return;
	}

	gt.rules.setMode(BLOCK_PLANNING);
	updateMode();
}

void FetMainForm::on_modeTermsAction_triggered()
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

	bool ok2=getLastConfirmation(TERMS);
	if(!ok2){
		modeTermsAction->setChecked(false);
		return;
	}

	gt.rules.setMode(TERMS);
	updateMode();
}

void FetMainForm::on_dataTermsAction_triggered()
{
	assert(gt.rules.mode==TERMS);
	
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	TermsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

bool FetMainForm::getLastConfirmation(int newMode)
{
	QString removedTimeConstraintsString;
	QString removedSpaceConstraintsString;

	QList<TimeConstraint*> removedTimeConstraintsList;
	for(TimeConstraint* tc : qAsConst(gt.rules.timeConstraintsList))
		if((newMode==OFFICIAL && !tc->canBeUsedInOfficialMode())
		 || (newMode==MORNINGS_AFTERNOONS && !tc->canBeUsedInMorningsAfternoonsMode())
		 || (newMode==BLOCK_PLANNING && !tc->canBeUsedInBlockPlanningMode())
		 || (newMode==TERMS && !tc->canBeUsedInTermsMode())){
			removedTimeConstraintsList.append(tc);
			removedTimeConstraintsString+=tc->getDetailedDescription(gt.rules)+"\n";
		}

	QList<SpaceConstraint*> removedSpaceConstraintsList;
	for(SpaceConstraint* sc : qAsConst(gt.rules.spaceConstraintsList))
		if((newMode==OFFICIAL && !sc->canBeUsedInOfficialMode())
		 || (newMode==MORNINGS_AFTERNOONS && !sc->canBeUsedInMorningsAfternoonsMode())
		 || (newMode==BLOCK_PLANNING && !sc->canBeUsedInBlockPlanningMode())
		 || (newMode==TERMS && !sc->canBeUsedInTermsMode())){
			removedSpaceConstraintsList.append(sc);
			removedSpaceConstraintsString+=sc->getDetailedDescription(gt.rules)+"\n";
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
	QVBoxLayout* right=new QVBoxLayout();
	QHBoxLayout* all=new QHBoxLayout();
	all->addLayout(left);
	all->addLayout(right);

	QPlainTextEdit* remTim=new QPlainTextEdit();
	remTim->setReadOnly(true);
	remTim->setPlainText(removedTimeConstraintsString);
	QLabel* labRemTim=new QLabel(tr("These %1 time constraints will be removed:").arg(removedTimeConstraintsList.count()));
	labRemTim->setWordWrap(true);
	labRemTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labRemTim);
	left->addWidget(remTim);

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

	QObject::connect(lastpb2, SIGNAL(clicked()), &lastConfirmationDialog, SLOT(accept()));
	QObject::connect(lastpb1, SIGNAL(clicked()), &lastConfirmationDialog, SLOT(reject()));

	lastpb2->setDefault(true);
	lastpb2->setFocus();

	int lw=lastConfirmationDialog.sizeHint().width();
	int lh=lastConfirmationDialog.sizeHint().height();
	lastConfirmationDialog.resize(lw,lh);
	centerWidgetOnScreen(&lastConfirmationDialog);
	restoreFETDialogGeometry(&lastConfirmationDialog, QString("LastConfirmationChangeMode"));
	int ok=lastConfirmationDialog.exec();
	saveFETDialogGeometry(&lastConfirmationDialog, QString("LastConfirmationChangeMode"));
	if(ok==QDialog::Rejected)
		return false;
	assert(ok==QDialog::Accepted);
	////////////

	int removedTime=removedTimeConstraintsList.count();
	int removedSpace=removedSpaceConstraintsList.count();

	bool t=gt.rules.removeTimeConstraints(removedTimeConstraintsList);
	assert(t);

	removedTimeConstraintsList.clear();

	t=gt.rules.removeSpaceConstraints(removedSpaceConstraintsList);
	assert(t);

	removedSpaceConstraintsList.clear();

	QMessageBox::information(&lastConfirmationDialog, tr("FET information"), tr("There were removed %1 time constraints and"
	 " %2 space constraints.").arg(removedTime).arg(removedSpace));

	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	LockUnlock::increaseCommunicationSpinBox();

	return true;
}

/////////confirmations
void FetMainForm::on_settingsConfirmActivityPlanningAction_toggled()
{
	CONFIRM_ACTIVITY_PLANNING=settingsConfirmActivityPlanningAction->isChecked();
}

void FetMainForm::on_settingsConfirmSpreadActivitiesAction_toggled()
{
	CONFIRM_SPREAD_ACTIVITIES=settingsConfirmSpreadActivitiesAction->isChecked();
}

void FetMainForm::on_settingsConfirmRemoveRedundantAction_toggled()
{
	CONFIRM_REMOVE_REDUNDANT=settingsConfirmRemoveRedundantAction->isChecked();
}

void FetMainForm::on_settingsConfirmSaveTimetableAction_toggled()
{
	CONFIRM_SAVE_TIMETABLE=settingsConfirmSaveTimetableAction->isChecked();
}
/////////

void FetMainForm::on_settingsShowShortcutsOnMainWindowAction_toggled()
{
	SHOW_SHORTCUTS_ON_MAIN_WINDOW=settingsShowShortcutsOnMainWindowAction->isChecked();
	tabWidget->setVisible(SHOW_SHORTCUTS_ON_MAIN_WINDOW);
}

void FetMainForm::on_settingsShowToolTipsForConstraintsWithTablesAction_toggled()
{
	SHOW_TOOLTIPS_FOR_CONSTRAINTS_WITH_TABLES=settingsShowToolTipsForConstraintsWithTablesAction->isChecked();
}

void FetMainForm::on_settingsDivideTimetablesByDaysAction_toggled()
{
	DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS=settingsDivideTimetablesByDaysAction->isChecked();
}

void FetMainForm::on_settingsDuplicateVerticalNamesAction_toggled()
{
	TIMETABLE_HTML_REPEAT_NAMES=settingsDuplicateVerticalNamesAction->isChecked();
}

void FetMainForm::on_timetablesToWriteOnDiskAction_triggered()
{
	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	TimetablesToWriteOnDiskForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_studentsComboBoxesStyleAction_triggered()
{
	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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
		s+=tr("Could not search for possible updates on the internet - error message is: %1.").arg(networkReply->errorString());
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
	
	for(QWidget* wi : qAsConst(tlwl))
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

void FetMainForm::on_fileQuitAction_triggered()
{
	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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
	for(const QString& tn : qAsConst(recentFiles))
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

void FetMainForm::on_fileClearRecentFilesListAction_triggered()
{
	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	recentFiles.clear();
	updateRecentFileActions();
}

void FetMainForm::on_fileNewAction_triggered()
{
	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	int confirm=0;
	
	if(gt.rules.initialized && gt.rules.modified){
		switch( QMessageBox::question(
		 this,
		 tr("FET application"),
		 tr("Your current data file has been modified. Are you sure you want to reset to new empty data?"),
		 QMessageBox::Yes|QMessageBox::No
		 ) ){
		case QMessageBox::Yes: // Yes
			confirm=1;
			break;
		case QMessageBox::No: // No
			confirm=0;
			break;
		default:
			assert(0);
			break;
		}
	}
	else
		confirm=1;
	
	int tm=-1;
	if(confirm){
		GetModeForNewFileForm form(this);
		int result=form.exec();
		if(result==QDialog::Accepted){
			assert(form.mode==OFFICIAL || form.mode==MORNINGS_AFTERNOONS || form.mode==BLOCK_PLANNING || form.mode==TERMS);
			tm=form.mode;
		}
		else{
			confirm=0;
		}
	}

	if(confirm){
		INPUT_FILENAME_XML=QString("");
		setCurrentFile(INPUT_FILENAME_XML);
	
		if(gt.rules.initialized)
			gt.rules.kill();
		gt.rules.init();
		
		gt.rules.mode=tm;
		
		gt.rules.modified=true; //to avoid flicker of the main form modified flag

		bool tmp=gt.rules.addTimeConstraint(new ConstraintBasicCompulsoryTime(100));
		assert(tmp);
		tmp=gt.rules.addSpaceConstraint(new ConstraintBasicCompulsorySpace(100));
		assert(tmp);
		
		gt.rules.modified=true; //force update of the modified flag of the main window
		setRulesUnmodifiedAndOtherThings(&gt.rules);

		teachers_schedule_ready=false;
		students_schedule_ready=false;
		rooms_schedule_ready=false;

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

void FetMainForm::on_fileOpenAction_triggered()
{
	openFile(QString());
}

void FetMainForm::openFile(const QString& fileName)
{
	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	int confirm=0;
	
	if(gt.rules.initialized && gt.rules.modified){
		switch( QMessageBox::question(
		 this,
		 tr("FET application"),
		 tr("Your current data file has been modified. Are you sure you want to open another data file?"),
		 QMessageBox::Yes|QMessageBox::No
		 ) ){
		case QMessageBox::Yes: // Yes
			confirm=1;
			break;
		case QMessageBox::No: // No
			confirm=0;
			break;
		default:
			assert(0);
			break;
		}
	}
	else
		confirm=1;
	
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
				 tr("Please do not use a filename starting with white space(s), the html css code does not work."
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
			 tr("Please do not use quotation marks \" in filename, the html css code does not work."
			  " File was not loaded. Please rename it, removing not allowed characters and open it after that with FET."));
			return;
		}		
		if(s2.indexOf(";") >= 0){
			QMessageBox::warning(this, tr("FET information"),
			 tr("Please do not use semicolon ; in filename, the html css code does not work."
			  " File was not loaded. Please rename it, removing not allowed characters and open it after that with FET."));
			return;
		}
		if(s2.indexOf("#") >= 0){
			QMessageBox::warning(this, tr("FET information"),
			 tr("Please do not use # in filename, the html css code does not work."
			  " File was not loaded. Please rename it, removing not allowed characters and open it after that with FET."));
			return;
		}
		/*if(s2.indexOf("(") >= 0 || s2.indexOf(")")>=0){
			QMessageBox::information(this, tr("FET information"), tr("Please do not use parentheses () in filename, the html css code does not work"));
			return;
		}*/
		else{
			//QCursor orig=this->cursor();
			//this->setCursor(Qt::WaitCursor);
			statusBar()->showMessage(tr("Loading...", "This is a message in the status bar, that we are loading the file"), 0);
			pqapplication->processEvents();
		
			//bool before=gt.rules.modified;
			gt.rules.modified=true; //to avoid flicker of the main form modified flag

			if(gt.rules.read(this, s)){
				teachers_schedule_ready=false;
				students_schedule_ready=false;
				rooms_schedule_ready=false;

				INPUT_FILENAME_XML = s;
				
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
				//incorrect code - the old file may be broken - so we generate a new file.
				/*gt.rules.modified=before;
			
				statusBar()->showMessage("", STATUS_BAR_MILLISECONDS);
				
				setCurrentFile(INPUT_FILENAME_XML);*/

				//not needed, because if the file cannot be read we keep the old mode
				//updateMode();
				
				assert(!simulation_running);
				gt.rules.modified=false;
				statusBar()->showMessage(tr("Loading file failed...", "This is a message in the status bar, that opening the chosen file failed"), STATUS_BAR_MILLISECONDS);
				on_fileNewAction_triggered();
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
			 tr("Please do not use a filename starting with white space(s), the html css code does not work.")
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
		QMessageBox::warning(this, tr("FET information"), tr("Please do not use quotation marks \" in filename, the html css code does not work")
		 +"\n\n"+tr("File was not saved."));
		return false;
	}
	if(s2.indexOf(";") >= 0){
		QMessageBox::warning(this, tr("FET information"), tr("Please do not use semicolon ; in filename, the html css code does not work")
		 +"\n\n"+tr("File was not saved."));
		return false;
	}
	if(s2.indexOf("#") >= 0){
		QMessageBox::warning(this, tr("FET information"), tr("Please do not use # in filename, the html css code does not work")
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

void FetMainForm::on_fileSaveAsAction_triggered()
{
	fileSaveAs();
}

// Start of code contributed by Volker Dirr
void FetMainForm::on_fileImportCSVRoomsBuildingsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}
	Import::importCSVRoomsAndBuildings(this);
}

void FetMainForm::on_fileImportCSVSubjectsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}
	Import::importCSVSubjects(this);
}

void FetMainForm::on_fileImportCSVTeachersAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}
	Import::importCSVTeachers(this);
}

void FetMainForm::on_fileImportCSVActivitiesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}
	Import::importCSVActivities(this);

	//TODO: if the import takes care of locked activities, then we need
	//to do:
	//LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	//LockUnlock::increaseCommunicationSpinBox();
	//after the importing
}

void FetMainForm::on_fileImportCSVActivityTagsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}
	Import::importCSVActivityTags(this);
}

void FetMainForm::on_fileImportCSVYearsGroupsSubgroupsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}
	Import::importCSVStudents(this);
}

void FetMainForm::on_fileExportCSVAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}
	Export::exportCSV(this);
}
// End of code contributed by Volker Dirr

void FetMainForm::on_timetableSaveTimetableAsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!students_schedule_ready || !teachers_schedule_ready || !rooms_schedule_ready){
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
					 tr("Please do not use a filename starting with white space(s), the html css code does not work.")
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
				QMessageBox::warning(parent, tr("FET information"), tr("Please do not use quotation marks \" in filename, the html css code does not work")
				 +"\n\n"+tr("File was not saved."));
				return;
			}
			if(s2.indexOf(";") >= 0){
				QMessageBox::warning(parent, tr("FET information"), tr("Please do not use semicolon ; in filename, the html css code does not work")
				 +"\n\n"+tr("File was not saved."));
				return;
			}
			if(s2.indexOf("#") >= 0){
				QMessageBox::warning(parent, tr("FET information"), tr("Please do not use # in filename, the html css code does not work")
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
					for(int rr : qAsConst(tc->realRoomsList[ai]))
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
		
		for(TimeConstraint* tc : qAsConst(lockTimeConstraintsList))
			delete tc;
		lockTimeConstraintsList.clear();
		for(SpaceConstraint* sc : qAsConst(lockSpaceConstraintsList))
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

void FetMainForm::on_fileSaveAction_triggered()
{
	fileSave();
}

void FetMainForm::on_dataInstitutionNameAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	InstitutionNameForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataCommentsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	CommentsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataDaysAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	DaysForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataHoursAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	HoursForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataTeachersAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	TeachersForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataTeachersStatisticsAction_triggered()
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

void FetMainForm::on_dataSubjectsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	SubjectsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataSubjectsStatisticsAction_triggered()
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

void FetMainForm::on_dataActivityTagsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ActivityTagsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataYearsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	YearsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataGroupsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	GroupsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataSubgroupsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	SubgroupsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataStudentsStatisticsAction_triggered()
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

void FetMainForm::on_dataActivitiesRoomsStatisticsAction_triggered()
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

void FetMainForm::on_dataTeachersSubjectsQualificationsStatisticsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	QHash<QString, Teacher*> teachersHash;
	
	for(Teacher* tch : qAsConst(gt.rules.teachersList))
		teachersHash.insert(tch->name, tch);
		
	bool unqualifiedExist=false;

	QString s=tr("The teachers who are not qualified to teach a certain activity (in activities order):");
	s+="\n\n";
	
	bool begin=true;

	for(Activity* act : qAsConst(gt.rules.activitiesList)){
		bool alreadyAdded=false;
		QString subject=act->subjectName;
		for(const QString& teacher : qAsConst(act->teachersNames)){
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

void FetMainForm::on_helpSettingsAction_triggered()
{
	QString s;
	
	s+=tr("Probably some settings which are more difficult to understand are these ones:");
	s+="\n\n";
	s+=tr("Option 'Divide html timetables with time-axis by days':"
	" This means simply that the html timetables of type 'time horizontal' or 'time vertical' (see the generated html timetables)"
	" should be or not divided according to the days.");
	s+=" ";
	s+=tr("If the 'time horizontal' or 'time vertical' html timetables are too large for you, then you might need to select this option");
	
	s+="\n\n";
	s+=tr("Option 'Print activities with same starting time in timetables': selecting it means that the html timetables will contain for"
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
	s+=" -";
	s+=tr("activities and subactivities dialogs, the inactive activities will have a distinctive background color");
	s+="\n";
	s+=" -";
	s+=tr("all time constraints and all space constraints dialogs, the inactive constraints will have a distinctive background color");
	s+="\n";
	s+=" -";
	s+=tr("group activities in initial order items, the inactive items will have a distinctive background color");
	s+="\n";
	s+=" -";
	s+=tr("the timetable view dialogs");
	
	s+="\n\n";
	s+=tr("Enable activity tag max hours daily:");
	s+="\n";
	s+=tr("This will enable the menu for the constraints: teacher(s) or students (set) activity tag max hours daily / per real day (where applicable)."
		" These constraints are good, but not perfect and may bring slow down of generation or impossible timetables if used unproperly. Select only"
		" if you know what you're doing.");
	s+="\n\n";
	s+=tr("Enable students max gaps per day:");
	s+="\n";
	s+=tr("This will enable the menu for 2 constraints: students (set) max gaps per day. These 2 constraints are good, but not perfect and"
		" may bring slow down of generation or impossible timetables if used unproperly. Select only if you know what you're doing.");
		
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
		" on the hard disk, for operativity (the generation speed is not affected, only the overhead to write the partial/complete timetables"
		" when stopping/finishing the simulation). The timetables taking the longest time are the subgroups, groups AND years ones.");
	s+=" ";
	s+=tr("(Also the conflicts timetable might take long to write, if the file is large.)");
	s+=" ";
	s+=tr("After that, you can re-enable writing of the timetables and re-generate.");

	s+="\n\n";
	s+=tr("Show tool tips for constraints with tables: in the add/modify constraint dialogs which use tables, like the 'not available times' ones,"
		" each table cell will have a tool tip to show the current day/hour (column/row name).");
	
	LongTextMessageBox::largeInformation(this, tr("FET information"), s);
}

void FetMainForm::on_dataHelpOnStatisticsAction_triggered()
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

void FetMainForm::on_dataActivitiesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ActivitiesForm form(this, "", "", "", "");
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataSubactivitiesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	SubactivitiesForm form(this, "", "", "", "");
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataRoomsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	RoomsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataBuildingsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	BuildingsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataAllTimeConstraintsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	AllTimeConstraintsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataAllSpaceConstraintsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTwoActivitiesOrderedForm form(this);
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintActivitiesEndTeachersDayForm form(this);
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeacherNotAvailableTimesForm form(this);
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsMaxBuildingChangesPerDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataSpaceConstraintsStudentsSetMaxBuildingChangesPerWeekAction_triggered()
{
	if(simulation_running || simulation_running_multi){
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeachersMaxHoursDailyForm form(this);
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintActivitiesMaxInATermForm form(this);
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintActivitiesOccupyMaxTermsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_groupActivitiesInInitialOrderAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeacherMaxGapsPerDayForm form(this);
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsActivitiesNotOverlappingAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintMinDaysBetweenActivitiesForm form(this);
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintMaxDaysBetweenActivitiesForm form(this);
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeachersMaxRoomChangesPerRealDayForm form(this);
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeachersMaxMorningsPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::dataTimeConstraintsTeacherMaxTwoConsecutiveMorningsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeachersMaxTwoActivityTagsPerDayFromN1N2N3Form form(this);
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeacherMinHoursPerMorningForm form(this);
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHourForm form(this);
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsMinHoursPerMorningForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_helpMoroccoAction_triggered()
{
	HelpMoroccoForm* form=new HelpMoroccoForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
}

void FetMainForm::on_helpAlgeriaAction_triggered()
{
	HelpAlgeriaForm* form=new HelpAlgeriaForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
}

void FetMainForm::on_helpAboutFETAction_triggered()
{
	HelpAboutForm* form=new HelpAboutForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
}

void FetMainForm::on_helpAboutQtAction_triggered()
{
	QMessageBox::aboutQt(this);
}

void FetMainForm::on_helpAboutLibrariesAction_triggered()
{
	HelpAboutLibrariesForm* form=new HelpAboutLibrariesForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
}

void FetMainForm::on_helpHomepageAction_triggered()
{
	bool tds=QDesktopServices::openUrl(QUrl("https://lalescu.ro/liviu/fet/"));

	if(!tds){
		QMessageBox::warning(this, tr("FET warning"), tr("Could not start the default internet browser (trying to open the link %1)."
		" Maybe you can try to manually start your browser and open this link.").arg("https://lalescu.ro/liviu/fet/"));
	}
}

void FetMainForm::on_helpContentsAction_triggered()
{
	bool tds=QDesktopServices::openUrl(QUrl("https://lalescu.ro/liviu/fet/doc/"));

	if(!tds){
		QMessageBox::warning(this, tr("FET warning"), tr("Could not start the default internet browser (trying to open the link %1)."
		" Maybe you can try to manually start your browser and open this link.").arg("https://lalescu.ro/liviu/fet/doc/"));
	}
}

void FetMainForm::on_helpForumAction_triggered()
{
	bool tds=QDesktopServices::openUrl(QUrl("https://lalescu.ro/liviu/fet/forum/"));

	if(!tds){
		QMessageBox::warning(this, tr("FET warning"), tr("Could not start the default internet browser (trying to open the link %1)."
		" Maybe you can try to manually start your browser and open this link.").arg("https://lalescu.ro/liviu/fet/forum/"));
	}
}

void FetMainForm::on_helpAddressesAction_triggered()
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
	s+=tr("In case these addresses do not function, maybe the FET webpage has temporary problems, so try again later. Or maybe the FET webpage has changed, so search for the new page on the internet.");

	LongTextMessageBox::largeInformation(this, tr("FET web addresses"), s);
}

void FetMainForm::on_helpBlockPlanningAction_triggered()
{
	HelpBlockPlanningForm* form=new HelpBlockPlanningForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
}

void FetMainForm::on_helpTermsAction_triggered()
{
	HelpTermsForm* form=new HelpTermsForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
}

void FetMainForm::on_helpFAQAction_triggered()
{
	HelpFaqForm* form=new HelpFaqForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
}

void FetMainForm::on_helpTipsAction_triggered()
{
	HelpTipsForm* form=new HelpTipsForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
}

void FetMainForm::on_helpInstructionsAction_triggered()
{
	HelpInstructionsForm* form=new HelpInstructionsForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
}

void FetMainForm::on_timetableGenerateAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

void FetMainForm::on_timetableGenerateMultipleAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

void FetMainForm::on_timetableViewStudentsDaysHorizontalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready)){
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

void FetMainForm::on_timetableViewStudentsTimeHorizontalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready)){
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

void FetMainForm::on_timetableViewTeachersDaysHorizontalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready)){
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

void FetMainForm::on_timetableViewTeachersTimeHorizontalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready)){
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

void FetMainForm::on_timetableShowConflictsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready)){
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

void FetMainForm::on_timetableViewRoomsDaysHorizontalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready)){
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

void FetMainForm::on_timetableViewRoomsTimeHorizontalAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready)){
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

void FetMainForm::on_timetablePrintAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready)){
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

void FetMainForm::on_statisticsPrintAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	StartStatisticsPrint::startStatisticsPrint(this);
}

void FetMainForm::on_timetableLockAllActivitiesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready)){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	AdvancedLockUnlockForm::lockAll(this);
}

void FetMainForm::on_timetableUnlockAllActivitiesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready)){
		//QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		QMessageBox::information(this, tr("FET information"), tr("The timetable is not generated, but anyway FET will proceed now"));

		AdvancedLockUnlockForm::unlockAllWithoutTimetable(this);
	
		return;
	}

	//AdvancedLockUnlockForm::unlockAll(this);
	AdvancedLockUnlockForm::unlockAllWithoutTimetable(this);
}

void FetMainForm::on_timetableLockActivitiesDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready)){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	AdvancedLockUnlockForm::lockDay(this);
}

void FetMainForm::on_timetableUnlockActivitiesDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready)){
		//QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		QMessageBox::information(this, tr("FET information"), tr("The timetable is not generated, but anyway FET will proceed now"));
		
		AdvancedLockUnlockForm::unlockDayWithoutTimetable(this);
		
		return;
	}

	//AdvancedLockUnlockForm::unlockDay(this);
	AdvancedLockUnlockForm::unlockDayWithoutTimetable(this);
}

void FetMainForm::on_timetableLockActivitiesEndStudentsDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready)){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	AdvancedLockUnlockForm::lockEndStudentsDay(this);
}

void FetMainForm::on_timetableUnlockActivitiesEndStudentsDayAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready)){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	AdvancedLockUnlockForm::unlockEndStudentsDay(this);
}

void FetMainForm::on_timetableLockActivitiesWithASpecifiedActivityTagAction_triggered()
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

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready)){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	AdvancedLockUnlockForm::lockActivityTag(this);
}

void FetMainForm::on_timetableUnlockActivitiesWithASpecifiedActivityTagAction_triggered()
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

	if(!(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready)){
		//QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		QMessageBox::information(this, tr("FET information"), tr("The timetable is not generated, but anyway FET will proceed now"));
		
		AdvancedLockUnlockForm::unlockActivityTagWithoutTimetable(this);
		
		return;
	}

	//AdvancedLockUnlockForm::unlockActivityTag(this);
	AdvancedLockUnlockForm::unlockActivityTagWithoutTimetable(this);
}

void FetMainForm::on_languageAction_triggered()
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

	QObject::connect(tapb2, SIGNAL(clicked()), &dialog, SLOT(accept()));
	QObject::connect(tapb1, SIGNAL(clicked()), &dialog, SLOT(reject()));
	
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

void FetMainForm::on_settingsRestoreDefaultsAction_triggered()
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

	s+=tr("3")+QString(". ")+tr("In the shortcuts tab from the main window, the first section will be selected/shown", "Option refers to the main window tab widget for shortcuts, which currently contains 5 tabs: File, Data, "
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

	s+=tr("11")+QString(". ")+tr("Html level of the timetables will be %1", "%1 is default html level").arg(2);
	s+="\n";

	s+=tr("12")+QString(". ")+tr("Mark not available slots with -x- in timetables will be %1", "%1 is true or false. Lowercase -x-").arg(tr("true"));
	s+="\n";

	s+=tr("13")+QString(". ")+tr("Mark break slots with -X- in timetables will be %1", "%1 is true or false. Uppercase -X-").arg(tr("true"));
	s+="\n";

	s+=tr("14")+QString(". ")+tr("Divide html timetables with time-axis by days will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

	s+=tr("15")+QString(". ")+tr("Duplicate vertical headers to the right will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

	s+=tr("16")+QString(". ")+tr("Print activities with same starting time will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

	s+=tr("17")+QString(". ")+tr("Print activities tags will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";

	s+=tr("18")+QString(". ")+tr("Enable activity tag max hours daily will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

	s+=tr("19")+QString(". ")+tr("Enable activity tag min hours daily will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

	s+=tr("20")+QString(". ")+tr("Enable students max gaps per day will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

	s+=tr("21")+QString(". ")+tr("Warn if using not perfect constraints will be %1", "%1 is true or false. This is a warning if user uses not perfect constraints").arg(tr("true"));
	s+="\n";

	s+=tr("22")+QString(". ")+tr("Enable constraints students min hours daily with empty days will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

	s+=tr("23")+QString(". ")+tr("Warn if using constraints students min hours daily with empty days will be %1", "%1 is true or false. This is a warning if user uses a nonstandard constraint"
		" students min hours daily with allowed empty days").arg(tr("true"));
	s+="\n";

	///////////////confirmations
	s+=tr("24")+QString(". ")+tr("Confirm activity planning will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";
	s+=tr("25")+QString(". ")+tr("Confirm spread activities over the week will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";
	s+=tr("26")+QString(". ")+tr("Confirm remove redundant constraints will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";
	s+=tr("27")+QString(". ")+tr("Confirm save data and timetable as will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";
	///////////////
	
	s+=tr("28")+QString(". ")+tr("Enable group activities in the initial order of generation will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";
	s+=tr("29")+QString(". ")+tr("Warn if using group activities in the initial order of generation will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";
	///////////////

	s+=tr("30")+QString(". ")+tr("Show subgroups in combo boxes will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";
	s+=tr("31")+QString(". ")+tr("Show subgroups in activity planning will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";
	///////////////

	s+=tr("32")+QString(". ")+tr("Write on disk the %1 timetable will be %2", "%1 is a category of timetables, like conflicts, %2 is true or false")
	 .arg(tr("conflicts")).arg(tr("true"));
	s+="\n";

	s+=tr("33")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("statistics")).arg(tr("true"));
	s+="\n";
	s+=tr("34")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("XML")).arg(tr("true"));
	s+="\n";
	s+=tr("35")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("days horizontal")).arg(tr("true"));
	s+="\n";
	s+=tr("36")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("days vertical")).arg(tr("true"));
	s+="\n";
	s+=tr("37")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("time horizontal")).arg(tr("true"));
	s+="\n";
	s+=tr("38")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("time vertical")).arg(tr("true"));
	s+="\n";

	s+=tr("39")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("subgroups")).arg(tr("true"));
	s+="\n";
	s+=tr("40")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("groups")).arg(tr("true"));
	s+="\n";
	s+=tr("41")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("years")).arg(tr("true"));
	s+="\n";
	s+=tr("42")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("teachers")).arg(tr("true"));
	s+="\n";
	s+=tr("43")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("teachers free periods")).arg(tr("true"));
	s+="\n";
	s+=tr("44")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("rooms")).arg(tr("true"));
	s+="\n";
	s+=tr("45")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("subjects")).arg(tr("true"));
	s+="\n";
	s+=tr("46")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("activity tags")).arg(tr("true"));
	s+="\n";
	s+=tr("47")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("activities")).arg(tr("true"));
	s+="\n";
	s+=tr("48")+QString(". ")+tr("Show tool tips for constraints with tables will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";
	s+=tr("49")+QString(". ")+tr("Show warning for subgroups with the same activities will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";
	s+=tr("50")+QString(". ")+tr("Print detailed timetables will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";
	s+=tr("51")+QString(". ")+tr("Print detailed teachers' free periods timetables will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";
	
	//Just to have them translated, in case I need them.
	QString t1=tr("simple", "It is a style for students' combo boxes");
	Q_UNUSED(t1);
	QString t2=tr("icons", "It is a style for students' combo boxes");
	Q_UNUSED(t2);
	QString t3=tr("categorized", "It is a style for students' combo boxes");
	Q_UNUSED(t3);
	
	s+=tr("52")+QString(". ")+tr("Students' combo boxes style will be %1").arg(tr("simple", "It is a style for students' combo boxes"));
	s+="\n";
	
	s+=tr("53")+QString(". ")+tr("Print virtual rooms in the timetables will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

	s+=tr("54")+QString(". ")+tr("Show warning for activities not locked in time but locked in space in virtual rooms specifying the"
	 " real rooms will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";

	s+=tr("55")+QString(". ")+tr("Beep at the end of the generation will be %1, run external command at the end of generation will be %2,"
	 " and the external command will be empty",
	 "%1 and %2 are true or false").arg(tr("true")).arg(tr("false"));
	s+="\n";

	s+=tr("56")+QString(". ")+tr("Show warning if using constraints of type max hours daily with a weight less than 100%"
	 " will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";

	s+=tr("57")+QString(". ")+tr("Enable max gaps per real day / per week for real days will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

	s+=tr("58")+QString(". ")+tr("Enable constraints students min hours per morning with empty mornings will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

	s+=tr("59")+QString(". ")+tr("Warn if using constraints students min hours per morning with empty mornings will be %1", "%1 is true or false. This is a warning if user uses a nonstandard constraint"
		" students min hours per morning with allowed empty mornings").arg(tr("true"));
	s+="\n";

	switch( LongTextMessageBox::largeConfirmation( this, tr("FET confirmation"), s,
	 tr("&Yes"), tr("&No"), QString(), 0 , 1 ) ) {
	case 0: // Yes
		break;
	case 1: // No
		return;
	}

	QSettings settings(COMPANY, PROGRAM);
	settings.clear();
	
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
	
	disconnect(settingsUseColorsAction, SIGNAL(toggled(bool)), this, SLOT(useColorsToggled(bool)));
	USE_GUI_COLORS=false;
	settingsUseColorsAction->setChecked(USE_GUI_COLORS);
	connect(settingsUseColorsAction, SIGNAL(toggled(bool)), this, SLOT(useColorsToggled(bool)));
	
	SHOW_SUBGROUPS_IN_COMBO_BOXES=true;
	settingsShowSubgroupsInComboBoxesAction->setChecked(SHOW_SUBGROUPS_IN_COMBO_BOXES);
	
	SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING=true;
	settingsShowSubgroupsInActivityPlanningAction->setChecked(SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING);

	///
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
	enableMaxGapsPerRealDayAction->setChecked(ENABLE_MAX_GAPS_PER_REAL_DAY);

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
	enableStudentsMinHoursDailyWithAllowEmptyDaysAction->setChecked(ENABLE_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS);

	SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS=true;
	showWarningForStudentsMinHoursDailyWithAllowEmptyDaysAction->setChecked(SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS);

	ENABLE_STUDENTS_MIN_HOURS_PER_MORNING_WITH_ALLOW_EMPTY_MORNINGS=false;
	enableStudentsMinHoursPerMorningWithAllowEmptyMorningsAction->setChecked(ENABLE_STUDENTS_MIN_HOURS_PER_MORNING_WITH_ALLOW_EMPTY_MORNINGS);

	SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_PER_MORNING_WITH_ALLOW_EMPTY_MORNINGS=true;
	showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsAction->setChecked(SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_PER_MORNING_WITH_ALLOW_EMPTY_MORNINGS);

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
	
	settingsPrintActivityTagsAction->setChecked(true);
	TIMETABLE_HTML_PRINT_ACTIVITY_TAGS=true;
	
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
	
	BEEP_AT_END_OF_GENERATION=true;
	ENABLE_COMMAND_AT_END_OF_GENERATION=false;
	commandAtEndOfGeneration=QString("");
//	DETACHED_NOTIFICATION=false;
//	terminateCommandAfterSeconds=0;
//	killCommandAfterSeconds=0;

	setLanguage(*pqapplication, this);
	setCurrentFile(INPUT_FILENAME_XML);

	if(teachers_schedule_ready && students_schedule_ready && rooms_schedule_ready)
		LockUnlock::increaseCommunicationSpinBox(); //for GUI colors in timetables
}

void FetMainForm::on_settingsTimetableHtmlLevelAction_triggered()
{
	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	SettingsTimetableHtmlLevelForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_settingsPrintActivityTagsAction_toggled()
{
	TIMETABLE_HTML_PRINT_ACTIVITY_TAGS=settingsPrintActivityTagsAction->isChecked();
}

void FetMainForm::on_settingsShowVirtualRoomsInTimetablesAction_toggled()
{
	SHOW_VIRTUAL_ROOMS_IN_TIMETABLES=settingsShowVirtualRoomsInTimetablesAction->isChecked();
}

void FetMainForm::on_settingsPrintDetailedTimetablesAction_toggled()
{
	PRINT_DETAILED_HTML_TIMETABLES=settingsPrintDetailedTimetablesAction->isChecked();
}

void FetMainForm::on_settingsPrintDetailedTeachersFreePeriodsTimetablesAction_toggled()
{
	PRINT_DETAILED_HTML_TEACHERS_FREE_PERIODS=settingsPrintDetailedTeachersFreePeriodsTimetablesAction->isChecked();
}

void FetMainForm::on_settingsPrintNotAvailableSlotsAction_toggled()
{
	PRINT_NOT_AVAILABLE_TIME_SLOTS=settingsPrintNotAvailableSlotsAction->isChecked();
}

void FetMainForm::on_settingsPrintBreakSlotsAction_toggled()
{
	PRINT_BREAK_TIME_SLOTS=settingsPrintBreakSlotsAction->isChecked();
}

void FetMainForm::on_settingsPrintActivitiesWithSameStartingTimeAction_toggled()
{
	PRINT_ACTIVITIES_WITH_SAME_STARTING_TIME=settingsPrintActivitiesWithSameStartingTimeAction->isChecked();
}

void FetMainForm::on_settingsCommandAfterFinishingAction_triggered()
{
	NotificationCommandForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_activityPlanningAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

void FetMainForm::on_spreadActivitiesAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}
	
	if(gt.rules.nDaysPerWeek>=7){
		QString s;
		s=tr("You have more than 6 days per week, so probably you won't need this feature. Do you still want to continue?");
		
		int cfrm=0;
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
		}
	}

	if(gt.rules.mode!=MORNINGS_AFTERNOONS && gt.rules.nDaysPerWeek<=4){
		QString s;
		s=tr("You have less than 5 days per week, so probably you won't need this feature. Do you still want to continue?");

		int cfrm=0;
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
		}
	}
	else if(gt.rules.mode==MORNINGS_AFTERNOONS && gt.rules.nDaysPerWeek/2<=4){
		QString s;
		s=tr("You have less than 5 real days per week, so probably you won't need this feature. Do you still want to continue?");

		int cfrm=0;
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
		}
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

void FetMainForm::on_statisticsExportToDiskAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	StatisticsExport::exportStatistics(this);
}

void FetMainForm::on_removeRedundantConstraintsAction_triggered()
{
	if(!gt.rules.initialized){
		QMessageBox::information(this, tr("FET information"),
			tr("Please start a new file or open an existing one before accessing/modifying/saving/exporting the data."));
		return;
	}

	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

void FetMainForm::on_selectOutputDirAction_triggered()
{
	QString od;
	
	od = QFileDialog::getExistingDirectory(this, tr("Choose results (output) directory"), OUTPUT_DIR);
	
	if(!od.isNull()){
		QFile test(od+FILE_SEP+"test_write_permissions_3.tmp");
		bool existedBefore=test.exists();
		bool t=test.open(QIODevice::ReadWrite);
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

void FetMainForm::on_randomSeedAction_triggered()
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
			disconnect(enableActivityTagMaxHoursDailyAction, SIGNAL(toggled(bool)), this, SLOT(enableActivityTagMaxHoursDailyToggled(bool)));
			enableActivityTagMaxHoursDailyAction->setChecked(false);
			connect(enableActivityTagMaxHoursDailyAction, SIGNAL(toggled(bool)), this, SLOT(enableActivityTagMaxHoursDailyToggled(bool)));
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
			disconnect(enableActivityTagMinHoursDailyAction, SIGNAL(toggled(bool)), this, SLOT(enableActivityTagMinHoursDailyToggled(bool)));
			enableActivityTagMinHoursDailyAction->setChecked(false);
			connect(enableActivityTagMinHoursDailyAction, SIGNAL(toggled(bool)), this, SLOT(enableActivityTagMinHoursDailyToggled(bool)));
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
			disconnect(enableStudentsMaxGapsPerDayAction, SIGNAL(toggled(bool)), this, SLOT(enableStudentsMaxGapsPerDayToggled(bool)));
			enableStudentsMaxGapsPerDayAction->setChecked(false);
			connect(enableStudentsMaxGapsPerDayAction, SIGNAL(toggled(bool)), this, SLOT(enableStudentsMaxGapsPerDayToggled(bool)));
			return;
		}
	}
	
	ENABLE_STUDENTS_MAX_GAPS_PER_DAY=checked;
	
	setEnabledIcon(dataTimeConstraintsStudentsSetMaxGapsPerDayAction, ENABLE_STUDENTS_MAX_GAPS_PER_DAY);
	setEnabledIcon(dataTimeConstraintsStudentsMaxGapsPerDayAction, ENABLE_STUDENTS_MAX_GAPS_PER_DAY);
}

void FetMainForm::enableMaxGapsPerRealDayToggled(bool checked)
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
			disconnect(enableMaxGapsPerRealDayAction, SIGNAL(toggled(bool)), this, SLOT(enableMaxGapsPerRealDayToggled(bool)));
			enableMaxGapsPerRealDayAction->setChecked(false);
			connect(enableMaxGapsPerRealDayAction, SIGNAL(toggled(bool)), this, SLOT(enableMaxGapsPerRealDayToggled(bool)));
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
			disconnect(showWarningForSubgroupsWithTheSameActivitiesAction, SIGNAL(toggled(bool)), this, SLOT(showWarningForSubgroupsWithTheSameActivitiesToggled(bool)));
			showWarningForSubgroupsWithTheSameActivitiesAction->setChecked(true);
			connect(showWarningForSubgroupsWithTheSameActivitiesAction, SIGNAL(toggled(bool)), this, SLOT(showWarningForSubgroupsWithTheSameActivitiesToggled(bool)));
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
			disconnect(showWarningForActivitiesNotLockedTimeLockedSpaceVirtualRealRoomsAction, SIGNAL(toggled(bool)), this, SLOT(showWarningForActivitiesNotLockedTimeLockedSpaceVirtualRealRoomsToggled(bool)));
			showWarningForActivitiesNotLockedTimeLockedSpaceVirtualRealRoomsAction->setChecked(true);
			connect(showWarningForActivitiesNotLockedTimeLockedSpaceVirtualRealRoomsAction, SIGNAL(toggled(bool)), this, SLOT(showWarningForActivitiesNotLockedTimeLockedSpaceVirtualRealRoomsToggled(bool)));
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
			disconnect(showWarningForMaxHoursDailyWithUnder100WeightAction, SIGNAL(toggled(bool)), this, SLOT(showWarningForMaxHoursDailyWithUnder100WeightToggled(bool)));
			showWarningForMaxHoursDailyWithUnder100WeightAction->setChecked(true);
			connect(showWarningForMaxHoursDailyWithUnder100WeightAction, SIGNAL(toggled(bool)), this, SLOT(showWarningForMaxHoursDailyWithUnder100WeightToggled(bool)));
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
			disconnect(showWarningForNotPerfectConstraintsAction, SIGNAL(toggled(bool)), this, SLOT(showWarningForNotPerfectConstraintsToggled(bool)));
			showWarningForNotPerfectConstraintsAction->setChecked(true);
			connect(showWarningForNotPerfectConstraintsAction, SIGNAL(toggled(bool)), this, SLOT(showWarningForNotPerfectConstraintsToggled(bool)));
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
		s+=tr("Please use such constraints only if you are sure that there exists a timetable with empty days for students. If your instution requires"
			" each day for the students to be not empty, or if there does not exist a solution with empty days for students, "
			"it is IMPERATIVE (for performance reasons) that you use the standard constraint which does not allow empty days."
			" Otherwise, the timetable may be impossible to find.");
		s+="\n\n";
		s+=tr("Continue only if you know what you are doing.");
	
		QMessageBox::StandardButton b=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
	
		if(b!=QMessageBox::Ok){
			disconnect(enableStudentsMinHoursDailyWithAllowEmptyDaysAction, SIGNAL(toggled(bool)), this, SLOT(enableStudentsMinHoursDailyWithAllowEmptyDaysToggled(bool)));
			enableStudentsMinHoursDailyWithAllowEmptyDaysAction->setChecked(false);
			connect(enableStudentsMinHoursDailyWithAllowEmptyDaysAction, SIGNAL(toggled(bool)), this, SLOT(enableStudentsMinHoursDailyWithAllowEmptyDaysToggled(bool)));
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
			disconnect(showWarningForStudentsMinHoursDailyWithAllowEmptyDaysAction, SIGNAL(toggled(bool)), this, SLOT(showWarningForStudentsMinHoursDailyWithAllowEmptyDaysToggled(bool)));
			showWarningForStudentsMinHoursDailyWithAllowEmptyDaysAction->setChecked(true);
			connect(showWarningForStudentsMinHoursDailyWithAllowEmptyDaysAction, SIGNAL(toggled(bool)), this, SLOT(showWarningForStudentsMinHoursDailyWithAllowEmptyDaysToggled(bool)));
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

	disconnect(enableStudentsMinHoursPerMorningWithAllowEmptyMorningsAction, SIGNAL(toggled(bool)), this, SLOT(enableStudentsMinHoursPerMorningWithAllowEmptyMorningsToggled(bool)));
	enableStudentsMinHoursPerMorningWithAllowEmptyMorningsAction->setChecked(true);
	connect(enableStudentsMinHoursPerMorningWithAllowEmptyMorningsAction, SIGNAL(toggled(bool)), this, SLOT(enableStudentsMinHoursPerMorningWithAllowEmptyMorningsToggled(bool)));*/

	return;
}

void FetMainForm::showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsToggled(bool checked)
{
	Q_UNUSED(checked);

	assert(0); //Yes, this option is always disabled

	/*QMessageBox::information(this, tr("FET information"), tr("This option must remain unselected in the custom FET version MA"));

	disconnect(showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsAction, SIGNAL(toggled(bool)), this, SLOT(showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsToggled(bool)));
	showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsAction->setChecked(false);
	connect(showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsAction, SIGNAL(toggled(bool)), this, SLOT(showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsToggled(bool)));*/

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
			disconnect(enableGroupActivitiesInInitialOrderAction, SIGNAL(toggled(bool)), this, SLOT(enableGroupActivitiesInInitialOrderToggled(bool)));
			enableGroupActivitiesInInitialOrderAction->setChecked(false);
			connect(enableGroupActivitiesInInitialOrderAction, SIGNAL(toggled(bool)), this, SLOT(enableGroupActivitiesInInitialOrderToggled(bool)));
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
			disconnect(showWarningForGroupActivitiesInInitialOrderAction, SIGNAL(toggled(bool)), this, SLOT(showWarningForGroupActivitiesInInitialOrderToggled(bool)));
			showWarningForGroupActivitiesInInitialOrderAction->setChecked(true);
			connect(showWarningForGroupActivitiesInInitialOrderAction, SIGNAL(toggled(bool)), this, SLOT(showWarningForGroupActivitiesInInitialOrderToggled(bool)));
			return;
		}
	}
	
	SHOW_WARNING_FOR_GROUP_ACTIVITIES_IN_INITIAL_ORDER=checked;
}


//time constraints
void FetMainForm::on_shortcutAllTimeConstraintsPushButton_clicked()
{
	on_dataAllTimeConstraintsAction_triggered();
}

void FetMainForm::on_shortcutBreakTimeConstraintsPushButton_clicked()
{
	dataTimeConstraintsBreakTimesAction_triggered();
}

void FetMainForm::on_shortcutTeachersTimeConstraintsPushButton_clicked()
{
	menuTeachers_time_constraints->popup(QCursor::pos());
}

void FetMainForm::on_shortcutStudentsTimeConstraintsPushButton_clicked()
{
	menuStudents_time_constraints->popup(QCursor::pos());
}

void FetMainForm::on_shortcutActivitiesTimeConstraintsPushButton_clicked()
{
	menuActivities_time_constraints->popup(QCursor::pos());
}

void FetMainForm::on_shortcutAdvancedTimeConstraintsPushButton_clicked()
{
	shortcutAdvancedTimeMenu->popup(QCursor::pos());
}


//space constraints
void FetMainForm::on_shortcutAllSpaceConstraintsPushButton_clicked()
{
	on_dataAllSpaceConstraintsAction_triggered();
}

void FetMainForm::on_shortcutRoomsSpaceConstraintsPushButton_clicked()
{
	menuRooms_space_constraints->popup(QCursor::pos());
}

void FetMainForm::on_shortcutTeachersSpaceConstraintsPushButton_clicked()
{
	menuTeachers_space_constraints->popup(QCursor::pos());
}

void FetMainForm::on_shortcutStudentsSpaceConstraintsPushButton_clicked()
{
	menuStudents_space_constraints->popup(QCursor::pos());
}

void FetMainForm::on_shortcutSubjectsSpaceConstraintsPushButton_clicked()
{
	menuSubjects_space_constraints->popup(QCursor::pos());
}

void FetMainForm::on_shortcutActivityTagsSpaceConstraintsPushButton_clicked()
{
	menuActivity_tags_space_constraints->popup(QCursor::pos());
}

void FetMainForm::on_shortcutSubjectsAndActivityTagsSpaceConstraintsPushButton_clicked()
{
	menuSubjects_and_activity_tags_space_constraints->popup(QCursor::pos());
}

void FetMainForm::on_shortcutActivitiesSpaceConstraintsPushButton_clicked()
{
	menuActivities_space_constraints->popup(QCursor::pos());
}

//timetable
void FetMainForm::on_shortcutGeneratePushButton_clicked()
{
	on_timetableGenerateAction_triggered();
}

void FetMainForm::on_shortcutGenerateMultiplePushButton_clicked()
{
	on_timetableGenerateMultipleAction_triggered();
}

void FetMainForm::on_shortcutViewTeachersPushButton_clicked()
{
	menuView_teachers->popup(QCursor::pos());
	//old
	//on_timetableViewTeachersAction_triggered();
}

void FetMainForm::on_shortcutViewStudentsPushButton_clicked()
{
	menuView_students->popup(QCursor::pos());
	//old
	//on_timetableViewStudentsAction_triggered();
}

void FetMainForm::on_shortcutViewRoomsPushButton_clicked()
{
	menuView_rooms->popup(QCursor::pos());
	//old
	//on_timetableViewRoomsAction_triggered();
}

void FetMainForm::on_shortcutShowSoftConflictsPushButton_clicked()
{
	on_timetableShowConflictsAction_triggered();
}

//2014-07-01
void FetMainForm::on_shortcutsTimetableAdvancedPushButton_clicked()
{
	shortcutTimetableAdvancedMenu->popup(QCursor::pos());
}

void FetMainForm::on_shortcutsTimetablePrintPushButton_clicked()
{
	on_timetablePrintAction_triggered();
}

void FetMainForm::on_shortcutsTimetableLockingPushButton_clicked()
{
	shortcutTimetableLockingMenu->popup(QCursor::pos());
}

//data shortcut
void FetMainForm::on_shortcutBasicPushButton_clicked()
{
	shortcutBasicMenu->popup(QCursor::pos());
}

void FetMainForm::on_shortcutSubjectsPushButton_clicked()
{
	on_dataSubjectsAction_triggered();
}

void FetMainForm::on_shortcutActivityTagsPushButton_clicked()
{
	on_dataActivityTagsAction_triggered();
}

void FetMainForm::on_shortcutTeachersPushButton_clicked()
{
	on_dataTeachersAction_triggered();
}

void FetMainForm::on_shortcutStudentsPushButton_clicked()
{
	menuStudents->popup(QCursor::pos());
}

void FetMainForm::on_shortcutActivitiesPushButton_clicked()
{
	on_dataActivitiesAction_triggered();
}

void FetMainForm::on_shortcutSubactivitiesPushButton_clicked()
{
	on_dataSubactivitiesAction_triggered();
}

void FetMainForm::on_shortcutDataSpacePushButton_clicked()
{
	shortcutDataSpaceMenu->popup(QCursor::pos());
}

void FetMainForm::on_shortcutDataAdvancedPushButton_clicked()
{
	shortcutDataAdvancedMenu->popup(QCursor::pos());
}

//file shortcut
void FetMainForm::on_shortcutNewPushButton_clicked()
{
	on_fileNewAction_triggered();
}

void FetMainForm::on_shortcutOpenPushButton_clicked()
{
	on_fileOpenAction_triggered();
}

void FetMainForm::on_shortcutOpenRecentPushButton_clicked()
{
	fileOpenRecentMenu->popup(QCursor::pos());
}

void FetMainForm::on_shortcutSavePushButton_clicked()
{
	on_fileSaveAction_triggered();
}

void FetMainForm::on_shortcutSaveAsPushButton_clicked()
{
	on_fileSaveAsAction_triggered();
}

#else
bool SHOW_WARNING_FOR_SUBGROUPS_WITH_THE_SAME_ACTIVITIES=true;

bool SHOW_WARNING_FOR_ACTIVITIES_FIXED_SPACE_VIRTUAL_REAL_ROOMS_BUT_NOT_FIXED_TIME=true;

bool SHOW_WARNING_FOR_MAX_HOURS_DAILY_WITH_UNDER_100_WEIGHT=true;

bool SHOW_WARNING_FOR_NOT_PERFECT_CONSTRAINTS=true;
bool SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS=true;

bool SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_PER_MORNING_WITH_ALLOW_EMPTY_MORNINGS=true;

bool SHOW_WARNING_FOR_GROUP_ACTIVITIES_IN_INITIAL_ORDER=true;
#endif
