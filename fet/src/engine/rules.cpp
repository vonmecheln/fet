/*
File rules.cpp
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
along with FET; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "timetable_defs.h"
#include "rules.h"

#include <string.h>

#include <iostream>
//Added by qt3to4:
#include <QTextStream>
using namespace std;

#include <qdom.h>
#include <qstring.h>
#include <qtranslator.h>
#include <qmessagebox.h>

#include <QtAlgorithms>

void Rules::init() //initializes the rules (empty, but with default hours and days)
{
	//defaults
	this->nHoursPerDay=12;
	this->hoursOfTheDay[0]="08:00";
	this->hoursOfTheDay[1]="09:00";
	this->hoursOfTheDay[2]="10:00";
	this->hoursOfTheDay[3]="11:00";
	this->hoursOfTheDay[4]="12:00";
	this->hoursOfTheDay[5]="13:00";
	this->hoursOfTheDay[6]="14:00";
	this->hoursOfTheDay[7]="15:00";
	this->hoursOfTheDay[8]="16:00";
	this->hoursOfTheDay[9]="17:00";
	this->hoursOfTheDay[10]="18:00";
	this->hoursOfTheDay[11]="19:00";
	this->hoursOfTheDay[12]="20:00";

	this->nDaysPerWeek=5;
	this->daysOfTheWeek[0] = QObject::tr("Monday");
	this->daysOfTheWeek[1] = QObject::tr("Tuesday");
	this->daysOfTheWeek[2] = QObject::tr("Wednesday");
	this->daysOfTheWeek[3] = QObject::tr("Thursday");
	this->daysOfTheWeek[4] = QObject::tr("Friday");
	
	this->institutionName=QObject::tr("Default institution");
	this->comments=QObject::tr("Default comments");

	this->initialized=true;
}

bool Rules::computeInternalStructure()
{
	//The order is important - firstly the teachers, subjects and students.
	//After that, the rooms.
	//After that, the activities.
	//After that, the time constraints.
	//After that, the space constraints.

	if(this->teachersList.size()>MAX_TEACHERS){
		QMessageBox::warning(NULL, QObject::tr("FET information"),
		 QObject::tr("You have too many teachers.\nPlease talk to the author or increase variable MAX_TEACHERS"));
		return false;
	}
	if(this->subjectsList.size()>MAX_SUBJECTS){
		QMessageBox::warning(NULL, QObject::tr("FET information"),
		 QObject::tr("You have too many subjects.\nPlease talk to the author or increase variable MAX_SUBJECTS"));
		return false;
	}

	int tmpNSubgroups=0;
	if(this->yearsList.size()>MAX_YEARS){
		QMessageBox::warning(NULL, QObject::tr("FET information"),
		 QObject::tr("You have too many years.\nPlease talk to the author or increase variable MAX_YEARS"));
		return false;
	}
	for(int i=0; i<this->yearsList.size(); i++){
		StudentsYear* sty=this->yearsList.at(i);

		if(sty->groupsList.size()>MAX_GROUPS_PER_YEAR){
			QMessageBox::warning(NULL, QObject::tr("FET information"),
			 QObject::tr("You have too many groups per year.\nPlease talk to the author or increase variable MAX_GROUPS_PER_YEAR"));
			return false;
		}
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList.at(j);

			if(stg->subgroupsList.size()>MAX_SUBGROUPS_PER_GROUP){
				QMessageBox::warning(NULL, QObject::tr("FET information"),
				 QObject::tr("You have too many subgroups per group.\nPlease talk to the author or increase variable MAX_SUBGROUPS_PER_GROUP"));
				return false;
			}
			tmpNSubgroups+=stg->subgroupsList.size();
		}
	}
	if(tmpNSubgroups>MAX_TOTAL_SUBGROUPS){
		QMessageBox::warning(NULL, QObject::tr("FET information"),
		 QObject::tr("You have too many total subgroups.\nPlease talk to the author or increase variable MAX_TOTAL_SUBGROUPS"));
		return false;
	}

	int counter=0;
	for(int i=0; i<this->activitiesList.size(); i++){
		Activity* act=this->activitiesList.at(i);
		if(act->active)
			counter++;
	}
	if(counter>MAX_ACTIVITIES){
		QMessageBox::warning(NULL, QObject::tr("FET information"),
		 QObject::tr("You have too many active activities.\nPlease talk to the author or increase variable MAX_ACTIVITIES"));
		return false;
	}

	if(this->roomsList.size()>MAX_ROOMS){
		QMessageBox::warning(NULL, QObject::tr("FET information"),
		 QObject::tr("You have too many rooms.\nPlease talk to the author or increase variable MAX_ROOMS"));
		return false;
	}
	
	if(this->timeConstraintsList.size()>MAX_TIME_CONSTRAINTS){
		QMessageBox::warning(NULL, QObject::tr("FET information"),
		 QObject::tr("You have too many time constraints.\nPlease talk to the author or increase variable MAX_TIME_CONSTRAINTS"));
		return false;
	}

	if(this->spaceConstraintsList.size()>MAX_SPACE_CONSTRAINTS){
		QMessageBox::warning(NULL, QObject::tr("FET information"),
		 QObject::tr("You have too many space constraints.\nPlease talk to the author or increase variable MAX_SPACE_CONSTRAINTS"));
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
	for(i=0; i<this->teachersList.size(); i++){
		tch=teachersList[i];
		this->internalTeachersList[i]=tch;
	}
	assert(i==this->nInternalTeachers);

	//subjects
	Subject* sbj;
	this->nInternalSubjects=this->subjectsList.size();
	assert(this->nInternalSubjects<=MAX_SUBJECTS);	
	for(i=0; i<this->subjectsList.size(); i++){
		sbj=this->subjectsList[i];
		this->internalSubjectsList[i]=sbj;
	}
	assert(i==this->nInternalSubjects);

	//students
	this->nInternalSubgroups=0;
	assert(this->yearsList.size()<=MAX_YEARS);
	for(int i=0; i<this->yearsList.size(); i++){
		StudentsYear* sty=this->yearsList[i];

		//if this year has no groups, insert something to simulate the whole year
		if(sty->groupsList.count()==0){
			StudentsGroup* tmpGroup = new StudentsGroup();
			tmpGroup->name = sty->name+" WHOLE YEAR";
			tmpGroup->numberOfStudents = sty->numberOfStudents;
			sty->groupsList << tmpGroup;
		}
		
		assert(sty->groupsList.size()<=MAX_GROUPS_PER_YEAR);
		
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];

			//if this group has no subgroups, insert something to simulate the whole group
			if(stg->subgroupsList.size()==0){
				StudentsSubgroup* tmpSubgroup = new StudentsSubgroup();
				tmpSubgroup->name = stg->name+" WHOLE GROUP";
				tmpSubgroup->numberOfStudents=stg->numberOfStudents;
				stg->subgroupsList << tmpSubgroup;
			}
			
			assert(stg->subgroupsList.size()<=MAX_SUBGROUPS_PER_GROUP);
			
			for(int k=0; k<stg->subgroupsList.size(); k++){
				StudentsSubgroup* sts=stg->subgroupsList[k];

				bool existing=false;
				for(int i=0; i<this->nInternalSubgroups; i++)
					if(this->internalSubgroupsList[i]->name==sts->name){
						existing=true;
						break;
					}
				if(!existing){
					assert(this->nInternalSubgroups<MAX_TOTAL_SUBGROUPS);
					this->internalSubgroupsList[this->nInternalSubgroups++]=sts;
				}
			}
		}
	}

	//rooms
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

	//activities
	Activity* act;
	counter=0;
	for(int i=0; i<this->activitiesList.size(); i++){
		act=this->activitiesList[i];
		if(act->active){
			counter++;
			act->computeInternalStructure(*this);
		}
	}

	assert(counter<=MAX_ACTIVITIES);
	this->nInternalActivities=counter;
	int activei=0;
	for(int ai=0; ai<this->activitiesList.size(); ai++){
		act=this->activitiesList[ai];
		if(act->active)
			this->internalActivitiesList[activei++]=*act;
	}

	bool ok=true;

	//time constraints
	assert(this->timeConstraintsList.size()<=MAX_TIME_CONSTRAINTS);
	TimeConstraint* tctr;
	
	int tctri=0;
	for(int tctrindex=0; tctrindex<this->timeConstraintsList.size(); tctrindex++){
		tctr=this->timeConstraintsList[tctrindex];
		//if the activities which refer to this constraints are not active
		if(!tctr->computeInternalStructure(*this)){
			//assert(0);
			ok=false;
			continue;
		}
		this->internalTimeConstraintsList[tctri++]=tctr;
	}

	this->nInternalTimeConstraints=tctri;
	assert(this->nInternalTimeConstraints<=MAX_TIME_CONSTRAINTS);
	
	//space constraints
	SpaceConstraint* sctr;
	assert(this->spaceConstraintsList.size()<=MAX_SPACE_CONSTRAINTS);

	int sctri=0;
	for(int sctrindex=0; sctrindex<this->spaceConstraintsList.size(); sctrindex++){
		sctr=this->spaceConstraintsList[sctrindex];
	
		if(!sctr->computeInternalStructure(*this)){
			//assert(0);
			ok=false;
			continue;
		}
		this->internalSpaceConstraintsList[sctri++]=sctr;
	}
	this->nInternalSpaceConstraints=sctri;
	assert(this->nInternalSpaceConstraints<=MAX_SPACE_CONSTRAINTS);

	//done.
	this->internalStructureComputed=ok;
	
	return ok;
}

void Rules::kill() //clears memory for the rules, destroyes them
{
	//Teachers
	while(!teachersList.isEmpty())
		delete teachersList.takeFirst();

	//Subjects
	while(!subjectsList.isEmpty())
		delete subjectsList.takeFirst();

	//Subject tags
	while(!subjectTagsList.isEmpty())
		delete subjectTagsList.takeFirst();



	//Years
	/*while(!yearsList.isEmpty())
		delete yearsList.takeFirst();*/
		
	//students sets
	QList<StudentsYear*> years;
	QList<StudentsGroup*> groups;
	QList<StudentsSubgroup*> subgroups;
	foreach(StudentsYear* year, yearsList){
		if(!years.contains(year))
			years.append(year);
		foreach(StudentsGroup* group, year->groupsList){
			if(!groups.contains(group))
				groups.append(group);
			foreach(StudentsSubgroup* subgroup, group->subgroupsList){
				if(!subgroups.contains(subgroup))
					subgroups.append(subgroup);
			}
		}
	}
	foreach(StudentsYear* year, years){
		assert(year!=NULL);
		delete year;
	}
	foreach(StudentsGroup* group, groups){
		assert(group!=NULL);
		delete group;
	}
	foreach(StudentsSubgroup* subgroup, subgroups){
		assert(subgroup!=NULL);
		delete subgroup;
	}
	
	yearsList.clear();
		
		
		

	//Activities
	while(!activitiesList.isEmpty())
		delete activitiesList.takeFirst();

	//Time constraints
	while(!timeConstraintsList.isEmpty())
		delete timeConstraintsList.takeFirst();

	//Space constraints
	while(!spaceConstraintsList.isEmpty())
		delete spaceConstraintsList.takeFirst();

	//Rooms
	while(!roomsList.isEmpty())
		delete roomsList.takeFirst();

	//done
	this->internalStructureComputed=false;
	this->initialized=false;
}

Rules::Rules()
{
	this->initialized=false;
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
}

void Rules::setComments(const QString& newComments)
{
	this->comments=newComments;
	this->internalStructureComputed=false;
}

bool Rules::addTeacher(Teacher* teacher)
{
	for(int i=0; i<this->teachersList.size(); i++){
		Teacher* tch=this->teachersList[i];
		if(tch->name==teacher->name)
			return false;
	}
	
	this->internalStructureComputed=false;
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
	for(int i=0; i<this->activitiesList.size(); ){
		Activity* act=this->activitiesList[i];
		bool t=act->removeTeacher(teacherName);
		if(t && act->teachersNames.size()==0){
			this->removeActivity(act->id, act->activityGroupId);
			i=0;
			//(You have to be careful, there can be erased more activities here)
		}
		else
			i++;
	}
	
	for(int i=0; i<this->timeConstraintsList.size(); ){
		TimeConstraint* ctr=this->timeConstraintsList[i];
		if(ctr->type==CONSTRAINT_TEACHER_NOT_AVAILABLE){
			ConstraintTeacherNotAvailable* crt_constraint=(ConstraintTeacherNotAvailable*)ctr;
			if(teacherName==crt_constraint->teacherName)
				this->removeTimeConstraint(ctr); //single constraint removal
			else
				i++;
		}
		else
			i++;
	}

	for(int i=0; i<this->timeConstraintsList.size(); ){
		TimeConstraint* ctr=this->timeConstraintsList[i];
		if(ctr->type==CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK){
			ConstraintTeacherMaxGapsPerWeek* crt_constraint=(ConstraintTeacherMaxGapsPerWeek*)ctr;
			if(teacherName==crt_constraint->teacherName)
				this->removeTimeConstraint(ctr); //single constraint removal
			else
				i++;
		}
		else
			i++;
	}

	for(int i=0; i<this->timeConstraintsList.size(); ){
		TimeConstraint* ctr=this->timeConstraintsList[i];
		if(ctr->type==CONSTRAINT_TEACHER_MAX_HOURS_DAILY){
			ConstraintTeacherMaxHoursDaily* crt_constraint=(ConstraintTeacherMaxHoursDaily*)ctr;
			if(teacherName==crt_constraint->teacherName)
				this->removeTimeConstraint(ctr); //single constraint removal
			else
				i++;
		}
		else
			i++;
	}

	for(int i=0; i<this->timeConstraintsList.size(); ){
		TimeConstraint* ctr=this->timeConstraintsList[i];
		if(ctr->type==CONSTRAINT_TEACHER_MIN_HOURS_DAILY){
			ConstraintTeacherMinHoursDaily* crt_constraint=(ConstraintTeacherMinHoursDaily*)ctr;
			if(teacherName==crt_constraint->teacherName)
				this->removeTimeConstraint(ctr); //single constraint removal
			else
				i++;
		}
		else
			i++;
	}

	for(int i=0; i<this->timeConstraintsList.size(); ){
		TimeConstraint* ctr=this->timeConstraintsList[i];
		if(ctr->type==CONSTRAINT_TEACHER_MAX_DAYS_PER_WEEK){
			ConstraintTeacherMaxDaysPerWeek* crt_constraint=(ConstraintTeacherMaxDaysPerWeek*)ctr;
			if(teacherName==crt_constraint->teacherName)
				this->removeTimeConstraint(ctr); //single constraint removal
			else
				i++;
		}
		else
			i++;
	}

	for(int i=0; i<this->timeConstraintsList.size(); ){
		TimeConstraint* ctr=this->timeConstraintsList[i];
		if(ctr->type==CONSTRAINT_ACTIVITIES_PREFERRED_TIMES){
			ConstraintActivitiesPreferredTimes* crt_constraint=(ConstraintActivitiesPreferredTimes*)ctr;
			if(teacherName==crt_constraint->teacherName)
				this->removeTimeConstraint(ctr); //single constraint removal
			else
				i++;
		}
		else
			i++;
	}


	for(int i=0; i<this->teachersList.size(); i++)
		if(this->teachersList.at(i)->name==teacherName){
			Teacher* tch=this->teachersList[i];
			this->teachersList.removeAt(i);
			delete tch;
			break;
		}
	
	this->internalStructureComputed=false;

	return true;
}

