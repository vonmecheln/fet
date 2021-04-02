/*
File spaceconstraint.cpp
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

#define minimu(x,y)	((x)<(y)?(x):(y))
#define maximu(x,y)	((x)>(y)?(x):(y))

#include <iostream>
using namespace std;

#include "timetable_defs.h"
#include "spaceconstraint.h"
#include "rules.h"
#include "activity.h"
#include "teacher.h"
#include "subject.h"
#include "subjecttag.h"
#include "studentsset.h"
#include "room.h"
#include "solution.h"

#include <qstring.h>

#include <QMessageBox>

//static Solution* crt_chrom=NULL;
//static Rules* crt_rules=NULL;

#define yesNo(x)				((x)==0?"no":"yes")
#define yesNoTranslated(x)		((x)==0?QObject::tr("no"):QObject::tr("yes"))

static qint8 roomsMatrix[MAX_ROOMS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

static int rooms_conflicts=-1;

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

SpaceConstraint::SpaceConstraint()
{
	this->type=CONSTRAINT_GENERIC_SPACE;
}

SpaceConstraint::~SpaceConstraint()
{
}

SpaceConstraint::SpaceConstraint(double wp)
{
	this->weightPercentage=wp;
	assert(wp<=100 && wp>=0);
	this->type=CONSTRAINT_GENERIC_SPACE;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

ConstraintBasicCompulsorySpace::ConstraintBasicCompulsorySpace()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_BASIC_COMPULSORY_SPACE;
	this->weightPercentage=100;
}

ConstraintBasicCompulsorySpace::ConstraintBasicCompulsorySpace(double wp)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_BASIC_COMPULSORY_SPACE;
}

bool ConstraintBasicCompulsorySpace::computeInternalStructure(Rules& r)
{
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;
	/*do nothing*/
	
	return true;
}

QString ConstraintBasicCompulsorySpace::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s = "<ConstraintBasicCompulsorySpace>\n";
	s += "	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s += "</ConstraintBasicCompulsorySpace>\n";
	return s;
}

QString ConstraintBasicCompulsorySpace::getDescription(Rules& r)
{
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;
	
	QString s = QObject::tr("Basic compulsory constraints (space), WP:%1\%").arg(this->weightPercentage);
	
	return s;
}

QString ConstraintBasicCompulsorySpace::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("These are the basic compulsory constraints \n"
			"(referring to rooms allocation) for any timetable\n");
	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";

	s+=QObject::tr("The basic space constraints try to avoid:\n");
	s+=QObject::tr("- rooms assigned to more than one activity simultaneously\n");
	s+=QObject::tr("- activities with more students than the capacity of the room\n");

	return s;
}

