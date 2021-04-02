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
#include "activitytag.h"
#include "studentsset.h"
#include "room.h"
#include "solution.h"

#include <qstring.h>

#include <QMessageBox>

//static Solution* crt_chrom=NULL;
//static Rules* crt_rules=NULL;

#define yesNo(x)				((x)==0?"no":"yes")
#define yesNoTranslated(x)		((x)==0?QObject::tr("no"):QObject::tr("yes"))

#include "generate_pre.h"

static qint8 roomsMatrix[MAX_ROOMS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

static int rooms_conflicts=-1;

//extern QList<int> activitiesPreferredRoomsPreferredRooms[MAX_ACTIVITIES];

static qint8 subgroupsBuildingsTimetable[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
static qint8 teachersBuildingsTimetable[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

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

		c.getSubgroupsBuildingsTimetable(r, subgroupsBuildingsTimetable);
		c.getTeachersBuildingsTimetable(r, teachersBuildingsTimetable);
		 
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

bool ConstraintBasicCompulsorySpace::isRelatedToActivityTag(ActivityTag* s)
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

ConstraintRoomNotAvailableTimes::ConstraintRoomNotAvailableTimes()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES;
}

ConstraintRoomNotAvailableTimes::ConstraintRoomNotAvailableTimes(double wp, const QString& rn, QList<int> d, QList<int> h)
	: SpaceConstraint(wp)
{
	this->room=rn;
	this->days=d;
	this->hours=h;
	this->type=CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES;
}

QString ConstraintRoomNotAvailableTimes::getXmlDescription(Rules& r){
	QString s="<ConstraintRoomNotAvailableTimes>\n";
	s+="	<Weight_Percentage>"+QString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Room>"+protect(this->room)+"</Room>\n";

	s+="	<Number_of_Not_Available_Times>"+QString::number(this->days.count())+"</Number_of_Not_Available_Times>\n";
	assert(days.count()==hours.count());
	for(int i=0; i<days.count(); i++){
		s+="	<Not_Available_Time>\n";
		if(this->days.at(i)>=0)
			s+="		<Day>"+protect(r.daysOfTheWeek[this->days.at(i)])+"</Day>\n";
		if(this->hours.at(i)>=0)
			s+="		<Hour>"+protect(r.hoursOfTheDay[this->hours.at(i)])+"</Hour>\n";
		s+="	</Not_Available_Time>\n";
	}

	s+="</ConstraintRoomNotAvailableTimes>\n";
	return s;
}

QString ConstraintRoomNotAvailableTimes::getDescription(Rules& r){
	QString s=QObject::tr("Room not available times");s+=", ";
	s+=(QObject::tr("WP:%1\%", "Weight percentage").arg(this->weightPercentage));s+=", ";
	s+=(QObject::tr("R:%1", "Room").arg(this->room));s+=", ";

	s+=QObject::tr("NA at:", "Not available at");
	s+=" ";
	assert(days.count()==hours.count());
	for(int i=0; i<days.count(); i++){
		if(this->days.at(i)>=0){
			s+=r.daysOfTheWeek[this->days.at(i)];
			s+=" ";
		}
		if(this->hours.at(i)>=0){
			s+=r.hoursOfTheDay[this->hours.at(i)];
		}
		if(i<days.count()-1)
			s+="; ";
	}

	return s;
}

QString ConstraintRoomNotAvailableTimes::getDetailedDescription(Rules& r){
	QString s=QObject::tr("Space constraint");s+="\n";
	s+=QObject::tr("Room not available times");s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	s+=(QObject::tr("Room=%1").arg(this->room));s+="\n";

	s+=QObject::tr("Not available at:");
	s+="\n";
	assert(days.count()==hours.count());
	for(int i=0; i<days.count(); i++){
		if(this->days.at(i)>=0){
			s+=r.daysOfTheWeek[this->days.at(i)];
			s+=" ";
		}
		if(this->hours.at(i)>=0){
			s+=r.hoursOfTheDay[this->hours.at(i)];
		}
		if(i<days.count()-1)
			s+="; ";
	}

	return s;
}

bool ConstraintRoomNotAvailableTimes::computeInternalStructure(Rules& r){
	this->room_ID=r.searchRoom(this->room);
	
	if(this->room_ID<0){
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Constraint room not available times is wrong because it refers to inexistent room."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}	
	/*if(this->d >= r.nDaysPerWeek){
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
	}*/

	assert(days.count()==hours.count());
	for(int k=0; k<days.count(); k++){
		if(this->days.at(k) >= r.nDaysPerWeek){
			QMessageBox::information(NULL, QObject::tr("FET information"),
			 QObject::tr("Constraint room not available times is wrong because it refers to removed day. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}		
		if(this->hours.at(k) >= r.nHoursPerDay){
			QMessageBox::information(NULL, QObject::tr("FET information"),
			 QObject::tr("Constraint room not available times is wrong because an hour is too late (after the last acceptable slot). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
	}
	
	assert(this->room_ID>=0);
	
	return true;
}

double ConstraintRoomNotAvailableTimes::fitness(
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

		c.getSubgroupsBuildingsTimetable(r, subgroupsBuildingsTimetable);
		c.getTeachersBuildingsTimetable(r, teachersBuildingsTimetable);
		 
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
	int rm=this->room_ID;

	int nbroken;

	nbroken=0;

	assert(days.count()==hours.count());
	for(int k=0; k<days.count(); k++){
		int d=days.at(k);
		int h=hours.at(k);
		
		if(roomsMatrix[rm][d][h]>0){
			nbroken+=roomsMatrix[rm][d][h];
	
			if(conflictsString!=NULL){
				QString s= QObject::tr("Time constraint room not available times");
				s += " ";
				s += (QObject::tr("broken for room: %1 on day %2, hour %3")
				 .arg(r.internalRoomsList[rm]->name)
				 .arg(r.daysOfTheWeek[d])
				 .arg(r.hoursOfTheDay[h]));
				s += ". ";
				s += (QObject::tr("This increases the conflicts total by %1")
				 .arg(roomsMatrix[rm][d][h]*weightPercentage/100));
				 
				dl.append(s);
				cl.append(roomsMatrix[rm][d][h]*weightPercentage/100);
			
				*conflictsString += s+"\n";
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintRoomNotAvailableTimes::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintRoomNotAvailableTimes::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintRoomNotAvailableTimes::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintRoomNotAvailableTimes::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintRoomNotAvailableTimes::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintRoomNotAvailableTimes::isRelatedToRoom(Room* r)
{
	return this->room==r->name;
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

	if(this->_room<0){
		QMessageBox::warning(NULL, QObject::tr("FET error in data"), 
			QObject::tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}

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
	
		if(r.activitiesList[ai]->activityTagName!="")
			s+=QObject::tr(",AT:", "Activity tag")+r.activitiesList[ai]->activityTagName;
	
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
	
		if(r.activitiesList[ai]->activityTagName!="")
			s+=QObject::tr(",AT:", "Activity tag")+r.activitiesList[ai]->activityTagName;
	
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

		c.getSubgroupsBuildingsTimetable(r, subgroupsBuildingsTimetable);
		c.getTeachersBuildingsTimetable(r, teachersBuildingsTimetable);
		 
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

bool ConstraintActivityPreferredRoom::isRelatedToActivityTag(ActivityTag* s)
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
			QObject::tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
	
	this->_rooms.clear();
	foreach(QString rm, this->roomsNames){
		int t=r.searchRoom(rm);

		if(t<0){
			QMessageBox::warning(NULL, QObject::tr("FET error in data"), 
				QObject::tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

			return false;
		}

		assert(t>=0);
		this->_rooms.append(t);
	}
		
	/*this->_n_preferred_rooms=this->roomsNames.count();
	int i=0;
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		this->_rooms[i] = r.searchRoom(*it);
		assert(this->_rooms[i]>=0);
		i++;
	}*/
	
	return true;
}

QString ConstraintActivityPreferredRooms::getXmlDescription(Rules& r){
	Q_UNUSED(r);

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
	
		if(r.activitiesList[ai]->activityTagName!="")
			s+=QObject::tr(",AT:", "Activity tag")+r.activitiesList[ai]->activityTagName;
	
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
	
		if(r.activitiesList[ai]->activityTagName!="")
			s+=QObject::tr(",AT:", "Activity tag")+r.activitiesList[ai]->activityTagName;
	
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

		c.getSubgroupsBuildingsTimetable(r, subgroupsBuildingsTimetable);
		c.getTeachersBuildingsTimetable(r, teachersBuildingsTimetable);
		 
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
		for(i=0; i<this->_rooms.count(); i++)
			if(this->_rooms.at(i)==rm)
				break;
		if(i==this->_rooms.count()){
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

	return false;
}

bool ConstraintActivityPreferredRooms::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivityPreferredRooms::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivityPreferredRooms::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivityPreferredRooms::isRelatedToRoom(Room* r)
{
	return this->roomsNames.contains(r->name);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetHomeRoom::ConstraintStudentsSetHomeRoom()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_SET_HOME_ROOM;
}

ConstraintStudentsSetHomeRoom::ConstraintStudentsSetHomeRoom(double wp, QString st, QString rm)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_SET_HOME_ROOM;
	this->studentsName=st;
	this->roomName=rm;
}

bool ConstraintStudentsSetHomeRoom::computeInternalStructure(Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the subject of the constraint.
	
	QStringList::iterator it;
	Activity* act;

	this->_activities.clear();

	for(int ac=0; ac<r.nInternalActivities; ac++){
		act=&r.internalActivitiesList[ac];

		//check if this activity has the corresponding students
		bool commonStudents=false;
		/*foreach(QString st, act->studentsNames)
			if(r.studentsSetsRelated(st, studentsName)){
				commonStudents=true;
				break;
			}*/
		if(act->studentsNames.count()==1)
			if(act->studentsNames.at(0)==studentsName)
				commonStudents=true;
	
		if(!commonStudents)
			continue;
		
		this->_activities.append(ac);
	}

	this->_room = r.searchRoom(this->roomName);
	assert(this->_room>=0);
	
	return true;
}

QString ConstraintStudentsSetHomeRoom::getXmlDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="<ConstraintStudentsSetHomeRoom>\n";
	s+="	<Weight_Percentage>"+QString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Students>"+protect(this->studentsName)+"</Students>\n";
	s+="	<Room>"+protect(this->roomName)+"</Room>\n";
		
	s+="</ConstraintStudentsSetHomeRoom>\n";

	return s;
}

QString ConstraintStudentsSetHomeRoom::getDescription(Rules& r)
{
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="Students set home room"; s+=", ";

	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);s+=", ";
	
	s+=QObject::tr("St:%1", "St means students").arg(this->studentsName);s+=", ";

	s+=QObject::tr("R:%1", "R means Room").arg(this->roomName);

	return s;
}

QString ConstraintStudentsSetHomeRoom::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Space constraint"); s+="\n";
	s+=QObject::tr("Students set home room"); s+="\n";
	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";

	s+=QObject::tr("Students=%1").arg(this->studentsName);s+="\n";

	s+=QObject::tr("Room name=%1").arg(this->roomName);s+="\n";

	return s;
}

double ConstraintStudentsSetHomeRoom::fitness(
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

		c.getSubgroupsBuildingsTimetable(r, subgroupsBuildingsTimetable);
		c.getTeachersBuildingsTimetable(r, teachersBuildingsTimetable);
		 
		//crt_chrom = &c;
		//crt_rules = &r;
		
		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and activity tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	foreach(int ac, this->_activities){
	//for(int i=0; i<this->_nActivities; i++){	
	//	int ac=this->_activities[i];
		int rm=c.rooms[ac];
		if(rm==UNALLOCATED_SPACE) //counted as unallocated
			continue;
		
		bool ok=true;
		//if(rm!=this->_room)
		if(rm==UNSPECIFIED_ROOM) //it may be other room, from subject (activity tag) preferred room(s), which is OK
			ok=false;
		else if(rm==this->_room){
		} //OK
		else{ //other room, from subject (activity tag) pref. room(s)
			bool okk=false;
			foreach(PreferredRoomsItem it, activitiesPreferredRoomsList[ac])
				if(it.preferredRooms.contains(rm))
					okk=true;
			assert(okk);
			//assert(activitiesPreferredRoomsPreferredRooms[ac].contains(rm));
		}

		if(!ok){
			if(rm!=UNALLOCATED_SPACE)
				ok2=false;
		
			if(conflictsString!=NULL){
				QString s=QObject::tr("Space constraint students set home room broken for activity with id %1 (students=%2)")
					.arg(r.internalActivitiesList[ac].id)
					.arg(this->studentsName);
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

bool ConstraintStudentsSetHomeRoom::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetHomeRoom::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsSetHomeRoom::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsSetHomeRoom::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetHomeRoom::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);

	return s->name==this->studentsName;
}

bool ConstraintStudentsSetHomeRoom::isRelatedToRoom(Room* r)
{
	return r->name==this->roomName;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetHomeRooms::ConstraintStudentsSetHomeRooms()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_SET_HOME_ROOMS;
}

ConstraintStudentsSetHomeRooms::ConstraintStudentsSetHomeRooms(double wp, QString st, const QStringList& rms)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_SET_HOME_ROOMS;

	this->studentsName=st;

	this->roomsNames=rms;
}

bool ConstraintStudentsSetHomeRooms::computeInternalStructure(Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the constraint.
	
	this->_activities.clear();

	QStringList::iterator it;
	Activity* act;

	for(int ac=0; ac<r.nInternalActivities; ac++){
		act=&r.internalActivitiesList[ac];

		//check if this activity has the corresponding students
		bool commonStudents=false;
		/*foreach(QString st, act->studentsNames)
			if(r.studentsSetsRelated(st, studentsName)){
				commonStudents=true;
				break;
			}*/
		if(act->studentsNames.count()==1)
			if(act->studentsNames.at(0)==studentsName)
				commonStudents=true;
	
		if(!commonStudents)
			continue;
		
		this->_activities.append(ac);
	}

	this->_rooms.clear();

	foreach(QString rm, this->roomsNames){
		int t=r.searchRoom(rm);
		if(t<0){
			QMessageBox::warning(NULL, QObject::tr("FET error in data"), 
				QObject::tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

			return false;
		}
		else{
			assert(t>=0);
			this->_rooms.append(t);
		}
	}
	
	/*this->_n_preferred_rooms=this->roomsNames.count();
	int i=0;
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		this->_rooms[i] = r.searchRoom(*it);
		assert(this->_rooms[i]>=0);
		i++;
	}*/
	
	return true;
}

QString ConstraintStudentsSetHomeRooms::getXmlDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="<ConstraintStudentsSetHomeRooms>\n";
	s+="	<Weight_Percentage>"+QString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Students>"+protect(this->studentsName)+"</Students>\n";
	s+="	<Number_of_Preferred_Rooms>"+QString::number(this->roomsNames.count())+"</Number_of_Preferred_Rooms>\n";
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++)
		s+="	<Preferred_Room>"+protect(*it)+"</Preferred_Room>\n";
		
	s+="</ConstraintStudentsSetHomeRooms>\n";

	return s;
}

QString ConstraintStudentsSetHomeRooms::getDescription(Rules& r){
	Q_UNUSED(r);

	QString s="Students set home rooms"; s+=", ";
	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);s+=", ";

	s+=QObject::tr("St:%1", "St means students").arg(this->studentsName);s+=", ";

	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		s+=", ";
		s+=QObject::tr("R:%1", "R means Room").arg(*it);
	}

	return s;
}

QString ConstraintStudentsSetHomeRooms::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=QObject::tr("Space constraint"); s+="\n";
	s+=QObject::tr("Students set home rooms"); s+="\n";
	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";

	s+=QObject::tr("Students=%1").arg(this->studentsName);s+="\n";

	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		s+=QObject::tr("Room=%1").arg(*it);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsSetHomeRooms::fitness(
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

		c.getSubgroupsBuildingsTimetable(r, subgroupsBuildingsTimetable);
		c.getTeachersBuildingsTimetable(r, teachersBuildingsTimetable);
		 
		//crt_chrom = &c;
		//crt_rules = &r;
		
		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and activity tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	//for(int i=0; i<this->_nActivities; i++){	
	//	int ac=this->_activities[i];
	foreach(int ac, this->_activities){
		int rm=c.rooms[ac];
		if(rm==UNALLOCATED_SPACE)
			continue;
	
		bool ok=true;
		int i;
		//for(i=0; i<this->_n_preferred_rooms; i++)
		for(i=0; i<this->_rooms.count(); i++)
			if(this->_rooms[i]==rm)
				break;
		//if(i==this->_n_preferred_rooms)
		if(i==this->_rooms.count()){
			if(rm==UNSPECIFIED_ROOM)
				ok=false;
			else{
				bool okk=false;
				foreach(PreferredRoomsItem it, activitiesPreferredRoomsList[ac])
					if(it.preferredRooms.contains(rm))
						okk=true;
				assert(okk);
				//assert(activitiesPreferredRoomsPreferredRooms[ac].contains(rm));
			}
		}

		if(!ok){
			if(rm!=UNALLOCATED_SPACE)
				ok2=false;
			
			if(conflictsString!=NULL){
				QString s=QObject::tr("Space constraint students set home rooms broken for activity with id %1 (students=%2)")
					.arg(r.internalActivitiesList[ac].id)
					.arg(this->studentsName);
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

bool ConstraintStudentsSetHomeRooms::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetHomeRooms::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintStudentsSetHomeRooms::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s)
	
	return false;
}

bool ConstraintStudentsSetHomeRooms::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetHomeRooms::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);

	return s->name==this->studentsName;
}

bool ConstraintStudentsSetHomeRooms::isRelatedToRoom(Room* r)
{
	return this->roomsNames.contains(r->name);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherHomeRoom::ConstraintTeacherHomeRoom()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHER_HOME_ROOM;
}

ConstraintTeacherHomeRoom::ConstraintTeacherHomeRoom(double wp, QString tc, QString rm)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHER_HOME_ROOM;
	this->teacherName=tc;
	this->roomName=rm;
}

bool ConstraintTeacherHomeRoom::computeInternalStructure(Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the subject of the constraint.
	
	QStringList::iterator it;
	Activity* act;

	this->_activities.clear();

	for(int ac=0; ac<r.nInternalActivities; ac++){
		act=&r.internalActivitiesList[ac];

		//check if this activity has the corresponding students
		bool sameTeacher=false;
		/*foreach(QString st, act->studentsNames)
			if(r.studentsSetsRelated(st, studentsName)){
				commonStudents=true;
				break;
			}*/
		if(act->teachersNames.count()==1)
			if(act->teachersNames.at(0)==teacherName)
				sameTeacher=true;
	
		if(!sameTeacher)
			continue;
		
		this->_activities.append(ac);
	}

	this->_room = r.searchRoom(this->roomName);
	assert(this->_room>=0);
	
	return true;
}

QString ConstraintTeacherHomeRoom::getXmlDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="<ConstraintTeacherHomeRoom>\n";
	s+="	<Weight_Percentage>"+QString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Teacher>"+protect(this->teacherName)+"</Teacher>\n";
	s+="	<Room>"+protect(this->roomName)+"</Room>\n";
		
	s+="</ConstraintTeacherHomeRoom>\n";

	return s;
}

QString ConstraintTeacherHomeRoom::getDescription(Rules& r)
{
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="Teacher home room"; s+=", ";

	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);s+=", ";
	
	s+=QObject::tr("T:%1", "T means teacher").arg(this->teacherName);s+=", ";

	s+=QObject::tr("R:%1", "R means Room").arg(this->roomName);

	return s;
}

QString ConstraintTeacherHomeRoom::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Space constraint"); s+="\n";
	s+=QObject::tr("Teacher home room"); s+="\n";
	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";

	s+=QObject::tr("Teacher=%1").arg(this->teacherName);s+="\n";

	s+=QObject::tr("Room name=%1").arg(this->roomName);s+="\n";

	return s;
}

double ConstraintTeacherHomeRoom::fitness(
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

		c.getSubgroupsBuildingsTimetable(r, subgroupsBuildingsTimetable);
		c.getTeachersBuildingsTimetable(r, teachersBuildingsTimetable);
		 
		//crt_chrom = &c;
		//crt_rules = &r;
		
		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and activity tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	foreach(int ac, this->_activities){
	//for(int i=0; i<this->_nActivities; i++){	
	//	int ac=this->_activities[i];
		int rm=c.rooms[ac];
		if(rm==UNALLOCATED_SPACE) //counted as unallocated
			continue;
		
		bool ok=true;
		//if(rm!=this->_room)
		if(rm==UNSPECIFIED_ROOM) //it may be other room, from subject (activity tag) preferred room(s), which is OK
			ok=false;
		else if(rm==this->_room){
		} //OK
		else{ //other room, from subject (activity tag) pref. room(s)
			bool okk=false;
			foreach(PreferredRoomsItem it, activitiesPreferredRoomsList[ac])
				if(it.preferredRooms.contains(rm))
					okk=true;
			assert(okk);
			//assert(activitiesPreferredRoomsPreferredRooms[ac].contains(rm));
		}

		if(!ok){
			if(rm!=UNALLOCATED_SPACE)
				ok2=false;
		
			if(conflictsString!=NULL){
				QString s=QObject::tr("Space constraint teacher home room broken for activity with id %1 (teacher=%2)")
					.arg(r.internalActivitiesList[ac].id)
					.arg(this->teacherName);
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

bool ConstraintTeacherHomeRoom::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherHomeRoom::isRelatedToTeacher(Teacher* t)
{
	return teacherName==t->name;
}

bool ConstraintTeacherHomeRoom::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherHomeRoom::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherHomeRoom::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherHomeRoom::isRelatedToRoom(Room* r)
{
	return r->name==this->roomName;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherHomeRooms::ConstraintTeacherHomeRooms()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHER_HOME_ROOMS;
}

ConstraintTeacherHomeRooms::ConstraintTeacherHomeRooms(double wp, QString tc, const QStringList& rms)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHER_HOME_ROOMS;

	this->teacherName=tc;

	this->roomsNames=rms;
}

bool ConstraintTeacherHomeRooms::computeInternalStructure(Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the constraint.
	
	this->_activities.clear();

	QStringList::iterator it;
	Activity* act;

	for(int ac=0; ac<r.nInternalActivities; ac++){
		act=&r.internalActivitiesList[ac];

		//check if this activity has the corresponding students
		bool sameTeacher=false;
		if(act->teachersNames.count()==1)
			if(act->teachersNames.at(0)==teacherName)
				sameTeacher=true;
	
		if(!sameTeacher)
			continue;
		
		this->_activities.append(ac);
	}

	this->_rooms.clear();

	foreach(QString rm, this->roomsNames){
		int t=r.searchRoom(rm);
		if(t<0){
			QMessageBox::warning(NULL, QObject::tr("FET error in data"), 
				QObject::tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

			return false;
		}
		else{
			assert(t>=0);
			this->_rooms.append(t);
		}
	}
	
	/*this->_n_preferred_rooms=this->roomsNames.count();
	int i=0;
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		this->_rooms[i] = r.searchRoom(*it);
		assert(this->_rooms[i]>=0);
		i++;
	}*/
	
	return true;
}

QString ConstraintTeacherHomeRooms::getXmlDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="<ConstraintTeacherHomeRooms>\n";
	s+="	<Weight_Percentage>"+QString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Teacher>"+protect(this->teacherName)+"</Teacher>\n";
	s+="	<Number_of_Preferred_Rooms>"+QString::number(this->roomsNames.count())+"</Number_of_Preferred_Rooms>\n";
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++)
		s+="	<Preferred_Room>"+protect(*it)+"</Preferred_Room>\n";
		
	s+="</ConstraintTeacherHomeRooms>\n";

	return s;
}

QString ConstraintTeacherHomeRooms::getDescription(Rules& r){
	Q_UNUSED(r);

	QString s="Teacher home rooms"; s+=", ";
	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);s+=", ";

	s+=QObject::tr("T:%1", "T means teacher").arg(this->teacherName);s+=", ";

	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		s+=", ";
		s+=QObject::tr("R:%1", "R means Room").arg(*it);
	}

	return s;
}

QString ConstraintTeacherHomeRooms::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=QObject::tr("Space constraint"); s+="\n";
	s+=QObject::tr("Teacher home rooms"); s+="\n";
	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";

	s+=QObject::tr("Teacher=%1").arg(this->teacherName);s+="\n";

	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		s+=QObject::tr("Room=%1").arg(*it);
		s+="\n";
	}

	return s;
}

double ConstraintTeacherHomeRooms::fitness(
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

		c.getSubgroupsBuildingsTimetable(r, subgroupsBuildingsTimetable);
		c.getTeachersBuildingsTimetable(r, teachersBuildingsTimetable);
		 
		//crt_chrom = &c;
		//crt_rules = &r;
		
		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and activity tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	//for(int i=0; i<this->_nActivities; i++){	
	//	int ac=this->_activities[i];
	foreach(int ac, this->_activities){
		int rm=c.rooms[ac];
		if(rm==UNALLOCATED_SPACE)
			continue;
	
		bool ok=true;
		int i;
		//for(i=0; i<this->_n_preferred_rooms; i++)
		for(i=0; i<this->_rooms.count(); i++)
			if(this->_rooms[i]==rm)
				break;
		//if(i==this->_n_preferred_rooms)
		if(i==this->_rooms.count()){
			if(rm==UNSPECIFIED_ROOM)
				ok=false;
			else{
				bool okk=false;
				foreach(PreferredRoomsItem it, activitiesPreferredRoomsList[ac])
					if(it.preferredRooms.contains(rm))
						okk=true;
				assert(okk);
				//	assert(activitiesPreferredRoomsPreferredRooms[ac].contains(rm));
			}
		}

		if(!ok){
			if(rm!=UNALLOCATED_SPACE)
				ok2=false;
			
			if(conflictsString!=NULL){
				QString s=QObject::tr("Space constraint teacher home rooms broken for activity with id %1 (teacher=%2)")
					.arg(r.internalActivitiesList[ac].id)
					.arg(this->teacherName);
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

bool ConstraintTeacherHomeRooms::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherHomeRooms::isRelatedToTeacher(Teacher* t)
{
	return teacherName==t->name;
}

bool ConstraintTeacherHomeRooms::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s)
	
	return false;
}

bool ConstraintTeacherHomeRooms::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherHomeRooms::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	return false;
}

bool ConstraintTeacherHomeRooms::isRelatedToRoom(Room* r)
{
	return this->roomsNames.contains(r->name);
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

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
	
	this->_activities.clear();
	for(int ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].subjectName == this->subjectName){
			this->_activities.append(ac);
		}
	
	/*this->_nActivities=0;
	for(int ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].subjectName == this->subjectName){
			assert(this->_nActivities<MAX_ACTIVITIES_FOR_A_SUBJECT);
			this->_activities[this->_nActivities++]=ac;
		}*/
		
	this->_room = r.searchRoom(this->roomName);
	if(this->_room<0){
		QMessageBox::warning(NULL, QObject::tr("FET error in data"), 
			QObject::tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
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

		c.getSubgroupsBuildingsTimetable(r, subgroupsBuildingsTimetable);
		c.getTeachersBuildingsTimetable(r, teachersBuildingsTimetable);
		 
		//crt_chrom = &c;
		//crt_rules = &r;
		
		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and activity tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	foreach(int ac, this->_activities){
	//for(int i=0; i<this->_nActivities; i++){	
	//	int ac=this->_activities[i];
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

bool ConstraintSubjectPreferredRoom::isRelatedToActivityTag(ActivityTag* s)
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
}

bool ConstraintSubjectPreferredRooms::computeInternalStructure(Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the subject of the constraint.
	
	this->_activities.clear();
	for(int ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].subjectName == this->subjectName){
			this->_activities.append(ac);
		}
	
	this->_rooms.clear();
	foreach(QString rm, this->roomsNames){
		int t=r.searchRoom(rm);
		if(t<0){
			QMessageBox::warning(NULL, QObject::tr("FET error in data"), 
				QObject::tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

			return false;
		}
		assert(t>=0);
		this->_rooms.append(t);
	}

	/*this->_nActivities=0;
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
	}*/
	
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

		c.getSubgroupsBuildingsTimetable(r, subgroupsBuildingsTimetable);
		c.getTeachersBuildingsTimetable(r, teachersBuildingsTimetable);
		 
		//crt_chrom = &c;
		//crt_rules = &r;
		
		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and activity tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	foreach(int ac, this->_activities){
	//for(int i=0; i<this->_nActivities; i++){	
	//	int ac=this->_activities[i];
		int rm=c.rooms[ac];
		if(rm==UNALLOCATED_SPACE)
			continue;
	
		bool ok=true;
		int i;
		for(i=0; i<this->_rooms.count(); i++)
			if(this->_rooms.at(i)==rm)
				break;
		if(i==this->_rooms.count())
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

bool ConstraintSubjectPreferredRooms::isRelatedToActivityTag(ActivityTag* s)
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

ConstraintSubjectActivityTagPreferredRoom::ConstraintSubjectActivityTagPreferredRoom()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM;
}

ConstraintSubjectActivityTagPreferredRoom::ConstraintSubjectActivityTagPreferredRoom(double wp, const QString& subj, const QString& subjTag, const QString& rm)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM;
	this->subjectName=subj;
	this->activityTagName=subjTag;
	this->roomName=rm;
}

bool ConstraintSubjectActivityTagPreferredRoom::computeInternalStructure(Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the subject of the constraint.
	
	this->_activities.clear();
	for(int ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].subjectName == this->subjectName
		 && r.internalActivitiesList[ac].activityTagName == this->activityTagName){
		 	this->_activities.append(ac);
		}
		
	/*this->_nActivities=0;
	for(int ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].subjectName == this->subjectName
		 && r.internalActivitiesList[ac].activityTagName == this->activityTagName){
			assert(this->_nActivities<MAX_ACTIVITIES_FOR_A_SUBJECT_SUBJECT_TAG);
			this->_activities[this->_nActivities++]=ac;
		}*/
		
	this->_room = r.searchRoom(this->roomName);
	if(this->_room<0){
		QMessageBox::warning(NULL, QObject::tr("FET error in data"), 
			QObject::tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
	assert(this->_room>=0);
	
	return true;
}

QString ConstraintSubjectActivityTagPreferredRoom::getXmlDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="<ConstraintSubjectActivityTagPreferredRoom>\n";
	s+="	<Weight_Percentage>"+QString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Subject>"+protect(this->subjectName)+"</Subject>\n";
	s+="	<Activity_Tag>"+protect(this->activityTagName)+"</Activity_Tag>\n";
	s+="	<Room>"+protect(this->roomName)+"</Room>\n";
		
	s+="</ConstraintSubjectActivityTagPreferredRoom>\n";

	return s;
}

QString ConstraintSubjectActivityTagPreferredRoom::getDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="Subject activity tag preferred room"; s+=", ";
	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);s+=", ";
	s+=QObject::tr("S:%1").arg(this->subjectName);s+=",";
	s+=QObject::tr("AT:%1", "Activity tag").arg(this->activityTagName);s+=",";
	s+=QObject::tr("R:%1").arg(this->roomName);

	return s;
}

QString ConstraintSubjectActivityTagPreferredRoom::getDetailedDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Space constraint"); s+="\n";
	s+=QObject::tr("Subject activity tag preferred room"); s+="\n";
	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";
	s+=QObject::tr("Subject=%1").arg(this->subjectName);s+="\n";
	s+=QObject::tr("Activity tag=%1").arg(this->activityTagName);s+="\n";
	s+=QObject::tr("Room name=%1").arg(this->roomName);s+="\n";

	return s;
}

double ConstraintSubjectActivityTagPreferredRoom::fitness(
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

		c.getSubgroupsBuildingsTimetable(r, subgroupsBuildingsTimetable);
		c.getTeachersBuildingsTimetable(r, teachersBuildingsTimetable);
		 
		//crt_chrom = &c;
		//crt_rules = &r;
		
		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and activity tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	foreach(int ac, this->_activities){
	//for(int i=0; i<this->_nActivities; i++){	
	//	int ac=this->_activities[i];
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
				QString s=QObject::tr("Space constraint subject activity tag preferred room broken for activity with id %1 (subject=%2, activity tag=%3)")
					.arg(r.internalActivitiesList[ac].id)
					.arg(this->subjectName)
					.arg(this->activityTagName);
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

bool ConstraintSubjectActivityTagPreferredRoom::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);
	//if(a)
	//	;

	return false;
}

bool ConstraintSubjectActivityTagPreferredRoom::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintSubjectActivityTagPreferredRoom::isRelatedToSubject(Subject* s)
{
	if(this->subjectName==s->name)
		return true;
	return false;
}

bool ConstraintSubjectActivityTagPreferredRoom::isRelatedToActivityTag(ActivityTag* s)
{
	if(this->activityTagName==s->name)
		return true;
	return false;
}

bool ConstraintSubjectActivityTagPreferredRoom::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/

	return false;
}

bool ConstraintSubjectActivityTagPreferredRoom::isRelatedToRoom(Room* r)
{
	return r->name==this->roomName;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintSubjectActivityTagPreferredRooms::ConstraintSubjectActivityTagPreferredRooms()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS;
}

ConstraintSubjectActivityTagPreferredRooms::ConstraintSubjectActivityTagPreferredRooms(double wp, const QString& subj, const QString& subjTag, const QStringList& rms)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS;
	this->subjectName=subj;
	this->activityTagName=subjTag;
	this->roomsNames=rms;
}

bool ConstraintSubjectActivityTagPreferredRooms::computeInternalStructure(Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the subject of the constraint.
	
	this->_activities.clear();
	for(int ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].subjectName == this->subjectName
		 && r.internalActivitiesList[ac].activityTagName == this->activityTagName){
			this->_activities.append(ac);
		}

	this->_rooms.clear();
	foreach(QString rm, roomsNames){
		int t=r.searchRoom(rm);
		if(t<0){
			QMessageBox::warning(NULL, QObject::tr("FET error in data"), 
				QObject::tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

			return false;
		}
		assert(t>=0);
		this->_rooms.append(t);
	}
	
	/*this->_nActivities=0;
	for(int ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].subjectName == this->subjectName
		 && r.internalActivitiesList[ac].activityTagName == this->activityTagName){
			assert(this->_nActivities<MAX_ACTIVITIES_FOR_A_SUBJECT_SUBJECT_TAG);
			this->_activities[this->_nActivities++]=ac;
		}

	this->_n_preferred_rooms=this->roomsNames.count();
	int i=0;
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		this->_rooms[i] = r.searchRoom(*it);
		assert(this->_rooms[i]>=0);
		i++;
	}*/
	
	return true;
}

QString ConstraintSubjectActivityTagPreferredRooms::getXmlDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="<ConstraintSubjectActivityTagPreferredRooms>\n";
	s+="	<Weight_Percentage>"+QString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Subject>"+protect(this->subjectName)+"</Subject>\n";
	s+="	<Activity_Tag>"+protect(this->activityTagName)+"</Activity_Tag>\n";
	s+="	<Number_of_Preferred_Rooms>"+QString::number(this->roomsNames.count())+"</Number_of_Preferred_Rooms>\n";
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++)
		s+="	<Preferred_Room>"+protect(*it)+"</Preferred_Room>\n";
		
	s+="</ConstraintSubjectActivityTagPreferredRooms>\n";

	return s;
}

