/*
File fetcl.cpp - this is where FET-CL starts
*/

/***************************************************************************
                          fetcl.cpp  -  description
                             -------------------
    begin                : 2026
    copyright            : (C) 2026 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "fetcl.h"

#include "matrix.h"

#include "messageboxes.h"

#include "rules.h"

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

Generate gen;

QString logsDir;
QString csvOutputDirectory;

#include <QCoreApplication>

//#include <QMutex>
#include <QString>
#include <QTranslator>

#include <QDir>
#include <QFileInfo>

#include <QTextStream>
#include <QFile>

#include <Qt>

#include <csignal>
#include <QtGlobal>

//#include <atomic>
//#include <thread>
//#include <chrono>

#include <iostream>

//extern Solution highestStageSolution;

//extern int maxActivitiesPlaced;

extern bool students_schedule_ready;
extern bool teachers_schedule_ready;
extern bool rooms_buildings_schedule_ready;

QString tempOutputDirectory;

//#ifndef FET_COM MAND_LINE
//extern QMutex myMutex;
//#else
//QMutex myMutex;
//#endif

class FetMainForm;

FetMainForm* pFetMainForm;

void updateFetMainFormAfterHistoryRestored(int iterationsBackward)
{
	Q_UNUSED(iterationsBackward);
}

void EngineMessageBox::critical(QWidget* parent, const QString& title, const QString& message)
{
	//should only appear in the interface
	Q_UNUSED(parent);
	Q_UNUSED(title);
	Q_UNUSED(message);
	assert(0);
}

void showStatusBarAutosaved()
{
	//should only appear in the interface
	assert(0);
}

bool generatePreMessage(QWidget* parent, const QString& s)
{
	int b=GeneratePreReconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s, GeneratePreTranslate::tr("Yes"), GeneratePreTranslate::tr("No"), QString(), 0, 1);
	if(b!=0)
		return false;
	else
		return true;
}

void setRulesModifiedAndOtherThings(Rules* rules)
{
	Q_UNUSED(rules);
}

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

//extern int XX;
//extern int YY;

Generate* terminateGeneratePointer;

#include "exportcl.h"

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
//#endif

//#ifdef FET_COM MAND_LINE

//https://stackoverflow.com/questions/6736536/c-input-and-output-to-the-console-window-at-the-same-time/31500127#31500127
//https://stackoverflow.com/questions/4184468/sleep-for-milliseconds
/*void pollFile(std::atomic<bool>& run)
{
	while(run.load()){
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		if(!terminateGeneratePointer->writeCurrentAndHighestTimetable && QFile::exists(communicationFile)){
			bool t=QFile::remove(communicationFile);
			if(t)
				terminateGeneratePointer->writeCurrentAndHighestTimetable=true;
		}
	}
}*/

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

		"\t--separatedaysnamelongnamebybreak=B1\n"
		"\t--separatehoursnamelongnamebybreak=B2\n"
		"\t--separatesubjectsnamelongnamecodebybreak=B3\n"
		"\t--separateactivitytagsnamelongnamecodebybreak=B4\n"
		"\t--separateteachersnamelongnamecodebybreak=B5\n"
		"\t--separatestudentsnamelongnamecodebybreak=B6\n"
		"\t--separatebuildingsnamelongnamecodebybreak=B7\n"
		"\t--separateroomsnamelongnamecodebybreak=B8\n"
		"\t\tB1 to B8 are either true or false (default false).\n"
		"\n"
		///////

		"\t--printsubjectscomments=B1\n"
		"\t--printactivitytagscomments=B2\n"
		"\t--printteacherscomments=B3\n"
		"\t--printsubgroupscomments=B4\n"
		"\t--printgroupscomments=B5\n"
		"\t--printyearscomments=B6\n"
		"\t--printbuildingscomments=B7\n"
		"\t--printroomscomments=B8\n"
		"\t\tB1 to B8 are either true or false (default false).\n"
		"\n"

		"\t--subgroupsdayshvprintdaysnames=B1\n"
		"\t--subgroupsdayshvprintdayslongnames=B2\n"
		"\t--subgroupsdayshvprinthoursnames=B3\n"
		"\t--subgroupsdayshvprinthourslongnames=B4\n"
		"\t--subgroupsdayshvprintsubjectsnames=B5\n"
		"\t--subgroupsdayshvprintsubjectslongnames=B6\n"
		"\t--subgroupsdayshvprintsubjectscodes=B7\n"
		"\t--subgroupsdayshvprintactivitytagsnames=B8\n"
		"\t--subgroupsdayshvprintactivitytagslongnames=B9\n"
		"\t--subgroupsdayshvprintactivitytagscodes=B10\n"
		"\t--subgroupsdayshvprintteachersnames=B11\n"
		"\t--subgroupsdayshvprintteacherslongnames=B12\n"
		"\t--subgroupsdayshvprintteacherscodes=B13\n"
		"\t--subgroupsdayshvprintstudentsnames=B14\n"
		"\t--subgroupsdayshvprintstudentslongnames=B15\n"
		"\t--subgroupsdayshvprintstudentscodes=B16\n"
		"\t--subgroupsdayshvprintroomsnames=B17\n"
		"\t--subgroupsdayshvprintroomslongnames=B18\n"
		"\t--subgroupsdayshvprintroomscodes=B19\n"
		"\t--subgroupsdayshvprintsubjectslegend=B20\n"
		"\t--subgroupsdayshvprintactivitytagslegend=B21\n"
		"\t--subgroupsdayshvprintteacherslegend=B22\n"
		"\t--subgroupsdayshvprintstudentslegend=B23\n"
		"\t--subgroupsdayshvprintroomslegend=B24\n"
		"\t--subgroupsdayshvprintlegendcodesfirst=B25\n"
		"\t\tB1 to B25 are either true or false (B1, B3, B5, B8, B11, B14, B17 are by default true, the rest are by default false).\n"
		"\t\t('hv' means 'horizontal and vertical'.)\n"
		"\n"
		"\t--subgroupstimehvprintdaysnames=B1\n"
		"\t--subgroupstimehvprintdayslongnames=B2\n"
		"\t--subgroupstimehvprinthoursnames=B3\n"
		"\t--subgroupstimehvprinthourslongnames=B4\n"
		"\t--subgroupstimehvprintsubjectsnames=B5\n"
		"\t--subgroupstimehvprintsubjectslongnames=B6\n"
		"\t--subgroupstimehvprintsubjectscodes=B7\n"
		"\t--subgroupstimehvprintactivitytagsnames=B8\n"
		"\t--subgroupstimehvprintactivitytagslongnames=B9\n"
		"\t--subgroupstimehvprintactivitytagscodes=B10\n"
		"\t--subgroupstimehvprintteachersnames=B11\n"
		"\t--subgroupstimehvprintteacherslongnames=B12\n"
		"\t--subgroupstimehvprintteacherscodes=B13\n"
		"\t--subgroupstimehvprintstudentsnames=B14\n"
		"\t--subgroupstimehvprintstudentslongnames=B15\n"
		"\t--subgroupstimehvprintstudentscodes=B16\n"
		"\t--subgroupstimehvprintroomsnames=B17\n"
		"\t--subgroupstimehvprintroomslongnames=B18\n"
		"\t--subgroupstimehvprintroomscodes=B19\n"
		"\t--subgroupstimehvprintsubjectslegend=B20\n"
		"\t--subgroupstimehvprintactivitytagslegend=B21\n"
		"\t--subgroupstimehvprintteacherslegend=B22\n"
		"\t--subgroupstimehvprintstudentslegend=B23\n"
		"\t--subgroupstimehvprintroomslegend=B24\n"
		"\t--subgroupstimehvprintlegendcodesfirst=B25\n"
		"\t\tB1 to B25 are either true or false (B1, B3, B5, B8, B11, B14, B17 are by default true, the rest are by default false).\n"
		"\t\t('hv' means 'horizontal and vertical'.)\n"
		"\n"

		"\t--groupsdayshvprintdaysnames=B1\n"
		"\t--groupsdayshvprintdayslongnames=B2\n"
		"\t--groupsdayshvprinthoursnames=B3\n"
		"\t--groupsdayshvprinthourslongnames=B4\n"
		"\t--groupsdayshvprintsubjectsnames=B5\n"
		"\t--groupsdayshvprintsubjectslongnames=B6\n"
		"\t--groupsdayshvprintsubjectscodes=B7\n"
		"\t--groupsdayshvprintactivitytagsnames=B8\n"
		"\t--groupsdayshvprintactivitytagslongnames=B9\n"
		"\t--groupsdayshvprintactivitytagscodes=B10\n"
		"\t--groupsdayshvprintteachersnames=B11\n"
		"\t--groupsdayshvprintteacherslongnames=B12\n"
		"\t--groupsdayshvprintteacherscodes=B13\n"
		"\t--groupsdayshvprintstudentsnames=B14\n"
		"\t--groupsdayshvprintstudentslongnames=B15\n"
		"\t--groupsdayshvprintstudentscodes=B16\n"
		"\t--groupsdayshvprintroomsnames=B17\n"
		"\t--groupsdayshvprintroomslongnames=B18\n"
		"\t--groupsdayshvprintroomscodes=B19\n"
		"\t--groupsdayshvprintsubjectslegend=B20\n"
		"\t--groupsdayshvprintactivitytagslegend=B21\n"
		"\t--groupsdayshvprintteacherslegend=B22\n"
		"\t--groupsdayshvprintstudentslegend=B23\n"
		"\t--groupsdayshvprintroomslegend=B24\n"
		"\t--groupsdayshvprintlegendcodesfirst=B25\n"
		"\t\tB1 to B25 are either true or false (B1, B3, B5, B8, B11, B14, B17 are by default true, the rest are by default false).\n"
		"\t\t('hv' means 'horizontal and vertical'.)\n"
		"\n"
		"\t--groupstimehvprintdaysnames=B1\n"
		"\t--groupstimehvprintdayslongnames=B2\n"
		"\t--groupstimehvprinthoursnames=B3\n"
		"\t--groupstimehvprinthourslongnames=B4\n"
		"\t--groupstimehvprintsubjectsnames=B5\n"
		"\t--groupstimehvprintsubjectslongnames=B6\n"
		"\t--groupstimehvprintsubjectscodes=B7\n"
		"\t--groupstimehvprintactivitytagsnames=B8\n"
		"\t--groupstimehvprintactivitytagslongnames=B9\n"
		"\t--groupstimehvprintactivitytagscodes=B10\n"
		"\t--groupstimehvprintteachersnames=B11\n"
		"\t--groupstimehvprintteacherslongnames=B12\n"
		"\t--groupstimehvprintteacherscodes=B13\n"
		"\t--groupstimehvprintstudentsnames=B14\n"
		"\t--groupstimehvprintstudentslongnames=B15\n"
		"\t--groupstimehvprintstudentscodes=B16\n"
		"\t--groupstimehvprintroomsnames=B17\n"
		"\t--groupstimehvprintroomslongnames=B18\n"
		"\t--groupstimehvprintroomscodes=B19\n"
		"\t--groupstimehvprintsubjectslegend=B20\n"
		"\t--groupstimehvprintactivitytagslegend=B21\n"
		"\t--groupstimehvprintteacherslegend=B22\n"
		"\t--groupstimehvprintstudentslegend=B23\n"
		"\t--groupstimehvprintroomslegend=B24\n"
		"\t--groupstimehvprintlegendcodesfirst=B25\n"
		"\t\tB1 to B25 are either true or false (B1, B3, B5, B8, B11, B14, B17 are by default true, the rest are by default false).\n"
		"\t\t('hv' means 'horizontal and vertical'.)\n"
		"\n"

		"\t--yearsdayshvprintdaysnames=B1\n"
		"\t--yearsdayshvprintdayslongnames=B2\n"
		"\t--yearsdayshvprinthoursnames=B3\n"
		"\t--yearsdayshvprinthourslongnames=B4\n"
		"\t--yearsdayshvprintsubjectsnames=B5\n"
		"\t--yearsdayshvprintsubjectslongnames=B6\n"
		"\t--yearsdayshvprintsubjectscodes=B7\n"
		"\t--yearsdayshvprintactivitytagsnames=B8\n"
		"\t--yearsdayshvprintactivitytagslongnames=B9\n"
		"\t--yearsdayshvprintactivitytagscodes=B10\n"
		"\t--yearsdayshvprintteachersnames=B11\n"
		"\t--yearsdayshvprintteacherslongnames=B12\n"
		"\t--yearsdayshvprintteacherscodes=B13\n"
		"\t--yearsdayshvprintstudentsnames=B14\n"
		"\t--yearsdayshvprintstudentslongnames=B15\n"
		"\t--yearsdayshvprintstudentscodes=B16\n"
		"\t--yearsdayshvprintroomsnames=B17\n"
		"\t--yearsdayshvprintroomslongnames=B18\n"
		"\t--yearsdayshvprintroomscodes=B19\n"
		"\t--yearsdayshvprintsubjectslegend=B20\n"
		"\t--yearsdayshvprintactivitytagslegend=B21\n"
		"\t--yearsdayshvprintteacherslegend=B22\n"
		"\t--yearsdayshvprintstudentslegend=B23\n"
		"\t--yearsdayshvprintroomslegend=B24\n"
		"\t--yearsdayshvprintlegendcodesfirst=B25\n"
		"\t\tB1 to B25 are either true or false (B1, B3, B5, B8, B11, B14, B17 are by default true, the rest are by default false).\n"
		"\t\t('hv' means 'horizontal and vertical'.)\n"
		"\n"
		"\t--yearstimehvprintdaysnames=B1\n"
		"\t--yearstimehvprintdayslongnames=B2\n"
		"\t--yearstimehvprinthoursnames=B3\n"
		"\t--yearstimehvprinthourslongnames=B4\n"
		"\t--yearstimehvprintsubjectsnames=B5\n"
		"\t--yearstimehvprintsubjectslongnames=B6\n"
		"\t--yearstimehvprintsubjectscodes=B7\n"
		"\t--yearstimehvprintactivitytagsnames=B8\n"
		"\t--yearstimehvprintactivitytagslongnames=B9\n"
		"\t--yearstimehvprintactivitytagscodes=B10\n"
		"\t--yearstimehvprintteachersnames=B11\n"
		"\t--yearstimehvprintteacherslongnames=B12\n"
		"\t--yearstimehvprintteacherscodes=B13\n"
		"\t--yearstimehvprintstudentsnames=B14\n"
		"\t--yearstimehvprintstudentslongnames=B15\n"
		"\t--yearstimehvprintstudentscodes=B16\n"
		"\t--yearstimehvprintroomsnames=B17\n"
		"\t--yearstimehvprintroomslongnames=B18\n"
		"\t--yearstimehvprintroomscodes=B19\n"
		"\t--yearstimehvprintsubjectslegend=B20\n"
		"\t--yearstimehvprintactivitytagslegend=B21\n"
		"\t--yearstimehvprintteacherslegend=B22\n"
		"\t--yearstimehvprintstudentslegend=B23\n"
		"\t--yearstimehvprintroomslegend=B24\n"
		"\t--yearstimehvprintlegendcodesfirst=B25\n"
		"\t\tB1 to B25 are either true or false (B1, B3, B5, B8, B11, B14, B17 are by default true, the rest are by default false).\n"
		"\t\t('hv' means 'horizontal and vertical'.)\n"
		"\n"

		"\t--teachersdayshvprintdaysnames=B1\n"
		"\t--teachersdayshvprintdayslongnames=B2\n"
		"\t--teachersdayshvprinthoursnames=B3\n"
		"\t--teachersdayshvprinthourslongnames=B4\n"
		"\t--teachersdayshvprintsubjectsnames=B5\n"
		"\t--teachersdayshvprintsubjectslongnames=B6\n"
		"\t--teachersdayshvprintsubjectscodes=B7\n"
		"\t--teachersdayshvprintactivitytagsnames=B8\n"
		"\t--teachersdayshvprintactivitytagslongnames=B9\n"
		"\t--teachersdayshvprintactivitytagscodes=B10\n"
		"\t--teachersdayshvprintteachersnames=B11\n"
		"\t--teachersdayshvprintteacherslongnames=B12\n"
		"\t--teachersdayshvprintteacherscodes=B13\n"
		"\t--teachersdayshvprintstudentsnames=B14\n"
		"\t--teachersdayshvprintstudentslongnames=B15\n"
		"\t--teachersdayshvprintstudentscodes=B16\n"
		"\t--teachersdayshvprintroomsnames=B17\n"
		"\t--teachersdayshvprintroomslongnames=B18\n"
		"\t--teachersdayshvprintroomscodes=B19\n"
		"\t--teachersdayshvprintsubjectslegend=B20\n"
		"\t--teachersdayshvprintactivitytagslegend=B21\n"
		"\t--teachersdayshvprintteacherslegend=B22\n"
		"\t--teachersdayshvprintstudentslegend=B23\n"
		"\t--teachersdayshvprintroomslegend=B24\n"
		"\t--teachersdayshvprintlegendcodesfirst=B25\n"
		"\t\tB1 to B25 are either true or false (B1, B3, B5, B8, B11, B14, B17 are by default true, the rest are by default false).\n"
		"\t\t('hv' means 'horizontal and vertical'.)\n"
		"\n"
		"\t--teacherstimehvprintdaysnames=B1\n"
		"\t--teacherstimehvprintdayslongnames=B2\n"
		"\t--teacherstimehvprinthoursnames=B3\n"
		"\t--teacherstimehvprinthourslongnames=B4\n"
		"\t--teacherstimehvprintsubjectsnames=B5\n"
		"\t--teacherstimehvprintsubjectslongnames=B6\n"
		"\t--teacherstimehvprintsubjectscodes=B7\n"
		"\t--teacherstimehvprintactivitytagsnames=B8\n"
		"\t--teacherstimehvprintactivitytagslongnames=B9\n"
		"\t--teacherstimehvprintactivitytagscodes=B10\n"
		"\t--teacherstimehvprintteachersnames=B11\n"
		"\t--teacherstimehvprintteacherslongnames=B12\n"
		"\t--teacherstimehvprintteacherscodes=B13\n"
		"\t--teacherstimehvprintstudentsnames=B14\n"
		"\t--teacherstimehvprintstudentslongnames=B15\n"
		"\t--teacherstimehvprintstudentscodes=B16\n"
		"\t--teacherstimehvprintroomsnames=B17\n"
		"\t--teacherstimehvprintroomslongnames=B18\n"
		"\t--teacherstimehvprintroomscodes=B19\n"
		"\t--teacherstimehvprintsubjectslegend=B20\n"
		"\t--teacherstimehvprintactivitytagslegend=B21\n"
		"\t--teacherstimehvprintteacherslegend=B22\n"
		"\t--teacherstimehvprintstudentslegend=B23\n"
		"\t--teacherstimehvprintroomslegend=B24\n"
		"\t--teacherstimehvprintlegendcodesfirst=B25\n"
		"\t\tB1 to B25 are either true or false (B1, B3, B5, B8, B11, B14, B17 are by default true, the rest are by default false).\n"
		"\t\t('hv' means 'horizontal and vertical'.)\n"
		"\n"

		"\t--roomsdayshvprintdaysnames=B1\n"
		"\t--roomsdayshvprintdayslongnames=B2\n"
		"\t--roomsdayshvprinthoursnames=B3\n"
		"\t--roomsdayshvprinthourslongnames=B4\n"
		"\t--roomsdayshvprintsubjectsnames=B5\n"
		"\t--roomsdayshvprintsubjectslongnames=B6\n"
		"\t--roomsdayshvprintsubjectscodes=B7\n"
		"\t--roomsdayshvprintactivitytagsnames=B8\n"
		"\t--roomsdayshvprintactivitytagslongnames=B9\n"
		"\t--roomsdayshvprintactivitytagscodes=B10\n"
		"\t--roomsdayshvprintteachersnames=B11\n"
		"\t--roomsdayshvprintteacherslongnames=B12\n"
		"\t--roomsdayshvprintteacherscodes=B13\n"
		"\t--roomsdayshvprintstudentsnames=B14\n"
		"\t--roomsdayshvprintstudentslongnames=B15\n"
		"\t--roomsdayshvprintstudentscodes=B16\n"
		"\t--roomsdayshvprintroomsnames=B17\n"
		"\t--roomsdayshvprintroomslongnames=B18\n"
		"\t--roomsdayshvprintroomscodes=B19\n"
		"\t--roomsdayshvprintsubjectslegend=B20\n"
		"\t--roomsdayshvprintactivitytagslegend=B21\n"
		"\t--roomsdayshvprintteacherslegend=B22\n"
		"\t--roomsdayshvprintstudentslegend=B23\n"
		"\t--roomsdayshvprintroomslegend=B24\n"
		"\t--roomsdayshvprintlegendcodesfirst=B25\n"
		"\t\tB1 to B25 are either true or false (B1, B3, B5, B8, B11, B14, B17 are by default true, the rest are by default false).\n"
		"\t\t('hv' means 'horizontal and vertical'.)\n"
		"\n"
		"\t--roomstimehvprintdaysnames=B1\n"
		"\t--roomstimehvprintdayslongnames=B2\n"
		"\t--roomstimehvprinthoursnames=B3\n"
		"\t--roomstimehvprinthourslongnames=B4\n"
		"\t--roomstimehvprintsubjectsnames=B5\n"
		"\t--roomstimehvprintsubjectslongnames=B6\n"
		"\t--roomstimehvprintsubjectscodes=B7\n"
		"\t--roomstimehvprintactivitytagsnames=B8\n"
		"\t--roomstimehvprintactivitytagslongnames=B9\n"
		"\t--roomstimehvprintactivitytagscodes=B10\n"
		"\t--roomstimehvprintteachersnames=B11\n"
		"\t--roomstimehvprintteacherslongnames=B12\n"
		"\t--roomstimehvprintteacherscodes=B13\n"
		"\t--roomstimehvprintstudentsnames=B14\n"
		"\t--roomstimehvprintstudentslongnames=B15\n"
		"\t--roomstimehvprintstudentscodes=B16\n"
		"\t--roomstimehvprintroomsnames=B17\n"
		"\t--roomstimehvprintroomslongnames=B18\n"
		"\t--roomstimehvprintroomscodes=B19\n"
		"\t--roomstimehvprintsubjectslegend=B20\n"
		"\t--roomstimehvprintactivitytagslegend=B21\n"
		"\t--roomstimehvprintteacherslegend=B22\n"
		"\t--roomstimehvprintstudentslegend=B23\n"
		"\t--roomstimehvprintroomslegend=B24\n"
		"\t--roomstimehvprintlegendcodesfirst=B25\n"
		"\t\tB1 to B25 are either true or false (B1, B3, B5, B8, B11, B14, B17 are by default true, the rest are by default false).\n"
		"\t\t('hv' means 'horizontal and vertical'.)\n"
		"\n"

		"\t--buildingsdayshvprintdaysnames=B1\n"
		"\t--buildingsdayshvprintdayslongnames=B2\n"
		"\t--buildingsdayshvprinthoursnames=B3\n"
		"\t--buildingsdayshvprinthourslongnames=B4\n"
		"\t--buildingsdayshvprintsubjectsnames=B5\n"
		"\t--buildingsdayshvprintsubjectslongnames=B6\n"
		"\t--buildingsdayshvprintsubjectscodes=B7\n"
		"\t--buildingsdayshvprintactivitytagsnames=B8\n"
		"\t--buildingsdayshvprintactivitytagslongnames=B9\n"
		"\t--buildingsdayshvprintactivitytagscodes=B10\n"
		"\t--buildingsdayshvprintteachersnames=B11\n"
		"\t--buildingsdayshvprintteacherslongnames=B12\n"
		"\t--buildingsdayshvprintteacherscodes=B13\n"
		"\t--buildingsdayshvprintstudentsnames=B14\n"
		"\t--buildingsdayshvprintstudentslongnames=B15\n"
		"\t--buildingsdayshvprintstudentscodes=B16\n"
		"\t--buildingsdayshvprintroomsnames=B17\n"
		"\t--buildingsdayshvprintroomslongnames=B18\n"
		"\t--buildingsdayshvprintroomscodes=B19\n"
		"\t--buildingsdayshvprintsubjectslegend=B20\n"
		"\t--buildingsdayshvprintactivitytagslegend=B21\n"
		"\t--buildingsdayshvprintteacherslegend=B22\n"
		"\t--buildingsdayshvprintstudentslegend=B23\n"
		"\t--buildingsdayshvprintroomslegend=B24\n"
		"\t--buildingsdayshvprintlegendcodesfirst=B25\n"
		"\t\tB1 to B25 are either true or false (B1, B3, B5, B8, B11, B14, B17 are by default true, the rest are by default false).\n"
		"\t\t('hv' means 'horizontal and vertical'.)\n"
		"\n"
		"\t--buildingstimehvprintdaysnames=B1\n"
		"\t--buildingstimehvprintdayslongnames=B2\n"
		"\t--buildingstimehvprinthoursnames=B3\n"
		"\t--buildingstimehvprinthourslongnames=B4\n"
		"\t--buildingstimehvprintsubjectsnames=B5\n"
		"\t--buildingstimehvprintsubjectslongnames=B6\n"
		"\t--buildingstimehvprintsubjectscodes=B7\n"
		"\t--buildingstimehvprintactivitytagsnames=B8\n"
		"\t--buildingstimehvprintactivitytagslongnames=B9\n"
		"\t--buildingstimehvprintactivitytagscodes=B10\n"
		"\t--buildingstimehvprintteachersnames=B11\n"
		"\t--buildingstimehvprintteacherslongnames=B12\n"
		"\t--buildingstimehvprintteacherscodes=B13\n"
		"\t--buildingstimehvprintstudentsnames=B14\n"
		"\t--buildingstimehvprintstudentslongnames=B15\n"
		"\t--buildingstimehvprintstudentscodes=B16\n"
		"\t--buildingstimehvprintroomsnames=B17\n"
		"\t--buildingstimehvprintroomslongnames=B18\n"
		"\t--buildingstimehvprintroomscodes=B19\n"
		"\t--buildingstimehvprintsubjectslegend=B20\n"
		"\t--buildingstimehvprintactivitytagslegend=B21\n"
		"\t--buildingstimehvprintteacherslegend=B22\n"
		"\t--buildingstimehvprintstudentslegend=B23\n"
		"\t--buildingstimehvprintroomslegend=B24\n"
		"\t--buildingstimehvprintlegendcodesfirst=B25\n"
		"\t\tB1 to B25 are either true or false (B1, B3, B5, B8, B11, B14, B17 are by default true, the rest are by default false).\n"
		"\t\t('hv' means 'horizontal and vertical'.)\n"
		"\n"

		"\t--subjectsdayshvprintdaysnames=B1\n"
		"\t--subjectsdayshvprintdayslongnames=B2\n"
		"\t--subjectsdayshvprinthoursnames=B3\n"
		"\t--subjectsdayshvprinthourslongnames=B4\n"
		"\t--subjectsdayshvprintsubjectsnames=B5\n"
		"\t--subjectsdayshvprintsubjectslongnames=B6\n"
		"\t--subjectsdayshvprintsubjectscodes=B7\n"
		"\t--subjectsdayshvprintactivitytagsnames=B8\n"
		"\t--subjectsdayshvprintactivitytagslongnames=B9\n"
		"\t--subjectsdayshvprintactivitytagscodes=B10\n"
		"\t--subjectsdayshvprintteachersnames=B11\n"
		"\t--subjectsdayshvprintteacherslongnames=B12\n"
		"\t--subjectsdayshvprintteacherscodes=B13\n"
		"\t--subjectsdayshvprintstudentsnames=B14\n"
		"\t--subjectsdayshvprintstudentslongnames=B15\n"
		"\t--subjectsdayshvprintstudentscodes=B16\n"
		"\t--subjectsdayshvprintroomsnames=B17\n"
		"\t--subjectsdayshvprintroomslongnames=B18\n"
		"\t--subjectsdayshvprintroomscodes=B19\n"
		"\t--subjectsdayshvprintsubjectslegend=B20\n"
		"\t--subjectsdayshvprintactivitytagslegend=B21\n"
		"\t--subjectsdayshvprintteacherslegend=B22\n"
		"\t--subjectsdayshvprintstudentslegend=B23\n"
		"\t--subjectsdayshvprintroomslegend=B24\n"
		"\t--subjectsdayshvprintlegendcodesfirst=B25\n"
		"\t\tB1 to B25 are either true or false (B1, B3, B5, B8, B11, B14, B17 are by default true, the rest are by default false).\n"
		"\t\t('hv' means 'horizontal and vertical'.)\n"
		"\n"
		"\t--subjectstimehvprintdaysnames=B1\n"
		"\t--subjectstimehvprintdayslongnames=B2\n"
		"\t--subjectstimehvprinthoursnames=B3\n"
		"\t--subjectstimehvprinthourslongnames=B4\n"
		"\t--subjectstimehvprintsubjectsnames=B5\n"
		"\t--subjectstimehvprintsubjectslongnames=B6\n"
		"\t--subjectstimehvprintsubjectscodes=B7\n"
		"\t--subjectstimehvprintactivitytagsnames=B8\n"
		"\t--subjectstimehvprintactivitytagslongnames=B9\n"
		"\t--subjectstimehvprintactivitytagscodes=B10\n"
		"\t--subjectstimehvprintteachersnames=B11\n"
		"\t--subjectstimehvprintteacherslongnames=B12\n"
		"\t--subjectstimehvprintteacherscodes=B13\n"
		"\t--subjectstimehvprintstudentsnames=B14\n"
		"\t--subjectstimehvprintstudentslongnames=B15\n"
		"\t--subjectstimehvprintstudentscodes=B16\n"
		"\t--subjectstimehvprintroomsnames=B17\n"
		"\t--subjectstimehvprintroomslongnames=B18\n"
		"\t--subjectstimehvprintroomscodes=B19\n"
		"\t--subjectstimehvprintsubjectslegend=B20\n"
		"\t--subjectstimehvprintactivitytagslegend=B21\n"
		"\t--subjectstimehvprintteacherslegend=B22\n"
		"\t--subjectstimehvprintstudentslegend=B23\n"
		"\t--subjectstimehvprintroomslegend=B24\n"
		"\t--subjectstimehvprintlegendcodesfirst=B25\n"
		"\t\tB1 to B25 are either true or false (B1, B3, B5, B8, B11, B14, B17 are by default true, the rest are by default false).\n"
		"\t\t('hv' means 'horizontal and vertical'.)\n"
		"\n"

		"\t--activitytagsdayshvprintdaysnames=B1\n"
		"\t--activitytagsdayshvprintdayslongnames=B2\n"
		"\t--activitytagsdayshvprinthoursnames=B3\n"
		"\t--activitytagsdayshvprinthourslongnames=B4\n"
		"\t--activitytagsdayshvprintsubjectsnames=B5\n"
		"\t--activitytagsdayshvprintsubjectslongnames=B6\n"
		"\t--activitytagsdayshvprintsubjectscodes=B7\n"
		"\t--activitytagsdayshvprintactivitytagsnames=B8\n"
		"\t--activitytagsdayshvprintactivitytagslongnames=B9\n"
		"\t--activitytagsdayshvprintactivitytagscodes=B10\n"
		"\t--activitytagsdayshvprintteachersnames=B11\n"
		"\t--activitytagsdayshvprintteacherslongnames=B12\n"
		"\t--activitytagsdayshvprintteacherscodes=B13\n"
		"\t--activitytagsdayshvprintstudentsnames=B14\n"
		"\t--activitytagsdayshvprintstudentslongnames=B15\n"
		"\t--activitytagsdayshvprintstudentscodes=B16\n"
		"\t--activitytagsdayshvprintroomsnames=B17\n"
		"\t--activitytagsdayshvprintroomslongnames=B18\n"
		"\t--activitytagsdayshvprintroomscodes=B19\n"
		"\t--activitytagsdayshvprintsubjectslegend=B20\n"
		"\t--activitytagsdayshvprintactivitytagslegend=B21\n"
		"\t--activitytagsdayshvprintteacherslegend=B22\n"
		"\t--activitytagsdayshvprintstudentslegend=B23\n"
		"\t--activitytagsdayshvprintroomslegend=B24\n"
		"\t--activitytagsdayshvprintlegendcodesfirst=B25\n"
		"\t\tB1 to B25 are either true or false (B1, B3, B5, B8, B11, B14, B17 are by default true, the rest are by default false).\n"
		"\t\t('hv' means 'horizontal and vertical'.)\n"
		"\n"
		"\t--activitytagstimehvprintdaysnames=B1\n"
		"\t--activitytagstimehvprintdayslongnames=B2\n"
		"\t--activitytagstimehvprinthoursnames=B3\n"
		"\t--activitytagstimehvprinthourslongnames=B4\n"
		"\t--activitytagstimehvprintsubjectsnames=B5\n"
		"\t--activitytagstimehvprintsubjectslongnames=B6\n"
		"\t--activitytagstimehvprintsubjectscodes=B7\n"
		"\t--activitytagstimehvprintactivitytagsnames=B8\n"
		"\t--activitytagstimehvprintactivitytagslongnames=B9\n"
		"\t--activitytagstimehvprintactivitytagscodes=B10\n"
		"\t--activitytagstimehvprintteachersnames=B11\n"
		"\t--activitytagstimehvprintteacherslongnames=B12\n"
		"\t--activitytagstimehvprintteacherscodes=B13\n"
		"\t--activitytagstimehvprintstudentsnames=B14\n"
		"\t--activitytagstimehvprintstudentslongnames=B15\n"
		"\t--activitytagstimehvprintstudentscodes=B16\n"
		"\t--activitytagstimehvprintroomsnames=B17\n"
		"\t--activitytagstimehvprintroomslongnames=B18\n"
		"\t--activitytagstimehvprintroomscodes=B19\n"
		"\t--activitytagstimehvprintsubjectslegend=B20\n"
		"\t--activitytagstimehvprintactivitytagslegend=B21\n"
		"\t--activitytagstimehvprintteacherslegend=B22\n"
		"\t--activitytagstimehvprintstudentslegend=B23\n"
		"\t--activitytagstimehvprintroomslegend=B24\n"
		"\t--activitytagstimehvprintlegendcodesfirst=B25\n"
		"\t\tB1 to B25 are either true or false (B1, B3, B5, B8, B11, B14, B17 are by default true, the rest are by default false).\n"
		"\t\t('hv' means 'horizontal and vertical'.)\n"
		"\n"

		"\t--activitiesdayshvprintdaysnames=B1\n"
		"\t--activitiesdayshvprintdayslongnames=B2\n"
		"\t--activitiesdayshvprinthoursnames=B3\n"
		"\t--activitiesdayshvprinthourslongnames=B4\n"
		"\t--activitiesdayshvprintsubjectsnames=B5\n"
		"\t--activitiesdayshvprintsubjectslongnames=B6\n"
		"\t--activitiesdayshvprintsubjectscodes=B7\n"
		"\t--activitiesdayshvprintactivitytagsnames=B8\n"
		"\t--activitiesdayshvprintactivitytagslongnames=B9\n"
		"\t--activitiesdayshvprintactivitytagscodes=B10\n"
		"\t--activitiesdayshvprintteachersnames=B11\n"
		"\t--activitiesdayshvprintteacherslongnames=B12\n"
		"\t--activitiesdayshvprintteacherscodes=B13\n"
		"\t--activitiesdayshvprintstudentsnames=B14\n"
		"\t--activitiesdayshvprintstudentslongnames=B15\n"
		"\t--activitiesdayshvprintstudentscodes=B16\n"
		"\t--activitiesdayshvprintroomsnames=B17\n"
		"\t--activitiesdayshvprintroomslongnames=B18\n"
		"\t--activitiesdayshvprintroomscodes=B19\n"
		"\t--activitiesdayshvprintsubjectslegend=B20\n"
		"\t--activitiesdayshvprintactivitytagslegend=B21\n"
		"\t--activitiesdayshvprintteacherslegend=B22\n"
		"\t--activitiesdayshvprintstudentslegend=B23\n"
		"\t--activitiesdayshvprintroomslegend=B24\n"
		"\t--activitiesdayshvprintlegendcodesfirst=B25\n"
		"\t\tB1 to B25 are either true or false (B1, B3, B5, B8, B11, B14, B17 are by default true, the rest are by default false).\n"
		"\t\t('hv' means 'horizontal and vertical'.)\n"
		"\n"
		"\t--activitiestimehvprintdaysnames=B1\n"
		"\t--activitiestimehvprintdayslongnames=B2\n"
		"\t--activitiestimehvprinthoursnames=B3\n"
		"\t--activitiestimehvprinthourslongnames=B4\n"
		"\t--activitiestimehvprintsubjectsnames=B5\n"
		"\t--activitiestimehvprintsubjectslongnames=B6\n"
		"\t--activitiestimehvprintsubjectscodes=B7\n"
		"\t--activitiestimehvprintactivitytagsnames=B8\n"
		"\t--activitiestimehvprintactivitytagslongnames=B9\n"
		"\t--activitiestimehvprintactivitytagscodes=B10\n"
		"\t--activitiestimehvprintteachersnames=B11\n"
		"\t--activitiestimehvprintteacherslongnames=B12\n"
		"\t--activitiestimehvprintteacherscodes=B13\n"
		"\t--activitiestimehvprintstudentsnames=B14\n"
		"\t--activitiestimehvprintstudentslongnames=B15\n"
		"\t--activitiestimehvprintstudentscodes=B16\n"
		"\t--activitiestimehvprintroomsnames=B17\n"
		"\t--activitiestimehvprintroomslongnames=B18\n"
		"\t--activitiestimehvprintroomscodes=B19\n"
		"\t--activitiestimehvprintsubjectslegend=B20\n"
		"\t--activitiestimehvprintactivitytagslegend=B21\n"
		"\t--activitiestimehvprintteacherslegend=B22\n"
		"\t--activitiestimehvprintstudentslegend=B23\n"
		"\t--activitiestimehvprintroomslegend=B24\n"
		"\t--activitiestimehvprintlegendcodesfirst=B25\n"
		"\t\tB1 to B25 are either true or false (B1, B3, B5, B8, B11, B14, B17 are by default true, the rest are by default false).\n"
		"\t\t('hv' means 'horizontal and vertical'.)\n"
		"\n"

		//2026-03-11 begin
		"\t--teachersfreeperiodsprintdaysnames=B1\n"
		"\t--teachersfreeperiodsprintdayslongnames=B2\n"
		"\t--teachersfreeperiodsprinthoursnames=B3\n"
		"\t--teachersfreeperiodsprinthourslongnames=B4\n"
		"\t--teachersfreeperiodsprintteachersnames=B5\n"
		"\t--teachersfreeperiodsprintteacherslongnames=B6\n"
		"\t--teachersfreeperiodsprintteacherscodes=B7\n"
		"\t\tB1 to B7 are either true or false (B1, B3, B5 are by default true, the rest are by default false).\n"
		"\n"

		"\t--statisticsprintteachersnames=B1\n"
		"\t--statisticsprintteacherslongnames=B2\n"
		"\t--statisticsprintteacherscodes=B3\n"
		"\t\tB1 to B3 are either true or false (B1 is by default true, the rest are by default false).\n"
		"\n"

		"\t--statisticsprintstudentsnames=B1\n"
		"\t--statisticsprintstudentslongnames=B2\n"
		"\t--statisticsprintstudentscodes=B3\n"
		"\t\tB1 to B3 are either true or false (B1 is by default true, the rest are by default false).\n"
		"\n"
		//2026-03-11 end

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
		"If you create a file named exactly 'sigwrite' in the root output directory, the program will try to remove this file and, if the file was successfully removed, "
		"it will output the current and the highest stage timetables without stopping the generation (a poll for the existence of this file is done once every second)."
	);
	
	std::cout<<qPrintable(s)<<std::endl;
	if(out!=nullptr)
		(*out)<<qPrintable(s)<<Qt::endl;
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

