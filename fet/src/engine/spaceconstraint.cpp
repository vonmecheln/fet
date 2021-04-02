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

#include "genetictimetable_defs.h"
#include "spaceconstraint.h"
#include "rules.h"
#include "spacechromosome.h"
#include "activity.h"
#include "teacher.h"
#include "subject.h"
#include "subjecttag.h"
#include "studentsset.h"
#include "equipment.h"
#include "building.h"
#include "room.h"

#include <qstring.h>

static SpaceChromosome* crt_chrom=NULL;
static Rules* crt_rules=NULL;

#define yesNo(x)				((x)==0?"no":"yes")
#define yesNoTranslated(x)		((x)==0?QObject::tr("no"):QObject::tr("yes"))

static int16 roomsMatrix[MAX_ROOMS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

static bool subgroupsRooms[MAX_TOTAL_SUBGROUPS][MAX_ROOMS]; //used only for ConstraintMinimizeNumberOfRoomsForStudents::fitness

static bool teachersRooms[MAX_TEACHERS][MAX_ROOMS]; //used only for ConstraintMinimizeNumberOfRoomsForTeachers::fitness

//used for ConstraintMaxBuildingChangesPerDayForTeachers and
//ConstraintMaxRoomChangesPerDayForTeachers.
static SpaceChromosome* crt_chrom_2=NULL;

//used for ConstraintMaxBuildingChangesPerDayForStudents and
//ConstraintMaxRoomChangesPerDayForStudents.
static SpaceChromosome* crt_chrom_3=NULL;

//used for ConstraintMaxBuildingChangesPerDayForTeachers and
//ConstraintMaxRoomChangesPerDayForTeachers.
static int16 teachersRoomsWeek1[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

//used for ConstraintMaxBuildingChangesPerDayForStudents and
//ConstraintMaxRoomChangesPerDayForStudents.
static int16 subgroupsRoomsWeek1[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

static int rooms_conflicts=-1;

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

static void computeTeachersRoomsWeek1(SpaceChromosome& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/])
{
	assert(r.initialized);
	assert(r.internalStructureComputed);

	int i, j, k;
	for(i=0; i<r.nInternalTeachers; i++)
		for(j=0; j<r.nDaysPerWeek; j++)
			for(k=0; k<r.nHoursPerDay; k++)
				teachersRoomsWeek1[i][j][k]=UNALLOCATED_SPACE;

	Activity *act;
	for(i=0; i<r.nInternalActivities; i++) 
		if(days[i]!=UNALLOCATED_TIME && hours[i]!=UNALLOCATED_TIME){
			int room=c.rooms[i];
			if(room!=UNALLOCATED_SPACE){
				act=&r.internalActivitiesList[i];
				for(int dd=0; dd < act->duration && hours[i]+dd < r.nHoursPerDay; dd++){
					for(int ti=0; ti<act->nTeachers; ti++){
						int tch = act->teachers[ti]; //teacher index
						if(teachersRoomsWeek1[tch][days[i]][hours[i]+dd]==UNALLOCATED_SPACE)
							teachersRoomsWeek1[tch][days[i]][hours[i]+dd]=room;
					}
				}
			}
		}
}

static void computeSubgroupsRoomsWeek1(SpaceChromosome& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/])
{
	assert(r.initialized);
	assert(r.internalStructureComputed);

	int i, j, k;
	for(i=0; i<r.nInternalSubgroups; i++)
		for(j=0; j<r.nDaysPerWeek; j++)
			for(k=0; k<r.nHoursPerDay; k++)
				subgroupsRoomsWeek1[i][j][k]=UNALLOCATED_SPACE;

	Activity *act;
	for(i=0; i<r.nInternalActivities; i++)
		if(days[i]!=UNALLOCATED_TIME && hours[i]!=UNALLOCATED_TIME) {
			int room=c.rooms[i];
			if(room!=UNALLOCATED_SPACE){
				act=&r.internalActivitiesList[i];
				for(int dd=0; dd < act->duration && hours[i]+dd < r.nHoursPerDay; dd++){
					for(int isg=0; isg < act->nSubgroups; isg++){ //isg -> index subgroup
						int sg = act->subgroups[isg]; //sg -> subgroup
						if(subgroupsRoomsWeek1[sg][days[i]][hours[i]+dd]==UNALLOCATED_SPACE)
							subgroupsRoomsWeek1[sg][days[i]][hours[i]+dd]=room;
					}
				}
			}
		}
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

SpaceConstraint::SpaceConstraint()
{
	this->type=CONSTRAINT_GENERIC_SPACE;
}

SpaceConstraint::~SpaceConstraint()
{
}

SpaceConstraint::SpaceConstraint(double w, bool c)
{
	this->weight=w;
	assert(w<=100.0);
	this->type=CONSTRAINT_GENERIC_SPACE;
	this->compulsory=c;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

ConstraintBasicCompulsorySpace::ConstraintBasicCompulsorySpace()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_BASIC_COMPULSORY_SPACE;
	this->compulsory=true;
}

ConstraintBasicCompulsorySpace::ConstraintBasicCompulsorySpace(double w)
	: SpaceConstraint(w, true)
{
	this->type=CONSTRAINT_BASIC_COMPULSORY_SPACE;
}

bool ConstraintBasicCompulsorySpace::computeInternalStructure(Rules& r)
{
	if(&r!=NULL)
		;
	/*do nothing*/
	
	return true;
}

QString ConstraintBasicCompulsorySpace::getXmlDescription(Rules& r)
{
	if(&r!=NULL)
		;

	QString s = "<ConstraintBasicCompulsorySpace>\n";
	s += "	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";
	s+=yesNo(this->compulsory);
	s+="</Compulsory>\n";
	s += "</ConstraintBasicCompulsorySpace>\n";
	return s;
}

QString ConstraintBasicCompulsorySpace::getDescription(Rules& r)
{
	if(&r!=NULL)
		;
	
	QString s = QObject::tr("Basic compulsory constraints (space), W:%1").arg(this->weight);
	s+=", ";
	s += QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory));
	
	return s;
}

QString ConstraintBasicCompulsorySpace::getDetailedDescription(Rules& r)
{
	if(&r!=NULL)
		;

	QString s=QObject::tr("These are the basic compulsory constraints \n"
			"(referring to space allocation) for any timetable\n");
	s+=QObject::tr("Weight=%1").arg(this->weight);s+="\n";
	s+=QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory));s+="\n";

	s+=QObject::tr("The basic space constraints try to avoid:\n");
	s+=QObject::tr("- unallocated activities\n");
	s+=QObject::tr("- activities with more students than the capacity of the room\n");
	s+=QObject::tr("- rooms assigned to more than one activity simultaneously\n");

	return s;
}

//critical function here - must be optimized for speed
int ConstraintBasicCompulsorySpace::fitness(
	SpaceChromosome& c,
	Rules& r,
	const int days[/*MAX_ACTIVITIES*/],
	const int hours[/*MAX_ACTIVITIES*/],
	QString* conflictsString)
	{

	assert(r.internalStructureComputed);

	int roomsConflicts;

	//This constraint fitness calculation routine is called firstly,
	//so we can compute the rooms conflicts faster this way.
	if(crt_chrom!=&c || crt_rules!=&r || rooms_conflicts<0 || c.spaceChangedForMatrixCalculation){
		roomsConflicts = c.getRoomsMatrix(r, days, hours, roomsMatrix);

		crt_chrom = &c;
		crt_rules = &r;
		
		c.spaceChangedForMatrixCalculation=false;
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
			else{
				//The capacity of each room must be respected
				//(the number of students must be less than the capacity)
				int rm=c.rooms[i];
				if(r.internalActivitiesList[i].nTotalStudents>r.internalRoomsList[rm]->capacity){
					if(r.internalActivitiesList[i].parity==PARITY_WEEKLY)
						nor+=2;
					else
						nor++;
				}
			}

		//Below, for rooms, please remember that 2 means a weekly activity
		//and 1 fortnightly one. So, is the matrix roomsMatrix[rooms][day][hour]==2,
		//it is ok.

		//Calculates the number of rooms exhaustion (when a room is occupied
		//for more than one activity at the same time)
		/*nre=0;
		for(i=0; i<r.nInternalRooms; i++)
			for(int j=0; j<r.nDaysPerWeek; j++)
				for(int k=0; k<r.nHoursPerDay; k++){
					int tmp=roomsMatrix[i][j][k]-2;
					if(tmp>0){
						if(conflictsString!=NULL){
							(*conflictsString)+="Space constraint basic ";
							(*conflictsString)+="compulsory: rooms with name ";
							(*conflictsString)+=r.internalRoomsList[i]->name;
							(*conflictsString)+=" has more than one allocated activity on";
							(*conflictsString)+=" day ";
							(*conflictsString)+=r.daysOfTheWeek[j];
							(*conflictsString)+=", hour ";
							(*conflictsString)+=QString::number(k);
							(*conflictsString)+=". This increases the conflicts total by ";
							(*conflictsString)+=QString::number(tmp*weight);
							(*conflictsString)+="\n";
						}
						nre+=tmp;
					}
				}*/
		nre = roomsConflicts;
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
					(*conflictsString) += QObject::tr("Space constraint basic compulsory: unallocated activity with id=%1").arg(r.internalActivitiesList[i].id);
					(*conflictsString) += QObject::tr(" - this increases the conflicts total by %1")
						.arg(weight* /*r.internalActivitiesList[i].duration * r.internalActivitiesList[i].nSubgroups * */ 10000);
					(*conflictsString) += "\n";
				}
			}
			else{
				//The capacity of each room must be respected
				//(the number of students must be less than the capacity)
				int rm=c.rooms[i];
				if(r.internalActivitiesList[i].nTotalStudents>r.internalRoomsList[rm]->capacity){
					int tmp;
					if(r.internalActivitiesList[i].parity==PARITY_WEEKLY)
						tmp=2;
					else
						tmp=1;
	
					nor+=tmp;

					if(conflictsString!=NULL){
						QString s;
						s=QObject::tr("Space constraint basic compulsory: room %1 has allocated activity with id %2 and the capacity of the room is overloaded")
						.arg(r.internalRoomsList[rm]->name).arg(r.internalActivitiesList[i].id);
						(*conflictsString) += s;
						(*conflictsString) += "\n";
					}
				}
			}

		//Below, for rooms, please remember that 2 means a weekly activity
		//and 1 fortnightly one. So, is the matrix roomsMatrix[rooms][day][hour]==2,
		//it is ok.

		//Calculates the number of rooms exhaustion (when a room is occupied
		//for more than one activity at the same time)
		nre=0;
		for(i=0; i<r.nInternalRooms; i++)
			for(int j=0; j<r.nDaysPerWeek; j++)
				for(int k=0; k<r.nHoursPerDay; k++){
					int tmp=roomsMatrix[i][j][k]-2;
					if(tmp>0){
						if(conflictsString!=NULL){
							(*conflictsString)+=QObject::tr("Space constraint basic compulsory: room with name %1 has more than one allocated activity on day %2, hour %3.")
								.arg(r.internalRoomsList[i]->name)
								.arg(r.daysOfTheWeek[j])
								.arg(r.hoursOfTheDay[k]);
							(*conflictsString)+=" ";
							(*conflictsString)+=QObject::tr("This increases the conflicts total by %1").arg(tmp*weight);
							(*conflictsString)+="\n";
						}
						nre+=tmp;
					}
				}
	}
	/*if(roomsConflicts!=-1)
		assert(nre==roomsConflicts);*/ //just a check, works only on logged fitness calculation

	return int (ceil ( weight * (unallocated + nre + nor) ) ); //fitness factor
}

bool ConstraintBasicCompulsorySpace::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintBasicCompulsorySpace::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintBasicCompulsorySpace::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintBasicCompulsorySpace::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintBasicCompulsorySpace::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

	return false;
}

bool ConstraintBasicCompulsorySpace::isRelatedToEquipment(Equipment* e)
{
	if(e)
		;

	return false;
}

bool ConstraintBasicCompulsorySpace::isRelatedToBuilding(Building* b)
{
	if(b)
		;

	return false;
}

bool ConstraintBasicCompulsorySpace::isRelatedToRoom(Room* r)
{
	if(r)
		;

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

ConstraintRoomNotAvailable::ConstraintRoomNotAvailable()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_ROOM_NOT_AVAILABLE;
}

ConstraintRoomNotAvailable::ConstraintRoomNotAvailable(double w, bool c, const QString& rn, int day, int start_hour, int end_hour)
	: SpaceConstraint(w, c)
{
	this->roomName=rn;
	this->d=day;
	this->h1=start_hour;
	this->h2=end_hour;
	this->type=CONSTRAINT_ROOM_NOT_AVAILABLE;
}

QString ConstraintRoomNotAvailable::getXmlDescription(Rules& r){
	QString s="<ConstraintRoomNotAvailable>\n";
	s+="	<Weight>"+QString::number(weight)+"</Weight>\n";
	s+="	<Compulsory>";
	s+=yesNo(this->compulsory);
	s+="</Compulsory>\n";
	s+="	<Room_Name>"+protect(this->roomName)+"</Room_Name>\n";
	s+="	<Day>"+protect(r.daysOfTheWeek[this->d])+"</Day>\n";
	s+="	<Start_Hour>"+protect(r.hoursOfTheDay[this->h1])+"</Start_Hour>\n";
	s+="	<End_Hour>"+protect(r.hoursOfTheDay[this->h2])+"</End_Hour>\n";
	s+="</ConstraintRoomNotAvailable>\n";
	return s;
}

QString ConstraintRoomNotAvailable::getDescription(Rules& r){
	QString s=QObject::tr("Room not available");s+=",";
	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
	s+=(QObject::tr("R:%1").arg(this->roomName));s+=", ";
	s+=(QObject::tr("D:%1").arg(r.daysOfTheWeek[this->d]));s+=", ";
	s+=(QObject::tr("SH:%1").arg(r.hoursOfTheDay[this->h1]));s+=", ";
	s+=(QObject::tr("EH:%1").arg(r.hoursOfTheDay[this->h2]));s+=", ";

	return s;
}

QString ConstraintRoomNotAvailable::getDetailedDescription(Rules& r){
	QString s=QObject::tr("Space constraint");s+="\n";
	s+=QObject::tr("Room not available");s+="\n";
	s+=(QObject::tr("Weight=%1").arg(this->weight));s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	s+=(QObject::tr("Room=%1").arg(this->roomName));s+="\n";
	s+=(QObject::tr("Day=%1").arg(r.daysOfTheWeek[this->d]));s+="\n";
	s+=(QObject::tr("Start hour=%1").arg(r.hoursOfTheDay[this->h1]));s+="\n";
	s+=(QObject::tr("End hour=%1").arg(r.hoursOfTheDay[this->h2]));s+="\n";

	return s;
}

bool ConstraintRoomNotAvailable::computeInternalStructure(Rules& r){
	this->room_ID=r.searchRoom(this->roomName);
	assert(this->room_ID>=0);
	
	return true;
}