QString ConstraintSubjectActivityTagPreferredRooms::getDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="Subject activity tag preferred rooms"; s+=", ";
	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);s+=", ";
	s+=QObject::tr("S:%1").arg(this->subjectName);
	s+=QObject::tr("AT:%1", "Activity tag").arg(this->activityTagName);
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		s+=", ";
		s+=QObject::tr("R:%1").arg(*it);
	}

	return s;
}

QString ConstraintSubjectActivityTagPreferredRooms::getDetailedDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Space constraint"); s+="\n";
	s+=QObject::tr("Subject activity tag preferred rooms"); s+="\n";
	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";
	s+=QObject::tr("Subject=%1").arg(this->subjectName);s+="\n";
	s+=QObject::tr("Activity tag=%1").arg(this->activityTagName);s+="\n";
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		s+=QObject::tr("Room=%1").arg(*it);
		s+="\n";
	}

	return s;
}

double ConstraintSubjectActivityTagPreferredRooms::fitness(
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

		c.getSubgroupsBuildingsTimetable(r, subgroupsBuildingsTimetable);
		c.getTeachersBuildingsTimetable(r, teachersBuildingsTimetable);
		 
		//crt_chrom = &c;
		//crt_rules = &r;
		
		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and activity tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	foreach(int ac, this->_activities){
	//for(int i=0; i<this->_nActivities; i++){	
	//	int ac=this->_activities[i];
		int rm=c.rooms[ac];
		if(rm==UNALLOCATED_SPACE)
			continue;
	
		bool ok=true;
		int i;
		for(i=0; i<this->_rooms.count(); i++)
			if(this->_rooms.at(i)==rm)
				break;
		if(i==this->_rooms.count())
			ok=false;

		if(!ok){
			if(rm!=UNALLOCATED_SPACE)
				ok2=false;
			
			if(conflictsString!=NULL){
				QString s=QObject::tr("Space constraint subject activity tag preferred rooms broken for activity with id %1 (subject=%2, activity tag=%3)")
					.arg(r.internalActivitiesList[ac].id)
					.arg(this->subjectName)
					.arg(this->activityTagName);
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

bool ConstraintSubjectActivityTagPreferredRooms::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintSubjectActivityTagPreferredRooms::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintSubjectActivityTagPreferredRooms::isRelatedToSubject(Subject* s)
{
	if(this->subjectName==s->name)
		return true;
	return false;
}

bool ConstraintSubjectActivityTagPreferredRooms::isRelatedToActivityTag(ActivityTag* s)
{
	if(this->activityTagName==s->name)
		return true;
	return false;
}

bool ConstraintSubjectActivityTagPreferredRooms::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintSubjectActivityTagPreferredRooms::isRelatedToRoom(Room* r)
{
	return this->roomsNames.contains(r->name);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetMaxBuildingChangesPerDay::ConstraintStudentsSetMaxBuildingChangesPerDay()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY;
}

ConstraintStudentsSetMaxBuildingChangesPerDay::ConstraintStudentsSetMaxBuildingChangesPerDay(double wp, QString st, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY;
	this->studentsName=st;
	this->maxBuildingChangesPerDay=mc;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDay::computeInternalStructure(Rules& r)
{
	this->iSubgroupsList.clear();
	
	StudentsSet* ss=r.searchAugmentedStudentsSet(this->studentsName);
			
	if(ss==NULL){
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Constraint students set max building changes per day is wrong because it refers to inexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
									 		 
		return false;
	}												
	
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			this->iSubgroupsList.append(tmp);
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);

	return true;
}

QString ConstraintStudentsSetMaxBuildingChangesPerDay::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintStudentsSetMaxBuildingChangesPerDay>\n";

	s+="	<Weight_Percentage>"+QString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Students>"+protect(this->studentsName)+"</Students>\n";
	s+="	<Max_Building_Changes_Per_Day>"+QString::number(this->maxBuildingChangesPerDay)+"</Max_Building_Changes_Per_Day>\n";
		
	s+="</ConstraintStudentsSetMaxBuildingChangesPerDay>\n";

	return s;
}

QString ConstraintStudentsSetMaxBuildingChangesPerDay::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="Students set max building changes per day"; s+=", ";

	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);s+=", ";
	
	s+=QObject::tr("St:%1", "St means students").arg(this->studentsName);s+=", ";

	s+=QObject::tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerDay);

	return s;
}