double ConstraintBasicCompulsorySpace::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
	{

	assert(r.internalStructureComputed);

	int roomsConflicts;

	//This constraint fitness calculation routine is called firstly,
	//so we can compute the rooms conflicts faster this way.
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || rooms_conflicts<0 || c.changedForMatrixCalculation){
		rooms_conflicts = roomsConflicts = c.getRoomsMatrix(r, roomsMatrix);

		//crt_chrom = &c;
		//crt_rules = &r;
		
		c.changedForMatrixCalculation=false;
	}
	else{
		assert(rooms_conflicts>=0);
		roomsConflicts=rooms_conflicts;
	}

	int i;

	int unallocated; //unallocated activities
	int nre; //number of room exhaustions
	int nor; //number of overwhelmed rooms

	//part without logging....................................................................
	if(conflictsString==NULL){
		//Unallocated activities
		unallocated=0;
		nor=0;
		for(i=0; i<r.nInternalActivities; i++)
			if(c.rooms[i]==UNALLOCATED_SPACE){
				//Firstly, we consider a big clash each unallocated activity.
				//Needs to be very a large constant, bigger than any other broken constraint.
				unallocated += /*r.internalActivitiesList[i].duration * r.internalActivitiesList[i].nSubgroups * */ 10000;
				//(an unallocated activity for a year is more important than an unallocated activity for a subgroup)
			}
			else if(c.rooms[i]!=UNSPECIFIED_ROOM){
				//The capacity of each room must be respected
				//(the number of students must be less than the capacity)
				int rm=c.rooms[i];
				if(r.internalActivitiesList[i].nTotalStudents>r.internalRoomsList[rm]->capacity){
					int tmp;
					//if(r.internalActivitiesList[i].parity==PARITY_WEEKLY)
					//	tmp=2;
					//else
						tmp=1;
	
					nor+=tmp;
				}
			}

		//Calculates the number of rooms exhaustion (when a room is occupied
		//for more than one activity at the same time)
		/*nre=0;
		for(i=0; i<r.nInternalRooms; i++)
			for(int j=0; j<r.nDaysPerWeek; j++)
				for(int k=0; k<r.nHoursPerDay; k++){
					int tmp=roomsMatrix[i][j][k]-1;
					if(tmp>0){
						if(conflictsString!=NULL){
							QString s=QObject::tr("Space constraint basic compulsory: room with name %1 has more than one allocated activity on day %2, hour %3.")
								.arg(r.internalRoomsList[i]->name)
								.arg(r.daysOfTheWeek[j])
								.arg(r.hoursOfTheDay[k]);
							s+=" ";
							s+=QObject::tr("This increases the conflicts total by %1").arg(tmp*weightPercentage/100);
						
							dl.append(s);
							cl.append(tmp*weightPercentage/100);
						
							*conflictsString += s+"\n";
						}
						nre+=tmp;
					}
				}
		*/
		nre=roomsConflicts;
	}
	//part with logging....................................................................
	else{
		//Unallocated activities
		unallocated=0;
		nor=0;
		for(i=0; i<r.nInternalActivities; i++)
			if(c.rooms[i]==UNALLOCATED_SPACE){
				//Firstly, we consider a big clash each unallocated activity.
				//Needs to be very a large constant, bigger than any other broken constraint.
				unallocated += /*r.internalActivitiesList[i].duration * r.internalActivitiesList[i].nSubgroups * */ 10000;
				//(an unallocated activity for a year is more important than an unallocated activity for a subgroup)
				if(conflictsString!=NULL){
					QString s=QObject::tr("Space constraint basic compulsory: unallocated activity with id=%1").arg(r.internalActivitiesList[i].id);
					s+=" - ";
					s+=QObject::tr(" - this increases the conflicts total by %1").arg(weightPercentage/100*10000);
					
					dl.append(s);
					cl.append(weightPercentage/100 * 10000);
					
					*conflictsString+=s+"\n";

					/*(*conflictsString) += QObject::tr("Space constraint basic compulsory: unallocated activity with id=%1").arg(r.internalActivitiesList[i].id);
					(*conflictsString) += QObject::tr(" - this increases the conflicts total by %1")
						.arg(weight*10000);
					(*conflictsString) += "\n";*/
				}
			}
			else if(c.rooms[i]!=UNSPECIFIED_ROOM){
				//The capacity of each room must be respected
				//(the number of students must be less than the capacity)
				int rm=c.rooms[i];
				if(r.internalActivitiesList[i].nTotalStudents>r.internalRoomsList[rm]->capacity){
					int tmp;
					//if(r.internalActivitiesList[i].parity==PARITY_WEEKLY)
					//	tmp=2;
					//else
						tmp=1;
	
					nor+=tmp;

					if(conflictsString!=NULL){
						QString s;
						s=QObject::tr("Space constraint basic compulsory: room %1 has allocated activity with id %2 and the capacity of the room is overloaded")
						.arg(r.internalRoomsList[rm]->name).arg(r.internalActivitiesList[i].id);
						s+=". ";
						s+=QObject::tr("This increases conflicts total by %1").arg(weightPercentage/100);
						
						dl.append(s);
						cl.append(weightPercentage/100);
						
						*conflictsString += s+"\n";
					}
				}
			}

		//Calculates the number of rooms exhaustion (when a room is occupied
		//for more than one activity at the same time)
		nre=0;
		for(i=0; i<r.nInternalRooms; i++)
			for(int j=0; j<r.nDaysPerWeek; j++)
				for(int k=0; k<r.nHoursPerDay; k++){
					int tmp=roomsMatrix[i][j][k]-1;
					if(tmp>0){
						if(conflictsString!=NULL){
							QString s=QObject::tr("Space constraint basic compulsory: room with name %1 has more than one allocated activity on day %2, hour %3.")
								.arg(r.internalRoomsList[i]->name)
								.arg(r.daysOfTheWeek[j])
								.arg(r.hoursOfTheDay[k]);
							s+=" ";
							s+=QObject::tr("This increases the conflicts total by %1").arg(tmp*weightPercentage/100);
						
							dl.append(s);
							cl.append(tmp*weightPercentage/100);
						
							*conflictsString += s+"\n";
							/*(*conflictsString)+=QObject::tr("Space constraint basic compulsory: room with name %1 has more than one allocated activity on day %2, hour %3.")
								.arg(r.internalRoomsList[i]->name)
								.arg(r.daysOfTheWeek[j])
								.arg(r.hoursOfTheDay[k]);
							(*conflictsString)+=" ";
							(*conflictsString)+=QObject::tr("This increases the conflicts total by %1").arg(tmp*weight);
							(*conflictsString)+="\n";*/
						}
						nre+=tmp;
					}
				}
	}
	/*if(roomsConflicts!=-1)
		assert(nre==roomsConflicts);*/ //just a check, works only on logged fitness calculation
		
	if(this->weightPercentage==100){
		//assert(unallocated==0);
		assert(nre==0);
		assert(nor==0);
	}

	return weightPercentage/100 * (unallocated + nre + nor); //fitness factor
}

bool ConstraintBasicCompulsorySpace::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);
	//if(a)
	//	;

	return false;
}

bool ConstraintBasicCompulsorySpace::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintBasicCompulsorySpace::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintBasicCompulsorySpace::isRelatedToSubjectTag(SubjectTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintBasicCompulsorySpace::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/

	return false;
}

bool ConstraintBasicCompulsorySpace::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	//if(r)
	//	;

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

ConstraintRoomNotAvailable::ConstraintRoomNotAvailable()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_ROOM_NOT_AVAILABLE;
}

ConstraintRoomNotAvailable::ConstraintRoomNotAvailable(double wp, const QString& rn, int day, int start_hour, int end_hour)
	: SpaceConstraint(wp)
{
	this->roomName=rn;
	this->d=day;
	this->h1=start_hour;
	this->h2=end_hour;
	this->type=CONSTRAINT_ROOM_NOT_AVAILABLE;
}

QString ConstraintRoomNotAvailable::getXmlDescription(Rules& r){
	QString s="<ConstraintRoomNotAvailable>\n";
	s+="	<Weight_Percentage>"+QString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Room_Name>"+protect(this->roomName)+"</Room_Name>\n";
	s+="	<Day>"+protect(r.daysOfTheWeek[this->d])+"</Day>\n";
	s+="	<Start_Hour>"+protect(r.hoursOfTheDay[this->h1])+"</Start_Hour>\n";
	s+="	<End_Hour>"+protect(r.hoursOfTheDay[this->h2])+"</End_Hour>\n";
	s+="</ConstraintRoomNotAvailable>\n";
	return s;
}

QString ConstraintRoomNotAvailable::getDescription(Rules& r){
	QString s=QObject::tr("Room not available");s+=",";
	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));s+=", ";
	s+=(QObject::tr("R:%1").arg(this->roomName));s+=", ";
	s+=(QObject::tr("D:%1").arg(r.daysOfTheWeek[this->d]));s+=", ";
	s+=(QObject::tr("SH:%1").arg(r.hoursOfTheDay[this->h1]));s+=", ";
	s+=(QObject::tr("EH:%1").arg(r.hoursOfTheDay[this->h2]));s+=", ";

	return s;
}