//critical function here - must be optimized for speed
int ConstraintRoomNotAvailable::fitness(
	SpaceChromosome& c,
	Rules& r,
	const int days[/*MAX_ACTIVITIES*/],
	const int hours[/*MAX_ACTIVITIES*/],
	QString* conflictsString)
{
	//if the matrices roomsMatrix is already calculated, do not calculate it again!
	if(crt_chrom!=&c || crt_rules!=&r || rooms_conflicts<0 || c.spaceChangedForMatrixCalculation){
		rooms_conflicts = c.getRoomsMatrix(r, days, hours, roomsMatrix);

		crt_chrom = &c;
		crt_rules = &r;
		
		c.spaceChangedForMatrixCalculation=false;
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
					*conflictsString+=
						(QObject::tr("Space constraint room not available broken for room %1 on day %2, hour %3")
						.arg(r.internalRoomsList[i]->name)
						.arg(r.daysOfTheWeek[j])
						.arg(k));
					*conflictsString += ". ";
					*conflictsString += (QObject::tr("This increases the conflicts total by %1").arg(roomsMatrix[i][j][k]*weight));
					*conflictsString += "\n";
				}
				nbroken+=roomsMatrix[i][j][k];
			}
	}

	return int (ceil ( weight * nbroken ) );
}

bool ConstraintRoomNotAvailable::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintRoomNotAvailable::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintRoomNotAvailable::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintRoomNotAvailable::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintRoomNotAvailable::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

	return false;
}

bool ConstraintRoomNotAvailable::isRelatedToEquipment(Equipment* e)
{
	if(e)
		;

	return false;
}

bool ConstraintRoomNotAvailable::isRelatedToBuilding(Building* b)
{
	if(b)
		;

	return false;
}

bool ConstraintRoomNotAvailable::isRelatedToRoom(Room* r)
{
	return this->roomName==r->name;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintRoomTypeNotAllowedSubjects::ConstraintRoomTypeNotAllowedSubjects()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_ROOM_TYPE_NOT_ALLOWED_SUBJECTS;
}

ConstraintRoomTypeNotAllowedSubjects::ConstraintRoomTypeNotAllowedSubjects(double w, bool c, const QString& room_type)
	: SpaceConstraint(w, c)
{
	this->type=CONSTRAINT_ROOM_TYPE_NOT_ALLOWED_SUBJECTS;
	this->roomType=room_type;
}

bool ConstraintRoomTypeNotAllowedSubjects::computeInternalStructure(Rules& r)
{
	this->_nRooms=0;
	for(int i=0; i<r.nInternalRooms; i++)
		if(r.internalRoomsList[i]->type == this->roomType)
			this->_rooms[this->_nRooms++]=i;
			
	this->_nActivities=0;
	for(int i=0; i<r.nInternalActivities; i++)
		if(this->searchNotAllowedSubject(r.internalActivitiesList[i].subjectName) == true)
			this->_activities[this->_nActivities++]=i;
			
	return true;
}

void ConstraintRoomTypeNotAllowedSubjects::addNotAllowedSubject(const QString& subjectName)
{
	this->subjects.append(subjectName);
}

int ConstraintRoomTypeNotAllowedSubjects::removeNotAllowedSubject(const QString& subjectName)
{
	int tmp=this->subjects.remove(subjectName);
	return tmp;
}

bool ConstraintRoomTypeNotAllowedSubjects::searchNotAllowedSubject(const QString& subjectName)
{
	int tmp=this->subjects.findIndex(subjectName);
	if(tmp>=0)
		return true;
	else
		return false;
}

QString ConstraintRoomTypeNotAllowedSubjects::getXmlDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s="<ConstraintRoomTypeNotAllowedSubjects>\n";
	s+="	<Weight>"+QString::number(weight)+"</Weight>\n";
	s+="	<Compulsory>";
	s+=yesNo(this->compulsory);
	s+="</Compulsory>\n";
	s+="	<Room_Type>"+protect(this->roomType)+"</Room_Type>\n";

	for(QStringList::Iterator it=this->subjects.begin(); it!=this->subjects.end(); it++)
		s+="	<Subject>"+protect(*it)+"</Subject>\n";
	s+="</ConstraintRoomTypeNotAllowedSubjects>\n";

	return s;
}

QString ConstraintRoomTypeNotAllowedSubjects::getDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s="Room type not allowed subjects"; s+=", ";
	s+=QObject::tr("W:%1").arg(this->weight);s+=", ";
	s+=QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory));s+=", ";
	s+=QObject::tr("RT:%1").arg(this->roomType);s+=",";

	for(QStringList::Iterator it=this->subjects.begin(); it!=this->subjects.end(); it++)
		s+=QObject::tr("S:%1").arg(*it); s+=",";

	return s;
}

QString ConstraintRoomTypeNotAllowedSubjects::getDetailedDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s=QObject::tr("Space constraint"); s+="\n";
	s+=QObject::tr("Room type not allowed subjects"); s+="\n";
	s+=QObject::tr("Weight=%1").arg(this->weight);s+="\n";
	s+=QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory));s+="\n";
	s+=QObject::tr("Room Type=%1").arg(this->roomType);s+="\n";

	for(QStringList::Iterator it=this->subjects.begin(); it!=this->subjects.end(); it++){
		s+=QObject::tr("Subject=%1").arg(*it); 
		s+="\n";
	}

	return s;
}

//critical function here - must be optimized for speed
int ConstraintRoomTypeNotAllowedSubjects::fitness(
	SpaceChromosome& c,
	Rules& r,
	const int days[/*MAX_ACTIVITIES*/],
	const int hours[/*MAX_ACTIVITIES*/],
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(crt_chrom!=&c || crt_rules!=&r || rooms_conflicts<0 || c.spaceChangedForMatrixCalculation){
		rooms_conflicts = c.getRoomsMatrix(r, days, hours, roomsMatrix);

		crt_chrom = &c;
		crt_rules = &r;
		
		c.spaceChangedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts - the type of the room
	//does not allow a certain kind of subject
	//The fastest way seems to iterate over all activities
	//involved in this constraint (every not allowed subject),
	//find the scheduled room and check to see if the type
	//of this room is accepted or not.

	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int i=0; i<this->_nActivities; i++){
			int ac=this->_activities[i];
			int parity=r.internalActivitiesList[ac].parity==PARITY_WEEKLY?2:1;
			int rm=c.rooms[ac];
			if(rm==UNALLOCATED_SPACE)
				continue;
			QString typ=r.internalRoomsList[rm]->type;
			//maybe this can be a little speeded up by keeping a list of the types
			//and comparing integers instead of strings.
			if(typ==this->roomType)
				nbroken+=parity;
		}
	}
	//with logging
	else{
		nbroken=0;
		for(int i=0; i<this->_nActivities; i++){
			int ac=this->_activities[i];
			int parity=r.internalActivitiesList[ac].parity==PARITY_WEEKLY?2:1;
			int rm=c.rooms[ac];
			if(rm==UNALLOCATED_SPACE)
				continue;
			QString typ=r.internalRoomsList[rm]->type;
			//maybe this can be a little speeded up by keeping a list of the types
			//and comparing integers instead of strings.
			if(typ==this->roomType){
				if(conflictsString!=NULL){
					*conflictsString+=
						(QObject::tr("Space constraint room type not allowed subjects broken for room %1, activity with id %2")
						.arg(r.internalRoomsList[rm]->name)
						.arg(r.internalActivitiesList[ac].id));
					*conflictsString += ". ";
					*conflictsString += (QObject::tr("This increases the conflicts total by %1").arg(parity*weight));
					*conflictsString += "\n";
				}
	
				nbroken+=parity;
			}
		}
	}

	return int (ceil ( weight * nbroken ) );
}

bool ConstraintRoomTypeNotAllowedSubjects::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintRoomTypeNotAllowedSubjects::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintRoomTypeNotAllowedSubjects::isRelatedToSubject(Subject* s)
{
	if(this->searchNotAllowedSubject(s->name)==true)
		return true;
	return false;
}

bool ConstraintRoomTypeNotAllowedSubjects::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintRoomTypeNotAllowedSubjects::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

	return false;
}

bool ConstraintRoomTypeNotAllowedSubjects::isRelatedToEquipment(Equipment* e)
{
	if(e)
		;

	return false;
}

bool ConstraintRoomTypeNotAllowedSubjects::isRelatedToBuilding(Building* b)
{
	if(b)
		;

	return false;
}

bool ConstraintRoomTypeNotAllowedSubjects::isRelatedToRoom(Room* r)
{
	return this->roomType==r->type;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintSubjectRequiresEquipments::ConstraintSubjectRequiresEquipments()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_SUBJECT_REQUIRES_EQUIPMENTS;
}

ConstraintSubjectRequiresEquipments::ConstraintSubjectRequiresEquipments(double w, bool c, const QString& subj)
	: SpaceConstraint(w, c)
{
	this->type=CONSTRAINT_SUBJECT_REQUIRES_EQUIPMENTS;
	this->subjectName=subj;
}

bool ConstraintSubjectRequiresEquipments::computeInternalStructure(Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the subject of the constraint.
	//Computes also the internal list of indices of equipments
	
	this->_nActivities=0;
	for(int ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].subjectName == this->subjectName){
			assert(this->_nActivities<MAX_ACTIVITIES_FOR_A_SUBJECT);
			this->_activities[this->_nActivities++]=ac;
		}
		
	this->_nEquipments=0;
	for(int eq=0; eq<r.nInternalEquipments; eq++)
		if(this->searchRequiredEquipment(r.internalEquipmentsList[eq]->name)){
			assert(this->_nEquipments<MAX_EQUIPMENTS_FOR_A_CONSTRAINT);
			this->_equipments[this->_nEquipments++]=eq;
		}
		
	return true;
}

void ConstraintSubjectRequiresEquipments::addRequiredEquipment(const QString& equip)
{
	this->equipmentsNames.append(equip);
}

int ConstraintSubjectRequiresEquipments::removeRequiredEquipment(const QString& equip)
{
	int tmp=this->equipmentsNames.remove(equip);
	return tmp;
}

bool ConstraintSubjectRequiresEquipments::searchRequiredEquipment(const QString& equip)
{
	int tmp=this->equipmentsNames.findIndex(equip);
	if(tmp>=0)
		return true;
	else
		return false;
}

QString ConstraintSubjectRequiresEquipments::getXmlDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s="<ConstraintSubjectRequiresEquipments>\n";
	s+="	<Weight>"+QString::number(weight)+"</Weight>\n";
	s+="	<Compulsory>";
	s+=yesNo(this->compulsory);
	s+="</Compulsory>\n";
	s+="	<Subject>"+protect(this->subjectName)+"</Subject>\n";

	for(QStringList::Iterator it=this->equipmentsNames.begin(); it!=this->equipmentsNames.end(); it++)
		s+="	<Equipment>"+protect(*it)+"</Equipment>\n";
		
	s+="</ConstraintSubjectRequiresEquipments>\n";

	return s;
}

QString ConstraintSubjectRequiresEquipments::getDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s="Subject requires equipments"; s+=", ";
	s+=QObject::tr("W:%1").arg(this->weight);s+=", ";
	s+=QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory));s+=", ";
	s+=QObject::tr("S:%1").arg(this->subjectName);s+=",";

	for(QStringList::Iterator it=this->equipmentsNames.begin(); it!=this->equipmentsNames.end(); it++)
		s+=QObject::tr("E:%1").arg(*it); s+=",";

	return s;
}

QString ConstraintSubjectRequiresEquipments::getDetailedDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s=QObject::tr("Space constraint"); s+="\n";
	s+=QObject::tr("Subject requires equipments"); s+="\n";
	s+=QObject::tr("Weight=%1").arg(this->weight);s+="\n";
	s+=QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory));s+="\n";
	s+=QObject::tr("Subject=%1").arg(this->subjectName);s+="\n";

	for(QStringList::Iterator it=this->equipmentsNames.begin(); it!=this->equipmentsNames.end(); it++){
		s+=QObject::tr("Equipment=%1").arg(*it); 
		s+="\n";
	}

	return s;
}

//critical function here - must be optimized for speed
int ConstraintSubjectRequiresEquipments::fitness(
	SpaceChromosome& c,
	Rules& r,
	const int days[/*MAX_ACTIVITIES*/],
	const int hours[/*MAX_ACTIVITIES*/],
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(crt_chrom!=&c || crt_rules!=&r || rooms_conflicts<0 || c.spaceChangedForMatrixCalculation){
		rooms_conflicts = c.getRoomsMatrix(r, days, hours, roomsMatrix);

		crt_chrom = &c;
		crt_rules = &r;
		
		c.spaceChangedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int i=0; i<this->_nActivities; i++){	
			int ac=this->_activities[i];
			int parity=r.internalActivitiesList[ac].parity==PARITY_WEEKLY?2:1;
			int rm=c.rooms[ac];
			if(rm==UNALLOCATED_SPACE)
				continue;
		
			bool ok=true;
			for(int j=0; j<this->_nEquipments; j++){
				int eq=this->_equipments[j];
				if(!r.roomHasEquipment[rm][eq]){
					ok=false;
					break;
				}
			}

			if(!ok)
				nbroken+=parity;
		}
	}
		//with logging
	else{
		nbroken=0;
		for(int i=0; i<this->_nActivities; i++){	
			int ac=this->_activities[i];
			int parity=r.internalActivitiesList[ac].parity==PARITY_WEEKLY?2:1;
			int rm=c.rooms[ac];
			if(rm==UNALLOCATED_SPACE)
				continue;
			
			bool ok=true;
			for(int j=0; j<this->_nEquipments; j++){
				int eq=this->_equipments[j];
				if(!r.roomHasEquipment[rm][eq]){
					ok=false;
					break;
				}
			}

			if(!ok){
				if(conflictsString!=NULL){
					*conflictsString+=
						(QObject::tr("Space constraint subject requires equipments broken for room %1, activity with id %2")
						.arg(r.internalRoomsList[rm]->name)
						.arg(r.internalActivitiesList[ac].id));
					*conflictsString += ". ";
					*conflictsString += (QObject::tr("This increases the conflicts total by %1").arg(weight*parity));
					*conflictsString += "\n";
				}

				nbroken+=parity;
			}
		}
	}

	return int (ceil ( weight * nbroken ) );
}

bool ConstraintSubjectRequiresEquipments::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintSubjectRequiresEquipments::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintSubjectRequiresEquipments::isRelatedToSubject(Subject* s)
{
	if(this->subjectName==s->name)
		return true;
	return false;
}

bool ConstraintSubjectRequiresEquipments::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintSubjectRequiresEquipments::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

	return false;
}

bool ConstraintSubjectRequiresEquipments::isRelatedToEquipment(Equipment* e)
{
	return this->searchRequiredEquipment(e->name);
}

bool ConstraintSubjectRequiresEquipments::isRelatedToBuilding(Building* b)
{
	if(b)
		;

	return false;
}