bool Rules::modifyTeacher(const QString& initialTeacherName, const QString& finalTeacherName)
{
	assert(this->searchTeacher(finalTeacherName)==-1);
	assert(this->searchTeacher(initialTeacherName)>=0);

	//TODO: improve this part
	for(int i=0; i<this->activitiesList.size(); i++)
		this->activitiesList.at(i)->renameTeacher(initialTeacherName, finalTeacherName);

	for(int i=0; i<this->timeConstraintsList.size(); i++){
		TimeConstraint* ctr=this->timeConstraintsList[i];	

		if(ctr->type==CONSTRAINT_TEACHER_NOT_AVAILABLE){
			ConstraintTeacherNotAvailable* crt_constraint=(ConstraintTeacherNotAvailable*)ctr;
			if(initialTeacherName == crt_constraint->teacherName)
				crt_constraint->teacherName=finalTeacherName;
		}
	}

	for(int i=0; i<this->timeConstraintsList.size(); i++){
		TimeConstraint* ctr=this->timeConstraintsList[i];	

		if(ctr->type==CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK){
			ConstraintTeacherMaxGapsPerWeek* crt_constraint=(ConstraintTeacherMaxGapsPerWeek*)ctr;
			if(initialTeacherName == crt_constraint->teacherName)
				crt_constraint->teacherName=finalTeacherName;
		}
	}

	for(int i=0; i<this->timeConstraintsList.size(); i++){
		TimeConstraint* ctr=this->timeConstraintsList[i];	

		if(ctr->type==CONSTRAINT_TEACHER_MAX_HOURS_DAILY){
			ConstraintTeacherMaxHoursDaily* crt_constraint=(ConstraintTeacherMaxHoursDaily*)ctr;
			if(initialTeacherName == crt_constraint->teacherName)
				crt_constraint->teacherName=finalTeacherName;
		}
	}

	for(int i=0; i<this->timeConstraintsList.size(); i++){
		TimeConstraint* ctr=this->timeConstraintsList[i];	

		if(ctr->type==CONSTRAINT_TEACHER_MIN_HOURS_DAILY){
			ConstraintTeacherMinHoursDaily* crt_constraint=(ConstraintTeacherMinHoursDaily*)ctr;
			if(initialTeacherName == crt_constraint->teacherName)
				crt_constraint->teacherName=finalTeacherName;
		}
	}

	for(int i=0; i<this->timeConstraintsList.size(); i++){
		TimeConstraint* ctr=this->timeConstraintsList[i];	

		if(ctr->type==CONSTRAINT_TEACHER_MAX_DAYS_PER_WEEK){
			ConstraintTeacherMaxDaysPerWeek* crt_constraint=(ConstraintTeacherMaxDaysPerWeek*)ctr;
			if(initialTeacherName == crt_constraint->teacherName)
				crt_constraint->teacherName=finalTeacherName;
		}
	}
	
	for(int i=0; i<this->timeConstraintsList.size(); i++){
		TimeConstraint* ctr=this->timeConstraintsList[i];	

		if(ctr->type==CONSTRAINT_ACTIVITIES_PREFERRED_TIMES){
			ConstraintActivitiesPreferredTimes* crt_constraint=(ConstraintActivitiesPreferredTimes*)ctr;
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
	assert(t<=1);

	this->internalStructureComputed=false;

	if(t==0)
		return false;
	else
		return true;
}

void Rules::sortTeachersAlphabetically()
{
	qSort(this->teachersList.begin(), this->teachersList.end(), teachersAscending);

	this->internalStructureComputed=false;
}

bool Rules::addSubject(Subject* subject)
{
	for(int i=0; i<this->subjectsList.size(); i++){
		Subject* sbj=this->subjectsList[i];	
		if(sbj->name==subject->name)
			return false;
	}
	
	this->internalStructureComputed=false;
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
	//check the activities first
	for(int i=0; i<this->activitiesList.size(); ){
		Activity* act=this->activitiesList[i];
		if(act->subjectName==subjectName){
			this->removeActivity(act->id, act->activityGroupId);
			i=0;
			//(You have to be careful, there can be erased more activities here)
		}
		else
			i++;
	}
	
	//delete the time constraints related to this subject

	for(int i=0; i<this->timeConstraintsList.size(); ){
		TimeConstraint* ctr=this->timeConstraintsList[i];
		if(ctr->type==CONSTRAINT_ACTIVITIES_PREFERRED_TIMES){
			ConstraintActivitiesPreferredTimes* crt_constraint=(ConstraintActivitiesPreferredTimes*)ctr;
			if(subjectName==crt_constraint->subjectName)
				this->removeTimeConstraint(ctr); //single constraint removal
			else
				i++;
		}
		else
			i++;
	}


	//delete the space constraints related to this subject
	for(int i=0; i<this->spaceConstraintsList.size(); ){
		SpaceConstraint* ctr=this->spaceConstraintsList[i];

		if(ctr->type==CONSTRAINT_SUBJECT_PREFERRED_ROOM){
			ConstraintSubjectPreferredRoom* c=(ConstraintSubjectPreferredRoom*)ctr;

			if(c->subjectName == subjectName)
				this->removeSpaceConstraint(ctr);
			else
				i++;
		}
		else if(ctr->type==CONSTRAINT_SUBJECT_PREFERRED_ROOMS){
			ConstraintSubjectPreferredRooms* c=(ConstraintSubjectPreferredRooms*)ctr;

			if(c->subjectName == subjectName)
				this->removeSpaceConstraint(ctr);
			else
				i++;
		}
		else if(ctr->type==CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOM){
			ConstraintSubjectSubjectTagPreferredRoom* c=(ConstraintSubjectSubjectTagPreferredRoom*)ctr;

			if(c->subjectName == subjectName)
				this->removeSpaceConstraint(ctr);
			else
				i++;
		}
		else if(ctr->type==CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOMS){
			ConstraintSubjectSubjectTagPreferredRooms* c=(ConstraintSubjectSubjectTagPreferredRooms*)ctr;

			if(c->subjectName == subjectName)
				this->removeSpaceConstraint(ctr);
			else
				i++;
		}
		else
			i++;
	}

	//remove the subject from the list
	for(int i=0; i<this->subjectsList.size(); i++)
		if(this->subjectsList[i]->name==subjectName){
			Subject* sbj=this->subjectsList[i];
			this->subjectsList.removeAt(i);
			delete sbj;
			break;
		}
	
	this->internalStructureComputed=false;

	return true;
}

bool Rules::modifySubject(const QString& initialSubjectName, const QString& finalSubjectName)
{
	assert(this->searchSubject(finalSubjectName)==-1);
	assert(this->searchSubject(initialSubjectName)>=0);

	//check the activities first
	for(int i=0; i<this->activitiesList.size(); i++){
		Activity* act=this->activitiesList[i];

		if( act->subjectName == initialSubjectName)
			act->subjectName=finalSubjectName;
	}
	
	//modify the time constraints related to this subject
	for(int i=0; i<this->timeConstraintsList.size(); i++){
		TimeConstraint* ctr=this->timeConstraintsList[i];

		if(ctr->type==CONSTRAINT_ACTIVITIES_PREFERRED_TIMES){
			ConstraintActivitiesPreferredTimes* crt_constraint=(ConstraintActivitiesPreferredTimes*)ctr;
			if(initialSubjectName == crt_constraint->subjectName)
				crt_constraint->subjectName=finalSubjectName;
		}
	}

	//modify the space constraints related to this subject
	for(int i=0; i<this->spaceConstraintsList.size(); i++){
		SpaceConstraint* ctr=this->spaceConstraintsList[i];

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
		else if(ctr->type==CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOM){
			ConstraintSubjectSubjectTagPreferredRoom* c=(ConstraintSubjectSubjectTagPreferredRoom*)ctr;
			if(c->subjectName == initialSubjectName)
				c->subjectName=finalSubjectName;
		}
		else if(ctr->type==CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOMS){
			ConstraintSubjectSubjectTagPreferredRooms* c=(ConstraintSubjectSubjectTagPreferredRooms*)ctr;
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
	assert(t<=1);

	this->internalStructureComputed=false;

	return true;
}

void Rules::sortSubjectsAlphabetically()
{
	qSort(this->subjectsList.begin(), this->subjectsList.end(), subjectsAscending);

	this->internalStructureComputed=false;
}

bool Rules::addSubjectTag(SubjectTag* subjectTag)
{
	for(int i=0; i<this->subjectTagsList.size(); i++){
		SubjectTag* sbt=this->subjectTagsList[i];

		if(sbt->name==subjectTag->name)
			return false;
	}

	this->internalStructureComputed=false;
	this->subjectTagsList << subjectTag;
	return true;
}

int Rules::searchSubjectTag(const QString& subjectTagName)
{
	for(int i=0; i<this->subjectTagsList.size(); i++)
		if(this->subjectTagsList.at(i)->name==subjectTagName)
			return i;

	return -1;
}

bool Rules::removeSubjectTag(const QString& subjectTagName)
{
	//check the activities first
	for(int i=0; i<this->activitiesList.size(); i++){
		Activity* act=this->activitiesList[i];

		if( act->subjectTagName == subjectTagName)
			act->subjectTagName="";
	}
	
	//delete the time constraints related to this subject tag
	for(int i=0; i<this->timeConstraintsList.size(); ){
		TimeConstraint* ctr=this->timeConstraintsList[i];
		
		if(ctr->type==CONSTRAINT_ACTIVITIES_PREFERRED_TIMES){
			ConstraintActivitiesPreferredTimes* crt_constraint=(ConstraintActivitiesPreferredTimes*)ctr;
			if(subjectTagName == crt_constraint->subjectTagName)
				this->removeTimeConstraint(ctr); //single constraint removal
			else
				i++;
		}
		else
			i++;
	}

	//delete the space constraints related to this subject tag
	for(int i=0; i<this->spaceConstraintsList.size(); ){
		SpaceConstraint* ctr=this->spaceConstraintsList[i];
		
		if(ctr->type==CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOM){
			ConstraintSubjectSubjectTagPreferredRoom* c=(ConstraintSubjectSubjectTagPreferredRoom*)ctr;

			if(c->subjectTagName == subjectTagName)
				this->removeSpaceConstraint(ctr);
			else
				i++;
		}
		else if(ctr->type==CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOMS){
			ConstraintSubjectSubjectTagPreferredRooms* c=(ConstraintSubjectSubjectTagPreferredRooms*)ctr;

			if(c->subjectTagName == subjectTagName)
				this->removeSpaceConstraint(ctr);
			else
				i++;
		}
		else
			i++;
	}

	//remove the subject tag from the list
	for(int i=0; i<this->subjectTagsList.size(); i++)
		if(this->subjectTagsList[i]->name==subjectTagName){
			SubjectTag* sbt=this->subjectTagsList[i];
			this->subjectTagsList.removeAt(i);
			delete sbt;
			break;
		}
	
	this->internalStructureComputed=false;

	return true;
}

bool Rules::modifySubjectTag(const QString& initialSubjectTagName, const QString& finalSubjectTagName)
{
	assert(this->searchSubjectTag(finalSubjectTagName)==-1);
	assert(this->searchSubjectTag(initialSubjectTagName)>=0);

	//check the activities first
	for(int i=0; i<this->activitiesList.size(); i++){
		Activity* act=this->activitiesList[i];

		if( act->subjectTagName == initialSubjectTagName)
			act->subjectTagName=finalSubjectTagName;
	}
	
	//modify the constraints related to this subject tag
	for(int i=0; i<this->timeConstraintsList.size(); i++){
		TimeConstraint* ctr=this->timeConstraintsList[i];
	
		if(ctr->type==CONSTRAINT_ACTIVITIES_PREFERRED_TIMES){
			ConstraintActivitiesPreferredTimes* crt_constraint=(ConstraintActivitiesPreferredTimes*)ctr;
			if(initialSubjectTagName == crt_constraint->subjectTagName)
				crt_constraint->subjectTagName=finalSubjectTagName;
		}
	}

	//modify the space constraints related to this subject tag
	for(int i=0; i<this->spaceConstraintsList.size(); i++){
		SpaceConstraint* ctr=this->spaceConstraintsList[i];

		if(ctr->type==CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOM){
			ConstraintSubjectSubjectTagPreferredRoom* c=(ConstraintSubjectSubjectTagPreferredRoom*)ctr;
			if(c->subjectTagName == initialSubjectTagName)
				c->subjectTagName=finalSubjectTagName;
		}
		else if(ctr->type==CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOMS){
			ConstraintSubjectSubjectTagPreferredRooms* c=(ConstraintSubjectSubjectTagPreferredRooms*)ctr;
			if(c->subjectTagName == initialSubjectTagName)
				c->subjectTagName=finalSubjectTagName;
		}
	}

	//rename the subject tag in the list
	int t=0;
	
	for(int i=0; i<this->subjectTagsList.size(); i++){
		SubjectTag* sbt=this->subjectTagsList[i];

		if(sbt->name==initialSubjectTagName){
			t++;
			sbt->name=finalSubjectTagName;
		}
	}
	
	assert(t<=1);

	this->internalStructureComputed=false;

	return true;
}

void Rules::sortSubjectTagsAlphabetically()
{
	qSort(this->subjectTagsList.begin(), this->subjectTagsList.end(), subjectTagsAscending);

	this->internalStructureComputed=false;
}

bool Rules::studentsSetsRelated(const QString& studentsSet1, const QString& studentsSet2)
{
	StudentsSet* s1=this->searchStudentsSet(studentsSet1);
	StudentsSet* s2=this->searchStudentsSet(studentsSet2);
	assert(s1!=NULL);
	assert(s2!=NULL);
	if(s1->type==STUDENTS_YEAR && s2->type==STUDENTS_YEAR){
		StudentsYear* sy1=(StudentsYear*)s1;
		StudentsYear* sy2=(StudentsYear*)s2;
		if(sy1->name==sy2->name)
			return true;
			
		for(int i=0; i<sy1->groupsList.size(); i++){
			StudentsGroup* sg1=sy1->groupsList[i];
			for(int j=0; j<sy2->groupsList.size(); j++){
				StudentsGroup* sg2=sy2->groupsList[j];
				if(sg1->name==sg2->name)
					return true;
					
				for(int k=0; k<sg1->subgroupsList.size(); k++){
					StudentsSubgroup* ss1=sg1->subgroupsList[k];
					for(int l=0; l<sg2->subgroupsList.size(); l++){
						StudentsSubgroup* ss2=sg2->subgroupsList[l];
						if(ss1->name==ss2->name)
							return true;
					}
				}						
			}
		}
	}
	else if(s1->type==STUDENTS_GROUP && s2->type==STUDENTS_YEAR){
		StudentsGroup* sg1=(StudentsGroup*)s1;
		StudentsYear* sy2=(StudentsYear*)s2;
		
		for(int i=0; i<sy2->groupsList.size(); i++){
			StudentsGroup* sg2=sy2->groupsList[i];
			if(sg1->name==sg2->name)
				return true;
			for(int j=0; j<sg2->subgroupsList.size(); j++){
				StudentsSubgroup* ss2=sg2->subgroupsList[j];
				for(int k=0; k<sg1->subgroupsList.size(); k++){
					StudentsSubgroup* ss1=sg1->subgroupsList[k];
					if(ss1->name==ss2->name)
						return true;
				}
			}						
		}
	}
	else if(s1->type==STUDENTS_SUBGROUP && s2->type==STUDENTS_YEAR){
		StudentsSubgroup* ss1=(StudentsSubgroup*)s1;
		StudentsYear* sy2=(StudentsYear*)s2;
		for(int i=0; i<sy2->groupsList.size(); i++){
			StudentsGroup* sg2=sy2->groupsList[i];
			for(int j=0; j<sg2->subgroupsList.size(); j++){
				StudentsSubgroup* ss2=sg2->subgroupsList[j];
				if(ss1->name==ss2->name)
					return true;
			}
		}						
	}
	else if(s1->type==STUDENTS_YEAR && s2->type==STUDENTS_GROUP){
		StudentsYear* sy1=(StudentsYear*)s1;
		StudentsGroup* sg2=(StudentsGroup*)s2;
		for(int i=0; i<sy1->groupsList.size(); i++){
			StudentsGroup* sg1=sy1->groupsList[i];
			if(sg1->name==sg2->name)
				return true;
			
			for(int j=0; j<sg1->subgroupsList.size(); j++){
				StudentsSubgroup* ss1=sg1->subgroupsList[j];
				for(int k=0; k<sg2->subgroupsList.size(); k++){
					StudentsSubgroup* ss2=sg2->subgroupsList[k];
					if(ss1->name==ss2->name)
						return true;
				}
			}						
		}
	}
	else if(s1->type==STUDENTS_YEAR && s2->type==STUDENTS_SUBGROUP){
		StudentsYear* sy1=(StudentsYear*)s1;
		StudentsSubgroup* ss2=(StudentsSubgroup*)s2;
		for(int i=0; i<sy1->groupsList.size(); i++){
			StudentsGroup* sg1=sy1->groupsList[i];
			for(int k=0; k<sg1->subgroupsList.size(); k++){
				StudentsSubgroup* ss1=sg1->subgroupsList[k];
				if(ss1->name==ss2->name)
					return true;
			}
		}						
	}
	else if(s1->type==STUDENTS_GROUP && s2->type==STUDENTS_GROUP){
		StudentsGroup* sg1=(StudentsGroup*)s1;
		StudentsGroup* sg2=(StudentsGroup*)s2;
		if(sg1->name==sg2->name)
			return true;
			
		for(int i=0; i<sg1->subgroupsList.size(); i++){
			StudentsSubgroup* ss1=sg1->subgroupsList[i];
			for(int j=0; j<sg2->subgroupsList.size(); j++){
				StudentsSubgroup* ss2=sg2->subgroupsList[j];
				if(ss1->name==ss2->name)
					return true;
			}
		}						
	}
	else if(s1->type==STUDENTS_SUBGROUP && s2->type==STUDENTS_GROUP){
		StudentsSubgroup* ss1=(StudentsSubgroup*)s1;
		StudentsGroup* sg2=(StudentsGroup*)s2;
		for(int i=0; i<sg2->subgroupsList.size(); i++){
			StudentsSubgroup* ss2=sg2->subgroupsList[i];
			if(ss1->name==ss2->name)
				return true;
		}
	}						
	else if(s1->type==STUDENTS_GROUP && s2->type==STUDENTS_SUBGROUP){
		StudentsGroup* sg1=(StudentsGroup*)s1;
		StudentsSubgroup* ss2=(StudentsSubgroup*)s2;
		for(int i=0; i<sg1->subgroupsList.size(); i++){
			StudentsSubgroup* ss1=sg1->subgroupsList[i];
			if(ss1->name==ss2->name)
				return true;
		}
	}						
	else if(s1->type==STUDENTS_SUBGROUP && s2->type==STUDENTS_SUBGROUP){
		StudentsSubgroup* ss1=(StudentsSubgroup*)s1;
		StudentsSubgroup* ss2=(StudentsSubgroup*)s2;
		if(ss1->name==ss2->name)
			return true;
	}						
	else 
		assert(0);

	return false;
}

StudentsSet* Rules::searchStudentsSet(const QString& setName)
{
	for(int i=0; i<this->yearsList.size(); i++){
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
	return NULL;
}

bool Rules::addYear(StudentsYear* year)
{
	//already existing?
	if(this->searchStudentsSet(year->name)!=NULL)
		return false;
	this->yearsList << year;
	this->internalStructureComputed=false;
	return true;
}

bool Rules::removeYear(const QString& yearName)
{
	StudentsYear* year=(StudentsYear*)searchStudentsSet(yearName);
	assert(year!=NULL);
	int nStudents=year->numberOfStudents;
	while(year->groupsList.size()>0){
		QString groupName=year->groupsList[0]->name;
		this->removeGroup(yearName, groupName);
	}

	for(int i=0; i<this->activitiesList.size(); ){
		Activity* act=this->activitiesList[i];
		bool t=act->removeStudents(*this, yearName, nStudents);
		
		if(t && act->studentsNames.count()==0){
			this->removeActivity(act->id, act->activityGroupId);
			i=0;
			//(You have to be careful, there can be erased more activities here)
		}
		else
			i++;
	}
	
	for(int i=0; i<this->timeConstraintsList.size(); ){
		TimeConstraint* ctr=this->timeConstraintsList[i];
	
		bool erased=false;

		if(ctr->type==CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE){
			ConstraintStudentsSetNotAvailable* crt_constraint=(ConstraintStudentsSetNotAvailable*)ctr;
			if(yearName == crt_constraint->students){
				this->removeTimeConstraint(ctr);
				erased=true;
			}
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY){
			ConstraintStudentsSetMaxHoursDaily* crt_constraint=(ConstraintStudentsSetMaxHoursDaily*)ctr;
			if(yearName == crt_constraint->students){
				this->removeTimeConstraint(ctr);
				erased=true;
			}
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY){
			ConstraintStudentsSetMinHoursDaily* crt_constraint=(ConstraintStudentsSetMinHoursDaily*)ctr;
			if(yearName == crt_constraint->students){
				this->removeTimeConstraint(ctr);
				erased=true;
			}
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_EARLY){
			ConstraintStudentsSetEarly* crt_constraint=(ConstraintStudentsSetEarly*)ctr;
			if(yearName == crt_constraint->students){
				this->removeTimeConstraint(ctr);
				erased=true;
			}
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_NO_GAPS){
			ConstraintStudentsSetNoGaps* crt_constraint=(ConstraintStudentsSetNoGaps*)ctr;
			if(yearName == crt_constraint->students){
				this->removeTimeConstraint(ctr);
				erased=true;
			}
		}
		else if(ctr->type==CONSTRAINT_ACTIVITIES_PREFERRED_TIMES){
			ConstraintActivitiesPreferredTimes* crt_constraint=(ConstraintActivitiesPreferredTimes*)ctr;
			if(yearName == crt_constraint->studentsName){
				this->removeTimeConstraint(ctr);
				erased=true;
			}
		}

		if(!erased)
			i++;
	}

	for(int i=0; i<this->yearsList.size(); i++)
		if(this->yearsList.at(i)->name==yearName){
			delete this->yearsList[i]; //added in version 4.0.2
			this->yearsList.removeAt(i);
			break;
		}

	this->internalStructureComputed=false;
	return true;
}

int Rules::searchYear(const QString& yearName)
{
	for(int i=0; i<this->yearsList.size(); i++)
		if(this->yearsList[i]->name==yearName)
			return i;

	return -1;
}

bool Rules::modifyYear(const QString& initialYearName, const QString& finalYearName, int finalNumberOfStudents)
{
	QString _initialYearName=initialYearName;

	assert(searchYear(_initialYearName)>=0);
	assert(searchStudentsSet(finalYearName)==NULL || _initialYearName==finalYearName);

	for(int i=0; i<this->activitiesList.size(); i++){
		Activity* act=this->activitiesList[i];
		act->renameStudents(*this, _initialYearName, finalYearName);
	}

	for(int i=0; i<this->timeConstraintsList.size(); i++){
		TimeConstraint* ctr=this->timeConstraintsList[i];

		if(ctr->type==CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE){
			ConstraintStudentsSetNotAvailable* crt_constraint=(ConstraintStudentsSetNotAvailable*)ctr;
			if(_initialYearName == crt_constraint->students)
				crt_constraint->students=finalYearName;
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY){
			ConstraintStudentsSetMaxHoursDaily* crt_constraint=(ConstraintStudentsSetMaxHoursDaily*)ctr;
			if(_initialYearName == crt_constraint->students)
				crt_constraint->students=finalYearName;
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY){
			ConstraintStudentsSetMinHoursDaily* crt_constraint=(ConstraintStudentsSetMinHoursDaily*)ctr;
			if(_initialYearName == crt_constraint->students)
				crt_constraint->students=finalYearName;
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_EARLY){
			ConstraintStudentsSetEarly* crt_constraint=(ConstraintStudentsSetEarly*)ctr;
			if(_initialYearName == crt_constraint->students)
				crt_constraint->students=finalYearName;
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_NO_GAPS){
			ConstraintStudentsSetNoGaps* crt_constraint=(ConstraintStudentsSetNoGaps*)ctr;
			if(_initialYearName == crt_constraint->students)
				crt_constraint->students=finalYearName;
		}
		else if(ctr->type==CONSTRAINT_ACTIVITIES_PREFERRED_TIMES){
			ConstraintActivitiesPreferredTimes* crt_constraint=(ConstraintActivitiesPreferredTimes*)ctr;
			if(_initialYearName == crt_constraint->studentsName)
				crt_constraint->studentsName=finalYearName;
		}
	}

	int t=0;
	
	for(int i=0; i<this->yearsList.size(); i++){
		StudentsYear* sty=this->yearsList[i];
	
		if(sty->name==_initialYearName){
			sty->name=finalYearName;
			sty->numberOfStudents=finalNumberOfStudents;
			t++;
			
			for(int j=0; j<sty->groupsList.size(); j++){
				StudentsGroup* stg=sty->groupsList[j];

				if(stg->name.right(11)==" WHOLE YEAR" && stg->name.left(stg->name.length()-11)==_initialYearName)
					this->modifyGroup(sty->name, stg->name, sty->name+" WHOLE YEAR", stg->numberOfStudents);
			}
		}
	}
		
	assert(t<=1);

	this->internalStructureComputed=false;
	
	if(t==0)
		return false;
	else
		return true;
}

void Rules::sortYearsAlphabetically()
{
	qSort(this->yearsList.begin(), this->yearsList.end(), yearsAscending);

	this->internalStructureComputed=false;
}

bool Rules::addGroup(const QString& yearName, StudentsGroup* group)
{
	StudentsYear* sty=NULL;
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

	this->internalStructureComputed=false;
	return true;
}

bool Rules::removeGroup(const QString& yearName, const QString& groupName)
{
	StudentsGroup* group=(StudentsGroup*)searchStudentsSet(groupName);
	assert(group!=NULL);
	int nStudents=group->numberOfStudents;
	while(group->subgroupsList.size()>0){
		QString subgroupName=group->subgroupsList[0]->name;
		this->removeSubgroup(yearName, groupName, subgroupName);
	}

	StudentsYear* sty=NULL;
	for(int i=0; i<this->yearsList.size(); i++){
		sty=yearsList[i];
		if(sty->name==yearName)
			break;
	}
	assert(sty);

	StudentsGroup* stg=NULL;
	for(int i=0; i<sty->groupsList.size(); i++){
		stg=sty->groupsList[i];
		if(stg->name==groupName){
			sty->groupsList.removeAt(i);
			break;
		}
	}

	this->internalStructureComputed=false;

	if(this->searchStudentsSet(stg->name)!=NULL)
		//group still exists
		return true;

	delete stg;

	for(int i=0; i<this->activitiesList.size(); ){
		Activity* act=this->activitiesList[i];

		bool t=act->removeStudents(*this, groupName, nStudents);
		if(t && act->studentsNames.count()==0){
			this->removeActivity(act->id, act->activityGroupId);
			i=0;
			//(You have to be careful, there can be erased more activities here)
		}
		else
			i++;
	}

	for(int i=0; i<this->timeConstraintsList.size(); ){
		TimeConstraint* ctr=this->timeConstraintsList[i];

		bool erased=false;
		if(ctr->type==CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE){
			ConstraintStudentsSetNotAvailable* crt_constraint=(ConstraintStudentsSetNotAvailable*)ctr;
			if(groupName == crt_constraint->students){
				this->removeTimeConstraint(ctr);
				erased=true;
			}
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY){
			ConstraintStudentsSetMaxHoursDaily* crt_constraint=(ConstraintStudentsSetMaxHoursDaily*)ctr;
			if(groupName == crt_constraint->students){
				this->removeTimeConstraint(ctr);
				erased=true;
			}
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY){
			ConstraintStudentsSetMinHoursDaily* crt_constraint=(ConstraintStudentsSetMinHoursDaily*)ctr;
			if(groupName == crt_constraint->students){
				this->removeTimeConstraint(ctr);
				erased=true;
			}
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_EARLY){
			ConstraintStudentsSetEarly* crt_constraint=(ConstraintStudentsSetEarly*)ctr;
			if(groupName == crt_constraint->students){
				this->removeTimeConstraint(ctr);
				erased=true;
			}
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_NO_GAPS){
			ConstraintStudentsSetNoGaps* crt_constraint=(ConstraintStudentsSetNoGaps*)ctr;
			if(groupName == crt_constraint->students){
				this->removeTimeConstraint(ctr);
				erased=true;
			}
		}
		else if(ctr->type==CONSTRAINT_ACTIVITIES_PREFERRED_TIMES){
			ConstraintActivitiesPreferredTimes* crt_constraint=(ConstraintActivitiesPreferredTimes*)ctr;
			if(groupName == crt_constraint->studentsName){
				this->removeTimeConstraint(ctr);
				erased=true;
			}
		}

		if(!erased)
			i++;
	}

	return true;
}

int Rules::searchGroup(const QString& yearName, const QString& groupName)
{
	StudentsYear* sty=this->yearsList[this->searchYear(yearName)];
	assert(sty);
	
	for(int i=0; i<sty->groupsList.size(); i++)
		if(sty->groupsList[i]->name==groupName)
			return i;
	
	return -1;
}

bool Rules::modifyGroup(const QString& yearName, const QString& initialGroupName, const QString& finalGroupName, int finalNumberOfStudents)
{
	//cout<<"Begin: initialGroupName=='"<<(const char*)initialGroupName<<"'"<<endl;
	
	QString _initialGroupName=initialGroupName;

	assert(searchGroup(yearName, _initialGroupName)>=0);
	assert(searchStudentsSet(finalGroupName)==NULL || _initialGroupName==finalGroupName);

	StudentsYear* sty=NULL;
	for(int i=0; i<this->yearsList.size(); i++){
		sty=this->yearsList[i];
		if(sty->name==yearName)
			break;
	}
	assert(sty);
	
	StudentsGroup* stg=NULL;
	for(int i=0; i<sty->groupsList.size(); i++){
		stg=sty->groupsList[i];
		if(stg->name==_initialGroupName){
			stg->name=finalGroupName;
			stg->numberOfStudents=finalNumberOfStudents;

			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				//cout<<"sts->name=='"<<(const char*)sts->name<<"'"<<endl;
				//cout<<"initialGroupName=='"<<(const char*)initialGroupName<<"'"<<endl;
				if(sts->name.right(12)==" WHOLE GROUP" && sts->name.left(sts->name.length()-12)==_initialGroupName){
					//cout<<"here: sts->name=='"<<(const char*)sts->name<<"'"<<endl;
					this->modifySubgroup(sty->name, stg->name, sts->name, stg->name+" WHOLE GROUP", sts->numberOfStudents);
				}
			}

			break;
		}
	}
	assert(stg);

	for(int i=0; i<this->activitiesList.size(); i++)
		this->activitiesList[i]->renameStudents(*this, _initialGroupName, finalGroupName);
	
	for(int i=0; i<this->timeConstraintsList.size(); i++){
		TimeConstraint* ctr=this->timeConstraintsList[i];

		if(ctr->type==CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE){
			ConstraintStudentsSetNotAvailable* crt_constraint=(ConstraintStudentsSetNotAvailable*)ctr;
			if(_initialGroupName == crt_constraint->students)
				crt_constraint->students=finalGroupName;
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY){
			ConstraintStudentsSetMaxHoursDaily* crt_constraint=(ConstraintStudentsSetMaxHoursDaily*)ctr;
			if(_initialGroupName == crt_constraint->students)
				crt_constraint->students=finalGroupName;
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY){
			ConstraintStudentsSetMinHoursDaily* crt_constraint=(ConstraintStudentsSetMinHoursDaily*)ctr;
			if(_initialGroupName == crt_constraint->students)
				crt_constraint->students=finalGroupName;
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_EARLY){
			ConstraintStudentsSetEarly* crt_constraint=(ConstraintStudentsSetEarly*)ctr;
			if(_initialGroupName == crt_constraint->students)
				crt_constraint->students=finalGroupName;
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_NO_GAPS){
			ConstraintStudentsSetNoGaps* crt_constraint=(ConstraintStudentsSetNoGaps*)ctr;
			if(_initialGroupName == crt_constraint->students)
				crt_constraint->students=finalGroupName;
		}
		else if(ctr->type==CONSTRAINT_ACTIVITIES_PREFERRED_TIMES){
			ConstraintActivitiesPreferredTimes* crt_constraint=(ConstraintActivitiesPreferredTimes*)ctr;
			if(_initialGroupName == crt_constraint->studentsName)
				crt_constraint->studentsName=finalGroupName;
		}
	}

	this->internalStructureComputed=false;

	return true;
}

void Rules::sortGroupsAlphabetically(const QString& yearName)
{
	StudentsYear* sty=this->yearsList[this->searchYear(yearName)];
	assert(sty);

	qSort(sty->groupsList.begin(), sty->groupsList.end(), groupsAscending);

	this->internalStructureComputed=false;
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

	this->internalStructureComputed=false;
	return true;
}

bool Rules::removeSubgroup(const QString& yearName, const QString& groupName, const QString& subgroupName)
{
	StudentsSubgroup* subgroup=(StudentsSubgroup*)searchStudentsSet(subgroupName);
	assert(subgroup!=NULL);
	int nStudents=subgroup->numberOfStudents;

	StudentsYear* sty=this->yearsList.at(this->searchYear(yearName));
	assert(sty);
	StudentsGroup* stg=sty->groupsList.at(this->searchGroup(yearName, groupName));
	assert(stg);

	StudentsSubgroup* sts=NULL;
	for(int i=0; i<stg->subgroupsList.size(); i++){
		sts=stg->subgroupsList[i];
		if(sts->name==subgroupName){
			stg->subgroupsList.removeAt(i);
			break;
		}
	}

	if(this->searchStudentsSet(sts->name)!=NULL)
		//subgroup still exists, in other group
		return true;

	delete sts;

	this->internalStructureComputed=false;

	for(int i=0; i<this->activitiesList.size(); ){
		Activity* act=this->activitiesList[i];

		bool t=act->removeStudents(*this, subgroupName, nStudents);
		if(t && act->studentsNames.count()==0){
			this->removeActivity(act->id, act->activityGroupId);
			i=0;
			//(You have to be careful, there can be erased more activities here)
		}
		else
			i++;
	}
	
	for(int i=0; i<this->timeConstraintsList.size(); ){
		TimeConstraint* ctr=this->timeConstraintsList[i];

		bool erased=false;
		if(ctr->type==CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE){
			ConstraintStudentsSetNotAvailable* crt_constraint=(ConstraintStudentsSetNotAvailable*)ctr;
			if(subgroupName == crt_constraint->students){
				this->removeTimeConstraint(ctr);
				erased=true;
			}
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY){
			ConstraintStudentsSetMaxHoursDaily* crt_constraint=(ConstraintStudentsSetMaxHoursDaily*)ctr;
			if(subgroupName == crt_constraint->students){
				this->removeTimeConstraint(ctr);
				erased=true;
			}
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY){
			ConstraintStudentsSetMinHoursDaily* crt_constraint=(ConstraintStudentsSetMinHoursDaily*)ctr;
			if(subgroupName == crt_constraint->students){
				this->removeTimeConstraint(ctr);
				erased=true;
			}
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_EARLY){
			ConstraintStudentsSetEarly* crt_constraint=(ConstraintStudentsSetEarly*)ctr;
			if(subgroupName == crt_constraint->students){
				this->removeTimeConstraint(ctr);
				erased=true;
			}
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_NO_GAPS){
			ConstraintStudentsSetNoGaps* crt_constraint=(ConstraintStudentsSetNoGaps*)ctr;
			if(subgroupName == crt_constraint->students){
				this->removeTimeConstraint(ctr);
				erased=true;
			}
		}
		else if(ctr->type==CONSTRAINT_ACTIVITIES_PREFERRED_TIMES){
			ConstraintActivitiesPreferredTimes* crt_constraint=(ConstraintActivitiesPreferredTimes*)ctr;
			if(subgroupName == crt_constraint->studentsName){
				this->removeTimeConstraint(ctr);
				erased=true;
			}
		}

		if(!erased)
			i++;
	}

	return true;
}

int Rules::searchSubgroup(const QString& yearName, const QString& groupName, const QString& subgroupName)
{
	StudentsYear* sty=this->yearsList.at(this->searchYear(yearName));
	assert(sty);
	StudentsGroup* stg=sty->groupsList.at(this->searchGroup(yearName, groupName));
	assert(stg);
	
	for(int i=0; i<stg->subgroupsList.size(); i++)
		if(stg->subgroupsList[i]->name==subgroupName)
			return i;
	
	return -1;
}

bool Rules::modifySubgroup(const QString& yearName, const QString& groupName, const QString& initialSubgroupName, const QString& finalSubgroupName, int finalNumberOfStudents)
{
	QString _initialSubgroupName=initialSubgroupName;

	assert(searchSubgroup(yearName, groupName, _initialSubgroupName)>=0);
	assert(searchStudentsSet(finalSubgroupName)==NULL || _initialSubgroupName==finalSubgroupName);

	StudentsYear* sty=this->yearsList.at(this->searchYear(yearName));
	assert(sty);
	StudentsGroup* stg=sty->groupsList.at(this->searchGroup(yearName, groupName));
	assert(stg);

	StudentsSubgroup* sts=NULL;
	for(int i=0; i<stg->subgroupsList.size(); i++){
		sts=stg->subgroupsList[i];

		if(sts->name==_initialSubgroupName){
			sts->name=finalSubgroupName;
			sts->numberOfStudents=finalNumberOfStudents;
			break;
		}
	}
	assert(sts);

	//TODO: improve this part
	for(int i=0; i<this->activitiesList.size(); i++)
		this->activitiesList[i]->renameStudents(*this, _initialSubgroupName, finalSubgroupName);

	for(int i=0; i<this->timeConstraintsList.size(); i++){
		TimeConstraint* ctr=this->timeConstraintsList[i];

		if(ctr->type==CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE){
			ConstraintStudentsSetNotAvailable* crt_constraint=(ConstraintStudentsSetNotAvailable*)ctr;
			if(_initialSubgroupName == crt_constraint->students)
				crt_constraint->students=finalSubgroupName;
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY){
			ConstraintStudentsSetMaxHoursDaily* crt_constraint=(ConstraintStudentsSetMaxHoursDaily*)ctr;
			if(_initialSubgroupName == crt_constraint->students)
				crt_constraint->students=finalSubgroupName;
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY){
			ConstraintStudentsSetMinHoursDaily* crt_constraint=(ConstraintStudentsSetMinHoursDaily*)ctr;
			if(_initialSubgroupName == crt_constraint->students)
				crt_constraint->students=finalSubgroupName;
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_EARLY){
			ConstraintStudentsSetEarly* crt_constraint=(ConstraintStudentsSetEarly*)ctr;
			if(_initialSubgroupName == crt_constraint->students)
				crt_constraint->students=finalSubgroupName;
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_NO_GAPS){
			ConstraintStudentsSetNoGaps* crt_constraint=(ConstraintStudentsSetNoGaps*)ctr;
			if(_initialSubgroupName == crt_constraint->students)
				crt_constraint->students=finalSubgroupName;
		}
		else if(ctr->type==CONSTRAINT_ACTIVITIES_PREFERRED_TIMES){
			ConstraintActivitiesPreferredTimes* crt_constraint=(ConstraintActivitiesPreferredTimes*)ctr;
			if(_initialSubgroupName == crt_constraint->studentsName)
				crt_constraint->studentsName=finalSubgroupName;
		}
	}

	this->internalStructureComputed=false;

	return true;
}

void Rules::sortSubgroupsAlphabetically(const QString& yearName, const QString& groupName)
{
	StudentsYear* sty=this->yearsList.at(this->searchYear(yearName));
	assert(sty);
	StudentsGroup* stg=sty->groupsList.at(this->searchGroup(yearName, groupName));
	assert(stg);

	qSort(stg->subgroupsList.begin(), stg->subgroupsList.end(), subgroupsAscending);
	
	this->internalStructureComputed=false;
}

bool Rules::addSimpleActivity(
	int _id,
	int _activityGroupId,
	const QStringList& _teachersNames,
	const QString& _subjectName,
	const QString& _subjectTagName,
	const QStringList& _studentsNames,
	int _duration, /*duration, in hours*/
	int _totalDuration,
	//int _parity, /*parity: PARITY_WEEKLY or PARITY_FORTNIGHTLY*/
	bool _active,
	//int _preferredDay,
	//int _preferredHour,
	bool _computeNTotalStudents,
	int _nTotalStudents)
{
	//assert(_parity==PARITY_WEEKLY || _parity==PARITY_FORTNIGHTLY); //weekly or fortnightly

	Activity *act=new Activity(*this, _id, _activityGroupId, _teachersNames, _subjectName, _subjectTagName,
		_studentsNames, _duration, _totalDuration, /*_parity,*/ _active, _computeNTotalStudents, _nTotalStudents);

	this->activitiesList << act; //append

	/*if(_preferredDay>=0 || _preferredHour>=0){
		TimeConstraint *ctr=new ConstraintActivityPreferredTime(0.0, _id, _preferredDay, _preferredHour);
		bool tmp=this->addTimeConstraint(ctr);
		assert(tmp);
	}*/

	this->internalStructureComputed=false;

	return true;
}

bool Rules::addSplitActivity(
	int _firstActivityId,
	int _activityGroupId,
	const QStringList& _teachersNames,
	const QString& _subjectName,
	const QString& _subjectTagName,
	const QStringList& _studentsNames,
	int _nSplits,
	int _totalDuration,
	int _durations[],
	//int _parities[],
	bool _active[],
	int _minDayDistance,
	double _weightPercentage,
	bool _consecutiveIfSameDay,
	//int _preferredDays[],
	//int _preferredHours[],
	bool _computeNTotalStudents,
	int _nTotalStudents)
{
	assert(_firstActivityId==_activityGroupId);

	int acts[MAX_CONSTRAINT_MIN_N_DAYS_BETWEEN_ACTIVITIES];
	assert(_nSplits<=MAX_CONSTRAINT_MIN_N_DAYS_BETWEEN_ACTIVITIES);

	//for(int i=0; i<_nSplits; i++)
	//	assert(_parities[i]==PARITY_WEEKLY || _parities[i]==PARITY_FORTNIGHTLY); //weekly or fortnightly

	for(int i=0; i<_nSplits; i++){
		Activity *act;
		if(i==0)
			act=new Activity(*this, _firstActivityId+i, _activityGroupId,
				_teachersNames, _subjectName, _subjectTagName, _studentsNames,
				_durations[i], _totalDuration, /*_parities[i],*/ _active[i], _computeNTotalStudents, _nTotalStudents);
		else
			act=new Activity(*this, _firstActivityId+i, _activityGroupId,
				_teachersNames, _subjectName, _subjectTagName, _studentsNames,
				_durations[i], _totalDuration, /*_parities[i],*/ _active[i], _computeNTotalStudents, _nTotalStudents);

		this->activitiesList << act; //append

		acts[i]=_firstActivityId+i;

		/*if(_preferredDays[i]>=0 || _preferredHours[i]>=0){
			TimeConstraint *constr=new ConstraintActivityPreferredTime(0.0, act->id, _preferredDays[i], _preferredHours[i]); //non-compulsory constraint
			bool tmp = this->addTimeConstraint(constr);
			assert(tmp);
		}*/
	}

	if(_minDayDistance>0){
		//TimeConstraint *constr=new ConstraintMinNDaysBetweenActivities(1.0, true, _nSplits, acts, _minDayDistance); //compulsory constraint
		TimeConstraint *constr=new ConstraintMinNDaysBetweenActivities(_weightPercentage, _consecutiveIfSameDay, _nSplits, acts, _minDayDistance);
		bool tmp=this->addTimeConstraint(constr);
		assert(tmp);
	}

	this->internalStructureComputed=false;

	return true;
}

void Rules::removeActivity(int _id)
{
	for(int i=0; i<this->activitiesList.size(); i++){
		Activity* act=this->activitiesList[i];
		if(_id==act->id){
			//removing ConstraintActivityPreferredTime-s referring to this activity
			for(int j=0; j<this->timeConstraintsList.size(); ){
				TimeConstraint* ctr=this->timeConstraintsList[j];
				if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_TIME){
					ConstraintActivityPreferredTime *apt=(ConstraintActivityPreferredTime*)ctr;
					if(apt->activityId==act->id){
						cout<<"Removing constraint "<<(const char*)(apt->getDescription(*this))<<endl;
						this->removeTimeConstraint(ctr);
					}
					else
						j++;
				}
				else
					j++;
			}
			//removing Constraint2ActivitiesConsecutive-s referring to this activity
			for(int j=0; j<this->timeConstraintsList.size(); ){
				TimeConstraint* ctr=this->timeConstraintsList[j];
				if(ctr->type==CONSTRAINT_2_ACTIVITIES_CONSECUTIVE){
					Constraint2ActivitiesConsecutive *apt=(Constraint2ActivitiesConsecutive*)ctr;
					if(apt->firstActivityId==act->id){
						cout<<"Removing constraint "<<(const char*)(apt->getDescription(*this))<<endl;
						this->removeTimeConstraint(ctr);
					}
					else if(apt->secondActivityId==act->id){
						cout<<"Removing constraint "<<(const char*)(apt->getDescription(*this))<<endl;
						this->removeTimeConstraint(ctr);
					}
					else
						j++;
				}
				else
					j++;
			}
			//removing ConstraintActivityPreferredTimes-s referring to this activity
			for(int j=0; j<this->timeConstraintsList.size(); ){
				TimeConstraint* ctr=this->timeConstraintsList[j];
				if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_TIMES){
					ConstraintActivityPreferredTimes *apt=(ConstraintActivityPreferredTimes*)ctr;
					if(apt->activityId==act->id){
						cout<<"Removing constraint "<<(const char*)(apt->getDescription(*this))<<endl;
						this->removeTimeConstraint(ctr);
					}
					else
						j++;
				}
				else
					j++;
			}
			//removing ConstraintActivityEndsStudentsDay-s referring to this activity
			for(int j=0; j<this->timeConstraintsList.size(); ){
				TimeConstraint* ctr=this->timeConstraintsList[j];
				if(ctr->type==CONSTRAINT_ACTIVITY_ENDS_STUDENTS_DAY){
					ConstraintActivityEndsStudentsDay *apt=(ConstraintActivityEndsStudentsDay*)ctr;
					if(apt->activityId==act->id){
						cout<<"Removing constraint "<<(const char*)(apt->getDescription(*this))<<endl;
						this->removeTimeConstraint(ctr);
					}
					else
						j++;
				}
				else
					j++;
			}

			//removing ConstraintActivityPreferredRoom-s referring to this activity
			for(int j=0; j<this->spaceConstraintsList.size(); ){
				SpaceConstraint* ctr=this->spaceConstraintsList[j];
				if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
					if(((ConstraintActivityPreferredRoom*)ctr)->activityId==act->id)
						this->removeSpaceConstraint(ctr);
					else
						j++;
				}
				else
					j++;
			}
			//removing ConstraintActivityPreferredRooms-s referring to this activity
			for(int j=0; j<this->spaceConstraintsList.size(); ){
				SpaceConstraint* ctr=this->spaceConstraintsList[j];
				if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOMS){
					if(((ConstraintActivityPreferredRooms*)ctr)->activityId==act->id)
						this->removeSpaceConstraint(ctr);
					else
						j++;
				}
				else
					j++;
			}

			//remove the activity
			delete this->activitiesList[i];
			this->activitiesList.removeAt(i); 
			break;
		}
	}

	for(int i=0; i<this->timeConstraintsList.size(); ){
		TimeConstraint* ctr=this->timeConstraintsList[i];
		if(ctr->type==CONSTRAINT_MIN_N_DAYS_BETWEEN_ACTIVITIES){
			((ConstraintMinNDaysBetweenActivities*)ctr)->removeUseless(*this);
			if(((ConstraintMinNDaysBetweenActivities*)ctr)->n_activities<2)
				this->removeTimeConstraint(ctr);
			else
				i++;
		}
		else
			i++;
	}

	for(int i=0; i<this->timeConstraintsList.size(); ){
		TimeConstraint* ctr=this->timeConstraintsList[i];
		if(ctr->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME){
			((ConstraintActivitiesSameStartingTime*)ctr)->removeUseless(*this);
			if(((ConstraintActivitiesSameStartingTime*)ctr)->n_activities<2)
				this->removeTimeConstraint(ctr);
			else
				i++;
		}
		else
			i++;
	}

	for(int i=0; i<this->timeConstraintsList.size(); ){
		TimeConstraint* ctr=this->timeConstraintsList[i];
		if(ctr->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR){
			((ConstraintActivitiesSameStartingHour*)ctr)->removeUseless(*this);
			if(((ConstraintActivitiesSameStartingHour*)ctr)->n_activities<2)
				this->removeTimeConstraint(ctr);
			else
				i++;
		}
		else
			i++;
	}

	for(int i=0; i<this->timeConstraintsList.size(); ){
		TimeConstraint* ctr=this->timeConstraintsList[i];
		if(ctr->type==CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING){
			((ConstraintActivitiesNotOverlapping*)ctr)->removeUseless(*this);
			if(((ConstraintActivitiesNotOverlapping*)ctr)->n_activities<2)
				this->removeTimeConstraint(ctr);
			else
				i++;
		}
		else
			i++;
	}

	this->internalStructureComputed=false;
}