QString ConstraintRoomNotAvailable::getDetailedDescription(Rules& r){
	QString s=QObject::tr("Space constraint");s+="\n";
	s+=QObject::tr("Room not available");s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	s+=(QObject::tr("Room=%1").arg(this->roomName));s+="\n";
	s+=(QObject::tr("Day=%1").arg(r.daysOfTheWeek[this->d]));s+="\n";
	s+=(QObject::tr("Start hour=%1").arg(r.hoursOfTheDay[this->h1]));s+="\n";
	s+=(QObject::tr("End hour=%1").arg(r.hoursOfTheDay[this->h2]));s+="\n";

	return s;
}

bool ConstraintRoomNotAvailable::computeInternalStructure(Rules& r){
	this->room_ID=r.searchRoom(this->roomName);
	
	if(this->room_ID<0){
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Constraint room not available is wrong because it refers to inexistent room."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}	
	if(this->d >= r.nDaysPerWeek){
		QMessageBox::information(NULL, QObject::tr("FET information"),
		 QObject::tr("Constraint room not available is wrong because it refers to removed day. Please correct"
		 " and try again. Correcting means editing it and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}
	if(this->h1 > r.nHoursPerDay){
		QMessageBox::information(NULL, QObject::tr("FET information"),
		 QObject::tr("Constraint room not available is wrong because it refers to removed start hour. Please correct"
		 " and try again. Correcting means editing it and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}
	if(this->h2 > r.nHoursPerDay){
		QMessageBox::information(NULL, QObject::tr("FET information"),
		 QObject::tr("Constraint room not available is wrong because it refers to removed end hour. Please correct"
		 " and try again. Correcting means editing it and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}
	if(this->h1 >= this->h2){
		QMessageBox::information(NULL, QObject::tr("FET information"),
		 QObject::tr("Constraint room not available is wrong because start hour >= end hour. Please correct"
		 " and try again. Correcting means editing it and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}
	
	assert(this->room_ID>=0);
	
	return true;
}

double ConstraintRoomNotAvailable::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrices roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || rooms_conflicts<0 || c.changedForMatrixCalculation){
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		//crt_chrom = &c;
		//crt_rules = &r;
		
		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of hours when the roomr is supposed to be occupied,
	//but it is not available
	//This function consideres all the hours, I mean if there are for example 5 weekly courses
	//scheduled on that hour (which is already a broken compulsory constraint - we only
	//are allowed 1 weekly activity for a certain room at a certain hour) we calculate
	//5 broken constraints for that function.
	//TODO: decide if it is better to consider only 2 or 10 as a return value in this particular case
	//(currently it is 10)
	int i=this->room_ID;
	int j=this->d;
	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int k=h1; k<h2; k++)
			if(roomsMatrix[i][j][k]>0)
				nbroken+=roomsMatrix[i][j][k];
	}
	//with logging
	else{
		nbroken=0;
		for(int k=h1; k<h2; k++)
			if(roomsMatrix[i][j][k]>0){
				if(conflictsString!=NULL){
					QString s=QObject::tr("Space constraint room not available broken for room %1 on day %2, hour %3")
						.arg(r.internalRoomsList[i]->name)
						.arg(r.daysOfTheWeek[j])
						.arg(k);
					s += ". ";
					s += QObject::tr("This increases the conflicts total by %1").arg(roomsMatrix[i][j][k]*weightPercentage/100);
				
					dl.append(s);
					cl.append(roomsMatrix[i][j][k]*weightPercentage/100);
			
					*conflictsString += s+"\n";
					
					/**conflictsString+=
						(QObject::tr("Space constraint room not available broken for room %1 on day %2, hour %3")
						.arg(r.internalRoomsList[i]->name)
						.arg(r.daysOfTheWeek[j])
						.arg(k));
					*conflictsString += ". ";
					*conflictsString += (QObject::tr("This increases the conflicts total by %1").arg(roomsMatrix[i][j][k]*weight));
					*conflictsString += "\n";*/
				}
				nbroken+=roomsMatrix[i][j][k];
			}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintRoomNotAvailable::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);
	//if(a)
	//	;

	return false;
}

bool ConstraintRoomNotAvailable::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintRoomNotAvailable::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintRoomNotAvailable::isRelatedToSubjectTag(SubjectTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintRoomNotAvailable::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/

	return false;
}

bool ConstraintRoomNotAvailable::isRelatedToRoom(Room* r)
{
	return this->roomName==r->name;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivityPreferredRoom::ConstraintActivityPreferredRoom()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_ACTIVITY_PREFERRED_ROOM;
}

ConstraintActivityPreferredRoom::ConstraintActivityPreferredRoom(double wp, int aid, const QString& room)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_ACTIVITY_PREFERRED_ROOM;
	this->activityId=aid;
	this->roomName=room;
}

bool ConstraintActivityPreferredRoom::operator==(ConstraintActivityPreferredRoom& c){
	if(this->roomName!=c.roomName)
		return false;
	if(this->activityId!=c.activityId)
		return false;
	if(this->weightPercentage!=c.weightPercentage)
		return false;
	return true;
}

bool ConstraintActivityPreferredRoom::computeInternalStructure(Rules& r)
{
	this->_activity=-1;
	int ac;
	for(ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].id==this->activityId){
			assert(this->_activity==-1);
			this->_activity=ac;
			break;
		}
	if(ac==r.nInternalActivities){
		QMessageBox::warning(NULL, QObject::tr("FET error in data"), 
			QObject ::tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
		
	this->_room = r.searchRoom(this->roomName);
	assert(this->_room>=0);
	
	return true;
}

QString ConstraintActivityPreferredRoom::getXmlDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="<ConstraintActivityPreferredRoom>\n";
	s+="	<Weight_Percentage>"+QString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Activity_Id>"+QString::number(this->activityId)+"</Activity_Id>\n";
	s+="	<Room>"+protect(this->roomName)+"</Room>\n";
		
	s+="</ConstraintActivityPreferredRoom>\n";

	return s;
}

QString ConstraintActivityPreferredRoom::getDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="Activity preferred room"; s+=", ";
	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);s+=", ";
	s+=QObject::tr("A:%1").arg(this->activityId);

	//* write the teachers, subject and students sets
	int ai;
	for(ai=0; ai<r.activitiesList.size(); ai++)
		if(r.activitiesList[ai]->id==this->activityId)
			break;
	s+=" (";	
	if(ai==r.activitiesList.size()){
		s+=QObject::tr(" Invalid (inexistent) activity id for constraint activity preferred room");
	}
	else{
		assert(ai<r.activitiesList.size());
		s+=QObject::tr("T:");
		int k=0;
		foreach(QString ss, r.activitiesList[ai]->teachersNames){
			if(k>0)
				s+=",";
			s+=ss;
			k++;
		}
	
		s+=QObject::tr(",S:");
		s+=r.activitiesList[ai]->subjectName;
	
		if(r.activitiesList[ai]->subjectTagName!="")
			s+=QObject::tr(",ST:")+r.activitiesList[ai]->subjectTagName;
	
		s+=QObject::tr(",St:");
		k=0;
		foreach(QString ss, r.activitiesList[ai]->studentsNames){
			if(k>0)
				s+=",";
			s+=ss;
			k++;
		}	
	}
	s+=")";
	//* end section

	s+=", ";

	s+=QObject::tr("R:%1").arg(this->roomName);

	return s;
}

QString ConstraintActivityPreferredRoom::getDetailedDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Space constraint"); s+="\n";
	s+=QObject::tr("Activity preferred room"); s+="\n";
	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";
	
	s+=QObject::tr("Activity id=%1").arg(this->activityId);
	
	//* write the teachers, subject and students sets
	int ai;
	for(ai=0; ai<r.activitiesList.size(); ai++)
		if(r.activitiesList[ai]->id==this->activityId)
			break;
	s+=" (";	
	if(ai==r.activitiesList.size()){
		s+=QObject::tr(" Invalid (inexistent) activity id for constraint activity preferred room");
	}
	else{
		assert(ai<r.activitiesList.size());
		s+=QObject::tr("T:");
		int k=0;
		foreach(QString ss, r.activitiesList[ai]->teachersNames){
			if(k>0)
				s+=",";
			s+=ss;
			k++;
		}
	
		s+=QObject::tr(",S:");
		s+=r.activitiesList[ai]->subjectName;
	
		if(r.activitiesList[ai]->subjectTagName!="")
			s+=QObject::tr(",ST:")+r.activitiesList[ai]->subjectTagName;
	
		s+=QObject::tr(",St:");
		k=0;
		foreach(QString ss, r.activitiesList[ai]->studentsNames){
			if(k>0)
				s+=",";
			s+=ss;
			k++;
		}	
	}
	s+=")";
	//* end section
		
	s+="\n";
	
	s+=QObject::tr("Room=%1").arg(this->roomName);s+="\n";

	return s;
}

double ConstraintActivityPreferredRoom::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || rooms_conflicts<0 || c.changedForMatrixCalculation){
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		//crt_chrom = &c;
		//crt_rules = &r;
		
		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts

	int nbroken;
	
	bool ok=true;

	nbroken=0;

	int rm=c.rooms[this->_activity];
	if(/*rm!=UNALLOCATED_SPACE &&*/ rm!=this->_room){
		if(rm!=UNALLOCATED_SPACE){
			ok=false;

			if(conflictsString!=NULL){
				QString s=QObject::tr("Space constraint activity preferred room broken for activity with id=%1 (subject=%2), room=%3")
					.arg(this->activityId)
					.arg(r.internalActivitiesList[this->_activity].subjectName)
					.arg(this->roomName);
					s += ". ";
				s += (QObject::tr("This increases the conflicts total by %1").arg(weightPercentage/100* 1));
		
				dl.append(s);
				cl.append(1*weightPercentage/100);
			
				*conflictsString += s+"\n";
			}
			nbroken++;
		}
	}
	
	if(this->weightPercentage==100)
		assert(ok);

	return weightPercentage/100 * nbroken;
}

bool ConstraintActivityPreferredRoom::isRelatedToActivity(Activity* a)
{
	if(this->activityId==a->id)
		return true;
	return false;
}

bool ConstraintActivityPreferredRoom::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintActivityPreferredRoom::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintActivityPreferredRoom::isRelatedToSubjectTag(SubjectTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintActivityPreferredRoom::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/

	return false;
}

bool ConstraintActivityPreferredRoom::isRelatedToRoom(Room* r)
{
	return r->name==this->roomName;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivityPreferredRooms::ConstraintActivityPreferredRooms()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_ACTIVITY_PREFERRED_ROOMS;
}

ConstraintActivityPreferredRooms::ConstraintActivityPreferredRooms(double wp, int aid, const QStringList& roomsList)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_ACTIVITY_PREFERRED_ROOMS;
	this->activityId=aid;
	this->roomsNames=roomsList;
	assert(roomsList.size()<=MAX_CONSTRAINT_ACTIVITY_PREFERRED_ROOMS);
}

bool ConstraintActivityPreferredRooms::computeInternalStructure(Rules& r)
{
	this->_activity=-1;
	int ac;
	for(ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].id==this->activityId){
			assert(this->_activity==-1);
			this->_activity=ac;
			break;
		}
		
	if(ac==r.nInternalActivities){
		QMessageBox::warning(NULL, QObject::tr("FET error in data"), 
			QObject ::tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
		
	this->_n_preferred_rooms=this->roomsNames.count();
	int i=0;
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		this->_rooms[i] = r.searchRoom(*it);
		assert(this->_rooms[i]>=0);
		i++;
	}
	
	return true;
}

QString ConstraintActivityPreferredRooms::getXmlDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="<ConstraintActivityPreferredRooms>\n";
	s+="	<Weight_Percentage>"+QString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Activity_Id>"+QString::number(this->activityId)+"</Activity_Id>\n";
	s+="	<Number_of_Preferred_Rooms>"+QString::number(this->roomsNames.count())+"</Number_of_Preferred_Rooms>\n";
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++)
		s+="	<Preferred_Room>"+protect(*it)+"</Preferred_Room>\n";
		
	s+="</ConstraintActivityPreferredRooms>\n";

	return s;
}

QString ConstraintActivityPreferredRooms::getDescription(Rules& r){
	//Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="Activity preferred rooms"; s+=", ";
	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);s+=", ";
	s+=QObject::tr("A:%1").arg(this->activityId);

	//* write the teachers, subject and students sets
	int ai;
	for(ai=0; ai<r.activitiesList.size(); ai++)
		if(r.activitiesList[ai]->id==this->activityId)
			break;
	s+=" (";	
	if(ai==r.activitiesList.size()){
		s+=QObject::tr(" Invalid (inexistent) activity id for constraint activity preferred rooms");
	}
	else{
		assert(ai<r.activitiesList.size());
		s+=QObject::tr("T:");
		int k=0;
		foreach(QString ss, r.activitiesList[ai]->teachersNames){
			if(k>0)
				s+=",";
			s+=ss;
			k++;
		}
	
		s+=QObject::tr(",S:");
		s+=r.activitiesList[ai]->subjectName;
	
		if(r.activitiesList[ai]->subjectTagName!="")
			s+=QObject::tr(",ST:")+r.activitiesList[ai]->subjectTagName;
	
		s+=QObject::tr(",St:");
		k=0;
		foreach(QString ss, r.activitiesList[ai]->studentsNames){
			if(k>0)
				s+=",";
			s+=ss;
			k++;
		}	
	}
	s+=")";
	//* end section

	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		s+=", ";
		s+=QObject::tr("R:%1").arg(*it);
	}

	return s;
}

