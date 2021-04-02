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

extern bool students_schedule_ready, teachers_schedule_ready;

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


qint16 teachers_timetable_weekly[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
qint16 students_timetable_weekly[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
qint16 rooms_timetable_weekly[MAX_ROOMS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

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
	QSettings settings("FET free software", "FET");
	FET_LANGUAGE=settings.value("language", "en_GB").toString();
	WORKING_DIRECTORY=settings.value("working-directory", "sample_inputs").toString();
	checkForUpdates=settings.value("check-for-updates", "-1").toInt();
	QString ver=settings.value("version", "-1").toString();
	cout<<"Settings read"<<endl;
}

void writeSimulationParameters(){
	QSettings settings("FET free software", "FET");
	settings.setValue("language", FET_LANGUAGE);
	settings.setValue("working-directory", WORKING_DIRECTORY);
	settings.setValue("version", FET_VERSION);
	settings.setValue("check-for-updates", checkForUpdates);
}

/**
FET starts here
*/
int main(int argc, char **argv){

	//srand(unsigned(time(NULL)));
	initRandomKnuth();

	QDir dir;
	
	bool t=true;

	//make sure that the output directory exists
	if(!dir.exists(OUTPUT_DIR))
		t=dir.mkdir(OUTPUT_DIR);
	if(!t){
		assert(0);
		exit(1);
	}

	readSimulationParameters();

	students_schedule_ready=0;
	teachers_schedule_ready=0;

	QApplication qapplication(argc, argv);
	
	QObject::connect(&qapplication, SIGNAL(lastWindowClosed()), &qapplication, SLOT(quit()));
	
	//translator stuff
	QDir d("/usr/share/fet/translations");
	
	if(FET_LANGUAGE=="ro"){
		if(d.exists())
			translator.load("fet_ro", "/usr/share/fet/translations");
		else
			translator.load("fet_ro", "translations");
	}
	else if(FET_LANGUAGE=="fr"){
		if(d.exists())
			translator.load("fet_fr", "/usr/share/fet/translations");
		else
			translator.load("fet_fr", "translations");
	}
	else if(FET_LANGUAGE=="ca"){
		if(d.exists())
			translator.load("fet_ca", "/usr/share/fet/translations");
		else
			translator.load("fet_ca", "translations");
	}
	else if(FET_LANGUAGE=="ms"){
		if(d.exists())
			translator.load("fet_ms", "/usr/share/fet/translations");
		else
			translator.load("fet_ms", "translations");
	}
	else if(FET_LANGUAGE=="pl"){
		if(d.exists())
			translator.load("fet_pl", "/usr/share/fet/translations");
		else
			translator.load("fet_pl", "translations");
	}
	else if(FET_LANGUAGE=="tr"){
		if(d.exists())
			translator.load("fet_tr", "/usr/share/fet/translations");
		else
			translator.load("fet_tr", "translations");
	}
	else if(FET_LANGUAGE=="nl"){
		if(d.exists())
			translator.load("fet_nl", "/usr/share/fet/translations");
		else
			translator.load("fet_nl", "translations");
	}
	else if(FET_LANGUAGE=="de"){
		if(d.exists())
			translator.load("fet_de", "/usr/share/fet/translations");
		else
			translator.load("fet_de", "translations");
	}
	else if(FET_LANGUAGE=="hu"){
		if(d.exists())
			translator.load("fet_hu", "/usr/share/fet/translations");
		else
			translator.load("fet_hu", "translations");
	}
	else if(FET_LANGUAGE=="mk"){
		if(d.exists())
			translator.load("fet_mk", "/usr/share/fet/translations");
		else
			translator.load("fet_mk", "translations");
	}
	else{
		assert(FET_LANGUAGE=="en_GB");
	}
		
	qapplication.installTranslator(&translator);	
	
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

	pqapplication=&qapplication;
	FetMainForm fetMainForm;
	//qapplication.setMainWidget(&fetMainForm);
	fetMainForm.show();

	int tmp2=qapplication.exec();
	
	writeSimulationParameters();
	
	cout<<"Settings saved"<<endl;
	
	return tmp2;
}