bool ConstraintSubjectRequiresEquipments::isRelatedToRoom(Room* r)
{
	if(r)
		;

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintSubjectSubjectTagRequireEquipments::ConstraintSubjectSubjectTagRequireEquipments()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_SUBJECT_SUBJECT_TAG_REQUIRE_EQUIPMENTS;
}

ConstraintSubjectSubjectTagRequireEquipments::ConstraintSubjectSubjectTagRequireEquipments(double w, bool c, const QString& subj, const QString& subt)
	: SpaceConstraint(w, c)
{
	this->type=CONSTRAINT_SUBJECT_SUBJECT_TAG_REQUIRE_EQUIPMENTS;
	this->subjectName=subj;
	this->subjectTagName=subt;
}

bool ConstraintSubjectSubjectTagRequireEquipments::computeInternalStructure(Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the subject of the constraint.
	//Computes also the internal list of indices of equipments
	
	this->_nActivities=0;
	for(int ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].subjectName == this->subjectName
		 &&r.internalActivitiesList[ac].subjectTagName==this->subjectTagName){
			assert(this->_nActivities<MAX_ACTIVITIES_FOR_A_SUBJECT);
			this->_activities[this->_nActivities++]=ac;
		}
		
	this->_nEquipments=0;
	for(int eq=0; eq<r.nInternalEquipments; eq++)
		if(this->searchRequiredEquipment(r.internalEquipmentsList[eq]->name)){
			assert(this->_nEquipments<MAX_EQUIPMENTS_FOR_A_CONSTRAINT);
			this->_equipments[this->_nEquipments++]=eq;
		}
		
	return true;
}

void ConstraintSubjectSubjectTagRequireEquipments::addRequiredEquipment(const QString& equip)
{
	this->equipmentsNames.append(equip);
}

int ConstraintSubjectSubjectTagRequireEquipments::removeRequiredEquipment(const QString& equip)
{
	int tmp=this->equipmentsNames.remove(equip);
	return tmp;
}

bool ConstraintSubjectSubjectTagRequireEquipments::searchRequiredEquipment(const QString& equip)
{
	int tmp=this->equipmentsNames.findIndex(equip);
	if(tmp>=0)
		return true;
	else
		return false;
}

QString ConstraintSubjectSubjectTagRequireEquipments::getXmlDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s="<ConstraintSubjectSubjectTagRequireEquipments>\n";
	s+="	<Weight>"+QString::number(weight)+"</Weight>\n";
	s+="	<Compulsory>";
	s+=yesNo(this->compulsory);
	s+="</Compulsory>\n";
	s+="	<Subject>"+protect(this->subjectName)+"</Subject>\n";
	s+="	<Subject_Tag>"+protect(this->subjectTagName)+"</Subject_Tag>\n";

	for(QStringList::Iterator it=this->equipmentsNames.begin(); it!=this->equipmentsNames.end(); it++)
		s+="	<Equipment>"+protect(*it)+"</Equipment>\n";
		
	s+="</ConstraintSubjectSubjectTagRequireEquipments>\n";

	return s;
}

QString ConstraintSubjectSubjectTagRequireEquipments::getDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s="Subject subject tag require equipments"; s+=", ";
	s+=QObject::tr("W:%1").arg(this->weight);s+=", ";
	s+=QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory));s+=", ";
	s+=QObject::tr("S:%1").arg(this->subjectName);s+=",";
	s+=QObject::tr("ST:%1").arg(this->subjectTagName);s+=",";

	for(QStringList::Iterator it=this->equipmentsNames.begin(); it!=this->equipmentsNames.end(); it++)
		s+=QObject::tr("E:%1").arg(*it); s+=",";

	return s;
}

QString ConstraintSubjectSubjectTagRequireEquipments::getDetailedDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s=QObject::tr("Space constraint"); s+="\n";
	s+=QObject::tr("Subject subject tag require equipments"); s+="\n";
	s+=QObject::tr("Weight=%1").arg(this->weight);s+="\n";
	s+=QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory));s+="\n";
	s+=QObject::tr("Subject=%1").arg(this->subjectName);s+="\n";
	s+=QObject::tr("Subject tag=%1").arg(this->subjectTagName);s+="\n";

	for(QStringList::Iterator it=this->equipmentsNames.begin(); it!=this->equipmentsNames.end(); it++){
		s+=QObject::tr("Equipment=%1").arg(*it); 
		s+="\n";
	}

	return s;
}

//critical function here - must be optimized for speed
int ConstraintSubjectSubjectTagRequireEquipments::fitness(
	SpaceChromosome& c,
	Rules& r,
	const int days[/*MAX_ACTIVITIES*/],
	const int hours[/*MAX_ACTIVITIES*/],
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(crt_chrom!=&c || crt_rules!=&r || rooms_conflicts<0 || c.spaceChangedForMatrixCalculation){
		rooms_conflicts = c.getRoomsMatrix(r, days, hours, roomsMatrix);

		crt_chrom = &c;
		crt_rules = &r;
		
		c.spaceChangedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and subject tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int i=0; i<this->_nActivities; i++){	
			int ac=this->_activities[i];
			int parity=r.internalActivitiesList[ac].parity==PARITY_WEEKLY?2:1;
			int rm=c.rooms[ac];
			if(rm==UNALLOCATED_SPACE)
				continue;
		
			bool ok=true;
			for(int j=0; j<this->_nEquipments; j++){
				int eq=this->_equipments[j];
				if(!r.roomHasEquipment[rm][eq]){
					ok=false;
					break;
				}
			}

			if(!ok)
				nbroken+=parity;
		}
	}
	//with logging
	else{
		nbroken=0;
		for(int i=0; i<this->_nActivities; i++){	
			int ac=this->_activities[i];
			int parity=r.internalActivitiesList[ac].parity==PARITY_WEEKLY?2:1;
			int rm=c.rooms[ac];
			if(rm==UNALLOCATED_SPACE)
				continue;
		
			bool ok=true;
			for(int j=0; j<this->_nEquipments; j++){
				int eq=this->_equipments[j];
				if(!r.roomHasEquipment[rm][eq]){
					ok=false;
					break;
				}
			}

			if(!ok){
				if(conflictsString!=NULL){
					*conflictsString+=
						(QObject::tr("Space constraint subject subject tag require equipments broken for room %1, activity with id %2")
						.arg(r.internalRoomsList[rm]->name)
						.arg(r.internalActivitiesList[ac].id));
					*conflictsString += ". ";
					*conflictsString += (QObject::tr("This increases the conflicts total by %1").arg(weight*parity));
					*conflictsString += "\n";
				}

				nbroken+=parity;
			}
		}
	}

	return int (ceil ( weight * nbroken ) );
}

bool ConstraintSubjectSubjectTagRequireEquipments::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintSubjectSubjectTagRequireEquipments::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintSubjectSubjectTagRequireEquipments::isRelatedToSubject(Subject* s)
{
	if(this->subjectName==s->name)
		return true;
	return false;
}

bool ConstraintSubjectSubjectTagRequireEquipments::isRelatedToSubjectTag(SubjectTag* s)
{
	if(this->subjectTagName==s->name)
		return true;
	return false;
}

bool ConstraintSubjectSubjectTagRequireEquipments::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

	return false;
}

bool ConstraintSubjectSubjectTagRequireEquipments::isRelatedToEquipment(Equipment* e)
{
	return this->searchRequiredEquipment(e->name);
}

bool ConstraintSubjectSubjectTagRequireEquipments::isRelatedToBuilding(Building* b)
{
	if(b)
		;

	return false;
}

bool ConstraintSubjectSubjectTagRequireEquipments::isRelatedToRoom(Room* r)
{
	if(r)
		;

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherRequiresRoom::ConstraintTeacherRequiresRoom()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHER_REQUIRES_ROOM;
}

ConstraintTeacherRequiresRoom::ConstraintTeacherRequiresRoom(double w, bool c, const QString& teach, const QString& room)
	: SpaceConstraint(w, c)
{
	this->type=CONSTRAINT_TEACHER_REQUIRES_ROOM;
	this->teacherName=teach;
	this->roomName=room;
}

bool ConstraintTeacherRequiresRoom::computeInternalStructure(Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the teacher of the constraint,	
	this->_nActivities=0;
	for(int ac=0; ac<r.nInternalActivities; ac++)
		for(QStringList::Iterator it=r.internalActivitiesList[ac].teachersNames.begin(); it!=r.internalActivitiesList[ac].teachersNames.end(); it++)
			if(*it == this->teacherName){
				assert(this->_nActivities<MAX_ACTIVITIES_FOR_A_TEACHER);
				this->_activities[this->_nActivities++]=ac;
			}
		
	this->_room = r.searchRoom(this->roomName);
	assert(this->_room>=0);
	
	return true;
}

QString ConstraintTeacherRequiresRoom::getXmlDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s="<ConstraintTeacherRequiresRoom>\n";
	s+="	<Weight>"+QString::number(weight)+"</Weight>\n";
	s+="	<Compulsory>";
	s+=yesNo(this->compulsory);
	s+="</Compulsory>\n";
	s+="	<Teacher>"+protect(this->teacherName)+"</Teacher>\n";
	s+="	<Room>"+protect(this->roomName)+"</Room>\n";
		
	s+="</ConstraintTeacherRequiresRoom>\n";

	return s;
}

QString ConstraintTeacherRequiresRoom::getDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s="Teacher requires room"; s+=", ";
	s+=QObject::tr("W:%1").arg(this->weight);s+=", ";
	s+=QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory));s+=", ";
	s+=QObject::tr("T:%1").arg(this->teacherName);s+=",";
	s+=QObject::tr("R:%1").arg(this->roomName);s+=",";

	return s;
}

QString ConstraintTeacherRequiresRoom::getDetailedDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s=QObject::tr("Space constraint"); s+="\n";
	s+=QObject::tr("Teacher requires room"); s+="\n";
	s+=QObject::tr("Weight=%1").arg(this->weight);s+="\n";
	s+=QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory));s+="\n";
	s+=QObject::tr("Teacher=%1").arg(this->teacherName);s+="\n";
	s+=QObject::tr("Room=%1").arg(this->roomName);s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int ConstraintTeacherRequiresRoom::fitness(
	SpaceChromosome& c,
	Rules& r,
	const int days[/*MAX_ACTIVITIES*/],
	const int hours[/*MAX_ACTIVITIES*/],
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(crt_chrom!=&c || crt_rules!=&r || rooms_conflicts<0 || c.spaceChangedForMatrixCalculation){
		rooms_conflicts = c.getRoomsMatrix(r, days, hours, roomsMatrix);

		crt_chrom = &c;
		crt_rules = &r;
		
		c.spaceChangedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the teacher of this constraint)
	//and find the scheduled room and check to see if this
	//room is accepted or not

	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int i=0; i<this->_nActivities; i++){
			int ac=this->_activities[i];
			int parity=r.internalActivitiesList[ac].parity==PARITY_WEEKLY?2:1;
			int rm=c.rooms[ac];
			if(rm==UNALLOCATED_SPACE)
				continue;

			if(rm!=this->_room)
				nbroken+=parity;
		}
	}
	//with logging
	else{
		nbroken=0;
		for(int i=0; i<this->_nActivities; i++){
			int ac=this->_activities[i];
			int parity=r.internalActivitiesList[ac].parity==PARITY_WEEKLY?2:1;
			int rm=c.rooms[ac];
			if(rm==UNALLOCATED_SPACE)
				continue;

			if(rm!=this->_room){
				if(conflictsString!=NULL){
					*conflictsString+=
						(QObject::tr("Space constraint teacher requires room broken for teacher %1, room %2, activity with id=%3")
						.arg(this->teacherName)
						.arg(this->roomName)
						.arg(r.internalActivitiesList[ac].id));
					*conflictsString += ". ";
					*conflictsString += (QObject::tr("This increases the conflicts total by %1").arg(weight*parity));
					*conflictsString += "\n";
				}

				nbroken+=parity;
			}
		}
	}

	return int (ceil ( weight * nbroken ) );
}

bool ConstraintTeacherRequiresRoom::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintTeacherRequiresRoom::isRelatedToTeacher(Teacher* t)
{
	if(this->teacherName==t->name)
		return true;
	return false;
}

bool ConstraintTeacherRequiresRoom::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintTeacherRequiresRoom::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintTeacherRequiresRoom::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

	return false;
}

bool ConstraintTeacherRequiresRoom::isRelatedToEquipment(Equipment* e)
{
	if(e)
		;

	return false;
}

bool ConstraintTeacherRequiresRoom::isRelatedToBuilding(Building* b)
{
	if(b)
		;

	return false;
}

bool ConstraintTeacherRequiresRoom::isRelatedToRoom(Room* r)
{
	if(r)
		;

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherSubjectRequireRoom::ConstraintTeacherSubjectRequireRoom()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHER_SUBJECT_REQUIRE_ROOM;
}

ConstraintTeacherSubjectRequireRoom::ConstraintTeacherSubjectRequireRoom(double w, bool c, const QString& teach, const QString& subj, const QString& room)
	: SpaceConstraint(w, c)
{
	this->type=CONSTRAINT_TEACHER_SUBJECT_REQUIRE_ROOM;
	this->teacherName=teach;
	this->subjectName=subj;
	this->roomName=room;
}

bool ConstraintTeacherSubjectRequireRoom::computeInternalStructure(Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the teacher of the constraint,	
	this->_nActivities=0;
	for(int ac=0; ac<r.nInternalActivities; ac++){
		if(r.internalActivitiesList[ac].subjectName!=this->subjectName)
			continue;
		for(QStringList::Iterator it=r.internalActivitiesList[ac].teachersNames.begin(); it!=r.internalActivitiesList[ac].teachersNames.end(); it++)
			if(*it == this->teacherName){
				assert(this->_nActivities<MAX_ACTIVITIES_FOR_A_TEACHER);
				this->_activities[this->_nActivities++]=ac;
			}
	}
		
	this->_room = r.searchRoom(this->roomName);
	assert(this->_room>=0);
	
	return true;
}

QString ConstraintTeacherSubjectRequireRoom::getXmlDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s="<ConstraintTeacherSubjectRequireRoom>\n";
	s+="	<Weight>"+QString::number(weight)+"</Weight>\n";
	s+="	<Compulsory>";
	s+=yesNo(this->compulsory);
	s+="</Compulsory>\n";
	s+="	<Teacher>"+protect(this->teacherName)+"</Teacher>\n";
	s+="	<Subject>"+protect(this->subjectName)+"</Subject>\n";
	s+="	<Room>"+protect(this->roomName)+"</Room>\n";
		
	s+="</ConstraintTeacherSubjectRequireRoom>\n";

	return s;
}

QString ConstraintTeacherSubjectRequireRoom::getDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s="Teacher subject requires room"; s+=", ";
	s+=QObject::tr("W:%1").arg(this->weight);s+=", ";
	s+=QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory));s+=", ";
	s+=QObject::tr("T:%1").arg(this->teacherName);s+=",";
	s+=QObject::tr("S:%1").arg(this->subjectName);s+=",";
	s+=QObject::tr("R:%1").arg(this->roomName);s+=",";

	return s;
}

