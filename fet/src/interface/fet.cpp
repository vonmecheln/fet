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

#ifndef FET_COMMAND_LINE
#include <QMessageBox>

#include <QWidget>
#endif

#include <QLocale>
#include <QTime>
#include <QDate>
#include <QDateTime>

#include <QSet>

static QSet<QString> languagesSet;

//#include <ctime>
#include <cstdlib>

#include "timetableexport.h"
#include "generate.h"
#include "generate_pre.h"

#include "timetable_defs.h"
#include "timetable.h"

//extern MRG32k3a rng;

#ifndef FET_COMMAND_LINE
FetSettings fetSettings;
#endif

#ifdef FET_COMMAND_LINE
Generate gen;
#else
extern Generate gen;
#endif

#ifdef FET_COMMAND_LINE
QString logsDir;
QString csvOutputDirectory;
#endif

#ifndef FET_COMMAND_LINE
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
#endif

#include <QCoreApplication>

#ifndef FET_COMMAND_LINE
#include <QApplication>
#include <QWidgetList>

#include <QSettings>
#include <QRect>
#endif

#include <QMutex>
#include <QString>
#include <QTranslator>

#include <QDir>
#include <QFileInfo>

#include <QTextStream>
#include <QFile>

#include <Qt>

#ifdef FET_COMMAND_LINE
#include <csignal>
#include <QtGlobal>

#include <atomic>
#include <thread>
#include <chrono>
#endif

#include <iostream>
using namespace std;

#ifndef FET_COMMAND_LINE
extern QRect mainFormSettingsRect;
extern int MAIN_FORM_SHORTCUTS_TAB_POSITION;
#endif

//extern Solution highestStageSolution;

//extern int maxActivitiesPlaced;

extern bool students_schedule_ready;
extern bool teachers_schedule_ready;
extern bool rooms_buildings_schedule_ready;

QString tempOutputDirectory;

//#ifndef FET_COMMAND_LINE
//extern QMutex myMutex;
//#else
//QMutex myMutex;
//#endif

void writeDefaultGenerationParameters();

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

#ifndef FET_COMMAND_LINE
QApplication* pqapplication=nullptr;

FetMainForm* pFetMainForm=nullptr;
#endif

//extern int XX;
//extern int YY;

Generate* terminateGeneratePointer;

#ifdef FET_COMMAND_LINE
#include "export.h"

extern bool EXPORT_CSV;
extern bool EXPORT_ALLOW_OVERWRITE;
extern bool EXPORT_FIRST_LINE_IS_HEADING;
extern int EXPORT_QUOTES;

extern const int EXPORT_DOUBLE_QUOTES;
extern const int EXPORT_SINGLE_QUOTES;
extern const int EXPORT_NO_QUOTES;

extern int EXPORT_FIELD_SEPARATOR;

extern const int EXPORT_COMMA;
extern const int EXPORT_SEMICOLON;
extern const int EXPORT_VERTICALBAR;

//extern Solution best_solution;

extern QString DIRECTORY_CSV;

QString communicationFile;
#endif

#ifdef FET_COMMAND_LINE

//https://stackoverflow.com/questions/6736536/c-input-and-output-to-the-console-window-at-the-same-time/31500127#31500127
//https://stackoverflow.com/questions/4184468/sleep-for-milliseconds
void pollFile(std::atomic<bool>& run)
{
	while(run.load()){
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		if(!terminateGeneratePointer->writeCurrentAndHighestTimetable && QFile::exists(communicationFile)){
			bool t=QFile::remove(communicationFile);
			if(t)
				terminateGeneratePointer->writeCurrentAndHighestTimetable=true;
		}
	}
}

//for command line version, if the user stops using a signal
void terminate(int param)
{
	Q_UNUSED(param);

	assert(terminateGeneratePointer!=nullptr);
	
	terminateGeneratePointer->abortOptimization=true;
}

