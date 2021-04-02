/*
File fet.cpp - program using the main engine "timetable"
*/

/*
Copyright 2002, 2003 Lalescu Liviu.

This file is part of FET.

FET is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

FET is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with timetable; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <QDate>
#include <QTime>
#include <QLocale>

#include <ctime>

#include "timetableexport.h"
#include "generate.h"

#include "timetable_defs.h"
#include "timetable.h"
#include "fetmainform.h"

#include <qapplication.h>
#include <qmutex.h>
#include <qstring.h>
#include <qtranslator.h>

#include <QDir>
#include <QTranslator>

#include <QSettings>

#include <fstream>
#include <iostream>
using namespace std;

extern Solution best_solution;

extern bool students_schedule_ready, teachers_schedule_ready, rooms_schedule_ready;

extern QMutex mutex;

void writeDefaultSimulationParameters();

QTranslator translator;

/**
The one and only instantiation of the main class.
*/
Timetable gt;

/**
Log file.
*/
ofstream logg;

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


qint16 teachers_timetable_weekly[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
qint16 students_timetable_weekly[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
qint16 rooms_timetable_weekly[MAX_ROOMS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
QList<qint16> teachers_free_periods_timetable_weekly[TEACHERS_FREE_PERIODS_N_CATEGORIES][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

QApplication* pqapplication=NULL;

/*static int fetch_line(ifstream& in, char *s){
	for(;;){
		in.getline(s, 250);
		if(in.eof())
			return 0;
		if(s[0]=='#')
			continue;

		int i;
		for(i=0; s[i] && !isprint(s[i]); i++)
			;
		if(isprint(s[i]))
			return 1;
	}
}*/

void readSimulationParameters(){
	const QString predefDir=QDir::homePath()+FILE_SEP+"fet-results";

	QSettings newSettings(COMPANY, PROGRAM);
	//QString newVer=newSettings.value("version", "-1").toString();
	//if(newVer=="-1"){	
	if(!newSettings.contains("version")){
		QSettings oldSettings("FET free software", "FET");
		
		if(oldSettings.contains("fetmainformgeometry")){
			if(!oldSettings.contains("output-directory")){
				OUTPUT_DIR=predefDir;
				
				QString s;
#ifdef Q_OS_WIN
				s+=QObject::tr("IMPORTANT NOTE for Windows users: the results directory location will be changed. Please"
				 " check out the new location specified below:");
				s+="\n\n";
#endif
				s+=QObject::tr("You upgraded FET to a new version. Beginning with FET-5.10.0 (June 2009),"
				 " you can change the output directory of FET (see the 'Settings' menu).")
				 +"\n\n"+QObject::tr("For the moment, the output (results) directory will be set to the predefined value %1")
				 .arg(QDir::toNativeSeparators(OUTPUT_DIR))
 				 +" . "+QObject::tr("You can change it later", "It refers to the output directory");

				QMessageBox::information(NULL, QObject::tr("FET important note"), s, QObject::tr("OK, I have read this"));
			}
			else{
				OUTPUT_DIR=oldSettings.value("output-directory", predefDir).toString();
				QDir dir;
				if(!dir.exists(OUTPUT_DIR)){
					bool t=dir.mkpath(OUTPUT_DIR);
					if(!t){
						QMessageBox::warning(NULL, QObject::tr("FET warning"), QObject::tr("Output directory %1 does not exist and cannot be"
						 " created - output directory will be made the default value %2")
						 .arg(QDir::toNativeSeparators(OUTPUT_DIR)).arg(QDir::toNativeSeparators(predefDir)));
						OUTPUT_DIR=predefDir;
					}
				}
			}
		}
		else{
			OUTPUT_DIR=oldSettings.value("output-directory", predefDir).toString();
		}
		
		FET_LANGUAGE=oldSettings.value("language", "en_GB").toString();
		WORKING_DIRECTORY=oldSettings.value("working-directory", "examples").toString();
		IMPORT_DIRECTORY=oldSettings.value("import-directory", OUTPUT_DIR).toString();
	
		QDir d(WORKING_DIRECTORY);
		if(!d.exists())
			WORKING_DIRECTORY="examples";
		QDir d2(WORKING_DIRECTORY);
		if(!d2.exists())
			WORKING_DIRECTORY=QDir::homePath();
	
		QDir i(IMPORT_DIRECTORY);
		if(!i.exists())
			IMPORT_DIRECTORY=OUTPUT_DIR;	// IMPORT_DIRECTORY="import";
	
		checkForUpdates=oldSettings.value("check-for-updates", "-1").toInt();
		QString ver=oldSettings.value("version", "-1").toString();
		
		if(!oldSettings.contains("html-level"))
			TIMETABLE_HTML_LEVEL=oldSettings.value("timetable-html-level", "2").toInt();
		else
			TIMETABLE_HTML_LEVEL=oldSettings.value("html-level", "2").toInt();
	
		int tmp=oldSettings.value("print-not-available", "1").toInt();
		PRINT_NOT_AVAILABLE_TIME_SLOTS=tmp;
		
		int tmp2=oldSettings.value("divide-html-timetables-with-time-axis-by-days", "0").toInt();
		DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS=tmp2;
	}
	else{
		if(newSettings.contains("fetmainformgeometry")){
			if(!newSettings.contains("output-directory")){
				OUTPUT_DIR=predefDir;
				
				QString s;
#ifdef Q_OS_WIN
				s+=QObject::tr("IMPORTANT NOTE for Windows users: the results directory location will be changed. Please"
				 " check out the new location specified below:");
				s+="\n\n";
#endif
				s+=QObject::tr("You upgraded FET to a new version. Beginning with FET-5.10.0 (June 2009),"
				 " you can change the output directory of FET (see the 'Settings' menu).")
				 +"\n\n"+QObject::tr("For the moment, the output (results) directory will be set to the predefined value %1")
				 .arg(QDir::toNativeSeparators(OUTPUT_DIR))
				 +" . "+ QObject::tr("You can change it later", "It refers to the output directory");

				QMessageBox::information(NULL, QObject::tr("FET important note"), s, QObject::tr("OK, I have read this"));
			}
			else{
				OUTPUT_DIR=newSettings.value("output-directory", predefDir).toString();
				QDir dir;
				if(!dir.exists(OUTPUT_DIR)){
					bool t=dir.mkpath(OUTPUT_DIR);
					if(!t){
						QMessageBox::warning(NULL, QObject::tr("FET warning"), QObject::tr("Output directory %1 does not exist and cannot be"
						 " created - output directory will be made the default value %2")
						 .arg(QDir::toNativeSeparators(OUTPUT_DIR)).arg(QDir::toNativeSeparators(predefDir)));
						OUTPUT_DIR=predefDir;
					}
				}
			}
		}
		else{
			//assert(0);
			QMessageBox::critical(NULL, QObject::tr("FET critical"), QObject::tr("You have found a bug in FET - please report error. The bug is in file %1, line %2")
			 .arg(__FILE__).arg(__LINE__)+"\n\n"+QObject::tr("The bug is that FET wrote in configuration file the version, but did not write the geometry of the main form")
			 +"\n\n"+QObject::tr("FET will now continue, but you most likely found a bug."));
			OUTPUT_DIR=newSettings.value("output-directory", predefDir).toString();
		}

		FET_LANGUAGE=newSettings.value("language", "en_GB").toString();
		WORKING_DIRECTORY=newSettings.value("working-directory", "examples").toString();
		IMPORT_DIRECTORY=newSettings.value("import-directory", OUTPUT_DIR).toString();
	
		QDir d(WORKING_DIRECTORY);
		if(!d.exists())
			WORKING_DIRECTORY="examples";
		QDir d2(WORKING_DIRECTORY);
		if(!d2.exists())
			WORKING_DIRECTORY=QDir::homePath();
	
		QDir i(IMPORT_DIRECTORY);
		if(!i.exists())
			IMPORT_DIRECTORY=OUTPUT_DIR;	// IMPORT_DIRECTORY="import";
	
		checkForUpdates=newSettings.value("check-for-updates", "-1").toInt();
		QString ver=newSettings.value("version", "-1").toString();
		
		if(!newSettings.contains("html-level"))
			TIMETABLE_HTML_LEVEL=newSettings.value("timetable-html-level", "2").toInt();
		else
			TIMETABLE_HTML_LEVEL=newSettings.value("html-level", "2").toInt();
	
		int tmp=newSettings.value("print-not-available", "1").toInt();
		PRINT_NOT_AVAILABLE_TIME_SLOTS=tmp;

		int tmp2=newSettings.value("divide-html-timetables-with-time-axis-by-days", "0").toInt();
		DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS=tmp2;
	}
	
	cout<<"Settings read"<<endl;
}

void writeSimulationParameters(){
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue("output-directory", OUTPUT_DIR);
	settings.setValue("language", FET_LANGUAGE);
	settings.setValue("working-directory", WORKING_DIRECTORY);
	settings.setValue("import-directory", IMPORT_DIRECTORY);
	settings.setValue("version", FET_VERSION);
	settings.setValue("check-for-updates", checkForUpdates);
	//settings.setValue("timetable-html-level", TIMETABLE_HTML_LEVEL);
	settings.setValue("html-level", TIMETABLE_HTML_LEVEL);

	int k;
	if(DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS)
		k=1;
	else
		k=0;
	settings.setValue("divide-html-timetables-with-time-axis-by-days", k);
	
	int tmp=1;
	if(!PRINT_NOT_AVAILABLE_TIME_SLOTS)
		tmp=0;
	settings.setValue("print-not-available", tmp);
}

void setLanguage(QApplication& qapplication)
{
	//translator stuff
	QDir d("/usr/share/fet/translations");
	
	bool translation_loaded=false;
	
	//this is one place (out of 2) in which you need to add a new language
	if(FET_LANGUAGE=="ar" || FET_LANGUAGE=="ca" || FET_LANGUAGE=="de" || FET_LANGUAGE=="es" || FET_LANGUAGE=="el" || FET_LANGUAGE=="fr"
	 || FET_LANGUAGE=="hu" || FET_LANGUAGE=="mk" || FET_LANGUAGE=="ms" || FET_LANGUAGE=="nl" || FET_LANGUAGE=="pl" || FET_LANGUAGE=="ro"
	 || FET_LANGUAGE=="tr" || FET_LANGUAGE=="id" || FET_LANGUAGE=="it" || FET_LANGUAGE=="lt"
	 || FET_LANGUAGE=="ru" || FET_LANGUAGE=="fa"){
	 /*
		if(d.exists())
			translation_loaded=translator.load("fet_"+FET_LANGUAGE, "/usr/share/fet/translations");
		if(!d.exists() || !translation_loaded){
			translation_loaded=translator.load("fet_"+FET_LANGUAGE, qapplication.applicationDirPath()+"/translations");
			if(!translation_loaded)
				translation_loaded=translator.load("fet_"+FET_LANGUAGE, qapplication.applicationDirPath());
		}*/
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
		if(FET_LANGUAGE!="en_GB"){
			QMessageBox::warning(NULL, QObject::tr("FET warning"), 
			 QObject::tr("Specified language is incorrect - making it en_GB (English)"));
			FET_LANGUAGE="en_GB";
		}
		
		assert(FET_LANGUAGE=="en_GB");
		
		translation_loaded=true;
	}
	
	if(!translation_loaded){
		QMessageBox::warning(NULL, QObject::tr("FET warning"), 
		 QObject::tr("Translation for specified language not loaded - this is an error, maybe translation file is missing - making language en_GB (English)")
		+"\n\n"+
		QObject::tr("FET searched for translation file %1 in directories %2 and %3 (and %4 under UNIX like systems), but could not find it.")
		 .arg("fet_"+FET_LANGUAGE+".qm")
		 .arg(QDir::toNativeSeparators(qapplication.applicationDirPath()))
		 .arg(QDir::toNativeSeparators(qapplication.applicationDirPath()+"/translations"))
		 .arg("/usr/share/fet/translations")
		 );
		FET_LANGUAGE="en_GB";
	}
	
	if(FET_LANGUAGE=="ar" || FET_LANGUAGE=="he" || FET_LANGUAGE=="fa" || FET_LANGUAGE=="ur" /* or others??? */){
		LANGUAGE_STYLE_RIGHT_TO_LEFT=true;
	}
	else{
		LANGUAGE_STYLE_RIGHT_TO_LEFT=false;
	}
	
	//if(FET_LANGUAGE=="en_GB")
	//	LANGUAGE_FOR_HTML="en";
	//else
	if(FET_LANGUAGE=="zh_CN")
		LANGUAGE_FOR_HTML="zh-Hans";
	else if(FET_LANGUAGE=="zh_TW")
		LANGUAGE_FOR_HTML="zh-Hant";
	else{
		if(FET_LANGUAGE=="en_GB")
			LANGUAGE_FOR_HTML=FET_LANGUAGE.left(2);
		else
			LANGUAGE_FOR_HTML=FET_LANGUAGE.replace(QString("_"), QString("-"));
	}
		
	qapplication.installTranslator(&translator);	
	
	/*QTranslator qtTranslator;
	qtTranslator.load("qt_" + FET_LANGUAGE, qapplication.applicationDirPath());
	qapplication.installTranslator(&qtTranslator);*/
	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==true)
		qapplication.setLayoutDirection(Qt::RightToLeft);
}

/**
FET starts here
*/
int main(int argc, char **argv)
{
	QApplication qapplication(argc, argv);
	
	QObject::connect(&qapplication, SIGNAL(lastWindowClosed()), &qapplication, SLOT(quit()));

	srand(unsigned(time(NULL))); //useless, I use randomKnuth(), but just in case I use somewhere rand() by mistake...

	initRandomKnuth();

	OUTPUT_DIR=QDir::homePath()+FILE_SEP+"fet-results";

	if(argc==1){
		readSimulationParameters();
	
		QDir dir;
	
		bool t=true;

		//make sure that the output directory exists
		if(!dir.exists(OUTPUT_DIR))
			t=dir.mkpath(OUTPUT_DIR);

		if(!t){
			/*QMessageBox::critical(NULL, QObject::tr("FET critical"), QObject::tr("Cannot create or use %1 directory - FET will now abort").arg(QDir::toNativeSeparators(OUTPUT_DIR)));
			assert(0);
			exit(1);*/
			QMessageBox::critical(NULL, QObject::tr("FET critical"), QObject::tr("Cannot create or use %1 directory (where the results should be stored) - you can continue operation, but you might not be able to work with FET."
			 " Maybe you can try to change the output directory from the 'Settings' menu. If this is a bug - please report it.").arg(QDir::toNativeSeparators(OUTPUT_DIR)));
		}
		
		QString testFileName=OUTPUT_DIR+FILE_SEP+"test_write_permissions_1.tmp";
		QFile test(testFileName);
		bool existedBefore=test.exists();
		bool t_t=test.open(QIODevice::ReadWrite);
		//if(!test.exists())
		//	t_t=false;
		if(!t_t){
			QMessageBox::critical(NULL, QObject::tr("FET critical"), QObject::tr("You don't have write permissions in the output directory "
			 "(FET cannot open or create file %1) - you might not be able to work correctly with FET. Maybe you can try to change the output directory from the 'Settings' menu."
			 " If this is a bug - please report it.").arg(testFileName));
		}
		else{
			test.close();
			if(!existedBefore)
				test.remove();
		}
	}

	students_schedule_ready=0;
	teachers_schedule_ready=0;
	rooms_schedule_ready=0;

	if(argc==1 && checkForUpdates==-1){
		/*int t=QMessageBox::question(NULL, QObject::tr("FET question"),
		 QObject::tr("Would you like FET to inform you of available new version by checking the FET web page?\n\n"
		 "This setting can be changed later from Settings menu\n\n"
		 ""),
		 QObject::tr("&Yes"), QObject::tr("&No"), QString(),
		 0, 1 );
		
		if(t==0){ //yes
			cout<<"Pressed yes"<<endl;
			checkForUpdates=1;
		}
		else{
			assert(t==1);
			cout<<"Pressed no"<<endl;
			checkForUpdates=0;
		}*/
		checkForUpdates=0;
	}


	/////////////////////////////////////////////////
	//begin command line
	if(argc>1){
		QString outputDirectory="";
	
		/*if(argc>=5){
			cout<<"Usage: fet inputfile.fet [timelimitseconds] [timetablehtmllevel (0..5)]"<<endl;
			return 1;
		}*/
		
		INPUT_FILENAME_XML="";
		
		QString filename="";
		
		int secondsLimit=2000000000;
		
		TIMETABLE_HTML_LEVEL=2;
		
		FET_LANGUAGE="en_GB";
		
		PRINT_NOT_AVAILABLE_TIME_SLOTS=true;
		
		DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS=false;

		for(int i=1; i<argc; i++){
			QString s=argv[i];
			
			if(s.left(12)=="--inputfile=")
				filename=s.right(s.length()-12);
			else if(s.left(19)=="--timelimitseconds=")
				secondsLimit=s.right(s.length()-19).toInt();
			else if(s.left(21)=="--timetablehtmllevel=")
				TIMETABLE_HTML_LEVEL=s.right(s.length()-21).toInt();
			else if(s.left(12)=="--htmllevel=")
				TIMETABLE_HTML_LEVEL=s.right(s.length()-12).toInt();
			else if(s.left(11)=="--language=")
				FET_LANGUAGE=s.right(s.length()-11);
			else if(s.left(20)=="--printnotavailable="){
				if(s.right(5)=="false")
					PRINT_NOT_AVAILABLE_TIME_SLOTS=false;
				else
					PRINT_NOT_AVAILABLE_TIME_SLOTS=true;
			}
			else if(s.left(23)=="--dividetimeaxisbydays="){
				if(s.right(5)=="false")
					DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS=false;
				else
					DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS=true;
			}
			else if(s.left(12)=="--outputdir="){
				outputDirectory=s.right(s.length()-12);
			}
		}
		
		INPUT_FILENAME_XML=filename;
		
		QString initialDir=outputDirectory;
		if(initialDir!="")
			initialDir.append(FILE_SEP);
		
		if(outputDirectory!="")
			outputDirectory.append(FILE_SEP);
		outputDirectory.append("timetables");

		//////////
		if(INPUT_FILENAME_XML!=""){
			outputDirectory.append(FILE_SEP);
			outputDirectory.append(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1));
			if(outputDirectory.right(4)==".fet")
				outputDirectory=outputDirectory.left(outputDirectory.length()-4);
			//else if(INPUT_FILENAME_XML!="")
			//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;
		}
		//////////
		
		QDir dir;
		if(outputDirectory!="")
			if(!dir.exists(outputDirectory))
				dir.mkpath(outputDirectory);
		
		if(outputDirectory!="")
			outputDirectory.append(FILE_SEP);
			
		QString logsDir=initialDir+"logs";
		if(!dir.exists(logsDir))
			dir.mkpath(logsDir);
		logsDir.append(FILE_SEP);
		
		////////
		QFile logFile(logsDir+"result.txt");
		bool tttt=logFile.open(QIODevice::WriteOnly);
		if(!tttt){
			cout<<"FET critical - you don't have write permissions in the output directory - (FET cannot open or create file test_write_permissions_2.tmp) (or in the current directory, if you did not specify output directory)"
			 " If this is a bug - please report it.\n";
			return 1;
		}
		QTextStream out(&logFile);
		//ofstream out(logsDir+"result.txt");
		///////
		
		out<<"This file contains the result (log) of last operation"<<endl<<endl;
		
		QFile test(outputDirectory+"test_write_permissions_2.tmp");
		bool existedBefore=test.exists();
		bool t_t=test.open(QIODevice::ReadWrite);
		if(!t_t){
			cout<<"FET critical - you don't have write permissions in the output directory - (FET cannot open or create file test_write_permissions_2.tmp) (or in the current directory, if you did not specify output directory)"
			 " If this is a bug - please report it.\n";
			out<<"FET critical - you don't have write permissions in the output directory - (FET cannot open or create file test_write_permissions_2.tmp)(or in the current directory, if you did not specify output directory)"
			 " If this is a bug - please report it.\n";
			//test.close();
			return 1;
		}
		else{
			test.close();
			if(!existedBefore)
				test.remove();
		}
		
		if(filename==""){
			cout<<"Incorrect parameters (input file not specified). Please see README for usage (basically,\n"
			 "fet --inputfile=x [--outputdirectory=d] [--timelimitseconds=y] [--htmllevel=z] [--language=t] [--printnotavailable=true|false] [--dividetimeaxisbydays=true|false]\n"
			 "where z is from 0 to 6 and language is en_GB, de, ro or other implemented language in FET)"<<endl;
			out<<"Incorrect parameters (input file not specified). Please see README for usage (basically,\n"
			 "fet --inputfile=x [--outputdirectory=d] [--timelimitseconds=y] [--timetablehtmllevel=z] [--language=t] [--printnotavailable=true|false] [--dividetimeaxisbydays=true|false]\n"
			 "where z is from 0 to 6 and language is en_GB, de, ro or other implemented language in FET)"<<endl;
			logFile.close();
			return 1;
		}	
		if(secondsLimit==0){
			cout<<"Incorrect parameters (time limit is 0 seconds). Please see README for usage (basically,\n"
			 "fet --inputfile=x [--outputdirectory=d] [--timelimitseconds=y] [--htmllevel=z] [--language=t] [--printnotavailable=true|false] [--dividetimeaxisbydays=true|false]\n"
			 "where z is from 0 to 6 and language is en_GB, de, ro or other implemented language in FET)"<<endl;
			out<<"Incorrect parameters (time limit is 0 seconds). Please see README for usage (basically,\n"
			 "fet --inputfile=x [--outputdirectory=d] [--timelimitseconds=y] [--htmllevel=z] [--language=t] [--printnotavailable=true|false] [--dividetimeaxisbydays=true|false]\n"
			 "where z is from 0 to 6 and language is en_GB, de, ro or other implemented language in FET)"<<endl;
			logFile.close();
			return 1;
		}	
		if(TIMETABLE_HTML_LEVEL>6 || TIMETABLE_HTML_LEVEL<0){
			cout<<"Incorrect parameters (html level must be 0, 1, 2, 3, 4, 5 or 6). Please see README for usage (basically,\n"
			 "fet --inputfile=x [--outputdirectory=d] [--timelimitseconds=y] [--htmllevel=z] [--language=t] [--printnotavailable=true|false] [--dividetimeaxisbydays=true|false]\n"
			 "where z is from 0 to 6 and language is en_GB, de, ro or other implemented language in FET)"<<endl;
			out<<"Incorrect parameters (html level must be 0, 1, 2, 3, 4, 5 or 6). Please see README for usage (basically,\n"
			 "fet --inputfile=x [--outputdirectory=d] [--timelimitseconds=y] [--htmllevel=z] [--language=t] [--printnotavailable=true|false] [--dividetimeaxisbydays=true|false]\n"
			 "where z is from 0 to 6 and language is en_GB, de, ro or other implemented language in FET)"<<endl;
			logFile.close();
			return 1;
		}	
		
		setLanguage(qapplication);
		
		if(TIMETABLE_HTML_LEVEL>6 || TIMETABLE_HTML_LEVEL<0)
			TIMETABLE_HTML_LEVEL=2;
	
		bool t=gt.rules.read(filename, true, initialDir);
		if(!t){
			cout<<"Cannot read file - aborting"<<endl;
			out<<"Cannot read file - aborting"<<endl;
			logFile.close();
			return 1;
		}
		
		t=gt.rules.computeInternalStructure();
		if(!t){
			cout<<"Cannot compute internal structure - aborting"<<endl;
			out<<"Cannot compute internal structure - aborting"<<endl;
			logFile.close();
			return 1;
		}
	
		Generate gen;
	
		gen.abortOptimization=false;
		bool ok=gen.precompute();
		
		if(!ok){
			cout<<"Cannot precompute - data is wrong - aborting"<<endl;
			out<<"Cannot precompute - data is wrong - aborting"<<endl;
			logFile.close();
			return 1;
		}
	
		bool impossible, timeExceeded;
		
		cout<<"secondsLimit=="<<secondsLimit<<endl;
		//out<<"secondsLimit=="<<secondsLimit<<endl;
				
		gen.generate(secondsLimit, impossible, timeExceeded, false); //false means no thread
	
		if(impossible){
			cout<<"Impossible"<<endl;
			out<<"Impossible"<<endl;
		}
		else if(timeExceeded){
			cout<<"Time exceeded"<<endl;
			out<<"Time exceeded"<<endl;
		}
		else{
			cout<<"Simulation successful"<<endl;
			out<<"Simulation successful"<<endl;
		
			Solution& c=gen.c;

			//needed to find the conflicts strings
			QString tmp;
			c.fitness(gt.rules, &tmp);
			
			TimetableExport::getStudentsTimetable(c);
			TimetableExport::getTeachersTimetable(c);
			TimetableExport::getRoomsTimetable(c);

			TimetableExport::writeSimulationResultsCommandLine(outputDirectory);			
		}
	
		logFile.close();
		return 0;
	}
	//end command line
	/////////////////////////////////////////////////

	setLanguage(qapplication);

	pqapplication=&qapplication;
	FetMainForm fetMainForm;
	//qapplication.setMainWidget(&fetMainForm);
	fetMainForm.show();

	int tmp2=qapplication.exec();
	
	writeSimulationParameters();
	
	cout<<"Settings saved"<<endl;
	
	return tmp2;
}
