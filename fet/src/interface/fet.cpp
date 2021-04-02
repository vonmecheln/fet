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
	QSettings newSettings(COMPANY, PROGRAM);
	QString newVer=newSettings.value("version", "-1").toString();
	if(newVer=="-1"){
		QSettings oldSettings("FET free software", "FET");
		FET_LANGUAGE=oldSettings.value("language", "en_GB").toString();
		WORKING_DIRECTORY=oldSettings.value("working-directory", "sample_inputs").toString();
		IMPORT_DIRECTORY=oldSettings.value("import-directory", OUTPUT_DIR).toString();
	
		QDir d(WORKING_DIRECTORY);
		if(!d.exists())
			WORKING_DIRECTORY="sample_inputs";
	
		QDir i(IMPORT_DIRECTORY);
		if(!i.exists())
			IMPORT_DIRECTORY=OUTPUT_DIR;	// IMPORT_DIRECTORY="import";
	
		checkForUpdates=oldSettings.value("check-for-updates", "-1").toInt();
		QString ver=oldSettings.value("version", "-1").toString();
		TIMETABLE_HTML_LEVEL=oldSettings.value("timetable-html-level", "2").toInt();
	
		int tmp=oldSettings.value("print-not-available", "1").toInt();
		PRINT_NOT_AVAILABLE_TIME_SLOTS=tmp;
		
		int tmp2=oldSettings.value("divide-html-timetables-with-time-axis-by-days", "0").toInt();
		DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS=tmp2;
	}
	else{
		FET_LANGUAGE=newSettings.value("language", "en_GB").toString();
		WORKING_DIRECTORY=newSettings.value("working-directory", "sample_inputs").toString();
		IMPORT_DIRECTORY=newSettings.value("import-directory", OUTPUT_DIR).toString();
	
		QDir d(WORKING_DIRECTORY);
		if(!d.exists())
			WORKING_DIRECTORY="sample_inputs";
	
		QDir i(IMPORT_DIRECTORY);
		if(!i.exists())
			IMPORT_DIRECTORY=OUTPUT_DIR;	// IMPORT_DIRECTORY="import";
	
		checkForUpdates=newSettings.value("check-for-updates", "-1").toInt();
		QString ver=newSettings.value("version", "-1").toString();
		TIMETABLE_HTML_LEVEL=newSettings.value("timetable-html-level", "2").toInt();
	
		int tmp=newSettings.value("print-not-available", "1").toInt();
		PRINT_NOT_AVAILABLE_TIME_SLOTS=tmp;

		int tmp2=newSettings.value("divide-html-timetables-with-time-axis-by-days", "0").toInt();
		DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS=tmp2;
	}
	
	cout<<"Settings read"<<endl;
}