QString ConstraintTeacherSubjectRequireRoom::getDetailedDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s=QObject::tr("Space constraint"); s+="\n";
	s+=QObject::tr("Teacher subject requires room"); s+="\n";
	s+=QObject::tr("Weight=%1").arg(this->weight);s+="\n";
	s+=QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory));s+="\n";
	s+=QObject::tr("Teacher=%1").arg(this->teacherName);s+="\n";
	s+=QObject::tr("Subject=%1").arg(this->subjectName);s+="\n";
	s+=QObject::tr("Room=%1").arg(this->roomName);s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int ConstraintTeacherSubjectRequireRoom::fitness(
	SpaceChromosome& c,
	Rules& r,
	const int days[/*MAX_ACTIVITIES*/],
	const int hours[/*MAX_ACTIVITIES*/],
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(crt_chrom!=&c || crt_rules!=&r || rooms_conflicts<0 || c.spaceChangedForMatrixCalculation){
		rooms_conflicts = c.getRoomsMatrix(r, days, hours, roomsMatrix);

		crt_chrom = &c;
		crt_rules = &r;
		
		c.spaceChangedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the teacher of this constraint)
	//and find the scheduled room and check to see if this
	//room is accepted or not

	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int i=0; i<this->_nActivities; i++){
			int ac=this->_activities[i];
			int parity=r.internalActivitiesList[ac].parity==PARITY_WEEKLY?2:1;
			int rm=c.rooms[ac];
			if(rm==UNALLOCATED_SPACE)
				continue;

			if(rm!=this->_room)
				nbroken+=parity;
		}
	}
	//with logging
	else{
		nbroken=0;
		for(int i=0; i<this->_nActivities; i++){
			int ac=this->_activities[i];
			int parity=r.internalActivitiesList[ac].parity==PARITY_WEEKLY?2:1;
			int rm=c.rooms[ac];
			if(rm==UNALLOCATED_SPACE)
				continue;

			if(rm!=this->_room){
				if(conflictsString!=NULL){
					*conflictsString+=
						(QObject::tr("Space constraint teacher subject requires room broken for teacher %1, subject %2, room %3, activity with id=%4")
						.arg(this->teacherName)
						.arg(this->subjectName)
						.arg(this->roomName)
						.arg(r.internalActivitiesList[ac].id));
					*conflictsString += ". ";
					*conflictsString += (QObject::tr("This increases the conflicts total by %1").arg(weight*parity));
					*conflictsString += "\n";
				}

				nbroken+=parity;
			}
		}
	}

	return int (ceil ( weight * nbroken ) );
}

bool ConstraintTeacherSubjectRequireRoom::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintTeacherSubjectRequireRoom::isRelatedToTeacher(Teacher* t)
{
	if(this->teacherName==t->name)
		return true;
	return false;
}

bool ConstraintTeacherSubjectRequireRoom::isRelatedToSubject(Subject* s)
{
	if(this->subjectName==s->name)
		return true;
	return false;
}

bool ConstraintTeacherSubjectRequireRoom::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintTeacherSubjectRequireRoom::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

	return false;
}

bool ConstraintTeacherSubjectRequireRoom::isRelatedToEquipment(Equipment* e)
{
	if(e)
		;

	return false;
}

bool ConstraintTeacherSubjectRequireRoom::isRelatedToBuilding(Building* b)
{
	if(b)
		;

	return false;
}

bool ConstraintTeacherSubjectRequireRoom::isRelatedToRoom(Room* r)
{
	if(r)
		;

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintMinimizeNumberOfRoomsForStudents::ConstraintMinimizeNumberOfRoomsForStudents()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_MINIMIZE_NUMBER_OF_ROOMS_FOR_STUDENTS;
}

ConstraintMinimizeNumberOfRoomsForStudents::ConstraintMinimizeNumberOfRoomsForStudents(double w, bool c)
	: SpaceConstraint(w, c)
{
	this->type=CONSTRAINT_MINIMIZE_NUMBER_OF_ROOMS_FOR_STUDENTS;
}

bool ConstraintMinimizeNumberOfRoomsForStudents::computeInternalStructure(Rules& r)
{
	if(&r!=NULL)
		;
	/*do nothing*/
	
	return true;
}

QString ConstraintMinimizeNumberOfRoomsForStudents::getXmlDescription(Rules& r)
{
	if(&r!=NULL)
		;

	QString s = "<ConstraintMinimizeNumberOfRoomsForStudents>\n";
	s += "	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s += "	<Compulsory>";
	s += yesNo(this->compulsory);
	s += "</Compulsory>\n";
	s += "</ConstraintMinimizeNumberOfRoomsForStudents>\n";
	return s;
}

QString ConstraintMinimizeNumberOfRoomsForStudents::getDescription(Rules& r)
{
	if(&r!=NULL)
		;

	QString s=QObject::tr("Minimize number of rooms for students");s+=",";
	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";

	return s;
}

QString ConstraintMinimizeNumberOfRoomsForStudents::getDetailedDescription(Rules& r)
{
	if(&r!=NULL)
		;

	QString s=QObject::tr("Space constraint");s+="\n";
	s+=QObject::tr("Minimize number of rooms for each students set");s+="\n";
	s+=(QObject::tr("Weight=%1").arg(this->weight));s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int ConstraintMinimizeNumberOfRoomsForStudents::fitness(
	SpaceChromosome& c,
	Rules& r,
	const int days[/*MAX_ACTIVITIES*/],
	const int hours[/*MAX_ACTIVITIES*/],
	QString* conflictsString)
	{

	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(crt_chrom!=&c || crt_rules!=&r || rooms_conflicts<0 || c.spaceChangedForMatrixCalculation){
		rooms_conflicts = c.getRoomsMatrix(r, days, hours, roomsMatrix);

		crt_chrom = &c;
		crt_rules = &r;
		
		c.spaceChangedForMatrixCalculation=false;
	}

	assert(r.internalStructureComputed);

	int nbroken=0;
	
	//without logging
	if(conflictsString==NULL){
		nbroken=0;
	
		for(int i=0; i<r.nInternalSubgroups; i++)
			for(int j=0; j<r.nInternalRooms; j++)
				subgroupsRooms[i][j]=false;
			
		for(int i=0; i<r.nInternalActivities; i++){
			if(c.rooms[i]!=UNALLOCATED_SPACE){
				for(int j=0; j<r.internalActivitiesList[i].nSubgroups; j++){
					int k=r.internalActivitiesList[i].subgroups[j];
					subgroupsRooms[k][c.rooms[i]]=true;
				}
			}
		}
	
		int total[MAX_TOTAL_SUBGROUPS];
		for(int i=0; i<r.nInternalSubgroups; i++)
			total[i]=0;
		
		for(int i=0; i<r.nInternalSubgroups; i++)
			for(int j=0; j<r.nInternalRooms; j++)
				if(subgroupsRooms[i][j])
					total[i]++;
				
		for(int i=0; i<r.nInternalSubgroups; i++)
			if(total[i]>1)
				nbroken+=total[i]-1;
	}
	//with logging
	else{
		nbroken=0;
	
		for(int i=0; i<r.nInternalSubgroups; i++)
			for(int j=0; j<r.nInternalRooms; j++)
				subgroupsRooms[i][j]=false;
			
		for(int i=0; i<r.nInternalActivities; i++){
			if(c.rooms[i]!=UNALLOCATED_SPACE){
				for(int j=0; j<r.internalActivitiesList[i].nSubgroups; j++){
					int k=r.internalActivitiesList[i].subgroups[j];
					subgroupsRooms[k][c.rooms[i]]=true;
				}
			}	
		}
	
		int total[MAX_TOTAL_SUBGROUPS];
		for(int i=0; i<r.nInternalSubgroups; i++)
			total[i]=0;
		
		for(int i=0; i<r.nInternalSubgroups; i++)
			for(int j=0; j<r.nInternalRooms; j++)
				if(subgroupsRooms[i][j])
					total[i]++;
				
		for(int i=0; i<r.nInternalSubgroups; i++)
			if(total[i]>1){
				nbroken+=total[i]-1;
	
				if(conflictsString!=NULL){
					*conflictsString+=
						QObject::tr("Space constraint minimize number of rooms for students broken for subgroup %1, number of rooms=%2")
						.arg(r.internalSubgroupsList[i]->name)
						.arg(total[i]);
					*conflictsString += ". ";
					*conflictsString += (QObject::tr("This increases the conflicts total by %1").arg(weight*(total[i]-1)));
					*conflictsString += "\n";
				}
			}
	}

	return int (ceil ( weight * nbroken ) );
}

bool ConstraintMinimizeNumberOfRoomsForStudents::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintMinimizeNumberOfRoomsForStudents::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintMinimizeNumberOfRoomsForStudents::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintMinimizeNumberOfRoomsForStudents::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintMinimizeNumberOfRoomsForStudents::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

	return true;
}

bool ConstraintMinimizeNumberOfRoomsForStudents::isRelatedToEquipment(Equipment* e)
{
	if(e)
		;

	return false;
}

bool ConstraintMinimizeNumberOfRoomsForStudents::isRelatedToBuilding(Building* b)
{
	if(b)
		;

	return false;
}

bool ConstraintMinimizeNumberOfRoomsForStudents::isRelatedToRoom(Room* r)
{
	if(r)
		;

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintMinimizeNumberOfRoomsForTeachers::ConstraintMinimizeNumberOfRoomsForTeachers()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_MINIMIZE_NUMBER_OF_ROOMS_FOR_TEACHERS;
}

ConstraintMinimizeNumberOfRoomsForTeachers::ConstraintMinimizeNumberOfRoomsForTeachers(double w, bool c)
	: SpaceConstraint(w, c)
{
	this->type=CONSTRAINT_MINIMIZE_NUMBER_OF_ROOMS_FOR_TEACHERS;
}

bool ConstraintMinimizeNumberOfRoomsForTeachers::computeInternalStructure(Rules& r)
{
	if(&r!=NULL)
		;
	/*do nothing*/
	
	return true;
}

QString ConstraintMinimizeNumberOfRoomsForTeachers::getXmlDescription(Rules& r)
{
	if(&r!=NULL)
		;

	QString s = "<ConstraintMinimizeNumberOfRoomsForTeachers>\n";
	s += "	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s += "	<Compulsory>";
	s += yesNo(this->compulsory);
	s += "</Compulsory>\n";
	s += "</ConstraintMinimizeNumberOfRoomsForTeachers>\n";
	return s;
}

QString ConstraintMinimizeNumberOfRoomsForTeachers::getDescription(Rules& r)
{
	if(&r!=NULL)
		;

	QString s=QObject::tr("Minimize number of rooms for teachers");s+=",";
	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";

	return s;
}

QString ConstraintMinimizeNumberOfRoomsForTeachers::getDetailedDescription(Rules& r)
{
	if(&r!=NULL)
		;

	QString s=QObject::tr("Space constraint");s+="\n";
	s+=QObject::tr("Minimize number of rooms for each teacher");s+="\n";
	s+=(QObject::tr("Weight=%1").arg(this->weight));s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int ConstraintMinimizeNumberOfRoomsForTeachers::fitness(
	SpaceChromosome& c,
	Rules& r,
	const int days[/*MAX_ACTIVITIES*/],
	const int hours[/*MAX_ACTIVITIES*/],
	QString* conflictsString)
	{

	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(crt_chrom!=&c || crt_rules!=&r || rooms_conflicts<0 || c.spaceChangedForMatrixCalculation){
		rooms_conflicts = c.getRoomsMatrix(r, days, hours, roomsMatrix);

		crt_chrom = &c;
		crt_rules = &r;
		
		c.spaceChangedForMatrixCalculation=false;
	}

	assert(r.internalStructureComputed);

	int nbroken=0;
	
	//without logging
	if(conflictsString==NULL){
		nbroken=0;
	
		for(int i=0; i<r.nInternalTeachers; i++)
			for(int j=0; j<r.nInternalRooms; j++)
				teachersRooms[i][j]=false;
			
		for(int i=0; i<r.nInternalActivities; i++){
			if(c.rooms[i]!=UNALLOCATED_SPACE){
				for(int j=0; j<r.internalActivitiesList[i].nTeachers; j++){
					int k=r.internalActivitiesList[i].teachers[j];
					teachersRooms[k][c.rooms[i]]=true;
				}
			}	
		}
	
		int total[MAX_TEACHERS];
		for(int i=0; i<r.nInternalTeachers; i++)
			total[i]=0;
		
		for(int i=0; i<r.nInternalTeachers; i++)
			for(int j=0; j<r.nInternalRooms; j++)
				if(teachersRooms[i][j])
					total[i]++;
				
		for(int i=0; i<r.nInternalTeachers; i++)
			if(total[i]>1)
				nbroken+=total[i]-1;
	}
	//with logging
	else{
		nbroken=0;
	
		for(int i=0; i<r.nInternalTeachers; i++)
			for(int j=0; j<r.nInternalRooms; j++)
				teachersRooms[i][j]=false;
			
		for(int i=0; i<r.nInternalActivities; i++){
			if(c.rooms[i]!=UNALLOCATED_SPACE){
				for(int j=0; j<r.internalActivitiesList[i].nTeachers; j++){
					int k=r.internalActivitiesList[i].teachers[j];
					teachersRooms[k][c.rooms[i]]=true;
				}
			}	
		}
	
		int total[MAX_TEACHERS];
		for(int i=0; i<r.nInternalTeachers; i++)
			total[i]=0;
		
		for(int i=0; i<r.nInternalTeachers; i++)
			for(int j=0; j<r.nInternalRooms; j++)
				if(teachersRooms[i][j])
					total[i]++;
				
		for(int i=0; i<r.nInternalTeachers; i++)
			if(total[i]>1){
				nbroken+=total[i]-1;
	
				if(conflictsString!=NULL){
					*conflictsString+=
						QObject::tr("Space constraint minimize number of rooms for teachers broken for teacher %1, number of rooms=%2")
						.arg(r.internalTeachersList[i]->name)
						.arg(total[i]);
					*conflictsString += ". ";
					*conflictsString += (QObject::tr("This increases the conflicts total by %1").arg(weight*(total[i]-1)));
					*conflictsString += "\n";
				}
			}
	}

	return int (ceil ( weight * nbroken ) );
}

bool ConstraintMinimizeNumberOfRoomsForTeachers::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintMinimizeNumberOfRoomsForTeachers::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintMinimizeNumberOfRoomsForTeachers::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintMinimizeNumberOfRoomsForTeachers::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return true;
}

bool ConstraintMinimizeNumberOfRoomsForTeachers::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

	return false;
}

bool ConstraintMinimizeNumberOfRoomsForTeachers::isRelatedToEquipment(Equipment* e)
{
	if(e)
		;

	return false;
}