void setLanguage(QCoreApplication& qapplication, QWidget* parent)
{
//	Q_UNUSED(qapplication); //silence wrong MSVC warning

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
		 QString("FET searched for the translation file %1 in the directory %2, then in the directory %3, and "
		 "then in the directory %4, but could not find it.")
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
FET-CL starts here
*/
int main(int argc, char **argv)
{
	QCoreApplication qCoreApplication(argc, argv);

	initLanguagesSet();

	VERBOSE=false;

	terminateGeneratePointer=nullptr;
	
	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_buildings_schedule_ready=false;

	//srand(unsigned(time(nullptr))); //useless, I use randomKnuth(), but just in case I use somewhere rand() by mistake...

	//initRandomKnuth();
	gen.rng.initializeMRG32k3a();

	OUTPUT_DIR=QDir::homePath()+FILE_SEP+"fet-results";
	
	QStringList _args=QCoreApplication::arguments();

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

		SETTINGS_TIMETABLES_SEPARATE_DAYS_NAME_LONG_NAME_BY_BREAK=false;
		SETTINGS_TIMETABLES_SEPARATE_HOURS_NAME_LONG_NAME_BY_BREAK=false;
		SETTINGS_TIMETABLES_SEPARATE_SUBJECTS_NAME_LONG_NAME_CODE_BY_BREAK=false;
		SETTINGS_TIMETABLES_SEPARATE_ACTIVITY_TAGS_NAME_LONG_NAME_CODE_BY_BREAK=false;
		SETTINGS_TIMETABLES_SEPARATE_TEACHERS_NAME_LONG_NAME_CODE_BY_BREAK=false;
		SETTINGS_TIMETABLES_SEPARATE_STUDENTS_NAME_LONG_NAME_CODE_BY_BREAK=false;
		SETTINGS_TIMETABLES_SEPARATE_BUILDINGS_NAME_LONG_NAME_CODE_BY_BREAK=false;
		SETTINGS_TIMETABLES_SEPARATE_ROOMS_NAME_LONG_NAME_CODE_BY_BREAK=false;

		//only in days horizontal and days vertical.
		SETTINGS_TIMETABLES_PRINT_SUBJECTS_COMMENTS=false;
		SETTINGS_TIMETABLES_PRINT_ACTIVITY_TAGS_COMMENTS=false;
		SETTINGS_TIMETABLES_PRINT_TEACHERS_COMMENTS=false;
		SETTINGS_TIMETABLES_PRINT_SUBGROUPS_COMMENTS=false;
		SETTINGS_TIMETABLES_PRINT_GROUPS_COMMENTS=false;
		SETTINGS_TIMETABLES_PRINT_YEARS_COMMENTS=false;
		SETTINGS_TIMETABLES_PRINT_BUILDINGS_COMMENTS=false;
		SETTINGS_TIMETABLES_PRINT_ROOMS_COMMENTS=false;

		/////subgroups days horizontal and days vertical.
		SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_DAYS_NAMES=true;
		SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_DAYS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_HOURS_NAMES=true;
		SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_HOURS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_NAMES=true;
		SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_CODES=false;

		SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES=true;
		SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES=false;

		SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_NAMES=true;
		SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_CODES=false;

		SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_NAMES=true;
		SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_CODES=false;

		SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_NAMES=true;
		SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_CODES=false;

		SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_LEGEND=false;
		SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND=false;
		SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_LEGEND=false;
		SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_LEGEND=false;
		SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_LEGEND=false;
		SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_LEGEND_CODES_FIRST=false;
		/////

		/////subgroups time horizontal and time vertical.
		SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_NAMES=true;
		SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_NAMES=true;
		SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_NAMES=true;
		SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_CODES=false;

		SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES=true;
		SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES=false;

		SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_NAMES=true;
		SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_CODES=false;

		SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_NAMES=true;
		SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_CODES=false;

		SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_NAMES=true;
		SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_CODES=false;

		SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_LEGEND=false;
		SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND=false;
		SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_LEGEND=false;
		SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_LEGEND=false;
		SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_LEGEND=false;
		SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_LEGEND_CODES_FIRST=false;
		/////

		/////groups days horizontal and days vertical.
		SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_DAYS_NAMES=true;
		SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_DAYS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_HOURS_NAMES=true;
		SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_HOURS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_NAMES=true;
		SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_CODES=false;

		SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES=true;
		SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES=false;

		SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_NAMES=true;
		SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_CODES=false;

		SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_NAMES=true;
		SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_CODES=false;

		SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_NAMES=true;
		SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_CODES=false;

		SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_LEGEND=false;
		SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND=false;
		SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_LEGEND=false;
		SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_LEGEND=false;
		SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_LEGEND=false;
		SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_LEGEND_CODES_FIRST=false;
		/////

		/////groups time horizontal and time vertical.
		SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_NAMES=true;
		SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_NAMES=true;
		SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_NAMES=true;
		SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_CODES=false;

		SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES=true;
		SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES=false;

		SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_NAMES=true;
		SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_CODES=false;

		SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_NAMES=true;
		SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_CODES=false;

		SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_NAMES=true;
		SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_CODES=false;

		SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_LEGEND=false;
		SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND=false;
		SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_LEGEND=false;
		SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_LEGEND=false;
		SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_LEGEND=false;
		SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_LEGEND_CODES_FIRST=false;
		/////

		/////years days horizontal and days vertical.
		SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_DAYS_NAMES=true;
		SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_DAYS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_HOURS_NAMES=true;
		SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_HOURS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_NAMES=true;
		SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_CODES=false;

		SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES=true;
		SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES=false;

		SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_NAMES=true;
		SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_CODES=false;

		SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_NAMES=true;
		SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_CODES=false;

		SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_NAMES=true;
		SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_CODES=false;

		SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_LEGEND=false;
		SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND=false;
		SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_LEGEND=false;
		SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_LEGEND=false;
		SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_LEGEND=false;
		SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_LEGEND_CODES_FIRST=false;
		/////

		/////years time horizontal and time vertical.
		SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_NAMES=true;
		SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_NAMES=true;
		SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_NAMES=true;
		SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_CODES=false;

		SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES=true;
		SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES=false;

		SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_NAMES=true;
		SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_CODES=false;

		SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_NAMES=true;
		SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_CODES=false;

		SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_NAMES=true;
		SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_CODES=false;

		SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_LEGEND=false;
		SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND=false;
		SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_LEGEND=false;
		SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_LEGEND=false;
		SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_LEGEND=false;
		SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_LEGEND_CODES_FIRST=false;
		/////

		/////teachers days horizontal and days vertical.
		SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_DAYS_NAMES=true;
		SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_DAYS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_HOURS_NAMES=true;
		SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_HOURS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_NAMES=true;
		SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_CODES=false;

		SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES=true;
		SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES=false;

		SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_NAMES=true;
		SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_CODES=false;

		SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_NAMES=true;
		SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_CODES=false;

		SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_NAMES=true;
		SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_CODES=false;

		SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_LEGEND=false;
		SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND=false;
		SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_LEGEND=false;
		SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_LEGEND=false;
		SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_LEGEND=false;
		SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_LEGEND_CODES_FIRST=false;
		/////

		/////teachers time horizontal and time vertical.
		SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_NAMES=true;
		SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_NAMES=true;
		SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_NAMES=true;
		SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_CODES=false;

		SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES=true;
		SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES=false;

		SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_NAMES=true;
		SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_CODES=false;

		SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_NAMES=true;
		SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_CODES=false;

		SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_NAMES=true;
		SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_CODES=false;

		SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_LEGEND=false;
		SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND=false;
		SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_LEGEND=false;
		SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_LEGEND=false;
		SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_LEGEND=false;
		SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_LEGEND_CODES_FIRST=false;
		/////

		/////rooms days horizontal and days vertical.
		SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_DAYS_NAMES=true;
		SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_DAYS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_HOURS_NAMES=true;
		SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_HOURS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_NAMES=true;
		SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_CODES=false;

		SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES=true;
		SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES=false;

		SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_NAMES=true;
		SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_CODES=false;

		SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_NAMES=true;
		SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_CODES=false;

		SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_NAMES=true;
		SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_CODES=false;

		SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_LEGEND=false;
		SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND=false;
		SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_LEGEND=false;
		SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_LEGEND=false;
		SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_LEGEND=false;
		SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_LEGEND_CODES_FIRST=false;
		/////

		/////rooms time horizontal and time vertical.
		SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_NAMES=true;
		SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_NAMES=true;
		SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_NAMES=true;
		SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_CODES=false;

		SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES=true;
		SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES=false;

		SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_NAMES=true;
		SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_CODES=false;

		SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_NAMES=true;
		SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_CODES=false;

		SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_NAMES=true;
		SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_CODES=false;

		SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_LEGEND=false;
		SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND=false;
		SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_LEGEND=false;
		SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_LEGEND=false;
		SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_LEGEND=false;
		SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_LEGEND_CODES_FIRST=false;
		/////

		/////buildings days horizontal and days vertical.
		SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_DAYS_NAMES=true;
		SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_DAYS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_HOURS_NAMES=true;
		SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_HOURS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_NAMES=true;
		SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_CODES=false;

		SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES=true;
		SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES=false;

		SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_NAMES=true;
		SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_CODES=false;

		SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_NAMES=true;
		SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_CODES=false;

		SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_NAMES=true;
		SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_CODES=false;

		SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_LEGEND=false;
		SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND=false;
		SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_LEGEND=false;
		SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_LEGEND=false;
		SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_LEGEND=false;
		SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_LEGEND_CODES_FIRST=false;
		/////

		/////buildings time horizontal and time vertical.
		SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_NAMES=true;
		SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_NAMES=true;
		SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_NAMES=true;
		SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_CODES=false;

		SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES=true;
		SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES=false;

		SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_NAMES=true;
		SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_CODES=false;

		SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_NAMES=true;
		SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_CODES=false;

		SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_NAMES=true;
		SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_CODES=false;

		SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_LEGEND=false;
		SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND=false;
		SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_LEGEND=false;
		SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_LEGEND=false;
		SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_LEGEND=false;
		SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_LEGEND_CODES_FIRST=false;
		/////

		/////subjects days horizontal and days vertical.
		SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_DAYS_NAMES=true;
		SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_DAYS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_HOURS_NAMES=true;
		SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_HOURS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_NAMES=true;
		SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_CODES=false;

		SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES=true;
		SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES=false;

		SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_TEACHERS_NAMES=true;
		SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_TEACHERS_CODES=false;

		SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_STUDENTS_NAMES=true;
		SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_STUDENTS_CODES=false;

		SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ROOMS_NAMES=true;
		SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ROOMS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ROOMS_CODES=false;

		SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_LEGEND=false;
		SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND=false;
		SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_TEACHERS_LEGEND=false;
		SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_STUDENTS_LEGEND=false;
		SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ROOMS_LEGEND=false;
		SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_LEGEND_CODES_FIRST=false;
		/////

		/////subjects time horizontal and time vertical.
		SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_NAMES=true;
		SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_NAMES=true;
		SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_NAMES=true;
		SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_CODES=false;

		SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES=true;
		SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES=false;

		SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_NAMES=true;
		SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_CODES=false;

		SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_NAMES=true;
		SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_CODES=false;

		SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_NAMES=true;
		SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_CODES=false;

		SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_LEGEND=false;
		SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND=false;
		SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_LEGEND=false;
		SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_LEGEND=false;
		SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_LEGEND=false;
		SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_LEGEND_CODES_FIRST=false;
		/////

		/////activity tags days horizontal and days vertical.
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_DAYS_NAMES=true;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_DAYS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_HOURS_NAMES=true;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_HOURS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_SUBJECTS_NAMES=true;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_SUBJECTS_CODES=false;

		SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES=true;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES=false;

		SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_TEACHERS_NAMES=true;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_TEACHERS_CODES=false;

		SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_STUDENTS_NAMES=true;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_STUDENTS_CODES=false;

		SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ROOMS_NAMES=true;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ROOMS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ROOMS_CODES=false;

		SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_SUBJECTS_LEGEND=false;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND=false;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_TEACHERS_LEGEND=false;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_STUDENTS_LEGEND=false;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ROOMS_LEGEND=false;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_LEGEND_CODES_FIRST=false;
		/////

		/////activity tags time horizontal and time vertical.
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_NAMES=true;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_NAMES=true;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_NAMES=true;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_CODES=false;

		SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES=true;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES=false;

		SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_NAMES=true;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_CODES=false;

		SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_NAMES=true;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_CODES=false;

		SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_NAMES=true;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_CODES=false;

		SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_LEGEND=false;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND=false;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_LEGEND=false;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_LEGEND=false;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_LEGEND=false;
		SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_LEGEND_CODES_FIRST=false;
		/////

		/////activities days horizontal and days vertical.
		SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_DAYS_NAMES=true;
		SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_DAYS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_HOURS_NAMES=true;
		SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_HOURS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_SUBJECTS_NAMES=true;
		SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_SUBJECTS_CODES=false;

		SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES=true;
		SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES=false;

		SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_TEACHERS_NAMES=true;
		SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_TEACHERS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_TEACHERS_CODES=false;

		SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_STUDENTS_NAMES=true;
		SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_STUDENTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_STUDENTS_CODES=false;

		SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ROOMS_NAMES=true;
		SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ROOMS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ROOMS_CODES=false;

		SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_SUBJECTS_LEGEND=false;
		SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND=false;
		SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_TEACHERS_LEGEND=false;
		SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_STUDENTS_LEGEND=false;
		SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ROOMS_LEGEND=false;
		SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_LEGEND_CODES_FIRST=false;
		/////

		/////activities time horizontal and time vertical.
		SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_NAMES=true;
		SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_NAMES=true;
		SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_NAMES=true;
		SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_CODES=false;

		SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES=true;
		SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_CODES=false;

		SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_NAMES=true;
		SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_CODES=false;

		SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_NAMES=true;
		SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_CODES=false;

		SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_NAMES=true;
		SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_CODES=false;

		SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_LEGEND=false;
		SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND=false;
		SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_LEGEND=false;
		SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_LEGEND=false;
		SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_LEGEND=false;
		SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_LEGEND_CODES_FIRST=false;
		///////

		//2026-03-11 begin
		SETTINGS_TIMETABLES_TEACHERS_FREE_PERIODS_PRINT_DAYS_NAMES=true;
		SETTINGS_TIMETABLES_TEACHERS_FREE_PERIODS_PRINT_DAYS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_TEACHERS_FREE_PERIODS_PRINT_HOURS_NAMES=true;
		SETTINGS_TIMETABLES_TEACHERS_FREE_PERIODS_PRINT_HOURS_LONG_NAMES=false;

		SETTINGS_TIMETABLES_TEACHERS_FREE_PERIODS_PRINT_TEACHERS_NAMES=true;
		SETTINGS_TIMETABLES_TEACHERS_FREE_PERIODS_PRINT_TEACHERS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_TEACHERS_FREE_PERIODS_PRINT_TEACHERS_CODES=false;

		SETTINGS_TIMETABLES_STATISTICS_PRINT_TEACHERS_NAMES=true;
		SETTINGS_TIMETABLES_STATISTICS_PRINT_TEACHERS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_STATISTICS_PRINT_TEACHERS_CODES=false;

		SETTINGS_TIMETABLES_STATISTICS_PRINT_STUDENTS_NAMES=true;
		SETTINGS_TIMETABLES_STATISTICS_PRINT_STUDENTS_LONG_NAMES=false;
		SETTINGS_TIMETABLES_STATISTICS_PRINT_STUDENTS_CODES=false;
		//2026-03-11 end

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
			//keep this to refuse beginning the generation for FET-5.44.0 or later, because it is an obsolete option and we cannot bypass it
			else if(s.left(14)=="--randomseedx="){
				randomSeedXSpecified=true;
				//randomSeedX=s.right(s.length()-14).toInt();
			}
			//keep this to refuse beginning the generation for FET-5.44.0 or later, because it is an obsolete option and we cannot bypass it
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
			///////

			else if(s.startsWith("--separatedaysnamelongnamebybreak=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SEPARATE_DAYS_NAME_LONG_NAME_BY_BREAK=true;
			}
			else if(s.startsWith("--separatehoursnamelongnamebybreak=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SEPARATE_HOURS_NAME_LONG_NAME_BY_BREAK=true;
			}
			else if(s.startsWith("--separatesubjectsnamelongnamecodebybreak=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SEPARATE_SUBJECTS_NAME_LONG_NAME_CODE_BY_BREAK=true;
			}
			else if(s.startsWith("--separateactivitytagsnamelongnamecodebybreak=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SEPARATE_ACTIVITY_TAGS_NAME_LONG_NAME_CODE_BY_BREAK=true;
			}
			else if(s.startsWith("--separateteachersnamelongnamecodebybreak=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SEPARATE_TEACHERS_NAME_LONG_NAME_CODE_BY_BREAK=true;
			}
			else if(s.startsWith("--separatestudentsnamelongnamecodebybreak=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SEPARATE_STUDENTS_NAME_LONG_NAME_CODE_BY_BREAK=true;
			}
			else if(s.startsWith("--separatebuildingsnamelongnamecodebybreak=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SEPARATE_BUILDINGS_NAME_LONG_NAME_CODE_BY_BREAK=true;
			}
			else if(s.startsWith("--separateroomsnamelongnamecodebybreak=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SEPARATE_ROOMS_NAME_LONG_NAME_CODE_BY_BREAK=true;
			}
			///////

			else if(s.startsWith("--printsubjectscomments=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_PRINT_SUBJECTS_COMMENTS=true;
			}

			else if(s.startsWith("--printactivitytagscomments=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_PRINT_ACTIVITY_TAGS_COMMENTS=true;
			}
			else if(s.startsWith("--printteacherscomments=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_PRINT_TEACHERS_COMMENTS=true;
			}
			else if(s.startsWith("--printsubgroupscomments=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_PRINT_SUBGROUPS_COMMENTS=true;
			}
			else if(s.startsWith("--printgroupscomments=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_PRINT_GROUPS_COMMENTS=true;
			}
			else if(s.startsWith("--printyearscomments=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_PRINT_YEARS_COMMENTS=true;
			}
			else if(s.startsWith("--printbuildingscomments=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_PRINT_BUILDINGS_COMMENTS=true;
			}
			else if(s.startsWith("--printroomscomments=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_PRINT_ROOMS_COMMENTS=true;
			}
			
			///////

			else if(s.startsWith("--subgroupsdayshvprintdaysnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_DAYS_NAMES=false;
			}
			else if(s.startsWith("--subgroupsdayshvprintdayslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_DAYS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--subgroupsdayshvprinthoursnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_HOURS_NAMES=false;
			}
			else if(s.startsWith("--subgroupsdayshvprinthourslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_HOURS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--subgroupsdayshvprintsubjectsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_NAMES=false;
			}
			else if(s.startsWith("--subgroupsdayshvprintsubjectslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--subgroupsdayshvprintsubjectscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_CODES=true;
			}

			else if(s.startsWith("--subgroupsdayshvprintactivitytagsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES=false;
			}
			else if(s.startsWith("--subgroupsdayshvprintactivitytagslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=true;
			}
			else if(s.startsWith("--subgroupsdayshvprintactivitytagscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES=true;
			}

			else if(s.startsWith("--subgroupsdayshvprintteachersnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_NAMES=false;
			}
			else if(s.startsWith("--subgroupsdayshvprintteacherslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES=true;
			}
			else if(s.startsWith("--subgroupsdayshvprintteacherscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_CODES=true;
			}
			
			else if(s.startsWith("--subgroupsdayshvprintstudentsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_NAMES=false;
			}
			else if(s.startsWith("--subgroupsdayshvprintstudentslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--subgroupsdayshvprintstudentscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_CODES=true;
			}
			
			else if(s.startsWith("--subgroupsdayshvprintroomsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_NAMES=false;
			}
			else if(s.startsWith("--subgroupsdayshvprintroomslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_LONG_NAMES=true;
			}
			else if(s.startsWith("--subgroupsdayshvprintroomscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_CODES=true;
			}

			else if(s.startsWith("--subgroupsdayshvprintsubjectslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_LEGEND=true;
			}
			else if(s.startsWith("--subgroupsdayshvprintactivitytagslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND=true;
			}
			else if(s.startsWith("--subgroupsdayshvprintteacherslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_LEGEND=true;
			}
			else if(s.startsWith("--subgroupsdayshvprintstudentslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_LEGEND=true;
			}
			else if(s.startsWith("--subgroupsdayshvprintroomslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_LEGEND=true;
			}
			else if(s.startsWith("--subgroupsdayshvprintlegendcodesfirst=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_LEGEND_CODES_FIRST=true;
			}

			else if(s.startsWith("--subgroupstimehvprintdaysnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_NAMES=false;
			}
			else if(s.startsWith("--subgroupstimehvprintdayslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--subgroupstimehvprinthoursnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_NAMES=false;
			}
			else if(s.startsWith("--subgroupstimehvprinthourslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--subgroupstimehvprintsubjectsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_NAMES=false;
			}
			else if(s.startsWith("--subgroupstimehvprintsubjectslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--subgroupstimehvprintsubjectscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_CODES=true;
			}

			else if(s.startsWith("--subgroupstimehvprintactivitytagsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES=false;
			}
			else if(s.startsWith("--subgroupstimehvprintactivitytagslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=true;
			}
			else if(s.startsWith("--subgroupstimehvprintactivitytagscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES=true;
			}

			else if(s.startsWith("--subgroupstimehvprintteachersnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_NAMES=false;
			}
			else if(s.startsWith("--subgroupstimehvprintteacherslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_LONG_NAMES=true;
			}
			else if(s.startsWith("--subgroupstimehvprintteacherscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_CODES=true;
			}
			
			else if(s.startsWith("--subgroupstimehvprintstudentsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_NAMES=false;
			}
			else if(s.startsWith("--subgroupstimehvprintstudentslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--subgroupstimehvprintstudentscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_CODES=true;
			}
			
			else if(s.startsWith("--subgroupstimehvprintroomsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_NAMES=false;
			}
			else if(s.startsWith("--subgroupstimehvprintroomslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_LONG_NAMES=true;
			}
			else if(s.startsWith("--subgroupstimehvprintroomscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_CODES=true;
			}

			else if(s.startsWith("--subgroupstimehvprintsubjectslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_LEGEND=true;
			}
			else if(s.startsWith("--subgroupstimehvprintactivitytagslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND=true;
			}
			else if(s.startsWith("--subgroupstimehvprintteacherslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_LEGEND=true;
			}
			else if(s.startsWith("--subgroupstimehvprintstudentslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_LEGEND=true;
			}
			else if(s.startsWith("--subgroupstimehvprintroomslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_LEGEND=true;
			}
			else if(s.startsWith("--subgroupstimehvprintlegendcodesfirst=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_LEGEND_CODES_FIRST=true;
			}

			else if(s.startsWith("--groupsdayshvprintdaysnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_DAYS_NAMES=false;
			}
			else if(s.startsWith("--groupsdayshvprintdayslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_DAYS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--groupsdayshvprinthoursnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_HOURS_NAMES=false;
			}
			else if(s.startsWith("--groupsdayshvprinthourslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_HOURS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--groupsdayshvprintsubjectsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_NAMES=false;
			}
			else if(s.startsWith("--groupsdayshvprintsubjectslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--groupsdayshvprintsubjectscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_CODES=true;
			}

			else if(s.startsWith("--groupsdayshvprintactivitytagsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES=false;
			}
			else if(s.startsWith("--groupsdayshvprintactivitytagslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=true;
			}
			else if(s.startsWith("--groupsdayshvprintactivitytagscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES=true;
			}

			else if(s.startsWith("--groupsdayshvprintteachersnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_NAMES=false;
			}
			else if(s.startsWith("--groupsdayshvprintteacherslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES=true;
			}
			else if(s.startsWith("--groupsdayshvprintteacherscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_CODES=true;
			}
			
			else if(s.startsWith("--groupsdayshvprintstudentsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_NAMES=false;
			}
			else if(s.startsWith("--groupsdayshvprintstudentslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--groupsdayshvprintstudentscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_CODES=true;
			}
			
			else if(s.startsWith("--groupsdayshvprintroomsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_NAMES=false;
			}
			else if(s.startsWith("--groupsdayshvprintroomslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_LONG_NAMES=true;
			}
			else if(s.startsWith("--groupsdayshvprintroomscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_CODES=true;
			}

			else if(s.startsWith("--groupsdayshvprintsubjectslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_LEGEND=true;
			}
			else if(s.startsWith("--groupsdayshvprintactivitytagslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND=true;
			}
			else if(s.startsWith("--groupsdayshvprintteacherslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_LEGEND=true;
			}
			else if(s.startsWith("--groupsdayshvprintstudentslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_LEGEND=true;
			}
			else if(s.startsWith("--groupsdayshvprintroomslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_LEGEND=true;
			}
			else if(s.startsWith("--groupsdayshvprintlegendcodesfirst=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_LEGEND_CODES_FIRST=true;
			}

			else if(s.startsWith("--groupstimehvprintdaysnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_NAMES=false;
			}
			else if(s.startsWith("--groupstimehvprintdayslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--groupstimehvprinthoursnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_NAMES=false;
			}
			else if(s.startsWith("--groupstimehvprinthourslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--groupstimehvprintsubjectsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_NAMES=false;
			}
			else if(s.startsWith("--groupstimehvprintsubjectslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--groupstimehvprintsubjectscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_CODES=true;
			}

			else if(s.startsWith("--groupstimehvprintactivitytagsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES=false;
			}
			else if(s.startsWith("--groupstimehvprintactivitytagslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=true;
			}
			else if(s.startsWith("--groupstimehvprintactivitytagscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES=true;
			}

			else if(s.startsWith("--groupstimehvprintteachersnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_NAMES=false;
			}
			else if(s.startsWith("--groupstimehvprintteacherslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_LONG_NAMES=true;
			}
			else if(s.startsWith("--groupstimehvprintteacherscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_CODES=true;
			}
			
			else if(s.startsWith("--groupstimehvprintstudentsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_NAMES=false;
			}
			else if(s.startsWith("--groupstimehvprintstudentslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--groupstimehvprintstudentscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_CODES=true;
			}
			
			else if(s.startsWith("--groupstimehvprintroomsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_NAMES=false;
			}
			else if(s.startsWith("--groupstimehvprintroomslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_LONG_NAMES=true;
			}
			else if(s.startsWith("--groupstimehvprintroomscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_CODES=true;
			}

			else if(s.startsWith("--groupstimehvprintsubjectslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_LEGEND=true;
			}
			else if(s.startsWith("--groupstimehvprintactivitytagslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND=true;
			}
			else if(s.startsWith("--groupstimehvprintteacherslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_LEGEND=true;
			}
			else if(s.startsWith("--groupstimehvprintstudentslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_LEGEND=true;
			}
			else if(s.startsWith("--groupstimehvprintroomslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_LEGEND=true;
			}
			else if(s.startsWith("--groupstimehvprintlegendcodesfirst=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_LEGEND_CODES_FIRST=true;
			}

			else if(s.startsWith("--yearsdayshvprintdaysnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_DAYS_NAMES=false;
			}
			else if(s.startsWith("--yearsdayshvprintdayslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_DAYS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--yearsdayshvprinthoursnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_HOURS_NAMES=false;
			}
			else if(s.startsWith("--yearsdayshvprinthourslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_HOURS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--yearsdayshvprintsubjectsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_NAMES=false;
			}
			else if(s.startsWith("--yearsdayshvprintsubjectslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--yearsdayshvprintsubjectscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_CODES=true;
			}

			else if(s.startsWith("--yearsdayshvprintactivitytagsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES=false;
			}
			else if(s.startsWith("--yearsdayshvprintactivitytagslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=true;
			}
			else if(s.startsWith("--yearsdayshvprintactivitytagscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES=true;
			}

			else if(s.startsWith("--yearsdayshvprintteachersnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_NAMES=false;
			}
			else if(s.startsWith("--yearsdayshvprintteacherslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES=true;
			}
			else if(s.startsWith("--yearsdayshvprintteacherscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_CODES=true;
			}
			
			else if(s.startsWith("--yearsdayshvprintstudentsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_NAMES=false;
			}
			else if(s.startsWith("--yearsdayshvprintstudentslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--yearsdayshvprintstudentscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_CODES=true;
			}
			
			else if(s.startsWith("--yearsdayshvprintroomsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_NAMES=false;
			}
			else if(s.startsWith("--yearsdayshvprintroomslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_LONG_NAMES=true;
			}
			else if(s.startsWith("--yearsdayshvprintroomscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_CODES=true;
			}

			else if(s.startsWith("--yearsdayshvprintsubjectslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_LEGEND=true;
			}
			else if(s.startsWith("--yearsdayshvprintactivitytagslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND=true;
			}
			else if(s.startsWith("--yearsdayshvprintteacherslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_LEGEND=true;
			}
			else if(s.startsWith("--yearsdayshvprintstudentslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_LEGEND=true;
			}
			else if(s.startsWith("--yearsdayshvprintroomslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_LEGEND=true;
			}
			else if(s.startsWith("--yearsdayshvprintlegendcodesfirst=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_LEGEND_CODES_FIRST=true;
			}

			else if(s.startsWith("--yearstimehvprintdaysnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_NAMES=false;
			}
			else if(s.startsWith("--yearstimehvprintdayslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--yearstimehvprinthoursnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_NAMES=false;
			}
			else if(s.startsWith("--yearstimehvprinthourslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--yearstimehvprintsubjectsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_NAMES=false;
			}
			else if(s.startsWith("--yearstimehvprintsubjectslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--yearstimehvprintsubjectscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_CODES=true;
			}

			else if(s.startsWith("--yearstimehvprintactivitytagsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES=false;
			}
			else if(s.startsWith("--yearstimehvprintactivitytagslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=true;
			}
			else if(s.startsWith("--yearstimehvprintactivitytagscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES=true;
			}

			else if(s.startsWith("--yearstimehvprintteachersnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_NAMES=false;
			}
			else if(s.startsWith("--yearstimehvprintteacherslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_LONG_NAMES=true;
			}
			else if(s.startsWith("--yearstimehvprintteacherscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_CODES=true;
			}
			
			else if(s.startsWith("--yearstimehvprintstudentsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_NAMES=false;
			}
			else if(s.startsWith("--yearstimehvprintstudentslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--yearstimehvprintstudentscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_CODES=true;
			}
			
			else if(s.startsWith("--yearstimehvprintroomsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_NAMES=false;
			}
			else if(s.startsWith("--yearstimehvprintroomslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_LONG_NAMES=true;
			}
			else if(s.startsWith("--yearstimehvprintroomscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_CODES=true;
			}

			else if(s.startsWith("--yearstimehvprintsubjectslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_LEGEND=true;
			}
			else if(s.startsWith("--yearstimehvprintactivitytagslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND=true;
			}
			else if(s.startsWith("--yearstimehvprintteacherslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_LEGEND=true;
			}
			else if(s.startsWith("--yearstimehvprintstudentslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_LEGEND=true;
			}
			else if(s.startsWith("--yearstimehvprintroomslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_LEGEND=true;
			}
			else if(s.startsWith("--yearstimehvprintlegendcodesfirst=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_LEGEND_CODES_FIRST=true;
			}

			else if(s.startsWith("--teachersdayshvprintdaysnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_DAYS_NAMES=false;
			}
			else if(s.startsWith("--teachersdayshvprintdayslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_DAYS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--teachersdayshvprinthoursnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_HOURS_NAMES=false;
			}
			else if(s.startsWith("--teachersdayshvprinthourslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_HOURS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--teachersdayshvprintsubjectsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_NAMES=false;
			}
			else if(s.startsWith("--teachersdayshvprintsubjectslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--teachersdayshvprintsubjectscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_CODES=true;
			}

			else if(s.startsWith("--teachersdayshvprintactivitytagsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES=false;
			}
			else if(s.startsWith("--teachersdayshvprintactivitytagslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=true;
			}
			else if(s.startsWith("--teachersdayshvprintactivitytagscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES=true;
			}

			else if(s.startsWith("--teachersdayshvprintteachersnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_NAMES=false;
			}
			else if(s.startsWith("--teachersdayshvprintteacherslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES=true;
			}
			else if(s.startsWith("--teachersdayshvprintteacherscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_CODES=true;
			}
			
			else if(s.startsWith("--teachersdayshvprintstudentsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_NAMES=false;
			}
			else if(s.startsWith("--teachersdayshvprintstudentslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--teachersdayshvprintstudentscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_CODES=true;
			}
			
			else if(s.startsWith("--teachersdayshvprintroomsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_NAMES=false;
			}
			else if(s.startsWith("--teachersdayshvprintroomslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_LONG_NAMES=true;
			}
			else if(s.startsWith("--teachersdayshvprintroomscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_CODES=true;
			}

			else if(s.startsWith("--teachersdayshvprintsubjectslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_LEGEND=true;
			}
			else if(s.startsWith("--teachersdayshvprintactivitytagslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND=true;
			}
			else if(s.startsWith("--teachersdayshvprintteacherslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_LEGEND=true;
			}
			else if(s.startsWith("--teachersdayshvprintstudentslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_LEGEND=true;
			}
			else if(s.startsWith("--teachersdayshvprintroomslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_LEGEND=true;
			}
			else if(s.startsWith("--teachersdayshvprintlegendcodesfirst=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_LEGEND_CODES_FIRST=true;
			}

			else if(s.startsWith("--teacherstimehvprintdaysnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_NAMES=false;
			}
			else if(s.startsWith("--teacherstimehvprintdayslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--teacherstimehvprinthoursnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_NAMES=false;
			}
			else if(s.startsWith("--teacherstimehvprinthourslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--teacherstimehvprintsubjectsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_NAMES=false;
			}
			else if(s.startsWith("--teacherstimehvprintsubjectslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--teacherstimehvprintsubjectscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_CODES=true;
			}

			else if(s.startsWith("--teacherstimehvprintactivitytagsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES=false;
			}
			else if(s.startsWith("--teacherstimehvprintactivitytagslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=true;
			}
			else if(s.startsWith("--teacherstimehvprintactivitytagscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES=true;
			}

			else if(s.startsWith("--teacherstimehvprintteachersnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_NAMES=false;
			}
			else if(s.startsWith("--teacherstimehvprintteacherslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_LONG_NAMES=true;
			}
			else if(s.startsWith("--teacherstimehvprintteacherscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_CODES=true;
			}
			
			else if(s.startsWith("--teacherstimehvprintstudentsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_NAMES=false;
			}
			else if(s.startsWith("--teacherstimehvprintstudentslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--teacherstimehvprintstudentscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_CODES=true;
			}
			
			else if(s.startsWith("--teacherstimehvprintroomsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_NAMES=false;
			}
			else if(s.startsWith("--teacherstimehvprintroomslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_LONG_NAMES=true;
			}
			else if(s.startsWith("--teacherstimehvprintroomscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_CODES=true;
			}

			else if(s.startsWith("--teacherstimehvprintsubjectslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_LEGEND=true;
			}
			else if(s.startsWith("--teacherstimehvprintactivitytagslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND=true;
			}
			else if(s.startsWith("--teacherstimehvprintteacherslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_LEGEND=true;
			}
			else if(s.startsWith("--teacherstimehvprintstudentslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_LEGEND=true;
			}
			else if(s.startsWith("--teacherstimehvprintroomslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_LEGEND=true;
			}
			else if(s.startsWith("--teacherstimehvprintlegendcodesfirst=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_LEGEND_CODES_FIRST=true;
			}

			else if(s.startsWith("--roomsdayshvprintdaysnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_DAYS_NAMES=false;
			}
			else if(s.startsWith("--roomsdayshvprintdayslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_DAYS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--roomsdayshvprinthoursnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_HOURS_NAMES=false;
			}
			else if(s.startsWith("--roomsdayshvprinthourslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_HOURS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--roomsdayshvprintsubjectsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_NAMES=false;
			}
			else if(s.startsWith("--roomsdayshvprintsubjectslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--roomsdayshvprintsubjectscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_CODES=true;
			}

			else if(s.startsWith("--roomsdayshvprintactivitytagsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES=false;
			}
			else if(s.startsWith("--roomsdayshvprintactivitytagslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=true;
			}
			else if(s.startsWith("--roomsdayshvprintactivitytagscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES=true;
			}

			else if(s.startsWith("--roomsdayshvprintteachersnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_NAMES=false;
			}
			else if(s.startsWith("--roomsdayshvprintteacherslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES=true;
			}
			else if(s.startsWith("--roomsdayshvprintteacherscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_CODES=true;
			}
			
			else if(s.startsWith("--roomsdayshvprintstudentsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_NAMES=false;
			}
			else if(s.startsWith("--roomsdayshvprintstudentslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--roomsdayshvprintstudentscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_CODES=true;
			}
			
			else if(s.startsWith("--roomsdayshvprintroomsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_NAMES=false;
			}
			else if(s.startsWith("--roomsdayshvprintroomslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_LONG_NAMES=true;
			}
			else if(s.startsWith("--roomsdayshvprintroomscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_CODES=true;
			}

			else if(s.startsWith("--roomsdayshvprintsubjectslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_LEGEND=true;
			}
			else if(s.startsWith("--roomsdayshvprintactivitytagslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND=true;
			}
			else if(s.startsWith("--roomsdayshvprintteacherslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_LEGEND=true;
			}
			else if(s.startsWith("--roomsdayshvprintstudentslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_LEGEND=true;
			}
			else if(s.startsWith("--roomsdayshvprintroomslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_LEGEND=true;
			}
			else if(s.startsWith("--roomsdayshvprintlegendcodesfirst=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_LEGEND_CODES_FIRST=true;
			}

			else if(s.startsWith("--roomstimehvprintdaysnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_NAMES=false;
			}
			else if(s.startsWith("--roomstimehvprintdayslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--roomstimehvprinthoursnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_NAMES=false;
			}
			else if(s.startsWith("--roomstimehvprinthourslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--roomstimehvprintsubjectsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_NAMES=false;
			}
			else if(s.startsWith("--roomstimehvprintsubjectslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--roomstimehvprintsubjectscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_CODES=true;
			}

			else if(s.startsWith("--roomstimehvprintactivitytagsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES=false;
			}
			else if(s.startsWith("--roomstimehvprintactivitytagslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=true;
			}
			else if(s.startsWith("--roomstimehvprintactivitytagscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES=true;
			}

			else if(s.startsWith("--roomstimehvprintteachersnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_NAMES=false;
			}
			else if(s.startsWith("--roomstimehvprintteacherslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_LONG_NAMES=true;
			}
			else if(s.startsWith("--roomstimehvprintteacherscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_CODES=true;
			}
			
			else if(s.startsWith("--roomstimehvprintstudentsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_NAMES=false;
			}
			else if(s.startsWith("--roomstimehvprintstudentslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--roomstimehvprintstudentscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_CODES=true;
			}
			
			else if(s.startsWith("--roomstimehvprintroomsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_NAMES=false;
			}
			else if(s.startsWith("--roomstimehvprintroomslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_LONG_NAMES=true;
			}
			else if(s.startsWith("--roomstimehvprintroomscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_CODES=true;
			}

			else if(s.startsWith("--roomstimehvprintsubjectslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_LEGEND=true;
			}
			else if(s.startsWith("--roomstimehvprintactivitytagslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND=true;
			}
			else if(s.startsWith("--roomstimehvprintteacherslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_LEGEND=true;
			}
			else if(s.startsWith("--roomstimehvprintstudentslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_LEGEND=true;
			}
			else if(s.startsWith("--roomstimehvprintroomslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_LEGEND=true;
			}
			else if(s.startsWith("--roomstimehvprintlegendcodesfirst=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_LEGEND_CODES_FIRST=true;
			}

			else if(s.startsWith("--buildingsdayshvprintdaysnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_DAYS_NAMES=false;
			}
			else if(s.startsWith("--buildingsdayshvprintdayslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_DAYS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--buildingsdayshvprinthoursnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_HOURS_NAMES=false;
			}
			else if(s.startsWith("--buildingsdayshvprinthourslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_HOURS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--buildingsdayshvprintsubjectsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_NAMES=false;
			}
			else if(s.startsWith("--buildingsdayshvprintsubjectslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--buildingsdayshvprintsubjectscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_CODES=true;
			}

			else if(s.startsWith("--buildingsdayshvprintactivitytagsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES=false;
			}
			else if(s.startsWith("--buildingsdayshvprintactivitytagslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=true;
			}
			else if(s.startsWith("--buildingsdayshvprintactivitytagscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES=true;
			}

			else if(s.startsWith("--buildingsdayshvprintteachersnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_NAMES=false;
			}
			else if(s.startsWith("--buildingsdayshvprintteacherslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES=true;
			}
			else if(s.startsWith("--buildingsdayshvprintteacherscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_CODES=true;
			}
			
			else if(s.startsWith("--buildingsdayshvprintstudentsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_NAMES=false;
			}
			else if(s.startsWith("--buildingsdayshvprintstudentslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--buildingsdayshvprintstudentscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_CODES=true;
			}
			
			else if(s.startsWith("--buildingsdayshvprintroomsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_NAMES=false;
			}
			else if(s.startsWith("--buildingsdayshvprintroomslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_LONG_NAMES=true;
			}
			else if(s.startsWith("--buildingsdayshvprintroomscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_CODES=true;
			}

			else if(s.startsWith("--buildingsdayshvprintsubjectslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_LEGEND=true;
			}
			else if(s.startsWith("--buildingsdayshvprintactivitytagslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND=true;
			}
			else if(s.startsWith("--buildingsdayshvprintteacherslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_LEGEND=true;
			}
			else if(s.startsWith("--buildingsdayshvprintstudentslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_LEGEND=true;
			}
			else if(s.startsWith("--buildingsdayshvprintroomslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_LEGEND=true;
			}
			else if(s.startsWith("--buildingsdayshvprintlegendcodesfirst=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_LEGEND_CODES_FIRST=true;
			}

			else if(s.startsWith("--buildingstimehvprintdaysnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_NAMES=false;
			}
			else if(s.startsWith("--buildingstimehvprintdayslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--buildingstimehvprinthoursnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_NAMES=false;
			}
			else if(s.startsWith("--buildingstimehvprinthourslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--buildingstimehvprintsubjectsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_NAMES=false;
			}
			else if(s.startsWith("--buildingstimehvprintsubjectslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--buildingstimehvprintsubjectscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_CODES=true;
			}

			else if(s.startsWith("--buildingstimehvprintactivitytagsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES=false;
			}
			else if(s.startsWith("--buildingstimehvprintactivitytagslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=true;
			}
			else if(s.startsWith("--buildingstimehvprintactivitytagscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES=true;
			}

			else if(s.startsWith("--buildingstimehvprintteachersnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_NAMES=false;
			}
			else if(s.startsWith("--buildingstimehvprintteacherslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_LONG_NAMES=true;
			}
			else if(s.startsWith("--buildingstimehvprintteacherscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_CODES=true;
			}
			
			else if(s.startsWith("--buildingstimehvprintstudentsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_NAMES=false;
			}
			else if(s.startsWith("--buildingstimehvprintstudentslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--buildingstimehvprintstudentscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_CODES=true;
			}
			
			else if(s.startsWith("--buildingstimehvprintroomsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_NAMES=false;
			}
			else if(s.startsWith("--buildingstimehvprintroomslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_LONG_NAMES=true;
			}
			else if(s.startsWith("--buildingstimehvprintroomscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_CODES=true;
			}

			else if(s.startsWith("--buildingstimehvprintsubjectslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_LEGEND=true;
			}
			else if(s.startsWith("--buildingstimehvprintactivitytagslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND=true;
			}
			else if(s.startsWith("--buildingstimehvprintteacherslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_LEGEND=true;
			}
			else if(s.startsWith("--buildingstimehvprintstudentslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_LEGEND=true;
			}
			else if(s.startsWith("--buildingstimehvprintroomslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_LEGEND=true;
			}
			else if(s.startsWith("--buildingstimehvprintlegendcodesfirst=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_LEGEND_CODES_FIRST=true;
			}
			///////

			else if(s.startsWith("--subjectsdayshvprintdaysnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_DAYS_NAMES=false;
			}
			else if(s.startsWith("--subjectsdayshvprintdayslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_DAYS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--subjectsdayshvprinthoursnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_HOURS_NAMES=false;
			}
			else if(s.startsWith("--subjectsdayshvprinthourslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_HOURS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--subjectsdayshvprintsubjectsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_NAMES=false;
			}
			else if(s.startsWith("--subjectsdayshvprintsubjectslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--subjectsdayshvprintsubjectscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_CODES=true;
			}

			else if(s.startsWith("--subjectsdayshvprintactivitytagsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES=false;
			}
			else if(s.startsWith("--subjectsdayshvprintactivitytagslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=true;
			}
			else if(s.startsWith("--subjectsdayshvprintactivitytagscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES=true;
			}

			else if(s.startsWith("--subjectsdayshvprintteachersnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_TEACHERS_NAMES=false;
			}
			else if(s.startsWith("--subjectsdayshvprintteacherslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES=true;
			}
			else if(s.startsWith("--subjectsdayshvprintteacherscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_TEACHERS_CODES=true;
			}
			
			else if(s.startsWith("--subjectsdayshvprintstudentsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_STUDENTS_NAMES=false;
			}
			else if(s.startsWith("--subjectsdayshvprintstudentslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--subjectsdayshvprintstudentscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_STUDENTS_CODES=true;
			}
			
			else if(s.startsWith("--subjectsdayshvprintroomsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ROOMS_NAMES=false;
			}
			else if(s.startsWith("--subjectsdayshvprintroomslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ROOMS_LONG_NAMES=true;
			}
			else if(s.startsWith("--subjectsdayshvprintroomscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ROOMS_CODES=true;
			}

			else if(s.startsWith("--subjectsdayshvprintsubjectslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_LEGEND=true;
			}
			else if(s.startsWith("--subjectsdayshvprintactivitytagslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND=true;
			}
			else if(s.startsWith("--subjectsdayshvprintteacherslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_TEACHERS_LEGEND=true;
			}
			else if(s.startsWith("--subjectsdayshvprintstudentslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_STUDENTS_LEGEND=true;
			}
			else if(s.startsWith("--subjectsdayshvprintroomslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ROOMS_LEGEND=true;
			}
			else if(s.startsWith("--subjectsdayshvprintlegendcodesfirst=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_LEGEND_CODES_FIRST=true;
			}

			else if(s.startsWith("--subjectstimehvprintdaysnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_NAMES=false;
			}
			else if(s.startsWith("--subjectstimehvprintdayslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--subjectstimehvprinthoursnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_NAMES=false;
			}
			else if(s.startsWith("--subjectstimehvprinthourslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--subjectstimehvprintsubjectsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_NAMES=false;
			}
			else if(s.startsWith("--subjectstimehvprintsubjectslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--subjectstimehvprintsubjectscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_CODES=true;
			}

			else if(s.startsWith("--subjectstimehvprintactivitytagsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES=false;
			}
			else if(s.startsWith("--subjectstimehvprintactivitytagslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=true;
			}
			else if(s.startsWith("--subjectstimehvprintactivitytagscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES=true;
			}

			else if(s.startsWith("--subjectstimehvprintteachersnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_NAMES=false;
			}
			else if(s.startsWith("--subjectstimehvprintteacherslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_LONG_NAMES=true;
			}
			else if(s.startsWith("--subjectstimehvprintteacherscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_CODES=true;
			}
			
			else if(s.startsWith("--subjectstimehvprintstudentsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_NAMES=false;
			}
			else if(s.startsWith("--subjectstimehvprintstudentslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--subjectstimehvprintstudentscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_CODES=true;
			}
			
			else if(s.startsWith("--subjectstimehvprintroomsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_NAMES=false;
			}
			else if(s.startsWith("--subjectstimehvprintroomslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_LONG_NAMES=true;
			}
			else if(s.startsWith("--subjectstimehvprintroomscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_CODES=true;
			}

			else if(s.startsWith("--subjectstimehvprintsubjectslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_LEGEND=true;
			}
			else if(s.startsWith("--subjectstimehvprintactivitytagslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND=true;
			}
			else if(s.startsWith("--subjectstimehvprintteacherslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_LEGEND=true;
			}
			else if(s.startsWith("--subjectstimehvprintstudentslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_LEGEND=true;
			}
			else if(s.startsWith("--subjectstimehvprintroomslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_LEGEND=true;
			}
			else if(s.startsWith("--subjectstimehvprintlegendcodesfirst=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_LEGEND_CODES_FIRST=true;
			}
			///////

			else if(s.startsWith("--activitytagsdayshvprintdaysnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_DAYS_NAMES=false;
			}
			else if(s.startsWith("--activitytagsdayshvprintdayslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_DAYS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--activitytagsdayshvprinthoursnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_HOURS_NAMES=false;
			}
			else if(s.startsWith("--activitytagsdayshvprinthourslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_HOURS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--activitytagsdayshvprintsubjectsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_SUBJECTS_NAMES=false;
			}
			else if(s.startsWith("--activitytagsdayshvprintsubjectslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--activitytagsdayshvprintsubjectscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_SUBJECTS_CODES=true;
			}

			else if(s.startsWith("--activitytagsdayshvprintactivitytagsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES=false;
			}
			else if(s.startsWith("--activitytagsdayshvprintactivitytagslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=true;
			}
			else if(s.startsWith("--activitytagsdayshvprintactivitytagscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES=true;
			}

			else if(s.startsWith("--activitytagsdayshvprintteachersnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_TEACHERS_NAMES=false;
			}
			else if(s.startsWith("--activitytagsdayshvprintteacherslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES=true;
			}
			else if(s.startsWith("--activitytagsdayshvprintteacherscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_TEACHERS_CODES=true;
			}
			
			else if(s.startsWith("--activitytagsdayshvprintstudentsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_STUDENTS_NAMES=false;
			}
			else if(s.startsWith("--activitytagsdayshvprintstudentslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--activitytagsdayshvprintstudentscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_STUDENTS_CODES=true;
			}
			
			else if(s.startsWith("--activitytagsdayshvprintroomsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ROOMS_NAMES=false;
			}
			else if(s.startsWith("--activitytagsdayshvprintroomslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ROOMS_LONG_NAMES=true;
			}
			else if(s.startsWith("--activitytagsdayshvprintroomscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ROOMS_CODES=true;
			}

			else if(s.startsWith("--activitytagsdayshvprintsubjectslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_SUBJECTS_LEGEND=true;
			}
			else if(s.startsWith("--activitytagsdayshvprintactivitytagslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND=true;
			}
			else if(s.startsWith("--activitytagsdayshvprintteacherslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_TEACHERS_LEGEND=true;
			}
			else if(s.startsWith("--activitytagsdayshvprintstudentslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_STUDENTS_LEGEND=true;
			}
			else if(s.startsWith("--activitytagsdayshvprintroomslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ROOMS_LEGEND=true;
			}
			else if(s.startsWith("--activitytagsdayshvprintlegendcodesfirst=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_LEGEND_CODES_FIRST=true;
			}

			else if(s.startsWith("--activitytagstimehvprintdaysnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_NAMES=false;
			}
			else if(s.startsWith("--activitytagstimehvprintdayslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--activitytagstimehvprinthoursnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_NAMES=false;
			}
			else if(s.startsWith("--activitytagstimehvprinthourslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--activitytagstimehvprintsubjectsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_NAMES=false;
			}
			else if(s.startsWith("--activitytagstimehvprintsubjectslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--activitytagstimehvprintsubjectscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_CODES=true;
			}

			else if(s.startsWith("--activitytagstimehvprintactivitytagsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES=false;
			}
			else if(s.startsWith("--activitytagstimehvprintactivitytagslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=true;
			}
			else if(s.startsWith("--activitytagstimehvprintactivitytagscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES=true;
			}

			else if(s.startsWith("--activitytagstimehvprintteachersnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_NAMES=false;
			}
			else if(s.startsWith("--activitytagstimehvprintteacherslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_LONG_NAMES=true;
			}
			else if(s.startsWith("--activitytagstimehvprintteacherscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_CODES=true;
			}
			
			else if(s.startsWith("--activitytagstimehvprintstudentsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_NAMES=false;
			}
			else if(s.startsWith("--activitytagstimehvprintstudentslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--activitytagstimehvprintstudentscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_CODES=true;
			}
			
			else if(s.startsWith("--activitytagstimehvprintroomsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_NAMES=false;
			}
			else if(s.startsWith("--activitytagstimehvprintroomslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_LONG_NAMES=true;
			}
			else if(s.startsWith("--activitytagstimehvprintroomscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_CODES=true;
			}

			else if(s.startsWith("--activitytagstimehvprintsubjectslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_LEGEND=true;
			}
			else if(s.startsWith("--activitytagstimehvprintactivitytagslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND=true;
			}
			else if(s.startsWith("--activitytagstimehvprintteacherslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_LEGEND=true;
			}
			else if(s.startsWith("--activitytagstimehvprintstudentslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_LEGEND=true;
			}
			else if(s.startsWith("--activitytagstimehvprintroomslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_LEGEND=true;
			}
			else if(s.startsWith("--activitytagstimehvprintlegendcodesfirst=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_LEGEND_CODES_FIRST=true;
			}
			///////

			else if(s.startsWith("--activitiesdayshvprintdaysnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_DAYS_NAMES=false;
			}
			else if(s.startsWith("--activitiesdayshvprintdayslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_DAYS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--activitiesdayshvprinthoursnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_HOURS_NAMES=false;
			}
			else if(s.startsWith("--activitiesdayshvprinthourslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_HOURS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--activitiesdayshvprintsubjectsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_SUBJECTS_NAMES=false;
			}
			else if(s.startsWith("--activitiesdayshvprintsubjectslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--activitiesdayshvprintsubjectscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_SUBJECTS_CODES=true;
			}

			else if(s.startsWith("--activitiesdayshvprintactivitytagsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES=false;
			}
			else if(s.startsWith("--activitiesdayshvprintactivitytagslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=true;
			}
			else if(s.startsWith("--activitiesdayshvprintactivitytagscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES=true;
			}

			else if(s.startsWith("--activitiesdayshvprintteachersnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_TEACHERS_NAMES=false;
			}
			else if(s.startsWith("--activitiesdayshvprintteacherslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_TEACHERS_LONG_NAMES=true;
			}
			else if(s.startsWith("--activitiesdayshvprintteacherscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_TEACHERS_CODES=true;
			}
			
			else if(s.startsWith("--activitiesdayshvprintstudentsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_STUDENTS_NAMES=false;
			}
			else if(s.startsWith("--activitiesdayshvprintstudentslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_STUDENTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--activitiesdayshvprintstudentscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_STUDENTS_CODES=true;
			}
			
			else if(s.startsWith("--activitiesdayshvprintroomsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ROOMS_NAMES=false;
			}
			else if(s.startsWith("--activitiesdayshvprintroomslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ROOMS_LONG_NAMES=true;
			}
			else if(s.startsWith("--activitiesdayshvprintroomscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ROOMS_CODES=true;
			}

			else if(s.startsWith("--activitiesdayshvprintsubjectslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_SUBJECTS_LEGEND=true;
			}
			else if(s.startsWith("--activitiesdayshvprintactivitytagslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND=true;
			}
			else if(s.startsWith("--activitiesdayshvprintteacherslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_TEACHERS_LEGEND=true;
			}
			else if(s.startsWith("--activitiesdayshvprintstudentslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_STUDENTS_LEGEND=true;
			}
			else if(s.startsWith("--activitiesdayshvprintroomslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ROOMS_LEGEND=true;
			}
			else if(s.startsWith("--activitiesdayshvprintlegendcodesfirst=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_LEGEND_CODES_FIRST=true;
			}

			else if(s.startsWith("--activitiestimehvprintdaysnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_NAMES=false;
			}
			else if(s.startsWith("--activitiestimehvprintdayslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--activitiestimehvprinthoursnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_NAMES=false;
			}
			else if(s.startsWith("--activitiestimehvprinthourslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_LONG_NAMES=true;
			}
			
			else if(s.startsWith("--activitiestimehvprintsubjectsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_NAMES=false;
			}
			else if(s.startsWith("--activitiestimehvprintsubjectslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--activitiestimehvprintsubjectscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_CODES=true;
			}

			else if(s.startsWith("--activitiestimehvprintactivitytagsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES=false;
			}
			else if(s.startsWith("--activitiestimehvprintactivitytagslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES=true;
			}
			else if(s.startsWith("--activitiestimehvprintactivitytagscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_CODES=true;
			}

			else if(s.startsWith("--activitiestimehvprintteachersnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_NAMES=false;
			}
			else if(s.startsWith("--activitiestimehvprintteacherslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_LONG_NAMES=true;
			}
			else if(s.startsWith("--activitiestimehvprintteacherscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_CODES=true;
			}
			
			else if(s.startsWith("--activitiestimehvprintstudentsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_NAMES=false;
			}
			else if(s.startsWith("--activitiestimehvprintstudentslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--activitiestimehvprintstudentscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_CODES=true;
			}
			
			else if(s.startsWith("--activitiestimehvprintroomsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_NAMES=false;
			}
			else if(s.startsWith("--activitiestimehvprintroomslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_LONG_NAMES=true;
			}
			else if(s.startsWith("--activitiestimehvprintroomscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_CODES=true;
			}

			else if(s.startsWith("--activitiestimehvprintsubjectslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_LEGEND=true;
			}
			else if(s.startsWith("--activitiestimehvprintactivitytagslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND=true;
			}
			else if(s.startsWith("--activitiestimehvprintteacherslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_LEGEND=true;
			}
			else if(s.startsWith("--activitiestimehvprintstudentslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_LEGEND=true;
			}
			else if(s.startsWith("--activitiestimehvprintroomslegend=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_LEGEND=true;
			}
			else if(s.startsWith("--activitiestimehvprintlegendcodesfirst=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_LEGEND_CODES_FIRST=true;
			}
			///////2026-03-11 begin
			else if(s.startsWith("--teachersfreeperiodsprintdaysnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_TEACHERS_FREE_PERIODS_PRINT_DAYS_NAMES=false;
			}
			else if(s.startsWith("--teachersfreeperiodsprintdayslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_FREE_PERIODS_PRINT_DAYS_LONG_NAMES=true;
			}

			else if(s.startsWith("--teachersfreeperiodsprinthoursnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_TEACHERS_FREE_PERIODS_PRINT_HOURS_NAMES=false;
			}
			else if(s.startsWith("--teachersfreeperiodsprinthourslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_FREE_PERIODS_PRINT_HOURS_LONG_NAMES=true;
			}

			else if(s.startsWith("--teachersfreeperiodsprintteachersnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_TEACHERS_FREE_PERIODS_PRINT_TEACHERS_NAMES=false;
			}
			else if(s.startsWith("--teachersfreeperiodsprintteacherslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_FREE_PERIODS_PRINT_TEACHERS_LONG_NAMES=true;
			}
			else if(s.startsWith("--teachersfreeperiodsprintteacherscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_TEACHERS_FREE_PERIODS_PRINT_TEACHERS_CODES=true;
			}

			else if(s.startsWith("--statisticsprintteachersnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_STATISTICS_PRINT_TEACHERS_NAMES=false;
			}
			else if(s.startsWith("--statisticsprintteacherslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_STATISTICS_PRINT_TEACHERS_LONG_NAMES=true;
			}
			else if(s.startsWith("--statisticsprintteacherscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_STATISTICS_PRINT_TEACHERS_CODES=true;
			}

			else if(s.startsWith("--statisticsprintstudentsnames=")){
				if(s.endsWith("false"))
					SETTINGS_TIMETABLES_STATISTICS_PRINT_STUDENTS_NAMES=false;
			}
			else if(s.startsWith("--statisticsprintstudentslongnames=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_STATISTICS_PRINT_STUDENTS_LONG_NAMES=true;
			}
			else if(s.startsWith("--statisticsprintstudentscodes=")){
				if(s.endsWith("true"))
					SETTINGS_TIMETABLES_STATISTICS_PRINT_STUDENTS_CODES=true;
			}
			///////2026-03-11 end
			else
				unrecognizedOptions.append(s);
		}
		
		if(filename==""){
			if(unrecognizedOptions.count()>0){
				for(const QString& s : std::as_const(unrecognizedOptions)){
					std::cout<<"Unrecognized option: "<<qPrintable(s)<<std::endl;
				}
				std::cout<<std::endl;
			}

			if(showHelp){
				usage(nullptr, QString(""));
				return 0;
			}
			else if(showVersion){
				std::cout<<"FET version "<<qPrintable(FET_VERSION)<<std::endl;
				std::cout<<"Free timetabling software, licensed under the GNU Affero General Public License version 3"<<std::endl;
				//This is one of the two places in which we print the program's copyright period and authors.
				//The other place is in the file src/interface/helpaboutform.cpp.
				//(Also, don't forget about the README file and about the two files in the "man" directory!)
				std::cout<<"Copyright (C) 2002-2026 Liviu Lalescu, Volker Dirr"<<std::endl;
				std::cout<<"Homepage: https://lalescu.ro/liviu/fet/"<<std::endl;
				std::cout<<"This program uses Qt version "<<qVersion()<<", Copyright (C) The Qt Company Ltd and other contributors."<<std::endl;
				std::cout<<"Depending on the platform and compiler, this program may use libraries from:"<<std::endl;
				std::cout<<"  gcc, Copyright (C) Free Software Foundation, Inc."<<std::endl;
				std::cout<<"  MinGW-w64, Copyright (c) by the mingw-w64 project"<<std::endl;
				std::cout<<"  Clang"<<std::endl;
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
					std::cout<<"Unrecognized option: "<<qPrintable(s)<<std::endl;
				}
				std::cout<<std::endl;
			}

			std::cout<<"Error: the specified input file "<<qPrintable(QDir::toNativeSeparators(filename))<<" is not existing"<<std::endl;
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
			std::cout<<"FET critical - you don't have write permissions in the output directory - (FET cannot open or create file "<<qPrintable(QDir::toNativeSeparators(logsDir))<<"result.txt)."
			 " If this is a bug - please report it."<<std::endl;
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
				std::cout<<"Cannot remove the file named "<<qPrintable(QDir::toNativeSeparators(communicationFile))
				 <<". This is a fatal error for FET-CL. Please either remove this file, or start the generation in another directory."<<std::endl;
				exit(1);
			}
		}
		
		if(unrecognizedOptions.count()>0){
			for(const QString& s : std::as_const(unrecognizedOptions)){
				std::cout<<"Unrecognized option: "<<qPrintable(s)<<std::endl;
				out<<"Unrecognized option: "<<qPrintable(s)<<Qt::endl;
			}
			std::cout<<std::endl;
			out<<Qt::endl;
		}
		
		//Cleanup the previous unsuccessful generation, if any. No need to remove the other files, they are overwritten.
		QFile oldDifficultActivitiesFile(logsDir+"difficult_activities.txt");
		if(oldDifficultActivitiesFile.exists()){
			bool t=oldDifficultActivitiesFile.remove();
			if(!t){
				out<<"Cannot remove the old existing file "<<qPrintable(QDir::toNativeSeparators(logsDir))<<"difficult_activities.txt"<<Qt::endl;
				std::cout<<"Cannot remove the old existing file "<<qPrintable(QDir::toNativeSeparators(logsDir))<<"difficult_activities.txt"<<std::endl;
			}
		}
		
		QFile oldWarningsFile(logsDir+"warnings.txt");
		if(oldWarningsFile.exists()){
			bool t=oldWarningsFile.remove();
			if(!t){
				out<<"Cannot remove the old warnings file "<<qPrintable(QDir::toNativeSeparators(logsDir))<<"warnings.txt"<<Qt::endl;
				std::cout<<"Cannot remove the old warnings file "<<qPrintable(QDir::toNativeSeparators(logsDir))<<"warnings.txt"<<std::endl;
			}
		}
		
		QFile oldErrorsFile(logsDir+"errors.txt");
		if(oldErrorsFile.exists()){
			bool t=oldErrorsFile.remove();
			if(!t){
				out<<"Cannot remove the old errors file "<<qPrintable(QDir::toNativeSeparators(logsDir))<<"errors.txt"<<Qt::endl;
				std::cout<<"Cannot remove the old errors file "<<qPrintable(QDir::toNativeSeparators(logsDir))<<"errors.txt"<<std::endl;
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
			std::cout<<"fet: critical error - cannot open the log file for the maximum placed activities - (FET cannot open or create file "<<qPrintable(QDir::toNativeSeparators(logsDir))<<"max_placed_activities.txt."
			 " If this is a bug - please report it."<<std::endl;
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
		maxPlacedActivityStream<<FetTranslate::tr("This is the list of max placed activities, chronologically. If FET could reach maximum n-th activity, look at the n+1-st activity"
			" in the initial order of the activities")<<Qt::endl<<Qt::endl;
		
		QFile initialOrderFile(logsDir+"initial_order.txt");
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
		bool iof=initialOrderFile.open(QIODeviceBase::WriteOnly);
#else
		bool iof=initialOrderFile.open(QIODevice::WriteOnly);
#endif
		if(!iof){
			std::cout<<"fet: critical error - cannot open the log file for the initial order of the activities - (FET cannot open or create file "<<qPrintable(QDir::toNativeSeparators(logsDir))<<"initial_order.txt."
			 " If this is a bug - please report it."<<std::endl;
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
		
		out<<"This file contains the result (log) of last operation"<<Qt::endl<<Qt::endl;
		
		QDate dat=QDate::currentDate();
		QTime tim=QTime::currentTime();
		QLocale loc(FET_LANGUAGE);
		QString sTime=loc.toString(dat, QLocale::ShortFormat)+" "+loc.toString(tim, QLocale::ShortFormat);
		out<<"FET command line generation started on "<<qPrintable(sTime)<<Qt::endl<<Qt::endl;
		
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
			std::cout<<"fet: critical error - you don't have write permissions in the output directory - (FET cannot open or create file "<<qPrintable(QDir::toNativeSeparators(outputDirectory))<<"test_write_permissions_2.tmp)."
			 " If this is a bug - please report it."<<std::endl;
			out<<"fet: critical error - you don't have write permissions in the output directory - (FET cannot open or create file "<<qPrintable(QDir::toNativeSeparators(outputDirectory))<<"test_write_permissions_2.tmp)."
			 " If this is a bug - please report it."<<Qt::endl;
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
			std::cout<<"fet-cl: cannot read input file (not existing, in use, or incorrect file) - aborting"<<std::endl;
			out<<"Cannot read input file (not existing, in use, or incorrect file) - aborting"<<Qt::endl;
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
			std::cout<<"Please input at least one active activity before generating"<<std::endl;
			out<<"Please input at least one active activity before generating"<<Qt::endl;
			logFile.close();
			return 1;
		}
		
		t=gt.rules.computeInternalStructure(nullptr);
		if(!t){
			std::cout<<"Cannot compute internal structure - aborting"<<std::endl;
			out<<"Cannot compute internal structure - aborting"<<Qt::endl;
			logFile.close();
			return 1;
		}
		
		terminateGeneratePointer=&gen;
		signal(SIGTERM, terminate);
#ifdef SIGBREAK
		signal(SIGBREAK, terminate);
#endif
		
		//https://stackoverflow.com/questions/6736536/c-input-and-output-to-the-console-window-at-the-same-time/31500127#31500127
		//std::atomic<bool> run(true);
		//std::thread pollFileThread(pollFile, std::ref(run));

		gen.abortOptimization=false;
		gen.restart=false;
		gen.paused=false;
		gen.pausedTime=0;
		bool ok=gen.precompute(nullptr, &initialOrderStream);
		
		initialOrderFile.close();
		
		if(!ok){
			std::cout<<"Cannot precompute - data is wrong - aborting"<<std::endl;
			out<<"Cannot precompute - data is wrong - aborting"<<Qt::endl;
			logFile.close();
			return 1;
		}
	
		bool restarted, impossible, timeExceeded;
		
		std::cout<<"Starting timetable generation..."<<std::endl;
		out<<"Starting timetable generation..."<<Qt::endl;
		if(VERBOSE){
			std::cout<<"secondsLimit=="<<secondsLimit<<std::endl;
		}
		//out<<"secondsLimit=="<<secondsLimit<<endl;
		
		TimetableExport::writeRandomSeedCommandLine(nullptr, gen.rng, outputDirectory, true); //true represents 'before' state

		gen.generate(secondsLimit, restarted, impossible, timeExceeded, false, true, &maxPlacedActivityStream); //false means not threaded, true means command line

		//run.store(false);
		//pollFileThread.join();
		
		maxPlacedActivityFile.close();
	
		if(impossible){
			std::cout<<"Impossible"<<std::endl;
			out<<"Impossible"<<Qt::endl;
			
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
				std::cout<<"FET critical - you don't have write permissions in the output directory - (FET cannot open or create file "<<qPrintable(QDir::toNativeSeparators(logsDir))<<"difficult_activities.txt)."
				 " If this is a bug - please report it."<<std::endl;
				return 1;
			}
			QTextStream difficultActivitiesOut(&difficultActivitiesFile);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
			difficultActivitiesOut.setEncoding(QStringConverter::Utf8);
#else
			difficultActivitiesOut.setCodec("UTF-8");
#endif
			difficultActivitiesOut.setGenerateByteOrderMark(true);
			
			difficultActivitiesOut<<s<<Qt::endl;
			
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
				std::cout<<"Time exceeded"<<std::endl;
				out<<"Time exceeded"<<Qt::endl;
			}
			else if(gen.abortOptimization){
				std::cout<<"Generation interrupted"<<std::endl;
				out<<"Generation interrupted"<<Qt::endl;
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
				std::cout<<"FET critical - you don't have write permissions in the output directory - (FET cannot open or create file "<<qPrintable(QDir::toNativeSeparators(logsDir))<<"difficult_activities.txt)."
				 " If this is a bug - please report it."<<std::endl;
				return 1;
			}
			QTextStream difficultActivitiesOut(&difficultActivitiesFile);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
			difficultActivitiesOut.setEncoding(QStringConverter::Utf8);
#else
			difficultActivitiesOut.setCodec("UTF-8");
#endif
			difficultActivitiesOut.setGenerateByteOrderMark(true);
			
			difficultActivitiesOut<<s<<Qt::endl;
			
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
			std::cout<<"Generation successful"<<std::endl;
			out<<"Generation successful"<<Qt::endl;
		
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
	/////////////////////////////////////////////////
}