void usage(QTextStream* out, const QString& error)
{
	QString s="";
	
	if(!error.isEmpty()){
		s+=QString("Incorrect command-line parameters (%1).").arg(error);
	
		s+="\n\n";
	}
	
	s+=QString(
		"Usage: fet-cl --inputfile=FILE [options]\n"
		"\t\tFILE is the input file, for instance \"data.fet\"\n"
		"\n"
		"Options:\n"
		"\n"
		"\t--outputdir=DIR\n"
		"\t\tDIR is the path to results directory, without trailing slash or backslash (default is current working path). "
		"Make sure you have write permissions there.\n"
		"\n"
		"\t--timelimitseconds=TLS\n"
		"\t\tTLS is an integer representing the time limit, in seconds, after which the program will stop the generation "
		"(default 2000000000, which is practically infinite).\n"
		"\n"
		"\t--htmllevel=LEVEL\n"
		"\t\tLEVEL is an integer from 0 to 7 and represents the detail level for the generated HTML timetables "
		"(default 2, larger values have more details/facilities and larger file sizes).\n"
		"\n"
		"\t--language=LANG\n"
		"\t\tLANG is one of: ar, bg, bs, ca, cs, da, de, el, en_GB, en_US, es, eu, fa, fr, gl, he, hu, id, it, ja, lt, mk, ms, nl, pl, pt_BR, ro, ru, si, sk, sq, "
		"sr, tr, uk, uz, vi, zh_CN, zh_TW (default en_US).\n"
		"\n"
		"\t--writetimetableconflicts=WT1\n"
		"\t--writetimetablesstatistics=WT2\n"
		"\t--writetimetablesxml=WT3\n"
		"\t--writetimetablesdayshorizontal=WT4\n"
		"\t--writetimetablesdaysvertical=WT5\n"
		"\t--writetimetablestimehorizontal=WT6\n"
		"\t--writetimetablestimevertical=WT7\n"
		"\t--writetimetablessubgroups=WT8\n"
		"\t--writetimetablesgroups=WT9\n"
		"\t--writetimetablesyears=WT10\n"
		"\t--writetimetablesteachers=WT11\n"
		"\t--writetimetablesteachersfreeperiods=WT12\n"
		"\t--writetimetablesbuildings=WT13\n"
		"\t--writetimetablesrooms=WT14\n"
		"\t--writetimetablessubjects=WT15\n"
		"\t--writetimetablesactivitytags=WT16\n"
		"\t--writetimetablesactivities=WT17\n"
		"\t\tWT1 to WT17 are either true or false and represent whether you want the corresponding timetables to be written on the disk (default true).\n"
		"\n"
		
		"\t--printsubjects=PS\n"
		"\t\tPS is either true or false and represents if you want the subjects to be present in the final HTML timetables (default true).\n"
		"\n"
		"\t--printactivitytags=PAT\n"
		"\t\tPAT is either true or false and represents if you want the activity tags to be present in the final HTML timetables (default true).\n"
		"\n"
		"\t--printteachers=PT\n"
		"\t\tPT is either true or false and represents if you want the teachers to be present in the final HTML timetables (default true).\n"
		"\n"
		"\t--printstudents=PSt\n"
		"\t\tPSt is either true or false and represents if you want the students to be present in the final HTML timetables (default true).\n"
		"\n"
		"\t--printrooms=PR\n"
		"\t\tPR is either true or false and represents if you want the rooms to be present in the final HTML timetables (default true).\n"
		
		"\n"
		"\t--printnotavailable=PNA\n"
		"\t\tPNA is either true or false and represents if you want -x- (for true) or --- (for false) in the generated timetables for the "
		"not available slots (default true).\n"
		"\n"
		"\t--printbreak=PB\n"
		"\t\tPB is either true or false and represents if you want -X- (for true) or --- (for false) in the generated timetables for the "
		"break slots (default true).\n"
		"\n"
		"\t--sortsubgroups=SS\n"
		"\t\tSS is either true or false and represents if you want the timetables of the subgroups to be sorted alphabetically by subgroup name "
		"(default false).\n"
		"\n"
		"\t--dividetimeaxisbydays=DTAD\n"
		"\t\tDTAD is either true or false, represents if you want the HTML timetables with time axis divided by days (default false).\n"
		"\n"
		"\t--duplicateverticalheaders=DVH\n"
		"\t\tDVH is either true or false, represents if you want the HTML timetables to duplicate vertical headers to the right of the tables, for easier reading (default false).\n"
		"\n"
		"\t--printsimultaneousactivities=PSA\n"
		"\t\tPSA is either true or false, represents if you want the HTML timetables to show related activities which have constraints with same starting time (default false). "
		"(for instance, if A1 (T1, G1) and A2 (T2, G2) have constraint activities same starting time, then in T1's timetable will appear also A2, at the same slot "
		"as A1).\n"
		"\n"
		"\t--randomseeds10=s10 --randomseeds11=s11 --randomseeds12=s12 --randomseeds20=s20 --randomseeds21=s21 --randomseeds22=s22\n"
		"\t\twhere you need to specify all the 6 random seed components, and s10, s11, and s12 are integers from minimum 0 to maximum 4294967086,"
		" not all 3 zero, and s20, s21, and s22 are integers from minimum 0 to maximum 4294944442, not all 3 zero "
		"(you can get the same timetable if the input file is identical, if the FET version is the same (or if the generation algorithm did not change),"
		" and if all the 6 random seed components are respectively equal).\n"
		"\n"
		"\t--warnifusinggroupactivitiesininitialorder=WGA\n"
		"\t\tWGA is either true or false, represents whether you want a message box to be shown, with a warning, if the input file contains nonstandard timetable "
		"generation options to group activities in the initial order (default true).\n"
		"\n"
		"\t--warnsubgroupswiththesameactivities=WSSA\n"
		"\t\tWSSA is either true or false, represents whether you want a message box to be shown, with a warning, if your input file contains subgroups which have "
		"the same activities (default true).\n"
		"\n"
		"\t--printdetailedtimetables=PDT\n"
		"\t\tPDT is either true or false, represents whether you want to show the detailed (true) or less detailed (false) years and groups timetables (default true).\n"
		"\n"
		"\t--printdetailedteachersfreeperiodstimetables=PDTFP\n"
		"\t\tPDTFP is either true or false, represents whether you want to show the detailed (true) or less detailed (false) teachers free periods timetables (default true).\n"
		"\n"
		"\t--exportcsv=ECSV\n"
		"\t\tECSV is either true or false, represents whether you want to export the CSV file and timetables (default false).\n"
		"\n"
		"\t--overwritecsv=OCSV\n"
		"\t\tOCSV is either true or false, represents whether you want to overwrite old CSV files, if they exist (default false).\n"
		"\n"
		"\t--firstlineisheadingcsv=FLHCSV\n"
		"\t\tFLHCSV is either true or false, represents whether you want the heading of the CSV files to be header (true) or data (false). The default value is true.\n"
		"\n"
		"\t--quotescsv=QCSV\n"
		"\t\tQCSV is one value from the set [doublequotes|singlequotes|none] (write a single value from these three exactly as it is written here). The default value is "
		"doublequotes.\n"
		"\n"
		"\t--fieldseparatorcsv=FSCSV\n"
		"\t\tFSCSV is one value from the set [comma|semicolon|verticalbar] (write a single value from these three exactly as it is written here). The default value is "
		"comma.\n"
		"\n"
		"\t--showvirtualrooms=SVR\n"
		"\t\tSVR is either true or false, represents whether you want to show virtual rooms in the timetables (default false).\n"
		"\n"
		"\t--warnifusingactivitiesnotfixedtimefixedspacevirtualroomsrealrooms=WANFTFS\n"
		"\t\tWANFTFS is either true or false, represents whether you want the program to issue a warning if you are using activities which are not fixed in time, "
		"but are fixed in space in a virtual room, specifying also the real rooms (which is not recommended) (default true).\n"
		"\n"
		"\t--warnifusingmaxhoursdailywithlessthan100percentweight=WMHDWLT100PW\n"
		"\t\tWMHDWLT100PW is either true or false, represents whether you want the program to issue a warning if you are using constraints of type teacher(s)/students (set) "
		"max hours daily with a weight less than 100% (default true).\n"
		"\n"
		"\t--verbose=VBS\n"
		"\t\tVBS is either true or false, represents whether you want additional generation messages and other messages to be shown on the command line (default false).\n"
		"\n"
		"Run \"fet-cl --help\" to get usage information.\n"
		"\n"
		"Run \"fet-cl --version\" to get version information.\n"
		"\n"
		"You can ask the FET command line process to stop the timetable generation, by sending it the SIGTERM (or SIGBREAK, on Windows) signal. "
		"FET will then write the current timetable and the highest stage timetable and exit. "
		"(If you send the FET command line the SIGINT signal it will stop immediately, without writing the timetable.)\n"
		"\n"
		"If you create a file named exactly 'sigwrite' in the root output directory, the program will output the current and the "
		"highest stage timetables without stopping the generation (a poll for the existence of this file is done once every second), and then remove this file."
	);
	
	cout<<qPrintable(s)<<endl;
	if(out!=nullptr)
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
		(*out)<<qPrintable(s)<<Qt::endl;
#else
		(*out)<<qPrintable(s)<<endl;
#endif
}
#endif

#ifndef FET_COMMAND_LINE
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
		cout<<"Settings read"<<endl;
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
		cout<<"Settings saved"<<endl;
	}
	
	pFetMainForm=nullptr;
}
#endif