bool ConstraintMinimizeNumberOfRoomsForTeachers::isRelatedToBuilding(Building* b)
{
	if(b)
		;

	return false;
}

bool ConstraintMinimizeNumberOfRoomsForTeachers::isRelatedToRoom(Room* r)
{
	if(r)
		;

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivityPreferredRoom::ConstraintActivityPreferredRoom()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_ACTIVITY_PREFERRED_ROOM;
}

ConstraintActivityPreferredRoom::ConstraintActivityPreferredRoom(double w, bool c, int aid, const QString& room)
	: SpaceConstraint(w, c)
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
	if(this->compulsory!=c.compulsory)
		return false;
	if(this->weight!=c.weight)
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
	if(ac==r.nInternalActivities)
		return false;
		
	this->_room = r.searchRoom(this->roomName);
	assert(this->_room>=0);
	
	return true;
}

QString ConstraintActivityPreferredRoom::getXmlDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s="<ConstraintActivityPreferredRoom>\n";
	s+="	<Weight>"+QString::number(weight)+"</Weight>\n";
	s+="	<Compulsory>";
	s+=yesNo(this->compulsory);
	s+="</Compulsory>\n";
	s+="	<Activity_Id>"+QString::number(this->activityId)+"</Activity_Id>\n";
	s+="	<Room>"+protect(this->roomName)+"</Room>\n";
		
	s+="</ConstraintActivityPreferredRoom>\n";

	return s;
}

QString ConstraintActivityPreferredRoom::getDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s="Activity preferred room"; s+=", ";
	s+=QObject::tr("W:%1").arg(this->weight);s+=", ";
	s+=QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory));s+=", ";
	s+=QObject::tr("A:%1").arg(this->activityId);s+=", ";
	s+=QObject::tr("R:%1").arg(this->roomName);

	return s;
}

QString ConstraintActivityPreferredRoom::getDetailedDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s=QObject::tr("Space constraint"); s+="\n";
	s+=QObject::tr("Activity preferred room"); s+="\n";
	s+=QObject::tr("Weight=%1").arg(this->weight);s+="\n";
	s+=QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory));s+="\n";
	s+=QObject::tr("Activity id=%1").arg(this->activityId);s+="\n";
	s+=QObject::tr("Room=%1").arg(this->roomName);s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int ConstraintActivityPreferredRoom::fitness(
	SpaceChromosome& c,
	Rules& r,
	const int days[/*MAX_ACTIVITIES*/],
	const int hours[/*MAX_ACTIVITIES*/],
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(crt_chrom!=&c || crt_rules!=&r || rooms_conflicts<0 || c.spaceChangedForMatrixCalculation){
		rooms_conflicts = c.getRoomsMatrix(r, days, hours, roomsMatrix);

		crt_chrom = &c;
		crt_rules = &r;
		
		c.spaceChangedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts

	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		int parity=r.internalActivitiesList[this->_activity].parity==PARITY_WEEKLY?2:1;

		int rm=c.rooms[this->_activity];
		if(rm!=UNALLOCATED_SPACE && rm!=this->_room)
			nbroken+=parity;
	}
		//with logging
	else{
		nbroken=0;
		int parity=r.internalActivitiesList[this->_activity].parity==PARITY_WEEKLY?2:1;

		int rm=c.rooms[this->_activity];
		if(rm!=UNALLOCATED_SPACE && rm!=this->_room){
			if(conflictsString!=NULL){
				*conflictsString+=
					(QObject::tr("Space constraint activity preferred room broken for activity with id=%1, room=%2")
					.arg(this->activityId)
					.arg(this->roomName));
				*conflictsString += ". ";
				*conflictsString += (QObject::tr("This increases the conflicts total by %1").arg(weight*parity));
				*conflictsString += "\n";
			}

			nbroken+=parity;
		}
	}

	return int (ceil ( weight * nbroken ) );
}

bool ConstraintActivityPreferredRoom::isRelatedToActivity(Activity* a)
{
	if(this->activityId==a->id)
		return true;
	return false;
}

bool ConstraintActivityPreferredRoom::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintActivityPreferredRoom::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintActivityPreferredRoom::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintActivityPreferredRoom::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

	return false;
}

bool ConstraintActivityPreferredRoom::isRelatedToEquipment(Equipment* e)
{
	if(e)
		;

	return false;
}

bool ConstraintActivityPreferredRoom::isRelatedToBuilding(Building* b)
{
	if(b)
		;

	return false;
}

bool ConstraintActivityPreferredRoom::isRelatedToRoom(Room* r)
{
	if(r)
		;

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivityPreferredRooms::ConstraintActivityPreferredRooms()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_ACTIVITY_PREFERRED_ROOMS;
}

ConstraintActivityPreferredRooms::ConstraintActivityPreferredRooms(double w, bool c, int aid, const QStringList& roomsList)
	: SpaceConstraint(w, c)
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
		
	if(ac==r.nInternalActivities)
		return false;
		
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
	if(&r!=NULL)
		;

	QString s="<ConstraintActivityPreferredRooms>\n";
	s+="	<Weight>"+QString::number(weight)+"</Weight>\n";
	s+="	<Compulsory>";
	s+=yesNo(this->compulsory);
	s+="</Compulsory>\n";
	s+="	<Activity_Id>"+QString::number(this->activityId)+"</Activity_Id>\n";
	s+="	<Number_of_Preferred_Rooms>"+QString::number(this->roomsNames.count())+"</Number_of_Preferred_Rooms>\n";
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++)
		s+="	<Preferred_Room>"+protect(*it)+"</Preferred_Room>\n";
		
	s+="</ConstraintActivityPreferredRooms>\n";

	return s;
}

QString ConstraintActivityPreferredRooms::getDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s="Activity preferred rooms"; s+=", ";
	s+=QObject::tr("W:%1").arg(this->weight);s+=", ";
	s+=QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory));s+=", ";
	s+=QObject::tr("A:%1").arg(this->activityId);
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		s+=", ";
		s+=QObject::tr("R:%1").arg(*it);
	}

	return s;
}

QString ConstraintActivityPreferredRooms::getDetailedDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s=QObject::tr("Space constraint"); s+="\n";
	s+=QObject::tr("Activity preferred rooms"); s+="\n";
	s+=QObject::tr("Weight=%1").arg(this->weight);s+="\n";
	s+=QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory));s+="\n";
	s+=QObject::tr("Activity id=%1").arg(this->activityId);s+="\n";
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		s+=QObject::tr("Room=%1").arg(*it);
		s+="\n";
	}

	return s;
}

//critical function here - must be optimized for speed
int ConstraintActivityPreferredRooms::fitness(
	SpaceChromosome& c,
	Rules& r,
	const int days[/*MAX_ACTIVITIES*/],
	const int hours[/*MAX_ACTIVITIES*/],
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(crt_chrom!=&c || crt_rules!=&r || rooms_conflicts<0 || c.spaceChangedForMatrixCalculation){
		rooms_conflicts = c.getRoomsMatrix(r, days, hours, roomsMatrix);

		crt_chrom = &c;
		crt_rules = &r;
		
		c.spaceChangedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts

	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		int parity=r.internalActivitiesList[this->_activity].parity==PARITY_WEEKLY?2:1;

		int rm=c.rooms[this->_activity];
		if(rm!=UNALLOCATED_SPACE){
			int i;
			for(i=0; i<this->_n_preferred_rooms; i++)
				if(this->_rooms[i]==rm)
					break;
			if(i==this->_n_preferred_rooms)
				nbroken+=parity;
		}
	}
	//with logging
	else{
		nbroken=0;
		int parity=r.internalActivitiesList[this->_activity].parity==PARITY_WEEKLY?2:1;

		int rm=c.rooms[this->_activity];
		if(rm!=UNALLOCATED_SPACE){
			int i;
			for(i=0; i<this->_n_preferred_rooms; i++)
				if(this->_rooms[i]==rm)
					break;
			if(i==this->_n_preferred_rooms){
				if(conflictsString!=NULL){
					*conflictsString+=
						(QObject::tr("Space constraint activity preferred rooms broken for activity with id=%1")
						.arg(this->activityId));
					*conflictsString += ". ";
					*conflictsString += (QObject::tr("This increases the conflicts total by %1").arg(weight*parity));
					*conflictsString += "\n";
				}

				nbroken+=parity;
			}
		}
	}

	return int (ceil ( weight * nbroken ) );
}

bool ConstraintActivityPreferredRooms::isRelatedToActivity(Activity* a)
{
	if(this->activityId==a->id)
		return true;
	return false;
}

bool ConstraintActivityPreferredRooms::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintActivityPreferredRooms::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintActivityPreferredRooms::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintActivityPreferredRooms::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

	return false;
}

bool ConstraintActivityPreferredRooms::isRelatedToEquipment(Equipment* e)
{
	if(e)
		;

	return false;
}

bool ConstraintActivityPreferredRooms::isRelatedToBuilding(Building* b)
{
	if(b)
		;

	return false;
}

bool ConstraintActivityPreferredRooms::isRelatedToRoom(Room* r)
{
	if(r)
		;

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivitiesSameRoom::ConstraintActivitiesSameRoom()
	: SpaceConstraint()
{
	type=CONSTRAINT_ACTIVITIES_SAME_ROOM;
}

ConstraintActivitiesSameRoom::ConstraintActivitiesSameRoom(double w, bool c, int nact, const int act[])
 : SpaceConstraint(w, c)
 {
	assert(nact>=2 && nact<=MAX_CONSTRAINT_ACTIVITIES_SAME_ROOM);
	this->n_activities=nact;
	for(int i=0; i<nact; i++)
		this->activitiesId[i]=act[i];

	this->type=CONSTRAINT_ACTIVITIES_SAME_ROOM;
}

bool ConstraintActivitiesSameRoom::computeInternalStructure(Rules &r)
{
	//compute the indices of the activities,
	//based on their unique ID

	for(int j=0; j<n_activities; j++)
		this->_activities[j]=-1;

	this->_n_activities=0;
	for(int i=0; i<this->n_activities; i++){
		int j;
		Activity* act;
		for(j=0; j<r.nInternalActivities; j++){
			act=&r.internalActivitiesList[j];
			if(act->id==this->activitiesId[i]){
				this->_activities[this->_n_activities++]=j;
				break;
			}
		}
	}
	
	if(this->_n_activities<=1)
		return false;
		
	return true;
}

void ConstraintActivitiesSameRoom::removeUseless(Rules& r)
{
	//remove the activitiesId which no longer exist (used after the deletion of an activity)

	for(int i=0; i<this->n_activities; i++)
		this->_activities[i]=-1;

	for(int i=0; i<this->n_activities; i++){
		for(int k=0; k<r.activitiesList.size(); k++){
			Activity* act=r.activitiesList[k];
			if(act->id==this->activitiesId[i])
				this->_activities[i]=act->id;
		}
	}

	int i, j;
	i=0;
	for(j=0; j<this->n_activities; j++)
		if(this->_activities[j]>=0) //valid activity
			this->activitiesId[i++]=this->_activities[j];
	this->n_activities=i;
}

QString ConstraintActivitiesSameRoom::getXmlDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s="<ConstraintActivitiesSameRoom>\n";
	s+="	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Number_of_Activities>"+QString::number(this->n_activities)+"</Number_of_Activities>\n";
	for(int i=0; i<this->n_activities; i++)
		s+="	<Activity_Id>"+QString::number(this->activitiesId[i])+"</Activity_Id>\n";
	s+="</ConstraintActivitiesSameRoom>\n";
	return s;
}

QString ConstraintActivitiesSameRoom::getDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s;
	s+=QObject::tr("Activities same room");s+=", ";
	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
	s+=(QObject::tr("NA:%1").arg(this->n_activities));s+=", ";
	for(int i=0; i<this->n_activities; i++){
		s+=(QObject::tr("ID:%1").arg(this->activitiesId[i]));s+=", ";
	}

	return s;
}

QString ConstraintActivitiesSameRoom::getDetailedDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s;
	
	s=QObject::tr("Space constraint");s+="\n";
	s+=QObject::tr("Activities must have the same room");s+="\n";
	s+=(QObject::tr("Weight=%1").arg(this->weight));s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	s+=(QObject::tr("Number of activities=%1").arg(this->n_activities));s+="\n";
	for(int i=0; i<this->n_activities; i++){
		s+=(QObject::tr("Activity with id=%1").arg(this->activitiesId[i]));s+="\n";
	}

	return s;
}

//critical function here - must be optimized for speed
int ConstraintActivitiesSameRoom::fitness(
	SpaceChromosome& c,
	Rules& r,
	const int days[/*MAX_ACTIVITIES*/],
	const int hours[/*MAX_ACTIVITIES*/],
	QString* conflictsString)
{
	assert(r.internalStructureComputed);

	//if the matrices roomsMatrix is already calculated, do not calculate it again!
	if(crt_chrom!=&c || crt_rules!=&r || rooms_conflicts<0 || c.spaceChangedForMatrixCalculation){
		rooms_conflicts = c.getRoomsMatrix(r, days, hours, roomsMatrix);

		crt_chrom = &c;
		crt_rules = &r;
		
		c.spaceChangedForMatrixCalculation=false;
	}

	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int i=1; i<this->_n_activities; i++){
			int t1=c.rooms[this->_activities[i]];
			if(t1!=UNALLOCATED_SPACE){
				for(int j=0; j<i; j++){
					int t2=c.rooms[this->_activities[j]];
					if(t2!=UNALLOCATED_SPACE){
						int tmp=0;

						//activity weekly - counts as double
						if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY &&
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 4;
						else if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY ||
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 2;
						else
							tmp = 1;

						nbroken+=tmp;
					}
				}
			}
		}
	}
	//with logging
	else{
		nbroken=0;
		for(int i=1; i<this->_n_activities; i++){
			int t1=c.rooms[this->_activities[i]];
			if(t1!=UNALLOCATED_SPACE){
				for(int j=0; j<i; j++){
					int t2=c.rooms[this->_activities[j]];
					if(t2!=UNALLOCATED_SPACE){
						int tmp=0;

						//activity weekly - counts as double
						if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY &&
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 4;
						else if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY ||
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 2;
						else
							tmp = 1;

						nbroken+=tmp;

						if(tmp>0 && conflictsString!=NULL){
							*conflictsString+=(QObject::tr("Space constraint activities same room broken, because activity with id=%1 is not in the same room with activity with id=%2")
								.arg(this->activitiesId[i])
								.arg(this->activitiesId[j]));
							*conflictsString+=", ";
							*conflictsString+=(QObject::tr("conflicts factor increase=%1").arg(tmp*weight));
							*conflictsString+="\n";
						}
					}
				}
			}
		}
	}

	return int (ceil ( weight * nbroken ) );
}

bool ConstraintActivitiesSameRoom::isRelatedToActivity(Activity* a)
{
	for(int i=0; i<this->n_activities; i++)
		if(this->activitiesId[i]==a->id)
			return true;
	return false;
}