QString ConstraintActivityPreferredRooms::getDetailedDescription(Rules& r){
	//Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Space constraint"); s+="\n";
	s+=QObject::tr("Activity preferred rooms"); s+="\n";
	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";
	
	s+=QObject::tr("Activity id=%1").arg(this->activityId);
	
	//* write the teachers, subject and students sets
	int ai;
	for(ai=0; ai<r.activitiesList.size(); ai++)
		if(r.activitiesList[ai]->id==this->activityId)
			break;
	s+=" (";	
	if(ai==r.activitiesList.size()){
		s+=QObject::tr(" Invalid (inexistent) activity id for constraint activity preferred rooms");
	}
	else{
		assert(ai<r.activitiesList.size());
		s+=QObject::tr("T:");
		int k=0;
		foreach(QString ss, r.activitiesList[ai]->teachersNames){
			if(k>0)
				s+=",";
			s+=ss;
			k++;
		}
	
		s+=QObject::tr(",S:");
		s+=r.activitiesList[ai]->subjectName;
	
		if(r.activitiesList[ai]->subjectTagName!="")
			s+=QObject::tr(",ST:")+r.activitiesList[ai]->subjectTagName;
	
		s+=QObject::tr(",St:");
		k=0;
		foreach(QString ss, r.activitiesList[ai]->studentsNames){
			if(k>0)
				s+=",";
			s+=ss;
			k++;
		}	
	}
	s+=")";
	//* end section

	s+="\n";
	
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		s+=QObject::tr("Room=%1").arg(*it);
		s+="\n";
	}

	return s;
}