QString ConstraintStudentsSetMaxBuildingChangesPerDay::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Space constraint"); s+="\n";

	s+=QObject::tr("Students set max building changes per day"); s+="\n";

	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";

	s+=QObject::tr("Students=%1").arg(this->studentsName);s+="\n";

	s+=QObject::tr("Max building changes per day=%1").arg(this->maxBuildingChangesPerDay);s+="\n";

	return s;
}

double ConstraintStudentsSetMaxBuildingChangesPerDay::fitness(
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

		c.getSubgroupsBuildingsTimetable(r, subgroupsBuildingsTimetable);
		c.getTeachersBuildingsTimetable(r, teachersBuildingsTimetable);
		 
		//crt_chrom = &c;
		//crt_rules = &r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	foreach(int sbg, this->iSubgroupsList){
		for(int d2=0; d2<r.nDaysPerWeek; d2++){			
			int crt_building=-1;
			int n_changes=0;
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(subgroupsBuildingsTimetable[sbg][d2][h2]!=-1){
					if(crt_building!=subgroupsBuildingsTimetable[sbg][d2][h2]){
						if(crt_building!=-1)
							n_changes++;
						crt_building=subgroupsBuildingsTimetable[sbg][d2][h2];
					}
				}
			}
						
			if(n_changes>this->maxBuildingChangesPerDay){
				nbroken+=-this->maxBuildingChangesPerDay+n_changes;
		
				if(conflictsString!=NULL){
					QString s=QObject::tr("Space constraint students set max building changes per day broken for students=%1 on day %2")
						.arg(this->studentsName)
						.arg(r.daysOfTheWeek[d2]);
					s += ". ";
					s += QObject::tr("This increases the conflicts total by %1").arg(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes));
					
					dl.append(s);
					cl.append(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes));
				
					*conflictsString+=s+"\n";
				}
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDay::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDay::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.studentsSetsRelated(s->name, this->studentsName);
}