void Rules::removeActivity(int _id, int _activityGroupId)
{
	for(int i=0; i<this->activitiesList.size(); ){
		Activity* act=this->activitiesList[i];

		if(_id==act->id || _activityGroupId>0 && _activityGroupId==act->activityGroupId){

			//removing ConstraintActivityPreferredTime-s referring to this activity
			for(int j=0; j<this->timeConstraintsList.size(); ){
				TimeConstraint* ctr=this->timeConstraintsList[j];
				if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_TIME){
					ConstraintActivityPreferredTime *apt=(ConstraintActivityPreferredTime*)ctr;
					if(apt->activityId==act->id){
						cout<<"Removing constraint "<<(const char*)(apt->getDescription(*this))<<endl;
						this->removeTimeConstraint(ctr);
					}
					else
						j++;
				}
				else
					j++;
			}
			//removing Constraint2ActivitiesConsecutive-s referring to this activity
			for(int j=0; j<this->timeConstraintsList.size(); ){
				TimeConstraint* ctr=this->timeConstraintsList[j];
				if(ctr->type==CONSTRAINT_2_ACTIVITIES_CONSECUTIVE){
					Constraint2ActivitiesConsecutive *apt=(Constraint2ActivitiesConsecutive*)ctr;
					if(apt->firstActivityId==act->id){
						cout<<"Removing constraint "<<(const char*)(apt->getDescription(*this))<<endl;
						this->removeTimeConstraint(ctr);
					}
					else if(apt->secondActivityId==act->id){
						cout<<"Removing constraint "<<(const char*)(apt->getDescription(*this))<<endl;
						this->removeTimeConstraint(ctr);
					}
					else
						j++;
				}
				else
					j++;
			}
			//removing ConstraintActivityPreferredTimes-s referring to this activity
			for(int j=0; j<this->timeConstraintsList.size(); ){
				TimeConstraint* ctr=this->timeConstraintsList[j];
				if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_TIMES){
					ConstraintActivityPreferredTimes *apt=(ConstraintActivityPreferredTimes*)ctr;
					if(apt->activityId==act->id){
						cout<<"Removing constraint "<<(const char*)(apt->getDescription(*this))<<endl;
						this->removeTimeConstraint(ctr);
					}
					else
						j++;
				}
				else
					j++;
			}
			//removing ConstraintActivityEndsStudentsDay-s referring to this activity
			for(int j=0; j<this->timeConstraintsList.size(); ){
				TimeConstraint* ctr=this->timeConstraintsList[j];
				if(ctr->type==CONSTRAINT_ACTIVITY_ENDS_STUDENTS_DAY){
					ConstraintActivityEndsStudentsDay *apt=(ConstraintActivityEndsStudentsDay*)ctr;
					if(apt->activityId==act->id){
						cout<<"Removing constraint "<<(const char*)(apt->getDescription(*this))<<endl;
						this->removeTimeConstraint(ctr);
					}
					else
						j++;
				}
				else
					j++;
			}

			//removing ConstraintActivityPreferredRoom-s referring to this activity
			for(int j=0; j<this->spaceConstraintsList.size(); ){
				SpaceConstraint* ctr=this->spaceConstraintsList[j];
				if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
					if(((ConstraintActivityPreferredRoom*)ctr)->activityId==act->id)
						this->removeSpaceConstraint(ctr);
					else
						j++;
				}
				else
					j++;
			}
			//removing ConstraintActivityPreferredRooms-s referring to this activity
			for(int j=0; j<this->spaceConstraintsList.size(); ){
				SpaceConstraint* ctr=this->spaceConstraintsList[j];
				if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOMS){
					if(((ConstraintActivityPreferredRooms*)ctr)->activityId==act->id)
						this->removeSpaceConstraint(ctr);
					else
						j++;
				}
				else
					j++;
			}

			delete this->activitiesList[i];
			this->activitiesList.removeAt(i); //if this is the last activity, then we will make one more comparison above
		}
		else
			i++;
	}

	for(int i=0; i<this->timeConstraintsList.size(); ){
		TimeConstraint* ctr=this->timeConstraintsList[i];
		if(ctr->type==CONSTRAINT_MIN_N_DAYS_BETWEEN_ACTIVITIES){
			((ConstraintMinNDaysBetweenActivities*)ctr)->removeUseless(*this);
			if(((ConstraintMinNDaysBetweenActivities*)ctr)->n_activities<2)
				this->removeTimeConstraint(ctr);
			else
				i++;
		}
		else
			i++;
	}

	for(int i=0; i<this->timeConstraintsList.size(); ){
		TimeConstraint* ctr=this->timeConstraintsList[i];
		if(ctr->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME){
			((ConstraintActivitiesSameStartingTime*)ctr)->removeUseless(*this);
			if(((ConstraintActivitiesSameStartingTime*)ctr)->n_activities<2)
				this->removeTimeConstraint(ctr);
			else
				i++;
		}
		else
			i++;
	}

	for(int i=0; i<this->timeConstraintsList.size(); ){
		TimeConstraint* ctr=this->timeConstraintsList[i];
		if(ctr->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR){
			((ConstraintActivitiesSameStartingHour*)ctr)->removeUseless(*this);
			if(((ConstraintActivitiesSameStartingHour*)ctr)->n_activities<2)
				this->removeTimeConstraint(ctr);
			else
				i++;
		}
		else
			i++;
	}

	for(int i=0; i<this->timeConstraintsList.size(); ){
		TimeConstraint* ctr=this->timeConstraintsList[i];
		if(ctr->type==CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING){
			((ConstraintActivitiesNotOverlapping*)ctr)->removeUseless(*this);
			if(((ConstraintActivitiesNotOverlapping*)ctr)->n_activities<2)
				this->removeTimeConstraint(ctr);
			else
				i++;
		}
		else
			i++;
	}

	this->internalStructureComputed=false;
}