bool ConstraintActivitiesSameRoom::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintActivitiesSameRoom::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintActivitiesSameRoom::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintActivitiesSameRoom::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

	return false;
}

bool ConstraintActivitiesSameRoom::isRelatedToEquipment(Equipment* e)
{
	if(e)
		;

	return false;
}

bool ConstraintActivitiesSameRoom::isRelatedToBuilding(Building* b)
{
	if(b)
		;

	return false;
}

bool ConstraintActivitiesSameRoom::isRelatedToRoom(Room* r)
{
	if(r)
		;

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintSubjectSubjectTagPreferredRoom::ConstraintSubjectSubjectTagPreferredRoom()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOM;
}

ConstraintSubjectSubjectTagPreferredRoom::ConstraintSubjectSubjectTagPreferredRoom(double w, bool c, const QString& subj, const QString& subt, const QString& rm)
	: SpaceConstraint(w, c)
{
	this->type=CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOM;
	this->subjectName=subj;
	this->subjectTagName=subt;
	this->roomName=rm;
}

bool ConstraintSubjectSubjectTagPreferredRoom::computeInternalStructure(Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the subject of the constraint.
	
	this->_nActivities=0;
	for(int ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].subjectName == this->subjectName
		 &&r.internalActivitiesList[ac].subjectTagName==this->subjectTagName){
			assert(this->_nActivities<MAX_ACTIVITIES_FOR_A_SUBJECT);
			this->_activities[this->_nActivities++]=ac;
		}
		
	this->_room = r.searchRoom(this->roomName);
	assert(this->_room>=0);
	
	return true;
}

QString ConstraintSubjectSubjectTagPreferredRoom::getXmlDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s="<ConstraintSubjectSubjectTagPreferredRoom>\n";
	s+="	<Weight>"+QString::number(weight)+"</Weight>\n";
	s+="	<Compulsory>";
	s+=yesNo(this->compulsory);
	s+="</Compulsory>\n";
	s+="	<Subject>"+protect(this->subjectName)+"</Subject>\n";
	s+="	<Subject_Tag>"+protect(this->subjectTagName)+"</Subject_Tag>\n";
	s+="	<Room>"+protect(this->roomName)+"</Room>\n";
		
	s+="</ConstraintSubjectSubjectTagPreferredRoom>\n";

	return s;
}

QString ConstraintSubjectSubjectTagPreferredRoom::getDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s="Subject subject tag preferred room"; s+=", ";
	s+=QObject::tr("W:%1").arg(this->weight);s+=", ";
	s+=QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory));s+=", ";
	s+=QObject::tr("S:%1").arg(this->subjectName);s+=",";
	s+=QObject::tr("ST:%1").arg(this->subjectTagName);s+=",";
	s+=QObject::tr("R:%1").arg(this->roomName);

	return s;
}

QString ConstraintSubjectSubjectTagPreferredRoom::getDetailedDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s=QObject::tr("Space constraint"); s+="\n";
	s+=QObject::tr("Subject subject tag preferred room"); s+="\n";
	s+=QObject::tr("Weight=%1").arg(this->weight);s+="\n";
	s+=QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory));s+="\n";
	s+=QObject::tr("Subject=%1").arg(this->subjectName);s+="\n";
	s+=QObject::tr("Subject tag=%1").arg(this->subjectTagName);s+="\n";
	s+=QObject::tr("Room name=%1").arg(this->roomName);s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int ConstraintSubjectSubjectTagPreferredRoom::fitness(
	SpaceChromosome& c,
	Rules& r,
	const int days[/*MAX_ACTIVITIES*/],
	const int hours[/*MAX_ACTIVITIES*/],
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(crt_chrom!=&c || crt_rules!=&r || rooms_conflicts<0 || c.spaceChangedForMatrixCalculation){
		rooms_conflicts = c.getRoomsMatrix(r, days, hours, roomsMatrix);

		crt_chrom = &c;
		crt_rules = &r;
		
		c.spaceChangedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and subject tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int i=0; i<this->_nActivities; i++){	
			int ac=this->_activities[i];
			int parity=r.internalActivitiesList[ac].parity==PARITY_WEEKLY?2:1;
			int rm=c.rooms[ac];
			if(rm==UNALLOCATED_SPACE)
				continue;
		
			bool ok=true;
			if(rm!=this->_room)
				ok=false;
	
			if(!ok)
				nbroken+=parity;
		}
	}
	//with logging
	else{
		nbroken=0;
		for(int i=0; i<this->_nActivities; i++){	
			int ac=this->_activities[i];
			int parity=r.internalActivitiesList[ac].parity==PARITY_WEEKLY?2:1;
			int rm=c.rooms[ac];
			if(rm==UNALLOCATED_SPACE)
				continue;
		
			bool ok=true;
			if(rm!=this->_room)
				ok=false;

			if(!ok){
				if(conflictsString!=NULL){
					*conflictsString+=
						(QObject::tr("Space constraint subject subject tag preferred room broken for activity with id %1")
						.arg(r.internalActivitiesList[ac].id));
					*conflictsString += ". ";
					*conflictsString += (QObject::tr("This increases the conflicts total by %1").arg(weight*parity));
					*conflictsString += "\n";
				}

				nbroken+=parity;
			}
		}
	}

	return int (ceil ( weight * nbroken ) );
}

bool ConstraintSubjectSubjectTagPreferredRoom::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintSubjectSubjectTagPreferredRoom::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

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
	if(s)
		;
	if(&r)
		;

	return false;
}

bool ConstraintSubjectSubjectTagPreferredRoom::isRelatedToEquipment(Equipment* e)
{
	if(e)
		;

	return false;
}

bool ConstraintSubjectSubjectTagPreferredRoom::isRelatedToBuilding(Building* b)
{
	if(b)
		;

	return false;
}

bool ConstraintSubjectSubjectTagPreferredRoom::isRelatedToRoom(Room* r)
{
	if(r)
		;

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintSubjectSubjectTagPreferredRooms::ConstraintSubjectSubjectTagPreferredRooms()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOMS;
}

ConstraintSubjectSubjectTagPreferredRooms::ConstraintSubjectSubjectTagPreferredRooms(double w, bool c, const QString& subj, const QString& subt, const QStringList& rms)
	: SpaceConstraint(w, c)
{
	this->type=CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOMS;
	this->subjectName=subj;
	this->subjectTagName=subt;
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
		 &&r.internalActivitiesList[ac].subjectTagName==this->subjectTagName){
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

QString ConstraintSubjectSubjectTagPreferredRooms::getXmlDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s="<ConstraintSubjectSubjectTagPreferredRooms>\n";
	s+="	<Weight>"+QString::number(weight)+"</Weight>\n";
	s+="	<Compulsory>";
	s+=yesNo(this->compulsory);
	s+="</Compulsory>\n";
	s+="	<Subject>"+protect(this->subjectName)+"</Subject>\n";
	s+="	<Subject_Tag>"+protect(this->subjectTagName)+"</Subject_Tag>\n";
	s+="	<Number_of_Preferred_Rooms>"+QString::number(this->roomsNames.count())+"</Number_of_Preferred_Rooms>\n";
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++)
		s+="	<Preferred_Room>"+protect(*it)+"</Preferred_Room>\n";
		
	s+="</ConstraintSubjectSubjectTagPreferredRooms>\n";

	return s;
}

QString ConstraintSubjectSubjectTagPreferredRooms::getDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s="Subject subject tag preferred rooms"; s+=", ";
	s+=QObject::tr("W:%1").arg(this->weight);s+=", ";
	s+=QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory));s+=", ";
	s+=QObject::tr("S:%1").arg(this->subjectName);s+=",";
	s+=QObject::tr("ST:%1").arg(this->subjectTagName);
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		s+=", ";
		s+=QObject::tr("R:%1").arg(*it);
	}

	return s;
}

QString ConstraintSubjectSubjectTagPreferredRooms::getDetailedDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s=QObject::tr("Space constraint"); s+="\n";
	s+=QObject::tr("Subject subject tag preferred rooms"); s+="\n";
	s+=QObject::tr("Weight=%1").arg(this->weight);s+="\n";
	s+=QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory));s+="\n";
	s+=QObject::tr("Subject=%1").arg(this->subjectName);s+="\n";
	s+=QObject::tr("Subject tag=%1").arg(this->subjectTagName);s+="\n";
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		s+=QObject::tr("Room=%1").arg(*it);
		s+="\n";
	}

	return s;
}

//critical function here - must be optimized for speed
int ConstraintSubjectSubjectTagPreferredRooms::fitness(
	SpaceChromosome& c,
	Rules& r,
	const int days[/*MAX_ACTIVITIES*/],
	const int hours[/*MAX_ACTIVITIES*/],
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(crt_chrom!=&c || crt_rules!=&r || rooms_conflicts<0 || c.spaceChangedForMatrixCalculation){
		rooms_conflicts = c.getRoomsMatrix(r, days, hours, roomsMatrix);

		crt_chrom = &c;
		crt_rules = &r;
		
		c.spaceChangedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and subject tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int i=0; i<this->_nActivities; i++){	
			int ac=this->_activities[i];
			int parity=r.internalActivitiesList[ac].parity==PARITY_WEEKLY?2:1;
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

			if(!ok)
				nbroken+=parity;
		}
	}
	//with logging
	else{
		nbroken=0;
		for(int i=0; i<this->_nActivities; i++){	
			int ac=this->_activities[i];
			int parity=r.internalActivitiesList[ac].parity==PARITY_WEEKLY?2:1;
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
				if(conflictsString!=NULL){
					*conflictsString+=
						(QObject::tr("Space constraint subject subject tag preferred room broken for activity with id %1")
						.arg(r.internalActivitiesList[ac].id));
					*conflictsString += ". ";
					*conflictsString += (QObject::tr("This increases the conflicts total by %1").arg(weight*parity));
					*conflictsString += "\n";
				}

				nbroken+=parity;
			}
		}
	}

	return int (ceil ( weight * nbroken ) );
}

bool ConstraintSubjectSubjectTagPreferredRooms::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintSubjectSubjectTagPreferredRooms::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

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
	if(s)
		;
	if(&r)
		;

	return false;
}

bool ConstraintSubjectSubjectTagPreferredRooms::isRelatedToEquipment(Equipment* e)
{
	if(e)
		;

	return false;
}

bool ConstraintSubjectSubjectTagPreferredRooms::isRelatedToBuilding(Building* b)
{
	if(b)
		;

	return false;
}

bool ConstraintSubjectSubjectTagPreferredRooms::isRelatedToRoom(Room* r)
{
	if(r)
		;

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintSubjectPreferredRoom::ConstraintSubjectPreferredRoom()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_SUBJECT_PREFERRED_ROOM;
}

ConstraintSubjectPreferredRoom::ConstraintSubjectPreferredRoom(double w, bool c, const QString& subj, const QString& rm)
	: SpaceConstraint(w, c)
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
	if(&r!=NULL)
		;

	QString s="<ConstraintSubjectPreferredRoom>\n";
	s+="	<Weight>"+QString::number(weight)+"</Weight>\n";
	s+="	<Compulsory>";
	s+=yesNo(this->compulsory);
	s+="</Compulsory>\n";
	s+="	<Subject>"+protect(this->subjectName)+"</Subject>\n";
	s+="	<Room>"+protect(this->roomName)+"</Room>\n";
		
	s+="</ConstraintSubjectPreferredRoom>\n";

	return s;
}

QString ConstraintSubjectPreferredRoom::getDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s="Subject preferred room"; s+=", ";
	s+=QObject::tr("W:%1").arg(this->weight);s+=", ";
	s+=QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory));s+=", ";
	s+=QObject::tr("S:%1").arg(this->subjectName);s+=",";
	s+=QObject::tr("R:%1").arg(this->roomName);

	return s;
}

QString ConstraintSubjectPreferredRoom::getDetailedDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s=QObject::tr("Space constraint"); s+="\n";
	s+=QObject::tr("Subject preferred room"); s+="\n";
	s+=QObject::tr("Weight=%1").arg(this->weight);s+="\n";
	s+=QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory));s+="\n";
	s+=QObject::tr("Subject=%1").arg(this->subjectName);s+="\n";
	s+=QObject::tr("Room name=%1").arg(this->roomName);s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int ConstraintSubjectPreferredRoom::fitness(
	SpaceChromosome& c,
	Rules& r,
	const int days[/*MAX_ACTIVITIES*/],
	const int hours[/*MAX_ACTIVITIES*/],
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(crt_chrom!=&c || crt_rules!=&r || rooms_conflicts<0 || c.spaceChangedForMatrixCalculation){
		rooms_conflicts = c.getRoomsMatrix(r, days, hours, roomsMatrix);

		crt_chrom = &c;
		crt_rules = &r;
		
		c.spaceChangedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and subject tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int i=0; i<this->_nActivities; i++){	
			int ac=this->_activities[i];
			int parity=r.internalActivitiesList[ac].parity==PARITY_WEEKLY?2:1;
			int rm=c.rooms[ac];
			if(rm==UNALLOCATED_SPACE)
				continue;
		
			bool ok=true;
			if(rm!=this->_room)
				ok=false;
	
			if(!ok)
				nbroken+=parity;
		}
	}
	//with logging
	else{
		nbroken=0;
		for(int i=0; i<this->_nActivities; i++){	
			int ac=this->_activities[i];
			int parity=r.internalActivitiesList[ac].parity==PARITY_WEEKLY?2:1;
			int rm=c.rooms[ac];
			if(rm==UNALLOCATED_SPACE)
				continue;
		
			bool ok=true;
			if(rm!=this->_room)
				ok=false;

			if(!ok){
				if(conflictsString!=NULL){
					*conflictsString+=
						(QObject::tr("Space constraint subject preferred room broken for activity with id %1")
						.arg(r.internalActivitiesList[ac].id));
					*conflictsString += ". ";
					*conflictsString += (QObject::tr("This increases the conflicts total by %1").arg(weight*parity));
					*conflictsString += "\n";
				}

				nbroken+=parity;
			}
		}
	}

	return int (ceil ( weight * nbroken ) );
}

bool ConstraintSubjectPreferredRoom::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintSubjectPreferredRoom::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

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
	if(s)
		;

	return false;
}

bool ConstraintSubjectPreferredRoom::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

	return false;
}

bool ConstraintSubjectPreferredRoom::isRelatedToEquipment(Equipment* e)
{
	if(e)
		;

	return false;
}

bool ConstraintSubjectPreferredRoom::isRelatedToBuilding(Building* b)
{
	if(b)
		;

	return false;
}

bool ConstraintSubjectPreferredRoom::isRelatedToRoom(Room* r)
{
	if(r)
		;

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintSubjectPreferredRooms::ConstraintSubjectPreferredRooms()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_SUBJECT_PREFERRED_ROOMS;
}