bool ConstraintStudentsSetMaxBuildingChangesPerDay::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsMaxBuildingChangesPerDay::ConstraintStudentsMaxBuildingChangesPerDay()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY;
}

ConstraintStudentsMaxBuildingChangesPerDay::ConstraintStudentsMaxBuildingChangesPerDay(double wp, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY;
	this->maxBuildingChangesPerDay=mc;
}

bool ConstraintStudentsMaxBuildingChangesPerDay::computeInternalStructure(Rules& r)
{
	Q_UNUSED(r);
	
	return true;
}

QString ConstraintStudentsMaxBuildingChangesPerDay::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintStudentsMaxBuildingChangesPerDay>\n";

	s+="	<Weight_Percentage>"+QString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Max_Building_Changes_Per_Day>"+QString::number(this->maxBuildingChangesPerDay)+"</Max_Building_Changes_Per_Day>\n";
		
	s+="</ConstraintStudentsMaxBuildingChangesPerDay>\n";

	return s;
}

QString ConstraintStudentsMaxBuildingChangesPerDay::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="Students max building changes per day"; s+=", ";

	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);s+=", ";
	
	s+=QObject::tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerDay);

	return s;
}

QString ConstraintStudentsMaxBuildingChangesPerDay::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Space constraint"); s+="\n";

	s+=QObject::tr("Students max building changes per day"); s+="\n";

	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";

	s+=QObject::tr("Max building changes per day=%1").arg(this->maxBuildingChangesPerDay);s+="\n";

	return s;
}