void Rules::modifyActivity(
	int _id,
	int _activityGroupId,
	const QStringList& _teachersNames,
	const QString& _subjectName,
	const QString& _subjectTagName,
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
		if(_activityGroupId==0 && act->id==_id || _activityGroupId!=0 && act->activityGroupId==_activityGroupId){
			act->teachersNames=_teachersNames;
			act->subjectName=_subjectName;
			act->subjectTagName=_subjectTagName;
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
}

bool Rules::addRoom(Room* rm)
{
	if(this->searchRoom(rm->name) >= 0)
		return false;
	this->roomsList << rm; //append
	this->internalStructureComputed=false;
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

	for(int j=0; j<this->spaceConstraintsList.size(); ){
		SpaceConstraint* ctr=this->spaceConstraintsList[j];
		if(ctr->type==CONSTRAINT_ROOM_NOT_AVAILABLE){
			ConstraintRoomNotAvailable* crna=(ConstraintRoomNotAvailable*)ctr;
			if(crna->roomName==roomName)
				this->removeSpaceConstraint(ctr);
			else
				j++;
		}
		else if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
			ConstraintActivityPreferredRoom* c=(ConstraintActivityPreferredRoom*)ctr;
			if(c->roomName==roomName)
				this->removeSpaceConstraint(ctr);
			else
				j++;
		}
		else if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOMS){
			ConstraintActivityPreferredRooms* c=(ConstraintActivityPreferredRooms*)ctr;
			int t=c->roomsNames.remove(roomName);
			assert(t<=1);
			if(t==1 && c->roomsNames.count()==0)
				this->removeSpaceConstraint(ctr);
			else if(t==1 && c->roomsNames.count()==1){
				ConstraintActivityPreferredRoom* c2=new ConstraintActivityPreferredRoom
				 (c->weightPercentage, c->activityId, c->roomsNames.at(0));

				QMessageBox::information(NULL, QObject::tr("FET information"), 
				 QObject::tr("The constraint\n%1 will be modified into constraint\n%2 because"
				 " there is only one room left in the constraint")
				 .arg(c->getDetailedDescription(*this))
				 .arg(c2->getDetailedDescription(*this)));

				this->removeSpaceConstraint(ctr);
				this->addSpaceConstraint(c2);
			}
			else
				j++;
		}
		else if(ctr->type==CONSTRAINT_SUBJECT_PREFERRED_ROOM){
			ConstraintSubjectPreferredRoom* c=(ConstraintSubjectPreferredRoom*)ctr;
			if(c->roomName==roomName)
				this->removeSpaceConstraint(ctr);
			else
				j++;
		}
		else if(ctr->type==CONSTRAINT_SUBJECT_PREFERRED_ROOMS){
			ConstraintSubjectPreferredRooms* c=(ConstraintSubjectPreferredRooms*)ctr;
			int t=c->roomsNames.remove(roomName);
			assert(t<=1);
			if(t==1 && c->roomsNames.count()==0)
				this->removeSpaceConstraint(ctr);
			else if(t==1 && c->roomsNames.count()==1){
				ConstraintSubjectPreferredRoom* c2=new ConstraintSubjectPreferredRoom
				 (c->weightPercentage, c->subjectName, c->roomsNames.at(0));

				QMessageBox::information(NULL, QObject::tr("FET information"), 
				 QObject::tr("The constraint\n%1 will be modified into constraint\n%2 because"
				 " there is only one room left in the constraint")
				 .arg(c->getDetailedDescription(*this))
				 .arg(c2->getDetailedDescription(*this)));

				this->removeSpaceConstraint(ctr);
				this->addSpaceConstraint(c2);
			}
			else
				j++;
		}
		else if(ctr->type==CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOM){
			ConstraintSubjectSubjectTagPreferredRoom* c=(ConstraintSubjectSubjectTagPreferredRoom*)ctr;
			if(c->roomName==roomName)
				this->removeSpaceConstraint(ctr);
			else
				j++;
		}
		else if(ctr->type==CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOMS){
			ConstraintSubjectSubjectTagPreferredRooms* c=(ConstraintSubjectSubjectTagPreferredRooms*)ctr;
			int t=c->roomsNames.remove(roomName);
			assert(t<=1);
			if(t==1 && c->roomsNames.count()==0)
				this->removeSpaceConstraint(ctr);
			else if(t==1 && c->roomsNames.count()==1){
				ConstraintSubjectSubjectTagPreferredRoom* c2=new ConstraintSubjectSubjectTagPreferredRoom
				 (c->weightPercentage, c->subjectName, c->subjectTagName, c->roomsNames.at(0));

				QMessageBox::information(NULL, QObject::tr("FET information"), 
				 QObject::tr("The constraint\n%1 will be modified into constraint\n%2 because"
				 " there is only one room left in the constraint")
				 .arg(c->getDetailedDescription(*this))
				 .arg(c2->getDetailedDescription(*this)));

				this->removeSpaceConstraint(ctr);
				this->addSpaceConstraint(c2);
			}
			else
				j++;
		}
		else
			j++;
	}

	delete this->roomsList[i];
	this->roomsList.removeAt(i);
	this->internalStructureComputed=false;
	return true;
}

bool Rules::modifyRoom(const QString& initialRoomName, const QString& finalRoomName/*, const QString& type, const QString& building*/, int capacity)
{
	int i=this->searchRoom(initialRoomName);
	if(i<0)
		return false;

	Room* searchedRoom=this->roomsList[i];
	assert(searchedRoom->name==initialRoomName);

	for(int j=0; j<this->spaceConstraintsList.size(); ){
		SpaceConstraint* ctr=this->spaceConstraintsList[j];
		if(ctr->type==CONSTRAINT_ROOM_NOT_AVAILABLE){
			ConstraintRoomNotAvailable* crna=(ConstraintRoomNotAvailable*)ctr;
			if(crna->roomName==initialRoomName)
				crna->roomName=finalRoomName;
			j++;
		}
		else if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
			ConstraintActivityPreferredRoom* c=(ConstraintActivityPreferredRoom*)ctr;
			if(c->roomName==initialRoomName)
				c->roomName=finalRoomName;
			j++;
		}
		else if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOMS){
			ConstraintActivityPreferredRooms* c=(ConstraintActivityPreferredRooms*)ctr;
			int t=0;
			for(QStringList::Iterator it=c->roomsNames.begin(); it!=c->roomsNames.end(); it++){
				if((*it)==initialRoomName){
					*it=finalRoomName;
					t++;
				}
			}
			assert(t<=1);
			j++;
		}
		else if(ctr->type==CONSTRAINT_SUBJECT_PREFERRED_ROOM){
			ConstraintSubjectPreferredRoom* c=(ConstraintSubjectPreferredRoom*)ctr;
			if(c->roomName==initialRoomName)
				c->roomName=finalRoomName;
			j++;
		}
		else if(ctr->type==CONSTRAINT_SUBJECT_PREFERRED_ROOMS){
			ConstraintSubjectPreferredRooms* c=(ConstraintSubjectPreferredRooms*)ctr;
			int t=0;
			for(QStringList::Iterator it=c->roomsNames.begin(); it!=c->roomsNames.end(); it++){
				if((*it)==initialRoomName){
					*it=finalRoomName;
					t++;
				}
			}
			assert(t<=1);
			j++;
		}
		else if(ctr->type==CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOM){
			ConstraintSubjectSubjectTagPreferredRoom* c=(ConstraintSubjectSubjectTagPreferredRoom*)ctr;
			if(c->roomName==initialRoomName)
				c->roomName=finalRoomName;
			j++;
		}
		else if(ctr->type==CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOMS){
			ConstraintSubjectSubjectTagPreferredRooms* c=(ConstraintSubjectSubjectTagPreferredRooms*)ctr;
			int t=0;
			for(QStringList::Iterator it=c->roomsNames.begin(); it!=c->roomsNames.end(); it++){
				if((*it)==initialRoomName){
					*it=finalRoomName;
					t++;
				}
			}
			assert(t<=1);
			j++;
		}
		else
			j++;
	}

	searchedRoom->name=finalRoomName;
	searchedRoom->capacity=capacity;

	this->internalStructureComputed=false;
	return true;
}

void Rules::sortRoomsAlphabetically()
{
	qSort(this->roomsList.begin(), this->roomsList.end(), roomsAscending);

	this->internalStructureComputed=false;
}

bool Rules::addTimeConstraint(TimeConstraint *ctr)
{
	bool ok=true;

	//TODO: improve this

	//check if this constraint is already added, for ConstraintActivityPreferredTime
	if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_TIME){
		int i;
		for(i=0; i<this->timeConstraintsList.size(); i++){
			TimeConstraint* ctr2=this->timeConstraintsList[i];
			if(ctr2->type==CONSTRAINT_ACTIVITY_PREFERRED_TIME) 
				if(
				 *((ConstraintActivityPreferredTime*)ctr2)
				 ==
				 *((ConstraintActivityPreferredTime*)ctr)
				)
					break;
		}
				
		if(i<this->timeConstraintsList.size())
			ok=false;
	}

	//check if this constraint is already added, for ConstraintMinNDaysBetweenActivities
	if(ctr->type==CONSTRAINT_MIN_N_DAYS_BETWEEN_ACTIVITIES){
		int i;
		for(i=0; i<this->timeConstraintsList.size(); i++){
			TimeConstraint* ctr2=this->timeConstraintsList[i];
			if(ctr2->type==CONSTRAINT_MIN_N_DAYS_BETWEEN_ACTIVITIES)
				if(
				 *((ConstraintMinNDaysBetweenActivities*)ctr2)
				 ==
				 *((ConstraintMinNDaysBetweenActivities*)ctr)
				 )
					break;
		}

		if(i<this->timeConstraintsList.size())
			ok=false;
	}

	if(ok){
		this->timeConstraintsList << ctr; //append
		this->internalStructureComputed=false;
		return true;
	}
	else
		return false;
}

bool Rules::removeTimeConstraint(TimeConstraint *ctr)
{
	for(int i=0; i<this->timeConstraintsList.size(); i++)
		if(this->timeConstraintsList[i]==ctr){
			delete ctr;
			this->timeConstraintsList.removeAt(i);
			this->internalStructureComputed=false;
			return true;
		}

	return false;
}

bool Rules::addSpaceConstraint(SpaceConstraint *ctr)
{
	bool ok=true;

	//TODO: check if this constraint is already added...(if any possibility of duplicates)
	if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
		int i;
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
			ok=false;
	}

	if(ok){
		this->spaceConstraintsList << ctr; //append
		this->internalStructureComputed=false;
		return true;
	}
	else
		return false;
}

bool Rules::removeSpaceConstraint(SpaceConstraint *ctr)
{
	for(int i=0; i<this->spaceConstraintsList.size(); i++)
		if(this->spaceConstraintsList[i]==ctr){
			delete ctr;
			this->spaceConstraintsList.removeAt(i);
			this->internalStructureComputed=false;
			return true;
		}

	return false;
}