ConstraintSubjectPreferredRooms::ConstraintSubjectPreferredRooms(double w, bool c, const QString& subj, const QStringList& rms)
	: SpaceConstraint(w, c)
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
	if(&r!=NULL)
		;

	QString s="<ConstraintSubjectPreferredRooms>\n";
	s+="	<Weight>"+QString::number(weight)+"</Weight>\n";
	s+="	<Compulsory>";
	s+=yesNo(this->compulsory);
	s+="</Compulsory>\n";
	s+="	<Subject>"+protect(this->subjectName)+"</Subject>\n";
	s+="	<Number_of_Preferred_Rooms>"+QString::number(this->roomsNames.count())+"</Number_of_Preferred_Rooms>\n";
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++)
		s+="	<Preferred_Room>"+protect(*it)+"</Preferred_Room>\n";
		
	s+="</ConstraintSubjectPreferredRooms>\n";

	return s;
}

QString ConstraintSubjectPreferredRooms::getDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s="Subject preferred rooms"; s+=", ";
	s+=QObject::tr("W:%1").arg(this->weight);s+=", ";
	s+=QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory));s+=", ";
	s+=QObject::tr("S:%1").arg(this->subjectName);
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		s+=", ";
		s+=QObject::tr("R:%1").arg(*it);
	}

	return s;
}

QString ConstraintSubjectPreferredRooms::getDetailedDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s=QObject::tr("Space constraint"); s+="\n";
	s+=QObject::tr("Subject preferred rooms"); s+="\n";
	s+=QObject::tr("Weight=%1").arg(this->weight);s+="\n";
	s+=QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory));s+="\n";
	s+=QObject::tr("Subject=%1").arg(this->subjectName);s+="\n";
	for(QStringList::Iterator it=this->roomsNames.begin(); it!=this->roomsNames.end(); it++){
		s+=QObject::tr("Room=%1").arg(*it);
		s+="\n";
	}

	return s;
}

//critical function here - must be optimized for speed
int ConstraintSubjectPreferredRooms::fitness(
	SpaceChromosome& c,
	Rules& r,
	const int days[/*MAX_ACTIVITIES*/],
	const int hours[/*MAX_ACTIVITIES*/],
	QString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(crt_chrom!=&c || crt_rules!=&r || rooms_conflicts<0 || c.spaceChangedForMatrixCalculation){
		rooms_conflicts = c.getRoomsMatrix(r, days, hours, roomsMatrix);

		crt_chrom = &c;
		crt_rules = &r;
		
		c.spaceChangedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and subject tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int i=0; i<this->_nActivities; i++){	
			int ac=this->_activities[i];
			int parity=r.internalActivitiesList[ac].parity==PARITY_WEEKLY?2:1;
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

			if(!ok)
				nbroken+=parity;
		}
	}
	//with logging
	else{
		nbroken=0;
		for(int i=0; i<this->_nActivities; i++){	
			int ac=this->_activities[i];
			int parity=r.internalActivitiesList[ac].parity==PARITY_WEEKLY?2:1;
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
				if(conflictsString!=NULL){
					*conflictsString+=
						(QObject::tr("Space constraint subject preferred room broken for activity with id %1")
						.arg(r.internalActivitiesList[ac].id));
					*conflictsString += ". ";
					*conflictsString += (QObject::tr("This increases the conflicts total by %1").arg(weight*parity));
					*conflictsString += "\n";
				}

				nbroken+=parity;
			}
		}
	}

	return int (ceil ( weight * nbroken ) );
}

bool ConstraintSubjectPreferredRooms::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintSubjectPreferredRooms::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

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
	if(s)
		;

	return false;
}

bool ConstraintSubjectPreferredRooms::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

	return false;
}

bool ConstraintSubjectPreferredRooms::isRelatedToEquipment(Equipment* e)
{
	if(e)
		;

	return false;
}

bool ConstraintSubjectPreferredRooms::isRelatedToBuilding(Building* b)
{
	if(b)
		;

	return false;
}

bool ConstraintSubjectPreferredRooms::isRelatedToRoom(Room* r)
{
	if(r)
		;

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

ConstraintMaxBuildingChangesPerDayForTeachers::ConstraintMaxBuildingChangesPerDayForTeachers()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_MAX_BUILDING_CHANGES_PER_DAY_FOR_TEACHERS;
}

ConstraintMaxBuildingChangesPerDayForTeachers::ConstraintMaxBuildingChangesPerDayForTeachers(double w, bool c, int max_building_changes)
	: SpaceConstraint(w, c)
{
	this->type=CONSTRAINT_MAX_BUILDING_CHANGES_PER_DAY_FOR_TEACHERS;
	this->maxBuildingChanges=max_building_changes;
}

bool ConstraintMaxBuildingChangesPerDayForTeachers::computeInternalStructure(Rules& r)
{
	if(&r!=NULL)
		;
	/*do nothing*/
	
	return true;
}

QString ConstraintMaxBuildingChangesPerDayForTeachers::getXmlDescription(Rules& r)
{
	if(&r!=NULL)
		;

	QString s = "<ConstraintMaxBuildingChangesPerDayForTeachers>\n";
	s += "	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";
	s+=yesNo(this->compulsory);
	s+="</Compulsory>\n";
	s+="	<Max_Building_Changes>";
	s+=QString::number(this->maxBuildingChanges);
	s+="</Max_Building_Changes>\n";
	s += "</ConstraintMaxBuildingChangesPerDayForTeachers>\n";
	return s;
}

QString ConstraintMaxBuildingChangesPerDayForTeachers::getDescription(Rules& r)
{
	if(&r!=NULL)
		;
	
	QString s = QObject::tr("Max building changes per day for teachers, W:%1").arg(this->weight);
	s+=", ";
	s += QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory));
	s+=", ";
	s+=QObject::tr("MBC:%1").arg(this->maxBuildingChanges);
	
	return s;
}

QString ConstraintMaxBuildingChangesPerDayForTeachers::getDetailedDescription(Rules& r)
{
	if(&r!=NULL)
		;

	QString s=QObject::tr("Constraint to specify maximum the number of building\n"
	 "changes for teachers, in each day\n");
	s+=QObject::tr("Weight=%1").arg(this->weight);s+="\n";
	s+=QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory));s+="\n";
	s+=QObject::tr("Max building changes=%1").arg(this->maxBuildingChanges);s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int ConstraintMaxBuildingChangesPerDayForTeachers::fitness(
	SpaceChromosome& c,
	Rules& r,
	const int days[/*MAX_ACTIVITIES*/],
	const int hours[/*MAX_ACTIVITIES*/],
	QString* conflictsString)
	{

	assert(r.internalStructureComputed);

	int nbroken=0;

	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(crt_chrom!=&c || crt_rules!=&r || rooms_conflicts<0 || c.spaceChangedForMatrixCalculation){
		rooms_conflicts = c.getRoomsMatrix(r, days, hours, roomsMatrix);

		crt_chrom = &c;
		crt_rules = &r;
		
		c.spaceChangedForMatrixCalculation=false;
	}
	
	if(crt_chrom_2!=&c){
		computeTeachersRoomsWeek1(c, r, days, hours);
	
		crt_chrom_2=&c;
	}

	//part without logging....................................................................
	if(conflictsString==NULL){
		for(int t=0; t<r.nInternalTeachers; t++){
			for(int d=0; d<r.nDaysPerWeek; d++){
				int h;
				int n_changes=0;
				for(h=0; h<r.nHoursPerDay; h++)
					if(teachersRoomsWeek1[t][d][h]!=UNALLOCATED_SPACE
					 && r.internalRoomsList[teachersRoomsWeek1[t][d][h]]->buildingIndex!=-1)
							break;
				char last_building=-1;
				if(h<r.nHoursPerDay)
					last_building=r.internalRoomsList[teachersRoomsWeek1[t][d][h]]->buildingIndex;
				for(h++; h<r.nHoursPerDay; h++)
					if(teachersRoomsWeek1[t][d][h]!=UNALLOCATED_SPACE
					 && r.internalRoomsList[teachersRoomsWeek1[t][d][h]]->buildingIndex!=-1
					 && r.internalRoomsList[teachersRoomsWeek1[t][d][h]]->buildingIndex!=last_building){
					 	last_building=r.internalRoomsList[teachersRoomsWeek1[t][d][h]]->buildingIndex;
						n_changes++;
					}
						
				if(n_changes>this->maxBuildingChanges)
					nbroken+=n_changes-this->maxBuildingChanges;
			}
		}
	}
	//part with logging....................................................................
	else{
		for(int t=0; t<r.nInternalTeachers; t++){
			for(int d=0; d<r.nDaysPerWeek; d++){
				int h;
				int n_changes=0;
				for(h=0; h<r.nHoursPerDay; h++)
					if(teachersRoomsWeek1[t][d][h]!=UNALLOCATED_SPACE
					 && r.internalRoomsList[teachersRoomsWeek1[t][d][h]]->buildingIndex!=-1)
							break;
				char last_building=-1;
				if(h<r.nHoursPerDay)
					last_building=r.internalRoomsList[teachersRoomsWeek1[t][d][h]]->buildingIndex;
				for(h++; h<r.nHoursPerDay; h++)
					if(teachersRoomsWeek1[t][d][h]!=UNALLOCATED_SPACE
					 && r.internalRoomsList[teachersRoomsWeek1[t][d][h]]->buildingIndex!=-1
					 && r.internalRoomsList[teachersRoomsWeek1[t][d][h]]->buildingIndex!=last_building){
					 	last_building=r.internalRoomsList[teachersRoomsWeek1[t][d][h]]->buildingIndex;
						n_changes++;
					}
						
				if(n_changes>this->maxBuildingChanges){
					nbroken+=n_changes-this->maxBuildingChanges;
				
					(*conflictsString) += QObject::tr("Space constraint max building changes per day for teachers broken for teacher %1 on day %2, max changes=%3, actual changes=%4")
					 .arg(r.internalTeachersList[t]->name)
					 .arg(r.daysOfTheWeek[d])
					 .arg(this->maxBuildingChanges)
					 .arg(n_changes);
				}
			}
		}
	}

	return int (ceil ( weight * nbroken ) ); //fitness factor
}

bool ConstraintMaxBuildingChangesPerDayForTeachers::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintMaxBuildingChangesPerDayForTeachers::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintMaxBuildingChangesPerDayForTeachers::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintMaxBuildingChangesPerDayForTeachers::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintMaxBuildingChangesPerDayForTeachers::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

	return false;
}

bool ConstraintMaxBuildingChangesPerDayForTeachers::isRelatedToEquipment(Equipment* e)
{
	if(e)
		;

	return false;
}

bool ConstraintMaxBuildingChangesPerDayForTeachers::isRelatedToBuilding(Building* b)
{
	if(b)
		;

	return true;
}

bool ConstraintMaxBuildingChangesPerDayForTeachers::isRelatedToRoom(Room* r)
{
	if(r)
		;

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

ConstraintMaxBuildingChangesPerDayForStudents::ConstraintMaxBuildingChangesPerDayForStudents()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_MAX_BUILDING_CHANGES_PER_DAY_FOR_STUDENTS;
}

ConstraintMaxBuildingChangesPerDayForStudents::ConstraintMaxBuildingChangesPerDayForStudents(double w, bool c, int max_building_changes)
	: SpaceConstraint(w, c)
{
	this->type=CONSTRAINT_MAX_BUILDING_CHANGES_PER_DAY_FOR_STUDENTS;
	this->maxBuildingChanges=max_building_changes;
}

bool ConstraintMaxBuildingChangesPerDayForStudents::computeInternalStructure(Rules& r)
{
	if(&r!=NULL)
		;
	/*do nothing*/
	
	return true;
}

QString ConstraintMaxBuildingChangesPerDayForStudents::getXmlDescription(Rules& r)
{
	if(&r!=NULL)
		;

	QString s = "<ConstraintMaxBuildingChangesPerDayForStudents>\n";
	s += "	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";
	s+=yesNo(this->compulsory);
	s+="</Compulsory>\n";
	s+="	<Max_Building_Changes>";
	s+=QString::number(this->maxBuildingChanges);
	s+="</Max_Building_Changes>\n";
	s += "</ConstraintMaxBuildingChangesPerDayForStudents>\n";
	return s;
}

QString ConstraintMaxBuildingChangesPerDayForStudents::getDescription(Rules& r)
{
	if(&r!=NULL)
		;
	
	QString s = QObject::tr("Max building changes per day for students, W:%1").arg(this->weight);
	s+=", ";
	s += QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory));
	s+=", ";
	s+=QObject::tr("MBC:%1").arg(this->maxBuildingChanges);
	
	return s;
}

QString ConstraintMaxBuildingChangesPerDayForStudents::getDetailedDescription(Rules& r)
{
	if(&r!=NULL)
		;

	QString s=QObject::tr("Constraint to specify maximum the number of building\n"
	 "changes for students, in each day\n");
	s+=QObject::tr("Weight=%1").arg(this->weight);s+="\n";
	s+=QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory));s+="\n";
	s+=QObject::tr("Max building changes=%1").arg(this->maxBuildingChanges);s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int ConstraintMaxBuildingChangesPerDayForStudents::fitness(
	SpaceChromosome& c,
	Rules& r,
	const int days[/*MAX_ACTIVITIES*/],
	const int hours[/*MAX_ACTIVITIES*/],
	QString* conflictsString)
	{

	assert(r.internalStructureComputed);

	int nbroken=0;

	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(crt_chrom!=&c || crt_rules!=&r || rooms_conflicts<0 || c.spaceChangedForMatrixCalculation){
		rooms_conflicts = c.getRoomsMatrix(r, days, hours, roomsMatrix);

		crt_chrom = &c;
		crt_rules = &r;
		
		c.spaceChangedForMatrixCalculation=false;
	}
	
	if(crt_chrom_3!=&c){
		computeSubgroupsRoomsWeek1(c, r, days, hours);
	
		crt_chrom_3=&c;
	}

	//part without logging....................................................................
	if(conflictsString==NULL){
		for(int s=0; s<r.nInternalSubgroups; s++){
			for(int d=0; d<r.nDaysPerWeek; d++){
				int h;
				int n_changes=0;
				for(h=0; h<r.nHoursPerDay; h++)
					if(subgroupsRoomsWeek1[s][d][h]!=UNALLOCATED_SPACE
					 && r.internalRoomsList[subgroupsRoomsWeek1[s][d][h]]->buildingIndex!=-1)
						break;
				char last_building=-1;
				if(h<r.nHoursPerDay)
					last_building=r.internalRoomsList[subgroupsRoomsWeek1[s][d][h]]->buildingIndex;
				for(h++; h<r.nHoursPerDay; h++)
					if(subgroupsRoomsWeek1[s][d][h]!=UNALLOCATED_SPACE 
					 && r.internalRoomsList[subgroupsRoomsWeek1[s][d][h]]->buildingIndex!=-1
					 && r.internalRoomsList[subgroupsRoomsWeek1[s][d][h]]->buildingIndex!=last_building){
					 	last_building=r.internalRoomsList[subgroupsRoomsWeek1[s][d][h]]->buildingIndex;
						n_changes++;
					}
						
				if(n_changes>this->maxBuildingChanges)
					nbroken+=n_changes-this->maxBuildingChanges;
			}
		}
	}
	//part with logging....................................................................
	else{
		for(int s=0; s<r.nInternalSubgroups; s++){
			for(int d=0; d<r.nDaysPerWeek; d++){
				int h;
				int n_changes=0;
				for(h=0; h<r.nHoursPerDay; h++)
					if(subgroupsRoomsWeek1[s][d][h]!=UNALLOCATED_SPACE
					 && r.internalRoomsList[subgroupsRoomsWeek1[s][d][h]]->buildingIndex!=-1)
						break;
				char last_building=-1;
				if(h<r.nHoursPerDay)
					last_building=r.internalRoomsList[subgroupsRoomsWeek1[s][d][h]]->buildingIndex;
				for(h++; h<r.nHoursPerDay; h++)
					if(subgroupsRoomsWeek1[s][d][h]!=UNALLOCATED_SPACE 
					 && r.internalRoomsList[subgroupsRoomsWeek1[s][d][h]]->buildingIndex!=-1
					 && r.internalRoomsList[subgroupsRoomsWeek1[s][d][h]]->buildingIndex!=last_building){
					 	last_building=r.internalRoomsList[subgroupsRoomsWeek1[s][d][h]]->buildingIndex;
						n_changes++;
					}
						
				if(n_changes>this->maxBuildingChanges){
					nbroken+=n_changes-this->maxBuildingChanges;
				
					(*conflictsString) += QObject::tr("Space constraint max building changes per day for students broken for subgroup %1 on day %2, max changes=%3, actual changes=%4")
					 .arg(r.internalSubgroupsList[s]->name)
					 .arg(r.daysOfTheWeek[d])
					 .arg(this->maxBuildingChanges)
					 .arg(n_changes);
				}
			}
		}
	}

	return int (ceil ( weight * nbroken ) ); //fitness factor
}