double ConstraintActivityPreferredRooms::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || rooms_conflicts<0 || c.changedForMatrixCalculation){
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		//crt_chrom = &c;
		//crt_rules = &r;
		
		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts

	int nbroken;
	
	bool ok=true;

	nbroken=0;

	int rm=c.rooms[this->_activity];
	if(1 || rm!=UNALLOCATED_SPACE){
		int i;
		for(i=0; i<this->_n_preferred_rooms; i++)
			if(this->_rooms[i]==rm)
				break;
		if(i==this->_n_preferred_rooms){
			if(rm!=UNALLOCATED_SPACE){
				ok=false;
		
				if(conflictsString!=NULL){
					QString s=QObject::tr("Space constraint activity preferred rooms broken for activity with id=%1 (subject=%2)")
						.arg(this->activityId)
						.arg(r.internalActivitiesList[this->_activity].subjectName);
					s += ". ";
					s += QObject::tr("This increases the conflicts total by %1").arg(weightPercentage/100 * 1);
				
					dl.append(s);
					cl.append(weightPercentage/100 * 1);
				
					*conflictsString += s+"\n";
				}

				nbroken++;
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(ok);

	return weightPercentage/100 * nbroken;
}

bool ConstraintActivityPreferredRooms::isRelatedToActivity(Activity* a)
{
	if(this->activityId==a->id)
		return true;
	return false;
}

bool ConstraintActivityPreferredRooms::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintActivityPreferredRooms::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintActivityPreferredRooms::isRelatedToSubjectTag(SubjectTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintActivityPreferredRooms::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/

	return false;
}

bool ConstraintActivityPreferredRooms::isRelatedToRoom(Room* r)
{
	return this->roomsNames.contains(r->name);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintSubjectPreferredRoom::ConstraintSubjectPreferredRoom()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_SUBJECT_PREFERRED_ROOM;
}

ConstraintSubjectPreferredRoom::ConstraintSubjectPreferredRoom(double wp, const QString& subj, const QString& rm)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_SUBJECT_PREFERRED_ROOM;
	this->subjectName=subj;
	this->roomName=rm;
}

bool ConstraintSubjectPreferredRoom::computeInternalStructure(Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the subject of the constraint.
	
	this->_nActivities=0;
	for(int ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].subjectName == this->subjectName){
			assert(this->_nActivities<MAX_ACTIVITIES_FOR_A_SUBJECT);
			this->_activities[this->_nActivities++]=ac;
		}
		
	this->_room = r.searchRoom(this->roomName);
	assert(this->_room>=0);
	
	return true;
}

QString ConstraintSubjectPreferredRoom::getXmlDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="<ConstraintSubjectPreferredRoom>\n";
	s+="	<Weight_Percentage>"+QString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Subject>"+protect(this->subjectName)+"</Subject>\n";
	s+="	<Room>"+protect(this->roomName)+"</Room>\n";
		
	s+="</ConstraintSubjectPreferredRoom>\n";

	return s;
}

