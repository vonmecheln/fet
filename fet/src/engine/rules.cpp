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

#include "genetictimetable_defs.h"
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
	this->hoursOfTheDay[0]="0";
	this->hoursOfTheDay[1]="1";
	this->hoursOfTheDay[2]="2";
	this->hoursOfTheDay[3]="3";
	this->hoursOfTheDay[4]="4";
	this->hoursOfTheDay[5]="5";
	this->hoursOfTheDay[6]="6";
	this->hoursOfTheDay[7]="7";
	this->hoursOfTheDay[8]="8";
	this->hoursOfTheDay[9]="9";
	this->hoursOfTheDay[10]="10";
	this->hoursOfTheDay[11]="11";
	this->hoursOfTheDay[12]="12";

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
	//After that, the equipments.
	//After that, the rooms.
	//After that, the activities.
	//After that, the time constraints.
	//After that, initialization of the matrix "roomHasEquipment".
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

	if(this->equipmentsList.size()>MAX_EQUIPMENTS){
		QMessageBox::warning(NULL, QObject::tr("FET information"),
		 QObject::tr("You have too many equipments.\nPlease talk to the author or increase variable MAX_EQUIPMENTS"));
		return false;
	}
	if(this->roomsList.size()>MAX_ROOMS){
		QMessageBox::warning(NULL, QObject::tr("FET information"),
		 QObject::tr("You have too many rooms.\nPlease talk to the author or increase variable MAX_ROOMS"));
		return false;
	}
	if(this->buildingsList.size()>MAX_BUILDINGS){
		QMessageBox::warning(NULL, QObject::tr("FET information"),
		 QObject::tr("You have too many buildings.\nPlease talk to the author or increase variable MAX_BUILDINGS"));
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

	//equipments internal list
	this->nInternalEquipments=0;
	assert(this->equipmentsList.size()<=MAX_EQUIPMENTS);
	for(int i=0; i<this->equipmentsList.size(); i++)
		this->internalEquipmentsList[this->nInternalEquipments++]=this->equipmentsList.at(i);
	assert(this->nInternalEquipments==this->equipmentsList.count());

	//buildings internal list
	this->nInternalBuildings=0;
	assert(this->buildingsList.size()<=MAX_BUILDINGS);
	for(int i=0; i<this->buildingsList.size(); i++)
		this->internalBuildingsList[this->nInternalBuildings++]=this->buildingsList.at(i);
	assert(this->nInternalBuildings==this->buildingsList.size());

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

	//time constraints
	assert(this->timeConstraintsList.size()<=MAX_TIME_CONSTRAINTS);
	TimeConstraint* tctr;

	for(int i=0; i<this->nInternalActivities; i++){
		this->fixedDay[i]=-1;
		this->fixedHour[i]=-1;
		this->sameDay[i]=-1;
		this->sameHour[i]=-1;
	}
	
	//getting rid of compulsory preferred time-s, same starting hour-s & same starting time-s - 
	//these will be used separately to repair the chromosomes (it was practically
	//found that this is the best method).
	int tctri=0;
	for(int tctrindex=0; tctrindex<this->timeConstraintsList.size(); tctrindex++){
		tctr=this->timeConstraintsList[tctrindex];
		//if the activities which refer to this constraints are not active
		if(!tctr->computeInternalStructure(*this))
			continue;
		if(tctr->type==CONSTRAINT_ACTIVITY_PREFERRED_TIME && tctr->compulsory==true){
			ConstraintActivityPreferredTime* c = (ConstraintActivityPreferredTime*)tctr;

			bool t1=this->fixedDay[c->activityIndex]==-1 || this->fixedDay[c->activityIndex]==c->day;
			bool t2=this->fixedHour[c->activityIndex]==-1 || this->fixedHour[c->activityIndex]==c->hour;
			if(!t1){
				QMessageBox::warning(NULL, QObject::tr("FET information"), 
				 QObject::tr("Activity with id=%1 has fixed day on %2 and %3 - impossible\n"
				 "Please fix that")
				 .arg(this->internalActivitiesList[c->activityIndex].id)
				 .arg(this->daysOfTheWeek[this->fixedDay[c->activityIndex]])
				 .arg(this->daysOfTheWeek[c->day]));
				this->internalStructureComputed=false;
				return false;
			}
			if(!t2){
				QMessageBox::warning(NULL, QObject::tr("FET information"), 
				 QObject::tr("Activity with id=%1 has fixed hour on %2 and %3 - impossible\n"
				 "Please fix that")
				 .arg(this->internalActivitiesList[c->activityIndex].id)
				 .arg(this->hoursOfTheDay[this->fixedHour[c->activityIndex]])
				 .arg(this->hoursOfTheDay[c->hour]));
				this->internalStructureComputed=false;
				return false;
			}

			this->fixedDay[c->activityIndex] = c->day;
			this->fixedHour[c->activityIndex] = c->hour;
		}
		else if(tctr->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME && tctr->compulsory==true){
			ConstraintActivitiesSameStartingTime* c=(ConstraintActivitiesSameStartingTime*)tctr;
			int ai1=c->_activities[0];
			for(int i=1; i<c->_n_activities; i++){
				int ai2=c->_activities[i];
				
				int old=sameDay[ai2];
				if(old>=0){
					bool visited[MAX_ACTIVITIES];
					for(int i=0; i<this->nInternalActivities; i++)
						visited[i]=false;					
					visited[old]=true;
					while(sameDay[old]>=0 && !visited[sameDay[old]]){
						old=sameDay[old];
						visited[old]=true;
					}
					this->sameDay[old]=ai1;
				}
				else
					this->sameDay[ai2]=ai1;
				
				old=sameHour[ai2];
				if(old>=0){
					bool visited[MAX_ACTIVITIES];
					for(int i=0; i<this->nInternalActivities; i++)
						visited[i]=false;					
					visited[old]=true;
					while(sameHour[old]>=0 && !visited[sameHour[old]]){
						old=sameHour[old];
						visited[old]=true;
					}
					this->sameHour[old]=ai1;
				}
				else
					this->sameHour[ai2]=ai1;
			}
		}
		else if(tctr->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR && tctr->compulsory==true){
			ConstraintActivitiesSameStartingHour* c=(ConstraintActivitiesSameStartingHour*)tctr;
			int ai1=c->_activities[0];
			for(int i=1; i<c->_n_activities; i++){
				int ai2=c->_activities[i];

				int old=sameHour[ai2];
				if(old>=0){
					bool visited[MAX_ACTIVITIES];
					for(int i=0; i<this->nInternalActivities; i++)
						visited[i]=false;
					visited[old]=true;
					while(sameHour[old]>=0 && !visited[sameHour[old]]){
						old=sameHour[old];
						visited[old]=true;
					}
					this->sameHour[old]=ai1;
				}
				else
					this->sameHour[ai2]=ai1;
			}
		}
		else{
			this->internalTimeConstraintsList[tctri++]=tctr;
		}
	}

	//all activities will depend on only one "head"
	for(int i=0; i<this->nInternalActivities; i++){
		for(int j=0; j<this->nInternalActivities; j++){
			int k=this->sameDay[j];
			if(k>=0 && this->sameDay[k]>=0)
				this->sameDay[j]=this->sameDay[k];
			k=this->sameHour[j];
			if(k>=0 && this->sameHour[k]>=0)
				this->sameHour[j]=this->sameHour[k];
		}
	}
	//make the head have the fixed day & hour
	for(int i=0; i<this->nInternalActivities; i++){
		if(this->fixedDay[i]>=0 && this->sameDay[i]>=0){
			bool t=this->fixedDay[this->sameDay[i]]==this->fixedDay[i] || this->fixedDay[this->sameDay[i]]==-1;
			if(!t){
				QMessageBox::warning(NULL, QObject::tr("FET information"), 
					 QObject::tr("Activity with id=%1 has fixed day on %2 and must be scheduled on the same\n"
					 " day with act. with id=%3, which is impossible. Please check your constraints").arg(this->internalActivitiesList[i].id)
					 .arg(this->daysOfTheWeek[this->fixedDay[i]]).arg(this->internalActivitiesList[this->sameDay[i]].id));
				this->internalStructureComputed=false;
				return false;
			}

			assert(this->fixedDay[this->sameDay[i]]==this->fixedDay[i] || this->fixedDay[this->sameDay[i]]==-1);
			this->fixedDay[this->sameDay[i]]=this->fixedDay[i];
		}
		if(this->fixedHour[i]>=0 && this->sameHour[i]>=0){
			bool t=this->fixedHour[this->sameHour[i]]==this->fixedHour[i] || this->fixedHour[this->sameHour[i]]==-1;
			if(!t){
				QMessageBox::warning(NULL, QObject::tr("FET information"), 
					 QObject::tr("Activity with id=%1 has fixed hour on %2 and must be scheduled on the same\n"
					 " hour with act. with id=%3, which is impossible. Please check your constraints").arg(this->internalActivitiesList[i].id)
					 .arg(this->hoursOfTheDay[this->fixedHour[i]]).arg(this->internalActivitiesList[this->sameHour[i]].id));
				this->internalStructureComputed=false;
				return false;
			}

			assert(this->fixedHour[this->sameHour[i]]==this->fixedHour[i] || this->fixedHour[this->sameHour[i]]==-1);
			this->fixedHour[this->sameHour[i]]=this->fixedHour[i];
		}
	}
	//make all have the fixed day & hour
	for(int i=0; i<this->nInternalActivities; i++){
		if(this->sameDay[i]>=0){
			assert(this->fixedDay[i]==this->fixedDay[this->sameDay[i]] || this->fixedDay[i]==-1);
			this->fixedDay[i]=this->fixedDay[this->sameDay[i]];
		}
		if(this->sameHour[i]>=0){
			assert(this->fixedHour[i]==this->fixedHour[this->sameHour[i]] || this->fixedHour[i]==-1);
			this->fixedHour[i]=this->fixedHour[this->sameHour[i]];
		}
	}
	
	this->nInternalTimeConstraints=tctri;
	assert(this->nInternalTimeConstraints<=MAX_TIME_CONSTRAINTS);
	
	//roomHasEquipment
	for(int rm=0; rm<this->nInternalRooms; rm++){
		for(int eq=0; eq<this->nInternalEquipments; eq++){
			this->roomHasEquipment[rm][eq] = 
			 (this->internalRoomsList[rm])->searchEquipment(this->internalEquipmentsList[eq]->name);
		}
	}

	//space constraints
	SpaceConstraint* sctr;
	assert(this->spaceConstraintsList.size()<=MAX_SPACE_CONSTRAINTS);

	for(int i=0; i<this->nInternalActivities; i++){
		this->fixedRoom[i]=-1;
		this->sameRoom[i]=-1;
	}

	int sctri=0;
	for(int sctrindex=0; sctrindex<this->spaceConstraintsList.size(); sctrindex++){
		sctr=this->spaceConstraintsList[sctrindex];
	
		if(!sctr->computeInternalStructure(*this))
			continue;
		if(sctr->type==CONSTRAINT_TEACHER_REQUIRES_ROOM && sctr->compulsory==true){
			//works by repairing
			ConstraintTeacherRequiresRoom* ctrr=(ConstraintTeacherRequiresRoom*) sctr;

			for(int i=0; i<ctrr->_nActivities; i++){
				bool t=this->fixedRoom[ctrr->_activities[i]]==-1 || this->fixedRoom[ctrr->_activities[i]]==ctrr->_room;
				if(!t){
					QMessageBox::warning(NULL, QObject::tr("FET information"), 
					 QObject::tr("ConstraintTeacherRequiresRoom: Teacher=%1,\n"
					 "Activity with id=%2 has fixed room on %3 and %4 - impossible\n"
					 "Please fix that")
					 .arg(ctrr->teacherName)
					 .arg(this->internalActivitiesList[ctrr->_activities[i]].id)
					 .arg(this->roomsList.at(this->fixedRoom[ctrr->_activities[i]])->name)
					 .arg(this->roomsList.at(ctrr->_room)->name));
					this->internalStructureComputed=false;
					return false;
				}

				this->fixedRoom[ctrr->_activities[i]]=ctrr->_room;
			}
		}
		if(sctr->type==CONSTRAINT_TEACHER_SUBJECT_REQUIRE_ROOM && sctr->compulsory==true){
			//works by repairing
			ConstraintTeacherSubjectRequireRoom* ctrr=(ConstraintTeacherSubjectRequireRoom*) sctr;

			for(int i=0; i<ctrr->_nActivities; i++){
				bool t=this->fixedRoom[ctrr->_activities[i]]==-1 || this->fixedRoom[ctrr->_activities[i]]==ctrr->_room;
				if(!t){
					QMessageBox::warning(NULL, QObject::tr("FET information"), 
					 QObject::tr("ConstraintTeacherSubjectRequireRoom: Teacher=%1, Subject=%2\n"
					 "Activity with id=%3 has fixed room on %4 and %5 - impossible\n"
					 "Please fix that")
					 .arg(ctrr->teacherName)
					 .arg(ctrr->subjectName)
					 .arg(this->internalActivitiesList[ctrr->_activities[i]].id)
					 .arg(this->roomsList.at(this->fixedRoom[ctrr->_activities[i]])->name)
					 .arg(this->roomsList.at(ctrr->_room)->name));
					this->internalStructureComputed=false;
					return false;
				}

				this->fixedRoom[ctrr->_activities[i]]=ctrr->_room;
			}
		}
		else if(sctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM && sctr->compulsory==true){
			//works by repairing
			ConstraintActivityPreferredRoom* capr=(ConstraintActivityPreferredRoom*) sctr;

			bool t=this->fixedRoom[capr->_activity]==-1 || this->fixedRoom[capr->_activity]==capr->_room;
			if(!t){
				QMessageBox::warning(NULL, QObject::tr("FET information"), 
				 QObject::tr("Activity with id=%1 has fixed room on %2 and %3 - impossible\n"
				 "Please fix that")
				 .arg(this->internalActivitiesList[capr->_activity].id)
				 .arg(this->roomsList.at(this->fixedRoom[capr->_activity])->name)
				 .arg(this->roomsList.at(capr->_room)->name));
				this->internalStructureComputed=false;
				return false;
			}

			this->fixedRoom[capr->_activity]=capr->_room;
		}
		else if(sctr->type==CONSTRAINT_ACTIVITIES_SAME_ROOM && sctr->compulsory==true){
			ConstraintActivitiesSameRoom* c=(ConstraintActivitiesSameRoom*)sctr;
			int ai1=c->_activities[0];
			for(int i=1; i<c->_n_activities; i++){
				int ai2=c->_activities[i];

				int old=sameRoom[ai2];
				if(old>=0){
					bool visited[MAX_ACTIVITIES];
					for(int i=0; i<this->nInternalActivities; i++)
						visited[i]=false;					
					visited[old]=true;
					while(sameRoom[old]>=0 && !visited[sameRoom[old]]){
						old=sameRoom[old];
						visited[old]=true;
					}
					this->sameRoom[old]=ai1;
				}
				else
					this->sameRoom[ai2]=ai1;
			}
		}
		else if(sctr->type==CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOM && sctr->compulsory==true){
			//works by repairing
			ConstraintSubjectSubjectTagPreferredRoom* c=(ConstraintSubjectSubjectTagPreferredRoom*) sctr;

			for(int i=0; i<c->_nActivities; i++){
				int ai=c->_activities[i];
				bool t=this->fixedRoom[ai]==-1 || this->fixedRoom[ai]==c->_room;
				if(!t){
					QMessageBox::warning(NULL, QObject::tr("FET information"), 
					 QObject::tr("Because of space constraint\n%1\n"
					 "Activity with id=%2 has fixed room on %3 and %4 - impossible\n"
					 "Please fix that")
					 .arg(c->getDetailedDescription(*this))
					 .arg(this->internalActivitiesList[ai].id)
					 .arg(this->roomsList.at(this->fixedRoom[ai])->name)
					 .arg(this->roomsList.at(c->_room)->name));
					this->internalStructureComputed=false;
					return false;
				}

				this->fixedRoom[ai]=c->_room;
			}
		}
		else if(sctr->type==CONSTRAINT_SUBJECT_PREFERRED_ROOM && sctr->compulsory==true){
			//works by repairing
			ConstraintSubjectPreferredRoom* c=(ConstraintSubjectPreferredRoom*) sctr;

			for(int i=0; i<c->_nActivities; i++){
				int ai=c->_activities[i];
				bool t=this->fixedRoom[ai]==-1 || this->fixedRoom[ai]==c->_room;
				if(!t){
					QMessageBox::warning(NULL, QObject::tr("FET information"), 
					 QObject::tr("Because of space constraint\n%1\n"
					 "Activity with id=%2 has fixed room on %3 and %4 - impossible\n"
					 "Please fix that")
					 .arg(c->getDetailedDescription(*this))
					 .arg(this->internalActivitiesList[ai].id)
					 .arg(this->roomsList.at(this->fixedRoom[ai])->name)
					 .arg(this->roomsList.at(c->_room)->name));
					this->internalStructureComputed=false;
					return false;
				}

				this->fixedRoom[ai]=c->_room;
			}
		}
		else
			this->internalSpaceConstraintsList[sctri++]=sctr;
	}
	this->nInternalSpaceConstraints=sctri;
	assert(this->nInternalSpaceConstraints<=MAX_SPACE_CONSTRAINTS);

	//all activities will depend on only one "head"
	for(int i=0; i<this->nInternalActivities; i++){
		for(int j=0; j<this->nInternalActivities; j++){
			int k=this->sameRoom[j];
			if(k>=0 && this->sameRoom[k]>=0)
				this->sameRoom[j]=this->sameRoom[k];
		}
	}
	//make the head have the fixed room
	for(int i=0; i<this->nInternalActivities; i++){
		if(this->fixedRoom[i]>=0 && this->sameRoom[i]>=0){
			bool t=this->fixedRoom[this->sameRoom[i]]==this->fixedRoom[i] || this->fixedRoom[this->sameRoom[i]]==-1;
			if(!t){
				QMessageBox::warning(NULL, QObject::tr("FET information"), 
					 QObject::tr("Activity with id=%1 has fixed room in %2 and must be scheduled in the same\n"
					 " room with act. with id=%3, which is impossible. Please check your constraints").arg(this->internalActivitiesList[i].id)
					 .arg(this->fixedRoom[i]).arg(this->internalActivitiesList[this->sameRoom[i]].id));
				this->internalStructureComputed=false;
				return false;
			}

			assert(this->fixedRoom[this->sameRoom[i]]==this->fixedRoom[i] || this->fixedRoom[this->sameRoom[i]]==-1);
			this->fixedRoom[this->sameRoom[i]]=this->fixedRoom[i];
		}
	}
	//make all have the fixed room
	for(int i=0; i<this->nInternalActivities; i++){
		if(this->sameRoom[i]>=0){
			assert(this->fixedRoom[i]==this->fixedRoom[this->sameRoom[i]] || this->fixedRoom[i]==-1);
			this->fixedRoom[i]=this->fixedRoom[this->sameRoom[i]];
		}
	}

	//done.
	this->internalStructureComputed=true;
	
	return true;
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
	while(!yearsList.isEmpty())
		delete yearsList.takeFirst();

	//Activities
	while(!activitiesList.isEmpty())
		delete activitiesList.takeFirst();

	//Time constraints
	while(!timeConstraintsList.isEmpty())
		delete timeConstraintsList.takeFirst();

	//Space constraints
	while(!spaceConstraintsList.isEmpty())
		delete spaceConstraintsList.takeFirst();

	//Equipments
	while(!equipmentsList.isEmpty())
		delete equipmentsList.takeFirst();

	//Buildings
	while(!buildingsList.isEmpty())
		delete buildingsList.takeFirst();

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
		act->removeTeacher(teacherName);
		if(act->teachersNames.size()==0){
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


	for(int i=0; i<this->spaceConstraintsList.size(); ){
		SpaceConstraint* ctr=this->spaceConstraintsList[i];
		if(ctr->type==CONSTRAINT_TEACHER_REQUIRES_ROOM){
			ConstraintTeacherRequiresRoom* crt_constraint=(ConstraintTeacherRequiresRoom*)ctr;
			if(teacherName==crt_constraint->teacherName)
				this->removeSpaceConstraint(ctr); //single constraint removal
			else
				i++;
		}
		else
			i++;
	}

	for(int i=0; i<this->spaceConstraintsList.size(); ){
		SpaceConstraint* ctr=this->spaceConstraintsList[i];
		if(ctr->type==CONSTRAINT_TEACHER_SUBJECT_REQUIRE_ROOM){
			ConstraintTeacherSubjectRequireRoom* crt_constraint=(ConstraintTeacherSubjectRequireRoom*)ctr;
			if(teacherName==crt_constraint->teacherName)
				this->removeSpaceConstraint(ctr); //single constraint removal
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
	
	for(int i=0; i<this->spaceConstraintsList.size(); i++){
		SpaceConstraint* ctr=this->spaceConstraintsList[i];	

		if(ctr->type==CONSTRAINT_TEACHER_REQUIRES_ROOM){
			ConstraintTeacherRequiresRoom* crt_constraint=(ConstraintTeacherRequiresRoom*)ctr;
			if(initialTeacherName == crt_constraint->teacherName)
				crt_constraint->teacherName=finalTeacherName;
		}
	}

	for(int i=0; i<this->spaceConstraintsList.size(); i++){
		SpaceConstraint* ctr=this->spaceConstraintsList[i];	

		if(ctr->type==CONSTRAINT_TEACHER_SUBJECT_REQUIRE_ROOM){
			ConstraintTeacherSubjectRequireRoom* crt_constraint=(ConstraintTeacherSubjectRequireRoom*)ctr;
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

		if(ctr->type==CONSTRAINT_ROOM_TYPE_NOT_ALLOWED_SUBJECTS){
			ConstraintRoomTypeNotAllowedSubjects* crtnas=(ConstraintRoomTypeNotAllowedSubjects*)ctr;
			QStringList::Iterator it;
			for(it=crtnas->subjects.begin(); it!=crtnas->subjects.end(); it++)
				if(*it==subjectName)
					break;
			if(it!=crtnas->subjects.end()){
				int tmp=crtnas->removeNotAllowedSubject(subjectName);
				assert(tmp==1);
			}

			//remove the constraint if subjects list empty
			if(crtnas->subjects.isEmpty())
				this->removeSpaceConstraint(ctr);
			else
				i++;
		}
		else if(ctr->type==CONSTRAINT_SUBJECT_REQUIRES_EQUIPMENTS){
			ConstraintSubjectRequiresEquipments* csre=(ConstraintSubjectRequiresEquipments*)ctr;

			if(csre->subjectName == subjectName)
				this->removeSpaceConstraint(ctr);
			else
				i++;
		}
		else if(ctr->type==CONSTRAINT_SUBJECT_SUBJECT_TAG_REQUIRE_EQUIPMENTS){
			ConstraintSubjectSubjectTagRequireEquipments* c=(ConstraintSubjectSubjectTagRequireEquipments*)ctr;

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
		else if(ctr->type==CONSTRAINT_SUBJECT_PREFERRED_ROOM){
			ConstraintSubjectPreferredRoom* c=(ConstraintSubjectPreferredRoom*)ctr;

			if(c->subjectName == subjectName)
				this->removeSpaceConstraint(ctr);
			else
				i++;
		}
		else if(ctr->type==CONSTRAINT_TEACHER_SUBJECT_REQUIRE_ROOM){
			ConstraintTeacherSubjectRequireRoom* c=(ConstraintTeacherSubjectRequireRoom*)ctr;

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

		if(ctr->type==CONSTRAINT_ROOM_TYPE_NOT_ALLOWED_SUBJECTS){
			ConstraintRoomTypeNotAllowedSubjects* crtas=(ConstraintRoomTypeNotAllowedSubjects*)ctr;
			QStringList::Iterator it;
			for(it=crtas->subjects.begin(); it!=crtas->subjects.end(); it++)
				if(*it==initialSubjectName){
					*it=finalSubjectName;
					break;
				}
		}
		else if(ctr->type==CONSTRAINT_SUBJECT_REQUIRES_EQUIPMENTS){
			ConstraintSubjectRequiresEquipments* csre=(ConstraintSubjectRequiresEquipments*)ctr;
			if(csre->subjectName == initialSubjectName)
				csre->subjectName=finalSubjectName;
		}
		else if(ctr->type==CONSTRAINT_SUBJECT_SUBJECT_TAG_REQUIRE_EQUIPMENTS){
			ConstraintSubjectSubjectTagRequireEquipments* c=(ConstraintSubjectSubjectTagRequireEquipments*)ctr;
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
		else if(ctr->type==CONSTRAINT_SUBJECT_PREFERRED_ROOM){
			ConstraintSubjectPreferredRoom* c=(ConstraintSubjectPreferredRoom*)ctr;
			if(c->subjectName == initialSubjectName)
				c->subjectName=finalSubjectName;
		}
		else if(ctr->type==CONSTRAINT_TEACHER_SUBJECT_REQUIRE_ROOM){
			ConstraintTeacherSubjectRequireRoom* c=(ConstraintTeacherSubjectRequireRoom*)ctr;
			if(c->subjectName == initialSubjectName)
				c->subjectName=finalSubjectName;
		}
		else if(ctr->type==CONSTRAINT_SUBJECT_PREFERRED_ROOMS){
			ConstraintSubjectPreferredRooms* c=(ConstraintSubjectPreferredRooms*)ctr;
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

		if(ctr->type==CONSTRAINT_SUBJECT_SUBJECT_TAG_REQUIRE_EQUIPMENTS){
			ConstraintSubjectSubjectTagRequireEquipments* c=(ConstraintSubjectSubjectTagRequireEquipments*)ctr;

			if(c->subjectTagName == subjectTagName)
				this->removeSpaceConstraint(ctr);
			else
				i++;
		}
		else if(ctr->type==CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOM){
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
		else if(ctr->type==CONSTRAINT_TEACHERS_SUBJECT_TAG_MAX_HOURS_CONTINUOUSLY){
			ConstraintTeachersSubjectTagMaxHoursContinuously* c=(ConstraintTeachersSubjectTagMaxHoursContinuously*)ctr;

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
	
	//modify the time constraints related to this subject tag
	for(int i=0; i<this->timeConstraintsList.size(); i++){
		TimeConstraint* ctr=this->timeConstraintsList[i];
	
		if(ctr->type==CONSTRAINT_ACTIVITIES_PREFERRED_TIMES){
			ConstraintActivitiesPreferredTimes* crt_constraint=(ConstraintActivitiesPreferredTimes*)ctr;
			if(initialSubjectTagName == crt_constraint->subjectTagName)
				crt_constraint->subjectTagName=finalSubjectTagName;
		}
	}

	//modify the space constraints related to this subject
	for(int i=0; i<this->spaceConstraintsList.size(); i++){
		SpaceConstraint* ctr=this->spaceConstraintsList[i];

		if(ctr->type==CONSTRAINT_SUBJECT_SUBJECT_TAG_REQUIRE_EQUIPMENTS){
			ConstraintSubjectSubjectTagRequireEquipments* c=(ConstraintSubjectSubjectTagRequireEquipments*)ctr;
			if(c->subjectTagName == initialSubjectTagName)
				c->subjectTagName=finalSubjectTagName;
		}
		else if(ctr->type==CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOM){
			ConstraintSubjectSubjectTagPreferredRoom* c=(ConstraintSubjectSubjectTagPreferredRoom*)ctr;
			if(c->subjectTagName == initialSubjectTagName)
				c->subjectTagName=finalSubjectTagName;
		}
		else if(ctr->type==CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOMS){
			ConstraintSubjectSubjectTagPreferredRooms* c=(ConstraintSubjectSubjectTagPreferredRooms*)ctr;
			if(c->subjectTagName == initialSubjectTagName)
				c->subjectTagName=finalSubjectTagName;
		}
		else if(ctr->type==CONSTRAINT_TEACHERS_SUBJECT_TAG_MAX_HOURS_CONTINUOUSLY){
			ConstraintTeachersSubjectTagMaxHoursContinuously* c=(ConstraintTeachersSubjectTagMaxHoursContinuously*)ctr;
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
	/*if(s1==NULL)
		cout<<"s1==NULL, studentsSet1="<<(const char*)studentsSet1<<endl;
	if(s2==NULL)
		cout<<"s2==NULL, studentsSet2="<<(const char*)studentsSet2<<endl;*/
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
		act->removeStudents(*this, yearName, nStudents);
		
		if(act->studentsNames.count()==0){
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
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_N_HOURS_DAILY){
			ConstraintStudentsSetNHoursDaily* crt_constraint=(ConstraintStudentsSetNHoursDaily*)ctr;
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
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_INTERVAL_MAX_DAYS_PER_WEEK){
			ConstraintStudentsSetIntervalMaxDaysPerWeek* crt_constraint=(ConstraintStudentsSetIntervalMaxDaysPerWeek*)ctr;
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
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_N_HOURS_DAILY){
			ConstraintStudentsSetNHoursDaily* crt_constraint=(ConstraintStudentsSetNHoursDaily*)ctr;
			if(_initialYearName == crt_constraint->students)
				crt_constraint->students=finalYearName;
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_NO_GAPS){
			ConstraintStudentsSetNoGaps* crt_constraint=(ConstraintStudentsSetNoGaps*)ctr;
			if(_initialYearName == crt_constraint->students)
				crt_constraint->students=finalYearName;
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_INTERVAL_MAX_DAYS_PER_WEEK){
			ConstraintStudentsSetIntervalMaxDaysPerWeek* crt_constraint=(ConstraintStudentsSetIntervalMaxDaysPerWeek*)ctr;
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

		act->removeStudents(*this, groupName, nStudents);
		if(act->studentsNames.count()==0){
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
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_N_HOURS_DAILY){
			ConstraintStudentsSetNHoursDaily* crt_constraint=(ConstraintStudentsSetNHoursDaily*)ctr;
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
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_INTERVAL_MAX_DAYS_PER_WEEK){
			ConstraintStudentsSetIntervalMaxDaysPerWeek* crt_constraint=(ConstraintStudentsSetIntervalMaxDaysPerWeek*)ctr;
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
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_N_HOURS_DAILY){
			ConstraintStudentsSetNHoursDaily* crt_constraint=(ConstraintStudentsSetNHoursDaily*)ctr;
			if(_initialGroupName == crt_constraint->students)
				crt_constraint->students=finalGroupName;
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_NO_GAPS){
			ConstraintStudentsSetNoGaps* crt_constraint=(ConstraintStudentsSetNoGaps*)ctr;
			if(_initialGroupName == crt_constraint->students)
				crt_constraint->students=finalGroupName;
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_INTERVAL_MAX_DAYS_PER_WEEK){
			ConstraintStudentsSetIntervalMaxDaysPerWeek* crt_constraint=(ConstraintStudentsSetIntervalMaxDaysPerWeek*)ctr;
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

		act->removeStudents(*this, subgroupName, nStudents);
		if(act->studentsNames.count()==0){
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
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_N_HOURS_DAILY){
			ConstraintStudentsSetNHoursDaily* crt_constraint=(ConstraintStudentsSetNHoursDaily*)ctr;
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
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_INTERVAL_MAX_DAYS_PER_WEEK){
			ConstraintStudentsSetIntervalMaxDaysPerWeek* crt_constraint=(ConstraintStudentsSetIntervalMaxDaysPerWeek*)ctr;
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
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_N_HOURS_DAILY){
			ConstraintStudentsSetNHoursDaily* crt_constraint=(ConstraintStudentsSetNHoursDaily*)ctr;
			if(_initialSubgroupName == crt_constraint->students)
				crt_constraint->students=finalSubgroupName;
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_NO_GAPS){
			ConstraintStudentsSetNoGaps* crt_constraint=(ConstraintStudentsSetNoGaps*)ctr;
			if(_initialSubgroupName == crt_constraint->students)
				crt_constraint->students=finalSubgroupName;
		}
		else if(ctr->type==CONSTRAINT_STUDENTS_SET_INTERVAL_MAX_DAYS_PER_WEEK){
			ConstraintStudentsSetIntervalMaxDaysPerWeek* crt_constraint=(ConstraintStudentsSetIntervalMaxDaysPerWeek*)ctr;
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
	int _parity, /*parity: PARITY_WEEKLY or PARITY_FORTNIGHTLY*/
	bool _active,
	int _preferredDay,
	int _preferredHour,
	bool _computeNTotalStudents,
	int _nTotalStudents)
{
	assert(_parity==PARITY_WEEKLY || _parity==PARITY_FORTNIGHTLY); //weekly or fortnightly

	Activity *act=new Activity(*this, _id, _activityGroupId, _teachersNames, _subjectName, _subjectTagName,
		_studentsNames, _duration, _totalDuration, _parity, _active, _computeNTotalStudents, _nTotalStudents);

	this->activitiesList << act; //append

	if(_preferredDay>=0 || _preferredHour>=0){
		TimeConstraint *ctr=new ConstraintActivityPreferredTime(1.0, false, _id, _preferredDay, _preferredHour);
		bool tmp=this->addTimeConstraint(ctr);
		assert(tmp);
	}

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
	int _parities[],
	bool _active[],
	int _minDayDistance,
	int _preferredDays[],
	int _preferredHours[],
	bool _computeNTotalStudents,
	int _nTotalStudents)
{
	assert(_firstActivityId==_activityGroupId);

	int acts[MAX_CONSTRAINT_MIN_N_DAYS_BETWEEN_ACTIVITIES];
	assert(_nSplits<=MAX_CONSTRAINT_MIN_N_DAYS_BETWEEN_ACTIVITIES);

	for(int i=0; i<_nSplits; i++)
		assert(_parities[i]==PARITY_WEEKLY || _parities[i]==PARITY_FORTNIGHTLY); //weekly or fortnightly

	for(int i=0; i<_nSplits; i++){
		Activity *act;
		if(i==0)
			act=new Activity(*this, _firstActivityId+i, _activityGroupId,
				_teachersNames, _subjectName, _subjectTagName, _studentsNames,
				_durations[i], _totalDuration, _parities[i], _active[i], _computeNTotalStudents, _nTotalStudents);
		else
			act=new Activity(*this, _firstActivityId+i, _activityGroupId,
				_teachersNames, _subjectName, _subjectTagName, _studentsNames,
				_durations[i], _totalDuration, _parities[i], _active[i], _computeNTotalStudents, _nTotalStudents);

		this->activitiesList << act; //append

		acts[i]=_firstActivityId+i;

		if(_preferredDays[i]>=0 || _preferredHours[i]>=0){
			TimeConstraint *constr=new ConstraintActivityPreferredTime(1.0, false, act->id, _preferredDays[i], _preferredHours[i]); //non-compulsory constraint
			bool tmp = this->addTimeConstraint(constr);
			assert(tmp);
		}
	}

	if(_minDayDistance>0){
		TimeConstraint *constr=new ConstraintMinNDaysBetweenActivities(1.0, true, _nSplits, acts, _minDayDistance); //compulsory constraint
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
			//removing ConstraintActivityEndsDay-s referring to this activity
			for(int j=0; j<this->timeConstraintsList.size(); ){
				TimeConstraint* ctr=this->timeConstraintsList[j];
				if(ctr->type==CONSTRAINT_ACTIVITY_ENDS_DAY){
					ConstraintActivityEndsDay *apt=(ConstraintActivityEndsDay*)ctr;
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
					if(apt->firstActivityId==act->id || apt->secondActivityId==act->id){
						cout<<"Removing constraint "<<(const char*)(apt->getDescription(*this))<<endl;
						this->removeTimeConstraint(ctr);
					}
					else
						j++;
				}
				else
					j++;
			}
			//removing Constraint2ActivitiesOrdered-s referring to this activity
			for(int j=0; j<this->timeConstraintsList.size(); ){
				TimeConstraint* ctr=this->timeConstraintsList[j];
				if(ctr->type==CONSTRAINT_2_ACTIVITIES_ORDERED){
					Constraint2ActivitiesOrdered *apt=(Constraint2ActivitiesOrdered*)ctr;
					if(apt->firstActivityId==act->id || apt->secondActivityId==act->id){
						cout<<"Removing constraint "<<(const char*)(apt->getDescription(*this))<<endl;
						this->removeTimeConstraint(ctr);
					}
					else
						j++;
				}
				else
					j++;
			}
			//removing Constraint2ActivitiesGrouped-s referring to this activity
			for(int j=0; j<this->timeConstraintsList.size(); ){
				TimeConstraint* ctr=this->timeConstraintsList[j];
				if(ctr->type==CONSTRAINT_2_ACTIVITIES_GROUPED){
					Constraint2ActivitiesGrouped *apt=(Constraint2ActivitiesGrouped*)ctr;
					if(apt->firstActivityId==act->id || apt->secondActivityId==act->id){
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

	for(int i=0; i<this->spaceConstraintsList.size(); ){
		SpaceConstraint* ctr=this->spaceConstraintsList[i];
		if(ctr->type==CONSTRAINT_ACTIVITIES_SAME_ROOM){
			((ConstraintActivitiesSameRoom*)ctr)->removeUseless(*this);
			if(((ConstraintActivitiesSameRoom*)ctr)->n_activities<2)
				this->removeSpaceConstraint(ctr);
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
			//removing ConstraintActivityEndsDay-s referring to this activity
			for(int j=0; j<this->timeConstraintsList.size(); ){
				TimeConstraint* ctr=this->timeConstraintsList[j];
				if(ctr->type==CONSTRAINT_ACTIVITY_ENDS_DAY){
					ConstraintActivityEndsDay *apt=(ConstraintActivityEndsDay*)ctr;
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
					if(apt->firstActivityId==act->id || apt->secondActivityId==act->id){
						cout<<"Removing constraint "<<(const char*)(apt->getDescription(*this))<<endl;
						this->removeTimeConstraint(ctr);
					}
					else
						j++;
				}
				else
					j++;
			}
			//removing Constraint2ActivitiesGrouped-s referring to this activity
			for(int j=0; j<this->timeConstraintsList.size(); ){
				TimeConstraint* ctr=this->timeConstraintsList[j];
				if(ctr->type==CONSTRAINT_2_ACTIVITIES_GROUPED){
					Constraint2ActivitiesGrouped *apt=(Constraint2ActivitiesGrouped*)ctr;
					if(apt->firstActivityId==act->id || apt->secondActivityId==act->id){
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

	for(int i=0; i<this->spaceConstraintsList.size(); ){
		SpaceConstraint* ctr=this->spaceConstraintsList[i];
		if(ctr->type==CONSTRAINT_ACTIVITIES_SAME_ROOM){
			((ConstraintActivitiesSameRoom*)ctr)->removeUseless(*this);
			if(((ConstraintActivitiesSameRoom*)ctr)->n_activities<2)
				this->removeSpaceConstraint(ctr);
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
	int _parities[],
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
			act->parity=_parities[i];
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

bool Rules::addEquipment(Equipment* eq)
{
	if(this->searchEquipment(eq->name) >= 0)
		return false;
	this->equipmentsList << eq; //append
	this->internalStructureComputed=false;
	return true;
}

int Rules::searchEquipment(const QString& equipmentName)
{
	for(int i=0; i<this->equipmentsList.size(); i++)
		if(this->equipmentsList[i]->name==equipmentName)
			return i;

	return -1;
}

bool Rules::removeEquipment(const QString& equipmentName)
{
	int i=this->searchEquipment(equipmentName);
	if(i<0)
		return false;

	Equipment* searchedEquipment=this->equipmentsList[i];
	assert(searchedEquipment->name==equipmentName);
	
	//check the constraints
	for(int j=0; j<this->spaceConstraintsList.size(); ){
		SpaceConstraint* ctr=this->spaceConstraintsList[j];
		if(ctr->type==CONSTRAINT_SUBJECT_REQUIRES_EQUIPMENTS){
			ConstraintSubjectRequiresEquipments* csre=(ConstraintSubjectRequiresEquipments*)ctr;
			QStringList::Iterator it;
			for(it=csre->equipmentsNames.begin(); it!=csre->equipmentsNames.end(); it++)
				if(*it==equipmentName)
					break;
			if(it!=csre->equipmentsNames.end()){
				int tmp=csre->removeRequiredEquipment(equipmentName);
				assert(tmp==1);
			}

			//remove the constraint if equipments list empty
			if(csre->equipmentsNames.isEmpty())
				this->removeSpaceConstraint(ctr);
			else
				j++;
		}
		else
			j++;
	}
	
	//Remove this equipment from all the rooms that posess it.
	for(int j=0; j<this->roomsList.size(); j++)
		this->roomsList[j]->removeEquipment(equipmentName);

	delete this->equipmentsList[i];
	this->equipmentsList.removeAt(i);
	
	this->internalStructureComputed=false;
	
	return true;
}

bool Rules::modifyEquipment(const QString& initialEquipmentName, const QString& finalEquipmentName)
{
	int i=this->searchEquipment(initialEquipmentName);
	assert(i>=0);
	assert(this->searchEquipment(finalEquipmentName)==-1 || initialEquipmentName==finalEquipmentName);

	Equipment* searchedEquipment=this->equipmentsList[i];
	assert(searchedEquipment->name==initialEquipmentName);
	
	//check the constraints
	for(int j=0; j<this->spaceConstraintsList.size(); j++){
		SpaceConstraint* ctr=this->spaceConstraintsList[j];
		if(ctr->type==CONSTRAINT_SUBJECT_REQUIRES_EQUIPMENTS){
			ConstraintSubjectRequiresEquipments* csre=(ConstraintSubjectRequiresEquipments*)ctr;
			for(QStringList::Iterator it=csre->equipmentsNames.begin(); it!=csre->equipmentsNames.end(); it++)
				if(*it==initialEquipmentName)
					*it=finalEquipmentName;
		}
	}
	
	//Modify this equipment in all the rooms that posess it.
	for(int j=0; j<this->roomsList.size(); j++){
		Room* rm=this->roomsList[j];
		for(QStringList::Iterator it=rm->equipments.begin(); it!=rm->equipments.end(); it++)
			if(*it==initialEquipmentName)
				*it=finalEquipmentName;
	}
				
	searchedEquipment->name=finalEquipmentName;

	this->internalStructureComputed=false;
	return true;
}

void Rules::sortEquipmentsAlphabetically()
{
	qSort(this->equipmentsList.begin(), this->equipmentsList.end(), equipmentsAscending);

	this->internalStructureComputed=false;
}

bool Rules::addBuilding(Building* bu)
{
	if(this->searchBuilding(bu->name) >= 0)
		return false;
	this->buildingsList << bu;
	this->internalStructureComputed=false;
	return true;
}

int Rules::searchBuilding(const QString& buildingName)
{
	for(int i=0; i<this->buildingsList.size(); i++)
		if(this->buildingsList.at(i)->name==buildingName)
			return i;
	return -1;
}

bool Rules::removeBuilding(const QString& buildingName)
{
	int i=this->searchBuilding(buildingName);
	if(i<0)
		return false;

	Building* searchedBuilding=this->buildingsList.at(i);
	assert(searchedBuilding->name==buildingName);
	
	//Remove this building from all the rooms that have it.
	for(int i=0; i<this->roomsList.size(); i++){
		Room* rm=this->roomsList[i];
		if(rm->building==buildingName)
			rm->building="";
	}

	delete this->buildingsList[i];
	this->buildingsList.removeAt(i);
	this->internalStructureComputed=false;
	return true;
}

bool Rules::modifyBuilding(const QString& initialBuildingName, const QString& finalBuildingName)
{
	int i=this->searchBuilding(initialBuildingName);
	assert(i>=0);
	assert(this->searchBuilding(finalBuildingName)==-1 || initialBuildingName==finalBuildingName);

	Building* searchedBuilding=this->buildingsList.at(i);
	assert(searchedBuilding->name==initialBuildingName);
	
	//Modify this building in all the rooms that posess it.
	for(int j=0; j<this->roomsList.size(); j++){
		Room* rm=this->roomsList[j];
		if(rm->building==initialBuildingName)
			rm->building=finalBuildingName;
	}
				
	searchedBuilding->name=finalBuildingName;

	this->internalStructureComputed=false;
	return true;
}

void Rules::sortBuildingsAlphabetically()
{
	qSort(this->buildingsList.begin(), this->buildingsList.end(), buildingsAscending);

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

	//Removing useless ConstraintRoomTypeNotAllowedSubjects
	//Firstly, check whether the type of this room exists at another room
	bool typeExisting=false;
	
	for(int j=0; j<this->roomsList.size(); j++){	
		Room* rm=this->roomsList[j];
		if(rm->name!=searchedRoom->name && rm->type==searchedRoom->type){
			typeExisting=true;
			break;
		}
	}
		
	for(int j=0; j<this->spaceConstraintsList.size(); ){
		SpaceConstraint* ctr=this->spaceConstraintsList[j];
		if(!typeExisting && ctr->type==CONSTRAINT_ROOM_TYPE_NOT_ALLOWED_SUBJECTS){
			ConstraintRoomTypeNotAllowedSubjects* crtas=(ConstraintRoomTypeNotAllowedSubjects*)ctr;
			if(crtas->roomType==searchedRoom->type)
				this->removeSpaceConstraint(ctr);
			else
				j++;
		}
		else if(ctr->type==CONSTRAINT_ROOM_NOT_AVAILABLE){
			ConstraintRoomNotAvailable* crna=(ConstraintRoomNotAvailable*)ctr;
			if(crna->roomName==roomName)
				this->removeSpaceConstraint(ctr);
			else
				j++;
		}
		else if(ctr->type==CONSTRAINT_TEACHER_REQUIRES_ROOM){
			ConstraintTeacherRequiresRoom* c=(ConstraintTeacherRequiresRoom*)ctr;
			if(c->roomName==roomName)
				this->removeSpaceConstraint(ctr);
			else
				j++;
		}
		else if(ctr->type==CONSTRAINT_TEACHER_SUBJECT_REQUIRE_ROOM){
			ConstraintTeacherSubjectRequireRoom* c=(ConstraintTeacherSubjectRequireRoom*)ctr;
			if(c->roomName==roomName)
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
				 (c->weight, c->compulsory, c->activityId, c->roomsNames.at(0));

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
				 (c->weight, c->compulsory, c->subjectName, c->subjectTagName, c->roomsNames.at(0));

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
				 (c->weight, c->compulsory, c->subjectName, c->roomsNames.at(0));

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

bool Rules::modifyRoom(const QString& initialRoomName, const QString& finalRoomName, const QString& type, const QString& building, int capacity)
{
	int i=this->searchRoom(initialRoomName);
	if(i<0)
		return false;

	Room* searchedRoom=this->roomsList[i];
	assert(searchedRoom->name==initialRoomName);

	//Removing useless ConstraintRoomTypeNotAllowedSubjects
	//Firstly, check whether the type of this room exists at another room
	bool typeExisting=false;
	
	for(int j=0; j<this->roomsList.size(); j++){
		Room* rm=this->roomsList[j];
		if(rm->name!=searchedRoom->name && rm->type==searchedRoom->type){
			typeExisting=true;
			break;
		}
	}
		
	if(searchedRoom->type==type) //type unchanged, keep the constraints
		typeExisting=true;

	for(int j=0; j<this->spaceConstraintsList.size(); ){
		SpaceConstraint* ctr=this->spaceConstraintsList[j];
		if(!typeExisting && ctr->type==CONSTRAINT_ROOM_TYPE_NOT_ALLOWED_SUBJECTS){
			ConstraintRoomTypeNotAllowedSubjects* crtas=(ConstraintRoomTypeNotAllowedSubjects*)ctr;
			if(crtas->roomType==searchedRoom->type)
				this->removeSpaceConstraint(ctr);
			else
				j++;
		}
		else if(ctr->type==CONSTRAINT_ROOM_NOT_AVAILABLE){
			ConstraintRoomNotAvailable* crna=(ConstraintRoomNotAvailable*)ctr;
			if(crna->roomName==initialRoomName)
				crna->roomName=finalRoomName;
			j++;
		}
		else if(ctr->type==CONSTRAINT_TEACHER_REQUIRES_ROOM){
			ConstraintTeacherRequiresRoom* c=(ConstraintTeacherRequiresRoom*)ctr;
			if(c->roomName==initialRoomName)
				c->roomName=finalRoomName;
			j++;
		}
		else if(ctr->type==CONSTRAINT_TEACHER_SUBJECT_REQUIRE_ROOM){
			ConstraintTeacherSubjectRequireRoom* c=(ConstraintTeacherSubjectRequireRoom*)ctr;
			if(c->roomName==initialRoomName)
				c->roomName=finalRoomName;
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
		else
			j++;
	}

	searchedRoom->name=finalRoomName;
	searchedRoom->type=type;
	searchedRoom->building=building;
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
	QString xmlReadingLog="";
	QString tmp=OUTPUT_DIR+FILE_SEP+XML_PARSING_LOG_FILENAME;
	QFile file2(tmp);
	if(!file2.open(QIODevice::WriteOnly)){
		assert(0);
		exit(1);
	}
	QTextStream logStream(&file2);
	logStream.setEncoding(QTextStream::UnicodeUTF8);
	
	QDomElement elem1=doc.documentElement();
	xmlReadingLog+=" Found "+elem1.tagName()+" tag\n";
	bool okAbove3_12_17=true;
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
		}
	}
	if(!okAbove3_12_17){ //trying version 3.6.1 to version 3.12.16
		cout<<"Invalid fet 3.12.17 or above - trying older version"<<endl;
		if(elem1.tagName()!=INPUT_FILE_TAG_3_6_1){
			file2.close();
			return false;
		}
	}
	else if(warning){
		QMessageBox::warning(NULL, QObject::tr("FET information"), 
		 QObject::tr("Trying to open a newer file - please update your FET software to the latest version"));
	}
	
	this->nHoursPerDay=12;
	this->hoursOfTheDay[0]="0";
	this->hoursOfTheDay[1]="1";
	this->hoursOfTheDay[2]="2";
	this->hoursOfTheDay[3]="3";
	this->hoursOfTheDay[4]="4";
	this->hoursOfTheDay[5]="5";
	this->hoursOfTheDay[6]="6";
	this->hoursOfTheDay[7]="7";
	this->hoursOfTheDay[8]="8";
	this->hoursOfTheDay[9]="9";
	this->hoursOfTheDay[10]="10";
	this->hoursOfTheDay[11]="11";
	this->hoursOfTheDay[12]="12";

	this->nDaysPerWeek=5;
	this->daysOfTheWeek[0] = QObject::tr("Monday");
	this->daysOfTheWeek[1] = QObject::tr("Tuesday");
	this->daysOfTheWeek[2] = QObject::tr("Wednesday");
	this->daysOfTheWeek[3] = QObject::tr("Thursday");
	this->daysOfTheWeek[4] = QObject::tr("Friday");

	this->institutionName=QObject::tr("Default institution");
	this->comments=QObject::tr("Default comments");

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
					int p=PARITY_NOT_INITIALIZED;
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
							xmlReadingLog+="    Current activity is weekly\n";
							assert(p==PARITY_NOT_INITIALIZED);
							p=PARITY_WEEKLY;
						}
						//old tag
						else if(elem4.tagName()=="Biweekly"){
							xmlReadingLog+="    Current activity is fortnightly\n";
							assert(p==PARITY_NOT_INITIALIZED);
							p=PARITY_FORTNIGHTLY;
						}
						else if(elem4.tagName()=="Fortnightly"){
							xmlReadingLog+="    Current activity is fortnightly\n";
							assert(p==PARITY_NOT_INITIALIZED);
							p=PARITY_FORTNIGHTLY;
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
							d, td, p, ac, -1, -1, cnos, nos);
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
			int tmp=0;
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
			xmlReadingLog+="  Added "+QString::number(tmp)+" equipments\n";
		}
		else if(elem2.tagName()=="Buildings_List"){
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
			xmlReadingLog+="  Added "+QString::number(tmp)+" buildings\n";
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
					rm->type="";
					rm->capacity=1000; //infinite, if not specified
					rm->building="";
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
							rm->type=elem4.text();
							xmlReadingLog+="    Read room type: "+rm->type+"\n";
						}
						else if(elem4.tagName()=="Capacity"){
							rm->capacity=elem4.text().toInt();
							xmlReadingLog+="    Read room capacity: "+QString::number(rm->capacity)+"\n";
						}
						else if(elem4.tagName()=="Equipment"){
							rm->addEquipment(elem4.text());
							xmlReadingLog+="    Read room equipment: "+elem4.text()+"\n";
						}
						else if(elem4.tagName()=="Building"){
							rm->building=elem4.text();
							xmlReadingLog+="    Read room building: "+elem4.text()+"\n";
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
						else if(elem4.tagName()=="Teacher_Name"){
							cn->teacherName=elem4.text();
							xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
						}
						else if(elem4.tagName()=="Day"){
							for(cn->d=0; cn->d<this->nDaysPerWeek; cn->d++)
								if(this->daysOfTheWeek[cn->d]==elem4.text())
									break;
							assert(cn->d<this->nDaysPerWeek);
							xmlReadingLog+="    Crt. day="+this->daysOfTheWeek[cn->d]+"\n";
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
						else if(elem4.tagName()=="Teacher_Name"){
							cn->teacherName=elem4.text();
							xmlReadingLog+="    Read teacher name="+cn->teacherName+"\n";
						}
						else if(elem4.tagName()=="Max_Days_Per_Week"){
							cn->maxDaysPerWeek=elem4.text().toInt();
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
						else if(elem4.tagName()=="Day"){
							for(cn->d=0; cn->d<this->nDaysPerWeek; cn->d++)
								if(this->daysOfTheWeek[cn->d]==elem4.text())
									break;
							assert(cn->d<this->nDaysPerWeek);
							xmlReadingLog+="    Crt. day="+this->daysOfTheWeek[cn->d]+"\n";
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
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintMinNDaysBetweenActivities"){
					ConstraintMinNDaysBetweenActivities* cn=new ConstraintMinNDaysBetweenActivities();
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
						else if(elem4.tagName()=="MinDays"){
							cn->minDays=elem4.text().toInt();
							xmlReadingLog+="    Read MinDays="+QString::number(cn->minDays)+"\n";
						}
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
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintTeachersMaxHoursContinuously"
					//TODO: erase the line below. It is only kept for compatibility with older versions
					|| elem3.tagName()=="ConstraintTeachersNoMoreThanXHoursContinuously"){
					ConstraintTeachersMaxHoursContinuously* cn=new ConstraintTeachersMaxHoursContinuously();
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
						else if(elem4.tagName()=="Maximum_Hours_Continuously"){
							cn->maxHoursContinuously=elem4.text().toInt();
							xmlReadingLog+="    Read maxHoursContinuously="+QString::number(cn->maxHoursContinuously)+"\n";
						}
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintTeachersMaxHoursDaily"
					//TODO: erase the line below. It is only kept for compatibility with older versions
					|| elem3.tagName()=="ConstraintTeachersNoMoreThanXHoursDaily"){
					ConstraintTeachersMaxHoursDaily* cn=new ConstraintTeachersMaxHoursDaily();
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
						else if(elem4.tagName()=="Maximum_Hours_Daily"){
							cn->maxHoursDaily=elem4.text().toInt();
							xmlReadingLog+="    Read maxHoursDaily="+QString::number(cn->maxHoursDaily)+"\n";
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
						else if(elem4.tagName()=="Minimum_Hours_Daily"){
							cn->minHoursDaily=elem4.text().toInt();
							xmlReadingLog+="    Read minHoursDaily="+QString::number(cn->minHoursDaily)+"\n";
						}
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintTeachersSubgroupsMaxHoursDaily"
					//TODO: erase the line below. It is only kept for compatibility with older versions
					|| elem3.tagName()=="ConstraintTeachersSubgroupsNoMoreThanXHoursDaily"){
					ConstraintTeachersSubgroupsMaxHoursDaily* cn=new ConstraintTeachersSubgroupsMaxHoursDaily();
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
						else if(elem4.tagName()=="Maximum_Hours_Daily"){
							cn->maxHoursDaily=elem4.text().toInt();
							xmlReadingLog+="    Read maxHoursDaily="+QString::number(cn->maxHoursDaily)+"\n";
						}
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintStudentsNHoursDaily"){
					ConstraintStudentsNHoursDaily* cn=new ConstraintStudentsNHoursDaily();
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
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintStudentsSetNHoursDaily"){
					ConstraintStudentsSetNHoursDaily* cn=new ConstraintStudentsSetNHoursDaily();
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
						else if(elem4.tagName()=="Activity_Id"){
							cn->activityId=elem4.text().toInt();
							xmlReadingLog+="    Read activity id="+QString::number(cn->activityId)+"\n";
						}
						else if(elem4.tagName()=="Preferred_Day"){
							for(cn->day=0; cn->day<this->nDaysPerWeek; cn->day++)
								if(this->daysOfTheWeek[cn->day]==elem4.text())
									break;
							assert(cn->day<this->nDaysPerWeek);
							xmlReadingLog+="    Preferred day="+this->daysOfTheWeek[cn->day]+"\n";
						}
						else if(elem4.tagName()=="Preferred_Hour"){
							for(cn->hour=0; cn->hour < this->nHoursPerDay; cn->hour++)
								if(this->hoursOfTheDay[cn->hour]==elem4.text())
									break;
							assert(cn->hour>=0 && cn->hour < this->nHoursPerDay);
							xmlReadingLog+="    Preferred hour="+this->hoursOfTheDay[cn->hour]+"\n";
						}
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintActivityEndsDay"){
					ConstraintActivityEndsDay* cn=new ConstraintActivityEndsDay();
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
						else if(elem4.tagName()=="Activity_Id"){
							cn->activityId=elem4.text().toInt();
							xmlReadingLog+="    Read activity id="+QString::number(cn->activityId)+"\n";
						}
					}
					crt_constraint=cn;
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
									assert(cn->days[i]<this->nDaysPerWeek);
									xmlReadingLog+="    Preferred day="+this->daysOfTheWeek[cn->days[i]]+"("+QString::number(i)+")"+"\n";
								}
								else if(elem5.tagName()=="Preferred_Hour"){
									for(cn->hours[i]=0; cn->hours[i] < this->nHoursPerDay; cn->hours[i]++)
										if(this->hoursOfTheDay[cn->hours[i]]==elem5.text())
											break;
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
						else if(elem4.tagName()=="Day"){
							for(cn->d=0; cn->d<this->nDaysPerWeek; cn->d++)
								if(this->daysOfTheWeek[cn->d]==elem4.text())
									break;
							assert(cn->d<this->nDaysPerWeek);
							xmlReadingLog+="    Crt. day="+this->daysOfTheWeek[cn->d]+"\n";
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
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintTeachersNoGaps"){
					ConstraintTeachersNoGaps* cn=new ConstraintTeachersNoGaps();
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
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintStudentsNoGaps"){
					ConstraintStudentsNoGaps* cn=new ConstraintStudentsNoGaps();
					bool compulsory_read=false;
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
							compulsory_read=true;
						}
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintStudentsSetNoGaps"){
					ConstraintStudentsSetNoGaps* cn=new ConstraintStudentsSetNoGaps();
					bool compulsory_read=false;
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
							compulsory_read=true;
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
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintStudentsSetIntervalMaxDaysPerWeek"){
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
				}
				if(elem3.tagName()=="ConstraintTeacherIntervalMaxDaysPerWeek"){
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
				if(elem3.tagName()=="Constraint2ActivitiesOrdered"){
					Constraint2ActivitiesOrdered* cn=new Constraint2ActivitiesOrdered();
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
				if(elem3.tagName()=="Constraint2ActivitiesGrouped"){
					Constraint2ActivitiesGrouped* cn=new Constraint2ActivitiesGrouped();
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
									assert(cn->days[i]<this->nDaysPerWeek);
									xmlReadingLog+="    Preferred day="+this->daysOfTheWeek[cn->days[i]]+"("+QString::number(i)+")"+"\n";
								}
								else if(elem5.tagName()=="Preferred_Hour"){
									for(cn->hours[i]=0; cn->hours[i] < this->nHoursPerDay; cn->hours[i]++)
										if(this->hoursOfTheDay[cn->hours[i]]==elem5.text())
											break;
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
				if(elem3.tagName()=="ConstraintTeachersSubjectTagsMaxHoursContinuously"){
					ConstraintTeachersSubjectTagsMaxHoursContinuously* cn=new ConstraintTeachersSubjectTagsMaxHoursContinuously();
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
						else if(elem4.tagName()=="Maximum_Hours_Continuously"){
							cn->maxHoursContinuously=elem4.text().toInt();
							xmlReadingLog+="    Read maxHoursContinuously="+QString::number(cn->maxHoursContinuously)+"\n";
						}
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintTeachersSubjectTagMaxHoursContinuously"){
					ConstraintTeachersSubjectTagMaxHoursContinuously* cn=new ConstraintTeachersSubjectTagMaxHoursContinuously();
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
				}

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
						else if(elem4.tagName()=="Room_Name"){
							cn->roomName=elem4.text();
							xmlReadingLog+="    Read room name="+cn->roomName+"\n";
						}
						else if(elem4.tagName()=="Day"){
							for(cn->d=0; cn->d<this->nDaysPerWeek; cn->d++)
								if(this->daysOfTheWeek[cn->d]==elem4.text())
									break;
							assert(cn->d<this->nDaysPerWeek);
							xmlReadingLog+="    Crt. day="+this->daysOfTheWeek[cn->d]+"\n";
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
					}
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintRoomTypeNotAllowedSubjects"){
					ConstraintRoomTypeNotAllowedSubjects* cn=new ConstraintRoomTypeNotAllowedSubjects();
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
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintSubjectRequiresEquipments"){
					ConstraintSubjectRequiresEquipments* cn=new ConstraintSubjectRequiresEquipments();
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
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintSubjectSubjectTagRequireEquipments"){
					ConstraintSubjectSubjectTagRequireEquipments* cn=new ConstraintSubjectSubjectTagRequireEquipments();
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
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintTeacherRequiresRoom"){
					ConstraintTeacherRequiresRoom* cn=new ConstraintTeacherRequiresRoom();
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
				}
				if(elem3.tagName()=="ConstraintTeacherSubjectRequireRoom"){
					ConstraintTeacherSubjectRequireRoom* cn=new ConstraintTeacherSubjectRequireRoom();
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
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintMinimizeNumberOfRoomsForStudents"){
					ConstraintMinimizeNumberOfRoomsForStudents* cn=new ConstraintMinimizeNumberOfRoomsForStudents();
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
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintMinimizeNumberOfRoomsForTeachers"){
					ConstraintMinimizeNumberOfRoomsForTeachers* cn=new ConstraintMinimizeNumberOfRoomsForTeachers();
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
					crt_constraint=cn;
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
				if(elem3.tagName()=="ConstraintActivitiesSameRoom"){
					ConstraintActivitiesSameRoom* cn=new ConstraintActivitiesSameRoom();
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
				if(elem3.tagName()=="ConstraintMaxBuildingChangesPerDayForTeachers"){
					ConstraintMaxBuildingChangesPerDayForTeachers* cn=new ConstraintMaxBuildingChangesPerDayForTeachers();
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
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintMaxBuildingChangesPerDayForStudents"){
					ConstraintMaxBuildingChangesPerDayForStudents* cn=new ConstraintMaxBuildingChangesPerDayForStudents();
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
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintMaxRoomChangesPerDayForTeachers"){
					ConstraintMaxRoomChangesPerDayForTeachers* cn=new ConstraintMaxRoomChangesPerDayForTeachers();
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
					crt_constraint=cn;
				}
				if(elem3.tagName()=="ConstraintMaxRoomChangesPerDayForStudents"){
					ConstraintMaxRoomChangesPerDayForStudents* cn=new ConstraintMaxRoomChangesPerDayForStudents();
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
					crt_constraint=cn;
				}

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
			xmlReadingLog+="  Added "+QString::number(nc)+" space constraints\n";
		}
	}

	this->internalStructureComputed=false;

	logStream<<xmlReadingLog;
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
		assert(0);
		exit(1);
	}

	QTextStream tos(&file);
	
	tos.setEncoding(QTextStream::UnicodeUTF8);

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

	//equipments list
	s += "<Equipments_List>\n";
	for(int i=0; i<this->equipmentsList.size(); i++){
		Equipment* eq=this->equipmentsList[i];
		s += eq->getXmlDescription();
	}
	s += "</Equipments_List>\n\n";

	//buildings list
	s += "<Buildings_List>\n";
	for(int i=0; i<this->buildingsList.size(); i++)
		s+=this->buildingsList.at(i)->getXmlDescription();
	s += "</Buildings_List>\n\n";

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
