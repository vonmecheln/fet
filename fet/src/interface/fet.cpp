/*
File fet.cpp - program using the main engine "genetictimetable"
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

#include "genetictimetable_defs.h"
#include "genetictimetable.h"
#include "fetmainform.h"

#include <qapplication.h>
#include <qmutex.h>
#include <qstring.h>
#include <qtranslator.h>

#include <QDir>
#include <QTranslator>

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
GeneticTimetable gt;

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


/***The simulation parameters***/

/**
Simulation parameter, read from the fet.ini file
*/
time_t timelimit;

/**
Simulation parameter, read from the fet.ini file
*/
int population_number;

/**
Simulation parameter, read from the fet.ini file
*/
int evolution_method;

/**
Simulation parameter, read from the fet.ini file
*/
int init_method;

/**
Simulation parameter, read from the file "fet.ini"
It represents the maximum allowed number of generations to iterate
*/
int max_generations;

int16 teachers_timetable_week1[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
int16 teachers_timetable_week2[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
int16 students_timetable_week1[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
int16 students_timetable_week2[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
int16 rooms_timetable_week1[MAX_ROOMS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
int16 rooms_timetable_week2[MAX_ROOMS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

QApplication* pqapplication=NULL;

static int fetch_line(ifstream& in, char *s){
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
}

void readSimulationParameters(){
	if(!QFile::exists(INI_FILENAME)){
		cout<<"File "<<(const char*)(INI_FILENAME)<<" not found...making a new one\n";
		writeDefaultSimulationParameters();
	}

	ifstream in(INI_FILENAME);
	char s[256];

	cout<<"Initializing parameters...reading file "<<(const char*)(INI_FILENAME)<<endl;

	//read main parameters of the simulation
	int tmp=fetch_line(in, s);
	assert(tmp==1);
	WORKING_DIRECTORY=s;
	cout<<"Read: working directory="<<s<<endl;

	tmp=fetch_line(in, s);
	assert(tmp==1);
	int tmp2;
	tmp=sscanf(s, "%d", &tmp2);
	assert(tmp==1);
	timelimit=tmp2;
	cout<<"Read: timelimit="<<timelimit<<" seconds (the maximum time the simulation is allowed to run)"<<endl;

	tmp=fetch_line(in, s);
	assert(tmp==1);
	tmp=sscanf(s, "%d", &tmp2);
	assert(tmp==1);
	max_generations=tmp2;
	cout<<"Read: max_generations the simulation is allowed to run="<<max_generations<<endl;

	tmp=fetch_line(in, s);
	assert(tmp==1);
	tmp=sscanf(s, "%d", &population_number);
	assert(tmp==1);
	if(population_number>MAX_POPULATION_SIZE){
		population_number=MAX_POPULATION_SIZE;
		cout<<"Population too large ("<<population_number<<"), making it MAX_POPULATION_SIZE="<<MAX_POPULATION_SIZE<<endl;
	}
	assert(population_number>0 && population_number<=MAX_POPULATION_SIZE);
	cout<<"Read: population number for the simulation="<<population_number<<endl;

	tmp=fetch_line(in, s);
	assert(tmp==1);
	tmp=sscanf(s, "%d", &evolution_method);
	assert(evolution_method==1 || evolution_method==2);
	cout<<"Read: simulation evolution method="<<evolution_method<<endl;

	//method 1 probabilities
	tmp=fetch_line(in, s);
	assert(tmp==1);
	tmp=sscanf(s, "%d", &METHOD1_MUTATION1_PROBABILITY);
	cout<<"Read: method1 mutation1 probability="<<METHOD1_MUTATION1_PROBABILITY<<endl;

	tmp=fetch_line(in, s);
	assert(tmp==1);
	tmp=sscanf(s, "%d", &METHOD1_MUTATION2_PROBABILITY);
	cout<<"Read: method1 mutation2 probability="<<METHOD1_MUTATION2_PROBABILITY<<endl;

	tmp=fetch_line(in, s);
	assert(tmp==1);
	tmp=sscanf(s, "%d", &METHOD1_CROSSOVER_PROBABILITY);
	cout<<"Read: method1 crossover probability="<<METHOD1_CROSSOVER_PROBABILITY<<endl;

	assert(METHOD1_MUTATION1_PROBABILITY+METHOD1_MUTATION2_PROBABILITY+METHOD1_CROSSOVER_PROBABILITY==100);

	//method 2 probabilities
	tmp=fetch_line(in, s);
	assert(tmp==1);
	tmp=sscanf(s, "%d", &METHOD2_MUTATION1_PROBABILITY);
	cout<<"Read: method2 mutation1 probability="<<METHOD2_MUTATION1_PROBABILITY<<endl;

	tmp=fetch_line(in, s);
	assert(tmp==1);
	tmp=sscanf(s, "%d", &METHOD2_MUTATION2_PROBABILITY);
	cout<<"Read: method2 mutation2 probability="<<METHOD2_MUTATION2_PROBABILITY<<endl;

	tmp=fetch_line(in, s);
	assert(tmp==1);
	tmp=sscanf(s, "%d", &METHOD2_CROSSOVER_PROBABILITY);
	cout<<"Read: method2 crossover probability="<<METHOD2_CROSSOVER_PROBABILITY<<endl;

	tmp=fetch_line(in, s);
	assert(tmp==1);
	tmp=sscanf(s, "%d", &METHOD2_PROPAGATION_PROBABILITY);
	cout<<"Read: method2 propagation probability="<<METHOD2_PROPAGATION_PROBABILITY<<endl;

	assert(METHOD2_MUTATION1_PROBABILITY+METHOD2_MUTATION2_PROBABILITY+METHOD2_CROSSOVER_PROBABILITY+METHOD2_PROPAGATION_PROBABILITY==100);
	
	tmp=fetch_line(in, s);
	if(tmp!=1){
		//older than version 3.12.21 - has no language saved
		FET_LANGUAGE="en_GB";
	}
	else{
		char ss[100];
		sscanf(s, "%s", ss);
		FET_LANGUAGE=ss;
		cout<<"Read: language="<<(const char*)(FET_LANGUAGE)<<endl;
		if(FET_LANGUAGE!="en_GB" && FET_LANGUAGE!="fr" 
		 && FET_LANGUAGE!="ro" && FET_LANGUAGE!="ca" 
		 && FET_LANGUAGE!="ms" && FET_LANGUAGE!="pl"
		 && FET_LANGUAGE!="tr" && FET_LANGUAGE!="nl"
		 && FET_LANGUAGE!="de" && FET_LANGUAGE!="hu"
		 && FET_LANGUAGE!="mk"){
			cout<<"Invalid language - making it english"<<endl;
			FET_LANGUAGE="en_GB";
		}
	}
}

void writeSimulationParameters(){
	ofstream out(INI_FILENAME);
	if(!out){
		assert(0);
		exit(1);
	}

	cout<<"Writing parameters to file "<<(const char*)(INI_FILENAME)<<endl;

	//read main parameters of the simulation
	out<<"# This is FET's configuration file (FET version="<<(const char*)(FET_VERSION)<<")"<<endl<<endl<<endl;

	out<<"# This is the working directory (used in open/save as file dialog)"<<endl;
	out<<(const char*)(WORKING_DIRECTORY)<<endl<<endl<<endl;

	out<<"# The time limit that the program is allowed to search for the solution (in seconds). Minimum"<<endl;
	out<<"# recommended value is 600"<<endl;
	out<<timelimit<<endl<<endl<<endl;

	out<<"# The maximum number of allowed generations"<<endl;
	out<<max_generations<<endl<<endl<<endl;

	out<<"# The population number (minimum recommended: 512, maximum: "<<MAX_POPULATION_SIZE<<")"<<endl;
	out<<"# (variable MAX_POPULATION_SIZE in file genetictimetable_defs.h)"<<endl;
	out<<"# (non-technical description: increasing this variable slows down the program,"<<endl;
	out<<"# but gives better results)."<<endl;
	out<<population_number<<endl<<endl<<endl;

	out<<"# The evolution method:"<<endl;
	out<<"# 1. Evolution1 - double the population, then select the best n/2 individuals (experimental)"<<endl;
	out<<"# 2. Evolution2 - selection is based on 3 tournament (classical)"<<endl;
	out<<"# (recommended: 2)"<<endl;
	out<<evolution_method<<endl<<endl<<endl;

	//method 1 probabilities
	out<<"#Evolution 1 - Mutation 1 probability (random swap - classical)"<<endl;
	out<<METHOD1_MUTATION1_PROBABILITY<<endl;

	out<<"#Evolution 1 - Mutation 2 probability (randomization - experimental)"<<endl;
	out<<METHOD1_MUTATION2_PROBABILITY<<endl;

	out<<"#Evolution 1 - Crossover probability"<<endl;
	out<<METHOD1_CROSSOVER_PROBABILITY<<endl<<endl<<endl;

	assert(METHOD1_MUTATION1_PROBABILITY+METHOD1_MUTATION2_PROBABILITY+METHOD1_CROSSOVER_PROBABILITY==100);

	//method 2 probabilities
	out<<"#Evolution 2 - Mutation 1 probability (random swap - classical)"<<endl;
	out<<METHOD2_MUTATION1_PROBABILITY<<endl;

	out<<"#Evolution 2 - Mutation 2 probability (randomization - experimental)"<<endl;
	out<<METHOD2_MUTATION2_PROBABILITY<<endl;

	out<<"#Evolution 2 - Crossover probability"<<endl;
	out<<METHOD2_CROSSOVER_PROBABILITY<<endl;

	out<<"#Evolution 2 - Propagation probability"<<endl;
	out<<METHOD2_PROPAGATION_PROBABILITY<<endl<<endl<<endl;

	assert(METHOD2_MUTATION1_PROBABILITY+METHOD2_MUTATION2_PROBABILITY+METHOD2_CROSSOVER_PROBABILITY+METHOD2_PROPAGATION_PROBABILITY==100);
	
	out<<"#FET Language"<<endl;
	out<<(const char*)(FET_LANGUAGE)<<endl;
}

void writeDefaultSimulationParameters(){
	ofstream out(INI_FILENAME);
	if(!out){
		assert(0);
		exit(1);
	}

	cout<<"Writing parameters to file "<<(const char*)(INI_FILENAME)<<endl;

	//read main parameters of the simulation
	out<<"# This is FET's configuration file (FET version="<<(const char*)(FET_VERSION)<<")"<<endl<<endl<<endl;

	out<<"# This is the working directory (used in open/save as file dialog)"<<endl;
	cout<<"Home dir path="<<(const char*)(QDir::homeDirPath())<<endl;
	out<<"sample_inputs"<<endl<<endl<<endl;

	out<<"# The time limit that the program is allowed to search for the solution (in seconds). Minimum"<<endl;
	out<<"# recommended value is 600"<<endl;
	out<<2000000000<<endl<<endl<<endl;

	out<<"# The maximum number of allowed generations"<<endl;
	out<<2000000000<<endl<<endl<<endl;

	out<<"# The population number (minimum recommended: 512, maximum: "<<MAX_POPULATION_SIZE<<")"<<endl;
	out<<"# (variable MAX_POPULATION_SIZE in file genetictimetable_defs.h)"<<endl;
	out<<"# (non-technical description: increasing this variable slows down the program,"<<endl;
	out<<"# but gives better results)."<<endl;
	out<<MAX_POPULATION_SIZE<<endl<<endl<<endl;

	out<<"# The evolution method:"<<endl;
	out<<"# 1. Evolution1 - double the population, then select the best n/2 individuals (experimental)"<<endl;
	out<<"# 2. Evolution2 - selection is based on 3 tournament (classical)"<<endl;
	out<<"# (recommended: 2)"<<endl;
	out<<2<<endl<<endl<<endl;

	//method 1 probabilities
	out<<"#Evolution 1 - Mutation 1 probability (random swap - classical)"<<endl;
	out<<40<<endl;

	out<<"#Evolution 1 - Mutation 2 probability (randomization - experimental)"<<endl;
	out<<40<<endl;

	out<<"#Evolution 1 - Crossover probability"<<endl;
	out<<20<<endl<<endl<<endl;

	//method 2 probabilities
	out<<"#Evolution 2 - Mutation 1 probability (random swap - classical)"<<endl;
	out<<35<<endl;

	out<<"#Evolution 2 - Mutation 2 probability (randomization - experimental)"<<endl;
	out<<35<<endl;

	out<<"#Evolution 2 - Crossover probability"<<endl;
	out<<20<<endl;

	out<<"#Evolution 2 - Propagation probability"<<endl;
	out<<10<<endl<<endl<<endl;

	out<<"#FET Language"<<endl;
	out<<(const char*)(FET_LANGUAGE)<<endl;
}

/**
FET starts here
*/
int main(int argc, char **argv){

	srand(unsigned(time(NULL)));

	QDir dir;
	
	bool t=true;

	//make sure that the input directory exists - only for GNU/Linux
	//For Windows, I make a "fet.ini" in the current working directory
#ifndef WIN32
	if(!dir.exists(QDir::homeDirPath()+"/.fet"))
		t=dir.mkdir(QDir::homeDirPath()+"/.fet");
	if(!t){
		assert(0);
		exit(1);
	}
#endif

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
	//end translator stuff
	
	pqapplication=&qapplication;
	FetMainForm fetMainForm;
	//qapplication.setMainWidget(&fetMainForm);
	fetMainForm.show();

	int tmp2=qapplication.exec();
	
	writeSimulationParameters();
	return tmp2;
}