QString ConstraintSubjectPreferredRoom::getDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="Subject preferred room"; s+=", ";
	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);s+=", ";
	s+=QObject::tr("S:%1").arg(this->subjectName);s+=",";
	s+=QObject::tr("R:%1").arg(this->roomName);

	return s;
}

QString ConstraintSubjectPreferredRoom::getDetailedDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Space constraint"); s+="\n";
	s+=QObject::tr("Subject preferred room"); s+="\n";
	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";
	s+=QObject::tr("Subject=%1").arg(this->subjectName);s+="\n";
	s+=QObject::tr("Room name=%1").arg(this->roomName);s+="\n";

	return s;
}

double ConstraintSubjectPreferredRoom::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || rooms_conflicts<0 || c.changedForMatrixCalculation){
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		//crt_chrom = &c;
		//crt_rules = &r;
		
		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and subject tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	for(int i=0; i<this->_nActivities; i++){	
		int ac=this->_activities[i];
		int rm=c.rooms[ac];
		if(rm==UNALLOCATED_SPACE) //counted as unallocated
			continue;
		
		bool ok=true;
		if(rm!=this->_room)
			ok=false;

		if(!ok){
			if(rm!=UNALLOCATED_SPACE)
				ok2=false;
		
			if(conflictsString!=NULL){
				QString s=QObject::tr("Space constraint subject preferred room broken for activity with id %1 (subject=%2)")
					.arg(r.internalActivitiesList[ac].id)
					.arg(this->subjectName);
				s += ". ";
				s += QObject::tr("This increases the conflicts total by %1").arg(weightPercentage/100* 1);
				
				dl.append(s);
				cl.append(weightPercentage/100* 1);
				
				*conflictsString+=s+"\n";
			}

			nbroken++;
		}
	}
	
	if(this->weightPercentage==100)
		assert(ok2);

	return weightPercentage/100 * nbroken;
}

bool ConstraintSubjectPreferredRoom::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);
	//if(a)
	//	;

	return false;
}

bool ConstraintSubjectPreferredRoom::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintSubjectPreferredRoom::isRelatedToSubject(Subject* s)
{
	if(this->subjectName==s->name)
		return true;
	return false;
}

bool ConstraintSubjectPreferredRoom::isRelatedToSubjectTag(SubjectTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintSubjectPreferredRoom::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/

	return false;
}

bool ConstraintSubjectPreferredRoom::isRelatedToRoom(Room* r)
{
	return r->name==this->roomName;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintSubjectPreferredRooms::ConstraintSubjectPreferredRooms()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_SUBJECT_PREFERRED_ROOMS;
}

ConstraintSubjectPreferredRooms::ConstraintSubjectPreferredRooms(double wp, const QString& subj, const QStringList& rms)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_SUBJECT_PREFERRED_ROOMS;
	this->subjectName=subj;
	this->roomsNames=rms;
	assert(rms.size()<=MAX_CONSTRAINT_SUBJECT_PREFERRED_ROOMS);
}

bool ConstraintSubjectPreferredRooms::computeInternalStructure(Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the subject of the constraint.
	
	this->_nActivities=0;
	for(int ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].subjectName == this->subjectName){
			assert(this->_nActivities<MAX_ACTIVITIES_FOR_A_SUBJECT);
			this->_activities[this->_nActivities++]=ac;
		}

	this->_n_preferred_rooms=this->roomsNames.count();
	int i=0;
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		this->_rooms[i] = r.searchRoom(*it);
		assert(this->_rooms[i]>=0);
		i++;
	}
	
	return true;
}

QString ConstraintSubjectPreferredRooms::getXmlDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="<ConstraintSubjectPreferredRooms>\n";
	s+="	<Weight_Percentage>"+QString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Subject>"+protect(this->subjectName)+"</Subject>\n";
	s+="	<Number_of_Preferred_Rooms>"+QString::number(this->roomsNames.count())+"</Number_of_Preferred_Rooms>\n";
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++)
		s+="	<Preferred_Room>"+protect(*it)+"</Preferred_Room>\n";
		
	s+="</ConstraintSubjectPreferredRooms>\n";

	return s;
}

QString ConstraintSubjectPreferredRooms::getDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="Subject preferred rooms"; s+=", ";
	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);s+=", ";
	s+=QObject::tr("S:%1").arg(this->subjectName);
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		s+=", ";
		s+=QObject::tr("R:%1").arg(*it);
	}

	return s;
}

QString ConstraintSubjectPreferredRooms::getDetailedDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Space constraint"); s+="\n";
	s+=QObject::tr("Subject preferred rooms"); s+="\n";
	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";
	s+=QObject::tr("Subject=%1").arg(this->subjectName);s+="\n";
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		s+=QObject::tr("Room=%1").arg(*it);
		s+="\n";
	}

	return s;
}