bool Rules::read(const QString& filename)
{
	QFile file(filename);
	if(!file.open(QIODevice::ReadOnly)){
		return false;
	}
	QDomDocument doc("xml_rules");
	if(!doc.setContent(&file)){
		file.close();
		cout<<"Could not open file - not existing or in use\n";
		return false;
	}
	file.close();

	//Clear old rules, initialize new rules
	if(this->initialized)
		this->kill();
	this->init();

	////////////////////////////////////////

	//logging part
	QDir dir;
	bool t=true;
	if(!dir.exists(OUTPUT_DIR))
		t=dir.mkdir(OUTPUT_DIR);
	if(!t){
		QMessageBox::warning(NULL, QObject::tr("FET warning"), QObject::tr("Cannot create or use directory %1 - cannot continue").arg(OUTPUT_DIR));
		return false;
	}
	assert(t);
	
	QString xmlReadingLog="";
	QString tmp=OUTPUT_DIR+FILE_SEP+XML_PARSING_LOG_FILENAME;
	QFile file2(tmp);
	if(!file2.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 QObject::tr("Cannot open log file for writing ... please check your disk free space. Opening of file aborted"));
		 
		return false;
	
		assert(0);
		exit(1);
	}
	QTextStream logStream(&file2);
	//logStream.setEncoding(QTextStream::UnicodeUTF8);
	logStream.setCodec("UTF-8");
	logStream.setGenerateByteOrderMark(true);
	
	QDomElement elem1=doc.documentElement();
	xmlReadingLog+=" Found "+elem1.tagName()+" tag\n";
	bool okAbove3_12_17=true;
	bool version5AndAbove=false;
	bool warning=false;
	if(elem1.tagName()!="FET")
		okAbove3_12_17=false;
	else{
		QDomDocumentType dt=doc.doctype();
		if(dt.isNull() || dt.name()!="FET")
			okAbove3_12_17=false;
		else{
			QDomAttr a=elem1.attributeNode("version");
			QString version=a.value();
			int v[3];
			//cout<<"version=|"<<version<<"|"<<endl;
			int t=sscanf((const char*)(version), "%d.%d.%d", &v[0], &v[1], &v[2]);
			assert(t==3);

			int w[3];
			t=sscanf((const char*)(FET_VERSION), "%d.%d.%d", &w[0], &w[1], &w[2]);
			assert(t==3);
			
			if(v[0]>w[0] || (v[0]==w[0] && v[1]>w[1]) || (v[0]==w[0]&&v[1]==w[1]&&v[2]>w[2]))
				warning=true;
				
			if(v[0]>=5)
				version5AndAbove=true;
		}
	}
	if(!okAbove3_12_17){ //trying version 3.6.1 to version 3.12.16
		cout<<"Invalid fet 3.12.17 or above - trying older version"<<endl;
		if(elem1.tagName()!=INPUT_FILE_TAG_3_6_1){
			file2.close();
			return false;
		}
	}
	
	if(!version5AndAbove){
		QMessageBox::warning(NULL, QObject::tr("FET information"), 
		 QObject::tr("Opening older file - it will be converted to latest format, automatically "
		 "assigning weight percentages to constraints and dropping parity for activities. "
		 "You are adviced to make a backup of your old file before saving in new format.\n\n"
		 "Please note that the default weight percentage of constraints min n days between activities "
		 "will be 95% (mainly satisfied, not always) and 'force consecutive if same day' will be set to true "
		 "(meaning that if the activities are in the same day, they will be placed continuously, in a bigger duration activity)"
		 "If you want, you can modify this percent to be 100%, manually in the fet input file "
		 "or from the interface"));
	}
	
	if(warning){
		QMessageBox::warning(NULL, QObject::tr("FET information"), 
		 QObject::tr("Opening a newer FET version file ... file will be opened but it is recommended to update your FET software to the latest version"));
	}
	
	this->nHoursPerDay=12;
	this->hoursOfTheDay[0]="08:00";
	this->hoursOfTheDay[1]="09:00";
	this->hoursOfTheDay[2]="10:00";
	this->hoursOfTheDay[3]="11:00";
	this->hoursOfTheDay[4]="12:00";
	this->hoursOfTheDay[5]="13:00";
	this->hoursOfTheDay[6]="14:00";
	this->hoursOfTheDay[7]="15:00";
	this->hoursOfTheDay[8]="16:00";
	this->hoursOfTheDay[9]="17:00";
	this->hoursOfTheDay[10]="18:00";
	this->hoursOfTheDay[11]="19:00";
	this->hoursOfTheDay[12]="20:00";

	this->nDaysPerWeek=5;
	this->daysOfTheWeek[0] = QObject::tr("Monday");
	this->daysOfTheWeek[1] = QObject::tr("Tuesday");
	this->daysOfTheWeek[2] = QObject::tr("Wednesday");
	this->daysOfTheWeek[3] = QObject::tr("Thursday");
	this->daysOfTheWeek[4] = QObject::tr("Friday");

	this->institutionName=QObject::tr("Default institution");
	this->comments=QObject::tr("Default comments");

	bool skipDeprecatedConstraints=false;

	for(QDomNode node2=elem1.firstChild(); !node2.isNull(); node2=node2.nextSibling()){
		QDomElement elem2=node2.toElement();
		if(elem2.isNull())
			xmlReadingLog+="  Null node here\n";
		xmlReadingLog+="  Found "+elem2.tagName()+" tag\n";
		if(elem2.tagName()=="Institution_Name"){
			this->institutionName=elem2.text();
			xmlReadingLog+="  Found institution name="+this->institutionName+"\n";
		}
		else if(elem2.tagName()=="Comments"){
			this->comments=elem2.text();
			xmlReadingLog+="  Found comments="+this->comments+"\n";
		}
		else if(elem2.tagName()=="Hours_List"){
			int tmp=0;
			for(QDomNode node3=elem2.firstChild(); !node3.isNull(); node3=node3.nextSibling()){
				QDomElement elem3=node3.toElement();
				if(elem3.isNull()){
					xmlReadingLog+="   Null node here\n";
					continue;
				}
				xmlReadingLog+="   Found "+elem3.tagName()+" tag\n";
				if(elem3.tagName()=="Number"){
					this->nHoursPerDay=elem3.text().toInt();
					xmlReadingLog+="   Found the number of hours per day = "+
					 QString::number(this->nHoursPerDay)+"\n";
					assert(this->nHoursPerDay>0);
				}
				else if(elem3.tagName()=="Name"){
					this->hoursOfTheDay[tmp]=elem3.text();
					xmlReadingLog+="   Found hour "+this->hoursOfTheDay[tmp]+"\n";
					tmp++;
				}
			}
		}
		else if(elem2.tagName()=="Days_List"){
			int tmp=0;
			for(QDomNode node3=elem2.firstChild(); !node3.isNull(); node3=node3.nextSibling()){
				QDomElement elem3=node3.toElement();
				if(elem3.isNull()){
					xmlReadingLog+="   Null node here\n";
					continue;
				}
				xmlReadingLog+="   Found "+elem3.tagName()+" tag\n";
				if(elem3.tagName()=="Number"){
					this->nDaysPerWeek=elem3.text().toInt();
					xmlReadingLog+="   Found the number of days per week = "+
					 QString::number(this->nDaysPerWeek)+"\n";
					assert(this->nDaysPerWeek>0);
				}
				else if(elem3.tagName()=="Name"){
					this->daysOfTheWeek[tmp]=elem3.text();
					xmlReadingLog+="   Found day "+this->daysOfTheWeek[tmp]+"\n";
					tmp++;
				}
			}
			assert(tmp==this->nDaysPerWeek);
		}
		else if(elem2.tagName()=="Teachers_List"){
			int tmp=0;
			for(QDomNode node3=elem2.firstChild(); !node3.isNull(); node3=node3.nextSibling()){
				QDomElement elem3=node3.toElement();
				if(elem3.isNull()){
					xmlReadingLog+="   Null node here\n";
					continue;
				}
				xmlReadingLog+="   Found "+elem3.tagName()+" tag\n";
				if(elem3.tagName()=="Teacher"){
					Teacher* teacher=new Teacher();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Name"){
							teacher->name=elem4.text();
							xmlReadingLog+="    Read teacher name: "+teacher->name+"\n";
						}
					}
					bool tmp2=this->addTeacher(teacher);
					assert(tmp2==true);
					tmp++;
					xmlReadingLog+="   Teacher added\n";
				}
			}
			assert(tmp==this->teachersList.size());
			xmlReadingLog+="  Added "+QString::number(tmp)+" teachers\n";
		}
		else if(elem2.tagName()=="Subjects_List"){
			int tmp=0;
			for(QDomNode node3=elem2.firstChild(); !node3.isNull(); node3=node3.nextSibling()){
				QDomElement elem3=node3.toElement();
				if(elem3.isNull()){
					xmlReadingLog+="   Null node here\n";
					continue;
				}
				xmlReadingLog+="   Found "+elem3.tagName()+" tag\n";
				if(elem3.tagName()=="Subject"){
					Subject* subject=new Subject();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Name"){
							subject->name=elem4.text();
							xmlReadingLog+="    Read subject name: "+subject->name+"\n";
						}
					}
					bool tmp2=this->addSubject(subject);
					assert(tmp2==true);
					tmp++;
					xmlReadingLog+="   Subject inserted\n";
				}
			}
			assert(tmp==this->subjectsList.size());
			xmlReadingLog+="  Added "+QString::number(tmp)+" subjects\n";
		}
		else if(elem2.tagName()=="Subject_Tags_List"){
			int tmp=0;
			for(QDomNode node3=elem2.firstChild(); !node3.isNull(); node3=node3.nextSibling()){
				QDomElement elem3=node3.toElement();
				if(elem3.isNull()){
					xmlReadingLog+="   Null node here\n";
					continue;
				}
				xmlReadingLog+="   Found "+elem3.tagName()+" tag\n";
				if(elem3.tagName()=="Subject_Tag"){
					SubjectTag* subjectTag=new SubjectTag();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Name"){
							subjectTag->name=elem4.text();
							xmlReadingLog+="    Read subject tag name: "+subjectTag->name+"\n";
						}
					}
					bool tmp2=this->addSubjectTag(subjectTag);
					assert(tmp2==true);
					tmp++;
					xmlReadingLog+="   Subject tag inserted\n";
				}
			}
			assert(tmp==this->subjectTagsList.size());
			xmlReadingLog+="  Added "+QString::number(tmp)+" subject tags\n";
		}
		else if(elem2.tagName()=="Students_List"){
			int ny=0;
			for(QDomNode node3=elem2.firstChild(); !node3.isNull(); node3=node3.nextSibling()){
				QDomElement elem3=node3.toElement();
				if(elem3.isNull()){
					xmlReadingLog+="   Null node here\n";
					continue;
				}
				xmlReadingLog+="   Found "+elem3.tagName()+" tag\n";
				if(elem3.tagName()=="Year"){
					StudentsYear* sty=new StudentsYear();
					int ng=0;

					bool tmp2=this->addYear(sty);
					assert(tmp2==true);
					ny++;

					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Name"){
							sty->name=elem4.text();
							xmlReadingLog+="    Read year name: "+sty->name+"\n";
						}
						else if(elem4.tagName()=="Number_of_Students"){
							sty->numberOfStudents=elem4.text().toInt();
							xmlReadingLog+="    Read year number of students: "+QString::number(sty->numberOfStudents)+"\n";
						}
						else if(elem4.tagName()=="Group"){
							StudentsGroup* stg=new StudentsGroup();
							int ns=0;

							bool tmp4=this->addGroup(sty->name, stg);
							assert(tmp4==true);
							ng++;

							for(QDomNode node5=elem4.firstChild(); !node5.isNull(); node5=node5.nextSibling()){
								QDomElement elem5=node5.toElement();
								if(elem5.isNull()){
									xmlReadingLog+="     Null node here\n";
									continue;
								}
								xmlReadingLog+="     Found "+elem5.tagName()+" tag\n";
								if(elem5.tagName()=="Name"){
									stg->name=elem5.text();
									xmlReadingLog+="     Read group name: "+stg->name+"\n";
								}
								else if(elem5.tagName()=="Number_of_Students"){
									stg->numberOfStudents=elem5.text().toInt();
									xmlReadingLog+="     Read group number of students: "+QString::number(stg->numberOfStudents)+"\n";
								}
								else if(elem5.tagName()=="Subgroup"){
									StudentsSubgroup* sts=new StudentsSubgroup();

									bool tmp6=this->addSubgroup(sty->name, stg->name, sts);
									assert(tmp6==true);
									ns++;

									for(QDomNode node6=elem5.firstChild(); !node6.isNull(); node6=node6.nextSibling()){
										QDomElement elem6=node6.toElement();
										if(elem6.isNull()){
											xmlReadingLog+="     Null node here\n";
											continue;
										}
										xmlReadingLog+="     Found "+elem6.tagName()+" tag\n";
										if(elem6.tagName()=="Name"){
											sts->name=elem6.text();
											xmlReadingLog+="     Read subgroup name: "+sts->name+"\n";
										}
										else if(elem6.tagName()=="Number_of_Students"){
											sts->numberOfStudents=elem6.text().toInt();
											xmlReadingLog+="     Read subgroup number of students: "+QString::number(sts->numberOfStudents)+"\n";
										}
									}
								}
							}
							assert(ns == stg->subgroupsList.size());
							xmlReadingLog+="    Added "+QString::number(ns)+" subgroups\n";
						}
					}
					assert(ng == sty->groupsList.size());
					xmlReadingLog+="   Added "+QString::number(ng)+" groups\n";
				}
			}
			xmlReadingLog+="  Added "+QString::number(ny)+" years\n";
			assert(this->yearsList.size()==ny);
		}
		else if(elem2.tagName()=="Activities_List"){
			int na=0;
			for(QDomNode node3=elem2.firstChild(); !node3.isNull(); node3=node3.nextSibling()){
				QDomElement elem3=node3.toElement();
				if(elem3.isNull()){
					xmlReadingLog+="   Null node here\n";
					continue;
				}
				xmlReadingLog+="   Found "+elem3.tagName()+" tag\n";
				if(elem3.tagName()=="Activity"){
					bool correct=true;
				
					QString tn="";
					QStringList tl;
					QString sjn="";
					QString stgn="";
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
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="   Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weekly"){
							xmlReadingLog+="    Current activity is weekly - ignoring tag\n";
							//assert(p==PARITY_NOT_INITIALIZED);
							//p=PARITY_WEEKLY;
						}
						//old tag
						else if(elem4.tagName()=="Biweekly"){
							xmlReadingLog+="    Current activity is fortnightly - ignoring tag\n";
							//assert(p==PARITY_NOT_INITIALIZED);
							//p=PARITY_FORTNIGHTLY;
						}
						else if(elem4.tagName()=="Fortnightly"){
							xmlReadingLog+="    Current activity is fortnightly - ignoring tag\n";
							//assert(p==PARITY_NOT_INITIALIZED);
							//p=PARITY_FORTNIGHTLY;
						}
						else if(elem4.tagName()=="Active"){
							if(elem4.text()=="yes"){
								ac=true;
								xmlReadingLog+="	Current activity is active\n";
							}
							else{
								assert(elem4.text()=="no");
								ac=false;
								xmlReadingLog+="	Current activity is not active\n";
							}
						}
						else if(elem4.tagName()=="Teacher"){
							tn=elem4.text();
							xmlReadingLog+="    Crt. activity teacher="+tn+"\n";
							tl.append(tn);
							if(this->searchTeacher(tn)<0)
								correct=false;
						}
						else if(elem4.tagName()=="Subject"){
							sjn=elem4.text();
							xmlReadingLog+="    Crt. activity subject="+sjn+"\n";
							if(this->searchSubject(sjn)<0)
								correct=false;
						}
						else if(elem4.tagName()=="Subject_Tag"){
							stgn=elem4.text();
							xmlReadingLog+="    Crt. activity subject tag="+stgn+"\n";
							if(this->searchSubjectTag(stgn)<0 && stgn!="")
								correct=false;
						}
						else if(elem4.tagName()=="Students"){
							stn=elem4.text();
							xmlReadingLog+="    Crt. activity students+="+stn+"\n";
							stl.append(stn);
							if(this->searchStudentsSet(stn)==NULL)
								correct=false;
						}
						else if(elem4.tagName()=="Duration"){
							d=elem4.text().toInt();
							xmlReadingLog+="    Crt. activity duration="+QString::number(d)+"\n";
						}
						else if(elem4.tagName()=="Total_Duration"){
							td=elem4.text().toInt();
							xmlReadingLog+="    Crt. activity total duration="+QString::number(td)+"\n";
						}
						else if(elem4.tagName()=="Id"){
							id=elem4.text().toInt();
							xmlReadingLog+="    Crt. activity id="+QString::number(id)+"\n";
						}
						else if(elem4.tagName()=="Activity_Group_Id"){
							gid=elem4.text().toInt();
							xmlReadingLog+="    Crt. activity group id="+QString::number(gid)+"\n";
						}
						else if(elem4.tagName()=="Number_Of_Students"){
							cnos=false;
							nos=elem4.text().toInt();
							xmlReadingLog+="    Crt. activity number of students="+QString::number(nos)+"\n";
						}
					}
					if(correct){
						assert(id>=0 && gid>=0);
						assert(d>0);
						if(td<0)
							td=d;
						this->addSimpleActivity(id, gid, tl, sjn, stgn, stl,
							d, td, /*p,*/ ac, /*-1, -1,*/ cnos, nos);
						na++;
						xmlReadingLog+="   Added the activity\n";
					}
					else{
						xmlReadingLog+="   Activity with id ="+QString::number(id)+" contains invalid data - skipping\n";
						QMessageBox::warning(NULL, QObject::tr("FET information"), 
						 QObject::tr("Activity with id=%1 contains invalid data - skipping").arg(id));
					}
				}
			}
			xmlReadingLog+="  Added "+QString::number(na)+" activities\n";
		}
		else if(elem2.tagName()=="Equipments_List"){
		 	QMessageBox::warning(NULL, QObject::tr("FET warning"),
			 QObject::tr("File contains deprecated equipments list - will be ignored\n"));
		
			/*int tmp=0;
			for(QDomNode node3=elem2.firstChild(); !node3.isNull(); node3=node3.nextSibling()){
				QDomElement elem3=node3.toElement();
				if(elem3.isNull()){
					xmlReadingLog+="   Null node here\n";
					continue;
				}
				xmlReadingLog+="   Found "+elem3.tagName()+" tag\n";
				if(elem3.tagName()=="Equipment"){
					Equipment* eq=new Equipment();
					eq->name="";
					
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Name"){
							eq->name=elem4.text();
							xmlReadingLog+="    Read equipment name: "+eq->name+"\n";
						}
					}
					bool tmp2=this->addEquipment(eq);
					assert(tmp2==true);
					tmp++;
					xmlReadingLog+="   Equipment added\n";
				}
			}
			assert(tmp==this->equipmentsList.size());
			xmlReadingLog+="  Added "+QString::number(tmp)+" equipments\n";*/
		}
		else if(elem2.tagName()=="Buildings_List"){
		 	QMessageBox::warning(NULL, QObject::tr("FET warning"),
			 QObject::tr("File contains deprecated buildings list - will be ignored\n"));
			/*
			int tmp=0;
			for(QDomNode node3=elem2.firstChild(); !node3.isNull(); node3=node3.nextSibling()){
				QDomElement elem3=node3.toElement();
				if(elem3.isNull()){
					xmlReadingLog+="   Null node here\n";
					continue;
				}
				xmlReadingLog+="   Found "+elem3.tagName()+" tag\n";
				if(elem3.tagName()=="Building"){
					Building* bu=new Building();
					bu->name="";
					
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Name"){
							bu->name=elem4.text();
							xmlReadingLog+="    Read building name: "+bu->name+"\n";
						}
					}
					bool tmp2=this->addBuilding(bu);
					assert(tmp2==true);
					tmp++;
					xmlReadingLog+="   Building added\n";
				}
			}
			assert(tmp==this->buildingsList.size());
			xmlReadingLog+="  Added "+QString::number(tmp)+" buildings\n";*/
		}
		else if(elem2.tagName()=="Rooms_List"){
			int tmp=0;
			for(QDomNode node3=elem2.firstChild(); !node3.isNull(); node3=node3.nextSibling()){
				QDomElement elem3=node3.toElement();
				if(elem3.isNull()){
					xmlReadingLog+="   Null node here\n";
					continue;
				}
				xmlReadingLog+="   Found "+elem3.tagName()+" tag\n";
				if(elem3.tagName()=="Room"){
					Room* rm=new Room();
					rm->name="";
					//rm->type="";
					rm->capacity=1000; //infinite, if not specified
					//rm->building="";
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Name"){
							rm->name=elem4.text();
							xmlReadingLog+="    Read room name: "+rm->name+"\n";
						}
						else if(elem4.tagName()=="Type"){
							//rm->type=elem4.text();
							xmlReadingLog+="    Ignoring old tag room type:\n";
						}
						else if(elem4.tagName()=="Capacity"){
							rm->capacity=elem4.text().toInt();
							xmlReadingLog+="    Read room capacity: "+QString::number(rm->capacity)+"\n";
						}
						else if(elem4.tagName()=="Equipment"){
							//rm->addEquipment(elem4.text());
							xmlReadingLog+="    Ignoring old tag - room equipment:\n";
						}
						else if(elem4.tagName()=="Building"){
							//rm->building=elem4.text();
							xmlReadingLog+="    Ignoring old tag - room building:\n";
						}
					}
					bool tmp2=this->addRoom(rm);
					if(!tmp2){
						cout<<"Duplicate room - "<<(const char*)rm->name<<endl;
						assert(0);
						exit(1);
					}
					assert(tmp2==true);
					tmp++;
					xmlReadingLog+="   Room added\n";
				}
			}
			assert(tmp==this->roomsList.size());
			xmlReadingLog+="  Added "+QString::number(tmp)+" rooms\n";
		}
		else if(elem2.tagName()=="Time_Constraints_List"){
			int nc=0;
			TimeConstraint *crt_constraint;
			for(QDomNode node3=elem2.firstChild(); !node3.isNull(); node3=node3.nextSibling()){
				crt_constraint=NULL;
				QDomElement elem3=node3.toElement();
				if(elem3.isNull()){
					xmlReadingLog+="   Null node here\n";
					continue;
				}
				xmlReadingLog+="   Found "+elem3.tagName()+" tag\n";
				if(elem3.tagName()=="ConstraintBasicCompulsoryTime"){
					ConstraintBasicCompulsoryTime* cn=new ConstraintBasicCompulsoryTime();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - generating automatic 100% weight percentage\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintTeacherNotAvailable"){
					ConstraintTeacherNotAvailable* cn=new ConstraintTeacherNotAvailable();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight. Generating 100% weight percentage\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="Teacher_Name"){
							cn->teacherName=elem4.text();
							xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
						}
						else if(elem4.tagName()=="Day"){
							for(cn->d=0; cn->d<this->nDaysPerWeek; cn->d++)
								if(this->daysOfTheWeek[cn->d]==elem4.text())
									break;
							if(cn->d>=this->nDaysPerWeek){
								QMessageBox::information(NULL, QObject::tr("FET information"), 
								 QObject::tr("Constraint TeacherNotAvailable day corrupt for teacher %1, day %2 is inexistent ... ignoring constraint")
								 .arg(cn->teacherName)
								 .arg(elem4.text()));
								cn=NULL;
								goto corruptConstraintTime;
							}
							assert(cn->d<this->nDaysPerWeek);
							xmlReadingLog+="    Crt. day="+this->daysOfTheWeek[cn->d]+"\n";
						}
						else if(elem4.tagName()=="Start_Hour"){
							for(cn->h1=0; cn->h1 < this->nHoursPerDay; cn->h1++)
								if(this->hoursOfTheDay[cn->h1]==elem4.text())
									break;
							if(cn->h1>=this->nHoursPerDay){
								QMessageBox::information(NULL, QObject::tr("FET information"), 
								 QObject::tr("Constraint TeacherNotAvailable start hour corrupt for teacher %1, hour %2 is inexistent ... ignoring constraint")
								 .arg(cn->teacherName)
								 .arg(elem4.text()));
								cn=NULL;
								goto corruptConstraintTime;
							}
							assert(cn->h1>=0 && cn->h1 < this->nHoursPerDay);
							xmlReadingLog+="    Start hour="+this->hoursOfTheDay[cn->h1]+"\n";
						}
						else if(elem4.tagName()=="End_Hour"){
							for(cn->h2=0; cn->h2 < this->nHoursPerDay; cn->h2++)
								if(this->hoursOfTheDay[cn->h2]==elem4.text())
									break;
							if(cn->h2<=0 || cn->h2>this->nHoursPerDay){
								QMessageBox::information(NULL, QObject::tr("FET information"), 
								 QObject::tr("Constraint TeacherNotAvailable end hour corrupt for teacher %1, hour %2 is inexistent ... ignoring constraint")
								 .arg(cn->teacherName)
								 .arg(elem4.text()));
								cn=NULL;
								goto corruptConstraintTime;
							}
							assert(cn->h2>0 && cn->h2 <= this->nHoursPerDay);
							xmlReadingLog+="    End hour="+this->hoursOfTheDay[cn->h2]+"\n";
						}
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintTeacherMaxDaysPerWeek"){
					ConstraintTeacherMaxDaysPerWeek* cn=new ConstraintTeacherMaxDaysPerWeek();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - generating 100% weight percentage\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="Teacher_Name"){
							cn->teacherName=elem4.text();
							xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
						}
						else if(elem4.tagName()=="Max_Days_Per_Week"){
							cn->maxDaysPerWeek=elem4.text().toInt();
							if(cn->maxDaysPerWeek<=0 || cn->maxDaysPerWeek>this->nDaysPerWeek){
								QMessageBox::information(NULL, QObject::tr("FET information"), 
								 QObject::tr("Constraint TeacherMaxDaysPerWeek day corrupt for teacher %1, max days %2 <= 0 or >nDaysPerWeek, ignoring constraint")
								 .arg(cn->teacherName)
								 .arg(elem4.text()));
								cn=NULL;
								goto corruptConstraintTime;
							}
							assert(cn->maxDaysPerWeek>0 && cn->maxDaysPerWeek <= this->nDaysPerWeek);
							xmlReadingLog+="    Max. days per week="+QString::number(cn->maxDaysPerWeek)+"\n";
						}
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintStudentsSetNotAvailable"){
					ConstraintStudentsSetNotAvailable* cn=new ConstraintStudentsSetNotAvailable();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - adding weight percentage=100%\n";
							cn->weightPercentage=100;
						}
						if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="Day"){
							for(cn->d=0; cn->d<this->nDaysPerWeek; cn->d++)
								if(this->daysOfTheWeek[cn->d]==elem4.text())
									break;
							if(cn->d>=this->nDaysPerWeek){
								QMessageBox::information(NULL, QObject::tr("FET information"), 
								 QObject::tr("Constraint StudentsSetNotAvailable day corrupt for students %1, day %2 is inexistent ... ignoring constraint")
								 .arg(cn->students)
								 .arg(elem4.text()));
								cn=NULL;
								goto corruptConstraintTime;
							}
							assert(cn->d<this->nDaysPerWeek);
							xmlReadingLog+="    Crt. day="+this->daysOfTheWeek[cn->d]+"\n";
						}
						else if(elem4.tagName()=="Start_Hour"){
							for(cn->h1=0; cn->h1 < this->nHoursPerDay; cn->h1++)
								if(this->hoursOfTheDay[cn->h1]==elem4.text())
									break;
							if(cn->h1>=this->nHoursPerDay){
								QMessageBox::information(NULL, QObject::tr("FET information"), 
								 QObject::tr("Constraint StudentsSetNotAvailable start hour corrupt for students set %1, hour %2 is inexistent ... ignoring constraint")
								 .arg(cn->students)
								 .arg(elem4.text()));
								cn=NULL;
								goto corruptConstraintTime;
							}
							assert(cn->h1>=0 && cn->h1 < this->nHoursPerDay);
							xmlReadingLog+="    Start hour="+this->hoursOfTheDay[cn->h1]+"\n";
						}
						else if(elem4.tagName()=="End_Hour"){
							for(cn->h2=0; cn->h2 < this->nHoursPerDay; cn->h2++)
								if(this->hoursOfTheDay[cn->h2]==elem4.text())
									break;
							if(cn->h2<=0 || cn->h2>this->nHoursPerDay){
								QMessageBox::information(NULL, QObject::tr("FET information"), 
								 QObject::tr("Constraint StudentsSetNotAvailable end hour corrupt for students %1, hour %2 is inexistent ... ignoring constraint")
								 .arg(cn->students)
								 .arg(elem4.text()));
								cn=NULL;
								goto corruptConstraintTime;
							}
							assert(cn->h2>0 && cn->h2 <= this->nHoursPerDay);
							xmlReadingLog+="    End hour="+this->hoursOfTheDay[cn->h2]+"\n";
						}
						else if(elem4.tagName()=="Students"){
							cn->students=elem4.text();
							xmlReadingLog+="    Read students name="+cn->students+"\n";
						}
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintMinNDaysBetweenActivities"){
					ConstraintMinNDaysBetweenActivities* cn=new ConstraintMinNDaysBetweenActivities();
					bool foundCISD=false;
					int n_act=0;
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - generating weightPercentage=95%\n";
							cn->weightPercentage=95;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weightPercentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Consecutive_If_Same_Day" || elem4.tagName()=="Adjacent_If_Broken"){
							if(elem4.text()=="yes"){
								cn->consecutiveIfSameDay=true;
								foundCISD=true;
								xmlReadingLog+="    Current constraint has consecutive if same day=true\n";
							}
							else{
								cn->consecutiveIfSameDay=false;
								foundCISD=true;
								xmlReadingLog+="    Current constraint has consecutive if same day=false\n";
							}
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="Number_of_Activities"){
							cn->n_activities=elem4.text().toInt();
							xmlReadingLog+="    Read n_activities="+QString::number(cn->n_activities)+"\n";
						}
						else if(elem4.tagName()=="Activity_Id"){
							cn->activitiesId[n_act]=elem4.text().toInt();
							xmlReadingLog+="    Read activity id="+QString::number(cn->activitiesId[n_act])+"\n";
							n_act++;
						}
						else if(elem4.tagName()=="MinDays"){
							cn->minDays=elem4.text().toInt();
							xmlReadingLog+="    Read MinDays="+QString::number(cn->minDays)+"\n";
						}
					}
					if(!foundCISD){
						xmlReadingLog+="    Could not find consecutive if same day information - making it true\n";
						cn->consecutiveIfSameDay=true;
					}
					assert(n_act==cn->n_activities);
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintActivitiesNotOverlapping"){
					ConstraintActivitiesNotOverlapping* cn=new ConstraintActivitiesNotOverlapping();
					int n_act=0;
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="Number_of_Activities"){
							cn->n_activities=elem4.text().toInt();
							xmlReadingLog+="    Read n_activities="+QString::number(cn->n_activities)+"\n";
						}
						else if(elem4.tagName()=="Activity_Id"){
							cn->activitiesId[n_act]=elem4.text().toInt();
							xmlReadingLog+="    Read activity id="+QString::number(cn->activitiesId[n_act])+"\n";
							n_act++;
						}
					}
					assert(n_act==cn->n_activities);
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintActivitiesSameStartingTime"){
					ConstraintActivitiesSameStartingTime* cn=new ConstraintActivitiesSameStartingTime();
					int n_act=0;
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="Number_of_Activities"){
							cn->n_activities=elem4.text().toInt();
							xmlReadingLog+="    Read n_activities="+QString::number(cn->n_activities)+"\n";
						}
						else if(elem4.tagName()=="Activity_Id"){
							cn->activitiesId[n_act]=elem4.text().toInt();
							xmlReadingLog+="    Read activity id="+QString::number(cn->activitiesId[n_act])+"\n";
							n_act++;
						}
					}
					assert(cn->n_activities==n_act);
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintActivitiesSameStartingHour"){
					ConstraintActivitiesSameStartingHour* cn=new ConstraintActivitiesSameStartingHour();
					int n_act=0;
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="Number_of_Activities"){
							cn->n_activities=elem4.text().toInt();
							xmlReadingLog+="    Read n_activities="+QString::number(cn->n_activities)+"\n";
						}
						else if(elem4.tagName()=="Activity_Id"){
							cn->activitiesId[n_act]=elem4.text().toInt();
							xmlReadingLog+="    Read activity id="+QString::number(cn->activitiesId[n_act])+"\n";
							n_act++;
						}
					}
					assert(cn->n_activities==n_act);
					crt_constraint=cn;
				}
				if((elem3.tagName()=="ConstraintTeachersMaxHoursContinuously"
				 //TODO: erase the line below. It is only kept for compatibility with older versions
				 || elem3.tagName()=="ConstraintTeachersNoMoreThanXHoursContinuously") && !skipDeprecatedConstraints){
				 
				 	int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("File contains deprecated constraint teachers max hours continuously - will be ignored\n"),
					 "Skip rest of deprecated constraints", "See next deprecated constraint", QString(),
					 1, 0 );
													 
					if(t==0)
						skipDeprecatedConstraints=true;

				 /*
					ConstraintTeachersMaxHoursContinuously* cn=new ConstraintTeachersMaxHoursContinuously();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="Maximum_Hours_Continuously"){
							cn->maxHoursContinuously=elem4.text().toInt();
							xmlReadingLog+="    Read maxHoursContinuously="+QString::number(cn->maxHoursContinuously)+"\n";
						}
					}
					crt_constraint=cn;*/
					crt_constraint=NULL;
				}
				if(elem3.tagName()=="ConstraintTeachersMaxHoursDaily"){
					ConstraintTeachersMaxHoursDaily* cn=new ConstraintTeachersMaxHoursDaily();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="Maximum_Hours_Daily"){
							cn->maxHoursDaily=elem4.text().toInt();
							xmlReadingLog+="    Read maxHoursDaily="+QString::number(cn->maxHoursDaily)+"\n";
						}
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintTeacherMaxHoursDaily"){
					ConstraintTeacherMaxHoursDaily* cn=new ConstraintTeacherMaxHoursDaily();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="Maximum_Hours_Daily"){
							cn->maxHoursDaily=elem4.text().toInt();
							xmlReadingLog+="    Read maxHoursDaily="+QString::number(cn->maxHoursDaily)+"\n";
						}
						else if(elem4.tagName()=="Teacher_Name"){
							cn->teacherName=elem4.text();
							xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
						}
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintTeachersMinHoursDaily"){
					ConstraintTeachersMinHoursDaily* cn=new ConstraintTeachersMinHoursDaily();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="Minimum_Hours_Daily"){
							cn->minHoursDaily=elem4.text().toInt();
							xmlReadingLog+="    Read minHoursDaily="+QString::number(cn->minHoursDaily)+"\n";
						}
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintTeacherMinHoursDaily"){
					ConstraintTeacherMinHoursDaily* cn=new ConstraintTeacherMinHoursDaily();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="Minimum_Hours_Daily"){
							cn->minHoursDaily=elem4.text().toInt();
							xmlReadingLog+="    Read minHoursDaily="+QString::number(cn->minHoursDaily)+"\n";
						}
						else if(elem4.tagName()=="Teacher_Name"){
							cn->teacherName=elem4.text();
							xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
						}
					}
					crt_constraint=cn;
				}
				if((elem3.tagName()=="ConstraintTeachersSubgroupsMaxHoursDaily"
				 //TODO: erase the line below. It is only kept for compatibility with older versions
				 || elem3.tagName()=="ConstraintTeachersSubgroupsNoMoreThanXHoursDaily") && !skipDeprecatedConstraints){
					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("File contains deprecated constraint teachers subgroups max hours daily - will be ignored\n"),
					 "Skip rest of deprecated constraints", "See next deprecated constraint", QString(),
					 1, 0 );
													 
					if(t==0)
						skipDeprecatedConstraints=true;
					/*
					ConstraintTeachersSubgroupsMaxHoursDaily* cn=new ConstraintTeachersSubgroupsMaxHoursDaily();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="Maximum_Hours_Daily"){
							cn->maxHoursDaily=elem4.text().toInt();
							xmlReadingLog+="    Read maxHoursDaily="+QString::number(cn->maxHoursDaily)+"\n";
						}
					}
					crt_constraint=cn;
					*/
					crt_constraint=NULL;
				}
				if(elem3.tagName()=="ConstraintStudentsNHoursDaily" && !skipDeprecatedConstraints){
					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("File contains deprecated constraint students n hours daily - will be ignored\n"),
					 "Skip rest of deprecated constraints", "See next deprecated constraint", QString(),
					 1, 0 );
													 
					if(t==0)
						skipDeprecatedConstraints=true;
					/*ConstraintStudentsNHoursDaily* cn=new ConstraintStudentsNHoursDaily();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="MaxHoursDaily"){
							cn->maxHoursDaily=elem4.text().toInt();
							xmlReadingLog+="    Read maxHoursDaily="+QString::number(cn->maxHoursDaily)+"\n";
						}
						else if(elem4.tagName()=="MinHoursDaily"){
							cn->minHoursDaily=elem4.text().toInt();
							xmlReadingLog+="    Read minHoursDaily="+QString::number(cn->minHoursDaily)+"\n";
						}
					}
					assert(cn->maxHoursDaily>=0 || cn->minHoursDaily>=0);
					crt_constraint=cn;*/
					crt_constraint=NULL;
				}
				if(elem3.tagName()=="ConstraintStudentsSetNHoursDaily" && !skipDeprecatedConstraints){
					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("File contains deprecated constraint students set n hours daily - will be ignored\n"),
					 "Skip rest of deprecated constraints", "See next deprecated constraint", QString(),
					 1, 0 );
													 
					if(t==0)
						skipDeprecatedConstraints=true;
					/*ConstraintStudentsSetNHoursDaily* cn=new ConstraintStudentsSetNHoursDaily();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="MaxHoursDaily"){
							cn->maxHoursDaily=elem4.text().toInt();
							xmlReadingLog+="    Read maxHoursDaily="+QString::number(cn->maxHoursDaily)+"\n";
						}
						else if(elem4.tagName()=="MinHoursDaily"){
							cn->minHoursDaily=elem4.text().toInt();
							xmlReadingLog+="    Read minHoursDaily="+QString::number(cn->minHoursDaily)+"\n";
						}
						else if(elem4.tagName()=="Students"){
							cn->students=elem4.text();
							xmlReadingLog+="    Read students name="+cn->students+"\n";
						}
					}
					assert(cn->maxHoursDaily>=0 || cn->minHoursDaily>=0);
					crt_constraint=cn;*/
					crt_constraint=NULL;
				}
				if(elem3.tagName()=="ConstraintStudentsMaxHoursDaily"){
					ConstraintStudentsMaxHoursDaily* cn=new ConstraintStudentsMaxHoursDaily();
					cn->maxHoursDaily=-1;
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="Maximum_Hours_Daily"){
							cn->maxHoursDaily=elem4.text().toInt();
							xmlReadingLog+="    Read maxHoursDaily="+QString::number(cn->maxHoursDaily)+"\n";
						}
					}
					assert(cn->maxHoursDaily>=0);
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintStudentsSetMaxHoursDaily"){
					ConstraintStudentsSetMaxHoursDaily* cn=new ConstraintStudentsSetMaxHoursDaily();
					cn->maxHoursDaily=-1;
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="Maximum_Hours_Daily"){
							cn->maxHoursDaily=elem4.text().toInt();
							xmlReadingLog+="    Read maxHoursDaily="+QString::number(cn->maxHoursDaily)+"\n";
						}
						else if(elem4.tagName()=="Students"){
							cn->students=elem4.text();
							xmlReadingLog+="    Read students name="+cn->students+"\n";
						}
					}
					assert(cn->maxHoursDaily>=0);
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintStudentsMinHoursDaily"){
					ConstraintStudentsMinHoursDaily* cn=new ConstraintStudentsMinHoursDaily();
					cn->minHoursDaily=-1;
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="Minimum_Hours_Daily"){
							cn->minHoursDaily=elem4.text().toInt();
							xmlReadingLog+="    Read minHoursDaily="+QString::number(cn->minHoursDaily)+"\n";
						}
					}
					assert(cn->minHoursDaily>=0);
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintStudentsSetMinHoursDaily"){
					ConstraintStudentsSetMinHoursDaily* cn=new ConstraintStudentsSetMinHoursDaily();
					cn->minHoursDaily=-1;
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="Minimum_Hours_Daily"){
							cn->minHoursDaily=elem4.text().toInt();
							xmlReadingLog+="    Read minHoursDaily="+QString::number(cn->minHoursDaily)+"\n";
						}
						else if(elem4.tagName()=="Students"){
							cn->students=elem4.text();
							xmlReadingLog+="    Read students name="+cn->students+"\n";
						}
					}
					assert(cn->minHoursDaily>=0);
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintActivityPreferredTime"){
					ConstraintActivityPreferredTime* cn=new ConstraintActivityPreferredTime();
					cn->day = cn->hour = -1;
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="Activity_Id"){
							cn->activityId=elem4.text().toInt();
							xmlReadingLog+="    Read activity id="+QString::number(cn->activityId)+"\n";
						}
						else if(elem4.tagName()=="Preferred_Day"){
							for(cn->day=0; cn->day<this->nDaysPerWeek; cn->day++)
								if(this->daysOfTheWeek[cn->day]==elem4.text())
									break;
							if(cn->day>=this->nDaysPerWeek){
								QMessageBox::information(NULL, QObject::tr("FET information"), 
								 QObject::tr("Constraint ActivityPreferredTime day corrupt for activity with id %1, day %2 is inexistent ... ignoring constraint")
								 .arg(cn->activityId)
								 .arg(elem4.text()));
								cn=NULL;
								goto corruptConstraintTime;
							}
							assert(cn->day<this->nDaysPerWeek);
							xmlReadingLog+="    Preferred day="+this->daysOfTheWeek[cn->day]+"\n";
						}
						else if(elem4.tagName()=="Preferred_Hour"){
							for(cn->hour=0; cn->hour < this->nHoursPerDay; cn->hour++)
								if(this->hoursOfTheDay[cn->hour]==elem4.text())
									break;
							if(cn->hour>=this->nHoursPerDay){
								QMessageBox::information(NULL, QObject::tr("FET information"), 
								 QObject::tr("Constraint ActivityPreferredTime hour corrupt for activity with id %1, hour %2 is inexistent ... ignoring constraint")
								 .arg(cn->activityId)
								 .arg(elem4.text()));
								cn=NULL;
								goto corruptConstraintTime;
							}
							assert(cn->hour>=0 && cn->hour < this->nHoursPerDay);
							xmlReadingLog+="    Preferred hour="+this->hoursOfTheDay[cn->hour]+"\n";
						}
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintActivityEndsStudentsDay"){
					ConstraintActivityEndsStudentsDay* cn=new ConstraintActivityEndsStudentsDay();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Activity_Id"){
							cn->activityId=elem4.text().toInt();
							xmlReadingLog+="    Read activity id="+QString::number(cn->activityId)+"\n";
						}
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="Constraint2ActivitiesConsecutive"){
					Constraint2ActivitiesConsecutive* cn=new Constraint2ActivitiesConsecutive();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="First_Activity_Id"){
							cn->firstActivityId=elem4.text().toInt();
							xmlReadingLog+="    Read first activity id="+QString::number(cn->firstActivityId)+"\n";
						}
						else if(elem4.tagName()=="Second_Activity_Id"){
							cn->secondActivityId=elem4.text().toInt();
							xmlReadingLog+="    Read second activity id="+QString::number(cn->secondActivityId)+"\n";
						}
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintActivityEndsDay" && !skipDeprecatedConstraints ){
					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("File contains deprecated constraint activity ends day - will be ignored\n"),
					 "Skip rest of deprecated constraints", "See next deprecated constraint", QString(),
					 1, 0 );
													 
					if(t==0)
						skipDeprecatedConstraints=true;
					/*
					ConstraintActivityEndsDay* cn=new ConstraintActivityEndsDay();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="Activity_Id"){
							cn->activityId=elem4.text().toInt();
							xmlReadingLog+="    Read activity id="+QString::number(cn->activityId)+"\n";
						}
					}
					crt_constraint=cn;*/
					crt_constraint=NULL;
				}
				if(elem3.tagName()=="ConstraintActivityPreferredTimes"){
					ConstraintActivityPreferredTimes* cn=new ConstraintActivityPreferredTimes();
					cn->nPreferredTimes=0;
					int i;
					for(i=0; i<MAX_N_CONSTRAINT_ACTIVITY_PREFERRED_TIMES; i++){
						cn->days[i] = cn->hours[i] = -1;
					}
					i=0;
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="Activity_Id"){
							cn->activityId=elem4.text().toInt();
							xmlReadingLog+="    Read activity id="+QString::number(cn->activityId)+"\n";
						}
						else if(elem4.tagName()=="Number_of_Preferred_Times"){
							cn->nPreferredTimes=elem4.text().toInt();
							xmlReadingLog+="    Read number of preferred times="+QString::number(cn->nPreferredTimes)+"\n";
						}
						else if(elem4.tagName()=="Preferred_Time"){
							xmlReadingLog+="    Read: preferred time\n";

							for(QDomNode node5=elem4.firstChild(); !node5.isNull(); node5=node5.nextSibling()){
								QDomElement elem5=node5.toElement();
								if(elem5.isNull()){
									xmlReadingLog+="    Null node here\n";
									continue;
								}
								xmlReadingLog+="    Found "+elem5.tagName()+" tag\n";
								if(elem5.tagName()=="Preferred_Day"){
									for(cn->days[i]=0; cn->days[i]<this->nDaysPerWeek; cn->days[i]++)
										if(this->daysOfTheWeek[cn->days[i]]==elem5.text())
											break;

									if(cn->days[i]>=this->nDaysPerWeek){
										QMessageBox::information(NULL, QObject::tr("FET information"), 
										 QObject::tr("Constraint ActivityPreferredTimes day corrupt for activity with id %1, day %2 is inexistent ... ignoring constraint")
										 .arg(cn->activityId)
										 .arg(elem5.text()));
										cn=NULL;
										goto corruptConstraintTime;
									}
						
									assert(cn->days[i]<this->nDaysPerWeek);
									xmlReadingLog+="    Preferred day="+this->daysOfTheWeek[cn->days[i]]+"("+QString::number(i)+")"+"\n";
								}
								else if(elem5.tagName()=="Preferred_Hour"){
									for(cn->hours[i]=0; cn->hours[i] < this->nHoursPerDay; cn->hours[i]++)
										if(this->hoursOfTheDay[cn->hours[i]]==elem5.text())
											break;
									
									if(cn->hours[i]>=this->nHoursPerDay){
										QMessageBox::information(NULL, QObject::tr("FET information"), 
										 QObject::tr("Constraint ActivityPreferredTimes hour corrupt for activity with id %1, hour %2 is inexistent ... ignoring constraint")
										 .arg(cn->activityId)
										 .arg(elem5.text()));
										cn=NULL;
										goto corruptConstraintTime;
									}
									
									assert(cn->hours[i]>=0 && cn->hours[i] < this->nHoursPerDay);
									xmlReadingLog+="    Preferred hour="+this->hoursOfTheDay[cn->hours[i]]+"\n";
								}
							}

							i++;
						}
					}
					assert(i==cn->nPreferredTimes);
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintBreak"){
					ConstraintBreak* cn=new ConstraintBreak();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="Day"){
							for(cn->d=0; cn->d<this->nDaysPerWeek; cn->d++)
								if(this->daysOfTheWeek[cn->d]==elem4.text())
									break;
									
							if(cn->d>=this->nDaysPerWeek){
								QMessageBox::information(NULL, QObject::tr("FET information"), 
								 QObject::tr("Constraint Break day corrupt,  day %1 is inexistent ... ignoring constraint")
								 .arg(elem4.text()));
								cn=NULL;
								goto corruptConstraintTime;
							}
									
							assert(cn->d<this->nDaysPerWeek);
							xmlReadingLog+="    Crt. day="+this->daysOfTheWeek[cn->d]+"\n";
						}
						else if(elem4.tagName()=="Start_Hour"){
							for(cn->h1=0; cn->h1 < this->nHoursPerDay; cn->h1++)
								if(this->hoursOfTheDay[cn->h1]==elem4.text())
									break;
									
							if(cn->h1>=this->nHoursPerDay){
								QMessageBox::information(NULL, QObject::tr("FET information"), 
								 QObject::tr("Constraint Break start hour corrupt, hour %! is inexistent ... ignoring constraint")
								 .arg(elem4.text()));
								cn=NULL;
								goto corruptConstraintTime;
							}
									
							assert(cn->h1>=0 && cn->h1 < this->nHoursPerDay);
							xmlReadingLog+="    Start hour="+this->hoursOfTheDay[cn->h1]+"\n";
						}
						else if(elem4.tagName()=="End_Hour"){
							for(cn->h2=0; cn->h2 < this->nHoursPerDay; cn->h2++)
								if(this->hoursOfTheDay[cn->h2]==elem4.text())
									break;
									
							if(cn->h2<=0 || cn->h2>this->nHoursPerDay){
								QMessageBox::information(NULL, QObject::tr("FET information"), 
								 QObject::tr("Constraint Break end hour corrupt, hour %1 is inexistent ... ignoring constraint")
								 .arg(elem4.text()));
								cn=NULL;
								goto corruptConstraintTime;
							}

							assert(cn->h2>0 && cn->h2 <= this->nHoursPerDay);
							xmlReadingLog+="    End hour="+this->hoursOfTheDay[cn->h2]+"\n";
						}
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintTeachersNoGaps"){
					ConstraintTeachersMaxGapsPerWeek* cn=new ConstraintTeachersMaxGapsPerWeek();
					cn->maxGaps=0;
					//ConstraintTeachersNoGaps* cn=new ConstraintTeachersNoGaps();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintTeachersMaxGapsPerWeek"){
					ConstraintTeachersMaxGapsPerWeek* cn=new ConstraintTeachersMaxGapsPerWeek();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Max_Gaps"){
							cn->maxGaps=elem4.text().toInt();
							xmlReadingLog+="    Adding max gaps="+QString::number(cn->maxGaps)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintTeacherMaxGapsPerWeek"){
					ConstraintTeacherMaxGapsPerWeek* cn=new ConstraintTeacherMaxGapsPerWeek();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Teacher_Name"){
							cn->teacherName=elem4.text();
							xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Max_Gaps"){
							cn->maxGaps=elem4.text().toInt();
							xmlReadingLog+="    Adding max gaps="+QString::number(cn->maxGaps)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintStudentsNoGaps"){
					ConstraintStudentsNoGaps* cn=new ConstraintStudentsNoGaps();
					//bool compulsory_read=false;
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintStudentsSetNoGaps"){
					ConstraintStudentsSetNoGaps* cn=new ConstraintStudentsSetNoGaps();
					//bool compulsory_read=false;
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="Students"){
							cn->students=elem4.text();
							xmlReadingLog+="    Read students name="+cn->students+"\n";
						}
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintStudentsEarly"){
					ConstraintStudentsEarly* cn=new ConstraintStudentsEarly();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintStudentsSetEarly"){
					ConstraintStudentsSetEarly* cn=new ConstraintStudentsSetEarly();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="Students"){
							cn->students=elem4.text();
							xmlReadingLog+="    Read students name="+cn->students+"\n";
						}
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintStudentsSetIntervalMaxDaysPerWeek" && !skipDeprecatedConstraints){
					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("File contains deprecated constraint students set interval max days per week - will be ignored\n"),
					 "Skip rest of deprecated constraints", "See next deprecated constraint", QString(),
					 1, 0 );
													 
					if(t==0)
						skipDeprecatedConstraints=true;
					/*
					ConstraintStudentsSetIntervalMaxDaysPerWeek* cn=new ConstraintStudentsSetIntervalMaxDaysPerWeek();
					assert(cn!=NULL);
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="Start_Hour"){
							for(cn->h1=0; cn->h1 < this->nHoursPerDay; cn->h1++)
								if(this->hoursOfTheDay[cn->h1]==elem4.text())
									break;
							assert(cn->h1>=0 && cn->h1 < this->nHoursPerDay);
							xmlReadingLog+="    Start hour="+this->hoursOfTheDay[cn->h1]+"\n";
						}
						else if(elem4.tagName()=="End_Hour"){
							for(cn->h2=0; cn->h2 < this->nHoursPerDay; cn->h2++)
								if(this->hoursOfTheDay[cn->h2]==elem4.text())
									break;
							assert(cn->h2>0 && cn->h2 <= this->nHoursPerDay);
							xmlReadingLog+="    End hour="+this->hoursOfTheDay[cn->h2]+"\n";
						}
						else if(elem4.tagName()=="Students"){
							cn->students=elem4.text();
							xmlReadingLog+="    Read students name="+cn->students+"\n";
						}
						if(elem4.tagName()=="Max_Intervals"){
							cn->n=elem4.text().toInt();
							xmlReadingLog+="    Adding max intervals="+QString::number(cn->n)+"\n";
						}
					}
					crt_constraint=cn;
					*/
					crt_constraint=NULL;
				}
				if(elem3.tagName()=="ConstraintTeacherIntervalMaxDaysPerWeek" && !skipDeprecatedConstraints){
					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("File contains deprecated constraint teacher interval max days per week - will be ignored\n"),
					 "Skip rest of deprecated constraints", "See next deprecated constraint", QString(),
					 1, 0 );
													 
					if(t==0)
						skipDeprecatedConstraints=true;
					/*
					ConstraintTeacherIntervalMaxDaysPerWeek* cn=new ConstraintTeacherIntervalMaxDaysPerWeek();
					assert(cn!=NULL);
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="Start_Hour"){
							for(cn->h1=0; cn->h1 < this->nHoursPerDay; cn->h1++)
								if(this->hoursOfTheDay[cn->h1]==elem4.text())
									break;
							assert(cn->h1>=0 && cn->h1 < this->nHoursPerDay);
							xmlReadingLog+="    Start hour="+this->hoursOfTheDay[cn->h1]+"\n";
						}
						else if(elem4.tagName()=="End_Hour"){
							for(cn->h2=0; cn->h2 < this->nHoursPerDay; cn->h2++)
								if(this->hoursOfTheDay[cn->h2]==elem4.text())
									break;
							assert(cn->h2>0 && cn->h2 <= this->nHoursPerDay);
							xmlReadingLog+="    End hour="+this->hoursOfTheDay[cn->h2]+"\n";
						}
						else if(elem4.tagName()=="Teacher"){
							cn->teacher=elem4.text();
							xmlReadingLog+="    Read teacher name="+cn->teacher+"\n";
						}
						if(elem4.tagName()=="Max_Intervals"){
							cn->n=elem4.text().toInt();
							xmlReadingLog+="    Adding max intervals="+QString::number(cn->n)+"\n";
						}
					}
					crt_constraint=cn;*/
					crt_constraint=NULL;
				}
				if(elem3.tagName()=="Constraint2ActivitiesOrdered" && !skipDeprecatedConstraints){
					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("File contains deprecated constraint 2 activities ordered - will be ignored\n"),
					 "Skip rest of deprecated constraints", "See next deprecated constraint", QString(),
					 1, 0 );
													 
					if(t==0)
						skipDeprecatedConstraints=true;
					/*
					Constraint2ActivitiesOrdered* cn=new Constraint2ActivitiesOrdered();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="First_Activity_Id"){
							cn->firstActivityId=elem4.text().toInt();
							xmlReadingLog+="    Read first activity id="+QString::number(cn->firstActivityId)+"\n";
						}
						else if(elem4.tagName()=="Second_Activity_Id"){
							cn->secondActivityId=elem4.text().toInt();
							xmlReadingLog+="    Read second activity id="+QString::number(cn->secondActivityId)+"\n";
						}
					}
					crt_constraint=cn;*/
					crt_constraint=NULL;
				}
				if(elem3.tagName()=="Constraint2ActivitiesGrouped" && !skipDeprecatedConstraints){
					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("File contains deprecated constraint 2 activities grouped - will be ignored\n"),
					 "Skip rest of deprecated constraints", "See next deprecated constraint", QString(),
					 1, 0 );
													 
					if(t==0)
						skipDeprecatedConstraints=true;
					/*
					Constraint2ActivitiesGrouped* cn=new Constraint2ActivitiesGrouped();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="First_Activity_Id"){
							cn->firstActivityId=elem4.text().toInt();
							xmlReadingLog+="    Read first activity id="+QString::number(cn->firstActivityId)+"\n";
						}
						else if(elem4.tagName()=="Second_Activity_Id"){
							cn->secondActivityId=elem4.text().toInt();
							xmlReadingLog+="    Read second activity id="+QString::number(cn->secondActivityId)+"\n";
						}
					}
					crt_constraint=cn;
					*/
					crt_constraint=NULL;
				}
				if(elem3.tagName()=="ConstraintActivitiesPreferredTimes"){
					ConstraintActivitiesPreferredTimes* cn=new ConstraintActivitiesPreferredTimes();
					cn->nPreferredTimes=0;
					int i;
					for(i=0; i<MAX_N_CONSTRAINT_ACTIVITIES_PREFERRED_TIMES; i++){
						cn->days[i] = cn->hours[i] = -1;
					}
					cn->teacherName="";
					cn->studentsName="";
					cn->subjectName="";
					cn->subjectTagName="";
					
					i=0;
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="Teacher_Name"){
							cn->teacherName=elem4.text();
							xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
						}
						else if(elem4.tagName()=="Students_Name"){
							cn->studentsName=elem4.text();
							xmlReadingLog+="    Read students name="+cn->studentsName+"\n";
						}
						else if(elem4.tagName()=="Subject_Name"){
							cn->subjectName=elem4.text();
							xmlReadingLog+="    Read subject name="+cn->subjectName+"\n";
						}
						else if(elem4.tagName()=="Subject_Tag_Name"){
							cn->subjectTagName=elem4.text();
							xmlReadingLog+="    Read subject tag name="+cn->subjectTagName+"\n";
						}
						else if(elem4.tagName()=="Number_of_Preferred_Times"){
							cn->nPreferredTimes=elem4.text().toInt();
							xmlReadingLog+="    Read number of preferred times="+QString::number(cn->nPreferredTimes)+"\n";
						}
						else if(elem4.tagName()=="Preferred_Time"){
							xmlReadingLog+="    Read: preferred time\n";

							for(QDomNode node5=elem4.firstChild(); !node5.isNull(); node5=node5.nextSibling()){
								QDomElement elem5=node5.toElement();
								if(elem5.isNull()){
									xmlReadingLog+="    Null node here\n";
									continue;
								}
								xmlReadingLog+="    Found "+elem5.tagName()+" tag\n";
								if(elem5.tagName()=="Preferred_Day"){
									for(cn->days[i]=0; cn->days[i]<this->nDaysPerWeek; cn->days[i]++)
										if(this->daysOfTheWeek[cn->days[i]]==elem5.text())
											break;
											
									if(cn->days[i]>=this->nDaysPerWeek){
										QMessageBox::information(NULL, QObject::tr("FET information"), 
										 QObject::tr("Constraint ActivitiesPreferredTimes day corrupt for teacher name=%1, students names=%2, subject name=%3, subject tag name=%4, day %5 is inexistent ... ignoring constraint")
										 .arg(cn->teacherName)
										 .arg(cn->studentsName)
										 .arg(cn->subjectName)
										 .arg(cn->subjectTagName)
										 .arg(elem5.text()));
										cn=NULL;
										goto corruptConstraintTime;
									}
											
									assert(cn->days[i]<this->nDaysPerWeek);
									xmlReadingLog+="    Preferred day="+this->daysOfTheWeek[cn->days[i]]+"("+QString::number(i)+")"+"\n";
								}
								else if(elem5.tagName()=="Preferred_Hour"){
									for(cn->hours[i]=0; cn->hours[i] < this->nHoursPerDay; cn->hours[i]++)
										if(this->hoursOfTheDay[cn->hours[i]]==elem5.text())
											break;
											
									if(cn->hours[i]>=this->nHoursPerDay){
										QMessageBox::information(NULL, QObject::tr("FET information"), 
										 QObject::tr("Constraint ActivitiesPreferredTimes hour corrupt for teacher name=%1, students names=%2, subject name=%3, subject tag name=%4, hour %5 is inexistent ... ignoring constraint")
										 .arg(cn->teacherName)
										 .arg(cn->studentsName)
										 .arg(cn->subjectName)
										 .arg(cn->subjectTagName)
										 .arg(elem5.text()));
										cn=NULL;
										goto corruptConstraintTime;
									}
											
									assert(cn->hours[i]>=0 && cn->hours[i] < this->nHoursPerDay);
									xmlReadingLog+="    Preferred hour="+this->hoursOfTheDay[cn->hours[i]]+"\n";
								}
							}

							i++;
						}
					}
					assert(i==cn->nPreferredTimes);
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintTeachersSubjectTagsMaxHoursContinuously" && !skipDeprecatedConstraints){
					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("File contains deprecated constraint teachers subject tags max hours continuously - will be ignored\n"),
					 "Skip rest of deprecated constraints", "See next deprecated constraint", QString(),
					 1, 0 );
													 
					if(t==0)
						skipDeprecatedConstraints=true;
					/*
					ConstraintTeachersSubjectTagsMaxHoursContinuously* cn=new ConstraintTeachersSubjectTagsMaxHoursContinuously();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="Maximum_Hours_Continuously"){
							cn->maxHoursContinuously=elem4.text().toInt();
							xmlReadingLog+="    Read maxHoursContinuously="+QString::number(cn->maxHoursContinuously)+"\n";
						}
					}
					crt_constraint=cn;
					*/
					crt_constraint=NULL;
				}
				if(elem3.tagName()=="ConstraintTeachersSubjectTagMaxHoursContinuously" && !skipDeprecatedConstraints){
					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("File contains deprecated constraint teachers subject tag max hours continuously - will be ignored\n"),
					 "Skip rest of deprecated constraints", "See next deprecated constraint", QString(),
					 1, 0 );
													 
					if(t==0)
						skipDeprecatedConstraints=true;
					/*
					ConstraintTeachersSubjectTagMaxHoursContinuously* cn=new ConstraintTeachersSubjectTagMaxHoursContinuously();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						else if(elem4.tagName()=="Maximum_Hours_Continuously"){
							cn->maxHoursContinuously=elem4.text().toInt();
							xmlReadingLog+="    Read maxHoursContinuously="+QString::number(cn->maxHoursContinuously)+"\n";
						}
						else if(elem4.tagName()=="Subject_Tag"){
							cn->subjectTagName=elem4.text();
							xmlReadingLog+="    Read subject tag name="+cn->subjectTagName+"\n";
						}
					}
					crt_constraint=cn;
					*/
					crt_constraint=NULL;
				}