double ConstraintStudentsMaxBuildingChangesPerDay::fitness(
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

		c.getSubgroupsBuildingsTimetable(r, subgroupsBuildingsTimetable);
		c.getTeachersBuildingsTimetable(r, teachersBuildingsTimetable);
		 
		//crt_chrom = &c;
		//crt_rules = &r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	for(int sbg=0; sbg<r.nInternalSubgroups; sbg++){
		for(int d2=0; d2<r.nDaysPerWeek; d2++){			
			int crt_building=-1;
			int n_changes=0;
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(subgroupsBuildingsTimetable[sbg][d2][h2]!=-1){
					if(crt_building!=subgroupsBuildingsTimetable[sbg][d2][h2]){
						if(crt_building!=-1)
							n_changes++;
						crt_building=subgroupsBuildingsTimetable[sbg][d2][h2];
					}
				}
			}
						
			if(n_changes>this->maxBuildingChangesPerDay){
				nbroken+=-this->maxBuildingChangesPerDay+n_changes;
		
				if(conflictsString!=NULL){
					QString s=QObject::tr("Space constraint students max building changes per day broken for students=%1 on day %2")
						.arg(r.internalSubgroupsList[sbg]->name)
						.arg(r.daysOfTheWeek[d2]);
					s += ". ";
					s += QObject::tr("This increases the conflicts total by %1").arg(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes));
					
					dl.append(s);
					cl.append(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes));
				
					*conflictsString+=s+"\n";
				}
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsMaxBuildingChangesPerDay::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerDay::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return true;
}

bool ConstraintStudentsMaxBuildingChangesPerDay::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetMaxBuildingChangesPerWeek::ConstraintStudentsSetMaxBuildingChangesPerWeek()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_WEEK;
}

ConstraintStudentsSetMaxBuildingChangesPerWeek::ConstraintStudentsSetMaxBuildingChangesPerWeek(double wp, QString st, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_WEEK;
	this->studentsName=st;
	this->maxBuildingChangesPerWeek=mc;
}

bool ConstraintStudentsSetMaxBuildingChangesPerWeek::computeInternalStructure(Rules& r)
{
	this->iSubgroupsList.clear();
	
	StudentsSet* ss=r.searchAugmentedStudentsSet(this->studentsName);
			
	if(ss==NULL){
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Constraint students set max building changes per week is wrong because it refers to inexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
									 		 
		return false;
	}												
	
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			this->iSubgroupsList.append(tmp);
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);

	return true;
}

QString ConstraintStudentsSetMaxBuildingChangesPerWeek::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintStudentsSetMaxBuildingChangesPerWeek>\n";

	s+="	<Weight_Percentage>"+QString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Students>"+protect(this->studentsName)+"</Students>\n";
	s+="	<Max_Building_Changes_Per_Week>"+QString::number(this->maxBuildingChangesPerWeek)+"</Max_Building_Changes_Per_Week>\n";
		
	s+="</ConstraintStudentsSetMaxBuildingChangesPerWeek>\n";

	return s;
}

QString ConstraintStudentsSetMaxBuildingChangesPerWeek::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="Students set max building changes per week"; s+=", ";

	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);s+=", ";
	
	s+=QObject::tr("St:%1", "St means students").arg(this->studentsName);s+=", ";

	s+=QObject::tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerWeek);

	return s;
}

QString ConstraintStudentsSetMaxBuildingChangesPerWeek::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Space constraint"); s+="\n";

	s+=QObject::tr("Students set max building changes per week"); s+="\n";

	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";

	s+=QObject::tr("Students=%1").arg(this->studentsName);s+="\n";

	s+=QObject::tr("Max building changes per week=%1").arg(this->maxBuildingChangesPerWeek);s+="\n";

	return s;
}

double ConstraintStudentsSetMaxBuildingChangesPerWeek::fitness(
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

		c.getSubgroupsBuildingsTimetable(r, subgroupsBuildingsTimetable);
		c.getTeachersBuildingsTimetable(r, teachersBuildingsTimetable);
		 
		//crt_chrom = &c;
		//crt_rules = &r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	foreach(int sbg, this->iSubgroupsList){
		int n_changes=0;
		for(int d2=0; d2<r.nDaysPerWeek; d2++){			
			int crt_building=-1;
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(subgroupsBuildingsTimetable[sbg][d2][h2]!=-1){
					if(crt_building!=subgroupsBuildingsTimetable[sbg][d2][h2]){
						if(crt_building!=-1)
							n_changes++;
						crt_building=subgroupsBuildingsTimetable[sbg][d2][h2];
					}
				}
			}
		}
						
		if(n_changes>this->maxBuildingChangesPerWeek){
			nbroken+=-this->maxBuildingChangesPerWeek+n_changes;
		
			if(conflictsString!=NULL){
				QString s=QObject::tr("Space constraint students set max building changes per week broken for students=%1")
					.arg(this->studentsName);
				s += ". ";
				s += QObject::tr("This increases the conflicts total by %1").arg(weightPercentage/100* (-maxBuildingChangesPerWeek+n_changes));
				
				dl.append(s);
				cl.append(weightPercentage/100* (-maxBuildingChangesPerWeek+n_changes));
			
				*conflictsString+=s+"\n";
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsSetMaxBuildingChangesPerWeek::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerWeek::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.studentsSetsRelated(s->name, this->studentsName);
}

bool ConstraintStudentsSetMaxBuildingChangesPerWeek::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsMaxBuildingChangesPerWeek::ConstraintStudentsMaxBuildingChangesPerWeek()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_WEEK;
}

ConstraintStudentsMaxBuildingChangesPerWeek::ConstraintStudentsMaxBuildingChangesPerWeek(double wp, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_WEEK;
	this->maxBuildingChangesPerWeek=mc;
}

bool ConstraintStudentsMaxBuildingChangesPerWeek::computeInternalStructure(Rules& r)
{
	Q_UNUSED(r);

	return true;
}

QString ConstraintStudentsMaxBuildingChangesPerWeek::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintStudentsMaxBuildingChangesPerWeek>\n";

	s+="	<Weight_Percentage>"+QString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Max_Building_Changes_Per_Week>"+QString::number(this->maxBuildingChangesPerWeek)+"</Max_Building_Changes_Per_Week>\n";
		
	s+="</ConstraintStudentsMaxBuildingChangesPerWeek>\n";

	return s;
}

QString ConstraintStudentsMaxBuildingChangesPerWeek::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="Students max building changes per week"; s+=", ";

	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);s+=", ";
	
	s+=QObject::tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerWeek);

	return s;
}

QString ConstraintStudentsMaxBuildingChangesPerWeek::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Space constraint"); s+="\n";

	s+=QObject::tr("Students max building changes per week"); s+="\n";

	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";

	s+=QObject::tr("Max building changes per week=%1").arg(this->maxBuildingChangesPerWeek);s+="\n";

	return s;
}