bool ConstraintMaxBuildingChangesPerDayForStudents::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintMaxBuildingChangesPerDayForStudents::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintMaxBuildingChangesPerDayForStudents::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintMaxBuildingChangesPerDayForStudents::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintMaxBuildingChangesPerDayForStudents::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

	return false;
}

bool ConstraintMaxBuildingChangesPerDayForStudents::isRelatedToEquipment(Equipment* e)
{
	if(e)
		;

	return false;
}

bool ConstraintMaxBuildingChangesPerDayForStudents::isRelatedToBuilding(Building* b)
{
	if(b)
		;

	return true;
}

bool ConstraintMaxBuildingChangesPerDayForStudents::isRelatedToRoom(Room* r)
{
	if(r)
		;

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

ConstraintMaxRoomChangesPerDayForTeachers::ConstraintMaxRoomChangesPerDayForTeachers()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_MAX_ROOM_CHANGES_PER_DAY_FOR_TEACHERS;
}

ConstraintMaxRoomChangesPerDayForTeachers::ConstraintMaxRoomChangesPerDayForTeachers(double w, bool c, int max_room_changes)
	: SpaceConstraint(w, c)
{
	this->type=CONSTRAINT_MAX_ROOM_CHANGES_PER_DAY_FOR_TEACHERS;
	this->maxRoomChanges=max_room_changes;
}

bool ConstraintMaxRoomChangesPerDayForTeachers::computeInternalStructure(Rules& r)
{
	if(&r!=NULL)
		;
	/*do nothing*/
	
	return true;
}

QString ConstraintMaxRoomChangesPerDayForTeachers::getXmlDescription(Rules& r)
{
	if(&r!=NULL)
		;

	QString s = "<ConstraintMaxRoomChangesPerDayForTeachers>\n";
	s += "	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";
	s+=yesNo(this->compulsory);
	s+="</Compulsory>\n";
	s+="	<Max_Room_Changes>";
	s+=QString::number(this->maxRoomChanges);
	s+="</Max_Room_Changes>\n";
	s += "</ConstraintMaxRoomChangesPerDayForTeachers>\n";
	return s;
}

QString ConstraintMaxRoomChangesPerDayForTeachers::getDescription(Rules& r)
{
	if(&r!=NULL)
		;
	
	QString s = QObject::tr("Max room changes per day for teachers, W:%1").arg(this->weight);
	s+=", ";
	s += QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory));
	s+=", ";
	s+=QObject::tr("MRC:%1").arg(this->maxRoomChanges);
	
	return s;
}

QString ConstraintMaxRoomChangesPerDayForTeachers::getDetailedDescription(Rules& r)
{
	if(&r!=NULL)
		;

	QString s=QObject::tr("Constraint to specify maximum the number of room\n"
	 "changes for teachers, in each day\n");
	s+=QObject::tr("Weight=%1").arg(this->weight);s+="\n";
	s+=QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory));s+="\n";
	s+=QObject::tr("Max room changes=%1").arg(this->maxRoomChanges);s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int ConstraintMaxRoomChangesPerDayForTeachers::fitness(
	SpaceChromosome& c,
	Rules& r,
	const int days[/*MAX_ACTIVITIES*/],
	const int hours[/*MAX_ACTIVITIES*/],
	QString* conflictsString)
	{

	assert(r.internalStructureComputed);

	int nbroken=0;

	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(crt_chrom!=&c || crt_rules!=&r || rooms_conflicts<0 || c.spaceChangedForMatrixCalculation){
		rooms_conflicts = c.getRoomsMatrix(r, days, hours, roomsMatrix);

		crt_chrom = &c;
		crt_rules = &r;
		
		c.spaceChangedForMatrixCalculation=false;
	}
	
	if(crt_chrom_2!=&c){
		computeTeachersRoomsWeek1(c, r, days, hours);
	
		crt_chrom_2=&c;
	}

	//part without logging....................................................................
	if(conflictsString==NULL){
		for(int t=0; t<r.nInternalTeachers; t++){
			for(int d=0; d<r.nDaysPerWeek; d++){
				int h;
				int n_changes=0;
				for(h=0; h<r.nHoursPerDay; h++)
					if(teachersRoomsWeek1[t][d][h]!=UNALLOCATED_SPACE)
						break;
				char last_room=UNALLOCATED_SPACE;
				if(h<r.nHoursPerDay)
					last_room=teachersRoomsWeek1[t][d][h];
				for(h++; h<r.nHoursPerDay; h++)
					if(teachersRoomsWeek1[t][d][h]!=UNALLOCATED_SPACE
					 && teachersRoomsWeek1[t][d][h]!=last_room){
					 	last_room=teachersRoomsWeek1[t][d][h];
						n_changes++;
					}
						
				if(n_changes>this->maxRoomChanges)
					nbroken+=n_changes-this->maxRoomChanges;
			}
		}
	}
	//part with logging....................................................................
	else{
		for(int t=0; t<r.nInternalTeachers; t++){
			for(int d=0; d<r.nDaysPerWeek; d++){
				int h;
				int n_changes=0;
				for(h=0; h<r.nHoursPerDay; h++)
					if(teachersRoomsWeek1[t][d][h]!=UNALLOCATED_SPACE)
						break;
				char last_room=UNALLOCATED_SPACE;
				if(h<r.nHoursPerDay)
					last_room=teachersRoomsWeek1[t][d][h];
				for(h++; h<r.nHoursPerDay; h++)
					if(teachersRoomsWeek1[t][d][h]!=UNALLOCATED_SPACE
					 && teachersRoomsWeek1[t][d][h]!=last_room){
					 	last_room=teachersRoomsWeek1[t][d][h];
						n_changes++;
					}
						
				if(n_changes>this->maxRoomChanges){
					nbroken+=n_changes-this->maxRoomChanges;
				
					(*conflictsString) += QObject::tr("Space constraint max room changes per day for teachers broken for teacher %1 on day %2, max changes=%3, actual changes=%4")
					 .arg(r.internalTeachersList[t]->name)
					 .arg(r.daysOfTheWeek[d])
					 .arg(this->maxRoomChanges)
					 .arg(n_changes);
				}
			}
		}
	}

	return int (ceil ( weight * nbroken ) ); //fitness factor
}

bool ConstraintMaxRoomChangesPerDayForTeachers::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintMaxRoomChangesPerDayForTeachers::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintMaxRoomChangesPerDayForTeachers::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintMaxRoomChangesPerDayForTeachers::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintMaxRoomChangesPerDayForTeachers::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

	return false;
}

bool ConstraintMaxRoomChangesPerDayForTeachers::isRelatedToEquipment(Equipment* e)
{
	if(e)
		;

	return false;
}

bool ConstraintMaxRoomChangesPerDayForTeachers::isRelatedToBuilding(Building* b)
{
	if(b)
		;

	return false;
}

bool ConstraintMaxRoomChangesPerDayForTeachers::isRelatedToRoom(Room* r)
{
	if(r)
		;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

ConstraintMaxRoomChangesPerDayForStudents::ConstraintMaxRoomChangesPerDayForStudents()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_MAX_ROOM_CHANGES_PER_DAY_FOR_STUDENTS;
}

ConstraintMaxRoomChangesPerDayForStudents::ConstraintMaxRoomChangesPerDayForStudents(double w, bool c, int max_room_changes)
	: SpaceConstraint(w, c)
{
	this->type=CONSTRAINT_MAX_ROOM_CHANGES_PER_DAY_FOR_STUDENTS;
	this->maxRoomChanges=max_room_changes;
}

bool ConstraintMaxRoomChangesPerDayForStudents::computeInternalStructure(Rules& r)
{
	if(&r!=NULL)
		;
	/*do nothing*/
	
	return true;
}

QString ConstraintMaxRoomChangesPerDayForStudents::getXmlDescription(Rules& r)
{
	if(&r!=NULL)
		;

	QString s = "<ConstraintMaxRoomChangesPerDayForStudents>\n";
	s += "	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";
	s+=yesNo(this->compulsory);
	s+="</Compulsory>\n";
	s+="	<Max_Room_Changes>";
	s+=QString::number(this->maxRoomChanges);
	s+="</Max_Room_Changes>\n";
	s += "</ConstraintMaxRoomChangesPerDayForStudents>\n";
	return s;
}

QString ConstraintMaxRoomChangesPerDayForStudents::getDescription(Rules& r)
{
	if(&r!=NULL)
		;
	
	QString s = QObject::tr("Max room changes per day for students, W:%1").arg(this->weight);
	s+=", ";
	s += QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory));
	s+=", ";
	s+=QObject::tr("MRC:%1").arg(this->maxRoomChanges);
	
	return s;
}

QString ConstraintMaxRoomChangesPerDayForStudents::getDetailedDescription(Rules& r)
{
	if(&r!=NULL)
		;

	QString s=QObject::tr("Constraint to specify maximum the number of room\n"
	 "changes for students, in each day\n");
	s+=QObject::tr("Weight=%1").arg(this->weight);s+="\n";
	s+=QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory));s+="\n";
	s+=QObject::tr("Max room changes=%1").arg(this->maxRoomChanges);s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int ConstraintMaxRoomChangesPerDayForStudents::fitness(
	SpaceChromosome& c,
	Rules& r,
	const int days[/*MAX_ACTIVITIES*/],
	const int hours[/*MAX_ACTIVITIES*/],
	QString* conflictsString)
	{

	assert(r.internalStructureComputed);

	int nbroken=0;

	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(crt_chrom!=&c || crt_rules!=&r || rooms_conflicts<0 || c.spaceChangedForMatrixCalculation){
		rooms_conflicts = c.getRoomsMatrix(r, days, hours, roomsMatrix);

		crt_chrom = &c;
		crt_rules = &r;
		
		c.spaceChangedForMatrixCalculation=false;
	}
	
	if(crt_chrom_3!=&c){
		computeSubgroupsRoomsWeek1(c, r, days, hours);
	
		crt_chrom_3=&c;
	}

	//part without logging....................................................................
	if(conflictsString==NULL){
		for(int s=0; s<r.nInternalSubgroups; s++){
			for(int d=0; d<r.nDaysPerWeek; d++){
				int h;
				int n_changes=0;
				for(h=0; h<r.nHoursPerDay; h++)
					if(subgroupsRoomsWeek1[s][d][h]!=UNALLOCATED_SPACE)
						break;
				char last_building=-1;
				if(h<r.nHoursPerDay)
					last_building=subgroupsRoomsWeek1[s][d][h];
				for(h++; h<r.nHoursPerDay; h++)
					if(subgroupsRoomsWeek1[s][d][h]!=UNALLOCATED_SPACE 
					 && subgroupsRoomsWeek1[s][d][h]!=last_building){
					 	last_building=subgroupsRoomsWeek1[s][d][h];
						n_changes++;
					}
						
				if(n_changes>this->maxRoomChanges)
					nbroken+=n_changes-this->maxRoomChanges;
			}
		}
	}
	//part with logging....................................................................
	else{
		for(int s=0; s<r.nInternalSubgroups; s++){
			for(int d=0; d<r.nDaysPerWeek; d++){
				int h;
				int n_changes=0;
				for(h=0; h<r.nHoursPerDay; h++)
					if(subgroupsRoomsWeek1[s][d][h]!=UNALLOCATED_SPACE)
						break;
				char last_building=-1;
				if(h<r.nHoursPerDay)
					last_building=subgroupsRoomsWeek1[s][d][h];
				for(h++; h<r.nHoursPerDay; h++)
					if(subgroupsRoomsWeek1[s][d][h]!=UNALLOCATED_SPACE 
					 && subgroupsRoomsWeek1[s][d][h]!=last_building){
					 	last_building=subgroupsRoomsWeek1[s][d][h];
						n_changes++;
					}
						
				if(n_changes>this->maxRoomChanges){
					nbroken+=n_changes-this->maxRoomChanges;
				
					(*conflictsString) += QObject::tr("Space constraint max room changes per day for students broken for subgroup %1 on day %2, max changes=%3, actual changes=%4")
					 .arg(r.internalSubgroupsList[s]->name)
					 .arg(r.daysOfTheWeek[d])
					 .arg(this->maxRoomChanges)
					 .arg(n_changes);
				}
			}
		}
	}

	return int (ceil ( weight * nbroken ) ); //fitness factor
}

bool ConstraintMaxRoomChangesPerDayForStudents::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintMaxRoomChangesPerDayForStudents::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintMaxRoomChangesPerDayForStudents::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintMaxRoomChangesPerDayForStudents::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintMaxRoomChangesPerDayForStudents::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

	return false;
}

bool ConstraintMaxRoomChangesPerDayForStudents::isRelatedToEquipment(Equipment* e)
{
	if(e)
		;

	return false;
}

bool ConstraintMaxRoomChangesPerDayForStudents::isRelatedToBuilding(Building* b)
{
	if(b)
		;

	return false;
}

bool ConstraintMaxRoomChangesPerDayForStudents::isRelatedToRoom(Room* r)
{
	if(r)
		;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