corruptConstraintTime:
				//here we skip invalid constraint or add valid one
				if(crt_constraint!=NULL){
					assert(crt_constraint!=NULL);
					bool tmp=this->addTimeConstraint(crt_constraint);
					if(!tmp){
						QMessageBox::warning(NULL, QObject::tr("FET information"),
						 QObject::tr("Constraint\n%1\nnot added - must be a duplicate").
						 arg(crt_constraint->getDetailedDescription(*this)));
						delete crt_constraint;
					}
					nc++;
				}
			}
			xmlReadingLog+="  Added "+QString::number(nc)+" time constraints\n";
		}
		else if(elem2.tagName()=="Space_Constraints_List"){
			int nc=0;
			SpaceConstraint *crt_constraint;
			for(QDomNode node3=elem2.firstChild(); !node3.isNull(); node3=node3.nextSibling()){
				crt_constraint=NULL;
				QDomElement elem3=node3.toElement();
				if(elem3.isNull()){
					xmlReadingLog+="   Null node here\n";
					continue;
				}
				xmlReadingLog+="   Found "+elem3.tagName()+" tag\n";
				if(elem3.tagName()=="ConstraintBasicCompulsorySpace"){
					ConstraintBasicCompulsorySpace* cn=new ConstraintBasicCompulsorySpace();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}

						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";

						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						/*if(elem4.tagName()=="Weight"){
							cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight="+QString::number(cn->weight)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
								cn->compulsory=true;
								xmlReadingLog+="    Current constraint is compulsory\n";
							}
							else{
								cn->compulsory=false;
								xmlReadingLog+="    Current constraint is not compulsory\n";
							}
						}*/
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintRoomNotAvailable"){
					ConstraintRoomNotAvailable* cn=new ConstraintRoomNotAvailable();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						/*if(elem4.tagName()=="Weight"){
							cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight="+QString::number(cn->weight)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
								cn->compulsory=true;
								xmlReadingLog+="    Current constraint is compulsory\n";
							}
							else{
								cn->compulsory=false;
								xmlReadingLog+="    Current constraint is not compulsory\n";
							}
						}*/
						else if(elem4.tagName()=="Room_Name"){
							cn->roomName=elem4.text();
							xmlReadingLog+="    Read room name="+cn->roomName+"\n";
						}
						else if(elem4.tagName()=="Day"){
							for(cn->d=0; cn->d<this->nDaysPerWeek; cn->d++)
								if(this->daysOfTheWeek[cn->d]==elem4.text())
									break;

							if(cn->d>=this->nDaysPerWeek){
								QMessageBox::information(NULL, QObject::tr("FET information"), 
								 QObject::tr("Constraint RoomNotAvailable day corrupt for room %1, day %2 is inexistent ... ignoring constraint")
								 .arg(cn->roomName)
								 .arg(elem4.text()));
								cn=NULL;
								goto corruptConstraintSpace;
							}

							assert(cn->d<this->nDaysPerWeek);
							xmlReadingLog+="    Crt. day="+this->daysOfTheWeek[cn->d]+"\n";
						}
						else if(elem4.tagName()=="Start_Hour"){
							for(cn->h1=0; cn->h1 < this->nHoursPerDay; cn->h1++)
								if(this->hoursOfTheDay[cn->h1]==elem4.text())
									break;

							if(cn->h1>=this->nHoursPerDay){
								QMessageBox::information(NULL, QObject::tr("FET information"), 
								 QObject::tr("Constraint RoomNotAvailable start hour corrupt for room %1, hour %2 is inexistent ... ignoring constraint")
								 .arg(cn->roomName)
								 .arg(elem4.text()));
								cn=NULL;
								goto corruptConstraintSpace;
							}

							assert(cn->h1>=0 && cn->h1 < this->nHoursPerDay);
							xmlReadingLog+="    Start hour="+this->hoursOfTheDay[cn->h1]+"\n";
						}
						else if(elem4.tagName()=="End_Hour"){
							for(cn->h2=0; cn->h2 < this->nHoursPerDay; cn->h2++)
								if(this->hoursOfTheDay[cn->h2]==elem4.text())
									break;

							if(cn->h2<=0 || cn->h2>this->nHoursPerDay){
								QMessageBox::information(NULL, QObject::tr("FET information"), 
								 QObject::tr("Constraint RoomNotAvailable end hour corrupt for room %1, hour %2 is inexistent ... ignoring constraint")
								 .arg(cn->roomName)
								 .arg(elem4.text()));
								cn=NULL;
								goto corruptConstraintSpace;
							}

							assert(cn->h2>0 && cn->h2 <= this->nHoursPerDay);
							xmlReadingLog+="    End hour="+this->hoursOfTheDay[cn->h2]+"\n";
						}
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintRoomTypeNotAllowedSubjects" && !skipDeprecatedConstraints){
				
					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("File contains deprecated constraint room type not allowed subjects - will be ignored\n"),
					 QObject::tr("Skip rest of deprecated constraints"), QObject::tr("See next deprecated constraint"), QString(),
					 1, 0);
					 
					if(t==0)
						skipDeprecatedConstraints=true;
				
					/*ConstraintRoomTypeNotAllowedSubjects* cn=new ConstraintRoomTypeNotAllowedSubjects();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight="+QString::number(cn->weight)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
								cn->compulsory=true;
								xmlReadingLog+="    Current constraint is compulsory\n";
							}
							else{
								cn->compulsory=false;
								xmlReadingLog+="    Current constraint is not compulsory\n";
							}
						}
						else if(elem4.tagName()=="Room_Type"){
							cn->roomType=elem4.text();
							xmlReadingLog+="    Read room type="+cn->roomType+"\n";
						}
						else if(elem4.tagName()=="Subject"){
							cn->addNotAllowedSubject(elem4.text());
							xmlReadingLog+="    Read not allowed subject="+elem4.text()+"\n";
						}
					}
					crt_constraint=cn;*/
					crt_constraint=NULL;
				}
				if(elem3.tagName()=="ConstraintSubjectRequiresEquipments" && !skipDeprecatedConstraints){
				
					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("File contains deprecated constraint subject requires equipments - will be ignored\n"),
					 QObject::tr("Skip rest of deprecated constraints"), QObject::tr("See next deprecated constraint"), QString(),
					 1, 0);
					 
					if(t==0)
						skipDeprecatedConstraints=true;
				
					/*ConstraintSubjectRequiresEquipments* cn=new ConstraintSubjectRequiresEquipments();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight="+QString::number(cn->weight)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
								cn->compulsory=true;
								xmlReadingLog+="    Current constraint is compulsory\n";
							}
							else{
								cn->compulsory=false;
								xmlReadingLog+="    Current constraint is not compulsory\n";
							}
						}
						else if(elem4.tagName()=="Subject"){
							cn->subjectName=elem4.text();
							xmlReadingLog+="    Read subject="+cn->subjectName+"\n";
						}
						else if(elem4.tagName()=="Equipment"){
							cn->addRequiredEquipment(elem4.text());
							xmlReadingLog+="    Read required equipment="+elem4.text()+"\n";
						}
					}
					crt_constraint=cn;*/
					crt_constraint=NULL;
				}
				if(elem3.tagName()=="ConstraintSubjectSubjectTagRequireEquipments" && !skipDeprecatedConstraints){
				
					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("File contains deprecated constraint subject tag requires equipments - will be ignored\n"),
					 QObject::tr("Skip rest of deprecated constraints"), QObject::tr("See next deprecated constraint"), QString(),
					 1, 0);
					 
					if(t==0)
						skipDeprecatedConstraints=true;
				
					/*ConstraintSubjectSubjectTagRequireEquipments* cn=new ConstraintSubjectSubjectTagRequireEquipments();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight="+QString::number(cn->weight)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
								cn->compulsory=true;
								xmlReadingLog+="    Current constraint is compulsory\n";
							}
							else{
								cn->compulsory=false;
								xmlReadingLog+="    Current constraint is not compulsory\n";
							}
						}
						else if(elem4.tagName()=="Subject"){
							cn->subjectName=elem4.text();
							xmlReadingLog+="    Read subject="+cn->subjectName+"\n";
						}
						else if(elem4.tagName()=="Subject_Tag"){
							cn->subjectTagName=elem4.text();
							xmlReadingLog+="    Read subject tag="+cn->subjectTagName+"\n";
						}
						else if(elem4.tagName()=="Equipment"){
							cn->addRequiredEquipment(elem4.text());
							xmlReadingLog+="    Read required equipment="+elem4.text()+"\n";
						}
					}
					crt_constraint=cn;*/
					crt_constraint=NULL;
				}
				if(elem3.tagName()=="ConstraintTeacherRequiresRoom" && !skipDeprecatedConstraints){
				
					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("File contains deprecated constraint teacher requires room - will be ignored\n"),
					 QObject::tr("Skip rest of deprecated constraints"), QObject::tr("See next deprecated constraint"), QString(),
					 1, 0);
					 
					if(t==0)
						skipDeprecatedConstraints=true;
				
					/*ConstraintTeacherRequiresRoom* cn=new ConstraintTeacherRequiresRoom();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight="+QString::number(cn->weight)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
								cn->compulsory=true;
								xmlReadingLog+="    Current constraint is compulsory\n";
							}
							else{
								cn->compulsory=false;
								xmlReadingLog+="    Current constraint is not compulsory\n";
							}
						}
						else if(elem4.tagName()=="Teacher"){
							cn->teacherName=elem4.text();
							xmlReadingLog+="    Read teacher="+cn->teacherName+"\n";
						}
						else if(elem4.tagName()=="Room"){
							cn->roomName=elem4.text();
							xmlReadingLog+="    Read room="+elem4.text()+"\n";
						}
					}
					crt_constraint=cn;
					*/
					crt_constraint=NULL;
				}
				if(elem3.tagName()=="ConstraintTeacherSubjectRequireRoom" && !skipDeprecatedConstraints){
				
					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("File contains deprecated constraint teacher subject require room - will be ignored\n"),
					 QObject::tr("Skip rest of deprecated constraints"), QObject::tr("See next deprecated constraint"), QString(),
					 1, 0);
					 
					if(t==0)
						skipDeprecatedConstraints=true;
				
					/*ConstraintTeacherSubjectRequireRoom* cn=new ConstraintTeacherSubjectRequireRoom();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight="+QString::number(cn->weight)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
								cn->compulsory=true;
								xmlReadingLog+="    Current constraint is compulsory\n";
							}
							else{
								cn->compulsory=false;
								xmlReadingLog+="    Current constraint is not compulsory\n";
							}
						}
						else if(elem4.tagName()=="Teacher"){
							cn->teacherName=elem4.text();
							xmlReadingLog+="    Read teacher="+cn->teacherName+"\n";
						}
						else if(elem4.tagName()=="Subject"){
							cn->subjectName=elem4.text();
							xmlReadingLog+="    Read subject="+cn->subjectName+"\n";
						}
						else if(elem4.tagName()=="Room"){
							cn->roomName=elem4.text();
							xmlReadingLog+="    Read room="+elem4.text()+"\n";
						}
					}
					crt_constraint=cn;*/
					crt_constraint=NULL;
				}
				if(elem3.tagName()=="ConstraintMinimizeNumberOfRoomsForStudents" && !skipDeprecatedConstraints){
				
					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("File contains deprecated constraint minimize number of rooms for students - will be ignored\n"),
					 QObject::tr("Skip rest of deprecated constraints"), QObject::tr("See next deprecated constraint"), QString(),
					 1, 0);
					 
					if(t==0)
						skipDeprecatedConstraints=true;
				
					/*ConstraintMinimizeNumberOfRoomsForStudents* cn=new ConstraintMinimizeNumberOfRoomsForStudents();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight="+QString::number(cn->weight)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
								cn->compulsory=true;
								xmlReadingLog+="    Current constraint is compulsory\n";
							}
							else{
								cn->compulsory=false;
								xmlReadingLog+="    Current constraint is not compulsory\n";
							}
						}
					}
					crt_constraint=cn;*/
					crt_constraint=NULL;
				}
				if(elem3.tagName()=="ConstraintMinimizeNumberOfRoomsForTeachers" && !skipDeprecatedConstraints){
				
					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("File contains deprecated constraint minimize number of rooms for teachers - will be ignored\n"),
					 QObject::tr("Skip rest of deprecated constraints"), QObject::tr("See next deprecated constraint"), QString(),
					 1, 0);
					 
					if(t==0)
						skipDeprecatedConstraints=true;
				
					/*ConstraintMinimizeNumberOfRoomsForTeachers* cn=new ConstraintMinimizeNumberOfRoomsForTeachers();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight="+QString::number(cn->weight)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
								cn->compulsory=true;
								xmlReadingLog+="    Current constraint is compulsory\n";
							}
							else{
								cn->compulsory=false;
								xmlReadingLog+="    Current constraint is not compulsory\n";
							}
						}
					}
					crt_constraint=cn;*/
					crt_constraint=NULL;
				}
				if(elem3.tagName()=="ConstraintActivityPreferredRoom"){
					ConstraintActivityPreferredRoom* cn=new ConstraintActivityPreferredRoom();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						/*if(elem4.tagName()=="Weight"){
							cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight="+QString::number(cn->weight)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
								cn->compulsory=true;
								xmlReadingLog+="    Current constraint is compulsory\n";
							}
							else{
								cn->compulsory=false;
								xmlReadingLog+="    Current constraint is not compulsory\n";
							}
						}*/
						else if(elem4.tagName()=="Activity_Id"){
							cn->activityId=elem4.text().toInt();
							xmlReadingLog+="    Read activity id="+QString::number(cn->activityId)+"\n";
						}
						else if(elem4.tagName()=="Room"){
							cn->roomName=elem4.text();
							xmlReadingLog+="    Read room="+elem4.text()+"\n";
						}
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintActivityPreferredRooms"){
					ConstraintActivityPreferredRooms* cn=new ConstraintActivityPreferredRooms();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						/*if(elem4.tagName()=="Weight"){
							cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight="+QString::number(cn->weight)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
								cn->compulsory=true;
								xmlReadingLog+="    Current constraint is compulsory\n";
							}
							else{
								cn->compulsory=false;
								xmlReadingLog+="    Current constraint is not compulsory\n";
							}
						}*/
						else if(elem4.tagName()=="Activity_Id"){
							cn->activityId=elem4.text().toInt();
							xmlReadingLog+="    Read activity id="+QString::number(cn->activityId)+"\n";
						}
						else if(elem4.tagName()=="Number_of_Preferred_Rooms"){
							cn->_n_preferred_rooms=elem4.text().toInt();
							xmlReadingLog+="    Read number of preferred rooms: "+QString::number(cn->_n_preferred_rooms)+"\n";
							assert(cn->_n_preferred_rooms>=2);
						}
						else if(elem4.tagName()=="Preferred_Room"){
							cn->roomsNames.append(elem4.text());
							xmlReadingLog+="    Read room="+elem4.text()+"\n";
						}
					}
					assert(cn->_n_preferred_rooms==cn->roomsNames.count());
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintActivitiesSameRoom" && !skipDeprecatedConstraints){
				
					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("File contains deprecated constraint activities same room - will be ignored\n"),
					 QObject::tr("Skip rest of deprecated constraints"), QObject::tr("See next deprecated constraint"), QString(),
					 1, 0);
					 
					if(t==0)
						skipDeprecatedConstraints=true;
				
					/*ConstraintActivitiesSameRoom* cn=new ConstraintActivitiesSameRoom();
					int n_act=0;
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight="+QString::number(cn->weight)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
								cn->compulsory=true;
								xmlReadingLog+="    Current constraint is compulsory\n";
							}
							else{
								cn->compulsory=false;
								xmlReadingLog+="    Current constraint is not compulsory\n";
							}
						}
						else if(elem4.tagName()=="Number_of_Activities"){
							cn->n_activities=elem4.text().toInt();
							xmlReadingLog+="    Read n_activities="+QString::number(cn->n_activities)+"\n";
						}
						else if(elem4.tagName()=="Activity_Id"){
							cn->activitiesId[n_act]=elem4.text().toInt();
							xmlReadingLog+="    Read activity id="+QString::number(cn->activitiesId[n_act])+"\n";
							n_act++;
						}
					}
					assert(cn->n_activities==n_act);
					crt_constraint=cn;*/
					crt_constraint=NULL;
				}
				if(elem3.tagName()=="ConstraintSubjectPreferredRoom"){
					ConstraintSubjectPreferredRoom* cn=new ConstraintSubjectPreferredRoom();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						/*if(elem4.tagName()=="Weight"){
							cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight="+QString::number(cn->weight)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
								cn->compulsory=true;
								xmlReadingLog+="    Current constraint is compulsory\n";
							}
							else{
								cn->compulsory=false;
								xmlReadingLog+="    Current constraint is not compulsory\n";
							}
						}*/
						else if(elem4.tagName()=="Subject"){
							cn->subjectName=elem4.text();
							xmlReadingLog+="    Read subject="+cn->subjectName+"\n";
						}
						else if(elem4.tagName()=="Room"){
							cn->roomName=elem4.text();
							xmlReadingLog+="    Read room="+cn->roomName+"\n";
						}
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintSubjectPreferredRooms"){
					ConstraintSubjectPreferredRooms* cn=new ConstraintSubjectPreferredRooms();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						/*if(elem4.tagName()=="Weight"){
							cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight="+QString::number(cn->weight)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
								cn->compulsory=true;
								xmlReadingLog+="    Current constraint is compulsory\n";
							}
							else{
								cn->compulsory=false;
								xmlReadingLog+="    Current constraint is not compulsory\n";
							}
						}*/
						else if(elem4.tagName()=="Subject"){
							cn->subjectName=elem4.text();
							xmlReadingLog+="    Read subject="+cn->subjectName+"\n";
						}
						else if(elem4.tagName()=="Number_of_Preferred_Rooms"){
							cn->_n_preferred_rooms=elem4.text().toInt();
							xmlReadingLog+="    Read number of preferred rooms: "+QString::number(cn->_n_preferred_rooms)+"\n";
							assert(cn->_n_preferred_rooms>=2);
						}
						else if(elem4.tagName()=="Preferred_Room"){
							cn->roomsNames.append(elem4.text());
							xmlReadingLog+="    Read room="+elem4.text()+"\n";
						}
					}
					assert(cn->_n_preferred_rooms==cn->roomsNames.count());
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintSubjectSubjectTagPreferredRoom"){
					ConstraintSubjectSubjectTagPreferredRoom* cn=new ConstraintSubjectSubjectTagPreferredRoom();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						/*if(elem4.tagName()=="Weight"){
							cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight="+QString::number(cn->weight)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
								cn->compulsory=true;
								xmlReadingLog+="    Current constraint is compulsory\n";
							}
							else{
								cn->compulsory=false;
								xmlReadingLog+="    Current constraint is not compulsory\n";
							}
						}*/
						else if(elem4.tagName()=="Subject"){
							cn->subjectName=elem4.text();
							xmlReadingLog+="    Read subject="+cn->subjectName+"\n";
						}
						else if(elem4.tagName()=="Subject_Tag"){
							cn->subjectTagName=elem4.text();
							xmlReadingLog+="    Read subject tag="+cn->subjectTagName+"\n";
						}
						else if(elem4.tagName()=="Room"){
							cn->roomName=elem4.text();
							xmlReadingLog+="    Read room="+cn->roomName+"\n";
						}
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintSubjectSubjectTagPreferredRooms"){
					ConstraintSubjectSubjectTagPreferredRooms* cn=new ConstraintSubjectSubjectTagPreferredRooms();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							//cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Ignoring old tag - weight - making weight percentage=100\n";
							cn->weightPercentage=100;
						}
						else if(elem4.tagName()=="Weight_Percentage"){
							cn->weightPercentage=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight percentage="+QString::number(cn->weightPercentage)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
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
						/*if(elem4.tagName()=="Weight"){
							cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight="+QString::number(cn->weight)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
								cn->compulsory=true;
								xmlReadingLog+="    Current constraint is compulsory\n";
							}
							else{
								cn->compulsory=false;
								xmlReadingLog+="    Current constraint is not compulsory\n";
							}
						}*/
						else if(elem4.tagName()=="Subject"){
							cn->subjectName=elem4.text();
							xmlReadingLog+="    Read subject="+cn->subjectName+"\n";
						}
						else if(elem4.tagName()=="Subject_Tag"){
							cn->subjectTagName=elem4.text();
							xmlReadingLog+="    Read subject tag="+cn->subjectTagName+"\n";
						}
						else if(elem4.tagName()=="Number_of_Preferred_Rooms"){
							cn->_n_preferred_rooms=elem4.text().toInt();
							xmlReadingLog+="    Read number of preferred rooms: "+QString::number(cn->_n_preferred_rooms)+"\n";
							assert(cn->_n_preferred_rooms>=2);
						}
						else if(elem4.tagName()=="Preferred_Room"){
							cn->roomsNames.append(elem4.text());
							xmlReadingLog+="    Read room="+elem4.text()+"\n";
						}
					}
					assert(cn->_n_preferred_rooms==cn->roomsNames.count());
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintMaxBuildingChangesPerDayForTeachers" && !skipDeprecatedConstraints){
				
					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("File contains deprecated constraint max building changes per day for teachers - will be ignored\n"),
					 QObject::tr("Skip rest of deprecated constraints"), QObject::tr("See next deprecated constraint"), QString(),
					 1, 0);
					 
					if(t==0)
						skipDeprecatedConstraints=true;
				
					/*ConstraintMaxBuildingChangesPerDayForTeachers* cn=new ConstraintMaxBuildingChangesPerDayForTeachers();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight="+QString::number(cn->weight)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
								cn->compulsory=true;
								xmlReadingLog+="    Current constraint is compulsory\n";
							}
							else{
								cn->compulsory=false;
								xmlReadingLog+="    Current constraint is not compulsory\n";
							}
						}
						else if(elem4.tagName()=="Max_Building_Changes"){
							cn->maxBuildingChanges=elem4.text().toInt();
							xmlReadingLog+="    Adding max building changes="+QString::number(cn->maxBuildingChanges)+"\n";
						}
					}
					crt_constraint=cn;*/
					crt_constraint=NULL;
				}
				if(elem3.tagName()=="ConstraintMaxBuildingChangesPerDayForStudents" && !skipDeprecatedConstraints){
				
					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("File contains deprecated constraint max building changes per day for students - will be ignored\n"),
					 QObject::tr("Skip rest of deprecated constraints"), QObject::tr("See next deprecated constraint"), QString(),
					 1, 0);
					 
					if(t==0)
						skipDeprecatedConstraints=true;
				
					/*ConstraintMaxBuildingChangesPerDayForStudents* cn=new ConstraintMaxBuildingChangesPerDayForStudents();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight="+QString::number(cn->weight)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
								cn->compulsory=true;
								xmlReadingLog+="    Current constraint is compulsory\n";
							}
							else{
								cn->compulsory=false;
								xmlReadingLog+="    Current constraint is not compulsory\n";
							}
						}
						else if(elem4.tagName()=="Max_Building_Changes"){
							cn->maxBuildingChanges=elem4.text().toInt();
							xmlReadingLog+="    Adding max building changes="+QString::number(cn->maxBuildingChanges)+"\n";
						}
					}
					crt_constraint=cn;*/
					crt_constraint=NULL;
				}
				if(elem3.tagName()=="ConstraintMaxRoomChangesPerDayForTeachers" && !skipDeprecatedConstraints){
				
					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("File contains deprecated constraint max room changes per day for teachers - will be ignored\n"),
					 QObject::tr("Skip rest of deprecated constraints"), QObject::tr("See next deprecated constraint"), QString(),
					 1, 0);
					 
					if(t==0)
						skipDeprecatedConstraints=true;
				
					/*ConstraintMaxRoomChangesPerDayForTeachers* cn=new ConstraintMaxRoomChangesPerDayForTeachers();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight="+QString::number(cn->weight)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
								cn->compulsory=true;
								xmlReadingLog+="    Current constraint is compulsory\n";
							}
							else{
								cn->compulsory=false;
								xmlReadingLog+="    Current constraint is not compulsory\n";
							}
						}
						else if(elem4.tagName()=="Max_Room_Changes"){
							cn->maxRoomChanges=elem4.text().toInt();
							xmlReadingLog+="    Adding max room changes="+QString::number(cn->maxRoomChanges)+"\n";
						}
					}
					crt_constraint=cn;*/
					crt_constraint=NULL;
				}
				if(elem3.tagName()=="ConstraintMaxRoomChangesPerDayForStudents" && !skipDeprecatedConstraints){
				
					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("File contains deprecated constraint max room changes per day for students - will be ignored\n"),
					 QObject::tr("Skip rest of deprecated constraints"), QObject::tr("See next deprecated constraint"), QString(),
					 1, 0);
					 
					if(t==0)
						skipDeprecatedConstraints=true;
				
					/*ConstraintMaxRoomChangesPerDayForStudents* cn=new ConstraintMaxRoomChangesPerDayForStudents();
					for(QDomNode node4=elem3.firstChild(); !node4.isNull(); node4=node4.nextSibling()){
						QDomElement elem4=node4.toElement();
						if(elem4.isNull()){
							xmlReadingLog+="    Null node here\n";
							continue;
						}
						xmlReadingLog+="    Found "+elem4.tagName()+" tag\n";
						if(elem4.tagName()=="Weight"){
							cn->weight=elem4.text().toDouble();
							xmlReadingLog+="    Adding weight="+QString::number(cn->weight)+"\n";
						}
						else if(elem4.tagName()=="Compulsory"){
							if(elem4.text()=="yes"){
								cn->compulsory=true;
								xmlReadingLog+="    Current constraint is compulsory\n";
							}
							else{
								cn->compulsory=false;
								xmlReadingLog+="    Current constraint is not compulsory\n";
							}
						}
						else if(elem4.tagName()=="Max_Room_Changes"){
							cn->maxRoomChanges=elem4.text().toInt();
							xmlReadingLog+="    Adding max room changes="+QString::number(cn->maxRoomChanges)+"\n";
						}
					}
					crt_constraint=cn;*/
					crt_constraint=NULL;
				}