double ConstraintStudentsMaxBuildingChangesPerWeek::fitness(
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

		c.getSubgroupsBuildingsTimetable(r, subgroupsBuildingsTimetable);
		c.getTeachersBuildingsTimetable(r, teachersBuildingsTimetable);
		 
		//crt_chrom = &c;
		//crt_rules = &r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	for(int sbg=0; sbg<r.nInternalSubgroups; sbg++){
		int n_changes=0;
		for(int d2=0; d2<r.nDaysPerWeek; d2++){			
			int crt_building=-1;
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(subgroupsBuildingsTimetable[sbg][d2][h2]!=-1){
					if(crt_building!=subgroupsBuildingsTimetable[sbg][d2][h2]){
						if(crt_building!=-1)
							n_changes++;
						crt_building=subgroupsBuildingsTimetable[sbg][d2][h2];
					}
				}
			}
		}
						
		if(n_changes>this->maxBuildingChangesPerWeek){
			nbroken+=-this->maxBuildingChangesPerWeek+n_changes;
		
			if(conflictsString!=NULL){
				QString s=QObject::tr("Space constraint students max building changes per week broken for students=%1")
					.arg(r.internalSubgroupsList[sbg]->name);
				s += ". ";
				s += QObject::tr("This increases the conflicts total by %1").arg(weightPercentage/100* (-maxBuildingChangesPerWeek+n_changes));
				
				dl.append(s);
				cl.append(weightPercentage/100* (-maxBuildingChangesPerWeek+n_changes));
			
				*conflictsString+=s+"\n";
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsMaxBuildingChangesPerWeek::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerWeek::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return true;
}

bool ConstraintStudentsMaxBuildingChangesPerWeek::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetMinGapsBetweenBuildingChanges::ConstraintStudentsSetMinGapsBetweenBuildingChanges()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_BUILDING_CHANGES;
}

ConstraintStudentsSetMinGapsBetweenBuildingChanges::ConstraintStudentsSetMinGapsBetweenBuildingChanges(double wp, QString st, int mg)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_BUILDING_CHANGES;
	this->studentsName=st;
	this->minGapsBetweenBuildingChanges=mg;
}

bool ConstraintStudentsSetMinGapsBetweenBuildingChanges::computeInternalStructure(Rules& r)
{
	this->iSubgroupsList.clear();
	
	StudentsSet* ss=r.searchAugmentedStudentsSet(this->studentsName);
			
	if(ss==NULL){
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Constraint students set min gaps between building changes is wrong because it refers to inexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
									 		 
		return false;
	}												
	
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			this->iSubgroupsList.append(tmp);
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);

	return true;
}

QString ConstraintStudentsSetMinGapsBetweenBuildingChanges::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintStudentsSetMinGapsBetweenBuildingChanges>\n";

	s+="	<Weight_Percentage>"+QString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Students>"+protect(this->studentsName)+"</Students>\n";
	s+="	<Min_Gaps_Between_Building_Changes>"+QString::number(this->minGapsBetweenBuildingChanges)+"</Min_Gaps_Between_Building_Changes>\n";
		
	s+="</ConstraintStudentsSetMinGapsBetweenBuildingChanges>\n";

	return s;
}

QString ConstraintStudentsSetMinGapsBetweenBuildingChanges::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="Students set min gaps between building changes"; s+=", ";

	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);s+=", ";
	
	s+=QObject::tr("St:%1", "St means students").arg(this->studentsName);s+=", ";

	s+=QObject::tr("MG:%1", "MC means min gaps").arg(this->minGapsBetweenBuildingChanges);

	return s;
}

QString ConstraintStudentsSetMinGapsBetweenBuildingChanges::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Space constraint"); s+="\n";

	s+=QObject::tr("Students set min gaps between building changes"); s+="\n";

	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";

	s+=QObject::tr("Students=%1").arg(this->studentsName);s+="\n";

	s+=QObject::tr("Min gaps between building changes=%1").arg(this->minGapsBetweenBuildingChanges);s+="\n";

	return s;
}

double ConstraintStudentsSetMinGapsBetweenBuildingChanges::fitness(
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

		c.getSubgroupsBuildingsTimetable(r, subgroupsBuildingsTimetable);
		c.getTeachersBuildingsTimetable(r, teachersBuildingsTimetable);
		 
		//crt_chrom = &c;
		//crt_rules = &r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	foreach(int sbg, this->iSubgroupsList){
		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int h2;
			for(h2=0; h2<r.nHoursPerDay; h2++)
				if(subgroupsBuildingsTimetable[sbg][d2][h2]!=-1)
					break;

			int crt_building=-1;					
			if(h2<r.nHoursPerDay)
				crt_building=subgroupsBuildingsTimetable[sbg][d2][h2];
			
			int cnt_gaps=0;
			
			for(h2++; h2<r.nHoursPerDay; h2++){
				if(subgroupsBuildingsTimetable[sbg][d2][h2]!=-1){
					if(subgroupsBuildingsTimetable[sbg][d2][h2]==crt_building)
						cnt_gaps=0;
					else{
						if(cnt_gaps<this->minGapsBetweenBuildingChanges){
							nbroken++;
						
							if(conflictsString!=NULL){
								QString s=QObject::tr("Space constraint students set min gaps between building changes broken for students=%1 on day %2")
									.arg(this->studentsName)
									.arg(r.daysOfTheWeek[d2]);
								s += ". ";
								s += QObject::tr("This increases the conflicts total by %1").arg(weightPercentage/100*1);
					
								dl.append(s);
								cl.append(weightPercentage/100*1);
						
								*conflictsString+=s+"\n";
							}
						}
						
						crt_building=subgroupsBuildingsTimetable[sbg][d2][h2];
						cnt_gaps=0;
					}
				}
				else
					cnt_gaps++;
			}
		}
	}

	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsSetMinGapsBetweenBuildingChanges::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetMinGapsBetweenBuildingChanges::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsSetMinGapsBetweenBuildingChanges::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsSetMinGapsBetweenBuildingChanges::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMinGapsBetweenBuildingChanges::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.studentsSetsRelated(s->name, this->studentsName);
}

bool ConstraintStudentsSetMinGapsBetweenBuildingChanges::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsMinGapsBetweenBuildingChanges::ConstraintStudentsMinGapsBetweenBuildingChanges()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_BUILDING_CHANGES;
}

ConstraintStudentsMinGapsBetweenBuildingChanges::ConstraintStudentsMinGapsBetweenBuildingChanges(double wp, int mg)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_BUILDING_CHANGES;
	this->minGapsBetweenBuildingChanges=mg;
}

bool ConstraintStudentsMinGapsBetweenBuildingChanges::computeInternalStructure(Rules& r)
{
	Q_UNUSED(r);
		
	return true;
}

QString ConstraintStudentsMinGapsBetweenBuildingChanges::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintStudentsMinGapsBetweenBuildingChanges>\n";

	s+="	<Weight_Percentage>"+QString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Min_Gaps_Between_Building_Changes>"+QString::number(this->minGapsBetweenBuildingChanges)+"</Min_Gaps_Between_Building_Changes>\n";
		
	s+="</ConstraintStudentsMinGapsBetweenBuildingChanges>\n";

	return s;
}

QString ConstraintStudentsMinGapsBetweenBuildingChanges::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="Students min gaps between building changes"; s+=", ";

	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);s+=", ";
	
	s+=QObject::tr("MG:%1", "MC means min gaps").arg(this->minGapsBetweenBuildingChanges);

	return s;
}

QString ConstraintStudentsMinGapsBetweenBuildingChanges::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Space constraint"); s+="\n";

	s+=QObject::tr("Students min gaps between building changes"); s+="\n";

	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";

	s+=QObject::tr("Min gaps between building changes=%1").arg(this->minGapsBetweenBuildingChanges);s+="\n";

	return s;
}

double ConstraintStudentsMinGapsBetweenBuildingChanges::fitness(
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

		c.getSubgroupsBuildingsTimetable(r, subgroupsBuildingsTimetable);
		c.getTeachersBuildingsTimetable(r, teachersBuildingsTimetable);
		 
		//crt_chrom = &c;
		//crt_rules = &r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	for(int sbg=0; sbg<r.nInternalSubgroups; sbg++){
		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int h2;
			for(h2=0; h2<r.nHoursPerDay; h2++)
				if(subgroupsBuildingsTimetable[sbg][d2][h2]!=-1)
					break;

			int crt_building=-1;					
			if(h2<r.nHoursPerDay)
				crt_building=subgroupsBuildingsTimetable[sbg][d2][h2];
			
			int cnt_gaps=0;
			
			for(h2++; h2<r.nHoursPerDay; h2++){
				if(subgroupsBuildingsTimetable[sbg][d2][h2]!=-1){
					if(subgroupsBuildingsTimetable[sbg][d2][h2]==crt_building)
						cnt_gaps=0;
					else{
						if(cnt_gaps<this->minGapsBetweenBuildingChanges){
							nbroken++;
						
							if(conflictsString!=NULL){
								QString s=QObject::tr("Space constraint students min gaps between building changes broken for students=%1 on day %2")
									.arg(r.internalSubgroupsList[sbg]->name)
									.arg(r.daysOfTheWeek[d2]);
								s += ". ";
								s += QObject::tr("This increases the conflicts total by %1").arg(weightPercentage/100*1);
					
								dl.append(s);
								cl.append(weightPercentage/100*1);
						
								*conflictsString+=s+"\n";
							}
						}
						
						crt_building=subgroupsBuildingsTimetable[sbg][d2][h2];
						cnt_gaps=0;
					}
				}
				else
					cnt_gaps++;
			}
		}
	}

	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsMinGapsBetweenBuildingChanges::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsMinGapsBetweenBuildingChanges::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsMinGapsBetweenBuildingChanges::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsMinGapsBetweenBuildingChanges::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMinGapsBetweenBuildingChanges::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return true;
}

bool ConstraintStudentsMinGapsBetweenBuildingChanges::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMaxBuildingChangesPerDay::ConstraintTeacherMaxBuildingChangesPerDay()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY;
}

ConstraintTeacherMaxBuildingChangesPerDay::ConstraintTeacherMaxBuildingChangesPerDay(double wp, QString tc, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY;
	this->teacherName=tc;
	this->maxBuildingChangesPerDay=mc;
}

bool ConstraintTeacherMaxBuildingChangesPerDay::computeInternalStructure(Rules& r)
{
	this->teacher_ID=r.searchTeacher(this->teacherName);
	
	if(this->teacher_ID<0){
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Constraint teacher max building changes per day is wrong because it refers to inexistent teacher."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
							 		 
		return false;
	}	

	return true;
}

QString ConstraintTeacherMaxBuildingChangesPerDay::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintTeacherMaxBuildingChangesPerDay>\n";

	s+="	<Weight_Percentage>"+QString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Teacher>"+protect(this->teacherName)+"</Teacher>\n";
	s+="	<Max_Building_Changes_Per_Day>"+QString::number(this->maxBuildingChangesPerDay)+"</Max_Building_Changes_Per_Day>\n";
		
	s+="</ConstraintTeacherMaxBuildingChangesPerDay>\n";

	return s;
}

