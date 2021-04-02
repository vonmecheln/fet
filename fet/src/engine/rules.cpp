/*
File rules.cpp
*/

/***************************************************************************
                          rules.cpp  -  description
                             -------------------
    begin                : 2002
    copyright            : (C) 2002 by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "timetable_defs.h"
#include "rules.h"

#include <QDir>

#include <algorithm>
#include <iostream>
using namespace std;

#include <QTextStream>
#include <QFile>
#include <QFileInfo>

#include <QDate>
#include <QTime>
#include <QLocale>

#include <QString>

#include <QXmlStreamReader>

#include <QTranslator>

#include <QtAlgorithms>
#include <QtGlobal>

#include <QSet>
#include <QHash>

#include <list>
#include <iterator>

//#include <QApplication>
#ifndef FET_COMMAND_LINE
#include <QProgressDialog>
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#else
#include <QRegExp>
#endif

#include "messageboxes.h"

#include "lockunlock.h"

//static bool toSkipTime[MAX_TIME_CONSTRAINTS];
//static bool toSkipSpace[MAX_SPACE_CONSTRAINTS];

//extern QApplication* pqapplication;

extern bool students_schedule_ready;
extern bool rooms_schedule_ready;
extern bool teachers_schedule_ready;

FakeString::FakeString()
{
}

void FakeString::operator=(const QString& other)
{
	Q_UNUSED(other);
}

void FakeString::operator=(const char* str)
{
	Q_UNUSED(str);
}

void FakeString::operator+=(const QString& other)
{
	Q_UNUSED(other);
}

void FakeString::operator+=(const char* str)
{
	Q_UNUSED(str);
}


void Rules::init() //initializes the rules (empty, but with default hours and days)
{
	this->institutionName=tr("Default institution");
	this->comments=tr("Default comments");

	this->nDaysPerWeek=5;
	this->daysOfTheWeek[0] = tr("Monday");
	this->daysOfTheWeek[1] = tr("Tuesday");
	this->daysOfTheWeek[2] = tr("Wednesday");
	this->daysOfTheWeek[3] = tr("Thursday");
	this->daysOfTheWeek[4] = tr("Friday");
	
	//defaults
	this->nHoursPerDay=12;
	this->hoursOfTheDay[0]=tr("08:00", "Hour name");
	this->hoursOfTheDay[1]=tr("09:00", "Hour name");
	this->hoursOfTheDay[2]=tr("10:00", "Hour name");
	this->hoursOfTheDay[3]=tr("11:00", "Hour name");
	this->hoursOfTheDay[4]=tr("12:00", "Hour name");
	this->hoursOfTheDay[5]=tr("13:00", "Hour name");
	this->hoursOfTheDay[6]=tr("14:00", "Hour name");
	this->hoursOfTheDay[7]=tr("15:00", "Hour name");
	this->hoursOfTheDay[8]=tr("16:00", "Hour name");
	this->hoursOfTheDay[9]=tr("17:00", "Hour name");
	this->hoursOfTheDay[10]=tr("18:00", "Hour name");
	this->hoursOfTheDay[11]=tr("19:00", "Hour name");
	//this->hoursOfTheDay[12]=tr("20:00", "Hours name");

	permanentStudentsHash.clear();
	
	activitiesPointerHash.clear();
	bctSet.clear();
	btSet.clear();
	bcsSet.clear();
	apstHash.clear();
	aprHash.clear();
	mdbaHash.clear();
	tnatHash.clear();
	ssnatHash.clear();
	
	this->initialized=true;
}

bool Rules::computeInternalStructure(QWidget* parent)
{
	//To fix a bug reported by Frans on forum, on 7 May 2010.
	//If user generates, then changes some activities (changes teachers of them), then tries to generate but FET cannot precompute in generate_pre.cpp,
	//then if user views the timetable, the timetable of a teacher contains activities of other teacher.
	//The bug appeared because it is possible to compute internal structure, so internal activities change the teacher, but the timetables remain the same,
	//with the same activities indexes.
	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	//The order is important - firstly the teachers, subjects, activity tags and students.
	//After that, the buildings.
	//After that, the rooms.
	//After that, the activities.
	//After that, the time constraints.
	//After that, the space constraints.

	if(this->teachersList.size()>MAX_TEACHERS){
		RulesImpossible::warning(parent, tr("FET information"),
		 tr("You have too many teachers. You need to increase the variable MAX_TEACHERS (which is currently %1).")
		 .arg(MAX_TEACHERS));
		return false;
	}
	
	//kill augmented students sets
	QList<StudentsYear*> ayears;
	QList<StudentsGroup*> agroups;
	QList<StudentsSubgroup*> asubgroups;
	for(StudentsYear* year : qAsConst(augmentedYearsList)){
		if(!ayears.contains(year))
			ayears.append(year);
		for(StudentsGroup* group : qAsConst(year->groupsList)){
			if(!agroups.contains(group))
				agroups.append(group);
			for(StudentsSubgroup* subgroup : qAsConst(group->subgroupsList)){
				if(!asubgroups.contains(subgroup))
					asubgroups.append(subgroup);
			}
		}
	}
	for(StudentsYear* year : qAsConst(ayears)){
		assert(year!=nullptr);
		delete year;
	}
	for(StudentsGroup* group : qAsConst(agroups)){
		assert(group!=nullptr);
		delete group;
	}
	for(StudentsSubgroup* subgroup : qAsConst(asubgroups)){
		assert(subgroup!=nullptr);
		delete subgroup;
	}
	augmentedYearsList.clear();
	//////////////////
	
	//copy list of students sets into augmented list
	QHash<QString, StudentsSet*> augmentedHash;
	
	for(StudentsYear* y : qAsConst(yearsList)){
		StudentsYear* ay=new StudentsYear();
		ay->name=y->name;
		ay->numberOfStudents=y->numberOfStudents;
		
		//2020-09-04 - this is not really important
		ay->divisions=y->divisions;
		ay->separator=y->separator;
		
		ay->groupsList.clear();
		augmentedYearsList << ay;
		
		assert(!augmentedHash.contains(ay->name));
		augmentedHash.insert(ay->name, ay);
		
		for(StudentsGroup* g : qAsConst(y->groupsList)){
			if(augmentedHash.contains(g->name)){
				StudentsSet* tmpg=augmentedHash.value(g->name);
				assert(tmpg->type==STUDENTS_GROUP);
				ay->groupsList<<((StudentsGroup*)tmpg);
			}
			else{
				StudentsGroup* ag=new StudentsGroup();
				ag->name=g->name;
				ag->numberOfStudents=g->numberOfStudents;
				ag->subgroupsList.clear();
				ay->groupsList << ag;
				
				assert(!augmentedHash.contains(ag->name));
				augmentedHash.insert(ag->name, ag);
			
				for(StudentsSubgroup* s : qAsConst(g->subgroupsList)){
					if(augmentedHash.contains(s->name)){
						StudentsSet* tmps=augmentedHash.value(s->name);
						assert(tmps->type==STUDENTS_SUBGROUP);
						ag->subgroupsList<<((StudentsSubgroup*)tmps);
					}
					else{
						StudentsSubgroup* as=new StudentsSubgroup();
						as->name=s->name;
						as->numberOfStudents=s->numberOfStudents;
						ag->subgroupsList << as;
						
						assert(!augmentedHash.contains(as->name));
						augmentedHash.insert(as->name, as);
					}
				}
			}
		}
	}

	/////////
	for(int i=0; i<this->augmentedYearsList.size(); i++){
		StudentsYear* sty=this->augmentedYearsList[i];

		//if this year has no groups, insert something to simulate the whole year
		if(sty->groupsList.count()==0){
			StudentsGroup* tmpGroup = new StudentsGroup();
			tmpGroup->name = sty->name+" "+tr("Automatic Group", "Please keep the translation short. It is used when a year contains no groups and an automatic group "
			 "is added in the year, in the timetable (when viewing the students timetable from FET and also in the html timetables for students groups or subgroups)"
			 ". In the empty year there will be added a group with name = yearName+a space character+your translation of 'Automatic Group'.");
			tmpGroup->numberOfStudents = sty->numberOfStudents;
			sty->groupsList << tmpGroup;
		}
		
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];

			//if this group has no subgroups, insert something to simulate the whole group
			if(stg->subgroupsList.size()==0){
				StudentsSubgroup* tmpSubgroup = new StudentsSubgroup();
				tmpSubgroup->name = stg->name+" "+tr("Automatic Subgroup", "Please keep the translation short. It is used when a group contains no subgroups and an automatic subgroup "
				 "is added in the group, in the timetable (when viewing the students timetable from FET and also in the html timetables for students subgroups)"
				 ". In the empty group there will be added a subgroup with name = groupName+a space character+your translation of 'Automatic Subgroup'.");
				tmpSubgroup->numberOfStudents=stg->numberOfStudents;
				stg->subgroupsList << tmpSubgroup;
			}
		}
	}
	//////////
	
	QSet<StudentsGroup*> allGroupsSet;
	QSet<StudentsSubgroup*> allSubgroupsSet;
	QList<StudentsGroup*> allGroupsList;
	QList<StudentsSubgroup*> allSubgroupsList;
	
	for(int i=0; i<this->augmentedYearsList.size(); i++){
		StudentsYear* sty=this->augmentedYearsList.at(i);
		sty->indexInAugmentedYearsList=i;

		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList.at(j);
			if(!allGroupsSet.contains(stg)){
				allGroupsSet.insert(stg);
				allGroupsList.append(stg);
				stg->indexInInternalGroupsList=allGroupsSet.count()-1;
			}
			
			for(int k=0; k<stg->subgroupsList.size(); k++)
				if(!allSubgroupsSet.contains(stg->subgroupsList.at(k))){
					allSubgroupsSet.insert(stg->subgroupsList.at(k));
					allSubgroupsList.append(stg->subgroupsList.at(k));
					stg->subgroupsList.at(k)->indexInInternalSubgroupsList=allSubgroupsSet.count()-1;
				}
		}
	}
	int tmpNSubgroups=allSubgroupsList.count();
	if(tmpNSubgroups>MAX_TOTAL_SUBGROUPS){
		RulesImpossible::warning(parent, tr("FET information"),
		 tr("You have too many total subgroups. You need to increase the variable MAX_TOTAL_SUBGROUPS (which is currently %1).")
		 .arg(MAX_TOTAL_SUBGROUPS));
		return false;
	}
	this->internalSubgroupsList.resize(tmpNSubgroups);

	int counter=0;
	for(int i=0; i<this->activitiesList.size(); i++){
		Activity* act=this->activitiesList.at(i);
		if(act->active)
			counter++;
	}
	if(counter>MAX_ACTIVITIES){
		RulesImpossible::warning(parent, tr("FET information"),
		 tr("You have too many active activities. You need to increase the variable MAX_ACTIVITIES (which is currently %1).")
		 .arg(MAX_ACTIVITIES));
		return false;
	}

	if(this->buildingsList.size()>MAX_BUILDINGS){
		RulesImpossible::warning(parent, tr("FET information"),
		 tr("You have too many buildings. You need to increase the variable MAX_BUILDINGS (which is currently %1).")
		 .arg(MAX_BUILDINGS));
		return false;
	}
	
	if(this->roomsList.size()>MAX_ROOMS){
		RulesImpossible::warning(parent, tr("FET information"),
		 tr("You have too many rooms. You need to increase the variable MAX_ROOMS (which is currently %1).")
		 .arg(MAX_ROOMS));
		return false;
	}
	
	assert(this->initialized);

	//days and hours
	assert(this->nHoursPerDay>0);
	assert(this->nDaysPerWeek>0);
	this->nHoursPerWeek=this->nHoursPerDay*this->nDaysPerWeek;

	//teachers
	int i;
	Teacher* tch;
	this->nInternalTeachers=this->teachersList.size();
	assert(this->nInternalTeachers<=MAX_TEACHERS);
	this->internalTeachersList.resize(this->nInternalTeachers);
	for(i=0; i<this->teachersList.size(); i++){
		tch=teachersList[i];
		this->internalTeachersList[i]=tch;
	}
	assert(i==this->nInternalTeachers);

	teachersHash.clear();
	for(int i=0; i<nInternalTeachers; i++)
		teachersHash.insert(internalTeachersList[i]->name, i);

	//subjects
	Subject* sbj;
	this->nInternalSubjects=this->subjectsList.size();
	this->internalSubjectsList.resize(this->nInternalSubjects);
	for(i=0; i<this->subjectsList.size(); i++){
		sbj=this->subjectsList[i];
		this->internalSubjectsList[i]=sbj;
	}
	assert(i==this->nInternalSubjects);

	subjectsHash.clear();
	for(int i=0; i<nInternalSubjects; i++)
		subjectsHash.insert(internalSubjectsList[i]->name, i);

	//activity tags
	ActivityTag* at;
	this->nInternalActivityTags=this->activityTagsList.size();
	this->internalActivityTagsList.resize(this->nInternalActivityTags);
	for(i=0; i<this->activityTagsList.size(); i++){
		at=this->activityTagsList[i];
		this->internalActivityTagsList[i]=at;
	}
	assert(i==this->nInternalActivityTags);

	activityTagsHash.clear();
	for(int i=0; i<nInternalActivityTags; i++)
		activityTagsHash.insert(internalActivityTagsList[i]->name, i);

	//students
	this->nInternalSubgroups=0;
	for(int i=0; i<allSubgroupsList.count(); i++){
		assert(allSubgroupsList.at(i)->indexInInternalSubgroupsList==i);
		this->internalSubgroupsList[this->nInternalSubgroups]=allSubgroupsList.at(i);
		this->nInternalSubgroups++;
	}

	this->internalGroupsList.clear();
	for(int i=0; i<allGroupsList.count(); i++){
		assert(allGroupsList.at(i)->indexInInternalGroupsList==i);
		this->internalGroupsList.append(allGroupsList.at(i));
	}
	
	studentsHash.clear();
	for(StudentsYear* year : qAsConst(augmentedYearsList)){
		studentsHash.insert(year->name, year);
		for(StudentsGroup* group : qAsConst(year->groupsList)){
			studentsHash.insert(group->name, group);
			for(StudentsSubgroup* subgroup : qAsConst(group->subgroupsList))
				studentsHash.insert(subgroup->name, subgroup);
		}
	}
	
	assert(this->nInternalSubgroups==tmpNSubgroups);

	//buildings
	internalBuildingsList.resize(buildingsList.size());
	this->nInternalBuildings=0;
	assert(this->buildingsList.size()<=MAX_BUILDINGS);
	for(int i=0; i<this->buildingsList.size(); i++){
		Building* bu=this->buildingsList[i];
		bu->computeInternalStructure(*this);
	}
	
	for(int i=0; i<this->buildingsList.size(); i++){
		Building* bu=this->buildingsList[i];
		this->internalBuildingsList[this->nInternalBuildings++]=bu;
	}
	assert(this->nInternalBuildings==this->buildingsList.size());

	buildingsHash.clear();
	for(int i=0; i<nInternalBuildings; i++)
		buildingsHash.insert(internalBuildingsList[i]->name, i);

	//rooms
	internalRoomsList.resize(roomsList.size());
	this->nInternalRooms=0;
	assert(this->roomsList.size()<=MAX_ROOMS);
	for(int i=0; i<this->roomsList.size(); i++){
		Room* rm=this->roomsList[i];
		rm->computeInternalStructure(*this);
	}
	
	for(int i=0; i<this->roomsList.size(); i++){
		Room* rm=this->roomsList[i];
		this->internalRoomsList[this->nInternalRooms++]=rm;
	}
	assert(this->nInternalRooms==this->roomsList.size());

	roomsHash.clear();
	for(int i=0; i<nInternalRooms; i++)
		roomsHash.insert(internalRoomsList[i]->name, i);
		
	for(int i=0; i<nInternalRooms; i++){
		Room* rm=internalRoomsList[i];
		
		if(rm->isVirtual){
			if(rm->realRoomsSetsList.count()==0){
				RulesImpossible::warning(parent, tr("FET information"), tr("Virtual room %1 has 0 sets of real rooms to choose"
				 " from. The number of sets of real rooms must be at least 2. Please correct this.").arg(rm->name));
				return false;
			}
			if(rm->realRoomsSetsList.count()==1){
				RulesImpossible::warning(parent, tr("FET information"), tr("Virtual room %1 has only 1 set of real rooms to choose"
				 " from. The number of sets of real rooms must be at least 2. Please correct this.").arg(rm->name));
				return false;
			}
			
			int i=0;
			for(const QStringList& tl : qAsConst(rm->realRoomsSetsList)){
				if(tl.count()==0){
					RulesImpossible::warning(parent, tr("FET information"), tr("Virtual room %1, in set number %2, has 0 real rooms"
					 " to choose from. The number of real rooms from each set must be at least 1. Please correct this.").arg(rm->name).arg(i+1));
					return false;
				}
				
				QStringList tl2;
				QSet<QString> ts;
				for(const QString& t : qAsConst(tl))
					if(!ts.contains(t))
						ts.insert(t);
					else
						tl2.append(t);
				if(!tl2.isEmpty()){
					RulesImpossible::warning(parent, tr("FET information"), tr("Virtual room %1, in set number %2, has these duplicate real rooms names: %3."
					 " Please correct this.").arg(rm->name).arg(i+1).arg(tl2.join(", ")));
					return false;
				}
				
				for(const QString& rr : qAsConst(tl)){
					int rri=roomsHash.value(rr, -1);

					if(rri==-1){
						RulesImpossible::warning(parent, tr("FET information"),
						 tr("The virtual room %1 contains the unrecognized real room %2 in the sets of real rooms - please correct this.").arg(rm->name).arg(rr));
						
						return false;
					}
					
					assert(rri>=0);
					if(internalRoomsList[rri]->isVirtual==true){
						RulesImpossible::warning(parent, tr("FET information"), tr("Virtual room %1, in set number %2, contains the virtual room"
						 " %3 - please correct this (a virtual room can only contain sets with real rooms).").arg(rm->name).arg(i+1).arg(rr));
						return false;
					}
				}
				
				i++;
			}
		}

		rm->computeInternalStructureRealRoomsSetsList(*this);
	}

	//activities
	int range=0;
	for(Activity* act : qAsConst(this->activitiesList))
		if(act->active)
			range++;
	QProgressDialog progress(parent);
	progress.setWindowTitle(tr("Computing internal structure", "Title of a progress dialog"));
	progress.setLabelText(tr("Processing internally the activities ... please wait"));
	progress.setRange(0, qMax(range, 1));
	progress.setModal(true);
	int ttt=0;
		
	Activity* act;
	counter=0;
	
	this->inactiveActivities.clear();
	
	for(int i=0; i<this->activitiesList.size(); i++){
		act=this->activitiesList[i];
		if(act->active){
			progress.setValue(ttt);
			//pqapplication->processEvents();
			if(progress.wasCanceled()){
				progress.setValue(range);
				RulesImpossible::warning(parent, tr("FET information"), tr("Canceled"));
				return false;
			}
			ttt++;

			counter++;
			act->computeInternalStructure(*this);
		}
		else
			inactiveActivities.insert(act->id);
	}
	
	progress.setValue(qMax(range, 1));

	for(int i=0; i<nInternalSubgroups; i++)
		internalSubgroupsList[i]->activitiesForSubgroup.clear();
	for(int i=0; i<nInternalTeachers; i++)
		internalTeachersList[i]->activitiesForTeacher.clear();

	assert(counter<=MAX_ACTIVITIES);
	this->nInternalActivities=counter;
	this->internalActivitiesList.resize(this->nInternalActivities);
	int activei=0;
	for(int ai=0; ai<this->activitiesList.size(); ai++){
		act=this->activitiesList[ai];
		if(act->active){
			this->internalActivitiesList[activei]=*act;
			
			for(int j=0; j<act->iSubgroupsList.count(); j++){
				int k=act->iSubgroupsList.at(j);
				//The test below takes time
				//assert(!internalSubgroupsList[k]->activitiesForSubgroup.contains(activei));
				internalSubgroupsList[k]->activitiesForSubgroup.append(activei);
			}
			
			for(int j=0; j<act->iTeachersList.count(); j++){
				int k=act->iTeachersList.at(j);
				//The test below takes time
				//assert(!internalTeachersList[k]->activitiesForTeacher.contains(activei));
				internalTeachersList[k]->activitiesForTeacher.append(activei);
			}
			
			activei++;
		}
	}

	activitiesHash.clear();
	for(int i=0; i<nInternalActivities; i++){
		assert(!activitiesHash.contains(internalActivitiesList[i].id));
		activitiesHash.insert(internalActivitiesList[i].id, i);
	}

	//activities list for each subject - used for subjects timetable - in order for students and teachers
	activitiesForSubjectList.resize(nInternalSubjects);
	activitiesForSubjectSet.resize(nInternalSubjects);
	for(int sb=0; sb<nInternalSubjects; sb++){
		activitiesForSubjectList[sb].clear();
		activitiesForSubjectSet[sb].clear();
	}

	for(int i=0; i<this->augmentedYearsList.size(); i++){
		StudentsYear* sty=this->augmentedYearsList[i];

		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];

			for(int k=0; k<stg->subgroupsList.size(); k++){
				StudentsSubgroup* sts=stg->subgroupsList[k];
				
				for(int ai : qAsConst(internalSubgroupsList[sts->indexInInternalSubgroupsList]->activitiesForSubgroup))
					if(!activitiesForSubjectSet[internalActivitiesList[ai].subjectIndex].contains(ai)){
						activitiesForSubjectList[internalActivitiesList[ai].subjectIndex].append(ai);
						activitiesForSubjectSet[internalActivitiesList[ai].subjectIndex].insert(ai);
					}
			}
		}
	}
	
	for(int i=0; i<nInternalTeachers; i++){
		for(int ai : qAsConst(internalTeachersList[i]->activitiesForTeacher))
			if(!activitiesForSubjectSet[internalActivitiesList[ai].subjectIndex].contains(ai)){
				activitiesForSubjectList[internalActivitiesList[ai].subjectIndex].append(ai);
				activitiesForSubjectSet[internalActivitiesList[ai].subjectIndex].insert(ai);
			}
	}
	
	//for activities without students or teachers
	for(int ai=0; ai<nInternalActivities; ai++){
		int si=internalActivitiesList[ai].subjectIndex;
		if(!activitiesForSubjectSet[si].contains(ai)){
			activitiesForSubjectList[si].append(ai);
			activitiesForSubjectSet[si].insert(ai);
		}
	}
	/////////////////////////////////////////////////////////////////
	
	//activities list for each activity tag - used for activity tags timetable - in order for students and teachers
	activitiesForActivityTagList.resize(nInternalActivityTags);
	activitiesForActivityTagSet.resize(nInternalActivityTags);
	for(int a=0; a<nInternalActivityTags; a++){
		activitiesForActivityTagList[a].clear();
		activitiesForActivityTagSet[a].clear();
	}

	for(int i=0; i<this->augmentedYearsList.size(); i++){
		StudentsYear* sty=this->augmentedYearsList[i];

		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];

			for(int k=0; k<stg->subgroupsList.size(); k++){
				StudentsSubgroup* sts=stg->subgroupsList[k];
				
				for(int ai : qAsConst(internalSubgroupsList[sts->indexInInternalSubgroupsList]->activitiesForSubgroup))
					for(int activityTagInt : qAsConst(internalActivitiesList[ai].iActivityTagsSet))
						if(!activitiesForActivityTagSet[activityTagInt].contains(ai)){
							activitiesForActivityTagList[activityTagInt].append(ai);
							activitiesForActivityTagSet[activityTagInt].insert(ai);
						}
			}
		}
	}
	
	for(int i=0; i<nInternalTeachers; i++){
		for(int ai : qAsConst(internalTeachersList[i]->activitiesForTeacher))
			for(int activityTagInt : qAsConst(internalActivitiesList[ai].iActivityTagsSet))
				if(!activitiesForActivityTagSet[activityTagInt].contains(ai)){
					activitiesForActivityTagList[activityTagInt].append(ai);
					activitiesForActivityTagSet[activityTagInt].insert(ai);
				}
	}

	//for activities without students or teachers
	for(int ai=0; ai<nInternalActivities; ai++){
		for(int ati : qAsConst(internalActivitiesList[ai].iActivityTagsSet)){
			if(!activitiesForActivityTagSet[ati].contains(ai)){
				activitiesForActivityTagList[ati].append(ai);
				activitiesForActivityTagSet[ati].insert(ai);
			}
		}
	}
	/////////////////////////////////////////////////////////////////

	bool ok=true;

	//time constraints
	//progress.reset();
	
	bool skipInactiveTimeConstraints=false;
	
	TimeConstraint* tctr;
	
	QSet<int> toSkipTimeSet;
	
	int _c=0;
	
	for(int tctrindex=0; tctrindex<this->timeConstraintsList.size(); tctrindex++){
		tctr=this->timeConstraintsList[tctrindex];

		if(!tctr->active){
			toSkipTimeSet.insert(tctrindex);
		}
		else if(tctr->hasInactiveActivities(*this)){
			//toSkipTime[tctrindex]=true;
			toSkipTimeSet.insert(tctrindex);
		
			if(!skipInactiveTimeConstraints){
				QString s=tr("The following time constraint is ignored, because it refers to inactive activities:");
				s+="\n";
				s+=tctr->getDetailedDescription(*this);
				
				int t=RulesConstraintIgnored::mediumConfirmation(parent, tr("FET information"), s,
				 tr("Skip rest"), tr("See next"), QString(),
 				 1, 0 );

				if(t==0)
					skipInactiveTimeConstraints=true;
			}
		}
		else{
			//toSkipTime[tctrindex]=false;
			_c++;
		}
	}
	
	internalTimeConstraintsList.resize(_c);
	
	progress.setLabelText(tr("Processing internally the time constraints ... please wait"));
	progress.setRange(0, qMax(timeConstraintsList.size(), 1));
	ttt=0;
		
	//assert(this->timeConstraintsList.size()<=MAX_TIME_CONSTRAINTS);
	int tctri=0;
	
	for(int tctrindex=0; tctrindex<this->timeConstraintsList.size(); tctrindex++){
		progress.setValue(ttt);
		//pqapplication->processEvents();
		if(progress.wasCanceled()){
			progress.setValue(timeConstraintsList.size());
			RulesImpossible::warning(parent, tr("FET information"), tr("Canceled"));
			return false;
		}
		ttt++;

		tctr=this->timeConstraintsList[tctrindex];
		
		if(toSkipTimeSet.contains(tctrindex))
			continue;
		
		if(!tctr->computeInternalStructure(parent, *this)){
			//assert(0);
			ok=false;
			continue;
		}
		this->internalTimeConstraintsList[tctri++]=tctr;
	}

	progress.setValue(qMax(timeConstraintsList.size(), 1));

	this->nInternalTimeConstraints=tctri;
	if(VERBOSE){
		cout<<_c<<" time constraints after first pass (after removing inactive ones)"<<endl;
		cout<<"  "<<this->nInternalTimeConstraints<<" time constraints after second pass (after removing wrong ones)"<<endl;
	}
	assert(_c>=this->nInternalTimeConstraints); //because some constraints may have toSkipTime false, but computeInternalStructure also false
	//assert(this->nInternalTimeConstraints<=MAX_TIME_CONSTRAINTS);
	
	//space constraints
	//progress.reset();
	
	bool skipInactiveSpaceConstraints=false;
	
	SpaceConstraint* sctr;
	
	QSet<int> toSkipSpaceSet;
	
	_c=0;

	for(int sctrindex=0; sctrindex<this->spaceConstraintsList.size(); sctrindex++){
		sctr=this->spaceConstraintsList[sctrindex];

		if(!sctr->active){
			toSkipSpaceSet.insert(sctrindex);
		}
		else if(sctr->hasInactiveActivities(*this)){
			//toSkipSpace[sctrindex]=true;
			toSkipSpaceSet.insert(sctrindex);
		
			if(!skipInactiveSpaceConstraints){
				QString s=tr("The following space constraint is ignored, because it refers to inactive activities:");
				s+="\n";
				s+=sctr->getDetailedDescription(*this);
				
				int t=RulesConstraintIgnored::mediumConfirmation(parent, tr("FET information"), s,
				 tr("Skip rest"), tr("See next"), QString(),
 				 1, 0 );

				if(t==0)
					skipInactiveSpaceConstraints=true;
			}
		}
		else{
			_c++;
			//toSkipSpace[sctrindex]=false;
		}
	}
	
	internalSpaceConstraintsList.resize(_c);
	
	progress.setLabelText(tr("Processing internally the space constraints ... please wait"));
	progress.setRange(0, qMax(spaceConstraintsList.size(), 1));
	ttt=0;
	//assert(this->spaceConstraintsList.size()<=MAX_SPACE_CONSTRAINTS);

	int sctri=0;

	for(int sctrindex=0; sctrindex<this->spaceConstraintsList.size(); sctrindex++){
		progress.setValue(ttt);
		//pqapplication->processEvents();
		if(progress.wasCanceled()){
			progress.setValue(spaceConstraintsList.size());
			RulesImpossible::warning(parent, tr("FET information"), tr("Canceled"));
			return false;
		}
		ttt++;

		sctr=this->spaceConstraintsList[sctrindex];
	
		if(toSkipSpaceSet.contains(sctrindex))
			continue;
		
		if(!sctr->computeInternalStructure(parent, *this)){
			//assert(0);
			ok=false;
			continue;
		}
		this->internalSpaceConstraintsList[sctri++]=sctr;
	}

	progress.setValue(qMax(spaceConstraintsList.size(), 1));

	this->nInternalSpaceConstraints=sctri;
	if(VERBOSE){
		cout<<_c<<" space constraints after first pass (after removing inactive ones)"<<endl;
		cout<<"  "<<this->nInternalSpaceConstraints<<" space constraints after second pass (after removing wrong ones)"<<endl;
	}
	assert(_c>=this->nInternalSpaceConstraints); //because some constraints may have toSkipSpace false, but computeInternalStructure also false
	//assert(this->nInternalSpaceConstraints<=MAX_SPACE_CONSTRAINTS);
	
	//group activities in initial order
	if(groupActivitiesInInitialOrderList.count()>0){
		QStringList fetBugs;
		QStringList userErrors;
	
		QSet<int> visitedIds;
		for(int j=0; j<groupActivitiesInInitialOrderList.count(); j++){
			GroupActivitiesInInitialOrderItem* item=groupActivitiesInInitialOrderList[j];
			
			if(!item->active)
				continue;
			
			if(item->ids.count()<2){
				fetBugs.append(tr("All 'group activities in the initial order for timetable generation' items should contain at least two activities ids."
				 " This is not true for item number %1. Please report potential bug.").arg(j+1));
			}

			item->indices.clear();
			for(int id : qAsConst(item->ids)){
				if(visitedIds.contains(id)){
					userErrors.append(tr("All 'group activities in the initial order for timetable generation' items should have different activities ids."
					 " (Each activity id must appear at most once in all the items.) This is not true for item number %1 and activity id %2.").arg(j+1).arg(id));
				}
				else{
					visitedIds.insert(id);
					int index=activitiesHash.value(id, -1);
					if(index>=0)
						item->indices.append(index);
				}
			}

			if(!fetBugs.isEmpty() || !userErrors.isEmpty()){
				RulesImpossible::warning(parent, tr("FET information"), fetBugs.join("\n\n")+userErrors.join("\n\n"));
				return false;
			}
		}
	}

	//done.
	this->internalStructureComputed=ok;
	
	return ok;
}

void Rules::kill() //clears memory for the rules, destroys them
{
	//Teachers
	for(Teacher* tch : qAsConst(teachersList))
		delete tch;
	teachersList.clear();
	//while(!teachersList.isEmpty())
	//	delete teachersList.takeFirst();

	//Subjects
	for(Subject* sbj : qAsConst(subjectsList))
		delete sbj;
	subjectsList.clear();
	//while(!subjectsList.isEmpty())
	//	delete subjectsList.takeFirst();

	//Activity tags
	for(ActivityTag* at : qAsConst(activityTagsList))
		delete at;
	activityTagsList.clear();
	//while(!activityTagsList.isEmpty())
	//	delete activityTagsList.takeFirst();

	//Years
	/*while(!yearsList.isEmpty())
		delete yearsList.takeFirst();*/
		
	//students sets
	QSet<StudentsYear*> iyears;
	QSet<StudentsGroup*> igroups;
	QSet<StudentsSubgroup*> isubgroups;
	for(StudentsYear* year : qAsConst(yearsList)){
		if(!iyears.contains(year))
			iyears.insert(year);
		for(StudentsGroup* group : qAsConst(year->groupsList)){
			if(!igroups.contains(group))
				igroups.insert(group);
			for(StudentsSubgroup* subgroup : qAsConst(group->subgroupsList)){
				if(!isubgroups.contains(subgroup))
					isubgroups.insert(subgroup);
			}
		}
	}
	for(StudentsYear* year : qAsConst(iyears)){
		assert(year!=nullptr);
		delete year;
	}
	for(StudentsGroup* group : qAsConst(igroups)){
		assert(group!=nullptr);
		delete group;
	}
	for(StudentsSubgroup* subgroup : qAsConst(isubgroups)){
		assert(subgroup!=nullptr);
		delete subgroup;
	}
	yearsList.clear();
	
	permanentStudentsHash.clear();
	//////////////////

	//kill augmented students sets
	QList<StudentsYear*> ayears;
	QList<StudentsGroup*> agroups;
	QList<StudentsSubgroup*> asubgroups;
	for(StudentsYear* year : qAsConst(augmentedYearsList)){
		if(!ayears.contains(year))
			ayears.append(year);
		for(StudentsGroup* group : qAsConst(year->groupsList)){
			if(!agroups.contains(group))
				agroups.append(group);
			for(StudentsSubgroup* subgroup : qAsConst(group->subgroupsList)){
				if(!asubgroups.contains(subgroup))
					asubgroups.append(subgroup);
			}
		}
	}
	for(StudentsYear* year : qAsConst(ayears)){
		assert(year!=nullptr);
		delete year;
	}
	for(StudentsGroup* group : qAsConst(agroups)){
		assert(group!=nullptr);
		delete group;
	}
	for(StudentsSubgroup* subgroup : qAsConst(asubgroups)){
		assert(subgroup!=nullptr);
		delete subgroup;
	}
	augmentedYearsList.clear();
	//////////////////
	
	//Activities
	for(Activity* act : qAsConst(activitiesList))
		delete act;
	activitiesList.clear();
	//while(!activitiesList.isEmpty())
	//	delete activitiesList.takeFirst();

	//Time constraints
	for(TimeConstraint* tc : qAsConst(timeConstraintsList))
		delete tc;
	timeConstraintsList.clear();
	//while(!timeConstraintsList.isEmpty())
	//	delete timeConstraintsList.takeFirst();

	//Space constraints
	for(SpaceConstraint* sc : qAsConst(spaceConstraintsList))
		delete sc;
	spaceConstraintsList.clear();
	//while(!spaceConstraintsList.isEmpty())
	//	delete spaceConstraintsList.takeFirst();

	//Buildings
	for(Building* bu : qAsConst(buildingsList))
		delete bu;
	buildingsList.clear();
	//while(!buildingsList.isEmpty())
	//	delete buildingsList.takeFirst();

	//Rooms
	for(Room* rm : qAsConst(roomsList))
		delete rm;
	roomsList.clear();
	//while(!roomsList.isEmpty())
	//	delete roomsList.takeFirst();
		
	for(GroupActivitiesInInitialOrderItem* ga : qAsConst(groupActivitiesInInitialOrderList))
		delete ga;
	groupActivitiesInInitialOrderList.clear();
	//while(!groupActivitiesInInitialOrderList.isEmpty())
	//	delete groupActivitiesInInitialOrderList.takeFirst();

	activitiesPointerHash.clear();
	bctSet.clear();
	btSet.clear();
	bcsSet.clear();
	apstHash.clear();
	aprHash.clear();
	mdbaHash.clear();
	tnatHash.clear();
	ssnatHash.clear();
	
	teachersHash.clear();
	subjectsHash.clear();
	activityTagsHash.clear();
	studentsHash.clear();
	buildingsHash.clear();
	roomsHash.clear();
	activitiesHash.clear();

	//done
	this->internalStructureComputed=false;
	this->initialized=false;

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;
}

Rules::Rules()
{
	this->initialized=false;
	this->modified=false;
}

Rules::~Rules()
{
	if(this->initialized)
		this->kill();
}

void Rules::setInstitutionName(const QString& newInstitutionName)
{
	this->institutionName=newInstitutionName;
	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);
}

void Rules::setComments(const QString& newComments)
{
	this->comments=newComments;
	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);
}

bool Rules::addTeacher(Teacher* teacher)
{
	for(int i=0; i<this->teachersList.size(); i++){
		Teacher* tch=this->teachersList[i];
		if(tch->name==teacher->name)
			return false;
	}
	
	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	this->teachersList.append(teacher);
	return true;
}

bool Rules::addTeacherFast(Teacher* teacher)
{
	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	this->teachersList.append(teacher);
	return true;
}

int Rules::searchTeacher(const QString& teacherName)
{
	for(int i=0; i<this->teachersList.size(); i++)
		if(this->teachersList.at(i)->name==teacherName)
			return i;

	return -1;
}

bool Rules::removeTeacher(const QString& teacherName)
{
	QList<int> idsToBeRemoved;
	for(Activity* act : qAsConst(activitiesList)){
		bool t=act->removeTeacher(teacherName);
		if(t && act->teachersNames.count()==0)
			idsToBeRemoved.append(act->id);
	}
	removeActivities(idsToBeRemoved, false);

	for(int i=0; i<this->teachersList.size(); i++)
		if(this->teachersList.at(i)->name==teacherName){
			Teacher* tch=this->teachersList[i];
			this->teachersList.removeAt(i);
			delete tch;
			break;
		}
	
	updateConstraintsAfterRemoval();

	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	return true;
}

bool Rules::modifyTeacher(const QString& initialTeacherName, const QString& finalTeacherName)
{
	assert(this->searchTeacher(finalTeacherName)==-1);
	assert(this->searchTeacher(initialTeacherName)>=0);

	for(int i=0; i<this->activitiesList.size(); i++)
		this->activitiesList.at(i)->renameTeacher(initialTeacherName, finalTeacherName);
		
	for(TimeConstraint* ctr : qAsConst(timeConstraintsList)){
		if(ctr->type==CONSTRAINT_TEACHER_NOT_AVAILABLE_TIMES){
			ConstraintTeacherNotAvailableTimes* crt_constraint=(ConstraintTeacherNotAvailableTimes*)ctr;
			if(initialTeacherName == crt_constraint->teacher)
				crt_constraint->teacher=finalTeacherName;
		}
		else if(ctr->type==CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK){
			ConstraintTeacherMaxGapsPerWeek* crt_constraint=(ConstraintTeacherMaxGapsPerWeek*)ctr;
			if(initialTeacherName == crt_constraint->teacherName)
				crt_constraint->teacherName=finalTeacherName;
		}
		else if(ctr->type==CONSTRAINT_TEACHER_MAX_GAPS_PER_DAY){
			ConstraintTeacherMaxGapsPerDay* crt_constraint=(ConstraintTeacherMaxGapsPerDay*)ctr;
			if(initialTeacherName == crt_constraint->teacherName)
				crt_constraint->teacherName=finalTeacherName;
		}
		else if(ctr->type==CONSTRAINT_TEACHER_MAX_HOURS_DAILY){
			ConstraintTeacherMaxHoursDaily* crt_constraint=(ConstraintTeacherMaxHoursDaily*)ctr;
			if(initialTeacherName == crt_constraint->teacherName)
				crt_constraint->teacherName=finalTeacherName;
		}
		else if(ctr->type==CONSTRAINT_TEACHER_MAX_HOURS_CONTINUOUSLY){
			ConstraintTeacherMaxHoursContinuously* crt_constraint=(ConstraintTeacherMaxHoursContinuously*)ctr;
			if(initialTeacherName == crt_constraint->teacherName)
				crt_constraint->teacherName=finalTeacherName;
		}
		else if(ctr->type==CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
			ConstraintTeacherActivityTagMaxHoursContinuously* crt_constraint=(ConstraintTeacherActivityTagMaxHoursContinuously*)ctr;
			if(initialTeacherName == crt_constraint->teacherName)
				crt_constraint->teacherName=finalTeacherName;
		}
		else if(ctr->type==CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_DAILY){
			ConstraintTeacherActivityTagMaxHoursDaily* crt_constraint=(ConstraintTeacherActivityTagMaxHoursDaily*)ctr;
			if(initialTeacherName == crt_constraint->teacherName)
				crt_constraint->teacherName=finalTeacherName;
		}
		else if(ctr->type==CONSTRAINT_TEACHER_ACTIVITY_TAG_MIN_HOURS_DAILY){
			ConstraintTeacherActivityTagMinHoursDaily* crt_constraint=(ConstraintTeacherActivityTagMinHoursDaily*)ctr;
			if(initialTeacherName == crt_constraint->teacherName)
				crt_constraint->teacherName=finalTeacherName;
		}
		else if(ctr->type==CONSTRAINT_TEACHER_MIN_HOURS_DAILY){
			ConstraintTeacherMinHoursDaily* crt_constraint=(ConstraintTeacherMinHoursDaily*)ctr;
			if(initialTeacherName == crt_constraint->teacherName)
				crt_constraint->teacherName=finalTeacherName;
		}
		else if(ctr->type==CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS){
			ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTags* crt_constraint=(ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTags*)ctr;
			if(initialTeacherName == crt_constraint->teacher)
				crt_constraint->teacher=finalTeacherName;
		}
		else if(ctr->type==CONSTRAINT_TEACHER_MAX_DAYS_PER_WEEK){
			ConstraintTeacherMaxDaysPerWeek* crt_constraint=(ConstraintTeacherMaxDaysPerWeek*)ctr;
			if(initialTeacherName == crt_constraint->teacherName)
				crt_constraint->teacherName=finalTeacherName;
		}
		else if(ctr->type==CONSTRAINT_TEACHER_MIN_DAYS_PER_WEEK){
			ConstraintTeacherMinDaysPerWeek* crt_constraint=(ConstraintTeacherMinDaysPerWeek*)ctr;
			if(initialTeacherName == crt_constraint->teacherName)
				crt_constraint->teacherName=finalTeacherName;
		}
		else if(ctr->type==CONSTRAINT_TEACHER_INTERVAL_MAX_DAYS_PER_WEEK){
			ConstraintTeacherIntervalMaxDaysPerWeek* crt_constraint=(ConstraintTeacherIntervalMaxDaysPerWeek*)ctr;
			if(initialTeacherName == crt_constraint->teacherName)
				crt_constraint->teacherName=finalTeacherName;
		}
		else if(ctr->type==CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS){
			ConstraintActivitiesPreferredTimeSlots* crt_constraint=(ConstraintActivitiesPreferredTimeSlots*)ctr;
			if(initialTeacherName == crt_constraint->p_teacherName)
				crt_constraint->p_teacherName=finalTeacherName;
		}
		else if(ctr->type==CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES){
			ConstraintActivitiesPreferredStartingTimes* crt_constraint=(ConstraintActivitiesPreferredStartingTimes*)ctr;
			if(initialTeacherName == crt_constraint->teacherName)
				crt_constraint->teacherName=finalTeacherName;
		}
		else if(ctr->type==CONSTRAINT_ACTIVITIES_END_STUDENTS_DAY){
			ConstraintActivitiesEndStudentsDay* crt_constraint=(ConstraintActivitiesEndStudentsDay*)ctr;
			if(initialTeacherName == crt_constraint->teacherName)
				crt_constraint->teacherName=finalTeacherName;
		}
		else if(ctr->type==CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS){
			ConstraintSubactivitiesPreferredTimeSlots* crt_constraint=(ConstraintSubactivitiesPreferredTimeSlots*)ctr;
			if(initialTeacherName == crt_constraint->p_teacherName)
				crt_constraint->p_teacherName=finalTeacherName;
		}
		else if(ctr->type==CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES){
			ConstraintSubactivitiesPreferredStartingTimes* crt_constraint=(ConstraintSubactivitiesPreferredStartingTimes*)ctr;
			if(initialTeacherName == crt_constraint->teacherName)
				crt_constraint->teacherName=finalTeacherName;
		}
		//2017-02-07
		else if(ctr->type==CONSTRAINT_TEACHER_MAX_SPAN_PER_DAY){
			ConstraintTeacherMaxSpanPerDay* crt_constraint=(ConstraintTeacherMaxSpanPerDay*)ctr;
			if(initialTeacherName == crt_constraint->teacherName)
				crt_constraint->teacherName=finalTeacherName;
		}
		else if(ctr->type==CONSTRAINT_TEACHER_MIN_RESTING_HOURS){
			ConstraintTeacherMinRestingHours* crt_constraint=(ConstraintTeacherMinRestingHours*)ctr;
			if(initialTeacherName == crt_constraint->teacherName)
				crt_constraint->teacherName=finalTeacherName;
		}
	}
	
	for(SpaceConstraint* ctr : qAsConst(spaceConstraintsList)){
		if(ctr->type==CONSTRAINT_TEACHER_HOME_ROOM){
			ConstraintTeacherHomeRoom* crt_constraint=(ConstraintTeacherHomeRoom*)ctr;
			if(initialTeacherName == crt_constraint->teacherName)
				crt_constraint->teacherName=finalTeacherName;
		}
		else if(ctr->type==CONSTRAINT_TEACHER_HOME_ROOMS){
			ConstraintTeacherHomeRooms* crt_constraint=(ConstraintTeacherHomeRooms*)ctr;
			if(initialTeacherName == crt_constraint->teacherName)
				crt_constraint->teacherName=finalTeacherName;
		}
		else if(ctr->type==CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY){
			ConstraintTeacherMaxBuildingChangesPerDay* crt_constraint=(ConstraintTeacherMaxBuildingChangesPerDay*)ctr;
			if(initialTeacherName == crt_constraint->teacherName)
				crt_constraint->teacherName=finalTeacherName;
		}
		else if(ctr->type==CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_WEEK){
			ConstraintTeacherMaxBuildingChangesPerWeek* crt_constraint=(ConstraintTeacherMaxBuildingChangesPerWeek*)ctr;
			if(initialTeacherName == crt_constraint->teacherName)
				crt_constraint->teacherName=finalTeacherName;
		}
		else if(ctr->type==CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_BUILDING_CHANGES){
			ConstraintTeacherMinGapsBetweenBuildingChanges* crt_constraint=(ConstraintTeacherMinGapsBetweenBuildingChanges*)ctr;
			if(initialTeacherName == crt_constraint->teacherName)
				crt_constraint->teacherName=finalTeacherName;
		}
		else if(ctr->type==CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY){
			ConstraintTeacherMaxRoomChangesPerDay* crt_constraint=(ConstraintTeacherMaxRoomChangesPerDay*)ctr;
			if(initialTeacherName == crt_constraint->teacherName)
				crt_constraint->teacherName=finalTeacherName;
		}
		else if(ctr->type==CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_WEEK){
			ConstraintTeacherMaxRoomChangesPerWeek* crt_constraint=(ConstraintTeacherMaxRoomChangesPerWeek*)ctr;
			if(initialTeacherName == crt_constraint->teacherName)
				crt_constraint->teacherName=finalTeacherName;
		}
		else if(ctr->type==CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ROOM_CHANGES){
			ConstraintTeacherMinGapsBetweenRoomChanges* crt_constraint=(ConstraintTeacherMinGapsBetweenRoomChanges*)ctr;
			if(initialTeacherName == crt_constraint->teacherName)
				crt_constraint->teacherName=finalTeacherName;
		}
	}
	
	int t=0;
	for(int i=0; i<this->teachersList.size(); i++){
		Teacher* tch=this->teachersList[i];

		if(tch->name==initialTeacherName){
			tch->name=finalTeacherName;
			t++;
		}
	}
	assert(t==1);
	
	if(tnatHash.contains(initialTeacherName)){
		QSet<ConstraintTeacherNotAvailableTimes*> cs=tnatHash.value(initialTeacherName);
		tnatHash.remove(initialTeacherName);
		assert(!tnatHash.contains(finalTeacherName));
		tnatHash.insert(finalTeacherName, cs);
	}

	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	return true;
}

void Rules::sortTeachersAlphabetically()
{
	std::stable_sort(this->teachersList.begin(), this->teachersList.end(), teachersAscending);

	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;
}

bool Rules::addSubject(Subject* subject)
{
	for(int i=0; i<this->subjectsList.size(); i++){
		Subject* sbj=this->subjectsList[i];	
		if(sbj->name==subject->name)
			return false;
	}
	
	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	this->subjectsList << subject;
	return true;
}

bool Rules::addSubjectFast(Subject* subject)
{
	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	this->subjectsList << subject;
	return true;
}

int Rules::searchSubject(const QString& subjectName)
{
	for(int i=0; i<this->subjectsList.size(); i++)
		if(this->subjectsList.at(i)->name == subjectName)
			return i;

	return -1;
}

bool Rules::removeSubject(const QString& subjectName)
{
	//check the qualified subjects for teachers
	for(Teacher* tch : qAsConst(teachersList)){
		if(tch->qualifiedSubjectsHash.contains(subjectName)){
			std::list<QString>::iterator it=tch->qualifiedSubjectsHash.value(subjectName);
			assert((*it)==subjectName);
			
			tch->qualifiedSubjectsList.erase(it);
			
			tch->qualifiedSubjectsHash.remove(subjectName);
		}
	}

	QList<int> idsToBeRemoved;
	for(Activity* act : qAsConst(activitiesList)){
		if(act->subjectName==subjectName)
			idsToBeRemoved.append(act->id);
	}
	removeActivities(idsToBeRemoved, false);

	//remove the subject from the list
	for(int i=0; i<this->subjectsList.size(); i++)
		if(this->subjectsList[i]->name==subjectName){
			Subject* sbj=this->subjectsList[i];
			this->subjectsList.removeAt(i);
			delete sbj;
			break;
		}
	
	updateConstraintsAfterRemoval();

	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	return true;
}

bool Rules::modifySubject(const QString& initialSubjectName, const QString& finalSubjectName)
{
	assert(this->searchSubject(finalSubjectName)==-1);
	assert(this->searchSubject(initialSubjectName)>=0);

	//check the qualified subjects for teachers
	for(Teacher* tch : qAsConst(teachersList)){
		if(tch->qualifiedSubjectsHash.contains(initialSubjectName)){
			std::list<QString>::iterator it=tch->qualifiedSubjectsHash.value(initialSubjectName);
			assert((*it)==initialSubjectName);
			
			(*it)=finalSubjectName;
			
			tch->qualifiedSubjectsHash.remove(initialSubjectName);
			tch->qualifiedSubjectsHash.insert(finalSubjectName, it);
		}
	}

	//check the activities
	for(int i=0; i<this->activitiesList.size(); i++){
		Activity* act=this->activitiesList[i];

		if( act->subjectName == initialSubjectName)
			act->subjectName=finalSubjectName;
	}
	
	//modify the time constraints related to this subject
	for(TimeConstraint* ctr : qAsConst(timeConstraintsList)){
		if(ctr->type==CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS){
			ConstraintActivitiesPreferredTimeSlots* crt_constraint=(ConstraintActivitiesPreferredTimeSlots*)ctr;
			if(initialSubjectName == crt_constraint->p_subjectName)
				crt_constraint->p_subjectName=finalSubjectName;
		}
		else if(ctr->type==CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES){
			ConstraintActivitiesPreferredStartingTimes* crt_constraint=(ConstraintActivitiesPreferredStartingTimes*)ctr;
			if(initialSubjectName == crt_constraint->subjectName)
				crt_constraint->subjectName=finalSubjectName;
		}
		else if(ctr->type==CONSTRAINT_ACTIVITIES_END_STUDENTS_DAY){
			ConstraintActivitiesEndStudentsDay* crt_constraint=(ConstraintActivitiesEndStudentsDay*)ctr;
			if(initialSubjectName == crt_constraint->subjectName)
				crt_constraint->subjectName=finalSubjectName;
		}
		else if(ctr->type==CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS){
			ConstraintSubactivitiesPreferredTimeSlots* crt_constraint=(ConstraintSubactivitiesPreferredTimeSlots*)ctr;
			if(initialSubjectName == crt_constraint->p_subjectName)
				crt_constraint->p_subjectName=finalSubjectName;
		}
		else if(ctr->type==CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES){
			ConstraintSubactivitiesPreferredStartingTimes* crt_constraint=(ConstraintSubactivitiesPreferredStartingTimes*)ctr;
			if(initialSubjectName == crt_constraint->subjectName)
				crt_constraint->subjectName=finalSubjectName;
		}
	}
	
	//modify the space constraints related to this subject
	for(SpaceConstraint* ctr : qAsConst(spaceConstraintsList)){
		if(ctr->type==CONSTRAINT_SUBJECT_PREFERRED_ROOM){
			ConstraintSubjectPreferredRoom* c=(ConstraintSubjectPreferredRoom*)ctr;
			if(c->subjectName == initialSubjectName)
				c->subjectName=finalSubjectName;
		}
		else if(ctr->type==CONSTRAINT_SUBJECT_PREFERRED_ROOMS){
			ConstraintSubjectPreferredRooms* c=(ConstraintSubjectPreferredRooms*)ctr;
			if(c->subjectName == initialSubjectName)
				c->subjectName=finalSubjectName;
		}
		else if(ctr->type==CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM){
			ConstraintSubjectActivityTagPreferredRoom* c=(ConstraintSubjectActivityTagPreferredRoom*)ctr;
			if(c->subjectName == initialSubjectName)
				c->subjectName=finalSubjectName;
		}
		else if(ctr->type==CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS){
			ConstraintSubjectActivityTagPreferredRooms* c=(ConstraintSubjectActivityTagPreferredRooms*)ctr;
			if(c->subjectName == initialSubjectName)
				c->subjectName=finalSubjectName;
		}
	}

	//rename the subject in the list
	int t=0;
	for(int i=0; i<this->subjectsList.size(); i++){
		Subject* sbj=this->subjectsList[i];

		if(sbj->name==initialSubjectName){
			t++;
			sbj->name=finalSubjectName;
		}
	}
	assert(t==1);

	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	return true;
}

void Rules::sortSubjectsAlphabetically()
{
	std::stable_sort(this->subjectsList.begin(), this->subjectsList.end(), subjectsAscending);

	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;
}

bool Rules::addActivityTag(ActivityTag* activityTag)
{
	for(int i=0; i<this->activityTagsList.size(); i++){
		ActivityTag* sbt=this->activityTagsList[i];

		if(sbt->name==activityTag->name)
			return false;
	}

	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	this->activityTagsList << activityTag;
	return true;
}

bool Rules::addActivityTagFast(ActivityTag* activityTag)
{
	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	this->activityTagsList << activityTag;
	return true;
}

int Rules::searchActivityTag(const QString& activityTagName)
{
	for(int i=0; i<this->activityTagsList.size(); i++)
		if(this->activityTagsList.at(i)->name==activityTagName)
			return i;

	return -1;
}

bool Rules::removeActivityTag(const QString& activityTagName)
{
	for(Activity* act : qAsConst(activitiesList))
		if(act->activityTagsNames.contains(activityTagName))
			act->activityTagsNames.removeAll(activityTagName);

	//remove the activity tag from the list
	for(int i=0; i<this->activityTagsList.size(); i++)
		if(this->activityTagsList[i]->name==activityTagName){
			ActivityTag* sbt=this->activityTagsList[i];
			this->activityTagsList.removeAt(i);
			delete sbt;
			break;
		}
	
	updateConstraintsAfterRemoval();

	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	return true;
}

bool Rules::modifyActivityTag(const QString& initialActivityTagName, const QString& finalActivityTagName)
{
	assert(this->searchActivityTag(finalActivityTagName)==-1);
	assert(this->searchActivityTag(initialActivityTagName)>=0);

	//check the activities first
	for(int i=0; i<this->activitiesList.size(); i++){
		Activity* act=this->activitiesList[i];

		for(int kk=0; kk<act->activityTagsNames.count(); kk++)
			if(act->activityTagsNames.at(kk)==initialActivityTagName)
				act->activityTagsNames[kk]=finalActivityTagName;
	}
	
	//modify the time constraints related to this activity tag
	for(TimeConstraint* ctr : qAsConst(timeConstraintsList)){
		if(ctr->type==CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
			ConstraintTeacherActivityTagMaxHoursContinuously* crt_constraint=(ConstraintTeacherActivityTagMaxHoursContinuously*)ctr;
			if(initialActivityTagName == crt_constraint->activityTagName)
				crt_constraint->activityTagName=finalActivityTagName;
		}
		else if(ctr->type==CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_DAILY){
			ConstraintTeacherActivityTagMaxHoursDaily* crt_constraint=(ConstraintTeacherActivityTagMaxHoursDaily*)ctr;
			if(initialActivityTagName == crt_constraint->activityTagName)
				crt_constraint->activityTagName=finalActivityTagName;
		}
		else if(ctr->type==CONSTRAINT_TEACHER_ACTIVITY_TAG_MIN_HOURS_DAILY){
			ConstraintTeacherActivityTagMinHoursDaily* crt_constraint=(ConstraintTeacherActivityTagMinHoursDaily*)ctr;
			if(initialActivityTagName == crt_constraint->activityTagName)
				crt_constraint->activityTagName=finalActivityTagName;
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS){
			ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTags* crt_constraint=(ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTags*)ctr;
			if(initialActivityTagName == crt_constraint->firstActivityTag)
				crt_constraint->firstActivityTag=finalActivityTagName;
			if(initialActivityTagName == crt_constraint->secondActivityTag)
				crt_constraint->secondActivityTag=finalActivityTagName;
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS){
			ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTags* crt_constraint=(ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTags*)ctr;
			if(initialActivityTagName == crt_constraint->firstActivityTag)
				crt_constraint->firstActivityTag=finalActivityTagName;
			if(initialActivityTagName == crt_constraint->secondActivityTag)
				crt_constraint->secondActivityTag=finalActivityTagName;
		}
		else if(ctr->type==CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS){
			ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTags* crt_constraint=(ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTags*)ctr;
			if(initialActivityTagName == crt_constraint->firstActivityTag)
				crt_constraint->firstActivityTag=finalActivityTagName;
			if(initialActivityTagName == crt_constraint->secondActivityTag)
				crt_constraint->secondActivityTag=finalActivityTagName;
		}
		else if(ctr->type==CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS){
			ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTags* crt_constraint=(ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTags*)ctr;
			if(initialActivityTagName == crt_constraint->firstActivityTag)
				crt_constraint->firstActivityTag=finalActivityTagName;
			if(initialActivityTagName == crt_constraint->secondActivityTag)
				crt_constraint->secondActivityTag=finalActivityTagName;
		}
		else if(ctr->type==CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
			ConstraintTeachersActivityTagMaxHoursContinuously* crt_constraint=(ConstraintTeachersActivityTagMaxHoursContinuously*)ctr;
			if(initialActivityTagName == crt_constraint->activityTagName)
				crt_constraint->activityTagName=finalActivityTagName;
		}
		else if(ctr->type==CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_DAILY){
			ConstraintTeachersActivityTagMaxHoursDaily* crt_constraint=(ConstraintTeachersActivityTagMaxHoursDaily*)ctr;
			if(initialActivityTagName == crt_constraint->activityTagName)
				crt_constraint->activityTagName=finalActivityTagName;
		}
		else if(ctr->type==CONSTRAINT_TEACHERS_ACTIVITY_TAG_MIN_HOURS_DAILY){
			ConstraintTeachersActivityTagMinHoursDaily* crt_constraint=(ConstraintTeachersActivityTagMinHoursDaily*)ctr;
			if(initialActivityTagName == crt_constraint->activityTagName)
				crt_constraint->activityTagName=finalActivityTagName;
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
			ConstraintStudentsActivityTagMaxHoursContinuously* crt_constraint=(ConstraintStudentsActivityTagMaxHoursContinuously*)ctr;
			if(initialActivityTagName == crt_constraint->activityTagName)
				crt_constraint->activityTagName=finalActivityTagName;
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_DAILY){
			ConstraintStudentsActivityTagMaxHoursDaily* crt_constraint=(ConstraintStudentsActivityTagMaxHoursDaily*)ctr;
			if(initialActivityTagName == crt_constraint->activityTagName)
				crt_constraint->activityTagName=finalActivityTagName;
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_ACTIVITY_TAG_MIN_HOURS_DAILY){
			ConstraintStudentsActivityTagMinHoursDaily* crt_constraint=(ConstraintStudentsActivityTagMinHoursDaily*)ctr;
			if(initialActivityTagName == crt_constraint->activityTagName)
				crt_constraint->activityTagName=finalActivityTagName;
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
			ConstraintStudentsSetActivityTagMaxHoursContinuously* crt_constraint=(ConstraintStudentsSetActivityTagMaxHoursContinuously*)ctr;
			if(initialActivityTagName == crt_constraint->activityTagName)
				crt_constraint->activityTagName=finalActivityTagName;
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_DAILY){
			ConstraintStudentsSetActivityTagMaxHoursDaily* crt_constraint=(ConstraintStudentsSetActivityTagMaxHoursDaily*)ctr;
			if(initialActivityTagName == crt_constraint->activityTagName)
				crt_constraint->activityTagName=finalActivityTagName;
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MIN_HOURS_DAILY){
			ConstraintStudentsSetActivityTagMinHoursDaily* crt_constraint=(ConstraintStudentsSetActivityTagMinHoursDaily*)ctr;
			if(initialActivityTagName == crt_constraint->activityTagName)
				crt_constraint->activityTagName=finalActivityTagName;
		}
		else if(ctr->type==CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS){
			ConstraintActivitiesPreferredTimeSlots* crt_constraint=(ConstraintActivitiesPreferredTimeSlots*)ctr;
			if(initialActivityTagName == crt_constraint->p_activityTagName)
				crt_constraint->p_activityTagName=finalActivityTagName;
		}
		else if(ctr->type==CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES){
			ConstraintActivitiesPreferredStartingTimes* crt_constraint=(ConstraintActivitiesPreferredStartingTimes*)ctr;
			if(initialActivityTagName == crt_constraint->activityTagName)
				crt_constraint->activityTagName=finalActivityTagName;
		}
		else if(ctr->type==CONSTRAINT_ACTIVITIES_END_STUDENTS_DAY){
			ConstraintActivitiesEndStudentsDay* crt_constraint=(ConstraintActivitiesEndStudentsDay*)ctr;
			if(initialActivityTagName == crt_constraint->activityTagName)
				crt_constraint->activityTagName=finalActivityTagName;
		}
		else if(ctr->type==CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS){
			ConstraintSubactivitiesPreferredTimeSlots* crt_constraint=(ConstraintSubactivitiesPreferredTimeSlots*)ctr;
			if(initialActivityTagName == crt_constraint->p_activityTagName)
				crt_constraint->p_activityTagName=finalActivityTagName;
		}
		else if(ctr->type==CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES){
			ConstraintSubactivitiesPreferredStartingTimes* crt_constraint=(ConstraintSubactivitiesPreferredStartingTimes*)ctr;
			if(initialActivityTagName == crt_constraint->activityTagName)
				crt_constraint->activityTagName=finalActivityTagName;
		}
		else if(ctr->type==CONSTRAINT_ACTIVITY_TAGS_NOT_OVERLAPPING){
			ConstraintActivityTagsNotOverlapping* crt_constraint=(ConstraintActivityTagsNotOverlapping*)ctr;
			int cnt=0;
			for(int i=0; i<crt_constraint->activityTagsNames.count(); i++){
				if(crt_constraint->activityTagsNames.at(i)==initialActivityTagName){
					crt_constraint->activityTagsNames[i]=finalActivityTagName;
					cnt++;
				}
			}
			assert(cnt<=1);
		}
	}

	//modify the space constraints related to this activity tag
	for(SpaceConstraint* ctr : qAsConst(spaceConstraintsList)){
		if(ctr->type==CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM){
			ConstraintSubjectActivityTagPreferredRoom* c=(ConstraintSubjectActivityTagPreferredRoom*)ctr;
			if(c->activityTagName == initialActivityTagName)
				c->activityTagName=finalActivityTagName;
		}
		else if(ctr->type==CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS){
			ConstraintSubjectActivityTagPreferredRooms* c=(ConstraintSubjectActivityTagPreferredRooms*)ctr;
			if(c->activityTagName == initialActivityTagName)
				c->activityTagName=finalActivityTagName;
		}
		else if(ctr->type==CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM){
			ConstraintActivityTagPreferredRoom* c=(ConstraintActivityTagPreferredRoom*)ctr;
			if(c->activityTagName == initialActivityTagName)
				c->activityTagName=finalActivityTagName;
		}
		else if(ctr->type==CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS){
			ConstraintActivityTagPreferredRooms* c=(ConstraintActivityTagPreferredRooms*)ctr;
			if(c->activityTagName == initialActivityTagName)
				c->activityTagName=finalActivityTagName;
		}
	}

	//rename the activity tag in the list
	int t=0;
	
	for(int i=0; i<this->activityTagsList.size(); i++){
		ActivityTag* sbt=this->activityTagsList[i];

		if(sbt->name==initialActivityTagName){
			t++;
			sbt->name=finalActivityTagName;
		}
	}
	
	assert(t==1);

	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	return true;
}

void Rules::sortActivityTagsAlphabetically()
{
	std::stable_sort(this->activityTagsList.begin(), this->activityTagsList.end(), activityTagsAscending);

	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;
}

bool Rules::setsShareStudents(const QString& studentsSet1, const QString& studentsSet2)
{
	StudentsSet* s1=this->searchStudentsSet(studentsSet1);
	StudentsSet* s2=this->searchStudentsSet(studentsSet2);
	assert(s1!=nullptr);
	assert(s2!=nullptr);
	
	QSet<QString> downwardSets1;
	
	if(s1->type==STUDENTS_YEAR){
		StudentsYear* year1=(StudentsYear*)s1;
		downwardSets1.insert(year1->name);
		for(StudentsGroup* group1 : qAsConst(year1->groupsList)){
			downwardSets1.insert(group1->name);
			for(StudentsSubgroup* subgroup1 : qAsConst(group1->subgroupsList))
				downwardSets1.insert(subgroup1->name);
		}
	}
	else if(s1->type==STUDENTS_GROUP){
		StudentsGroup* group1=(StudentsGroup*)s1;
		downwardSets1.insert(group1->name);
		for(StudentsSubgroup* subgroup1 : qAsConst(group1->subgroupsList))
			downwardSets1.insert(subgroup1->name);
	}
	else if(s1->type==STUDENTS_SUBGROUP){
		StudentsSubgroup* subgroup1=(StudentsSubgroup*)s1;
		downwardSets1.insert(subgroup1->name);
	}
	else
		assert(0);
		
	if(s2->type==STUDENTS_YEAR){
		StudentsYear* year2=(StudentsYear*)s2;
		if(downwardSets1.contains(year2->name))
			return true;
		for(StudentsGroup* group2 : qAsConst(year2->groupsList)){
			if(downwardSets1.contains(group2->name))
				return true;
			for(StudentsSubgroup* subgroup2 : qAsConst(group2->subgroupsList))
				if(downwardSets1.contains(subgroup2->name))
					return true;
		}
	}
	else if(s2->type==STUDENTS_GROUP){
		StudentsGroup* group2=(StudentsGroup*)s2;
		if(downwardSets1.contains(group2->name))
			return true;
		for(StudentsSubgroup* subgroup2 : qAsConst(group2->subgroupsList))
			if(downwardSets1.contains(subgroup2->name))
				return true;
	}
	else if(s2->type==STUDENTS_SUBGROUP){
		StudentsSubgroup* subgroup2=(StudentsSubgroup*)s2;
		if(downwardSets1.contains(subgroup2->name))
			return true;
	}
	else
		assert(0);
	
	return false;
	
}

bool Rules::augmentedSetsShareStudentsFaster(const QString& studentsSet1, const QString& studentsSet2)
{
	//StudentsSet* s1=this->searchStudentsSet(studentsSet1);
	StudentsSet* s1=studentsHash.value(studentsSet1, nullptr);
	//StudentsSet* s2=this->searchStudentsSet(studentsSet2);
	StudentsSet* s2=studentsHash.value(studentsSet2, nullptr);
	assert(s1!=nullptr);
	assert(s2!=nullptr);
	
	QSet<QString> downwardSets1;
	
	if(s1->type==STUDENTS_YEAR){
		StudentsYear* year1=(StudentsYear*)s1;
		downwardSets1.insert(year1->name);
		for(StudentsGroup* group1 : qAsConst(year1->groupsList)){
			downwardSets1.insert(group1->name);
			for(StudentsSubgroup* subgroup1 : qAsConst(group1->subgroupsList))
				downwardSets1.insert(subgroup1->name);
		}
	}
	else if(s1->type==STUDENTS_GROUP){
		StudentsGroup* group1=(StudentsGroup*)s1;
		downwardSets1.insert(group1->name);
		for(StudentsSubgroup* subgroup1 : qAsConst(group1->subgroupsList))
			downwardSets1.insert(subgroup1->name);
	}
	else if(s1->type==STUDENTS_SUBGROUP){
		StudentsSubgroup* subgroup1=(StudentsSubgroup*)s1;
		downwardSets1.insert(subgroup1->name);
	}
	else
		assert(0);
		
	if(s2->type==STUDENTS_YEAR){
		StudentsYear* year2=(StudentsYear*)s2;
		if(downwardSets1.contains(year2->name))
			return true;
		for(StudentsGroup* group2 : qAsConst(year2->groupsList)){
			if(downwardSets1.contains(group2->name))
				return true;
			for(StudentsSubgroup* subgroup2 : qAsConst(group2->subgroupsList))
				if(downwardSets1.contains(subgroup2->name))
					return true;
		}
	}
	else if(s2->type==STUDENTS_GROUP){
		StudentsGroup* group2=(StudentsGroup*)s2;
		if(downwardSets1.contains(group2->name))
			return true;
		for(StudentsSubgroup* subgroup2 : qAsConst(group2->subgroupsList))
			if(downwardSets1.contains(subgroup2->name))
				return true;
	}
	else if(s2->type==STUDENTS_SUBGROUP){
		StudentsSubgroup* subgroup2=(StudentsSubgroup*)s2;
		if(downwardSets1.contains(subgroup2->name))
			return true;
	}
	else
		assert(0);
	
	return false;
	
}

void Rules::computePermanentStudentsHash()
{
	//The commented tests are good, but bring a somewhat slowdown.
	permanentStudentsHash.clear();
	
	for(StudentsYear* year : qAsConst(yearsList)){
		assert(!permanentStudentsHash.contains(year->name));
		permanentStudentsHash.insert(year->name, year);
		
		//QSet<QString> groupsInYear;
		
		for(StudentsGroup* group : qAsConst(year->groupsList)){
			//assert(!groupsInYear.contains(group->name));
			//groupsInYear.insert(group->name);
		
			if(!permanentStudentsHash.contains(group->name))
				permanentStudentsHash.insert(group->name, group);
			else
				assert(permanentStudentsHash.value(group->name)==group);
			
			//QSet<QString> subgroupsInGroup;
			
			for(StudentsSubgroup* subgroup : qAsConst(group->subgroupsList)){
				//assert(!subgroupsInGroup.contains(subgroup->name));
				//subgroupsInGroup.insert(subgroup->name);
			
				if(!permanentStudentsHash.contains(subgroup->name))
					permanentStudentsHash.insert(subgroup->name, subgroup);
				else
					assert(permanentStudentsHash.value(subgroup->name)==subgroup);
			}
		}
	}
}

StudentsSet* Rules::searchStudentsSet(const QString& setName)
{
	return permanentStudentsHash.value(setName, nullptr);

	/*for(int i=0; i<this->yearsList.size(); i++){
		StudentsYear* sty=this->yearsList[i];
		if(sty->name==setName)
			return sty;
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			if(stg->name==setName)
				return stg;
			for(int k=0; k<stg->subgroupsList.size(); k++){
				StudentsSubgroup* sts=stg->subgroupsList[k];
				if(sts->name==setName)
					return sts;
			}
		}
	}
	return nullptr;*/
}

StudentsSet* Rules::searchAugmentedStudentsSet(const QString& setName)
{
	for(int i=0; i<this->augmentedYearsList.size(); i++){
		StudentsYear* sty=this->augmentedYearsList[i];
		if(sty->name==setName)
			return sty;
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			if(stg->name==setName)
				return stg;
			for(int k=0; k<stg->subgroupsList.size(); k++){
				StudentsSubgroup* sts=stg->subgroupsList[k];
				if(sts->name==setName)
					return sts;
			}
		}
	}
	return nullptr;
}

bool Rules::addYear(StudentsYear* year)
{
	//already existing?
	for(StudentsYear* ty : qAsConst(yearsList))
		if(ty->name==year->name)
			return false;
	//if(this->searchStudentsSet(year->name)!=nullptr)
	//	return false;
	this->yearsList << year;
	
	assert(!permanentStudentsHash.contains(year->name));
	permanentStudentsHash.insert(year->name, year);
	
	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	return true;
}

bool Rules::addYearFast(StudentsYear* year)
{
	this->yearsList << year;
	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	return true;
}

/*bool Rules::removeYear(const QString& yearName)
{
	return removeYear(yearName, true);
}

bool Rules::emptyYear(const QString& yearName)
{
	return removeYear(yearName, false);
}*/

bool Rules::removeYear(const QString& yearName/*, bool removeAlsoThisYear*/)
{
	const bool removeAlsoThisYear=true;

	StudentsYear* yearPointer=nullptr;
	for(StudentsYear* ty : qAsConst(this->yearsList)){
		if(ty->name==yearName){
			yearPointer=ty;
			break;
		}
	}

	assert(yearPointer!=nullptr);
	
	//pointers
	QSet<StudentsSet*> tmpSet;
	for(StudentsYear* year : qAsConst(yearsList))
		if(year->name!=yearName){
			tmpSet.insert(year);
			for(StudentsGroup* group : qAsConst(year->groupsList)){
				tmpSet.insert(group);
				for(StudentsSubgroup* subgroup : qAsConst(group->subgroupsList))
					tmpSet.insert(subgroup);
			}
		}
	
	QSet<StudentsSet*> toBeRemoved;
	if(removeAlsoThisYear)
		toBeRemoved.insert(yearPointer);
	for(StudentsGroup* group : qAsConst(yearPointer->groupsList)){
		assert(!toBeRemoved.contains(group));
		if(!tmpSet.contains(group))
			toBeRemoved.insert(group);
		for(StudentsSubgroup* subgroup : qAsConst(group->subgroupsList)){
			//assert(!toBeRemoved.contains(subgroup));
			if(!tmpSet.contains(subgroup) && !toBeRemoved.contains(subgroup))
				toBeRemoved.insert(subgroup);
		}
	}
	
	updateActivitiesWhenRemovingStudents(toBeRemoved, false);
	
	if(removeAlsoThisYear){
		for(int i=0; i<yearsList.count(); i++)
			if(yearsList.at(i)==yearPointer){
				yearsList.removeAt(i);
				break;
			}
	}
	else{
		yearPointer->groupsList.clear();
	}
	
	for(StudentsSet* studentsSet : qAsConst(toBeRemoved)){
		assert(permanentStudentsHash.contains(studentsSet->name));
		permanentStudentsHash.remove(studentsSet->name);
	
		delete studentsSet;
	}
		
	if(toBeRemoved.count()>0)
		updateConstraintsAfterRemoval();
	
	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	return true;
}

bool Rules::removeYearPointerAfterSplit(StudentsYear* yearPointer)
{
	assert(yearPointer!=nullptr);
	
	//names
	QSet<StudentsSet*> toBeRemoved;

	//Not here, because there exists another pointer (to the new year) with the same name,
	//and I don't want to remove the activities with this year name
	//toBeRemoved.insert(yearPointer);
	for(StudentsGroup* group : qAsConst(yearPointer->groupsList)){
		assert(!toBeRemoved.contains(group));
		if(!permanentStudentsHash.contains(group->name))
			toBeRemoved.insert(group);
		for(StudentsSubgroup* subgroup : qAsConst(group->subgroupsList)){
			//assert(!toBeRemoved.contains(subgroup));
			if(!permanentStudentsHash.contains(subgroup->name) && !toBeRemoved.contains(subgroup))
				toBeRemoved.insert(subgroup);
		}
	}
	
	updateActivitiesWhenRemovingStudents(toBeRemoved, false);
	
	toBeRemoved.insert(yearPointer);
	for(StudentsSet* studentsSet : qAsConst(toBeRemoved))
		delete studentsSet;
	
	if(toBeRemoved.count()>1)
		updateConstraintsAfterRemoval();
	
	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	return true;
}

int Rules::searchYear(const QString& yearName)
{
	for(int i=0; i<this->yearsList.size(); i++)
		if(this->yearsList[i]->name==yearName)
			return i;

	return -1;
}

int Rules::searchAugmentedYear(const QString& yearName)
{
	for(int i=0; i<this->augmentedYearsList.size(); i++)
		if(this->augmentedYearsList[i]->name==yearName)
			return i;

	return -1;
}

bool Rules::modifyStudentsSet(const QString& initialStudentsSetName, const QString& finalStudentsSetName, int finalNumberOfStudents)
{
	StudentsSet* studentsSet=searchStudentsSet(initialStudentsSetName);
	assert(studentsSet!=nullptr);
	if(initialStudentsSetName!=finalStudentsSetName)
		assert(searchStudentsSet(finalStudentsSetName)==nullptr);
	int initialNumberOfStudents=studentsSet->numberOfStudents;
	
	for(Activity* act : qAsConst(activitiesList))
		act->renameStudents(*this, initialStudentsSetName, finalStudentsSetName, initialNumberOfStudents, finalNumberOfStudents);
	
	if(initialStudentsSetName!=finalStudentsSetName){
		for(TimeConstraint* ctr : qAsConst(timeConstraintsList)){
			if(ctr->type==CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE_TIMES){
				ConstraintStudentsSetNotAvailableTimes* crt_constraint=(ConstraintStudentsSetNotAvailableTimes*)ctr;
				if(initialStudentsSetName == crt_constraint->students)
					crt_constraint->students=finalStudentsSetName;
			}
			else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY){
				ConstraintStudentsSetMaxHoursDaily* crt_constraint=(ConstraintStudentsSetMaxHoursDaily*)ctr;
				if(initialStudentsSetName == crt_constraint->students)
					crt_constraint->students=finalStudentsSetName;
			}
			else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_DAYS_PER_WEEK){
				ConstraintStudentsSetMaxDaysPerWeek* crt_constraint=(ConstraintStudentsSetMaxDaysPerWeek*)ctr;
				if(initialStudentsSetName == crt_constraint->students)
					crt_constraint->students=finalStudentsSetName;
			}
			else if(ctr->type==CONSTRAINT_STUDENTS_SET_INTERVAL_MAX_DAYS_PER_WEEK){
				ConstraintStudentsSetIntervalMaxDaysPerWeek* crt_constraint=(ConstraintStudentsSetIntervalMaxDaysPerWeek*)ctr;
				if(initialStudentsSetName == crt_constraint->students)
					crt_constraint->students=finalStudentsSetName;
			}
			else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_HOURS_CONTINUOUSLY){
				ConstraintStudentsSetMaxHoursContinuously* crt_constraint=(ConstraintStudentsSetMaxHoursContinuously*)ctr;
				if(initialStudentsSetName == crt_constraint->students)
					crt_constraint->students=finalStudentsSetName;
			}
			else if(ctr->type==CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
				ConstraintStudentsSetActivityTagMaxHoursContinuously* crt_constraint=(ConstraintStudentsSetActivityTagMaxHoursContinuously*)ctr;
				if(initialStudentsSetName == crt_constraint->students)
					crt_constraint->students=finalStudentsSetName;
			}
			else if(ctr->type==CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_DAILY){
				ConstraintStudentsSetActivityTagMaxHoursDaily* crt_constraint=(ConstraintStudentsSetActivityTagMaxHoursDaily*)ctr;
				if(initialStudentsSetName == crt_constraint->students)
					crt_constraint->students=finalStudentsSetName;
			}
			else if(ctr->type==CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MIN_HOURS_DAILY){
				ConstraintStudentsSetActivityTagMinHoursDaily* crt_constraint=(ConstraintStudentsSetActivityTagMinHoursDaily*)ctr;
				if(initialStudentsSetName == crt_constraint->students)
					crt_constraint->students=finalStudentsSetName;
			}
			else if(ctr->type==CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY){
				ConstraintStudentsSetMinHoursDaily* crt_constraint=(ConstraintStudentsSetMinHoursDaily*)ctr;
				if(initialStudentsSetName == crt_constraint->students)
					crt_constraint->students=finalStudentsSetName;
			}
			else if(ctr->type==CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS){
				ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTags* crt_constraint=(ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTags*)ctr;
				if(initialStudentsSetName == crt_constraint->students)
					crt_constraint->students=finalStudentsSetName;
			}
			else if(ctr->type==CONSTRAINT_STUDENTS_SET_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR){
				ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour* crt_constraint=(ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour*)ctr;
				if(initialStudentsSetName == crt_constraint->students)
					crt_constraint->students=finalStudentsSetName;
			}
			else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_WEEK){
				ConstraintStudentsSetMaxGapsPerWeek* crt_constraint=(ConstraintStudentsSetMaxGapsPerWeek*)ctr;
				if(initialStudentsSetName == crt_constraint->students)
					crt_constraint->students=finalStudentsSetName;
			}
			else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_DAY){
				ConstraintStudentsSetMaxGapsPerDay* crt_constraint=(ConstraintStudentsSetMaxGapsPerDay*)ctr;
				if(initialStudentsSetName == crt_constraint->students)
					crt_constraint->students=finalStudentsSetName;
			}
			else if(ctr->type==CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS){
				ConstraintActivitiesPreferredTimeSlots* crt_constraint=(ConstraintActivitiesPreferredTimeSlots*)ctr;
				if(initialStudentsSetName == crt_constraint->p_studentsName)
					crt_constraint->p_studentsName=finalStudentsSetName;
			}
			else if(ctr->type==CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES){
				ConstraintActivitiesPreferredStartingTimes* crt_constraint=(ConstraintActivitiesPreferredStartingTimes*)ctr;
				if(initialStudentsSetName == crt_constraint->studentsName)
					crt_constraint->studentsName=finalStudentsSetName;
			}
			else if(ctr->type==CONSTRAINT_ACTIVITIES_END_STUDENTS_DAY){
				ConstraintActivitiesEndStudentsDay* crt_constraint=(ConstraintActivitiesEndStudentsDay*)ctr;
				if(initialStudentsSetName == crt_constraint->studentsName)
					crt_constraint->studentsName=finalStudentsSetName;
			}
			else if(ctr->type==CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS){
				ConstraintSubactivitiesPreferredTimeSlots* crt_constraint=(ConstraintSubactivitiesPreferredTimeSlots*)ctr;
				if(initialStudentsSetName == crt_constraint->p_studentsName)
					crt_constraint->p_studentsName=finalStudentsSetName;
			}
			else if(ctr->type==CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES){
				ConstraintSubactivitiesPreferredStartingTimes* crt_constraint=(ConstraintSubactivitiesPreferredStartingTimes*)ctr;
				if(initialStudentsSetName == crt_constraint->studentsName)
					crt_constraint->studentsName=finalStudentsSetName;
			}
			//2017-02-07
			else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_SPAN_PER_DAY){
				ConstraintStudentsSetMaxSpanPerDay* crt_constraint=(ConstraintStudentsSetMaxSpanPerDay*)ctr;
				if(initialStudentsSetName == crt_constraint->students)
					crt_constraint->students=finalStudentsSetName;
			}
			else if(ctr->type==CONSTRAINT_STUDENTS_SET_MIN_RESTING_HOURS){
				ConstraintStudentsSetMinRestingHours* crt_constraint=(ConstraintStudentsSetMinRestingHours*)ctr;
				if(initialStudentsSetName == crt_constraint->students)
					crt_constraint->students=finalStudentsSetName;
			}
		}

		for(SpaceConstraint* ctr : qAsConst(spaceConstraintsList)){
			if(ctr->type==CONSTRAINT_STUDENTS_SET_HOME_ROOM){
				ConstraintStudentsSetHomeRoom* crt_constraint=(ConstraintStudentsSetHomeRoom*)ctr;
				if(initialStudentsSetName == crt_constraint->studentsName)
					crt_constraint->studentsName=finalStudentsSetName;
			}
			else if(ctr->type==CONSTRAINT_STUDENTS_SET_HOME_ROOMS){
				ConstraintStudentsSetHomeRooms* crt_constraint=(ConstraintStudentsSetHomeRooms*)ctr;
				if(initialStudentsSetName == crt_constraint->studentsName)
					crt_constraint->studentsName=finalStudentsSetName;
			}
			else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY){
				ConstraintStudentsSetMaxBuildingChangesPerDay* crt_constraint=(ConstraintStudentsSetMaxBuildingChangesPerDay*)ctr;
				if(initialStudentsSetName == crt_constraint->studentsName)
					crt_constraint->studentsName=finalStudentsSetName;
			}
			else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_WEEK){
				ConstraintStudentsSetMaxBuildingChangesPerWeek* crt_constraint=(ConstraintStudentsSetMaxBuildingChangesPerWeek*)ctr;
				if(initialStudentsSetName == crt_constraint->studentsName)
					crt_constraint->studentsName=finalStudentsSetName;
			}
			else if(ctr->type==CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_BUILDING_CHANGES){
				ConstraintStudentsSetMinGapsBetweenBuildingChanges* crt_constraint=(ConstraintStudentsSetMinGapsBetweenBuildingChanges*)ctr;
				if(initialStudentsSetName == crt_constraint->studentsName)
					crt_constraint->studentsName=finalStudentsSetName;
			}
			else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY){
				ConstraintStudentsSetMaxRoomChangesPerDay* crt_constraint=(ConstraintStudentsSetMaxRoomChangesPerDay*)ctr;
				if(initialStudentsSetName == crt_constraint->studentsName)
					crt_constraint->studentsName=finalStudentsSetName;
			}
			else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_WEEK){
				ConstraintStudentsSetMaxRoomChangesPerWeek* crt_constraint=(ConstraintStudentsSetMaxRoomChangesPerWeek*)ctr;
				if(initialStudentsSetName == crt_constraint->studentsName)
					crt_constraint->studentsName=finalStudentsSetName;
			}
			else if(ctr->type==CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ROOM_CHANGES){
				ConstraintStudentsSetMinGapsBetweenRoomChanges* crt_constraint=(ConstraintStudentsSetMinGapsBetweenRoomChanges*)ctr;
				if(initialStudentsSetName == crt_constraint->studentsName)
					crt_constraint->studentsName=finalStudentsSetName;
			}
		}
	}

	assert(studentsSet->name==initialStudentsSetName);
	assert(studentsSet->numberOfStudents==initialNumberOfStudents);
	studentsSet->name=finalStudentsSetName;
	studentsSet->numberOfStudents=finalNumberOfStudents;
	
	assert(permanentStudentsHash.contains(initialStudentsSetName));
	if(initialStudentsSetName!=finalStudentsSetName){
		permanentStudentsHash.remove(initialStudentsSetName);
		permanentStudentsHash.insert(studentsSet->name, studentsSet);

		if(ssnatHash.contains(initialStudentsSetName)){
			QSet<ConstraintStudentsSetNotAvailableTimes*> cs=ssnatHash.value(initialStudentsSetName);
			ssnatHash.remove(initialStudentsSetName);
			assert(!ssnatHash.contains(finalStudentsSetName));
			ssnatHash.insert(finalStudentsSetName, cs);
		}
	}
	
	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;
	
	return true;
}

//by Volker Dirr (start) - very similar to Liviu's modifyStudentsSet
bool Rules::modifyStudentsSets(const QHash<QString, QString>& oldAndNewStudentsSetNames){
	if(oldAndNewStudentsSetNames.isEmpty())
		return true;

	for(Activity* act : qAsConst(activitiesList)){
		for(int i=0; i<act->studentsNames.count(); i++)
			if(oldAndNewStudentsSetNames.contains(act->studentsNames.at(i)))
				act->studentsNames[i]=oldAndNewStudentsSetNames.value(act->studentsNames.at(i));
	}
	
	for(TimeConstraint* ctr : qAsConst(timeConstraintsList)){
		if(ctr->type==CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE_TIMES){
			ConstraintStudentsSetNotAvailableTimes* crt_constraint=(ConstraintStudentsSetNotAvailableTimes*)ctr;
			if(oldAndNewStudentsSetNames.contains(crt_constraint->students))
				crt_constraint->students=oldAndNewStudentsSetNames.value(crt_constraint->students);
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY){
			ConstraintStudentsSetMaxHoursDaily* crt_constraint=(ConstraintStudentsSetMaxHoursDaily*)ctr;
			if(oldAndNewStudentsSetNames.contains(crt_constraint->students))
				crt_constraint->students=oldAndNewStudentsSetNames.value(crt_constraint->students);
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_DAYS_PER_WEEK){
			ConstraintStudentsSetMaxDaysPerWeek* crt_constraint=(ConstraintStudentsSetMaxDaysPerWeek*)ctr;
			if(oldAndNewStudentsSetNames.contains(crt_constraint->students))
				crt_constraint->students=oldAndNewStudentsSetNames.value(crt_constraint->students);
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_INTERVAL_MAX_DAYS_PER_WEEK){
			ConstraintStudentsSetIntervalMaxDaysPerWeek* crt_constraint=(ConstraintStudentsSetIntervalMaxDaysPerWeek*)ctr;
			if(oldAndNewStudentsSetNames.contains(crt_constraint->students))
				crt_constraint->students=oldAndNewStudentsSetNames.value(crt_constraint->students);
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_HOURS_CONTINUOUSLY){
			ConstraintStudentsSetMaxHoursContinuously* crt_constraint=(ConstraintStudentsSetMaxHoursContinuously*)ctr;
			if(oldAndNewStudentsSetNames.contains(crt_constraint->students))
				crt_constraint->students=oldAndNewStudentsSetNames.value(crt_constraint->students);
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
			ConstraintStudentsSetActivityTagMaxHoursContinuously* crt_constraint=(ConstraintStudentsSetActivityTagMaxHoursContinuously*)ctr;
			if(oldAndNewStudentsSetNames.contains(crt_constraint->students))
				crt_constraint->students=oldAndNewStudentsSetNames.value(crt_constraint->students);
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_DAILY){
			ConstraintStudentsSetActivityTagMaxHoursDaily* crt_constraint=(ConstraintStudentsSetActivityTagMaxHoursDaily*)ctr;
			if(oldAndNewStudentsSetNames.contains(crt_constraint->students))
				crt_constraint->students=oldAndNewStudentsSetNames.value(crt_constraint->students);
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MIN_HOURS_DAILY){
			ConstraintStudentsSetActivityTagMinHoursDaily* crt_constraint=(ConstraintStudentsSetActivityTagMinHoursDaily*)ctr;
			if(oldAndNewStudentsSetNames.contains(crt_constraint->students))
				crt_constraint->students=oldAndNewStudentsSetNames.value(crt_constraint->students);
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY){
			ConstraintStudentsSetMinHoursDaily* crt_constraint=(ConstraintStudentsSetMinHoursDaily*)ctr;
			if(oldAndNewStudentsSetNames.contains(crt_constraint->students))
				crt_constraint->students=oldAndNewStudentsSetNames.value(crt_constraint->students);
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS){
			ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTags* crt_constraint=(ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTags*)ctr;
			if(oldAndNewStudentsSetNames.contains(crt_constraint->students))
				crt_constraint->students=oldAndNewStudentsSetNames.value(crt_constraint->students);
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR){
			ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour* crt_constraint=(ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour*)ctr;
			if(oldAndNewStudentsSetNames.contains(crt_constraint->students))
				crt_constraint->students=oldAndNewStudentsSetNames.value(crt_constraint->students);
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_WEEK){
			ConstraintStudentsSetMaxGapsPerWeek* crt_constraint=(ConstraintStudentsSetMaxGapsPerWeek*)ctr;
			if(oldAndNewStudentsSetNames.contains(crt_constraint->students))
				crt_constraint->students=oldAndNewStudentsSetNames.value(crt_constraint->students);
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_DAY){
			ConstraintStudentsSetMaxGapsPerDay* crt_constraint=(ConstraintStudentsSetMaxGapsPerDay*)ctr;
			if(oldAndNewStudentsSetNames.contains(crt_constraint->students))
				crt_constraint->students=oldAndNewStudentsSetNames.value(crt_constraint->students);
		}
		else if(ctr->type==CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS){
			ConstraintActivitiesPreferredTimeSlots* crt_constraint=(ConstraintActivitiesPreferredTimeSlots*)ctr;
			if(oldAndNewStudentsSetNames.contains(crt_constraint->p_studentsName))
				crt_constraint->p_studentsName=oldAndNewStudentsSetNames.value(crt_constraint->p_studentsName);
		}
		else if(ctr->type==CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES){
			ConstraintActivitiesPreferredStartingTimes* crt_constraint=(ConstraintActivitiesPreferredStartingTimes*)ctr;
			if(oldAndNewStudentsSetNames.contains(crt_constraint->studentsName))
				crt_constraint->studentsName=oldAndNewStudentsSetNames.value(crt_constraint->studentsName);
		}
		else if(ctr->type==CONSTRAINT_ACTIVITIES_END_STUDENTS_DAY){
			ConstraintActivitiesEndStudentsDay* crt_constraint=(ConstraintActivitiesEndStudentsDay*)ctr;
			if(oldAndNewStudentsSetNames.contains(crt_constraint->studentsName))
				crt_constraint->studentsName=oldAndNewStudentsSetNames.value(crt_constraint->studentsName);
		}
		else if(ctr->type==CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS){
			ConstraintSubactivitiesPreferredTimeSlots* crt_constraint=(ConstraintSubactivitiesPreferredTimeSlots*)ctr;
			if(oldAndNewStudentsSetNames.contains(crt_constraint->p_studentsName))
				crt_constraint->p_studentsName=oldAndNewStudentsSetNames.value(crt_constraint->p_studentsName);
		}
		else if(ctr->type==CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES){
			ConstraintSubactivitiesPreferredStartingTimes* crt_constraint=(ConstraintSubactivitiesPreferredStartingTimes*)ctr;
			if(oldAndNewStudentsSetNames.contains(crt_constraint->studentsName))
				crt_constraint->studentsName=oldAndNewStudentsSetNames.value(crt_constraint->studentsName);
		}
		//2017-02-07
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_SPAN_PER_DAY){
			ConstraintStudentsSetMaxSpanPerDay* crt_constraint=(ConstraintStudentsSetMaxSpanPerDay*)ctr;
			if(oldAndNewStudentsSetNames.contains(crt_constraint->students))
				crt_constraint->students=oldAndNewStudentsSetNames.value(crt_constraint->students);
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_MIN_RESTING_HOURS){
			ConstraintStudentsSetMinRestingHours* crt_constraint=(ConstraintStudentsSetMinRestingHours*)ctr;
			if(oldAndNewStudentsSetNames.contains(crt_constraint->students))
				crt_constraint->students=oldAndNewStudentsSetNames.value(crt_constraint->students);
		}
	}

	for(SpaceConstraint* ctr : qAsConst(spaceConstraintsList)){
		if(ctr->type==CONSTRAINT_STUDENTS_SET_HOME_ROOM){
			ConstraintStudentsSetHomeRoom* crt_constraint=(ConstraintStudentsSetHomeRoom*)ctr;
			if(oldAndNewStudentsSetNames.contains(crt_constraint->studentsName))
				crt_constraint->studentsName=oldAndNewStudentsSetNames.value(crt_constraint->studentsName);
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_HOME_ROOMS){
			ConstraintStudentsSetHomeRooms* crt_constraint=(ConstraintStudentsSetHomeRooms*)ctr;
			if(oldAndNewStudentsSetNames.contains(crt_constraint->studentsName))
				crt_constraint->studentsName=oldAndNewStudentsSetNames.value(crt_constraint->studentsName);
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY){
			ConstraintStudentsSetMaxBuildingChangesPerDay* crt_constraint=(ConstraintStudentsSetMaxBuildingChangesPerDay*)ctr;
			if(oldAndNewStudentsSetNames.contains(crt_constraint->studentsName))
				crt_constraint->studentsName=oldAndNewStudentsSetNames.value(crt_constraint->studentsName);
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_WEEK){
			ConstraintStudentsSetMaxBuildingChangesPerWeek* crt_constraint=(ConstraintStudentsSetMaxBuildingChangesPerWeek*)ctr;
			if(oldAndNewStudentsSetNames.contains(crt_constraint->studentsName))
				crt_constraint->studentsName=oldAndNewStudentsSetNames.value(crt_constraint->studentsName);
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_BUILDING_CHANGES){
			ConstraintStudentsSetMinGapsBetweenBuildingChanges* crt_constraint=(ConstraintStudentsSetMinGapsBetweenBuildingChanges*)ctr;
			if(oldAndNewStudentsSetNames.contains(crt_constraint->studentsName))
				crt_constraint->studentsName=oldAndNewStudentsSetNames.value(crt_constraint->studentsName);
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY){
			ConstraintStudentsSetMaxRoomChangesPerDay* crt_constraint=(ConstraintStudentsSetMaxRoomChangesPerDay*)ctr;
			if(oldAndNewStudentsSetNames.contains(crt_constraint->studentsName))
				crt_constraint->studentsName=oldAndNewStudentsSetNames.value(crt_constraint->studentsName);
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_WEEK){
			ConstraintStudentsSetMaxRoomChangesPerWeek* crt_constraint=(ConstraintStudentsSetMaxRoomChangesPerWeek*)ctr;
			if(oldAndNewStudentsSetNames.contains(crt_constraint->studentsName))
				crt_constraint->studentsName=oldAndNewStudentsSetNames.value(crt_constraint->studentsName);
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ROOM_CHANGES){
			ConstraintStudentsSetMinGapsBetweenRoomChanges* crt_constraint=(ConstraintStudentsSetMinGapsBetweenRoomChanges*)ctr;
			if(oldAndNewStudentsSetNames.contains(crt_constraint->studentsName))
				crt_constraint->studentsName=oldAndNewStudentsSetNames.value(crt_constraint->studentsName);
		}
	}

	QHash<QString, QString>::const_iterator i=oldAndNewStudentsSetNames.constBegin();
	while(i!=oldAndNewStudentsSetNames.constEnd()) {
		StudentsSet* studentsSet=searchStudentsSet(i.key());
		assert(studentsSet!=nullptr);
		studentsSet->name=i.value();
		
		assert(permanentStudentsHash.contains(i.key()));
		permanentStudentsHash.remove(i.key());
		permanentStudentsHash.insert(studentsSet->name, studentsSet);

		assert(i.key()!=i.value());
		if(ssnatHash.contains(i.key())){
			QSet<ConstraintStudentsSetNotAvailableTimes*> cs=ssnatHash.value(i.key());
			ssnatHash.remove(i.key());
			assert(!ssnatHash.contains(i.value()));
			ssnatHash.insert(i.value(), cs);
		}
		i++;
	}
	
	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);
	
	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	return true;
}
//by Volker Dirr (end) - very similar to Liviu's modifyStudentsSet

void Rules::sortYearsAlphabetically()
{
	std::stable_sort(this->yearsList.begin(), this->yearsList.end(), yearsAscending);

	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;
}

bool Rules::addGroup(const QString& yearName, StudentsGroup* group)
{
	StudentsYear* sty=nullptr;
	for(int i=0; i<this->yearsList.size(); i++){
		sty=yearsList[i];
		if(sty->name==yearName)
			break;
	}
	assert(sty);
	
	for(int i=0; i<sty->groupsList.size(); i++){
		StudentsGroup* stg=sty->groupsList[i];
		if(stg->name==group->name)
			return false;
	}
	
	sty->groupsList << group; //append
	
	if(!permanentStudentsHash.contains(group->name))
		permanentStudentsHash.insert(group->name, group);

	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	return true;
}

bool Rules::addGroupFast(StudentsYear* year, StudentsGroup* group)
{
	year->groupsList << group; //append

	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	return true;
}

bool Rules::removeGroup(const QString& yearName, const QString& groupName)
{
	StudentsYear* yearPointer=nullptr;
	for(StudentsYear* ty : qAsConst(this->yearsList)){
		if(ty->name==yearName){
			yearPointer=ty;
			break;
		}
	}

	assert(yearPointer!=nullptr);
	
	StudentsGroup* groupPointer=nullptr;
	for(StudentsGroup* tg : qAsConst(yearPointer->groupsList)){
		if(tg->name==groupName){
			groupPointer=tg;
			break;
		}
	}
	
	assert(groupPointer!=nullptr);

	//pointers
	QSet<StudentsSet*> tmpSet;
	for(StudentsYear* year : qAsConst(yearsList)){
		if(year->name!=yearName){
			//tmpSet.insert(year); useless
			for(StudentsGroup* group : qAsConst(year->groupsList)){
				if(group->name==groupName) //we shall not purge groupName, because it still exists in the current year
					tmpSet.insert(group);
				for(StudentsSubgroup* subgroup : qAsConst(group->subgroupsList))
					tmpSet.insert(subgroup);
			}
		}
		else{
			for(StudentsGroup* group : qAsConst(year->groupsList))
				if(group->name!=groupName){
					//tmpSet.insert(group); //useless
					for(StudentsSubgroup* subgroup : qAsConst(group->subgroupsList))
						tmpSet.insert(subgroup);
				}
		}
	}
	
	QSet<StudentsSet*> toBeRemoved;
	if(!tmpSet.contains(groupPointer))
		toBeRemoved.insert(groupPointer);
	for(StudentsSubgroup* subgroup : qAsConst(groupPointer->subgroupsList)){
		assert(!toBeRemoved.contains(subgroup));
		if(!tmpSet.contains(subgroup))
			toBeRemoved.insert(subgroup);
	}
	
	updateActivitiesWhenRemovingStudents(toBeRemoved, false);
	
	for(int i=0; i<yearPointer->groupsList.count(); i++)
		if(yearPointer->groupsList.at(i)==groupPointer){
			yearPointer->groupsList.removeAt(i);
			break;
		}
	
	for(StudentsSet* studentsSet : qAsConst(toBeRemoved)){
		assert(permanentStudentsHash.contains(studentsSet->name));
		permanentStudentsHash.remove(studentsSet->name);
	
		delete studentsSet;
	}
		
	if(toBeRemoved.count()>0)
		updateConstraintsAfterRemoval();
	
	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	return true;
}

bool Rules::purgeGroup(const QString& groupName)
{
	StudentsGroup* groupPointer=nullptr;
	for(StudentsYear* year : qAsConst(yearsList)){
		int j=-1;
		for(int i=0; i<year->groupsList.count(); i++){
			if(year->groupsList.at(i)->name==groupName){
				j=i;
				if(groupPointer==nullptr)
					groupPointer=year->groupsList.at(i);
				else
					assert(groupPointer==year->groupsList.at(i));
				break;
			}
		}
		if(j>=0)
			year->groupsList.removeAt(j);
	}
	
	assert(groupPointer!=nullptr);

	//pointers
	QSet<StudentsSet*> tmpSet;
	for(StudentsYear* year : qAsConst(yearsList))
		for(StudentsGroup* group : qAsConst(year->groupsList))
			for(StudentsSubgroup* subgroup : qAsConst(group->subgroupsList))
				tmpSet.insert(subgroup);
	
	QSet<StudentsSet*> toBeRemoved;
	if(!tmpSet.contains(groupPointer))
		toBeRemoved.insert(groupPointer);
	else
		assert(0);
	for(StudentsSubgroup* subgroup : qAsConst(groupPointer->subgroupsList)){
		assert(!toBeRemoved.contains(subgroup));
		if(!tmpSet.contains(subgroup))
			toBeRemoved.insert(subgroup);
	}
	
	updateActivitiesWhenRemovingStudents(toBeRemoved, false);
	
	for(StudentsSet* studentsSet : qAsConst(toBeRemoved)){
		assert(permanentStudentsHash.contains(studentsSet->name));
		permanentStudentsHash.remove(studentsSet->name);
	
		delete studentsSet;
	}
		
	if(toBeRemoved.count()>0)
		updateConstraintsAfterRemoval();
	
	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	return true;
}

int Rules::searchGroup(const QString& yearName, const QString& groupName)
{
	StudentsYear* sty=nullptr;
	for(StudentsYear* ty : qAsConst(yearsList))
		if(ty->name==yearName){
			sty=ty;
			break;
		}
	assert(sty!=nullptr);

	for(int i=0; i<sty->groupsList.size(); i++)
		if(sty->groupsList[i]->name==groupName)
			return i;
	
	return -1;
}

int Rules::searchAugmentedGroup(const QString& yearName, const QString& groupName)
{
	StudentsYear* sty=nullptr;
	for(StudentsYear* ty : qAsConst(augmentedYearsList))
		if(ty->name==yearName){
			sty=ty;
			break;
		}
	assert(sty!=nullptr);
	
	for(int i=0; i<sty->groupsList.size(); i++)
		if(sty->groupsList[i]->name==groupName)
			return i;
	
	return -1;
}

void Rules::sortGroupsAlphabetically(const QString& yearName)
{
	StudentsYear* sty=this->yearsList[this->searchYear(yearName)];
	assert(sty);

	std::stable_sort(sty->groupsList.begin(), sty->groupsList.end(), groupsAscending);

	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;
}

bool Rules::addSubgroup(const QString& yearName, const QString& groupName, StudentsSubgroup* subgroup)
{
	StudentsYear* sty=this->yearsList.at(this->searchYear(yearName));
	assert(sty);
	StudentsGroup* stg=sty->groupsList.at(this->searchGroup(yearName, groupName));
	assert(stg);

	for(int i=0; i<stg->subgroupsList.size(); i++){
		StudentsSubgroup* sts=stg->subgroupsList[i];
		if(sts->name==subgroup->name)
			return false;
	}
	
	stg->subgroupsList << subgroup; //append
	
	if(!permanentStudentsHash.contains(subgroup->name))
		permanentStudentsHash.insert(subgroup->name, subgroup);

	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	return true;
}

bool Rules::addSubgroupFast(StudentsYear* year, StudentsGroup* group, StudentsSubgroup* subgroup)
{
	Q_UNUSED(year);

	group->subgroupsList << subgroup; //append

	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	return true;
}

bool Rules::removeSubgroup(const QString& yearName, const QString& groupName, const QString& subgroupName)
{
	StudentsYear* yearPointer=nullptr;
	for(StudentsYear* ty : qAsConst(this->yearsList)){
		if(ty->name==yearName){
			yearPointer=ty;
			break;
		}
	}

	assert(yearPointer!=nullptr);
	
	StudentsGroup* groupPointer=nullptr;
	for(StudentsGroup* tg : qAsConst(yearPointer->groupsList)){
		if(tg->name==groupName){
			groupPointer=tg;
			break;
		}
	}
	
	assert(groupPointer!=nullptr);
	
	StudentsSubgroup* subgroupPointer=nullptr;
	for(StudentsSubgroup* ts : qAsConst(groupPointer->subgroupsList)){
		if(ts->name==subgroupName){
			subgroupPointer=ts;
			break;
		}
	}
	
	assert(subgroupPointer!=nullptr);
	
	//pointers
	QSet<StudentsSet*> toBeRemoved;
	toBeRemoved.insert(subgroupPointer);
	for(StudentsYear* year : qAsConst(yearsList))
		for(StudentsGroup* group : qAsConst(year->groupsList))
			for(StudentsSubgroup* subgroup : qAsConst(group->subgroupsList))
				if(subgroup->name==subgroupName && (year->name!=yearName || group->name!=groupName))
					toBeRemoved.remove(subgroupPointer);
					
	updateActivitiesWhenRemovingStudents(toBeRemoved, false);
	
	for(int i=0; i<groupPointer->subgroupsList.count(); i++)
		if(groupPointer->subgroupsList.at(i)==subgroupPointer){
			groupPointer->subgroupsList.removeAt(i);
			break;
		}
	
	for(StudentsSet* studentsSet : qAsConst(toBeRemoved)){
		assert(permanentStudentsHash.contains(studentsSet->name));
		permanentStudentsHash.remove(studentsSet->name);
	
		delete studentsSet;
	}
		
	if(toBeRemoved.count()>0)
		updateConstraintsAfterRemoval();
	
	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	return true;
}

bool Rules::purgeSubgroup(const QString& subgroupName)
{
	StudentsSubgroup* subgroupPointer=nullptr;
	for(StudentsYear* year : qAsConst(yearsList))
		for(StudentsGroup* group : qAsConst(year->groupsList)){
			int j=-1;
			for(int i=0; i<group->subgroupsList.count(); i++){
				if(group->subgroupsList.at(i)->name==subgroupName){
					j=i;
					if(subgroupPointer==nullptr)
						subgroupPointer=group->subgroupsList.at(i);
					else
						assert(subgroupPointer==group->subgroupsList.at(i));
					break;
				}
			}
			if(j>=0)
				group->subgroupsList.removeAt(j);
		}

	assert(subgroupPointer!=nullptr);
	
	//pointers
	QSet<StudentsSet*> toBeRemoved;
	toBeRemoved.insert(subgroupPointer);
	
	updateActivitiesWhenRemovingStudents(toBeRemoved, false);
	
	for(StudentsSet* studentsSet : qAsConst(toBeRemoved)){
		assert(permanentStudentsHash.contains(studentsSet->name));
		permanentStudentsHash.remove(studentsSet->name);
	
		delete studentsSet;
	}
	
	if(toBeRemoved.count()>0)
		updateConstraintsAfterRemoval();
	
	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	return true;
}

int Rules::searchSubgroup(const QString& yearName, const QString& groupName, const QString& subgroupName)
{
	StudentsYear* sty=nullptr;
	for(StudentsYear* ty : qAsConst(yearsList))
		if(ty->name==yearName){
			sty=ty;
			break;
		}
	assert(sty!=nullptr);

	StudentsGroup* stg=nullptr;
	for(StudentsGroup* tg : qAsConst(sty->groupsList))
		if(tg->name==groupName){
			stg=tg;
			break;
		}
	assert(stg!=nullptr);

	for(int i=0; i<stg->subgroupsList.size(); i++)
		if(stg->subgroupsList[i]->name==subgroupName)
			return i;
	
	return -1;
}

int Rules::searchAugmentedSubgroup(const QString& yearName, const QString& groupName, const QString& subgroupName)
{
	StudentsYear* sty=nullptr;
	for(StudentsYear* ty : qAsConst(augmentedYearsList))
		if(ty->name==yearName){
			sty=ty;
			break;
		}
	assert(sty!=nullptr);

	StudentsGroup* stg=nullptr;
	for(StudentsGroup* tg : qAsConst(sty->groupsList))
		if(tg->name==groupName){
			stg=tg;
			break;
		}
	assert(stg!=nullptr);

	for(int i=0; i<stg->subgroupsList.size(); i++)
		if(stg->subgroupsList[i]->name==subgroupName)
			return i;
	
	return -1;
}

void Rules::sortSubgroupsAlphabetically(const QString& yearName, const QString& groupName)
{
	StudentsYear* sty=this->yearsList.at(this->searchYear(yearName));
	assert(sty);
	StudentsGroup* stg=sty->groupsList.at(this->searchGroup(yearName, groupName));
	assert(stg);

	std::stable_sort(stg->subgroupsList.begin(), stg->subgroupsList.end(), subgroupsAscending);
	
	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;
}

bool Rules::addSimpleActivityFast(
	QWidget* parent,
	int _id,
	int _activityGroupId,
	const QStringList& _teachersNames,
	const QString& _subjectName,
	const QStringList& _activityTagsNames,
	const QStringList& _studentsNames,
	int _duration,
	int _totalDuration,
	bool _active,
	bool _computeNTotalStudents,
	int _nTotalStudents,
	int _computedNumberOfStudents)
{
	//check for duplicates - idea and code by Volker Dirr
	int t=QStringList(_teachersNames).removeDuplicates();
	if(t>0)
		RulesReconcilableMessage::warning(parent, tr("FET warning"), tr("Activity with Id=%1 contains %2 duplicate teachers - please correct that")
		 .arg(_id).arg(t));

	t=QStringList(_studentsNames).removeDuplicates();
	if(t>0)
		RulesReconcilableMessage::warning(parent, tr("FET warning"), tr("Activity with Id=%1 contains %2 duplicate students sets - please correct that")
		 .arg(_id).arg(t));

	t=QStringList(_activityTagsNames).removeDuplicates();
	if(t>0)
		RulesReconcilableMessage::warning(parent, tr("FET warning"), tr("Activity with Id=%1 contains %2 duplicate activity tags - please correct that")
		 .arg(_id).arg(t));

	Activity *act=new Activity(*this, _id, _activityGroupId, _teachersNames, _subjectName, _activityTagsNames,
		_studentsNames, _duration, _totalDuration, _active, _computeNTotalStudents, _nTotalStudents, _computedNumberOfStudents);

	this->activitiesList << act; //append

	assert(!activitiesPointerHash.contains(act->id));
	activitiesPointerHash.insert(act->id, act);

	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	return true;
}

bool Rules::addSplitActivityFast(
	QWidget* parent,
	int _firstActivityId,
	int _activityGroupId,
	const QStringList& _teachersNames,
	const QString& _subjectName,
	const QStringList& _activityTagsNames,
	const QStringList& _studentsNames,
	int _nSplits,
	int _totalDuration,
	int _durations[],
	bool _active[],
	int _minDayDistance,
	double _weightPercentage,
	bool _consecutiveIfSameDay,
	bool _computeNTotalStudents,
	int _nTotalStudents,
	int _computedNumberOfStudents)
{
	//check for duplicates - idea and code by Volker Dirr
	int t=QStringList(_teachersNames).removeDuplicates();
	if(t>0)
		RulesReconcilableMessage::warning(parent, tr("FET warning"), tr("Activities with group_Id=%1 contain %2 duplicate teachers - please correct that")
		 .arg(_activityGroupId).arg(t));

	t=QStringList(_studentsNames).removeDuplicates();
	if(t>0)
		RulesReconcilableMessage::warning(parent, tr("FET warning"), tr("Activities with group_Id=%1 contain %2 duplicate students sets - please correct that")
		 .arg(_activityGroupId).arg(t));

	t=QStringList(_activityTagsNames).removeDuplicates();
	if(t>0)
		RulesReconcilableMessage::warning(parent, tr("FET warning"), tr("Activities with group_Id=%1 contain %2 duplicate activity tags - please correct that")
		 .arg(_activityGroupId).arg(t));

	assert(_firstActivityId==_activityGroupId);

	QList<int> acts;

	acts.clear();
	for(int i=0; i<_nSplits; i++){
		Activity *act=new Activity(*this, _firstActivityId+i, _activityGroupId,
		 _teachersNames, _subjectName, _activityTagsNames, _studentsNames,
		 _durations[i], _totalDuration, _active[i], _computeNTotalStudents, _nTotalStudents, _computedNumberOfStudents);

		this->activitiesList << act; //append

		assert(!activitiesPointerHash.contains(act->id));
		activitiesPointerHash.insert(act->id, act);

		acts.append(_firstActivityId+i);
	}

	if(_minDayDistance>0){
		TimeConstraint *constr=new ConstraintMinDaysBetweenActivities(_weightPercentage, _consecutiveIfSameDay, _nSplits, acts, _minDayDistance);
		bool tmp=this->addTimeConstraint(constr);
		assert(tmp);
	}

	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	return true;
}

/*void Rules::removeActivity(int _id)
{
	QList<int> tmpList;
	tmpList.append(_id);
	removeActivities(tmpList, true);
}*/

void Rules::removeActivity(int _id, int _activityGroupId)
{
	QList<int> tmpList;
	for(Activity* act : qAsConst(activitiesList))
		if(_id==act->id || (_activityGroupId>0 && _activityGroupId==act->activityGroupId))
			tmpList.append(act->id);
	removeActivities(tmpList, true);
}

void Rules::removeActivities(const QList<int>& _idsList, bool updateConstraints)
{
	if(_idsList.count()==0)
		return;

#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
	QSet<int> _removedIdsSet=QSet<int>(_idsList.begin(), _idsList.end());
#else
	QSet<int> _removedIdsSet=_idsList.toSet();
#endif
	
	QSet<int> _groupIdsSet;
	for(Activity* act : qAsConst(activitiesList))
		if(act->activityGroupId>0 && _removedIdsSet.contains(act->id))
			_groupIdsSet.insert(act->activityGroupId);
	for(Activity* act : qAsConst(activitiesList))
		if(act->activityGroupId>0 && _groupIdsSet.contains(act->activityGroupId))
			_removedIdsSet.insert(act->id);
	
	ActivitiesList newActivitiesList;
	ActivitiesList toBeRemoved;
	
	for(int i=0; i<activitiesList.count(); i++){
		int id=activitiesList.at(i)->id;
		if(!_removedIdsSet.contains(id)){
			newActivitiesList.append(activitiesList[i]);
		}
		else{
			toBeRemoved.append(activitiesList[i]);

			assert(activitiesPointerHash.contains(id));
			activitiesPointerHash.remove(id);
			
			if(mdbaHash.contains(id)){
				int t=mdbaHash.remove(id);
				assert(t==1);
			}
		}
	}

	for(Activity* act : qAsConst(toBeRemoved))
		delete act;
	toBeRemoved.clear();
	//while(!toBeRemoved.isEmpty())
	//	delete toBeRemoved.takeFirst();
		
	activitiesList=newActivitiesList;
	
	updateGroupActivitiesInInitialOrderAfterRemoval();
	if(updateConstraints)
		updateConstraintsAfterRemoval();
	
	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;
}

void Rules::modifyActivity(
	int _id,
	int _activityGroupId,
	const QStringList& _teachersNames,
	const QString& _subjectName,
	const QStringList& _activityTagsNames,
	const QStringList& _studentsNames,
	//int _nTotalStudents,
	int _nSplits,
	int _totalDuration,
	int _durations[],
	//int _parities[],
	bool _active[],
	bool _computeNTotalStudents,
	int _nTotalStudents)
{
	int i=0;
	for(int j=0; j<this->activitiesList.size(); j++){
		Activity* act=this->activitiesList[j];
		if((_activityGroupId==0 && act->id==_id) || (_activityGroupId!=0 && act->activityGroupId==_activityGroupId)){
			act->teachersNames=_teachersNames;
			act->subjectName=_subjectName;
			act->activityTagsNames=_activityTagsNames;
			act->studentsNames=_studentsNames;
			act->duration=_durations[i];
			//act->parity=_parities[i];
			act->active=_active[i];
			act->totalDuration=_totalDuration;
			act->computeNTotalStudents=_computeNTotalStudents;
			act->nTotalStudents=_nTotalStudents;
			i++;
		}
	}
	
	assert(i==_nSplits);
	
	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;
}

void Rules::modifySubactivity(
	int _id,
	int _activityGroupId,
	const QStringList& _teachersNames,
	const QString& _subjectName,
	const QStringList& _activityTagsNames,
	const QStringList& _studentsNames,
	int _duration,
	bool _active,
	bool _computeNTotalStudents,
	int _nTotalStudents)
{
	QList<Activity*> actsList;
	Activity* crtAct=nullptr;
	
	for(Activity* act : qAsConst(this->activitiesList)){
		if(act->id==_id && act->activityGroupId==_activityGroupId){
			crtAct=act;
			//actsList.append(act);
		}
		else if(act->activityGroupId!=0 && _activityGroupId!=0 && act->activityGroupId==_activityGroupId){
			actsList.append(act);
		}
	}
	
	assert(crtAct!=nullptr);
	
	int td=0;
	for(Activity* act : qAsConst(actsList))
		td+=act->duration;
	td+=_duration; //crtAct->duration;
	for(Activity* act : qAsConst(actsList))
		act->totalDuration=td;

	crtAct->teachersNames=_teachersNames;
	crtAct->subjectName=_subjectName;
	crtAct->activityTagsNames=_activityTagsNames;
	crtAct->studentsNames=_studentsNames;
	crtAct->duration=_duration;
	crtAct->totalDuration=td;
	crtAct->active=_active;
	crtAct->computeNTotalStudents=_computeNTotalStudents;
	crtAct->nTotalStudents=_nTotalStudents;
	
	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;
}

bool Rules::addRoom(Room* rm)
{
	if(this->searchRoom(rm->name) >= 0)
		return false;
	this->roomsList << rm; //append
	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	return true;
}

bool Rules::addRoomFast(Room* rm)
{
	this->roomsList << rm; //append
	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	return true;
}

int Rules::searchRoom(const QString& roomName)
{
	for(int i=0; i<this->roomsList.size(); i++)
		if(this->roomsList[i]->name==roomName)
			return i;
	
	return -1;
}

bool Rules::removeRoom(const QString& roomName)
{
	int i=this->searchRoom(roomName);
	if(i<0)
		return false;

	Room* searchedRoom=this->roomsList[i];
	assert(searchedRoom->name==roomName);
	
	//the real room to be removed may be found in some virtual rooms' sets
	if(searchedRoom->isVirtual==false){
		for(Room* r : qAsConst(roomsList)){
			if(r->isVirtual==true){
				for(int j=0; j<r->realRoomsSetsList.count(); j++){
					QStringList& sl=r->realRoomsSetsList[j];
					if(sl.contains(roomName)){
						int t=sl.removeAll(roomName);
						assert(t==1);
					}
				}
			}
		}
	}
	
	for(SpaceConstraint* ctr : qAsConst(spaceConstraintsList)){
		if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
			ConstraintActivityPreferredRoom* c=(ConstraintActivityPreferredRoom*)ctr;
			
			if(c->preferredRealRoomsNames.contains(roomName))
				c->preferredRealRoomsNames.removeAll(roomName);
		}
	}

	delete this->roomsList[i];
	this->roomsList.removeAt(i);

	updateConstraintsAfterRemoval();

	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	return true;
}

bool Rules::modifyRoom(const QString& initialRoomName, const QString& finalRoomName, const QString& building, int capacity)
{
	int i=this->searchRoom(initialRoomName);
	if(i<0)
		return false;

	Room* searchedRoom=this->roomsList[i];
	assert(searchedRoom->name==initialRoomName);
	
	//the real room to be renamed may be found in some virtual rooms' sets
	if(initialRoomName!=finalRoomName){
		if(searchedRoom->isVirtual==false){
			for(Room* r : qAsConst(roomsList)){
				if(r->isVirtual==true){
					for(int j=0; j<r->realRoomsSetsList.count(); j++){
						QStringList& sl=r->realRoomsSetsList[j];
						for(int k=0; k<sl.count(); k++){
							if(sl[k]==initialRoomName){
								sl[k]=finalRoomName;
							}
						}
					}
				}
			}
		}
	}

	for(SpaceConstraint* ctr : qAsConst(spaceConstraintsList)){
		if(ctr->type==CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES){
			ConstraintRoomNotAvailableTimes* crna=(ConstraintRoomNotAvailableTimes*)ctr;
			if(crna->room==initialRoomName)
				crna->room=finalRoomName;
		}
		else if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
			ConstraintActivityPreferredRoom* c=(ConstraintActivityPreferredRoom*)ctr;
			if(c->roomName==initialRoomName)
				c->roomName=finalRoomName;
				
			for(int i=0; i<c->preferredRealRoomsNames.count(); i++)
				if(c->preferredRealRoomsNames.at(i)==initialRoomName)
					c->preferredRealRoomsNames[i]=finalRoomName;
		}
		else if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOMS){
			ConstraintActivityPreferredRooms* c=(ConstraintActivityPreferredRooms*)ctr;
			int t=0;
			for(QStringList::iterator it=c->roomsNames.begin(); it!=c->roomsNames.end(); it++){
				if((*it)==initialRoomName){
					*it=finalRoomName;
					t++;
				}
			}
			assert(t<=1);
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_HOME_ROOM){
			ConstraintStudentsSetHomeRoom* c=(ConstraintStudentsSetHomeRoom*)ctr;
			if(c->roomName==initialRoomName)
				c->roomName=finalRoomName;
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_HOME_ROOMS){
			ConstraintStudentsSetHomeRooms* c=(ConstraintStudentsSetHomeRooms*)ctr;
			int t=0;
			for(QStringList::iterator it=c->roomsNames.begin(); it!=c->roomsNames.end(); it++){
				if((*it)==initialRoomName){
					*it=finalRoomName;
					t++;
				}
			}
			assert(t<=1);
		}
		else if(ctr->type==CONSTRAINT_TEACHER_HOME_ROOM){
			ConstraintTeacherHomeRoom* c=(ConstraintTeacherHomeRoom*)ctr;
			if(c->roomName==initialRoomName)
				c->roomName=finalRoomName;
		}
		else if(ctr->type==CONSTRAINT_TEACHER_HOME_ROOMS){
			ConstraintTeacherHomeRooms* c=(ConstraintTeacherHomeRooms*)ctr;
			int t=0;
			for(QStringList::iterator it=c->roomsNames.begin(); it!=c->roomsNames.end(); it++){
				if((*it)==initialRoomName){
					*it=finalRoomName;
					t++;
				}
			}
			assert(t<=1);
		}
		else if(ctr->type==CONSTRAINT_SUBJECT_PREFERRED_ROOM){
			ConstraintSubjectPreferredRoom* c=(ConstraintSubjectPreferredRoom*)ctr;
			if(c->roomName==initialRoomName)
				c->roomName=finalRoomName;
		}
		else if(ctr->type==CONSTRAINT_SUBJECT_PREFERRED_ROOMS){
			ConstraintSubjectPreferredRooms* c=(ConstraintSubjectPreferredRooms*)ctr;
			int t=0;
			for(QStringList::iterator it=c->roomsNames.begin(); it!=c->roomsNames.end(); it++){
				if((*it)==initialRoomName){
					*it=finalRoomName;
					t++;
				}
			}
			assert(t<=1);
		}
		else if(ctr->type==CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM){
			ConstraintSubjectActivityTagPreferredRoom* c=(ConstraintSubjectActivityTagPreferredRoom*)ctr;
			if(c->roomName==initialRoomName)
				c->roomName=finalRoomName;
		}
		else if(ctr->type==CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS){
			ConstraintSubjectActivityTagPreferredRooms* c=(ConstraintSubjectActivityTagPreferredRooms*)ctr;
			int t=0;
			for(QStringList::iterator it=c->roomsNames.begin(); it!=c->roomsNames.end(); it++){
				if((*it)==initialRoomName){
					*it=finalRoomName;
					t++;
				}
			}
			assert(t<=1);
		}
		else if(ctr->type==CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM){
			ConstraintActivityTagPreferredRoom* c=(ConstraintActivityTagPreferredRoom*)ctr;
			if(c->roomName==initialRoomName)
				c->roomName=finalRoomName;
		}
		else if(ctr->type==CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS){
			ConstraintActivityTagPreferredRooms* c=(ConstraintActivityTagPreferredRooms*)ctr;
			int t=0;
			for(QStringList::iterator it=c->roomsNames.begin(); it!=c->roomsNames.end(); it++){
				if((*it)==initialRoomName){
					*it=finalRoomName;
					t++;
				}
			}
			assert(t<=1);
		}
	}

	searchedRoom->name=finalRoomName;
	searchedRoom->building=building;
	searchedRoom->capacity=capacity;

	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	return true;
}

void Rules::sortRoomsAlphabetically()
{
	std::stable_sort(this->roomsList.begin(), this->roomsList.end(), roomsAscending);

	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;
}

bool Rules::addBuilding(Building* bu)
{
	if(this->searchBuilding(bu->name) >= 0)
		return false;
	this->buildingsList << bu; //append
	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	return true;
}

bool Rules::addBuildingFast(Building* bu)
{
	this->buildingsList << bu; //append
	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	return true;
}

int Rules::searchBuilding(const QString& buildingName)
{
	for(int i=0; i<this->buildingsList.size(); i++)
		if(this->buildingsList[i]->name==buildingName)
			return i;
	
	return -1;
}

bool Rules::removeBuilding(const QString& buildingName)
{
	for(Room* rm : qAsConst(this->roomsList))
		if(rm->building==buildingName)
			rm->building="";

	int i=this->searchBuilding(buildingName);
	if(i<0)
		return false;

	Building* searchedBuilding=this->buildingsList[i];
	assert(searchedBuilding->name==buildingName);

	delete this->buildingsList[i];
	this->buildingsList.removeAt(i);
	
	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	return true;
}

bool Rules::modifyBuilding(const QString& initialBuildingName, const QString& finalBuildingName)
{
	for(Room* rm : qAsConst(roomsList))
		if(rm->building==initialBuildingName)
			rm->building=finalBuildingName;

	int i=this->searchBuilding(initialBuildingName);
	if(i<0)
		return false;

	Building* searchedBuilding=this->buildingsList[i];
	assert(searchedBuilding->name==initialBuildingName);
	searchedBuilding->name=finalBuildingName;

	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	return true;
}

void Rules::sortBuildingsAlphabetically()
{
	std::stable_sort(this->buildingsList.begin(), this->buildingsList.end(), buildingsAscending);

	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;
}

bool Rules::addTimeConstraint(TimeConstraint* ctr)
{
	bool ok=true;

	//TODO: improve this

	//check if this constraint is already added, for ConstraintActivityPreferredStartingTime
	if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME){
		ConstraintActivityPreferredStartingTime* c=(ConstraintActivityPreferredStartingTime*) ctr;
		QSet<ConstraintActivityPreferredStartingTime*> cs=apstHash.value(c->activityId, QSet<ConstraintActivityPreferredStartingTime*>());
		for(ConstraintActivityPreferredStartingTime* oldc : qAsConst(cs)){
			if((*oldc)==(*c)){
				ok=false;
				break;
			}
		}

		/*int i;
		for(i=0; i<this->timeConstraintsList.size(); i++){
			TimeConstraint* ctr2=this->timeConstraintsList[i];
			if(ctr2->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME)
				if(
				 *((ConstraintActivityPreferredStartingTime*)ctr2)
				 ==
				 *((ConstraintActivityPreferredStartingTime*)ctr)
				)
					break;
		}
				
		if(i<this->timeConstraintsList.size())
			ok=false;*/
	}

	//check if this constraint is already added, for ConstraintMinDaysBetweenActivities
	else if(ctr->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES){
		ConstraintMinDaysBetweenActivities* c=(ConstraintMinDaysBetweenActivities*) ctr;
		for(int aid : qAsConst(c->activitiesId)){
			QSet<ConstraintMinDaysBetweenActivities*> cs=mdbaHash.value(aid, QSet<ConstraintMinDaysBetweenActivities*>());
			for(ConstraintMinDaysBetweenActivities* oldc : qAsConst(cs)){
				if((*oldc)==(*c)){
					ok=false;
					break;
				}
			}
			if(!ok)
				break;
		}

		/*int i;
		for(i=0; i<this->timeConstraintsList.size(); i++){
			TimeConstraint* ctr2=this->timeConstraintsList[i];
			if(ctr2->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES)
				if(
				 *((ConstraintMinDaysBetweenActivities*)ctr2)
				 ==
				 *((ConstraintMinDaysBetweenActivities*)ctr)
				 )
					break;
		}

		if(i<this->timeConstraintsList.size())
			ok=false;*/
	}
	
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE_TIMES){
		ConstraintStudentsSetNotAvailableTimes* c=(ConstraintStudentsSetNotAvailableTimes*) ctr;
		QSet<ConstraintStudentsSetNotAvailableTimes*> cs=ssnatHash.value(c->students, QSet<ConstraintStudentsSetNotAvailableTimes*>());
		for(ConstraintStudentsSetNotAvailableTimes* oldc : qAsConst(cs)){
			if(oldc->students==c->students){
				ok=false;
				break;
			}
		}

		/*int i;
		ConstraintStudentsSetNotAvailableTimes* ssna=(ConstraintStudentsSetNotAvailableTimes*)ctr;
		for(i=0; i<this->timeConstraintsList.size(); i++){
			TimeConstraint* ctr2=this->timeConstraintsList[i];
			if(ctr2->type==CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE_TIMES) {
				ConstraintStudentsSetNotAvailableTimes* ssna2=(ConstraintStudentsSetNotAvailableTimes*)ctr2;
				if(ssna->students==ssna2->students)
					break;
			}
		}
				
		if(i<this->timeConstraintsList.size())
			ok=false;*/
	}
	
	else if(ctr->type==CONSTRAINT_TEACHER_NOT_AVAILABLE_TIMES){
		ConstraintTeacherNotAvailableTimes* c=(ConstraintTeacherNotAvailableTimes*) ctr;
		QSet<ConstraintTeacherNotAvailableTimes*> cs=tnatHash.value(c->teacher, QSet<ConstraintTeacherNotAvailableTimes*>());
		for(ConstraintTeacherNotAvailableTimes* oldc : qAsConst(cs)){
			if(oldc->teacher==c->teacher){
				ok=false;
				break;
			}
		}

		/*int i;
		ConstraintTeacherNotAvailableTimes* tna=(ConstraintTeacherNotAvailableTimes*)ctr;
		for(i=0; i<this->timeConstraintsList.size(); i++){
			TimeConstraint* ctr2=this->timeConstraintsList[i];
			if(ctr2->type==CONSTRAINT_TEACHER_NOT_AVAILABLE_TIMES) {
				ConstraintTeacherNotAvailableTimes* tna2=(ConstraintTeacherNotAvailableTimes*)ctr2;
				if(tna->teacher==tna2->teacher)
					break;
			}
		}
				
		if(i<this->timeConstraintsList.size())
			ok=false;*/
	}
	
	else if(ctr->type==CONSTRAINT_BREAK_TIMES){
		//ConstraintBreakTimes* c=(ConstraintBreakTimes*) ctr;
		QSet<ConstraintBreakTimes*> cs=btSet;
		if(cs.count()>0)
			ok=false;
		/*int i;
		for(i=0; i<this->timeConstraintsList.size(); i++){
			TimeConstraint* ctr2=this->timeConstraintsList[i];
			if(ctr2->type==CONSTRAINT_BREAK_TIMES)
				break;
		}
				
		if(i<this->timeConstraintsList.size())
			ok=false;*/
	}
	
	else if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_TIME){
		//ConstraintBasicCompulsoryTime* c=(ConstraintBasicCompulsoryTime*) ctr;
		QSet<ConstraintBasicCompulsoryTime*> cs=bctSet;
		if(cs.count()>0)
			ok=false;
		/*int i;
		for(i=0; i<this->timeConstraintsList.size(); i++){
			TimeConstraint* ctr2=this->timeConstraintsList[i];
			if(ctr2->type==CONSTRAINT_BASIC_COMPULSORY_TIME)
				break;
		}
				
		if(i<this->timeConstraintsList.size())
			ok=false;*/
	}
	
	if(ok){
		this->timeConstraintsList << ctr; //append
		
		if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME){
			ConstraintActivityPreferredStartingTime* c=(ConstraintActivityPreferredStartingTime*) ctr;
			QSet<ConstraintActivityPreferredStartingTime*> cs=apstHash.value(c->activityId, QSet<ConstraintActivityPreferredStartingTime*>());
			assert(!cs.contains(c));
			cs.insert(c);
			apstHash.insert(c->activityId, cs);
		}

		else if(ctr->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES){
			ConstraintMinDaysBetweenActivities* c=(ConstraintMinDaysBetweenActivities*) ctr;
			for(int aid : qAsConst(c->activitiesId)){
				QSet<ConstraintMinDaysBetweenActivities*> cs=mdbaHash.value(aid, QSet<ConstraintMinDaysBetweenActivities*>());
				assert(!cs.contains(c));
				cs.insert(c);
				mdbaHash.insert(aid, cs);
			}
		}

		else if(ctr->type==CONSTRAINT_TEACHER_NOT_AVAILABLE_TIMES){
			ConstraintTeacherNotAvailableTimes* c=(ConstraintTeacherNotAvailableTimes*) ctr;
			QSet<ConstraintTeacherNotAvailableTimes*> cs=tnatHash.value(c->teacher, QSet<ConstraintTeacherNotAvailableTimes*>());
			assert(!cs.contains(c));
			cs.insert(c);
			tnatHash.insert(c->teacher, cs);
		}

		else if(ctr->type==CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE_TIMES){
			ConstraintStudentsSetNotAvailableTimes* c=(ConstraintStudentsSetNotAvailableTimes*) ctr;
			QSet<ConstraintStudentsSetNotAvailableTimes*> cs=ssnatHash.value(c->students, QSet<ConstraintStudentsSetNotAvailableTimes*>());
			assert(!cs.contains(c));
			cs.insert(c);
			ssnatHash.insert(c->students, cs);
		}
		else if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_TIME){
			ConstraintBasicCompulsoryTime* c=(ConstraintBasicCompulsoryTime*) ctr;
			QSet<ConstraintBasicCompulsoryTime*> &cs=bctSet;
			assert(!cs.contains(c));
			cs.insert(c);
		}
		else if(ctr->type==CONSTRAINT_BREAK_TIMES){
			ConstraintBreakTimes* c=(ConstraintBreakTimes*) ctr;
			QSet<ConstraintBreakTimes*> &cs=btSet;
			assert(!cs.contains(c));
			cs.insert(c);
		}

		this->internalStructureComputed=false;
		setRulesModifiedAndOtherThings(this);
		return true;
	}
	else
		return false;
}

bool Rules::removeTimeConstraint(TimeConstraint* ctr)
{
	for(int i=0; i<this->timeConstraintsList.size(); i++){
		if(this->timeConstraintsList[i]==ctr){
			if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME){
				ConstraintActivityPreferredStartingTime* c=(ConstraintActivityPreferredStartingTime*) ctr;
				QSet<ConstraintActivityPreferredStartingTime*> cs=apstHash.value(c->activityId, QSet<ConstraintActivityPreferredStartingTime*>());
				assert(cs.contains(c));
				cs.remove(c);
				if(!cs.isEmpty()){
					apstHash.insert(c->activityId, cs);
				}
				else{
					int t=apstHash.remove(c->activityId);
					assert(t==1);
				}
			}

			else if(ctr->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES){
				ConstraintMinDaysBetweenActivities* c=(ConstraintMinDaysBetweenActivities*) ctr;
				for(int aid : qAsConst(c->activitiesId)){
					QSet<ConstraintMinDaysBetweenActivities*> cs=mdbaHash.value(aid, QSet<ConstraintMinDaysBetweenActivities*>());
					assert(cs.contains(c));
					cs.remove(c);
					if(!cs.isEmpty()){
						mdbaHash.insert(aid, cs);
					}
					else{
						int t=mdbaHash.remove(aid);
						assert(t==1);
					}
				}
			}

			else if(ctr->type==CONSTRAINT_TEACHER_NOT_AVAILABLE_TIMES){
				ConstraintTeacherNotAvailableTimes* c=(ConstraintTeacherNotAvailableTimes*) ctr;
				QSet<ConstraintTeacherNotAvailableTimes*> cs=tnatHash.value(c->teacher, QSet<ConstraintTeacherNotAvailableTimes*>());
				assert(cs.contains(c));
				cs.remove(c);
				if(!cs.isEmpty()){
					assert(0);
					tnatHash.insert(c->teacher, cs);
				}
				else{
					int t=tnatHash.remove(c->teacher);
					assert(t==1);
				}
			}

			else if(ctr->type==CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE_TIMES){
				ConstraintStudentsSetNotAvailableTimes* c=(ConstraintStudentsSetNotAvailableTimes*) ctr;
				QSet<ConstraintStudentsSetNotAvailableTimes*> cs=ssnatHash.value(c->students, QSet<ConstraintStudentsSetNotAvailableTimes*>());
				assert(cs.contains(c));
				cs.remove(c);
				if(!cs.isEmpty()){
					assert(0);
					ssnatHash.insert(c->students, cs);
				}
				else{
					int t=ssnatHash.remove(c->students);
					assert(t==1);
				}
			}
			else if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_TIME){
				ConstraintBasicCompulsoryTime* c=(ConstraintBasicCompulsoryTime*) ctr;
				QSet<ConstraintBasicCompulsoryTime*> &cs=bctSet;
				assert(cs.contains(c));
				cs.remove(c);
			}
			else if(ctr->type==CONSTRAINT_BREAK_TIMES){
				ConstraintBreakTimes* c=(ConstraintBreakTimes*) ctr;
				QSet<ConstraintBreakTimes*> &cs=btSet;
				assert(cs.contains(c));
				cs.remove(c);
			}
	
			delete ctr;
			this->timeConstraintsList.removeAt(i);
			this->internalStructureComputed=false;
			setRulesModifiedAndOtherThings(this);

			return true;
		}
	}

	return false;
}

bool Rules::removeTimeConstraints(QList<TimeConstraint*> _tcl)
{
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
	QSet<TimeConstraint*> _tcs=QSet<TimeConstraint*>(_tcl.begin(), _tcl.end());
#else
	QSet<TimeConstraint*> _tcs=_tcl.toSet();
#endif
	QList<TimeConstraint*> remaining;

	for(int i=0; i<this->timeConstraintsList.size(); i++){
		TimeConstraint* ctr=timeConstraintsList[i];
		if(_tcs.contains(ctr)){
			if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME){
				ConstraintActivityPreferredStartingTime* c=(ConstraintActivityPreferredStartingTime*) ctr;
				QSet<ConstraintActivityPreferredStartingTime*> cs=apstHash.value(c->activityId, QSet<ConstraintActivityPreferredStartingTime*>());
				assert(cs.contains(c));
				cs.remove(c);
				if(!cs.isEmpty()){
					apstHash.insert(c->activityId, cs);
				}
				else{
					int t=apstHash.remove(c->activityId);
					assert(t==1);
				}
			}

			else if(ctr->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES){
				ConstraintMinDaysBetweenActivities* c=(ConstraintMinDaysBetweenActivities*) ctr;
				for(int aid : qAsConst(c->activitiesId)){
					QSet<ConstraintMinDaysBetweenActivities*> cs=mdbaHash.value(aid, QSet<ConstraintMinDaysBetweenActivities*>());
					assert(cs.contains(c));
					cs.remove(c);
					if(!cs.isEmpty()){
						mdbaHash.insert(aid, cs);
					}
					else{
						int t=mdbaHash.remove(aid);
						assert(t==1);
					}
				}
			}

			else if(ctr->type==CONSTRAINT_TEACHER_NOT_AVAILABLE_TIMES){
				ConstraintTeacherNotAvailableTimes* c=(ConstraintTeacherNotAvailableTimes*) ctr;
				QSet<ConstraintTeacherNotAvailableTimes*> cs=tnatHash.value(c->teacher, QSet<ConstraintTeacherNotAvailableTimes*>());
				assert(cs.contains(c));
				cs.remove(c);
				if(!cs.isEmpty()){
					assert(0);
					tnatHash.insert(c->teacher, cs);
				}
				else{
					int t=tnatHash.remove(c->teacher);
					assert(t==1);
				}
			}

			else if(ctr->type==CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE_TIMES){
				ConstraintStudentsSetNotAvailableTimes* c=(ConstraintStudentsSetNotAvailableTimes*) ctr;
				QSet<ConstraintStudentsSetNotAvailableTimes*> cs=ssnatHash.value(c->students, QSet<ConstraintStudentsSetNotAvailableTimes*>());
				assert(cs.contains(c));
				cs.remove(c);
				if(!cs.isEmpty()){
					assert(0);
					ssnatHash.insert(c->students, cs);
				}
				else{
					int t=ssnatHash.remove(c->students);
					assert(t==1);
				}
			}
			else if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_TIME){
				ConstraintBasicCompulsoryTime* c=(ConstraintBasicCompulsoryTime*) ctr;
				QSet<ConstraintBasicCompulsoryTime*> &cs=bctSet;
				assert(cs.contains(c));
				cs.remove(c);
			}
			else if(ctr->type==CONSTRAINT_BREAK_TIMES){
				ConstraintBreakTimes* c=(ConstraintBreakTimes*) ctr;
				QSet<ConstraintBreakTimes*> &cs=btSet;
				assert(cs.contains(c));
				cs.remove(c);
			}
	
			//_tcs.remove(ctr);
			delete ctr;
		}
		else
			remaining.append(ctr);
	}
	
	bool ok;
	assert(timeConstraintsList.count()<=remaining.count()+_tcs.count());
	if(timeConstraintsList.count()==remaining.count()+_tcs.count())
		ok=true;
	else
		ok=false;

	if(remaining.count()!=timeConstraintsList.count()){
		timeConstraintsList=remaining;
	
		this->internalStructureComputed=false;
		setRulesModifiedAndOtherThings(this);
	}

	return ok;
}

bool Rules::addSpaceConstraint(SpaceConstraint* ctr)
{
	bool ok=true;

	//TODO: check if this constraint is already added...(if any possibility of duplicates)
	if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
		ConstraintActivityPreferredRoom* c=(ConstraintActivityPreferredRoom*) ctr;
		QSet<ConstraintActivityPreferredRoom*> cs=aprHash.value(c->activityId, QSet<ConstraintActivityPreferredRoom*>());
		for(ConstraintActivityPreferredRoom* oldc : qAsConst(cs)){
			if((*oldc)==(*c)){
				ok=false;
				break;
			}
		}

		/*int i;
		for(i=0; i<this->spaceConstraintsList.size(); i++){
			SpaceConstraint* ctr2=this->spaceConstraintsList[i];
			if(ctr2->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM)
				if(
				 *((ConstraintActivityPreferredRoom*)ctr2)
				 ==
				 *((ConstraintActivityPreferredRoom*)ctr)
				)
					break;
		}
		
		if(i<this->spaceConstraintsList.size())
			ok=false;*/
	}
/*	else if(ctr->type==CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES){
		int i;
		ConstraintRoomNotAvailableTimes* c=(ConstraintRoomNotAvailableTimes*)ctr;
		for(i=0; i<this->spaceConstraintsList.size(); i++){
			SpaceConstraint* ctr2=this->spaceConstraintsList[i];
			if(ctr2->type==CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES){
				ConstraintRoomNotAvailableTimes* c2=(ConstraintRoomNotAvailableTimes*)ctr2;				
				if(c->room==c2->room)
					break;
			}
		}
		
		if(i<this->spaceConstraintsList.size())
			ok=false;
	}*/
	else if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_SPACE){
		//ConstraintBasicCompulsorySpace* c=(ConstraintBasicCompulsorySpace*) ctr;
		QSet<ConstraintBasicCompulsorySpace*> cs=bcsSet;
		if(cs.count()>0)
			ok=false;
		/*int i;
		for(i=0; i<this->spaceConstraintsList.size(); i++){
			SpaceConstraint* ctr2=this->spaceConstraintsList[i];
			if(ctr2->type==CONSTRAINT_BASIC_COMPULSORY_SPACE)
				break;
		}
				
		if(i<this->spaceConstraintsList.size())
			ok=false;*/
	}

	if(ok){
		this->spaceConstraintsList << ctr; //append
		
		if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
			ConstraintActivityPreferredRoom* c=(ConstraintActivityPreferredRoom*) ctr;
			QSet<ConstraintActivityPreferredRoom*> cs=aprHash.value(c->activityId, QSet<ConstraintActivityPreferredRoom*>());
			assert(!cs.contains(c));
			cs.insert(c);
			aprHash.insert(c->activityId, cs);
		}
		else if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_SPACE){
			ConstraintBasicCompulsorySpace* c=(ConstraintBasicCompulsorySpace*) ctr;
			QSet<ConstraintBasicCompulsorySpace*> &cs=bcsSet;
			assert(!cs.contains(c));
			cs.insert(c);
		}
		
		this->internalStructureComputed=false;
		setRulesModifiedAndOtherThings(this);
		return true;
	}
	else
		return false;
}

bool Rules::removeSpaceConstraint(SpaceConstraint* ctr)
{
	for(int i=0; i<this->spaceConstraintsList.size(); i++){
		if(this->spaceConstraintsList[i]==ctr){
			if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
				ConstraintActivityPreferredRoom* c=(ConstraintActivityPreferredRoom*) ctr;
				QSet<ConstraintActivityPreferredRoom*> cs=aprHash.value(c->activityId, QSet<ConstraintActivityPreferredRoom*>());
				assert(cs.contains(c));
				cs.remove(c);
				aprHash.insert(c->activityId, cs);
			}

			else if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_SPACE){
				ConstraintBasicCompulsorySpace* c=(ConstraintBasicCompulsorySpace*) ctr;
				QSet<ConstraintBasicCompulsorySpace*> &cs=bcsSet;
				assert(cs.contains(c));
				cs.remove(c);
			}
	
			delete ctr;
			this->spaceConstraintsList.removeAt(i);
			this->internalStructureComputed=false;
			setRulesModifiedAndOtherThings(this);

			return true;
		}
	}

	return false;
}

bool Rules::removeSpaceConstraints(QList<SpaceConstraint*> _scl)
{
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
	QSet<SpaceConstraint*> _scs=QSet<SpaceConstraint*>(_scl.begin(), _scl.end());
#else
	QSet<SpaceConstraint*> _scs=_scl.toSet();
#endif
	QList<SpaceConstraint*> remaining;

	for(int i=0; i<this->spaceConstraintsList.size(); i++){
		SpaceConstraint* ctr=spaceConstraintsList[i];
		if(_scs.contains(ctr)){
			if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
				ConstraintActivityPreferredRoom* c=(ConstraintActivityPreferredRoom*) ctr;
				QSet<ConstraintActivityPreferredRoom*> cs=aprHash.value(c->activityId, QSet<ConstraintActivityPreferredRoom*>());
				assert(cs.contains(c));
				cs.remove(c);
				aprHash.insert(c->activityId, cs);
			}

			else if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_SPACE){
				ConstraintBasicCompulsorySpace* c=(ConstraintBasicCompulsorySpace*) ctr;
				QSet<ConstraintBasicCompulsorySpace*> &cs=bcsSet;
				assert(cs.contains(c));
				cs.remove(c);
			}
	
			//_scs.remove(ctr);
			delete ctr;
		}
		else
			remaining.append(ctr);
	}

	bool ok;
	assert(spaceConstraintsList.count()<=remaining.count()+_scs.count());
	if(spaceConstraintsList.count()==remaining.count()+_scs.count())
		ok=true;
	else
		ok=false;
		
	if(remaining.count()!=spaceConstraintsList.count()){
		spaceConstraintsList=remaining;
		
		this->internalStructureComputed=false;
		setRulesModifiedAndOtherThings(this);
	}
	
	return ok;
}

void Rules::updateGroupActivitiesInInitialOrderAfterRemoval()
{
	GroupActivitiesInInitialOrderList newList;
	GroupActivitiesInInitialOrderList toBeRemovedList;
	
	for(GroupActivitiesInInitialOrderItem* item : qAsConst(groupActivitiesInInitialOrderList)){
		item->removeUseless(*this);
		if(item->ids.count()<2)
			toBeRemovedList.append(item);
		else
			newList.append(item);
	}
	
	groupActivitiesInInitialOrderList=newList;
	
	for(GroupActivitiesInInitialOrderItem* ga : qAsConst(toBeRemovedList))
		delete ga;
	toBeRemovedList.clear();
	//while(!toBeRemovedList.isEmpty())
	//	delete toBeRemovedList.takeFirst();
}

void Rules::updateActivitiesWhenRemovingStudents(const QSet<StudentsSet*>& studentsSets, bool updateConstraints)
{
	if(studentsSets.count()==0)
		return;

	QList<int> toBeRemovedIds;

	QHash<QString, int> numberOfStudentsPerSet;
	
	for(StudentsSet* studentsSet : qAsConst(studentsSets))
		numberOfStudentsPerSet.insert(studentsSet->name, studentsSet->numberOfStudents);
		
	for(Activity* act : qAsConst(activitiesList)){
		QStringList newStudents;
		for(const QString& st : qAsConst(act->studentsNames)){
			if(!numberOfStudentsPerSet.contains(st)){
				newStudents.append(st);
			}
			else{
				if(act->computeNTotalStudents){
					act->nTotalStudents-=numberOfStudentsPerSet.value(st);
					assert(act->nTotalStudents>=0);
				}
			}
		}
		if(act->studentsNames.count()>0 && newStudents.count()==0)
			toBeRemovedIds.append(act->id);
		act->studentsNames=newStudents;
	}
	
	removeActivities(toBeRemovedIds, updateConstraints);
}

void Rules::updateConstraintsAfterRemoval()
{
	bool recomputeTime=false;
	bool recomputeSpace=false;

	QSet<int> existingActivitiesIds;
	QSet<QString> existingTeachersNames;
	//QSet<QString> existingStudentsNames;
	QSet<QString> existingSubjectsNames;
	QSet<QString> existingActivityTagsNames;
	QSet<QString> existingRoomsNames;
	
	QList<TimeConstraint*> toBeRemovedTime;
	QList<SpaceConstraint*> toBeRemovedSpace;
	
	for(Activity* act : qAsConst(activitiesList))
		existingActivitiesIds.insert(act->id);
		
	for(Teacher* tch : qAsConst(teachersList))
		existingTeachersNames.insert(tch->name);
		
	for(Subject* sbj : qAsConst(subjectsList))
		existingSubjectsNames.insert(sbj->name);
		
	for(ActivityTag* at : qAsConst(activityTagsList))
		existingActivityTagsNames.insert(at->name);
		
	for(Room* rm : qAsConst(roomsList))
		existingRoomsNames.insert(rm->name);
		
	for(TimeConstraint* tc : qAsConst(timeConstraintsList)){
		if(tc->type==CONSTRAINT_TEACHER_NOT_AVAILABLE_TIMES){
			ConstraintTeacherNotAvailableTimes* c=(ConstraintTeacherNotAvailableTimes*)tc;
			if(!existingTeachersNames.contains(c->teacher))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE_TIMES){
			ConstraintStudentsSetNotAvailableTimes* c=(ConstraintStudentsSetNotAvailableTimes*)tc;
			if(!permanentStudentsHash.contains(c->students))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME){
			ConstraintActivitiesSameStartingTime* c=(ConstraintActivitiesSameStartingTime*)tc;
			c->removeUseless(*this);
			if(c->n_activities<2)
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING){
			ConstraintActivitiesNotOverlapping* c=(ConstraintActivitiesNotOverlapping*)tc;
			c->removeUseless(*this);
			if(c->n_activities<2)
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_ACTIVITY_TAGS_NOT_OVERLAPPING){
			ConstraintActivityTagsNotOverlapping* c=(ConstraintActivityTagsNotOverlapping*)tc;

			QStringList atl;
			for(const QString& at : qAsConst(c->activityTagsNames))
				if(existingActivityTagsNames.contains(at))
					atl.append(at);
			c->activityTagsNames=atl;

			if(c->activityTagsNames.count()<2)
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES){
			ConstraintMinDaysBetweenActivities* c=(ConstraintMinDaysBetweenActivities*)tc;
			c->removeUseless(*this);
			if(c->n_activities<2)
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_MAX_DAYS_BETWEEN_ACTIVITIES){
			ConstraintMaxDaysBetweenActivities* c=(ConstraintMaxDaysBetweenActivities*)tc;
			c->removeUseless(*this);
			if(c->n_activities<2)
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_MIN_GAPS_BETWEEN_ACTIVITIES){
			ConstraintMinGapsBetweenActivities* c=(ConstraintMinGapsBetweenActivities*)tc;
			c->removeUseless(*this);
			if(c->n_activities<2)
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_TEACHER_MAX_HOURS_DAILY){
			ConstraintTeacherMaxHoursDaily* c=(ConstraintTeacherMaxHoursDaily*)tc;
			if(!existingTeachersNames.contains(c->teacherName))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_TEACHER_MAX_HOURS_CONTINUOUSLY){
			ConstraintTeacherMaxHoursContinuously* c=(ConstraintTeacherMaxHoursContinuously*)tc;
			if(!existingTeachersNames.contains(c->teacherName))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
			ConstraintTeachersActivityTagMaxHoursContinuously* c=(ConstraintTeachersActivityTagMaxHoursContinuously*)tc;
			if(!existingActivityTagsNames.contains(c->activityTagName))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
			ConstraintTeacherActivityTagMaxHoursContinuously* c=(ConstraintTeacherActivityTagMaxHoursContinuously*)tc;
			if(!existingActivityTagsNames.contains(c->activityTagName) || !existingTeachersNames.contains(c->teacherName))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_TEACHER_MAX_DAYS_PER_WEEK){
			ConstraintTeacherMaxDaysPerWeek* c=(ConstraintTeacherMaxDaysPerWeek*)tc;
			if(!existingTeachersNames.contains(c->teacherName))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_TEACHER_MIN_DAYS_PER_WEEK){
			ConstraintTeacherMinDaysPerWeek* c=(ConstraintTeacherMinDaysPerWeek*)tc;
			if(!existingTeachersNames.contains(c->teacherName))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_WEEK){
			ConstraintStudentsSetMaxGapsPerWeek* c=(ConstraintStudentsSetMaxGapsPerWeek*)tc;
			if(!permanentStudentsHash.contains(c->students))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK){
			ConstraintTeacherMaxGapsPerWeek* c=(ConstraintTeacherMaxGapsPerWeek*)tc;
			if(!existingTeachersNames.contains(c->teacherName))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_TEACHER_MAX_GAPS_PER_DAY){
			ConstraintTeacherMaxGapsPerDay* c=(ConstraintTeacherMaxGapsPerDay*)tc;
			if(!existingTeachersNames.contains(c->teacherName))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_STUDENTS_SET_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR){
			ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour* c=(ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour*)tc;
			if(!permanentStudentsHash.contains(c->students))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY){
			ConstraintStudentsSetMaxHoursDaily* c=(ConstraintStudentsSetMaxHoursDaily*)tc;
			if(!permanentStudentsHash.contains(c->students))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_STUDENTS_SET_MAX_HOURS_CONTINUOUSLY){
			ConstraintStudentsSetMaxHoursContinuously* c=(ConstraintStudentsSetMaxHoursContinuously*)tc;
			if(!permanentStudentsHash.contains(c->students))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
			ConstraintStudentsActivityTagMaxHoursContinuously* c=(ConstraintStudentsActivityTagMaxHoursContinuously*)tc;
			if(!existingActivityTagsNames.contains(c->activityTagName))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
			ConstraintStudentsSetActivityTagMaxHoursContinuously* c=(ConstraintStudentsSetActivityTagMaxHoursContinuously*)tc;
			if(!existingActivityTagsNames.contains(c->activityTagName) || !permanentStudentsHash.contains(c->students))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY){
			ConstraintStudentsSetMinHoursDaily* c=(ConstraintStudentsSetMinHoursDaily*)tc;
			if(!permanentStudentsHash.contains(c->students))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS){
			ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTags* c=(ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTags*)tc;
			if(!permanentStudentsHash.contains(c->students) ||
			 !existingActivityTagsNames.contains(c->firstActivityTag) ||
			 !existingActivityTagsNames.contains(c->secondActivityTag))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS){
			ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTags* c=(ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTags*)tc;
			if(!existingActivityTagsNames.contains(c->firstActivityTag) ||
			 !existingActivityTagsNames.contains(c->secondActivityTag))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS){
			ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTags* c=(ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTags*)tc;
			if(!existingTeachersNames.contains(c->teacher) ||
			 !existingActivityTagsNames.contains(c->firstActivityTag) ||
			 !existingActivityTagsNames.contains(c->secondActivityTag))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS){
			ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTags* c=(ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTags*)tc;
			if(!existingActivityTagsNames.contains(c->firstActivityTag) ||
			 !existingActivityTagsNames.contains(c->secondActivityTag))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME){
			ConstraintActivityPreferredStartingTime* c=(ConstraintActivityPreferredStartingTime*)tc;
			if(!existingActivitiesIds.contains(c->activityId)){
				toBeRemovedTime.append(tc);
				recomputeTime=true;
			}
		}
		else if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_TIME_SLOTS){
			ConstraintActivityPreferredTimeSlots* c=(ConstraintActivityPreferredTimeSlots*)tc;
			if(!existingActivitiesIds.contains(c->p_activityId))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIMES){
			ConstraintActivityPreferredStartingTimes* c=(ConstraintActivityPreferredStartingTimes*)tc;
			if(!existingActivitiesIds.contains(c->activityId))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS){
			ConstraintActivitiesPreferredTimeSlots* c=(ConstraintActivitiesPreferredTimeSlots*)tc;
			if( (c->p_teacherName!="" && !existingTeachersNames.contains(c->p_teacherName)) ||
			 (c->p_studentsName!="" && !permanentStudentsHash.contains(c->p_studentsName)) ||
			 (c->p_subjectName!="" && !existingSubjectsNames.contains(c->p_subjectName)) ||
			 (c->p_activityTagName!="" && !existingActivityTagsNames.contains(c->p_activityTagName)) )
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS){
			ConstraintSubactivitiesPreferredTimeSlots* c=(ConstraintSubactivitiesPreferredTimeSlots*)tc;
			if( (c->p_teacherName!="" && !existingTeachersNames.contains(c->p_teacherName)) ||
			 (c->p_studentsName!="" && !permanentStudentsHash.contains(c->p_studentsName)) ||
			 (c->p_subjectName!="" && !existingSubjectsNames.contains(c->p_subjectName)) ||
			 (c->p_activityTagName!="" && !existingActivityTagsNames.contains(c->p_activityTagName)) )
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES){
			ConstraintActivitiesPreferredStartingTimes* c=(ConstraintActivitiesPreferredStartingTimes*)tc;
			if( (c->teacherName!="" && !existingTeachersNames.contains(c->teacherName)) ||
			 (c->studentsName!="" && !permanentStudentsHash.contains(c->studentsName)) ||
			 (c->subjectName!="" && !existingSubjectsNames.contains(c->subjectName)) ||
			 (c->activityTagName!="" && !existingActivityTagsNames.contains(c->activityTagName)) )
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES){
			ConstraintSubactivitiesPreferredStartingTimes* c=(ConstraintSubactivitiesPreferredStartingTimes*)tc;
			if( (c->teacherName!="" && !existingTeachersNames.contains(c->teacherName)) ||
			 (c->studentsName!="" && !permanentStudentsHash.contains(c->studentsName)) ||
			 (c->subjectName!="" && !existingSubjectsNames.contains(c->subjectName)) ||
			 (c->activityTagName!="" && !existingActivityTagsNames.contains(c->activityTagName)) )
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR){
			ConstraintActivitiesSameStartingHour* c=(ConstraintActivitiesSameStartingHour*)tc;
			c->removeUseless(*this);
			if(c->n_activities<2)
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY){
			ConstraintActivitiesSameStartingDay* c=(ConstraintActivitiesSameStartingDay*)tc;
			c->removeUseless(*this);
			if(c->n_activities<2)
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_TWO_ACTIVITIES_CONSECUTIVE){
			ConstraintTwoActivitiesConsecutive* c=(ConstraintTwoActivitiesConsecutive*)tc;
			if( !existingActivitiesIds.contains(c->firstActivityId) ||
			 !existingActivitiesIds.contains(c->secondActivityId) )
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_TWO_ACTIVITIES_GROUPED){
			ConstraintTwoActivitiesGrouped* c=(ConstraintTwoActivitiesGrouped*)tc;
			if( !existingActivitiesIds.contains(c->firstActivityId) ||
			 !existingActivitiesIds.contains(c->secondActivityId) )
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_THREE_ACTIVITIES_GROUPED){
			ConstraintThreeActivitiesGrouped* c=(ConstraintThreeActivitiesGrouped*)tc;
			if( !existingActivitiesIds.contains(c->firstActivityId) ||
			 !existingActivitiesIds.contains(c->secondActivityId) ||
			 !existingActivitiesIds.contains(c->thirdActivityId) )
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_TWO_ACTIVITIES_ORDERED){
			ConstraintTwoActivitiesOrdered* c=(ConstraintTwoActivitiesOrdered*)tc;
			if( !existingActivitiesIds.contains(c->firstActivityId) ||
			 !existingActivitiesIds.contains(c->secondActivityId) )
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_TWO_ACTIVITIES_ORDERED_IF_SAME_DAY){
			ConstraintTwoActivitiesOrderedIfSameDay* c=(ConstraintTwoActivitiesOrderedIfSameDay*)tc;
			if( !existingActivitiesIds.contains(c->firstActivityId) ||
			 !existingActivitiesIds.contains(c->secondActivityId) )
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_ACTIVITY_ENDS_STUDENTS_DAY){
			ConstraintActivityEndsStudentsDay* c=(ConstraintActivityEndsStudentsDay*)tc;
			if(!existingActivitiesIds.contains(c->activityId))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_TEACHER_MIN_HOURS_DAILY){
			ConstraintTeacherMinHoursDaily* c=(ConstraintTeacherMinHoursDaily*)tc;
			if(!existingTeachersNames.contains(c->teacherName))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_TEACHER_INTERVAL_MAX_DAYS_PER_WEEK){
			ConstraintTeacherIntervalMaxDaysPerWeek* c=(ConstraintTeacherIntervalMaxDaysPerWeek*)tc;
			if(!existingTeachersNames.contains(c->teacherName))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_STUDENTS_SET_INTERVAL_MAX_DAYS_PER_WEEK){
			ConstraintStudentsSetIntervalMaxDaysPerWeek* c=(ConstraintStudentsSetIntervalMaxDaysPerWeek*)tc;
			if(!permanentStudentsHash.contains(c->students))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_ACTIVITIES_END_STUDENTS_DAY){
			ConstraintActivitiesEndStudentsDay* c=(ConstraintActivitiesEndStudentsDay*)tc;
			if( (c->teacherName!="" && !existingTeachersNames.contains(c->teacherName)) ||
			 (c->studentsName!="" && !permanentStudentsHash.contains(c->studentsName)) ||
			 (c->subjectName!="" && !existingSubjectsNames.contains(c->subjectName)) ||
			 (c->activityTagName!="" && !existingActivityTagsNames.contains(c->activityTagName)) )
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_DAILY){
			ConstraintTeachersActivityTagMaxHoursDaily* c=(ConstraintTeachersActivityTagMaxHoursDaily*)tc;
			if(!existingActivityTagsNames.contains(c->activityTagName))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_DAILY){
			ConstraintTeacherActivityTagMaxHoursDaily* c=(ConstraintTeacherActivityTagMaxHoursDaily*)tc;
			if(!existingActivityTagsNames.contains(c->activityTagName) || !existingTeachersNames.contains(c->teacherName))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_DAILY){
			ConstraintStudentsActivityTagMaxHoursDaily* c=(ConstraintStudentsActivityTagMaxHoursDaily*)tc;
			if(!existingActivityTagsNames.contains(c->activityTagName))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_DAILY){
			ConstraintStudentsSetActivityTagMaxHoursDaily* c=(ConstraintStudentsSetActivityTagMaxHoursDaily*)tc;
			if(!existingActivityTagsNames.contains(c->activityTagName) || !permanentStudentsHash.contains(c->students))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_TEACHERS_ACTIVITY_TAG_MIN_HOURS_DAILY){
			ConstraintTeachersActivityTagMinHoursDaily* c=(ConstraintTeachersActivityTagMinHoursDaily*)tc;
			if(!existingActivityTagsNames.contains(c->activityTagName))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_TEACHER_ACTIVITY_TAG_MIN_HOURS_DAILY){
			ConstraintTeacherActivityTagMinHoursDaily* c=(ConstraintTeacherActivityTagMinHoursDaily*)tc;
			if(!existingActivityTagsNames.contains(c->activityTagName) || !existingTeachersNames.contains(c->teacherName))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_STUDENTS_ACTIVITY_TAG_MIN_HOURS_DAILY){
			ConstraintStudentsActivityTagMinHoursDaily* c=(ConstraintStudentsActivityTagMinHoursDaily*)tc;
			if(!existingActivityTagsNames.contains(c->activityTagName))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MIN_HOURS_DAILY){
			ConstraintStudentsSetActivityTagMinHoursDaily* c=(ConstraintStudentsSetActivityTagMinHoursDaily*)tc;
			if(!existingActivityTagsNames.contains(c->activityTagName) || !permanentStudentsHash.contains(c->students))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_DAY){
			ConstraintStudentsSetMaxGapsPerDay* c=(ConstraintStudentsSetMaxGapsPerDay*)tc;
			if(!permanentStudentsHash.contains(c->students))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_ACTIVITIES_OCCUPY_MAX_TIME_SLOTS_FROM_SELECTION){
			ConstraintActivitiesOccupyMaxTimeSlotsFromSelection* c=(ConstraintActivitiesOccupyMaxTimeSlotsFromSelection*)tc;
			c->removeUseless(*this);
			if(c->activitiesIds.count()<1)
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_ACTIVITIES_OCCUPY_MIN_TIME_SLOTS_FROM_SELECTION){
			ConstraintActivitiesOccupyMinTimeSlotsFromSelection* c=(ConstraintActivitiesOccupyMinTimeSlotsFromSelection*)tc;
			c->removeUseless(*this);
			if(c->activitiesIds.count()<1)
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_ACTIVITIES_MAX_SIMULTANEOUS_IN_SELECTED_TIME_SLOTS){
			ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots* c=(ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots*)tc;
			c->removeUseless(*this);
			if(c->activitiesIds.count()<1)
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_ACTIVITIES_MIN_SIMULTANEOUS_IN_SELECTED_TIME_SLOTS){
			ConstraintActivitiesMinSimultaneousInSelectedTimeSlots* c=(ConstraintActivitiesMinSimultaneousInSelectedTimeSlots*)tc;
			c->removeUseless(*this);
			if(c->activitiesIds.count()<1)
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_STUDENTS_SET_MAX_DAYS_PER_WEEK){
			ConstraintStudentsSetMaxDaysPerWeek* c=(ConstraintStudentsSetMaxDaysPerWeek*)tc;
			if(!permanentStudentsHash.contains(c->students))
				toBeRemovedTime.append(tc);
		}
		//2017-02-07
		else if(tc->type==CONSTRAINT_TEACHER_MAX_SPAN_PER_DAY){
			ConstraintTeacherMaxSpanPerDay* c=(ConstraintTeacherMaxSpanPerDay*)tc;
			if(!existingTeachersNames.contains(c->teacherName))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_TEACHER_MIN_RESTING_HOURS){
			ConstraintTeacherMinRestingHours* c=(ConstraintTeacherMinRestingHours*)tc;
			if(!existingTeachersNames.contains(c->teacherName))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_STUDENTS_SET_MAX_SPAN_PER_DAY){
			ConstraintStudentsSetMaxSpanPerDay* c=(ConstraintStudentsSetMaxSpanPerDay*)tc;
			if(!permanentStudentsHash.contains(c->students))
				toBeRemovedTime.append(tc);
		}
		else if(tc->type==CONSTRAINT_STUDENTS_SET_MIN_RESTING_HOURS){
			ConstraintStudentsSetMinRestingHours* c=(ConstraintStudentsSetMinRestingHours*)tc;
			if(!permanentStudentsHash.contains(c->students))
				toBeRemovedTime.append(tc);
		}
	}

	for(SpaceConstraint* sc : qAsConst(spaceConstraintsList)){
		if(sc->type==CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES){
			ConstraintRoomNotAvailableTimes* c=(ConstraintRoomNotAvailableTimes*)sc;
			if(!existingRoomsNames.contains(c->room))
				toBeRemovedSpace.append(sc);
		}
		else if(sc->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
			ConstraintActivityPreferredRoom* c=(ConstraintActivityPreferredRoom*)sc;
			if(!existingActivitiesIds.contains(c->activityId) || !existingRoomsNames.contains(c->roomName)){
				toBeRemovedSpace.append(sc);
				recomputeSpace=true;
			}
		}
		else if(sc->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOMS){
			ConstraintActivityPreferredRooms* c=(ConstraintActivityPreferredRooms*)sc;
			if(!existingActivitiesIds.contains(c->activityId))
				toBeRemovedSpace.append(sc);
			else{
				QStringList newRooms;
				for(const QString& room : qAsConst(c->roomsNames))
					if(existingRoomsNames.contains(room))
						newRooms.append(room);
				c->roomsNames=newRooms;
				if(c->roomsNames.count()==0)
					toBeRemovedSpace.append(sc);
			}
		}
		else if(sc->type==CONSTRAINT_STUDENTS_SET_HOME_ROOM){
			ConstraintStudentsSetHomeRoom* c=(ConstraintStudentsSetHomeRoom*)sc;
			if(!permanentStudentsHash.contains(c->studentsName) || !existingRoomsNames.contains(c->roomName))
				toBeRemovedSpace.append(sc);
		}
		else if(sc->type==CONSTRAINT_STUDENTS_SET_HOME_ROOMS){
			ConstraintStudentsSetHomeRooms* c=(ConstraintStudentsSetHomeRooms*)sc;
			if(!permanentStudentsHash.contains(c->studentsName))
				toBeRemovedSpace.append(sc);
			else{
				QStringList newRooms;
				for(const QString& room : qAsConst(c->roomsNames))
					if(existingRoomsNames.contains(room))
						newRooms.append(room);
				c->roomsNames=newRooms;
				if(c->roomsNames.count()==0)
					toBeRemovedSpace.append(sc);
			}
		}
		else if(sc->type==CONSTRAINT_TEACHER_HOME_ROOM){
			ConstraintTeacherHomeRoom* c=(ConstraintTeacherHomeRoom*)sc;
			if(!existingTeachersNames.contains(c->teacherName) || !existingRoomsNames.contains(c->roomName))
				toBeRemovedSpace.append(sc);
		}
		else if(sc->type==CONSTRAINT_TEACHER_HOME_ROOMS){
			ConstraintTeacherHomeRooms* c=(ConstraintTeacherHomeRooms*)sc;
			if(!existingTeachersNames.contains(c->teacherName))
				toBeRemovedSpace.append(sc);
			else{
				QStringList newRooms;
				for(const QString& room : qAsConst(c->roomsNames))
					if(existingRoomsNames.contains(room))
						newRooms.append(room);
				c->roomsNames=newRooms;
				if(c->roomsNames.count()==0)
					toBeRemovedSpace.append(sc);
			}
		}
		else if(sc->type==CONSTRAINT_SUBJECT_PREFERRED_ROOM){
			ConstraintSubjectPreferredRoom* c=(ConstraintSubjectPreferredRoom*)sc;
			if(!existingSubjectsNames.contains(c->subjectName) || !existingRoomsNames.contains(c->roomName))
				toBeRemovedSpace.append(sc);
		}
		else if(sc->type==CONSTRAINT_SUBJECT_PREFERRED_ROOMS){
			ConstraintSubjectPreferredRooms* c=(ConstraintSubjectPreferredRooms*)sc;
			if(!existingSubjectsNames.contains(c->subjectName))
				toBeRemovedSpace.append(sc);
			else{
				QStringList newRooms;
				for(const QString& room : qAsConst(c->roomsNames))
					if(existingRoomsNames.contains(room))
						newRooms.append(room);
				c->roomsNames=newRooms;
				if(c->roomsNames.count()==0)
					toBeRemovedSpace.append(sc);
			}
		}
		else if(sc->type==CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM){
			ConstraintSubjectActivityTagPreferredRoom* c=(ConstraintSubjectActivityTagPreferredRoom*)sc;
			if(!existingSubjectsNames.contains(c->subjectName) || !existingActivityTagsNames.contains(c->activityTagName) ||
			 !existingRoomsNames.contains(c->roomName))
				toBeRemovedSpace.append(sc);
		}
		else if(sc->type==CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS){
			ConstraintSubjectActivityTagPreferredRooms* c=(ConstraintSubjectActivityTagPreferredRooms*)sc;
			if(!existingSubjectsNames.contains(c->subjectName) || !existingActivityTagsNames.contains(c->activityTagName))
				toBeRemovedSpace.append(sc);
			else{
				QStringList newRooms;
				for(const QString& room : qAsConst(c->roomsNames))
					if(existingRoomsNames.contains(room))
						newRooms.append(room);
				c->roomsNames=newRooms;
				if(c->roomsNames.count()==0)
					toBeRemovedSpace.append(sc);
			}
		}
		else if(sc->type==CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM){
			ConstraintActivityTagPreferredRoom* c=(ConstraintActivityTagPreferredRoom*)sc;
			if(!existingActivityTagsNames.contains(c->activityTagName) || !existingRoomsNames.contains(c->roomName))
				toBeRemovedSpace.append(sc);
		}
		else if(sc->type==CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS){
			ConstraintActivityTagPreferredRooms* c=(ConstraintActivityTagPreferredRooms*)sc;
			if(!existingActivityTagsNames.contains(c->activityTagName))
				toBeRemovedSpace.append(sc);
			else{
				QStringList newRooms;
				for(const QString& room : qAsConst(c->roomsNames))
					if(existingRoomsNames.contains(room))
						newRooms.append(room);
				c->roomsNames=newRooms;
				if(c->roomsNames.count()==0)
					toBeRemovedSpace.append(sc);
			}
		}
		else if(sc->type==CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY){
			ConstraintStudentsSetMaxBuildingChangesPerDay* c=(ConstraintStudentsSetMaxBuildingChangesPerDay*)sc;
			if(!permanentStudentsHash.contains(c->studentsName))
				toBeRemovedSpace.append(sc);
		}
		else if(sc->type==CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_WEEK){
			ConstraintStudentsSetMaxBuildingChangesPerWeek* c=(ConstraintStudentsSetMaxBuildingChangesPerWeek*)sc;
			if(!permanentStudentsHash.contains(c->studentsName))
				toBeRemovedSpace.append(sc);
		}
		else if(sc->type==CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_BUILDING_CHANGES){
			ConstraintStudentsSetMinGapsBetweenBuildingChanges* c=(ConstraintStudentsSetMinGapsBetweenBuildingChanges*)sc;
			if(!permanentStudentsHash.contains(c->studentsName))
				toBeRemovedSpace.append(sc);
		}
		else if(sc->type==CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY){
			ConstraintTeacherMaxBuildingChangesPerDay* c=(ConstraintTeacherMaxBuildingChangesPerDay*)sc;
			if(!existingTeachersNames.contains(c->teacherName))
				toBeRemovedSpace.append(sc);
		}
		else if(sc->type==CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_WEEK){
			ConstraintTeacherMaxBuildingChangesPerWeek* c=(ConstraintTeacherMaxBuildingChangesPerWeek*)sc;
			if(!existingTeachersNames.contains(c->teacherName))
				toBeRemovedSpace.append(sc);
		}
		else if(sc->type==CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_BUILDING_CHANGES){
			ConstraintTeacherMinGapsBetweenBuildingChanges* c=(ConstraintTeacherMinGapsBetweenBuildingChanges*)sc;
			if(!existingTeachersNames.contains(c->teacherName))
				toBeRemovedSpace.append(sc);
		}
		else if(sc->type==CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY){
			ConstraintStudentsSetMaxRoomChangesPerDay* c=(ConstraintStudentsSetMaxRoomChangesPerDay*)sc;
			if(!permanentStudentsHash.contains(c->studentsName))
				toBeRemovedSpace.append(sc);
		}
		else if(sc->type==CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_WEEK){
			ConstraintStudentsSetMaxRoomChangesPerWeek* c=(ConstraintStudentsSetMaxRoomChangesPerWeek*)sc;
			if(!permanentStudentsHash.contains(c->studentsName))
				toBeRemovedSpace.append(sc);
		}
		else if(sc->type==CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ROOM_CHANGES){
			ConstraintStudentsSetMinGapsBetweenRoomChanges* c=(ConstraintStudentsSetMinGapsBetweenRoomChanges*)sc;
			if(!permanentStudentsHash.contains(c->studentsName))
				toBeRemovedSpace.append(sc);
		}
		else if(sc->type==CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY){
			ConstraintTeacherMaxRoomChangesPerDay* c=(ConstraintTeacherMaxRoomChangesPerDay*)sc;
			if(!existingTeachersNames.contains(c->teacherName))
				toBeRemovedSpace.append(sc);
		}
		else if(sc->type==CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_WEEK){
			ConstraintTeacherMaxRoomChangesPerWeek* c=(ConstraintTeacherMaxRoomChangesPerWeek*)sc;
			if(!existingTeachersNames.contains(c->teacherName))
				toBeRemovedSpace.append(sc);
		}
		else if(sc->type==CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ROOM_CHANGES){
			ConstraintTeacherMinGapsBetweenRoomChanges* c=(ConstraintTeacherMinGapsBetweenRoomChanges*)sc;
			if(!existingTeachersNames.contains(c->teacherName))
				toBeRemovedSpace.append(sc);
		}
		else if(sc->type==CONSTRAINT_ACTIVITIES_OCCUPY_MAX_DIFFERENT_ROOMS){
			ConstraintActivitiesOccupyMaxDifferentRooms* c=(ConstraintActivitiesOccupyMaxDifferentRooms*)sc;
			c->removeUseless(*this);
			if(c->activitiesIds.count()<2)
				toBeRemovedSpace.append(sc);
		}
		else if(sc->type==CONSTRAINT_ACTIVITIES_SAME_ROOM_IF_CONSECUTIVE){
			ConstraintActivitiesSameRoomIfConsecutive* c=(ConstraintActivitiesSameRoomIfConsecutive*)sc;
			c->removeUseless(*this);
			if(c->activitiesIds.count()<2)
				toBeRemovedSpace.append(sc);
		}
	}
	
	bool t;
	if(toBeRemovedTime.count()>0){
		t=removeTimeConstraints(toBeRemovedTime);
		assert(t);
	}
	if(toBeRemovedSpace.count()>0){
		t=removeSpaceConstraints(toBeRemovedSpace);
		assert(t);
	}
	
	if(recomputeTime){
		LockUnlock::computeLockedUnlockedActivitiesOnlyTime();
	}
	if(recomputeSpace){
		LockUnlock::computeLockedUnlockedActivitiesOnlySpace();
	}
	if(recomputeTime || recomputeSpace){
		LockUnlock::increaseCommunicationSpinBox();
	}
}

bool Rules::read(QWidget* parent, const QString& fileName, bool commandLine, QString commandLineDirectory) //commandLineDirectory has trailing FILE_SEP
{
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly)){
		//cout<<"Could not open file - not existing or in use\n";
		RulesIrreconcilableMessage::warning(parent, tr("FET warning"), tr("Could not open file - not existing or in use"));
		return false;
	}
	
	xmlReaderNumberOfUnrecognizedFields=0;
	
	unrecognizedXmlTags.clear();
	unrecognizedXmlLineNumbers.clear();
	unrecognizedXmlColumnNumbers.clear();
	
	QXmlStreamReader xmlReader(&file);
	
	////////////////////////////////////////

	if(!commandLine){
		//logging part
		QDir dir;
		bool t=true;
		if(!dir.exists(OUTPUT_DIR+FILE_SEP+"logs"))
			t=dir.mkpath(OUTPUT_DIR+FILE_SEP+"logs");
		if(!t){
			RulesIrreconcilableMessage::warning(parent, tr("FET warning"), tr("Cannot create or use directory %1 - cannot continue").arg(QDir::toNativeSeparators(OUTPUT_DIR+FILE_SEP+"logs")));
			return false;
		}
		assert(t);
	}
	else{
		QDir dir;
		bool t=true;
		if(!dir.exists(commandLineDirectory+"logs"))
			t=dir.mkpath(commandLineDirectory+"logs");
		if(!t){
			RulesIrreconcilableMessage::warning(parent, tr("FET warning"), tr("Cannot create or use directory %1 - cannot continue").arg(QDir::toNativeSeparators(commandLineDirectory+"logs")));
			return false;
		}
		assert(t);
	}
	
	FakeString xmlReadingLog;
	xmlReadingLog="";

	QDate dat=QDate::currentDate();
	QTime tim=QTime::currentTime();
	QLocale loc(FET_LANGUAGE);
	QString sTime=loc.toString(dat, QLocale::ShortFormat)+" "+loc.toString(tim, QLocale::ShortFormat);

	QString reducedXmlLog="";
	reducedXmlLog+="Log generated by FET "+FET_VERSION+" on "+sTime+"\n\n";
	QString shortFileName=fileName.right(fileName.length()-fileName.lastIndexOf(FILE_SEP)-1);
	reducedXmlLog+="Reading file "+shortFileName+"\n";
	QFileInfo fileinfo(fileName);
	reducedXmlLog+="Complete file name, including path: "+QDir::toNativeSeparators(fileinfo.absoluteFilePath())+"\n";
	reducedXmlLog+="\n";

	QString tmp;
	if(commandLine)
		tmp=commandLineDirectory+"logs"+FILE_SEP+XML_PARSING_LOG_FILENAME;
	else
		tmp=OUTPUT_DIR+FILE_SEP+"logs"+FILE_SEP+XML_PARSING_LOG_FILENAME;
	QFile file2(tmp);
	bool canWriteLogFile=true;
	if(!file2.open(QIODevice::WriteOnly)){
		QString s=tr("FET cannot open the log file %1 for writing. This might mean that you don't"
			" have write permissions in this location. You can continue operation, but you might not be able to save the generated timetables"
			" as html files").arg(QDir::toNativeSeparators(tmp))+
			"\n\n"+tr("A solution is to remove that file (if it exists already) or set its permissions to allow writing")+
			"\n\n"+tr("Please report possible bug");
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"), s);
		canWriteLogFile=false;
	}
	QTextStream logStream;
	if(canWriteLogFile){
		logStream.setDevice(&file2);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
		logStream.setEncoding(QStringConverter::Utf8);
#else
		logStream.setCodec("UTF-8");
#endif
		logStream.setGenerateByteOrderMark(true);
	}
	
	bool okAbove3_12_17=true;
	bool version5AndAbove=false;
	bool warning=false;

	QString file_version;

	if(xmlReader.readNextStartElement()){
		xmlReadingLog+=" Found "+xmlReader.name().toString()+" tag\n";
	
		bool okfetTag;
		if(xmlReader.name()==QString("fet") || xmlReader.name()==QString("FET")) //the new tag is fet, the old tag - FET
			okfetTag=true;
		else
			okfetTag=false;
	
		if(!okfetTag)
			okAbove3_12_17=false;
		else{
			assert(xmlReader.isStartElement());
			assert(okAbove3_12_17==true);
			/*QDomDocumentType dt=doc.doctype();
			if(dt.isNull() || dt.name()!="FET")
				okAbove3_12_17=false;
			else*/
			int filev[3], fetv[3];
			
			assert(xmlReader.name()==QString("fet") || xmlReader.name()==QString("FET"));
			QString version=xmlReader.attributes().value("version").toString();
			file_version=version;
		
/*			QDomAttr a=elem1.attributeNode("version");
			
			QString version=a.value();
			file_version=version;*/
			
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
			QRegularExpression fileVerReCap("^(\\d+)\\.(\\d+)\\.(\\d+)(.*)$");
			QRegularExpressionMatch match=fileVerReCap.match(file_version);
			filev[0]=filev[1]=filev[2]=-1;
			if(!match.hasMatch()){
				RulesReconcilableMessage::warning(parent, tr("FET warning"), tr("File contains a version numbering scheme which"
				" is not matched by v.v.va (3 numbers separated by points, followed by any string a, which may be empty). File will be opened, but you are advised"
				" to check the version of the .fet file (in the beginning of the file). If this is a FET bug, please report it")+"\n\n"+
				tr("If you are opening a file older than FET format version 5, it will be converted to latest FET data format"));
				if(VERBOSE){
					cout<<"Opened file version not matched by regexp"<<endl;
				}
			}
			else{
				bool ok;
				filev[0]=match.captured(1).toInt(&ok);
				assert(ok);
				filev[1]=match.captured(2).toInt(&ok);
				assert(ok);
				filev[2]=match.captured(3).toInt(&ok);
				assert(ok);
				if(VERBOSE){
					cout<<"Opened file version matched by regexp: major="<<filev[0]<<", minor="<<filev[1]<<", revision="<<filev[2];
					cout<<", additional text="<<qPrintable(match.captured(4))<<"."<<endl;
				}
			}
		
			QRegularExpression fetVerReCap("^(\\d+)\\.(\\d+)\\.(\\d+)(.*)$");
			match=fetVerReCap.match(FET_VERSION);
			
			fetv[0]=fetv[1]=fetv[2]=-1;
			if(!match.hasMatch()){
				RulesReconcilableMessage::warning(parent, tr("FET warning"), tr("FET version does not respect the format v.v.va"
				" (3 numbers separated by points, followed by any string a, which may be empty). This is probably a bug in FET - please report it"));
				if(VERBOSE){
					cout<<"FET version not matched by regexp"<<endl;
				}
			}
			else{
				bool ok;
				fetv[0]=match.captured(1).toInt(&ok);
				assert(ok);
				fetv[1]=match.captured(2).toInt(&ok);
				assert(ok);
				fetv[2]=match.captured(3).toInt(&ok);
				assert(ok);
				if(VERBOSE){
					cout<<"FET version matched by regexp: major="<<fetv[0]<<", minor="<<fetv[1]<<", revision="<<fetv[2];
					cout<<", additional text="<<qPrintable(match.captured(4))<<"."<<endl;
				}
			}
#else
			QRegExp fileVerReCap("^(\\d+)\\.(\\d+)\\.(\\d+)(.*)$");
			int tfile=fileVerReCap.indexIn(file_version);
			filev[0]=filev[1]=filev[2]=-1;
			if(tfile!=0){
				RulesReconcilableMessage::warning(parent, tr("FET warning"), tr("File contains a version numbering scheme which"
				" is not matched by v.v.va (3 numbers separated by points, followed by any string a, which may be empty). File will be opened, but you are advised"
				" to check the version of the .fet file (in the beginning of the file). If this is a FET bug, please report it")+"\n\n"+
				tr("If you are opening a file older than FET format version 5, it will be converted to latest FET data format"));
				if(VERBOSE){
					cout<<"Opened file version not matched by regexp"<<endl;
				}
			}
			else{
				bool ok;
				filev[0]=fileVerReCap.cap(1).toInt(&ok);
				assert(ok);
				filev[1]=fileVerReCap.cap(2).toInt(&ok);
				assert(ok);
				filev[2]=fileVerReCap.cap(3).toInt(&ok);
				assert(ok);
				if(VERBOSE){
					cout<<"Opened file version matched by regexp: major="<<filev[0]<<", minor="<<filev[1]<<", revision="<<filev[2];
					cout<<", additional text="<<qPrintable(fileVerReCap.cap(4))<<"."<<endl;
				}
			}
		
			QRegExp fetVerReCap("^(\\d+)\\.(\\d+)\\.(\\d+)(.*)$");
			int tfet=fetVerReCap.indexIn(FET_VERSION);
			fetv[0]=fetv[1]=fetv[2]=-1;
			if(tfet!=0){
				RulesReconcilableMessage::warning(parent, tr("FET warning"), tr("FET version does not respect the format v.v.va"
				" (3 numbers separated by points, followed by any string a, which may be empty). This is probably a bug in FET - please report it"));
				if(VERBOSE){
					cout<<"FET version not matched by regexp"<<endl;
				}
			}
			else{
				bool ok;
				fetv[0]=fetVerReCap.cap(1).toInt(&ok);
				assert(ok);
				fetv[1]=fetVerReCap.cap(2).toInt(&ok);
				assert(ok);
				fetv[2]=fetVerReCap.cap(3).toInt(&ok);
				assert(ok);
				if(VERBOSE){
					cout<<"FET version matched by regexp: major="<<fetv[0]<<", minor="<<fetv[1]<<", revision="<<fetv[2];
					cout<<", additional text="<<qPrintable(fetVerReCap.cap(4))<<"."<<endl;
				}
			}
#endif
			
			if(filev[0]>=0 && fetv[0]>=0 && filev[1]>=0 && fetv[1]>=0 && filev[2]>=0 && fetv[2]>=0){
				if(filev[0]>fetv[0] || (filev[0]==fetv[0] && filev[1]>fetv[1]) || (filev[0]==fetv[0]&&filev[1]==fetv[1]&&filev[2]>fetv[2])){
					warning=true;
				}
			}
		
			if(filev[0]>=5 || (filev[0]==-1 && filev[1]==-1 && filev[2]==-1))
			//if major is >= 5 or major cannot be read
				version5AndAbove=true;
		}
	}
	else{
		okAbove3_12_17=false;
	}
	if(!okAbove3_12_17){
		if(VERBOSE){
			cout<<"Invalid fet 3.12.17 or above input file"<<endl;
		}
		file.close();
		file2.close();
		RulesIrreconcilableMessage::warning(parent, tr("FET warning"), tr("File is incorrect...it cannot be opened"));
		return false;
	}
	
	if(!version5AndAbove){
		RulesReconcilableMessage::warning(parent, tr("FET information"),
		 tr("Opening older file - it will be converted to latest format, automatically "
		 "assigning weight percentages to constraints and dropping parity for activities. "
		 "You are advised to make a backup of your old file before saving in new format.\n\n"
		 "Please note that the default weight percentage of constraints min days between activities "
		 "will be 95% (mainly satisfied, not always) and 'force consecutive if same day' will be set to true "
		 "(meaning that if the activities are in the same day, they will be placed continuously, in a bigger duration activity). "
		 "If you want, you can modify this percent to be 100%, manually in the fet input file "
		 "or from the interface"));
	}
	
	if(warning){
		RulesReconcilableMessage::warning(parent, tr("FET information"),
		 tr("Opening a file generated with a newer version than your current FET software ... file will be opened but it is recommended to update your FET software to the latest version")
		 +"\n\n"+tr("Your FET version: %1, file version: %2").arg(FET_VERSION).arg(file_version));
	}
	
	//Clear old rules, initialize new rules
	if(this->initialized)
		this->kill();
	this->init();
	
	this->institutionName=tr("Default institution");
	this->comments=tr("Default comments");

	this->nDaysPerWeek=5;
	this->daysOfTheWeek[0] = tr("Monday");
	this->daysOfTheWeek[1] = tr("Tuesday");
	this->daysOfTheWeek[2] = tr("Wednesday");
	this->daysOfTheWeek[3] = tr("Thursday");
	this->daysOfTheWeek[4] = tr("Friday");

	this->nHoursPerDay=12;
	this->hoursOfTheDay[0]=tr("08:00", "Hour name");
	this->hoursOfTheDay[1]=tr("09:00", "Hour name");
	this->hoursOfTheDay[2]=tr("10:00", "Hour name");
	this->hoursOfTheDay[3]=tr("11:00", "Hour name");
	this->hoursOfTheDay[4]=tr("12:00", "Hour name");
	this->hoursOfTheDay[5]=tr("13:00", "Hour name");
	this->hoursOfTheDay[6]=tr("14:00", "Hour name");
	this->hoursOfTheDay[7]=tr("15:00", "Hour name");
	this->hoursOfTheDay[8]=tr("16:00", "Hour name");
	this->hoursOfTheDay[9]=tr("17:00", "Hour name");
	this->hoursOfTheDay[10]=tr("18:00", "Hour name");
	this->hoursOfTheDay[11]=tr("19:00", "Hour name");
	//this->hoursOfTheDay[12]=tr("20:00", "Hour name");

	bool skipDeprecatedConstraints=false;
	
	bool skipDuplicatedStudentsSets=false;
	
	assert(xmlReader.isStartElement() && (xmlReader.name()==QString("fet") || xmlReader.name()==QString("FET")));
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="  Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Institution_Name")){
			QString text=xmlReader.readElementText();
			this->institutionName=text;
			xmlReadingLog+="  Found institution name="+this->institutionName+"\n";
			reducedXmlLog+="Read institution name="+this->institutionName+"\n";
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			this->comments=text;
			xmlReadingLog+="  Found comments="+this->comments+"\n";
			reducedXmlLog+="Read comments="+this->comments+"\n";
		}
		else if(xmlReader.name()==QString("Days_List")){
			int tmp=0;
			bool numberWasFound=false;
			assert(xmlReader.isStartElement());
			QString numberString="Number_of_Days", dayString="Name";
			while(xmlReader.readNextStartElement()){
				xmlReadingLog+="   Found "+xmlReader.name().toString()+" tag\n";
				if(xmlReader.name()==QString("Number") || xmlReader.name()==QString("Number_of_Days") ){
					numberString=xmlReader.name().toString();
					QString text=xmlReader.readElementText();
					this->nDaysPerWeek=text.toInt();
					numberWasFound=true;
					xmlReadingLog+="   Found the number of days per week = "+
					 CustomFETString::number(this->nDaysPerWeek)+"\n";
					reducedXmlLog+="Added "+
					 CustomFETString::number(this->nDaysPerWeek)+" days per week\n";
					if(nDaysPerWeek<=0){
						xmlReader.raiseError(tr("%1 is incorrect").arg(numberString));
					}
					else if(nDaysPerWeek>MAX_DAYS_PER_WEEK){
						xmlReader.raiseError(tr("%1 is too large. Maximum allowed is %2.").arg(numberString).arg(MAX_DAYS_PER_WEEK));
					}
					else{
						assert(this->nDaysPerWeek>0 && nDaysPerWeek<=MAX_DAYS_PER_WEEK);
					}
				}
				//old .fet XML format
				else if(xmlReader.name()==QString("Name")){
					dayString=xmlReader.name().toString();
					if(tmp>=MAX_DAYS_PER_WEEK){
						xmlReader.raiseError(tr("Too many %1 items. Maximum allowed is %2.").arg(dayString).arg(MAX_DAYS_PER_WEEK));
						xmlReader.skipCurrentElement();
					}
					else{
						QString text=xmlReader.readElementText();
						this->daysOfTheWeek[tmp]=text;
						xmlReadingLog+="   Found day "+this->daysOfTheWeek[tmp]+"\n";
						tmp++;
					}
				}
				//end old .fet XML format
				else if(xmlReader.name()==QString("Day")){
					assert(xmlReader.isStartElement());
					while(xmlReader.readNextStartElement()){
						xmlReadingLog+="   Found "+xmlReader.name().toString()+" tag\n";
						if(xmlReader.name()==QString("Name")){
							dayString=xmlReader.name().toString();
							if(tmp>=MAX_DAYS_PER_WEEK){
								xmlReader.raiseError(tr("Too many %1 items. Maximum allowed is %2.").arg(dayString).arg(MAX_DAYS_PER_WEEK));
								xmlReader.skipCurrentElement();
							}
							else{
								QString text=xmlReader.readElementText();
								this->daysOfTheWeek[tmp]=text;
								xmlReadingLog+="   Found day "+this->daysOfTheWeek[tmp]+"\n";
								tmp++;
							}
						}
						else{
							unrecognizedXmlTags.append(xmlReader.name().toString());
							unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
							unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

							xmlReader.skipCurrentElement();
							xmlReaderNumberOfUnrecognizedFields++;
						}
					}
				}
				else{
					unrecognizedXmlTags.append(xmlReader.name().toString());
					unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
					unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

					xmlReader.skipCurrentElement();
					xmlReaderNumberOfUnrecognizedFields++;
				}
			}
			if(!xmlReader.error()){
				if(!numberWasFound){
					xmlReader.raiseError(tr("%1 not found").arg(numberString));
				}
				else{
					if(!(tmp==nDaysPerWeek))
						xmlReader.raiseError(tr("%1: %2 and the number of %3 read do not correspond").arg("Days_List").arg(numberString).arg(dayString));
					else
						assert(tmp==this->nDaysPerWeek);
				}
			}
		}
		else if(xmlReader.name()==QString("Hours_List")){
			int tmp=0;
			bool numberWasFound=false;
			assert(xmlReader.isStartElement());
			QString numberString="Number_of_Hours", hourString="Name";
			while(xmlReader.readNextStartElement()){
				xmlReadingLog+="   Found "+xmlReader.name().toString()+" tag\n";
				if(xmlReader.name()==QString("Number") || xmlReader.name()==QString("Number_of_Hours")){
					numberString=xmlReader.name().toString();
					QString text=xmlReader.readElementText();
					this->nHoursPerDay=text.toInt();
					numberWasFound=true;
					xmlReadingLog+="   Found the number of hours per day = "+
					 CustomFETString::number(this->nHoursPerDay)+"\n";
					reducedXmlLog+="Added "+
					 CustomFETString::number(this->nHoursPerDay)+" hours per day\n";
					if(nHoursPerDay<=0){
						xmlReader.raiseError(tr("%1 is incorrect").arg(numberString));
					}
					else if(nHoursPerDay>MAX_HOURS_PER_DAY){
						xmlReader.raiseError(tr("%1 is too large. Maximum allowed is %2.").arg("Number").arg(MAX_HOURS_PER_DAY));
					}
					else{
						assert(this->nHoursPerDay>0 && nHoursPerDay<=MAX_HOURS_PER_DAY);
					}
				}
				//old .fet XML format
				else if(xmlReader.name()==QString("Name")){
					hourString=xmlReader.name().toString();
					if(tmp>=MAX_HOURS_PER_DAY){
						xmlReader.raiseError(tr("Too many %1 items. Maximum allowed is %2.").arg(hourString).arg(MAX_HOURS_PER_DAY));
						xmlReader.skipCurrentElement();
					}
					else{
						QString text=xmlReader.readElementText();
						this->hoursOfTheDay[tmp]=text;
						xmlReadingLog+="   Found hour "+this->hoursOfTheDay[tmp]+"\n";
						tmp++;
					}
				}
				//end old .fet XML format
				else if(xmlReader.name()==QString("Hour")){
					assert(xmlReader.isStartElement());
					while(xmlReader.readNextStartElement()){
						xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
						if(xmlReader.name()==QString("Name")){
							hourString=xmlReader.name().toString();
							if(tmp>=MAX_HOURS_PER_DAY){
								xmlReader.raiseError(tr("Too many %1 items. Maximum allowed is %2.").arg(hourString).arg(MAX_HOURS_PER_DAY));
								xmlReader.skipCurrentElement();
							}
							else{
								QString text=xmlReader.readElementText();
								this->hoursOfTheDay[tmp]=text;
								xmlReadingLog+="    Found hour "+this->hoursOfTheDay[tmp]+"\n";
								tmp++;
							}
						}
						else{
							unrecognizedXmlTags.append(xmlReader.name().toString());
							unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
							unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

							xmlReader.skipCurrentElement();
							xmlReaderNumberOfUnrecognizedFields++;
						}
					}
				}
				else{
					unrecognizedXmlTags.append(xmlReader.name().toString());
					unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
					unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

					xmlReader.skipCurrentElement();
					xmlReaderNumberOfUnrecognizedFields++;
				}
			}
			if(!xmlReader.error()){
				if(!numberWasFound){
					xmlReader.raiseError(tr("%1 not found").arg(numberString));
				}
				else{
					if(numberString=="Number"){
						//some older files contain also the end of day hour, so tmp==nHoursPerDay+1 in this case
						if(!(tmp==nHoursPerDay || tmp==nHoursPerDay+1))
							xmlReader.raiseError(tr("%1: %2 and the number of %3 read do not correspond").arg("Hours_List").arg(numberString).arg(hourString));
						else
							assert(tmp==nHoursPerDay || tmp==nHoursPerDay+1);
					}
					else{
						assert(numberString=="Number_of_Hours");
						if(!(tmp==nHoursPerDay))
							xmlReader.raiseError(tr("%1: %2 and the number of %3 read do not correspond").arg("Hours_List").arg(numberString).arg(hourString));
						else
							assert(tmp==nHoursPerDay);
					}
				}
			}
		}
		else if(xmlReader.name()==QString("Teachers_List")){
			QSet<QString> teachersRead;
			
			QSet<QString> subjectsRead; //we assume that the reading of the subjects is done before the reading of the teachers
			for(Subject* sbj : qAsConst(subjectsList))
				subjectsRead.insert(sbj->name);
		
			int tmp=0;
			assert(xmlReader.isStartElement());
			while(xmlReader.readNextStartElement()){
				xmlReadingLog+="   Found "+xmlReader.name().toString()+" tag\n";
				if(xmlReader.name()==QString("Teacher")){
					Teacher* teacher=new Teacher();

					assert(xmlReader.isStartElement());
					while(xmlReader.readNextStartElement()){
						xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
						if(xmlReader.name()==QString("Name")){
							QString text=xmlReader.readElementText();
							teacher->name=text;
							xmlReadingLog+="    Read teacher name: "+teacher->name+"\n";
						}
						else if(xmlReader.name()==QString("Target_Number_of_Hours")){
							QString text=xmlReader.readElementText();
							teacher->targetNumberOfHours=text.toInt();
							xmlReadingLog+="    Read teacher target number of hours: "+CustomFETString::number(teacher->targetNumberOfHours)+"\n";
						}
						else if(xmlReader.name()==QString("Qualified_Subjects")){
							assert(xmlReader.isStartElement());
							while(xmlReader.readNextStartElement()){
								xmlReadingLog+="   Found "+xmlReader.name().toString()+" tag\n";
								if(xmlReader.name()==QString("Qualified_Subject")){
									QString text=xmlReader.readElementText();
									if(teacher->qualifiedSubjectsHash.contains(text)){
										xmlReader.raiseError(tr("The teacher %1 has duplicate qualified subject %2.").arg(teacher->name).arg(text));
									}
									else if(!subjectsRead.contains(text)){
										xmlReader.raiseError(tr("The teacher %1 has attached an inexistent qualified subject %2."
										 " Please note that starting with FET version %3 the list of subjects (tag <Subjects_List> in the .fet XML file)"
										 " must appear before the list of teachers (tag <Teachers_List> in the .fet XML file)", "Please keep <Subjects_List> and"
										 " <Teachers_List> untranslated, as in the original English string")
										 .arg(teacher->name).arg(text).arg("5.30.0"));
									}
									else{
										teacher->qualifiedSubjectsList.push_back(text);
										teacher->qualifiedSubjectsHash.insert(text, std::prev(teacher->qualifiedSubjectsList.end()));
									}
								}
								else{
									unrecognizedXmlTags.append(xmlReader.name().toString());
									unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
									unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

									xmlReader.skipCurrentElement();
									xmlReaderNumberOfUnrecognizedFields++;
								}
							}
						}
						else if(xmlReader.name()==QString("Comments")){
							QString text=xmlReader.readElementText();
							teacher->comments=text;
							xmlReadingLog+="    Crt. teacher comments="+teacher->comments+"\n";
						}
						else{
							unrecognizedXmlTags.append(xmlReader.name().toString());
							unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
							unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

							xmlReader.skipCurrentElement();
							xmlReaderNumberOfUnrecognizedFields++;
						}
					}
					bool tmp2=teachersRead.contains(teacher->name);
					if(tmp2){
						RulesReconcilableMessage::warning(parent, tr("FET warning"),
						 tr("Duplicate teacher %1 found - ignoring").arg(teacher->name));
						xmlReadingLog+="   Teacher not added - duplicate\n";
						
						delete teacher;
					}
					else{
						teachersRead.insert(teacher->name);
						this->addTeacherFast(teacher);
						tmp++;
						xmlReadingLog+="   Teacher added\n";
					}
				}
				else{
					unrecognizedXmlTags.append(xmlReader.name().toString());
					unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
					unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

					xmlReader.skipCurrentElement();
					xmlReaderNumberOfUnrecognizedFields++;
				}
			}
			if(!(tmp==this->teachersList.size()))
				xmlReader.raiseError(tr("%1 is incorrect").arg("Teachers_List"));
			else{
				assert(tmp==this->teachersList.size());
				xmlReadingLog+="  Added "+CustomFETString::number(tmp)+" teachers\n";
				reducedXmlLog+="Added "+CustomFETString::number(tmp)+" teachers\n";
			}
		}
		else if(xmlReader.name()==QString("Subjects_List")){
			QSet<QString> subjectsRead;
		
			int tmp=0;
			assert(xmlReader.isStartElement());
			while(xmlReader.readNextStartElement()){
				xmlReadingLog+="   Found "+xmlReader.name().toString()+" tag\n";
				if(xmlReader.name()==QString("Subject")){
					Subject* subject=new Subject();
					assert(xmlReader.isStartElement());
					while(xmlReader.readNextStartElement()){
						xmlReadingLog+="   Found "+xmlReader.name().toString()+" tag\n";
						if(xmlReader.name()==QString("Name")){
							QString text=xmlReader.readElementText();
							subject->name=text;
							xmlReadingLog+="    Read subject name: "+subject->name+"\n";
						}
						else if(xmlReader.name()==QString("Comments")){
							QString text=xmlReader.readElementText();
							subject->comments=text;
							xmlReadingLog+="    Crt. subject comments="+subject->comments+"\n";
						}
						else{
							unrecognizedXmlTags.append(xmlReader.name().toString());
							unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
							unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

							xmlReader.skipCurrentElement();
							xmlReaderNumberOfUnrecognizedFields++;
						}
					}
					bool tmp2=subjectsRead.contains(subject->name);
					if(tmp2){
						RulesReconcilableMessage::warning(parent, tr("FET warning"),
						 tr("Duplicate subject %1 found - ignoring").arg(subject->name));
						xmlReadingLog+="   Subject not added - duplicate\n";
						
						delete subject;
					}
					else{
						subjectsRead.insert(subject->name);
						this->addSubjectFast(subject);
						tmp++;
						xmlReadingLog+="   Subject inserted\n";
					}
				}
				else{
					unrecognizedXmlTags.append(xmlReader.name().toString());
					unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
					unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

					xmlReader.skipCurrentElement();
					xmlReaderNumberOfUnrecognizedFields++;
				}
			}
			if(!(tmp==this->subjectsList.size()))
				xmlReader.raiseError(tr("%1 is incorrect").arg("Subjects_List"));
			else{
				assert(tmp==this->subjectsList.size());
				xmlReadingLog+="  Added "+CustomFETString::number(tmp)+" subjects\n";
				reducedXmlLog+="Added "+CustomFETString::number(tmp)+" subjects\n";
			}
		}
		else if(xmlReader.name()==QString("Subject_Tags_List")){
			QSet<QString> activityTagsRead;
		
			RulesReconcilableMessage::information(parent, tr("FET information"), tr("Your file contains subject tags list"
			  ", which is named in versions>=5.5.0 activity tags list"));
		
			int tmp=0;
			assert(xmlReader.isStartElement());
			while(xmlReader.readNextStartElement()){
				xmlReadingLog+="   Found "+xmlReader.name().toString()+" tag\n";
				if(xmlReader.name()==QString("Subject_Tag")){
					ActivityTag* activityTag=new ActivityTag();
					assert(xmlReader.isStartElement());
					while(xmlReader.readNextStartElement()){
						xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
						if(xmlReader.name()==QString("Name")){
							QString text=xmlReader.readElementText();
							activityTag->name=text;
							xmlReadingLog+="    Read activity tag name: "+activityTag->name+"\n";
						}
						else if(xmlReader.name()==QString("Comments")){
							QString text=xmlReader.readElementText();
							activityTag->comments=text;
							xmlReadingLog+="    Crt. activity tag comments="+activityTag->comments+"\n";
						}
						else{
							unrecognizedXmlTags.append(xmlReader.name().toString());
							unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
							unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

							xmlReader.skipCurrentElement();
							xmlReaderNumberOfUnrecognizedFields++;
						}
					}
					bool tmp2=activityTagsRead.contains(activityTag->name);
					if(tmp2){
						RulesReconcilableMessage::warning(parent, tr("FET warning"),
						 tr("Duplicate activity tag %1 found - ignoring").arg(activityTag->name));
						xmlReadingLog+="   Activity tag not added - duplicate\n";
						
						delete activityTag;
					}
					else{
						activityTagsRead.insert(activityTag->name);
						addActivityTagFast(activityTag);
						tmp++;
						xmlReadingLog+="   Activity tag inserted\n";
					}
				}
				else{
					unrecognizedXmlTags.append(xmlReader.name().toString());
					unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
					unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

					xmlReader.skipCurrentElement();
					xmlReaderNumberOfUnrecognizedFields++;
				}
			}
			if(!(tmp==this->activityTagsList.size()))
				xmlReader.raiseError(tr("%1 is incorrect").arg("Subject_Tags_List"));
			else{
				assert(tmp==this->activityTagsList.size());
				xmlReadingLog+="  Added "+CustomFETString::number(tmp)+" activity tags\n";
				reducedXmlLog+="Added "+CustomFETString::number(tmp)+" activity tags\n";
			}
		}
		else if(xmlReader.name()==QString("Activity_Tags_List")){
			QSet<QString> activityTagsRead;
		
			int tmp=0;
			assert(xmlReader.isStartElement());
			while(xmlReader.readNextStartElement()){
				xmlReadingLog+="   Found "+xmlReader.name().toString()+" tag\n";
				if(xmlReader.name()==QString("Activity_Tag")){
					ActivityTag* activityTag=new ActivityTag();
					assert(xmlReader.isStartElement());
					while(xmlReader.readNextStartElement()){
						xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
						if(xmlReader.name()==QString("Name")){
							QString text=xmlReader.readElementText();
							activityTag->name=text;
							xmlReadingLog+="    Read activity tag name: "+activityTag->name+"\n";
						}
						else if(xmlReader.name()==QString("Printable")){
							QString text=xmlReader.readElementText();
							if(text=="true")
								activityTag->printable=true;
							else
								activityTag->printable=false;
							xmlReadingLog+="    Read activity tag printable="+text+"\n";
						}
						else if(xmlReader.name()==QString("Comments")){
							QString text=xmlReader.readElementText();
							activityTag->comments=text;
							xmlReadingLog+="    Crt. activity tag comments="+activityTag->comments+"\n";
						}
						else{
							unrecognizedXmlTags.append(xmlReader.name().toString());
							unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
							unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

							xmlReader.skipCurrentElement();
							xmlReaderNumberOfUnrecognizedFields++;
						}
					}
					bool tmp2=activityTagsRead.contains(activityTag->name);
					if(tmp2){
						RulesReconcilableMessage::warning(parent, tr("FET warning"),
						 tr("Duplicate activity tag %1 found - ignoring").arg(activityTag->name));
						xmlReadingLog+="   Activity tag not added - duplicate\n";
						
						delete activityTag;
					}
					else{
						activityTagsRead.insert(activityTag->name);
						addActivityTagFast(activityTag);
						tmp++;
						xmlReadingLog+="   Activity tag inserted\n";
					}
				}
				else{
					unrecognizedXmlTags.append(xmlReader.name().toString());
					unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
					unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

					xmlReader.skipCurrentElement();
					xmlReaderNumberOfUnrecognizedFields++;
				}
			}
			if(!(tmp==this->activityTagsList.size()))
				xmlReader.raiseError(tr("%1 is incorrect").arg("Activity_Tags_List"));
			else{
				assert(tmp==this->activityTagsList.size());
				xmlReadingLog+="  Added "+CustomFETString::number(tmp)+" activity tags\n";
				reducedXmlLog+="Added "+CustomFETString::number(tmp)+" activity tags\n";
			}
		}
		else if(xmlReader.name()==QString("Students_List")){
			QSet<StudentsSet*> allAllocatedStudentsSets;
		
			bool okStudents=true;
	
			//permanentStudentsHash.clear();
		
			QHash<QString, StudentsSet*> currentStudentsHash;
		
			int tsgr=0;
			int tgr=0;
		
			int ny=0;
			assert(xmlReader.isStartElement());
			while(xmlReader.readNextStartElement()){
				xmlReadingLog+="   Found "+xmlReader.name().toString()+" tag\n";
				if(xmlReader.name()==QString("Year")){
					StudentsYear* sty=new StudentsYear();
					allAllocatedStudentsSets.insert(sty);
					int ng=0;
					
					//sty->divisions.clear();
					//sty->separator="";
					int readCategories=-1;
					int metCategories=0;
					
					QSet<QString> groupsInYear;

					bool tmp2=this->addYearFast(sty);
					assert(tmp2==true);
					ny++;

					assert(xmlReader.isStartElement());
					while(xmlReader.readNextStartElement()){
						xmlReadingLog+="   Found "+xmlReader.name().toString()+" tag\n";
						if(xmlReader.name()==QString("Name")){
							QString text=xmlReader.readElementText();
							if(!skipDuplicatedStudentsSets){
								QString nn=text;
								//StudentsSet* ss=this->searchStudentsSet(nn);
								StudentsSet* ss=currentStudentsHash.value(nn, nullptr);
								if(ss!=nullptr){
									QString str;
									
									if(ss->type==STUDENTS_YEAR)
										str=tr("Trying to add year %1, which is already added as another year.").arg(nn);
									else if(ss->type==STUDENTS_GROUP)
										str=tr("Trying to add year %1, which is already added as another group.").arg(nn);
									else if(ss->type==STUDENTS_SUBGROUP)
										str=tr("Trying to add year %1, which is already added as another subgroup.").arg(nn);
										
									xmlReader.raiseError(str);
									okStudents=false;

									//int t=1;
									
									/*str=tr("Could not read file - XML parse error at line %1, column %2:\n%3", "The error description is %3")
									 .arg(xmlReader.lineNumber()).arg(xmlReader.columnNumber()).arg(str);
								
									int t=RulesIrreconcilableMessage::warning(parent, tr("FET warning"), str,
									 tr("Skip rest"), tr("See next"), QString(),
									 1, 0 );
									
									if(okStudents){
										//xmlReader.raiseError(tr("Students structure error(s)."));
										okStudents=false;
									}*/
					
									/*if(t==0)
										skipDuplicatedStudentsSets=true;*/
								}
							}
						
							sty->name=text;
							if(!currentStudentsHash.contains(sty->name))
								currentStudentsHash.insert(sty->name, sty);
							xmlReadingLog+="    Read year name: "+sty->name+"\n";
						}
						else if(xmlReader.name()==QString("Number_of_Students")){
							QString text=xmlReader.readElementText();
							sty->numberOfStudents=text.toInt();
							xmlReadingLog+="    Read year number of students: "+CustomFETString::number(sty->numberOfStudents)+"\n";
						}
						else if(xmlReader.name()==QString("Comments")){
							QString text=xmlReader.readElementText();
							sty->comments=text;
							xmlReadingLog+="    Crt. year comments="+sty->comments+"\n";
						}
						
						//2020-09-03
						else if(xmlReader.name()==QString("Number_of_Categories")){
							QString text=xmlReader.readElementText();
							readCategories=text.toInt();
							xmlReadingLog+="    Read number of categories: "+CustomFETString::number(readCategories)+"\n";
						}
						else if(xmlReader.name()==QString("Category")){
							xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";

							metCategories++;
							
							int readDivisions=-1;
							int metDivisions=0;
							QStringList divisionsTL;

							assert(xmlReader.isStartElement());
							while(xmlReader.readNextStartElement()){
								xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
								if(xmlReader.name()==QString("Number_of_Divisions")){
									QString text=xmlReader.readElementText();
									readDivisions=text.toInt();
									xmlReadingLog+="     Read number of divisions: "+CustomFETString::number(readDivisions)+"\n";
								}
								else if(xmlReader.name()==QString("Division")){
									xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
									
									metDivisions++;
									
									QString text=xmlReader.readElementText();
									divisionsTL.append(text);
									xmlReadingLog+="     Read new division: "+divisionsTL.last()+"\n";
								}
								else{
									unrecognizedXmlTags.append(xmlReader.name().toString());
									unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
									unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

									xmlReader.skipCurrentElement();
									xmlReaderNumberOfUnrecognizedFields++;
								}
							}
							
							if(readDivisions==-1){
								if(metDivisions==0){
									//ok
								}
								else{
									xmlReader.raiseError(tr("The specified number of divisions was not found, but there were found specified the divisions."));
									okStudents=false;
								}
							}
							else if(readDivisions!=metDivisions){
								xmlReader.raiseError(tr("The specified number of divisions is not equal with the met number of divisions."));
								okStudents=false;
							}
							else{
								sty->divisions.append(divisionsTL);
							}
						}
						else if(xmlReader.name()==QString("Separator")){
							QString text=xmlReader.readElementText();
							sty->separator=text;
							xmlReadingLog+="    Read year separator="+sty->separator+"\n";
						}
						
						else if(xmlReader.name()==QString("Group")){
							QSet<StudentsSubgroup*> allocatedSubgroups;
						
							StudentsGroup* stg=new StudentsGroup();
							allAllocatedStudentsSets.insert(stg);
							int ns=0;

							QSet<QString> subgroupsInGroup;
							
							/*bool tmp4=this->addGroupFast(sty, stg);
							assert(tmp4==true);
							ng++;*/

							assert(xmlReader.isStartElement());
							while(xmlReader.readNextStartElement()){
								xmlReadingLog+="   Found "+xmlReader.name().toString()+" tag\n";
								if(xmlReader.name()==QString("Name")){
									QString text=xmlReader.readElementText();
									if(!skipDuplicatedStudentsSets){
										QString nn=text;
										StudentsSet* ss=currentStudentsHash.value(nn, nullptr);
										if(ss!=nullptr){
											QString str;
											bool haveError=false;
									
											if(ss->type==STUDENTS_YEAR){
												str=tr("Trying to add group %1, which is already added as another year.").arg(nn);
												haveError=true;
											}
											else if(ss->type==STUDENTS_GROUP){
												if(groupsInYear.contains(nn)){
													str=tr("Trying to add group %1 in year %2 but it is already added.").arg(nn).arg(sty->name);
													haveError=true;
												}
												else{
													str="";
													assert(haveError==false);
												}
											}
											else if(ss->type==STUDENTS_SUBGROUP){
												str=tr("Trying to add group %1, which is already added as another subgroup.").arg(nn);
												haveError=true;
											}
								
											//int t=1;
											if(haveError){
												xmlReader.raiseError(str);
												okStudents=false;

												/*str=tr("Could not read file - XML parse error at line %1, column %2:\n%3", "The error description is %3")
												 .arg(xmlReader.lineNumber()).arg(xmlReader.columnNumber()).arg(str);

												t=RulesIrreconcilableMessage::warning(parent, tr("FET warning"), str,
												 tr("Skip rest"), tr("See next"), QString(),
												 1, 0 );
												
												if(okStudents){
													//xmlReader.raiseError(tr("Students structure error(s)."));
													okStudents=false;
												}*/
											}
					
											//if(t==0)
											//	skipDuplicatedStudentsSets=true;
										}
									}
									
									groupsInYear.insert(text);

									if(currentStudentsHash.contains(text)){
										for(StudentsSubgroup* sts : qAsConst(allocatedSubgroups)){
											assert(currentStudentsHash.contains(sts->name));
											currentStudentsHash.remove(sts->name);
											
											assert(allAllocatedStudentsSets.contains(sts));
											allAllocatedStudentsSets.remove(sts);
											delete sts;
										}
										allocatedSubgroups.clear(); //not really needed
									
										assert(allAllocatedStudentsSets.contains(stg));
										allAllocatedStudentsSets.remove(stg);
										delete stg;
										
										if(okStudents){
											assert(currentStudentsHash.value(text)->type==STUDENTS_GROUP);
											stg=(StudentsGroup*)(currentStudentsHash.value(text));
	
											bool tmp4=this->addGroupFast(sty, stg);
											assert(tmp4==true);
											//ng++;
										}
										
										xmlReader.skipCurrentElement();
										break;
									}

									bool tmp4=this->addGroupFast(sty, stg);
									assert(tmp4==true);
									ng++;

									stg->name=text;
									if(!currentStudentsHash.contains(stg->name))
										currentStudentsHash.insert(stg->name, stg);
									xmlReadingLog+="     Read group name: "+stg->name+"\n";
								}
								else if(xmlReader.name()==QString("Number_of_Students")){
									QString text=xmlReader.readElementText();
									stg->numberOfStudents=text.toInt();
									xmlReadingLog+="     Read group number of students: "+CustomFETString::number(stg->numberOfStudents)+"\n";
								}
								else if(xmlReader.name()==QString("Comments")){
									QString text=xmlReader.readElementText();
									stg->comments=text;
									xmlReadingLog+="    Crt. group comments="+stg->comments+"\n";
								}
								else if(xmlReader.name()==QString("Subgroup")){
									StudentsSubgroup* sts=new StudentsSubgroup();
									allAllocatedStudentsSets.insert(sts);

									/*bool tmp6=this->addSubgroupFast(sty, stg, sts);
									assert(tmp6==true);
									ns++;*/

									assert(xmlReader.isStartElement());
									while(xmlReader.readNextStartElement()){
										xmlReadingLog+="   Found "+xmlReader.name().toString()+" tag\n";
										if(xmlReader.name()==QString("Name")){
											QString text=xmlReader.readElementText();
											if(!skipDuplicatedStudentsSets){
												QString nn=text;
												StudentsSet* ss=currentStudentsHash.value(nn, nullptr);
												if(ss!=nullptr){
													QString str;
													bool haveError=false;
									
													if(ss->type==STUDENTS_YEAR){
														str=tr("Trying to add subgroup %1, which is already added as another year.").arg(nn);
														haveError=true;
													}
													else if(ss->type==STUDENTS_GROUP){
														str=tr("Trying to add subgroup %1, which is already added as another group.").arg(nn);
														haveError=true;
													}
													else if(ss->type==STUDENTS_SUBGROUP){
														if(subgroupsInGroup.contains(nn)){
															str=tr("Trying to add subgroup %1 in year %2, group %3 but it is already added.").arg(nn).arg(sty->name).arg(stg->name);
															haveError=true;
														}
														else{
															str="";
															assert(haveError==false);
														}
													}
								
													//int t=1;
													if(haveError){
														xmlReader.raiseError(str);
														okStudents=false;

														/*str=tr("Could not read file - XML parse error at line %1, column %2:\n%3", "The error description is %3")
														 .arg(xmlReader.lineNumber()).arg(xmlReader.columnNumber()).arg(str);

														t=RulesIrreconcilableMessage::warning(parent, tr("FET warning"), str,
														 tr("Skip rest"), tr("See next"), QString(),
														 1, 0 );
														
														if(okStudents){
															//xmlReader.raiseError(tr("Students structure error(s)."));
															okStudents=false;
														}*/
													}
							
													//if(t==0)
													//	skipDuplicatedStudentsSets=true;
												}
											}
											
											subgroupsInGroup.insert(text);

											if(currentStudentsHash.contains(text)){
												assert(allAllocatedStudentsSets.contains(sts));
												allAllocatedStudentsSets.remove(sts);
												delete sts;
												
												if(okStudents){
													assert(currentStudentsHash.value(text)->type==STUDENTS_SUBGROUP);
													sts=(StudentsSubgroup*)(currentStudentsHash.value(text));
	
													bool tmp6=this->addSubgroupFast(sty, stg, sts);
													assert(tmp6==true);
													//ns++;
												}
												
												xmlReader.skipCurrentElement();
												break;
											}
											else{
												allocatedSubgroups.insert(sts);
											}

											bool tmp6=this->addSubgroupFast(sty, stg, sts);
											assert(tmp6==true);
											ns++;
											
											sts->name=text;
											if(!currentStudentsHash.contains(sts->name))
												currentStudentsHash.insert(sts->name, sts);
											xmlReadingLog+="     Read subgroup name: "+sts->name+"\n";
										}
										else if(xmlReader.name()==QString("Number_of_Students")){
											QString text=xmlReader.readElementText();
											sts->numberOfStudents=text.toInt();
											xmlReadingLog+="     Read subgroup number of students: "+CustomFETString::number(sts->numberOfStudents)+"\n";
										}
										else if(xmlReader.name()==QString("Comments")){
											QString text=xmlReader.readElementText();
											sts->comments=text;
											xmlReadingLog+="    Crt. subgroup comments="+sts->comments+"\n";
										}
										else{
											unrecognizedXmlTags.append(xmlReader.name().toString());
											unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
											unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

											xmlReader.skipCurrentElement();
											xmlReaderNumberOfUnrecognizedFields++;
										}
									}
								}
								else{
									unrecognizedXmlTags.append(xmlReader.name().toString());
									unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
									unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

									xmlReader.skipCurrentElement();
									xmlReaderNumberOfUnrecognizedFields++;
								}
							}
							if(ns == stg->subgroupsList.size()){
								xmlReadingLog+="    Added "+CustomFETString::number(ns)+" subgroups\n";
								tsgr+=ns;
								//reducedXmlLog+="		Added "+CustomFETString::number(ns)+" subgroups\n";
							}
						}
						else{
							unrecognizedXmlTags.append(xmlReader.name().toString());
							unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
							unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

							xmlReader.skipCurrentElement();
							xmlReaderNumberOfUnrecognizedFields++;
						}
					}
					if(ng == sty->groupsList.size()){
						xmlReadingLog+="   Added "+CustomFETString::number(ng)+" groups\n";
						tgr+=ng;
						//reducedXmlLog+="	Added "+CustomFETString::number(ng)+" groups\n";
					}
					
					if(readCategories==-1){
						if(metCategories==0){
							//ok
						}
						else{
							xmlReader.raiseError(tr("The specified number of categories was not found, but there were found specified the categories."));
							okStudents=false;
						}
					}
					else if(readCategories!=metCategories){
						xmlReader.raiseError(tr("The specified number of categories is not equal with the met number of categories."));
						okStudents=false;
					}
					else{
						//ok
					}
				}
				else{
					unrecognizedXmlTags.append(xmlReader.name().toString());
					unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
					unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

					xmlReader.skipCurrentElement();
					xmlReaderNumberOfUnrecognizedFields++;
				}
			}
			xmlReadingLog+="  Added "+CustomFETString::number(ny)+" years\n";
			reducedXmlLog+="Added "+CustomFETString::number(ny)+" students years\n";
			//reducedXmlLog+="Added "+CustomFETString::number(tgr)+" students groups (see note below)\n";
			reducedXmlLog+="Added "+CustomFETString::number(tgr)+" students groups\n";
			//reducedXmlLog+="Added "+CustomFETString::number(tsgr)+" students subgroups (see note below)\n";
			reducedXmlLog+="Added "+CustomFETString::number(tsgr)+" students subgroups\n";
			assert(this->yearsList.size()==ny);
			
			if(okStudents){
				//This is redundant, but I make this an additional test, just in case anything was wrong.
				computePermanentStudentsHash();
				assert(permanentStudentsHash==currentStudentsHash);
			}
			else{
				for(StudentsSet* studentsSet : qAsConst(allAllocatedStudentsSets))
					delete studentsSet;
				yearsList.clear();
			}
		}
		else if(xmlReader.name()==QString("Activities_List")){
			QSet<QString> allTeachers;
			QHash<QString, int> studentsSetsCount;
			QSet<QString> allSubjects;
			QSet<QString> allActivityTags;
			
			for(Teacher* tch : qAsConst(this->teachersList))
				allTeachers.insert(tch->name);

			for(Subject* sbj : qAsConst(this->subjectsList))
				allSubjects.insert(sbj->name);

			for(ActivityTag* at : qAsConst(this->activityTagsList))
				allActivityTags.insert(at->name);

			for(StudentsYear* year : qAsConst(this->yearsList)){
				if(!studentsSetsCount.contains(year->name))
					studentsSetsCount.insert(year->name, year->numberOfStudents);
				else if(studentsSetsCount.value(year->name)!=year->numberOfStudents){
					//cout<<"Mistake: year "<<qPrintable(year->name)<<" appears in more places with different number of students"<<endl;
				}

				for(StudentsGroup* group : qAsConst(year->groupsList)){
					if(!studentsSetsCount.contains(group->name))
						studentsSetsCount.insert(group->name, group->numberOfStudents);
					else if(studentsSetsCount.value(group->name)!=group->numberOfStudents){
						//cout<<"Mistake: group "<<qPrintable(group->name)<<" appears in more places with different number of students"<<endl;
					}
			
					for(StudentsSubgroup* subgroup : qAsConst(group->subgroupsList)){
						if(!studentsSetsCount.contains(subgroup->name))
							studentsSetsCount.insert(subgroup->name, subgroup->numberOfStudents);
						else if(studentsSetsCount.value(subgroup->name)!=subgroup->numberOfStudents){
							//cout<<"Mistake: subgroup "<<qPrintable(subgroup->name)<<" appears in more places with different number of students"<<endl;
						}
					}
				}
			}

			//int nchildrennodes=elem2.childNodes().length();
			
			/*QProgressDialog progress(parent);
			progress.setLabelText(tr("Loading activities ... please wait"));
			progress.setRange(0, nchildrennodes);
			progress.setModal(true);*/
			//progress.setCancelButton(parent);
			
			//int ttt=0;
		
			int na=0;
			assert(xmlReader.isStartElement());
			while(xmlReader.readNextStartElement()){
			
				/*progress.setValue(ttt);
				pqapplication->processEvents();
				if(progress.wasCanceled()){
					QMessageBox::information(parent, tr("FET information"), tr("Interrupted - only partial file was loaded"));
					return true;
				}
				
				ttt++;*/
			
				xmlReadingLog+="   Found "+xmlReader.name().toString()+" tag\n";
				if(xmlReader.name()==QString("Activity")){
					bool correct=true;
				
					QString cm=QString(""); //comments
					QString tn="";
					QStringList tl;
					QString sjn="";
					QString atn="";
					QStringList atl;
					QString stn="";
					QStringList stl;
					//int p=PARITY_NOT_INITIALIZED;
					int td=-1;
					int d=-1;
					int id=-1;
					int gid=-1;
					bool ac=true;
					int nos=-1;
					bool cnos=true;
					
					QSet<QString> _teachersSet;
					QSet<QString> _studentsSet;
					QSet<QString> _activityTagsSet;
					int _duplicateTeachersCount=0;
					int _duplicateStudentsCount=0;
					int _duplicateActivityTagsCount=0;
					
					assert(xmlReader.isStartElement());
					while(xmlReader.readNextStartElement()){
						xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
						if(xmlReader.name()==QString("Weekly")){
							xmlReader.skipCurrentElement();
							xmlReadingLog+="    Current activity is weekly - ignoring tag\n";
							//assert(p==PARITY_NOT_INITIALIZED);
							//p=PARITY_WEEKLY;
						}
						//old tag
						else if(xmlReader.name()==QString("Biweekly")){
							xmlReader.skipCurrentElement();
							xmlReadingLog+="    Current activity is fortnightly - ignoring tag\n";
							//assert(p==PARITY_NOT_INITIALIZED);
							//p=PARITY_FORTNIGHTLY;
						}
						else if(xmlReader.name()==QString("Fortnightly")){
							xmlReader.skipCurrentElement();
							xmlReadingLog+="    Current activity is fortnightly - ignoring tag\n";
							//assert(p==PARITY_NOT_INITIALIZED);
							//p=PARITY_FORTNIGHTLY;
						}
						else if(xmlReader.name()==QString("Active")){
							QString text=xmlReader.readElementText();
							if(text=="yes" || text=="true" || text=="1"){
								ac=true;
								xmlReadingLog+="	Current activity is active\n";
							}
							else{
								if(!(text=="no" || text=="false" || text=="0")){
									RulesReconcilableMessage::warning(parent, tr("FET warning"),
									 tr("Found activity active tag which is not 'true', 'false', 'yes', 'no', '1' or '0'."
									 " The activity will be considered not active",
									 "Instructions for translators: please leave the 'true', 'false', 'yes' and 'no' fields untranslated, as they are in English"));
								}
								//assert(text=="no" || text=="false" || text=="0");
								ac=false;
								xmlReadingLog+="	Current activity is not active\n";
							}
						}
						else if(xmlReader.name()==QString("Comments")){
							QString text=xmlReader.readElementText();
							cm=text;
							xmlReadingLog+="    Crt. activity comments="+cm+"\n";
						}
						else if(xmlReader.name()==QString("Teacher")){
							QString text=xmlReader.readElementText();
							tn=text;
							xmlReadingLog+="    Crt. activity teacher="+tn+"\n";
							
							if(_teachersSet.contains(tn))
								_duplicateTeachersCount++;
							else
								_teachersSet.insert(tn);
							
							tl.append(tn);
							if(!allTeachers.contains(tn))
							//if(this->searchTeacher(tn)<0)
								correct=false;
						}
						else if(xmlReader.name()==QString("Subject")){
							QString text=xmlReader.readElementText();
							sjn=text;
							xmlReadingLog+="    Crt. activity subject="+sjn+"\n";
							if(!allSubjects.contains(sjn))
							//if(this->searchSubject(sjn)<0)
								correct=false;
						}
						else if(xmlReader.name()==QString("Subject_Tag")){
							QString text=xmlReader.readElementText();
							atn=text;
							xmlReadingLog+="    Crt. activity activity_tag="+atn+"\n";

							if(_activityTagsSet.contains(atn))
								_duplicateActivityTagsCount++;
							else
								_activityTagsSet.insert(atn);
							

							if(atn!="")
								atl.append(atn);
							if(atn!="" && !allActivityTags.contains(atn))
							//if(atn!="" && this->searchActivityTag(atn)<0)
								correct=false;
						}
						else if(xmlReader.name()==QString("Activity_Tag")){
							QString text=xmlReader.readElementText();
							atn=text;
							xmlReadingLog+="    Crt. activity activity_tag="+atn+"\n";

							if(_activityTagsSet.contains(atn))
								_duplicateActivityTagsCount++;
							else
								_activityTagsSet.insert(atn);
							
							if(atn!="")
								atl.append(atn);
							if(atn!="" && !allActivityTags.contains(atn))
							//if(atn!="" && this->searchActivityTag(atn)<0)
								correct=false;
						}
						else if(xmlReader.name()==QString("Students")){
							QString text=xmlReader.readElementText();
							stn=text;
							xmlReadingLog+="    Crt. activity students+="+stn+"\n";

							if(_studentsSet.contains(stn))
								_duplicateStudentsCount++;
							else
								_studentsSet.insert(stn);

							stl.append(stn);
							if(!studentsSetsCount.contains(stn))
							//if(this->searchStudentsSet(stn)==nullptr)
								correct=false;
						}
						else if(xmlReader.name()==QString("Duration")){
							QString text=xmlReader.readElementText();
							d=text.toInt();
							xmlReadingLog+="    Crt. activity duration="+CustomFETString::number(d)+"\n";
						}
						else if(xmlReader.name()==QString("Total_Duration")){
							QString text=xmlReader.readElementText();
							td=text.toInt();
							xmlReadingLog+="    Crt. activity total duration="+CustomFETString::number(td)+"\n";
						}
						else if(xmlReader.name()==QString("Id")){
							QString text=xmlReader.readElementText();
							id=text.toInt();
							xmlReadingLog+="    Crt. activity id="+CustomFETString::number(id)+"\n";
						}
						else if(xmlReader.name()==QString("Activity_Group_Id")){
							QString text=xmlReader.readElementText();
							gid=text.toInt();
							xmlReadingLog+="    Crt. activity group id="+CustomFETString::number(gid)+"\n";
						}
						else if(xmlReader.name()==QString("Number_Of_Students")){
							QString text=xmlReader.readElementText();
							cnos=false;
							nos=text.toInt();
							xmlReadingLog+="    Crt. activity number of students="+CustomFETString::number(nos)+"\n";
						}
						else{
							unrecognizedXmlTags.append(xmlReader.name().toString());
							unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
							unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

							xmlReader.skipCurrentElement();
							xmlReaderNumberOfUnrecognizedFields++;
						}
					}
					if(id<0)
						xmlReader.raiseError(tr("%1 is incorrect").arg("Id"));
					else if(activitiesPointerHash.contains(id))
						xmlReader.raiseError(tr("%1 is incorrect (already existing)").arg("Id")); //duplicate Id
					else if(gid<0)
						xmlReader.raiseError(tr("%1 is incorrect").arg("Activity_Group_Id"));
					else if(d<=0)
						xmlReader.raiseError(tr("%1 is incorrect").arg("Duration"));
					else if(_duplicateTeachersCount>0)
						xmlReader.raiseError(tr("Activity with Id=%1 contains %2 duplicate teachers - please correct that").arg(id).arg(_duplicateTeachersCount));
					else if(_duplicateStudentsCount>0)
						xmlReader.raiseError(tr("Activity with Id=%1 contains %2 duplicate students sets - please correct that").arg(id).arg(_duplicateStudentsCount));
					else if(_duplicateActivityTagsCount>0)
						xmlReader.raiseError(tr("Activity with Id=%1 contains %2 duplicate activity tags - please correct that").arg(id).arg(_duplicateActivityTagsCount));
					else if(correct){
						assert(id>=0 && gid>=0);
						assert(d>0);
						if(td<0)
							td=d;
						
						if(cnos==true){
							assert(nos==-1);
							int _ns=0;
							for(const QString& _s : qAsConst(stl)){
								assert(studentsSetsCount.contains(_s));
								_ns+=studentsSetsCount.value(_s);
							}
							this->addSimpleActivityFast(parent, id, gid, tl, sjn, atl, stl,
								d, td, ac, cnos, nos, _ns);
						}
						else{
							this->addSimpleActivityFast(parent, id, gid, tl, sjn, atl, stl,
								d, td, ac, cnos, nos, nos);
						}
						
						this->activitiesList[this->activitiesList.count()-1]->comments=cm;
						
						na++;
						xmlReadingLog+="   Added the activity\n";
					}
					else{
						xmlReader.raiseError(tr("The activity with id=%1 contains incorrect data").arg(id));
						/*xmlReadingLog+="   Activity with id ="+CustomFETString::number(id)+" contains invalid data - skipping\n";
						RulesReconcilableMessage::warning(parent, tr("FET information"),
						 tr("Activity with id=%1 contains invalid data - skipping").arg(id));*/
					}
				}
				else{
					unrecognizedXmlTags.append(xmlReader.name().toString());
					unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
					unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

					xmlReader.skipCurrentElement();
					xmlReaderNumberOfUnrecognizedFields++;
				}
			}
			xmlReadingLog+="  Added "+CustomFETString::number(na)+" activities\n";
			reducedXmlLog+="Added "+CustomFETString::number(na)+" activities\n";
		}
		else if(xmlReader.name()==QString("Equipments_List")){
			RulesReconcilableMessage::warning(parent, tr("FET warning"),
			 tr("File contains deprecated equipments list - will be ignored"));
			xmlReader.skipCurrentElement();
			//NOT! xmlReaderNumberOfUnrecognizedFields++; because this entry was once allowed
		}
		else if(xmlReader.name()==QString("Buildings_List")){
			QSet<QString> buildingsRead;
		
			int tmp=0;
			assert(xmlReader.isStartElement());
			while(xmlReader.readNextStartElement()){
				xmlReadingLog+="   Found "+xmlReader.name().toString()+" tag\n";
				if(xmlReader.name()==QString("Building")){
					Building* bu=new Building();
					bu->name="";
					
					assert(xmlReader.isStartElement());
					while(xmlReader.readNextStartElement()){
						xmlReadingLog+="   Found "+xmlReader.name().toString()+" tag\n";
						if(xmlReader.name()==QString("Name")){
							QString text=xmlReader.readElementText();
							bu->name=text;
							xmlReadingLog+="    Read building name: "+bu->name+"\n";
						}
						else if(xmlReader.name()==QString("Comments")){
							QString text=xmlReader.readElementText();
							bu->comments=text;
							xmlReadingLog+="    Crt. building comments="+bu->comments+"\n";
						}
						else{
							unrecognizedXmlTags.append(xmlReader.name().toString());
							unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
							unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

							xmlReader.skipCurrentElement();
							xmlReaderNumberOfUnrecognizedFields++;
						}
					}

					bool tmp2=buildingsRead.contains(bu->name);
					if(tmp2){
						RulesReconcilableMessage::warning(parent, tr("FET warning"),
						 tr("Duplicate building %1 found - ignoring").arg(bu->name));
						xmlReadingLog+="   Building not added - duplicate\n";
						
						delete bu;
					}
					else{
						buildingsRead.insert(bu->name);
						addBuildingFast(bu);
						tmp++;
						xmlReadingLog+="   Building inserted\n";
					}
				}
				else{
					unrecognizedXmlTags.append(xmlReader.name().toString());
					unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
					unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

					xmlReader.skipCurrentElement();
					xmlReaderNumberOfUnrecognizedFields++;
				}
			}
			if(!(tmp==this->buildingsList.size()))
				xmlReader.raiseError(tr("%1 is incorrect").arg("Buildings_List"));
			else{
				assert(tmp==this->buildingsList.size());
				xmlReadingLog+="  Added "+CustomFETString::number(tmp)+" buildings\n";
				reducedXmlLog+="Added "+CustomFETString::number(tmp)+" buildings\n";
			}
		}
		else if(xmlReader.name()==QString("Rooms_List")){
			QSet<QString> roomsRead;
		
			int tmp=0;
			assert(xmlReader.isStartElement());
			while(xmlReader.readNextStartElement()){
				xmlReadingLog+="   Found "+xmlReader.name().toString()+" tag\n";
				if(xmlReader.name()==QString("Room")){
					Room* rm=new Room();
					rm->name="";
					rm->capacity=MAX_ROOM_CAPACITY; //infinite, if not specified
					rm->building="";
					rm->isVirtual=false;
					rm->realRoomsSetsList.clear();
					int specifiedNumberOfSetsOfRooms=-1;
					int metNumberOfSetsOfRooms=0;
					bool correct=true;
					assert(xmlReader.isStartElement());
					while(xmlReader.readNextStartElement()){
						xmlReadingLog+="   Found "+xmlReader.name().toString()+" tag\n";
						if(xmlReader.name()==QString("Name")){
							QString text=xmlReader.readElementText();
							rm->name=text;
							xmlReadingLog+="    Read room name: "+rm->name+"\n";
						}
						else if(xmlReader.name()==QString("Type")){
							//rm->type=text;
							xmlReader.skipCurrentElement();
							xmlReadingLog+="    Ignoring old tag room type:\n";
						}
						else if(xmlReader.name()==QString("Capacity")){
							QString text=xmlReader.readElementText();
							rm->capacity=text.toInt();
							xmlReadingLog+="    Read room capacity: "+CustomFETString::number(rm->capacity)+"\n";
						}
						else if(xmlReader.name()==QString("Virtual")){
							QString text=xmlReader.readElementText();
							if(text=="true"){
								rm->isVirtual=true;
							}
						}
						else if(xmlReader.name()==QString("Number_of_Sets_of_Real_Rooms")){
							QString text=xmlReader.readElementText();
							specifiedNumberOfSetsOfRooms=text.toInt();
						}
						else if(xmlReader.name()==QString("Set_of_Real_Rooms")){
							metNumberOfSetsOfRooms++;
							
							QStringList rl;
							
							int metNumberOfRooms=0;
							int specifiedNumberOfRooms=-1;
							assert(xmlReader.isStartElement());
							while(xmlReader.readNextStartElement()){
								if(xmlReader.name()==QString("Number_of_Real_Rooms")){
									QString text=xmlReader.readElementText();
									specifiedNumberOfRooms=text.toInt();
								}
								else if(xmlReader.name()==QString("Real_Room")){
									metNumberOfRooms++;
									QString text=xmlReader.readElementText();
									rl.append(text);
								}
								else{
									unrecognizedXmlTags.append(xmlReader.name().toString());
									unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
									unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

									xmlReader.skipCurrentElement();
									xmlReaderNumberOfUnrecognizedFields++;
								}
							}
							
							if(specifiedNumberOfRooms==-1){
								xmlReader.raiseError(tr("The specified number of real rooms was not found in the input file"
								 " for the virtual room %1, for the set of real rooms number %2").arg(rm->name).arg(metNumberOfSetsOfRooms));
								correct=false;
							}
							else if(metNumberOfRooms!=specifiedNumberOfRooms){
								xmlReader.raiseError(tr("The specified number of real rooms is not equal with the met number of real rooms"
								 " for the virtual room %1").arg(rm->name));
								correct=false;
							}
							else{
								rm->realRoomsSetsList.append(rl);
							}
						}
						else if(xmlReader.name()==QString("Equipment")){
							//rm->addEquipment(text);
							xmlReader.skipCurrentElement();
							xmlReadingLog+="    Ignoring old tag - room equipment:\n";
						}
						else if(xmlReader.name()==QString("Building")){
							QString text=xmlReader.readElementText();
							rm->building=text;
							xmlReadingLog+="    Read room building:\n"+rm->building;
						}
						else if(xmlReader.name()==QString("Comments")){
							QString text=xmlReader.readElementText();
							rm->comments=text;
							xmlReadingLog+="    Crt. room comments="+rm->comments+"\n";
						}
						else{
							unrecognizedXmlTags.append(xmlReader.name().toString());
							unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
							unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

							xmlReader.skipCurrentElement();
							xmlReaderNumberOfUnrecognizedFields++;
						}
					}
					
					if(!correct)
						delete rm;
					
					if(correct){
						if(rm->isVirtual){
							if(specifiedNumberOfSetsOfRooms==-1){
								xmlReader.raiseError(tr("The specified number of sets of real rooms was not found in the input file"
								 " for the virtual room %1").arg(rm->name));
								correct=false;
								
								delete rm;
							}
							else if(metNumberOfSetsOfRooms!=specifiedNumberOfSetsOfRooms){
								xmlReader.raiseError(tr("The specified number of sets of real rooms is not equal with the met number of sets of real rooms"
								 " for the virtual room %1").arg(rm->name));
								correct=false;
								
								delete rm;
							}
						}
					}
					
					if(correct){
						bool tmp2=roomsRead.contains(rm->name);
						if(tmp2){
							RulesReconcilableMessage::warning(parent, tr("FET warning"),
							 tr("Duplicate room %1 found - ignoring").arg(rm->name));
							xmlReadingLog+="   Room not added - duplicate\n";
							
							delete rm;
						}
						else{
							roomsRead.insert(rm->name);
							addRoomFast(rm);
							tmp++;
							xmlReadingLog+="   Room inserted\n";
						}
					}
				}
				else{
					unrecognizedXmlTags.append(xmlReader.name().toString());
					unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
					unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

					xmlReader.skipCurrentElement();
					xmlReaderNumberOfUnrecognizedFields++;
				}
			}
			if(!(tmp==this->roomsList.size()))
				xmlReader.raiseError(tr("%1 is incorrect").arg("Rooms_List"));
			else{
				assert(tmp==this->roomsList.size());
				xmlReadingLog+="  Added "+CustomFETString::number(tmp)+" rooms\n";
				reducedXmlLog+="Added "+CustomFETString::number(tmp)+" rooms\n";
			}
		}
		else if(xmlReader.name()==QString("Time_Constraints_List")){
			bool reportMaxBeginningsAtSecondHourChange=true;
			bool reportMaxGapsChange=true;
			bool reportStudentsSetNotAvailableChange=true;
			bool reportTeacherNotAvailableChange=true;
			bool reportBreakChange=true;
			
			bool reportActivityPreferredTimeChange=true;
			
			bool reportActivityPreferredTimesChange=true;
			bool reportActivitiesPreferredTimesChange=true;
			
			bool reportUnspecifiedPermanentlyLockedTime=true;
			
			bool reportUnspecifiedDayOrHourPreferredStartingTime=true;
			
#if 0
			bool reportIncorrectMinDays=true;
#endif
		
			bool seeNextWarnNotAddedTimeConstraint=true;
			
			int nc=0;
			TimeConstraint *crt_constraint;
			assert(xmlReader.isStartElement());
			
			while(xmlReader.readNextStartElement()){
				xmlReadingLog+="   Found "+xmlReader.name().toString()+" tag\n";
				crt_constraint=nullptr;
				if(xmlReader.name()==QString("ConstraintBasicCompulsoryTime")){
					crt_constraint=readBasicCompulsoryTime(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeacherNotAvailable")){
					if(reportTeacherNotAvailableChange){
						int t=RulesReconcilableMessage::information(parent, tr("FET information"),
						 tr("File contains constraint teacher not available, which is old (it was improved in FET 5.5.0), and will be converted"
						 " to the similar constraint of this type, constraint teacher not available times (a matrix)."),
						  tr("Skip rest"), tr("See next"), QString(), 1, 0 );
						if(t==0)
							reportTeacherNotAvailableChange=false;
					}

					crt_constraint=readTeacherNotAvailable(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeacherNotAvailableTimes")){
					crt_constraint=readTeacherNotAvailableTimes(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeacherMaxDaysPerWeek")){
					crt_constraint=readTeacherMaxDaysPerWeek(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeachersMaxDaysPerWeek")){
					crt_constraint=readTeachersMaxDaysPerWeek(xmlReader, xmlReadingLog);
				}

				else if(xmlReader.name()==QString("ConstraintTeacherMinDaysPerWeek")){
					crt_constraint=readTeacherMinDaysPerWeek(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeachersMinDaysPerWeek")){
					crt_constraint=readTeachersMinDaysPerWeek(xmlReader, xmlReadingLog);
				}

				else if(xmlReader.name()==QString("ConstraintTeacherIntervalMaxDaysPerWeek")){
					crt_constraint=readTeacherIntervalMaxDaysPerWeek(parent, xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeachersIntervalMaxDaysPerWeek")){
					crt_constraint=readTeachersIntervalMaxDaysPerWeek(parent, xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsSetMaxDaysPerWeek")){
					crt_constraint=readStudentsSetMaxDaysPerWeek(parent, xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsMaxDaysPerWeek")){
					crt_constraint=readStudentsMaxDaysPerWeek(parent, xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsSetIntervalMaxDaysPerWeek")){
					crt_constraint=readStudentsSetIntervalMaxDaysPerWeek(parent, xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsIntervalMaxDaysPerWeek")){
					crt_constraint=readStudentsIntervalMaxDaysPerWeek(parent, xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsSetNotAvailable")){
					if(reportStudentsSetNotAvailableChange){
						int t=RulesReconcilableMessage::information(parent, tr("FET information"),
						 tr("File contains constraint students set not available, which is old (it was improved in FET 5.5.0), and will be converted"
						 " to the similar constraint of this type, constraint students set not available times (a matrix)."),
						  tr("Skip rest"), tr("See next"), QString(), 1, 0 );
						if(t==0)
							reportStudentsSetNotAvailableChange=false;
					}

					crt_constraint=readStudentsSetNotAvailable(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsSetNotAvailableTimes")){
					crt_constraint=readStudentsSetNotAvailableTimes(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintMinNDaysBetweenActivities")){
					crt_constraint=readMinNDaysBetweenActivities(parent, xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintMinDaysBetweenActivities")){
					crt_constraint=readMinDaysBetweenActivities(parent, xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintMaxDaysBetweenActivities")){
					crt_constraint=readMaxDaysBetweenActivities(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintMinGapsBetweenActivities")){
					crt_constraint=readMinGapsBetweenActivities(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintActivitiesNotOverlapping")){
					crt_constraint=readActivitiesNotOverlapping(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintActivityTagsNotOverlapping")){
					crt_constraint=readActivityTagsNotOverlapping(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintActivitiesSameStartingTime")){
					crt_constraint=readActivitiesSameStartingTime(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintActivitiesSameStartingHour")){
					crt_constraint=readActivitiesSameStartingHour(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintActivitiesSameStartingDay")){
					crt_constraint=readActivitiesSameStartingDay(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeachersMaxHoursDaily")){
					crt_constraint=readTeachersMaxHoursDaily(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeacherMaxHoursDaily")){
					crt_constraint=readTeacherMaxHoursDaily(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeachersMaxHoursContinuously")){
					crt_constraint=readTeachersMaxHoursContinuously(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeacherMaxHoursContinuously")){
					crt_constraint=readTeacherMaxHoursContinuously(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeacherActivityTagMaxHoursContinuously")){
					crt_constraint=readTeacherActivityTagMaxHoursContinuously(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeachersActivityTagMaxHoursContinuously")){
					crt_constraint=readTeachersActivityTagMaxHoursContinuously(xmlReader, xmlReadingLog);
				}

				else if(xmlReader.name()==QString("ConstraintTeacherActivityTagMaxHoursDaily")){
					crt_constraint=readTeacherActivityTagMaxHoursDaily(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeachersActivityTagMaxHoursDaily")){
					crt_constraint=readTeachersActivityTagMaxHoursDaily(xmlReader, xmlReadingLog);
				}

				else if(xmlReader.name()==QString("ConstraintTeacherActivityTagMinHoursDaily")){
					crt_constraint=readTeacherActivityTagMinHoursDaily(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeachersActivityTagMinHoursDaily")){
					crt_constraint=readTeachersActivityTagMinHoursDaily(xmlReader, xmlReadingLog);
				}

				else if(xmlReader.name()==QString("ConstraintTeachersMinHoursDaily")){
					crt_constraint=readTeachersMinHoursDaily(parent, xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeacherMinHoursDaily")){
					crt_constraint=readTeacherMinHoursDaily(parent, xmlReader, xmlReadingLog);
				}
				else if((xmlReader.name()==QString("ConstraintTeachersSubgroupsMaxHoursDaily")
				 //TODO: erase the line below. It is only kept for compatibility with older versions
				 || xmlReader.name()==QString("ConstraintTeachersSubgroupsNoMoreThanXHoursDaily")) && !skipDeprecatedConstraints){
					int t=RulesReconcilableMessage::warning(parent, tr("FET warning"),
					 tr("File contains deprecated constraint teachers subgroups max hours daily - will be ignored"),
					 tr("Skip rest"), tr("See next"), QString(),
					 1, 0 );
					
					if(t==0)
						skipDeprecatedConstraints=true;
					crt_constraint=nullptr;
					xmlReader.skipCurrentElement();
				}
				else if(xmlReader.name()==QString("ConstraintStudentsNHoursDaily") && !skipDeprecatedConstraints){
					int t=RulesReconcilableMessage::warning(parent, tr("FET warning"),
					 tr("File contains deprecated constraint students n hours daily - will be ignored"),
					 tr("Skip rest"), tr("See next"), QString(),
					 1, 0 );
					
					if(t==0)
						skipDeprecatedConstraints=true;
					crt_constraint=nullptr;
					xmlReader.skipCurrentElement();
				}
				else if(xmlReader.name()==QString("ConstraintStudentsSetNHoursDaily") && !skipDeprecatedConstraints){
					int t=RulesReconcilableMessage::warning(parent, tr("FET warning"),
					 tr("File contains deprecated constraint students set n hours daily - will be ignored"),
					 tr("Skip rest"), tr("See next"), QString(),
					 1, 0 );
					
					if(t==0)
						skipDeprecatedConstraints=true;
					crt_constraint=nullptr;
					xmlReader.skipCurrentElement();
				}
				else if(xmlReader.name()==QString("ConstraintStudentsMaxHoursDaily")){
					crt_constraint=readStudentsMaxHoursDaily(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsSetMaxHoursDaily")){
					crt_constraint=readStudentsSetMaxHoursDaily(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsMaxHoursContinuously")){
					crt_constraint=readStudentsMaxHoursContinuously(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsSetMaxHoursContinuously")){
					crt_constraint=readStudentsSetMaxHoursContinuously(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsSetActivityTagMaxHoursContinuously")){
					crt_constraint=readStudentsSetActivityTagMaxHoursContinuously(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsActivityTagMaxHoursContinuously")){
					crt_constraint=readStudentsActivityTagMaxHoursContinuously(xmlReader, xmlReadingLog);
				}

				else if(xmlReader.name()==QString("ConstraintStudentsSetActivityTagMaxHoursDaily")){
					crt_constraint=readStudentsSetActivityTagMaxHoursDaily(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsActivityTagMaxHoursDaily")){
					crt_constraint=readStudentsActivityTagMaxHoursDaily(xmlReader, xmlReadingLog);
				}

				else if(xmlReader.name()==QString("ConstraintStudentsSetActivityTagMinHoursDaily")){
					crt_constraint=readStudentsSetActivityTagMinHoursDaily(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsActivityTagMinHoursDaily")){
					crt_constraint=readStudentsActivityTagMinHoursDaily(xmlReader, xmlReadingLog);
				}

				else if(xmlReader.name()==QString("ConstraintStudentsMinHoursDaily")){
					crt_constraint=readStudentsMinHoursDaily(parent, xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsSetMinHoursDaily")){
					crt_constraint=readStudentsSetMinHoursDaily(parent, xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTags")){
					crt_constraint=readStudentsSetMinGapsBetweenOrderedPairOfActivityTags(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTags")){
					crt_constraint=readStudentsMinGapsBetweenOrderedPairOfActivityTags(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTags")){
					crt_constraint=readTeacherMinGapsBetweenOrderedPairOfActivityTags(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTags")){
					crt_constraint=readTeachersMinGapsBetweenOrderedPairOfActivityTags(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintActivityPreferredTime")){
					if(reportActivityPreferredTimeChange){
						int t=RulesReconcilableMessage::information(parent, tr("FET information"),
						 tr("File contains old constraint type activity preferred time, which will be converted"
						 " to the newer similar constraint of this type, constraint activity preferred STARTING time."
						 " This improvement is done in versions 5.5.9 and above"),
						  tr("Skip rest"), tr("See next"), QString(), 1, 0 );
						if(t==0)
							reportActivityPreferredTimeChange=false;
					}
					
					crt_constraint=readActivityPreferredTime(parent, xmlReader, xmlReadingLog,
						reportUnspecifiedPermanentlyLockedTime, reportUnspecifiedDayOrHourPreferredStartingTime);
				}
				
				else if(xmlReader.name()==QString("ConstraintActivityPreferredStartingTime")){
					crt_constraint=readActivityPreferredStartingTime(parent, xmlReader, xmlReadingLog,
						reportUnspecifiedPermanentlyLockedTime, reportUnspecifiedDayOrHourPreferredStartingTime);
				}
				else if(xmlReader.name()==QString("ConstraintActivityEndsStudentsDay")){
					crt_constraint=readActivityEndsStudentsDay(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintActivitiesEndStudentsDay")){
					crt_constraint=readActivitiesEndStudentsDay(xmlReader, xmlReadingLog);
				}
				//old, with 2 and 3
				else if(xmlReader.name()==QString("Constraint2ActivitiesConsecutive")){
					crt_constraint=read2ActivitiesConsecutive(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("Constraint2ActivitiesGrouped")){
					crt_constraint=read2ActivitiesGrouped(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("Constraint3ActivitiesGrouped")){
					crt_constraint=read3ActivitiesGrouped(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("Constraint2ActivitiesOrdered")){
					crt_constraint=read2ActivitiesOrdered(xmlReader, xmlReadingLog);
				}
				//end old
				else if(xmlReader.name()==QString("ConstraintTwoActivitiesConsecutive")){
					crt_constraint=readTwoActivitiesConsecutive(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTwoActivitiesGrouped")){
					crt_constraint=readTwoActivitiesGrouped(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintThreeActivitiesGrouped")){
					crt_constraint=readThreeActivitiesGrouped(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTwoActivitiesOrdered")){
					crt_constraint=readTwoActivitiesOrdered(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTwoActivitiesOrderedIfSameDay")){
					crt_constraint=readTwoActivitiesOrderedIfSameDay(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintActivityEndsDay") && !skipDeprecatedConstraints ){
					int t=RulesReconcilableMessage::warning(parent, tr("FET warning"),
					 tr("File contains deprecated constraint activity ends day - will be ignored"),
					 tr("Skip rest"), tr("See next"), QString(),
					 1, 0 );
					
					if(t==0)
						skipDeprecatedConstraints=true;
					crt_constraint=nullptr;
					xmlReader.skipCurrentElement();
				}
				else if(xmlReader.name()==QString("ConstraintActivityPreferredTimes")){
					if(reportActivityPreferredTimesChange){
						int t=RulesReconcilableMessage::information(parent, tr("FET information"),
						 tr("Your file contains old constraint activity preferred times, which will be converted to"
						 " new equivalent constraint activity preferred starting times. Beginning with FET-5.5.9 it is possible"
						 " to specify: 1. the starting times of an activity (constraint activity preferred starting times)"
						 " or: 2. the accepted time slots (constraint activity preferred time slots)."
						 " If what you need is type 2 of this constraint, you will have to add it by yourself from the interface."),
						  tr("Skip rest"), tr("See next"), QString(), 1, 0 );
						if(t==0)
							reportActivityPreferredTimesChange=false;
					}
					
					crt_constraint=readActivityPreferredTimes(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintActivityPreferredTimeSlots")){
					crt_constraint=readActivityPreferredTimeSlots(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintActivityPreferredStartingTimes")){
					crt_constraint=readActivityPreferredStartingTimes(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintBreak")){
					if(reportBreakChange){
						int t=RulesReconcilableMessage::information(parent, tr("FET information"),
						 tr("File contains constraint break, which is old (it was improved in FET 5.5.0), and will be converted"
						 " to the similar constraint of this type, constraint break times (a matrix)."),
						  tr("Skip rest"), tr("See next"), QString(), 1, 0 );
						if(t==0)
							reportBreakChange=false;
					}
					
					crt_constraint=readBreak(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintBreakTimes")){
					crt_constraint=readBreakTimes(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeachersNoGaps")){
					crt_constraint=readTeachersNoGaps(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeachersMaxGapsPerWeek")){
					crt_constraint=readTeachersMaxGapsPerWeek(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeacherMaxGapsPerWeek")){
					crt_constraint=readTeacherMaxGapsPerWeek(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeachersMaxGapsPerDay")){
					crt_constraint=readTeachersMaxGapsPerDay(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeacherMaxGapsPerDay")){
					crt_constraint=readTeacherMaxGapsPerDay(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsNoGaps")){
					if(reportMaxGapsChange){
						int t=RulesReconcilableMessage::information(parent, tr("FET information"),
						 tr("File contains constraint students no gaps, which is old (it was improved in FET 5.5.0), and will be converted"
						 " to the similar constraint of this type, constraint students max gaps per week,"
						 " with max gaps=0. If you like, you can modify this constraint to allow"
						 " more gaps per week (normally not accepted in schools)"),
						  tr("Skip rest"), tr("See next"), QString(), 1, 0 );
						if(t==0)
							reportMaxGapsChange=false;
					}
					
					crt_constraint=readStudentsNoGaps(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsSetNoGaps")){
					if(reportMaxGapsChange){
						int t=RulesReconcilableMessage::information(parent, tr("FET information"),
						 tr("File contains constraint students set no gaps, which is old (it was improved in FET 5.5.0), and will be converted"
						 " to the similar constraint of this type, constraint students set max gaps per week,"
						 " with max gaps=0. If you like, you can modify this constraint to allow"
						 " more gaps per week (normally not accepted in schools)"),
						  tr("Skip rest"), tr("See next"), QString(), 1, 0 );
						if(t==0)
							reportMaxGapsChange=false;
					}
					
					crt_constraint=readStudentsSetNoGaps(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsMaxGapsPerWeek")){
					crt_constraint=readStudentsMaxGapsPerWeek(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsSetMaxGapsPerWeek")){
					crt_constraint=readStudentsSetMaxGapsPerWeek(xmlReader, xmlReadingLog);
				}

				else if(xmlReader.name()==QString("ConstraintStudentsMaxGapsPerDay")){
					crt_constraint=readStudentsMaxGapsPerDay(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsSetMaxGapsPerDay")){
					crt_constraint=readStudentsSetMaxGapsPerDay(xmlReader, xmlReadingLog);
				}

				else if(xmlReader.name()==QString("ConstraintStudentsEarly")){
					if(reportMaxBeginningsAtSecondHourChange){
						int t=RulesReconcilableMessage::information(parent, tr("FET information"),
						 tr("File contains constraint students early, which is old (it was improved in FET 5.5.0), and will be converted"
						 " to the similar constraint of this type, constraint students early max beginnings at second hour,"
						 " with max beginnings=0. If you like, you can modify this constraint to allow"
						 " more beginnings at second available hour (above 0 - this will make the timetable easier)"),
						  tr("Skip rest"), tr("See next"), QString(), 1, 0 );
 						if(t==0)
							reportMaxBeginningsAtSecondHourChange=false;
					}
					
					crt_constraint=readStudentsEarly(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsEarlyMaxBeginningsAtSecondHour")){
					crt_constraint=readStudentsEarlyMaxBeginningsAtSecondHour(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsSetEarly")){
					if(reportMaxBeginningsAtSecondHourChange){
						int t=RulesReconcilableMessage::information(parent, tr("FET information"),
						 tr("File contains constraint students set early, which is old (it was improved in FET 5.5.0), and will be converted"
						 " to the similar constraint of this type, constraint students set early max beginnings at second hour,"
						 " with max beginnings=0. If you like, you can modify this constraint to allow"
						 " more beginnings at second available hour (above 0 - this will make the timetable easier)"),
						  tr("Skip rest"), tr("See next"), QString(), 1, 0 );
						if(t==0)
							reportMaxBeginningsAtSecondHourChange=false;
					}
					
					crt_constraint=readStudentsSetEarly(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour")){
					crt_constraint=readStudentsSetEarlyMaxBeginningsAtSecondHour(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintActivitiesPreferredTimes")){
					if(reportActivitiesPreferredTimesChange){
						int t=RulesReconcilableMessage::information(parent, tr("FET information"),
						 tr("Your file contains old constraint activities preferred times, which will be converted to"
						 " new equivalent constraint activities preferred starting times. Beginning with FET-5.5.9 it is possible"
						 " to specify: 1. the starting times of several activities (constraint activities preferred starting times)"
						 " or: 2. the accepted time slots (constraint activities preferred time slots)."
						 " If what you need is type 2 of this constraint, you will have to add it by yourself from the interface."),
						  tr("Skip rest"), tr("See next"), QString(), 1, 0 );
						if(t==0)
							reportActivitiesPreferredTimesChange=false;
					}
					
					crt_constraint=readActivitiesPreferredTimes(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintActivitiesPreferredTimeSlots")){
					crt_constraint=readActivitiesPreferredTimeSlots(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintActivitiesPreferredStartingTimes")){
					crt_constraint=readActivitiesPreferredStartingTimes(xmlReader, xmlReadingLog);
				}
////////////////
				else if(xmlReader.name()==QString("ConstraintSubactivitiesPreferredTimeSlots")){
					crt_constraint=readSubactivitiesPreferredTimeSlots(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintSubactivitiesPreferredStartingTimes")){
					crt_constraint=readSubactivitiesPreferredStartingTimes(xmlReader, xmlReadingLog);
				}
////////////////2011-09-25
				else if(xmlReader.name()==QString("ConstraintActivitiesOccupyMaxTimeSlotsFromSelection")){
					crt_constraint=readActivitiesOccupyMaxTimeSlotsFromSelection(xmlReader, xmlReadingLog);
				}
////////////////
////////////////2019-11-16
				else if(xmlReader.name()==QString("ConstraintActivitiesOccupyMinTimeSlotsFromSelection")){
					crt_constraint=readActivitiesOccupyMinTimeSlotsFromSelection(xmlReader, xmlReadingLog);
				}
////////////////
////////////////2011-09-30
				else if(xmlReader.name()==QString("ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots")){
					crt_constraint=readActivitiesMaxSimultaneousInSelectedTimeSlots(xmlReader, xmlReadingLog);
				}
////////////////
////////////////2019-11-16
				else if(xmlReader.name()==QString("ConstraintActivitiesMinSimultaneousInSelectedTimeSlots")){
					crt_constraint=readActivitiesMinSimultaneousInSelectedTimeSlots(xmlReader, xmlReadingLog);
				}
////////////////

				else if(xmlReader.name()==QString("ConstraintTeachersSubjectTagsMaxHoursContinuously") && !skipDeprecatedConstraints){
					int t=RulesReconcilableMessage::warning(parent, tr("FET warning"),
					 tr("File contains deprecated constraint teachers subject tags max hours continuously - will be ignored"),
					 tr("Skip rest"), tr("See next"), QString(),
					 1, 0 );
					
					if(t==0)
						skipDeprecatedConstraints=true;
					crt_constraint=nullptr;
					xmlReader.skipCurrentElement();
				}
				else if(xmlReader.name()==QString("ConstraintTeachersSubjectTagMaxHoursContinuously") && !skipDeprecatedConstraints){
					int t=RulesReconcilableMessage::warning(parent, tr("FET warning"),
					 tr("File contains deprecated constraint teachers subject tag max hours continuously - will be ignored"),
					 tr("Skip rest"), tr("See next"), QString(),
					 1, 0 );
					
					if(t==0)
						skipDeprecatedConstraints=true;
					crt_constraint=nullptr;
					xmlReader.skipCurrentElement();
				}
				/////////begin 2017-02-07
				else if(xmlReader.name()==QString("ConstraintTeacherMaxSpanPerDay")){
					crt_constraint=readTeacherMaxSpanPerDay(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeachersMaxSpanPerDay")){
					crt_constraint=readTeachersMaxSpanPerDay(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsSetMaxSpanPerDay")){
					crt_constraint=readStudentsSetMaxSpanPerDay(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsMaxSpanPerDay")){
					crt_constraint=readStudentsMaxSpanPerDay(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeacherMinRestingHours")){
					crt_constraint=readTeacherMinRestingHours(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeachersMinRestingHours")){
					crt_constraint=readTeachersMinRestingHours(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsSetMinRestingHours")){
					crt_constraint=readStudentsSetMinRestingHours(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsMinRestingHours")){
					crt_constraint=readStudentsMinRestingHours(xmlReader, xmlReadingLog);
				}
				/////////  end 2017-02-07
				else{
					unrecognizedXmlTags.append(xmlReader.name().toString());
					unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
					unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

					xmlReader.skipCurrentElement();
					xmlReaderNumberOfUnrecognizedFields++;
				}

//corruptConstraintTime:
				//here we skip an invalid constraint or add a valid one
				if(crt_constraint!=nullptr){
					assert(crt_constraint!=nullptr);
					bool tmp=this->addTimeConstraint(crt_constraint);
					if(!tmp){
						if(seeNextWarnNotAddedTimeConstraint){
							int t=RulesReconcilableMessage::warning(parent, tr("FET information"),
							 tr("Constraint\n%1\nnot added - must be a duplicate").
							 arg(crt_constraint->getDetailedDescription(*this)), tr("Skip rest"), tr("See next"), QString(""), 1, 0);
							if(t==0)
								seeNextWarnNotAddedTimeConstraint=false;
						}
						delete crt_constraint;
					}
					else
						nc++;
				}
			}
			xmlReadingLog+="  Added "+CustomFETString::number(nc)+" time constraints\n";
			reducedXmlLog+="Added "+CustomFETString::number(nc)+" time constraints\n";
		}
		else if(xmlReader.name()==QString("Space_Constraints_List")){
			bool reportRoomNotAvailableChange=true;

			bool reportUnspecifiedPermanentlyLockedSpace=true;
			
			bool seeNextWarnNotAddedSpaceConstraint=true;

			int nc=0;
			SpaceConstraint *crt_constraint;
			
			assert(xmlReader.isStartElement());
			while(xmlReader.readNextStartElement()){
				xmlReadingLog+="   Found "+xmlReader.name().toString()+" tag\n";
				crt_constraint=nullptr;
				if(xmlReader.name()==QString("ConstraintBasicCompulsorySpace")){
					crt_constraint=readBasicCompulsorySpace(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintRoomNotAvailable")){
					if(reportRoomNotAvailableChange){
						int t=RulesReconcilableMessage::information(parent, tr("FET information"),
						 tr("File contains constraint room not available, which is old (it was improved in FET 5.5.0), and will be converted"
						 " to the similar constraint of this type, constraint room not available times (a matrix)."),
						  tr("Skip rest"), tr("See next"), QString(), 1, 0 );
						if(t==0)
							reportRoomNotAvailableChange=false;
					}
					
					crt_constraint=readRoomNotAvailable(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintRoomNotAvailableTimes")){
					crt_constraint=readRoomNotAvailableTimes(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintRoomTypeNotAllowedSubjects") && !skipDeprecatedConstraints){
				
					int t=RulesReconcilableMessage::warning(parent, tr("FET warning"),
					 tr("File contains deprecated constraint room type not allowed subjects - will be ignored"),
					 tr("Skip rest"), tr("See next"), QString(),
					 1, 0);
					
					if(t==0)
						skipDeprecatedConstraints=true;
					crt_constraint=nullptr;
					xmlReader.skipCurrentElement();
				}
				else if(xmlReader.name()==QString("ConstraintSubjectRequiresEquipments") && !skipDeprecatedConstraints){
				
					int t=RulesReconcilableMessage::warning(parent, tr("FET warning"),
					 tr("File contains deprecated constraint subject requires equipments - will be ignored"),
					 tr("Skip rest"), tr("See next"), QString(),
					 1, 0);
					
					if(t==0)
						skipDeprecatedConstraints=true;
				
					crt_constraint=nullptr;
					xmlReader.skipCurrentElement();
				}
				else if(xmlReader.name()==QString("ConstraintSubjectSubjectTagRequireEquipments") && !skipDeprecatedConstraints){
				
					int t=RulesReconcilableMessage::warning(parent, tr("FET warning"),
					 tr("File contains deprecated constraint subject tag requires equipments - will be ignored"),
					 tr("Skip rest"), tr("See next"), QString(),
					 1, 0);
					
					if(t==0)
						skipDeprecatedConstraints=true;
					crt_constraint=nullptr;
					xmlReader.skipCurrentElement();
				}
				else if(xmlReader.name()==QString("ConstraintTeacherRequiresRoom") && !skipDeprecatedConstraints){
				
					int t=RulesReconcilableMessage::warning(parent, tr("FET warning"),
					 tr("File contains deprecated constraint teacher requires room - will be ignored"),
					 tr("Skip rest"), tr("See next"), QString(),
					 1, 0);
					
					if(t==0)
						skipDeprecatedConstraints=true;
					crt_constraint=nullptr;
					xmlReader.skipCurrentElement();
				}
				else if(xmlReader.name()==QString("ConstraintTeacherSubjectRequireRoom") && !skipDeprecatedConstraints){
				
					int t=RulesReconcilableMessage::warning(parent, tr("FET warning"),
					 tr("File contains deprecated constraint teacher subject require room - will be ignored"),
					 tr("Skip rest"), tr("See next"), QString(),
					 1, 0);
					
					if(t==0)
						skipDeprecatedConstraints=true;
					crt_constraint=nullptr;
					xmlReader.skipCurrentElement();
				}
				else if(xmlReader.name()==QString("ConstraintMinimizeNumberOfRoomsForStudents") && !skipDeprecatedConstraints){
				
					int t=RulesReconcilableMessage::warning(parent, tr("FET warning"),
					 tr("File contains deprecated constraint minimize number of rooms for students - will be ignored"),
					 tr("Skip rest"), tr("See next"), QString(),
					 1, 0);
					
					if(t==0)
						skipDeprecatedConstraints=true;
					crt_constraint=nullptr;
					xmlReader.skipCurrentElement();
				}
				else if(xmlReader.name()==QString("ConstraintMinimizeNumberOfRoomsForTeachers") && !skipDeprecatedConstraints){
				
					int t=RulesReconcilableMessage::warning(parent, tr("FET warning"),
					 tr("File contains deprecated constraint minimize number of rooms for teachers - will be ignored"),
					 tr("Skip rest"), tr("See next"), QString(),
					 1, 0);
					
					if(t==0)
						skipDeprecatedConstraints=true;
					crt_constraint=nullptr;
					xmlReader.skipCurrentElement();
				}
				else if(xmlReader.name()==QString("ConstraintActivityPreferredRoom")){
					crt_constraint=readActivityPreferredRoom(parent, xmlReader, xmlReadingLog, reportUnspecifiedPermanentlyLockedSpace);
				}
				else if(xmlReader.name()==QString("ConstraintActivityPreferredRooms")){
					crt_constraint=readActivityPreferredRooms(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintActivitiesSameRoom") && !skipDeprecatedConstraints){
				
					int t=RulesReconcilableMessage::warning(parent, tr("FET warning"),
					 tr("File contains deprecated constraint activities same room - will be ignored"),
					 tr("Skip rest"), tr("See next"), QString(),
					 1, 0);
					
					if(t==0)
						skipDeprecatedConstraints=true;
					crt_constraint=nullptr;
					xmlReader.skipCurrentElement();
				}
				else if(xmlReader.name()==QString("ConstraintSubjectPreferredRoom")){
					crt_constraint=readSubjectPreferredRoom(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintSubjectPreferredRooms")){
					crt_constraint=readSubjectPreferredRooms(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintSubjectSubjectTagPreferredRoom")){
					crt_constraint=readSubjectSubjectTagPreferredRoom(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintSubjectSubjectTagPreferredRooms")){
					crt_constraint=readSubjectSubjectTagPreferredRooms(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintSubjectActivityTagPreferredRoom")){
					crt_constraint=readSubjectActivityTagPreferredRoom(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintSubjectActivityTagPreferredRooms")){
					crt_constraint=readSubjectActivityTagPreferredRooms(xmlReader, xmlReadingLog);
				}
				//added 6 apr 2009
				else if(xmlReader.name()==QString("ConstraintActivityTagPreferredRoom")){
					crt_constraint=readActivityTagPreferredRoom(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintActivityTagPreferredRooms")){
					crt_constraint=readActivityTagPreferredRooms(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsSetHomeRoom")){
					crt_constraint=readStudentsSetHomeRoom(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsSetHomeRooms")){
					crt_constraint=readStudentsSetHomeRooms(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeacherHomeRoom")){
					crt_constraint=readTeacherHomeRoom(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeacherHomeRooms")){
					crt_constraint=readTeacherHomeRooms(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintMaxBuildingChangesPerDayForTeachers") && !skipDeprecatedConstraints){
				
					int t=RulesReconcilableMessage::warning(parent, tr("FET warning"),
					 tr("File contains deprecated constraint max building changes per day for teachers - will be ignored"),
					 tr("Skip rest"), tr("See next"), QString(),
					 1, 0);
					
					if(t==0)
						skipDeprecatedConstraints=true;
					crt_constraint=nullptr;
					xmlReader.skipCurrentElement();
				}
				else if(xmlReader.name()==QString("ConstraintMaxBuildingChangesPerDayForStudents") && !skipDeprecatedConstraints){
				
					int t=RulesReconcilableMessage::warning(parent, tr("FET warning"),
					 tr("File contains deprecated constraint max building changes per day for students - will be ignored"),
					 tr("Skip rest"), tr("See next"), QString(),
					 1, 0);
					
					if(t==0)
						skipDeprecatedConstraints=true;
					crt_constraint=nullptr;
					xmlReader.skipCurrentElement();
				}
				else if(xmlReader.name()==QString("ConstraintMaxRoomChangesPerDayForTeachers") && !skipDeprecatedConstraints){
				
					int t=RulesReconcilableMessage::warning(parent, tr("FET warning"),
					 tr("File contains deprecated constraint max room changes per day for teachers - will be ignored"),
					 tr("Skip rest"), tr("See next"), QString(),
					 1, 0);
					
					if(t==0)
						skipDeprecatedConstraints=true;
					crt_constraint=nullptr;
					xmlReader.skipCurrentElement();
				}
				else if(xmlReader.name()==QString("ConstraintMaxRoomChangesPerDayForStudents") && !skipDeprecatedConstraints){
				
					int t=RulesReconcilableMessage::warning(parent, tr("FET warning"),
					 tr("File contains deprecated constraint max room changes per day for students - will be ignored"),
					 tr("Skip rest"), tr("See next"), QString(),
					 1, 0);
					
					if(t==0)
						skipDeprecatedConstraints=true;

					crt_constraint=nullptr;
					xmlReader.skipCurrentElement();
				}
				else if(xmlReader.name()==QString("ConstraintTeacherMaxBuildingChangesPerDay")){
					crt_constraint=readTeacherMaxBuildingChangesPerDay(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeachersMaxBuildingChangesPerDay")){
					crt_constraint=readTeachersMaxBuildingChangesPerDay(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeacherMaxBuildingChangesPerWeek")){
					crt_constraint=readTeacherMaxBuildingChangesPerWeek(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeachersMaxBuildingChangesPerWeek")){
					crt_constraint=readTeachersMaxBuildingChangesPerWeek(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeacherMinGapsBetweenBuildingChanges")){
					crt_constraint=readTeacherMinGapsBetweenBuildingChanges(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeachersMinGapsBetweenBuildingChanges")){
					crt_constraint=readTeachersMinGapsBetweenBuildingChanges(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsSetMaxBuildingChangesPerDay")){
					crt_constraint=readStudentsSetMaxBuildingChangesPerDay(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsMaxBuildingChangesPerDay")){
					crt_constraint=readStudentsMaxBuildingChangesPerDay(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsSetMaxBuildingChangesPerWeek")){
					crt_constraint=readStudentsSetMaxBuildingChangesPerWeek(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsMaxBuildingChangesPerWeek")){
					crt_constraint=readStudentsMaxBuildingChangesPerWeek(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsSetMinGapsBetweenBuildingChanges")){
					crt_constraint=readStudentsSetMinGapsBetweenBuildingChanges(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsMinGapsBetweenBuildingChanges")){
					crt_constraint=readStudentsMinGapsBetweenBuildingChanges(xmlReader, xmlReadingLog);
				}
				//2019-11-14
				else if(xmlReader.name()==QString("ConstraintTeacherMaxRoomChangesPerDay")){
					crt_constraint=readTeacherMaxRoomChangesPerDay(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeachersMaxRoomChangesPerDay")){
					crt_constraint=readTeachersMaxRoomChangesPerDay(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeacherMaxRoomChangesPerWeek")){
					crt_constraint=readTeacherMaxRoomChangesPerWeek(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeachersMaxRoomChangesPerWeek")){
					crt_constraint=readTeachersMaxRoomChangesPerWeek(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeacherMinGapsBetweenRoomChanges")){
					crt_constraint=readTeacherMinGapsBetweenRoomChanges(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintTeachersMinGapsBetweenRoomChanges")){
					crt_constraint=readTeachersMinGapsBetweenRoomChanges(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsSetMaxRoomChangesPerDay")){
					crt_constraint=readStudentsSetMaxRoomChangesPerDay(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsMaxRoomChangesPerDay")){
					crt_constraint=readStudentsMaxRoomChangesPerDay(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsSetMaxRoomChangesPerWeek")){
					crt_constraint=readStudentsSetMaxRoomChangesPerWeek(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsMaxRoomChangesPerWeek")){
					crt_constraint=readStudentsMaxRoomChangesPerWeek(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsSetMinGapsBetweenRoomChanges")){
					crt_constraint=readStudentsSetMinGapsBetweenRoomChanges(xmlReader, xmlReadingLog);
				}
				else if(xmlReader.name()==QString("ConstraintStudentsMinGapsBetweenRoomChanges")){
					crt_constraint=readStudentsMinGapsBetweenRoomChanges(xmlReader, xmlReadingLog);
				}
////////////////2012-04-29
				else if(xmlReader.name()==QString("ConstraintActivitiesOccupyMaxDifferentRooms")){
					crt_constraint=readActivitiesOccupyMaxDifferentRooms(xmlReader, xmlReadingLog);
				}
////////////////
////////////////2013-09-14
				else if(xmlReader.name()==QString("ConstraintActivitiesSameRoomIfConsecutive")){
					crt_constraint=readActivitiesSameRoomIfConsecutive(xmlReader, xmlReadingLog);
				}
////////////////
				else{
					unrecognizedXmlTags.append(xmlReader.name().toString());
					unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
					unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

					xmlReader.skipCurrentElement();
					xmlReaderNumberOfUnrecognizedFields++;
				}

//corruptConstraintSpace:
				//here we skip an invalid constraint or add a valid one
				if(crt_constraint!=nullptr){
					assert(crt_constraint!=nullptr);
					
					bool tmp=this->addSpaceConstraint(crt_constraint);
					if(!tmp){
						if(seeNextWarnNotAddedSpaceConstraint){
							int t=RulesReconcilableMessage::warning(parent, tr("FET information"),
							 tr("Constraint\n%1\nnot added - must be a duplicate").
							 arg(crt_constraint->getDetailedDescription(*this)), tr("Skip rest"), tr("See next"), QString(""), 1, 0);
							if(t==0)
								seeNextWarnNotAddedSpaceConstraint=false;
						}
						delete crt_constraint;
					}
					else
						nc++;
				}
			}
			xmlReadingLog+="  Added "+CustomFETString::number(nc)+" space constraints\n";
			reducedXmlLog+="Added "+CustomFETString::number(nc)+" space constraints\n";
		}
		else if(xmlReader.name()==QString("Timetable_Generation_Options_List")){
			int tgol=0;
			assert(xmlReader.isStartElement());
			while(xmlReader.readNextStartElement()){
				xmlReadingLog+="   Found "+xmlReader.name().toString()+" tag\n";
				
				if(xmlReader.name()==QString("GroupActivitiesInInitialOrder")){
					tgol++;
					GroupActivitiesInInitialOrderItem* item=new GroupActivitiesInInitialOrderItem();
					int nActs=-1;
					assert(xmlReader.isStartElement());
					while(xmlReader.readNextStartElement()){
						xmlReadingLog+="   Found "+xmlReader.name().toString()+" tag\n";

						if(xmlReader.name()==QString("Number_of_Activities")){
							QString text=xmlReader.readElementText();
							nActs=text.toInt();
							xmlReadingLog+="    Read n_activities="+CustomFETString::number(nActs)+"\n";
						}
						else if(xmlReader.name()==QString("Activity_Id")){
							QString text=xmlReader.readElementText();
							int id=text.toInt();
							xmlReadingLog+="    Activity id="+CustomFETString::number(id)+"\n";
							item->ids.append(id);
						}
						else if(xmlReader.name()==QString("Active")){
							QString text=xmlReader.readElementText();
							if(text=="false"){
								item->active=false;
							}
						}
						else if(xmlReader.name()==QString("Comments")){
							QString text=xmlReader.readElementText();
							item->comments=text;
						}
						else{
							unrecognizedXmlTags.append(xmlReader.name().toString());
							unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
							unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

							xmlReader.skipCurrentElement();
							xmlReaderNumberOfUnrecognizedFields++;
						}
					}
					if(!(nActs==item->ids.count())){
						xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Activities").arg("Activity_Id"));
						delete item;
					}
					else{
						assert(nActs==item->ids.count());
						groupActivitiesInInitialOrderList.append(item);
					}
				}
				else{
					unrecognizedXmlTags.append(xmlReader.name().toString());
					unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
					unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

					xmlReader.skipCurrentElement();
					xmlReaderNumberOfUnrecognizedFields++;
				}
			}
			xmlReadingLog+="  Added "+CustomFETString::number(tgol)+" timetable generation options\n";
			reducedXmlLog+="Added "+CustomFETString::number(tgol)+" timetable generation options\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	
	if(xmlReader.error()){
		RulesIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Could not read file - XML parse error at line %1, column %2:\n%3", "The error description is %3")
		 .arg(xmlReader.lineNumber())
		 .arg(xmlReader.columnNumber())
		 .arg(xmlReader.errorString()));
	
		file.close();
		return false;
	}
	file.close();
	
	if(unrecognizedXmlTags.count()>0){
		QString s;
		if(unrecognizedXmlTags.count()>=2){
			s+=tr("There are %1 unrecognized XML tags in your input file. They are written below. Your file will be opened, but"
			 " these tags will be ignored and they probably represent mistakes in your input file:", "You can assume that %1 is at least 2")
			 .arg(unrecognizedXmlTags.count());
		}
		else{
			assert(unrecognizedXmlTags.count()==1);
			s+=tr("There is an unrecognized XML tag in your input file. It is written below. Your file will be opened, but"
			 " this tag will be ignored and it probably represents a mistake in your input file:");
		}
		s+="\n\n";
		
		for(int i=0; i<unrecognizedXmlTags.count(); i++){
			s+=tr("Line %1, column %2: %3", "%3 is the unrecognized XML tag which is met in the .fet input file in line %1, column %2")
			 .arg(unrecognizedXmlLineNumbers.at(i)).arg(unrecognizedXmlColumnNumbers.at(i)).arg(unrecognizedXmlTags.at(i));
			s+="\n";
		}
		s.chop(1);
		
		RulesReconcilableMessage::warning(parent, tr("FET information"), s);
	}

	this->internalStructureComputed=false;
	
	/*reducedXmlLog+="\n";
	reducedXmlLog+="Note: if you have overlapping students sets (years or groups), a group or a subgroup may be counted more than once. "
		"A unique group name is counted once for each year it belongs to and a unique subgroup name is counted once for each year+group it belongs to.\n";*/

	if(xmlReaderNumberOfUnrecognizedFields>0){
		xmlReadingLog+="  Warning: There were "+CustomFETString::number(xmlReaderNumberOfUnrecognizedFields)+" unrecognized fields in the input file\n";

		reducedXmlLog+="\n";
		reducedXmlLog+="Warning: There were "+CustomFETString::number(xmlReaderNumberOfUnrecognizedFields)+" unrecognized fields in the input file\n";
	}

	if(canWriteLogFile){
		//logStream<<xmlReadingLog;
		logStream<<reducedXmlLog;
	}

	if(file2.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 tr("Saving of logging gave error code %1, which means you cannot see the log of reading the file. Please check your disk free space")
		 .arg(file2.error()));
	}

	if(canWriteLogFile)
		file2.close();

	////////////////////////////////////////

	return true;
}

bool Rules::write(QWidget* parent, const QString& filename)
{
	assert(this->initialized);

	//QString s;
	
	bool exists=false;
	QString filenameTmp=filename;
	if(QFile::exists(filenameTmp)){
		exists=true;

		filenameTmp.append(QString(".tmp"));
		
		if(QFile::exists(filenameTmp)){
			int i=1;
			for(;;){
				QString t2=filenameTmp+CustomFETString::number(i);
				if(!QFile::exists(t2)){
					filenameTmp=t2;
					break;
				}
				i++;
			}
		}
	}

	assert(!QFile::exists(filenameTmp));

	QFile file(filenameTmp);
	if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate)){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 tr("Cannot open %1 for writing ... please check write permissions of the selected directory or your disk free space. Saving of file aborted").arg(QFileInfo(filenameTmp).fileName()));
		
		return false;
	}

	QTextStream tos(&file);
	
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);
	//tos.setEncoding(QTextStream::UnicodeUTF8);
	
	tos<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n";

//	tos<<"<!DOCTYPE FET><FET version=\""+FET_VERSION+"\">\n\n";
	tos<<"<fet version=\""+FET_VERSION+"\">\n\n";
	
	//the institution name and the comments
	tos<<"<Institution_Name>"+protect(this->institutionName)+"</Institution_Name>\n\n";
	tos<<"<Comments>"+protect(this->comments)+"</Comments>\n\n";

	//the days and the hours
	tos<<"<Days_List>\n<Number_of_Days>"+CustomFETString::number(this->nDaysPerWeek)+"</Number_of_Days>\n";
	for(int i=0; i<this->nDaysPerWeek; i++){
		tos<<"<Day>\n";
		tos<<"	<Name>"+protect(this->daysOfTheWeek[i])+"</Name>\n";
		tos<<"</Day>\n";
	}
	tos<<"</Days_List>\n\n";
	
	tos<<"<Hours_List>\n<Number_of_Hours>"+CustomFETString::number(this->nHoursPerDay)+"</Number_of_Hours>\n";
	for(int i=0; i<this->nHoursPerDay; i++){
		tos<<"<Hour>\n";
		tos<<"	<Name>"+protect(this->hoursOfTheDay[i])+"</Name>\n";
		tos<<"</Hour>\n";
	}
	tos<<"</Hours_List>\n\n";

	//subjects list - should be before teachers list, because each teacher has a list of associated qualified subjects
	tos << "<Subjects_List>\n";
	for(int i=0; i<this->subjectsList.size(); i++){
		Subject* sbj=this->subjectsList[i];
		tos << sbj->getXmlDescription();
	}
	tos << "</Subjects_List>\n\n";

	//activity tags list
	tos << "<Activity_Tags_List>\n";
	for(int i=0; i<this->activityTagsList.size(); i++){
		ActivityTag* stg=this->activityTagsList[i];
		tos << stg->getXmlDescription();
	}
	tos << "</Activity_Tags_List>\n\n";

	//teachers list
	tos << "<Teachers_List>\n";
	for(int i=0; i<this->teachersList.size(); i++){
		Teacher* tch=this->teachersList[i];
		tos << tch->getXmlDescription();
	}
	tos << "</Teachers_List>\n\n";

	//students list
	tos<<"<Students_List>\n";
	for(int i=0; i<this->yearsList.size(); i++){
		StudentsYear* sty=this->yearsList[i];
		tos << sty->getXmlDescription();
	}
	tos<<"</Students_List>\n\n";

	//activities list
	tos << "<Activities_List>\n";
	for(int i=0; i<this->activitiesList.size(); i++){
		Activity* act=this->activitiesList[i];
		tos << act->getXmlDescription(*this);
		tos << "\n";
	}
	tos << "</Activities_List>\n\n";

	//buildings list
	tos << "<Buildings_List>\n";
	for(int i=0; i<this->buildingsList.size(); i++){
		Building* bu=this->buildingsList[i];
		tos << bu->getXmlDescription();
	}
	tos << "</Buildings_List>\n\n";

	//rooms list
	tos << "<Rooms_List>\n";
	for(int i=0; i<this->roomsList.size(); i++){
		Room* rm=this->roomsList[i];
		tos << rm->getXmlDescription();
	}
	tos << "</Rooms_List>\n\n";

	//time constraints list
	tos << "<Time_Constraints_List>\n";
	for(int i=0; i<this->timeConstraintsList.size(); i++){
		TimeConstraint* ctr=this->timeConstraintsList[i];
		tos << ctr->getXmlDescription(*this);
	}
	tos << "</Time_Constraints_List>\n\n";

	//constraints list
	tos << "<Space_Constraints_List>\n";
	for(int i=0; i<this->spaceConstraintsList.size(); i++){
		SpaceConstraint* ctr=this->spaceConstraintsList[i];
		tos << ctr->getXmlDescription(*this);
	}
	tos << "</Space_Constraints_List>\n\n";
	
	if(groupActivitiesInInitialOrderList.count()>0){
		tos << "<Timetable_Generation_Options_List>\n";
		for(int i=0; i<groupActivitiesInInitialOrderList.count(); i++){
			GroupActivitiesInInitialOrderItem* item=groupActivitiesInInitialOrderList[i];
			tos << item->getXmlDescription(*this);
		}
		tos << "</Timetable_Generation_Options_List>\n\n";
	}

//	tos<<"</FET>\n";
	tos<<"</fet>\n";

	//tos<<s;
	
	if(file.error()>0){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 tr("Saved file gave error code %1, which means saving is compromised. Please check your disk free space")
		 .arg(file.error()));
		
		file.close();
		return false;
	}
	
	file.close();
	
	if(exists){
		bool tf=QFile::remove(filename);
		assert(tf);
		tf=QFile::rename(filenameTmp, filename);
		assert(tf);
	}
	
	return true;
}

int Rules::activateTeacher(const QString& teacherName)
{
	int count=0;
	for(int i=0; i<this->activitiesList.size(); i++){
		Activity* act=this->activitiesList[i];
		if(act->searchTeacher(teacherName)){
			if(!act->active)
				count++;
			act->active=true;
		}
	}

	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;
	
	return count;
}

int Rules::activateStudents(const QString& studentsName)
{
	QSet<QString> allSets;
	
	StudentsSet* studentsSet=this->searchStudentsSet(studentsName);
	assert(studentsSet!=nullptr);
	if(studentsSet->type==STUDENTS_SUBGROUP)
		allSets.insert(studentsName);
	else if(studentsSet->type==STUDENTS_GROUP){
		allSets.insert(studentsName);
		StudentsGroup* g=(StudentsGroup*)studentsSet;
		for(StudentsSubgroup* s : qAsConst(g->subgroupsList))
			allSets.insert(s->name);
	}
	else if(studentsSet->type==STUDENTS_YEAR){
		allSets.insert(studentsName);
		StudentsYear* y=(StudentsYear*)studentsSet;
		for(StudentsGroup* g : qAsConst(y->groupsList)){
			allSets.insert(g->name);
			for(StudentsSubgroup* s : qAsConst(g->subgroupsList))
				allSets.insert(s->name);
		}
	}

	int count=0;
	for(int i=0; i<this->activitiesList.size(); i++){
		Activity* act=this->activitiesList[i];
		if(!act->active){
			for(const QString& studentsSetName : qAsConst(act->studentsNames)){
				if(allSets.contains(studentsSetName)){
					count++;
					act->active=true;
					break;
				}
			}
		}
	}

	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;
	
	return count;
}

int Rules::activateSubject(const QString& subjectName)
{
	int count=0;
	for(int i=0; i<this->activitiesList.size(); i++){
		Activity* act=this->activitiesList[i];
		if(act->subjectName==subjectName){
			if(!act->active)
				count++;
			act->active=true;
		}
	}

	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;
	
	return count;
}

int Rules::activateActivityTag(const QString& activityTagName)
{
	int count=0;
	for(int i=0; i<this->activitiesList.size(); i++){
		Activity* act=this->activitiesList[i];
		if(act->activityTagsNames.contains(activityTagName)){
			if(!act->active)
				count++;
			act->active=true;
		}
	}

	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;
	
	return count;
}

int Rules::deactivateTeacher(const QString& teacherName)
{
	int count=0;
	for(int i=0; i<this->activitiesList.size(); i++){
		Activity* act=this->activitiesList[i];
		if(act->searchTeacher(teacherName)){
			if(act->active)
				count++;
			act->active=false;
		}
	}

	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;
	
	return count;
}

int Rules::deactivateStudents(const QString& studentsName)
{
	QSet<QString> allSets;
	
	StudentsSet* studentsSet=this->searchStudentsSet(studentsName);
	assert(studentsSet!=nullptr);
	if(studentsSet->type==STUDENTS_SUBGROUP)
		allSets.insert(studentsName);
	else if(studentsSet->type==STUDENTS_GROUP){
		allSets.insert(studentsName);
		StudentsGroup* g=(StudentsGroup*)studentsSet;
		for(StudentsSubgroup* s : qAsConst(g->subgroupsList))
			allSets.insert(s->name);
	}
	else if(studentsSet->type==STUDENTS_YEAR){
		allSets.insert(studentsName);
		StudentsYear* y=(StudentsYear*)studentsSet;
		for(StudentsGroup* g : qAsConst(y->groupsList)){
			allSets.insert(g->name);
			for(StudentsSubgroup* s : qAsConst(g->subgroupsList))
				allSets.insert(s->name);
		}
	}

	int count=0;
	for(int i=0; i<this->activitiesList.size(); i++){
		Activity* act=this->activitiesList[i];
		if(act->active){
			for(const QString& studentsSetName : qAsConst(act->studentsNames)){
				if(allSets.contains(studentsSetName)){
					count++;
					act->active=false;
					break;
				}
			}
		}
	}

	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;
	
	return count;
}

int Rules::deactivateSubject(const QString& subjectName)
{
	int count=0;
	for(int i=0; i<this->activitiesList.size(); i++){
		Activity* act=this->activitiesList[i];
		if(act->subjectName==subjectName){
			if(act->active)
				count++;
			act->active=false;
		}
	}

	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;
	
	return count;
}

int Rules::deactivateActivityTag(const QString& activityTagName)
{
	int count=0;
	for(int i=0; i<this->activitiesList.size(); i++){
		Activity* act=this->activitiesList[i];
		if(act->activityTagsNames.contains(activityTagName)){
			if(act->active)
				count++;
			act->active=false;
		}
	}

	this->internalStructureComputed=false;
	setRulesModifiedAndOtherThings(this);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;
	
	return count;
}

void Rules::makeActivityTagPrintable(const QString& activityTagName)
{
	int i=searchActivityTag(activityTagName);
	assert(i>=0 && i<activityTagsList.count());
	
	ActivityTag* at=activityTagsList[i];
	
	if(at->printable==false){
		at->printable=true;

		this->internalStructureComputed=false;
		setRulesModifiedAndOtherThings(this);

		teachers_schedule_ready=false;
		students_schedule_ready=false;
		rooms_schedule_ready=false;
	}
}

void Rules::makeActivityTagNotPrintable(const QString& activityTagName)
{
	int i=searchActivityTag(activityTagName);
	assert(i>=0 && i<activityTagsList.count());
	
	ActivityTag* at=activityTagsList[i];
	
	if(at->printable==true){
		at->printable=false;

		this->internalStructureComputed=false;
		setRulesModifiedAndOtherThings(this);

		teachers_schedule_ready=false;
		students_schedule_ready=false;
		rooms_schedule_ready=false;
	}
}

TimeConstraint* Rules::readBasicCompulsoryTime(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintBasicCompulsoryTime"));
	ConstraintBasicCompulsoryTime* cn=new ConstraintBasicCompulsoryTime();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - generating automatic 100% weight percentage\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readTeacherNotAvailable(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeacherNotAvailable"));

	QList<int> days;
	QList<int> hours;
	QString teacher;
	double weightPercentage=100;
	int d=-1, h1=-1, h2=-1;
	bool active=true;
	QString comments=QString("");
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			weightPercentage=customFETStrToDouble(text);
			if(weightPercentage<0){
				xmlReader.raiseError(tr("Weight percentage incorrect"));
				return nullptr;
			}
			assert(weightPercentage>=0);
			xmlReadingLog+="    Read weight percentage="+CustomFETString::number(weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			comments=text;
		}
		else if(xmlReader.name()==QString("Day")){
			QString text=xmlReader.readElementText();
			for(d=0; d<this->nDaysPerWeek; d++)
				if(this->daysOfTheWeek[d]==text)
					break;
			if(d>=this->nDaysPerWeek){
				xmlReader.raiseError(tr("Day %1 is inexistent").arg(text));
				/*RulesReconcilableMessage::information(parent, tr("FET information"),
					tr("Constraint TeacherNotAvailable day corrupt for teacher %1, day %2 is inexistent ... ignoring constraint")
					.arg(teacher)
					.arg(text));*/
				//cn=nullptr;
				
				return nullptr;
				//goto corruptConstraintTime;
			}
			assert(d<this->nDaysPerWeek);
			xmlReadingLog+="    Crt. day="+this->daysOfTheWeek[d]+"\n";
		}
		else if(xmlReader.name()==QString("Start_Hour")){
			QString text=xmlReader.readElementText();
			for(h1=0; h1 < this->nHoursPerDay; h1++)
				if(this->hoursOfTheDay[h1]==text)
					break;
			if(h1==this->nHoursPerDay){
				xmlReader.raiseError(tr("Hour %1 is the last hour - impossible").arg(text));
				return nullptr;
			}
			else if(h1>this->nHoursPerDay){
				xmlReader.raiseError(tr("Hour %1 is inexistent").arg(text));
				/*RulesReconcilableMessage::information(parent, tr("FET information"),
					tr("Constraint TeacherNotAvailable start hour corrupt for teacher %1, hour %2 is inexistent ... ignoring constraint")
					.arg(teacher)
					.arg(text));*/
				//cn=nullptr;
				
				return nullptr;
				//goto corruptConstraintTime;
			}
			assert(h1>=0 && h1 < this->nHoursPerDay);
			xmlReadingLog+="    Start hour="+this->hoursOfTheDay[h1]+"\n";
		}
		else if(xmlReader.name()==QString("End_Hour")){
			QString text=xmlReader.readElementText();
			for(h2=0; h2 < this->nHoursPerDay; h2++)
				if(this->hoursOfTheDay[h2]==text)
					break;
			if(h2==0){
				xmlReader.raiseError(tr("Hour %1 is the first hour - impossible").arg(text));
				return nullptr;
			}
			else if(h2>this->nHoursPerDay){
				xmlReader.raiseError(tr("Hour %1 is inexistent").arg(text));
				return nullptr;
			}
			/*if(h2<=0 || h2>this->nHoursPerDay){
				RulesReconcilableMessage::information(parent, tr("FET information"),
					tr("Constraint TeacherNotAvailable end hour corrupt for teacher %1, hour %2 is inexistent ... ignoring constraint")
					.arg(teacher)
					.arg(text));
					
				return nullptr;
				//goto corruptConstraintTime;
			}*/
			assert(h2>0 && h2 <= this->nHoursPerDay);
			xmlReadingLog+="    End hour="+this->hoursOfTheDay[h2]+"\n";
		}
		else if(xmlReader.name()==QString("Teacher_Name")){
			QString text=xmlReader.readElementText();
			teacher=text;
			xmlReadingLog+="    Read teacher name="+teacher+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	
	assert(weightPercentage>=0);
	if(d<0){
		xmlReader.raiseError(tr("Field missing: %1").arg("Day"));
		return nullptr;
	}
	else if(h1<0){
		xmlReader.raiseError(tr("Field missing: %1").arg("Start_Hour"));
		return nullptr;
	}
	else if(h2<0){
		xmlReader.raiseError(tr("Field missing: %1").arg("End_Hour"));
		return nullptr;
	}
	assert(d>=0 && h1>=0 && h2>=0);
	
	ConstraintTeacherNotAvailableTimes* cn = nullptr;
	
	bool found=false;
	for(TimeConstraint* c : qAsConst(this->timeConstraintsList))
		if(c->type==CONSTRAINT_TEACHER_NOT_AVAILABLE_TIMES){
			ConstraintTeacherNotAvailableTimes* tna=(ConstraintTeacherNotAvailableTimes*) c;
			if(tna->teacher==teacher){
				found=true;
				
				for(int hh=h1; hh<h2; hh++){
					int k;
					for(k=0; k<tna->days.count(); k++)
						if(tna->days.at(k)==d && tna->hours.at(k)==hh)
							break;
					if(k==tna->days.count()){
						tna->days.append(d);
						tna->hours.append(hh);
					}
				}
				
				assert(tna->days.count()==tna->hours.count());
			}
		}
	if(!found){
		days.clear();
		hours.clear();
		for(int hh=h1; hh<h2; hh++){
			days.append(d);
			hours.append(hh);
		}
	
		cn=new ConstraintTeacherNotAvailableTimes(weightPercentage, teacher, days, hours);
		cn->active=active;
		cn->comments=comments;

		return cn;
	}
	else
		return nullptr;
}

TimeConstraint* Rules::readTeacherNotAvailableTimes(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeacherNotAvailableTimes"));
	
	ConstraintTeacherNotAvailableTimes* cn=new ConstraintTeacherNotAvailableTimes();
	int nNotAvailableSlots=-1;
	int i=0;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Read weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}

		else if(xmlReader.name()==QString("Number_of_Not_Available_Times")){
			QString text=xmlReader.readElementText();
			nNotAvailableSlots=text.toInt();
			xmlReadingLog+="    Read number of not available times="+CustomFETString::number(nNotAvailableSlots)+"\n";
		}

		else if(xmlReader.name()==QString("Not_Available_Time")){
			xmlReadingLog+="    Read: not available time\n";
			
			int d=-1;
			int h=-1;

			assert(xmlReader.isStartElement());
			while(xmlReader.readNextStartElement()){
				xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
				if(xmlReader.name()==QString("Day")){
					QString text=xmlReader.readElementText();
					for(d=0; d<this->nDaysPerWeek; d++)
						if(this->daysOfTheWeek[d]==text)
							break;

					if(d>=this->nDaysPerWeek){
						xmlReader.raiseError(tr("Day %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr("Constraint TeacherNotAvailableTimes day corrupt for teacher %1, day %2 is inexistent ... ignoring constraint")
							.arg(cn->teacher)
							.arg(text));*/
						delete cn;
						cn=nullptr;
						//goto corruptConstraintTime;
						return nullptr;
					}
		
					assert(d<this->nDaysPerWeek);
					xmlReadingLog+="    Day="+this->daysOfTheWeek[d]+"("+CustomFETString::number(i)+")"+"\n";
				}
				else if(xmlReader.name()==QString("Hour")){
					QString text=xmlReader.readElementText();
					for(h=0; h < this->nHoursPerDay; h++)
						if(this->hoursOfTheDay[h]==text)
							break;
					
					if(h>=this->nHoursPerDay){
						xmlReader.raiseError(tr("Hour %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr("Constraint TeacherNotAvailableTimes hour corrupt for teacher %1, hour %2 is inexistent ... ignoring constraint")
							.arg(cn->teacher)
							.arg(text));*/
						delete cn;
						cn=nullptr;
						//goto corruptConstraintTime;
						return nullptr;
					}
					
					assert(h>=0 && h < this->nHoursPerDay);
					xmlReadingLog+="    Hour="+this->hoursOfTheDay[h]+"\n";
				}
				else{
					unrecognizedXmlTags.append(xmlReader.name().toString());
					unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
					unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

					xmlReader.skipCurrentElement();
					xmlReaderNumberOfUnrecognizedFields++;
				}
			}
			i++;
			
			cn->days.append(d);
			cn->hours.append(h);

			if(d==-1 || h==-1){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Not_Available_Time"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
		}
		else if(xmlReader.name()==QString("Teacher")){
			QString text=xmlReader.readElementText();
			cn->teacher=text;
			xmlReadingLog+="    Read teacher name="+cn->teacher+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	assert(i==cn->days.count() && i==cn->hours.count());
	if(!(i==nNotAvailableSlots)){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Not_Available_Times").arg("Not_Available_Time"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(i==nNotAvailableSlots);

	return cn;
}

TimeConstraint* Rules::readTeacherMaxDaysPerWeek(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeacherMaxDaysPerWeek"));
	
	ConstraintTeacherMaxDaysPerWeek* cn=new ConstraintTeacherMaxDaysPerWeek();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - generating 100% weight percentage\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Teacher_Name")){
			QString text=xmlReader.readElementText();
			cn->teacherName=text;
			xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
		}
		else if(xmlReader.name()==QString("Max_Days_Per_Week")){
			QString text=xmlReader.readElementText();
			cn->maxDaysPerWeek=text.toInt();
			if(cn->maxDaysPerWeek<=0 || cn->maxDaysPerWeek>this->nDaysPerWeek){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Max_Days_Per_Week"));
				/*RulesReconcilableMessage::information(parent, tr("FET information"),
					tr("Constraint TeacherMaxDaysPerWeek max days corrupt for teacher %1, max days %2 <= 0 or >nDaysPerWeek, ignoring constraint")
					.arg(cn->teacherName)
					.arg(text));*/
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			assert(cn->maxDaysPerWeek>0 && cn->maxDaysPerWeek <= this->nDaysPerWeek);
			xmlReadingLog+="    Max. days per week="+CustomFETString::number(cn->maxDaysPerWeek)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readTeachersMaxDaysPerWeek(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeachersMaxDaysPerWeek"));
	
	ConstraintTeachersMaxDaysPerWeek* cn=new ConstraintTeachersMaxDaysPerWeek();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Max_Days_Per_Week")){
			QString text=xmlReader.readElementText();
			cn->maxDaysPerWeek=text.toInt();
			if(cn->maxDaysPerWeek<=0 || cn->maxDaysPerWeek>this->nDaysPerWeek){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Max_Days_Per_Week"));
				/*RulesReconcilableMessage::information(parent, tr("FET information"),
					tr("Constraint TeachersMaxDaysPerWeek max days corrupt, max days %1 <= 0 or >nDaysPerWeek, ignoring constraint")
					.arg(text));*/
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			assert(cn->maxDaysPerWeek>0 && cn->maxDaysPerWeek <= this->nDaysPerWeek);
			xmlReadingLog+="    Max. days per week="+CustomFETString::number(cn->maxDaysPerWeek)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readTeacherMinDaysPerWeek(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeacherMinDaysPerWeek"));
	
	ConstraintTeacherMinDaysPerWeek* cn=new ConstraintTeacherMinDaysPerWeek();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Teacher_Name")){
			QString text=xmlReader.readElementText();
			cn->teacherName=text;
			xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
		}
		else if(xmlReader.name()==QString("Minimum_Days_Per_Week")){
			QString text=xmlReader.readElementText();
			cn->minDaysPerWeek=text.toInt();
			if(cn->minDaysPerWeek<=0 || cn->minDaysPerWeek>this->nDaysPerWeek){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Minimum_Days_Per_Week"));
				/*RulesReconcilableMessage::information(parent, tr("FET information"),
					tr("Constraint TeacherMinDaysPerWeek min days corrupt for teacher %1, min days %2 <= 0 or >nDaysPerWeek, ignoring constraint")
					.arg(cn->teacherName)
					.arg(text));*/
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			assert(cn->minDaysPerWeek>0 && cn->minDaysPerWeek <= this->nDaysPerWeek);
			xmlReadingLog+="    Min. days per week="+CustomFETString::number(cn->minDaysPerWeek)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readTeachersMinDaysPerWeek(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeachersMinDaysPerWeek"));
	
	ConstraintTeachersMinDaysPerWeek* cn=new ConstraintTeachersMinDaysPerWeek();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Minimum_Days_Per_Week")){
			QString text=xmlReader.readElementText();
			cn->minDaysPerWeek=text.toInt();
			if(cn->minDaysPerWeek<=0 || cn->minDaysPerWeek>this->nDaysPerWeek){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Minimum_Days_Per_Week"));
				/*RulesReconcilableMessage::information(parent, tr("FET information"),
					tr("Constraint TeachersMinDaysPerWeek min days corrupt, min days %1 <= 0 or >nDaysPerWeek, ignoring constraint")
					.arg(text));*/
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			assert(cn->minDaysPerWeek>0 && cn->minDaysPerWeek <= this->nDaysPerWeek);
			xmlReadingLog+="    Min. days per week="+CustomFETString::number(cn->minDaysPerWeek)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readTeacherIntervalMaxDaysPerWeek(QWidget* parent, QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeacherIntervalMaxDaysPerWeek"));
	ConstraintTeacherIntervalMaxDaysPerWeek* cn=new ConstraintTeacherIntervalMaxDaysPerWeek();
	cn->maxDaysPerWeek=this->nDaysPerWeek;
	cn->startHour=this->nHoursPerDay;
	cn->endHour=this->nHoursPerDay;
	int h1, h2;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - generating 100% weight percentage\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Teacher_Name")){
			QString text=xmlReader.readElementText();
			cn->teacherName=text;
			xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
		}
		else if(xmlReader.name()==QString("Max_Days_Per_Week")){
			QString text=xmlReader.readElementText();
			cn->maxDaysPerWeek=text.toInt();
			if(cn->maxDaysPerWeek>this->nDaysPerWeek){
				RulesReconcilableMessage::information(parent, tr("FET information"),
					tr("Constraint TeacherIntervalMaxDaysPerWeek max days corrupt for teacher %1, max days %2 >nDaysPerWeek, constraint added, please correct constraint")
					.arg(cn->teacherName)
					.arg(text));
				/*delete cn;
				cn=nullptr;
				goto corruptConstraintTime;*/
			}
			//assert(cn->maxDaysPerWeek>0 && cn->maxDaysPerWeek <= this->nDaysPerWeek);
			xmlReadingLog+="    Max. days per week="+CustomFETString::number(cn->maxDaysPerWeek)+"\n";
		}
		else if(xmlReader.name()==QString("Interval_Start_Hour")){
			QString text=xmlReader.readElementText();
			for(h1=0; h1 < this->nHoursPerDay; h1++)
				if(this->hoursOfTheDay[h1]==text)
					break;
			if(h1>=this->nHoursPerDay){
				xmlReader.raiseError(tr("Hour %1 is inexistent").arg(text));
				/*RulesReconcilableMessage::information(parent, tr("FET information"),
					tr("Constraint Teacher interval max days per week start hour corrupt for teacher %1, hour %2 is inexistent ... ignoring constraint")
					.arg(cn->teacherName)
					.arg(text));*/
				delete cn;
				//cn=nullptr;
				//goto corruptConstraintTime;
				return nullptr;
			}
			assert(h1>=0 && h1 < this->nHoursPerDay);
			xmlReadingLog+="    Interval start hour="+this->hoursOfTheDay[h1]+"\n";
			cn->startHour=h1;
		}
		else if(xmlReader.name()==QString("Interval_End_Hour")){
			QString text=xmlReader.readElementText();
			if(text==""){
				xmlReadingLog+="    Interval end hour void, meaning end of day\n";
				cn->endHour=this->nHoursPerDay;
			}
			else{
				for(h2=0; h2 < this->nHoursPerDay; h2++)
					if(this->hoursOfTheDay[h2]==text)
						break;
				if(h2>=this->nHoursPerDay){
					xmlReader.raiseError(tr("Hour %1 is inexistent (it is also not void, to specify end of the day)").arg(text));
					/*RulesReconcilableMessage::information(parent, tr("FET information"),
						tr("Constraint Teacher interval max days per week end hour corrupt for teacher %1, hour %2 is inexistent (it is also not void, to specify end of the day) ... ignoring constraint")
						.arg(cn->teacherName)
						.arg(text));*/
					delete cn;
					//cn=nullptr;
					//goto corruptConstraintTime;
					return nullptr;
				}
				assert(h2>=0 && h2 < this->nHoursPerDay);
				xmlReadingLog+="    Interval end hour="+this->hoursOfTheDay[h2]+"\n";
				cn->endHour=h2;
			}
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readTeachersIntervalMaxDaysPerWeek(QWidget* parent, QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeachersIntervalMaxDaysPerWeek"));
	ConstraintTeachersIntervalMaxDaysPerWeek* cn=new ConstraintTeachersIntervalMaxDaysPerWeek();
	cn->maxDaysPerWeek=this->nDaysPerWeek;
	cn->startHour=this->nHoursPerDay;
	cn->endHour=this->nHoursPerDay;
	int h1, h2;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - generating 100% weight percentage\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		/*else if(xmlReader.name()==QString("Teacher_Name")){
			cn->teacherName=text;
			xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
		}*/
		else if(xmlReader.name()==QString("Max_Days_Per_Week")){
			QString text=xmlReader.readElementText();
			cn->maxDaysPerWeek=text.toInt();
			if(cn->maxDaysPerWeek>this->nDaysPerWeek){
				RulesReconcilableMessage::information(parent, tr("FET information"),
					tr("Constraint TeachersIntervalMaxDaysPerWeek max days corrupt, max days %1 >nDaysPerWeek, constraint added, please correct constraint")
					//.arg(cn->teacherName)
					.arg(text));
				/*delete cn;
				cn=nullptr;
				goto corruptConstraintTime;*/
			}
			//assert(cn->maxDaysPerWeek>0 && cn->maxDaysPerWeek <= this->nDaysPerWeek);
			xmlReadingLog+="    Max. days per week="+CustomFETString::number(cn->maxDaysPerWeek)+"\n";
		}
		else if(xmlReader.name()==QString("Interval_Start_Hour")){
			QString text=xmlReader.readElementText();
			for(h1=0; h1 < this->nHoursPerDay; h1++)
				if(this->hoursOfTheDay[h1]==text)
					break;
			if(h1>=this->nHoursPerDay){
				xmlReader.raiseError(tr("Hour %1 is inexistent").arg(text));
				/*RulesReconcilableMessage::information(parent, tr("FET information"),
					tr("Constraint Teachers interval max days per week start hour corrupt because hour %1 is inexistent ... ignoring constraint")
					//.arg(cn->teacherName)
					.arg(text));*/
				delete cn;
				//cn=nullptr;
				//goto corruptConstraintTime;
				return nullptr;
			}
			assert(h1>=0 && h1 < this->nHoursPerDay);
			xmlReadingLog+="    Interval start hour="+this->hoursOfTheDay[h1]+"\n";
			cn->startHour=h1;
		}
		else if(xmlReader.name()==QString("Interval_End_Hour")){
			QString text=xmlReader.readElementText();
			if(text==""){
				xmlReadingLog+="    Interval end hour void, meaning end of day\n";
				cn->endHour=this->nHoursPerDay;
			}
			else{
				for(h2=0; h2 < this->nHoursPerDay; h2++)
					if(this->hoursOfTheDay[h2]==text)
						break;
				if(h2>=this->nHoursPerDay){
					xmlReader.raiseError(tr("Hour %1 is inexistent (it is also not void, to specify end of the day)").arg(text));
					/*RulesReconcilableMessage::information(parent, tr("FET information"),
						tr("Constraint Teachers interval max days per week end hour corrupt because hour %1 is inexistent (it is also not void, to specify end of the day) ... ignoring constraint")
						//.arg(cn->teacherName)
						.arg(text));*/
					delete cn;
					//cn=nullptr;
					//goto corruptConstraintTime;
					return nullptr;
				}
				assert(h2>=0 && h2 < this->nHoursPerDay);
				xmlReadingLog+="    Interval end hour="+this->hoursOfTheDay[h2]+"\n";
				cn->endHour=h2;
			}
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readStudentsSetMaxDaysPerWeek(QWidget* parent, QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsSetMaxDaysPerWeek"));
	ConstraintStudentsSetMaxDaysPerWeek* cn=new ConstraintStudentsSetMaxDaysPerWeek();
	cn->maxDaysPerWeek=this->nDaysPerWeek;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Students")){
			QString text=xmlReader.readElementText();
			cn->students=text;
			xmlReadingLog+="    Read students set name="+cn->students+"\n";
		}
		else if(xmlReader.name()==QString("Max_Days_Per_Week")){
			QString text=xmlReader.readElementText();
			cn->maxDaysPerWeek=text.toInt();
			if(cn->maxDaysPerWeek>this->nDaysPerWeek){
				RulesReconcilableMessage::information(parent, tr("FET information"),
					tr("Constraint StudentsSetMaxDaysPerWeek max days corrupt for students set %1, max days %2 >nDaysPerWeek, constraint added, please correct constraint")
					.arg(cn->students)
					.arg(text));
			}
			xmlReadingLog+="    Max. days per week="+CustomFETString::number(cn->maxDaysPerWeek)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readStudentsMaxDaysPerWeek(QWidget* parent, QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsMaxDaysPerWeek"));
	ConstraintStudentsMaxDaysPerWeek* cn=new ConstraintStudentsMaxDaysPerWeek();
	cn->maxDaysPerWeek=this->nDaysPerWeek;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Max_Days_Per_Week")){
			QString text=xmlReader.readElementText();
			cn->maxDaysPerWeek=text.toInt();
			if(cn->maxDaysPerWeek>this->nDaysPerWeek){
				RulesReconcilableMessage::information(parent, tr("FET information"),
					tr("Constraint StudentsMaxDaysPerWeek max days corrupt, max days %1 >nDaysPerWeek, constraint added, please correct constraint")
					.arg(text));
			}
			xmlReadingLog+="    Max. days per week="+CustomFETString::number(cn->maxDaysPerWeek)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readStudentsSetIntervalMaxDaysPerWeek(QWidget* parent, QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsSetIntervalMaxDaysPerWeek"));
	ConstraintStudentsSetIntervalMaxDaysPerWeek* cn=new ConstraintStudentsSetIntervalMaxDaysPerWeek();
	cn->maxDaysPerWeek=this->nDaysPerWeek;
	cn->startHour=this->nHoursPerDay;
	cn->endHour=this->nHoursPerDay;
	int h1, h2;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - generating 100% weight percentage\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Students")){
			QString text=xmlReader.readElementText();
			cn->students=text;
			xmlReadingLog+="    Read students set name="+cn->students+"\n";
		}
		else if(xmlReader.name()==QString("Max_Days_Per_Week")){
			QString text=xmlReader.readElementText();
			cn->maxDaysPerWeek=text.toInt();
			if(cn->maxDaysPerWeek>this->nDaysPerWeek){
				RulesReconcilableMessage::information(parent, tr("FET information"),
					tr("Constraint StudentsSetIntervalMaxDaysPerWeek max days corrupt for students set %1, max days %2 >nDaysPerWeek, constraint added, please correct constraint")
					.arg(cn->students)
					.arg(text));
				/*delete cn;
				cn=nullptr;
				goto corruptConstraintTime;*/
			}
			//assert(cn->maxDaysPerWeek>0 && cn->maxDaysPerWeek <= this->nDaysPerWeek);
			xmlReadingLog+="    Max. days per week="+CustomFETString::number(cn->maxDaysPerWeek)+"\n";
		}
		else if(xmlReader.name()==QString("Interval_Start_Hour")){
			QString text=xmlReader.readElementText();
			for(h1=0; h1 < this->nHoursPerDay; h1++)
				if(this->hoursOfTheDay[h1]==text)
					break;
			if(h1>=this->nHoursPerDay){
				xmlReader.raiseError(tr("Hour %1 is inexistent").arg(text));
				/*RulesReconcilableMessage::information(parent, tr("FET information"),
					tr("Constraint Students set interval max days per week start hour corrupt for students %1, hour %2 is inexistent ... ignoring constraint")
					.arg(cn->students)
					.arg(text));*/
				delete cn;
				//cn=nullptr;
				//goto corruptConstraintTime;
				return nullptr;
			}
			assert(h1>=0 && h1 < this->nHoursPerDay);
			xmlReadingLog+="    Interval start hour="+this->hoursOfTheDay[h1]+"\n";
			cn->startHour=h1;
		}
		else if(xmlReader.name()==QString("Interval_End_Hour")){
			QString text=xmlReader.readElementText();
			if(text==""){
				xmlReadingLog+="    Interval end hour void, meaning end of day\n";
				cn->endHour=this->nHoursPerDay;
			}
			else{
				for(h2=0; h2 < this->nHoursPerDay; h2++)
					if(this->hoursOfTheDay[h2]==text)
						break;
				if(h2>=this->nHoursPerDay){
					xmlReader.raiseError(tr("Hour %1 is inexistent (it is also not void, to specify end of the day)").arg(text));
					/*RulesReconcilableMessage::information(parent, tr("FET information"),
						tr("Constraint Students set interval max days per week end hour corrupt for students %1, hour %2 is inexistent (it is also not void, to specify end of the day) ... ignoring constraint")
						.arg(cn->students)
						.arg(text));*/
					delete cn;
					//cn=nullptr;
					//goto corruptConstraintTime;
					return nullptr;
				}
				assert(h2>=0 && h2 < this->nHoursPerDay);
				xmlReadingLog+="    Interval end hour="+this->hoursOfTheDay[h2]+"\n";
				cn->endHour=h2;
			}
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readStudentsIntervalMaxDaysPerWeek(QWidget* parent, QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsIntervalMaxDaysPerWeek"));
	ConstraintStudentsIntervalMaxDaysPerWeek* cn=new ConstraintStudentsIntervalMaxDaysPerWeek();
	cn->maxDaysPerWeek=this->nDaysPerWeek;
	cn->startHour=this->nHoursPerDay;
	cn->endHour=this->nHoursPerDay;
	int h1, h2;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - generating 100% weight percentage\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		/*else if(xmlReader.name()==QString("Students")){
			cn->students=text;
			xmlReadingLog+="    Read students set name="+cn->students+"\n";
		}*/
		else if(xmlReader.name()==QString("Max_Days_Per_Week")){
			QString text=xmlReader.readElementText();
			cn->maxDaysPerWeek=text.toInt();
			if(cn->maxDaysPerWeek>this->nDaysPerWeek){
				RulesReconcilableMessage::information(parent, tr("FET information"),
					tr("Constraint StudentsIntervalMaxDaysPerWeek max days corrupt: max days %1 >nDaysPerWeek, constraint added, please correct constraint")
					.arg(text));
				/*delete cn;
				cn=nullptr;
				goto corruptConstraintTime;*/
			}
			//assert(cn->maxDaysPerWeek>0 && cn->maxDaysPerWeek <= this->nDaysPerWeek);
			xmlReadingLog+="    Max. days per week="+CustomFETString::number(cn->maxDaysPerWeek)+"\n";
		}
		else if(xmlReader.name()==QString("Interval_Start_Hour")){
			QString text=xmlReader.readElementText();
			for(h1=0; h1 < this->nHoursPerDay; h1++)
				if(this->hoursOfTheDay[h1]==text)
					break;
			if(h1>=this->nHoursPerDay){
				xmlReader.raiseError(tr("Hour %1 is inexistent").arg(text));
				/*RulesReconcilableMessage::information(parent, tr("FET information"),
					tr("Constraint Students interval max days per week start hour corrupt: hour %1 is inexistent ... ignoring constraint")
					//.arg(cn->students)
					.arg(text));*/
				delete cn;
				//cn=nullptr;
				//goto corruptConstraintTime;
				return nullptr;
			}
			assert(h1>=0 && h1 < this->nHoursPerDay);
			xmlReadingLog+="    Interval start hour="+this->hoursOfTheDay[h1]+"\n";
			cn->startHour=h1;
		}
		else if(xmlReader.name()==QString("Interval_End_Hour")){
			QString text=xmlReader.readElementText();
			if(text==""){
				xmlReadingLog+="    Interval end hour void, meaning end of day\n";
				cn->endHour=this->nHoursPerDay;
			}
			else{
				for(h2=0; h2 < this->nHoursPerDay; h2++)
					if(this->hoursOfTheDay[h2]==text)
						break;
				if(h2>=this->nHoursPerDay){
					xmlReader.raiseError(tr("Hour %1 is inexistent (it is also not void, to specify end of the day)").arg(text));
					/*RulesReconcilableMessage::information(parent, tr("FET information"),
						tr("Constraint Students interval max days per week end hour corrupt: hour %1 is inexistent (it is also not void, to specify end of the day) ... ignoring constraint")
						//.arg(cn->students)
						.arg(text));*/
					delete cn;
					//cn=nullptr;
					//goto corruptConstraintTime;
					return nullptr;
				}
				assert(h2>=0 && h2 < this->nHoursPerDay);
				xmlReadingLog+="    Interval end hour="+this->hoursOfTheDay[h2]+"\n";
				cn->endHour=h2;
			}
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readStudentsSetNotAvailable(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsSetNotAvailable"));

	//ConstraintStudentsSetNotAvailableTimes* cn=new ConstraintStudentsSetNotAvailableTimes();
	QList<int> days;
	QList<int> hours;
	QString students;
	double weightPercentage=100;
	int d=-1, h1=-1, h2=-1;
	bool active=true;
	QString comments=QString("");
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			weightPercentage=customFETStrToDouble(text);
			if(weightPercentage<0){
				xmlReader.raiseError(tr("Weight percentage incorrect"));
				return nullptr;
			}
			assert(weightPercentage>=0);
			xmlReadingLog+="    Read weight percentage="+CustomFETString::number(weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			comments=text;
		}
		else if(xmlReader.name()==QString("Day")){
			QString text=xmlReader.readElementText();
			for(d=0; d<this->nDaysPerWeek; d++)
				if(this->daysOfTheWeek[d]==text)
					break;
			if(d>=this->nDaysPerWeek){
				xmlReader.raiseError(tr("Day %1 is inexistent").arg(text));
				/*RulesReconcilableMessage::information(parent, tr("FET information"),
					tr("Constraint StudentsSetNotAvailable day corrupt for students %1, day %2 is inexistent ... ignoring constraint")
					.arg(students)
					.arg(text));*/
				//cn=nullptr;
				//goto corruptConstraintTime;
				return nullptr;
			}
			assert(d<this->nDaysPerWeek);
			xmlReadingLog+="    Crt. day="+this->daysOfTheWeek[d]+"\n";
		}
		else if(xmlReader.name()==QString("Start_Hour")){
			QString text=xmlReader.readElementText();
			for(h1=0; h1 < this->nHoursPerDay; h1++)
				if(this->hoursOfTheDay[h1]==text)
					break;
			if(h1==this->nHoursPerDay){
				xmlReader.raiseError(tr("Hour %1 is the last hour - impossible").arg(text));
				return nullptr;
			}
			else if(h1>this->nHoursPerDay){
				xmlReader.raiseError(tr("Hour %1 is inexistent").arg(text));
				/*RulesReconcilableMessage::information(parent, tr("FET information"),
					tr("Constraint StudentsSetNotAvailable start hour corrupt for students set %1, hour %2 is inexistent ... ignoring constraint")
					.arg(students)
					.arg(text));*/
				//cn=nullptr;
				//goto corruptConstraintTime;
				return nullptr;
			}
			assert(h1>=0 && h1 < this->nHoursPerDay);
			xmlReadingLog+="    Start hour="+this->hoursOfTheDay[h1]+"\n";
		}
		else if(xmlReader.name()==QString("End_Hour")){
			QString text=xmlReader.readElementText();
			for(h2=0; h2 < this->nHoursPerDay; h2++)
				if(this->hoursOfTheDay[h2]==text)
					break;
			if(h2==0){
				xmlReader.raiseError(tr("Hour %1 is the first hour - impossible").arg(text));
				return nullptr;
			}
			else if(h2>this->nHoursPerDay){
				xmlReader.raiseError(tr("Hour %1 is inexistent").arg(text));
				return nullptr;
			}
			/*if(h2<=0 || h2>this->nHoursPerDay){
				RulesReconcilableMessage::information(parent, tr("FET information"),
					tr("Constraint StudentsSetNotAvailable end hour corrupt for students %1, hour %2 is inexistent ... ignoring constraint")
					.arg(students)
					.arg(text));
				//goto corruptConstraintTime;
				return nullptr;
			}*/
			assert(h2>0 && h2 <= this->nHoursPerDay);
			xmlReadingLog+="    End hour="+this->hoursOfTheDay[h2]+"\n";
		}
		else if(xmlReader.name()==QString("Students")){
			QString text=xmlReader.readElementText();
			students=text;
			xmlReadingLog+="    Read students name="+students+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	
	assert(weightPercentage>=0);
	if(d<0){
		xmlReader.raiseError(tr("Field missing: %1").arg("Day"));
		return nullptr;
	}
	else if(h1<0){
		xmlReader.raiseError(tr("Field missing: %1").arg("Start_Hour"));
		return nullptr;
	}
	else if(h2<0){
		xmlReader.raiseError(tr("Field missing: %1").arg("End_Hour"));
		return nullptr;
	}
	assert(d>=0 && h1>=0 && h2>=0);
	
	ConstraintStudentsSetNotAvailableTimes* cn = nullptr;
	
	bool found=false;
	for(TimeConstraint* c : qAsConst(this->timeConstraintsList))
		if(c->type==CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE_TIMES){
			ConstraintStudentsSetNotAvailableTimes* ssna=(ConstraintStudentsSetNotAvailableTimes*) c;
			if(ssna->students==students){
				found=true;
				
				for(int hh=h1; hh<h2; hh++){
					int k;
					for(k=0; k<ssna->days.count(); k++)
						if(ssna->days.at(k)==d && ssna->hours.at(k)==hh)
							break;
					if(k==ssna->days.count()){
						ssna->days.append(d);
						ssna->hours.append(hh);
					}
				}
				
				assert(ssna->days.count()==ssna->hours.count());
			}
		}
	if(!found){
		days.clear();
		hours.clear();
		for(int hh=h1; hh<h2; hh++){
			days.append(d);
			hours.append(hh);
		}
	
		cn=new ConstraintStudentsSetNotAvailableTimes(weightPercentage, students, days, hours);
		cn->active=active;
		cn->comments=comments;

		//crt_constraint=cn;
		return cn;
	}
	else
		return nullptr;
}

TimeConstraint* Rules::readStudentsSetNotAvailableTimes(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsSetNotAvailableTimes"));
	
	ConstraintStudentsSetNotAvailableTimes* cn=new ConstraintStudentsSetNotAvailableTimes();
	int nNotAvailableSlots=0;
	int i=0;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Read weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}

		else if(xmlReader.name()==QString("Number_of_Not_Available_Times")){
			QString text=xmlReader.readElementText();
			nNotAvailableSlots=text.toInt();
			xmlReadingLog+="    Read number of not available times="+CustomFETString::number(nNotAvailableSlots)+"\n";
		}

		else if(xmlReader.name()==QString("Not_Available_Time")){
			xmlReadingLog+="    Read: not available time\n";
			
			int d=-1;
			int h=-1;

			assert(xmlReader.isStartElement());
			while(xmlReader.readNextStartElement()){
				xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
				if(xmlReader.name()==QString("Day")){
					QString text=xmlReader.readElementText();
					for(d=0; d<this->nDaysPerWeek; d++)
						if(this->daysOfTheWeek[d]==text)
							break;

					if(d>=this->nDaysPerWeek){
						xmlReader.raiseError(tr("Day %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr("Constraint StudentsSetNotAvailableTimes day corrupt for students %1, day %2 is inexistent ... ignoring constraint")
							.arg(cn->students)
							.arg(text));*/
						delete cn;
						cn=nullptr;
						//goto corruptConstraintTime;
						return nullptr;
					}
		
					assert(d<this->nDaysPerWeek);
					xmlReadingLog+="    Day="+this->daysOfTheWeek[d]+"("+CustomFETString::number(i)+")"+"\n";
				}
				else if(xmlReader.name()==QString("Hour")){
					QString text=xmlReader.readElementText();
					for(h=0; h < this->nHoursPerDay; h++)
						if(this->hoursOfTheDay[h]==text)
							break;
					
					if(h>=this->nHoursPerDay){
						xmlReader.raiseError(tr("Hour %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr("Constraint StudentsSetNotAvailableTimes hour corrupt for students %1, hour %2 is inexistent ... ignoring constraint")
							.arg(cn->students)
							.arg(text));*/
						delete cn;
						cn=nullptr;
						//goto corruptConstraintTime;
						return nullptr;
					}
					
					assert(h>=0 && h < this->nHoursPerDay);
					xmlReadingLog+="    Hour="+this->hoursOfTheDay[h]+"\n";
				}
				else{
					unrecognizedXmlTags.append(xmlReader.name().toString());
					unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
					unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

					xmlReader.skipCurrentElement();
					xmlReaderNumberOfUnrecognizedFields++;
				}
			}
			i++;
			
			cn->days.append(d);
			cn->hours.append(h);

			if(d==-1 || h==-1){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Not_Available_Time"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
		}
		else if(xmlReader.name()==QString("Students")){
			QString text=xmlReader.readElementText();
			cn->students=text;
			xmlReadingLog+="    Read students name="+cn->students+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	assert(i==cn->days.count() && i==cn->hours.count());
	if(!(i==nNotAvailableSlots)){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Not_Available_Times").arg("Not_Available_Time"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(i==nNotAvailableSlots);

	return cn;
}

TimeConstraint* Rules::readMinNDaysBetweenActivities(QWidget* parent, QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintMinNDaysBetweenActivities"));
	
	ConstraintMinDaysBetweenActivities* cn=new ConstraintMinDaysBetweenActivities();
	cn->n_activities=0;
	bool foundCISD=false;
	int n_act=0;
	cn->activitiesId.clear();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - generating weightPercentage=95%\n";
			cn->weightPercentage=95;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weightPercentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Consecutive_If_Same_Day") || xmlReader.name()==QString("Adjacent_If_Broken")){
			QString text=xmlReader.readElementText();
			if(text=="yes" || text=="true" || text=="1"){
				cn->consecutiveIfSameDay=true;
				foundCISD=true;
				xmlReadingLog+="    Current constraint has consecutive if same day=true\n";
			}
			else{
				if(!(text=="no" || text=="false" || text=="0")){
					RulesReconcilableMessage::warning(parent, tr("FET warning"),
						tr("Found constraint min days between activities with tag consecutive if same day"
						" which is not 'true', 'false', 'yes', 'no', '1' or '0'."
						" The tag will be considered false",
						"Instructions for translators: please leave the 'true', 'false', 'yes' and 'no' fields untranslated, as they are in English"));
				}
				//assert(text=="no" || text=="false" || text=="0");
				cn->consecutiveIfSameDay=false;
				foundCISD=true;
				xmlReadingLog+="    Current constraint has consecutive if same day=false\n";
			}
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=95;
				cn->consecutiveIfSameDay=true;
				foundCISD=true;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
				cn->consecutiveIfSameDay=false;
				foundCISD=true;
			}
		}
		else if(xmlReader.name()==QString("Number_of_Activities")){
			QString text=xmlReader.readElementText();
			cn->n_activities=text.toInt();
			xmlReadingLog+="    Read n_activities="+CustomFETString::number(cn->n_activities)+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Id")){
			QString text=xmlReader.readElementText();
			//cn->activitiesId[n_act]=text.toInt();
			cn->activitiesId.append(text.toInt());
			assert(n_act==cn->activitiesId.count()-1);
			xmlReadingLog+="    Read activity id="+CustomFETString::number(cn->activitiesId[n_act])+"\n";
			n_act++;
		}
		else if(xmlReader.name()==QString("MinDays")){
			QString text=xmlReader.readElementText();
			cn->minDays=text.toInt();
			xmlReadingLog+="    Read MinDays="+CustomFETString::number(cn->minDays)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(!foundCISD){
		xmlReadingLog+="    Could not find consecutive if same day information - making it true\n";
		cn->consecutiveIfSameDay=true;
	}
	if(!(n_act==cn->n_activities)){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Activities").arg("Activity_Id"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(n_act==cn->n_activities);
	return cn;
/*
#if 0
	if(0 && reportIncorrectMinDays && cn->n_activities > this->nDaysPerWeek){
		QString s=tr("You have a constraint min days between activities with more activities than the number of days per week.");
		s+=" ";
		s+=tr("Constraint is:");
		s+="\n";
		s+=crt_constraint->getDescription(*this);
		s+="\n";
		s+=tr("This is a very bad practice from the way the algorithm of generation works (it slows down the generation and makes it harder to find a solution).");
		s+="\n\n";
		s+=tr("To improve your file, you are advised to remove the corresponding activities and constraint and add activities again, respecting the following rules:");
		s+="\n\n";
		s+=tr("1. If you add 'force consecutive if same day', then couple extra activities in pairs to obtain a number of activities equal to the number of days per week"
			". Example: 7 activities with duration 1 in a 5 days week, then transform into 5 activities with durations: 2,2,1,1,1 and add a single container activity with these 5 components"
			" (possibly raising the weight of added constraint min days between activities up to 100%)");
		s+="\n\n";

		s+=tr("2. If you don't add 'force consecutive if same day', then add a larger activity split into a number of"
			" activities equal with the number of days per week and the remaining components into other larger split activity."
			" For example, suppose you need to add 7 activities with duration 1 in a 5 days week. Add 2 larger container activities,"
			" first one split into 5 activities with duration 1 and second one split into 2 activities with duration 1"
			" (possibly raising the weight of added constraints min days between activities for each of the 2 containers up to 100%)");
		
		int t=QMessageBox::warning(parent, tr("FET warning"), s,
			tr("Skip rest"), tr("See next"), QString(),
			1, 0 );
										
		if(t==0)
			reportIncorrectMinDays=false;
	}
#endif
*/
}

TimeConstraint* Rules::readMinDaysBetweenActivities(QWidget* parent, QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintMinDaysBetweenActivities"));
	
	ConstraintMinDaysBetweenActivities* cn=new ConstraintMinDaysBetweenActivities();
	cn->n_activities=0;
	bool foundCISD=false;
	int n_act=0;
	cn->activitiesId.clear();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - generating weightPercentage=95%\n";
			cn->weightPercentage=95;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weightPercentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Consecutive_If_Same_Day") || xmlReader.name()==QString("Adjacent_If_Broken")){
			QString text=xmlReader.readElementText();
			if(text=="yes" || text=="true" || text=="1"){
				cn->consecutiveIfSameDay=true;
				foundCISD=true;
				xmlReadingLog+="    Current constraint has consecutive if same day=true\n";
			}
			else{
				if(!(text=="no" || text=="false" || text=="0")){
					RulesReconcilableMessage::warning(parent, tr("FET warning"),
						tr("Found constraint min days between activities with tag consecutive if same day"
						" which is not 'true', 'false', 'yes', 'no', '1' or '0'."
						" The tag will be considered false",
						"Instructions for translators: please leave the 'true', 'false', 'yes' and 'no' fields untranslated, as they are in English"));
				}
				//assert(text=="no" || text=="false" || text=="0");
				cn->consecutiveIfSameDay=false;
				foundCISD=true;
				xmlReadingLog+="    Current constraint has consecutive if same day=false\n";
			}
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=95;
				cn->consecutiveIfSameDay=true;
				foundCISD=true;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
				cn->consecutiveIfSameDay=false;
				foundCISD=true;
			}
		}
		else if(xmlReader.name()==QString("Number_of_Activities")){
			QString text=xmlReader.readElementText();
			cn->n_activities=text.toInt();
			xmlReadingLog+="    Read n_activities="+CustomFETString::number(cn->n_activities)+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Id")){
			QString text=xmlReader.readElementText();
			//cn->activitiesId[n_act]=text.toInt();
			cn->activitiesId.append(text.toInt());
			assert(n_act==cn->activitiesId.count()-1);
			xmlReadingLog+="    Read activity id="+CustomFETString::number(cn->activitiesId[n_act])+"\n";
			n_act++;
		}
		else if(xmlReader.name()==QString("MinDays")){
			QString text=xmlReader.readElementText();
			cn->minDays=text.toInt();
			xmlReadingLog+="    Read MinDays="+CustomFETString::number(cn->minDays)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(!foundCISD){
		xmlReadingLog+="    Could not find consecutive if same day information - making it true\n";
		cn->consecutiveIfSameDay=true;
	}
	if(!(n_act==cn->n_activities)){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Activities").arg("Activity_Id"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(n_act==cn->n_activities);
	return cn;
/*
#if 0
	if(0 && reportIncorrectMinDays && cn->n_activities > this->nDaysPerWeek){
		QString s=tr("You have a constraint min days between activities with more activities than the number of days per week.");
		s+=" ";
		s+=tr("Constraint is:");
		s+="\n";
		s+=crt_constraint->getDescription(*this);
		s+="\n";
		s+=tr("This is a very bad practice from the way the algorithm of generation works (it slows down the generation and makes it harder to find a solution).");
		s+="\n\n";
		s+=tr("To improve your file, you are advised to remove the corresponding activities and constraint and add activities again, respecting the following rules:");
		s+="\n\n";
		s+=tr("1. If you add 'force consecutive if same day', then couple extra activities in pairs to obtain a number of activities equal to the number of days per week"
			". Example: 7 activities with duration 1 in a 5 days week, then transform into 5 activities with durations: 2,2,1,1,1 and add a single container activity with these 5 components"
			" (possibly raising the weight of added constraint min days between activities up to 100%)");
		s+="\n\n";

		s+=tr("2. If you don't add 'force consecutive if same day', then add a larger activity split into a number of"
			" activities equal with the number of days per week and the remaining components into other larger split activity."
			" For example, suppose you need to add 7 activities with duration 1 in a 5 days week. Add 2 larger container activities,"
			" first one split into 5 activities with duration 1 and second one split into 2 activities with duration 1"
			" (possibly raising the weight of added constraints min days between activities for each of the 2 containers up to 100%)");
		
		int t=QMessageBox::warning(parent, tr("FET warning"), s,
			tr("Skip rest"), tr("See next"), QString(),
			1, 0 );
										
		if(t==0)
			reportIncorrectMinDays=false;
	}
#endif
*/
}

TimeConstraint* Rules::readMaxDaysBetweenActivities(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintMaxDaysBetweenActivities"));
	
	ConstraintMaxDaysBetweenActivities* cn=new ConstraintMaxDaysBetweenActivities();
	cn->n_activities=0;
	int n_act=0;
	cn->activitiesId.clear();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weightPercentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Number_of_Activities")){
			QString text=xmlReader.readElementText();
			cn->n_activities=text.toInt();
			xmlReadingLog+="    Read n_activities="+CustomFETString::number(cn->n_activities)+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->activitiesId.append(text.toInt());
			assert(n_act==cn->activitiesId.count()-1);
			//cn->activitiesId[n_act]=text.toInt();
			xmlReadingLog+="    Read activity id="+CustomFETString::number(cn->activitiesId[n_act])+"\n";
			n_act++;
		}
		else if(xmlReader.name()==QString("MaxDays")){
			QString text=xmlReader.readElementText();
			cn->maxDays=text.toInt();
			xmlReadingLog+="    Read MaxDays="+CustomFETString::number(cn->maxDays)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(!(n_act==cn->n_activities)){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Activities").arg("Activity_Id"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(n_act==cn->n_activities);
	return cn;
}

TimeConstraint* Rules::readMinGapsBetweenActivities(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintMinGapsBetweenActivities"));
	ConstraintMinGapsBetweenActivities* cn=new ConstraintMinGapsBetweenActivities();
	cn->n_activities=0;
	int n_act=0;
	cn->activitiesId.clear();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weightPercentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Number_of_Activities")){
			QString text=xmlReader.readElementText();
			cn->n_activities=text.toInt();
			xmlReadingLog+="    Read n_activities="+CustomFETString::number(cn->n_activities)+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->activitiesId.append(text.toInt());
			assert(n_act==cn->activitiesId.count()-1);
			xmlReadingLog+="    Read activity id="+CustomFETString::number(cn->activitiesId[n_act])+"\n";
			n_act++;
		}
		else if(xmlReader.name()==QString("MinGaps")){
			QString text=xmlReader.readElementText();
			cn->minGaps=text.toInt();
			xmlReadingLog+="    Read MinGaps="+CustomFETString::number(cn->minGaps)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(!(n_act==cn->n_activities)){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Activities").arg("Activity_Id"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(n_act==cn->n_activities);
	return cn;
}

TimeConstraint* Rules::readActivitiesNotOverlapping(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintActivitiesNotOverlapping"));
	ConstraintActivitiesNotOverlapping* cn=new ConstraintActivitiesNotOverlapping();
	cn->n_activities=0;
	int n_act=0;
	cn->activitiesId.clear();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Number_of_Activities")){
			QString text=xmlReader.readElementText();
			cn->n_activities=text.toInt();
			xmlReadingLog+="    Read n_activities="+CustomFETString::number(cn->n_activities)+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Id")){
			QString text=xmlReader.readElementText();
			//cn->activitiesId[n_act]=text.toInt();
			cn->activitiesId.append(text.toInt());
			assert(n_act==cn->activitiesId.count()-1);
			xmlReadingLog+="    Read activity id="+CustomFETString::number(cn->activitiesId[n_act])+"\n";
			n_act++;
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(!(n_act==cn->n_activities)){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Activities").arg("Activity_Id"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(n_act==cn->n_activities);
	return cn;
}

TimeConstraint* Rules::readActivityTagsNotOverlapping(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintActivityTagsNotOverlapping"));
	ConstraintActivityTagsNotOverlapping* cn=new ConstraintActivityTagsNotOverlapping();
	int nActivityTags=-1;
	cn->activityTagsNames.clear();
	QSet<QString> readTags;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Number_of_Activity_Tags")){
			QString text=xmlReader.readElementText();
			nActivityTags=text.toInt();
			if(nActivityTags<2){
				xmlReader.raiseError(tr("The number of activity tags in the constraint activity tags not overlapping is lower than two"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			xmlReadingLog+="    Read n activity tags="+CustomFETString::number(nActivityTags)+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Tag")){
			QString text=xmlReader.readElementText();
			if(readTags.contains(text)){
				xmlReader.raiseError(tr("Duplicate activity tag %1 found in constraint activity tags not overlapping").arg(text));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			else{
				readTags.insert(text);
			}
			cn->activityTagsNames.append(text);
			xmlReadingLog+="    Read activity tag="+cn->activityTagsNames.at(cn->activityTagsNames.count()-1)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(!(nActivityTags==cn->activityTagsNames.count())){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Activity_Tags").arg("Activity_Tag"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(nActivityTags==cn->activityTagsNames.count());
	return cn;
}

TimeConstraint* Rules::readActivitiesSameStartingTime(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintActivitiesSameStartingTime"));
	ConstraintActivitiesSameStartingTime* cn=new ConstraintActivitiesSameStartingTime();
	cn->n_activities=0;
	int n_act=0;
	cn->activitiesId.clear();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Number_of_Activities")){
			QString text=xmlReader.readElementText();
			cn->n_activities=text.toInt();
			xmlReadingLog+="    Read n_activities="+CustomFETString::number(cn->n_activities)+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Id")){
			QString text=xmlReader.readElementText();
			//cn->activitiesId[n_act]=text.toInt();
			cn->activitiesId.append(text.toInt());
			assert(n_act==cn->activitiesId.count()-1);
			xmlReadingLog+="    Read activity id="+CustomFETString::number(cn->activitiesId[n_act])+"\n";
			n_act++;
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(!(n_act==cn->n_activities)){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Activities").arg("Activity_Id"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(n_act==cn->n_activities);
	return cn;
}

TimeConstraint* Rules::readActivitiesSameStartingHour(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintActivitiesSameStartingHour"));
	ConstraintActivitiesSameStartingHour* cn=new ConstraintActivitiesSameStartingHour();
	cn->n_activities=0;
	int n_act=0;
	cn->activitiesId.clear();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Number_of_Activities")){
			QString text=xmlReader.readElementText();
			cn->n_activities=text.toInt();
			xmlReadingLog+="    Read n_activities="+CustomFETString::number(cn->n_activities)+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Id")){
			QString text=xmlReader.readElementText();
			//cn->activitiesId[n_act]=text.toInt();
			cn->activitiesId.append(text.toInt());
			assert(n_act==cn->activitiesId.count()-1);
			xmlReadingLog+="    Read activity id="+CustomFETString::number(cn->activitiesId[n_act])+"\n";
			n_act++;
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(!(n_act==cn->n_activities)){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Activities").arg("Activity_Id"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(n_act==cn->n_activities);
	return cn;
}

TimeConstraint* Rules::readActivitiesSameStartingDay(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintActivitiesSameStartingDay"));
	ConstraintActivitiesSameStartingDay* cn=new ConstraintActivitiesSameStartingDay();
	cn->n_activities=0;
	int n_act=0;
	cn->activitiesId.clear();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Number_of_Activities")){
			QString text=xmlReader.readElementText();
			cn->n_activities=text.toInt();
			xmlReadingLog+="    Read n_activities="+CustomFETString::number(cn->n_activities)+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Id")){
			QString text=xmlReader.readElementText();
			//cn->activitiesId[n_act]=text.toInt();
			cn->activitiesId.append(text.toInt());
			assert(n_act==cn->activitiesId.count()-1);
			xmlReadingLog+="    Read activity id="+CustomFETString::number(cn->activitiesId[n_act])+"\n";
			n_act++;
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(!(n_act==cn->n_activities)){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Activities").arg("Activity_Id"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(n_act==cn->n_activities);
	return cn;
}

TimeConstraint* Rules::readTeachersMaxHoursDaily(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeachersMaxHoursDaily"));
	ConstraintTeachersMaxHoursDaily* cn=new ConstraintTeachersMaxHoursDaily();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Maximum_Hours_Daily")){
			QString text=xmlReader.readElementText();
			cn->maxHoursDaily=text.toInt();
			xmlReadingLog+="    Read maxHoursDaily="+CustomFETString::number(cn->maxHoursDaily)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readTeacherMaxHoursDaily(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeacherMaxHoursDaily"));
	ConstraintTeacherMaxHoursDaily* cn=new ConstraintTeacherMaxHoursDaily();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Maximum_Hours_Daily")){
			QString text=xmlReader.readElementText();
			cn->maxHoursDaily=text.toInt();
			xmlReadingLog+="    Read maxHoursDaily="+CustomFETString::number(cn->maxHoursDaily)+"\n";
		}
		else if(xmlReader.name()==QString("Teacher_Name")){
			QString text=xmlReader.readElementText();
			cn->teacherName=text;
			xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readTeachersMaxHoursContinuously(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeachersMaxHoursContinuously"));
	ConstraintTeachersMaxHoursContinuously* cn=new ConstraintTeachersMaxHoursContinuously();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Maximum_Hours_Continuously")){
			QString text=xmlReader.readElementText();
			cn->maxHoursContinuously=text.toInt();
			xmlReadingLog+="    Read maxHoursContinuously="+CustomFETString::number(cn->maxHoursContinuously)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readTeacherMaxHoursContinuously(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeacherMaxHoursContinuously"));
	ConstraintTeacherMaxHoursContinuously* cn=new ConstraintTeacherMaxHoursContinuously();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Maximum_Hours_Continuously")){
			QString text=xmlReader.readElementText();
			cn->maxHoursContinuously=text.toInt();
			xmlReadingLog+="    Read maxHoursContinuously="+CustomFETString::number(cn->maxHoursContinuously)+"\n";
		}
		else if(xmlReader.name()==QString("Teacher_Name")){
			QString text=xmlReader.readElementText();
			cn->teacherName=text;
			xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readTeacherActivityTagMaxHoursContinuously(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeacherActivityTagMaxHoursContinuously"));
	ConstraintTeacherActivityTagMaxHoursContinuously* cn=new ConstraintTeacherActivityTagMaxHoursContinuously();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Maximum_Hours_Continuously")){
			QString text=xmlReader.readElementText();
			cn->maxHoursContinuously=text.toInt();
			xmlReadingLog+="    Read maxHoursContinuously="+CustomFETString::number(cn->maxHoursContinuously)+"\n";
		}
		else if(xmlReader.name()==QString("Teacher_Name")){
			QString text=xmlReader.readElementText();
			cn->teacherName=text;
			xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Tag_Name")){
			QString text=xmlReader.readElementText();
			cn->activityTagName=text;
			xmlReadingLog+="    Read activity tag name="+cn->activityTagName+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readTeachersActivityTagMaxHoursContinuously(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeachersActivityTagMaxHoursContinuously"));
	ConstraintTeachersActivityTagMaxHoursContinuously* cn=new ConstraintTeachersActivityTagMaxHoursContinuously();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Maximum_Hours_Continuously")){
			QString text=xmlReader.readElementText();
			cn->maxHoursContinuously=text.toInt();
			xmlReadingLog+="    Read maxHoursContinuously="+CustomFETString::number(cn->maxHoursContinuously)+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Tag_Name")){
			QString text=xmlReader.readElementText();
			cn->activityTagName=text;
			xmlReadingLog+="    Read activity tag name="+cn->activityTagName+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readTeacherActivityTagMaxHoursDaily(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeacherActivityTagMaxHoursDaily"));
	ConstraintTeacherActivityTagMaxHoursDaily* cn=new ConstraintTeacherActivityTagMaxHoursDaily();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Maximum_Hours_Daily")){
			QString text=xmlReader.readElementText();
			cn->maxHoursDaily=text.toInt();
			xmlReadingLog+="    Read maxHoursDaily="+CustomFETString::number(cn->maxHoursDaily)+"\n";
		}
		else if(xmlReader.name()==QString("Teacher_Name")){
			QString text=xmlReader.readElementText();
			cn->teacherName=text;
			xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Tag_Name")){
			QString text=xmlReader.readElementText();
			cn->activityTagName=text;
			xmlReadingLog+="    Read activity tag name="+cn->activityTagName+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readTeachersActivityTagMaxHoursDaily(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeachersActivityTagMaxHoursDaily"));
	ConstraintTeachersActivityTagMaxHoursDaily* cn=new ConstraintTeachersActivityTagMaxHoursDaily();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Maximum_Hours_Daily")){
			QString text=xmlReader.readElementText();
			cn->maxHoursDaily=text.toInt();
			xmlReadingLog+="    Read maxHoursDaily="+CustomFETString::number(cn->maxHoursDaily)+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Tag_Name")){
			QString text=xmlReader.readElementText();
			cn->activityTagName=text;
			xmlReadingLog+="    Read activity tag name="+cn->activityTagName+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readTeacherActivityTagMinHoursDaily(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeacherActivityTagMinHoursDaily"));
	ConstraintTeacherActivityTagMinHoursDaily* cn=new ConstraintTeacherActivityTagMinHoursDaily();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Minimum_Hours_Daily")){
			QString text=xmlReader.readElementText();
			cn->minHoursDaily=text.toInt();
			xmlReadingLog+="    Read minHoursDaily="+CustomFETString::number(cn->minHoursDaily)+"\n";
		}
		else if(xmlReader.name()==QString("Teacher_Name")){
			QString text=xmlReader.readElementText();
			cn->teacherName=text;
			xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Tag_Name")){
			QString text=xmlReader.readElementText();
			cn->activityTagName=text;
			xmlReadingLog+="    Read activity tag name="+cn->activityTagName+"\n";
		}
		else if(xmlReader.name()==QString("Allow_Empty_Days")){
			QString text=xmlReader.readElementText();
			if(text=="true")
				cn->allowEmptyDays=true;
			else
				cn->allowEmptyDays=false;
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readTeachersActivityTagMinHoursDaily(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeachersActivityTagMinHoursDaily"));
	ConstraintTeachersActivityTagMinHoursDaily* cn=new ConstraintTeachersActivityTagMinHoursDaily();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Minimum_Hours_Daily")){
			QString text=xmlReader.readElementText();
			cn->minHoursDaily=text.toInt();
			xmlReadingLog+="    Read minHoursDaily="+CustomFETString::number(cn->minHoursDaily)+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Tag_Name")){
			QString text=xmlReader.readElementText();
			cn->activityTagName=text;
			xmlReadingLog+="    Read activity tag name="+cn->activityTagName+"\n";
		}
		else if(xmlReader.name()==QString("Allow_Empty_Days")){
			QString text=xmlReader.readElementText();
			if(text=="true")
				cn->allowEmptyDays=true;
			else
				cn->allowEmptyDays=false;
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readTeachersMinHoursDaily(QWidget* parent, QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeachersMinHoursDaily"));
	ConstraintTeachersMinHoursDaily* cn=new ConstraintTeachersMinHoursDaily();
	cn->allowEmptyDays=true;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Minimum_Hours_Daily")){
			QString text=xmlReader.readElementText();
			cn->minHoursDaily=text.toInt();
			xmlReadingLog+="    Read minHoursDaily="+CustomFETString::number(cn->minHoursDaily)+"\n";
		}
		else if(xmlReader.name()==QString("Allow_Empty_Days")){
			QString text=xmlReader.readElementText();
			if(text=="true" || text=="1" || text=="yes"){
				xmlReadingLog+="    Read allow empty days=true\n";
				cn->allowEmptyDays=true;
			}
			else{
				if(!(text=="no" || text=="false" || text=="0")){
					RulesReconcilableMessage::warning(parent, tr("FET warning"),
						tr("Found constraint teachers min hours daily with tag allow empty days"
						" which is not 'true', 'false', 'yes', 'no', '1' or '0'."
						" The tag will be considered false",
						"Instructions for translators: please leave the 'true', 'false', 'yes' and 'no' fields untranslated, as they are in English"));
				}
				//assert(text=="false" || text=="0" || text=="no");
				xmlReadingLog+="    Read allow empty days=false\n";
				cn->allowEmptyDays=false;
			}
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readTeacherMinHoursDaily(QWidget* parent, QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeacherMinHoursDaily"));
	ConstraintTeacherMinHoursDaily* cn=new ConstraintTeacherMinHoursDaily();
	cn->allowEmptyDays=true;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Minimum_Hours_Daily")){
			QString text=xmlReader.readElementText();
			cn->minHoursDaily=text.toInt();
			xmlReadingLog+="    Read minHoursDaily="+CustomFETString::number(cn->minHoursDaily)+"\n";
		}
		else if(xmlReader.name()==QString("Teacher_Name")){
			QString text=xmlReader.readElementText();
			cn->teacherName=text;
			xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
		}
		else if(xmlReader.name()==QString("Allow_Empty_Days")){
			QString text=xmlReader.readElementText();
			if(text=="true" || text=="1" || text=="yes"){
				xmlReadingLog+="    Read allow empty days=true\n";
				cn->allowEmptyDays=true;
			}
			else{
				if(!(text=="no" || text=="false" || text=="0")){
					RulesReconcilableMessage::warning(parent, tr("FET warning"),
						tr("Found constraint teacher min hours daily with tag allow empty days"
						" which is not 'true', 'false', 'yes', 'no', '1' or '0'."
						" The tag will be considered false",
						"Instructions for translators: please leave the 'true', 'false', 'yes' and 'no' fields untranslated, as they are in English"));
				}
				//assert(text=="false" || text=="0" || text=="no");
				xmlReadingLog+="    Read allow empty days=false\n";
				cn->allowEmptyDays=false;
			}
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readStudentsMaxHoursDaily(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsMaxHoursDaily"));
	ConstraintStudentsMaxHoursDaily* cn=new ConstraintStudentsMaxHoursDaily();
	cn->maxHoursDaily=-1;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Maximum_Hours_Daily")){
			QString text=xmlReader.readElementText();
			cn->maxHoursDaily=text.toInt();
			if(cn->maxHoursDaily<0){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Maximum_Hours_Daily"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			xmlReadingLog+="    Read maxHoursDaily="+CustomFETString::number(cn->maxHoursDaily)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(cn->maxHoursDaily<0){
		xmlReader.raiseError(tr("%1 not found").arg("Maximum_Hours_Daily"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(cn->maxHoursDaily>=0);
	return cn;
}

TimeConstraint* Rules::readStudentsSetMaxHoursDaily(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsSetMaxHoursDaily"));
	ConstraintStudentsSetMaxHoursDaily* cn=new ConstraintStudentsSetMaxHoursDaily();
	cn->maxHoursDaily=-1;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Maximum_Hours_Daily")){
			QString text=xmlReader.readElementText();
			cn->maxHoursDaily=text.toInt();
			if(cn->maxHoursDaily<0){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Maximum_Hours_Daily"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			xmlReadingLog+="    Read maxHoursDaily="+CustomFETString::number(cn->maxHoursDaily)+"\n";
		}
		else if(xmlReader.name()==QString("Students")){
			QString text=xmlReader.readElementText();
			cn->students=text;
			xmlReadingLog+="    Read students name="+cn->students+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(cn->maxHoursDaily<0){
		xmlReader.raiseError(tr("%1 not found").arg("Maximum_Hours_Daily"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(cn->maxHoursDaily>=0);
	return cn;
}

TimeConstraint* Rules::readStudentsMaxHoursContinuously(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsMaxHoursContinuously"));
	ConstraintStudentsMaxHoursContinuously* cn=new ConstraintStudentsMaxHoursContinuously();
	cn->maxHoursContinuously=-1;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Maximum_Hours_Continuously")){
			QString text=xmlReader.readElementText();
			cn->maxHoursContinuously=text.toInt();
			if(cn->maxHoursContinuously<0){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Maximum_Hours_Continuously"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			xmlReadingLog+="    Read maxHoursContinuously="+CustomFETString::number(cn->maxHoursContinuously)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(cn->maxHoursContinuously<0){
		xmlReader.raiseError(tr("%1 not found").arg("Maximum_Hours_Continuously"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(cn->maxHoursContinuously>=0);
	return cn;
}

TimeConstraint* Rules::readStudentsSetMaxHoursContinuously(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsSetMaxHoursContinuously"));
	ConstraintStudentsSetMaxHoursContinuously* cn=new ConstraintStudentsSetMaxHoursContinuously();
	cn->maxHoursContinuously=-1;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Maximum_Hours_Continuously")){
			QString text=xmlReader.readElementText();
			cn->maxHoursContinuously=text.toInt();
			if(cn->maxHoursContinuously<0){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Maximum_Hours_Continuously"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			xmlReadingLog+="    Read maxHoursContinuously="+CustomFETString::number(cn->maxHoursContinuously)+"\n";
		}
		else if(xmlReader.name()==QString("Students")){
			QString text=xmlReader.readElementText();
			cn->students=text;
			xmlReadingLog+="    Read students name="+cn->students+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(cn->maxHoursContinuously<0){
		xmlReader.raiseError(tr("%1 not found").arg("Maximum_Hours_Continuously"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(cn->maxHoursContinuously>=0);
	return cn;
}

TimeConstraint* Rules::readStudentsSetActivityTagMaxHoursContinuously(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsSetActivityTagMaxHoursContinuously"));
	ConstraintStudentsSetActivityTagMaxHoursContinuously* cn=new ConstraintStudentsSetActivityTagMaxHoursContinuously();
	cn->maxHoursContinuously=-1;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Maximum_Hours_Continuously")){
			QString text=xmlReader.readElementText();
			cn->maxHoursContinuously=text.toInt();
			if(cn->maxHoursContinuously<0){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Maximum_Hours_Continuously"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			xmlReadingLog+="    Read maxHoursContinuously="+CustomFETString::number(cn->maxHoursContinuously)+"\n";
		}
		else if(xmlReader.name()==QString("Students")){
			QString text=xmlReader.readElementText();
			cn->students=text;
			xmlReadingLog+="    Read students name="+cn->students+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Tag")){
			QString text=xmlReader.readElementText();
			cn->activityTagName=text;
			xmlReadingLog+="    Read activity tag name="+cn->activityTagName+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(cn->maxHoursContinuously<0){
		xmlReader.raiseError(tr("%1 not found").arg("Maximum_Hours_Continuously"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(cn->maxHoursContinuously>=0);
	return cn;
}

TimeConstraint* Rules::readStudentsActivityTagMaxHoursContinuously(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsActivityTagMaxHoursContinuously"));
	ConstraintStudentsActivityTagMaxHoursContinuously* cn=new ConstraintStudentsActivityTagMaxHoursContinuously();
	cn->maxHoursContinuously=-1;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Maximum_Hours_Continuously")){
			QString text=xmlReader.readElementText();
			cn->maxHoursContinuously=text.toInt();
			if(cn->maxHoursContinuously<0){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Maximum_Hours_Continuously"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			xmlReadingLog+="    Read maxHoursContinuously="+CustomFETString::number(cn->maxHoursContinuously)+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Tag")){
			QString text=xmlReader.readElementText();
			cn->activityTagName=text;
			xmlReadingLog+="    Read activity tag name="+cn->activityTagName+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(cn->maxHoursContinuously<0){
		xmlReader.raiseError(tr("%1 not found").arg("Maximum_Hours_Continuously"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(cn->maxHoursContinuously>=0);
	return cn;
}

TimeConstraint* Rules::readStudentsSetActivityTagMaxHoursDaily(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsSetActivityTagMaxHoursDaily"));
	ConstraintStudentsSetActivityTagMaxHoursDaily* cn=new ConstraintStudentsSetActivityTagMaxHoursDaily();
	cn->maxHoursDaily=-1;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Maximum_Hours_Daily")){
			QString text=xmlReader.readElementText();
			cn->maxHoursDaily=text.toInt();
			if(cn->maxHoursDaily<0){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Maximum_Hours_Daily"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			xmlReadingLog+="    Read maxHoursDaily="+CustomFETString::number(cn->maxHoursDaily)+"\n";
		}
		else if(xmlReader.name()==QString("Students")){
			QString text=xmlReader.readElementText();
			cn->students=text;
			xmlReadingLog+="    Read students name="+cn->students+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Tag")){
			QString text=xmlReader.readElementText();
			cn->activityTagName=text;
			xmlReadingLog+="    Read activity tag name="+cn->activityTagName+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(cn->maxHoursDaily<0){
		xmlReader.raiseError(tr("%1 not found").arg("Maximum_Hours_Daily"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(cn->maxHoursDaily>=0);
	return cn;
}

TimeConstraint* Rules::readStudentsActivityTagMaxHoursDaily(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsActivityTagMaxHoursDaily"));
	ConstraintStudentsActivityTagMaxHoursDaily* cn=new ConstraintStudentsActivityTagMaxHoursDaily();
	cn->maxHoursDaily=-1;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Maximum_Hours_Daily")){
			QString text=xmlReader.readElementText();
			cn->maxHoursDaily=text.toInt();
			if(cn->maxHoursDaily<0){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Maximum_Hours_Daily"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			xmlReadingLog+="    Read maxHoursDaily="+CustomFETString::number(cn->maxHoursDaily)+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Tag")){
			QString text=xmlReader.readElementText();
			cn->activityTagName=text;
			xmlReadingLog+="    Read activity tag name="+cn->activityTagName+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(cn->maxHoursDaily<0){
		xmlReader.raiseError(tr("%1 not found").arg("Maximum_Hours_Daily"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(cn->maxHoursDaily>=0);
	return cn;
}

TimeConstraint* Rules::readStudentsSetActivityTagMinHoursDaily(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsSetActivityTagMinHoursDaily"));
	ConstraintStudentsSetActivityTagMinHoursDaily* cn=new ConstraintStudentsSetActivityTagMinHoursDaily();
	cn->minHoursDaily=-1;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Minimum_Hours_Daily")){
			QString text=xmlReader.readElementText();
			cn->minHoursDaily=text.toInt();
			if(cn->minHoursDaily<0){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Minimum_Hours_Daily"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			xmlReadingLog+="    Read minHoursDaily="+CustomFETString::number(cn->minHoursDaily)+"\n";
		}
		else if(xmlReader.name()==QString("Students")){
			QString text=xmlReader.readElementText();
			cn->students=text;
			xmlReadingLog+="    Read students name="+cn->students+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Tag")){
			QString text=xmlReader.readElementText();
			cn->activityTagName=text;
			xmlReadingLog+="    Read activity tag name="+cn->activityTagName+"\n";
		}
		else if(xmlReader.name()==QString("Allow_Empty_Days")){
			QString text=xmlReader.readElementText();
			if(text=="true")
				cn->allowEmptyDays=true;
			else
				cn->allowEmptyDays=false;
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(cn->minHoursDaily<0){
		xmlReader.raiseError(tr("%1 not found").arg("Minimum_Hours_Daily"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(cn->minHoursDaily>=0);
	return cn;
}

TimeConstraint* Rules::readStudentsActivityTagMinHoursDaily(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsActivityTagMinHoursDaily"));
	ConstraintStudentsActivityTagMinHoursDaily* cn=new ConstraintStudentsActivityTagMinHoursDaily();
	cn->minHoursDaily=-1;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Minimum_Hours_Daily")){
			QString text=xmlReader.readElementText();
			cn->minHoursDaily=text.toInt();
			if(cn->minHoursDaily<0){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Minimum_Hours_Daily"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			xmlReadingLog+="    Read minHoursDaily="+CustomFETString::number(cn->minHoursDaily)+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Tag")){
			QString text=xmlReader.readElementText();
			cn->activityTagName=text;
			xmlReadingLog+="    Read activity tag name="+cn->activityTagName+"\n";
		}
		else if(xmlReader.name()==QString("Allow_Empty_Days")){
			QString text=xmlReader.readElementText();
			if(text=="true")
				cn->allowEmptyDays=true;
			else
				cn->allowEmptyDays=false;
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(cn->minHoursDaily<0){
		xmlReader.raiseError(tr("%1 not found").arg("Minimum_Hours_Daily"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(cn->minHoursDaily>=0);
	return cn;
}

TimeConstraint* Rules::readStudentsMinHoursDaily(QWidget* parent, QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsMinHoursDaily"));
	ConstraintStudentsMinHoursDaily* cn=new ConstraintStudentsMinHoursDaily();
	cn->minHoursDaily=-1;
	cn->allowEmptyDays=false;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Minimum_Hours_Daily")){
			QString text=xmlReader.readElementText();
			cn->minHoursDaily=text.toInt();
			if(cn->minHoursDaily<0){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Minimum_Hours_Daily"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			xmlReadingLog+="    Read minHoursDaily="+CustomFETString::number(cn->minHoursDaily)+"\n";
		}
		else if(xmlReader.name()==QString("Allow_Empty_Days")){
			QString text=xmlReader.readElementText();
			if(text=="true" || text=="1" || text=="yes"){
				xmlReadingLog+="    Read allow empty days=true\n";
				cn->allowEmptyDays=true;
			}
			else{
				if(!(text=="no" || text=="false" || text=="0")){
					RulesReconcilableMessage::warning(parent, tr("FET warning"),
						tr("Found constraint students min hours daily with tag allow empty days"
						" which is not 'true', 'false', 'yes', 'no', '1' or '0'."
						" The tag will be considered false",
						"Instructions for translators: please leave the 'true', 'false', 'yes' and 'no' fields untranslated, as they are in English"));
				}
				//assert(text=="false" || text=="0" || text=="no");
				xmlReadingLog+="    Read allow empty days=false\n";
				cn->allowEmptyDays=false;
			}
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(cn->minHoursDaily<0){
		xmlReader.raiseError(tr("%1 not found").arg("Minimum_Hours_Daily"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(cn->minHoursDaily>=0);
	return cn;
}

TimeConstraint* Rules::readStudentsSetMinHoursDaily(QWidget* parent, QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsSetMinHoursDaily"));
	ConstraintStudentsSetMinHoursDaily* cn=new ConstraintStudentsSetMinHoursDaily();
	cn->minHoursDaily=-1;
	cn->allowEmptyDays=false;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Minimum_Hours_Daily")){
			QString text=xmlReader.readElementText();
			cn->minHoursDaily=text.toInt();
			if(cn->minHoursDaily<0){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Minimum_Hours_Daily"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			xmlReadingLog+="    Read minHoursDaily="+CustomFETString::number(cn->minHoursDaily)+"\n";
		}
		else if(xmlReader.name()==QString("Students")){
			QString text=xmlReader.readElementText();
			cn->students=text;
			xmlReadingLog+="    Read students name="+cn->students+"\n";
		}
		else if(xmlReader.name()==QString("Allow_Empty_Days")){
			QString text=xmlReader.readElementText();
			if(text=="true" || text=="1" || text=="yes"){
				xmlReadingLog+="    Read allow empty days=true\n";
				cn->allowEmptyDays=true;
			}
			else{
				if(!(text=="no" || text=="false" || text=="0")){
					RulesReconcilableMessage::warning(parent, tr("FET warning"),
						tr("Found constraint students set min hours daily with tag allow empty days"
						" which is not 'true', 'false', 'yes', 'no', '1' or '0'."
						" The tag will be considered false",
						"Instructions for translators: please leave the 'true', 'false', 'yes' and 'no' fields untranslated, as they are in English"));
				}
				//assert(text=="false" || text=="0" || text=="no");
				xmlReadingLog+="    Read allow empty days=false\n";
				cn->allowEmptyDays=false;
			}
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(cn->minHoursDaily<0){
		xmlReader.raiseError(tr("%1 not found").arg("Minimum_Hours_Daily"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(cn->minHoursDaily>=0);
	return cn;
}

TimeConstraint* Rules::readStudentsSetMinGapsBetweenOrderedPairOfActivityTags(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTags"));
	ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTags* cn=new ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTags();
	cn->minGaps=-1;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("MinGaps")){
			QString text=xmlReader.readElementText();
			cn->minGaps=text.toInt();
			if(cn->minGaps<0){
				xmlReader.raiseError(tr("%1 is incorrect").arg("MinGaps"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			xmlReadingLog+="    Read minGaps="+CustomFETString::number(cn->minGaps)+"\n";
		}
		else if(xmlReader.name()==QString("Students")){
			QString text=xmlReader.readElementText();
			cn->students=text;
			xmlReadingLog+="    Read students name="+cn->students+"\n";
		}
		else if(xmlReader.name()==QString("First_Activity_Tag")){
			QString text=xmlReader.readElementText();
			cn->firstActivityTag=text;
			xmlReadingLog+="    Read first activity name="+cn->firstActivityTag+"\n";
		}
		else if(xmlReader.name()==QString("Second_Activity_Tag")){
			QString text=xmlReader.readElementText();
			cn->secondActivityTag=text;
			xmlReadingLog+="    Read second activity name="+cn->secondActivityTag+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(cn->minGaps<0){
		xmlReader.raiseError(tr("%1 not found").arg("MinGaps"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(cn->minGaps>=0);
	return cn;
}

TimeConstraint* Rules::readStudentsMinGapsBetweenOrderedPairOfActivityTags(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTags"));
	ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTags* cn=new ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTags();
	cn->minGaps=-1;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("MinGaps")){
			QString text=xmlReader.readElementText();
			cn->minGaps=text.toInt();
			if(cn->minGaps<0){
				xmlReader.raiseError(tr("%1 is incorrect").arg("MinGaps"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			xmlReadingLog+="    Read minGaps="+CustomFETString::number(cn->minGaps)+"\n";
		}
		else if(xmlReader.name()==QString("First_Activity_Tag")){
			QString text=xmlReader.readElementText();
			cn->firstActivityTag=text;
			xmlReadingLog+="    Read first activity name="+cn->firstActivityTag+"\n";
		}
		else if(xmlReader.name()==QString("Second_Activity_Tag")){
			QString text=xmlReader.readElementText();
			cn->secondActivityTag=text;
			xmlReadingLog+="    Read second activity name="+cn->secondActivityTag+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(cn->minGaps<0){
		xmlReader.raiseError(tr("%1 not found").arg("MinGaps"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(cn->minGaps>=0);
	return cn;
}

TimeConstraint* Rules::readTeacherMinGapsBetweenOrderedPairOfActivityTags(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTags"));
	ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTags* cn=new ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTags();
	cn->minGaps=-1;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("MinGaps")){
			QString text=xmlReader.readElementText();
			cn->minGaps=text.toInt();
			if(cn->minGaps<0){
				xmlReader.raiseError(tr("%1 is incorrect").arg("MinGaps"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			xmlReadingLog+="    Read minGaps="+CustomFETString::number(cn->minGaps)+"\n";
		}
		else if(xmlReader.name()==QString("Teacher")){
			QString text=xmlReader.readElementText();
			cn->teacher=text;
			xmlReadingLog+="    Read teacher name="+cn->teacher+"\n";
		}
		else if(xmlReader.name()==QString("First_Activity_Tag")){
			QString text=xmlReader.readElementText();
			cn->firstActivityTag=text;
			xmlReadingLog+="    Read first activity name="+cn->firstActivityTag+"\n";
		}
		else if(xmlReader.name()==QString("Second_Activity_Tag")){
			QString text=xmlReader.readElementText();
			cn->secondActivityTag=text;
			xmlReadingLog+="    Read second activity name="+cn->secondActivityTag+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(cn->minGaps<0){
		xmlReader.raiseError(tr("%1 not found").arg("MinGaps"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(cn->minGaps>=0);
	return cn;
}

TimeConstraint* Rules::readTeachersMinGapsBetweenOrderedPairOfActivityTags(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTags"));
	ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTags* cn=new ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTags();
	cn->minGaps=-1;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("MinGaps")){
			QString text=xmlReader.readElementText();
			cn->minGaps=text.toInt();
			if(cn->minGaps<0){
				xmlReader.raiseError(tr("%1 is incorrect").arg("MinGaps"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			xmlReadingLog+="    Read minGaps="+CustomFETString::number(cn->minGaps)+"\n";
		}
		else if(xmlReader.name()==QString("First_Activity_Tag")){
			QString text=xmlReader.readElementText();
			cn->firstActivityTag=text;
			xmlReadingLog+="    Read first activity name="+cn->firstActivityTag+"\n";
		}
		else if(xmlReader.name()==QString("Second_Activity_Tag")){
			QString text=xmlReader.readElementText();
			cn->secondActivityTag=text;
			xmlReadingLog+="    Read second activity name="+cn->secondActivityTag+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(cn->minGaps<0){
		xmlReader.raiseError(tr("%1 not found").arg("MinGaps"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(cn->minGaps>=0);
	return cn;
}

TimeConstraint* Rules::readActivityPreferredTime(QWidget* parent, QXmlStreamReader& xmlReader, FakeString& xmlReadingLog,
bool& reportUnspecifiedPermanentlyLockedTime, bool& reportUnspecifiedDayOrHourPreferredStartingTime){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintActivityPreferredTime"));

	ConstraintActivityPreferredStartingTime* cn=new ConstraintActivityPreferredStartingTime();
	cn->day = cn->hour = -1;
	cn->permanentlyLocked=false; //default not locked
	bool foundLocked=false;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Permanently_Locked")){
			QString text=xmlReader.readElementText();
			if(text=="true" || text=="1" || text=="yes"){
				xmlReadingLog+="    Permanently locked\n";
				cn->permanentlyLocked=true;
			}
			else{
				if(!(text=="no" || text=="false" || text=="0")){
					RulesReconcilableMessage::warning(parent, tr("FET warning"),
						tr("Found constraint activity preferred starting time with tag permanently locked"
						" which is not 'true', 'false', 'yes', 'no', '1' or '0'."
						" The tag will be considered false",
						"Instructions for translators: please leave the 'true', 'false', 'yes' and 'no' fields untranslated, as they are in English"));
				}
				//assert(text=="false" || text=="0" || text=="no");
				xmlReadingLog+="    Not permanently locked\n";
				cn->permanentlyLocked=false;
			}
			foundLocked=true;
		}
		else if(xmlReader.name()==QString("Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->activityId=text.toInt();
			xmlReadingLog+="    Read activity id="+CustomFETString::number(cn->activityId)+"\n";
		}
		else if(xmlReader.name()==QString("Preferred_Day")){
			QString text=xmlReader.readElementText();
			for(cn->day=0; cn->day<this->nDaysPerWeek; cn->day++)
				if(this->daysOfTheWeek[cn->day]==text)
					break;
			if(cn->day>=this->nDaysPerWeek){
				xmlReader.raiseError(tr("Day %1 is inexistent").arg(text));
				/*RulesReconcilableMessage::information(parent, tr("FET information"),
					tr("Constraint ActivityPreferredTime day corrupt for activity with id %1, day %2 is inexistent ... ignoring constraint")
					.arg(cn->activityId)
					.arg(text));*/
				delete cn;
				cn=nullptr;
				//goto corruptConstraintTime;
				return nullptr;
			}
			assert(cn->day<this->nDaysPerWeek);
			xmlReadingLog+="    Preferred day="+this->daysOfTheWeek[cn->day]+"\n";
		}
		else if(xmlReader.name()==QString("Preferred_Hour")){
			QString text=xmlReader.readElementText();
			for(cn->hour=0; cn->hour < this->nHoursPerDay; cn->hour++)
				if(this->hoursOfTheDay[cn->hour]==text)
					break;
			if(cn->hour>=this->nHoursPerDay){
				xmlReader.raiseError(tr("Hour %1 is inexistent").arg(text));
				/*RulesReconcilableMessage::information(parent, tr("FET information"),
					tr("Constraint ActivityPreferredTime hour corrupt for activity with id %1, hour %2 is inexistent ... ignoring constraint")
					.arg(cn->activityId)
					.arg(text));*/
				delete cn;
				cn=nullptr;
				//goto corruptConstraintTime;
				return nullptr;
			}
			assert(cn->hour>=0 && cn->hour < this->nHoursPerDay);
			xmlReadingLog+="    Preferred hour="+this->hoursOfTheDay[cn->hour]+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	//crt_constraint=cn;

	if(cn->hour>=0 && cn->day>=0 && !foundLocked && reportUnspecifiedPermanentlyLockedTime){
		int t=RulesReconcilableMessage::information(parent, tr("FET information"),
			tr("Found constraint activity preferred starting time, with unspecified tag"
			" 'permanently locked' - this tag will be set to 'false' by default. You can always modify it"
			" by editing the constraint in the 'Data' menu")+"\n\n"
			+tr("Explanation: starting with version 5.8.0 (January 2009), the constraint"
			" activity preferred starting time has"
			" a new tag, 'permanently locked' (true or false)."
			" It is recommended to make the tag 'permanently locked' true for the constraints you"
			" need to be not modifiable from the 'Timetable' menu"
			" and leave this tag false for the constraints you need to be modifiable from the 'Timetable' menu"
			" (the 'permanently locked' tag can be modified by editing the constraint from the 'Data' menu)."
			" This way, when viewing the timetable"
			" and locking/unlocking some activities, you will not unlock the constraints which"
			" need to be locked all the time."
			),
			tr("Skip rest"), tr("See next"), QString(), 1, 0 );
		if(t==0)
			reportUnspecifiedPermanentlyLockedTime=false;
	}
	
	if(cn->hour==-1 || cn->day==-1){
		if(reportUnspecifiedDayOrHourPreferredStartingTime){
			int t=RulesReconcilableMessage::information(parent, tr("FET information"),
				tr("Found constraint activity preferred starting time, with unspecified day or hour."
				" This constraint will be transformed into constraint activity preferred starting times (a set of times, not only one)."
				" This change is done in FET versions 5.8.1 and higher."
				),
				tr("Skip rest"), tr("See next"), QString(), 1, 0 );
			if(t==0)
				reportUnspecifiedDayOrHourPreferredStartingTime=false;
		}
			
		ConstraintActivityPreferredStartingTimes* cgood=new ConstraintActivityPreferredStartingTimes();
		if(cn->day==-1){
			cgood->activityId=cn->activityId;
			cgood->weightPercentage=cn->weightPercentage;
			cgood->nPreferredStartingTimes_L=this->nDaysPerWeek;
			for(int i=0; i<cgood->nPreferredStartingTimes_L; i++){
				/*cgood->days[i]=i;
				cgood->hours[i]=cn->hour;*/
				cgood->days_L.append(i);
				cgood->hours_L.append(cn->hour);
			}
		}
		else{
			assert(cn->hour==-1);
			cgood->activityId=cn->activityId;
			cgood->weightPercentage=cn->weightPercentage;
			cgood->nPreferredStartingTimes_L=this->nHoursPerDay;
			for(int i=0; i<cgood->nPreferredStartingTimes_L; i++){
				/*cgood->days[i]=cn->day;
				cgood->hours[i]=i;*/
				cgood->days_L.append(cn->day);
				cgood->hours_L.append(i);
			}
		}
		
		delete cn;
		return cgood;
	}
	
	return cn;
}

TimeConstraint* Rules::readActivityPreferredStartingTime(QWidget* parent, QXmlStreamReader& xmlReader, FakeString& xmlReadingLog,
bool& reportUnspecifiedPermanentlyLockedTime, bool& reportUnspecifiedDayOrHourPreferredStartingTime){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintActivityPreferredStartingTime"));
	ConstraintActivityPreferredStartingTime* cn=new ConstraintActivityPreferredStartingTime();
	cn->day = cn->hour = -1;
	cn->permanentlyLocked=false; //default false
	bool foundLocked=false;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Permanently_Locked")){
			QString text=xmlReader.readElementText();
			if(text=="true" || text=="1" || text=="yes"){
				xmlReadingLog+="    Permanently locked\n";
				cn->permanentlyLocked=true;
			}
			else{
				if(!(text=="no" || text=="false" || text=="0")){
					RulesReconcilableMessage::warning(parent, tr("FET warning"),
						tr("Found constraint activity preferred starting time with tag permanently locked"
						" which is not 'true', 'false', 'yes', 'no', '1' or '0'."
						" The tag will be considered false",
						"Instructions for translators: please leave the 'true', 'false', 'yes' and 'no' fields untranslated, as they are in English"));
				}
				//assert(text=="false" || text=="0" || text=="no");
				xmlReadingLog+="    Not permanently locked\n";
				cn->permanentlyLocked=false;
			}
			foundLocked=true;
		}
		else if(xmlReader.name()==QString("Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->activityId=text.toInt();
			xmlReadingLog+="    Read activity id="+CustomFETString::number(cn->activityId)+"\n";
		}
		else if(xmlReader.name()==QString("Preferred_Day")){
			QString text=xmlReader.readElementText();
			for(cn->day=0; cn->day<this->nDaysPerWeek; cn->day++)
				if(this->daysOfTheWeek[cn->day]==text)
					break;
			if(cn->day>=this->nDaysPerWeek){
				xmlReader.raiseError(tr("Day %1 is inexistent").arg(text));
				/*RulesReconcilableMessage::information(parent, tr("FET information"),
					tr("Constraint ActivityPreferredStartingTime day corrupt for activity with id %1, day %2 is inexistent ... ignoring constraint")
					.arg(cn->activityId)
					.arg(text));*/
				delete cn;
				cn=nullptr;
				//goto corruptConstraintTime;
				return nullptr;
			}
			assert(cn->day<this->nDaysPerWeek);
			xmlReadingLog+="    Preferred day="+this->daysOfTheWeek[cn->day]+"\n";
		}
		else if(xmlReader.name()==QString("Preferred_Hour")){
			QString text=xmlReader.readElementText();
			for(cn->hour=0; cn->hour < this->nHoursPerDay; cn->hour++)
				if(this->hoursOfTheDay[cn->hour]==text)
					break;
			if(cn->hour>=this->nHoursPerDay){
				xmlReader.raiseError(tr("Hour %1 is inexistent").arg(text));
				/*RulesReconcilableMessage::information(parent, tr("FET information"),
					tr("Constraint ActivityPreferredStartingTime hour corrupt for activity with id %1, hour %2 is inexistent ... ignoring constraint")
					.arg(cn->activityId)
					.arg(text));*/
				delete cn;
				cn=nullptr;
				//goto corruptConstraintTime;
				return nullptr;
			}
			assert(cn->hour>=0 && cn->hour < this->nHoursPerDay);
			xmlReadingLog+="    Preferred hour="+this->hoursOfTheDay[cn->hour]+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	//crt_constraint=cn;

	if(cn->hour>=0 && cn->day>=0 && !foundLocked && reportUnspecifiedPermanentlyLockedTime){
		int t=RulesReconcilableMessage::information(parent, tr("FET information"),
			tr("Found constraint activity preferred starting time, with unspecified tag"
			" 'permanently locked' - this tag will be set to 'false' by default. You can always modify it"
			" by editing the constraint in the 'Data' menu")+"\n\n"
			+tr("Explanation: starting with version 5.8.0 (January 2009), the constraint"
			" activity preferred starting time has"
			" a new tag, 'permanently locked' (true or false)."
			" It is recommended to make the tag 'permanently locked' true for the constraints you"
			" need to be not modifiable from the 'Timetable' menu"
			" and leave this tag false for the constraints you need to be modifiable from the 'Timetable' menu"
			" (the 'permanently locked' tag can be modified by editing the constraint from the 'Data' menu)."
			" This way, when viewing the timetable"
			" and locking/unlocking some activities, you will not unlock the constraints which"
			" need to be locked all the time."
			),
			tr("Skip rest"), tr("See next"), QString(), 1, 0 );
		if(t==0)
			reportUnspecifiedPermanentlyLockedTime=false;
	}

	if(cn->hour==-1 || cn->day==-1){
		if(reportUnspecifiedDayOrHourPreferredStartingTime){
			int t=RulesReconcilableMessage::information(parent, tr("FET information"),
				tr("Found constraint activity preferred starting time, with unspecified day or hour."
				" This constraint will be transformed into constraint activity preferred starting times (a set of times, not only one)."
				" This change is done in FET versions 5.8.1 and higher."
				),
				tr("Skip rest"), tr("See next"), QString(), 1, 0 );
			if(t==0)
				reportUnspecifiedDayOrHourPreferredStartingTime=false;
		}
			
		ConstraintActivityPreferredStartingTimes* cgood=new ConstraintActivityPreferredStartingTimes();
		if(cn->day==-1){
			cgood->activityId=cn->activityId;
			cgood->weightPercentage=cn->weightPercentage;
			cgood->nPreferredStartingTimes_L=this->nDaysPerWeek;
			for(int i=0; i<cgood->nPreferredStartingTimes_L; i++){
				/*cgood->days[i]=i;
				cgood->hours[i]=cn->hour;*/
				cgood->days_L.append(i);
				cgood->hours_L.append(cn->hour);
			}
		}
		else{
			assert(cn->hour==-1);
			cgood->activityId=cn->activityId;
			cgood->weightPercentage=cn->weightPercentage;
			cgood->nPreferredStartingTimes_L=this->nHoursPerDay;
			for(int i=0; i<cgood->nPreferredStartingTimes_L; i++){
				/*cgood->days[i]=cn->day;
				cgood->hours[i]=i;*/
				cgood->days_L.append(cn->day);
				cgood->hours_L.append(i);
			}
		}
		
		delete cn;
		return cgood;
	}
	
	return cn;
}

TimeConstraint* Rules::readActivityEndsStudentsDay(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintActivityEndsStudentsDay"));
	ConstraintActivityEndsStudentsDay* cn=new ConstraintActivityEndsStudentsDay();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->activityId=text.toInt();
			xmlReadingLog+="    Read activity id="+CustomFETString::number(cn->activityId)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readActivitiesEndStudentsDay(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintActivitiesEndStudentsDay"));
	ConstraintActivitiesEndStudentsDay* cn=new ConstraintActivitiesEndStudentsDay();
	cn->teacherName="";
	cn->studentsName="";
	cn->subjectName="";
	cn->activityTagName="";
	
	//i=0;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Teacher_Name")){
			QString text=xmlReader.readElementText();
			cn->teacherName=text;
			xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
		}
		else if(xmlReader.name()==QString("Students_Name")){
			QString text=xmlReader.readElementText();
			cn->studentsName=text;
			xmlReadingLog+="    Read students name="+cn->studentsName+"\n";
		}
		else if(xmlReader.name()==QString("Subject_Name")){
			QString text=xmlReader.readElementText();
			cn->subjectName=text;
			xmlReadingLog+="    Read subject name="+cn->subjectName+"\n";
		}
		else if(xmlReader.name()==QString("Subject_Tag_Name")){
			QString text=xmlReader.readElementText();
			cn->activityTagName=text;
			xmlReadingLog+="    Read activity tag name="+cn->activityTagName+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Tag_Name")){
			QString text=xmlReader.readElementText();
			cn->activityTagName=text;
			xmlReadingLog+="    Read activity tag name="+cn->activityTagName+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::read2ActivitiesConsecutive(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("Constraint2ActivitiesConsecutive"));
	ConstraintTwoActivitiesConsecutive* cn=new ConstraintTwoActivitiesConsecutive();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("First_Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->firstActivityId=text.toInt();
			xmlReadingLog+="    Read first activity id="+CustomFETString::number(cn->firstActivityId)+"\n";
		}
		else if(xmlReader.name()==QString("Second_Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->secondActivityId=text.toInt();
			xmlReadingLog+="    Read second activity id="+CustomFETString::number(cn->secondActivityId)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::read2ActivitiesGrouped(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("Constraint2ActivitiesGrouped"));
	ConstraintTwoActivitiesGrouped* cn=new ConstraintTwoActivitiesGrouped();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("First_Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->firstActivityId=text.toInt();
			xmlReadingLog+="    Read first activity id="+CustomFETString::number(cn->firstActivityId)+"\n";
		}
		else if(xmlReader.name()==QString("Second_Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->secondActivityId=text.toInt();
			xmlReadingLog+="    Read second activity id="+CustomFETString::number(cn->secondActivityId)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::read3ActivitiesGrouped(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("Constraint3ActivitiesGrouped"));
	ConstraintThreeActivitiesGrouped* cn=new ConstraintThreeActivitiesGrouped();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("First_Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->firstActivityId=text.toInt();
			xmlReadingLog+="    Read first activity id="+CustomFETString::number(cn->firstActivityId)+"\n";
		}
		else if(xmlReader.name()==QString("Second_Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->secondActivityId=text.toInt();
			xmlReadingLog+="    Read second activity id="+CustomFETString::number(cn->secondActivityId)+"\n";
		}
		else if(xmlReader.name()==QString("Third_Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->thirdActivityId=text.toInt();
			xmlReadingLog+="    Read third activity id="+CustomFETString::number(cn->thirdActivityId)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::read2ActivitiesOrdered(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("Constraint2ActivitiesOrdered"));
	ConstraintTwoActivitiesOrdered* cn=new ConstraintTwoActivitiesOrdered();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("First_Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->firstActivityId=text.toInt();
			xmlReadingLog+="    Read first activity id="+CustomFETString::number(cn->firstActivityId)+"\n";
		}
		else if(xmlReader.name()==QString("Second_Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->secondActivityId=text.toInt();
			xmlReadingLog+="    Read second activity id="+CustomFETString::number(cn->secondActivityId)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readTwoActivitiesConsecutive(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTwoActivitiesConsecutive"));
	ConstraintTwoActivitiesConsecutive* cn=new ConstraintTwoActivitiesConsecutive();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("First_Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->firstActivityId=text.toInt();
			xmlReadingLog+="    Read first activity id="+CustomFETString::number(cn->firstActivityId)+"\n";
		}
		else if(xmlReader.name()==QString("Second_Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->secondActivityId=text.toInt();
			xmlReadingLog+="    Read second activity id="+CustomFETString::number(cn->secondActivityId)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readTwoActivitiesGrouped(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTwoActivitiesGrouped"));
	ConstraintTwoActivitiesGrouped* cn=new ConstraintTwoActivitiesGrouped();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("First_Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->firstActivityId=text.toInt();
			xmlReadingLog+="    Read first activity id="+CustomFETString::number(cn->firstActivityId)+"\n";
		}
		else if(xmlReader.name()==QString("Second_Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->secondActivityId=text.toInt();
			xmlReadingLog+="    Read second activity id="+CustomFETString::number(cn->secondActivityId)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readThreeActivitiesGrouped(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintThreeActivitiesGrouped"));
	ConstraintThreeActivitiesGrouped* cn=new ConstraintThreeActivitiesGrouped();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("First_Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->firstActivityId=text.toInt();
			xmlReadingLog+="    Read first activity id="+CustomFETString::number(cn->firstActivityId)+"\n";
		}
		else if(xmlReader.name()==QString("Second_Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->secondActivityId=text.toInt();
			xmlReadingLog+="    Read second activity id="+CustomFETString::number(cn->secondActivityId)+"\n";
		}
		else if(xmlReader.name()==QString("Third_Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->thirdActivityId=text.toInt();
			xmlReadingLog+="    Read third activity id="+CustomFETString::number(cn->thirdActivityId)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readTwoActivitiesOrdered(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTwoActivitiesOrdered"));
	ConstraintTwoActivitiesOrdered* cn=new ConstraintTwoActivitiesOrdered();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("First_Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->firstActivityId=text.toInt();
			xmlReadingLog+="    Read first activity id="+CustomFETString::number(cn->firstActivityId)+"\n";
		}
		else if(xmlReader.name()==QString("Second_Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->secondActivityId=text.toInt();
			xmlReadingLog+="    Read second activity id="+CustomFETString::number(cn->secondActivityId)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readTwoActivitiesOrderedIfSameDay(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTwoActivitiesOrderedIfSameDay"));
	ConstraintTwoActivitiesOrderedIfSameDay* cn=new ConstraintTwoActivitiesOrderedIfSameDay();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("First_Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->firstActivityId=text.toInt();
			xmlReadingLog+="    Read first activity id="+CustomFETString::number(cn->firstActivityId)+"\n";
		}
		else if(xmlReader.name()==QString("Second_Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->secondActivityId=text.toInt();
			xmlReadingLog+="    Read second activity id="+CustomFETString::number(cn->secondActivityId)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readActivityPreferredTimes(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintActivityPreferredTimes"));

	ConstraintActivityPreferredStartingTimes* cn=new ConstraintActivityPreferredStartingTimes();
	cn->nPreferredStartingTimes_L=0;
	int i;
	/*for(i=0; i<MAX_N_CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIMES; i++){
		cn->days[i] = cn->hours[i] = -1;
	}*/
	i=0;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->activityId=text.toInt();
			xmlReadingLog+="    Read activity id="+CustomFETString::number(cn->activityId)+"\n";
		}
		else if(xmlReader.name()==QString("Number_of_Preferred_Times")){
			QString text=xmlReader.readElementText();
			cn->nPreferredStartingTimes_L=text.toInt();
			xmlReadingLog+="    Read number of preferred times="+CustomFETString::number(cn->nPreferredStartingTimes_L)+"\n";
		}
		else if(xmlReader.name()==QString("Preferred_Time")){
			xmlReadingLog+="    Read: preferred time\n";

			assert(xmlReader.isStartElement());
			while(xmlReader.readNextStartElement()){
				xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
				if(xmlReader.name()==QString("Preferred_Day")){
					QString text=xmlReader.readElementText();
					cn->days_L.append(0);
					assert(cn->days_L.count()-1==i);
					for(cn->days_L[i]=0; cn->days_L[i]<this->nDaysPerWeek; cn->days_L[i]++)
						if(this->daysOfTheWeek[cn->days_L[i]]==text)
							break;

					if(cn->days_L[i]>=this->nDaysPerWeek){
						xmlReader.raiseError(tr("Day %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr("Constraint ActivityPreferredTimes day corrupt for activity with id %1, day %2 is inexistent ... ignoring constraint")
							.arg(cn->activityId)
							.arg(text));*/
						delete cn;
						cn=nullptr;
						//goto corruptConstraintTime;
						return nullptr;
					}
		
					assert(cn->days_L[i]<this->nDaysPerWeek);
					xmlReadingLog+="    Preferred day="+this->daysOfTheWeek[cn->days_L[i]]+"("+CustomFETString::number(i)+")"+"\n";
				}
				else if(xmlReader.name()==QString("Preferred_Hour")){
					QString text=xmlReader.readElementText();
					cn->hours_L.append(0);
					assert(cn->hours_L.count()-1==i);
					for(cn->hours_L[i]=0; cn->hours_L[i] < this->nHoursPerDay; cn->hours_L[i]++)
						if(this->hoursOfTheDay[cn->hours_L[i]]==text)
							break;
					
					if(cn->hours_L[i]>=this->nHoursPerDay){
						xmlReader.raiseError(tr("Hour %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr("Constraint ActivityPreferredTimes hour corrupt for activity with id %1, hour %2 is inexistent ... ignoring constraint")
							.arg(cn->activityId)
							.arg(text));*/
						delete cn;
						cn=nullptr;
						//goto corruptConstraintTime;
						return nullptr;
					}
					
					assert(cn->hours_L[i]>=0 && cn->hours_L[i] < this->nHoursPerDay);
					xmlReadingLog+="    Preferred hour="+this->hoursOfTheDay[cn->hours_L[i]]+"\n";
				}
				else{
					unrecognizedXmlTags.append(xmlReader.name().toString());
					unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
					unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

					xmlReader.skipCurrentElement();
					xmlReaderNumberOfUnrecognizedFields++;
				}
			}
			i++;

			if(!(i==cn->days_L.count()) || !(i==cn->hours_L.count())){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Preferred_Time"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			assert(i==cn->days_L.count());
			assert(i==cn->hours_L.count());
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(!(i==cn->nPreferredStartingTimes_L)){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Preferred_Times").arg("Preferred_Time"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(i==cn->nPreferredStartingTimes_L);
	return cn;
}

TimeConstraint* Rules::readActivityPreferredTimeSlots(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintActivityPreferredTimeSlots"));
	ConstraintActivityPreferredTimeSlots* cn=new ConstraintActivityPreferredTimeSlots();
	cn->p_nPreferredTimeSlots_L=0;
	int i;
	/*for(i=0; i<MAX_N_CONSTRAINT_ACTIVITY_PREFERRED_TIME_SLOTS; i++){
		cn->p_days[i] = cn->p_hours[i] = -1;
	}*/
	i=0;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->p_activityId=text.toInt();
			xmlReadingLog+="    Read activity id="+CustomFETString::number(cn->p_activityId)+"\n";
		}
		else if(xmlReader.name()==QString("Number_of_Preferred_Time_Slots")){
			QString text=xmlReader.readElementText();
			cn->p_nPreferredTimeSlots_L=text.toInt();
			xmlReadingLog+="    Read number of preferred times="+CustomFETString::number(cn->p_nPreferredTimeSlots_L)+"\n";
		}
		else if(xmlReader.name()==QString("Preferred_Time_Slot")){
			xmlReadingLog+="    Read: preferred time slot\n";

			assert(xmlReader.isStartElement());
			while(xmlReader.readNextStartElement()){
				xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
				if(xmlReader.name()==QString("Preferred_Day")){
					QString text=xmlReader.readElementText();
					cn->p_days_L.append(0);
					assert(cn->p_days_L.count()-1==i);
					for(cn->p_days_L[i]=0; cn->p_days_L[i]<this->nDaysPerWeek; cn->p_days_L[i]++)
						if(this->daysOfTheWeek[cn->p_days_L[i]]==text)
							break;

					if(cn->p_days_L[i]>=this->nDaysPerWeek){
						xmlReader.raiseError(tr("Day %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr("Constraint ActivityPreferredTimeSlots day corrupt for activity with id %1, day %2 is inexistent ... ignoring constraint")
							.arg(cn->p_activityId)
							.arg(text));*/
						delete cn;
						cn=nullptr;
						//goto corruptConstraintTime;
						return nullptr;
					}
		
					assert(cn->p_days_L[i]<this->nDaysPerWeek);
					xmlReadingLog+="    Preferred day="+this->daysOfTheWeek[cn->p_days_L[i]]+"("+CustomFETString::number(i)+")"+"\n";
				}
				else if(xmlReader.name()==QString("Preferred_Hour")){
					QString text=xmlReader.readElementText();
					cn->p_hours_L.append(0);
					assert(cn->p_hours_L.count()-1==i);
					for(cn->p_hours_L[i]=0; cn->p_hours_L[i] < this->nHoursPerDay; cn->p_hours_L[i]++)
						if(this->hoursOfTheDay[cn->p_hours_L[i]]==text)
							break;
					
					if(cn->p_hours_L[i]>=this->nHoursPerDay){
						xmlReader.raiseError(tr("Hour %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr("Constraint ActivityPreferredTimeSlots hour corrupt for activity with id %1, hour %2 is inexistent ... ignoring constraint")
							.arg(cn->p_activityId)
							.arg(text));*/
						delete cn;
						cn=nullptr;
						//goto corruptConstraintTime;
						return nullptr;
					}
					
					assert(cn->p_hours_L[i]>=0 && cn->p_hours_L[i] < this->nHoursPerDay);
					xmlReadingLog+="    Preferred hour="+this->hoursOfTheDay[cn->p_hours_L[i]]+"\n";
				}
				else{
					unrecognizedXmlTags.append(xmlReader.name().toString());
					unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
					unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

					xmlReader.skipCurrentElement();
					xmlReaderNumberOfUnrecognizedFields++;
				}
			}

			i++;

			if(!(i==cn->p_days_L.count()) || !(i==cn->p_hours_L.count())){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Preferred_Time_Slot"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			assert(i==cn->p_days_L.count());
			assert(i==cn->p_hours_L.count());
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(!(i==cn->p_nPreferredTimeSlots_L)){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Preferred_Time_Slots").arg("Preferred_Time_Slot"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(i==cn->p_nPreferredTimeSlots_L);
	return cn;
}

TimeConstraint* Rules::readActivityPreferredStartingTimes(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintActivityPreferredStartingTimes"));
	ConstraintActivityPreferredStartingTimes* cn=new ConstraintActivityPreferredStartingTimes();
	cn->nPreferredStartingTimes_L=0;
	int i;
	/*for(i=0; i<MAX_N_CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIMES; i++){
		cn->days[i] = cn->hours[i] = -1;
	}*/
	i=0;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->activityId=text.toInt();
			xmlReadingLog+="    Read activity id="+CustomFETString::number(cn->activityId)+"\n";
		}
		else if(xmlReader.name()==QString("Number_of_Preferred_Starting_Times")){
			QString text=xmlReader.readElementText();
			cn->nPreferredStartingTimes_L=text.toInt();
			xmlReadingLog+="    Read number of preferred starting times="+CustomFETString::number(cn->nPreferredStartingTimes_L)+"\n";
		}
		else if(xmlReader.name()==QString("Preferred_Starting_Time")){
			xmlReadingLog+="    Read: preferred starting time\n";

			assert(xmlReader.isStartElement());
			while(xmlReader.readNextStartElement()){
				xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
				if(xmlReader.name()==QString("Preferred_Starting_Day")){
					QString text=xmlReader.readElementText();
					cn->days_L.append(0);
					assert(cn->days_L.count()-1==i);
					for(cn->days_L[i]=0; cn->days_L[i]<this->nDaysPerWeek; cn->days_L[i]++)
						if(this->daysOfTheWeek[cn->days_L[i]]==text)
							break;

					if(cn->days_L[i]>=this->nDaysPerWeek){
						xmlReader.raiseError(tr("Day %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr("Constraint ActivityPreferredStartingTimes day corrupt for activity with id %1, day %2 is inexistent ... ignoring constraint")
							.arg(cn->activityId)
							.arg(text));*/
						delete cn;
						cn=nullptr;
						//goto corruptConstraintTime;
						return nullptr;
					}
					
					assert(cn->days_L[i]<this->nDaysPerWeek);
					xmlReadingLog+="    Preferred starting day="+this->daysOfTheWeek[cn->days_L[i]]+"("+CustomFETString::number(i)+")"+"\n";
				}
				else if(xmlReader.name()==QString("Preferred_Starting_Hour")){
					QString text=xmlReader.readElementText();
					cn->hours_L.append(0);
					assert(cn->hours_L.count()-1==i);
					for(cn->hours_L[i]=0; cn->hours_L[i] < this->nHoursPerDay; cn->hours_L[i]++)
						if(this->hoursOfTheDay[cn->hours_L[i]]==text)
							break;
					
					if(cn->hours_L[i]>=this->nHoursPerDay){
						xmlReader.raiseError(tr("Hour %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr("Constraint ActivityPreferredStartingTimes hour corrupt for activity with id %1, hour %2 is inexistent ... ignoring constraint")
							.arg(cn->activityId)
							.arg(text));*/
						delete cn;
						cn=nullptr;
						//goto corruptConstraintTime;
						return nullptr;
					}
					
					assert(cn->hours_L[i]>=0 && cn->hours_L[i] < this->nHoursPerDay);
					xmlReadingLog+="    Preferred starting hour="+this->hoursOfTheDay[cn->hours_L[i]]+"\n";
				}
				else{
					unrecognizedXmlTags.append(xmlReader.name().toString());
					unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
					unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

					xmlReader.skipCurrentElement();
					xmlReaderNumberOfUnrecognizedFields++;
				}
			}

			i++;

			if(!(i==cn->days_L.count()) || !(i==cn->hours_L.count())){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Preferred_Starting_Time"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			assert(i==cn->days_L.count());
			assert(i==cn->hours_L.count());
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(!(i==cn->nPreferredStartingTimes_L)){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Preferred_Starting_Times").arg("Preferred_Starting_Time"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(i==cn->nPreferredStartingTimes_L);
	return cn;
}

TimeConstraint* Rules::readBreak(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintBreak"));

	QList<int> days;
	QList<int> hours;
	double weightPercentage=100;
	int d=-1, h1=-1, h2=-1;
	bool active=true;
	QString comments=QString("");
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			weightPercentage=customFETStrToDouble(text);
			if(weightPercentage<0){
				xmlReader.raiseError(tr("Weight percentage incorrect"));
				return nullptr;
			}
			assert(weightPercentage>=0);
			xmlReadingLog+="    Read weight percentage="+CustomFETString::number(weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			comments=text;
		}
		else if(xmlReader.name()==QString("Day")){
			QString text=xmlReader.readElementText();
			for(d=0; d<this->nDaysPerWeek; d++)
				if(this->daysOfTheWeek[d]==text)
					break;
			if(d>=this->nDaysPerWeek){
				xmlReader.raiseError(tr("Day %1 is inexistent").arg(text));
				/*RulesReconcilableMessage::information(parent, tr("FET information"),
					tr("Constraint Break day corrupt for day %1 is inexistent ... ignoring constraint")
					.arg(text));*/
				//cn=nullptr;
				//goto corruptConstraintTime;
				return nullptr;
			}
			assert(d<this->nDaysPerWeek);
			xmlReadingLog+="    Crt. day="+this->daysOfTheWeek[d]+"\n";
		}
		else if(xmlReader.name()==QString("Start_Hour")){
			QString text=xmlReader.readElementText();
			for(h1=0; h1 < this->nHoursPerDay; h1++)
				if(this->hoursOfTheDay[h1]==text)
					break;
			if(h1==this->nHoursPerDay){
				xmlReader.raiseError(tr("Hour %1 is the last hour - impossible").arg(text));
				return nullptr;
			}
			else if(h1>this->nHoursPerDay){
				xmlReader.raiseError(tr("Hour %1 is inexistent").arg(text));
				/*RulesReconcilableMessage::information(parent, tr("FET information"),
					tr("Constraint Break start hour corrupt for hour %1 is inexistent ... ignoring constraint")
					.arg(text));*/
				//cn=nullptr;
				//goto corruptConstraintTime;
				return nullptr;
			}
			assert(h1>=0 && h1 < this->nHoursPerDay);
			xmlReadingLog+="    Start hour="+this->hoursOfTheDay[h1]+"\n";
		}
		else if(xmlReader.name()==QString("End_Hour")){
			QString text=xmlReader.readElementText();
			for(h2=0; h2 < this->nHoursPerDay; h2++)
				if(this->hoursOfTheDay[h2]==text)
					break;
			if(h2==0){
				xmlReader.raiseError(tr("Hour %1 is the first hour - impossible").arg(text));
				return nullptr;
			}
			else if(h2>this->nHoursPerDay){
				xmlReader.raiseError(tr("Hour %1 is inexistent").arg(text));
				return nullptr;
			}
			/*if(h2<=0 || h2>this->nHoursPerDay){
				RulesReconcilableMessage::information(parent, tr("FET information"),
					tr("Constraint Break end hour corrupt for hour %1 is inexistent ... ignoring constraint")
					.arg(text));
				//goto corruptConstraintTime;
				return nullptr;
			}*/
			assert(h2>0 && h2 <= this->nHoursPerDay);
			xmlReadingLog+="    End hour="+this->hoursOfTheDay[h2]+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}

	assert(weightPercentage>=0);
	if(d<0){
		xmlReader.raiseError(tr("Field missing: %1").arg("Day"));
		return nullptr;
	}
	else if(h1<0){
		xmlReader.raiseError(tr("Field missing: %1").arg("Start_Hour"));
		return nullptr;
	}
	else if(h2<0){
		xmlReader.raiseError(tr("Field missing: %1").arg("End_Hour"));
		return nullptr;
	}
	assert(d>=0 && h1>=0 && h2>=0);

	ConstraintBreakTimes* cn = nullptr;
	
	bool found=false;
	for(TimeConstraint* c : qAsConst(this->timeConstraintsList))
		if(c->type==CONSTRAINT_BREAK_TIMES){
			ConstraintBreakTimes* tna=(ConstraintBreakTimes*) c;
			if(true){
				found=true;
				
				for(int hh=h1; hh<h2; hh++){
					int k;
					for(k=0; k<tna->days.count(); k++)
						if(tna->days.at(k)==d && tna->hours.at(k)==hh)
							break;
					if(k==tna->days.count()){
						tna->days.append(d);
						tna->hours.append(hh);
					}
				}
				
				assert(tna->days.count()==tna->hours.count());
			}
		}
	if(!found){
		days.clear();
		hours.clear();
		for(int hh=h1; hh<h2; hh++){
			days.append(d);
			hours.append(hh);
		}
	
		cn=new ConstraintBreakTimes(weightPercentage, days, hours);
		cn->active=active;
		cn->comments=comments;

		return cn;
	}
	else
		return nullptr;
}

TimeConstraint* Rules::readBreakTimes(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintBreakTimes"));
	ConstraintBreakTimes* cn=new ConstraintBreakTimes();
	int nNotAvailableSlots=-1;
	int i=0;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Read weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}

		else if(xmlReader.name()==QString("Number_of_Break_Times")){
			QString text=xmlReader.readElementText();
			nNotAvailableSlots=text.toInt();
			xmlReadingLog+="    Read number of break times="+CustomFETString::number(nNotAvailableSlots)+"\n";
		}

		else if(xmlReader.name()==QString("Break_Time")){
			xmlReadingLog+="    Read: not available time\n";
			
			int d=-1;
			int h=-1;

			assert(xmlReader.isStartElement());
			while(xmlReader.readNextStartElement()){
				xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
				if(xmlReader.name()==QString("Day")){
					QString text=xmlReader.readElementText();
					for(d=0; d<this->nDaysPerWeek; d++)
						if(this->daysOfTheWeek[d]==text)
							break;

					if(d>=this->nDaysPerWeek){
						xmlReader.raiseError(tr("Day %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr("Constraint BreakTimes day corrupt for day %1 is inexistent ... ignoring constraint")
							.arg(text));*/
						delete cn;
						cn=nullptr;
						//goto corruptConstraintTime;
						return nullptr;
					}
		
					assert(d<this->nDaysPerWeek);
					xmlReadingLog+="    Day="+this->daysOfTheWeek[d]+"("+CustomFETString::number(i)+")"+"\n";
				}
				else if(xmlReader.name()==QString("Hour")){
					QString text=xmlReader.readElementText();
					for(h=0; h < this->nHoursPerDay; h++)
						if(this->hoursOfTheDay[h]==text)
							break;
					
					if(h>=this->nHoursPerDay){
						xmlReader.raiseError(tr("Hour %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr("Constraint BreakTimes hour corrupt for hour %1 is inexistent ... ignoring constraint")
							.arg(text));*/
						delete cn;
						cn=nullptr;
						//goto corruptConstraintTime;
						return nullptr;
					}
					
					assert(h>=0 && h < this->nHoursPerDay);
					xmlReadingLog+="    Hour="+this->hoursOfTheDay[h]+"\n";
				}
				else{
					unrecognizedXmlTags.append(xmlReader.name().toString());
					unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
					unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

					xmlReader.skipCurrentElement();
					xmlReaderNumberOfUnrecognizedFields++;
				}
			}
			i++;
			
			cn->days.append(d);
			cn->hours.append(h);

			if(d==-1 || h==-1){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Break_Time"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(!(i==nNotAvailableSlots)){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Break_Times").arg("Break_Time"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(i==nNotAvailableSlots);
	return cn;
}

TimeConstraint* Rules::readTeachersNoGaps(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeachersNoGaps"));
	ConstraintTeachersMaxGapsPerWeek* cn=new ConstraintTeachersMaxGapsPerWeek();
	cn->maxGaps=0;
	//ConstraintTeachersNoGaps* cn=new ConstraintTeachersNoGaps();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readTeachersMaxGapsPerWeek(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeachersMaxGapsPerWeek"));
	ConstraintTeachersMaxGapsPerWeek* cn=new ConstraintTeachersMaxGapsPerWeek();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Max_Gaps")){
			QString text=xmlReader.readElementText();
			cn->maxGaps=text.toInt();
			xmlReadingLog+="    Adding max gaps="+CustomFETString::number(cn->maxGaps)+"\n";
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readTeacherMaxGapsPerWeek(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeacherMaxGapsPerWeek"));
	ConstraintTeacherMaxGapsPerWeek* cn=new ConstraintTeacherMaxGapsPerWeek();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Teacher_Name")){
			QString text=xmlReader.readElementText();
			cn->teacherName=text;
			xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Max_Gaps")){
			QString text=xmlReader.readElementText();
			cn->maxGaps=text.toInt();
			xmlReadingLog+="    Adding max gaps="+CustomFETString::number(cn->maxGaps)+"\n";
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readTeachersMaxGapsPerDay(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeachersMaxGapsPerDay"));
	ConstraintTeachersMaxGapsPerDay* cn=new ConstraintTeachersMaxGapsPerDay();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Max_Gaps")){
			QString text=xmlReader.readElementText();
			cn->maxGaps=text.toInt();
			xmlReadingLog+="    Adding max gaps="+CustomFETString::number(cn->maxGaps)+"\n";
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readTeacherMaxGapsPerDay(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeacherMaxGapsPerDay"));
	ConstraintTeacherMaxGapsPerDay* cn=new ConstraintTeacherMaxGapsPerDay();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Teacher_Name")){
			QString text=xmlReader.readElementText();
			cn->teacherName=text;
			xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Max_Gaps")){
			QString text=xmlReader.readElementText();
			cn->maxGaps=text.toInt();
			xmlReadingLog+="    Adding max gaps="+CustomFETString::number(cn->maxGaps)+"\n";
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readStudentsNoGaps(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsNoGaps"));

	ConstraintStudentsMaxGapsPerWeek* cn=new ConstraintStudentsMaxGapsPerWeek();
	
	cn->maxGaps=0;
	
	//bool compulsory_read=false;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
			//compulsory_read=true;
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readStudentsSetNoGaps(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsSetNoGaps"));

	ConstraintStudentsSetMaxGapsPerWeek* cn=new ConstraintStudentsSetMaxGapsPerWeek();
	
	cn->maxGaps=0;
	
	//bool compulsory_read=false;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
			//compulsory_read=true;
		}
		else if(xmlReader.name()==QString("Students")){
			QString text=xmlReader.readElementText();
			cn->students=text;
			xmlReadingLog+="    Read students name="+cn->students+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readStudentsMaxGapsPerWeek(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsMaxGapsPerWeek"));
	ConstraintStudentsMaxGapsPerWeek* cn=new ConstraintStudentsMaxGapsPerWeek();

	//bool compulsory_read=false;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Max_Gaps")){
			QString text=xmlReader.readElementText();
			cn->maxGaps=text.toInt();
			xmlReadingLog+="    Adding max gaps="+CustomFETString::number(cn->maxGaps)+"\n";
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
			//compulsory_read=true;
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readStudentsSetMaxGapsPerWeek(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsSetMaxGapsPerWeek"));
	ConstraintStudentsSetMaxGapsPerWeek* cn=new ConstraintStudentsSetMaxGapsPerWeek();
	
	//bool compulsory_read=false;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Max_Gaps")){
			QString text=xmlReader.readElementText();
			cn->maxGaps=text.toInt();
			xmlReadingLog+="    Adding max gaps="+CustomFETString::number(cn->maxGaps)+"\n";
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
			//compulsory_read=true;
		}
		else if(xmlReader.name()==QString("Students")){
			QString text=xmlReader.readElementText();
			cn->students=text;
			xmlReadingLog+="    Read students name="+cn->students+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readStudentsMaxGapsPerDay(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsMaxGapsPerDay"));
	ConstraintStudentsMaxGapsPerDay* cn=new ConstraintStudentsMaxGapsPerDay();

	//bool compulsory_read=false;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Max_Gaps")){
			QString text=xmlReader.readElementText();
			cn->maxGaps=text.toInt();
			xmlReadingLog+="    Adding max gaps="+CustomFETString::number(cn->maxGaps)+"\n";
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
			//compulsory_read=true;
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readStudentsSetMaxGapsPerDay(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsSetMaxGapsPerDay"));
	ConstraintStudentsSetMaxGapsPerDay* cn=new ConstraintStudentsSetMaxGapsPerDay();
	
	//bool compulsory_read=false;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Max_Gaps")){
			QString text=xmlReader.readElementText();
			cn->maxGaps=text.toInt();
			xmlReadingLog+="    Adding max gaps="+CustomFETString::number(cn->maxGaps)+"\n";
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
			//compulsory_read=true;
		}
		else if(xmlReader.name()==QString("Students")){
			QString text=xmlReader.readElementText();
			cn->students=text;
			xmlReadingLog+="    Read students name="+cn->students+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readStudentsEarly(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsEarly"));
	ConstraintStudentsEarlyMaxBeginningsAtSecondHour* cn=new ConstraintStudentsEarlyMaxBeginningsAtSecondHour();
	
	cn->maxBeginningsAtSecondHour=0;
	
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readStudentsEarlyMaxBeginningsAtSecondHour(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsEarlyMaxBeginningsAtSecondHour"));
	ConstraintStudentsEarlyMaxBeginningsAtSecondHour* cn=new ConstraintStudentsEarlyMaxBeginningsAtSecondHour();
	cn->maxBeginningsAtSecondHour=-1;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Max_Beginnings_At_Second_Hour")){
			QString text=xmlReader.readElementText();
			cn->maxBeginningsAtSecondHour=text.toInt();
			if(!(cn->maxBeginningsAtSecondHour>=0)){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Max_Beginnings_At_Second_Hour"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			xmlReadingLog+="    Adding max beginnings at second hour="+CustomFETString::number(cn->maxBeginningsAtSecondHour)+"\n";
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(!(cn->maxBeginningsAtSecondHour>=0)){
		xmlReader.raiseError(tr("%1 not found").arg("Max_Beginnings_At_Second_Hour"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(cn->maxBeginningsAtSecondHour>=0);
	return cn;
}

TimeConstraint* Rules::readStudentsSetEarly(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsSetEarly"));
	ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour* cn=new ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour();
	
	cn->maxBeginningsAtSecondHour=0;
	
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Students")){
			QString text=xmlReader.readElementText();
			cn->students=text;
			xmlReadingLog+="    Read students name="+cn->students+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readStudentsSetEarlyMaxBeginningsAtSecondHour(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour"));
	ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour* cn=new ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour();
	cn->maxBeginningsAtSecondHour=-1;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Max_Beginnings_At_Second_Hour")){
			QString text=xmlReader.readElementText();
			cn->maxBeginningsAtSecondHour=text.toInt();
			if(!(cn->maxBeginningsAtSecondHour>=0)){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Max_Beginnings_At_Second_Hour"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			xmlReadingLog+="    Adding max beginnings at second hour="+CustomFETString::number(cn->maxBeginningsAtSecondHour)+"\n";
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Students")){
			QString text=xmlReader.readElementText();
			cn->students=text;
			xmlReadingLog+="    Read students name="+cn->students+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(!(cn->maxBeginningsAtSecondHour>=0)){
		xmlReader.raiseError(tr("%1 not found").arg("Max_Beginnings_At_Second_Hour"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(cn->maxBeginningsAtSecondHour>=0);
	return cn;
}

TimeConstraint* Rules::readActivitiesPreferredTimes(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintActivitiesPreferredTimes"));

	ConstraintActivitiesPreferredStartingTimes* cn=new ConstraintActivitiesPreferredStartingTimes();
	cn->duration=-1;
	cn->nPreferredStartingTimes_L=0;
	int i;
	/*for(i=0; i<MAX_N_CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES; i++){
		cn->days[i] = cn->hours[i] = -1;
	}*/
	cn->teacherName="";
	cn->studentsName="";
	cn->subjectName="";
	cn->activityTagName="";
	
	i=0;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Teacher_Name")){
			QString text=xmlReader.readElementText();
			cn->teacherName=text;
			xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
		}
		else if(xmlReader.name()==QString("Students_Name")){
			QString text=xmlReader.readElementText();
			cn->studentsName=text;
			xmlReadingLog+="    Read students name="+cn->studentsName+"\n";
		}
		else if(xmlReader.name()==QString("Subject_Name")){
			QString text=xmlReader.readElementText();
			cn->subjectName=text;
			xmlReadingLog+="    Read subject name="+cn->subjectName+"\n";
		}
		else if(xmlReader.name()==QString("Subject_Tag_Name")){
			QString text=xmlReader.readElementText();
			cn->activityTagName=text;
			xmlReadingLog+="    Read activity tag name="+cn->activityTagName+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Tag_Name")){
			QString text=xmlReader.readElementText();
			cn->activityTagName=text;
			xmlReadingLog+="    Read activity tag name="+cn->activityTagName+"\n";
		}
		else if(xmlReader.name()==QString("Number_of_Preferred_Times")){
			QString text=xmlReader.readElementText();
			cn->nPreferredStartingTimes_L=text.toInt();
			xmlReadingLog+="    Read number of preferred times="+CustomFETString::number(cn->nPreferredStartingTimes_L)+"\n";
		}
		else if(xmlReader.name()==QString("Preferred_Time")){
			xmlReadingLog+="    Read: preferred time\n";

			assert(xmlReader.isStartElement());
			while(xmlReader.readNextStartElement()){
				xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
				if(xmlReader.name()==QString("Preferred_Day")){
					QString text=xmlReader.readElementText();
					cn->days_L.append(0);
					assert(cn->days_L.count()-1==i);
					for(cn->days_L[i]=0; cn->days_L[i]<this->nDaysPerWeek; cn->days_L[i]++)
						if(this->daysOfTheWeek[cn->days_L[i]]==text)
							break;
							
					if(cn->days_L[i]>=this->nDaysPerWeek){
						xmlReader.raiseError(tr("Day %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr("Constraint ActivitiesPreferredTimes day corrupt for teacher name=%1, students names=%2, subject name=%3, activity tag name=%4, day %5 is inexistent ... ignoring constraint")
							.arg(cn->teacherName)
							.arg(cn->studentsName)
							.arg(cn->subjectName)
							.arg(cn->activityTagName)
							.arg(text));*/
						delete cn;
						cn=nullptr;
						//goto corruptConstraintTime;
						return nullptr;
					}
							
					assert(cn->days_L[i]<this->nDaysPerWeek);
					xmlReadingLog+="    Preferred day="+this->daysOfTheWeek[cn->days_L[i]]+"("+CustomFETString::number(i)+")"+"\n";
				}
				else if(xmlReader.name()==QString("Preferred_Hour")){
					QString text=xmlReader.readElementText();
					cn->hours_L.append(0);
					assert(cn->hours_L.count()-1==i);
					for(cn->hours_L[i]=0; cn->hours_L[i] < this->nHoursPerDay; cn->hours_L[i]++)
						if(this->hoursOfTheDay[cn->hours_L[i]]==text)
							break;
							
					if(cn->hours_L[i]>=this->nHoursPerDay){
						xmlReader.raiseError(tr("Hour %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr("Constraint ActivitiesPreferredTimes hour corrupt for teacher name=%1, students names=%2, subject name=%3, activity tag name=%4, hour %5 is inexistent ... ignoring constraint")
							.arg(cn->teacherName)
							.arg(cn->studentsName)
							.arg(cn->subjectName)
							.arg(cn->activityTagName)
							.arg(text));*/
						delete cn;
						cn=nullptr;
						//goto corruptConstraintTime;
						return nullptr;
					}
							
					assert(cn->hours_L[i]>=0 && cn->hours_L[i] < this->nHoursPerDay);
					xmlReadingLog+="    Preferred hour="+this->hoursOfTheDay[cn->hours_L[i]]+"\n";
				}
				else{
					unrecognizedXmlTags.append(xmlReader.name().toString());
					unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
					unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

					xmlReader.skipCurrentElement();
					xmlReaderNumberOfUnrecognizedFields++;
				}
			}

			i++;

			if(!(i==cn->days_L.count()) || !(i==cn->hours_L.count())){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Preferred_Time"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			assert(i==cn->days_L.count());
			assert(i==cn->hours_L.count());
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(!(i==cn->nPreferredStartingTimes_L)){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Preferred_Times").arg("Preferred_Time"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(i==cn->nPreferredStartingTimes_L);
	return cn;
}

TimeConstraint* Rules::readActivitiesPreferredTimeSlots(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintActivitiesPreferredTimeSlots"));
	ConstraintActivitiesPreferredTimeSlots* cn=new ConstraintActivitiesPreferredTimeSlots();
	cn->duration=-1;
	cn->p_nPreferredTimeSlots_L=0;
	int i;
	/*for(i=0; i<MAX_N_CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS; i++){
		cn->p_days[i] = cn->p_hours[i] = -1;
	}*/
	cn->p_teacherName="";
	cn->p_studentsName="";
	cn->p_subjectName="";
	cn->p_activityTagName="";
	
	i=0;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Teacher_Name")){
			QString text=xmlReader.readElementText();
			cn->p_teacherName=text;
			xmlReadingLog+="    Read teacher name="+cn->p_teacherName+"\n";
		}
		else if(xmlReader.name()==QString("Students_Name")){
			QString text=xmlReader.readElementText();
			cn->p_studentsName=text;
			xmlReadingLog+="    Read students name="+cn->p_studentsName+"\n";
		}
		else if(xmlReader.name()==QString("Subject_Name")){
			QString text=xmlReader.readElementText();
			cn->p_subjectName=text;
			xmlReadingLog+="    Read subject name="+cn->p_subjectName+"\n";
		}
		else if(xmlReader.name()==QString("Subject_Tag_Name")){
			QString text=xmlReader.readElementText();
			cn->p_activityTagName=text;
			xmlReadingLog+="    Read activity tag name="+cn->p_activityTagName+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Tag_Name")){
			QString text=xmlReader.readElementText();
			cn->p_activityTagName=text;
			xmlReadingLog+="    Read activity tag name="+cn->p_activityTagName+"\n";
		}
		else if(xmlReader.name()==QString("Duration")){
			QString text=xmlReader.readElementText();
			if(!text.isEmpty()){
				cn->duration=text.toInt();
				xmlReadingLog+="    Read duration="+CustomFETString::number(cn->duration)+"\n";
			}
			else{
				cn->duration=-1;
			}
		}
		else if(xmlReader.name()==QString("Number_of_Preferred_Time_Slots")){
			QString text=xmlReader.readElementText();
			cn->p_nPreferredTimeSlots_L=text.toInt();
			xmlReadingLog+="    Read number of preferred times="+CustomFETString::number(cn->p_nPreferredTimeSlots_L)+"\n";
		}
		else if(xmlReader.name()==QString("Preferred_Time_Slot")){
			xmlReadingLog+="    Read: preferred time slot\n";

			assert(xmlReader.isStartElement());
			while(xmlReader.readNextStartElement()){
				xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
				if(xmlReader.name()==QString("Preferred_Day")){
					QString text=xmlReader.readElementText();
					cn->p_days_L.append(0);
					assert(cn->p_days_L.count()-1==i);
					for(cn->p_days_L[i]=0; cn->p_days_L[i]<this->nDaysPerWeek; cn->p_days_L[i]++)
						if(this->daysOfTheWeek[cn->p_days_L[i]]==text)
							break;
							
					if(cn->p_days_L[i]>=this->nDaysPerWeek){
						xmlReader.raiseError(tr("Day %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr("Constraint ActivitiesPreferredTimeSlots day corrupt for teacher name=%1, students names=%2, subject name=%3, activity tag name=%4, day %5 is inexistent ... ignoring constraint")
							.arg(cn->p_teacherName)
							.arg(cn->p_studentsName)
							.arg(cn->p_subjectName)
							.arg(cn->p_activityTagName)
							.arg(text));*/
						delete cn;
						cn=nullptr;
						//goto corruptConstraintTime;
						return nullptr;
					}
							
					assert(cn->p_days_L[i]<this->nDaysPerWeek);
					xmlReadingLog+="    Preferred day="+this->daysOfTheWeek[cn->p_days_L[i]]+"("+CustomFETString::number(i)+")"+"\n";
				}
				else if(xmlReader.name()==QString("Preferred_Hour")){
					QString text=xmlReader.readElementText();
					cn->p_hours_L.append(0);
					assert(cn->p_hours_L.count()-1==i);
					for(cn->p_hours_L[i]=0; cn->p_hours_L[i] < this->nHoursPerDay; cn->p_hours_L[i]++)
						if(this->hoursOfTheDay[cn->p_hours_L[i]]==text)
							break;
							
					if(cn->p_hours_L[i]>=this->nHoursPerDay){
						xmlReader.raiseError(tr("Hour %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr("Constraint ActivitiesPreferredTimeSlots hour corrupt for teacher name=%1, students names=%2, subject name=%3, activity tag name=%4, hour %5 is inexistent ... ignoring constraint")
							.arg(cn->p_teacherName)
							.arg(cn->p_studentsName)
							.arg(cn->p_subjectName)
							.arg(cn->p_activityTagName)
							.arg(text));*/
						delete cn;
						cn=nullptr;
						//goto corruptConstraintTime;
						return nullptr;
					}
							
					assert(cn->p_hours_L[i]>=0 && cn->p_hours_L[i] < this->nHoursPerDay);
					xmlReadingLog+="    Preferred hour="+this->hoursOfTheDay[cn->p_hours_L[i]]+"\n";
				}
				else{
					unrecognizedXmlTags.append(xmlReader.name().toString());
					unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
					unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

					xmlReader.skipCurrentElement();
					xmlReaderNumberOfUnrecognizedFields++;
				}
			}

			i++;

			if(!(i==cn->p_days_L.count()) || !(i==cn->p_hours_L.count())){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Preferred_Time_Slot"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			assert(i==cn->p_days_L.count());
			assert(i==cn->p_hours_L.count());
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(!(i==cn->p_nPreferredTimeSlots_L)){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Preferred_Time_Slots").arg("Preferred_Time_Slot"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(i==cn->p_nPreferredTimeSlots_L);
	return cn;
}

TimeConstraint* Rules::readActivitiesPreferredStartingTimes(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintActivitiesPreferredStartingTimes"));
	ConstraintActivitiesPreferredStartingTimes* cn=new ConstraintActivitiesPreferredStartingTimes();
	cn->duration=-1;
	cn->nPreferredStartingTimes_L=0;
	int i;
	/*for(i=0; i<MAX_N_CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES; i++){
		cn->days[i] = cn->hours[i] = -1;
	}*/
	cn->teacherName="";
	cn->studentsName="";
	cn->subjectName="";
	cn->activityTagName="";
	
	i=0;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Teacher_Name")){
			QString text=xmlReader.readElementText();
			cn->teacherName=text;
			xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
		}
		else if(xmlReader.name()==QString("Students_Name")){
			QString text=xmlReader.readElementText();
			cn->studentsName=text;
			xmlReadingLog+="    Read students name="+cn->studentsName+"\n";
		}
		else if(xmlReader.name()==QString("Subject_Name")){
			QString text=xmlReader.readElementText();
			cn->subjectName=text;
			xmlReadingLog+="    Read subject name="+cn->subjectName+"\n";
		}
		else if(xmlReader.name()==QString("Subject_Tag_Name")){
			QString text=xmlReader.readElementText();
			cn->activityTagName=text;
			xmlReadingLog+="    Read activity tag name="+cn->activityTagName+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Tag_Name")){
			QString text=xmlReader.readElementText();
			cn->activityTagName=text;
			xmlReadingLog+="    Read activity tag name="+cn->activityTagName+"\n";
		}
		else if(xmlReader.name()==QString("Duration")){
			QString text=xmlReader.readElementText();
			if(!text.isEmpty()){
				cn->duration=text.toInt();
				xmlReadingLog+="    Read duration="+CustomFETString::number(cn->duration)+"\n";
			}
			else{
				cn->duration=-1;
			}
		}
		else if(xmlReader.name()==QString("Number_of_Preferred_Starting_Times")){
			QString text=xmlReader.readElementText();
			cn->nPreferredStartingTimes_L=text.toInt();
			xmlReadingLog+="    Read number of preferred starting times="+CustomFETString::number(cn->nPreferredStartingTimes_L)+"\n";
		}
		else if(xmlReader.name()==QString("Preferred_Starting_Time")){
			xmlReadingLog+="    Read: preferred starting time\n";

			assert(xmlReader.isStartElement());
			while(xmlReader.readNextStartElement()){
				xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
				if(xmlReader.name()==QString("Preferred_Starting_Day")){
					QString text=xmlReader.readElementText();
					cn->days_L.append(0);
					assert(cn->days_L.count()-1==i);
					for(cn->days_L[i]=0; cn->days_L[i]<this->nDaysPerWeek; cn->days_L[i]++)
						if(this->daysOfTheWeek[cn->days_L[i]]==text)
							break;
							
					if(cn->days_L[i]>=this->nDaysPerWeek){
						xmlReader.raiseError(tr("Day %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr("Constraint ActivitiesPreferredStartingTimes day corrupt for teacher name=%1, students names=%2, subject name=%3, activity tag name=%4, day %5 is inexistent ... ignoring constraint")
							.arg(cn->teacherName)
							.arg(cn->studentsName)
							.arg(cn->subjectName)
							.arg(cn->activityTagName)
							.arg(text));*/
						delete cn;
						cn=nullptr;
						//goto corruptConstraintTime;
						return nullptr;
					}
							
					assert(cn->days_L[i]<this->nDaysPerWeek);
					xmlReadingLog+="    Preferred starting day="+this->daysOfTheWeek[cn->days_L[i]]+"("+CustomFETString::number(i)+")"+"\n";
				}
				else if(xmlReader.name()==QString("Preferred_Starting_Hour")){
					QString text=xmlReader.readElementText();
					cn->hours_L.append(0);
					assert(cn->hours_L.count()-1==i);
					for(cn->hours_L[i]=0; cn->hours_L[i] < this->nHoursPerDay; cn->hours_L[i]++)
						if(this->hoursOfTheDay[cn->hours_L[i]]==text)
							break;
							
					if(cn->hours_L[i]>=this->nHoursPerDay){
						xmlReader.raiseError(tr("Hour %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr("Constraint ActivitiesPreferredStartingTimes hour corrupt for teacher name=%1, students names=%2, subject name=%3, activity tag name=%4, hour %5 is inexistent ... ignoring constraint")
							.arg(cn->teacherName)
							.arg(cn->studentsName)
							.arg(cn->subjectName)
							.arg(cn->activityTagName)
							.arg(text));*/
						delete cn;
						cn=nullptr;
						//goto corruptConstraintTime;
						return nullptr;
					}
							
					assert(cn->hours_L[i]>=0 && cn->hours_L[i] < this->nHoursPerDay);
					xmlReadingLog+="    Preferred starting hour="+this->hoursOfTheDay[cn->hours_L[i]]+"\n";
				}
				else{
					unrecognizedXmlTags.append(xmlReader.name().toString());
					unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
					unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

					xmlReader.skipCurrentElement();
					xmlReaderNumberOfUnrecognizedFields++;
				}
			}

			i++;

			if(!(i==cn->days_L.count()) || !(i==cn->hours_L.count())){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Preferred_Starting_Time"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			assert(i==cn->days_L.count());
			assert(i==cn->hours_L.count());
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(!(i==cn->nPreferredStartingTimes_L)){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Preferred_Starting_Times").arg("Preferred_Starting_Time"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(i==cn->nPreferredStartingTimes_L);
	return cn;
}

////////////////
TimeConstraint* Rules::readSubactivitiesPreferredTimeSlots(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintSubactivitiesPreferredTimeSlots"));
	ConstraintSubactivitiesPreferredTimeSlots* cn=new ConstraintSubactivitiesPreferredTimeSlots();
	cn->duration=-1;
	cn->p_nPreferredTimeSlots_L=0;
	cn->componentNumber=0;
	int i;
	/*for(i=0; i<MAX_N_CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS; i++){
		cn->p_days[i] = cn->p_hours[i] = -1;
	}*/
	cn->p_teacherName="";
	cn->p_studentsName="";
	cn->p_subjectName="";
	cn->p_activityTagName="";
	
	i=0;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Component_Number")){
			QString text=xmlReader.readElementText();
			cn->componentNumber=text.toInt();
			xmlReadingLog+="    Adding component number="+CustomFETString::number(cn->componentNumber)+"\n";
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Teacher_Name")){
			QString text=xmlReader.readElementText();
			cn->p_teacherName=text;
			xmlReadingLog+="    Read teacher name="+cn->p_teacherName+"\n";
		}
		else if(xmlReader.name()==QString("Students_Name")){
			QString text=xmlReader.readElementText();
			cn->p_studentsName=text;
			xmlReadingLog+="    Read students name="+cn->p_studentsName+"\n";
		}
		else if(xmlReader.name()==QString("Subject_Name")){
			QString text=xmlReader.readElementText();
			cn->p_subjectName=text;
			xmlReadingLog+="    Read subject name="+cn->p_subjectName+"\n";
		}
		else if(xmlReader.name()==QString("Subject_Tag_Name")){
			QString text=xmlReader.readElementText();
			cn->p_activityTagName=text;
			xmlReadingLog+="    Read activity tag name="+cn->p_activityTagName+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Tag_Name")){
			QString text=xmlReader.readElementText();
			cn->p_activityTagName=text;
			xmlReadingLog+="    Read activity tag name="+cn->p_activityTagName+"\n";
		}
		else if(xmlReader.name()==QString("Duration")){
			QString text=xmlReader.readElementText();
			if(!text.isEmpty()){
				cn->duration=text.toInt();
				xmlReadingLog+="    Read duration="+CustomFETString::number(cn->duration)+"\n";
			}
			else{
				cn->duration=-1;
			}
		}
		else if(xmlReader.name()==QString("Number_of_Preferred_Time_Slots")){
			QString text=xmlReader.readElementText();
			cn->p_nPreferredTimeSlots_L=text.toInt();
			xmlReadingLog+="    Read number of preferred times="+CustomFETString::number(cn->p_nPreferredTimeSlots_L)+"\n";
		}
		else if(xmlReader.name()==QString("Preferred_Time_Slot")){
			xmlReadingLog+="    Read: preferred time slot\n";

			assert(xmlReader.isStartElement());
			while(xmlReader.readNextStartElement()){
				xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
				if(xmlReader.name()==QString("Preferred_Day")){
					QString text=xmlReader.readElementText();
					cn->p_days_L.append(0);
					assert(cn->p_days_L.count()-1==i);
					for(cn->p_days_L[i]=0; cn->p_days_L[i]<this->nDaysPerWeek; cn->p_days_L[i]++)
						if(this->daysOfTheWeek[cn->p_days_L[i]]==text)
							break;
							
					if(cn->p_days_L[i]>=this->nDaysPerWeek){
						xmlReader.raiseError(tr("Day %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr("Constraint ActivitiesPreferredTimeSlots day corrupt for teacher name=%1, students names=%2, subject name=%3, activity tag name=%4, day %5 is inexistent ... ignoring constraint")
							.arg(cn->p_teacherName)
							.arg(cn->p_studentsName)
							.arg(cn->p_subjectName)
							.arg(cn->p_activityTagName)
							.arg(text));*/
						delete cn;
						cn=nullptr;
						//goto corruptConstraintTime;
						return nullptr;
					}
					
					assert(cn->p_days_L[i]<this->nDaysPerWeek);
					xmlReadingLog+="    Preferred day="+this->daysOfTheWeek[cn->p_days_L[i]]+"("+CustomFETString::number(i)+")"+"\n";
				}
				else if(xmlReader.name()==QString("Preferred_Hour")){
					QString text=xmlReader.readElementText();
					cn->p_hours_L.append(0);
					assert(cn->p_hours_L.count()-1==i);
					for(cn->p_hours_L[i]=0; cn->p_hours_L[i] < this->nHoursPerDay; cn->p_hours_L[i]++)
						if(this->hoursOfTheDay[cn->p_hours_L[i]]==text)
							break;
							
					if(cn->p_hours_L[i]>=this->nHoursPerDay){
						xmlReader.raiseError(tr("Hour %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr("Constraint ActivitiesPreferredTimeSlots hour corrupt for teacher name=%1, students names=%2, subject name=%3, activity tag name=%4, hour %5 is inexistent ... ignoring constraint")
							.arg(cn->p_teacherName)
							.arg(cn->p_studentsName)
							.arg(cn->p_subjectName)
							.arg(cn->p_activityTagName)
							.arg(text));*/
						delete cn;
						cn=nullptr;
						//goto corruptConstraintTime;
						return nullptr;
					}
					
					assert(cn->p_hours_L[i]>=0 && cn->p_hours_L[i] < this->nHoursPerDay);
					xmlReadingLog+="    Preferred hour="+this->hoursOfTheDay[cn->p_hours_L[i]]+"\n";
				}
				else{
					unrecognizedXmlTags.append(xmlReader.name().toString());
					unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
					unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

					xmlReader.skipCurrentElement();
					xmlReaderNumberOfUnrecognizedFields++;
				}
			}

			i++;

			if(!(i==cn->p_days_L.count()) || !(i==cn->p_hours_L.count())){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Preferred_Time_Slot"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			assert(i==cn->p_days_L.count());
			assert(i==cn->p_hours_L.count());
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(!(i==cn->p_nPreferredTimeSlots_L)){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Preferred_Time_Slots").arg("Preferred_Time_Slot"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(i==cn->p_nPreferredTimeSlots_L);
	return cn;
}

TimeConstraint* Rules::readSubactivitiesPreferredStartingTimes(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintSubactivitiesPreferredStartingTimes"));
	ConstraintSubactivitiesPreferredStartingTimes* cn=new ConstraintSubactivitiesPreferredStartingTimes();
	cn->duration=-1;
	cn->nPreferredStartingTimes_L=0;
	cn->componentNumber=0;
	int i;
	/*for(i=0; i<MAX_N_CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES; i++){
		cn->days[i] = cn->hours[i] = -1;
	}*/
	cn->teacherName="";
	cn->studentsName="";
	cn->subjectName="";
	cn->activityTagName="";
	
	i=0;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Component_Number")){
			QString text=xmlReader.readElementText();
			cn->componentNumber=text.toInt();
			xmlReadingLog+="    Adding component number="+CustomFETString::number(cn->componentNumber)+"\n";
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Teacher_Name")){
			QString text=xmlReader.readElementText();
			cn->teacherName=text;
			xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
		}
		else if(xmlReader.name()==QString("Students_Name")){
			QString text=xmlReader.readElementText();
			cn->studentsName=text;
			xmlReadingLog+="    Read students name="+cn->studentsName+"\n";
		}
		else if(xmlReader.name()==QString("Subject_Name")){
			QString text=xmlReader.readElementText();
			cn->subjectName=text;
			xmlReadingLog+="    Read subject name="+cn->subjectName+"\n";
		}
		else if(xmlReader.name()==QString("Subject_Tag_Name")){
			QString text=xmlReader.readElementText();
			cn->activityTagName=text;
			xmlReadingLog+="    Read activity tag name="+cn->activityTagName+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Tag_Name")){
			QString text=xmlReader.readElementText();
			cn->activityTagName=text;
			xmlReadingLog+="    Read activity tag name="+cn->activityTagName+"\n";
		}
		else if(xmlReader.name()==QString("Duration")){
			QString text=xmlReader.readElementText();
			if(!text.isEmpty()){
				cn->duration=text.toInt();
				xmlReadingLog+="    Read duration="+CustomFETString::number(cn->duration)+"\n";
			}
			else{
				cn->duration=-1;
			}
		}
		else if(xmlReader.name()==QString("Number_of_Preferred_Starting_Times")){
			QString text=xmlReader.readElementText();
			cn->nPreferredStartingTimes_L=text.toInt();
			xmlReadingLog+="    Read number of preferred starting times="+CustomFETString::number(cn->nPreferredStartingTimes_L)+"\n";
		}
		else if(xmlReader.name()==QString("Preferred_Starting_Time")){
			xmlReadingLog+="    Read: preferred starting time\n";

			assert(xmlReader.isStartElement());
			while(xmlReader.readNextStartElement()){
				xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
				if(xmlReader.name()==QString("Preferred_Starting_Day")){
					QString text=xmlReader.readElementText();
					cn->days_L.append(0);
					assert(cn->days_L.count()-1==i);
					for(cn->days_L[i]=0; cn->days_L[i]<this->nDaysPerWeek; cn->days_L[i]++)
						if(this->daysOfTheWeek[cn->days_L[i]]==text)
							break;
							
					if(cn->days_L[i]>=this->nDaysPerWeek){
						xmlReader.raiseError(tr("Day %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr("Constraint ActivitiesPreferredStartingTimes day corrupt for teacher name=%1, students names=%2, subject name=%3, activity tag name=%4, day %5 is inexistent ... ignoring constraint")
							.arg(cn->teacherName)
							.arg(cn->studentsName)
							.arg(cn->subjectName)
							.arg(cn->activityTagName)
							.arg(text));*/
						delete cn;
						cn=nullptr;
						//goto corruptConstraintTime;
						return nullptr;
					}
					
					assert(cn->days_L[i]<this->nDaysPerWeek);
					xmlReadingLog+="    Preferred starting day="+this->daysOfTheWeek[cn->days_L[i]]+"("+CustomFETString::number(i)+")"+"\n";
				}
				else if(xmlReader.name()==QString("Preferred_Starting_Hour")){
					QString text=xmlReader.readElementText();
					cn->hours_L.append(0);
					assert(cn->hours_L.count()-1==i);
					for(cn->hours_L[i]=0; cn->hours_L[i] < this->nHoursPerDay; cn->hours_L[i]++)
						if(this->hoursOfTheDay[cn->hours_L[i]]==text)
							break;
							
					if(cn->hours_L[i]>=this->nHoursPerDay){
						xmlReader.raiseError(tr("Hour %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr("Constraint ActivitiesPreferredStartingTimes hour corrupt for teacher name=%1, students names=%2, subject name=%3, activity tag name=%4, hour %5 is inexistent ... ignoring constraint")
							.arg(cn->teacherName)
							.arg(cn->studentsName)
							.arg(cn->subjectName)
							.arg(cn->activityTagName)
							.arg(text));*/
						delete cn;
						cn=nullptr;
						//goto corruptConstraintTime;
						return nullptr;
					}
					
					assert(cn->hours_L[i]>=0 && cn->hours_L[i] < this->nHoursPerDay);
					xmlReadingLog+="    Preferred starting hour="+this->hoursOfTheDay[cn->hours_L[i]]+"\n";
				}
				else{
					unrecognizedXmlTags.append(xmlReader.name().toString());
					unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
					unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

					xmlReader.skipCurrentElement();
					xmlReaderNumberOfUnrecognizedFields++;
				}
			}

			i++;

			if(!(i==cn->days_L.count()) || !(i==cn->hours_L.count())){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Preferred_Starting_Time"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			assert(i==cn->days_L.count());
			assert(i==cn->hours_L.count());
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(!(i==cn->nPreferredStartingTimes_L)){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Preferred_Starting_Times").arg("Preferred_Starting_Time"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(i==cn->nPreferredStartingTimes_L);
	return cn;
}

//2011-09-25
TimeConstraint* Rules::readActivitiesOccupyMaxTimeSlotsFromSelection(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintActivitiesOccupyMaxTimeSlotsFromSelection"));
	ConstraintActivitiesOccupyMaxTimeSlotsFromSelection* cn=new ConstraintActivitiesOccupyMaxTimeSlotsFromSelection();
	
	int ac=0;
	int tsc=0;
	int i=0;
	
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";

		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Number_of_Activities")){
			QString text=xmlReader.readElementText();
			ac=text.toInt();
			xmlReadingLog+="    Read number of activities="+CustomFETString::number(ac)+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->activitiesIds.append(text.toInt());
			xmlReadingLog+="    Read activity id="+CustomFETString::number(cn->activitiesIds[cn->activitiesIds.count()-1])+"\n";
		}
		else if(xmlReader.name()==QString("Number_of_Selected_Time_Slots")){
			QString text=xmlReader.readElementText();
			tsc=text.toInt();
			xmlReadingLog+="    Read number of selected time slots="+CustomFETString::number(tsc)+"\n";
		}
		else if(xmlReader.name()==QString("Selected_Time_Slot")){
			xmlReadingLog+="    Read: selected time slot\n";

			assert(xmlReader.isStartElement());
			while(xmlReader.readNextStartElement()){
				xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
				if(xmlReader.name()==QString("Selected_Day")){
					QString text=xmlReader.readElementText();
					cn->selectedDays.append(0);
					assert(cn->selectedDays.count()-1==i);
					for(cn->selectedDays[i]=0; cn->selectedDays[i]<this->nDaysPerWeek; cn->selectedDays[i]++)
						if(this->daysOfTheWeek[cn->selectedDays[i]]==text)
							break;
							
					if(cn->selectedDays[i]>=this->nDaysPerWeek){
						xmlReader.raiseError(tr("Day %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr("Constraint ActivitiesOccupyMaxTimeSlotsFromSelection day corrupt, day %1 is inexistent ... ignoring constraint")
							.arg(text));*/
						delete cn;
						cn=nullptr;
						//goto corruptConstraintTime;
						return nullptr;
					}
					
					assert(cn->selectedDays[i]<this->nDaysPerWeek);
					xmlReadingLog+="    Selected day="+this->daysOfTheWeek[cn->selectedDays[i]]+"("+CustomFETString::number(i)+")"+"\n";
				}
				else if(xmlReader.name()==QString("Selected_Hour")){
					QString text=xmlReader.readElementText();
					cn->selectedHours.append(0);
					assert(cn->selectedHours.count()-1==i);
					for(cn->selectedHours[i]=0; cn->selectedHours[i] < this->nHoursPerDay; cn->selectedHours[i]++)
						if(this->hoursOfTheDay[cn->selectedHours[i]]==text)
							break;
							
					if(cn->selectedHours[i]>=this->nHoursPerDay){
						xmlReader.raiseError(tr("Hour %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr(" Constraint ActivitiesOccupyMaxTimeSlotsFromSelection hour corrupt, hour %1 is inexistent ... ignoring constraint")
							.arg(text));*/
						delete cn;
						cn=nullptr;
						//goto corruptConstraintTime;
						return nullptr;
					}
					
					assert(cn->selectedHours[i]>=0 && cn->selectedHours[i] < this->nHoursPerDay);
					xmlReadingLog+="    Selected hour="+this->hoursOfTheDay[cn->selectedHours[i]]+"\n";
				}
				else{
					unrecognizedXmlTags.append(xmlReader.name().toString());
					unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
					unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

					xmlReader.skipCurrentElement();
					xmlReaderNumberOfUnrecognizedFields++;
				}
			}

			i++;
			
			if(!(i==cn->selectedDays.count()) || !(i==cn->selectedHours.count())){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Selected_Time_Slot"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			assert(i==cn->selectedDays.count());
			assert(i==cn->selectedHours.count());
		}
		else if(xmlReader.name()==QString("Max_Number_of_Occupied_Time_Slots")){
			QString text=xmlReader.readElementText();
			cn->maxOccupiedTimeSlots=text.toInt();
			xmlReadingLog+="    Read max number of occupied time slots="+CustomFETString::number(cn->maxOccupiedTimeSlots)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	
	if(!(ac==cn->activitiesIds.count())){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Activities").arg("Activity_Id"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}

	if(!(i==tsc)){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Selected_Time_Slots").arg("Selected_Time_Slot"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}

	assert(ac==cn->activitiesIds.count());
	assert(i==tsc);
	return cn;
}
////////////////

//2019-11-16
TimeConstraint* Rules::readActivitiesOccupyMinTimeSlotsFromSelection(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintActivitiesOccupyMinTimeSlotsFromSelection"));
	ConstraintActivitiesOccupyMinTimeSlotsFromSelection* cn=new ConstraintActivitiesOccupyMinTimeSlotsFromSelection();
	
	int ac=0;
	int tsc=0;
	int i=0;
	
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";

		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Number_of_Activities")){
			QString text=xmlReader.readElementText();
			ac=text.toInt();
			xmlReadingLog+="    Read number of activities="+CustomFETString::number(ac)+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->activitiesIds.append(text.toInt());
			xmlReadingLog+="    Read activity id="+CustomFETString::number(cn->activitiesIds[cn->activitiesIds.count()-1])+"\n";
		}
		else if(xmlReader.name()==QString("Number_of_Selected_Time_Slots")){
			QString text=xmlReader.readElementText();
			tsc=text.toInt();
			xmlReadingLog+="    Read number of selected time slots="+CustomFETString::number(tsc)+"\n";
		}
		else if(xmlReader.name()==QString("Selected_Time_Slot")){
			xmlReadingLog+="    Read: selected time slot\n";

			assert(xmlReader.isStartElement());
			while(xmlReader.readNextStartElement()){
				xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
				if(xmlReader.name()==QString("Selected_Day")){
					QString text=xmlReader.readElementText();
					cn->selectedDays.append(0);
					assert(cn->selectedDays.count()-1==i);
					for(cn->selectedDays[i]=0; cn->selectedDays[i]<this->nDaysPerWeek; cn->selectedDays[i]++)
						if(this->daysOfTheWeek[cn->selectedDays[i]]==text)
							break;
							
					if(cn->selectedDays[i]>=this->nDaysPerWeek){
						xmlReader.raiseError(tr("Day %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr("Constraint ActivitiesOccupyMinTimeSlotsFromSelection day corrupt, day %1 is inexistent ... ignoring constraint")
							.arg(text));*/
						delete cn;
						cn=nullptr;
						//goto corruptConstraintTime;
						return nullptr;
					}
					
					assert(cn->selectedDays[i]<this->nDaysPerWeek);
					xmlReadingLog+="    Selected day="+this->daysOfTheWeek[cn->selectedDays[i]]+"("+CustomFETString::number(i)+")"+"\n";
				}
				else if(xmlReader.name()==QString("Selected_Hour")){
					QString text=xmlReader.readElementText();
					cn->selectedHours.append(0);
					assert(cn->selectedHours.count()-1==i);
					for(cn->selectedHours[i]=0; cn->selectedHours[i] < this->nHoursPerDay; cn->selectedHours[i]++)
						if(this->hoursOfTheDay[cn->selectedHours[i]]==text)
							break;
							
					if(cn->selectedHours[i]>=this->nHoursPerDay){
						xmlReader.raiseError(tr("Hour %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr(" Constraint ActivitiesOccupyMinTimeSlotsFromSelection hour corrupt, hour %1 is inexistent ... ignoring constraint")
							.arg(text));*/
						delete cn;
						cn=nullptr;
						//goto corruptConstraintTime;
						return nullptr;
					}
					
					assert(cn->selectedHours[i]>=0 && cn->selectedHours[i] < this->nHoursPerDay);
					xmlReadingLog+="    Selected hour="+this->hoursOfTheDay[cn->selectedHours[i]]+"\n";
				}
				else{
					unrecognizedXmlTags.append(xmlReader.name().toString());
					unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
					unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

					xmlReader.skipCurrentElement();
					xmlReaderNumberOfUnrecognizedFields++;
				}
			}

			i++;
			
			if(!(i==cn->selectedDays.count()) || !(i==cn->selectedHours.count())){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Selected_Time_Slot"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			assert(i==cn->selectedDays.count());
			assert(i==cn->selectedHours.count());
		}
		else if(xmlReader.name()==QString("Min_Number_of_Occupied_Time_Slots")){
			QString text=xmlReader.readElementText();
			cn->minOccupiedTimeSlots=text.toInt();
			xmlReadingLog+="    Read min number of occupied time slots="+CustomFETString::number(cn->minOccupiedTimeSlots)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	
	if(!(ac==cn->activitiesIds.count())){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Activities").arg("Activity_Id"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}

	if(!(i==tsc)){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Selected_Time_Slots").arg("Selected_Time_Slot"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}

	assert(ac==cn->activitiesIds.count());
	assert(i==tsc);
	return cn;
}
////////////////

//2011-09-30
TimeConstraint* Rules::readActivitiesMaxSimultaneousInSelectedTimeSlots(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots"));
	ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots* cn=new ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots();
	
	int ac=0;
	int tsc=0;
	int i=0;
	
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";

		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Number_of_Activities")){
			QString text=xmlReader.readElementText();
			ac=text.toInt();
			xmlReadingLog+="    Read number of activities="+CustomFETString::number(ac)+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->activitiesIds.append(text.toInt());
			xmlReadingLog+="    Read activity id="+CustomFETString::number(cn->activitiesIds[cn->activitiesIds.count()-1])+"\n";
		}
		else if(xmlReader.name()==QString("Number_of_Selected_Time_Slots")){
			QString text=xmlReader.readElementText();
			tsc=text.toInt();
			xmlReadingLog+="    Read number of selected time slots="+CustomFETString::number(tsc)+"\n";
		}
		else if(xmlReader.name()==QString("Selected_Time_Slot")){
			xmlReadingLog+="    Read: selected time slot\n";

			assert(xmlReader.isStartElement());
			while(xmlReader.readNextStartElement()){
				xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
				if(xmlReader.name()==QString("Selected_Day")){
					QString text=xmlReader.readElementText();
					cn->selectedDays.append(0);
					assert(cn->selectedDays.count()-1==i);
					for(cn->selectedDays[i]=0; cn->selectedDays[i]<this->nDaysPerWeek; cn->selectedDays[i]++)
						if(this->daysOfTheWeek[cn->selectedDays[i]]==text)
							break;
							
					if(cn->selectedDays[i]>=this->nDaysPerWeek){
						xmlReader.raiseError(tr("Day %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr("Constraint ActivitiesMaxSimultaneousInSelectedTimeSlots day corrupt, day %1 is inexistent ... ignoring constraint")
							.arg(text));*/
						delete cn;
						cn=nullptr;
						//goto corruptConstraintTime;
						return nullptr;
					}
							
					assert(cn->selectedDays[i]<this->nDaysPerWeek);
					xmlReadingLog+="    Selected day="+this->daysOfTheWeek[cn->selectedDays[i]]+"("+CustomFETString::number(i)+")"+"\n";
				}
				else if(xmlReader.name()==QString("Selected_Hour")){
					QString text=xmlReader.readElementText();
					cn->selectedHours.append(0);
					assert(cn->selectedHours.count()-1==i);
					for(cn->selectedHours[i]=0; cn->selectedHours[i] < this->nHoursPerDay; cn->selectedHours[i]++)
						if(this->hoursOfTheDay[cn->selectedHours[i]]==text)
							break;
							
					if(cn->selectedHours[i]>=this->nHoursPerDay){
						xmlReader.raiseError(tr("Day %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr(" Constraint ActivitiesMaxSimultaneousInSelectedTimeSlots hour corrupt, hour %1 is inexistent ... ignoring constraint")
							.arg(text));*/
						delete cn;
						cn=nullptr;
						//goto corruptConstraintTime;
						return nullptr;
					}
					
					assert(cn->selectedHours[i]>=0 && cn->selectedHours[i] < this->nHoursPerDay);
					xmlReadingLog+="    Selected hour="+this->hoursOfTheDay[cn->selectedHours[i]]+"\n";
				}
				else{
					unrecognizedXmlTags.append(xmlReader.name().toString());
					unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
					unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

					xmlReader.skipCurrentElement();
					xmlReaderNumberOfUnrecognizedFields++;
				}
			}

			i++;

			if(!(i==cn->selectedDays.count()) || !(i==cn->selectedHours.count())){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Selected_Time_Slot"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			assert(i==cn->selectedDays.count());
			assert(i==cn->selectedHours.count());
		}
		else if(xmlReader.name()==QString("Max_Number_of_Simultaneous_Activities")){
			QString text=xmlReader.readElementText();
			cn->maxSimultaneous=text.toInt();
			xmlReadingLog+="    Read max number of simultaneous activities="+CustomFETString::number(cn->maxSimultaneous)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	
	if(!(ac==cn->activitiesIds.count())){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Activities").arg("Activity_Id"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}

	if(!(i==tsc)){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Selected_Time_Slots").arg("Selected_Time_Slot"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}

	assert(ac==cn->activitiesIds.count());
	assert(i==tsc);
	return cn;
}
////////////////

//2019-11-16
TimeConstraint* Rules::readActivitiesMinSimultaneousInSelectedTimeSlots(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintActivitiesMinSimultaneousInSelectedTimeSlots"));
	ConstraintActivitiesMinSimultaneousInSelectedTimeSlots* cn=new ConstraintActivitiesMinSimultaneousInSelectedTimeSlots();
	
	cn->allowEmptySlots=false;
	
	int ac=0;
	int tsc=0;
	int i=0;
	
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";

		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Number_of_Activities")){
			QString text=xmlReader.readElementText();
			ac=text.toInt();
			xmlReadingLog+="    Read number of activities="+CustomFETString::number(ac)+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->activitiesIds.append(text.toInt());
			xmlReadingLog+="    Read activity id="+CustomFETString::number(cn->activitiesIds[cn->activitiesIds.count()-1])+"\n";
		}
		else if(xmlReader.name()==QString("Number_of_Selected_Time_Slots")){
			QString text=xmlReader.readElementText();
			tsc=text.toInt();
			xmlReadingLog+="    Read number of selected time slots="+CustomFETString::number(tsc)+"\n";
		}
		else if(xmlReader.name()==QString("Selected_Time_Slot")){
			xmlReadingLog+="    Read: selected time slot\n";

			assert(xmlReader.isStartElement());
			while(xmlReader.readNextStartElement()){
				xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
				if(xmlReader.name()==QString("Selected_Day")){
					QString text=xmlReader.readElementText();
					cn->selectedDays.append(0);
					assert(cn->selectedDays.count()-1==i);
					for(cn->selectedDays[i]=0; cn->selectedDays[i]<this->nDaysPerWeek; cn->selectedDays[i]++)
						if(this->daysOfTheWeek[cn->selectedDays[i]]==text)
							break;
							
					if(cn->selectedDays[i]>=this->nDaysPerWeek){
						xmlReader.raiseError(tr("Day %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr("Constraint ActivitiesMinSimultaneousInSelectedTimeSlots day corrupt, day %1 is inexistent ... ignoring constraint")
							.arg(text));*/
						delete cn;
						cn=nullptr;
						//goto corruptConstraintTime;
						return nullptr;
					}
							
					assert(cn->selectedDays[i]<this->nDaysPerWeek);
					xmlReadingLog+="    Selected day="+this->daysOfTheWeek[cn->selectedDays[i]]+"("+CustomFETString::number(i)+")"+"\n";
				}
				else if(xmlReader.name()==QString("Selected_Hour")){
					QString text=xmlReader.readElementText();
					cn->selectedHours.append(0);
					assert(cn->selectedHours.count()-1==i);
					for(cn->selectedHours[i]=0; cn->selectedHours[i] < this->nHoursPerDay; cn->selectedHours[i]++)
						if(this->hoursOfTheDay[cn->selectedHours[i]]==text)
							break;
							
					if(cn->selectedHours[i]>=this->nHoursPerDay){
						xmlReader.raiseError(tr("Day %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr(" Constraint ActivitiesMinSimultaneousInSelectedTimeSlots hour corrupt, hour %1 is inexistent ... ignoring constraint")
							.arg(text));*/
						delete cn;
						cn=nullptr;
						//goto corruptConstraintTime;
						return nullptr;
					}
					
					assert(cn->selectedHours[i]>=0 && cn->selectedHours[i] < this->nHoursPerDay);
					xmlReadingLog+="    Selected hour="+this->hoursOfTheDay[cn->selectedHours[i]]+"\n";
				}
				else{
					unrecognizedXmlTags.append(xmlReader.name().toString());
					unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
					unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

					xmlReader.skipCurrentElement();
					xmlReaderNumberOfUnrecognizedFields++;
				}
			}

			i++;

			if(!(i==cn->selectedDays.count()) || !(i==cn->selectedHours.count())){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Selected_Time_Slot"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
			assert(i==cn->selectedDays.count());
			assert(i==cn->selectedHours.count());
		}
		else if(xmlReader.name()==QString("Min_Number_of_Simultaneous_Activities")){
			QString text=xmlReader.readElementText();
			cn->minSimultaneous=text.toInt();
			xmlReadingLog+="    Read min number of simultaneous activities="+CustomFETString::number(cn->minSimultaneous)+"\n";
		}
		else if(xmlReader.name()==QString("Allow_Empty_Slots")){
			QString text=xmlReader.readElementText();
			if(text=="true")
				cn->allowEmptySlots=true;
			else
				cn->allowEmptySlots=false;
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	
	if(!(ac==cn->activitiesIds.count())){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Activities").arg("Activity_Id"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}

	if(!(i==tsc)){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Selected_Time_Slots").arg("Selected_Time_Slot"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}

	assert(ac==cn->activitiesIds.count());
	assert(i==tsc);
	return cn;
}
////////////////

/////2017-02-07
TimeConstraint* Rules::readTeacherMaxSpanPerDay(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeacherMaxSpanPerDay"));
	ConstraintTeacherMaxSpanPerDay* cn=new ConstraintTeacherMaxSpanPerDay();
	cn->allowOneDayExceptionPlusOne=false;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Allow_One_Day_Exception_of_Plus_One")){
			QString text=xmlReader.readElementText();
			if(text=="true"){
				cn->allowOneDayExceptionPlusOne=true;
			}
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Teacher_Name")){
			QString text=xmlReader.readElementText();
			cn->teacherName=text;
			xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
		}
		else if(xmlReader.name()==QString("Max_Span")){
			QString text=xmlReader.readElementText();
			cn->maxSpanPerDay=text.toInt();
			xmlReadingLog+="    Adding max span per day="+CustomFETString::number(cn->maxSpanPerDay)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readTeachersMaxSpanPerDay(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeachersMaxSpanPerDay"));
	ConstraintTeachersMaxSpanPerDay* cn=new ConstraintTeachersMaxSpanPerDay();
	cn->allowOneDayExceptionPlusOne=false;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Allow_One_Day_Exception_of_Plus_One")){
			QString text=xmlReader.readElementText();
			if(text=="true"){
				cn->allowOneDayExceptionPlusOne=true;
			}
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Max_Span")){
			QString text=xmlReader.readElementText();
			cn->maxSpanPerDay=text.toInt();
			xmlReadingLog+="    Adding max span per day="+CustomFETString::number(cn->maxSpanPerDay)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readStudentsSetMaxSpanPerDay(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsSetMaxSpanPerDay"));
	ConstraintStudentsSetMaxSpanPerDay* cn=new ConstraintStudentsSetMaxSpanPerDay();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Students")){
			QString text=xmlReader.readElementText();
			cn->students=text;
			xmlReadingLog+="    Read students name="+cn->students+"\n";
		}
		else if(xmlReader.name()==QString("Max_Span")){
			QString text=xmlReader.readElementText();
			cn->maxSpanPerDay=text.toInt();
			xmlReadingLog+="    Adding max span per day="+CustomFETString::number(cn->maxSpanPerDay)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readStudentsMaxSpanPerDay(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsMaxSpanPerDay"));
	ConstraintStudentsMaxSpanPerDay* cn=new ConstraintStudentsMaxSpanPerDay();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Max_Span")){
			QString text=xmlReader.readElementText();
			cn->maxSpanPerDay=text.toInt();
			xmlReadingLog+="    Adding max span per day="+CustomFETString::number(cn->maxSpanPerDay)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readTeacherMinRestingHours(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeacherMinRestingHours"));
	ConstraintTeacherMinRestingHours* cn=new ConstraintTeacherMinRestingHours();
	cn->circular=true;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Circular")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->circular=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Teacher_Name")){
			QString text=xmlReader.readElementText();
			cn->teacherName=text;
			xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
		}
		else if(xmlReader.name()==QString("Minimum_Resting_Hours")){
			QString text=xmlReader.readElementText();
			cn->minRestingHours=text.toInt();
			xmlReadingLog+="    Adding min resting hours="+CustomFETString::number(cn->minRestingHours)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readTeachersMinRestingHours(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeachersMinRestingHours"));
	ConstraintTeachersMinRestingHours* cn=new ConstraintTeachersMinRestingHours();
	cn->circular=true;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Circular")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->circular=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Minimum_Resting_Hours")){
			QString text=xmlReader.readElementText();
			cn->minRestingHours=text.toInt();
			xmlReadingLog+="    Adding min resting hours="+CustomFETString::number(cn->minRestingHours)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readStudentsSetMinRestingHours(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsSetMinRestingHours"));
	ConstraintStudentsSetMinRestingHours* cn=new ConstraintStudentsSetMinRestingHours();
	cn->circular=true;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Circular")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->circular=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Students")){
			QString text=xmlReader.readElementText();
			cn->students=text;
			xmlReadingLog+="    Read students name="+cn->students+"\n";
		}
		else if(xmlReader.name()==QString("Minimum_Resting_Hours")){
			QString text=xmlReader.readElementText();
			cn->minRestingHours=text.toInt();
			xmlReadingLog+="    Adding min resting hours="+CustomFETString::number(cn->minRestingHours)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

TimeConstraint* Rules::readStudentsMinRestingHours(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsMinRestingHours"));
	ConstraintStudentsMinRestingHours* cn=new ConstraintStudentsMinRestingHours();
	cn->circular=true;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Circular")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->circular=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Minimum_Resting_Hours")){
			QString text=xmlReader.readElementText();
			cn->minRestingHours=text.toInt();
			xmlReadingLog+="    Adding min resting hours="+CustomFETString::number(cn->minRestingHours)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}
///////////////

///space constraints reading routines
SpaceConstraint* Rules::readBasicCompulsorySpace(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintBasicCompulsorySpace"));
	ConstraintBasicCompulsorySpace* cn=new ConstraintBasicCompulsorySpace();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";

		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		/*if(xmlReader.name()==QString("Weight")){
			cn->weight=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight="+CustomFETString::number(cn->weight)+"\n";
		}
		else if(xmlReader.name()==QString("Compulsory")){
			if(text=="yes"){
				cn->compulsory=true;
				xmlReadingLog+="    Current constraint is compulsory\n";
			}
			else{
				cn->compulsory=false;
				xmlReadingLog+="    Current constraint is not compulsory\n";
			}
		}*/
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

SpaceConstraint* Rules::readRoomNotAvailable(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintRoomNotAvailable"));

	QList<int> days;
	QList<int> hours;
	QString room;
	double weightPercentage=100;
	int d=-1, h1=-1, h2=-1;
	bool active=true;
	QString comments=QString("");
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Read weight percentage="+CustomFETString::number(weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			comments=text;
		}
		else if(xmlReader.name()==QString("Day")){
			QString text=xmlReader.readElementText();
			for(d=0; d<this->nDaysPerWeek; d++)
				if(this->daysOfTheWeek[d]==text)
					break;
			if(d>=this->nDaysPerWeek){
				xmlReader.raiseError(tr("Day %1 is inexistent").arg(text));
				/*RulesReconcilableMessage::information(parent, tr("FET information"),
					tr("Constraint RoomNotAvailable day corrupt for room %1, day %2 is inexistent ... ignoring constraint")
					.arg(room)
					.arg(text));*/
				//cn=nullptr;
				//goto corruptConstraintSpace;
				return nullptr;
			}
			assert(d<this->nDaysPerWeek);
			xmlReadingLog+="    Crt. day="+this->daysOfTheWeek[d]+"\n";
		}
		else if(xmlReader.name()==QString("Start_Hour")){
			QString text=xmlReader.readElementText();
			for(h1=0; h1 < this->nHoursPerDay; h1++)
				if(this->hoursOfTheDay[h1]==text)
					break;
			if(h1==this->nHoursPerDay){
				xmlReader.raiseError(tr("Hour %1 is the last hour - impossible").arg(text));
				return nullptr;
			}
			else if(h1>this->nHoursPerDay){
				xmlReader.raiseError(tr("Hour %1 is inexistent").arg(text));
				/*RulesReconcilableMessage::information(parent, tr("FET information"),
					tr("Constraint RoomNotAvailable start hour corrupt for room %1, hour %2 is inexistent ... ignoring constraint")
					.arg(room)
					.arg(text));*/
				//cn=nullptr;
				//goto corruptConstraintSpace;
				return nullptr;
			}
			assert(h1>=0 && h1 < this->nHoursPerDay);
			xmlReadingLog+="    Start hour="+this->hoursOfTheDay[h1]+"\n";
		}
		else if(xmlReader.name()==QString("End_Hour")){
			QString text=xmlReader.readElementText();
			for(h2=0; h2 < this->nHoursPerDay; h2++)
				if(this->hoursOfTheDay[h2]==text)
					break;
			if(h2==0){
				xmlReader.raiseError(tr("Hour %1 is the first hour - impossible").arg(text));
				return nullptr;
			}
			else if(h2<0 || h2>this->nHoursPerDay){
				xmlReader.raiseError(tr("Hour %1 is inexistent").arg(text));
				/*RulesReconcilableMessage::information(parent, tr("FET information"),
					tr("Constraint RoomNotAvailable end hour corrupt for room %1, hour %2 is inexistent ... ignoring constraint")
					.arg(room)
					.arg(text));*/
				//goto corruptConstraintSpace;
				return nullptr;
			}
			assert(h2>0 && h2 <= this->nHoursPerDay);
			xmlReadingLog+="    End hour="+this->hoursOfTheDay[h2]+"\n";
		}
		else if(xmlReader.name()==QString("Room_Name")){
			QString text=xmlReader.readElementText();
			room=text;
			xmlReadingLog+="    Read room name="+room+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	
	if(d<0){
		xmlReader.raiseError(tr("Field missing: %1").arg("Day"));
		return nullptr;
	}
	else if(h1<0){
		xmlReader.raiseError(tr("Field missing: %1").arg("Start_Hour"));
		return nullptr;
	}
	else if(h2<0){
		xmlReader.raiseError(tr("Field missing: %1").arg("End_Hour"));
		return nullptr;
	}
	assert(weightPercentage>=0);
	assert(d>=0 && h1>=0 && h2>=0);

	ConstraintRoomNotAvailableTimes* cn = nullptr;
	
	bool found=false;
	for(SpaceConstraint* c : qAsConst(this->spaceConstraintsList))
		if(c->type==CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES){
			ConstraintRoomNotAvailableTimes* tna=(ConstraintRoomNotAvailableTimes*) c;
			if(tna->room==room){
				found=true;
				
				for(int hh=h1; hh<h2; hh++){
					int k;
					for(k=0; k<tna->days.count(); k++)
						if(tna->days.at(k)==d && tna->hours.at(k)==hh)
							break;
					if(k==tna->days.count()){
						tna->days.append(d);
						tna->hours.append(hh);
					}
				}
				
				assert(tna->days.count()==tna->hours.count());
			}
		}
	if(!found){
		days.clear();
		hours.clear();
		for(int hh=h1; hh<h2; hh++){
			days.append(d);
			hours.append(hh);
		}
	
		cn=new ConstraintRoomNotAvailableTimes(weightPercentage, room, days, hours);
		cn->active=active;
		cn->comments=comments;

		return cn;
	}
	else
		return nullptr;
}

SpaceConstraint* Rules::readRoomNotAvailableTimes(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintRoomNotAvailableTimes"));
	ConstraintRoomNotAvailableTimes* cn=new ConstraintRoomNotAvailableTimes();
	int nNotAvailableSlots=-1;
	int i=0;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Read weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}

		else if(xmlReader.name()==QString("Number_of_Not_Available_Times")){
			QString text=xmlReader.readElementText();
			nNotAvailableSlots=text.toInt();
			xmlReadingLog+="    Read number of not available times="+CustomFETString::number(nNotAvailableSlots)+"\n";
		}

		else if(xmlReader.name()==QString("Not_Available_Time")){
			xmlReadingLog+="    Read: not available time\n";
			
			int d=-1;
			int h=-1;

			assert(xmlReader.isStartElement());
			while(xmlReader.readNextStartElement()){
				xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
				if(xmlReader.name()==QString("Day")){
					QString text=xmlReader.readElementText();
					for(d=0; d<this->nDaysPerWeek; d++)
						if(this->daysOfTheWeek[d]==text)
							break;

					if(d>=this->nDaysPerWeek){
						xmlReader.raiseError(tr("Day %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr("Constraint RoomNotAvailableTimes day corrupt for room %1, day %2 is inexistent ... ignoring constraint")
							.arg(cn->room)
							.arg(text));*/
						delete cn;
						cn=nullptr;
						return nullptr;
						//goto corruptConstraintSpace;
					}
		
					assert(d<this->nDaysPerWeek);
					xmlReadingLog+="    Day="+this->daysOfTheWeek[d]+"("+CustomFETString::number(i)+")"+"\n";
				}
				else if(xmlReader.name()==QString("Hour")){
					QString text=xmlReader.readElementText();
					for(h=0; h < this->nHoursPerDay; h++)
						if(this->hoursOfTheDay[h]==text)
							break;
					
					if(h>=this->nHoursPerDay){
						xmlReader.raiseError(tr("Hour %1 is inexistent").arg(text));
						/*RulesReconcilableMessage::information(parent, tr("FET information"),
							tr("Constraint RoomNotAvailableTimes hour corrupt for room %1, hour %2 is inexistent ... ignoring constraint")
							.arg(cn->room)
							.arg(text));*/
						delete cn;
						cn=nullptr;
						//goto corruptConstraintSpace;
						return nullptr;
					}
					
					assert(h>=0 && h < this->nHoursPerDay);
					xmlReadingLog+="    Hour="+this->hoursOfTheDay[h]+"\n";
				}
				else{
					unrecognizedXmlTags.append(xmlReader.name().toString());
					unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
					unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

					xmlReader.skipCurrentElement();
					xmlReaderNumberOfUnrecognizedFields++;
				}
			}
			i++;
			
			cn->days.append(d);
			cn->hours.append(h);

			if(d==-1 || h==-1){
				xmlReader.raiseError(tr("%1 is incorrect").arg("Not_Available_Time"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
		}
		else if(xmlReader.name()==QString("Room")){
			QString text=xmlReader.readElementText();
			cn->room=text;
			xmlReadingLog+="    Read room name="+cn->room+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	assert(i==cn->days.count() && i==cn->hours.count());
	if(!(i==nNotAvailableSlots)){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Not_Available_Times").arg("Not_Available_Time"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(i==nNotAvailableSlots);
	return cn;
}

SpaceConstraint* Rules::readActivityPreferredRoom(QWidget* parent, QXmlStreamReader& xmlReader, FakeString& xmlReadingLog,
bool& reportUnspecifiedPermanentlyLockedSpace){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintActivityPreferredRoom"));
	ConstraintActivityPreferredRoom* cn=new ConstraintActivityPreferredRoom();
	cn->permanentlyLocked=false; //default
	bool foundLocked=false;
	int specifiedNumberOfRealRooms=0;
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Permanently_Locked")){
			QString text=xmlReader.readElementText();
			if(text=="true" || text=="1" || text=="yes"){
				xmlReadingLog+="    Permanently locked\n";
				cn->permanentlyLocked=true;
			}
			else{
				if(!(text=="no" || text=="false" || text=="0")){
					RulesReconcilableMessage::warning(parent, tr("FET warning"),
						tr("Found constraint activity preferred room with tag permanently locked"
						" which is not 'true', 'false', 'yes', 'no', '1' or '0'."
						" The tag will be considered false",
						"Instructions for translators: please leave the 'true', 'false', 'yes' and 'no' fields untranslated, as they are in English"));
				}
				//assert(text=="false" || text=="0" || text=="no");
				xmlReadingLog+="    Not permanently locked\n";
				cn->permanentlyLocked=false;
			}
			foundLocked=true;
		}

		/*if(xmlReader.name()==QString("Weight")){
			cn->weight=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight="+CustomFETString::number(cn->weight)+"\n";
		}
		else if(xmlReader.name()==QString("Compulsory")){
			if(text=="yes"){
				cn->compulsory=true;
				xmlReadingLog+="    Current constraint is compulsory\n";
			}
			else{
				cn->compulsory=false;
				xmlReadingLog+="    Current constraint is not compulsory\n";
			}
		}*/
		else if(xmlReader.name()==QString("Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->activityId=text.toInt();
			xmlReadingLog+="    Read activity id="+CustomFETString::number(cn->activityId)+"\n";
		}
		else if(xmlReader.name()==QString("Room")){
			QString text=xmlReader.readElementText();
			cn->roomName=text;
			xmlReadingLog+="    Read room="+text+"\n";
		}
		else if(xmlReader.name()==QString("Number_of_Real_Rooms")){
			QString text=xmlReader.readElementText();
			specifiedNumberOfRealRooms=text.toInt();
			xmlReadingLog+="    Read number of real rooms="+CustomFETString::number(specifiedNumberOfRealRooms)+"\n";
		}
		else if(xmlReader.name()==QString("Real_Room")){
			QString text=xmlReader.readElementText();
			cn->preferredRealRoomsNames.append(text);
			xmlReadingLog+="    Read real room="+text+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}

	//Might be slow, so I deactivated this check.
	//To add it again, initialize specifiedNumberOfRealRooms with -1 in the beginning.
	/*int r=gt.rules.searchRoom(cn->roomName);
	if(r>=0){
		if(gt.rules.roomsList[r]->isVirtual){
			if(specifiedNumberOfRealRooms==-1){
				xmlReader.raiseError(tr("Argument %1 missing in constraint.").arg("Number_of_Real_Rooms"));
				delete cn;
				cn=nullptr;
				return nullptr;
			}
		}
	}*/
	
	if(cn->preferredRealRoomsNames.count()!=specifiedNumberOfRealRooms){
		xmlReader.raiseError(tr("The specified number of real rooms is not equal to the read number of real rooms in constraint."));
		delete cn;
		cn=nullptr;
		return nullptr;
	}

	if(!foundLocked && reportUnspecifiedPermanentlyLockedSpace){
		int t=RulesReconcilableMessage::information(parent, tr("FET information"),
			tr("Found constraint activity preferred room, with unspecified tag"
			" 'permanently locked' - this tag will be set to 'false' by default. You can always modify it"
			" by editing the constraint in the 'Data' menu")+"\n\n"
			+tr("Explanation: starting with version 5.8.0 (January 2009), the constraint"
			" activity preferred room has"
			" a new tag, 'permanently locked' (true or false)."
			" It is recommended to make the tag 'permanently locked' true for the constraints you"
			" need to be not modifiable from the 'Timetable' menu"
			" and leave this tag false for the constraints you need to be modifiable from the 'Timetable' menu"
			" (the 'permanently locked' tag can be modified by editing the constraint from the 'Data' menu)."
			" This way, when viewing the timetable"
			" and locking/unlocking some activities, you will not unlock the constraints which"
			" need to be locked all the time."
			),
			tr("Skip rest"), tr("See next"), QString(), 1, 0 );
		if(t==0)
			reportUnspecifiedPermanentlyLockedSpace=false;
	}

	return cn;
}

SpaceConstraint* Rules::readActivityPreferredRooms(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintActivityPreferredRooms"));
	int _n_preferred_rooms=0;
	ConstraintActivityPreferredRooms* cn=new ConstraintActivityPreferredRooms();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		/*if(xmlReader.name()==QString("Weight")){
			cn->weight=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight="+CustomFETString::number(cn->weight)+"\n";
		}
		else if(xmlReader.name()==QString("Compulsory")){
			if(text=="yes"){
				cn->compulsory=true;
				xmlReadingLog+="    Current constraint is compulsory\n";
			}
			else{
				cn->compulsory=false;
				xmlReadingLog+="    Current constraint is not compulsory\n";
			}
		}*/
		else if(xmlReader.name()==QString("Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->activityId=text.toInt();
			xmlReadingLog+="    Read activity id="+CustomFETString::number(cn->activityId)+"\n";
		}
		else if(xmlReader.name()==QString("Number_of_Preferred_Rooms")){
			QString text=xmlReader.readElementText();
			_n_preferred_rooms=text.toInt();
			xmlReadingLog+="    Read number of preferred rooms: "+CustomFETString::number(_n_preferred_rooms)+"\n";
			//assert(_n_preferred_rooms>=2);
		}
		else if(xmlReader.name()==QString("Preferred_Room")){
			QString text=xmlReader.readElementText();
			cn->roomsNames.append(text);
			xmlReadingLog+="    Read room="+text+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(!(_n_preferred_rooms==cn->roomsNames.count())){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Preferred_Rooms").arg("Preferred_Room"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(_n_preferred_rooms==cn->roomsNames.count());
	return cn;
}

SpaceConstraint* Rules::readSubjectPreferredRoom(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintSubjectPreferredRoom"));
	ConstraintSubjectPreferredRoom* cn=new ConstraintSubjectPreferredRoom();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Subject")){
			QString text=xmlReader.readElementText();
			cn->subjectName=text;
			xmlReadingLog+="    Read subject="+cn->subjectName+"\n";
		}
		else if(xmlReader.name()==QString("Room")){
			QString text=xmlReader.readElementText();
			cn->roomName=text;
			xmlReadingLog+="    Read room="+cn->roomName+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

SpaceConstraint* Rules::readSubjectPreferredRooms(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintSubjectPreferredRooms"));
	int _n_preferred_rooms=0;
	ConstraintSubjectPreferredRooms* cn=new ConstraintSubjectPreferredRooms();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Subject")){
			QString text=xmlReader.readElementText();
			cn->subjectName=text;
			xmlReadingLog+="    Read subject="+cn->subjectName+"\n";
		}
		else if(xmlReader.name()==QString("Number_of_Preferred_Rooms")){
			QString text=xmlReader.readElementText();
			_n_preferred_rooms=text.toInt();
			xmlReadingLog+="    Read number of preferred rooms: "+CustomFETString::number(_n_preferred_rooms)+"\n";
			//assert(_n_preferred_rooms>=2);
		}
		else if(xmlReader.name()==QString("Preferred_Room")){
			QString text=xmlReader.readElementText();
			cn->roomsNames.append(text);
			xmlReadingLog+="    Read room="+text+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(!(_n_preferred_rooms==cn->roomsNames.count())){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Preferred_Rooms").arg("Preferred_Room"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(_n_preferred_rooms==cn->roomsNames.count());
	return cn;
}

SpaceConstraint* Rules::readSubjectSubjectTagPreferredRoom(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintSubjectSubjectTagPreferredRoom"));
	ConstraintSubjectActivityTagPreferredRoom* cn=new ConstraintSubjectActivityTagPreferredRoom();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Subject")){
			QString text=xmlReader.readElementText();
			cn->subjectName=text;
			xmlReadingLog+="    Read subject="+cn->subjectName+"\n";
		}
		else if(xmlReader.name()==QString("Subject_Tag")){
			QString text=xmlReader.readElementText();
			cn->activityTagName=text;
			xmlReadingLog+="    Read activity tag="+cn->activityTagName+"\n";
		}
		else if(xmlReader.name()==QString("Room")){
			QString text=xmlReader.readElementText();
			cn->roomName=text;
			xmlReadingLog+="    Read room="+cn->roomName+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

SpaceConstraint* Rules::readSubjectSubjectTagPreferredRooms(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintSubjectSubjectTagPreferredRooms"));
	int _n_preferred_rooms=0;
	ConstraintSubjectActivityTagPreferredRooms* cn=new ConstraintSubjectActivityTagPreferredRooms();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Subject")){
			QString text=xmlReader.readElementText();
			cn->subjectName=text;
			xmlReadingLog+="    Read subject="+cn->subjectName+"\n";
		}
		else if(xmlReader.name()==QString("Subject_Tag")){
			QString text=xmlReader.readElementText();
			cn->activityTagName=text;
			xmlReadingLog+="    Read activity tag="+cn->activityTagName+"\n";
		}
		else if(xmlReader.name()==QString("Number_of_Preferred_Rooms")){
			QString text=xmlReader.readElementText();
			_n_preferred_rooms=text.toInt();
			xmlReadingLog+="    Read number of preferred rooms: "+CustomFETString::number(_n_preferred_rooms)+"\n";
			//assert(_n_preferred_rooms>=2);
		}
		else if(xmlReader.name()==QString("Preferred_Room")){
			QString text=xmlReader.readElementText();
			cn->roomsNames.append(text);
			xmlReadingLog+="    Read room="+text+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(!(_n_preferred_rooms==cn->roomsNames.count())){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Preferred_Rooms").arg("Preferred_Room"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(_n_preferred_rooms==cn->roomsNames.count());
	return cn;
}

SpaceConstraint* Rules::readSubjectActivityTagPreferredRoom(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintSubjectActivityTagPreferredRoom"));
	ConstraintSubjectActivityTagPreferredRoom* cn=new ConstraintSubjectActivityTagPreferredRoom();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Subject")){
			QString text=xmlReader.readElementText();
			cn->subjectName=text;
			xmlReadingLog+="    Read subject="+cn->subjectName+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Tag")){
			QString text=xmlReader.readElementText();
			cn->activityTagName=text;
			xmlReadingLog+="    Read activity tag="+cn->activityTagName+"\n";
		}
		else if(xmlReader.name()==QString("Room")){
			QString text=xmlReader.readElementText();
			cn->roomName=text;
			xmlReadingLog+="    Read room="+cn->roomName+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

SpaceConstraint* Rules::readSubjectActivityTagPreferredRooms(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintSubjectActivityTagPreferredRooms"));
	int _n_preferred_rooms=0;
	ConstraintSubjectActivityTagPreferredRooms* cn=new ConstraintSubjectActivityTagPreferredRooms();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight")){
			//cn->weight=customFETStrToDouble(text);
			xmlReader.skipCurrentElement();
			xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
			cn->weightPercentage=100;
		}
		else if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Compulsory")){
			QString text=xmlReader.readElementText();
			if(text=="yes"){
				//cn->compulsory=true;
				xmlReadingLog+="    Ignoring old tag - Current constraint is compulsory\n";
				cn->weightPercentage=100;
			}
			else{
				//cn->compulsory=false;
				xmlReadingLog+="    Old tag - current constraint is not compulsory - making weightPercentage=0%\n";
				cn->weightPercentage=0;
			}
		}
		else if(xmlReader.name()==QString("Subject")){
			QString text=xmlReader.readElementText();
			cn->subjectName=text;
			xmlReadingLog+="    Read subject="+cn->subjectName+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Tag")){
			QString text=xmlReader.readElementText();
			cn->activityTagName=text;
			xmlReadingLog+="    Read activity tag="+cn->activityTagName+"\n";
		}
		else if(xmlReader.name()==QString("Number_of_Preferred_Rooms")){
			QString text=xmlReader.readElementText();
			_n_preferred_rooms=text.toInt();
			xmlReadingLog+="    Read number of preferred rooms: "+CustomFETString::number(_n_preferred_rooms)+"\n";
			//assert(_n_preferred_rooms>=2);
		}
		else if(xmlReader.name()==QString("Preferred_Room")){
			QString text=xmlReader.readElementText();
			cn->roomsNames.append(text);
			xmlReadingLog+="    Read room="+text+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(!(_n_preferred_rooms==cn->roomsNames.count())){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Preferred_Rooms").arg("Preferred_Room"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(_n_preferred_rooms==cn->roomsNames.count());
	return cn;
}

SpaceConstraint* Rules::readActivityTagPreferredRoom(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	//added 6 apr 2009
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintActivityTagPreferredRoom"));
	ConstraintActivityTagPreferredRoom* cn=new ConstraintActivityTagPreferredRoom();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Activity_Tag")){
			QString text=xmlReader.readElementText();
			cn->activityTagName=text;
			xmlReadingLog+="    Read activity tag="+cn->activityTagName+"\n";
		}
		else if(xmlReader.name()==QString("Room")){
			QString text=xmlReader.readElementText();
			cn->roomName=text;
			xmlReadingLog+="    Read room="+cn->roomName+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

SpaceConstraint* Rules::readActivityTagPreferredRooms(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintActivityTagPreferredRooms"));
	int _n_preferred_rooms=0;
	ConstraintActivityTagPreferredRooms* cn=new ConstraintActivityTagPreferredRooms();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Activity_Tag")){
			QString text=xmlReader.readElementText();
			cn->activityTagName=text;
			xmlReadingLog+="    Read activity tag="+cn->activityTagName+"\n";
		}
		else if(xmlReader.name()==QString("Number_of_Preferred_Rooms")){
			QString text=xmlReader.readElementText();
			_n_preferred_rooms=text.toInt();
			xmlReadingLog+="    Read number of preferred rooms: "+CustomFETString::number(_n_preferred_rooms)+"\n";
			//assert(_n_preferred_rooms>=2);
		}
		else if(xmlReader.name()==QString("Preferred_Room")){
			QString text=xmlReader.readElementText();
			cn->roomsNames.append(text);
			xmlReadingLog+="    Read room="+text+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(!(_n_preferred_rooms==cn->roomsNames.count())){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Preferred_Rooms").arg("Preferred_Room"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(_n_preferred_rooms==cn->roomsNames.count());
	return cn;
}

SpaceConstraint* Rules::readStudentsSetHomeRoom(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsSetHomeRoom"));
	ConstraintStudentsSetHomeRoom* cn=new ConstraintStudentsSetHomeRoom();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Students")){
			QString text=xmlReader.readElementText();
			cn->studentsName=text;
			xmlReadingLog+="    Read students="+cn->studentsName+"\n";
		}
		else if(xmlReader.name()==QString("Room")){
			QString text=xmlReader.readElementText();
			cn->roomName=text;
			xmlReadingLog+="    Read room="+cn->roomName+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

SpaceConstraint* Rules::readStudentsSetHomeRooms(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsSetHomeRooms"));
	int _n_preferred_rooms=0;
	ConstraintStudentsSetHomeRooms* cn=new ConstraintStudentsSetHomeRooms();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Students")){
			QString text=xmlReader.readElementText();
			cn->studentsName=text;
			xmlReadingLog+="    Read students="+cn->studentsName+"\n";
		}
		else if(xmlReader.name()==QString("Number_of_Preferred_Rooms")){
			QString text=xmlReader.readElementText();
			_n_preferred_rooms=text.toInt();
			xmlReadingLog+="    Read number of preferred rooms: "+CustomFETString::number(_n_preferred_rooms)+"\n";
			//assert(_n_preferred_rooms>=2);
		}
		else if(xmlReader.name()==QString("Preferred_Room")){
			QString text=xmlReader.readElementText();
			cn->roomsNames.append(text);
			xmlReadingLog+="    Read room="+text+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(!(_n_preferred_rooms==cn->roomsNames.count())){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Preferred_Rooms").arg("Preferred_Room"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(_n_preferred_rooms==cn->roomsNames.count());
	return cn;
}

SpaceConstraint* Rules::readTeacherHomeRoom(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeacherHomeRoom"));
	ConstraintTeacherHomeRoom* cn=new ConstraintTeacherHomeRoom();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Teacher")){
			QString text=xmlReader.readElementText();
			cn->teacherName=text;
			xmlReadingLog+="    Read teacher="+cn->teacherName+"\n";
		}
		else if(xmlReader.name()==QString("Room")){
			QString text=xmlReader.readElementText();
			cn->roomName=text;
			xmlReadingLog+="    Read room="+cn->roomName+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

SpaceConstraint* Rules::readTeacherHomeRooms(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeacherHomeRooms"));
	int _n_preferred_rooms=0;
	ConstraintTeacherHomeRooms* cn=new ConstraintTeacherHomeRooms();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Teacher")){
			QString text=xmlReader.readElementText();
			cn->teacherName=text;
			xmlReadingLog+="    Read teacher="+cn->teacherName+"\n";
		}
		else if(xmlReader.name()==QString("Number_of_Preferred_Rooms")){
			QString text=xmlReader.readElementText();
			_n_preferred_rooms=text.toInt();
			xmlReadingLog+="    Read number of preferred rooms: "+CustomFETString::number(_n_preferred_rooms)+"\n";
			//assert(_n_preferred_rooms>=2);
		}
		else if(xmlReader.name()==QString("Preferred_Room")){
			QString text=xmlReader.readElementText();
			cn->roomsNames.append(text);
			xmlReadingLog+="    Read room="+text+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	if(!(_n_preferred_rooms==cn->roomsNames.count())){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Preferred_Rooms").arg("Preferred_Room"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(_n_preferred_rooms==cn->roomsNames.count());
	return cn;
}

SpaceConstraint* Rules::readTeacherMaxBuildingChangesPerDay(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeacherMaxBuildingChangesPerDay"));
	ConstraintTeacherMaxBuildingChangesPerDay* cn=new ConstraintTeacherMaxBuildingChangesPerDay();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Teacher")){
			QString text=xmlReader.readElementText();
			cn->teacherName=text;
			xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
		}
		else if(xmlReader.name()==QString("Max_Building_Changes_Per_Day")){
			QString text=xmlReader.readElementText();
			cn->maxBuildingChangesPerDay=text.toInt();
			xmlReadingLog+="    Max. building changes per day="+CustomFETString::number(cn->maxBuildingChangesPerDay)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

SpaceConstraint* Rules::readTeachersMaxBuildingChangesPerDay(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeachersMaxBuildingChangesPerDay"));
	ConstraintTeachersMaxBuildingChangesPerDay* cn=new ConstraintTeachersMaxBuildingChangesPerDay();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Max_Building_Changes_Per_Day")){
			QString text=xmlReader.readElementText();
			cn->maxBuildingChangesPerDay=text.toInt();
			xmlReadingLog+="    Max. building changes per day="+CustomFETString::number(cn->maxBuildingChangesPerDay)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

SpaceConstraint* Rules::readTeacherMaxBuildingChangesPerWeek(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeacherMaxBuildingChangesPerWeek"));
	ConstraintTeacherMaxBuildingChangesPerWeek* cn=new ConstraintTeacherMaxBuildingChangesPerWeek();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Teacher")){
			QString text=xmlReader.readElementText();
			cn->teacherName=text;
			xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
		}
		else if(xmlReader.name()==QString("Max_Building_Changes_Per_Week")){
			QString text=xmlReader.readElementText();
			cn->maxBuildingChangesPerWeek=text.toInt();
			xmlReadingLog+="    Max. building changes per week="+CustomFETString::number(cn->maxBuildingChangesPerWeek)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

SpaceConstraint* Rules::readTeachersMaxBuildingChangesPerWeek(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeachersMaxBuildingChangesPerWeek"));
	ConstraintTeachersMaxBuildingChangesPerWeek* cn=new ConstraintTeachersMaxBuildingChangesPerWeek();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Max_Building_Changes_Per_Week")){
			QString text=xmlReader.readElementText();
			cn->maxBuildingChangesPerWeek=text.toInt();
			xmlReadingLog+="    Max. building changes per week="+CustomFETString::number(cn->maxBuildingChangesPerWeek)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

SpaceConstraint* Rules::readTeacherMinGapsBetweenBuildingChanges(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeacherMinGapsBetweenBuildingChanges"));
	ConstraintTeacherMinGapsBetweenBuildingChanges* cn=new ConstraintTeacherMinGapsBetweenBuildingChanges();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Teacher")){
			QString text=xmlReader.readElementText();
			cn->teacherName=text;
			xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
		}
		else if(xmlReader.name()==QString("Min_Gaps_Between_Building_Changes")){
			QString text=xmlReader.readElementText();
			cn->minGapsBetweenBuildingChanges=text.toInt();
			xmlReadingLog+="    Min gaps between building changes="+CustomFETString::number(cn->minGapsBetweenBuildingChanges)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

SpaceConstraint* Rules::readTeachersMinGapsBetweenBuildingChanges(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeachersMinGapsBetweenBuildingChanges"));
	ConstraintTeachersMinGapsBetweenBuildingChanges* cn=new ConstraintTeachersMinGapsBetweenBuildingChanges();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Min_Gaps_Between_Building_Changes")){
			QString text=xmlReader.readElementText();
			cn->minGapsBetweenBuildingChanges=text.toInt();
			xmlReadingLog+="    Min gaps between building changes="+CustomFETString::number(cn->minGapsBetweenBuildingChanges)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

SpaceConstraint* Rules::readStudentsSetMaxBuildingChangesPerDay(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsSetMaxBuildingChangesPerDay"));
	ConstraintStudentsSetMaxBuildingChangesPerDay* cn=new ConstraintStudentsSetMaxBuildingChangesPerDay();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Students")){
			QString text=xmlReader.readElementText();
			cn->studentsName=text;
			xmlReadingLog+="    Read students name="+cn->studentsName+"\n";
		}
		else if(xmlReader.name()==QString("Max_Building_Changes_Per_Day")){
			QString text=xmlReader.readElementText();
			cn->maxBuildingChangesPerDay=text.toInt();
			xmlReadingLog+="    Max. building changes per day="+CustomFETString::number(cn->maxBuildingChangesPerDay)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

SpaceConstraint* Rules::readStudentsMaxBuildingChangesPerDay(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsMaxBuildingChangesPerDay"));
	ConstraintStudentsMaxBuildingChangesPerDay* cn=new ConstraintStudentsMaxBuildingChangesPerDay();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Max_Building_Changes_Per_Day")){
			QString text=xmlReader.readElementText();
			cn->maxBuildingChangesPerDay=text.toInt();
			xmlReadingLog+="    Max. building changes per day="+CustomFETString::number(cn->maxBuildingChangesPerDay)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

SpaceConstraint* Rules::readStudentsSetMaxBuildingChangesPerWeek(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsSetMaxBuildingChangesPerWeek"));
	ConstraintStudentsSetMaxBuildingChangesPerWeek* cn=new ConstraintStudentsSetMaxBuildingChangesPerWeek();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Students")){
			QString text=xmlReader.readElementText();
			cn->studentsName=text;
			xmlReadingLog+="    Read students name="+cn->studentsName+"\n";
		}
		else if(xmlReader.name()==QString("Max_Building_Changes_Per_Week")){
			QString text=xmlReader.readElementText();
			cn->maxBuildingChangesPerWeek=text.toInt();
			xmlReadingLog+="    Max. building changes per week="+CustomFETString::number(cn->maxBuildingChangesPerWeek)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

SpaceConstraint* Rules::readStudentsMaxBuildingChangesPerWeek(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsMaxBuildingChangesPerWeek"));
	ConstraintStudentsMaxBuildingChangesPerWeek* cn=new ConstraintStudentsMaxBuildingChangesPerWeek();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Max_Building_Changes_Per_Week")){
			QString text=xmlReader.readElementText();
			cn->maxBuildingChangesPerWeek=text.toInt();
			xmlReadingLog+="    Max. building changes per week="+CustomFETString::number(cn->maxBuildingChangesPerWeek)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

SpaceConstraint* Rules::readStudentsSetMinGapsBetweenBuildingChanges(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsSetMinGapsBetweenBuildingChanges"));
	ConstraintStudentsSetMinGapsBetweenBuildingChanges* cn=new ConstraintStudentsSetMinGapsBetweenBuildingChanges();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Students")){
			QString text=xmlReader.readElementText();
			cn->studentsName=text;
			xmlReadingLog+="    Read students name="+cn->studentsName+"\n";
		}
		else if(xmlReader.name()==QString("Min_Gaps_Between_Building_Changes")){
			QString text=xmlReader.readElementText();
			cn->minGapsBetweenBuildingChanges=text.toInt();
			xmlReadingLog+="    min gaps between building changes="+CustomFETString::number(cn->minGapsBetweenBuildingChanges)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

SpaceConstraint* Rules::readStudentsMinGapsBetweenBuildingChanges(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsMinGapsBetweenBuildingChanges"));
	ConstraintStudentsMinGapsBetweenBuildingChanges* cn=new ConstraintStudentsMinGapsBetweenBuildingChanges();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Min_Gaps_Between_Building_Changes")){
			QString text=xmlReader.readElementText();
			cn->minGapsBetweenBuildingChanges=text.toInt();
			xmlReadingLog+="    min gaps between building changes="+CustomFETString::number(cn->minGapsBetweenBuildingChanges)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

//2019-11-14
SpaceConstraint* Rules::readTeacherMaxRoomChangesPerDay(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeacherMaxRoomChangesPerDay"));
	ConstraintTeacherMaxRoomChangesPerDay* cn=new ConstraintTeacherMaxRoomChangesPerDay();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Teacher")){
			QString text=xmlReader.readElementText();
			cn->teacherName=text;
			xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
		}
		else if(xmlReader.name()==QString("Max_Room_Changes_Per_Day")){
			QString text=xmlReader.readElementText();
			cn->maxRoomChangesPerDay=text.toInt();
			xmlReadingLog+="    Max. room changes per day="+CustomFETString::number(cn->maxRoomChangesPerDay)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

SpaceConstraint* Rules::readTeachersMaxRoomChangesPerDay(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeachersMaxRoomChangesPerDay"));
	ConstraintTeachersMaxRoomChangesPerDay* cn=new ConstraintTeachersMaxRoomChangesPerDay();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Max_Room_Changes_Per_Day")){
			QString text=xmlReader.readElementText();
			cn->maxRoomChangesPerDay=text.toInt();
			xmlReadingLog+="    Max. room changes per day="+CustomFETString::number(cn->maxRoomChangesPerDay)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

SpaceConstraint* Rules::readTeacherMaxRoomChangesPerWeek(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeacherMaxRoomChangesPerWeek"));
	ConstraintTeacherMaxRoomChangesPerWeek* cn=new ConstraintTeacherMaxRoomChangesPerWeek();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Teacher")){
			QString text=xmlReader.readElementText();
			cn->teacherName=text;
			xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
		}
		else if(xmlReader.name()==QString("Max_Room_Changes_Per_Week")){
			QString text=xmlReader.readElementText();
			cn->maxRoomChangesPerWeek=text.toInt();
			xmlReadingLog+="    Max. room changes per week="+CustomFETString::number(cn->maxRoomChangesPerWeek)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

SpaceConstraint* Rules::readTeachersMaxRoomChangesPerWeek(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeachersMaxRoomChangesPerWeek"));
	ConstraintTeachersMaxRoomChangesPerWeek* cn=new ConstraintTeachersMaxRoomChangesPerWeek();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Max_Room_Changes_Per_Week")){
			QString text=xmlReader.readElementText();
			cn->maxRoomChangesPerWeek=text.toInt();
			xmlReadingLog+="    Max. room changes per week="+CustomFETString::number(cn->maxRoomChangesPerWeek)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

SpaceConstraint* Rules::readTeacherMinGapsBetweenRoomChanges(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeacherMinGapsBetweenRoomChanges"));
	ConstraintTeacherMinGapsBetweenRoomChanges* cn=new ConstraintTeacherMinGapsBetweenRoomChanges();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Teacher")){
			QString text=xmlReader.readElementText();
			cn->teacherName=text;
			xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
		}
		else if(xmlReader.name()==QString("Min_Gaps_Between_Room_Changes")){
			QString text=xmlReader.readElementText();
			cn->minGapsBetweenRoomChanges=text.toInt();
			xmlReadingLog+="    Min gaps between room changes="+CustomFETString::number(cn->minGapsBetweenRoomChanges)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

SpaceConstraint* Rules::readTeachersMinGapsBetweenRoomChanges(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintTeachersMinGapsBetweenRoomChanges"));
	ConstraintTeachersMinGapsBetweenRoomChanges* cn=new ConstraintTeachersMinGapsBetweenRoomChanges();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Min_Gaps_Between_Room_Changes")){
			QString text=xmlReader.readElementText();
			cn->minGapsBetweenRoomChanges=text.toInt();
			xmlReadingLog+="    Min gaps between room changes="+CustomFETString::number(cn->minGapsBetweenRoomChanges)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

SpaceConstraint* Rules::readStudentsSetMaxRoomChangesPerDay(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsSetMaxRoomChangesPerDay"));
	ConstraintStudentsSetMaxRoomChangesPerDay* cn=new ConstraintStudentsSetMaxRoomChangesPerDay();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Students")){
			QString text=xmlReader.readElementText();
			cn->studentsName=text;
			xmlReadingLog+="    Read students name="+cn->studentsName+"\n";
		}
		else if(xmlReader.name()==QString("Max_Room_Changes_Per_Day")){
			QString text=xmlReader.readElementText();
			cn->maxRoomChangesPerDay=text.toInt();
			xmlReadingLog+="    Max. room changes per day="+CustomFETString::number(cn->maxRoomChangesPerDay)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

SpaceConstraint* Rules::readStudentsMaxRoomChangesPerDay(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsMaxRoomChangesPerDay"));
	ConstraintStudentsMaxRoomChangesPerDay* cn=new ConstraintStudentsMaxRoomChangesPerDay();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Max_Room_Changes_Per_Day")){
			QString text=xmlReader.readElementText();
			cn->maxRoomChangesPerDay=text.toInt();
			xmlReadingLog+="    Max. room changes per day="+CustomFETString::number(cn->maxRoomChangesPerDay)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

SpaceConstraint* Rules::readStudentsSetMaxRoomChangesPerWeek(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsSetMaxRoomChangesPerWeek"));
	ConstraintStudentsSetMaxRoomChangesPerWeek* cn=new ConstraintStudentsSetMaxRoomChangesPerWeek();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Students")){
			QString text=xmlReader.readElementText();
			cn->studentsName=text;
			xmlReadingLog+="    Read students name="+cn->studentsName+"\n";
		}
		else if(xmlReader.name()==QString("Max_Room_Changes_Per_Week")){
			QString text=xmlReader.readElementText();
			cn->maxRoomChangesPerWeek=text.toInt();
			xmlReadingLog+="    Max. room changes per week="+CustomFETString::number(cn->maxRoomChangesPerWeek)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

SpaceConstraint* Rules::readStudentsMaxRoomChangesPerWeek(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsMaxRoomChangesPerWeek"));
	ConstraintStudentsMaxRoomChangesPerWeek* cn=new ConstraintStudentsMaxRoomChangesPerWeek();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Max_Room_Changes_Per_Week")){
			QString text=xmlReader.readElementText();
			cn->maxRoomChangesPerWeek=text.toInt();
			xmlReadingLog+="    Max. room changes per week="+CustomFETString::number(cn->maxRoomChangesPerWeek)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

SpaceConstraint* Rules::readStudentsSetMinGapsBetweenRoomChanges(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsSetMinGapsBetweenRoomChanges"));
	ConstraintStudentsSetMinGapsBetweenRoomChanges* cn=new ConstraintStudentsSetMinGapsBetweenRoomChanges();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Students")){
			QString text=xmlReader.readElementText();
			cn->studentsName=text;
			xmlReadingLog+="    Read students name="+cn->studentsName+"\n";
		}
		else if(xmlReader.name()==QString("Min_Gaps_Between_Room_Changes")){
			QString text=xmlReader.readElementText();
			cn->minGapsBetweenRoomChanges=text.toInt();
			xmlReadingLog+="    min gaps between room changes="+CustomFETString::number(cn->minGapsBetweenRoomChanges)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

SpaceConstraint* Rules::readStudentsMinGapsBetweenRoomChanges(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintStudentsMinGapsBetweenRoomChanges"));
	ConstraintStudentsMinGapsBetweenRoomChanges* cn=new ConstraintStudentsMinGapsBetweenRoomChanges();
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";
		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Min_Gaps_Between_Room_Changes")){
			QString text=xmlReader.readElementText();
			cn->minGapsBetweenRoomChanges=text.toInt();
			xmlReadingLog+="    min gaps between room changes="+CustomFETString::number(cn->minGapsBetweenRoomChanges)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	return cn;
}

//2012-04-29
SpaceConstraint* Rules::readActivitiesOccupyMaxDifferentRooms(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintActivitiesOccupyMaxDifferentRooms"));
	ConstraintActivitiesOccupyMaxDifferentRooms* cn=new ConstraintActivitiesOccupyMaxDifferentRooms();
	
	int ac=0;
	
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";

		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Number_of_Activities")){
			QString text=xmlReader.readElementText();
			ac=text.toInt();
			xmlReadingLog+="    Read number of activities="+CustomFETString::number(ac)+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->activitiesIds.append(text.toInt());
			xmlReadingLog+="    Read activity id="+CustomFETString::number(cn->activitiesIds[cn->activitiesIds.count()-1])+"\n";
		}
		else if(xmlReader.name()==QString("Max_Number_of_Different_Rooms")){
			QString text=xmlReader.readElementText();
			cn->maxDifferentRooms=text.toInt();
			xmlReadingLog+="    Read max number of different rooms="+CustomFETString::number(cn->maxDifferentRooms)+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	
	if(!(ac==cn->activitiesIds.count())){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Activities").arg("Activity_Id"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(ac==cn->activitiesIds.count());
	
	return cn;
}

////////////////

//2013-09-14
SpaceConstraint* Rules::readActivitiesSameRoomIfConsecutive(QXmlStreamReader& xmlReader, FakeString& xmlReadingLog){
	assert(xmlReader.isStartElement() && xmlReader.name()==QString("ConstraintActivitiesSameRoomIfConsecutive"));
	ConstraintActivitiesSameRoomIfConsecutive* cn=new ConstraintActivitiesSameRoomIfConsecutive();
	
	int ac=0;
	
	while(xmlReader.readNextStartElement()){
		xmlReadingLog+="    Found "+xmlReader.name().toString()+" tag\n";

		if(xmlReader.name()==QString("Weight_Percentage")){
			QString text=xmlReader.readElementText();
			cn->weightPercentage=customFETStrToDouble(text);
			xmlReadingLog+="    Adding weight percentage="+CustomFETString::number(cn->weightPercentage)+"\n";
		}
		else if(xmlReader.name()==QString("Active")){
			QString text=xmlReader.readElementText();
			if(text=="false"){
				cn->active=false;
			}
		}
		else if(xmlReader.name()==QString("Comments")){
			QString text=xmlReader.readElementText();
			cn->comments=text;
		}
		else if(xmlReader.name()==QString("Number_of_Activities")){
			QString text=xmlReader.readElementText();
			ac=text.toInt();
			xmlReadingLog+="    Read number of activities="+CustomFETString::number(ac)+"\n";
		}
		else if(xmlReader.name()==QString("Activity_Id")){
			QString text=xmlReader.readElementText();
			cn->activitiesIds.append(text.toInt());
			xmlReadingLog+="    Read activity id="+CustomFETString::number(cn->activitiesIds[cn->activitiesIds.count()-1])+"\n";
		}
		else{
			unrecognizedXmlTags.append(xmlReader.name().toString());
			unrecognizedXmlLineNumbers.append(xmlReader.lineNumber());
			unrecognizedXmlColumnNumbers.append(xmlReader.columnNumber());

			xmlReader.skipCurrentElement();
			xmlReaderNumberOfUnrecognizedFields++;
		}
	}
	
	if(!(ac==cn->activitiesIds.count())){
		xmlReader.raiseError(tr("%1 does not coincide with the number of read %2").arg("Number_of_Activities").arg("Activity_Id"));
		delete cn;
		cn=nullptr;
		return nullptr;
	}
	assert(ac==cn->activitiesIds.count());
	
	return cn;
}

////////////////