corruptConstraintSpace:
				//here we skip invalid constraint or add valid one
				if(crt_constraint!=NULL){
					assert(crt_constraint!=NULL);
					bool tmp=this->addSpaceConstraint(crt_constraint);
					if(!tmp){
						QMessageBox::warning(NULL, QObject::tr("FET information"),
						 QObject::tr("Constraint\n%1\nnot added - must be a duplicate").
						 arg(crt_constraint->getDetailedDescription(*this)));
						delete crt_constraint;
					}
					nc++;
				}
			}
			xmlReadingLog+="  Added "+QString::number(nc)+" space constraints\n";
		}
	}

	this->internalStructureComputed=false;

	logStream<<xmlReadingLog;

	if(file2.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 QObject::tr("Saved of logging gave error code %1, which means you cannot see the log of reading the file. Please check your disk free space")
		 .arg(file2.error()));
	}

	file2.close();

	////////////////////////////////////////

	return true;
}

void Rules::write(const QString& filename)
{
	assert(this->initialized);

	QString s;

	QFile file(filename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 QObject::tr("Cannot open filename for writing ... please check your disk free space. Saving of file aborted"));
		 
		return;
	
		assert(0);
		exit(1);
	}

	QTextStream tos(&file);
	
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);
	//tos.setEncoding(QTextStream::UnicodeUTF8);

	s+="<!DOCTYPE FET><FET version=\""+FET_VERSION+"\">\n\n";
	
	//the institution name and comments
	s+="<Institution_Name>"+protect(this->institutionName)+"</Institution_Name>\n\n";
	s+="<Comments>"+protect(this->comments)+"</Comments>\n\n";

	//the hours and days
	s+="<Hours_List>\n	<Number>"+QString::number(this->nHoursPerDay)+"</Number>\n";
	for(int i=0; i<=this->nHoursPerDay; i++)
		s+="	<Name>"+protect(this->hoursOfTheDay[i])+"</Name>\n";
	s+="</Hours_List>\n\n";
	s+="<Days_List>\n	<Number>"+QString::number(this->nDaysPerWeek)+"</Number>\n";
	for(int i=0; i<this->nDaysPerWeek; i++)
		s+="	<Name>"+protect(this->daysOfTheWeek[i])+"</Name>\n";
	s+="</Days_List>\n\n";

	//students list
	s+="<Students_List>\n";
	for(int i=0; i<this->yearsList.size(); i++){
		StudentsYear* sty=this->yearsList[i];
		s += sty->getXmlDescription();
	}
	s+="</Students_List>\n\n";

	//teachers list
	s += "<Teachers_List>\n";
	for(int i=0; i<this->teachersList.size(); i++){
		Teacher* tch=this->teachersList[i];
		s += tch->getXmlDescription();
	}
	s += "</Teachers_List>\n\n";

	//subjects list
	s += "<Subjects_List>\n";
	for(int i=0; i<this->subjectsList.size(); i++){
		Subject* sbj=this->subjectsList[i];
		s += sbj->getXmlDescription();
	}
	s += "</Subjects_List>\n\n";

	//subject tags list
	s += "<Subject_Tags_List>\n";
	for(int i=0; i<this->subjectTagsList.size(); i++){
		SubjectTag* stg=this->subjectTagsList[i];
		s += stg->getXmlDescription();
	}
	s += "</Subject_Tags_List>\n\n";

	//activities list
	s += "<Activities_List>\n";
	for(int i=0; i<this->activitiesList.size(); i++){
		Activity* act=this->activitiesList[i];
		s += act->getXmlDescription(*this);
		s += "\n";
	}
	s += "</Activities_List>\n\n";

	//rooms list
	s += "<Rooms_List>\n";
	for(int i=0; i<this->roomsList.size(); i++){
		Room* rm=this->roomsList[i];
		s += rm->getXmlDescription();
	}
	s += "</Rooms_List>\n\n";

	//time constraints list
	s += "<Time_Constraints_List>\n";
	for(int i=0; i<this->timeConstraintsList.size(); i++){
		TimeConstraint* ctr=this->timeConstraintsList[i];
		s += ctr->getXmlDescription(*this);
	}
	s += "</Time_Constraints_List>\n\n";

	//constraints list
	s += "<Space_Constraints_List>\n";
	for(int i=0; i<this->spaceConstraintsList.size(); i++){
		SpaceConstraint* ctr=this->spaceConstraintsList[i];
		s += ctr->getXmlDescription(*this);
	}
	s += "</Space_Constraints_List>\n\n";

	s+="</FET>\n";

	tos<<s;
	
	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 QObject::tr("Saved file gave error code %1, which means saving is compromised. Please check your disk free space")
		 .arg(file.error()));
	}
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
	
	return count;
}