QString ConstraintTeacherMaxBuildingChangesPerDay::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="Teacher max building changes per day"; s+=", ";

	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);s+=", ";
	
	s+=QObject::tr("T:%1", "T means teacher").arg(this->teacherName);s+=", ";

	s+=QObject::tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerDay);

	return s;
}

QString ConstraintTeacherMaxBuildingChangesPerDay::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Space constraint"); s+="\n";

	s+=QObject::tr("Teacher max building changes per day"); s+="\n";

	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";

	s+=QObject::tr("Teacher=%1").arg(this->teacherName);s+="\n";

	s+=QObject::tr("Max building changes per day=%1").arg(this->maxBuildingChangesPerDay);s+="\n";

	return s;
}

double ConstraintTeacherMaxBuildingChangesPerDay::fitness(
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

		c.getSubgroupsBuildingsTimetable(r, subgroupsBuildingsTimetable);
		c.getTeachersBuildingsTimetable(r, teachersBuildingsTimetable);
		 
		//crt_chrom = &c;
		//crt_rules = &r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	int tch=this->teacher_ID;
	
	for(int d2=0; d2<r.nDaysPerWeek; d2++){			
		int crt_building=-1;
		int n_changes=0;
		for(int h2=0; h2<r.nHoursPerDay; h2++){
			if(teachersBuildingsTimetable[tch][d2][h2]!=-1){
				if(crt_building!=teachersBuildingsTimetable[tch][d2][h2]){
					if(crt_building!=-1)
						n_changes++;
					crt_building=teachersBuildingsTimetable[tch][d2][h2];
				}
			}
		}
					
		if(n_changes>this->maxBuildingChangesPerDay){
			nbroken+=-this->maxBuildingChangesPerDay+n_changes;
	
			if(conflictsString!=NULL){
				QString s=QObject::tr("Space constraint teacher max building changes per day broken for teacher=%1 on day %2")
					.arg(this->teacherName)
					.arg(r.daysOfTheWeek[d2]);
				s += ". ";
				s += QObject::tr("This increases the conflicts total by %1").arg(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes));
				
				dl.append(s);
				cl.append(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes));
			
				*conflictsString+=s+"\n";
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherMaxBuildingChangesPerDay::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerDay::isRelatedToTeacher(Teacher* t)
{
	return this->teacherName==t->name;
}

bool ConstraintTeacherMaxBuildingChangesPerDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerDay::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersMaxBuildingChangesPerDay::ConstraintTeachersMaxBuildingChangesPerDay()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY;
}

ConstraintTeachersMaxBuildingChangesPerDay::ConstraintTeachersMaxBuildingChangesPerDay(double wp, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY;
	this->maxBuildingChangesPerDay=mc;
}

bool ConstraintTeachersMaxBuildingChangesPerDay::computeInternalStructure(Rules& r)
{
	Q_UNUSED(r);

	return true;
}

QString ConstraintTeachersMaxBuildingChangesPerDay::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintTeachersMaxBuildingChangesPerDay>\n";

	s+="	<Weight_Percentage>"+QString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Max_Building_Changes_Per_Day>"+QString::number(this->maxBuildingChangesPerDay)+"</Max_Building_Changes_Per_Day>\n";
		
	s+="</ConstraintTeachersMaxBuildingChangesPerDay>\n";

	return s;
}

QString ConstraintTeachersMaxBuildingChangesPerDay::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="Teachers max building changes per day"; s+=", ";

	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);s+=", ";
	
	s+=QObject::tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerDay);

	return s;
}

QString ConstraintTeachersMaxBuildingChangesPerDay::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=QObject::tr("Space constraint"); s+="\n";

	s+=QObject::tr("Teachers max building changes per day"); s+="\n";

	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";

	s+=QObject::tr("Max building changes per day=%1").arg(this->maxBuildingChangesPerDay);s+="\n";

	return s;
}

double ConstraintTeachersMaxBuildingChangesPerDay::fitness(
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

		c.getSubgroupsBuildingsTimetable(r, subgroupsBuildingsTimetable);
		c.getTeachersBuildingsTimetable(r, teachersBuildingsTimetable);
		 
		//crt_chrom = &c;
		//crt_rules = &r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	for(int tch=0; tch<r.nInternalTeachers; tch++){
		for(int d2=0; d2<r.nDaysPerWeek; d2++){			
			int crt_building=-1;
			int n_changes=0;
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(teachersBuildingsTimetable[tch][d2][h2]!=-1){
					if(crt_building!=teachersBuildingsTimetable[tch][d2][h2]){
						if(crt_building!=-1)
							n_changes++;
						crt_building=teachersBuildingsTimetable[tch][d2][h2];
					}
				}
			}
					
			if(n_changes>this->maxBuildingChangesPerDay){
				nbroken+=-this->maxBuildingChangesPerDay+n_changes;
		
				if(conflictsString!=NULL){
					QString s=QObject::tr("Space constraint teachers max building changes per day broken for teacher=%1 on day %2")
						.arg(r.internalTeachersList[tch]->name)
						.arg(r.daysOfTheWeek[d2]);
					s += ". ";
					s += QObject::tr("This increases the conflicts total by %1").arg(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes));
					
					dl.append(s);
					cl.append(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes));
				
					*conflictsString+=s+"\n";
				}
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeachersMaxBuildingChangesPerDay::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerDay::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return true;
}

bool ConstraintTeachersMaxBuildingChangesPerDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerDay::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMaxBuildingChangesPerWeek::ConstraintTeacherMaxBuildingChangesPerWeek()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_WEEK;
}

ConstraintTeacherMaxBuildingChangesPerWeek::ConstraintTeacherMaxBuildingChangesPerWeek(double wp, QString tc, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_WEEK;
	this->teacherName=tc;
	this->maxBuildingChangesPerWeek=mc;
}

bool ConstraintTeacherMaxBuildingChangesPerWeek::computeInternalStructure(Rules& r)
{
	this->teacher_ID=r.searchTeacher(this->teacherName);
	
	if(this->teacher_ID<0){
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Constraint teacher max building changes per week is wrong because it refers to inexistent teacher."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
							 		 
		return false;
	}	

	return true;
}

QString ConstraintTeacherMaxBuildingChangesPerWeek::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintTeacherMaxBuildingChangesPerWeek>\n";

	s+="	<Weight_Percentage>"+QString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Teacher>"+protect(this->teacherName)+"</Teacher>\n";
	s+="	<Max_Building_Changes_Per_Week>"+QString::number(this->maxBuildingChangesPerWeek)+"</Max_Building_Changes_Per_Week>\n";
		
	s+="</ConstraintTeacherMaxBuildingChangesPerWeek>\n";

	return s;
}

QString ConstraintTeacherMaxBuildingChangesPerWeek::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="Teacher max building changes per week"; s+=", ";

	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);s+=", ";
	
	s+=QObject::tr("T:%1", "T means teacher").arg(this->teacherName);s+=", ";

	s+=QObject::tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerWeek);

	return s;
}

QString ConstraintTeacherMaxBuildingChangesPerWeek::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Space constraint"); s+="\n";

	s+=QObject::tr("Teacher max building changes per week"); s+="\n";

	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";

	s+=QObject::tr("Teacher=%1").arg(this->teacherName);s+="\n";

	s+=QObject::tr("Max building changes per week=%1").arg(this->maxBuildingChangesPerWeek);s+="\n";

	return s;
}

double ConstraintTeacherMaxBuildingChangesPerWeek::fitness(
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

		c.getSubgroupsBuildingsTimetable(r, subgroupsBuildingsTimetable);
		c.getTeachersBuildingsTimetable(r, teachersBuildingsTimetable);
		 
		//crt_chrom = &c;
		//crt_rules = &r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	int tch=this->teacher_ID;
	
	int n_changes=0;

	for(int d2=0; d2<r.nDaysPerWeek; d2++){
		int crt_building=-1;
		for(int h2=0; h2<r.nHoursPerDay; h2++){
			if(teachersBuildingsTimetable[tch][d2][h2]!=-1){
				if(crt_building!=teachersBuildingsTimetable[tch][d2][h2]){
					if(crt_building!=-1)
						n_changes++;
					crt_building=teachersBuildingsTimetable[tch][d2][h2];
				}
			}
		}
	}
					
	if(n_changes>this->maxBuildingChangesPerWeek){
		nbroken+=n_changes-this->maxBuildingChangesPerWeek;
	
		if(conflictsString!=NULL){
			QString s=QObject::tr("Space constraint teacher max building changes per week broken for teacher=%1")
				.arg(this->teacherName);
			s += ". ";
			s += QObject::tr("This increases the conflicts total by %1").arg(weightPercentage/100* (n_changes-maxBuildingChangesPerWeek));
			
			dl.append(s);
			cl.append(weightPercentage/100* (n_changes-maxBuildingChangesPerWeek));
		
			*conflictsString+=s+"\n";
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherMaxBuildingChangesPerWeek::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerWeek::isRelatedToTeacher(Teacher* t)
{
	return this->teacherName==t->name;
}

bool ConstraintTeacherMaxBuildingChangesPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerWeek::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersMaxBuildingChangesPerWeek::ConstraintTeachersMaxBuildingChangesPerWeek()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_WEEK;
}

ConstraintTeachersMaxBuildingChangesPerWeek::ConstraintTeachersMaxBuildingChangesPerWeek(double wp, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_WEEK;
	this->maxBuildingChangesPerWeek=mc;
}

bool ConstraintTeachersMaxBuildingChangesPerWeek::computeInternalStructure(Rules& r)
{
	Q_UNUSED(r);

	return true;
}

QString ConstraintTeachersMaxBuildingChangesPerWeek::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintTeachersMaxBuildingChangesPerWeek>\n";

	s+="	<Weight_Percentage>"+QString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Max_Building_Changes_Per_Week>"+QString::number(this->maxBuildingChangesPerWeek)+"</Max_Building_Changes_Per_Week>\n";
		
	s+="</ConstraintTeachersMaxBuildingChangesPerWeek>\n";

	return s;
}

QString ConstraintTeachersMaxBuildingChangesPerWeek::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="Teachers max building changes per week"; s+=", ";

	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);s+=", ";
	
	s+=QObject::tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerWeek);

	return s;
}

