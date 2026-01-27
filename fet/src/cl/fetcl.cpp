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