double ConstraintSubjectPreferredRooms::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || rooms_conflicts<0 || c.changedForMatrixCalculation){
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		//crt_chrom = &c;
		//crt_rules = &r;
		
		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and subject tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	for(int i=0; i<this->_nActivities; i++){	
		int ac=this->_activities[i];
		int rm=c.rooms[ac];
		if(rm==UNALLOCATED_SPACE)
			continue;
	
		bool ok=true;
		int i;
		for(i=0; i<this->_n_preferred_rooms; i++)
			if(this->_rooms[i]==rm)
				break;
		if(i==this->_n_preferred_rooms)
			ok=false;

		if(!ok){
			if(rm!=UNALLOCATED_SPACE)
				ok2=false;
			
			if(conflictsString!=NULL){
				QString s=QObject::tr("Space constraint subject preferred rooms broken for activity with id %1 (subject=%2)")
					.arg(r.internalActivitiesList[ac].id)
					.arg(this->subjectName);
				s += ". ";
				s += QObject::tr("This increases the conflicts total by %1").arg(weightPercentage/100* 1);
				
				dl.append(s);
				cl.append(weightPercentage/100* 1);
			
				*conflictsString+=s+"\n";
			}
			nbroken++;
		}
	}
	
	if(this->weightPercentage==100)
		assert(ok2);

	return weightPercentage/100 * nbroken;
}

bool ConstraintSubjectPreferredRooms::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);
	//if(a)
	//	;

	return false;
}

bool ConstraintSubjectPreferredRooms::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintSubjectPreferredRooms::isRelatedToSubject(Subject* s)
{
	if(this->subjectName==s->name)
		return true;
	return false;
}

bool ConstraintSubjectPreferredRooms::isRelatedToSubjectTag(SubjectTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintSubjectPreferredRooms::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/

	return false;
}

bool ConstraintSubjectPreferredRooms::isRelatedToRoom(Room* r)
{
	return this->roomsNames.contains(r->name);
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

ConstraintSubjectSubjectTagPreferredRoom::ConstraintSubjectSubjectTagPreferredRoom()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOM;
}

ConstraintSubjectSubjectTagPreferredRoom::ConstraintSubjectSubjectTagPreferredRoom(double wp, const QString& subj, const QString& subjTag, const QString& rm)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOM;
	this->subjectName=subj;
	this->subjectTagName=subjTag;
	this->roomName=rm;
}

bool ConstraintSubjectSubjectTagPreferredRoom::computeInternalStructure(Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the subject of the constraint.
	
	this->_nActivities=0;
	for(int ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].subjectName == this->subjectName
		 && r.internalActivitiesList[ac].subjectTagName == this->subjectTagName){
			assert(this->_nActivities<MAX_ACTIVITIES_FOR_A_SUBJECT_SUBJECT_TAG);
			this->_activities[this->_nActivities++]=ac;
		}
		
	this->_room = r.searchRoom(this->roomName);
	assert(this->_room>=0);
	
	return true;
}

QString ConstraintSubjectSubjectTagPreferredRoom::getXmlDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="<ConstraintSubjectSubjectTagPreferredRoom>\n";
	s+="	<Weight_Percentage>"+QString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Subject>"+protect(this->subjectName)+"</Subject>\n";
	s+="	<Subject_Tag>"+protect(this->subjectTagName)+"</Subject_Tag>\n";
	s+="	<Room>"+protect(this->roomName)+"</Room>\n";
		
	s+="</ConstraintSubjectSubjectTagPreferredRoom>\n";

	return s;
}

QString ConstraintSubjectSubjectTagPreferredRoom::getDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="Subject subject tag preferred room"; s+=", ";
	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);s+=", ";
	s+=QObject::tr("S:%1").arg(this->subjectName);s+=",";
	s+=QObject::tr("ST:%1").arg(this->subjectTagName);s+=",";
	s+=QObject::tr("R:%1").arg(this->roomName);

	return s;
}

QString ConstraintSubjectSubjectTagPreferredRoom::getDetailedDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Space constraint"); s+="\n";
	s+=QObject::tr("Subject subject tag preferred room"); s+="\n";
	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";
	s+=QObject::tr("Subject=%1").arg(this->subjectName);s+="\n";
	s+=QObject::tr("Subject tag=%1").arg(this->subjectTagName);s+="\n";
	s+=QObject::tr("Room name=%1").arg(this->roomName);s+="\n";

	return s;
}

double ConstraintSubjectSubjectTagPreferredRoom::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || rooms_conflicts<0 || c.changedForMatrixCalculation){
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		//crt_chrom = &c;
		//crt_rules = &r;
		
		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and subject tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	for(int i=0; i<this->_nActivities; i++){	
		int ac=this->_activities[i];
		int rm=c.rooms[ac];
		if(rm==UNALLOCATED_SPACE) //counted as unallocated
			continue;
		
		bool ok=true;
		if(rm!=this->_room)
			ok=false;

		if(!ok){
			if(rm!=UNALLOCATED_SPACE)
				ok2=false;
		
			if(conflictsString!=NULL){
				QString s=QObject::tr("Space constraint subject subject tag preferred room broken for activity with id %1 (subject=%2, subject tag=%3)")
					.arg(r.internalActivitiesList[ac].id)
					.arg(this->subjectName)
					.arg(this->subjectTagName);
				s += ". ";
				s += QObject::tr("This increases the conflicts total by %1").arg(weightPercentage/100* 1);
				
				dl.append(s);
				cl.append(weightPercentage/100* 1);
				
				*conflictsString+=s+"\n";
			}

			nbroken++;
		}
	}
	
	if(this->weightPercentage==100)
		assert(ok2);

	return weightPercentage/100 * nbroken;
}

