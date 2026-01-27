/*
File fet.cpp - this is where FET starts
*/

/***************************************************************************
                          fet.cpp  -  description
                             -------------------
    begin                : 2002
    copyright            : (C) 2002 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "fet.h"

#include "matrix.h"

#include "messageboxes.h"

#include "rules.h"

#include <QMessageBox>

#include <QWidget>

#include <QLocale>
#include <QTime>
#include <QDate>
#include <QDateTime>

#include <QSet>

static QSet<QString> languagesSet;

//#include <ctime>
//#include <cstdlib>

#include "timetableexport.h"
#include "generate.h"
#include "generate_pre.h"

#include "timetable_defs.h"
#include "timetable.h"

//extern MRG32k3a rng;

FetSettings fetSettings;

extern Generate gen;

#include "fetmainform.h"

#include "helpblockplanningform.h"
#include "helptermsform.h"
#include "helpmoroccoform.h"
#include "helpalgeriaform.h"

#include "helpaboutform.h"
#include "helpaboutlibrariesform.h"
#include "helpfaqform.h"
#include "helptipsform.h"
#include "helpinstructionsform.h"

#include "timetableshowconflictsform.h"
#include "timetableviewstudentsdayshorizontalform.h"
#include "timetableviewstudentsdaysverticalform.h"
#include "timetableviewstudentstimehorizontalform.h"
#include "timetableviewteachersdayshorizontalform.h"
#include "timetableviewteachersdaysverticalform.h"
#include "timetableviewteacherstimehorizontalform.h"
#include "timetableviewroomsdayshorizontalform.h"
#include "timetableviewroomsdaysverticalform.h"
#include "timetableviewroomstimehorizontalform.h"

#include <QCoreApplication>

#include <QApplication>
#include <QWidgetList>

#include <QSettings>
#include <QRect>

#include <QMutex>
#include <QString>
#include <QTranslator>

#include <QDir>
#include <QFileInfo>

#include <QTextStream>
#include <QFile>

#include <Qt>

#include <iostream>

extern QRect mainFormSettingsRect;
extern int MAIN_FORM_SHORTCUTS_TAB_POSITION;

//extern Solution highestStageSolution;

//extern int maxActivitiesPlaced;

extern bool students_schedule_ready;
extern bool teachers_schedule_ready;
extern bool rooms_buildings_schedule_ready;

QString tempOutputDirectory;

QString communicationFile;
QString logsDir;
QString csvOutputDirectory;

void exportExportCSV(Solution* bestOrHighest, Solution* current)
{
	Q_UNUSED(bestOrHighest);
	Q_UNUSED(current);
	//should be used only in the command line version
	assert(0);
}

//#ifndef FET_COM MAND_LINE
//extern QMutex myMutex;
//#else
//QMutex myMutex;
//#endif

void writeDefaultGenerationParameters();

bool generatePreMessage(QWidget* parent, const QString& s)
{
	QMessageBox::StandardButton b=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"), s, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
	if(b!=QMessageBox::Yes)
		return false;
	else
		return true;
}

EngineProgressDialog::EngineProgressDialog(QWidget* parent)
{
	qpd=new QProgressDialog(parent);
}

EngineProgressDialog::~EngineProgressDialog()
{
	assert(qpd!=nullptr);
	delete qpd; //Needed, even if qpd has a parent, because parent might be nullptr in the constructor of EngineProgressDialog.
}

void EngineProgressDialog::setWindowTitle(const QString& title)
{
	assert(qpd!=nullptr);
	qpd->setWindowTitle(title);
}

void EngineProgressDialog::setLabelText(const QString& title)
{
	assert(qpd!=nullptr);
	qpd->setLabelText(title);
}

void EngineProgressDialog::setRange(int a, int b)
{
	assert(qpd!=nullptr);
	qpd->setRange(a, b);
}

void EngineProgressDialog::setModal(bool m)
{
	assert(qpd!=nullptr);
	qpd->setModal(m);
}

void EngineProgressDialog::setValue(int v)
{
	assert(qpd!=nullptr);
	qpd->setValue(v);
}

bool EngineProgressDialog::wasCanceled()
{
	assert(qpd!=nullptr);
	return qpd->wasCanceled();
}

void EngineMessageBox::critical(QWidget* parent, const QString& title, const QString& message)
{
	QMessageBox::critical(parent, title, message);
}

QTranslator translator;

/**
The one and only instantiation of the main class.
*/
Timetable gt;

/**
The name of the file from where the rules are read.
*/
QString INPUT_FILENAME_XML;

/**
The working directory
*/
QString WORKING_DIRECTORY;

/**
The import directory
*/
QString IMPORT_DIRECTORY;

Matrix3D<int> teachers_timetable_weekly;
Matrix3D<int> students_timetable_weekly;
Matrix3D<int> rooms_timetable_weekly;
Matrix3D<QList<int>> virtual_rooms_timetable_weekly;
Matrix3D<QList<int>> buildings_timetable_weekly;
Matrix3D<QList<int>> teachers_free_periods_timetable_weekly;

QApplication* pqapplication=nullptr;

FetMainForm* pFetMainForm=nullptr;

//extern int XX;
//extern int YY;

Generate* terminateGeneratePointer;