void writeSimulationParameters(){
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue("language", FET_LANGUAGE);
	settings.setValue("working-directory", WORKING_DIRECTORY);
	settings.setValue("import-directory", IMPORT_DIRECTORY);
	settings.setValue("version", FET_VERSION);
	settings.setValue("check-for-updates", checkForUpdates);
	settings.setValue("timetable-html-level", TIMETABLE_HTML_LEVEL);

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
	
	if(FET_LANGUAGE=="ar" || FET_LANGUAGE=="ca" || FET_LANGUAGE=="de" || FET_LANGUAGE=="es" || FET_LANGUAGE=="el" || FET_LANGUAGE=="fr"
	 || FET_LANGUAGE=="hu" || FET_LANGUAGE=="mk" || FET_LANGUAGE=="ms" || FET_LANGUAGE=="nl" || FET_LANGUAGE=="pl" || FET_LANGUAGE=="ro"
	 || FET_LANGUAGE=="tr" || FET_LANGUAGE=="id" || FET_LANGUAGE=="it" || FET_LANGUAGE=="lt"){
		if(d.exists())
			translation_loaded=translator.load("fet_"+FET_LANGUAGE, "/usr/share/fet/translations");
		if(!d.exists() || !translation_loaded){
			translation_loaded=translator.load("fet_"+FET_LANGUAGE, qapplication.applicationDirPath()+"/translations");
			if(!translation_loaded)
				translation_loaded=translator.load("fet_"+FET_LANGUAGE, qapplication.applicationDirPath());
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
		QObject::tr("FET searched for translation file %1 in directories %2 (on UNIX like systems), %3 and %4 and could not find it.")
		 .arg("fet_"+FET_LANGUAGE+".qm")
		 .arg("/usr/share/fet/translations")
		 .arg(qapplication.applicationDirPath()+"/translations")
		 .arg(qapplication.applicationDirPath())
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

	srand(unsigned(time(NULL))); //useless, I use randomKnuth(), but just in case I use somewhere rand() by mistake...
	
	initRandomKnuth();

	bool t=true;

	if(argc==1){
		QDir dir;
	
		//make sure that the output directory exists
		if(!dir.exists(OUTPUT_DIR))
			t=dir.mkdir(OUTPUT_DIR);
	}

	readSimulationParameters();
	
	students_schedule_ready=0;
	teachers_schedule_ready=0;
	rooms_schedule_ready=0;

	QApplication qapplication(argc, argv);
	
	QObject::connect(&qapplication, SIGNAL(lastWindowClosed()), &qapplication, SLOT(quit()));
	
	if(!t){
		QMessageBox::critical(NULL, QObject::tr("FET critical"), QObject::tr("Cannot create or use %1 directory - FET will now abort").arg(QDir::toNativeSeparators(OUTPUT_DIR)));
		assert(0);
		exit(1);
	}

	if(checkForUpdates==-1){
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
		ofstream out("result.txt");
		
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
		}
		
		if(filename==""){
			cout<<"Incorrect parameters (input file not specified). Please see README for usage (basically,\n"
			 "fet --inputfile=x [--timelimitseconds=y] [--htmllevel=z] [--language=t] [--printnotavailable=true|false] [--dividetimeaxisbydays=true|false]\n"
			 "where z is from 0 to 6 and language is en_GB, de, ro or other implemented language in FET)"<<endl;
			out<<"Incorrect parameters (input file not specified). Please see README for usage (basically,\n"
			 "fet --inputfile=x [--timelimitseconds=y] [--timetablehtmllevel=z] [--language=t] [--printnotavailable=true|false] [--dividetimeaxisbydays=true|false]\n"
			 "where z is from 0 to 6 and language is en_GB, de, ro or other implemented language in FET)"<<endl;
			return 1;
		}	
		if(secondsLimit==0){
			cout<<"Incorrect parameters (time limit is 0 seconds). Please see README for usage (basically,\n"
			 "fet --inputfile=x [--timelimitseconds=y] [--htmllevel=z] [--language=t] [--printnotavailable=true|false] [--dividetimeaxisbydays=true|false]\n"
			 "where z is from 0 to 6 and language is en_GB, de, ro or other implemented language in FET)"<<endl;
			out<<"Incorrect parameters (time limit is 0 seconds). Please see README for usage (basically,\n"
			 "fet --inputfile=x [--timelimitseconds=y] [--htmllevel=z] [--language=t] [--printnotavailable=true|false] [--dividetimeaxisbydays=true|false]\n"
			 "where z is from 0 to 6 and language is en_GB, de, ro or other implemented language in FET)"<<endl;
			return 1;
		}	
		if(TIMETABLE_HTML_LEVEL>6 || TIMETABLE_HTML_LEVEL<0){
			cout<<"Incorrect parameters (html level must be 0, 1, 2, 3, 4, 5 or 6). Please see README for usage (basically,\n"
			 "fet --inputfile=x [--timelimitseconds=y] [--htmllevel=z] [--language=t] [--printnotavailable=true|false] [--dividetimeaxisbydays=true|false]\n"
			 "where z is from 0 to 6 and language is en_GB, de, ro or other implemented language in FET)"<<endl;
			out<<"Incorrect parameters (html level must be 0, 1, 2, 3, 4, 5 or 6). Please see README for usage (basically,\n"
			 "fet --inputfile=x [--timelimitseconds=y] [--htmllevel=z] [--language=t] [--printnotavailable=true|false] [--dividetimeaxisbydays=true|false]\n"
			 "where z is from 0 to 6 and language is en_GB, de, ro or other implemented language in FET)"<<endl;
			return 1;
		}	
		
		setLanguage(qapplication);
		
		if(TIMETABLE_HTML_LEVEL>6 || TIMETABLE_HTML_LEVEL<0)
			TIMETABLE_HTML_LEVEL=2;
	
		bool t=gt.rules.read(filename, true);
		if(!t){
			cout<<"Cannot read file - aborting"<<endl;
			out<<"Cannot read file - aborting"<<endl;
			return 1;
		}
		
		t=gt.rules.computeInternalStructure();
		if(!t){
			cout<<"Cannot compute internal structure - aborting"<<endl;
			out<<"Cannot compute internal structure - aborting"<<endl;
			return 1;
		}
	
		Generate gen;
	
		gen.abortOptimization=false;
		bool ok=gen.precompute();
		
		if(!ok){
			cout<<"Cannot precompute - data is wrong - aborting"<<endl;
			out<<"Cannot precompute - data is wrong - aborting"<<endl;
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

			TimetableExport::writeSimulationResultsCommandLine();			
		}
	
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