int Rules::activateStudents(const QString& studentsName)
{
	int count=0;
	for(int i=0; i<this->activitiesList.size(); i++){
		Activity* act=this->activitiesList[i];
		if(act->searchStudents(studentsName)){
			if(!act->active)
				count++;
			act->active=true;
		}
	}

	this->internalStructureComputed=false;
	
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
	
	return count;
}

int Rules::activateSubjectTag(const QString& subjectTagName)
{
	int count=0;
	for(int i=0; i<this->activitiesList.size(); i++){
		Activity* act=this->activitiesList[i];
		if(act->subjectTagName==subjectTagName){
			if(!act->active)
				count++;
			act->active=true;
		}
	}

	this->internalStructureComputed=false;
	
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
	
	return count;
}

int Rules::deactivateStudents(const QString& studentsName)
{
	int count=0;
	for(int i=0; i<this->activitiesList.size(); i++){
		Activity* act=this->activitiesList[i];
		if(act->searchStudents(studentsName)){
			if(act->active)
				count++;
			act->active=false;
		}
	}

	this->internalStructureComputed=false;
	
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
	
	return count;
}

int Rules::deactivateSubjectTag(const QString& subjectTagName)
{
	int count=0;
	for(int i=0; i<this->activitiesList.size(); i++){
		Activity* act=this->activitiesList[i];
		if(act->subjectTagName==subjectTagName){
			if(act->active)
				count++;
			act->active=false;
		}
	}

	this->internalStructureComputed=false;
	
	return count;
}