bool ConstraintSubjectSubjectTagPreferredRoom::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);
	//if(a)
	//	;

	return false;
}

bool ConstraintSubjectSubjectTagPreferredRoom::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintSubjectSubjectTagPreferredRoom::isRelatedToSubject(Subject* s)
{
	if(this->subjectName==s->name)
		return true;
	return false;
}

bool ConstraintSubjectSubjectTagPreferredRoom::isRelatedToSubjectTag(SubjectTag* s)
{
	if(this->subjectTagName==s->name)
		return true;
	return false;
}

bool ConstraintSubjectSubjectTagPreferredRoom::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/

	return false;
}

bool ConstraintSubjectSubjectTagPreferredRoom::isRelatedToRoom(Room* r)
{
	return r->name==this->roomName;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintSubjectSubjectTagPreferredRooms::ConstraintSubjectSubjectTagPreferredRooms()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOMS;
}

ConstraintSubjectSubjectTagPreferredRooms::ConstraintSubjectSubjectTagPreferredRooms(double wp, const QString& subj, const QString& subjTag, const QStringList& rms)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOMS;
	this->subjectName=subj;
	this->subjectTagName=subjTag;
	this->roomsNames=rms;
	assert(rms.size()<=MAX_CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOMS);
}

bool ConstraintSubjectSubjectTagPreferredRooms::computeInternalStructure(Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the subject of the constraint.
	
	this->_nActivities=0;
	for(int ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].subjectName == this->subjectName
		 && r.internalActivitiesList[ac].subjectTagName == this->subjectTagName){
			assert(this->_nActivities<MAX_ACTIVITIES_FOR_A_SUBJECT_SUBJECT_TAG);
			this->_activities[this->_nActivities++]=ac;
		}

	this->_n_preferred_rooms=this->roomsNames.count();
	int i=0;
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		this->_rooms[i] = r.searchRoom(*it);
		assert(this->_rooms[i]>=0);
		i++;
	}
	
	return true;
}

QString ConstraintSubjectSubjectTagPreferredRooms::getXmlDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="<ConstraintSubjectSubjectTagPreferredRooms>\n";
	s+="	<Weight_Percentage>"+QString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Subject>"+protect(this->subjectName)+"</Subject>\n";
	s+="	<Subject_Tag>"+protect(this->subjectTagName)+"</Subject_Tag>\n";
	s+="	<Number_of_Preferred_Rooms>"+QString::number(this->roomsNames.count())+"</Number_of_Preferred_Rooms>\n";
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++)
		s+="	<Preferred_Room>"+protect(*it)+"</Preferred_Room>\n";
		
	s+="</ConstraintSubjectSubjectTagPreferredRooms>\n";

	return s;
}

QString ConstraintSubjectSubjectTagPreferredRooms::getDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="Subject subject tag preferred rooms"; s+=", ";
	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);s+=", ";
	s+=QObject::tr("S:%1").arg(this->subjectName);
	s+=QObject::tr("ST:%1").arg(this->subjectTagName);
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		s+=", ";
		s+=QObject::tr("R:%1").arg(*it);
	}

	return s;
}

QString ConstraintSubjectSubjectTagPreferredRooms::getDetailedDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Space constraint"); s+="\n";
	s+=QObject::tr("Subject subject tag preferred rooms"); s+="\n";
	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";
	s+=QObject::tr("Subject=%1").arg(this->subjectName);s+="\n";
	s+=QObject::tr("Subject tag=%1").arg(this->subjectTagName);s+="\n";
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		s+=QObject::tr("Room=%1").arg(*it);
		s+="\n";
	}

	return s;
}

double ConstraintSubjectSubjectTagPreferredRooms::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || rooms_conflicts<0 || c.changedForMatrixCalculation){
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		//crt_chrom = &c;
		//crt_rules = &r;
		
		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and subject tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	for(int i=0; i<this->_nActivities; i++){	
		int ac=this->_activities[i];
		int rm=c.rooms[ac];
		if(rm==UNALLOCATED_SPACE)
			continue;
	
		bool ok=true;
		int i;
		for(i=0; i<this->_n_preferred_rooms; i++)
			if(this->_rooms[i]==rm)
				break;
		if(i==this->_n_preferred_rooms)
			ok=false;

		if(!ok){
			if(rm!=UNALLOCATED_SPACE)
				ok2=false;
			
			if(conflictsString!=NULL){
				QString s=QObject::tr("Space constraint subject subject tag preferred rooms broken for activity with id %1 (subject=%2, subject tag=%3)")
					.arg(r.internalActivitiesList[ac].id)
					.arg(this->subjectName)
					.arg(this->subjectTagName);
				s += ". ";
				s += QObject::tr("This increases the conflicts total by %1").arg(weightPercentage/100* 1);
				
				dl.append(s);
				cl.append(weightPercentage/100* 1);
			
				*conflictsString+=s+"\n";
			}
			nbroken++;
		}
	}
	
	if(this->weightPercentage==100)
		assert(ok2);

	return weightPercentage/100 * nbroken;
}

bool ConstraintSubjectSubjectTagPreferredRooms::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);
	//if(a)
	//	;

	return false;
}

bool ConstraintSubjectSubjectTagPreferredRooms::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintSubjectSubjectTagPreferredRooms::isRelatedToSubject(Subject* s)
{
	if(this->subjectName==s->name)
		return true;
	return false;
}

bool ConstraintSubjectSubjectTagPreferredRooms::isRelatedToSubjectTag(SubjectTag* s)
{
	if(this->subjectTagName==s->name)
		return true;
	return false;
}

bool ConstraintSubjectSubjectTagPreferredRooms::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/

	return false;
}

bool ConstraintSubjectSubjectTagPreferredRooms::isRelatedToRoom(Room* r)
{
	return this->roomsNames.contains(r->name);
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