void FetSettings::readGenerationParameters(QApplication& qapplication)
{
	const QString predefDir=QDir::homePath()+FILE_SEP+"fet-results";

	QSettings settings(COMPANY, PROGRAM);

#ifndef USE_SYSTEM_LOCALE
	FET_LANGUAGE=settings.value("language", "en_US").toString();
#else
	if(settings.contains("language")){
		FET_LANGUAGE=settings.value("language").toString();
	}
	else{
		FET_LANGUAGE=QLocale::system().name();

		bool ok=false;
		for(const QString& s : std::as_const(languagesSet)){
			if(FET_LANGUAGE.left(s.length())==s){
				FET_LANGUAGE=s;
				ok=true;
				break;
			}
		}
		if(!ok)
			FET_LANGUAGE="en_US";
	}
#endif
	
	if(FET_LANGUAGE=="ar")
		FET_LANGUAGE_WITH_LOCALE="ar_DZ";
	else
		FET_LANGUAGE_WITH_LOCALE=FET_LANGUAGE;
	
	setLanguage(qapplication, nullptr);
	
	QString s=settings.value("mode", "official").toString();
	if(s==QString("official"))
		gt.rules.mode=OFFICIAL;
	else if(s==QString("mornings-afternoons"))
		gt.rules.mode=MORNINGS_AFTERNOONS;
	else if(s==QString("block-planning"))
		gt.rules.mode=BLOCK_PLANNING;
	else if(s==QString("terms"))
		gt.rules.mode=TERMS;
	else{
		QMessageBox::warning(nullptr, FetTranslate::tr("FET warning"), FetTranslate::tr("Incorrect startup mode read from the settings - making"
		 " it %1.").arg(FetTranslate::tr("Official")));
		gt.rules.mode=OFFICIAL;
		//assert(0);
	}

	if(settings.contains("output-directory")){
		OUTPUT_DIR=settings.value("output-directory").toString();
		QDir dir;
		if(!dir.exists(OUTPUT_DIR)){
			bool t=dir.mkpath(OUTPUT_DIR);
			if(!t){
				QMessageBox::warning(nullptr, FetTranslate::tr("FET warning"), FetTranslate::tr("Output directory %1 does not exist and cannot be"
				 " created - output directory will be made the default value %2")
				 .arg(QDir::toNativeSeparators(OUTPUT_DIR)).arg(QDir::toNativeSeparators(predefDir)));
				OUTPUT_DIR=predefDir;
			}
		}
	}
	else{
		OUTPUT_DIR=predefDir;
	}
	
	WORKING_DIRECTORY=settings.value("working-directory", "examples").toString();
	IMPORT_DIRECTORY=settings.value("import-directory", OUTPUT_DIR).toString();
	
	QDir d(WORKING_DIRECTORY);
	if(!d.exists())
		WORKING_DIRECTORY="examples";
	QDir d2(WORKING_DIRECTORY);
	if(!d2.exists())
		WORKING_DIRECTORY=QDir::homePath();
	else
		WORKING_DIRECTORY=d2.absolutePath();

	QDir i(IMPORT_DIRECTORY);
	if(!i.exists())
		IMPORT_DIRECTORY=OUTPUT_DIR;
	
	checkForUpdates=settings.value("check-for-updates", "false").toBool();

	QString ver=settings.value("version", "-1").toString();
	
	OVERWRITE_SINGLE_GENERATION_FILES=settings.value("overwrite-single-generation-files", "false").toBool();

	TIMETABLE_HTML_LEVEL=settings.value("html-level", "2").toInt();
	if(TIMETABLE_HTML_LEVEL<0 || TIMETABLE_HTML_LEVEL>7){
		QMessageBox::warning(nullptr, FetTranslate::tr("FET warning"), FetTranslate::tr("Incorrect HTML level read from the settings - making it %1.").arg(2));
		TIMETABLE_HTML_LEVEL=2;
	}
	TIMETABLES_SUBGROUPS_SORTED=settings.value("timetables-subgroups-sorted", "false").toBool();
	TIMETABLE_HTML_PRINT_ACTIVITY_TAGS=settings.value("print-activity-tags", "true").toBool();
	
	TIMETABLE_HTML_PRINT_SUBJECTS=settings.value("print-subjects", "true").toBool();
	TIMETABLE_HTML_PRINT_TEACHERS=settings.value("print-teachers", "true").toBool();
	TIMETABLE_HTML_PRINT_STUDENTS=settings.value("print-students", "true").toBool();
	TIMETABLE_HTML_PRINT_ROOMS=settings.value("print-rooms", "true").toBool();
	
	PRINT_DETAILED_HTML_TIMETABLES=settings.value("print-detailed-timetables", "true").toBool();
	PRINT_DETAILED_HTML_TEACHERS_FREE_PERIODS=settings.value("print-detailed-teachers-free-periods-timetables", "true").toBool();
	PRINT_ACTIVITIES_WITH_SAME_STARTING_TIME=settings.value("print-activities-with-same-starting-time", "false").toBool();
	PRINT_NOT_AVAILABLE_TIME_SLOTS=settings.value("print-not-available", "true").toBool();
	PRINT_BREAK_TIME_SLOTS=settings.value("print-break", "true").toBool();
	DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS=settings.value("divide-html-timetables-with-time-axis-by-days", "false").toBool();
	TIMETABLE_HTML_REPEAT_NAMES=settings.value("timetables-repeat-vertical-names", "false").toBool();
	
	INTERFACE_STYLE=settings.value("interface-style", "").toString();
	INTERFACE_COLOR_SCHEME=settings.value("interface-color-scheme", "automatic").toString();
	if(INTERFACE_COLOR_SCHEME=="auto") //old name, in FET-6.27.0
		INTERFACE_COLOR_SCHEME="automatic";
	//To avoid assert(0) in fetmainform.cpp if the setting is corrupt (I am not sure I kept the assert, but does not hurt).
	if(INTERFACE_COLOR_SCHEME!="automatic" && INTERFACE_COLOR_SCHEME!="light" && INTERFACE_COLOR_SCHEME!="dark")
		INTERFACE_COLOR_SCHEME="automatic";

	SHOW_SUBGROUPS_IN_COMBO_BOXES=settings.value("show-subgroups-in-combo-boxes", "true").toBool();
	SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING=settings.value("show-subgroups-in-activity-planning", "true").toBool();
	
	WRITE_TIMETABLE_CONFLICTS=settings.value("write-timetable-conflicts", "true").toBool();

	WRITE_TIMETABLES_STATISTICS=settings.value("write-timetables-statistics", "true").toBool();
	WRITE_TIMETABLES_XML=settings.value("write-timetables-xml", "true").toBool();
	WRITE_TIMETABLES_DAYS_HORIZONTAL=settings.value("write-timetables-days-horizontal", "true").toBool();
	WRITE_TIMETABLES_DAYS_VERTICAL=settings.value("write-timetables-days-vertical", "true").toBool();
	WRITE_TIMETABLES_TIME_HORIZONTAL=settings.value("write-timetables-time-horizontal", "true").toBool();
	WRITE_TIMETABLES_TIME_VERTICAL=settings.value("write-timetables-time-vertical", "true").toBool();

	WRITE_TIMETABLES_SUBGROUPS=settings.value("write-timetables-subgroups", "true").toBool();
	WRITE_TIMETABLES_GROUPS=settings.value("write-timetables-groups", "true").toBool();
	WRITE_TIMETABLES_YEARS=settings.value("write-timetables-years", "true").toBool();
	WRITE_TIMETABLES_TEACHERS=settings.value("write-timetables-teachers", "true").toBool();
	WRITE_TIMETABLES_TEACHERS_FREE_PERIODS=settings.value("write-timetables-teachers-free-periods", "true").toBool();
	WRITE_TIMETABLES_BUILDINGS=settings.value("write-timetables-buildings", "true").toBool();
	WRITE_TIMETABLES_ROOMS=settings.value("write-timetables-rooms", "true").toBool();
	WRITE_TIMETABLES_SUBJECTS=settings.value("write-timetables-subjects", "true").toBool();
	WRITE_TIMETABLES_ACTIVITY_TAGS=settings.value("write-timetables-activity-tags", "true").toBool();
	WRITE_TIMETABLES_ACTIVITIES=settings.value("write-timetables-activities", "true").toBool();
	
	if(settings.contains("students-combo-boxes-style"))
		STUDENTS_COMBO_BOXES_STYLE=settings.value("students-combo-boxes-style").toInt();
	else
		STUDENTS_COMBO_BOXES_STYLE=STUDENTS_COMBO_BOXES_STYLE_SIMPLE;

/////////confirmations
	CONFIRM_ACTIVITY_PLANNING=settings.value("confirm-activity-planning", "true").toBool();
	CONFIRM_SPREAD_ACTIVITIES=settings.value("confirm-spread-activities", "true").toBool();
	CONFIRM_REMOVE_REDUNDANT=settings.value("confirm-remove-redundant", "true").toBool();
	CONFIRM_SAVE_TIMETABLE=settings.value("confirm-save-data-and-timetable", "true").toBool();
	CONFIRM_ACTIVATE_DEACTIVATE_ACTIVITIES_CONSTRAINTS=settings.value("confirm-activate-deactivate-activities-constraints", "true").toBool();
/////////

	SHORTCUT_PLUS=settings.value("shorcut-plus", "false").toBool();
	SHORTCUT_M=settings.value("shorcut-m", "false").toBool();
	SHORTCUT_DELETE=settings.value("shorcut-delete", "false").toBool();
	SHORTCUT_A=settings.value("shorcut-a", "false").toBool();
	SHORTCUT_D=settings.value("shorcut-d", "false").toBool();
	SHORTCUT_C=settings.value("shorcut-c", "false").toBool();
	SHORTCUT_U=settings.value("shorcut-u", "false").toBool();
	SHORTCUT_J=settings.value("shorcut-j", "false").toBool();
	SHORTCUT_W=settings.value("shorcut-w", "false").toBool();

	SHOW_WARNING_FOR_SUBGROUPS_WITH_THE_SAME_ACTIVITIES=settings.value("warn-subgroups-with-the-same-activities", "true").toBool();
	SHOW_WARNING_FOR_ACTIVITIES_FIXED_SPACE_VIRTUAL_REAL_ROOMS_BUT_NOT_FIXED_TIME=settings.value("warn-activities-not-fixed-time-fixed-space-virtual-real", "true").toBool();
	SHOW_WARNING_FOR_MAX_HOURS_DAILY_WITH_UNDER_100_WEIGHT=settings.value("warn-max-hours-daily-with-under-100-weight", "true").toBool();

	ENABLE_GROUP_ACTIVITIES_IN_INITIAL_ORDER=settings.value("enable-group-activities-in-initial-order", "false").toBool();
	SHOW_WARNING_FOR_GROUP_ACTIVITIES_IN_INITIAL_ORDER=settings.value("warn-if-using-group-activities-in-initial-order", "true").toBool();

	SHOW_VIRTUAL_ROOMS_IN_TIMETABLES=settings.value("show-virtual-rooms-in-timetables", "false").toBool();

	//2024-06-12 begin
	//
	SETTINGS_TIMETABLES_SEPARATE_DAYS_NAME_LONG_NAME_BY_BREAK=settings.value("settings-timetables-separate-days-name-long-name-by-break", "false").toBool();
	SETTINGS_TIMETABLES_SEPARATE_HOURS_NAME_LONG_NAME_BY_BREAK=settings.value("settings-timetables-separate-hours-name-long-name-by-break", "false").toBool();
	SETTINGS_TIMETABLES_SEPARATE_SUBJECTS_NAME_LONG_NAME_CODE_BY_BREAK=settings.value("settings-timetables-separate-subjects-name-long-name-code-by-break", "false").toBool();
	SETTINGS_TIMETABLES_SEPARATE_ACTIVITY_TAGS_NAME_LONG_NAME_CODE_BY_BREAK=settings.value("settings-timetables-separate-activity-tags-name-long-name-code-by-break", "false").toBool();
	SETTINGS_TIMETABLES_SEPARATE_TEACHERS_NAME_LONG_NAME_CODE_BY_BREAK=settings.value("settings-timetables-separate-teachers-name-long-name-code-by-break", "false").toBool();
	SETTINGS_TIMETABLES_SEPARATE_STUDENTS_NAME_LONG_NAME_CODE_BY_BREAK=settings.value("settings-timetables-separate-students-name-long-name-code-by-break", "false").toBool();
	SETTINGS_TIMETABLES_SEPARATE_BUILDINGS_NAME_LONG_NAME_CODE_BY_BREAK=settings.value("settings-timetables-separate-buildings-name-long-name-code-by-break", "false").toBool();
	SETTINGS_TIMETABLES_SEPARATE_ROOMS_NAME_LONG_NAME_CODE_BY_BREAK=settings.value("settings-timetables-separate-rooms-name-long-name-code-by-break", "false").toBool();

	//only in days horizontal and days vertical.
	SETTINGS_TIMETABLES_PRINT_SUBJECTS_COMMENTS=settings.value("settings-timetables-print-subjects-comments", "false").toBool();
	SETTINGS_TIMETABLES_PRINT_ACTIVITY_TAGS_COMMENTS=settings.value("settings-timetables-print-activity-tags-comments", "false").toBool();
	SETTINGS_TIMETABLES_PRINT_TEACHERS_COMMENTS=settings.value("settings-timetables-print-teachers-comments", "false").toBool();
	SETTINGS_TIMETABLES_PRINT_SUBGROUPS_COMMENTS=settings.value("settings-timetables-print-subgroups-comments", "false").toBool();
	SETTINGS_TIMETABLES_PRINT_GROUPS_COMMENTS=settings.value("settings-timetables-print-groups-comments", "false").toBool();
	SETTINGS_TIMETABLES_PRINT_YEARS_COMMENTS=settings.value("settings-timetables-print-years-comments", "false").toBool();
	SETTINGS_TIMETABLES_PRINT_BUILDINGS_COMMENTS=settings.value("settings-timetables-print-buildings-comments", "false").toBool();
	SETTINGS_TIMETABLES_PRINT_ROOMS_COMMENTS=settings.value("settings-timetables-print-rooms-comments", "false").toBool();

	/////subgroups days horizontal and days vertical.
	SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_DAYS_NAMES=settings.value("settings-timetables-subgroups-days-hv-print-days-names", "true").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_DAYS_LONG_NAMES=settings.value("settings-timetables-subgroups-days-hv-print-days-long-names", "false").toBool();

	SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_HOURS_NAMES=settings.value("settings-timetables-subgroups-days-hv-print-hours-names", "true").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_HOURS_LONG_NAMES=settings.value("settings-timetables-subgroups-days-hv-print-hours-long-names", "false").toBool();

	SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_NAMES=settings.value("settings-timetables-subgroups-days-hv-print-subjects-names", "true").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES=settings.value("settings-timetables-subgroups-days-hv-print-subjects-long-names", "false").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_CODES=settings.value("settings-timetables-subgroups-days-hv-print-subjects-codes", "false").toBool();

	SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES=settings.value("settings-timetables-subgroups-days-hv-print-activity-tags-names", "true").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=settings.value("settings-timetables-subgroups-days-hv-print-activity-tags-long-names", "false").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES=settings.value("settings-timetables-subgroups-days-hv-print-activity-tags-codes", "false").toBool();

	SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_NAMES=settings.value("settings-timetables-subgroups-days-hv-print-teachers-names", "true").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES=settings.value("settings-timetables-subgroups-days-hv-print-teachers-long-names", "false").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_CODES=settings.value("settings-timetables-subgroups-days-hv-print-teachers-codes", "false").toBool();

	SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_NAMES=settings.value("settings-timetables-subgroups-days-hv-print-students-names", "true").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES=settings.value("settings-timetables-subgroups-days-hv-print-students-long-names", "false").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_CODES=settings.value("settings-timetables-subgroups-days-hv-print-students-codes", "false").toBool();

	SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_NAMES=settings.value("settings-timetables-subgroups-days-hv-print-rooms-names", "true").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_LONG_NAMES=settings.value("settings-timetables-subgroups-days-hv-print-rooms-long-names", "false").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_CODES=settings.value("settings-timetables-subgroups-days-hv-print-rooms-codes", "false").toBool();

	SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_LEGEND=settings.value("settings-timetables-subgroups-days-hv-print-subjects-legend", "false").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND=settings.value("settings-timetables-subgroups-days-hv-print-activity-tags-legend", "false").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_LEGEND=settings.value("settings-timetables-subgroups-days-hv-print-teachers-legend", "false").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_LEGEND=settings.value("settings-timetables-subgroups-days-hv-print-students-legend", "false").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_LEGEND=settings.value("settings-timetables-subgroups-days-hv-print-rooms-legend", "false").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_LEGEND_CODES_FIRST=settings.value("settings-timetables-subgroups-days-hv-print-legend-codes-first", "false").toBool();
	/////

	/////subgroups time horizontal and time vertical.
	SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_NAMES=settings.value("settings-timetables-subgroups-time-hv-print-days-names", "true").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES=settings.value("settings-timetables-subgroups-time-hv-print-days-long-names", "false").toBool();

	SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_NAMES=settings.value("settings-timetables-subgroups-time-hv-print-hours-names", "true").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES=settings.value("settings-timetables-subgroups-time-hv-print-hours-long-names", "false").toBool();

	SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_NAMES=settings.value("settings-timetables-subgroups-time-hv-print-subjects-names", "true").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES=settings.value("settings-timetables-subgroups-time-hv-print-subjects-long-names", "false").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_CODES=settings.value("settings-timetables-subgroups-time-hv-print-subjects-codes", "false").toBool();

	SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES=settings.value("settings-timetables-subgroups-time-hv-print-activity-tags-names", "true").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=settings.value("settings-timetables-subgroups-time-hv-print-activity-tags-long-names", "false").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES=settings.value("settings-timetables-subgroups-time-hv-print-activity-tags-codes", "false").toBool();

	SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_NAMES=settings.value("settings-timetables-subgroups-time-hv-print-teachers-names", "true").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_LONG_NAMES=settings.value("settings-timetables-subgroups-time-hv-print-teachers-long-names", "false").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_CODES=settings.value("settings-timetables-subgroups-time-hv-print-teachers-codes", "false").toBool();

	SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_NAMES=settings.value("settings-timetables-subgroups-time-hv-print-students-names", "true").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES=settings.value("settings-timetables-subgroups-time-hv-print-students-long-names", "false").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_CODES=settings.value("settings-timetables-subgroups-time-hv-print-students-codes", "false").toBool();

	SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_NAMES=settings.value("settings-timetables-subgroups-time-hv-print-rooms-names", "true").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_LONG_NAMES=settings.value("settings-timetables-subgroups-time-hv-print-rooms-long-names", "false").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_CODES=settings.value("settings-timetables-subgroups-time-hv-print-rooms-codes", "false").toBool();

	SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_LEGEND=settings.value("settings-timetables-subgroups-time-hv-print-subjects-legend", "false").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND=settings.value("settings-timetables-subgroups-time-hv-print-activity-tags-legend", "false").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_LEGEND=settings.value("settings-timetables-subgroups-time-hv-print-teachers-legend", "false").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_LEGEND=settings.value("settings-timetables-subgroups-time-hv-print-students-legend", "false").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_LEGEND=settings.value("settings-timetables-subgroups-time-hv-print-rooms-legend", "false").toBool();
	SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_LEGEND_CODES_FIRST=settings.value("settings-timetables-subgroups-time-hv-print-legend-codes-first", "false").toBool();
	/////

	/////groups days horizontal and days vertical.
	SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_DAYS_NAMES=settings.value("settings-timetables-groups-days-hv-print-days-names", "true").toBool();
	SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_DAYS_LONG_NAMES=settings.value("settings-timetables-groups-days-hv-print-days-long-names", "false").toBool();

	SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_HOURS_NAMES=settings.value("settings-timetables-groups-days-hv-print-hours-names", "true").toBool();
	SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_HOURS_LONG_NAMES=settings.value("settings-timetables-groups-days-hv-print-hours-long-names", "false").toBool();

	SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_NAMES=settings.value("settings-timetables-groups-days-hv-print-subjects-names", "true").toBool();
	SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES=settings.value("settings-timetables-groups-days-hv-print-subjects-long-names", "false").toBool();
	SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_CODES=settings.value("settings-timetables-groups-days-hv-print-subjects-codes", "false").toBool();

	SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES=settings.value("settings-timetables-groups-days-hv-print-activity-tags-names", "true").toBool();
	SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=settings.value("settings-timetables-groups-days-hv-print-activity-tags-long-names", "false").toBool();
	SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES=settings.value("settings-timetables-groups-days-hv-print-activity-tags-codes", "false").toBool();

	SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_NAMES=settings.value("settings-timetables-groups-days-hv-print-teachers-names", "true").toBool();
	SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES=settings.value("settings-timetables-groups-days-hv-print-teachers-long-names", "false").toBool();
	SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_CODES=settings.value("settings-timetables-groups-days-hv-print-teachers-codes", "false").toBool();

	SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_NAMES=settings.value("settings-timetables-groups-days-hv-print-students-names", "true").toBool();
	SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES=settings.value("settings-timetables-groups-days-hv-print-students-long-names", "false").toBool();
	SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_CODES=settings.value("settings-timetables-groups-days-hv-print-students-codes", "false").toBool();

	SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_NAMES=settings.value("settings-timetables-groups-days-hv-print-rooms-names", "true").toBool();
	SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_LONG_NAMES=settings.value("settings-timetables-groups-days-hv-print-rooms-long-names", "false").toBool();
	SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_CODES=settings.value("settings-timetables-groups-days-hv-print-rooms-codes", "false").toBool();

	SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_LEGEND=settings.value("settings-timetables-groups-days-hv-print-subjects-legend", "false").toBool();
	SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND=settings.value("settings-timetables-groups-days-hv-print-activity-tags-legend", "false").toBool();
	SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_LEGEND=settings.value("settings-timetables-groups-days-hv-print-teachers-legend", "false").toBool();
	SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_LEGEND=settings.value("settings-timetables-groups-days-hv-print-students-legend", "false").toBool();
	SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_LEGEND=settings.value("settings-timetables-groups-days-hv-print-rooms-legend", "false").toBool();
	SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_LEGEND_CODES_FIRST=settings.value("settings-timetables-groups-days-hv-print-legend-codes-first", "false").toBool();
	/////

	/////groups time horizontal and time vertical.
	SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_NAMES=settings.value("settings-timetables-groups-time-hv-print-days-names", "true").toBool();
	SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES=settings.value("settings-timetables-groups-time-hv-print-days-long-names", "false").toBool();

	SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_NAMES=settings.value("settings-timetables-groups-time-hv-print-hours-names", "true").toBool();
	SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES=settings.value("settings-timetables-groups-time-hv-print-hours-long-names", "false").toBool();

	SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_NAMES=settings.value("settings-timetables-groups-time-hv-print-subjects-names", "true").toBool();
	SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES=settings.value("settings-timetables-groups-time-hv-print-subjects-long-names", "false").toBool();
	SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_CODES=settings.value("settings-timetables-groups-time-hv-print-subjects-codes", "false").toBool();

	SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES=settings.value("settings-timetables-groups-time-hv-print-activity-tags-names", "true").toBool();
	SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=settings.value("settings-timetables-groups-time-hv-print-activity-tags-long-names", "false").toBool();
	SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES=settings.value("settings-timetables-groups-time-hv-print-activity-tags-codes", "false").toBool();

	SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_NAMES=settings.value("settings-timetables-groups-time-hv-print-teachers-names", "true").toBool();
	SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_LONG_NAMES=settings.value("settings-timetables-groups-time-hv-print-teachers-long-names", "false").toBool();
	SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_CODES=settings.value("settings-timetables-groups-time-hv-print-teachers-codes", "false").toBool();

	SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_NAMES=settings.value("settings-timetables-groups-time-hv-print-students-names", "true").toBool();
	SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES=settings.value("settings-timetables-groups-time-hv-print-students-long-names", "false").toBool();
	SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_CODES=settings.value("settings-timetables-groups-time-hv-print-students-codes", "false").toBool();

	SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_NAMES=settings.value("settings-timetables-groups-time-hv-print-rooms-names", "true").toBool();
	SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_LONG_NAMES=settings.value("settings-timetables-groups-time-hv-print-rooms-long-names", "false").toBool();
	SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_CODES=settings.value("settings-timetables-groups-time-hv-print-rooms-codes", "false").toBool();

	SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_LEGEND=settings.value("settings-timetables-groups-time-hv-print-subjects-legend", "false").toBool();
	SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND=settings.value("settings-timetables-groups-time-hv-print-activity-tags-legend", "false").toBool();
	SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_LEGEND=settings.value("settings-timetables-groups-time-hv-print-teachers-legend", "false").toBool();
	SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_LEGEND=settings.value("settings-timetables-groups-time-hv-print-students-legend", "false").toBool();
	SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_LEGEND=settings.value("settings-timetables-groups-time-hv-print-rooms-legend", "false").toBool();
	SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_LEGEND_CODES_FIRST=settings.value("settings-timetables-groups-time-hv-print-legend-codes-first", "false").toBool();
	/////

	/////years days horizontal and days vertical.
	SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_DAYS_NAMES=settings.value("settings-timetables-years-days-hv-print-days-names", "true").toBool();
	SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_DAYS_LONG_NAMES=settings.value("settings-timetables-years-days-hv-print-days-long-names", "false").toBool();

	SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_HOURS_NAMES=settings.value("settings-timetables-years-days-hv-print-hours-names", "true").toBool();
	SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_HOURS_LONG_NAMES=settings.value("settings-timetables-years-days-hv-print-hours-long-names", "false").toBool();

	SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_NAMES=settings.value("settings-timetables-years-days-hv-print-subjects-names", "true").toBool();
	SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES=settings.value("settings-timetables-years-days-hv-print-subjects-long-names", "false").toBool();
	SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_CODES=settings.value("settings-timetables-years-days-hv-print-subjects-codes", "false").toBool();

	SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES=settings.value("settings-timetables-years-days-hv-print-activity-tags-names", "true").toBool();
	SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=settings.value("settings-timetables-years-days-hv-print-activity-tags-long-names", "false").toBool();
	SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES=settings.value("settings-timetables-years-days-hv-print-activity-tags-codes", "false").toBool();

	SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_NAMES=settings.value("settings-timetables-years-days-hv-print-teachers-names", "true").toBool();
	SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES=settings.value("settings-timetables-years-days-hv-print-teachers-long-names", "false").toBool();
	SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_CODES=settings.value("settings-timetables-years-days-hv-print-teachers-codes", "false").toBool();

	SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_NAMES=settings.value("settings-timetables-years-days-hv-print-students-names", "true").toBool();
	SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES=settings.value("settings-timetables-years-days-hv-print-students-long-names", "false").toBool();
	SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_CODES=settings.value("settings-timetables-years-days-hv-print-students-codes", "false").toBool();

	SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_NAMES=settings.value("settings-timetables-years-days-hv-print-rooms-names", "true").toBool();
	SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_LONG_NAMES=settings.value("settings-timetables-years-days-hv-print-rooms-long-names", "false").toBool();
	SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_CODES=settings.value("settings-timetables-years-days-hv-print-rooms-codes", "false").toBool();

	SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_LEGEND=settings.value("settings-timetables-years-days-hv-print-subjects-legend", "false").toBool();
	SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND=settings.value("settings-timetables-years-days-hv-print-activity-tags-legend", "false").toBool();
	SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_LEGEND=settings.value("settings-timetables-years-days-hv-print-teachers-legend", "false").toBool();
	SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_LEGEND=settings.value("settings-timetables-years-days-hv-print-students-legend", "false").toBool();
	SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_LEGEND=settings.value("settings-timetables-years-days-hv-print-rooms-legend", "false").toBool();
	SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_LEGEND_CODES_FIRST=settings.value("settings-timetables-years-days-hv-print-legend-codes-first", "false").toBool();
	/////

	/////years time horizontal and time vertical.
	SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_NAMES=settings.value("settings-timetables-years-time-hv-print-days-names", "true").toBool();
	SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_LONG_NAMES=settings.value("settings-timetables-years-time-hv-print-days-long-names", "false").toBool();

	SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_NAMES=settings.value("settings-timetables-years-time-hv-print-hours-names", "true").toBool();
	SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_LONG_NAMES=settings.value("settings-timetables-years-time-hv-print-hours-long-names", "false").toBool();

	SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_NAMES=settings.value("settings-timetables-years-time-hv-print-subjects-names", "true").toBool();
	SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES=settings.value("settings-timetables-years-time-hv-print-subjects-long-names", "false").toBool();
	SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_CODES=settings.value("settings-timetables-years-time-hv-print-subjects-codes", "false").toBool();

	SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES=settings.value("settings-timetables-years-time-hv-print-activity-tags-names", "true").toBool();
	SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=settings.value("settings-timetables-years-time-hv-print-activity-tags-long-names", "false").toBool();
	SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES=settings.value("settings-timetables-years-time-hv-print-activity-tags-codes", "false").toBool();

	SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_NAMES=settings.value("settings-timetables-years-time-hv-print-teachers-names", "true").toBool();
	SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_LONG_NAMES=settings.value("settings-timetables-years-time-hv-print-teachers-long-names", "false").toBool();
	SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_CODES=settings.value("settings-timetables-years-time-hv-print-teachers-codes", "false").toBool();

	SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_NAMES=settings.value("settings-timetables-years-time-hv-print-students-names", "true").toBool();
	SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_LONG_NAMES=settings.value("settings-timetables-years-time-hv-print-students-long-names", "false").toBool();
	SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_CODES=settings.value("settings-timetables-years-time-hv-print-students-codes", "false").toBool();

	SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_NAMES=settings.value("settings-timetables-years-time-hv-print-rooms-names", "true").toBool();
	SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_LONG_NAMES=settings.value("settings-timetables-years-time-hv-print-rooms-long-names", "false").toBool();
	SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_CODES=settings.value("settings-timetables-years-time-hv-print-rooms-codes", "false").toBool();

	SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_LEGEND=settings.value("settings-timetables-years-time-hv-print-subjects-legend", "false").toBool();
	SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND=settings.value("settings-timetables-years-time-hv-print-activity-tags-legend", "false").toBool();
	SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_LEGEND=settings.value("settings-timetables-years-time-hv-print-teachers-legend", "false").toBool();
	SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_LEGEND=settings.value("settings-timetables-years-time-hv-print-students-legend", "false").toBool();
	SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_LEGEND=settings.value("settings-timetables-years-time-hv-print-rooms-legend", "false").toBool();
	SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_LEGEND_CODES_FIRST=settings.value("settings-timetables-years-time-hv-print-legend-codes-first", "false").toBool();
	/////

	/////teachers days horizontal and days vertical.
	SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_DAYS_NAMES=settings.value("settings-timetables-teachers-days-hv-print-days-names", "true").toBool();
	SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_DAYS_LONG_NAMES=settings.value("settings-timetables-teachers-days-hv-print-days-long-names", "false").toBool();

	SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_HOURS_NAMES=settings.value("settings-timetables-teachers-days-hv-print-hours-names", "true").toBool();
	SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_HOURS_LONG_NAMES=settings.value("settings-timetables-teachers-days-hv-print-hours-long-names", "false").toBool();

	SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_NAMES=settings.value("settings-timetables-teachers-days-hv-print-subjects-names", "true").toBool();
	SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES=settings.value("settings-timetables-teachers-days-hv-print-subjects-long-names", "false").toBool();
	SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_CODES=settings.value("settings-timetables-teachers-days-hv-print-subjects-codes", "false").toBool();

	SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES=settings.value("settings-timetables-teachers-days-hv-print-activity-tags-names", "true").toBool();
	SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=settings.value("settings-timetables-teachers-days-hv-print-activity-tags-long-names", "false").toBool();
	SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES=settings.value("settings-timetables-teachers-days-hv-print-activity-tags-codes", "false").toBool();

	SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_NAMES=settings.value("settings-timetables-teachers-days-hv-print-teachers-names", "true").toBool();
	SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES=settings.value("settings-timetables-teachers-days-hv-print-teachers-long-names", "false").toBool();
	SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_CODES=settings.value("settings-timetables-teachers-days-hv-print-teachers-codes", "false").toBool();

	SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_NAMES=settings.value("settings-timetables-teachers-days-hv-print-students-names", "true").toBool();
	SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES=settings.value("settings-timetables-teachers-days-hv-print-students-long-names", "false").toBool();
	SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_CODES=settings.value("settings-timetables-teachers-days-hv-print-students-codes", "false").toBool();

	SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_NAMES=settings.value("settings-timetables-teachers-days-hv-print-rooms-names", "true").toBool();
	SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_LONG_NAMES=settings.value("settings-timetables-teachers-days-hv-print-rooms-long-names", "false").toBool();
	SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_CODES=settings.value("settings-timetables-teachers-days-hv-print-rooms-codes", "false").toBool();

	SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_LEGEND=settings.value("settings-timetables-teachers-days-hv-print-subjects-legend", "false").toBool();
	SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND=settings.value("settings-timetables-teachers-days-hv-print-activity-tags-legend", "false").toBool();
	SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_LEGEND=settings.value("settings-timetables-teachers-days-hv-print-teachers-legend", "false").toBool();
	SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_LEGEND=settings.value("settings-timetables-teachers-days-hv-print-students-legend", "false").toBool();
	SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_LEGEND=settings.value("settings-timetables-teachers-days-hv-print-rooms-legend", "false").toBool();
	SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_LEGEND_CODES_FIRST=settings.value("settings-timetables-teachers-days-hv-print-legend-codes-first", "false").toBool();
	/////

	/////teachers time horizontal and time vertical.
	SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_NAMES=settings.value("settings-timetables-teachers-time-hv-print-days-names", "true").toBool();
	SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_LONG_NAMES=settings.value("settings-timetables-teachers-time-hv-print-days-long-names", "false").toBool();

	SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_NAMES=settings.value("settings-timetables-teachers-time-hv-print-hours-names", "true").toBool();
	SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_LONG_NAMES=settings.value("settings-timetables-teachers-time-hv-print-hours-long-names", "false").toBool();

	SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_NAMES=settings.value("settings-timetables-teachers-time-hv-print-subjects-names", "true").toBool();
	SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES=settings.value("settings-timetables-teachers-time-hv-print-subjects-long-names", "false").toBool();
	SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_CODES=settings.value("settings-timetables-teachers-time-hv-print-subjects-codes", "false").toBool();

	SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES=settings.value("settings-timetables-teachers-time-hv-print-activity-tags-names", "true").toBool();
	SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=settings.value("settings-timetables-teachers-time-hv-print-activity-tags-long-names", "false").toBool();
	SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES=settings.value("settings-timetables-teachers-time-hv-print-activity-tags-codes", "false").toBool();

	SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_NAMES=settings.value("settings-timetables-teachers-time-hv-print-teachers-names", "true").toBool();
	SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_LONG_NAMES=settings.value("settings-timetables-teachers-time-hv-print-teachers-long-names", "false").toBool();
	SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_CODES=settings.value("settings-timetables-teachers-time-hv-print-teachers-codes", "false").toBool();

	SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_NAMES=settings.value("settings-timetables-teachers-time-hv-print-students-names", "true").toBool();
	SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_LONG_NAMES=settings.value("settings-timetables-teachers-time-hv-print-students-long-names", "false").toBool();
	SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_CODES=settings.value("settings-timetables-teachers-time-hv-print-students-codes", "false").toBool();

	SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_NAMES=settings.value("settings-timetables-teachers-time-hv-print-rooms-names", "true").toBool();
	SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_LONG_NAMES=settings.value("settings-timetables-teachers-time-hv-print-rooms-long-names", "false").toBool();
	SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_CODES=settings.value("settings-timetables-teachers-time-hv-print-rooms-codes", "false").toBool();

	SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_LEGEND=settings.value("settings-timetables-teachers-time-hv-print-subjects-legend", "false").toBool();
	SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND=settings.value("settings-timetables-teachers-time-hv-print-activity-tags-legend", "false").toBool();
	SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_LEGEND=settings.value("settings-timetables-teachers-time-hv-print-teachers-legend", "false").toBool();
	SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_LEGEND=settings.value("settings-timetables-teachers-time-hv-print-students-legend", "false").toBool();
	SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_LEGEND=settings.value("settings-timetables-teachers-time-hv-print-rooms-legend", "false").toBool();
	SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_LEGEND_CODES_FIRST=settings.value("settings-timetables-teachers-time-hv-print-legend-codes-first", "false").toBool();
	/////

	/////rooms days horizontal and days vertical.
	SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_DAYS_NAMES=settings.value("settings-timetables-rooms-days-hv-print-days-names", "true").toBool();
	SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_DAYS_LONG_NAMES=settings.value("settings-timetables-rooms-days-hv-print-days-long-names", "false").toBool();

	SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_HOURS_NAMES=settings.value("settings-timetables-rooms-days-hv-print-hours-names", "true").toBool();
	SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_HOURS_LONG_NAMES=settings.value("settings-timetables-rooms-days-hv-print-hours-long-names", "false").toBool();

	SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_NAMES=settings.value("settings-timetables-rooms-days-hv-print-subjects-names", "true").toBool();
	SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES=settings.value("settings-timetables-rooms-days-hv-print-subjects-long-names", "false").toBool();
	SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_CODES=settings.value("settings-timetables-rooms-days-hv-print-subjects-codes", "false").toBool();

	SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES=settings.value("settings-timetables-rooms-days-hv-print-activity-tags-names", "true").toBool();
	SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=settings.value("settings-timetables-rooms-days-hv-print-activity-tags-long-names", "false").toBool();
	SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES=settings.value("settings-timetables-rooms-days-hv-print-activity-tags-codes", "false").toBool();

	SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_NAMES=settings.value("settings-timetables-rooms-days-hv-print-teachers-names", "true").toBool();
	SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES=settings.value("settings-timetables-rooms-days-hv-print-teachers-long-names", "false").toBool();
	SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_CODES=settings.value("settings-timetables-rooms-days-hv-print-teachers-codes", "false").toBool();

	SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_NAMES=settings.value("settings-timetables-rooms-days-hv-print-students-names", "true").toBool();
	SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES=settings.value("settings-timetables-rooms-days-hv-print-students-long-names", "false").toBool();
	SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_CODES=settings.value("settings-timetables-rooms-days-hv-print-students-codes", "false").toBool();

	SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_NAMES=settings.value("settings-timetables-rooms-days-hv-print-rooms-names", "true").toBool();
	SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_LONG_NAMES=settings.value("settings-timetables-rooms-days-hv-print-rooms-long-names", "false").toBool();
	SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_CODES=settings.value("settings-timetables-rooms-days-hv-print-rooms-codes", "false").toBool();

	SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_LEGEND=settings.value("settings-timetables-rooms-days-hv-print-subjects-legend", "false").toBool();
	SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND=settings.value("settings-timetables-rooms-days-hv-print-activity-tags-legend", "false").toBool();
	SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_LEGEND=settings.value("settings-timetables-rooms-days-hv-print-teachers-legend", "false").toBool();
	SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_LEGEND=settings.value("settings-timetables-rooms-days-hv-print-students-legend", "false").toBool();
	SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_LEGEND=settings.value("settings-timetables-rooms-days-hv-print-rooms-legend", "false").toBool();
	SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_LEGEND_CODES_FIRST=settings.value("settings-timetables-rooms-days-hv-print-legend-codes-first", "false").toBool();
	/////

	/////rooms time horizontal and time vertical.
	SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_NAMES=settings.value("settings-timetables-rooms-time-hv-print-days-names", "true").toBool();
	SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_LONG_NAMES=settings.value("settings-timetables-rooms-time-hv-print-days-long-names", "false").toBool();

	SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_NAMES=settings.value("settings-timetables-rooms-time-hv-print-hours-names", "true").toBool();
	SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_LONG_NAMES=settings.value("settings-timetables-rooms-time-hv-print-hours-long-names", "false").toBool();

	SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_NAMES=settings.value("settings-timetables-rooms-time-hv-print-subjects-names", "true").toBool();
	SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES=settings.value("settings-timetables-rooms-time-hv-print-subjects-long-names", "false").toBool();
	SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_CODES=settings.value("settings-timetables-rooms-time-hv-print-subjects-codes", "false").toBool();

	SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES=settings.value("settings-timetables-rooms-time-hv-print-activity-tags-names", "true").toBool();
	SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=settings.value("settings-timetables-rooms-time-hv-print-activity-tags-long-names", "false").toBool();
	SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES=settings.value("settings-timetables-rooms-time-hv-print-activity-tags-codes", "false").toBool();

	SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_NAMES=settings.value("settings-timetables-rooms-time-hv-print-teachers-names", "true").toBool();
	SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_LONG_NAMES=settings.value("settings-timetables-rooms-time-hv-print-teachers-long-names", "false").toBool();
	SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_CODES=settings.value("settings-timetables-rooms-time-hv-print-teachers-codes", "false").toBool();

	SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_NAMES=settings.value("settings-timetables-rooms-time-hv-print-students-names", "true").toBool();
	SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_LONG_NAMES=settings.value("settings-timetables-rooms-time-hv-print-students-long-names", "false").toBool();
	SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_CODES=settings.value("settings-timetables-rooms-time-hv-print-students-codes", "false").toBool();

	SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_NAMES=settings.value("settings-timetables-rooms-time-hv-print-rooms-names", "true").toBool();
	SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_LONG_NAMES=settings.value("settings-timetables-rooms-time-hv-print-rooms-long-names", "false").toBool();
	SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_CODES=settings.value("settings-timetables-rooms-time-hv-print-rooms-codes", "false").toBool();

	SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_LEGEND=settings.value("settings-timetables-rooms-time-hv-print-subjects-legend", "false").toBool();
	SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND=settings.value("settings-timetables-rooms-time-hv-print-activity-tags-legend", "false").toBool();
	SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_LEGEND=settings.value("settings-timetables-rooms-time-hv-print-teachers-legend", "false").toBool();
	SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_LEGEND=settings.value("settings-timetables-rooms-time-hv-print-students-legend", "false").toBool();
	SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_LEGEND=settings.value("settings-timetables-rooms-time-hv-print-rooms-legend", "false").toBool();
	SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_LEGEND_CODES_FIRST=settings.value("settings-timetables-rooms-time-hv-print-legend-codes-first", "false").toBool();
	/////

	/////buildings days horizontal and days vertical.
	SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_DAYS_NAMES=settings.value("settings-timetables-buildings-days-hv-print-days-names", "true").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_DAYS_LONG_NAMES=settings.value("settings-timetables-buildings-days-hv-print-days-long-names", "false").toBool();

	SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_HOURS_NAMES=settings.value("settings-timetables-buildings-days-hv-print-hours-names", "true").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_HOURS_LONG_NAMES=settings.value("settings-timetables-buildings-days-hv-print-hours-long-names", "false").toBool();

	SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_NAMES=settings.value("settings-timetables-buildings-days-hv-print-subjects-names", "true").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES=settings.value("settings-timetables-buildings-days-hv-print-subjects-long-names", "false").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_CODES=settings.value("settings-timetables-buildings-days-hv-print-subjects-codes", "false").toBool();

	SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES=settings.value("settings-timetables-buildings-days-hv-print-activity-tags-names", "true").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=settings.value("settings-timetables-buildings-days-hv-print-activity-tags-long-names", "false").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES=settings.value("settings-timetables-buildings-days-hv-print-activity-tags-codes", "false").toBool();

	SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_NAMES=settings.value("settings-timetables-buildings-days-hv-print-teachers-names", "true").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES=settings.value("settings-timetables-buildings-days-hv-print-teachers-long-names", "false").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_CODES=settings.value("settings-timetables-buildings-days-hv-print-teachers-codes", "false").toBool();

	SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_NAMES=settings.value("settings-timetables-buildings-days-hv-print-students-names", "true").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES=settings.value("settings-timetables-buildings-days-hv-print-students-long-names", "false").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_CODES=settings.value("settings-timetables-buildings-days-hv-print-students-codes", "false").toBool();

	SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_NAMES=settings.value("settings-timetables-buildings-days-hv-print-rooms-names", "true").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_LONG_NAMES=settings.value("settings-timetables-buildings-days-hv-print-rooms-long-names", "false").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_CODES=settings.value("settings-timetables-buildings-days-hv-print-rooms-codes", "false").toBool();

	SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_LEGEND=settings.value("settings-timetables-buildings-days-hv-print-subjects-legend", "false").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND=settings.value("settings-timetables-buildings-days-hv-print-activity-tags-legend", "false").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_LEGEND=settings.value("settings-timetables-buildings-days-hv-print-teachers-legend", "false").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_LEGEND=settings.value("settings-timetables-buildings-days-hv-print-students-legend", "false").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_LEGEND=settings.value("settings-timetables-buildings-days-hv-print-rooms-legend", "false").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_LEGEND_CODES_FIRST=settings.value("settings-timetables-buildings-days-hv-print-legend-codes-first", "false").toBool();
	/////

	/////buildings time horizontal and time vertical.
	SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_NAMES=settings.value("settings-timetables-buildings-time-hv-print-days-names", "true").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_LONG_NAMES=settings.value("settings-timetables-buildings-time-hv-print-days-long-names", "false").toBool();

	SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_NAMES=settings.value("settings-timetables-buildings-time-hv-print-hours-names", "true").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_LONG_NAMES=settings.value("settings-timetables-buildings-time-hv-print-hours-long-names", "false").toBool();

	SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_NAMES=settings.value("settings-timetables-buildings-time-hv-print-subjects-names", "true").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES=settings.value("settings-timetables-buildings-time-hv-print-subjects-long-names", "false").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_CODES=settings.value("settings-timetables-buildings-time-hv-print-subjects-codes", "false").toBool();

	SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES=settings.value("settings-timetables-buildings-time-hv-print-activity-tags-names", "true").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=settings.value("settings-timetables-buildings-time-hv-print-activity-tags-long-names", "false").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES=settings.value("settings-timetables-buildings-time-hv-print-activity-tags-codes", "false").toBool();

	SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_NAMES=settings.value("settings-timetables-buildings-time-hv-print-teachers-names", "true").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_LONG_NAMES=settings.value("settings-timetables-buildings-time-hv-print-teachers-long-names", "false").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_CODES=settings.value("settings-timetables-buildings-time-hv-print-teachers-codes", "false").toBool();

	SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_NAMES=settings.value("settings-timetables-buildings-time-hv-print-students-names", "true").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_LONG_NAMES=settings.value("settings-timetables-buildings-time-hv-print-students-long-names", "false").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_CODES=settings.value("settings-timetables-buildings-time-hv-print-students-codes", "false").toBool();

	SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_NAMES=settings.value("settings-timetables-buildings-time-hv-print-rooms-names", "true").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_LONG_NAMES=settings.value("settings-timetables-buildings-time-hv-print-rooms-long-names", "false").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_CODES=settings.value("settings-timetables-buildings-time-hv-print-rooms-codes", "false").toBool();

	SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_LEGEND=settings.value("settings-timetables-buildings-time-hv-print-subjects-legend", "false").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND=settings.value("settings-timetables-buildings-time-hv-print-activity-tags-legend", "false").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_LEGEND=settings.value("settings-timetables-buildings-time-hv-print-teachers-legend", "false").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_LEGEND=settings.value("settings-timetables-buildings-time-hv-print-students-legend", "false").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_LEGEND=settings.value("settings-timetables-buildings-time-hv-print-rooms-legend", "false").toBool();
	SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_LEGEND_CODES_FIRST=settings.value("settings-timetables-buildings-time-hv-print-legend-codes-first", "false").toBool();
	/////

	/////subjects days horizontal and days vertical.
	SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_DAYS_NAMES=settings.value("settings-timetables-subjects-days-hv-print-days-names", "true").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_DAYS_LONG_NAMES=settings.value("settings-timetables-subjects-days-hv-print-days-long-names", "false").toBool();

	SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_HOURS_NAMES=settings.value("settings-timetables-subjects-days-hv-print-hours-names", "true").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_HOURS_LONG_NAMES=settings.value("settings-timetables-subjects-days-hv-print-hours-long-names", "false").toBool();

	SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_NAMES=settings.value("settings-timetables-subjects-days-hv-print-subjects-names", "true").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES=settings.value("settings-timetables-subjects-days-hv-print-subjects-long-names", "false").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_CODES=settings.value("settings-timetables-subjects-days-hv-print-subjects-codes", "false").toBool();

	SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES=settings.value("settings-timetables-subjects-days-hv-print-activity-tags-names", "true").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=settings.value("settings-timetables-subjects-days-hv-print-activity-tags-long-names", "false").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES=settings.value("settings-timetables-subjects-days-hv-print-activity-tags-codes", "false").toBool();

	SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_TEACHERS_NAMES=settings.value("settings-timetables-subjects-days-hv-print-teachers-names", "true").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES=settings.value("settings-timetables-subjects-days-hv-print-teachers-long-names", "false").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_TEACHERS_CODES=settings.value("settings-timetables-subjects-days-hv-print-teachers-codes", "false").toBool();

	SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_STUDENTS_NAMES=settings.value("settings-timetables-subjects-days-hv-print-students-names", "true").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES=settings.value("settings-timetables-subjects-days-hv-print-students-long-names", "false").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_STUDENTS_CODES=settings.value("settings-timetables-subjects-days-hv-print-students-codes", "false").toBool();

	SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ROOMS_NAMES=settings.value("settings-timetables-subjects-days-hv-print-rooms-names", "true").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ROOMS_LONG_NAMES=settings.value("settings-timetables-subjects-days-hv-print-rooms-long-names", "false").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ROOMS_CODES=settings.value("settings-timetables-subjects-days-hv-print-rooms-codes", "false").toBool();

	SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_LEGEND=settings.value("settings-timetables-subjects-days-hv-print-subjects-legend", "false").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND=settings.value("settings-timetables-subjects-days-hv-print-activity-tags-legend", "false").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_TEACHERS_LEGEND=settings.value("settings-timetables-subjects-days-hv-print-teachers-legend", "false").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_STUDENTS_LEGEND=settings.value("settings-timetables-subjects-days-hv-print-students-legend", "false").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ROOMS_LEGEND=settings.value("settings-timetables-subjects-days-hv-print-rooms-legend", "false").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_LEGEND_CODES_FIRST=settings.value("settings-timetables-subjects-days-hv-print-legend-codes-first", "false").toBool();
	/////

	/////subjects time horizontal and time vertical.
	SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_NAMES=settings.value("settings-timetables-subjects-time-hv-print-days-names", "true").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_LONG_NAMES=settings.value("settings-timetables-subjects-time-hv-print-days-long-names", "false").toBool();

	SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_NAMES=settings.value("settings-timetables-subjects-time-hv-print-hours-names", "true").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_LONG_NAMES=settings.value("settings-timetables-subjects-time-hv-print-hours-long-names", "false").toBool();

	SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_NAMES=settings.value("settings-timetables-subjects-time-hv-print-subjects-names", "true").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES=settings.value("settings-timetables-subjects-time-hv-print-subjects-long-names", "false").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_CODES=settings.value("settings-timetables-subjects-time-hv-print-subjects-codes", "false").toBool();

	SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES=settings.value("settings-timetables-subjects-time-hv-print-activity-tags-names", "true").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=settings.value("settings-timetables-subjects-time-hv-print-activity-tags-long-names", "false").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES=settings.value("settings-timetables-subjects-time-hv-print-activity-tags-codes", "false").toBool();

	SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_NAMES=settings.value("settings-timetables-subjects-time-hv-print-teachers-names", "true").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_LONG_NAMES=settings.value("settings-timetables-subjects-time-hv-print-teachers-long-names", "false").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_CODES=settings.value("settings-timetables-subjects-time-hv-print-teachers-codes", "false").toBool();

	SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_NAMES=settings.value("settings-timetables-subjects-time-hv-print-students-names", "true").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_LONG_NAMES=settings.value("settings-timetables-subjects-time-hv-print-students-long-names", "false").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_CODES=settings.value("settings-timetables-subjects-time-hv-print-students-codes", "false").toBool();

	SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_NAMES=settings.value("settings-timetables-subjects-time-hv-print-rooms-names", "true").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_LONG_NAMES=settings.value("settings-timetables-subjects-time-hv-print-rooms-long-names", "false").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_CODES=settings.value("settings-timetables-subjects-time-hv-print-rooms-codes", "false").toBool();

	SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_LEGEND=settings.value("settings-timetables-subjects-time-hv-print-subjects-legend", "false").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND=settings.value("settings-timetables-subjects-time-hv-print-activity-tags-legend", "false").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_LEGEND=settings.value("settings-timetables-subjects-time-hv-print-teachers-legend", "false").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_LEGEND=settings.value("settings-timetables-subjects-time-hv-print-students-legend", "false").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_LEGEND=settings.value("settings-timetables-subjects-time-hv-print-rooms-legend", "false").toBool();
	SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_LEGEND_CODES_FIRST=settings.value("settings-timetables-subjects-time-hv-print-legend-codes-first", "false").toBool();
	/////

	/////activity tags days horizontal and days vertical.
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_DAYS_NAMES=settings.value("settings-timetables-activity-tags-days-hv-print-days-names", "true").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_DAYS_LONG_NAMES=settings.value("settings-timetables-activity-tags-days-hv-print-days-long-names", "false").toBool();

	SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_HOURS_NAMES=settings.value("settings-timetables-activity-tags-days-hv-print-hours-names", "true").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_HOURS_LONG_NAMES=settings.value("settings-timetables-activity-tags-days-hv-print-hours-long-names", "false").toBool();

	SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_SUBJECTS_NAMES=settings.value("settings-timetables-activity-tags-days-hv-print-subjects-names", "true").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES=settings.value("settings-timetables-activity-tags-days-hv-print-subjects-long-names", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_SUBJECTS_CODES=settings.value("settings-timetables-activity-tags-days-hv-print-subjects-codes", "false").toBool();

	SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES=settings.value("settings-timetables-activity-tags-days-hv-print-activity-tags-names", "true").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=settings.value("settings-timetables-activity-tags-days-hv-print-activity-tags-long-names", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES=settings.value("settings-timetables-activity-tags-days-hv-print-activity-tags-codes", "false").toBool();

	SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_TEACHERS_NAMES=settings.value("settings-timetables-activity-tags-days-hv-print-teachers-names", "true").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES=settings.value("settings-timetables-activity-tags-days-hv-print-teachers-long-names", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_TEACHERS_CODES=settings.value("settings-timetables-activity-tags-days-hv-print-teachers-codes", "false").toBool();

	SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_STUDENTS_NAMES=settings.value("settings-timetables-activity-tags-days-hv-print-students-names", "true").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES=settings.value("settings-timetables-activity-tags-days-hv-print-students-long-names", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_STUDENTS_CODES=settings.value("settings-timetables-activity-tags-days-hv-print-students-codes", "false").toBool();

	SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ROOMS_NAMES=settings.value("settings-timetables-activity-tags-days-hv-print-rooms-names", "true").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ROOMS_LONG_NAMES=settings.value("settings-timetables-activity-tags-days-hv-print-rooms-long-names", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ROOMS_CODES=settings.value("settings-timetables-activity-tags-days-hv-print-rooms-codes", "false").toBool();

	SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_SUBJECTS_LEGEND=settings.value("settings-timetables-activity-tags-days-hv-print-subjects-legend", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND=settings.value("settings-timetables-activity-tags-days-hv-print-activity-tags-legend", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_TEACHERS_LEGEND=settings.value("settings-timetables-activity-tags-days-hv-print-teachers-legend", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_STUDENTS_LEGEND=settings.value("settings-timetables-activity-tags-days-hv-print-students-legend", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ROOMS_LEGEND=settings.value("settings-timetables-activity-tags-days-hv-print-rooms-legend", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_LEGEND_CODES_FIRST=settings.value("settings-timetables-activity-tags-days-hv-print-legend-codes-first", "false").toBool();
	/////

	/////activity tags time horizontal and time vertical.
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_NAMES=settings.value("settings-timetables-activity-tags-time-hv-print-days-names", "true").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_LONG_NAMES=settings.value("settings-timetables-activity-tags-time-hv-print-days-long-names", "false").toBool();

	SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_NAMES=settings.value("settings-timetables-activity-tags-time-hv-print-hours-names", "true").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_LONG_NAMES=settings.value("settings-timetables-activity-tags-time-hv-print-hours-long-names", "false").toBool();

	SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_NAMES=settings.value("settings-timetables-activity-tags-time-hv-print-subjects-names", "true").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES=settings.value("settings-timetables-activity-tags-time-hv-print-subjects-long-names", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_CODES=settings.value("settings-timetables-activity-tags-time-hv-print-subjects-codes", "false").toBool();

	SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES=settings.value("settings-timetables-activity-tags-time-hv-print-activity-tags-names", "true").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=settings.value("settings-timetables-activity-tags-time-hv-print-activity-tags-long-names", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES=settings.value("settings-timetables-activity-tags-time-hv-print-activity-tags-codes", "false").toBool();

	SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_NAMES=settings.value("settings-timetables-activity-tags-time-hv-print-teachers-names", "true").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_LONG_NAMES=settings.value("settings-timetables-activity-tags-time-hv-print-teachers-long-names", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_CODES=settings.value("settings-timetables-activity-tags-time-hv-print-teachers-codes", "false").toBool();

	SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_NAMES=settings.value("settings-timetables-activity-tags-time-hv-print-students-names", "true").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_LONG_NAMES=settings.value("settings-timetables-activity-tags-time-hv-print-students-long-names", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_CODES=settings.value("settings-timetables-activity-tags-time-hv-print-students-codes", "false").toBool();

	SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_NAMES=settings.value("settings-timetables-activity-tags-time-hv-print-rooms-names", "true").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_LONG_NAMES=settings.value("settings-timetables-activity-tags-time-hv-print-rooms-long-names", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_CODES=settings.value("settings-timetables-activity-tags-time-hv-print-rooms-codes", "false").toBool();

	SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_LEGEND=settings.value("settings-timetables-activity-tags-time-hv-print-subjects-legend", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND=settings.value("settings-timetables-activity-tags-time-hv-print-activity-tags-legend", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_LEGEND=settings.value("settings-timetables-activity-tags-time-hv-print-teachers-legend", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_LEGEND=settings.value("settings-timetables-activity-tags-time-hv-print-students-legend", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_LEGEND=settings.value("settings-timetables-activity-tags-time-hv-print-rooms-legend", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_LEGEND_CODES_FIRST=settings.value("settings-timetables-activity-tags-time-hv-print-legend-codes-first", "false").toBool();
	/////

	/////activities days horizontal and days vertical.
	SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_DAYS_NAMES=settings.value("settings-timetables-activities-days-hv-print-days-names", "true").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_DAYS_LONG_NAMES=settings.value("settings-timetables-activities-days-hv-print-days-long-names", "false").toBool();

	SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_HOURS_NAMES=settings.value("settings-timetables-activities-days-hv-print-hours-names", "true").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_HOURS_LONG_NAMES=settings.value("settings-timetables-activities-days-hv-print-hours-long-names", "false").toBool();

	SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_SUBJECTS_NAMES=settings.value("settings-timetables-activities-days-hv-print-subjects-names", "true").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES=settings.value("settings-timetables-activities-days-hv-print-subjects-long-names", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_SUBJECTS_CODES=settings.value("settings-timetables-activities-days-hv-print-subjects-codes", "false").toBool();

	SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES=settings.value("settings-timetables-activities-days-hv-print-activity-tags-names", "true").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=settings.value("settings-timetables-activities-days-hv-print-activity-tags-long-names", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES=settings.value("settings-timetables-activities-days-hv-print-activity-tags-codes", "false").toBool();

	SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_TEACHERS_NAMES=settings.value("settings-timetables-activities-days-hv-print-teachers-names", "true").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_TEACHERS_LONG_NAMES=settings.value("settings-timetables-activities-days-hv-print-teachers-long-names", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_TEACHERS_CODES=settings.value("settings-timetables-activities-days-hv-print-teachers-codes", "false").toBool();

	SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_STUDENTS_NAMES=settings.value("settings-timetables-activities-days-hv-print-students-names", "true").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_STUDENTS_LONG_NAMES=settings.value("settings-timetables-activities-days-hv-print-students-long-names", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_STUDENTS_CODES=settings.value("settings-timetables-activities-days-hv-print-students-codes", "false").toBool();

	SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ROOMS_NAMES=settings.value("settings-timetables-activities-days-hv-print-rooms-names", "true").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ROOMS_LONG_NAMES=settings.value("settings-timetables-activities-days-hv-print-rooms-long-names", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ROOMS_CODES=settings.value("settings-timetables-activities-days-hv-print-rooms-codes", "false").toBool();

	SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_SUBJECTS_LEGEND=settings.value("settings-timetables-activities-days-hv-print-subjects-legend", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND=settings.value("settings-timetables-activities-days-hv-print-activity-tags-legend", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_TEACHERS_LEGEND=settings.value("settings-timetables-activities-days-hv-print-teachers-legend", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_STUDENTS_LEGEND=settings.value("settings-timetables-activities-days-hv-print-students-legend", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ROOMS_LEGEND=settings.value("settings-timetables-activities-days-hv-print-rooms-legend", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_LEGEND_CODES_FIRST=settings.value("settings-timetables-activities-days-hv-print-legend-codes-first", "false").toBool();
	/////

	/////activities time horizontal and time vertical.
	SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_NAMES=settings.value("settings-timetables-activities-time-hv-print-days-names", "true").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_LONG_NAMES=settings.value("settings-timetables-activities-time-hv-print-days-long-names", "false").toBool();

	SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_NAMES=settings.value("settings-timetables-activities-time-hv-print-hours-names", "true").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_LONG_NAMES=settings.value("settings-timetables-activities-time-hv-print-hours-long-names", "false").toBool();

	SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_NAMES=settings.value("settings-timetables-activities-time-hv-print-subjects-names", "true").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_LONG_NAMES=settings.value("settings-timetables-activities-time-hv-print-subjects-long-names", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_CODES=settings.value("settings-timetables-activities-time-hv-print-subjects-codes", "false").toBool();

	SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES=settings.value("settings-timetables-activities-time-hv-print-activity-tags-names", "true").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=settings.value("settings-timetables-activities-time-hv-print-activity-tags-long-names", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_CODES=settings.value("settings-timetables-activities-time-hv-print-activity-tags-codes", "false").toBool();

	SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_NAMES=settings.value("settings-timetables-activities-time-hv-print-teachers-names", "true").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_LONG_NAMES=settings.value("settings-timetables-activities-time-hv-print-teachers-long-names", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_CODES=settings.value("settings-timetables-activities-time-hv-print-teachers-codes", "false").toBool();

	SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_NAMES=settings.value("settings-timetables-activities-time-hv-print-students-names", "true").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_LONG_NAMES=settings.value("settings-timetables-activities-time-hv-print-students-long-names", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_CODES=settings.value("settings-timetables-activities-time-hv-print-students-codes", "false").toBool();

	SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_NAMES=settings.value("settings-timetables-activities-time-hv-print-rooms-names", "true").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_LONG_NAMES=settings.value("settings-timetables-activities-time-hv-print-rooms-long-names", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_CODES=settings.value("settings-timetables-activities-time-hv-print-rooms-codes", "false").toBool();

	SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_LEGEND=settings.value("settings-timetables-activities-time-hv-print-subjects-legend", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND=settings.value("settings-timetables-activities-time-hv-print-activity-tags-legend", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_LEGEND=settings.value("settings-timetables-activities-time-hv-print-teachers-legend", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_LEGEND=settings.value("settings-timetables-activities-time-hv-print-students-legend", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_LEGEND=settings.value("settings-timetables-activities-time-hv-print-rooms-legend", "false").toBool();
	SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_LEGEND_CODES_FIRST=settings.value("settings-timetables-activities-time-hv-print-legend-codes-first", "false").toBool();
	/////
	//
	//2024-06-12 end
	
	//main form
	QRect rect=settings.value("FetMainForm/geometry", QRect(0,0,0,0)).toRect();
	mainFormSettingsRect=rect;
	//MAIN_FORM_SHORTCUTS_TAB_POSITION=settings.value("FetMainForm/shortcuts-tab-position", "0").toInt();
	MAIN_FORM_SHORTCUTS_TAB_POSITION=0; //always restoring to the first page, as suggested by a user
	
	if(settings.contains("FetMainForm/show-shortcut-buttons")){
		SHOW_SHORTCUTS_ON_MAIN_WINDOW=settings.value("FetMainForm/show-shortcut-buttons").toBool();
	}
	else if(settings.contains("FetMainForm/show-shortcuts")){ //obsolete style
		SHOW_SHORTCUTS_ON_MAIN_WINDOW=settings.value("FetMainForm/show-shortcuts").toBool();
		//settings.remove("FetMainForm/show-shortcuts");
	}
	else{
		SHOW_SHORTCUTS_ON_MAIN_WINDOW=true;
	}

	if(settings.contains("show-tooltips-for-constraints-with-tables")){
		SHOW_TOOLTIPS_FOR_CONSTRAINTS_WITH_TABLES=settings.value("show-tooltips-for-constraints-with-tables").toBool();
	}
	else if(settings.contains("FetMainForm/show-tooltips-for-constraints-with-tables")){ //obsolete style
		SHOW_TOOLTIPS_FOR_CONSTRAINTS_WITH_TABLES=settings.value("FetMainForm/show-tooltips-for-constraints-with-tables").toBool();
		//settings.remove("FetMainForm/show-tooltips-for-constraints-with-tables");
	}
	else{
		SHOW_TOOLTIPS_FOR_CONSTRAINTS_WITH_TABLES=false;
	}
	
	BEEP_AT_END_OF_GENERATION=settings.value("beep-at-the-end-of-generation", "true").toBool();
	ENABLE_COMMAND_AT_END_OF_GENERATION=settings.value("enable-command-at-the-end-of-generation", "false").toBool();
	commandAtEndOfGeneration=settings.value("command-at-the-end-of-generation", "").toString();

	BEEP_AT_END_OF_GENERATION_EACH_TIMETABLE=settings.value("beep-at-the-end-of-generation-each-timetable", "false").toBool();
	ENABLE_COMMAND_AT_END_OF_GENERATION_EACH_TIMETABLE=settings.value("enable-command-at-the-end-of-generation-each-timetable", "false").toBool();
	commandAtEndOfGenerationEachTimetable=settings.value("command-at-the-end-of-generation-each-timetable", "").toString();

//	DETACHED_NOTIFICATION=settings.value("detached-notification", "false").toBool();
//	terminateCommandAfterSeconds=settings.value("terminate-command-at-the-end-of-generation-after-seconds", "0").toInt();
//	killCommandAfterSeconds=settings.value("kill-command-at-the-end-of-generation-after-seconds", "0").toInt();
	
	if(VERBOSE){
		std::cout<<"Settings read"<<std::endl;
	}
}

void FetSettings::writeGenerationParameters()
{
	QSettings settings(COMPANY, PROGRAM);

	if(gt.rules.mode==OFFICIAL)
		settings.setValue("mode", "official");
	else if(gt.rules.mode==MORNINGS_AFTERNOONS)
		settings.setValue("mode", "mornings-afternoons");
	else if(gt.rules.mode==BLOCK_PLANNING)
		settings.setValue("mode", "block-planning");
	else if(gt.rules.mode==TERMS)
		settings.setValue("mode", "terms");
	else
		assert(0);

	settings.setValue("output-directory", OUTPUT_DIR);
	settings.setValue("language", FET_LANGUAGE);
	settings.setValue("working-directory", WORKING_DIRECTORY);
	settings.setValue("import-directory", IMPORT_DIRECTORY);
	settings.setValue("version", FET_VERSION);
	settings.setValue("check-for-updates", checkForUpdates);

	settings.setValue("overwrite-single-generation-files", OVERWRITE_SINGLE_GENERATION_FILES);

	settings.setValue("html-level", TIMETABLE_HTML_LEVEL);
	settings.setValue("print-activity-tags", TIMETABLE_HTML_PRINT_ACTIVITY_TAGS);
	
	settings.setValue("print-subjects", TIMETABLE_HTML_PRINT_SUBJECTS);
	settings.setValue("print-teachers", TIMETABLE_HTML_PRINT_TEACHERS);
	settings.setValue("print-students", TIMETABLE_HTML_PRINT_STUDENTS);
	settings.setValue("print-rooms", TIMETABLE_HTML_PRINT_ROOMS);
	
	settings.setValue("timetables-subgroups-sorted", TIMETABLES_SUBGROUPS_SORTED);
	settings.setValue("print-detailed-timetables", PRINT_DETAILED_HTML_TIMETABLES);
	settings.setValue("print-detailed-teachers-free-periods-timetables", PRINT_DETAILED_HTML_TEACHERS_FREE_PERIODS);
	settings.setValue("print-activities-with-same-starting-time", PRINT_ACTIVITIES_WITH_SAME_STARTING_TIME);
	settings.setValue("divide-html-timetables-with-time-axis-by-days", DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS);
	settings.setValue("timetables-repeat-vertical-names", TIMETABLE_HTML_REPEAT_NAMES);
	settings.setValue("print-not-available", PRINT_NOT_AVAILABLE_TIME_SLOTS);
	settings.setValue("print-break", PRINT_BREAK_TIME_SLOTS);

	settings.setValue("interface-style", INTERFACE_STYLE);
	settings.setValue("interface-color-scheme", INTERFACE_COLOR_SCHEME);

	settings.setValue("show-subgroups-in-combo-boxes", SHOW_SUBGROUPS_IN_COMBO_BOXES);
	settings.setValue("show-subgroups-in-activity-planning", SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING);
	
	settings.setValue("write-timetable-conflicts", WRITE_TIMETABLE_CONFLICTS);

	settings.setValue("write-timetables-statistics", WRITE_TIMETABLES_STATISTICS);
	settings.setValue("write-timetables-xml", WRITE_TIMETABLES_XML);
	settings.setValue("write-timetables-days-horizontal", WRITE_TIMETABLES_DAYS_HORIZONTAL);
	settings.setValue("write-timetables-days-vertical", WRITE_TIMETABLES_DAYS_VERTICAL);
	settings.setValue("write-timetables-time-horizontal", WRITE_TIMETABLES_TIME_HORIZONTAL);
	settings.setValue("write-timetables-time-vertical", WRITE_TIMETABLES_TIME_VERTICAL);

	settings.setValue("write-timetables-subgroups", WRITE_TIMETABLES_SUBGROUPS);
	settings.setValue("write-timetables-groups", WRITE_TIMETABLES_GROUPS);
	settings.setValue("write-timetables-years", WRITE_TIMETABLES_YEARS);
	settings.setValue("write-timetables-teachers", WRITE_TIMETABLES_TEACHERS);
	settings.setValue("write-timetables-teachers-free-periods", WRITE_TIMETABLES_TEACHERS_FREE_PERIODS);
	settings.setValue("write-timetables-buildings", WRITE_TIMETABLES_BUILDINGS);
	settings.setValue("write-timetables-rooms", WRITE_TIMETABLES_ROOMS);
	settings.setValue("write-timetables-subjects", WRITE_TIMETABLES_SUBJECTS);
	settings.setValue("write-timetables-activity-tags", WRITE_TIMETABLES_ACTIVITY_TAGS);
	settings.setValue("write-timetables-activities", WRITE_TIMETABLES_ACTIVITIES);
	
	settings.setValue("students-combo-boxes-style", STUDENTS_COMBO_BOXES_STYLE);

///////////confirmations
	settings.setValue("confirm-activity-planning", CONFIRM_ACTIVITY_PLANNING);
	settings.setValue("confirm-spread-activities", CONFIRM_SPREAD_ACTIVITIES);
	settings.setValue("confirm-remove-redundant", CONFIRM_REMOVE_REDUNDANT);
	settings.setValue("confirm-save-data-and-timetable", CONFIRM_SAVE_TIMETABLE);
	settings.setValue("confirm-activate-deactivate-activities-constraints", CONFIRM_ACTIVATE_DEACTIVATE_ACTIVITIES_CONSTRAINTS);
///////////

	settings.setValue("shorcut-plus", SHORTCUT_PLUS);
	settings.setValue("shorcut-m", SHORTCUT_M);
	settings.setValue("shorcut-delete", SHORTCUT_DELETE);
	settings.setValue("shorcut-a", SHORTCUT_A);
	settings.setValue("shorcut-d", SHORTCUT_D);
	settings.setValue("shorcut-c", SHORTCUT_C);
	settings.setValue("shorcut-u", SHORTCUT_U);
	settings.setValue("shorcut-j", SHORTCUT_J);
	settings.setValue("shorcut-w", SHORTCUT_W);

	settings.setValue("warn-subgroups-with-the-same-activities", SHOW_WARNING_FOR_SUBGROUPS_WITH_THE_SAME_ACTIVITIES);
	settings.setValue("warn-activities-not-fixed-time-fixed-space-virtual-real", SHOW_WARNING_FOR_ACTIVITIES_FIXED_SPACE_VIRTUAL_REAL_ROOMS_BUT_NOT_FIXED_TIME);
	settings.setValue("warn-max-hours-daily-with-under-100-weight", SHOW_WARNING_FOR_MAX_HOURS_DAILY_WITH_UNDER_100_WEIGHT);

	settings.setValue("enable-group-activities-in-initial-order", ENABLE_GROUP_ACTIVITIES_IN_INITIAL_ORDER);
	settings.setValue("warn-if-using-group-activities-in-initial-order", SHOW_WARNING_FOR_GROUP_ACTIVITIES_IN_INITIAL_ORDER);

	settings.setValue("show-virtual-rooms-in-timetables", SHOW_VIRTUAL_ROOMS_IN_TIMETABLES);

	//2024-06-12 begin
	//
	settings.setValue("settings-timetables-separate-days-name-long-name-by-break", SETTINGS_TIMETABLES_SEPARATE_DAYS_NAME_LONG_NAME_BY_BREAK);
	settings.setValue("settings-timetables-separate-hours-name-long-name-by-break", SETTINGS_TIMETABLES_SEPARATE_HOURS_NAME_LONG_NAME_BY_BREAK);
	settings.setValue("settings-timetables-separate-subjects-name-long-name-code-by-break", SETTINGS_TIMETABLES_SEPARATE_SUBJECTS_NAME_LONG_NAME_CODE_BY_BREAK);
	settings.setValue("settings-timetables-separate-activity-tags-name-long-name-code-by-break", SETTINGS_TIMETABLES_SEPARATE_ACTIVITY_TAGS_NAME_LONG_NAME_CODE_BY_BREAK);
	settings.setValue("settings-timetables-separate-teachers-name-long-name-code-by-break", SETTINGS_TIMETABLES_SEPARATE_TEACHERS_NAME_LONG_NAME_CODE_BY_BREAK);
	settings.setValue("settings-timetables-separate-students-name-long-name-code-by-break", SETTINGS_TIMETABLES_SEPARATE_STUDENTS_NAME_LONG_NAME_CODE_BY_BREAK);
	settings.setValue("settings-timetables-separate-buildings-name-long-name-code-by-break", SETTINGS_TIMETABLES_SEPARATE_BUILDINGS_NAME_LONG_NAME_CODE_BY_BREAK);
	settings.setValue("settings-timetables-separate-rooms-name-long-name-code-by-break", SETTINGS_TIMETABLES_SEPARATE_ROOMS_NAME_LONG_NAME_CODE_BY_BREAK);

	//only in days horizontal and days vertical.
	settings.setValue("settings-timetables-print-subjects-comments", SETTINGS_TIMETABLES_PRINT_SUBJECTS_COMMENTS);
	settings.setValue("settings-timetables-print-activity-tags-comments", SETTINGS_TIMETABLES_PRINT_ACTIVITY_TAGS_COMMENTS);
	settings.setValue("settings-timetables-print-teachers-comments", SETTINGS_TIMETABLES_PRINT_TEACHERS_COMMENTS);
	settings.setValue("settings-timetables-print-subgroups-comments", SETTINGS_TIMETABLES_PRINT_SUBGROUPS_COMMENTS);
	settings.setValue("settings-timetables-print-groups-comments", SETTINGS_TIMETABLES_PRINT_GROUPS_COMMENTS);
	settings.setValue("settings-timetables-print-years-comments", SETTINGS_TIMETABLES_PRINT_YEARS_COMMENTS);
	settings.setValue("settings-timetables-print-buildings-comments", SETTINGS_TIMETABLES_PRINT_BUILDINGS_COMMENTS);
	settings.setValue("settings-timetables-print-rooms-comments", SETTINGS_TIMETABLES_PRINT_ROOMS_COMMENTS);

	/////subgroups days horizontal and days vertical.
	settings.setValue("settings-timetables-subgroups-days-hv-print-days-names", SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_DAYS_NAMES);
	settings.setValue("settings-timetables-subgroups-days-hv-print-days-long-names", SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_DAYS_LONG_NAMES);

	settings.setValue("settings-timetables-subgroups-days-hv-print-hours-names", SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_HOURS_NAMES);
	settings.setValue("settings-timetables-subgroups-days-hv-print-hours-long-names", SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_HOURS_LONG_NAMES);

	settings.setValue("settings-timetables-subgroups-days-hv-print-subjects-names", SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_NAMES);
	settings.setValue("settings-timetables-subgroups-days-hv-print-subjects-long-names", SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES);
	settings.setValue("settings-timetables-subgroups-days-hv-print-subjects-codes", SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_CODES);

	settings.setValue("settings-timetables-subgroups-days-hv-print-activity-tags-names", SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES);
	settings.setValue("settings-timetables-subgroups-days-hv-print-activity-tags-long-names", SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES);
	settings.setValue("settings-timetables-subgroups-days-hv-print-activity-tags-codes", SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES);

	settings.setValue("settings-timetables-subgroups-days-hv-print-teachers-names", SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_NAMES);
	settings.setValue("settings-timetables-subgroups-days-hv-print-teachers-long-names", SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES);
	settings.setValue("settings-timetables-subgroups-days-hv-print-teachers-codes", SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_CODES);

	settings.setValue("settings-timetables-subgroups-days-hv-print-students-names", SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_NAMES);
	settings.setValue("settings-timetables-subgroups-days-hv-print-students-long-names", SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES);
	settings.setValue("settings-timetables-subgroups-days-hv-print-students-codes", SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_CODES);

	settings.setValue("settings-timetables-subgroups-days-hv-print-rooms-names", SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_NAMES);
	settings.setValue("settings-timetables-subgroups-days-hv-print-rooms-long-names", SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_LONG_NAMES);
	settings.setValue("settings-timetables-subgroups-days-hv-print-rooms-codes", SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_CODES);

	settings.setValue("settings-timetables-subgroups-days-hv-print-subjects-legend", SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_LEGEND);
	settings.setValue("settings-timetables-subgroups-days-hv-print-activity-tags-legend", SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND);
	settings.setValue("settings-timetables-subgroups-days-hv-print-teachers-legend", SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_LEGEND);
	settings.setValue("settings-timetables-subgroups-days-hv-print-students-legend", SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_LEGEND);
	settings.setValue("settings-timetables-subgroups-days-hv-print-rooms-legend", SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_LEGEND);
	settings.setValue("settings-timetables-subgroups-days-hv-print-legend-codes-first", SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_LEGEND_CODES_FIRST);
	/////

	/////subgroups time horizontal and time vertical.
	settings.setValue("settings-timetables-subgroups-time-hv-print-days-names", SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_NAMES);
	settings.setValue("settings-timetables-subgroups-time-hv-print-days-long-names", SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES);

	settings.setValue("settings-timetables-subgroups-time-hv-print-hours-names", SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_NAMES);
	settings.setValue("settings-timetables-subgroups-time-hv-print-hours-long-names", SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES);

	settings.setValue("settings-timetables-subgroups-time-hv-print-subjects-names", SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_NAMES);
	settings.setValue("settings-timetables-subgroups-time-hv-print-subjects-long-names", SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES);
	settings.setValue("settings-timetables-subgroups-time-hv-print-subjects-codes", SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_CODES);

	settings.setValue("settings-timetables-subgroups-time-hv-print-activity-tags-names", SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES);
	settings.setValue("settings-timetables-subgroups-time-hv-print-activity-tags-long-names", SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES);
	settings.setValue("settings-timetables-subgroups-time-hv-print-activity-tags-codes", SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES);

	settings.setValue("settings-timetables-subgroups-time-hv-print-teachers-names", SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_NAMES);
	settings.setValue("settings-timetables-subgroups-time-hv-print-teachers-long-names", SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_LONG_NAMES);
	settings.setValue("settings-timetables-subgroups-time-hv-print-teachers-codes", SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_CODES);

	settings.setValue("settings-timetables-subgroups-time-hv-print-students-names", SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_NAMES);
	settings.setValue("settings-timetables-subgroups-time-hv-print-students-long-names", SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES);
	settings.setValue("settings-timetables-subgroups-time-hv-print-students-codes", SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_CODES);

	settings.setValue("settings-timetables-subgroups-time-hv-print-rooms-names", SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_NAMES);
	settings.setValue("settings-timetables-subgroups-time-hv-print-rooms-long-names", SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_LONG_NAMES);
	settings.setValue("settings-timetables-subgroups-time-hv-print-rooms-codes", SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_CODES);

	settings.setValue("settings-timetables-subgroups-time-hv-print-subjects-legend", SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_LEGEND);
	settings.setValue("settings-timetables-subgroups-time-hv-print-activity-tags-legend", SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND);
	settings.setValue("settings-timetables-subgroups-time-hv-print-teachers-legend", SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_LEGEND);
	settings.setValue("settings-timetables-subgroups-time-hv-print-students-legend", SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_LEGEND);
	settings.setValue("settings-timetables-subgroups-time-hv-print-rooms-legend", SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_LEGEND);
	settings.setValue("settings-timetables-subgroups-time-hv-print-legend-codes-first", SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_LEGEND_CODES_FIRST);
	/////

	/////groups days horizontal and days vertical.
	settings.setValue("settings-timetables-groups-days-hv-print-days-names", SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_DAYS_NAMES);
	settings.setValue("settings-timetables-groups-days-hv-print-days-long-names", SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_DAYS_LONG_NAMES);

	settings.setValue("settings-timetables-groups-days-hv-print-hours-names", SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_HOURS_NAMES);
	settings.setValue("settings-timetables-groups-days-hv-print-hours-long-names", SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_HOURS_LONG_NAMES);

	settings.setValue("settings-timetables-groups-days-hv-print-subjects-names", SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_NAMES);
	settings.setValue("settings-timetables-groups-days-hv-print-subjects-long-names", SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES);
	settings.setValue("settings-timetables-groups-days-hv-print-subjects-codes", SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_CODES);

	settings.setValue("settings-timetables-groups-days-hv-print-activity-tags-names", SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES);
	settings.setValue("settings-timetables-groups-days-hv-print-activity-tags-long-names", SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES);
	settings.setValue("settings-timetables-groups-days-hv-print-activity-tags-codes", SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES);

	settings.setValue("settings-timetables-groups-days-hv-print-teachers-names", SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_NAMES);
	settings.setValue("settings-timetables-groups-days-hv-print-teachers-long-names", SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES);
	settings.setValue("settings-timetables-groups-days-hv-print-teachers-codes", SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_CODES);

	settings.setValue("settings-timetables-groups-days-hv-print-students-names", SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_NAMES);
	settings.setValue("settings-timetables-groups-days-hv-print-students-long-names", SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES);
	settings.setValue("settings-timetables-groups-days-hv-print-students-codes", SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_CODES);

	settings.setValue("settings-timetables-groups-days-hv-print-rooms-names", SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_NAMES);
	settings.setValue("settings-timetables-groups-days-hv-print-rooms-long-names", SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_LONG_NAMES);
	settings.setValue("settings-timetables-groups-days-hv-print-rooms-codes", SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_CODES);

	settings.setValue("settings-timetables-groups-days-hv-print-subjects-legend", SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_LEGEND);
	settings.setValue("settings-timetables-groups-days-hv-print-activity-tags-legend", SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND);
	settings.setValue("settings-timetables-groups-days-hv-print-teachers-legend", SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_LEGEND);
	settings.setValue("settings-timetables-groups-days-hv-print-students-legend", SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_LEGEND);
	settings.setValue("settings-timetables-groups-days-hv-print-rooms-legend", SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_LEGEND);
	settings.setValue("settings-timetables-groups-days-hv-print-legend-codes-first", SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_LEGEND_CODES_FIRST);
	/////

	/////groups time horizontal and time vertical.
	settings.setValue("settings-timetables-groups-time-hv-print-days-names", SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_NAMES);
	settings.setValue("settings-timetables-groups-time-hv-print-days-long-names", SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES);

	settings.setValue("settings-timetables-groups-time-hv-print-hours-names", SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_NAMES);
	settings.setValue("settings-timetables-groups-time-hv-print-hours-long-names", SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES);

	settings.setValue("settings-timetables-groups-time-hv-print-subjects-names", SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_NAMES);
	settings.setValue("settings-timetables-groups-time-hv-print-subjects-long-names", SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES);
	settings.setValue("settings-timetables-groups-time-hv-print-subjects-codes", SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_CODES);

	settings.setValue("settings-timetables-groups-time-hv-print-activity-tags-names", SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES);
	settings.setValue("settings-timetables-groups-time-hv-print-activity-tags-long-names", SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES);
	settings.setValue("settings-timetables-groups-time-hv-print-activity-tags-codes", SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES);

	settings.setValue("settings-timetables-groups-time-hv-print-teachers-names", SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_NAMES);
	settings.setValue("settings-timetables-groups-time-hv-print-teachers-long-names", SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_LONG_NAMES);
	settings.setValue("settings-timetables-groups-time-hv-print-teachers-codes", SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_CODES);

	settings.setValue("settings-timetables-groups-time-hv-print-students-names", SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_NAMES);
	settings.setValue("settings-timetables-groups-time-hv-print-students-long-names", SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES);
	settings.setValue("settings-timetables-groups-time-hv-print-students-codes", SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_CODES);

	settings.setValue("settings-timetables-groups-time-hv-print-rooms-names", SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_NAMES);
	settings.setValue("settings-timetables-groups-time-hv-print-rooms-long-names", SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_LONG_NAMES);
	settings.setValue("settings-timetables-groups-time-hv-print-rooms-codes", SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_CODES);

	settings.setValue("settings-timetables-groups-time-hv-print-subjects-legend", SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_LEGEND);
	settings.setValue("settings-timetables-groups-time-hv-print-activity-tags-legend", SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND);
	settings.setValue("settings-timetables-groups-time-hv-print-teachers-legend", SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_LEGEND);
	settings.setValue("settings-timetables-groups-time-hv-print-students-legend", SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_LEGEND);
	settings.setValue("settings-timetables-groups-time-hv-print-rooms-legend", SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_LEGEND);
	settings.setValue("settings-timetables-groups-time-hv-print-legend-codes-first", SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_LEGEND_CODES_FIRST);
	/////

	/////years days horizontal and days vertical.
	settings.setValue("settings-timetables-years-days-hv-print-days-names", SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_DAYS_NAMES);
	settings.setValue("settings-timetables-years-days-hv-print-days-long-names", SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_DAYS_LONG_NAMES);

	settings.setValue("settings-timetables-years-days-hv-print-hours-names", SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_HOURS_NAMES);
	settings.setValue("settings-timetables-years-days-hv-print-hours-long-names", SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_HOURS_LONG_NAMES);

	settings.setValue("settings-timetables-years-days-hv-print-subjects-names", SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_NAMES);
	settings.setValue("settings-timetables-years-days-hv-print-subjects-long-names", SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES);
	settings.setValue("settings-timetables-years-days-hv-print-subjects-codes", SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_CODES);

	settings.setValue("settings-timetables-years-days-hv-print-activity-tags-names", SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES);
	settings.setValue("settings-timetables-years-days-hv-print-activity-tags-long-names", SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES);
	settings.setValue("settings-timetables-years-days-hv-print-activity-tags-codes", SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES);

	settings.setValue("settings-timetables-years-days-hv-print-teachers-names", SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_NAMES);
	settings.setValue("settings-timetables-years-days-hv-print-teachers-long-names", SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES);
	settings.setValue("settings-timetables-years-days-hv-print-teachers-codes", SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_CODES);

	settings.setValue("settings-timetables-years-days-hv-print-students-names", SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_NAMES);
	settings.setValue("settings-timetables-years-days-hv-print-students-long-names", SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES);
	settings.setValue("settings-timetables-years-days-hv-print-students-codes", SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_CODES);

	settings.setValue("settings-timetables-years-days-hv-print-rooms-names", SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_NAMES);
	settings.setValue("settings-timetables-years-days-hv-print-rooms-long-names", SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_LONG_NAMES);
	settings.setValue("settings-timetables-years-days-hv-print-rooms-codes", SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_CODES);

	settings.setValue("settings-timetables-years-days-hv-print-subjects-legend", SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_LEGEND);
	settings.setValue("settings-timetables-years-days-hv-print-activity-tags-legend", SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND);
	settings.setValue("settings-timetables-years-days-hv-print-teachers-legend", SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_LEGEND);
	settings.setValue("settings-timetables-years-days-hv-print-students-legend", SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_LEGEND);
	settings.setValue("settings-timetables-years-days-hv-print-rooms-legend", SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_LEGEND);
	settings.setValue("settings-timetables-years-days-hv-print-legend-codes-first", SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_LEGEND_CODES_FIRST);
	/////

	/////years time horizontal and time vertical.
	settings.setValue("settings-timetables-years-time-hv-print-days-names", SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_NAMES);
	settings.setValue("settings-timetables-years-time-hv-print-days-long-names", SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_LONG_NAMES);

	settings.setValue("settings-timetables-years-time-hv-print-hours-names", SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_NAMES);
	settings.setValue("settings-timetables-years-time-hv-print-hours-long-names", SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_LONG_NAMES);

	settings.setValue("settings-timetables-years-time-hv-print-subjects-names", SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_NAMES);
	settings.setValue("settings-timetables-years-time-hv-print-subjects-long-names", SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES);
	settings.setValue("settings-timetables-years-time-hv-print-subjects-codes", SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_CODES);

	settings.setValue("settings-timetables-years-time-hv-print-activity-tags-names", SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES);
	settings.setValue("settings-timetables-years-time-hv-print-activity-tags-long-names", SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES);
	settings.setValue("settings-timetables-years-time-hv-print-activity-tags-codes", SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES);

	settings.setValue("settings-timetables-years-time-hv-print-teachers-names", SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_NAMES);
	settings.setValue("settings-timetables-years-time-hv-print-teachers-long-names", SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_LONG_NAMES);
	settings.setValue("settings-timetables-years-time-hv-print-teachers-codes", SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_CODES);

	settings.setValue("settings-timetables-years-time-hv-print-students-names", SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_NAMES);
	settings.setValue("settings-timetables-years-time-hv-print-students-long-names", SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_LONG_NAMES);
	settings.setValue("settings-timetables-years-time-hv-print-students-codes", SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_CODES);

	settings.setValue("settings-timetables-years-time-hv-print-rooms-names", SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_NAMES);
	settings.setValue("settings-timetables-years-time-hv-print-rooms-long-names", SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_LONG_NAMES);
	settings.setValue("settings-timetables-years-time-hv-print-rooms-codes", SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_CODES);

	settings.setValue("settings-timetables-years-time-hv-print-subjects-legend", SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_LEGEND);
	settings.setValue("settings-timetables-years-time-hv-print-activity-tags-legend", SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND);
	settings.setValue("settings-timetables-years-time-hv-print-teachers-legend", SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_LEGEND);
	settings.setValue("settings-timetables-years-time-hv-print-students-legend", SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_LEGEND);
	settings.setValue("settings-timetables-years-time-hv-print-rooms-legend", SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_LEGEND);
	settings.setValue("settings-timetables-years-time-hv-print-legend-codes-first", SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_LEGEND_CODES_FIRST);
	/////

	/////teachers days horizontal and days vertical.
	settings.setValue("settings-timetables-teachers-days-hv-print-days-names", SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_DAYS_NAMES);
	settings.setValue("settings-timetables-teachers-days-hv-print-days-long-names", SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_DAYS_LONG_NAMES);

	settings.setValue("settings-timetables-teachers-days-hv-print-hours-names", SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_HOURS_NAMES);
	settings.setValue("settings-timetables-teachers-days-hv-print-hours-long-names", SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_HOURS_LONG_NAMES);

	settings.setValue("settings-timetables-teachers-days-hv-print-subjects-names", SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_NAMES);
	settings.setValue("settings-timetables-teachers-days-hv-print-subjects-long-names", SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES);
	settings.setValue("settings-timetables-teachers-days-hv-print-subjects-codes", SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_CODES);

	settings.setValue("settings-timetables-teachers-days-hv-print-activity-tags-names", SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES);
	settings.setValue("settings-timetables-teachers-days-hv-print-activity-tags-long-names", SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES);
	settings.setValue("settings-timetables-teachers-days-hv-print-activity-tags-codes", SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES);

	settings.setValue("settings-timetables-teachers-days-hv-print-teachers-names", SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_NAMES);
	settings.setValue("settings-timetables-teachers-days-hv-print-teachers-long-names", SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES);
	settings.setValue("settings-timetables-teachers-days-hv-print-teachers-codes", SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_CODES);

	settings.setValue("settings-timetables-teachers-days-hv-print-students-names", SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_NAMES);
	settings.setValue("settings-timetables-teachers-days-hv-print-students-long-names", SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES);
	settings.setValue("settings-timetables-teachers-days-hv-print-students-codes", SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_CODES);

	settings.setValue("settings-timetables-teachers-days-hv-print-rooms-names", SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_NAMES);
	settings.setValue("settings-timetables-teachers-days-hv-print-rooms-long-names", SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_LONG_NAMES);
	settings.setValue("settings-timetables-teachers-days-hv-print-rooms-codes", SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_CODES);

	settings.setValue("settings-timetables-teachers-days-hv-print-subjects-legend", SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_LEGEND);
	settings.setValue("settings-timetables-teachers-days-hv-print-activity-tags-legend", SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND);
	settings.setValue("settings-timetables-teachers-days-hv-print-teachers-legend", SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_LEGEND);
	settings.setValue("settings-timetables-teachers-days-hv-print-students-legend", SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_LEGEND);
	settings.setValue("settings-timetables-teachers-days-hv-print-rooms-legend", SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_LEGEND);
	settings.setValue("settings-timetables-teachers-days-hv-print-legend-codes-first", SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_LEGEND_CODES_FIRST);
	/////

	/////teachers time horizontal and time vertical.
	settings.setValue("settings-timetables-teachers-time-hv-print-days-names", SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_NAMES);
	settings.setValue("settings-timetables-teachers-time-hv-print-days-long-names", SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_LONG_NAMES);

	settings.setValue("settings-timetables-teachers-time-hv-print-hours-names", SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_NAMES);
	settings.setValue("settings-timetables-teachers-time-hv-print-hours-long-names", SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_LONG_NAMES);

	settings.setValue("settings-timetables-teachers-time-hv-print-subjects-names", SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_NAMES);
	settings.setValue("settings-timetables-teachers-time-hv-print-subjects-long-names", SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES);
	settings.setValue("settings-timetables-teachers-time-hv-print-subjects-codes", SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_CODES);

	settings.setValue("settings-timetables-teachers-time-hv-print-activity-tags-names", SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES);
	settings.setValue("settings-timetables-teachers-time-hv-print-activity-tags-long-names", SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES);
	settings.setValue("settings-timetables-teachers-time-hv-print-activity-tags-codes", SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES);

	settings.setValue("settings-timetables-teachers-time-hv-print-teachers-names", SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_NAMES);
	settings.setValue("settings-timetables-teachers-time-hv-print-teachers-long-names", SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_LONG_NAMES);
	settings.setValue("settings-timetables-teachers-time-hv-print-teachers-codes", SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_CODES);

	settings.setValue("settings-timetables-teachers-time-hv-print-students-names", SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_NAMES);
	settings.setValue("settings-timetables-teachers-time-hv-print-students-long-names", SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_LONG_NAMES);
	settings.setValue("settings-timetables-teachers-time-hv-print-students-codes", SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_CODES);

	settings.setValue("settings-timetables-teachers-time-hv-print-rooms-names", SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_NAMES);
	settings.setValue("settings-timetables-teachers-time-hv-print-rooms-long-names", SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_LONG_NAMES);
	settings.setValue("settings-timetables-teachers-time-hv-print-rooms-codes", SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_CODES);

	settings.setValue("settings-timetables-teachers-time-hv-print-subjects-legend", SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_LEGEND);
	settings.setValue("settings-timetables-teachers-time-hv-print-activity-tags-legend", SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND);
	settings.setValue("settings-timetables-teachers-time-hv-print-teachers-legend", SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_LEGEND);
	settings.setValue("settings-timetables-teachers-time-hv-print-students-legend", SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_LEGEND);
	settings.setValue("settings-timetables-teachers-time-hv-print-rooms-legend", SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_LEGEND);
	settings.setValue("settings-timetables-teachers-time-hv-print-legend-codes-first", SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_LEGEND_CODES_FIRST);
	/////

	/////rooms days horizontal and days vertical.
	settings.setValue("settings-timetables-rooms-days-hv-print-days-names", SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_DAYS_NAMES);
	settings.setValue("settings-timetables-rooms-days-hv-print-days-long-names", SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_DAYS_LONG_NAMES);

	settings.setValue("settings-timetables-rooms-days-hv-print-hours-names", SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_HOURS_NAMES);
	settings.setValue("settings-timetables-rooms-days-hv-print-hours-long-names", SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_HOURS_LONG_NAMES);

	settings.setValue("settings-timetables-rooms-days-hv-print-subjects-names", SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_NAMES);
	settings.setValue("settings-timetables-rooms-days-hv-print-subjects-long-names", SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES);
	settings.setValue("settings-timetables-rooms-days-hv-print-subjects-codes", SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_CODES);

	settings.setValue("settings-timetables-rooms-days-hv-print-activity-tags-names", SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES);
	settings.setValue("settings-timetables-rooms-days-hv-print-activity-tags-long-names", SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES);
	settings.setValue("settings-timetables-rooms-days-hv-print-activity-tags-codes", SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES);

	settings.setValue("settings-timetables-rooms-days-hv-print-teachers-names", SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_NAMES);
	settings.setValue("settings-timetables-rooms-days-hv-print-teachers-long-names", SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES);
	settings.setValue("settings-timetables-rooms-days-hv-print-teachers-codes", SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_CODES);

	settings.setValue("settings-timetables-rooms-days-hv-print-students-names", SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_NAMES);
	settings.setValue("settings-timetables-rooms-days-hv-print-students-long-names", SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES);
	settings.setValue("settings-timetables-rooms-days-hv-print-students-codes", SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_CODES);

	settings.setValue("settings-timetables-rooms-days-hv-print-rooms-names", SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_NAMES);
	settings.setValue("settings-timetables-rooms-days-hv-print-rooms-long-names", SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_LONG_NAMES);
	settings.setValue("settings-timetables-rooms-days-hv-print-rooms-codes", SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_CODES);

	settings.setValue("settings-timetables-rooms-days-hv-print-subjects-legend", SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_LEGEND);
	settings.setValue("settings-timetables-rooms-days-hv-print-activity-tags-legend", SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND);
	settings.setValue("settings-timetables-rooms-days-hv-print-teachers-legend", SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_LEGEND);
	settings.setValue("settings-timetables-rooms-days-hv-print-students-legend", SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_LEGEND);
	settings.setValue("settings-timetables-rooms-days-hv-print-rooms-legend", SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_LEGEND);
	settings.setValue("settings-timetables-rooms-days-hv-print-legend-codes-first", SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_LEGEND_CODES_FIRST);
	/////

	/////rooms time horizontal and time vertical.
	settings.setValue("settings-timetables-rooms-time-hv-print-days-names", SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_NAMES);
	settings.setValue("settings-timetables-rooms-time-hv-print-days-long-names", SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_LONG_NAMES);

	settings.setValue("settings-timetables-rooms-time-hv-print-hours-names", SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_NAMES);
	settings.setValue("settings-timetables-rooms-time-hv-print-hours-long-names", SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_LONG_NAMES);

	settings.setValue("settings-timetables-rooms-time-hv-print-subjects-names", SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_NAMES);
	settings.setValue("settings-timetables-rooms-time-hv-print-subjects-long-names", SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES);
	settings.setValue("settings-timetables-rooms-time-hv-print-subjects-codes", SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_CODES);

	settings.setValue("settings-timetables-rooms-time-hv-print-activity-tags-names", SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES);
	settings.setValue("settings-timetables-rooms-time-hv-print-activity-tags-long-names", SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES);
	settings.setValue("settings-timetables-rooms-time-hv-print-activity-tags-codes", SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES);

	settings.setValue("settings-timetables-rooms-time-hv-print-teachers-names", SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_NAMES);
	settings.setValue("settings-timetables-rooms-time-hv-print-teachers-long-names", SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_LONG_NAMES);
	settings.setValue("settings-timetables-rooms-time-hv-print-teachers-codes", SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_CODES);

	settings.setValue("settings-timetables-rooms-time-hv-print-students-names", SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_NAMES);
	settings.setValue("settings-timetables-rooms-time-hv-print-students-long-names", SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_LONG_NAMES);
	settings.setValue("settings-timetables-rooms-time-hv-print-students-codes", SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_CODES);

	settings.setValue("settings-timetables-rooms-time-hv-print-rooms-names", SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_NAMES);
	settings.setValue("settings-timetables-rooms-time-hv-print-rooms-long-names", SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_LONG_NAMES);
	settings.setValue("settings-timetables-rooms-time-hv-print-rooms-codes", SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_CODES);

	settings.setValue("settings-timetables-rooms-time-hv-print-subjects-legend", SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_LEGEND);
	settings.setValue("settings-timetables-rooms-time-hv-print-activity-tags-legend", SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND);
	settings.setValue("settings-timetables-rooms-time-hv-print-teachers-legend", SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_LEGEND);
	settings.setValue("settings-timetables-rooms-time-hv-print-students-legend", SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_LEGEND);
	settings.setValue("settings-timetables-rooms-time-hv-print-rooms-legend", SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_LEGEND);
	settings.setValue("settings-timetables-rooms-time-hv-print-legend-codes-first", SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_LEGEND_CODES_FIRST);
	/////

	/////buildings days horizontal and days vertical.
	settings.setValue("settings-timetables-buildings-days-hv-print-days-names", SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_DAYS_NAMES);
	settings.setValue("settings-timetables-buildings-days-hv-print-days-long-names", SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_DAYS_LONG_NAMES);

	settings.setValue("settings-timetables-buildings-days-hv-print-hours-names", SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_HOURS_NAMES);
	settings.setValue("settings-timetables-buildings-days-hv-print-hours-long-names", SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_HOURS_LONG_NAMES);

	settings.setValue("settings-timetables-buildings-days-hv-print-subjects-names", SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_NAMES);
	settings.setValue("settings-timetables-buildings-days-hv-print-subjects-long-names", SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES);
	settings.setValue("settings-timetables-buildings-days-hv-print-subjects-codes", SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_CODES);

	settings.setValue("settings-timetables-buildings-days-hv-print-activity-tags-names", SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES);
	settings.setValue("settings-timetables-buildings-days-hv-print-activity-tags-long-names", SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES);
	settings.setValue("settings-timetables-buildings-days-hv-print-activity-tags-codes", SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES);

	settings.setValue("settings-timetables-buildings-days-hv-print-teachers-names", SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_NAMES);
	settings.setValue("settings-timetables-buildings-days-hv-print-teachers-long-names", SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES);
	settings.setValue("settings-timetables-buildings-days-hv-print-teachers-codes", SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_CODES);

	settings.setValue("settings-timetables-buildings-days-hv-print-students-names", SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_NAMES);
	settings.setValue("settings-timetables-buildings-days-hv-print-students-long-names", SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES);
	settings.setValue("settings-timetables-buildings-days-hv-print-students-codes", SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_CODES);

	settings.setValue("settings-timetables-buildings-days-hv-print-rooms-names", SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_NAMES);
	settings.setValue("settings-timetables-buildings-days-hv-print-rooms-long-names", SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_LONG_NAMES);
	settings.setValue("settings-timetables-buildings-days-hv-print-rooms-codes", SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_CODES);

	settings.setValue("settings-timetables-buildings-days-hv-print-subjects-legend", SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_LEGEND);
	settings.setValue("settings-timetables-buildings-days-hv-print-activity-tags-legend", SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND);
	settings.setValue("settings-timetables-buildings-days-hv-print-teachers-legend", SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_LEGEND);
	settings.setValue("settings-timetables-buildings-days-hv-print-students-legend", SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_LEGEND);
	settings.setValue("settings-timetables-buildings-days-hv-print-rooms-legend", SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_LEGEND);
	settings.setValue("settings-timetables-buildings-days-hv-print-legend-codes-first", SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_LEGEND_CODES_FIRST);
	/////

	/////buildings time horizontal and time vertical.
	settings.setValue("settings-timetables-buildings-time-hv-print-days-names", SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_NAMES);
	settings.setValue("settings-timetables-buildings-time-hv-print-days-long-names", SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_LONG_NAMES);

	settings.setValue("settings-timetables-buildings-time-hv-print-hours-names", SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_NAMES);
	settings.setValue("settings-timetables-buildings-time-hv-print-hours-long-names", SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_LONG_NAMES);

	settings.setValue("settings-timetables-buildings-time-hv-print-subjects-names", SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_NAMES);
	settings.setValue("settings-timetables-buildings-time-hv-print-subjects-long-names", SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES);
	settings.setValue("settings-timetables-buildings-time-hv-print-subjects-codes", SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_CODES);

	settings.setValue("settings-timetables-buildings-time-hv-print-activity-tags-names", SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES);
	settings.setValue("settings-timetables-buildings-time-hv-print-activity-tags-long-names", SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES);
	settings.setValue("settings-timetables-buildings-time-hv-print-activity-tags-codes", SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES);

	settings.setValue("settings-timetables-buildings-time-hv-print-teachers-names", SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_NAMES);
	settings.setValue("settings-timetables-buildings-time-hv-print-teachers-long-names", SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_LONG_NAMES);
	settings.setValue("settings-timetables-buildings-time-hv-print-teachers-codes", SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_CODES);

	settings.setValue("settings-timetables-buildings-time-hv-print-students-names", SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_NAMES);
	settings.setValue("settings-timetables-buildings-time-hv-print-students-long-names", SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_LONG_NAMES);
	settings.setValue("settings-timetables-buildings-time-hv-print-students-codes", SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_CODES);

	settings.setValue("settings-timetables-buildings-time-hv-print-rooms-names", SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_NAMES);
	settings.setValue("settings-timetables-buildings-time-hv-print-rooms-long-names", SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_LONG_NAMES);
	settings.setValue("settings-timetables-buildings-time-hv-print-rooms-codes", SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_CODES);

	settings.setValue("settings-timetables-buildings-time-hv-print-subjects-legend", SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_LEGEND);
	settings.setValue("settings-timetables-buildings-time-hv-print-activity-tags-legend", SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND);
	settings.setValue("settings-timetables-buildings-time-hv-print-teachers-legend", SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_LEGEND);
	settings.setValue("settings-timetables-buildings-time-hv-print-students-legend", SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_LEGEND);
	settings.setValue("settings-timetables-buildings-time-hv-print-rooms-legend", SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_LEGEND);
	settings.setValue("settings-timetables-buildings-time-hv-print-legend-codes-first", SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_LEGEND_CODES_FIRST);
	/////

	/////subjects days horizontal and days vertical.
	settings.setValue("settings-timetables-subjects-days-hv-print-days-names", SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_DAYS_NAMES);
	settings.setValue("settings-timetables-subjects-days-hv-print-days-long-names", SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_DAYS_LONG_NAMES);

	settings.setValue("settings-timetables-subjects-days-hv-print-hours-names", SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_HOURS_NAMES);
	settings.setValue("settings-timetables-subjects-days-hv-print-hours-long-names", SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_HOURS_LONG_NAMES);

	settings.setValue("settings-timetables-subjects-days-hv-print-subjects-names", SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_NAMES);
	settings.setValue("settings-timetables-subjects-days-hv-print-subjects-long-names", SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES);
	settings.setValue("settings-timetables-subjects-days-hv-print-subjects-codes", SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_CODES);

	settings.setValue("settings-timetables-subjects-days-hv-print-activity-tags-names", SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES);
	settings.setValue("settings-timetables-subjects-days-hv-print-activity-tags-long-names", SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES);
	settings.setValue("settings-timetables-subjects-days-hv-print-activity-tags-codes", SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES);

	settings.setValue("settings-timetables-subjects-days-hv-print-teachers-names", SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_TEACHERS_NAMES);
	settings.setValue("settings-timetables-subjects-days-hv-print-teachers-long-names", SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES);
	settings.setValue("settings-timetables-subjects-days-hv-print-teachers-codes", SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_TEACHERS_CODES);

	settings.setValue("settings-timetables-subjects-days-hv-print-students-names", SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_STUDENTS_NAMES);
	settings.setValue("settings-timetables-subjects-days-hv-print-students-long-names", SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES);
	settings.setValue("settings-timetables-subjects-days-hv-print-students-codes", SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_STUDENTS_CODES);

	settings.setValue("settings-timetables-subjects-days-hv-print-rooms-names", SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ROOMS_NAMES);
	settings.setValue("settings-timetables-subjects-days-hv-print-rooms-long-names", SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ROOMS_LONG_NAMES);
	settings.setValue("settings-timetables-subjects-days-hv-print-rooms-codes", SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ROOMS_CODES);

	settings.setValue("settings-timetables-subjects-days-hv-print-subjects-legend", SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_LEGEND);
	settings.setValue("settings-timetables-subjects-days-hv-print-activity-tags-legend", SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND);
	settings.setValue("settings-timetables-subjects-days-hv-print-teachers-legend", SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_TEACHERS_LEGEND);
	settings.setValue("settings-timetables-subjects-days-hv-print-students-legend", SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_STUDENTS_LEGEND);
	settings.setValue("settings-timetables-subjects-days-hv-print-rooms-legend", SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ROOMS_LEGEND);
	settings.setValue("settings-timetables-subjects-days-hv-print-legend-codes-first", SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_LEGEND_CODES_FIRST);
	/////

	/////subjects time horizontal and time vertical.
	settings.setValue("settings-timetables-subjects-time-hv-print-days-names", SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_NAMES);
	settings.setValue("settings-timetables-subjects-time-hv-print-days-long-names", SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_LONG_NAMES);

	settings.setValue("settings-timetables-subjects-time-hv-print-hours-names", SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_NAMES);
	settings.setValue("settings-timetables-subjects-time-hv-print-hours-long-names", SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_LONG_NAMES);

	settings.setValue("settings-timetables-subjects-time-hv-print-subjects-names", SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_NAMES);
	settings.setValue("settings-timetables-subjects-time-hv-print-subjects-long-names", SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES);
	settings.setValue("settings-timetables-subjects-time-hv-print-subjects-codes", SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_CODES);

	settings.setValue("settings-timetables-subjects-time-hv-print-activity-tags-names", SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES);
	settings.setValue("settings-timetables-subjects-time-hv-print-activity-tags-long-names", SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES);
	settings.setValue("settings-timetables-subjects-time-hv-print-activity-tags-codes", SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES);

	settings.setValue("settings-timetables-subjects-time-hv-print-teachers-names", SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_NAMES);
	settings.setValue("settings-timetables-subjects-time-hv-print-teachers-long-names", SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_LONG_NAMES);
	settings.setValue("settings-timetables-subjects-time-hv-print-teachers-codes", SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_CODES);

	settings.setValue("settings-timetables-subjects-time-hv-print-students-names", SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_NAMES);
	settings.setValue("settings-timetables-subjects-time-hv-print-students-long-names", SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_LONG_NAMES);
	settings.setValue("settings-timetables-subjects-time-hv-print-students-codes", SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_CODES);

	settings.setValue("settings-timetables-subjects-time-hv-print-rooms-names", SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_NAMES);
	settings.setValue("settings-timetables-subjects-time-hv-print-rooms-long-names", SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_LONG_NAMES);
	settings.setValue("settings-timetables-subjects-time-hv-print-rooms-codes", SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_CODES);

	settings.setValue("settings-timetables-subjects-time-hv-print-subjects-legend", SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_LEGEND);
	settings.setValue("settings-timetables-subjects-time-hv-print-activity-tags-legend", SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND);
	settings.setValue("settings-timetables-subjects-time-hv-print-teachers-legend", SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_LEGEND);
	settings.setValue("settings-timetables-subjects-time-hv-print-students-legend", SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_LEGEND);
	settings.setValue("settings-timetables-subjects-time-hv-print-rooms-legend", SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_LEGEND);
	settings.setValue("settings-timetables-subjects-time-hv-print-legend-codes-first", SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_LEGEND_CODES_FIRST);
	/////

	/////activity tags days horizontal and days vertical.
	settings.setValue("settings-timetables-activity-tags-days-hv-print-days-names", SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_DAYS_NAMES);
	settings.setValue("settings-timetables-activity-tags-days-hv-print-days-long-names", SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_DAYS_LONG_NAMES);

	settings.setValue("settings-timetables-activity-tags-days-hv-print-hours-names", SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_HOURS_NAMES);
	settings.setValue("settings-timetables-activity-tags-days-hv-print-hours-long-names", SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_HOURS_LONG_NAMES);

	settings.setValue("settings-timetables-activity-tags-days-hv-print-subjects-names", SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_SUBJECTS_NAMES);
	settings.setValue("settings-timetables-activity-tags-days-hv-print-subjects-long-names", SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES);
	settings.setValue("settings-timetables-activity-tags-days-hv-print-subjects-codes", SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_SUBJECTS_CODES);

	settings.setValue("settings-timetables-activity-tags-days-hv-print-activity-tags-names", SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES);
	settings.setValue("settings-timetables-activity-tags-days-hv-print-activity-tags-long-names", SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES);
	settings.setValue("settings-timetables-activity-tags-days-hv-print-activity-tags-codes", SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES);

	settings.setValue("settings-timetables-activity-tags-days-hv-print-teachers-names", SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_TEACHERS_NAMES);
	settings.setValue("settings-timetables-activity-tags-days-hv-print-teachers-long-names", SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES);
	settings.setValue("settings-timetables-activity-tags-days-hv-print-teachers-codes", SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_TEACHERS_CODES);

	settings.setValue("settings-timetables-activity-tags-days-hv-print-students-names", SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_STUDENTS_NAMES);
	settings.setValue("settings-timetables-activity-tags-days-hv-print-students-long-names", SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES);
	settings.setValue("settings-timetables-activity-tags-days-hv-print-students-codes", SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_STUDENTS_CODES);

	settings.setValue("settings-timetables-activity-tags-days-hv-print-rooms-names", SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ROOMS_NAMES);
	settings.setValue("settings-timetables-activity-tags-days-hv-print-rooms-long-names", SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ROOMS_LONG_NAMES);
	settings.setValue("settings-timetables-activity-tags-days-hv-print-rooms-codes", SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ROOMS_CODES);

	settings.setValue("settings-timetables-activity-tags-days-hv-print-subjects-legend", SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_SUBJECTS_LEGEND);
	settings.setValue("settings-timetables-activity-tags-days-hv-print-activity-tags-legend", SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND);
	settings.setValue("settings-timetables-activity-tags-days-hv-print-teachers-legend", SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_TEACHERS_LEGEND);
	settings.setValue("settings-timetables-activity-tags-days-hv-print-students-legend", SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_STUDENTS_LEGEND);
	settings.setValue("settings-timetables-activity-tags-days-hv-print-rooms-legend", SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ROOMS_LEGEND);
	settings.setValue("settings-timetables-activity-tags-days-hv-print-legend-codes-first", SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_LEGEND_CODES_FIRST);
	/////

	/////activity tags time horizontal and time vertical.
	settings.setValue("settings-timetables-activity-tags-time-hv-print-days-names", SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_NAMES);
	settings.setValue("settings-timetables-activity-tags-time-hv-print-days-long-names", SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_LONG_NAMES);

	settings.setValue("settings-timetables-activity-tags-time-hv-print-hours-names", SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_NAMES);
	settings.setValue("settings-timetables-activity-tags-time-hv-print-hours-long-names", SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_LONG_NAMES);

	settings.setValue("settings-timetables-activity-tags-time-hv-print-subjects-names", SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_NAMES);
	settings.setValue("settings-timetables-activity-tags-time-hv-print-subjects-long-names", SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES);
	settings.setValue("settings-timetables-activity-tags-time-hv-print-subjects-codes", SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_CODES);

	settings.setValue("settings-timetables-activity-tags-time-hv-print-activity-tags-names", SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES);
	settings.setValue("settings-timetables-activity-tags-time-hv-print-activity-tags-long-names", SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES);
	settings.setValue("settings-timetables-activity-tags-time-hv-print-activity-tags-codes", SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES);

	settings.setValue("settings-timetables-activity-tags-time-hv-print-teachers-names", SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_NAMES);
	settings.setValue("settings-timetables-activity-tags-time-hv-print-teachers-long-names", SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_LONG_NAMES);
	settings.setValue("settings-timetables-activity-tags-time-hv-print-teachers-codes", SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_CODES);

	settings.setValue("settings-timetables-activity-tags-time-hv-print-students-names", SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_NAMES);
	settings.setValue("settings-timetables-activity-tags-time-hv-print-students-long-names", SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_LONG_NAMES);
	settings.setValue("settings-timetables-activity-tags-time-hv-print-students-codes", SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_CODES);

	settings.setValue("settings-timetables-activity-tags-time-hv-print-rooms-names", SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_NAMES);
	settings.setValue("settings-timetables-activity-tags-time-hv-print-rooms-long-names", SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_LONG_NAMES);
	settings.setValue("settings-timetables-activity-tags-time-hv-print-rooms-codes", SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_CODES);

	settings.setValue("settings-timetables-activity-tags-time-hv-print-subjects-legend", SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_LEGEND);
	settings.setValue("settings-timetables-activity-tags-time-hv-print-activity-tags-legend", SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND);
	settings.setValue("settings-timetables-activity-tags-time-hv-print-teachers-legend", SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_LEGEND);
	settings.setValue("settings-timetables-activity-tags-time-hv-print-students-legend", SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_LEGEND);
	settings.setValue("settings-timetables-activity-tags-time-hv-print-rooms-legend", SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_LEGEND);
	settings.setValue("settings-timetables-activity-tags-time-hv-print-legend-codes-first", SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_LEGEND_CODES_FIRST);
	/////

	/////activities days horizontal and days vertical.
	settings.setValue("settings-timetables-activities-days-hv-print-days-names", SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_DAYS_NAMES);
	settings.setValue("settings-timetables-activities-days-hv-print-days-long-names", SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_DAYS_LONG_NAMES);

	settings.setValue("settings-timetables-activities-days-hv-print-hours-names", SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_HOURS_NAMES);
	settings.setValue("settings-timetables-activities-days-hv-print-hours-long-names", SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_HOURS_LONG_NAMES);

	settings.setValue("settings-timetables-activities-days-hv-print-subjects-names", SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_SUBJECTS_NAMES);
	settings.setValue("settings-timetables-activities-days-hv-print-subjects-long-names", SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES);
	settings.setValue("settings-timetables-activities-days-hv-print-subjects-codes", SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_SUBJECTS_CODES);

	settings.setValue("settings-timetables-activities-days-hv-print-activity-tags-names", SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES);
	settings.setValue("settings-timetables-activities-days-hv-print-activity-tags-long-names", SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES);
	settings.setValue("settings-timetables-activities-days-hv-print-activity-tags-codes", SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES);

	settings.setValue("settings-timetables-activities-days-hv-print-teachers-names", SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_TEACHERS_NAMES);
	settings.setValue("settings-timetables-activities-days-hv-print-teachers-long-names", SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_TEACHERS_LONG_NAMES);
	settings.setValue("settings-timetables-activities-days-hv-print-teachers-codes", SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_TEACHERS_CODES);

	settings.setValue("settings-timetables-activities-days-hv-print-students-names", SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_STUDENTS_NAMES);
	settings.setValue("settings-timetables-activities-days-hv-print-students-long-names", SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_STUDENTS_LONG_NAMES);
	settings.setValue("settings-timetables-activities-days-hv-print-students-codes", SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_STUDENTS_CODES);

	settings.setValue("settings-timetables-activities-days-hv-print-rooms-names", SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ROOMS_NAMES);
	settings.setValue("settings-timetables-activities-days-hv-print-rooms-long-names", SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ROOMS_LONG_NAMES);
	settings.setValue("settings-timetables-activities-days-hv-print-rooms-codes", SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ROOMS_CODES);

	settings.setValue("settings-timetables-activities-days-hv-print-subjects-legend", SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_SUBJECTS_LEGEND);
	settings.setValue("settings-timetables-activities-days-hv-print-activity-tags-legend", SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND);
	settings.setValue("settings-timetables-activities-days-hv-print-teachers-legend", SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_TEACHERS_LEGEND);
	settings.setValue("settings-timetables-activities-days-hv-print-students-legend", SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_STUDENTS_LEGEND);
	settings.setValue("settings-timetables-activities-days-hv-print-rooms-legend", SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ROOMS_LEGEND);
	settings.setValue("settings-timetables-activities-days-hv-print-legend-codes-first", SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_LEGEND_CODES_FIRST);
	/////

	/////activities time horizontal and time vertical.
	settings.setValue("settings-timetables-activities-time-hv-print-days-names", SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_NAMES);
	settings.setValue("settings-timetables-activities-time-hv-print-days-long-names", SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_LONG_NAMES);

	settings.setValue("settings-timetables-activities-time-hv-print-hours-names", SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_NAMES);
	settings.setValue("settings-timetables-activities-time-hv-print-hours-long-names", SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_LONG_NAMES);

	settings.setValue("settings-timetables-activities-time-hv-print-subjects-names", SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_NAMES);
	settings.setValue("settings-timetables-activities-time-hv-print-subjects-long-names", SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_LONG_NAMES);
	settings.setValue("settings-timetables-activities-time-hv-print-subjects-codes", SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_CODES);

	settings.setValue("settings-timetables-activities-time-hv-print-activity-tags-names", SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES);
	settings.setValue("settings-timetables-activities-time-hv-print-activity-tags-long-names", SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES);
	settings.setValue("settings-timetables-activities-time-hv-print-activity-tags-codes", SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_CODES);

	settings.setValue("settings-timetables-activities-time-hv-print-teachers-names", SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_NAMES);
	settings.setValue("settings-timetables-activities-time-hv-print-teachers-long-names", SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_LONG_NAMES);
	settings.setValue("settings-timetables-activities-time-hv-print-teachers-codes", SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_CODES);

	settings.setValue("settings-timetables-activities-time-hv-print-students-names", SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_NAMES);
	settings.setValue("settings-timetables-activities-time-hv-print-students-long-names", SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_LONG_NAMES);
	settings.setValue("settings-timetables-activities-time-hv-print-students-codes", SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_CODES);

	settings.setValue("settings-timetables-activities-time-hv-print-rooms-names", SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_NAMES);
	settings.setValue("settings-timetables-activities-time-hv-print-rooms-long-names", SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_LONG_NAMES);
	settings.setValue("settings-timetables-activities-time-hv-print-rooms-codes", SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_CODES);

	settings.setValue("settings-timetables-activities-time-hv-print-subjects-legend", SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_LEGEND);
	settings.setValue("settings-timetables-activities-time-hv-print-activity-tags-legend", SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND);
	settings.setValue("settings-timetables-activities-time-hv-print-teachers-legend", SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_LEGEND);
	settings.setValue("settings-timetables-activities-time-hv-print-students-legend", SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_LEGEND);
	settings.setValue("settings-timetables-activities-time-hv-print-rooms-legend", SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_LEGEND);
	settings.setValue("settings-timetables-activities-time-hv-print-legend-codes-first", SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_LEGEND_CODES_FIRST);
	/////
	//2024-06-12 end

	//main form
	settings.setValue("FetMainForm/geometry", mainFormSettingsRect);
	//settings.setValue("FetMainForm/shortcuts-tab-position", MAIN_FORM_SHORTCUTS_TAB_POSITION);
	//settings.setValue("FetMainForm/shortcuts-tab-position", 0); //always starting on the first page, as suggested by a user
	settings.setValue("FetMainForm/show-shortcut-buttons", SHOW_SHORTCUTS_ON_MAIN_WINDOW);

	settings.setValue("show-tooltips-for-constraints-with-tables", SHOW_TOOLTIPS_FOR_CONSTRAINTS_WITH_TABLES);

	settings.setValue("beep-at-the-end-of-generation", BEEP_AT_END_OF_GENERATION);
	settings.setValue("enable-command-at-the-end-of-generation", ENABLE_COMMAND_AT_END_OF_GENERATION);
	settings.setValue("command-at-the-end-of-generation", commandAtEndOfGeneration);

	settings.setValue("beep-at-the-end-of-generation-each-timetable", BEEP_AT_END_OF_GENERATION_EACH_TIMETABLE);
	settings.setValue("enable-command-at-the-end-of-generation-each-timetable", ENABLE_COMMAND_AT_END_OF_GENERATION_EACH_TIMETABLE);
	settings.setValue("command-at-the-end-of-generation-each-timetable", commandAtEndOfGenerationEachTimetable);

//	settings.setValue("detached-notification", DETACHED_NOTIFICATION);
//	settings.setValue("terminate-command-at-the-end-of-generation-after-seconds", terminateCommandAfterSeconds);
//	settings.setValue("kill-command-at-the-end-of-generation-after-seconds", killCommandAfterSeconds);
	
	if(VERBOSE){
		std::cout<<"Settings saved"<<std::endl;
	}
	
	pFetMainForm=nullptr;
}

void initLanguagesSet()
{
	//This is one of the two places to insert a new language in the sources (the other one is in fetmainform.cpp).
	//(Also, don't forget about the README file!)
	languagesSet.clear();

	languagesSet.insert("en_US");
	languagesSet.insert("en_GB");

	languagesSet.insert("ar");
	languagesSet.insert("ca");
	languagesSet.insert("de");
	languagesSet.insert("el");
	languagesSet.insert("es");
	languagesSet.insert("fr");
	languagesSet.insert("hu");
	languagesSet.insert("id");
	languagesSet.insert("it");
	languagesSet.insert("lt");
	languagesSet.insert("mk");
	languagesSet.insert("ms");
	languagesSet.insert("nl");
	languagesSet.insert("pl");
	languagesSet.insert("ro");
	languagesSet.insert("tr");
	languagesSet.insert("ru");
	languagesSet.insert("fa");
	languagesSet.insert("uk");
	languagesSet.insert("pt_BR");
	languagesSet.insert("da");
	languagesSet.insert("si");
	languagesSet.insert("sk");
	languagesSet.insert("he");
	languagesSet.insert("sr");
	languagesSet.insert("gl");
	languagesSet.insert("vi");
	languagesSet.insert("uz");
	languagesSet.insert("sq");
	languagesSet.insert("zh_CN");
	languagesSet.insert("zh_TW");
	languagesSet.insert("eu");
	languagesSet.insert("cs");
	languagesSet.insert("ja");
	languagesSet.insert("bg");
	languagesSet.insert("bs");
}

void setLanguage(QApplication& qapplication, QWidget* parent)
{
	Q_UNUSED(qapplication); //silence wrong MSVC warning

	static int cntTranslators=0;
	
	if(cntTranslators>0){
		qapplication.removeTranslator(&translator);
		cntTranslators=0;
	}

	//Translator stuff. The trick to use a relative path, so that the translations work with the fet executable
	//installed in /usr/bin or /usr/local/bin (and the translations in /usr/share/fet/translations or
	//respectively in /usr/local/share/fet/translations was suggested by Michael Towers on 2026-01-09
	QDir d(qapplication.applicationDirPath()+"/../share/fet/translations");
	
	bool translation_loaded=false;
	
	if(FET_LANGUAGE!="en_US" && languagesSet.contains(FET_LANGUAGE)){
		translation_loaded=translator.load("fet_"+FET_LANGUAGE, qapplication.applicationDirPath());
		if(!translation_loaded){
			translation_loaded=translator.load("fet_"+FET_LANGUAGE, qapplication.applicationDirPath()+"/translations");
			if(!translation_loaded){
				if(d.exists()){
					translation_loaded=translator.load("fet_"+FET_LANGUAGE, qapplication.applicationDirPath()+"/../share/fet/translations");
				}
			}
		}
	}
	else{
		if(FET_LANGUAGE!="en_US"){
			FetMessage::warning(parent, QString("FET warning"),
			 QString("Specified language is incorrect - making it en_US (US English)"));
			FET_LANGUAGE="en_US";
		}
		
		assert(FET_LANGUAGE=="en_US");
		
		translation_loaded=true;
	}
	
	if(!translation_loaded){
		FetMessage::warning(parent, QString("FET warning"),
		 QString("Translation for specified language not loaded - maybe the translation file is missing - setting the language to en_US (US English)")
		 +"\n\n"+
		 QString("FET searched for the translation file %1 in the directory %2, then in the directory %3 and "
		 "then in the directory %4 (under systems that support such a directory), but could not find it.")
		 .arg("fet_"+FET_LANGUAGE+".qm")
		 .arg(QDir::toNativeSeparators(qapplication.applicationDirPath()))
		 .arg(QDir::toNativeSeparators(qapplication.applicationDirPath()+"/translations"))
		 .arg(QDir::toNativeSeparators(qapplication.applicationDirPath()+"/../share/fet/translations"))
		 );
		FET_LANGUAGE="en_US";
	}
	
	if(FET_LANGUAGE=="ar")
		FET_LANGUAGE_WITH_LOCALE="ar_DZ";
	else
		FET_LANGUAGE_WITH_LOCALE=FET_LANGUAGE;
	
	if(FET_LANGUAGE=="ar" || FET_LANGUAGE=="he" || FET_LANGUAGE=="fa" || FET_LANGUAGE=="ur" /* and others? */)
		LANGUAGE_STYLE_RIGHT_TO_LEFT=true;
	else
		LANGUAGE_STYLE_RIGHT_TO_LEFT=false;
	
	if(FET_LANGUAGE=="zh_CN"){
		LANGUAGE_FOR_HTML="zh-Hans";
	}
	else if(FET_LANGUAGE=="zh_TW"){
		LANGUAGE_FOR_HTML="zh-Hant";
	}
	else{
		LANGUAGE_FOR_HTML=FET_LANGUAGE;
		LANGUAGE_FOR_HTML.replace(QString("_"), QString("-"));
	}
	
	assert(cntTranslators==0);
	if(FET_LANGUAGE!="en_US"){
		qapplication.installTranslator(&translator);
		cntTranslators=1;
	}
	
	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==true)
		qapplication.setLayoutDirection(Qt::RightToLeft);
	else
		qapplication.setLayoutDirection(Qt::LeftToRight);
	
	//retranslate
	//QList<QWidget*> tlwl=qapplication.topLevelWidgets();
	QWidgetList tlwl=QApplication::topLevelWidgets();

	for(QWidget* wi : std::as_const(tlwl))
		if(true){
		//if(wi->isVisible()){
			FetMainForm* mainform=qobject_cast<FetMainForm*>(wi);
			if(mainform!=nullptr){
				mainform->retranslateUi(mainform);
				mainform->retranslateConstraints();
				mainform->retranslateMode();
				continue;
			}

			//help block-planning
			HelpBlockPlanningForm* hbp=qobject_cast<HelpBlockPlanningForm*>(wi);
			if(hbp!=nullptr){
				hbp->retranslateUi(hbp);
				continue;
			}
			//help terms
			HelpTermsForm* ht=qobject_cast<HelpTermsForm*>(wi);
			if(ht!=nullptr){
				ht->retranslateUi(ht);
				continue;
			}
			//help Morocco
			HelpMoroccoForm* hm=qobject_cast<HelpMoroccoForm*>(wi);
			if(hm!=nullptr){
				hm->retranslateUi(hm);
				continue;
			}
			//help Algeria
			HelpAlgeriaForm* ha=qobject_cast<HelpAlgeriaForm*>(wi);
			if(ha!=nullptr){
				ha->retranslateUi(ha);
				continue;
			}

			//help
			HelpAboutForm* aboutf=qobject_cast<HelpAboutForm*>(wi);
			if(aboutf!=nullptr){
				aboutf->retranslateUi(aboutf);
				continue;
			}

			HelpAboutLibrariesForm* aboutlibsf=qobject_cast<HelpAboutLibrariesForm*>(wi);
			if(aboutlibsf!=nullptr){
				aboutlibsf->retranslateUi(aboutlibsf);
				continue;
			}

			HelpFaqForm* faqf=qobject_cast<HelpFaqForm*>(wi);
			if(faqf!=nullptr){
				faqf->retranslateUi(faqf);
				faqf->setText();
				continue;
			}

			HelpTipsForm* tipsf=qobject_cast<HelpTipsForm*>(wi);
			if(tipsf!=nullptr){
				tipsf->retranslateUi(tipsf);
				tipsf->setText();
				continue;
			}

			HelpInstructionsForm* instrf=qobject_cast<HelpInstructionsForm*>(wi);
			if(instrf!=nullptr){
				instrf->retranslateUi(instrf);
				instrf->setText();
				continue;
			}
			//////
			
			//timetable
			TimetableViewStudentsDaysHorizontalForm* vsdhf=qobject_cast<TimetableViewStudentsDaysHorizontalForm*>(wi);
			if(vsdhf!=nullptr){
				vsdhf->retranslateUi(vsdhf);
				vsdhf->updateStudentsTimetableTable();
				continue;
			}

			TimetableViewStudentsDaysVerticalForm* vsdvf=qobject_cast<TimetableViewStudentsDaysVerticalForm*>(wi);
			if(vsdvf!=nullptr){
				vsdvf->retranslateUi(vsdvf);
				vsdvf->updateStudentsTimetableTable();
				continue;
			}

			TimetableViewStudentsTimeHorizontalForm* vsthf=qobject_cast<TimetableViewStudentsTimeHorizontalForm*>(wi);
			if(vsthf!=nullptr){
				vsthf->retranslateUi(vsthf);
				vsthf->updateStudentsTimetableTable();
				continue;
			}

			TimetableViewTeachersDaysHorizontalForm* vtchdhf=qobject_cast<TimetableViewTeachersDaysHorizontalForm*>(wi);
			if(vtchdhf!=nullptr){
				vtchdhf->retranslateUi(vtchdhf);
				vtchdhf->updateTeachersTimetableTable();
				continue;
			}

			TimetableViewTeachersDaysVerticalForm* vtchdvf=qobject_cast<TimetableViewTeachersDaysVerticalForm*>(wi);
			if(vtchdvf!=nullptr){
				vtchdvf->retranslateUi(vtchdvf);
				vtchdvf->updateTeachersTimetableTable();
				continue;
			}

			TimetableViewTeachersTimeHorizontalForm* vtchthf=qobject_cast<TimetableViewTeachersTimeHorizontalForm*>(wi);
			if(vtchthf!=nullptr){
				vtchthf->retranslateUi(vtchthf);
				vtchthf->updateTeachersTimetableTable();
				continue;
			}

			TimetableViewRoomsDaysHorizontalForm* vrdhf=qobject_cast<TimetableViewRoomsDaysHorizontalForm*>(wi);
			if(vrdhf!=nullptr){
				vrdhf->retranslateUi(vrdhf);
				vrdhf->updateRoomsTimetableTable();
				continue;
			}

			TimetableViewRoomsDaysVerticalForm* vrdvf=qobject_cast<TimetableViewRoomsDaysVerticalForm*>(wi);
			if(vrdvf!=nullptr){
				vrdvf->retranslateUi(vrdvf);
				vrdvf->updateRoomsTimetableTable();
				continue;
			}

			TimetableViewRoomsTimeHorizontalForm* vrthf=qobject_cast<TimetableViewRoomsTimeHorizontalForm*>(wi);
			if(vrthf!=nullptr){
				vrthf->retranslateUi(vrthf);
				vrthf->updateRoomsTimetableTable();
				continue;
			}

			TimetableShowConflictsForm* scf=qobject_cast<TimetableShowConflictsForm*>(wi);
			if(scf!=nullptr){
				scf->retranslateUi(scf);
				continue;
			}
		}
}

/*void SomeQtTranslations()
{
	//This function is never actually used
	//It just contains some commonly used Qt strings, so that some Qt strings of FET are translated.
	QString s1=QCoreApplication::translate("QDialogButtonBox", "&OK", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different");
	Q_UNUSED(s1);
	QString s2=QCoreApplication::translate("QDialogButtonBox", "OK");
	Q_UNUSED(s2);
	
	QString s3=QCoreApplication::translate("QDialogButtonBox", "&Cancel", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different");
	Q_UNUSED(s3);
	QString s4=QCoreApplication::translate("QDialogButtonBox", "Cancel");
	Q_UNUSED(s4);
	
	QString s5=QCoreApplication::translate("QDialogButtonBox", "&Yes", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different");
	Q_UNUSED(s5);
	QString s6=QCoreApplication::translate("QDialogButtonBox", "Yes to &All", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different. Please keep the translation short.");
	Q_UNUSED(s6);
	QString s7=QCoreApplication::translate("QDialogButtonBox", "&No", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different");
	Q_UNUSED(s7);
	QString s8=QCoreApplication::translate("QDialogButtonBox", "N&o to All", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different. Please keep the translation short.");
	Q_UNUSED(s8);

	QString s9=QCoreApplication::translate("QDialogButtonBox", "Help");
	Q_UNUSED(s9);

	//It seems that Qt 5 uses another context:
	QString s10=QCoreApplication::translate("QPlatformTheme", "&OK", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different");
	Q_UNUSED(s10);
	QString s11=QCoreApplication::translate("QPlatformTheme", "OK");
	Q_UNUSED(s11);
	
	QString s12=QCoreApplication::translate("QPlatformTheme", "&Cancel", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different");
	Q_UNUSED(s12);
	QString s13=QCoreApplication::translate("QPlatformTheme", "Cancel");
	Q_UNUSED(s13);
	
	QString s14=QCoreApplication::translate("QPlatformTheme", "&Yes", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different");
	Q_UNUSED(s14);
	QString s15=QCoreApplication::translate("QPlatformTheme", "Yes to &All", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different. Please keep the translation short.");
	Q_UNUSED(s15);
	QString s16=QCoreApplication::translate("QPlatformTheme", "&No", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different");
	Q_UNUSED(s16);
	QString s17=QCoreApplication::translate("QPlatformTheme", "N&o to All", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different. Please keep the translation short.");
	Q_UNUSED(s17);

	QString s18=QCoreApplication::translate("QPlatformTheme", "Help");
	Q_UNUSED(s18);

	//It also seems that Qt might use this context:
	//(examining the Qt sources shows that only the fields "&OK" and "&Cancel" might be needed, but it does not hurt to add all the possible fields.)
	QString s19=QCoreApplication::translate("QGnomeTheme", "&OK", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different");
	Q_UNUSED(s19);
	QString s20=QCoreApplication::translate("QGnomeTheme", "OK");
	Q_UNUSED(s20);
	
	QString s21=QCoreApplication::translate("QGnomeTheme", "&Cancel", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different");
	Q_UNUSED(s21);
	QString s22=QCoreApplication::translate("QGnomeTheme", "Cancel");
	Q_UNUSED(s22);
	
	QString s23=QCoreApplication::translate("QGnomeTheme", "&Yes", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different");
	Q_UNUSED(s23);
	QString s24=QCoreApplication::translate("QGnomeTheme", "Yes to &All", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different. Please keep the translation short.");
	Q_UNUSED(s24);
	QString s25=QCoreApplication::translate("QGnomeTheme", "&No", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different");
	Q_UNUSED(s25);
	QString s26=QCoreApplication::translate("QGnomeTheme", "N&o to All", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different. Please keep the translation short.");
	Q_UNUSED(s26);

	QString s27=QCoreApplication::translate("QGnomeTheme", "Help");
	Q_UNUSED(s27);
	
	QString s28=QCoreApplication::translate("QFontDialog", "Select Font");
	Q_UNUSED(s28);
	QString s29=QCoreApplication::translate("QFontDialog", "&Font", "Accelerator key (letter after ampersand) for &Font, Font st&yle, &Size, Stri&keout, &Underline, Wr&iting System, must be different");
	Q_UNUSED(s29);
	QString s30=QCoreApplication::translate("QFontDialog", "Font st&yle", "Accelerator key (letter after ampersand) for &Font, Font st&yle, &Size, Stri&keout, &Underline, Wr&iting System, must be different");
	Q_UNUSED(s30);
	QString s31=QCoreApplication::translate("QFontDialog", "&Size", "Accelerator key (letter after ampersand) for &Font, Font st&yle, &Size, Stri&keout, &Underline, Wr&iting System, must be different");
	Q_UNUSED(s31);
	QString s32=QCoreApplication::translate("QFontDialog", "Effects");
	Q_UNUSED(s32);
	QString s33=QCoreApplication::translate("QFontDialog", "Stri&keout", "Accelerator key (letter after ampersand) for &Font, Font st&yle, &Size, Stri&keout, &Underline, Wr&iting System, must be different");
	Q_UNUSED(s33);
	QString s34=QCoreApplication::translate("QFontDialog", "&Underline", "Accelerator key (letter after ampersand) for &Font, Font st&yle, &Size, Stri&keout, &Underline, Wr&iting System, must be different");
	Q_UNUSED(s34);
	QString s35=QCoreApplication::translate("QFontDialog", "Sample");
	Q_UNUSED(s35);
	QString s36=QCoreApplication::translate("QFontDialog", "Wr&iting System", "Accelerator key (letter after ampersand) for &Font, Font st&yle, &Size, Stri&keout, &Underline, Wr&iting System, must be different");
	Q_UNUSED(s36);
}*/

/**
FET starts here
*/
int main(int argc, char **argv)
{
	QApplication qapplication(argc, argv);

	initLanguagesSet();

	VERBOSE=false;

	terminateGeneratePointer=nullptr;
	
	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_buildings_schedule_ready=false;

	QObject::connect(&qapplication, &QApplication::lastWindowClosed, &qapplication, &QApplication::quit);

	//srand(unsigned(time(nullptr))); //useless, I use randomKnuth(), but just in case I use somewhere rand() by mistake...

	//initRandomKnuth();
	gen.rng.initializeMRG32k3a();

	OUTPUT_DIR=QDir::homePath()+FILE_SEP+"fet-results";
	
	QStringList _args=QCoreApplication::arguments();

	if(_args.count()<=2){
		fetSettings.readGenerationParameters(qapplication);
		
		QDir dir;
		
		bool t=true;

		//make sure that the output directory exists
		if(!dir.exists(OUTPUT_DIR))
			t=dir.mkpath(OUTPUT_DIR);

		if(!t){
			QMessageBox::critical(nullptr, FetTranslate::tr("FET critical"), FetTranslate::tr("Cannot create or use %1 directory (where the results should be stored) - you can continue operation, but you might not be able to work with FET."
			 " Maybe you can try to change the output directory from the 'Settings' menu. If this is a bug - please report it.").arg(QDir::toNativeSeparators(OUTPUT_DIR)));
		}
		
		QString testFileName=OUTPUT_DIR+FILE_SEP+"test_write_permissions_1.tmp";
		QFile test(testFileName);
		bool existedBefore=test.exists();
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
		bool t_t=test.open(QIODeviceBase::ReadWrite);
#else
		bool t_t=test.open(QIODevice::ReadWrite);
#endif
		if(!t_t){
			QMessageBox::critical(nullptr, FetTranslate::tr("FET critical"), FetTranslate::tr("You don't have write permissions in the output directory "
			 "(FET cannot open or create file %1) - you might not be able to work correctly with FET. Maybe you can try to change the output directory from the 'Settings' menu."
			 " If this is a bug - please report it.").arg(testFileName));
		}
		else{
			test.close();
			if(!existedBefore)
				test.remove();
		}

		QCoreApplication::setApplicationName(FetTranslate::tr("FET"));
		
		pqapplication=&qapplication;
		FetMainForm fetMainForm;
		pFetMainForm=&fetMainForm;
		fetMainForm.show();
		
		QObject::connect(&qapplication, &QApplication::aboutToQuit, &fetSettings, &FetSettings::writeGenerationParameters);
		
		if(_args.count()==2){ //Trying to open a fet file given as argument.
			QString fileName=QDir::fromNativeSeparators(_args.at(1));
			QFileInfo fileinfo(fileName);
			QString completeFileName=fileinfo.canonicalFilePath();
			if(completeFileName.isEmpty())
				QMessageBox::warning(&fetMainForm, FetTranslate::tr("FET warning"), FetTranslate::tr("Could not open file '%1' - not existing")
				 .arg(_args.at(1)));
			else
				fetMainForm.openFile(completeFileName);
		}

		int tmp2=qapplication.exec();
		
		return tmp2;
	}
	else{
		fetSettings.readGenerationParameters(qapplication); //Used only to load the language and correctly translate the strings below.
		
		QMessageBox::warning(nullptr, FetTranslate::tr("FET warning"), FetTranslate::tr("To start FET in the interface mode, please either do not give any"
		 " command-line parameters, or give a single command-line parameter, which is the name of the fet data file to be loaded on startup."));
		
		return 1;
	}
	/////////////////////////////////////////////////
}