void initLanguagesSet()
{
	//This is one of the two places to insert a new language in the sources (the other one is in fetmainform.cpp).
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

#ifndef FET_COMMAND_LINE
void setLanguage(QApplication& qapplication, QWidget* parent)
#else
void setLanguage(QCoreApplication& qapplication, QWidget* parent)
#endif
{
	Q_UNUSED(qapplication); //silence wrong MSVC warning

	static int cntTranslators=0;
	
	if(cntTranslators>0){
		qapplication.removeTranslator(&translator);
		cntTranslators=0;
	}

	//translator stuff
	QDir d("/usr/share/fet/translations");
	
	bool translation_loaded=false;
	
	if(FET_LANGUAGE!="en_US" && languagesSet.contains(FET_LANGUAGE)){
		translation_loaded=translator.load("fet_"+FET_LANGUAGE, qapplication.applicationDirPath());
		if(!translation_loaded){
			translation_loaded=translator.load("fet_"+FET_LANGUAGE, qapplication.applicationDirPath()+"/translations");
			if(!translation_loaded){
				if(d.exists()){
					translation_loaded=translator.load("fet_"+FET_LANGUAGE, "/usr/share/fet/translations");
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
		 .arg("/usr/share/fet/translations")
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
	
#ifndef FET_COMMAND_LINE
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
#endif
}

void SomeQtTranslations()
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
}

/**
FET starts here
*/
int main(int argc, char **argv)
{
#ifndef FET_COMMAND_LINE
	QApplication qapplication(argc, argv);
#else
	QCoreApplication qCoreApplication(argc, argv);
#endif

	initLanguagesSet();

	VERBOSE=false;

	terminateGeneratePointer=nullptr;
	
	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_buildings_schedule_ready=false;

#ifndef FET_COMMAND_LINE
	QObject::connect(&qapplication, &QApplication::lastWindowClosed, &qapplication, &QApplication::quit);
#endif

	//srand(unsigned(time(nullptr))); //useless, I use randomKnuth(), but just in case I use somewhere rand() by mistake...

	//initRandomKnuth();
	gen.rng.initializeMRG32k3a();

	OUTPUT_DIR=QDir::homePath()+FILE_SEP+"fet-results";
	
	QStringList _args=QCoreApplication::arguments();

#ifndef FET_COMMAND_LINE
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
#else
	/////////////////////////////////////////////////
	//begin command line
	
	if(_args.count()>1){
		bool showHelp=false;
	
		qint64 randomSeedS10=-1;
		qint64 randomSeedS11=-1;
		qint64 randomSeedS12=-1;

		qint64 randomSeedS20=-1;
		qint64 randomSeedS21=-1;
		qint64 randomSeedS22=-1;

		bool randomSeedS10Specified=false;
		bool randomSeedS11Specified=false;
		bool randomSeedS12Specified=false;

		bool randomSeedS20Specified=false;
		bool randomSeedS21Specified=false;
		bool randomSeedS22Specified=false;

		bool randomSeedXSpecified=false;
		bool randomSeedYSpecified=false;
	
		QString outputDirectory="";
	
		INPUT_FILENAME_XML="";
		
		QString filename="";
		
		int secondsLimit=2000000000;
		
		TIMETABLE_HTML_LEVEL=2;
		
		TIMETABLE_HTML_PRINT_ACTIVITY_TAGS=true;

		TIMETABLE_HTML_PRINT_SUBJECTS=true;
		TIMETABLE_HTML_PRINT_TEACHERS=true;
		TIMETABLE_HTML_PRINT_STUDENTS=true;
		TIMETABLE_HTML_PRINT_ROOMS=true;

		PRINT_DETAILED_HTML_TIMETABLES=true;

		PRINT_DETAILED_HTML_TEACHERS_FREE_PERIODS=true;

		FET_LANGUAGE="en_US";
		FET_LANGUAGE_WITH_LOCALE=FET_LANGUAGE;
		
		TIMETABLES_SUBGROUPS_SORTED=false;
		
		PRINT_NOT_AVAILABLE_TIME_SLOTS=true;
		
		PRINT_BREAK_TIME_SLOTS=true;
		
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

		DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS=false;
		
		TIMETABLE_HTML_REPEAT_NAMES=false;

		PRINT_ACTIVITIES_WITH_SAME_STARTING_TIME=false;
		
		QStringList unrecognizedOptions;
		
		SHOW_WARNING_FOR_SUBGROUPS_WITH_THE_SAME_ACTIVITIES=true;
		
		SHOW_WARNING_FOR_ACTIVITIES_FIXED_SPACE_VIRTUAL_REAL_ROOMS_BUT_NOT_FIXED_TIME=true;

		SHOW_WARNING_FOR_MAX_HOURS_DAILY_WITH_UNDER_100_WEIGHT=true;
		
		SHOW_WARNING_FOR_GROUP_ACTIVITIES_IN_INITIAL_ORDER=true;
		
		SHOW_VIRTUAL_ROOMS_IN_TIMETABLES=false;
		
		EXPORT_CSV=false;
		EXPORT_ALLOW_OVERWRITE=false;
		EXPORT_FIRST_LINE_IS_HEADING=true;
		EXPORT_QUOTES=EXPORT_DOUBLE_QUOTES;
		EXPORT_FIELD_SEPARATOR=EXPORT_COMMA;

		bool showVersion=false;
		
		for(int i=1; i<_args.count(); i++){
			QString s=_args[i];
			
			if(s.left(12)=="--inputfile=")
				filename=QDir::fromNativeSeparators(s.right(s.length()-12));
			else if(s.left(19)=="--timelimitseconds=")
				secondsLimit=s.right(s.length()-19).toInt();
			else if(s.left(21)=="--timetablehtmllevel=")
				TIMETABLE_HTML_LEVEL=s.right(s.length()-21).toInt();
			else if(s.left(12)=="--htmllevel=")
				TIMETABLE_HTML_LEVEL=s.right(s.length()-12).toInt();
			else if(s.left(20)=="--printactivitytags="){
				if(s.right(5)=="false")
					TIMETABLE_HTML_PRINT_ACTIVITY_TAGS=false;
			}
			
			else if(s.left(16)=="--printsubjects="){
				if(s.right(5)=="false")
					TIMETABLE_HTML_PRINT_SUBJECTS=false;
			}
			else if(s.left(16)=="--printteachers="){
				if(s.right(5)=="false")
					TIMETABLE_HTML_PRINT_TEACHERS=false;
			}
			else if(s.left(16)=="--printstudents="){
				if(s.right(5)=="false")
					TIMETABLE_HTML_PRINT_STUDENTS=false;
			}
			else if(s.left(13)=="--printrooms="){
				if(s.right(5)=="false")
					TIMETABLE_HTML_PRINT_ROOMS=false;
			}
			
			else if(s.left(26)=="--printdetailedtimetables="){
				if(s.right(5)=="false")
					PRINT_DETAILED_HTML_TIMETABLES=false;
			}
			else if(s.left(45)=="--printdetailedteachersfreeperiodstimetables="){
				if(s.right(5)=="false")
					PRINT_DETAILED_HTML_TEACHERS_FREE_PERIODS=false;
			}
			else if(s.left(11)=="--language="){
				FET_LANGUAGE=s.right(s.length()-11);

				if(FET_LANGUAGE=="ar")
					FET_LANGUAGE_WITH_LOCALE="ar_DZ";
				else
					FET_LANGUAGE_WITH_LOCALE=FET_LANGUAGE;
			}
			else if(s.left(20)=="--printnotavailable="){
				if(s.right(5)=="false")
					PRINT_NOT_AVAILABLE_TIME_SLOTS=false;
			}
			else if(s.left(13)=="--printbreak="){
				if(s.right(5)=="false")
					PRINT_BREAK_TIME_SLOTS=false;
			}
			else if(s.left(16)=="--sortsubgroups="){
				if(s.right(4)=="true")
					TIMETABLES_SUBGROUPS_SORTED=true;
			}
			else if(s.left(23)=="--dividetimeaxisbydays="){
				if(s.right(4)=="true")
					DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS=true;
			}
			else if(s.left(27)=="--duplicateverticalheaders="){
				if(s.right(4)=="true")
					TIMETABLE_HTML_REPEAT_NAMES=true;
			}
			else if(s.left(12)=="--outputdir="){
				outputDirectory=QDir::fromNativeSeparators(s.right(s.length()-12));
			}
			else if(s.left(30)=="--printsimultaneousactivities="){
				if(s.right(4)=="true")
					PRINT_ACTIVITIES_WITH_SAME_STARTING_TIME=true;
			}
			//keep this to deny beginning the generation for FET-5.44.0 or later, because it is an obsolete option and we cannot bypass it
			else if(s.left(14)=="--randomseedx="){
				randomSeedXSpecified=true;
				//randomSeedX=s.right(s.length()-14).toInt();
			}
			//keep this to deny beginning the generation for FET-5.44.0 or later, because it is an obsolete option and we cannot bypass it
			else if(s.left(14)=="--randomseedy="){
				randomSeedYSpecified=true;
				//randomSeedY=s.right(s.length()-14).toInt();
			}

			else if(s.left(16)=="--randomseeds10="){
				randomSeedS10Specified=true;
				bool ok;
				randomSeedS10=s.right(s.length()-16).toLongLong(&ok);
				if(!ok)
					randomSeedS10=-1;
			}
			else if(s.left(16)=="--randomseeds11="){
				randomSeedS11Specified=true;
				bool ok;
				randomSeedS11=s.right(s.length()-16).toLongLong(&ok);
				if(!ok)
					randomSeedS11=-1;
			}
			else if(s.left(16)=="--randomseeds12="){
				randomSeedS12Specified=true;
				bool ok;
				randomSeedS12=s.right(s.length()-16).toLongLong(&ok);
				if(!ok)
					randomSeedS12=-1;
			}

			else if(s.left(16)=="--randomseeds20="){
				randomSeedS20Specified=true;
				bool ok;
				randomSeedS20=s.right(s.length()-16).toLongLong(&ok);
				if(!ok)
					randomSeedS20=-1;
			}
			else if(s.left(16)=="--randomseeds21="){
				randomSeedS21Specified=true;
				bool ok;
				randomSeedS21=s.right(s.length()-16).toLongLong(&ok);
				if(!ok)
					randomSeedS21=-1;
			}
			else if(s.left(16)=="--randomseeds22="){
				randomSeedS22Specified=true;
				bool ok;
				randomSeedS22=s.right(s.length()-16).toLongLong(&ok);
				if(!ok)
					randomSeedS22=-1;
			}

			else if(s.left(37)=="--warnsubgroupswiththesameactivities="){
				if(s.right(5)=="false")
					SHOW_WARNING_FOR_SUBGROUPS_WITH_THE_SAME_ACTIVITIES=false;
			}
			else if(s.left(67)=="--warnifusingactivitiesnotfixedtimefixedspacevirtualroomsrealrooms="){
				if(s.right(5)=="false")
					SHOW_WARNING_FOR_ACTIVITIES_FIXED_SPACE_VIRTUAL_REAL_ROOMS_BUT_NOT_FIXED_TIME=false;
			}
			else if(s.left(55)=="--warnifusingmaxhoursdailywithlessthan100percentweight="){
				if(s.right(5)=="false")
					SHOW_WARNING_FOR_MAX_HOURS_DAILY_WITH_UNDER_100_WEIGHT=false;
			}
			else if(s.left(43)=="--warnifusinggroupactivitiesininitialorder="){
				if(s.right(5)=="false")
					SHOW_WARNING_FOR_GROUP_ACTIVITIES_IN_INITIAL_ORDER=false;
			}
			else if(s.left(19)=="--showvirtualrooms="){
				if(s.right(4)=="true")
					SHOW_VIRTUAL_ROOMS_IN_TIMETABLES=true;
			}
			else if(s.left(10)=="--verbose="){
				if(s.right(4)=="true")
					VERBOSE=true;
			}
			else if(s=="--help"){
				showHelp=true;
			}
			else if(s=="--version"){
				showVersion=true;
			}
			///
			else if(s.left(26)=="--writetimetableconflicts="){
				if(s.right(5)=="false")
					WRITE_TIMETABLE_CONFLICTS=false;
			}
			//
			else if(s.left(28)=="--writetimetablesstatistics="){
				if(s.right(5)=="false")
					WRITE_TIMETABLES_STATISTICS=false;
			}
			else if(s.left(21)=="--writetimetablesxml="){
				if(s.right(5)=="false")
					WRITE_TIMETABLES_XML=false;
			}
			else if(s.left(32)=="--writetimetablesdayshorizontal="){
				if(s.right(5)=="false")
					WRITE_TIMETABLES_DAYS_HORIZONTAL=false;
			}
			else if(s.left(30)=="--writetimetablesdaysvertical="){
				if(s.right(5)=="false")
					WRITE_TIMETABLES_DAYS_VERTICAL=false;
			}
			else if(s.left(32)=="--writetimetablestimehorizontal="){
				if(s.right(5)=="false")
					WRITE_TIMETABLES_TIME_HORIZONTAL=false;
			}
			else if(s.left(30)=="--writetimetablestimevertical="){
				if(s.right(5)=="false")
					WRITE_TIMETABLES_TIME_VERTICAL=false;
			}
			//
			else if(s.left(27)=="--writetimetablessubgroups="){
				if(s.right(5)=="false")
					WRITE_TIMETABLES_SUBGROUPS=false;
			}
			else if(s.left(24)=="--writetimetablesgroups="){
				if(s.right(5)=="false")
					WRITE_TIMETABLES_GROUPS=false;
			}
			else if(s.left(23)=="--writetimetablesyears="){
				if(s.right(5)=="false")
					WRITE_TIMETABLES_YEARS=false;
			}
			else if(s.left(26)=="--writetimetablesteachers="){
				if(s.right(5)=="false")
					WRITE_TIMETABLES_TEACHERS=false;
			}
			else if(s.left(37)=="--writetimetablesteachersfreeperiods="){
				if(s.right(5)=="false")
					WRITE_TIMETABLES_TEACHERS_FREE_PERIODS=false;
			}
			else if(s.left(27)=="--writetimetablesbuildings="){
				if(s.right(5)=="false")
					WRITE_TIMETABLES_BUILDINGS=false;
			}
			else if(s.left(23)=="--writetimetablesrooms="){
				if(s.right(5)=="false")
					WRITE_TIMETABLES_ROOMS=false;
			}
			else if(s.left(26)=="--writetimetablessubjects="){
				if(s.right(5)=="false")
					WRITE_TIMETABLES_SUBJECTS=false;
			}
			else if(s.left(30)=="--writetimetablesactivitytags="){
				if(s.right(5)=="false")
					WRITE_TIMETABLES_ACTIVITY_TAGS=false;
			}
			else if(s.left(28)=="--writetimetablesactivities="){
				if(s.right(5)=="false")
					WRITE_TIMETABLES_ACTIVITIES=false;
			}
			//Export CSV
			else if(s.left(12)=="--exportcsv="){
				if(s.right(4)=="true")
					EXPORT_CSV=true;
			}
			else if(s.left(15)=="--overwritecsv="){
				if(s.right(4)=="true")
					EXPORT_ALLOW_OVERWRITE=true;
			}
			else if(s.left(24)=="--firstlineisheadingcsv="){
				if(s.right(5)=="false")
					EXPORT_FIRST_LINE_IS_HEADING=false;
			}
			else if(s.left(12)=="--quotescsv="){
				if(s.right(12)=="singlequotes")
					EXPORT_QUOTES=EXPORT_SINGLE_QUOTES;
				else if(s.right(4)=="none")
					EXPORT_QUOTES=EXPORT_NO_QUOTES;
			}
			else if(s.left(20)=="--fieldseparatorcsv="){
				if(s.right(9)=="semicolon")
					EXPORT_FIELD_SEPARATOR=EXPORT_SEMICOLON;
				else if(s.right(11)=="verticalbar")
					EXPORT_FIELD_SEPARATOR=EXPORT_VERTICALBAR;
			}
			///
			else
				unrecognizedOptions.append(s);
		}
		
		if(filename==""){
			if(unrecognizedOptions.count()>0){
				for(const QString& s : std::as_const(unrecognizedOptions)){
					cout<<"Unrecognized option: "<<qPrintable(s)<<endl;
				}
				cout<<endl;
			}

			if(showHelp){
				usage(nullptr, QString(""));
				return 0;
			}
			else if(showVersion){
				cout<<"FET version "<<qPrintable(FET_VERSION)<<endl;
				cout<<"Free timetabling software, licensed under the GNU Affero General Public License version 3"<<endl;
				cout<<"Copyright (C) 2002-2025 Liviu Lalescu, Volker Dirr"<<endl;
				cout<<"Homepage: https://lalescu.ro/liviu/fet/"<<endl;
				cout<<"This program uses Qt version "<<qVersion()<<", Copyright (C) The Qt Company Ltd and other contributors."<<endl;
				cout<<"Depending on the platform and compiler, this program may use libraries from:"<<endl;
				cout<<"  gcc, Copyright (C) Free Software Foundation, Inc."<<endl;
				cout<<"  MinGW-w64, Copyright (c) by the mingw-w64 project"<<endl;
				cout<<"  Clang"<<endl;
				return 0;
			}
			else{
				usage(nullptr, QString("Input file not specified"));
				return 1;
			}
		}
		else if(!QFile::exists(filename)){
			if(unrecognizedOptions.count()>0){
				for(const QString& s : std::as_const(unrecognizedOptions)){
					cout<<"Unrecognized option: "<<qPrintable(s)<<endl;
				}
				cout<<endl;
			}

			cout<<"Error: the specified input file "<<qPrintable(QDir::toNativeSeparators(filename))<<" is not existing"<<endl;
			return 1;
		}
		
		INPUT_FILENAME_XML=filename;
		
		QString initialDir=outputDirectory;
		if(initialDir!="")
			initialDir.append(FILE_SEP);
		
		csvOutputDirectory=outputDirectory;
		//cout<<"csvOutputDirectory="<<qPrintable(csvOutputDirectory)<<endl;
		
		if(outputDirectory!="")
			outputDirectory.append(FILE_SEP);
		outputDirectory.append("timetables");

		/*if(csvOutputDirectory!="")
			csvOutputDirectory.append(FILE_SEP);
		csvOutputDirectory.append("csv");*/

		//////////
		if(INPUT_FILENAME_XML!=""){
			outputDirectory.append(FILE_SEP);
			outputDirectory.append(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1));
			if(outputDirectory.right(4)==".fet")
				outputDirectory=outputDirectory.left(outputDirectory.length()-4);

			/*csvOutputDirectory.append(FILE_SEP);
			csvOutputDirectory.append(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1));
			if(csvOutputDirectory.right(4)==".fet")
				csvOutputDirectory=csvOutputDirectory.left(csvOutputDirectory.length()-4);*/
		}
		//////////
		
		QDir dir;
		logsDir=initialDir+"logs";
		if(!dir.exists(logsDir))
			dir.mkpath(logsDir);
		logsDir.append(FILE_SEP);
		
		////////
		QFile logFile(logsDir+"result.txt");
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
		bool tttt=logFile.open(QIODeviceBase::WriteOnly);
#else
		bool tttt=logFile.open(QIODevice::WriteOnly);
#endif
		if(!tttt){
			cout<<"FET critical - you don't have write permissions in the output directory - (FET cannot open or create file "<<qPrintable(QDir::toNativeSeparators(logsDir))<<"result.txt)."
			 " If this is a bug - please report it."<<endl;
			return 1;
		}
		QTextStream out(&logFile);
		///////

		communicationFile=initialDir+"sigwrite";
		if(QFile::exists(communicationFile)){
			bool t=QFile::remove(communicationFile);
			if(!t){
				out<<"Cannot remove the file named "<<qPrintable(QDir::toNativeSeparators(communicationFile))
				 <<". This is a fatal error for FET-CL. Please either remove this file, or start the generation in another directory."<<Qt::endl;
				cout<<"Cannot remove the file named "<<qPrintable(QDir::toNativeSeparators(communicationFile))
				 <<". This is a fatal error for FET-CL. Please either remove this file, or start the generation in another directory."<<endl;
				exit(1);
			}
		}
		
		if(unrecognizedOptions.count()>0){
			for(const QString& s : std::as_const(unrecognizedOptions)){
				cout<<"Unrecognized option: "<<qPrintable(s)<<endl;
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
				out<<"Unrecognized option: "<<qPrintable(s)<<Qt::endl;
#else
				out<<"Unrecognized option: "<<qPrintable(s)<<endl;
#endif
			}
			cout<<endl;
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
			out<<Qt::endl;
#else
			out<<endl;
#endif
		}
		
		//Cleanup the previous unsuccessful generation, if any. No need to remove the other files, they are overwritten.
		QFile oldDifficultActivitiesFile(logsDir+"difficult_activities.txt");
		if(oldDifficultActivitiesFile.exists()){
			bool t=oldDifficultActivitiesFile.remove();
			if(!t){
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
				out<<"Cannot remove the old existing file "<<qPrintable(QDir::toNativeSeparators(logsDir))<<"difficult_activities.txt"<<Qt::endl;
#else
				out<<"Cannot remove the old existing file "<<qPrintable(QDir::toNativeSeparators(logsDir))<<"difficult_activities.txt"<<endl;
#endif
				cout<<"Cannot remove the old existing file "<<qPrintable(QDir::toNativeSeparators(logsDir))<<"difficult_activities.txt"<<endl;
			}
		}
		
		QFile oldWarningsFile(logsDir+"warnings.txt");
		if(oldWarningsFile.exists()){
			bool t=oldWarningsFile.remove();
			if(!t){
				out<<"Cannot remove the old warnings file "<<qPrintable(QDir::toNativeSeparators(logsDir))<<"warnings.txt"<<Qt::endl;
				cout<<"Cannot remove the old warnings file "<<qPrintable(QDir::toNativeSeparators(logsDir))<<"warnings.txt"<<endl;
			}
		}
		
		QFile oldErrorsFile(logsDir+"errors.txt");
		if(oldErrorsFile.exists()){
			bool t=oldErrorsFile.remove();
			if(!t){
				out<<"Cannot remove the old errors file "<<qPrintable(QDir::toNativeSeparators(logsDir))<<"errors.txt"<<Qt::endl;
				cout<<"Cannot remove the old errors file "<<qPrintable(QDir::toNativeSeparators(logsDir))<<"errors.txt"<<endl;
			}
		}
		
		setLanguage(qCoreApplication, nullptr);
		
		QCoreApplication::setApplicationName(FetTranslate::tr("FET-CL"));
		
		QFile maxPlacedActivityFile(logsDir+"max_placed_activities.txt");
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
		bool tmpaf=maxPlacedActivityFile.open(QIODeviceBase::WriteOnly);
#else
		bool tmpaf=maxPlacedActivityFile.open(QIODevice::WriteOnly);
#endif
		if(!tmpaf){
			cout<<"fet: critical error - cannot open the log file for the maximum placed activities - (FET cannot open or create file "<<qPrintable(QDir::toNativeSeparators(logsDir))<<"max_placed_activities.txt."
			 " If this is a bug - please report it."<<endl;
			out<<"fet: critical error - cannot open the log file for the maximum placed activities - (FET cannot open or create file "<<qPrintable(QDir::toNativeSeparators(logsDir))<<"max_placed_activities.txt."
			 " If this is a bug - please report it."<<Qt::endl;

			return 1;
		}
		
		QTextStream maxPlacedActivityStream(&maxPlacedActivityFile);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
		maxPlacedActivityStream.setEncoding(QStringConverter::Utf8);
#else
		maxPlacedActivityStream.setCodec("UTF-8");
#endif
		maxPlacedActivityStream.setGenerateByteOrderMark(true);
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
		maxPlacedActivityStream<<FetTranslate::tr("This is the list of max placed activities, chronologically. If FET could reach maximum n-th activity, look at the n+1-st activity"
			" in the initial order of the activities")<<Qt::endl<<Qt::endl;
#else
		maxPlacedActivityStream<<FetTranslate::tr("This is the list of max placed activities, chronologically. If FET could reach maximum n-th activity, look at the n+1-st activity"
			" in the initial order of the activities")<<endl<<endl;
#endif
		
		QFile initialOrderFile(logsDir+"initial_order.txt");
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
		bool iof=initialOrderFile.open(QIODeviceBase::WriteOnly);
#else
		bool iof=initialOrderFile.open(QIODevice::WriteOnly);
#endif
		if(!iof){
			cout<<"fet: critical error - cannot open the log file for the initial order of the activities - (FET cannot open or create file "<<qPrintable(QDir::toNativeSeparators(logsDir))<<"initial_order.txt."
			 " If this is a bug - please report it."<<endl;
			out<<"fet: critical error - cannot open the log file for the initial order of the activities - (FET cannot open or create file "<<qPrintable(QDir::toNativeSeparators(logsDir))<<"initial_order.txt."
			 " If this is a bug - please report it."<<Qt::endl;

			return 1;
		}

		QTextStream initialOrderStream(&initialOrderFile);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
		initialOrderStream.setEncoding(QStringConverter::Utf8);
#else
		initialOrderStream.setCodec("UTF-8");
#endif
		initialOrderStream.setGenerateByteOrderMark(true);
		
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
		out<<"This file contains the result (log) of last operation"<<Qt::endl<<Qt::endl;
#else
		out<<"This file contains the result (log) of last operation"<<endl<<endl;
#endif
		
		QDate dat=QDate::currentDate();
		QTime tim=QTime::currentTime();
		QLocale loc(FET_LANGUAGE);
		QString sTime=loc.toString(dat, QLocale::ShortFormat)+" "+loc.toString(tim, QLocale::ShortFormat);
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
		out<<"FET command line generation started on "<<qPrintable(sTime)<<Qt::endl<<Qt::endl;
#else
		out<<"FET command line generation started on "<<qPrintable(sTime)<<endl<<endl;
#endif
		
		tempOutputDirectory=outputDirectory;
		
		if(QFileInfo::exists(outputDirectory)){
			int i=2;
			for(;;){
				QString CODN=outputDirectory+"-"+QString::number(i);
				if(!QFileInfo::exists(CODN)){
					outputDirectory=CODN;
					break;
				}
				i++;
			}
		}
		
		if(outputDirectory!="")
			if(!dir.exists(outputDirectory))
				dir.mkpath(outputDirectory);
		
		if(outputDirectory!="")
			outputDirectory.append(FILE_SEP);

		QFile test(outputDirectory+"test_write_permissions_2.tmp");
		bool existedBefore=test.exists();
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
		bool t_t=test.open(QIODeviceBase::ReadWrite);
#else
		bool t_t=test.open(QIODevice::ReadWrite);
#endif
		if(!t_t){
			cout<<"fet: critical error - you don't have write permissions in the output directory - (FET cannot open or create file "<<qPrintable(QDir::toNativeSeparators(outputDirectory))<<"test_write_permissions_2.tmp)."
			 " If this is a bug - please report it."<<endl;
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
			out<<"fet: critical error - you don't have write permissions in the output directory - (FET cannot open or create file "<<qPrintable(QDir::toNativeSeparators(outputDirectory))<<"test_write_permissions_2.tmp)."
			 " If this is a bug - please report it."<<Qt::endl;
#else
			out<<"fet: critical error - you don't have write permissions in the output directory - (FET cannot open or create file "<<qPrintable(QDir::toNativeSeparators(outputDirectory))<<"test_write_permissions_2.tmp)."
			 " If this is a bug - please report it."<<endl;
#endif
			return 1;
		}
		else{
			test.close();
			if(!existedBefore)
				test.remove();
		}

//		if(filename==""){
//			usage(/*&out*/nullptr, QString("Input file not specified"));
//			logFile.close();
//			return 1;
//		}
		if(secondsLimit==0){
			usage(nullptr, QString("Time limit is 0 seconds"));
			logFile.close();
			return 1;
		}
		if(TIMETABLE_HTML_LEVEL<0 || TIMETABLE_HTML_LEVEL>7){
			usage(nullptr, QString("The HTML level must be 0, 1, 2, 3, 4, 5, 6, or 7"));
			logFile.close();
			return 1;
		}
		if(randomSeedXSpecified || randomSeedYSpecified){
			usage(nullptr, QString("Starting with FET version 5.44.0 the random number generator was changed to a better one. Please see usage for instructions"
			 " on how to specify the random number generator seed at the start of the program (or do not specify a random seed at all)."
			 " The program will now abort the generation"));
			logFile.close();
			return 1;
		}
		if(randomSeedS10Specified && randomSeedS11Specified && randomSeedS12Specified
		 && randomSeedS20Specified && randomSeedS21Specified && randomSeedS22Specified){
			if(randomSeedS10<0 || randomSeedS10>=gen.rng.m1){
				usage(nullptr, QString("The random seed s10 component must be an integer number at least %1 and at most %2").arg(0).arg(gen.rng.m1-1));
				logFile.close();
				return 1;
			}
			if(randomSeedS11<0 || randomSeedS11>=gen.rng.m1){
				usage(nullptr, QString("The random seed s11 component must be an integer number at least %1 and at most %2").arg(0).arg(gen.rng.m1-1));
				logFile.close();
				return 1;
			}
			if(randomSeedS12<0 || randomSeedS12>=gen.rng.m1){
				usage(nullptr, QString("The random seed s12 component must be an integer number at least %1 and at most %2").arg(0).arg(gen.rng.m1-1));
				logFile.close();
				return 1;
			}

			if(randomSeedS20<0 || randomSeedS20>=gen.rng.m1){
				usage(nullptr, QString("The random seed s20 component must be an integer number at least %1 and at most %2").arg(0).arg(gen.rng.m1-1));
				logFile.close();
				return 1;
			}
			if(randomSeedS21<0 || randomSeedS21>=gen.rng.m1){
				usage(nullptr, QString("The random seed s21 component must be an integer number at least %1 and at most %2").arg(0).arg(gen.rng.m1-1));
				logFile.close();
				return 1;
			}
			if(randomSeedS22<0 || randomSeedS22>=gen.rng.m1){
				usage(nullptr, QString("The random seed s22 component must be an integer number at least %1 and at most %2").arg(0).arg(gen.rng.m1-1));
				logFile.close();
				return 1;
			}
			
			if(randomSeedS10==0 && randomSeedS11==0 && randomSeedS12==0){
				usage(nullptr, QString("The random seed numbers for component 1: s10, s11, and s12, must not all be zero"));
				logFile.close();
				return 1;
			}

			if(randomSeedS20==0 && randomSeedS21==0 && randomSeedS22==0){
				usage(nullptr, QString("The random seeds numbers for component 2: s20, s21, and s22, must not all be zero"));
				logFile.close();
				return 1;
			}

			gen.rng.initializeMRG32k3a(randomSeedS10, randomSeedS11, randomSeedS12,
			 randomSeedS20, randomSeedS21, randomSeedS22);
		}
		else if(randomSeedS10Specified || randomSeedS11Specified || randomSeedS12Specified
		 || randomSeedS20Specified || randomSeedS21Specified || randomSeedS22Specified){
			QStringList specified, notSpecified;

			if(randomSeedS10Specified)
				specified.append("s10");
			else
				notSpecified.append("s10");
				
			if(randomSeedS11Specified)
				specified.append("s11");
			else
				notSpecified.append("s11");
				
			if(randomSeedS12Specified)
				specified.append("s12");
			else
				notSpecified.append("s12");

			if(randomSeedS20Specified)
				specified.append("s20");
			else
				notSpecified.append("s20");
				
			if(randomSeedS21Specified)
				specified.append("s21");
			else
				notSpecified.append("s21");
				
			if(randomSeedS22Specified)
				specified.append("s22");
			else
				notSpecified.append("s22");
				
			usage(nullptr, QString("If you want to specify the random seed, you need to specify all the 6 components. You specified %1, but you did not"
			 " specify %2.").arg(specified.join(translatedCommaSpace())).arg(notSpecified.join(translatedCommaSpace())));
			logFile.close();
			return 1;
		}
		
		/*if(randomSeedXSpecified != randomSeedYSpecified){
			if(randomSeedXSpecified){
				usage(nullptr, QString("If you want to specify the random seed, you need to specify both the X and the Y components, not only the X component"));
			}
			else{
				assert(randomSeedYSpecified);
				usage(nullptr, QString("If you want to specify the random seed, you need to specify both the X and the Y components, not only the Y component"));
			}
			logFile.close();
			return 1;
		}
		assert(randomSeedXSpecified==randomSeedYSpecified);
		if(randomSeedXSpecified){
			if(randomSeedX<=0 || randomSeedX>=MM){
				usage(nullptr, QString("Random seed X component must be at least 1 and at most %1").arg(MM-1));
				logFile.close();
				return 1;
			}
		}
		if(randomSeedYSpecified){
			if(randomSeedY<=0 || randomSeedY>=MMM){
				usage(nullptr, QString("Random seed Y component must be at least 1 and at most %1").arg(MMM-1));
				logFile.close();
				return 1;
			}
		}
		
		if(randomSeedXSpecified){
			assert(randomSeedYSpecified);
			if(randomSeedX>0 && randomSeedX<MM && randomSeedY>0 && randomSeedY<MMM){
				XX=randomSeedX;
				YY=randomSeedY;
			}
		}*/
		
		if(TIMETABLE_HTML_LEVEL>7 || TIMETABLE_HTML_LEVEL<0)
			TIMETABLE_HTML_LEVEL=2;
	
		bool t=gt.rules.read(nullptr, filename, true, initialDir);
		if(!t){
			cout<<"fet-cl: cannot read input file (not existing, in use, or incorrect file) - aborting"<<endl;
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
			out<<"Cannot read input file (not existing, in use, or incorrect file) - aborting"<<Qt::endl;
#else
			out<<"Cannot read input file (not existing, in use, or incorrect file) - aborting"<<endl;
#endif
			logFile.close();
			return 1;
		}
		
		//2019-09-21
		int count=0;
		for(int i=0; i<gt.rules.activitiesList.size(); i++){
			Activity* act=gt.rules.activitiesList[i];
			if(act->active)
				count++;
		}
		if(count<1){
			cout<<"Please input at least one active activity before generating"<<endl;
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
			out<<"Please input at least one active activity before generating"<<Qt::endl;
#else
			out<<"Please input at least one active activity before generating"<<endl;
#endif
			logFile.close();
			return 1;
		}
		
		t=gt.rules.computeInternalStructure(nullptr);
		if(!t){
			cout<<"Cannot compute internal structure - aborting"<<endl;
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
			out<<"Cannot compute internal structure - aborting"<<Qt::endl;
#else
			out<<"Cannot compute internal structure - aborting"<<endl;
#endif
			logFile.close();
			return 1;
		}
		
		terminateGeneratePointer=&gen;
		signal(SIGTERM, terminate);
#ifdef SIGBREAK
		signal(SIGBREAK, terminate);
#endif
		
		//https://stackoverflow.com/questions/6736536/c-input-and-output-to-the-console-window-at-the-same-time/31500127#31500127
		std::atomic<bool> run(true);
		std::thread pollFileThread(pollFile, std::ref(run));

		gen.abortOptimization=false;
		gen.restart=false;
		gen.paused=false;
		gen.pausedTime=0;
		bool ok=gen.precompute(nullptr, &initialOrderStream);
		
		initialOrderFile.close();
		
		if(!ok){
			cout<<"Cannot precompute - data is wrong - aborting"<<endl;
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
			out<<"Cannot precompute - data is wrong - aborting"<<Qt::endl;
#else
			out<<"Cannot precompute - data is wrong - aborting"<<endl;
#endif
			logFile.close();
			return 1;
		}
	
		bool restarted, impossible, timeExceeded;
		
		cout<<"Starting timetable generation..."<<endl;
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
		out<<"Starting timetable generation..."<<Qt::endl;
#else
		out<<"Starting timetable generation..."<<endl;
#endif
		if(VERBOSE){
			cout<<"secondsLimit=="<<secondsLimit<<endl;
		}
		//out<<"secondsLimit=="<<secondsLimit<<endl;
		
		TimetableExport::writeRandomSeedCommandLine(nullptr, gen.rng, outputDirectory, true); //true represents 'before' state

		gen.generate(secondsLimit, restarted, impossible, timeExceeded, false, &maxPlacedActivityStream); //false means no thread

		run.store(false);
		pollFileThread.join();
		
		maxPlacedActivityFile.close();
	
		if(impossible){
			cout<<"Impossible"<<endl;
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
			out<<"Impossible"<<Qt::endl;
#else
			out<<"Impossible"<<endl;
#endif
			
			//2016-11-17 - suggested by thanhnambkhn, FET will write the impossible activity and the current and highest-stage timetables
			//(which should be identical)

			Solution& cc=gen.c;

			//needed to find the conflicts strings
			FakeString tmp;
			cc.fitness(gt.rules, &tmp);

			/*TimetableExport::getStudentsTimetable(cc);
			TimetableExport::getTeachersTimetable(cc);
			TimetableExport::getRoomsTimetable(cc);*/
			TimetableExport::getStudentsTeachersRoomsBuildingsTimetable(cc);

			QString toc=tempOutputDirectory;
			if(toc!="")
				toc+=QString("-current");
			else
				toc=QString("current");
			
			if(QFileInfo::exists(toc)){
				int i=2;
				for(;;){
					QString CODN=toc+"-"+QString::number(i);
					if(!QFileInfo::exists(CODN)){
						toc=CODN;
						break;
					}
					i++;
				}
			}
			
			if(toc!="")
				if(!dir.exists(toc))
					dir.mkpath(toc);
			
			toc+=FILE_SEP;

			TimetableExport::writeGenerationResultsCommandLine(nullptr, toc);
			
			QString s;

			s+=TimetableExport::tr("Please check the constraints related to the "
			 "activity below, which might be impossible to schedule:");
			s+="\n\n";
			for(int i=0; i<gen.nDifficultActivities; i++){
				int ai=gen.difficultActivities[i];

				s+=TimetableExport::tr("No: %1").arg(i+1);

				s+=translatedCommaSpace();

				s+=TimetableExport::tr("Id: %1 (%2)", "%1 is id of activity, %2 is detailed description of activity")
					.arg(gt.rules.internalActivitiesList[ai].id)
					.arg(getActivityDetailedDescription(gt.rules, gt.rules.internalActivitiesList[ai].id));

				s+="\n";
			}

			QFile difficultActivitiesFile(logsDir+"difficult_activities.txt");
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
			bool t=difficultActivitiesFile.open(QIODeviceBase::WriteOnly);
#else
			bool t=difficultActivitiesFile.open(QIODevice::WriteOnly);
#endif
			if(!t){
				cout<<"FET critical - you don't have write permissions in the output directory - (FET cannot open or create file "<<qPrintable(QDir::toNativeSeparators(logsDir))<<"difficult_activities.txt)."
				 " If this is a bug - please report it."<<endl;
				return 1;
			}
			QTextStream difficultActivitiesOut(&difficultActivitiesFile);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
			difficultActivitiesOut.setEncoding(QStringConverter::Utf8);
#else
			difficultActivitiesOut.setCodec("UTF-8");
#endif
			difficultActivitiesOut.setGenerateByteOrderMark(true);
			
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
			difficultActivitiesOut<<s<<Qt::endl;
#else
			difficultActivitiesOut<<s<<endl;
#endif
			
			//2011-11-11 (2)
			//write highest stage timetable
			Solution& ch=gen.highestStageSolution;

			//needed to find the conflicts strings
			FakeString tmp2;
			ch.fitness(gt.rules, &tmp2);

			/*TimetableExport::getStudentsTimetable(ch);
			TimetableExport::getTeachersTimetable(ch);
			TimetableExport::getRoomsTimetable(ch);*/
			TimetableExport::getStudentsTeachersRoomsBuildingsTimetable(ch);

			QString toh=tempOutputDirectory;
			
			if(toh!="")
				toh+=QString("-highest");
			else if(toh=="")
				toh=QString("highest");
			
			if(QFileInfo::exists(toh)){
				int i=2;
				for(;;){
					QString CODN=toh+"-"+QString::number(i);
					if(!QFileInfo::exists(CODN)){
						toh=CODN;
						break;
					}
					i++;
				}
			}
			
			if(toh!="")
				if(!dir.exists(toh))
					dir.mkpath(toh);
			
			toh+=FILE_SEP;

			TimetableExport::writeGenerationResultsCommandLine(nullptr, toh);

			QString oldDir=OUTPUT_DIR;
			OUTPUT_DIR=csvOutputDirectory;
			Export::exportCSV(&gen.highestStageSolution, &gen.c);
			OUTPUT_DIR=oldDir;
		}
		//2012-01-24 - suggestion and code by Ian Holden (ian AT ianholden.com), to write best and current timetable on time exceeded
		//previously, FET saved best and current timetable only on receiving SIGTERM (or SIGBREAK, on Windows)
		//by Ian Holden (begin)
		else if(timeExceeded || gen.abortOptimization){
			if(timeExceeded){
				cout<<"Time exceeded"<<endl;
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
				out<<"Time exceeded"<<Qt::endl;
#else
				out<<"Time exceeded"<<endl;
#endif
			}
			else if(gen.abortOptimization){
				cout<<"Generation interrupted"<<endl;
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
				out<<"Generation interrupted"<<Qt::endl;
#else
				out<<"Generation interrupted"<<endl;
#endif
			}
			//by Ian Holden (end)
			
			//2011-11-11 (1)
			//write current stage timetable
			Solution& cc=gen.c;

			//needed to find the conflicts strings
			FakeString tmp;
			cc.fitness(gt.rules, &tmp);

			/*TimetableExport::getStudentsTimetable(cc);
			TimetableExport::getTeachersTimetable(cc);
			TimetableExport::getRoomsTimetable(cc);*/
			TimetableExport::getStudentsTeachersRoomsBuildingsTimetable(cc);

			QString toc=tempOutputDirectory;

			if(toc!="")
				toc+=QString("-current");
			else if(toc=="")
				toc=QString("current");
			
			if(QFileInfo::exists(toc)){
				int i=2;
				for(;;){
					QString CODN=toc+"-"+QString::number(i);
					if(!QFileInfo::exists(CODN)){
						toc=CODN;
						break;
					}
					i++;
				}
			}

			if(toc!="")
				if(!dir.exists(toc))
					dir.mkpath(toc);
			
			toc+=FILE_SEP;

			TimetableExport::writeGenerationResultsCommandLine(nullptr, toc);
			
			QString s;

			if(gen.maxActivitiesPlaced>=0 && gen.maxActivitiesPlaced<gt.rules.nInternalActivities
			 && initialOrderOfActivitiesIndices[gen.maxActivitiesPlaced]>=0 && initialOrderOfActivitiesIndices[gen.maxActivitiesPlaced]<gt.rules.nInternalActivities){
				s=FetTranslate::tr("FET managed to schedule correctly the first %1 most difficult activities."
				 " You can see the initial order of placing the activities in the corresponding output file. The activity which might cause problems"
				 " might be the next activity in the initial order of evaluation. This activity is listed below:").arg(gen.maxActivitiesPlaced);
				s+=QString("\n\n");
			
				int ai=initialOrderOfActivitiesIndices[gen.maxActivitiesPlaced];

				s+=FetTranslate::tr("Id: %1 (%2)", "%1 is id of activity, %2 is detailed description of activity")
				 .arg(gt.rules.internalActivitiesList[ai].id)
				 .arg(getActivityDetailedDescription(gt.rules, gt.rules.internalActivitiesList[ai].id));
			}
			else
				s=FetTranslate::tr("Difficult activity cannot be computed - please report possible bug");
			
			s+=QString("\n\n----------\n\n");
			
			s+=FetTranslate::tr("Here are the placed activities which led to an inconsistency, "
			 "in order, from the first one to the last one (the last one FET failed to schedule "
			 "and the last ones are most likely the difficult ones):");
			s+="\n\n";
			for(int i=0; i<gen.nDifficultActivities; i++){
				int ai=gen.difficultActivities[i];

				s+=FetTranslate::tr("No: %1").arg(i+1);
		
				s+=translatedCommaSpace();

				s+=FetTranslate::tr("Id: %1 (%2)", "%1 is id of activity, %2 is detailed description of activity")
					.arg(gt.rules.internalActivitiesList[ai].id)
					.arg(getActivityDetailedDescription(gt.rules, gt.rules.internalActivitiesList[ai].id));

				s+="\n";
			}
			
			QFile difficultActivitiesFile(logsDir+"difficult_activities.txt");
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
			bool t=difficultActivitiesFile.open(QIODeviceBase::WriteOnly);
#else
			bool t=difficultActivitiesFile.open(QIODevice::WriteOnly);
#endif
			if(!t){
				cout<<"FET critical - you don't have write permissions in the output directory - (FET cannot open or create file "<<qPrintable(QDir::toNativeSeparators(logsDir))<<"difficult_activities.txt)."
				 " If this is a bug - please report it."<<endl;
				return 1;
			}
			QTextStream difficultActivitiesOut(&difficultActivitiesFile);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
			difficultActivitiesOut.setEncoding(QStringConverter::Utf8);
#else
			difficultActivitiesOut.setCodec("UTF-8");
#endif
			difficultActivitiesOut.setGenerateByteOrderMark(true);
			
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
			difficultActivitiesOut<<s<<Qt::endl;
#else
			difficultActivitiesOut<<s<<endl;
#endif
			
			//2011-11-11 (2)
			//write highest stage timetable
			Solution& ch=gen.highestStageSolution;

			//needed to find the conflicts strings
			FakeString tmp2;
			ch.fitness(gt.rules, &tmp2);

			/*TimetableExport::getStudentsTimetable(ch);
			TimetableExport::getTeachersTimetable(ch);
			TimetableExport::getRoomsTimetable(ch);*/
			TimetableExport::getStudentsTeachersRoomsBuildingsTimetable(ch);

			QString toh=tempOutputDirectory;

			if(toh!="")
				toh+=QString("-highest");
			else if(toh=="")
				toh=QString("highest");

			if(QFileInfo::exists(toh)){
				int i=2;
				for(;;){
					QString CODN=toh+"-"+QString::number(i);
					if(!QFileInfo::exists(CODN)){
						toh=CODN;
						break;
					}
					i++;
				}
			}
			
			if(toh!="")
				if(!dir.exists(toh))
					dir.mkpath(toh);
			
			toh+=FILE_SEP;

			TimetableExport::writeGenerationResultsCommandLine(nullptr, toh);

			QString oldDir=OUTPUT_DIR;
			OUTPUT_DIR=csvOutputDirectory;
			Export::exportCSV(&gen.highestStageSolution, &gen.c);
			OUTPUT_DIR=oldDir;
		}
		else{
			cout<<"Generation successful"<<endl;
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
			out<<"Generation successful"<<Qt::endl;
#else
			out<<"Generation successful"<<endl;
#endif
		
			TimetableExport::writeRandomSeedCommandLine(nullptr, gen.rng, outputDirectory, false); //false represents 'before' state

			Solution& c=gen.c;

			//needed to find the conflicts strings
			FakeString tmp;
			c.fitness(gt.rules, &tmp);
			
			/*TimetableExport::getStudentsTimetable(c);
			TimetableExport::getTeachersTimetable(c);
			TimetableExport::getRoomsTimetable(c);*/
			TimetableExport::getStudentsTeachersRoomsBuildingsTimetable(c);

			TimetableExport::writeGenerationResultsCommandLine(nullptr, outputDirectory);
			
			QString oldDir=OUTPUT_DIR;
			OUTPUT_DIR=csvOutputDirectory;
			Export::exportCSV(&c);
			OUTPUT_DIR=oldDir;
		}
	
		logFile.close();
		return 0;
	}
	else{
		usage(nullptr, QString("No arguments given"));
		return 1;
	}
	//end command line
#endif
	/////////////////////////////////////////////////
}