QString ConstraintTeachersMaxBuildingChangesPerWeek::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=QObject::tr("Space constraint"); s+="\n";

	s+=QObject::tr("Teachers max building changes per week"); s+="\n";

	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";

	s+=QObject::tr("Max building changes per week=%1").arg(this->maxBuildingChangesPerWeek);s+="\n";

	return s;
}

double ConstraintTeachersMaxBuildingChangesPerWeek::fitness(
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

		c.getSubgroupsBuildingsTimetable(r, subgroupsBuildingsTimetable);
		c.getTeachersBuildingsTimetable(r, teachersBuildingsTimetable);
		 
		//crt_chrom = &c;
		//crt_rules = &r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	for(int tch=0; tch<r.nInternalTeachers; tch++){
		int n_changes=0;

		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int crt_building=-1;
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(teachersBuildingsTimetable[tch][d2][h2]!=-1){
					if(crt_building!=teachersBuildingsTimetable[tch][d2][h2]){
						if(crt_building!=-1)
							n_changes++;
						crt_building=teachersBuildingsTimetable[tch][d2][h2];
					}
				}
			}
		}
					
		if(n_changes>this->maxBuildingChangesPerWeek){
			nbroken+=n_changes-this->maxBuildingChangesPerWeek;
		
			if(conflictsString!=NULL){
				QString s=QObject::tr("Space constraint teachers max building changes per week broken for teacher=%1")
					.arg(r.internalTeachersList[tch]->name);
				s += ". ";
				s += QObject::tr("This increases the conflicts total by %1").arg(weightPercentage/100* (n_changes-maxBuildingChangesPerWeek));
				
				dl.append(s);
				cl.append(weightPercentage/100* (n_changes-maxBuildingChangesPerWeek));
			
				*conflictsString+=s+"\n";
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeachersMaxBuildingChangesPerWeek::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerWeek::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return true;
}

bool ConstraintTeachersMaxBuildingChangesPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerWeek::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMinGapsBetweenBuildingChanges::ConstraintTeacherMinGapsBetweenBuildingChanges()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_BUILDING_CHANGES;
}

ConstraintTeacherMinGapsBetweenBuildingChanges::ConstraintTeacherMinGapsBetweenBuildingChanges(double wp, QString tc, int mg)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_BUILDING_CHANGES;
	this->teacherName=tc;
	this->minGapsBetweenBuildingChanges=mg;
}

bool ConstraintTeacherMinGapsBetweenBuildingChanges::computeInternalStructure(Rules& r)
{
	this->teacher_ID=r.searchTeacher(this->teacherName);
	
	if(this->teacher_ID<0){
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Constraint teacher min gaps between building change is wrong because it refers to inexistent teacher."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
							 		 
		return false;
	}	

	return true;
}

QString ConstraintTeacherMinGapsBetweenBuildingChanges::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintTeacherMinGapsBetweenBuildingChanges>\n";

	s+="	<Weight_Percentage>"+QString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Teacher>"+protect(this->teacherName)+"</Teacher>\n";
	s+="	<Min_Gaps_Between_Building_Changes>"+QString::number(this->minGapsBetweenBuildingChanges)+"</Min_Gaps_Between_Building_Changes>\n";
		
	s+="</ConstraintTeacherMinGapsBetweenBuildingChanges>\n";

	return s;
}

QString ConstraintTeacherMinGapsBetweenBuildingChanges::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="Teacher min gaps between building change"; s+=", ";

	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);s+=", ";
	
	s+=QObject::tr("T:%1", "T means teacher").arg(this->teacherName);s+=", ";

	s+=QObject::tr("MG:%1", "MC means min gaps").arg(this->minGapsBetweenBuildingChanges);

	return s;
}

QString ConstraintTeacherMinGapsBetweenBuildingChanges::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Space constraint"); s+="\n";

	s+=QObject::tr("Teacher min gaps between building change"); s+="\n";

	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";

	s+=QObject::tr("Teacher=%1").arg(this->teacherName);s+="\n";

	s+=QObject::tr("Min gaps between building change=%1").arg(this->minGapsBetweenBuildingChanges);s+="\n";

	return s;
}

double ConstraintTeacherMinGapsBetweenBuildingChanges::fitness(
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

		c.getSubgroupsBuildingsTimetable(r, subgroupsBuildingsTimetable);
		c.getTeachersBuildingsTimetable(r, teachersBuildingsTimetable);
		 
		//crt_chrom = &c;
		//crt_rules = &r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	int tch=this->teacher_ID;
	
	for(int d2=0; d2<r.nDaysPerWeek; d2++){
		int h2;
		for(h2=0; h2<r.nHoursPerDay; h2++)
			if(teachersBuildingsTimetable[tch][d2][h2]!=-1)
				break;

		int crt_building=-1;					
		if(h2<r.nHoursPerDay)
			crt_building=teachersBuildingsTimetable[tch][d2][h2];
		
		int cnt_gaps=0;
		
		for(h2++; h2<r.nHoursPerDay; h2++){
			if(teachersBuildingsTimetable[tch][d2][h2]!=-1){
				if(teachersBuildingsTimetable[tch][d2][h2]==crt_building)
					cnt_gaps=0;
				else{
					if(cnt_gaps<this->minGapsBetweenBuildingChanges){
						nbroken++;
					
						if(conflictsString!=NULL){
							QString s=QObject::tr("Space constraint teacher min gaps between building changes broken for teacher=%1 on day %2")
								.arg(this->teacherName)
								.arg(r.daysOfTheWeek[d2]);
							s += ". ";
							s += QObject::tr("This increases the conflicts total by %1").arg(weightPercentage/100*1);
				
							dl.append(s);
							cl.append(weightPercentage/100*1);
					
							*conflictsString+=s+"\n";
						}
					}
					
					crt_building=teachersBuildingsTimetable[tch][d2][h2];
					cnt_gaps=0;
				}
			}
			else
				cnt_gaps++;
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherMinGapsBetweenBuildingChanges::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherMinGapsBetweenBuildingChanges::isRelatedToTeacher(Teacher* t)
{
	return this->teacherName==t->name;
}

bool ConstraintTeacherMinGapsBetweenBuildingChanges::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMinGapsBetweenBuildingChanges::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMinGapsBetweenBuildingChanges::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMinGapsBetweenBuildingChanges::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersMinGapsBetweenBuildingChanges::ConstraintTeachersMinGapsBetweenBuildingChanges()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_BUILDING_CHANGES;
}

ConstraintTeachersMinGapsBetweenBuildingChanges::ConstraintTeachersMinGapsBetweenBuildingChanges(double wp, int mg)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_BUILDING_CHANGES;
	this->minGapsBetweenBuildingChanges=mg;
}

bool ConstraintTeachersMinGapsBetweenBuildingChanges::computeInternalStructure(Rules& r)
{
	Q_UNUSED(r);

	return true;
}

QString ConstraintTeachersMinGapsBetweenBuildingChanges::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s="<ConstraintTeachersMinGapsBetweenBuildingChanges>\n";

	s+="	<Weight_Percentage>"+QString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Min_Gaps_Between_Building_Changes>"+QString::number(this->minGapsBetweenBuildingChanges)+"</Min_Gaps_Between_Building_Changes>\n";
		
	s+="</ConstraintTeachersMinGapsBetweenBuildingChanges>\n";

	return s;
}

QString ConstraintTeachersMinGapsBetweenBuildingChanges::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s="Teachers min gaps between building change"; s+=", ";

	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);s+=", ";
	
	s+=QObject::tr("MG:%1", "MC means min gaps").arg(this->minGapsBetweenBuildingChanges);

	return s;
}

QString ConstraintTeachersMinGapsBetweenBuildingChanges::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Space constraint"); s+="\n";

	s+=QObject::tr("Teachers min gaps between building change"); s+="\n";

	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";

	s+=QObject::tr("Min gaps between building change=%1").arg(this->minGapsBetweenBuildingChanges);s+="\n";

	return s;
}

double ConstraintTeachersMinGapsBetweenBuildingChanges::fitness(
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

		c.getSubgroupsBuildingsTimetable(r, subgroupsBuildingsTimetable);
		c.getTeachersBuildingsTimetable(r, teachersBuildingsTimetable);
		 
		//crt_chrom = &c;
		//crt_rules = &r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	for(int tch=0; tch<r.nInternalTeachers; tch++){
		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int h2;
			for(h2=0; h2<r.nHoursPerDay; h2++)
				if(teachersBuildingsTimetable[tch][d2][h2]!=-1)
					break;

			int crt_building=-1;					
			if(h2<r.nHoursPerDay)
				crt_building=teachersBuildingsTimetable[tch][d2][h2];
				
			int cnt_gaps=0;
			
			for(h2++; h2<r.nHoursPerDay; h2++){
				if(teachersBuildingsTimetable[tch][d2][h2]!=-1){
					if(teachersBuildingsTimetable[tch][d2][h2]==crt_building)
						cnt_gaps=0;
					else{
						if(cnt_gaps<this->minGapsBetweenBuildingChanges){
							nbroken++;
					
							if(conflictsString!=NULL){
								QString s=QObject::tr("Space constraint teachers min gaps between building changes broken for teacher=%1 on day %2")
									.arg(r.internalTeachersList[tch]->name)
									.arg(r.daysOfTheWeek[d2]);
								s += ". ";
								s += QObject::tr("This increases the conflicts total by %1").arg(weightPercentage/100*1);
					
								dl.append(s);
								cl.append(weightPercentage/100*1);
						
								*conflictsString+=s+"\n";
							}
						}
					
						crt_building=teachersBuildingsTimetable[tch][d2][h2];
						cnt_gaps=0;
					}
				}
				else
					cnt_gaps++;
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeachersMinGapsBetweenBuildingChanges::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeachersMinGapsBetweenBuildingChanges::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return true;
}

bool ConstraintTeachersMinGapsBetweenBuildingChanges::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMinGapsBetweenBuildingChanges::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMinGapsBetweenBuildingChanges::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMinGapsBetweenBuildingChanges::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
