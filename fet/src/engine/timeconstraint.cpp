/*
File timeconstraint.cpp
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
#include "timeconstraint.h"
#include "rules.h"
#include "solution.h"
#include "activity.h"
#include "teacher.h"
#include "subject.h"
#include "activitytag.h"
#include "studentsset.h"

#include <qstring.h>

#include <QMessageBox>

#include <iostream>
using namespace std;

#define yesNo(x)				((x)==0?"no":"yes")
#define yesNoTranslated(x)		((x)==0?QObject::tr("no"):QObject::tr("yes"))

#define minimu(x,y)	((x)<(y)?(x):(y))
#define maximu(x,y)	((x)>(y)?(x):(y))

//static Solution* crt_chrom=NULL;
//static Rules* crt_rules=NULL;

//The following 2 matrices are kept to make the computation faster
//They are calculated only at the beginning of the computation of the fitness
//of the solution.
static qint8 subgroupsMatrix[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
static qint8 teachersMatrix[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

static int teachers_conflicts=-1;
static int subgroups_conflicts=-1;

//extern bool breakTime[MAX_HOURS_PER_WEEK];
extern bool breakDayHour[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

//extern bool teacherNotAvailableTime[MAX_TEACHERS][MAX_HOURS_PER_WEEK];
extern bool teacherNotAvailableDayHour[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

//extern bool subgroupNotAvailableTime[MAX_TOTAL_SUBGROUPS][MAX_HOURS_PER_WEEK];
extern bool subgroupNotAvailableDayHour[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

TimeConstraint::TimeConstraint()
{
	type=CONSTRAINT_GENERIC_TIME;
}

TimeConstraint::~TimeConstraint()
{
}

TimeConstraint::TimeConstraint(double wp)
{
	weightPercentage=wp;
	//assert(wp<=100.0 && wp>=0.0 && wp==floor(wp) && wp==ceil(wp)); //integer, for now
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

ConstraintBasicCompulsoryTime::ConstraintBasicCompulsoryTime(): TimeConstraint()
{
	this->type=CONSTRAINT_BASIC_COMPULSORY_TIME;
	this->weightPercentage=100;
}

ConstraintBasicCompulsoryTime::ConstraintBasicCompulsoryTime(double wp): TimeConstraint(wp)
{
	this->type=CONSTRAINT_BASIC_COMPULSORY_TIME;
}

bool ConstraintBasicCompulsoryTime::computeInternalStructure(Rules& r)
{
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;
	/*do nothing*/
	
	return true;
}

bool ConstraintBasicCompulsoryTime::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintBasicCompulsoryTime::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s = "<ConstraintBasicCompulsoryTime>\n";
	s += "	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	assert(this->weightPercentage==100.0);
	/*s+="	<Compulsory>";
	s+=yesNo(this->compulsory);
	s+="</Compulsory>\n";*/
	s += "</ConstraintBasicCompulsoryTime>\n";
	return s;
}

QString ConstraintBasicCompulsoryTime::getDescription(Rules& r)
{
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	return QObject::tr("Basic compulsory constraints (time)") +
		" " + QObject::tr("WP:%1\%").arg(this->weightPercentage);
}

QString ConstraintBasicCompulsoryTime::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);	
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("These are the basic compulsory constraints\n"
		"(referring to time allocation) for any timetable\n");

	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";
	//s+=QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory));s+="\n";
	s+=QObject::tr("The basic time constraints try to avoid:\n");
	//s+=QObject::tr("- unallocated activities\n");
	//s+=QObject::tr("- activities scheduled too late\n");
	s+=QObject::tr("- teachers assigned to more than one activity simultaneously\n");
	s+=QObject::tr("- students assigned to more than one activity simultaneously\n");

	return s;
}

double ConstraintBasicCompulsoryTime::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString){
	assert(r.internalStructureComputed);

	int teachersConflicts, subgroupsConflicts;
	
	assert(weightPercentage==100.0);

	//This constraint fitness calculation routine is called firstly,
	//so we can compute the teacher and subgroups conflicts faster this way.
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	
		subgroups_conflicts = subgroupsConflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = teachersConflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom = &c;
		//crt_rules = &r;
		
		c.changedForMatrixCalculation=false;
	}
	else{
		assert(subgroups_conflicts>=0);
		assert(teachers_conflicts>=0);
		subgroupsConflicts = subgroups_conflicts;
		teachersConflicts = teachers_conflicts;
	}

	int i,dd;

	int unallocated; //unallocated activities
	int late; //late activities
	int nte; //number of teacher exhaustions
	int nse; //number of students exhaustions

	//Part without logging..................................................................
	if(conflictsString==NULL){
		//Unallocated or late activities
		unallocated=0;
		late=0;
		for(i=0; i<r.nInternalActivities; i++){
			if(c.times[i]==UNALLOCATED_TIME){
				//Firstly, we consider a big clash each unallocated activity.
				//Needs to be very a large constant, bigger than any other broken constraint.
				unallocated += /*r.internalActivitiesList[i].duration * r.internalActivitiesList[i].nSubgroups * */ 10000;
				//(an unallocated activity for a year is more important than an unallocated activity for a subgroup)
			}
			else{
				//Calculates the number of activities that are scheduled too late (in fact we
				//calculate a function that increases as the activity is getting late)
				int h=c.times[i]/r.nDaysPerWeek;
				dd=r.internalActivitiesList[i].duration;
				if(h+dd>r.nHoursPerDay){
					int tmp;
					/*if(r.internalActivitiesList[i].parity==PARITY_WEEKLY)
						tmp=2;
					else{
						assert(r.internalActivitiesList[i].parity==PARITY_FORTNIGHTLY);*/
						tmp=1;
					//}
					late += (h+dd-r.nHoursPerDay) * tmp * r.internalActivitiesList[i].iSubgroupsList.count();
					//multiplied with 2 for weekly activities and with the number
					//of subgroups implied, for seeing the importance of the
					//activity
				}
			}
		}
		
		assert(late==0);

		//Below, for teachers and students, please remember that 2 means a weekly activity
		//and 1 fortnightly one. So, if the matrix teachersMatrix[teacher][day][hour]==2, it is ok.

		//Calculates the number of teachers exhaustion (when he has to teach more than
		//one activity at the same time)
		/*nte=0;
		for(i=0; i<r.nInternalTeachers; i++)
			for(int j=0; j<r.nDaysPerWeek; j++)
				for(int k=0; k<r.nHoursPerDay; k++){
					int tmp=teachersMatrix[i][j][k]-2;
					if(tmp>0)
						nte+=tmp;
				}*/
		nte = teachersConflicts; //faster
		
		assert(nte==0);

		//Calculates the number of subgroups exhaustion (a subgroup cannot attend two
		//activities at the same time)
		/*nse=0;
		for(i=0; i<r.nInternalSubgroups; i++)
			for(int j=0; j<r.nDaysPerWeek; j++)
				for(int k=0; k<r.nHoursPerDay; k++){
					int tmp=subgroupsMatrix[i][j][k]-2;
					if(tmp>0)
						nse += tmp;
				}*/
		nse = subgroupsConflicts; //faster
		
		assert(nse==0);			
	}
	//part with logging....................................................................
	else{
		//Unallocated or late activities
		unallocated=0;
		late=0;
		for(i=0; i<r.nInternalActivities; i++){
			if(c.times[i]==UNALLOCATED_TIME){
				//Firstly, we consider a big clash each unallocated activity.
				//Needs to be very a large constant, bigger than any other broken constraint.
				unallocated += /*r.internalActivitiesList[i].duration * r.internalActivitiesList[i].nSubgroups * */ 10000;
				//(an unallocated activity for a year is more important than an unallocated activity for a subgroup)
				if(conflictsString!=NULL){
					QString s= QObject::tr("Time constraint basic compulsory");
					s += ": ";
					s += QObject::tr("unallocated activity with id=%1").arg(r.internalActivitiesList[i].id);
					s += QObject::tr(" - this increases the conflicts total by %1")
					 .arg(weightPercentage/100 * /*r.internalActivitiesList[i].duration*r.internalActivitiesList[i].nSubgroups * */10000);
					//s += "\n";
					
					dl.append(s);
					cl.append(weightPercentage/100 * 10000);

					(*conflictsString) += s + "\n";
				}
			}
			else{
				//Calculates the number of activities that are scheduled too late (in fact we
				//calculate a function that increases as the activity is getting late)
				int h=c.times[i]/r.nDaysPerWeek;
				dd=r.internalActivitiesList[i].duration;
				if(h+dd>r.nHoursPerDay){
					assert(0);	
				
					int tmp;
					/*if(r.internalActivitiesList[i].parity==PARITY_WEEKLY)
						tmp=2;
					else{
						assert(r.internalActivitiesList[i].parity==PARITY_FORTNIGHTLY);*/
						tmp=1;
					//}
					late += (h+dd-r.nHoursPerDay) * tmp * r.internalActivitiesList[i].iSubgroupsList.count();
					//multiplied with 2 for weekly activities and with the number
					//of subgroups implied, for seeing the importance of the
					//activity

					if(conflictsString!=NULL){
						QString s=QObject::tr("Time constraint basic compulsory");
						s+=": ";
						s+=QObject::tr("activity with id=%1 is late.")
						 .arg(r.internalActivitiesList[i].id);
						s+=" ";
						s+=QObject::tr("This increases the conflicts total by %1")
						 .arg((h+dd-r.nHoursPerDay)*tmp*r.internalActivitiesList[i].iSubgroupsList.count()*weightPercentage/100);
						s+="\n";
						
						dl.append(s);
						cl.append((h+dd-r.nHoursPerDay)*tmp*r.internalActivitiesList[i].iSubgroupsList.count()*weightPercentage/100);

						(*conflictsString) += s+"\n";
					}
				}
			}
		}

		//Below, for teachers and students, please remember that 2 means a weekly activity
		//and 1 fortnightly one. So, if the matrix teachersMatrix[teacher][day][hour]==2,
		//that is ok.

		//Calculates the number of teachers exhaustion (when he has to teach more than
		//one activity at the same time)
		nte=0;
		for(i=0; i<r.nInternalTeachers; i++)
			for(int j=0; j<r.nDaysPerWeek; j++)
				for(int k=0; k<r.nHoursPerDay; k++){
					int tmp=teachersMatrix[i][j][k]-1;
					if(tmp>0){
						if(conflictsString!=NULL){
							QString s=QObject::tr("Time constraint basic compulsory");
							s+=": ";
							s+=QObject::tr("teacher with name %1 has more than one allocated activity on day %2, hour %3")
							 .arg(r.internalTeachersList[i]->name)
							 .arg(r.daysOfTheWeek[j])
							 .arg(r.hoursOfTheDay[k]);
							s+=". ";
							s+=QObject::tr("This increases the conflicts total by %1")
							 .arg(tmp*weightPercentage/100);
						
							(*conflictsString)+= s+"\n";
							
							dl.append(s);
							cl.append(tmp*weightPercentage/100);
						}
						nte+=tmp;
					}
				}

		assert(nte==0);
		
		//Calculates the number of subgroups exhaustion (a subgroup cannot attend two
		//activities at the same time)
		nse=0;
		for(i=0; i<r.nInternalSubgroups; i++)
			for(int j=0; j<r.nDaysPerWeek; j++)
				for(int k=0; k<r.nHoursPerDay; k++){
					int tmp=subgroupsMatrix[i][j][k]-1;
					if(tmp>0){
						if(conflictsString!=NULL){
							QString s=QObject::tr("Time constraint basic compulsory");
							s+=": ";
							s+=QObject::tr("subgroup %1 has more than one allocated activity on day %2, hour %3")
							 .arg(r.internalSubgroupsList[i]->name)
							 .arg(r.daysOfTheWeek[j])
							 .arg(r.hoursOfTheDay[k]);
							s+=". ";
							s+=QObject::tr("This increases the conflicts total by %1")
							 .arg((subgroupsMatrix[i][j][k]-1)*weightPercentage/100);
							 
							dl.append(s);
							cl.append((subgroupsMatrix[i][j][k]-1)*weightPercentage/100);
						
							*conflictsString += s+"\n";
						}
						nse += tmp;
					}
				}
			
		assert(nse==0);
	}

	/*if(nte!=teachersConflicts){
		cout<<"nte=="<<nte<<", teachersConflicts=="<<teachersConflicts<<endl;
		cout<<c.getTeachersMatrix(r, teachersMatrix)<<endl;
	}
	if(nse!=subgroupsConflicts){
		cout<<"nse=="<<nse<<", subgroupsConflicts=="<<subgroupsConflicts<<endl;
		cout<<c.getSubgroupsMatrix(r, subgroupsMatrix)<<endl;
	}*/
	
	/*assert(nte==teachersConflicts); //just a check, works only on logged fitness calculation
	assert(nse==subgroupsConflicts);*/

	//return int (ceil ( weight * (unallocated + late + nte + nse) ) ); //conflicts factor
	return weightPercentage/100 * (unallocated + late + nte + nse); //conflicts factor
}

bool ConstraintBasicCompulsoryTime::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(a);
	Q_UNUSED(r);
	//if(a)
	//	;

	return false;
}

bool ConstraintBasicCompulsoryTime::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	//if(t)
	//	;

	return false;
}

bool ConstraintBasicCompulsoryTime::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintBasicCompulsoryTime::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintBasicCompulsoryTime::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherNotAvailableTimes::ConstraintTeacherNotAvailableTimes()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHER_NOT_AVAILABLE_TIMES;
}

ConstraintTeacherNotAvailableTimes::ConstraintTeacherNotAvailableTimes(double wp, const QString& tn, QList<int> d, QList<int> h)
	: TimeConstraint(wp)
{
	this->teacher=tn;
	assert(d.count()==h.count());
	this->days=d;
	this->hours=h;
	this->type=CONSTRAINT_TEACHER_NOT_AVAILABLE_TIMES;
}

QString ConstraintTeacherNotAvailableTimes::getXmlDescription(Rules& r){
	QString s="<ConstraintTeacherNotAvailableTimes>\n";
	s+="	<Weight_Percentage>"+QString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Teacher>"+protect(this->teacher)+"</Teacher>\n";

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

	s+="</ConstraintTeacherNotAvailableTimes>\n";
	return s;
}

QString ConstraintTeacherNotAvailableTimes::getDescription(Rules& r){
	QString s=QObject::tr("Teacher not available");s+=",";
	s+=(QObject::tr("WP:%1\%", "Weight percentage").arg(this->weightPercentage));s+=", ";
	s+=(QObject::tr("T:%1", "Teacher").arg(this->teacher));s+=", ";

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

QString ConstraintTeacherNotAvailableTimes::getDetailedDescription(Rules& r){
	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Teacher not available");s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	s+=(QObject::tr("Teacher=%1").arg(this->teacher));s+="\n";

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

bool ConstraintTeacherNotAvailableTimes::computeInternalStructure(Rules& r){
	this->teacher_ID=r.searchTeacher(this->teacher);

	if(this->teacher_ID<0){
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Constraint teacher not available times is wrong because it refers to inexistent teacher."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}	
	/*if(this->d >= r.nDaysPerWeek){
		QMessageBox::information(NULL, QObject::tr("FET information"),
		 QObject::tr("Constraint teacher not available is wrong because it refers to removed day. Please correct"
		 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}
	if(this->h1 > r.nHoursPerDay){
		QMessageBox::information(NULL, QObject::tr("FET information"),
		 QObject::tr("Constraint teacher not available is wrong because it refers to removed start hour. Please correct"
		 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}
	if(this->h2 > r.nHoursPerDay){
		QMessageBox::information(NULL, QObject::tr("FET information"),
		 QObject::tr("Constraint teacher not available is wrong because it refers to removed end hour. Please correct"
		 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}
	if(this->h1 >= this->h2){
		QMessageBox::information(NULL, QObject::tr("FET information"),
		 QObject::tr("Constraint teacher not available is wrong because start hour >= end hour. Please correct"
		 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}*/
	assert(days.count()==hours.count());
	for(int k=0; k<days.count(); k++){
		if(this->days.at(k) >= r.nDaysPerWeek){
			QMessageBox::information(NULL, QObject::tr("FET information"),
			 QObject::tr("Constraint teacher not available times is wrong because it refers to removed day. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}		
		if(this->hours.at(k) >= r.nHoursPerDay){
			QMessageBox::information(NULL, QObject::tr("FET information"),
			 QObject::tr("Constraint teacher not available times is wrong because an hour is too late (after the last acceptable slot). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
	}

	assert(this->teacher_ID>=0);
	return true;
}

bool ConstraintTeacherNotAvailableTimes::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

double ConstraintTeacherNotAvailableTimes::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString *conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;

		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of hours when the teacher is supposed to be teaching, but he is not available
	//This function consideres all the hours, I mean if there are for example 5 weekly courses
	//scheduled on that hour (which is already a broken compulsory restriction - we only
	//are allowed 1 weekly course for a certain teacher at a certain hour) we calculate
	//5 broken restrictions for that function.
	//TODO: decide if it is better to consider only 2 or 10 as a return value in this particular case
	//(currently it is 10)
	int tch=this->teacher_ID;

	int nbroken;

	nbroken=0;

	assert(days.count()==hours.count());
	for(int k=0; k<days.count(); k++){
		int d=days.at(k);
		int h=hours.at(k);
		
		if(teachersMatrix[tch][d][h]>0){
			nbroken+=teachersMatrix[tch][d][h];
	
			if(conflictsString!=NULL){
				QString s= QObject::tr("Time constraint teacher not available times");
				s += " ";
				s += (QObject::tr("broken for teacher: %1 on day %2, hour %3")
				 .arg(r.internalTeachersList[tch]->name)
				 .arg(r.daysOfTheWeek[d])
				 .arg(r.hoursOfTheDay[h]));
				s += ". ";
				s += (QObject::tr("This increases the conflicts total by %1")
				 .arg(teachersMatrix[tch][d][h]*weightPercentage/100));
				 
				dl.append(s);
				cl.append(teachersMatrix[tch][d][h]*weightPercentage/100);
			
				*conflictsString += s+"\n";
			}
		}
	}

	if(weightPercentage==100.0)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherNotAvailableTimes::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(a);
	Q_UNUSED(r);

	return false;
}

bool ConstraintTeacherNotAvailableTimes::isRelatedToTeacher(Teacher* t)
{
	if(this->teacher==t->name)
		return true;
	return false;
}

bool ConstraintTeacherNotAvailableTimes::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherNotAvailableTimes::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherNotAvailableTimes::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetNotAvailableTimes::ConstraintStudentsSetNotAvailableTimes()
	: TimeConstraint()
{
	this->type=CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE_TIMES;
}

ConstraintStudentsSetNotAvailableTimes::ConstraintStudentsSetNotAvailableTimes(double wp, const QString& sn, QList<int> d, QList<int> h)
	 : TimeConstraint(wp){
	this->students = sn;
	assert(d.count()==h.count());
	this->days=d;
	this->hours=h;
	this->type=CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE_TIMES;
}

bool ConstraintStudentsSetNotAvailableTimes::computeInternalStructure(Rules& r){
	StudentsSet* ss=r.searchAugmentedStudentsSet(this->students);
	
	if(ss==NULL){
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Constraint students set not available is wrong because it refers to inexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}	
	/*if(this->d >= r.nDaysPerWeek){
		QMessageBox::information(NULL, QObject::tr("FET information"),
		 QObject::tr("Constraint students set not available is wrong because it refers to removed day. Please correct"
		 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}
	if(this->h1 > r.nHoursPerDay){
		QMessageBox::information(NULL, QObject::tr("FET information"),
		 QObject::tr("Constraint students set not available is wrong because it refers to removed start hour. Please correct"
		 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}
	if(this->h2 > r.nHoursPerDay){
		QMessageBox::information(NULL, QObject::tr("FET information"),
		 QObject::tr("Constraint students set not available is wrong because it refers to removed end hour. Please correct"
		 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}
	if(this->h1 >= this->h2){
		QMessageBox::information(NULL, QObject::tr("FET information"),
		 QObject::tr("Constraint students set not available is wrong because start hour >= end hour. Please correct"
		 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}*/
	
	assert(days.count()==hours.count());
	for(int k=0; k<days.count(); k++){
		if(this->days.at(k) >= r.nDaysPerWeek){
			QMessageBox::information(NULL, QObject::tr("FET information"),
			 QObject::tr("Constraint students set not available times is wrong because it refers to removed day. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}		
		if(this->hours.at(k) >= r.nHoursPerDay){
			QMessageBox::information(NULL, QObject::tr("FET information"),
			 QObject::tr("Constraint students set not available times is wrong because an hour is too late (after the last acceptable slot). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
	}
	
	assert(ss);

	//this->nSubgroups=0;
	this->iSubgroupsList.clear();
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		/*for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
			if(r.internalSubgroupsList[tmp]->name == ss->name)
				break;*/
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		//assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
		//this->subgroups[this->nSubgroups++]=tmp;
		if(!this->iSubgroupsList.contains(tmp))
			this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			/*for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
				if(r.internalSubgroupsList[tmp]->name == sts->name)
					break;*/
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			//assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
			//this->subgroups[this->nSubgroups++]=tmp;
			if(!this->iSubgroupsList.contains(tmp))
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
				/*for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
					if(r.internalSubgroupsList[tmp]->name == sts->name)
						break;*/
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				//assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
				//this->subgroups[this->nSubgroups++]=tmp;
				if(!this->iSubgroupsList.contains(tmp))
					this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);
	return true;
}

bool ConstraintStudentsSetNotAvailableTimes::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsSetNotAvailableTimes::getXmlDescription(Rules& r){
	QString s="<ConstraintStudentsSetNotAvailableTimes>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Students>"+protect(this->students)+"</Students>\n";

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

	s+="</ConstraintStudentsSetNotAvailableTimes>\n";
	return s;
}

QString ConstraintStudentsSetNotAvailableTimes::getDescription(Rules& r){
	QString s;
	s=QObject::tr("Students set not available times");s+=", ";
	s+=(QObject::tr("WP:%1\%", "Weight percentage").arg(this->weightPercentage));s+=", ";
	s+=(QObject::tr("S:%1", "Students").arg(this->students));s+=", ";

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

QString ConstraintStudentsSetNotAvailableTimes::getDetailedDescription(Rules& r){
	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Students set not available times");s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";

	s+=(QObject::tr("Students=%1").arg(this->students));s+="\n";

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

double ConstraintStudentsSetNotAvailableTimes::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString *conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	nbroken=0;
	for(int m=0; m<this->iSubgroupsList.count(); m++){
		int sbg=this->iSubgroupsList.at(m);
		
		assert(days.count()==hours.count());
		for(int k=0; k<days.count(); k++){
			int d=days.at(k);
			int h=hours.at(k);
			
			if(subgroupsMatrix[sbg][d][h]>0){
				nbroken+=subgroupsMatrix[sbg][d][h];

				if(conflictsString!=NULL){
					QString s= QObject::tr("Time constraint students not available times");
					s += " ";
					s += (QObject::tr("broken for subgroup: %1 on day %2, hour %3")
					 .arg(r.internalSubgroupsList[sbg]->name)
					 .arg(r.daysOfTheWeek[d])
					 .arg(r.hoursOfTheDay[h]));
					s += ". ";
					s += (QObject::tr("This increases the conflicts total by %1")
					 .arg(subgroupsMatrix[sbg][d][h]*weightPercentage/100));
					 
					dl.append(s);
					cl.append(subgroupsMatrix[sbg][d][h]*weightPercentage/100);
				
					*conflictsString += s+"\n";
				}
			}
		}
	}

	if(weightPercentage==100.0)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsSetNotAvailableTimes::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(a);
	Q_UNUSED(r);
	//if(a)
	//	;

	return false;
}

bool ConstraintStudentsSetNotAvailableTimes::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	/*if(t)
		;*/

	return false;
}

bool ConstraintStudentsSetNotAvailableTimes::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	/*if(s)
		;*/

	return false;
}

bool ConstraintStudentsSetNotAvailableTimes::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	/*if(s)
		;*/

	return false;
}

bool ConstraintStudentsSetNotAvailableTimes::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.studentsSetsRelated(this->students, s->name);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivitiesSameStartingTime::ConstraintActivitiesSameStartingTime()
	: TimeConstraint()
{
	type=CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME;
}

ConstraintActivitiesSameStartingTime::ConstraintActivitiesSameStartingTime(double wp, int nact, const int act[])
 : TimeConstraint(wp)
 {
	assert(nact>=2 && nact<=MAX_CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME);
	this->n_activities=nact;
	for(int i=0; i<nact; i++)
		this->activitiesId[i]=act[i];

	this->type=CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME;
}

bool ConstraintActivitiesSameStartingTime::computeInternalStructure(Rules &r)
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
	
	if(this->_n_activities<=1){
		QMessageBox::warning(NULL, QObject::tr("FET error in data"), 
			QObject ::tr("Following constraint is wrong (because you need 2 or more activities. Please correct it):\n%1").arg(this->getDetailedDescription(r)));
		//assert(0);
		return false;
	}

	return true;
}

void ConstraintActivitiesSameStartingTime::removeUseless(Rules& r)
{
	//remove the activitiesId which no longer exist (used after the deletion of an activity)

	for(int j=0; j<this->n_activities; j++)
		this->_activities[j]=-1;

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

bool ConstraintActivitiesSameStartingTime::hasInactiveActivities(Rules& r)
{
	int count=0;

	for(int i=0; i<this->n_activities; i++)
		if(r.inactiveActivities.contains(this->activitiesId[i]))
			count++;

	if(this->n_activities-count<=1)
		return true;
	else
		return false;
}

QString ConstraintActivitiesSameStartingTime::getXmlDescription(Rules& r){
	Q_UNUSED(r);
	/*if(&r!=NULL)
		;*/

	QString s="<ConstraintActivitiesSameStartingTime>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Number_of_Activities>"+QString::number(this->n_activities)+"</Number_of_Activities>\n";
	for(int i=0; i<this->n_activities; i++)
		s+="	<Activity_Id>"+QString::number(this->activitiesId[i])+"</Activity_Id>\n";
	s+="</ConstraintActivitiesSameStartingTime>\n";
	return s;
}

QString ConstraintActivitiesSameStartingTime::getDescription(Rules& r){
	Q_UNUSED(r);
	/*if(&r!=NULL)
		;*/

	QString s;
	s+=QObject::tr("Activities same starting time");s+=", ";
	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
	s+=(QObject::tr("NA:%1").arg(this->n_activities));s+=", ";
	for(int i=0; i<this->n_activities; i++){
		s+=(QObject::tr("ID:%1").arg(this->activitiesId[i]));s+=", ";
	}

	return s;
}

QString ConstraintActivitiesSameStartingTime::getDetailedDescription(Rules& r){
	Q_UNUSED(r);
	/*if(&r!=NULL)
		;*/

	QString s;
	
	s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Activities must have the same starting time");s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	s+=(QObject::tr("Number of activities=%1").arg(this->n_activities));s+="\n";
	for(int i=0; i<this->n_activities; i++){
		s+=(QObject::tr("Activity with id=%1").arg(this->activitiesId[i]));

		//* write the teachers, subject and students sets
		//added in version 5.1.10
		int ai;
		for(ai=0; ai<r.activitiesList.size(); ai++)
			if(r.activitiesList[ai]->id==this->activitiesId[i])
				break;
		if(ai==r.activitiesList.size()){
			s+=QObject::tr(" Invalid (inexistent) id for activity");
			s+="\n";
			return s;
		}
		assert(ai<r.activitiesList.size());
		s+=" ( ";
	
		s+=QObject::tr("T: ");
		int k=0;
		foreach(QString ss, r.activitiesList[ai]->teachersNames){
			if(k>0)
				s+=" ,";
			s+=ss;
			k++;
		}
	
		s+=QObject::tr(" , S: ");
		s+=r.activitiesList[ai]->subjectName;
		
		if(r.activitiesList[ai]->activityTagName!="")
			s+=QObject::tr(" , AT: ", "Activity tag")+r.activitiesList[ai]->activityTagName;
	
		s+=QObject::tr(" , St: ");
		k=0;
		foreach(QString ss, r.activitiesList[ai]->studentsNames){
			if(k>0)
				s+=",";
			s+=ss;
			k++;
		}
	
		s+=" )";
		//* end section
		
		s+="\n";
	}

	return s;
}

double ConstraintActivitiesSameStartingTime::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	assert(r.internalStructureComputed);

	int nbroken;

	//We do not use the matrices 'subgroupsMatrix' nor 'teachersMatrix'.

	//sum the differences in the scheduled time for all pairs of activities.

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int i=1; i<this->_n_activities; i++){
			int t1=c.times[this->_activities[i]];
			if(t1!=UNALLOCATED_TIME){
				int day1=t1%r.nDaysPerWeek;
				int hour1=t1/r.nDaysPerWeek;
				for(int j=0; j<i; j++){
					int t2=c.times[this->_activities[j]];
					if(t2!=UNALLOCATED_TIME){
						int day2=t2%r.nDaysPerWeek;
						int hour2=t2/r.nDaysPerWeek;
						int tmp=0;

						//activity weekly - counts as double
						/*if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY &&
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 4 * (abs(day1-day2) + abs(hour1-hour2));
						else if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY ||
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 2 * (abs(day1-day2) + abs(hour1-hour2));
						else*/
							tmp = abs(day1-day2) + abs(hour1-hour2);
							
						if(tmp>0)
							tmp=1;

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
			int t1=c.times[this->_activities[i]];
			if(t1!=UNALLOCATED_TIME){
				int day1=t1%r.nDaysPerWeek;
				int hour1=t1/r.nDaysPerWeek;
				for(int j=0; j<i; j++){
					int t2=c.times[this->_activities[j]];
					if(t2!=UNALLOCATED_TIME){
						int day2=t2%r.nDaysPerWeek;
						int hour2=t2/r.nDaysPerWeek;
						int tmp=0;

						//activity weekly - counts as double
						/*if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY &&
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 4 * (abs(day1-day2) + abs(hour1-hour2));
						else if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY ||
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 2 * (abs(day1-day2) + abs(hour1-hour2));
						else*/
							tmp = abs(day1-day2) + abs(hour1-hour2);
							
						if(tmp>0)
							tmp=1;

						nbroken+=tmp;

						if(tmp>0 && conflictsString!=NULL){
							QString s=QObject::tr("Time constraint activities same starting time broken, because activity with id=%1 is not at the same starting time with activity with id=%2")
							 .arg(this->activitiesId[i])
							 .arg(this->activitiesId[j]);
							s+=", ";
							s+=(QObject::tr("conflicts factor increase=%1").arg(tmp*weightPercentage/100));
						
							dl.append(s);
							cl.append(tmp*weightPercentage/100);
							
							*conflictsString+= s+"\n";
						}
					}
				}
			}
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintActivitiesSameStartingTime::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);

	for(int i=0; i<this->n_activities; i++)
		if(this->activitiesId[i]==a->id)
			return true;
	return false;
}

bool ConstraintActivitiesSameStartingTime::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintActivitiesSameStartingTime::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintActivitiesSameStartingTime::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintActivitiesSameStartingTime::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivitiesNotOverlapping::ConstraintActivitiesNotOverlapping()
	: TimeConstraint()
{
	type=CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING;
}

ConstraintActivitiesNotOverlapping::ConstraintActivitiesNotOverlapping(double wp, int nact, const int act[])
 : TimeConstraint(wp)
 {
  	assert(nact>=2 && nact<=MAX_CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING);
	this->n_activities=nact;
	for(int i=0; i<nact; i++)
		this->activitiesId[i]=act[i];

	this->type=CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING;
}

bool ConstraintActivitiesNotOverlapping::computeInternalStructure(Rules &r)
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
	
	if(this->_n_activities<=1){
		QMessageBox::warning(NULL, QObject::tr("FET error in data"), 
			QObject ::tr("Following constraint is wrong (because you need 2 or more activities. Please correct it):\n%1").arg(this->getDetailedDescription(r)));
		//assert(0);
		return false;
	}

	return true;
}

void ConstraintActivitiesNotOverlapping::removeUseless(Rules& r)
{
	//remove the activitiesId which no longer exist (used after the deletion of an activity)

	for(int j=0; j<this->n_activities; j++)
		this->_activities[j]=-1;

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

bool ConstraintActivitiesNotOverlapping::hasInactiveActivities(Rules& r)
{
	int count=0;

	for(int i=0; i<this->n_activities; i++)
		if(r.inactiveActivities.contains(this->activitiesId[i]))
			count++;

	if(this->n_activities-count<=1)
		return true;
	else
		return false;
}

QString ConstraintActivitiesNotOverlapping::getXmlDescription(Rules& r){
	Q_UNUSED(r);
	/*if(&r!=NULL)
		;*/

	QString s="<ConstraintActivitiesNotOverlapping>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Number_of_Activities>"+QString::number(this->n_activities)+"</Number_of_Activities>\n";
	for(int i=0; i<this->n_activities; i++)
		s+="	<Activity_Id>"+QString::number(this->activitiesId[i])+"</Activity_Id>\n";
	s+="</ConstraintActivitiesNotOverlapping>\n";
	return s;
}

QString ConstraintActivitiesNotOverlapping::getDescription(Rules& r){
	Q_UNUSED(r);
	/*if(&r!=NULL)
		;*/

	QString s;
	s+=QObject::tr("Activities not overlapping");s+=", ";
	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
	s+=(QObject::tr("NA:%1").arg(this->n_activities));s+=", ";
	for(int i=0; i<this->n_activities; i++){
		s+=(QObject::tr("ID:%1").arg(this->activitiesId[i]));s+=", ";
	}

	return s;
}

QString ConstraintActivitiesNotOverlapping::getDetailedDescription(Rules& r){
	Q_UNUSED(r);
	/*if(&r!=NULL)
		;*/

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Activities must not overlap");s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	s+=(QObject::tr("Number of activities=%1").arg(this->n_activities));s+="\n";
	for(int i=0; i<this->n_activities; i++){
		s+=(QObject::tr("Activity with id=%1").arg(this->activitiesId[i]));

		//* write the teachers, subject and students sets
		//added in version 5.1.10
		int ai;
		for(ai=0; ai<r.activitiesList.size(); ai++)
			if(r.activitiesList[ai]->id==this->activitiesId[i])
				break;
		if(ai==r.activitiesList.size()){
			s+=QObject::tr(" Invalid (inexistent) id for activity");
			s+="\n";
			return s;
		}
		assert(ai<r.activitiesList.size());
		s+=" ( ";
	
		s+=QObject::tr("T: ");
		int k=0;
		foreach(QString ss, r.activitiesList[ai]->teachersNames){
			if(k>0)
				s+=" ,";
			s+=ss;
			k++;
		}
	
		s+=QObject::tr(" , S: ");
		s+=r.activitiesList[ai]->subjectName;
	
		if(r.activitiesList[ai]->activityTagName!="")
			s+=QObject::tr(" , AT: ", "Activity tag")+r.activitiesList[ai]->activityTagName;
	
		s+=QObject::tr(" , St: ");
		k=0;
		foreach(QString ss, r.activitiesList[ai]->studentsNames){
			if(k>0)
				s+=",";
			s+=ss;
			k++;
		}
	
		s+=" )";
		//* end section
		
		s+="\n";
	}

	return s;
}

double ConstraintActivitiesNotOverlapping::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	assert(r.internalStructureComputed);

	int nbroken;

	//We do not use the matrices 'subgroupsMatrix' nor 'teachersMatrix'.

	//sum the overlapping hours for all pairs of activities.
	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int i=1; i<this->_n_activities; i++){
			int t1=c.times[this->_activities[i]];
			if(t1!=UNALLOCATED_TIME){
				int day1=t1%r.nDaysPerWeek;
				int hour1=t1/r.nDaysPerWeek;
				int duration1=r.internalActivitiesList[this->_activities[i]].duration;

				for(int j=0; j<i; j++){
					int t2=c.times[this->_activities[j]];
					if(t2!=UNALLOCATED_TIME){
						int day2=t2%r.nDaysPerWeek;
						int hour2=t2/r.nDaysPerWeek;
						int duration2=r.internalActivitiesList[this->_activities[j]].duration;

						//the number of overlapping hours
						int tt=0;
						if(day1==day2){
							int start=maximu(hour1, hour2);
							int stop=minimu(hour1+duration1, hour2+duration2);
							if(stop>start)
								tt+=stop-start;
						}
						//activity weekly - counts as double
						/*if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY &&
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tt = 4 * tt;
						else if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY ||
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tt = 2 * tt;*/
						/*else
							tt = tt;*/
						
						nbroken+=tt;
					}
				}
			}
		}
	}
	//with logging
	else{
		nbroken=0;
		for(int i=1; i<this->_n_activities; i++){
			int t1=c.times[this->_activities[i]];
			if(t1!=UNALLOCATED_TIME){
				int day1=t1%r.nDaysPerWeek;
				int hour1=t1/r.nDaysPerWeek;
				int duration1=r.internalActivitiesList[this->_activities[i]].duration;

				for(int j=0; j<i; j++){
					int t2=c.times[this->_activities[j]];
					if(t2!=UNALLOCATED_TIME){
						int day2=t2%r.nDaysPerWeek;
						int hour2=t2/r.nDaysPerWeek;
						int duration2=r.internalActivitiesList[this->_activities[j]].duration;
	
						//the number of overlapping hours
						int tt=0;
						if(day1==day2){
							int start=maximu(hour1, hour2);
							int stop=minimu(hour1+duration1, hour2+duration2);
							if(stop>start)
								tt+=stop-start;
						}

						//The overlapping hours, considering weekly activities more important than fortnightly ones
						int tmp=tt;

						//activity weekly - counts as double
						/*if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY &&
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 4 * tmp;
						else if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY ||
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 2 * tmp;*/
						/*else
							tmp = tmp;*/

						nbroken+=tmp;

						if(tt>0 && conflictsString!=NULL){
							QString ss;
							ss=QObject::tr("activity with id=%1 overlaps with activity with id=%2 on a number of %3 periods")
							 .arg(this->activitiesId[i])
							 .arg(this->activitiesId[j])
							 .arg(tt);
							cout<<qPrintable(ss)<<endl;
							cout<<"activity1: id=="<<r.internalActivitiesList[this->activitiesId[i]].id;
							cout<<", duration=="<<r.internalActivitiesList[this->activitiesId[i]].duration;
							cout<<", day=="<<qPrintable(r.daysOfTheWeek[day1]);
							cout<<", hour=="<<qPrintable(r.hoursOfTheDay[hour1]);
							cout<<endl;
							cout<<"activity2: id=="<<r.internalActivitiesList[this->activitiesId[j]].id;
							cout<<", duration=="<<r.internalActivitiesList[this->activitiesId[j]].duration;
							cout<<", day=="<<qPrintable(r.daysOfTheWeek[day2]);
							cout<<", hour=="<<qPrintable(r.hoursOfTheDay[hour2]);
							cout<<endl;
														

							QString s=QObject::tr("Time constraint activities not overlapping");
							s+=" ";
							s+=QObject::tr("broken:");
							s+=" ";
							s+=(QObject::tr("activity with id=%1 overlaps with activity with id=%2 on a number of %3 periods")
							 .arg(this->activitiesId[i])
							 .arg(this->activitiesId[j])
							 .arg(tt));
							s+=", ";
							s+=(QObject::tr("conflicts factor increase=%1").arg(tmp*weightPercentage/100));
							
							dl.append(s);
							cl.append(tmp*weightPercentage/100);
						
							*conflictsString+= s+"\n";
						}
					}
				}
			}
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintActivitiesNotOverlapping::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);

	for(int i=0; i<this->n_activities; i++)
		if(this->activitiesId[i]==a->id)
			return true;
	return false;
}

bool ConstraintActivitiesNotOverlapping::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintActivitiesNotOverlapping::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintActivitiesNotOverlapping::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintActivitiesNotOverlapping::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintMinNDaysBetweenActivities::ConstraintMinNDaysBetweenActivities()
	: TimeConstraint()
{
	type=CONSTRAINT_MIN_N_DAYS_BETWEEN_ACTIVITIES;
}

ConstraintMinNDaysBetweenActivities::ConstraintMinNDaysBetweenActivities(double wp, bool cisd, int nact, const int act[], int n)
 : TimeConstraint(wp)
 {
 	this->consecutiveIfSameDay=cisd;
 
  	assert(nact>=2 && nact<=MAX_CONSTRAINT_MIN_N_DAYS_BETWEEN_ACTIVITIES);
	this->n_activities=nact;
	for(int i=0; i<nact; i++)
		this->activitiesId[i]=act[i];

	assert(n>0);
	this->minDays=n;

	this->type=CONSTRAINT_MIN_N_DAYS_BETWEEN_ACTIVITIES;
}

bool ConstraintMinNDaysBetweenActivities::operator==(ConstraintMinNDaysBetweenActivities& c){
	if(this->n_activities!=c.n_activities)
		return false;
	for(int i=0; i<this->n_activities; i++)
		if(this->activitiesId[i]!=c.activitiesId[i])
			return false;
	if(this->minDays!=c.minDays)
		return false;
	if(this->weightPercentage!=c.weightPercentage)
		return false;
	if(this->consecutiveIfSameDay!=c.consecutiveIfSameDay)
		return false;
	return true;
}

bool ConstraintMinNDaysBetweenActivities::computeInternalStructure(Rules &r)
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
	
	if(this->_n_activities<=1){
		QMessageBox::warning(NULL, QObject::tr("FET error in data"), 
			QObject ::tr("Following constraint is wrong (because you need 2 or more activities. Please correct it):\n%1").arg(this->getDetailedDescription(r)));
		//assert(0);
		return false;
	}

	return true;
}

void ConstraintMinNDaysBetweenActivities::removeUseless(Rules& r)
{
	//remove the activitiesId which no longer exist (used after the deletion of an activity)

	for(int j=0; j<this->n_activities; j++)
		this->_activities[j]=-1;

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

bool ConstraintMinNDaysBetweenActivities::hasInactiveActivities(Rules& r)
{
	int count=0;

	for(int i=0; i<this->n_activities; i++)
		if(r.inactiveActivities.contains(this->activitiesId[i]))
			count++;

	if(this->n_activities-count<=1)
		return true;
	else
		return false;
}

QString ConstraintMinNDaysBetweenActivities::getXmlDescription(Rules& r){
	Q_UNUSED(r);
	/*if(&r!=NULL)
		;*/

	QString s="<ConstraintMinNDaysBetweenActivities>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Consecutive_If_Same_Day>";s+=yesNo(this->consecutiveIfSameDay);s+="</Consecutive_If_Same_Day>\n";
	s+="	<Number_of_Activities>"+QString::number(this->n_activities)+"</Number_of_Activities>\n";
	for(int i=0; i<this->n_activities; i++)
		s+="	<Activity_Id>"+QString::number(this->activitiesId[i])+"</Activity_Id>\n";
	s+="	<MinDays>"+QString::number(this->minDays)+"</MinDays>\n";
	s+="</ConstraintMinNDaysBetweenActivities>\n";
	return s;
}

QString ConstraintMinNDaysBetweenActivities::getDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s;
	s+=QObject::tr("Min N days between activities");s+=", ";
	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
	s+=(QObject::tr("CSD:%1").arg(yesNoTranslated(this->consecutiveIfSameDay)));s+=", ";
	s+=(QObject::tr("NA:%1").arg(this->n_activities));s+=", ";
	for(int i=0; i<this->n_activities; i++){
		s+=(QObject::tr("ID:%1").arg(this->activitiesId[i]));s+=", ";
	}
	s+=(QObject::tr("N:%1").arg(this->minDays));

	return s;
}

QString ConstraintMinNDaysBetweenActivities::getDetailedDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Minimum N days between activities");s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	s+=(QObject::tr("Consecutive if same day=%1").arg(yesNoTranslated(this->consecutiveIfSameDay)));s+="\n";
	s+=(QObject::tr("Number of activities=%1").arg(this->n_activities));s+="\n";
	for(int i=0; i<this->n_activities; i++){
		s+=(QObject::tr("Activity with id=%1").arg(this->activitiesId[i]));
		
		//* write the teachers, subject and students sets
		//added in version 5.1.10
		int ai;
		for(ai=0; ai<r.activitiesList.size(); ai++)
			if(r.activitiesList[ai]->id==this->activitiesId[i])
				break;
		if(ai==r.activitiesList.size()){
			s+=QObject::tr(" Invalid (inexistent) id for activity");
			s+="\n";
			return s;
		}
		assert(ai<r.activitiesList.size());
		s+=" ( ";
	
		s+=QObject::tr("T: ");
		int k=0;
		foreach(QString ss, r.activitiesList[ai]->teachersNames){
			if(k>0)
				s+=" ,";
			s+=ss;
			k++;
		}
	
		s+=QObject::tr(" , S: ");
		s+=r.activitiesList[ai]->subjectName;
	
		if(r.activitiesList[ai]->activityTagName!="")
			s+=QObject::tr(" , AT: ", "Activity tag")+r.activitiesList[ai]->activityTagName;
	
		s+=QObject::tr(" , St: ");
		k=0;
		foreach(QString ss, r.activitiesList[ai]->studentsNames){
			if(k>0)
				s+=",";
			s+=ss;
			k++;
		}
	
		s+=" )";
		//* end section
		
		s+="\n";

	}
	s+=(QObject::tr("Minimum number of days=%1").arg(this->minDays));s+="\n";

	return s;
}

double ConstraintMinNDaysBetweenActivities::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	assert(r.internalStructureComputed);

	int nbroken;

	//We do not use the matrices 'subgroupsMatrix' nor 'teachersMatrix'.

	//sum the overlapping hours for all pairs of activities.
	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int i=1; i<this->_n_activities; i++){
			int t1=c.times[this->_activities[i]];
			if(t1!=UNALLOCATED_TIME){
				int day1=t1%r.nDaysPerWeek;
				int hour1=t1/r.nDaysPerWeek;
				int duration1=r.internalActivitiesList[this->_activities[i]].duration;

				for(int j=0; j<i; j++){
					int t2=c.times[this->_activities[j]];
					if(t2!=UNALLOCATED_TIME){
						int day2=t2%r.nDaysPerWeek;
						int hour2=t2/r.nDaysPerWeek;
						int duration2=r.internalActivitiesList[this->_activities[j]].duration;
					
						int tmp;
						int tt=0;
						int dist=abs(day1-day2);
						if(dist<minDays){
							tt=minDays-dist;
							
							if(this->consecutiveIfSameDay && day1==day2)
								assert( day1==day2 && (hour1+duration1==hour2 || hour2+duration2==hour1) );
						}
						
						//NOT: conflicts increase as the activities are longer
						//tt*=duration1*duration2;
					
						tmp=tt;
	
						//activity weekly - counts as double
						/*if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY &&
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 4 * tmp;
						else if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY ||
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 2 * tmp;*/
						/*else
							tmp = tmp;*/

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
			int t1=c.times[this->_activities[i]];
			if(t1!=UNALLOCATED_TIME){
				int day1=t1%r.nDaysPerWeek;
				int hour1=t1/r.nDaysPerWeek;
				int duration1=r.internalActivitiesList[this->_activities[i]].duration;

				for(int j=0; j<i; j++){
					int t2=c.times[this->_activities[j]];
					if(t2!=UNALLOCATED_TIME){
						int day2=t2%r.nDaysPerWeek;
						int hour2=t2/r.nDaysPerWeek;
						int duration2=r.internalActivitiesList[this->_activities[j]].duration;
					
						int tmp;
						int tt=0;
						int dist=abs(day1-day2);

						if(dist<minDays){
							tt=minDays-dist;
							
							if(this->consecutiveIfSameDay && day1==day2)
								assert( day1==day2 && (hour1+duration1==hour2 || hour2+duration2==hour1) );
						}

						//NOT: conflicts increase as the activities are longer
						//tt*=duration1*duration2;
					
						tmp=tt;
	
						//activity weekly - counts as double
						/*if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY &&
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 4 * tmp;
						else if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY ||
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 2 * tmp;*/
						/*else
							tmp = tmp;*/

						nbroken+=tmp;

						if(tt>0 && conflictsString!=NULL){
							QString s=QObject::tr("Time constraint min n days between activities");
							s+=" ";
							s+=QObject::tr("broken:");
							s+=" ";
							s+=(QObject::tr("activity with id=%1 conflicts with activity with id=%2, being %3 days too close",
							 "Note for translators: close here means near")
							 .arg(this->activitiesId[i])
							 .arg(this->activitiesId[j])
							 .arg(tt));
							s+=", ";
							s+=(QObject::tr("on days %1 and %2")
							 .arg(r.daysOfTheWeek[day1])
							 .arg(r.daysOfTheWeek[day2]));
							s+=", ";
							
							QString tn1;
							foreach(QString t, r.internalActivitiesList[this->_activities[i]].teachersNames){
								tn1+=t;
								tn1+=" ";
							}
							QString sn1;
							foreach(QString s, r.internalActivitiesList[this->_activities[i]].studentsNames){
								sn1+=s;
								sn1+=" ";
							}
							QString tn2;
							foreach(QString t, r.internalActivitiesList[this->_activities[j]].teachersNames){
								tn2+=t;
								tn2+=" ";
							}
							QString sn2;
							foreach(QString s, r.internalActivitiesList[this->_activities[j]].studentsNames){
								sn2+=s;
								sn2+=" ";
							}
							
							s+=(QObject::tr("teachers1 %1, students sets1 %2, subject1 %3")
							 .arg(tn1)
							 .arg(sn1)
							 .arg(r.internalActivitiesList[this->_activities[i]].subjectName));

							if(r.internalActivitiesList[this->_activities[i]].activityTagName!="")
								s+=QObject::tr(", activity tag %4").arg(r.internalActivitiesList[this->_activities[i]].activityTagName);

							s+=", ";
							s+=(QObject::tr("teachers2 %1, students sets2 %2, subject2 %3")
							 .arg(tn2)
							 .arg(sn2)
							 .arg(r.internalActivitiesList[this->_activities[j]].subjectName));

							if(r.internalActivitiesList[this->_activities[j]].activityTagName!="")
								s+=QObject::tr(", activity tag %4").arg(r.internalActivitiesList[this->_activities[j]].activityTagName);

							s+=", ";
							s+=(QObject::tr("conflicts factor increase=%1").arg(tmp*weightPercentage/100));
							//s+="\n";
							s+=".";
							
							if(this->consecutiveIfSameDay && day1==day2){
								s+=" ";
								s+=QObject::tr("The activities are placed consecutively in the timetable, because you selected this option"
								 " in case the activities are in the same day");
							}
							
							dl.append(s);
							cl.append(tmp*weightPercentage/100);
							
							*conflictsString+= s+"\n";
						}
					}
				}
			}
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintMinNDaysBetweenActivities::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);

	for(int i=0; i<this->n_activities; i++)
		if(this->activitiesId[i]==a->id)
			return true;
	return false;
}

bool ConstraintMinNDaysBetweenActivities::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintMinNDaysBetweenActivities::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintMinNDaysBetweenActivities::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintMinNDaysBetweenActivities::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintMinGapsBetweenActivities::ConstraintMinGapsBetweenActivities()
	: TimeConstraint()
{
	type=CONSTRAINT_MIN_GAPS_BETWEEN_ACTIVITIES;
}

ConstraintMinGapsBetweenActivities::ConstraintMinGapsBetweenActivities(double wp, int nact, const int act[], int ngaps)
 : TimeConstraint(wp)
 {
 	//assert(nact>=2 && nact<=MAX_CONSTRAINT_MIN_GAPS_BETWEEN_ACTIVITIES);
	this->n_activities=nact;
	this->activitiesId.clear();
	for(int i=0; i<nact; i++)
		this->activitiesId.append(act[i]);

	assert(ngaps>0);
	this->minGaps=ngaps;

	this->type=CONSTRAINT_MIN_GAPS_BETWEEN_ACTIVITIES;
}

bool ConstraintMinGapsBetweenActivities::computeInternalStructure(Rules &r)
{
	//compute the indices of the activities,
	//based on their unique ID

	//for(int j=0; j<n_activities; j++)
	//	this->_activities[j]=-1;

	this->_n_activities=0;
	this->_activities.clear();
	assert(this->n_activities==this->activitiesId.count());
	for(int i=0; i<this->n_activities; i++){
		int j;
		Activity* act;
		for(j=0; j<r.nInternalActivities; j++){
			act=&r.internalActivitiesList[j];
			if(act->id==this->activitiesId[i]){
				this->_activities.append(j);
				this->_n_activities++;
				break;
			}
		}
	}
	assert(this->_n_activities==this->_activities.count());
	
	if(this->_n_activities<=1){
		QMessageBox::warning(NULL, QObject::tr("FET error in data"), 
			QObject ::tr("Following constraint is wrong (because you need 2 or more activities. Please correct it):\n%1").arg(this->getDetailedDescription(r)));
		//assert(0);
		return false;
	}

	return true;
}

void ConstraintMinGapsBetweenActivities::removeUseless(Rules& r)
{
	//remove the activitiesId which no longer exist (used after the deletion of an activity)

	this->_activities.clear();
	this->_n_activities=0;
	//for(int j=0; j<this->n_activities; j++)
	//	this->_activities[j]=-1;

	for(int i=0; i<this->n_activities; i++){
		for(int k=0; k<r.activitiesList.size(); k++){
			Activity* act=r.activitiesList[k];
			if(act->id==this->activitiesId[i]){
				this->_activities.append(act->id);
				this->_n_activities++;
			}
		}
	}
	assert(this->_n_activities==this->_activities.count());

	int i, j;
	i=0;
	for(j=0; j<this->_n_activities; j++){
		//assert(j<this->_activities.count());
		this->activitiesId[i++]=this->_activities[j];
	}
	this->n_activities=i;
}

bool ConstraintMinGapsBetweenActivities::hasInactiveActivities(Rules& r)
{
	int count=0;

	for(int i=0; i<this->n_activities; i++)
		if(r.inactiveActivities.contains(this->activitiesId[i]))
			count++;

	if(this->n_activities-count<=1)
		return true;
	else
		return false;
}

QString ConstraintMinGapsBetweenActivities::getXmlDescription(Rules& r){
	Q_UNUSED(r);
	/*if(&r!=NULL)
		;*/

	QString s="<ConstraintMinGapsBetweenActivities>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Number_of_Activities>"+QString::number(this->n_activities)+"</Number_of_Activities>\n";
	for(int i=0; i<this->n_activities; i++)
		s+="	<Activity_Id>"+QString::number(this->activitiesId[i])+"</Activity_Id>\n";
	s+="	<MinGaps>"+QString::number(this->minGaps)+"</MinGaps>\n";
	s+="</ConstraintMinGapsBetweenActivities>\n";
	return s;
}

QString ConstraintMinGapsBetweenActivities::getDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s;
	s+=QObject::tr("Min gaps between activities");s+=", ";
	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
	//s+=(QObject::tr("CSD:%1").arg(yesNoTranslated(this->consecutiveIfSameDay)));s+=", ";
	s+=(QObject::tr("NA:%1").arg(this->n_activities));s+=", ";
	for(int i=0; i<this->n_activities; i++){
		s+=(QObject::tr("ID:%1").arg(this->activitiesId[i]));s+=", ";
	}
	s+=(QObject::tr("MG:%1", "Min gaps").arg(this->minGaps));

	return s;
}

QString ConstraintMinGapsBetweenActivities::getDetailedDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Minimum gaps between activities (if activities on the same day)");s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	//s+=(QObject::tr("Consecutive if same day=%1").arg(yesNoTranslated(this->consecutiveIfSameDay)));s+="\n";
	s+=(QObject::tr("Number of activities=%1").arg(this->n_activities));s+="\n";
	for(int i=0; i<this->n_activities; i++){
		s+=(QObject::tr("Activity with id=%1").arg(this->activitiesId[i]));
		
		//* write the teachers, subject and students sets
		//added in version 5.1.10
		int ai;
		for(ai=0; ai<r.activitiesList.size(); ai++)
			if(r.activitiesList[ai]->id==this->activitiesId[i])
				break;
		if(ai==r.activitiesList.size()){
			s+=QObject::tr(" Invalid (inexistent) id for activity");
			s+="\n";
			return s;
		}
		assert(ai<r.activitiesList.size());
		s+=" ( ";
	
		s+=QObject::tr("T: ");
		int k=0;
		foreach(QString ss, r.activitiesList[ai]->teachersNames){
			if(k>0)
				s+=" ,";
			s+=ss;
			k++;
		}
	
		s+=QObject::tr(" , S: ");
		s+=r.activitiesList[ai]->subjectName;
	
		if(r.activitiesList[ai]->activityTagName!="")
			s+=QObject::tr(" , AT: ", "Activity tag")+r.activitiesList[ai]->activityTagName;
	
		s+=QObject::tr(" , St: ");
		k=0;
		foreach(QString ss, r.activitiesList[ai]->studentsNames){
			if(k>0)
				s+=",";
			s+=ss;
			k++;
		}
	
		s+=" )";
		//* end section
		
		s+="\n";

	}
	s+=(QObject::tr("Minimum number of gaps=%1").arg(this->minGaps));s+="\n";

	return s;
}

double ConstraintMinGapsBetweenActivities::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	assert(r.internalStructureComputed);

	int nbroken;

	//We do not use the matrices 'subgroupsMatrix' nor 'teachersMatrix'.

	nbroken=0;
	for(int i=1; i<this->_n_activities; i++){
		int t1=c.times[this->_activities[i]];
		if(t1!=UNALLOCATED_TIME){
			int day1=t1%r.nDaysPerWeek;
			int hour1=t1/r.nDaysPerWeek;
			int duration1=r.internalActivitiesList[this->_activities[i]].duration;

			for(int j=0; j<i; j++){
				int t2=c.times[this->_activities[j]];
				if(t2!=UNALLOCATED_TIME){
					int day2=t2%r.nDaysPerWeek;
					int hour2=t2/r.nDaysPerWeek;
					int duration2=r.internalActivitiesList[this->_activities[j]].duration;
				
					int tmp;
					int tt=0;
					int dist=abs(day1-day2);
					
					if(dist==0){ //same day
						assert(day1==day2);
						if(hour2>=hour1){
							assert(hour1+duration1<=hour2);
							if(hour1+duration1+minGaps > hour2)
								tt = (hour1+duration1+minGaps) - hour2;
						}
						else{
							assert(hour2+duration2<=hour1);
							if(hour2+duration2+minGaps > hour1)
								tt = (hour2+duration2+minGaps) - hour1;
						}
					}

					tmp=tt;
	
					nbroken+=tmp;

					if(tt>0 && conflictsString!=NULL){
						QString s=QObject::tr("Time constraint min gaps between activities");
						s+=" ";
						s+=QObject::tr("broken:");
						s+=" ";
						s+=(QObject::tr("activity with id=%1 conflicts with activity with id=%2, they are on the same day %3 and there are %4 extra hours between them")
						 .arg(this->activitiesId[i])
						 .arg(this->activitiesId[j])
						 .arg(r.daysOfTheWeek[day1])
						 .arg(tt));

						s+=", ";
						
						QString tn1;
						foreach(QString t, r.internalActivitiesList[this->_activities[i]].teachersNames){
							tn1+=t;
							tn1+=" ";
						}
						QString sn1;
						foreach(QString s, r.internalActivitiesList[this->_activities[i]].studentsNames){
							sn1+=s;
							sn1+=" ";
						}
						QString tn2;
						foreach(QString t, r.internalActivitiesList[this->_activities[j]].teachersNames){
							tn2+=t;
							tn2+=" ";
						}
						QString sn2;
						foreach(QString s, r.internalActivitiesList[this->_activities[j]].studentsNames){
							sn2+=s;
							sn2+=" ";
						}
						
						s+=(QObject::tr("teachers1 %1, students sets1 %2, subject1 %3")
						 .arg(tn1)
						 .arg(sn1)
						 .arg(r.internalActivitiesList[this->_activities[i]].subjectName));

						if(r.internalActivitiesList[this->_activities[i]].activityTagName!=""){
							s+=", ";
							s+=QObject::tr("activity tag %4").arg(r.internalActivitiesList[this->_activities[i]].activityTagName);
						}

						s+=", ";
						s+=(QObject::tr("teachers2 %1, students sets2 %2, subject2 %3")
						 .arg(tn2)
						 .arg(sn2)
						 .arg(r.internalActivitiesList[this->_activities[j]].subjectName));

						if(r.internalActivitiesList[this->_activities[j]].activityTagName!=""){
							s+=", ";
							s+=QObject::tr("activity tag %4").arg(r.internalActivitiesList[this->_activities[j]].activityTagName);
						}
	
						s+=", ";
						s+=(QObject::tr("conflicts factor increase=%1").arg(tmp*weightPercentage/100));
						//s+="\n";
						s+=".";
							
						dl.append(s);
						cl.append(tmp*weightPercentage/100);
							
						*conflictsString+= s+"\n";
					}
				}
			}
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintMinGapsBetweenActivities::isRelatedToActivity(
Rules& r, Activity* a)
{
	Q_UNUSED(r);

	for(int i=0; i<this->n_activities; i++)
		if(this->activitiesId[i]==a->id)
			return true;
	return false;
}

bool ConstraintMinGapsBetweenActivities::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintMinGapsBetweenActivities::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintMinGapsBetweenActivities::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintMinGapsBetweenActivities::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersMaxHoursDaily::ConstraintTeachersMaxHoursDaily()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHERS_MAX_HOURS_DAILY;
}

ConstraintTeachersMaxHoursDaily::ConstraintTeachersMaxHoursDaily(double wp, int maxhours)
 : TimeConstraint(wp)
 {
	assert(maxhours>0);
	this->maxHoursDaily=maxhours;

	this->type=CONSTRAINT_TEACHERS_MAX_HOURS_DAILY;
}

bool ConstraintTeachersMaxHoursDaily::computeInternalStructure(Rules& r)
{
	Q_UNUSED(r);
	/*if(&r!=NULL)
		;*/

	return true;
}

bool ConstraintTeachersMaxHoursDaily::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeachersMaxHoursDaily::getXmlDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="<ConstraintTeachersMaxHoursDaily>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Maximum_Hours_Daily>"+QString::number(this->maxHoursDaily)+"</Maximum_Hours_Daily>\n";
	s+="</ConstraintTeachersMaxHoursDaily>\n";
	return s;
}

QString ConstraintTeachersMaxHoursDaily::getDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s;
	s+=(QObject::tr("Teachers max %1 hours daily").arg(this->maxHoursDaily));s+=", ";
	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));//s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintTeachersMaxHoursDaily::getDetailedDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=(QObject::tr("Teachers must not have more than %1 hours daily").arg(this->maxHoursDaily));s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

double ConstraintTeachersMaxHoursDaily::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int i=0; i<r.nInternalTeachers; i++){
			for(int d=0; d<r.nDaysPerWeek; d++){
				int n_hours_daily=0;
				for(int h=0; h<r.nHoursPerDay; h++)
					if(teachersMatrix[i][d][h]>0)
						n_hours_daily++;

				if(n_hours_daily>this->maxHoursDaily)
					nbroken++;
			}
		}
	}
	//with logging
	else{
		nbroken=0;
		for(int i=0; i<r.nInternalTeachers; i++){
			for(int d=0; d<r.nDaysPerWeek; d++){
				int n_hours_daily=0;
				for(int h=0; h<r.nHoursPerDay; h++)
					if(teachersMatrix[i][d][h]>0)
						n_hours_daily++;

				if(n_hours_daily>this->maxHoursDaily){
					nbroken++;

					if(conflictsString!=NULL){
						QString s=(QObject::tr(
						 "Time constraint teachers max %1 hours daily broken for teacher %2, on day %3, length=%4.")
						 .arg(QString::number(this->maxHoursDaily))
						 .arg(r.internalTeachersList[i]->name)
						 .arg(r.daysOfTheWeek[d])
						 .arg(n_hours_daily)
						 )
						 +
						 " "
						 +
						 (QObject::tr("This increases the conflicts total by %1").arg(QString::number(weightPercentage/100)));
						
						dl.append(s);
						cl.append(weightPercentage/100);
					
						*conflictsString+= s+"\n";
					}
				}
			}
		}
	}

	if(weightPercentage==100)	
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintTeachersMaxHoursDaily::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);
	//if(a)
	//	;

	return false;
}

bool ConstraintTeachersMaxHoursDaily::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return true;
}

bool ConstraintTeachersMaxHoursDaily::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintTeachersMaxHoursDaily::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintTeachersMaxHoursDaily::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMaxHoursDaily::ConstraintTeacherMaxHoursDaily()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHER_MAX_HOURS_DAILY;
}

ConstraintTeacherMaxHoursDaily::ConstraintTeacherMaxHoursDaily(double wp, int maxhours, const QString& teacher)
 : TimeConstraint(wp)
 {
	assert(maxhours>0);
	this->maxHoursDaily=maxhours;
	this->teacherName=teacher;

	this->type=CONSTRAINT_TEACHER_MAX_HOURS_DAILY;
}

bool ConstraintTeacherMaxHoursDaily::computeInternalStructure(Rules& r)
{
	this->teacher_ID=r.searchTeacher(this->teacherName);
	assert(this->teacher_ID>=0);
	return true;
}

bool ConstraintTeacherMaxHoursDaily::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeacherMaxHoursDaily::getXmlDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="<ConstraintTeacherMaxHoursDaily>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Teacher_Name>"+protect(this->teacherName)+"</Teacher_Name>\n";
	s+="	<Maximum_Hours_Daily>"+QString::number(this->maxHoursDaily)+"</Maximum_Hours_Daily>\n";
	s+="</ConstraintTeacherMaxHoursDaily>\n";
	return s;
}

QString ConstraintTeacherMaxHoursDaily::getDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s;
	s+=(QObject::tr("Teacher max %1 hours daily").arg(this->maxHoursDaily));s+=", ";
	s+=QObject::tr("TN:%1").arg(this->teacherName);s+=", ";
	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));//s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintTeacherMaxHoursDaily::getDetailedDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Teacher %1 must not have more than %2 hours daily").arg(this->teacherName).arg(this->maxHoursDaily);s+="\n";
	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

double ConstraintTeacherMaxHoursDaily::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		int i=this->teacher_ID;
		//for(int i=0; i<r.nInternalTeachers; i++){
		for(int d=0; d<r.nDaysPerWeek; d++){
			int n_hours_daily=0;
			for(int h=0; h<r.nHoursPerDay; h++)
				if(teachersMatrix[i][d][h]>0)
					n_hours_daily++;

			if(n_hours_daily>this->maxHoursDaily){
				nbroken++;
			}
		}
		//}
	}
	//with logging
	else{
		nbroken=0;
		int i=this->teacher_ID;
		//for(int i=0; i<r.nInternalTeachers; i++){
		for(int d=0; d<r.nDaysPerWeek; d++){
			int n_hours_daily=0;
			for(int h=0; h<r.nHoursPerDay; h++)
				if(teachersMatrix[i][d][h]>0)
					n_hours_daily++;

			if(n_hours_daily>this->maxHoursDaily){
				nbroken++;

				if(conflictsString!=NULL){
					QString s=(QObject::tr(
					 "Time constraint teacher max %1 hours daily broken for teacher %2, on day %3, length=%4.")
					 .arg(QString::number(this->maxHoursDaily))
					 .arg(r.internalTeachersList[i]->name)
					 .arg(r.daysOfTheWeek[d])
					 .arg(n_hours_daily)
					 )
					 +" "
					 +
					 (QObject::tr("This increases the conflicts total by %1").arg(QString::number(weightPercentage/100)));
						
					dl.append(s);
					cl.append(weightPercentage/100);
				
					*conflictsString+= s+"\n";
				}
			}
		}
		//}
	}

	if(weightPercentage==100)	
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherMaxHoursDaily::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);
	//if(a)
	//	;

	return false;
}

bool ConstraintTeacherMaxHoursDaily::isRelatedToTeacher(Teacher* t)
{
	if(this->teacherName==t->name)
		return true;
	return false;
}

bool ConstraintTeacherMaxHoursDaily::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintTeacherMaxHoursDaily::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintTeacherMaxHoursDaily::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersMaxHoursContinuously::ConstraintTeachersMaxHoursContinuously()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHERS_MAX_HOURS_CONTINUOUSLY;
}

ConstraintTeachersMaxHoursContinuously::ConstraintTeachersMaxHoursContinuously(double wp, int maxhours)
 : TimeConstraint(wp)
 {
	assert(maxhours>0);
	this->maxHoursContinuously=maxhours;

	this->type=CONSTRAINT_TEACHERS_MAX_HOURS_CONTINUOUSLY;
}

bool ConstraintTeachersMaxHoursContinuously::computeInternalStructure(Rules& r)
{
	Q_UNUSED(r);
	/*if(&r!=NULL)
		;*/

	return true;
}

bool ConstraintTeachersMaxHoursContinuously::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeachersMaxHoursContinuously::getXmlDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="<ConstraintTeachersMaxHoursContinuously>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Maximum_Hours_Continuously>"+QString::number(this->maxHoursContinuously)+"</Maximum_Hours_Continuously>\n";
	s+="</ConstraintTeachersMaxHoursContinuously>\n";
	return s;
}

QString ConstraintTeachersMaxHoursContinuously::getDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s;
	s+=(QObject::tr("Teachers max %1 hours continuously").arg(this->maxHoursContinuously));s+=", ";
	s+=(QObject::tr("WP:%1\%", "Weight percentage").arg(this->weightPercentage));//s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintTeachersMaxHoursContinuously::getDetailedDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=(QObject::tr("Teachers must not have more than %1 hours continuously").arg(this->maxHoursContinuously));s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

double ConstraintTeachersMaxHoursContinuously::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	nbroken=0;
	for(int i=0; i<r.nInternalTeachers; i++){
		for(int d=0; d<r.nDaysPerWeek; d++){
			int nc=0;
			for(int h=0; h<r.nHoursPerDay; h++){
				if(teachersMatrix[i][d][h]>0)
					nc++;
				else{
					if(nc>this->maxHoursContinuously){
						nbroken++;

						if(conflictsString!=NULL){
							QString s=(QObject::tr(
							 "Time constraint teachers max %1 hours continuously broken for teacher %2, on day %3, length=%4.")
							 .arg(QString::number(this->maxHoursContinuously))
							 .arg(r.internalTeachersList[i]->name)
							 .arg(r.daysOfTheWeek[d])
							 .arg(nc)
							 )
							 +
							 " "
							 +
							 (QObject::tr("This increases the conflicts total by %1").arg(QString::number(weightPercentage/100)));
							
							dl.append(s);
							cl.append(weightPercentage/100);
				
							*conflictsString+= s+"\n";
						}
					}
				
					nc=0;
				}
			}

			if(nc>this->maxHoursContinuously){
				nbroken++;

				if(conflictsString!=NULL){
					QString s=(QObject::tr(
					 "Time constraint teachers max %1 hours continuously broken for teacher %2, on day %3, length=%4.")
					 .arg(QString::number(this->maxHoursContinuously))
					 .arg(r.internalTeachersList[i]->name)
					 .arg(r.daysOfTheWeek[d])
					 .arg(nc)
					 )
					 +
					 " "
					 +
					 (QObject::tr("This increases the conflicts total by %1").arg(QString::number(weightPercentage/100)));
							
					dl.append(s);
					cl.append(weightPercentage/100);
				
					*conflictsString+= s+"\n";
				}
			}
		}
	}

	if(weightPercentage==100)	
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintTeachersMaxHoursContinuously::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);
	//if(a)
	//	;

	return false;
}

bool ConstraintTeachersMaxHoursContinuously::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return true;
}

bool ConstraintTeachersMaxHoursContinuously::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintTeachersMaxHoursContinuously::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintTeachersMaxHoursContinuously::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMaxHoursContinuously::ConstraintTeacherMaxHoursContinuously()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHER_MAX_HOURS_CONTINUOUSLY;
}

ConstraintTeacherMaxHoursContinuously::ConstraintTeacherMaxHoursContinuously(double wp, int maxhours, const QString& teacher)
 : TimeConstraint(wp)
 {
	assert(maxhours>0);
	this->maxHoursContinuously=maxhours;
	this->teacherName=teacher;

	this->type=CONSTRAINT_TEACHER_MAX_HOURS_CONTINUOUSLY;
}

bool ConstraintTeacherMaxHoursContinuously::computeInternalStructure(Rules& r)
{
	this->teacher_ID=r.searchTeacher(this->teacherName);
	assert(this->teacher_ID>=0);
	return true;
}

bool ConstraintTeacherMaxHoursContinuously::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeacherMaxHoursContinuously::getXmlDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="<ConstraintTeacherMaxHoursContinuously>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Teacher_Name>"+protect(this->teacherName)+"</Teacher_Name>\n";
	s+="	<Maximum_Hours_Continuously>"+QString::number(this->maxHoursContinuously)+"</Maximum_Hours_Continuously>\n";
	s+="</ConstraintTeacherMaxHoursContinuously>\n";
	return s;
}

QString ConstraintTeacherMaxHoursContinuously::getDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s;
	s+=(QObject::tr("Teacher max %1 hours continuously").arg(this->maxHoursContinuously));s+=", ";
	s+=QObject::tr("TN:%1", "Teacher name").arg(this->teacherName);s+=", ";
	s+=(QObject::tr("WP:%1\%", "Weight percentage").arg(this->weightPercentage));//s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintTeacherMaxHoursContinuously::getDetailedDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Teacher %1 must not have more than %2 hours continuously").arg(this->teacherName).arg(this->maxHoursContinuously);s+="\n";
	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

double ConstraintTeacherMaxHoursContinuously::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	nbroken=0;
	int i=this->teacher_ID;
	//for(int i=0; i<r.nInternalTeachers; i++){
		for(int d=0; d<r.nDaysPerWeek; d++){
			int nc=0;
			for(int h=0; h<r.nHoursPerDay; h++){
				if(teachersMatrix[i][d][h]>0)
					nc++;
				else{
					if(nc>this->maxHoursContinuously){
						nbroken++;

						if(conflictsString!=NULL){
							QString s=(QObject::tr(
							 "Time constraint teacher max %1 hours continuously broken for teacher %2, on day %3, length=%4.")
							 .arg(QString::number(this->maxHoursContinuously))
							 .arg(r.internalTeachersList[i]->name)
							 .arg(r.daysOfTheWeek[d])
							 .arg(nc)
							 )
							 +
							 " "
							 +
							 (QObject::tr("This increases the conflicts total by %1").arg(QString::number(weightPercentage/100)));
							
							dl.append(s);
							cl.append(weightPercentage/100);
				
							*conflictsString+= s+"\n";
						}
					}
				
					nc=0;
				}
			}

			if(nc>this->maxHoursContinuously){
				nbroken++;

				if(conflictsString!=NULL){
					QString s=(QObject::tr(
					 "Time constraint teacher max %1 hours continuously broken for teacher %2, on day %3, length=%4.")
					 .arg(QString::number(this->maxHoursContinuously))
					 .arg(r.internalTeachersList[i]->name)
					 .arg(r.daysOfTheWeek[d])
					 .arg(nc)
					 )
					 +
					 " "
					 +
					 (QObject::tr("This increases the conflicts total by %1").arg(QString::number(weightPercentage/100)));
							
					dl.append(s);
					cl.append(weightPercentage/100);
				
					*conflictsString+= s+"\n";
				}
			}
		}
	//}

	if(weightPercentage==100)	
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherMaxHoursContinuously::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);
	//if(a)
	//	;

	return false;
}

bool ConstraintTeacherMaxHoursContinuously::isRelatedToTeacher(Teacher* t)
{
	if(this->teacherName==t->name)
		return true;
	return false;
}

bool ConstraintTeacherMaxHoursContinuously::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintTeacherMaxHoursContinuously::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintTeacherMaxHoursContinuously::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMaxDaysPerWeek::ConstraintTeacherMaxDaysPerWeek()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHER_MAX_DAYS_PER_WEEK;
}

ConstraintTeacherMaxDaysPerWeek::ConstraintTeacherMaxDaysPerWeek(double wp, int maxnd, QString tn)
	 : TimeConstraint(wp)
{
	this->teacherName = tn;
	this->maxDaysPerWeek=maxnd;
	this->type=CONSTRAINT_TEACHER_MAX_DAYS_PER_WEEK;
}

bool ConstraintTeacherMaxDaysPerWeek::computeInternalStructure(Rules& r)
{
	this->teacher_ID=r.searchTeacher(this->teacherName);
	assert(this->teacher_ID>=0);
	return true;
}

bool ConstraintTeacherMaxDaysPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeacherMaxDaysPerWeek::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="<ConstraintTeacherMaxDaysPerWeek>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Teacher_Name>"+protect(this->teacherName)+"</Teacher_Name>\n";
	s+="	<Max_Days_Per_Week>"+QString::number(this->maxDaysPerWeek)+"</Max_Days_Per_Week>\n";
	s+="</ConstraintTeacherMaxDaysPerWeek>\n";
	return s;
}

QString ConstraintTeacherMaxDaysPerWeek::getDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Teacher max days per week");s+=", ";
	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
	s+=(QObject::tr("T:%1").arg(this->teacherName));s+=", ";
	s+=(QObject::tr("MD:%1").arg(this->maxDaysPerWeek));

	return s;
}

QString ConstraintTeacherMaxDaysPerWeek::getDetailedDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Teacher max. days per week");s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	s+=(QObject::tr("Teacher=%1").arg(this->teacherName));s+="\n";
	s+=(QObject::tr("Max. days per week=%1").arg(this->maxDaysPerWeek));s+="\n";

	return s;
}

double ConstraintTeacherMaxDaysPerWeek::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString *conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		//count sort
		int t=this->teacher_ID;
		int nd[MAX_HOURS_PER_DAY + 1];
		for(int h=0; h<=r.nHoursPerDay; h++)
			nd[h]=0;
		for(int d=0; d<r.nDaysPerWeek; d++){
			int nh=0;
			for(int h=0; h<r.nHoursPerDay; h++)
				nh += teachersMatrix[t][d][h]>=1 ? 1 : 0;
			nd[nh]++;
		}
		//return the minimum occupied days which do not respect this constraint
		int i = r.nDaysPerWeek - this->maxDaysPerWeek;
		for(int k=0; k<=r.nHoursPerDay; k++){
			if(nd[k]>0){
				if(i>nd[k]){
					i-=nd[k];
					nbroken+=nd[k]*k;
				}
				else{
					nbroken+=i*k;
					break;
				}
			}
		}
	}
	//with logging
	else{
		nbroken=0;
		//count sort
		int t=this->teacher_ID;
		int nd[MAX_HOURS_PER_DAY + 1];
		for(int h=0; h<=r.nHoursPerDay; h++)
			nd[h]=0;
		for(int d=0; d<r.nDaysPerWeek; d++){
			int nh=0;
			for(int h=0; h<r.nHoursPerDay; h++)
				nh += teachersMatrix[t][d][h]>=1 ? 1 : 0;
			nd[nh]++;
		}
		//return the minimum occupied days which do not respect this constraint
		int i = r.nDaysPerWeek - this->maxDaysPerWeek;
		for(int k=0; k<=r.nHoursPerDay; k++){
			if(nd[k]>0){
				if(i>nd[k]){
					i-=nd[k];
					nbroken+=nd[k]*k;
				}
				else{
					nbroken+=i*k;
					break;
				}
			}
		}

		if(nbroken>0){
			QString s= QObject::tr("Time constraint teacher max days per week broken for");
			s += " ";
			s += QObject::tr("teacher: %1.")
			 .arg(r.internalTeachersList[t]->name);
			s += QObject::tr("This increases the conflicts total by %1")
			 .arg(nbroken*weightPercentage/100);
			 
			dl.append(s);
			cl.append(nbroken*weightPercentage/100);
		
			*conflictsString += s+"\n";
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherMaxDaysPerWeek::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);
	//if(a)
	//	;

	return false;
}

bool ConstraintTeacherMaxDaysPerWeek::isRelatedToTeacher(Teacher* t)
{
	if(this->teacherName==t->name)
		return true;
	return false;
}

bool ConstraintTeacherMaxDaysPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintTeacherMaxDaysPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintTeacherMaxDaysPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersMaxGapsPerWeek::ConstraintTeachersMaxGapsPerWeek()
	: TimeConstraint()
{
	this->type = CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK;
}

ConstraintTeachersMaxGapsPerWeek::ConstraintTeachersMaxGapsPerWeek(double wp, int mg)
	: TimeConstraint(wp)
{
	this->type = CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK;
	this->maxGaps=mg;
}

bool ConstraintTeachersMaxGapsPerWeek::computeInternalStructure(Rules& r)
{
	Q_UNUSED(r);
	/*if(&r!=NULL)
		;*/

	/*do nothing*/
	return true;
}

bool ConstraintTeachersMaxGapsPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeachersMaxGapsPerWeek::getXmlDescription(Rules& r){
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	/*if(&r==NULL)
		;*/

	QString s="<ConstraintTeachersMaxGapsPerWeek>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Max_Gaps>"+QString::number(this->maxGaps)+"</Max_Gaps>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="</ConstraintTeachersMaxGapsPerWeek>\n";
	return s;
}

QString ConstraintTeachersMaxGapsPerWeek::getDescription(Rules& r){
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	/*if(&r==NULL)
		;*/

	QString s;
	s+=QObject::tr("Teachers max gaps per week");s+=", ";
	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);s+=", ";
	s+=QObject::tr("MG:%1").arg(this->maxGaps);
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintTeachersMaxGapsPerWeek::getDetailedDescription(Rules& r){
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	/*if(&r==NULL)
		;*/

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Teachers max gaps per week");s+="\n";
	s+=QObject::tr("(breaks and teacher not available not counted)");s+="\n";
	s+=QObject::tr("Max gaps per week:%1").arg(this->maxGaps); s+="\n";
	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

double ConstraintTeachersMaxGapsPerWeek::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{ 
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}
	
	int tg;
	int i, j, k;
	int totalGaps;

	totalGaps=0;
	for(i=0; i<r.nInternalTeachers; i++){
		tg=0;
		for(j=0; j<r.nDaysPerWeek; j++){
			for(k=0; k<r.nHoursPerDay; k++)
				if(teachersMatrix[i][j][k]>0){
					assert(!breakDayHour[j][k] && !teacherNotAvailableDayHour[i][j][k]);
					break;
				}

			int cnt=0;
			for(; k<r.nHoursPerDay; k++) if(!breakDayHour[j][k] && !teacherNotAvailableDayHour[i][j][k]){
				if(teachersMatrix[i][j][k]>0){
					tg+=cnt;
					cnt=0;
				}
				else
					cnt++;
			}
		}
		if(tg>this->maxGaps){
			totalGaps+=tg-maxGaps;
			//assert(this->weightPercentage<100); partial solutions might break this rule
			if(conflictsString!=NULL){
				QString s=QObject::tr("Time constraint teachers max gaps per week broken: teacher: %1, conflicts factor increase=%2")
					.arg(r.internalTeachersList[i]->name)
					.arg((tg-maxGaps)*weightPercentage/100);
					
				*conflictsString+= s+"\n";
						
				dl.append(s);
				cl.append((tg-maxGaps)*weightPercentage/100);
			}
		}
	}
	
	/*int na=0;
	for(int i=0; i<r.nInternalActivities; i++)
		if(c.times[i]!=UNALLOCATED_TIME)
			na++;*/

	if(c.nPlacedActivities==r.nInternalActivities)
	//if(na==r.nInternalActivities)
		if(weightPercentage==100){
		
			/*if(totalGaps>0){
				cout<<"c.nPlacedActivities=="<<c.nPlacedActivities<<endl;
				
				int k=0;
				for(int j=0; j<r.nInternalActivities; j++){
					cout<<"j=="<<j<<", c.times[j]=="<<c.times[j]<<endl;
					if(c.times[j]!=UNALLOCATED_TIME)
						k++;
				}
				
				cout<<"sum(allocated activities)=="<<k<<endl;
			}*/
				
			assert(totalGaps==0); //for partial solutions this rule might be broken
		}
	return weightPercentage/100 * totalGaps;
}

bool ConstraintTeachersMaxGapsPerWeek::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);
	//if(a)
	//	;

	return false;
}

bool ConstraintTeachersMaxGapsPerWeek::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return true;
}

bool ConstraintTeachersMaxGapsPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintTeachersMaxGapsPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintTeachersMaxGapsPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMaxGapsPerWeek::ConstraintTeacherMaxGapsPerWeek()
	: TimeConstraint()
{
	this->type = CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK;
}

ConstraintTeacherMaxGapsPerWeek::ConstraintTeacherMaxGapsPerWeek(double wp, QString tn, int mg)
	: TimeConstraint(wp)
{
	this->type = CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK;
	this->teacherName=tn;
	this->maxGaps=mg;
}

bool ConstraintTeacherMaxGapsPerWeek::computeInternalStructure(Rules& r)
{
	this->teacherIndex=r.searchTeacher(this->teacherName);
	assert(this->teacherIndex>=0);
	return true;
}

bool ConstraintTeacherMaxGapsPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeacherMaxGapsPerWeek::getXmlDescription(Rules& r){
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s="<ConstraintTeacherMaxGapsPerWeek>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Teacher_Name>"+protect(this->teacherName)+"</Teacher_Name>\n";
	s+="	<Max_Gaps>"+QString::number(this->maxGaps)+"</Max_Gaps>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="</ConstraintTeacherMaxGapsPerWeek>\n";
	return s;
}

QString ConstraintTeacherMaxGapsPerWeek::getDescription(Rules& r){
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s;
	s+=QObject::tr("Teacher max gaps per week");s+=", ";
	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);s+=", ";
	s+=QObject::tr("T:%1").arg(this->teacherName); s+=", ";
	s+=QObject::tr("MG:%1").arg(this->maxGaps);
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintTeacherMaxGapsPerWeek::getDetailedDescription(Rules& r){
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s=QObject::tr("Time constraint"); s+="\n";
	s+=QObject::tr("Teacher max gaps per week"); s+="\n";
	s+=QObject::tr("(breaks and teacher not available not counted)");s+="\n";
	s+=QObject::tr("Teacher: %1").arg(this->teacherName); s+="\n";
	s+=QObject::tr("Max gaps per week: %1").arg(this->maxGaps); s+="\n";
	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage); s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

double ConstraintTeacherMaxGapsPerWeek::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{ 
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}
	
	int tg;
	int i, j, k;
	int totalGaps;

	totalGaps=0;
		
	i=this->teacherIndex;
	
	tg=0;
	for(j=0; j<r.nDaysPerWeek; j++){
		for(k=0; k<r.nHoursPerDay; k++)
			if(teachersMatrix[i][j][k]>0){
				assert(!breakDayHour[j][k] && !teacherNotAvailableDayHour[i][j][k]);
				break;
			}

		int cnt=0;
		for(; k<r.nHoursPerDay; k++) if(!breakDayHour[j][k] && !teacherNotAvailableDayHour[i][j][k]){
			if(teachersMatrix[i][j][k]>0){
				tg+=cnt;
				cnt=0;
			}
			else
				cnt++;
		}
	}
	if(tg>this->maxGaps){
		totalGaps+=tg-maxGaps;
		//assert(this->weightPercentage<100); partial solutions might break this rule
		if(conflictsString!=NULL){
			QString s=QObject::tr("Time constraint teacher max gaps per week broken: teacher: %1, conflicts factor increase=%2")
				.arg(r.internalTeachersList[i]->name)
				.arg((tg-maxGaps)*weightPercentage/100);
					
			*conflictsString+= s+"\n";
						
			dl.append(s);
			cl.append((tg-maxGaps)*weightPercentage/100);
		}
	}

	/*int na=0;
	for(int i=0; i<r.nInternalActivities; i++)
		if(c.times[i]!=UNALLOCATED_TIME)
			na++;*/
	if(c.nPlacedActivities==r.nInternalActivities)
	//if(na==r.nInternalActivities)
		if(weightPercentage==100)
			assert(totalGaps==0); //for partial solutions this rule might be broken
	return weightPercentage/100 * totalGaps;
}

bool ConstraintTeacherMaxGapsPerWeek::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);
	//if(a)
	//	;

	return false;
}

bool ConstraintTeacherMaxGapsPerWeek::isRelatedToTeacher(Teacher* t)
{
	if(this->teacherName==t->name)
		return true;
	return false;
}

bool ConstraintTeacherMaxGapsPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintTeacherMaxGapsPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintTeacherMaxGapsPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersMaxGapsPerDay::ConstraintTeachersMaxGapsPerDay()
	: TimeConstraint()
{
	this->type = CONSTRAINT_TEACHERS_MAX_GAPS_PER_DAY;
}

ConstraintTeachersMaxGapsPerDay::ConstraintTeachersMaxGapsPerDay(double wp, int mg)
	: TimeConstraint(wp)
{
	this->type = CONSTRAINT_TEACHERS_MAX_GAPS_PER_DAY;
	this->maxGaps=mg;
}

bool ConstraintTeachersMaxGapsPerDay::computeInternalStructure(Rules& r)
{
	Q_UNUSED(r);
	/*if(&r!=NULL)
		;*/

	/*do nothing*/
	return true;
}

bool ConstraintTeachersMaxGapsPerDay::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeachersMaxGapsPerDay::getXmlDescription(Rules& r){
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	/*if(&r==NULL)
		;*/

	QString s="<ConstraintTeachersMaxGapsPerDay>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Max_Gaps>"+QString::number(this->maxGaps)+"</Max_Gaps>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="</ConstraintTeachersMaxGapsPerDay>\n";
	return s;
}

QString ConstraintTeachersMaxGapsPerDay::getDescription(Rules& r){
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	/*if(&r==NULL)
		;*/

	QString s;
	s+=QObject::tr("Teachers max gaps per day");s+=", ";
	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);s+=", ";
	s+=QObject::tr("MG:%1").arg(this->maxGaps);
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintTeachersMaxGapsPerDay::getDetailedDescription(Rules& r){
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	/*if(&r==NULL)
		;*/

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Teachers max gaps per day");s+="\n";
	s+=QObject::tr("(breaks and teacher not available not counted)");s+="\n";
	s+=QObject::tr("Max gaps per day:%1").arg(this->maxGaps); s+="\n";
	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

double ConstraintTeachersMaxGapsPerDay::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{ 
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}
	
	int tg;
	int i, j, k;
	int totalGaps;

	totalGaps=0;
	for(i=0; i<r.nInternalTeachers; i++){
		for(j=0; j<r.nDaysPerWeek; j++){
			tg=0;
			for(k=0; k<r.nHoursPerDay; k++)
				if(teachersMatrix[i][j][k]>0){
					assert(!breakDayHour[j][k] && !teacherNotAvailableDayHour[i][j][k]);
					break;
				}

			int cnt=0;
			for(; k<r.nHoursPerDay; k++) if(!breakDayHour[j][k] && !teacherNotAvailableDayHour[i][j][k]){
				if(teachersMatrix[i][j][k]>0){
					tg+=cnt;
					cnt=0;
				}
				else
					cnt++;
			}
			if(tg>this->maxGaps){
				totalGaps+=tg-maxGaps;
				//assert(this->weightPercentage<100); partial solutions might break this rule
				if(conflictsString!=NULL){
					QString s=QObject::tr("Time constraint teachers max gaps per day broken: teacher: %1, day: %2, conflicts factor increase=%3")
						.arg(r.internalTeachersList[i]->name)
						.arg(r.daysOfTheWeek[j])
						.arg((tg-maxGaps)*weightPercentage/100);
					
					*conflictsString+= s+"\n";
								
					dl.append(s);
					cl.append((tg-maxGaps)*weightPercentage/100);
				}
			}
		}
	}
	
	if(c.nPlacedActivities==r.nInternalActivities)
		if(weightPercentage==100)
			assert(totalGaps==0); //for partial solutions this rule might be broken
	return weightPercentage/100 * totalGaps;
}

bool ConstraintTeachersMaxGapsPerDay::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);
	//if(a)
	//	;

	return false;
}

bool ConstraintTeachersMaxGapsPerDay::isRelatedToTeacher(Teacher* t)
{	
	Q_UNUSED(t);
	//if(t)
	//	;

	return true;
}

bool ConstraintTeachersMaxGapsPerDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintTeachersMaxGapsPerDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintTeachersMaxGapsPerDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMaxGapsPerDay::ConstraintTeacherMaxGapsPerDay()
	: TimeConstraint()
{
	this->type = CONSTRAINT_TEACHER_MAX_GAPS_PER_DAY;
}

ConstraintTeacherMaxGapsPerDay::ConstraintTeacherMaxGapsPerDay(double wp, QString tn, int mg)
	: TimeConstraint(wp)
{
	this->type = CONSTRAINT_TEACHER_MAX_GAPS_PER_DAY;
	this->teacherName=tn;
	this->maxGaps=mg;
}

bool ConstraintTeacherMaxGapsPerDay::computeInternalStructure(Rules& r)
{
	this->teacherIndex=r.searchTeacher(this->teacherName);
	assert(this->teacherIndex>=0);
	return true;
}

bool ConstraintTeacherMaxGapsPerDay::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeacherMaxGapsPerDay::getXmlDescription(Rules& r){
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s="<ConstraintTeacherMaxGapsPerDay>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Teacher_Name>"+protect(this->teacherName)+"</Teacher_Name>\n";
	s+="	<Max_Gaps>"+QString::number(this->maxGaps)+"</Max_Gaps>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="</ConstraintTeacherMaxGapsPerDay>\n";
	return s;
}

QString ConstraintTeacherMaxGapsPerDay::getDescription(Rules& r){
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s;
	s+=QObject::tr("Teacher max gaps per day");s+=", ";
	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);s+=", ";
	s+=QObject::tr("T:%1").arg(this->teacherName); s+=", ";
	s+=QObject::tr("MG:%1").arg(this->maxGaps);
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintTeacherMaxGapsPerDay::getDetailedDescription(Rules& r){
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s=QObject::tr("Time constraint"); s+="\n";
	s+=QObject::tr("Teacher max gaps per day"); s+="\n";
	s+=QObject::tr("(breaks and teacher not available not counted)");s+="\n";
	s+=QObject::tr("Teacher: %1").arg(this->teacherName); s+="\n";
	s+=QObject::tr("Max gaps per week: %1").arg(this->maxGaps); s+="\n";
	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage); s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

double ConstraintTeacherMaxGapsPerDay::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{ 
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}
	
	int tg;
	int i, j, k;
	int totalGaps;

	totalGaps=0;
		
	i=this->teacherIndex;
	
	for(j=0; j<r.nDaysPerWeek; j++){
		tg=0;
		for(k=0; k<r.nHoursPerDay; k++)
			if(teachersMatrix[i][j][k]>0){
				assert(!breakDayHour[j][k] && !teacherNotAvailableDayHour[i][j][k]);
				break;
			}

		int cnt=0;
		for(; k<r.nHoursPerDay; k++) if(!breakDayHour[j][k] && !teacherNotAvailableDayHour[i][j][k]){
			if(teachersMatrix[i][j][k]>0){
				tg+=cnt;
				cnt=0;
			}
			else
				cnt++;
		}
		if(tg>this->maxGaps){
			totalGaps+=tg-maxGaps;
			//assert(this->weightPercentage<100); partial solutions might break this rule
			if(conflictsString!=NULL){
				QString s=QObject::tr("Time constraint teacher max gaps per day broken: teacher: %1, day: %2, conflicts factor increase=%3")
					.arg(r.internalTeachersList[i]->name)
					.arg(r.daysOfTheWeek[j])
					.arg((tg-maxGaps)*weightPercentage/100);
						
				*conflictsString+= s+"\n";
							
				dl.append(s);
				cl.append((tg-maxGaps)*weightPercentage/100);
			}
		}
	}

	if(c.nPlacedActivities==r.nInternalActivities)
		if(weightPercentage==100)
			assert(totalGaps==0); //for partial solutions this rule might be broken
	return weightPercentage/100 * totalGaps;
}

bool ConstraintTeacherMaxGapsPerDay::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);
	//if(a)
	//	;

	return false;
}

bool ConstraintTeacherMaxGapsPerDay::isRelatedToTeacher(Teacher* t)
{
	if(this->teacherName==t->name)
		return true;
	return false;
}

bool ConstraintTeacherMaxGapsPerDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintTeacherMaxGapsPerDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintTeacherMaxGapsPerDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintBreakTimes::ConstraintBreakTimes()
	: TimeConstraint()
{
	this->type = CONSTRAINT_BREAK_TIMES;
}

ConstraintBreakTimes::ConstraintBreakTimes(double wp, QList<int> d, QList<int> h)
	: TimeConstraint(wp)
{
	this->days = d;
	this->hours = h;
	this->type = CONSTRAINT_BREAK_TIMES;
}

bool ConstraintBreakTimes::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintBreakTimes::getXmlDescription(Rules& r){
	QString s="<ConstraintBreakTimes>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";

	s+="	<Number_of_Break_Times>"+QString::number(this->days.count())+"</Number_of_Break_Times>\n";
	assert(days.count()==hours.count());
	for(int i=0; i<days.count(); i++){
		s+="	<Break_Time>\n";
		if(this->days.at(i)>=0)
			s+="		<Day>"+protect(r.daysOfTheWeek[this->days.at(i)])+"</Day>\n";
		if(this->hours.at(i)>=0)
			s+="		<Hour>"+protect(r.hoursOfTheDay[this->hours.at(i)])+"</Hour>\n";
		s+="	</Break_Time>\n";
	}

	s+="</ConstraintBreakTimes>\n";
	return s;
}

QString ConstraintBreakTimes::getDescription(Rules& r){
	QString s;
	s+=QObject::tr("Break times");s+=", ";
	s+=QObject::tr("WP:%1\%", "Weight percentage").arg(this->weightPercentage);s+=", ";

	s+=QObject::tr("B at:", "Break at");
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

QString ConstraintBreakTimes::getDetailedDescription(Rules& r){
	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Break times");s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";

	s+=QObject::tr("Break at:");
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

bool ConstraintBreakTimes::computeInternalStructure(Rules& r)
{
	Q_UNUSED(r);
	
	assert(days.count()==hours.count());
	for(int k=0; k<days.count(); k++){
		if(this->days.at(k) >= r.nDaysPerWeek){
			QMessageBox::information(NULL, QObject::tr("FET information"),
			 QObject::tr("Constraint break times is wrong because it refers to removed day. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}		
		if(this->hours.at(k) >= r.nHoursPerDay){
			QMessageBox::information(NULL, QObject::tr("FET information"),
			 QObject::tr("Constraint break times is wrong because an hour is too late (after the last acceptable slot). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
	}

	/*if(this->d >= r.nDaysPerWeek){
		QMessageBox::information(NULL, QObject::tr("FET information"),
		 QObject::tr("Constraint break is wrong because it refers to removed day. Please correct"
		 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}
	if(this->h1 > r.nHoursPerDay){
		QMessageBox::information(NULL, QObject::tr("FET information"),
		 QObject::tr("Constraint break is wrong because it refers to removed start hour. Please correct"
		 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}
	if(this->h2 > r.nHoursPerDay){
		QMessageBox::information(NULL, QObject::tr("FET information"),
		 QObject::tr("Constraint break is wrong because it refers to removed end hour. Please correct"
		 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}
	if(this->h1 >= this->h2){
		QMessageBox::information(NULL, QObject::tr("FET information"),
		 QObject::tr("Constraint break is wrong because start hour >= end hour. Please correct"
		 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}*/

	return true;
}

double ConstraintBreakTimes::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}

	//DEPRECATED COMMENT
	//For the moment, this function sums the number of hours each teacher
	//is teaching in this break period.
	//This function consideres all the hours, I mean if there are for example 5 weekly courses
	//scheduled on that hour (which is already a broken hard restriction - we only
	//are allowed 1 weekly course for a certain teacher at a certain hour) we calculate
	//5 broken restrictions for this break period.
	//TODO: decide if it is better to consider only 2 or 10 as a return value in this particular case
	//(currently it is 10)
	
	//int j=this->d;
	int nbroken;
	
	nbroken=0;
		
	for(int i=0; i<r.nInternalActivities; i++){
		int dayact=c.times[i]%r.nDaysPerWeek;
		int houract=c.times[i]/r.nDaysPerWeek;
		
		assert(days.count()==hours.count());
		for(int kk=0; kk<days.count(); kk++){
			int d=days.at(kk);
			int h=hours.at(kk);
			
			int dur=r.internalActivitiesList[i].duration;
			if(d==dayact && !(houract+dur<=h || houract>h))
			{			
				nbroken++;

				if(conflictsString!=NULL){
					QString s=QObject::tr("Time constraint break not respected for activity with id %1, on day %2, hours %3")
						.arg(r.internalActivitiesList[i].id)
						.arg(r.daysOfTheWeek[dayact])
						.arg(r.daysOfTheWeek[houract]);
					s+=". ";
					s+=QObject::tr("This increases the conflicts total by");
					s+=" "+QString::number(weightPercentage/100);
					
					dl.append(s);
					cl.append(weightPercentage/100);
				
					*conflictsString+= s+"\n";
				}
			}
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintBreakTimes::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintBreakTimes::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintBreakTimes::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintBreakTimes::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintBreakTimes::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsMaxGapsPerWeek::ConstraintStudentsMaxGapsPerWeek()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_MAX_GAPS_PER_WEEK;
}

ConstraintStudentsMaxGapsPerWeek::ConstraintStudentsMaxGapsPerWeek(double wp, int mg)
	: TimeConstraint(wp)
{
	this->type = CONSTRAINT_STUDENTS_MAX_GAPS_PER_WEEK;
	this->maxGaps=mg;
}

bool ConstraintStudentsMaxGapsPerWeek::computeInternalStructure(Rules& r)
{
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	/*do nothing*/
	return true;
}

bool ConstraintStudentsMaxGapsPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsMaxGapsPerWeek::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s="<ConstraintStudentsMaxGapsPerWeek>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Max_Gaps>"+QString::number(this->maxGaps)+"</Max_Gaps>\n";
	s+="</ConstraintStudentsMaxGapsPerWeek>\n";
	return s;
}

QString ConstraintStudentsMaxGapsPerWeek::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);

	QString s;
	s+=QObject::tr("Students max gaps per week");s+=", ";
	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));s+=", ";
	s+=QObject::tr("MG:%1").arg(this->maxGaps);

	return s;
}

QString ConstraintStudentsMaxGapsPerWeek::getDetailedDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Students max gaps per week");s+="\n";
	s+=QObject::tr("(breaks and students set not available not counted)");s+="\n";
	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";
	s+=QObject::tr("Max gaps=%1").arg(this->maxGaps);s+="\n";
	s+="\n";

	return s;
}

double ConstraintStudentsMaxGapsPerWeek::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//returns a number equal to the number of windows of the subgroups (in hours)

	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}

	int windows;
	int tmp;
	int i;
	
	int tIllegalWindows=0;

	for(i=0; i<r.nInternalSubgroups; i++){
		windows=0;
		for(int j=0; j<r.nDaysPerWeek; j++){
			int k;
			tmp=0;
			for(k=0; k<r.nHoursPerDay; k++)
				if(subgroupsMatrix[i][j][k]>0){
					assert(!breakDayHour[j][k] && !subgroupNotAvailableDayHour[i][j][k]);
					break;
				}
			for(; k<r.nHoursPerDay; k++) if(!breakDayHour[j][k] && !subgroupNotAvailableDayHour[i][j][k]){
				if(subgroupsMatrix[i][j][k]>0){
					windows+=tmp;
					tmp=0;
				}
				else
					tmp++;
			}
		}
		
		int illegalWindows=windows-this->maxGaps;
		if(illegalWindows<0)
			illegalWindows=0;

		if(illegalWindows>0 && conflictsString!=NULL){
			QString s=QObject::tr("Time constraint students max gaps broken for subgroup: %1, it has %2 extra gaps, conflicts increase=%3")
			 .arg(r.internalSubgroupsList[i]->name)
			 .arg(illegalWindows)
			 .arg(illegalWindows*weightPercentage/100);
						 
			dl.append(s);
			cl.append(illegalWindows*weightPercentage/100);
					
			*conflictsString+= s+"\n";
		}
		
		tIllegalWindows+=illegalWindows;
	}
		
	if(c.nPlacedActivities==r.nInternalActivities)
		if(weightPercentage==100)    //for partial solutions it might be broken
			assert(tIllegalWindows==0);
	return weightPercentage/100 * tIllegalWindows;
}

bool ConstraintStudentsMaxGapsPerWeek::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsMaxGapsPerWeek::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintStudentsMaxGapsPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMaxGapsPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMaxGapsPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetMaxGapsPerWeek::ConstraintStudentsSetMaxGapsPerWeek()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_WEEK;
}

ConstraintStudentsSetMaxGapsPerWeek::ConstraintStudentsSetMaxGapsPerWeek(double wp, int mg, const QString& st )
	: TimeConstraint(wp)
{
	this->type = CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_WEEK;
	this->maxGaps=mg;
	this->students = st;
}

bool ConstraintStudentsSetMaxGapsPerWeek::computeInternalStructure(Rules& r){
	StudentsSet* ss=r.searchAugmentedStudentsSet(this->students);

	if(ss==NULL){
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Constraint students set max gaps per week is wrong because it refers to inexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}	

	assert(ss);

	//this->nSubgroups=0;
	this->iSubgroupsList.clear();
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		/*for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
			if(r.internalSubgroupsList[tmp]->name == ss->name)
				break;*/
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		//assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
		//this->subgroups[this->nSubgroups++]=tmp;
		if(!this->iSubgroupsList.contains(tmp))
			this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			/*for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
				if(r.internalSubgroupsList[tmp]->name == sts->name)
					break;*/
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			//assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
			//this->subgroups[this->nSubgroups++]=tmp;
			if(!this->iSubgroupsList.contains(tmp))
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
				/*for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
					if(r.internalSubgroupsList[tmp]->name == sts->name)
						break;*/
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				//assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
				//this->subgroups[this->nSubgroups++]=tmp;
				if(!this->iSubgroupsList.contains(tmp))
					this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);
		
	return true;
}

bool ConstraintStudentsSetMaxGapsPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsSetMaxGapsPerWeek::getXmlDescription(Rules& r){
	//to avoid non-used parameter warning
	Q_UNUSED(r);

	QString s="<ConstraintStudentsSetMaxGapsPerWeek>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Max_Gaps>"+QString::number(this->maxGaps)+"</Max_Gaps>\n";
	s+="	<Students>"; s+=protect(this->students); s+="</Students>\n";
	s+="</ConstraintStudentsSetMaxGapsPerWeek>\n";
	return s;
}

QString ConstraintStudentsSetMaxGapsPerWeek::getDescription(Rules& r){
	//to avoid non-used parameter warning
	Q_UNUSED(r);

	QString s;
	s+=QObject::tr("Students set max gaps per week"); s+=", ";
	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage); s+=", ";
	s+=QObject::tr("MG:%1").arg(this->maxGaps);s+=", ";
	s+=QObject::tr("St:%1").arg(this->students);

	return s;
}

QString ConstraintStudentsSetMaxGapsPerWeek::getDetailedDescription(Rules& r){
	//to avoid non-used parameter warning
	Q_UNUSED(r);

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Students set max gaps per week");s+="\n";
	s+=QObject::tr("(breaks and students set not available not counted)");s+="\n";
	s+=QObject::tr("Weight (percentage)=%1").arg(this->weightPercentage);s+="\n";
	s+=QObject::tr("Max gaps=%1").arg(this->maxGaps);s+="\n";
	s+=QObject::tr("Students=%1").arg(this->students); s+="\n";
	
	return s;
}

double ConstraintStudentsSetMaxGapsPerWeek::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//OLD COMMENT
	//returns a number equal to the number of windows of the subgroups (in hours)

	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}
	
	int windows;
	int tmp;
	
	int tIllegalWindows=0;
	
	for(int sg=0; sg<this->iSubgroupsList.count(); sg++){
		windows=0;
		int i=this->iSubgroupsList.at(sg);
		for(int j=0; j<r.nDaysPerWeek; j++){
			int k;
			tmp=0;
			for(k=0; k<r.nHoursPerDay; k++)
				if(subgroupsMatrix[i][j][k]>0){
					assert(!breakDayHour[j][k] && !subgroupNotAvailableDayHour[i][j][k]);
					break;
				}
			for(; k<r.nHoursPerDay; k++) if(!breakDayHour[j][k] && !subgroupNotAvailableDayHour[i][j][k]){
				if(subgroupsMatrix[i][j][k]>0){
					windows+=tmp;
					tmp=0;
				}
				else
					tmp++;
			}
		}
		
		int illegalWindows=windows-this->maxGaps;
		if(illegalWindows<0)
			illegalWindows=0;

		if(illegalWindows>0 && conflictsString!=NULL){
			QString s=QObject::tr("Time constraint students set max gaps broken for subgroup: %1, extra gaps=%2, conflicts increase=%3")
			 .arg(r.internalSubgroupsList[i]->name)
			 .arg(illegalWindows)
			 .arg(weightPercentage/100*illegalWindows);
						 
			dl.append(s);
			cl.append(weightPercentage/100*illegalWindows);
				
			*conflictsString+= s+"\n";
		}
		
		tIllegalWindows+=illegalWindows;
	}

	if(c.nPlacedActivities==r.nInternalActivities)
		if(weightPercentage==100)     //for partial solutions it might be broken
			assert(tIllegalWindows==0);
	return weightPercentage/100 * tIllegalWindows;
}

bool ConstraintStudentsSetMaxGapsPerWeek::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetMaxGapsPerWeek::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintStudentsSetMaxGapsPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMaxGapsPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMaxGapsPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.studentsSetsRelated(this->students, s->name);
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsEarlyMaxBeginningsAtSecondHour::ConstraintStudentsEarlyMaxBeginningsAtSecondHour()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR;
}

ConstraintStudentsEarlyMaxBeginningsAtSecondHour::ConstraintStudentsEarlyMaxBeginningsAtSecondHour(double wp, int mBSH)
	: TimeConstraint(wp)
{
	this->type = CONSTRAINT_STUDENTS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR;
	this->maxBeginningsAtSecondHour=mBSH;
}

bool ConstraintStudentsEarlyMaxBeginningsAtSecondHour::computeInternalStructure(Rules& r)
{
	Q_UNUSED(r);
	
	return true;
}

bool ConstraintStudentsEarlyMaxBeginningsAtSecondHour::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsEarlyMaxBeginningsAtSecondHour::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);

	QString s="<ConstraintStudentsEarlyMaxBeginningsAtSecondHour>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Max_Beginnings_At_Second_Hour>"+QString::number(this->maxBeginningsAtSecondHour)+"</Max_Beginnings_At_Second_Hour>\n";
	s+="</ConstraintStudentsEarlyMaxBeginningsAtSecondHour>\n";
	return s;
}

QString ConstraintStudentsEarlyMaxBeginningsAtSecondHour::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);

	QString s;
	s+=QObject::tr("Students must arrive early, respecting maximum %1 arrivals at second hour")
	 .arg(this->maxBeginningsAtSecondHour);
	s+=", ";
	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));//s+=", ";

	return s;
}

QString ConstraintStudentsEarlyMaxBeginningsAtSecondHour::getDetailedDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Students must begin their courses early, respecting maximum %1 later arrivals, at second hour")
	 .arg(this->maxBeginningsAtSecondHour);s+="\n";
	s+=QObject::tr("(breaks and students set not available not counted)");s+="\n";
	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";
	s+="\n";

	return s;
}

double ConstraintStudentsEarlyMaxBeginningsAtSecondHour::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//considers the condition that the hours of subgroups begin as early as possible

	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}
	
	//int free;
	//int i;
	
	int conflTotal=0;
	
	//free=0; //number of free hours before starting the courses
	for(int i=0; i<r.nInternalSubgroups; i++){
		int nGapsFirstHour=0;
		for(int j=0; j<r.nDaysPerWeek; j++){
			int k;
			for(k=0; k<r.nHoursPerDay; k++)
				if(!breakDayHour[j][k] && !subgroupNotAvailableDayHour[i][j][k])
					break;
				
			bool firstHourOccupied=false;
			if(k<r.nHoursPerDay && subgroupsMatrix[i][j][k]>0)
				firstHourOccupied=true;
					
			bool dayOccupied=firstHourOccupied;
			
			bool illegalGap=false;
				
			for(k++; k<r.nHoursPerDay && !dayOccupied; k++)
				if(!breakDayHour[j][k] && !subgroupNotAvailableDayHour[i][j][k]){
					if(subgroupsMatrix[i][j][k]>0)
						dayOccupied=true;
					else
						illegalGap=true;
				}
				
			if(dayOccupied && illegalGap){
				if(conflictsString!=NULL){
					QString s=QObject::tr("Constraint students early max %1 beginnings at second hour broken for subgroup %2, on day %3,"
					 " because students have an illegal gap, increases conflicts total by %4")
					 .arg(this->maxBeginningsAtSecondHour)
					 .arg(r.internalSubgroupsList[i]->name)
					 .arg(r.daysOfTheWeek[j])
					 .arg(1*weightPercentage/100);
					 
					dl.append(s);
					cl.append(1*weightPercentage/100);
						
					*conflictsString+= s+"\n";
					
					conflTotal+=1;
				}
				
				if(c.nPlacedActivities==r.nInternalActivities){
					cout<<"day=="<<j<<endl;
					cout<<"subgroup=="<<qPrintable(r.internalSubgroupsList[i]->name)<<endl;
					for(int h2=0; h2<r.nHoursPerDay; h2++){
						for(int d2=0; d2<r.nDaysPerWeek; d2++)
							cout<<int(subgroupsMatrix[i][d2][h2]);
						cout<<endl;
					}
				
					assert(0);
				}
			}
			
			if(dayOccupied && !firstHourOccupied)
				nGapsFirstHour++;
		}
		
		if(nGapsFirstHour>this->maxBeginningsAtSecondHour){
			if(conflictsString!=NULL){
				QString s=QObject::tr("Constraint students early max %1 beginnings at second hour broken for subgroup %2,"
				 " because students have too many arrivals at second hour, increases conflicts total by %3")
				 .arg(this->maxBeginningsAtSecondHour)
				 .arg(r.internalSubgroupsList[i]->name)
				 .arg((nGapsFirstHour-this->maxBeginningsAtSecondHour)*weightPercentage/100);
				 
				dl.append(s);
				cl.append((nGapsFirstHour-this->maxBeginningsAtSecondHour)*weightPercentage/100);
					
				*conflictsString+= s+"\n";
				
				conflTotal+=(nGapsFirstHour-this->maxBeginningsAtSecondHour);
			}
			
			if(c.nPlacedActivities==r.nInternalActivities){
				for(int h=0; h<r.nHoursPerDay; h++){
					for(int d=0; d<r.nDaysPerWeek; d++)
						cout<<int(subgroupsMatrix[i][d][h]);
					cout<<endl;
				}
				cout<<endl;
			
				assert(0);
			}
		}
	}
					
	if(c.nPlacedActivities==r.nInternalActivities)
		if(weightPercentage==100)    //might be broken for partial solutions
			assert(conflTotal==0);
	return weightPercentage/100 * conflTotal;
}

bool ConstraintStudentsEarlyMaxBeginningsAtSecondHour::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);
	//if(a)
	//	;

	return false;
}

bool ConstraintStudentsEarlyMaxBeginningsAtSecondHour::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintStudentsEarlyMaxBeginningsAtSecondHour::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintStudentsEarlyMaxBeginningsAtSecondHour::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintStudentsEarlyMaxBeginningsAtSecondHour::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour::ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_SET_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR;
}

ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour::ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour(double wp, int mBSH, const QString& students)
	: TimeConstraint(wp)
{
	this->type = CONSTRAINT_STUDENTS_SET_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR;
	this->students=students;
	this->maxBeginningsAtSecondHour=mBSH;
}

bool ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour::computeInternalStructure(Rules& r)
{
	StudentsSet* ss=r.searchAugmentedStudentsSet(this->students);
	
	if(ss==NULL){
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Constraint students set early is wrong because it refers to inexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}	

	assert(ss);

	//this->nSubgroups=0;
	this->iSubgroupsList.clear();
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		/*for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
			if(r.internalSubgroupsList[tmp]->name == ss->name)
				break;*/
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		//assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
		//this->subgroups[this->nSubgroups++]=tmp;
		if(!this->iSubgroupsList.contains(tmp))
			this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			/*for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
				if(r.internalSubgroupsList[tmp]->name == sts->name)
					break;*/
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			//assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
			//this->subgroups[this->nSubgroups++]=tmp;
			if(!this->iSubgroupsList.contains(tmp))
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
				/*for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
					if(r.internalSubgroupsList[tmp]->name == sts->name)
						break;*/
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				//assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
				//this->subgroups[this->nSubgroups++]=tmp;
				if(!this->iSubgroupsList.contains(tmp))
					this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);
	return true;
}

bool ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s="<ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Max_Beginnings_At_Second_Hour>"+QString::number(this->maxBeginningsAtSecondHour)+"</Max_Beginnings_At_Second_Hour>\n";
	s+="	<Students>"+protect(this->students)+"</Students>\n";
	s+="</ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour>\n";
	return s;
}

QString ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;
	QString s;
	//s+=QObject::tr("Students set must begin their courses as early as possible (permitted by breaks and students not available)");s+=", ";

	s+=QObject::tr("Students set must arrive early, respecting maximum %1 arrivals at second hour")
	 .arg(this->maxBeginningsAtSecondHour);
	s+=", ";

	s+=QObject::tr("S:%1").arg(this->students); s+=", ";
	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);//s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour::getDetailedDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s=QObject::tr("Time constraint");s+="\n";

	s+=QObject::tr("Students set must begin their courses early, respecting maximum %1 later arrivals, at second hour")
	 .arg(this->maxBeginningsAtSecondHour);s+="\n";
	s+=QObject::tr("(breaks and students set not available not counted)");s+="\n";

	s+=QObject::tr("Students set=%1").arg(this->students); s+="\n";
	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";

	return s;
}

double ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//considers the condition that the hours of subgroups begin as early as possible

	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}
	
	//int free;
	//int i;
	
	int conflTotal=0;

	foreach(int i, this->iSubgroupsList){
	//for(i=0; i<r.nInternalSubgroups; i++){
		int nGapsFirstHour=0;
		for(int j=0; j<r.nDaysPerWeek; j++){
			int k;
			for(k=0; k<r.nHoursPerDay; k++)
				if(!breakDayHour[j][k] && !subgroupNotAvailableDayHour[i][j][k])
					break;
				
			bool firstHourOccupied=false;
			if(k<r.nHoursPerDay && subgroupsMatrix[i][j][k]>0)
				firstHourOccupied=true;
					
			bool dayOccupied=firstHourOccupied;
			
			bool illegalGap=false;
				
			for(k++; k<r.nHoursPerDay && !dayOccupied; k++)
				if(!breakDayHour[j][k] && !subgroupNotAvailableDayHour[i][j][k]){
					if(subgroupsMatrix[i][j][k]>0)
						dayOccupied=true;
					else
						illegalGap=true;
				}
				
			if(dayOccupied && illegalGap){
				if(conflictsString!=NULL){
					QString s=QObject::tr("Constraint students set early max %1 beginnings at second hour broken for subgroup %2, on day %3,"
					 " because students have an illegal gap, increases conflicts total by %4")
					 .arg(this->maxBeginningsAtSecondHour)
					 .arg(r.internalSubgroupsList[i]->name)
					 .arg(r.daysOfTheWeek[j])
					 .arg(1*weightPercentage/100);
					 
					dl.append(s);
					cl.append(1*weightPercentage/100);
						
					*conflictsString+= s+"\n";
					
					conflTotal+=1;
				}
				
				if(c.nPlacedActivities==r.nInternalActivities)
					assert(0);
			}
			
			if(dayOccupied && !firstHourOccupied)
				nGapsFirstHour++;
		}
		
		if(nGapsFirstHour>this->maxBeginningsAtSecondHour){
			if(conflictsString!=NULL){
				QString s=QObject::tr("Constraint students set early max %1 beginnings at second hour broken for subgroup %2,"
				 " because students have too many arrivals at second hour, increases conflicts total by %3")
				 .arg(this->maxBeginningsAtSecondHour)
				 .arg(r.internalSubgroupsList[i]->name)
				 .arg((nGapsFirstHour-this->maxBeginningsAtSecondHour)*weightPercentage/100);
				 
				dl.append(s);
				cl.append((nGapsFirstHour-this->maxBeginningsAtSecondHour)*weightPercentage/100);
					
				*conflictsString+= s+"\n";
				
				conflTotal+=(nGapsFirstHour-this->maxBeginningsAtSecondHour);
			}
			
			if(c.nPlacedActivities==r.nInternalActivities)
				assert(0);
		}
	}
					
	if(c.nPlacedActivities==r.nInternalActivities)
		if(weightPercentage==100)    //might be broken for partial solutions
			assert(conflTotal==0);
	return weightPercentage/100 * conflTotal;
}

bool ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);
	//if(a)
	//	;

	return false;
}

bool ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.studentsSetsRelated(this->students, s->name);
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsMaxHoursDaily::ConstraintStudentsMaxHoursDaily()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_MAX_HOURS_DAILY;
	this->maxHoursDaily = -1;
}

ConstraintStudentsMaxHoursDaily::ConstraintStudentsMaxHoursDaily(double wp, int maxnh)
	: TimeConstraint(wp)
{
	this->maxHoursDaily = maxnh;
	this->type = CONSTRAINT_STUDENTS_MAX_HOURS_DAILY;
}

bool ConstraintStudentsMaxHoursDaily::computeInternalStructure(Rules& r)
{
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	/*do nothing*/
	
	return true;
}

bool ConstraintStudentsMaxHoursDaily::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsMaxHoursDaily::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s="<ConstraintStudentsMaxHoursDaily>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	if(this->maxHoursDaily>=0)
		s+="	<Maximum_Hours_Daily>"+QString::number(this->maxHoursDaily)+"</Maximum_Hours_Daily>\n";
	else
		assert(0);
	s+="</ConstraintStudentsMaxHoursDaily>\n";
	return s;
}

QString ConstraintStudentsMaxHoursDaily::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s;
	s+=QObject::tr("Students max hours daily");s+=", ";
	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
	if(this->maxHoursDaily>=0)
		s+=(QObject::tr("MH:%1").arg(this->maxHoursDaily));
	else
		assert(0);

	return s;
}

QString ConstraintStudentsMaxHoursDaily::getDetailedDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("All students sets must have the maximum number of hours daily");s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	if(this->maxHoursDaily>=0){
		s+=(QObject::tr("Maximum recommended hours daily=%1").arg(this->maxHoursDaily));
		s+="\n";
	}
	else
		assert(0);

	return s;
}

double ConstraintStudentsMaxHoursDaily::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}

	int tmp;
	int too_much;
	//int too_little;
	
	assert(this->maxHoursDaily>=0);

	if(1){
		too_much=0;
		//too_little=0;
		for(int i=0; i<r.nInternalSubgroups; i++)
			for(int j=0; j<r.nDaysPerWeek; j++){
				tmp=0;
				for(int k=0; k<r.nHoursPerDay; k++){
					//OLD COMMENT
					//Here we want to see if we have a weekly activity or a 2 weeks activity
					//We don't do tmp+=subgroupsMatrix[i][j][k] because we already counted this as a hard hitness
					if(subgroupsMatrix[i][j][k]>=1)
						tmp++;
				}
				if(this->maxHoursDaily>=0 && tmp > this->maxHoursDaily){ //we would like no more than maxHoursDaily hours per day.
					too_much += 1; //tmp - this->maxHoursDaily;

					if(conflictsString!=NULL){
						QString s=QObject::tr("Time constraint students max hours daily broken for subgroup: %1, day: %2, lenght=%3, conflict increase=%4")
						 .arg(r.internalSubgroupsList[i]->name)
						 .arg(r.daysOfTheWeek[j])
						 .arg(QString::number(tmp))
						 .arg(weightPercentage/100*(1)); //tmp-this->maxHoursDaily));
						 
						dl.append(s);
						cl.append(weightPercentage/100*(1)); //tmp-this->maxHoursDaily));
					
						*conflictsString+= s+"\n";
					}
				}
			}
	}

	assert(too_much>=0);
	if(weightPercentage==100)
		assert(too_much==0);
	return too_much * weightPercentage/100;
}

bool ConstraintStudentsMaxHoursDaily::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);
	//if(a)
	//	;

	return false;
}

bool ConstraintStudentsMaxHoursDaily::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintStudentsMaxHoursDaily::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintStudentsMaxHoursDaily::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintStudentsMaxHoursDaily::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetMaxHoursDaily::ConstraintStudentsSetMaxHoursDaily()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY;
	this->maxHoursDaily = -1;
}

ConstraintStudentsSetMaxHoursDaily::ConstraintStudentsSetMaxHoursDaily(double wp, int maxnh, QString s)
	: TimeConstraint(wp)
{
	this->maxHoursDaily = maxnh;
	this->students = s;
	this->type = CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY;
}

bool ConstraintStudentsSetMaxHoursDaily::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsSetMaxHoursDaily::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s="<ConstraintStudentsSetMaxHoursDaily>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Maximum_Hours_Daily>"+QString::number(this->maxHoursDaily)+"</Maximum_Hours_Daily>\n";
	//s+="	<MinHoursDaily>"+QString::number(this->minHoursDaily)+"</MinHoursDaily>\n";
	s+="	<Students>"+protect(this->students)+"</Students>\n";
	s+="</ConstraintStudentsSetMaxHoursDaily>\n";
	return s;
}

QString ConstraintStudentsSetMaxHoursDaily::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s;
	s+=QObject::tr("Students set max hours daily");s+=", ";
	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
	if(this->maxHoursDaily>=0)
		s+=(QObject::tr("MH:%1").arg(this->maxHoursDaily));
	else
		assert(0);
	//if(this->minHoursDaily>=0)
	//	s+=(QObject::tr("mH:%1").arg(this->minHoursDaily));
	s+=", ";
	s+=(QObject::tr("St:%1").arg(this->students));

	return s;
}

QString ConstraintStudentsSetMaxHoursDaily::getDetailedDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Students set must respect the maximum number of hours daily");s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	if(this->maxHoursDaily>=0){
		s+=QObject::tr("Maximum recommended hours daily=%1").arg(this->maxHoursDaily);s+="\n";
	}
	else
		assert(0);
	//if(this->minHoursDaily>=0)
	//	s+=(QObject::tr("Minimum recommended hours daily=%1").arg(this->minHoursDaily));s+="\n";
	s+=(QObject::tr("Students set=%1").arg(this->students));s+="\n";

	return s;
}

bool ConstraintStudentsSetMaxHoursDaily::computeInternalStructure(Rules &r)
{
	StudentsSet* ss=r.searchAugmentedStudentsSet(this->students);
	
	if(ss==NULL){
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Constraint students set max hours daily is wrong because it refers to inexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}	

	assert(ss);

	//this->nSubgroups=0;
	this->iSubgroupsList.clear();
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		/*for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
			if(r.internalSubgroupsList[tmp]->name == ss->name)
				break;*/
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		//assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
		//this->subgroups[this->nSubgroups++]=tmp;
		if(!this->iSubgroupsList.contains(tmp))
			this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			/*for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
				if(r.internalSubgroupsList[tmp]->name == sts->name)
					break;*/
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			//assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
			//this->subgroups[this->nSubgroups++]=tmp;
			if(!this->iSubgroupsList.contains(tmp))
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
				/*for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
					if(r.internalSubgroupsList[tmp]->name == sts->name)
						break;*/
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				//assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
				//this->subgroups[this->nSubgroups++]=tmp;
				if(!this->iSubgroupsList.contains(tmp))
					this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);
		
	return true;
}

double ConstraintStudentsSetMaxHoursDaily::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}

	int tmp;
	int too_much;
	//int too_little;
	
	assert(this->maxHoursDaily>=0);

	if(1){
		too_much=0;
		//too_little=0;
		for(int sg=0; sg<this->iSubgroupsList.count(); sg++){
			int i=iSubgroupsList.at(sg);
			for(int j=0; j<r.nDaysPerWeek; j++){
				tmp=0;
				for(int k=0; k<r.nHoursPerDay; k++){
					//Here we want to see if we have a weekly activity or a 2 weeks activity
					//We don't do tmp+=subgroupsMatrix[i][j][k] because we already counted this as a hard hitness
					if(subgroupsMatrix[i][j][k]>=1)
						tmp++;
				}
				if(this->maxHoursDaily>=0 && tmp > this->maxHoursDaily){ //we would like no more than max_hours_daily hours per day.
					too_much += 1; //tmp - this->maxHoursDaily;

					if(conflictsString!=NULL){
						QString s=QObject::tr("Time constraint students set max hours daily broken for subgroup: %1, day: %2, lenght=%3, conflicts increase=%4")
						 .arg(r.internalSubgroupsList[i]->name)
						 .arg(r.daysOfTheWeek[j])
						 .arg(QString::number(tmp))
						 .arg( 1 /*(tmp-this->maxHoursDaily)*/ *weightPercentage/100);
						 
						dl.append(s);
						cl.append( 1 /*(tmp-this->maxHoursDaily)*/ *weightPercentage/100);
					
						*conflictsString+= s+"\n";
					}
				}
			}
		}
	}
	
	assert(too_much>=0);
	//assert(too_little>=0);
	if(weightPercentage==100)
		assert(too_much==0);
	return too_much * weightPercentage;
}

bool ConstraintStudentsSetMaxHoursDaily::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);
	//if(a)
	//	;

	return false;
}

bool ConstraintStudentsSetMaxHoursDaily::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintStudentsSetMaxHoursDaily::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintStudentsSetMaxHoursDaily::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintStudentsSetMaxHoursDaily::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.studentsSetsRelated(this->students, s->name);
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsMaxHoursContinuously::ConstraintStudentsMaxHoursContinuously()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_MAX_HOURS_CONTINUOUSLY;
	this->maxHoursContinuously = -1;
}

ConstraintStudentsMaxHoursContinuously::ConstraintStudentsMaxHoursContinuously(double wp, int maxnh)
	: TimeConstraint(wp)
{
	this->maxHoursContinuously = maxnh;
	this->type = CONSTRAINT_STUDENTS_MAX_HOURS_CONTINUOUSLY;
}

bool ConstraintStudentsMaxHoursContinuously::computeInternalStructure(Rules& r)
{
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	/*do nothing*/
	
	return true;
}

bool ConstraintStudentsMaxHoursContinuously::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsMaxHoursContinuously::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s="<ConstraintStudentsMaxHoursContinuously>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	if(this->maxHoursContinuously>=0)
		s+="	<Maximum_Hours_Continuously>"+QString::number(this->maxHoursContinuously)+"</Maximum_Hours_Continuously>\n";
	else
		assert(0);
	s+="</ConstraintStudentsMaxHoursContinuously>\n";
	return s;
}

QString ConstraintStudentsMaxHoursContinuously::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s;
	s+=QObject::tr("Students max hours continuously");s+=", ";
	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
	if(this->maxHoursContinuously>=0)
		s+=(QObject::tr("MH:%1", "Max hours").arg(this->maxHoursContinuously));
	else
		assert(0);

	return s;
}

QString ConstraintStudentsMaxHoursContinuously::getDetailedDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("All students sets must have the maximum number of hours continuously");s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	if(this->maxHoursContinuously>=0){
		s+=(QObject::tr("Maximum recommended hours continuously=%1").arg(this->maxHoursContinuously));
		s+="\n";
	}
	else
		assert(0);

	return s;
}

double ConstraintStudentsMaxHoursContinuously::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}
	
	int nbroken;

	nbroken=0;
	for(int i=0; i<r.nInternalSubgroups; i++){
		for(int d=0; d<r.nDaysPerWeek; d++){
			int nc=0;
			for(int h=0; h<r.nHoursPerDay; h++){
				if(subgroupsMatrix[i][d][h]>0)
					nc++;
				else{
					if(nc>this->maxHoursContinuously){
						nbroken++;

						if(conflictsString!=NULL){
							QString s=(QObject::tr(
							 "Time constraint students max %1 hours continuously broken for subgroup %2, on day %3, length=%4.")
							 .arg(QString::number(this->maxHoursContinuously))
							 .arg(r.internalSubgroupsList[i]->name)
							 .arg(r.daysOfTheWeek[d])
							 .arg(nc)
							 )
							 +
							 " "
							 +
							 (QObject::tr("This increases the conflicts total by %1").arg(QString::number(weightPercentage/100)));
							
							dl.append(s);
							cl.append(weightPercentage/100);
				
							*conflictsString+= s+"\n";
						}
					}
				
					nc=0;
				}
			}

			if(nc>this->maxHoursContinuously){
				nbroken++;

				if(conflictsString!=NULL){
					QString s=(QObject::tr(
					 "Time constraint students max %1 hours continuously broken for subgroup %2, on day %3, length=%4.")
					 .arg(QString::number(this->maxHoursContinuously))
					 .arg(r.internalSubgroupsList[i]->name)
					 .arg(r.daysOfTheWeek[d])
					 .arg(nc)
					 )
					 +
					 " "
					 +
					 (QObject::tr("This increases the conflicts total by %1").arg(QString::number(weightPercentage/100)));
							
					dl.append(s);
					cl.append(weightPercentage/100);
				
					*conflictsString+= s+"\n";
				}
			}
		}
	}

	if(weightPercentage==100)	
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsMaxHoursContinuously::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);
	//if(a)
	//	;

	return false;
}

bool ConstraintStudentsMaxHoursContinuously::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintStudentsMaxHoursContinuously::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintStudentsMaxHoursContinuously::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintStudentsMaxHoursContinuously::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetMaxHoursContinuously::ConstraintStudentsSetMaxHoursContinuously()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_SET_MAX_HOURS_CONTINUOUSLY;
	this->maxHoursContinuously = -1;
}

ConstraintStudentsSetMaxHoursContinuously::ConstraintStudentsSetMaxHoursContinuously(double wp, int maxnh, QString s)
	: TimeConstraint(wp)
{
	this->maxHoursContinuously = maxnh;
	this->students = s;
	this->type = CONSTRAINT_STUDENTS_SET_MAX_HOURS_CONTINUOUSLY;
}

bool ConstraintStudentsSetMaxHoursContinuously::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsSetMaxHoursContinuously::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s="<ConstraintStudentsSetMaxHoursContinuously>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Maximum_Hours_Continuously>"+QString::number(this->maxHoursContinuously)+"</Maximum_Hours_Continuously>\n";
	//s+="	<MinHoursDaily>"+QString::number(this->minHoursDaily)+"</MinHoursDaily>\n";
	s+="	<Students>"+protect(this->students)+"</Students>\n";
	s+="</ConstraintStudentsSetMaxHoursContinuously>\n";
	return s;
}

QString ConstraintStudentsSetMaxHoursContinuously::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s;
	s+=QObject::tr("Students set max hours continuously");s+=", ";
	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
	if(this->maxHoursContinuously>=0)
		s+=(QObject::tr("MH:%1", "Max hours").arg(this->maxHoursContinuously));
	else
		assert(0);
	//if(this->minHoursDaily>=0)
	//	s+=(QObject::tr("mH:%1").arg(this->minHoursDaily));
	s+=", ";
	s+=(QObject::tr("St:%1").arg(this->students));

	return s;
}

QString ConstraintStudentsSetMaxHoursContinuously::getDetailedDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Students set must respect the maximum number of hours continuously");s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	if(this->maxHoursContinuously>=0){
		s+=QObject::tr("Maximum recommended hours continuously=%1").arg(this->maxHoursContinuously);s+="\n";
	}
	else
		assert(0);
	//if(this->minHoursDaily>=0)
	//	s+=(QObject::tr("Minimum recommended hours daily=%1").arg(this->minHoursDaily));s+="\n";
	s+=(QObject::tr("Students set=%1").arg(this->students));s+="\n";

	return s;
}

bool ConstraintStudentsSetMaxHoursContinuously::computeInternalStructure(Rules &r)
{
	StudentsSet* ss=r.searchAugmentedStudentsSet(this->students);
	
	if(ss==NULL){
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Constraint students set max hours continuously is wrong because it refers to inexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}	

	assert(ss);

	//this->nSubgroups=0;
	this->iSubgroupsList.clear();
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		/*for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
			if(r.internalSubgroupsList[tmp]->name == ss->name)
				break;*/
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		//assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
		//this->subgroups[this->nSubgroups++]=tmp;
		if(!this->iSubgroupsList.contains(tmp))
			this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			/*for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
				if(r.internalSubgroupsList[tmp]->name == sts->name)
					break;*/
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			//assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
			//this->subgroups[this->nSubgroups++]=tmp;
			if(!this->iSubgroupsList.contains(tmp))
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
				/*for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
					if(r.internalSubgroupsList[tmp]->name == sts->name)
						break;*/
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				//assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
				//this->subgroups[this->nSubgroups++]=tmp;
				if(!this->iSubgroupsList.contains(tmp))
					this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);
		
	return true;
}

double ConstraintStudentsSetMaxHoursContinuously::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	nbroken=0;
	foreach(int i, this->iSubgroupsList){
		for(int d=0; d<r.nDaysPerWeek; d++){
			int nc=0;
			for(int h=0; h<r.nHoursPerDay; h++){
				if(subgroupsMatrix[i][d][h]>0)
					nc++;
				else{
					if(nc>this->maxHoursContinuously){
						nbroken++;

						if(conflictsString!=NULL){
							QString s=(QObject::tr(
							 "Time constraint students set max %1 hours continuously broken for subgroup %2, on day %3, length=%4.")
							 .arg(QString::number(this->maxHoursContinuously))
							 .arg(r.internalSubgroupsList[i]->name)
							 .arg(r.daysOfTheWeek[d])
							 .arg(nc)
							 )
							 +
							 " "
							 +
							 (QObject::tr("This increases the conflicts total by %1").arg(QString::number(weightPercentage/100)));
							
							dl.append(s);
							cl.append(weightPercentage/100);
				
							*conflictsString+= s+"\n";
						}
					}
				
					nc=0;
				}
			}

			if(nc>this->maxHoursContinuously){
				nbroken++;

				if(conflictsString!=NULL){
					QString s=(QObject::tr(
					 "Time constraint students set max %1 hours continuously broken for subgroup %2, on day %3, length=%4.")
					 .arg(QString::number(this->maxHoursContinuously))
					 .arg(r.internalSubgroupsList[i]->name)
					 .arg(r.daysOfTheWeek[d])
					 .arg(nc)
					 )
					 +
					 " "
					 +
					 (QObject::tr("This increases the conflicts total by %1").arg(QString::number(weightPercentage/100)));
							
					dl.append(s);
					cl.append(weightPercentage/100);
				
					*conflictsString+= s+"\n";
				}
			}
		}
	}

	if(weightPercentage==100)	
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsSetMaxHoursContinuously::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);
	//if(a)
	//	;

	return false;
}

bool ConstraintStudentsSetMaxHoursContinuously::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintStudentsSetMaxHoursContinuously::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintStudentsSetMaxHoursContinuously::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintStudentsSetMaxHoursContinuously::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.studentsSetsRelated(this->students, s->name);
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsMinHoursDaily::ConstraintStudentsMinHoursDaily()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_MIN_HOURS_DAILY;
	this->minHoursDaily = -1;
}

ConstraintStudentsMinHoursDaily::ConstraintStudentsMinHoursDaily(double wp, int minnh)
	: TimeConstraint(wp)
{
	this->minHoursDaily = minnh;
	this->type = CONSTRAINT_STUDENTS_MIN_HOURS_DAILY;
}

bool ConstraintStudentsMinHoursDaily::computeInternalStructure(Rules& r)
{
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	/*do nothing*/
	
	return true;
}

bool ConstraintStudentsMinHoursDaily::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsMinHoursDaily::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s="<ConstraintStudentsMinHoursDaily>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	if(this->minHoursDaily>=0)
		s+="	<Minimum_Hours_Daily>"+QString::number(this->minHoursDaily)+"</Minimum_Hours_Daily>\n";
	else
		assert(0);
	s+="</ConstraintStudentsMinHoursDaily>\n";
	return s;
}

QString ConstraintStudentsMinHoursDaily::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s;
	s+=QObject::tr("Students min hours daily");s+=", ";
	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
	if(this->minHoursDaily>=0)
		s+=(QObject::tr("mH:%1").arg(this->minHoursDaily));
	else
		assert(0);

	return s;
}

QString ConstraintStudentsMinHoursDaily::getDetailedDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("All students sets must have the minimum number of hours daily");s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	if(this->minHoursDaily>=0){
		s+=(QObject::tr("Minimum recommended hours daily=%1").arg(this->minHoursDaily));
		s+="\n";
	}
	else
		assert(0);
	s+=QObject::tr("Note: FET considers that each day of the week must have the minimum number of working hours, so you cannot have empty days for affected students (constraint is not flexible)");
	s+="\n";

	return s;
}

double ConstraintStudentsMinHoursDaily::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}

	int tmp;
	int too_little;
	
	assert(this->minHoursDaily>=0);

	too_little=0;
	for(int i=0; i<r.nInternalSubgroups; i++)
		for(int j=0; j<r.nDaysPerWeek; j++){
			tmp=0;
			for(int k=0; k<r.nHoursPerDay; k++){
				if(subgroupsMatrix[i][j][k]>=1)
					tmp++;
			}
			
			//if(tmp>0) - smart, for empty days does not consider
			
			if(tmp>0 && this->minHoursDaily>=0 && tmp < this->minHoursDaily){ //we would like no less than minHoursDaily hours per day.
				too_little += - tmp + this->minHoursDaily;

				if(conflictsString!=NULL){
					QString s=QObject::tr("Time constraint students min hours daily broken for subgroup: %1, day: %2, lenght=%3, conflict increase=%4")
					 .arg(r.internalSubgroupsList[i]->name)
					 .arg(r.daysOfTheWeek[j])
					 .arg(QString::number(tmp))
					 .arg(weightPercentage/100*(-tmp+this->minHoursDaily));
						 
					dl.append(s);
					cl.append(weightPercentage/100*(-tmp+this->minHoursDaily));
					
					*conflictsString+= s+"\n";
				}
			}
		}

	//for empty days should not consider
			
	assert(too_little>=0);

	/*int na=0;
	for(int i=0; i<r.nInternalActivities; i++)
		if(c.times[i]!=UNALLOCATED_TIME)
			na++;*/
	if(c.nPlacedActivities==r.nInternalActivities)
	//if(na==r.nInternalActivities)
		if(weightPercentage==100) //does not work for partial solutions
			assert(too_little==0);

	return too_little * weightPercentage/100;
}

bool ConstraintStudentsMinHoursDaily::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);
	//if(a)
	//	;

	return false;
}

bool ConstraintStudentsMinHoursDaily::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintStudentsMinHoursDaily::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintStudentsMinHoursDaily::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintStudentsMinHoursDaily::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetMinHoursDaily::ConstraintStudentsSetMinHoursDaily()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY;
	this->minHoursDaily = -1;
}

ConstraintStudentsSetMinHoursDaily::ConstraintStudentsSetMinHoursDaily(double wp, int minnh, QString s)
	: TimeConstraint(wp)
{
	this->minHoursDaily = minnh;
	this->students = s;
	this->type = CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY;
}

bool ConstraintStudentsSetMinHoursDaily::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsSetMinHoursDaily::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s="<ConstraintStudentsSetMinHoursDaily>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Minimum_Hours_Daily>"+QString::number(this->minHoursDaily)+"</Minimum_Hours_Daily>\n";
	//s+="	<MinHoursDaily>"+QString::number(this->minHoursDaily)+"</MinHoursDaily>\n";
	s+="	<Students>"+protect(this->students)+"</Students>\n";
	s+="</ConstraintStudentsSetMinHoursDaily>\n";
	return s;
}

QString ConstraintStudentsSetMinHoursDaily::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s;
	s+=QObject::tr("Students set min hours daily");s+=", ";
	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
	if(this->minHoursDaily>=0)
		s+=(QObject::tr("mH:%1").arg(this->minHoursDaily));
	else
		assert(0);
	//if(this->minHoursDaily>=0)
	//	s+=(QObject::tr("mH:%1").arg(this->minHoursDaily));
	s+=", ";
	s+=(QObject::tr("St:%1").arg(this->students));

	return s;
}

QString ConstraintStudentsSetMinHoursDaily::getDetailedDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Students set must respect the minimum number of hours daily");s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	if(this->minHoursDaily>=0){
		s+=QObject::tr("Minimum recommended hours daily=%1").arg(this->minHoursDaily);s+="\n";
	}
	else
		assert(0);
	//if(this->minHoursDaily>=0)
	//	s+=(QObject::tr("Minimum recommended hours daily=%1").arg(this->minHoursDaily));s+="\n";
	s+=(QObject::tr("Students set=%1").arg(this->students));s+="\n";
	s+=QObject::tr("Note: FET considers that each day of the week must have the minimum number of working hours, so you cannot have empty days for affected students (constraint is not flexible)");
	s+="\n";

	return s;
}

bool ConstraintStudentsSetMinHoursDaily::computeInternalStructure(Rules &r)
{
	StudentsSet* ss=r.searchAugmentedStudentsSet(this->students);
	
	if(ss==NULL){
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Constraint students set min hours daily is wrong because it refers to inexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}	

	assert(ss);

	this->iSubgroupsList.clear();
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		/*for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
			if(r.internalSubgroupsList[tmp]->name == ss->name)
				break;*/
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		//assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
		//this->subgroups[this->nSubgroups++]=tmp;
		if(!this->iSubgroupsList.contains(tmp))
			this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			/*for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
				if(r.internalSubgroupsList[tmp]->name == sts->name)
					break;*/
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			//assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
			//this->subgroups[this->nSubgroups++]=tmp;
			if(!this->iSubgroupsList.contains(tmp))
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
				/*for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
					if(r.internalSubgroupsList[tmp]->name == sts->name)
						break;*/
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				//assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
				//this->subgroups[this->nSubgroups++]=tmp;
				if(!this->iSubgroupsList.contains(tmp))
					this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);
		
	return true;
}

double ConstraintStudentsSetMinHoursDaily::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}

	int tmp;
	int too_little;
	
	assert(this->minHoursDaily>=0);

	too_little=0;
	for(int sg=0; sg<this->iSubgroupsList.count(); sg++){
		int i=iSubgroupsList.at(sg);
		for(int j=0; j<r.nDaysPerWeek; j++){
			tmp=0;
			for(int k=0; k<r.nHoursPerDay; k++){
				if(subgroupsMatrix[i][j][k]>=1)
					tmp++;
			}
			if(tmp>0 && this->minHoursDaily>=0 && tmp < this->minHoursDaily){ //we would like no more than max_hours_daily hours per day.
				too_little += - tmp + this->minHoursDaily;

				if(conflictsString!=NULL){
					QString s=QObject::tr("Time constraint students set min hours daily broken for subgroup: %1, day: %2, lenght=%3, conflicts increase=%4")
					 .arg(r.internalSubgroupsList[i]->name)
					 .arg(r.daysOfTheWeek[j])
					 .arg(QString::number(tmp))
					 .arg((-tmp+this->minHoursDaily)*weightPercentage/100);
						 
					dl.append(s);
					cl.append((-tmp+this->minHoursDaily)*weightPercentage/100);
					
					*conflictsString+= s+"\n";
				}
			}
		}
	}
	
	assert(too_little>=0);

	/*int na=0;
	for(int i=0; i<r.nInternalActivities; i++)
		if(c.times[i]!=UNALLOCATED_TIME)
			na++;*/
	if(c.nPlacedActivities==r.nInternalActivities)
	//if(na==r.nInternalActivities)
		if(weightPercentage==100) //does not work for partial solutions
			assert(too_little==0);

	return too_little * weightPercentage;
}

bool ConstraintStudentsSetMinHoursDaily::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);
	//if(a)
	//	;

	return false;
}

bool ConstraintStudentsSetMinHoursDaily::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintStudentsSetMinHoursDaily::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintStudentsSetMinHoursDaily::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintStudentsSetMinHoursDaily::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.studentsSetsRelated(this->students, s->name);
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivityPreferredStartingTime::ConstraintActivityPreferredStartingTime()
	: TimeConstraint()
{
	this->type = CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME;
}

ConstraintActivityPreferredStartingTime::ConstraintActivityPreferredStartingTime(double wp, int actId, int d, int h)
	: TimeConstraint(wp)
{
	this->activityId = actId;
	this->day = d;
	this->hour = h;
	this->type = CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME;
}

bool ConstraintActivityPreferredStartingTime::operator==(ConstraintActivityPreferredStartingTime& c){
	if(this->day!=c.day)
		return false;
	if(this->hour!=c.hour)
		return false;
	if(this->activityId!=c.activityId)
		return false;
	//if(this->compulsory!=c.compulsory)
	//	return false;
	if(this->weightPercentage!=c.weightPercentage)
		return false;
	return true;
}

bool ConstraintActivityPreferredStartingTime::computeInternalStructure(Rules& r)
{
	Activity* act;
	int i;
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		if(act->id==this->activityId)
			break;
	}
	
	if(i==r.nInternalActivities){	
		//assert(0);
		QMessageBox::warning(NULL, QObject::tr("FET error in data"), 
			QObject ::tr("Following constraint is wrong (because it refers to invalid activity id. Please correct (maybe removing it is a solution)):\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}

	if(this->day >= r.nDaysPerWeek){
		QMessageBox::information(NULL, QObject::tr("FET information"),
		 QObject::tr("Constraint activity preferred time is wrong because it refers to removed day. Please correct"
		 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}
	if(this->hour == r.nHoursPerDay){
		QMessageBox::information(NULL, QObject::tr("FET information"),
		 QObject::tr("Constraint activity preferred time is wrong because preferred hour is too late (after the last acceptable slot). Please correct"
		 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}
	if(this->hour > r.nHoursPerDay){
		QMessageBox::information(NULL, QObject::tr("FET information"),
		 QObject::tr("Constraint activity preferred time is wrong because it refers to removed hour. Please correct"
		 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}

	this->activityIndex=i;	
	return true;
}

bool ConstraintActivityPreferredStartingTime::hasInactiveActivities(Rules& r)
{
	if(r.inactiveActivities.contains(this->activityId))
		return true;
	return false;
}

QString ConstraintActivityPreferredStartingTime::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s="<ConstraintActivityPreferredStartingTime>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Activity_Id>"+QString::number(this->activityId)+"</Activity_Id>\n";
	if(this->day>=0)
		s+="	<Preferred_Day>"+protect(r.daysOfTheWeek[this->day])+"</Preferred_Day>\n";
	if(this->hour>=0)
		s+="	<Preferred_Hour>"+protect(r.hoursOfTheDay[this->hour])+"</Preferred_Hour>\n";
	s+="</ConstraintActivityPreferredStartingTime>\n";
	return s;
}

QString ConstraintActivityPreferredStartingTime::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s;
	s+=(QObject::tr("Act. id:%1").arg(this->activityId));
	
	//* write the teachers, subject and students sets
	//added in version 4.1.4
	int ai;
	for(ai=0; ai<r.activitiesList.size(); ai++)
		if(r.activitiesList[ai]->id==this->activityId)
			break;
	if(ai==r.activitiesList.size()){
		s+=QObject::tr(" Invalid (inexistent) activity id for constraint activity preferred starting time");
		return s;
	}
	assert(ai<r.activitiesList.size());
	s+=" (";
	
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
	
	s+=")";
	//* end section
	
	s+=", ";
	s+=QObject::tr("must be scheduled starting at: ");
	if(this->day>=0){
		s+=r.daysOfTheWeek[this->day];
		s+=" ";
	}
	if(this->hour>=0){
		s+=r.hoursOfTheDay[this->hour];
	}
	s+=", ";

	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));//s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintActivityPreferredStartingTime::getDetailedDescription(Rules& r)
{
	QString s=QObject::tr("Time constraint");s+="\n";
	s+=(QObject::tr("Activity with id=%1").arg(this->activityId));
	
	//* write the teachers, subject and students sets
	//added in version 4.1.4
	int ai;
	for(ai=0; ai<r.activitiesList.size(); ai++)
		if(r.activitiesList[ai]->id==this->activityId)
			break;
	if(ai==r.activitiesList.size()){
		s+=QObject::tr(" Invalid (inexistent) activity id for constraint activity preferred starting time");
		s+="\n";
		return s;
	}
	assert(ai<r.activitiesList.size());
	s+=" (";
	
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
	
	s+=")";
	//* end section

	s+="\n";
	s+=QObject::tr("must be scheduled starting at: ");
	if(this->day>=0){
		s+=r.daysOfTheWeek[this->day];
		s+=" ";
	}
	if(this->hour>=0){
		s+=r.hoursOfTheDay[this->hour];
	}
	s+="\n";

	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

double ConstraintActivityPreferredStartingTime::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	assert(r.internalStructureComputed);

	nbroken=0;
	if(c.times[this->activityIndex]!=UNALLOCATED_TIME){
		int d=c.times[this->activityIndex]%r.nDaysPerWeek; //the day when this activity was scheduled
		int h=c.times[this->activityIndex]/r.nDaysPerWeek; //the hour
		if(this->day>=0)
			nbroken+=abs(this->day-d);
		if(this->hour>=0)
			nbroken+=abs(this->hour-h);
		/*if(r.internalActivitiesList[this->activityIndex].parity==PARITY_WEEKLY) //for weekly activities, double the conflicts
			nbroken*=2;*/
	}
	if(nbroken>0)
		nbroken=1;

	if(conflictsString!=NULL && nbroken>0){
		QString s=QObject::tr("Time constraint activity preferred starting time broken for activity with id=%1, increases conflicts total by %2")
		 .arg(this->activityId)
		 .arg(weightPercentage/100*nbroken);

		dl.append(s);
		cl.append(weightPercentage/100*nbroken);
	
		*conflictsString+= s+"\n";
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return nbroken * weightPercentage/100;
}

bool ConstraintActivityPreferredStartingTime::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);

	if(this->activityId==a->id)
		return true;
	return false;
}

bool ConstraintActivityPreferredStartingTime::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintActivityPreferredStartingTime::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintActivityPreferredStartingTime::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintActivityPreferredStartingTime::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/
		
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivityPreferredTimeSlots::ConstraintActivityPreferredTimeSlots()
	: TimeConstraint()
{
	this->type = CONSTRAINT_ACTIVITY_PREFERRED_TIME_SLOTS;
}

ConstraintActivityPreferredTimeSlots::ConstraintActivityPreferredTimeSlots(double wp, int actId, int nPT, int d[], int h[])
	: TimeConstraint(wp)
{
	this->p_activityId=actId;
	this->p_nPreferredTimeSlots=nPT;
	assert(nPT<=MAX_N_CONSTRAINT_ACTIVITY_PREFERRED_TIME_SLOTS);
	for(int i=0; i<nPT; i++){
		this->p_days[i]=d[i];
		this->p_hours[i]=h[i];
	}
	this->type=CONSTRAINT_ACTIVITY_PREFERRED_TIME_SLOTS;
}

bool ConstraintActivityPreferredTimeSlots::computeInternalStructure(Rules& r)
{
	Activity* act;
	int i;
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		if(act->id==this->p_activityId)
			break;
	}

	if(i==r.nInternalActivities){
		QMessageBox::warning(NULL, QObject::tr("FET error in data"), 
			QObject ::tr("Following constraint is wrong (because it refers to invalid activity id. Please correct it (maybe removing it is a solution)):\n%1").arg(this->getDetailedDescription(r)));
		//assert(0);
		return false;
	}

	for(int k=0; k<p_nPreferredTimeSlots; k++){
		if(this->p_days[k] >= r.nDaysPerWeek){
			QMessageBox::information(NULL, QObject::tr("FET information"),
			 QObject::tr("Constraint activity preferred time slots is wrong because it refers to removed day. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}		
		if(this->p_hours[k] == r.nHoursPerDay){
			QMessageBox::information(NULL, QObject::tr("FET information"),
			 QObject::tr("Constraint activity preferred time slots is wrong because a preferred hour is too late (after the last acceptable slot). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
		if(this->p_hours[k] > r.nHoursPerDay){
			QMessageBox::information(NULL, QObject::tr("FET information"),
			 QObject::tr("Constraint activity preferred time slots is wrong because it refers to removed hour. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}

		if(this->p_hours[k]<0 || this->p_days[k]<0){
			QMessageBox::information(NULL, QObject::tr("FET information"),
			 QObject::tr("Constraint activity preferred time slots is wrong because it has hour or day not specified for a slot (-1). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
	}

	this->p_activityIndex=i;	
	return true;
}

bool ConstraintActivityPreferredTimeSlots::hasInactiveActivities(Rules& r)
{
	if(r.inactiveActivities.contains(this->p_activityId))
		return true;
	return false;
}

QString ConstraintActivityPreferredTimeSlots::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	//Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s="<ConstraintActivityPreferredTimeSlots>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Activity_Id>"+QString::number(this->p_activityId)+"</Activity_Id>\n";
	s+="	<Number_of_Preferred_Time_Slots>"+QString::number(this->p_nPreferredTimeSlots)+"</Number_of_Preferred_Time_Slots>\n";
	for(int i=0; i<p_nPreferredTimeSlots; i++){
		s+="	<Preferred_Time_Slot>\n";
		if(this->p_days[i]>=0)
			s+="		<Preferred_Day>"+protect(r.daysOfTheWeek[this->p_days[i]])+"</Preferred_Day>\n";
		if(this->p_hours[i]>=0)
			s+="		<Preferred_Hour>"+protect(r.hoursOfTheDay[this->p_hours[i]])+"</Preferred_Hour>\n";
		s+="	</Preferred_Time_Slot>\n";
	}
	s+="</ConstraintActivityPreferredTimeSlots>\n";
	return s;
}

QString ConstraintActivityPreferredTimeSlots::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	//Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s;
	s+=(QObject::tr("Act. id:%1").arg(this->p_activityId));
	
	//* write the teachers, subject and students sets
	//added in version 4.1.4
	int ai;
	for(ai=0; ai<r.activitiesList.size(); ai++)
		if(r.activitiesList[ai]->id==this->p_activityId)
			break;
	if(ai==r.activitiesList.size()){
		s+=QObject::tr(" Invalid (inexistent) activity id for constraint activity preferred time slots");
		return s;
	}
	assert(ai<r.activitiesList.size());
	s+=" (";
	
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
	
	s+=")";
	//* end section
	
	s+=", ";
	s+=QObject::tr("must be scheduled in the allowed slots: ");
	for(int i=0; i<this->p_nPreferredTimeSlots; i++){
		//s+=QString::number(i+1);
		//s+=":";
		if(this->p_days[i]>=0){
			s+=r.daysOfTheWeek[this->p_days[i]];
			s+=" ";
		}
		if(this->p_hours[i]>=0){
			s+=r.hoursOfTheDay[this->p_hours[i]];
		}
		s+="; ";
	}

	s+=(QObject::tr("WP:%1").arg(this->weightPercentage));//s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintActivityPreferredTimeSlots::getDetailedDescription(Rules& r)
{
	QString s=QObject::tr("Time constraint");s+="\n";
	s+=(QObject::tr("Activity with id=%1").arg(this->p_activityId));
	
	//* write the teachers, subject and students sets
	//added in version 4.1.4
	int ai;
	for(ai=0; ai<r.activitiesList.size(); ai++)
		if(r.activitiesList[ai]->id==this->p_activityId)
			break;
	if(ai==r.activitiesList.size()){
		s+=QObject::tr(" Invalid (inexistent) activity id for constraint activity preferred time slots");
		s+="\n";
		return s;
	}
	assert(ai<r.activitiesList.size());
	s+=" (";
	
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
	
	s+=")";
	//* end section
	
	s+="\n";
	s+=QObject::tr("must be scheduled in the allowed slots (all hours of activity are in the allowed slots):\n");
	for(int i=0; i<this->p_nPreferredTimeSlots; i++){
		//s+=QString::number(i+1);
		//s+=". ";
		if(this->p_days[i]>=0){
			s+=r.daysOfTheWeek[this->p_days[i]];
			s+=" ";
		}
		if(this->p_hours[i]>=0){
			s+=r.hoursOfTheDay[this->p_hours[i]];
		}
		//s+="\n";
		s+=";  ";
	}
	s+="\n";

	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

double ConstraintActivityPreferredTimeSlots::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	assert(r.internalStructureComputed);
	
	bool allowed[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
	for(int d=0; d<r.nDaysPerWeek; d++)
		for(int h=0; h<r.nHoursPerDay; h++)
			allowed[d][h]=false;
	for(int i=0; i<this->p_nPreferredTimeSlots; i++){
		if(this->p_days[i]>=0 && this->p_hours[i]>=0)
			allowed[this->p_days[i]][this->p_hours[i]]=true;
		else
			assert(0);
	}

	nbroken=0;
	if(c.times[this->p_activityIndex]!=UNALLOCATED_TIME){
		int d=c.times[this->p_activityIndex]%r.nDaysPerWeek; //the day when this activity was scheduled
		int h=c.times[this->p_activityIndex]/r.nDaysPerWeek; //the hour
		for(int dur=0; dur<r.internalActivitiesList[this->p_activityIndex].duration; dur++)
			if(!allowed[d][h+dur])
				nbroken++;
	}

	if(conflictsString!=NULL && nbroken>0){
		QString s=QObject::tr("Time constraint activity preferred time slots broken for activity with id=%1 on %2 hours, increases conflicts total by %3")
		 .arg(this->p_activityId)
		 .arg(nbroken)
		 .arg(weightPercentage/100*nbroken);
		 
		 
		 
		s+=" ";
		QString tn;
		foreach(QString t, r.internalActivitiesList[this->p_activityIndex].teachersNames){
			tn+=t;
			tn+=" ";
		}
		QString sn;
		foreach(QString t, r.internalActivitiesList[this->p_activityIndex].studentsNames){
			sn+=t;
			sn+=" ";
		}
		s+="(";
		s+=QObject::tr("teachers %1, students sets %2, subject %3")
		 .arg(tn)
		 .arg(sn)
		 .arg(r.internalActivitiesList[this->p_activityIndex].subjectName);
		if(r.internalActivitiesList[this->p_activityIndex].activityTagName!="")
			s+=QObject::tr(", activity tag %4").arg(r.internalActivitiesList[this->p_activityIndex].activityTagName);

		s+=")";



		dl.append(s);
		cl.append(weightPercentage/100*nbroken);
	
		*conflictsString+= s+"\n";
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return nbroken * weightPercentage/100;
}

bool ConstraintActivityPreferredTimeSlots::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);

	if(this->p_activityId==a->id)
		return true;
	return false;
}

bool ConstraintActivityPreferredTimeSlots::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintActivityPreferredTimeSlots::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintActivityPreferredTimeSlots::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintActivityPreferredTimeSlots::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/
		
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivitiesPreferredTimeSlots::ConstraintActivitiesPreferredTimeSlots()
	: TimeConstraint()
{
	this->type = CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS;
}

ConstraintActivitiesPreferredTimeSlots::ConstraintActivitiesPreferredTimeSlots(double wp, QString te,
	QString st, QString su, QString sut, int nPT, int d[], int h[])
	: TimeConstraint(wp)
{
	this->p_teacherName=te;
	this->p_subjectName=su;
	this->p_activityTagName=sut;
	this->p_studentsName=st;
	this->p_nPreferredTimeSlots=nPT;
	assert(nPT<=MAX_N_CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS);
	for(int i=0; i<nPT; i++){
		this->p_days[i]=d[i];
		this->p_hours[i]=h[i];
	}
	this->type=CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS;
}

bool ConstraintActivitiesPreferredTimeSlots::computeInternalStructure(Rules& r)
{
	//assert(this->teacherName!="" || this->studentsName!="" || this->subjectName!="" || this->subjectTagName!="");

	this->p_nActivities=0;

	QStringList::iterator it;
	Activity* act;
	int i;
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];

		//check if this activity has the corresponding teacher
		if(this->p_teacherName!=""){
			it = act->teachersNames.find(this->p_teacherName);
			if(it==act->teachersNames.end())
				continue;
		}
		//check if this activity has the corresponding students
		if(this->p_studentsName!=""){
			bool commonStudents=false;
			foreach(QString st, act->studentsNames)
				if(r.studentsSetsRelated(st, p_studentsName)){
					commonStudents=true;
					break;
				}
		
			/*it = act->studentsNames.find(this->studentsName);
			if(it==act->studentsNames.end())
				continue;*/
			if(!commonStudents)
				continue;
		}
		//check if this activity has the corresponding subject
		if(this->p_subjectName!="" && act->subjectName!=this->p_subjectName){
				continue;
		}
		//check if this activity has the corresponding activity tag
		if(this->p_activityTagName!="" && act->activityTagName!=this->p_activityTagName){
				continue;
		}
	
		assert(this->p_nActivities < MAX_ACTIVITIES);	
		this->p_activitiesIndices[this->p_nActivities++]=i;
	}

	//////////////////////	
	for(int k=0; k<p_nPreferredTimeSlots; k++){
		if(this->p_days[k] >= r.nDaysPerWeek){
			QMessageBox::information(NULL, QObject::tr("FET information"),
			 QObject::tr("Constraint activities preferred time slots is wrong because it refers to removed day. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
		if(this->p_hours[k] == r.nHoursPerDay){
			QMessageBox::information(NULL, QObject::tr("FET information"),
			 QObject::tr("Constraint activities preferred time slots is wrong because a preferred hour is too late (after the last acceptable slot). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
		if(this->p_hours[k] > r.nHoursPerDay){
			QMessageBox::information(NULL, QObject::tr("FET information"),
			 QObject::tr("Constraint activities preferred time slots is wrong because it refers to removed hour. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
		if(this->p_hours[k]<0 || this->p_days[k]<0){
			QMessageBox::information(NULL, QObject::tr("FET information"),
			 QObject::tr("Constraint activities preferred time slots is wrong because hour or day is not specified for a slot (-1). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
	}
	///////////////////////
	
	if(this->p_nActivities>0)
		return true;
	else{
		QMessageBox::warning(NULL, QObject::tr("FET error in data"), 
			QObject ::tr("Following constraint is wrong (refers to no activities. Please correct it):\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}
}

bool ConstraintActivitiesPreferredTimeSlots::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintActivitiesPreferredTimeSlots::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	//Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s="<ConstraintActivitiesPreferredTimeSlots>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	//if(this->teacherName!="")
		s+="	<Teacher_Name>"+protect(this->p_teacherName)+"</Teacher_Name>\n";
	//if(this->studentsName!="")
		s+="	<Students_Name>"+protect(this->p_studentsName)+"</Students_Name>\n";
	//if(this->subjectName!="")
		s+="	<Subject_Name>"+protect(this->p_subjectName)+"</Subject_Name>\n";
	//if(this->subjectTagName!="")
		s+="	<Activity_Tag_Name>"+protect(this->p_activityTagName)+"</Activity_Tag_Name>\n";
	s+="	<Number_of_Preferred_Time_Slots>"+QString::number(this->p_nPreferredTimeSlots)+"</Number_of_Preferred_Time_Slots>\n";
	for(int i=0; i<p_nPreferredTimeSlots; i++){
		s+="	<Preferred_Time_Slot>\n";
		if(this->p_days[i]>=0)
			s+="		<Preferred_Day>"+protect(r.daysOfTheWeek[this->p_days[i]])+"</Preferred_Day>\n";
		if(this->p_hours[i]>=0)
			s+="		<Preferred_Hour>"+protect(r.hoursOfTheDay[this->p_hours[i]])+"</Preferred_Hour>\n";
		s+="	</Preferred_Time_Slot>\n";
	}
	s+="</ConstraintActivitiesPreferredTimeSlots>\n";
	return s;
}

QString ConstraintActivitiesPreferredTimeSlots::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	//Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s;
	s+=QObject::tr("Activities with ");
	if(this->p_teacherName!="")
		s+=QObject::tr("teacher=%1, ").arg(this->p_teacherName);
	else
		s+=QObject::tr("all teachers, ");
	if(this->p_studentsName!="")
		s+=QObject::tr("students=%1, ").arg(this->p_studentsName);
	else
		s+=QObject::tr("all students, ");
	if(this->p_subjectName!="")
		s+=QObject::tr("subject=%1, ").arg(this->p_subjectName);
	else
		s+=QObject::tr("all subjects, ");
	if(this->p_activityTagName!="")
		s+=QObject::tr("activity tag=%1, ").arg(this->p_activityTagName);
	else
		s+=QObject::tr("all activity tags, ");
	s+=QObject::tr("must be scheduled in the allowed slots: ");
	for(int i=0; i<this->p_nPreferredTimeSlots; i++){
		//s+=QString::number(i+1);
		//s+=":";
		if(this->p_days[i]>=0){
			s+=r.daysOfTheWeek[this->p_days[i]];
			s+=" ";
		}
		if(this->p_hours[i]>=0){
			s+=r.hoursOfTheDay[this->p_hours[i]];
		}
		s+="; ";
	}

	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintActivitiesPreferredTimeSlots::getDetailedDescription(Rules& r)
{
	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Activities with:");s+="\n";

	if(this->p_teacherName!="")
		s+=QObject::tr("Teacher=%1\n").arg(this->p_teacherName);
	else
		s+=QObject::tr("All teachers\n");
	if(this->p_studentsName!="")
		s+=QObject::tr("Students=%1\n").arg(this->p_studentsName);
	else
		s+=QObject::tr("All students\n");
	if(this->p_subjectName!="")
		s+=QObject::tr("Subject=%1\n").arg(this->p_subjectName);
	else
		s+=QObject::tr("All subjects\n");
	if(this->p_activityTagName!="")
		s+=QObject::tr("Activity tag=%1\n").arg(this->p_activityTagName);
	else
		s+=QObject::tr("All activity tags\n");

	s+=QObject::tr("must be scheduled in the allowed slots (all hours of each activity are in the allowed slots):\n");
	for(int i=0; i<this->p_nPreferredTimeSlots; i++){
		//s+=QString::number(i+1);
		//s+=". ";
		if(this->p_days[i]>=0){
			s+=r.daysOfTheWeek[this->p_days[i]];
			s+=" ";
		}
		if(this->p_hours[i]>=0){
			s+=r.hoursOfTheDay[this->p_hours[i]];
		}
		s+=";  ";
	}
	s+="\n";

	s+=(QObject::tr("Weight (percentage)=%1").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

double ConstraintActivitiesPreferredTimeSlots::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	assert(r.internalStructureComputed);

///////////////////
	bool allowed[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
	for(int d=0; d<r.nDaysPerWeek; d++)
		for(int h=0; h<r.nHoursPerDay; h++)
			allowed[d][h]=false;
	for(int i=0; i<this->p_nPreferredTimeSlots; i++){
		if(this->p_days[i]>=0 && this->p_hours[i]>=0)
			allowed[this->p_days[i]][this->p_hours[i]]=true;
		else
			assert(0);
	}
////////////////////

	nbroken=0;
	int tmp;
	
	for(int i=0; i<this->p_nActivities; i++){
		tmp=0;
		int ai=this->p_activitiesIndices[i];
		if(c.times[ai]!=UNALLOCATED_TIME){
			int d=c.times[ai]%r.nDaysPerWeek; //the day when this activity was scheduled
			int h=c.times[ai]/r.nDaysPerWeek; //the hour
			
			for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++)
				if(!allowed[d][h+dur])
					tmp++;
		}
		nbroken+=tmp;
		if(conflictsString!=NULL && tmp>0){
			QString s=QObject::tr("Time constraint activities preferred time slots broken"
			 " for activity with id=%1 on %2 hours,"
			 " increases conflicts total by %3")
			 .arg(r.internalActivitiesList[ai].id)
			 .arg(tmp)
			 .arg(weightPercentage/100*tmp);

	 
			s+=" ";
			QString tn;
			foreach(QString t, r.internalActivitiesList[ai].teachersNames){
				tn+=t;
				tn+=" ";
			}
			QString sn;
			foreach(QString t, r.internalActivitiesList[ai].studentsNames){
				sn+=t;
				sn+=" ";
			}
			s+="(";
			s+=QObject::tr("teachers %1, students sets %2, subject %3")
			 .arg(tn)
			 .arg(sn)
			 .arg(r.internalActivitiesList[ai].subjectName);
			if(r.internalActivitiesList[ai].activityTagName!="")
				s+=QObject::tr(", activity tag %4").arg(r.internalActivitiesList[ai].activityTagName);

			s+=")";

				 
			dl.append(s);
			cl.append(weightPercentage/100*tmp);
		
			*conflictsString+= s+"\n";
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return nbroken * weightPercentage;
}

bool ConstraintActivitiesPreferredTimeSlots::isRelatedToActivity(Rules& r, Activity* a)
{
	QStringList::iterator it;

	//check if this activity has the corresponding teacher
	if(this->p_teacherName!=""){
		it = a->teachersNames.find(this->p_teacherName);
		if(it==a->teachersNames.end())
			return false;
	}
	//check if this activity has the corresponding students
	if(this->p_studentsName!=""){
		bool commonStudents=false;
		foreach(QString st, a->studentsNames){
			if(r.studentsSetsRelated(st, this->p_studentsName)){
				commonStudents=true;
				break;
			}
		}
		if(!commonStudents)
			return false;

		//it = a->studentsNames.find(this->p_studentsName);
		//if(it==a->studentsNames.end())
		//	return false;
	}
	//check if this activity has the corresponding subject
	if(this->p_subjectName!="" && a->subjectName!=this->p_subjectName)
		return false;
	//check if this activity has the corresponding activity tag
	if(this->p_activityTagName!="" && a->activityTagName!=this->p_activityTagName)
		return false;

	return true;
}

bool ConstraintActivitiesPreferredTimeSlots::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintActivitiesPreferredTimeSlots::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintActivitiesPreferredTimeSlots::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintActivitiesPreferredTimeSlots::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/
		
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintSubactivitiesPreferredTimeSlots::ConstraintSubactivitiesPreferredTimeSlots()
	: TimeConstraint()
{
	this->type = CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS;
}

ConstraintSubactivitiesPreferredTimeSlots::ConstraintSubactivitiesPreferredTimeSlots(double wp, int compNo, QString te,
	QString st, QString su, QString sut, int nPT, int d[], int h[])
	: TimeConstraint(wp)
{
	this->componentNumber=compNo;
	this->p_teacherName=te;
	this->p_subjectName=su;
	this->p_activityTagName=sut;
	this->p_studentsName=st;
	this->p_nPreferredTimeSlots=nPT;
	assert(nPT<=MAX_N_CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS);
	for(int i=0; i<nPT; i++){
		this->p_days[i]=d[i];
		this->p_hours[i]=h[i];
	}
	this->type=CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS;
}

bool ConstraintSubactivitiesPreferredTimeSlots::computeInternalStructure(Rules& r)
{
	//assert(this->teacherName!="" || this->studentsName!="" || this->subjectName!="" || this->subjectTagName!="");

	this->p_nActivities=0;

	QStringList::iterator it;
	Activity* act;
	int i;
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		
		if(!act->representsComponentNumber(this->componentNumber))
			continue;

		//check if this activity has the corresponding teacher
		if(this->p_teacherName!=""){
			it = act->teachersNames.find(this->p_teacherName);
			if(it==act->teachersNames.end())
				continue;
		}
		//check if this activity has the corresponding students
		if(this->p_studentsName!=""){
			bool commonStudents=false;
			foreach(QString st, act->studentsNames)
				if(r.studentsSetsRelated(st, p_studentsName)){
					commonStudents=true;
					break;
				}
		
			/*it = act->studentsNames.find(this->studentsName);
			if(it==act->studentsNames.end())
				continue;*/
			if(!commonStudents)
				continue;
		}
		//check if this activity has the corresponding subject
		if(this->p_subjectName!="" && act->subjectName!=this->p_subjectName){
				continue;
		}
		//check if this activity has the corresponding activity tag
		if(this->p_activityTagName!="" && act->activityTagName!=this->p_activityTagName){
				continue;
		}
	
		assert(this->p_nActivities < MAX_ACTIVITIES);	
		this->p_activitiesIndices[this->p_nActivities++]=i;
		
		//cout<<endl;
		//cout<<"Activity with id == "<<act->id<<" corresponds to constraint subactivities preferred time slots:"<<endl;
		//cout<<qPrintable(this->getDescription(r))<<endl;
	}

	//////////////////////	
	for(int k=0; k<p_nPreferredTimeSlots; k++){
		if(this->p_days[k] >= r.nDaysPerWeek){
			QMessageBox::information(NULL, QObject::tr("FET information"),
			 QObject::tr("Constraint subactivities preferred time slots is wrong because it refers to removed day. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
		if(this->p_hours[k] == r.nHoursPerDay){
			QMessageBox::information(NULL, QObject::tr("FET information"),
			 QObject::tr("Constraint subactivities preferred time slots is wrong because a preferred hour is too late (after the last acceptable slot). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
		if(this->p_hours[k] > r.nHoursPerDay){
			QMessageBox::information(NULL, QObject::tr("FET information"),
			 QObject::tr("Constraint subactivities preferred time slots is wrong because it refers to removed hour. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
		if(this->p_hours[k]<0 || this->p_days[k]<0){
			QMessageBox::information(NULL, QObject::tr("FET information"),
			 QObject::tr("Constraint subactivities preferred time slots is wrong because hour or day is not specified for a slot (-1). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
	}
	///////////////////////
	
	if(this->p_nActivities>0)
		return true;
	else{
		QMessageBox::warning(NULL, QObject::tr("FET error in data"), 
			QObject ::tr("Following constraint is wrong (refers to no activities. Please correct it):\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}
}

bool ConstraintSubactivitiesPreferredTimeSlots::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintSubactivitiesPreferredTimeSlots::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	//Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s="<ConstraintSubactivitiesPreferredTimeSlots>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	//if(this->teacherName!="")
	s+="	<Component_Number>"+QString::number(this->componentNumber)+"</Component_Number>\n";
		s+="	<Teacher_Name>"+protect(this->p_teacherName)+"</Teacher_Name>\n";
	//if(this->studentsName!="")
		s+="	<Students_Name>"+protect(this->p_studentsName)+"</Students_Name>\n";
	//if(this->subjectName!="")
		s+="	<Subject_Name>"+protect(this->p_subjectName)+"</Subject_Name>\n";
	//if(this->subjectTagName!="")
		s+="	<Activity_Tag_Name>"+protect(this->p_activityTagName)+"</Activity_Tag_Name>\n";
	s+="	<Number_of_Preferred_Time_Slots>"+QString::number(this->p_nPreferredTimeSlots)+"</Number_of_Preferred_Time_Slots>\n";
	for(int i=0; i<p_nPreferredTimeSlots; i++){
		s+="	<Preferred_Time_Slot>\n";
		if(this->p_days[i]>=0)
			s+="		<Preferred_Day>"+protect(r.daysOfTheWeek[this->p_days[i]])+"</Preferred_Day>\n";
		if(this->p_hours[i]>=0)
			s+="		<Preferred_Hour>"+protect(r.hoursOfTheDay[this->p_hours[i]])+"</Preferred_Hour>\n";
		s+="	</Preferred_Time_Slot>\n";
	}
	s+="</ConstraintSubactivitiesPreferredTimeSlots>\n";
	return s;
}

QString ConstraintSubactivitiesPreferredTimeSlots::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	//Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s;
	s+=QObject::tr("Subactivities with ");
	s+=QObject::tr("component number=%1").arg(this->componentNumber);
	s+=", ";
	if(this->p_teacherName!="")
		s+=QObject::tr("teacher=%1, ").arg(this->p_teacherName);
	else
		s+=QObject::tr("all teachers, ");
	if(this->p_studentsName!="")
		s+=QObject::tr("students=%1, ").arg(this->p_studentsName);
	else
		s+=QObject::tr("all students, ");
	if(this->p_subjectName!="")
		s+=QObject::tr("subject=%1, ").arg(this->p_subjectName);
	else
		s+=QObject::tr("all subjects, ");
	if(this->p_activityTagName!="")
		s+=QObject::tr("activity tag=%1, ").arg(this->p_activityTagName);
	else
		s+=QObject::tr("all activity tags, ");
	s+=QObject::tr("must be scheduled in the allowed slots: ");
	for(int i=0; i<this->p_nPreferredTimeSlots; i++){
		//s+=QString::number(i+1);
		//s+=":";
		if(this->p_days[i]>=0){
			s+=r.daysOfTheWeek[this->p_days[i]];
			s+=" ";
		}
		if(this->p_hours[i]>=0){
			s+=r.hoursOfTheDay[this->p_hours[i]];
		}
		s+="; ";
	}

	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintSubactivitiesPreferredTimeSlots::getDetailedDescription(Rules& r)
{
	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Subactivities with:");s+="\n";
	
	s+=QObject::tr("Component number=%1").arg(this->componentNumber);
	s+="\n";

	if(this->p_teacherName!="")
		s+=QObject::tr("Teacher=%1\n").arg(this->p_teacherName);
	else
		s+=QObject::tr("All teachers\n");
	if(this->p_studentsName!="")
		s+=QObject::tr("Students=%1\n").arg(this->p_studentsName);
	else
		s+=QObject::tr("All students\n");
	if(this->p_subjectName!="")
		s+=QObject::tr("Subject=%1\n").arg(this->p_subjectName);
	else
		s+=QObject::tr("All subjects\n");
	if(this->p_activityTagName!="")
		s+=QObject::tr("Activity tag=%1\n").arg(this->p_activityTagName);
	else
		s+=QObject::tr("All activity tags\n");

	s+=QObject::tr("must be scheduled in the allowed slots (all hours of each activity are in the allowed slots):\n");
	for(int i=0; i<this->p_nPreferredTimeSlots; i++){
		//s+=QString::number(i+1);
		//s+=". ";
		if(this->p_days[i]>=0){
			s+=r.daysOfTheWeek[this->p_days[i]];
			s+=" ";
		}
		if(this->p_hours[i]>=0){
			s+=r.hoursOfTheDay[this->p_hours[i]];
		}
		s+=";  ";
	}
	s+="\n";

	s+=(QObject::tr("Weight (percentage)=%1").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

double ConstraintSubactivitiesPreferredTimeSlots::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	assert(r.internalStructureComputed);

///////////////////
	bool allowed[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
	for(int d=0; d<r.nDaysPerWeek; d++)
		for(int h=0; h<r.nHoursPerDay; h++)
			allowed[d][h]=false;
	for(int i=0; i<this->p_nPreferredTimeSlots; i++){
		if(this->p_days[i]>=0 && this->p_hours[i]>=0)
			allowed[this->p_days[i]][this->p_hours[i]]=true;
		else
			assert(0);
	}
////////////////////

	nbroken=0;
	int tmp;
	
	for(int i=0; i<this->p_nActivities; i++){
		tmp=0;
		int ai=this->p_activitiesIndices[i];
		if(c.times[ai]!=UNALLOCATED_TIME){
			int d=c.times[ai]%r.nDaysPerWeek; //the day when this activity was scheduled
			int h=c.times[ai]/r.nDaysPerWeek; //the hour
			
			for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++)
				if(!allowed[d][h+dur])
					tmp++;
		}
		nbroken+=tmp;
		if(conflictsString!=NULL && tmp>0){
			QString s=QObject::tr("Time constraint subactivities preferred time slots broken"
			 " for activity with id=%1 on %2 hours,"
			 " increases conflicts total by %3")
			 .arg(r.internalActivitiesList[ai].id)
			 .arg(tmp)
			 .arg(weightPercentage/100*tmp);

	 
			s+=" ";
			QString tn;
			foreach(QString t, r.internalActivitiesList[ai].teachersNames){
				tn+=t;
				tn+=" ";
			}
			QString sn;
			foreach(QString t, r.internalActivitiesList[ai].studentsNames){
				sn+=t;
				sn+=" ";
			}
			s+="(";
			
			s+=QObject::tr("component number %1").arg(componentNumber);
			s+=", ";
			
			s+=QObject::tr("teachers %1, students sets %2, subject %3")
			 .arg(tn)
			 .arg(sn)
			 .arg(r.internalActivitiesList[ai].subjectName);
			if(r.internalActivitiesList[ai].activityTagName!="")
				s+=QObject::tr(", activity tag %4").arg(r.internalActivitiesList[ai].activityTagName);

			s+=")";

				 
			dl.append(s);
			cl.append(weightPercentage/100*tmp);
		
			*conflictsString+= s+"\n";
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return nbroken * weightPercentage;
}

bool ConstraintSubactivitiesPreferredTimeSlots::isRelatedToActivity(Rules& r, Activity* a)
{
	if(!a->representsComponentNumber(this->componentNumber))
		return false;

	QStringList::iterator it;

	//check if this activity has the corresponding teacher
	if(this->p_teacherName!=""){
		it = a->teachersNames.find(this->p_teacherName);
		if(it==a->teachersNames.end())
			return false;
	}
	//check if this activity has the corresponding students
	if(this->p_studentsName!=""){
		bool commonStudents=false;
		foreach(QString st, a->studentsNames){
			if(r.studentsSetsRelated(st, this->p_studentsName)){
				commonStudents=true;
				break;
			}
		}
		if(!commonStudents)
			return false;

		//it = a->studentsNames.find(this->p_studentsName);
		//if(it==a->studentsNames.end())
		//	return false;
	}
	//check if this activity has the corresponding subject
	if(this->p_subjectName!="" && a->subjectName!=this->p_subjectName)
		return false;
	//check if this activity has the corresponding activity tag
	if(this->p_activityTagName!="" && a->activityTagName!=this->p_activityTagName)
		return false;

	return true;
}

bool ConstraintSubactivitiesPreferredTimeSlots::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintSubactivitiesPreferredTimeSlots::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintSubactivitiesPreferredTimeSlots::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintSubactivitiesPreferredTimeSlots::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/
		
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivityPreferredStartingTimes::ConstraintActivityPreferredStartingTimes()
	: TimeConstraint()
{
	this->type = CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIMES;
}

ConstraintActivityPreferredStartingTimes::ConstraintActivityPreferredStartingTimes(double wp, int actId, int nPT, int d[], int h[])
	: TimeConstraint(wp)
{
	this->activityId=actId;
	this->nPreferredStartingTimes=nPT;
	assert(nPT<=MAX_N_CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIMES);
	for(int i=0; i<nPT; i++){
		this->days[i]=d[i];
		this->hours[i]=h[i];
	}
	this->type=CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIMES;
}

bool ConstraintActivityPreferredStartingTimes::computeInternalStructure(Rules& r)
{
	Activity* act;
	int i;
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		if(act->id==this->activityId)
			break;
	}

	if(i==r.nInternalActivities){
		QMessageBox::warning(NULL, QObject::tr("FET error in data"), 
			QObject ::tr("Following constraint is wrong (because it refers to invalid activity id. Please correct it (maybe removing it is a solution)):\n%1").arg(this->getDetailedDescription(r)));
		//assert(0);
		return false;
	}

	for(int k=0; k<nPreferredStartingTimes; k++){
		if(this->days[k] >= r.nDaysPerWeek){
			QMessageBox::information(NULL, QObject::tr("FET information"),
			 QObject::tr("Constraint activity preferred starting times is wrong because it refers to removed day. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}		
		if(this->hours[k] == r.nHoursPerDay){
			QMessageBox::information(NULL, QObject::tr("FET information"),
			 QObject::tr("Constraint activity preferred starting times is wrong because a preferred hour is too late (after the last acceptable slot). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
		if(this->hours[k] > r.nHoursPerDay){
			QMessageBox::information(NULL, QObject::tr("FET information"),
			 QObject::tr("Constraint activity preferred starting times is wrong because it refers to removed hour. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
	}

	this->activityIndex=i;	
	return true;
}

bool ConstraintActivityPreferredStartingTimes::hasInactiveActivities(Rules& r)
{
	if(r.inactiveActivities.contains(this->activityId))
		return true;
	return false;
}

QString ConstraintActivityPreferredStartingTimes::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	//Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s="<ConstraintActivityPreferredStartingTimes>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Activity_Id>"+QString::number(this->activityId)+"</Activity_Id>\n";
	s+="	<Number_of_Preferred_Starting_Times>"+QString::number(this->nPreferredStartingTimes)+"</Number_of_Preferred_Starting_Times>\n";
	for(int i=0; i<nPreferredStartingTimes; i++){
		s+="	<Preferred_Starting_Time>\n";
		if(this->days[i]>=0)
			s+="		<Preferred_Starting_Day>"+protect(r.daysOfTheWeek[this->days[i]])+"</Preferred_Starting_Day>\n";
		if(this->hours[i]>=0)
			s+="		<Preferred_Starting_Hour>"+protect(r.hoursOfTheDay[this->hours[i]])+"</Preferred_Starting_Hour>\n";
		s+="	</Preferred_Starting_Time>\n";
	}
	s+="</ConstraintActivityPreferredStartingTimes>\n";
	return s;
}

QString ConstraintActivityPreferredStartingTimes::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	//Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s;
	s+=(QObject::tr("Act. id:%1").arg(this->activityId));
	
	//* write the teachers, subject and students sets
	//added in version 4.1.4
	int ai;
	for(ai=0; ai<r.activitiesList.size(); ai++)
		if(r.activitiesList[ai]->id==this->activityId)
			break;
	if(ai==r.activitiesList.size()){
		s+=QObject::tr(" Invalid (inexistent) activity id for constraint activity preferred starting times");
		return s;
	}
	assert(ai<r.activitiesList.size());
	s+=" (";
	
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
	
	s+=")";
	//* end section
	
	s+=", ";
	s+=QObject::tr("must be scheduled starting at:");
	s+=" ";
	for(int i=0; i<this->nPreferredStartingTimes; i++){
		//s+=QString::number(i+1);
		//s+=":";
		if(this->days[i]>=0){
			s+=r.daysOfTheWeek[this->days[i]];
			s+=" ";
		}
		if(this->hours[i]>=0){
			s+=r.hoursOfTheDay[this->hours[i]];
		}
		s+="; ";
	}

	s+=(QObject::tr("WP:%1", "Weight Percentage").arg(this->weightPercentage));//s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintActivityPreferredStartingTimes::getDetailedDescription(Rules& r)
{
	QString s=QObject::tr("Time constraint");s+="\n";
	s+=(QObject::tr("Activity with id=%1").arg(this->activityId));
	
	//* write the teachers, subject and students sets
	//added in version 4.1.4
	int ai;
	for(ai=0; ai<r.activitiesList.size(); ai++)
		if(r.activitiesList[ai]->id==this->activityId)
			break;
	if(ai==r.activitiesList.size()){
		s+=QObject::tr(" Invalid (inexistent) activity id for constraint activity preferred starting times");
		s+="\n";
		return s;
	}
	assert(ai<r.activitiesList.size());
	s+=" (";
	
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
	
	s+=")";
	//* end section
	
	s+="\n";
	s+=QObject::tr("must be scheduled starting at:\n");
	for(int i=0; i<this->nPreferredStartingTimes; i++){
		//s+=QString::number(i+1);
		//s+=". ";
		if(this->days[i]>=0){
			s+=r.daysOfTheWeek[this->days[i]];
			s+=" ";
		}
		if(this->hours[i]>=0){
			s+=r.hoursOfTheDay[this->hours[i]];
		}
		//s+="\n";
		s+=";  ";
	}
	s+="\n";

	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

double ConstraintActivityPreferredStartingTimes::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	assert(r.internalStructureComputed);

	nbroken=0;
	if(c.times[this->activityIndex]!=UNALLOCATED_TIME){
		int d=c.times[this->activityIndex]%r.nDaysPerWeek; //the day when this activity was scheduled
		int h=c.times[this->activityIndex]/r.nDaysPerWeek; //the hour
		int i;
		for(i=0; i<this->nPreferredStartingTimes; i++){
			if(this->days[i]>=0 && this->days[i]!=d)
				continue;
			if(this->hours[i]>=0 && this->hours[i]!=h)
				continue;
			break;
		}
		if(i==this->nPreferredStartingTimes){
			nbroken=1;
			/*if(r.internalActivitiesList[this->activityIndex].parity==PARITY_WEEKLY) //for weekly activities, double the conflicts
				nbroken*=2;*/
		}
	}

	if(conflictsString!=NULL && nbroken>0){
		QString s=QObject::tr("Time constraint activity preferred starting times broken for activity with id=%1, increases conflicts total by %2")
		 .arg(this->activityId)
		 .arg(weightPercentage/100*nbroken);


		 
		s+=" ";
		QString tn;
		foreach(QString t, r.internalActivitiesList[this->activityIndex].teachersNames){
			tn+=t;
			tn+=" ";
		}
		QString sn;
		foreach(QString t, r.internalActivitiesList[this->activityIndex].studentsNames){
			sn+=t;
			sn+=" ";
		}
		s+="(";
		s+=QObject::tr("teachers %1, students sets %2, subject %3")
		 .arg(tn)
		 .arg(sn)
		 .arg(r.internalActivitiesList[this->activityIndex].subjectName);
		if(r.internalActivitiesList[this->activityIndex].activityTagName!="")
			s+=QObject::tr(", activity tag %4").arg(r.internalActivitiesList[this->activityIndex].activityTagName);

		s+=")";



		dl.append(s);
		cl.append(weightPercentage/100*nbroken);
	
		*conflictsString+= s+"\n";
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return nbroken * weightPercentage/100;
}

bool ConstraintActivityPreferredStartingTimes::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);

	if(this->activityId==a->id)
		return true;
	return false;
}

bool ConstraintActivityPreferredStartingTimes::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintActivityPreferredStartingTimes::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintActivityPreferredStartingTimes::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintActivityPreferredStartingTimes::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/
		
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivitiesPreferredStartingTimes::ConstraintActivitiesPreferredStartingTimes()
	: TimeConstraint()
{
	this->type = CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES;
}

ConstraintActivitiesPreferredStartingTimes::ConstraintActivitiesPreferredStartingTimes(double wp, QString te,
	QString st, QString su, QString sut, int nPT, int d[], int h[])
	: TimeConstraint(wp)
{
	this->teacherName=te;
	this->subjectName=su;
	this->activityTagName=sut;
	this->studentsName=st;
	this->nPreferredStartingTimes=nPT;
	assert(nPT<=MAX_N_CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES);
	for(int i=0; i<nPT; i++){
		this->days[i]=d[i];
		this->hours[i]=h[i];
	}
	this->type=CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES;
}

bool ConstraintActivitiesPreferredStartingTimes::computeInternalStructure(Rules& r)
{
	//assert(this->teacherName!="" || this->studentsName!="" || this->subjectName!="" || this->subjectTagName!="");

	this->nActivities=0;

	QStringList::iterator it;
	Activity* act;
	int i;
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];

		//check if this activity has the corresponding teacher
		if(this->teacherName!=""){
			it = act->teachersNames.find(this->teacherName);
			if(it==act->teachersNames.end())
				continue;
		}
		//check if this activity has the corresponding students
		if(this->studentsName!=""){
			bool commonStudents=false;
			foreach(QString st, act->studentsNames)
				if(r.studentsSetsRelated(st, studentsName)){
					commonStudents=true;
					break;
				}
		
			/*it = act->studentsNames.find(this->studentsName);
			if(it==act->studentsNames.end())
				continue;*/
			if(!commonStudents)
				continue;
		}
		//check if this activity has the corresponding subject
		if(this->subjectName!="" && act->subjectName!=this->subjectName){
				continue;
		}
		//check if this activity has the corresponding activity tag
		if(this->activityTagName!="" && act->activityTagName!=this->activityTagName){
				continue;
		}
	
		assert(this->nActivities < MAX_ACTIVITIES);	
		this->activitiesIndices[this->nActivities++]=i;
	}

	//////////////////////	
	for(int k=0; k<nPreferredStartingTimes; k++){
		if(this->days[k] >= r.nDaysPerWeek){
			QMessageBox::information(NULL, QObject::tr("FET information"),
			 QObject::tr("Constraint activities preferred starting times is wrong because it refers to removed day. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
		if(this->hours[k] == r.nHoursPerDay){
			QMessageBox::information(NULL, QObject::tr("FET information"),
			 QObject::tr("Constraint activities preferred starting times is wrong because a preferred hour is too late (after the last acceptable slot). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
		if(this->hours[k] > r.nHoursPerDay){
			QMessageBox::information(NULL, QObject::tr("FET information"),
			 QObject::tr("Constraint activities preferred starting times is wrong because it refers to removed hour. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
	}
	///////////////////////
	
	if(this->nActivities>0)
		return true;
	else{
		QMessageBox::warning(NULL, QObject::tr("FET error in data"), 
			QObject ::tr("Following constraint is wrong (refers to no activities. Please correct it):\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}
}

bool ConstraintActivitiesPreferredStartingTimes::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintActivitiesPreferredStartingTimes::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	//Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s="<ConstraintActivitiesPreferredStartingTimes>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	//if(this->teacherName!="")
		s+="	<Teacher_Name>"+protect(this->teacherName)+"</Teacher_Name>\n";
	//if(this->studentsName!="")
		s+="	<Students_Name>"+protect(this->studentsName)+"</Students_Name>\n";
	//if(this->subjectName!="")
		s+="	<Subject_Name>"+protect(this->subjectName)+"</Subject_Name>\n";
	//if(this->subjectTagName!="")
		s+="	<Activity_Tag_Name>"+protect(this->activityTagName)+"</Activity_Tag_Name>\n";
	s+="	<Number_of_Preferred_Starting_Times>"+QString::number(this->nPreferredStartingTimes)+"</Number_of_Preferred_Starting_Times>\n";
	for(int i=0; i<nPreferredStartingTimes; i++){
		s+="	<Preferred_Starting_Time>\n";
		if(this->days[i]>=0)
			s+="		<Preferred_Starting_Day>"+protect(r.daysOfTheWeek[this->days[i]])+"</Preferred_Starting_Day>\n";
		if(this->hours[i]>=0)
			s+="		<Preferred_Starting_Hour>"+protect(r.hoursOfTheDay[this->hours[i]])+"</Preferred_Starting_Hour>\n";
		s+="	</Preferred_Starting_Time>\n";
	}
	s+="</ConstraintActivitiesPreferredStartingTimes>\n";
	return s;
}

QString ConstraintActivitiesPreferredStartingTimes::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	//Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s;
	s+=QObject::tr("Activities with ");
	if(this->teacherName!="")
		s+=QObject::tr("teacher=%1, ").arg(this->teacherName);
	else
		s+=QObject::tr("all teachers, ");
	if(this->studentsName!="")
		s+=QObject::tr("students=%1, ").arg(this->studentsName);
	else
		s+=QObject::tr("all students, ");
	if(this->subjectName!="")
		s+=QObject::tr("subject=%1, ").arg(this->subjectName);
	else
		s+=QObject::tr("all subjects, ");
	if(this->activityTagName!="")
		s+=QObject::tr("activity tag=%1, ").arg(this->activityTagName);
	else
		s+=QObject::tr("all activity tags, ");
	s+=QObject::tr("must be scheduled starting at: ");
	for(int i=0; i<this->nPreferredStartingTimes; i++){
		//s+=QString::number(i+1);
		//s+=":";
		if(this->days[i]>=0){
			s+=r.daysOfTheWeek[this->days[i]];
			s+=" ";
		}
		if(this->hours[i]>=0){
			s+=r.hoursOfTheDay[this->hours[i]];
		}
		s+="; ";
	}

	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintActivitiesPreferredStartingTimes::getDetailedDescription(Rules& r)
{
	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Activities with:");s+="\n";

	if(this->teacherName!="")
		s+=QObject::tr("Teacher=%1\n").arg(this->teacherName);
	else
		s+=QObject::tr("All teachers\n");
	if(this->studentsName!="")
		s+=QObject::tr("Students=%1\n").arg(this->studentsName);
	else
		s+=QObject::tr("All students\n");
	if(this->subjectName!="")
		s+=QObject::tr("Subject=%1\n").arg(this->subjectName);
	else
		s+=QObject::tr("All subjects\n");
	if(this->activityTagName!="")
		s+=QObject::tr("Activity tag=%1\n").arg(this->activityTagName);
	else
		s+=QObject::tr("All activity tags\n");

	s+=QObject::tr("must be scheduled starting at:\n");
	for(int i=0; i<this->nPreferredStartingTimes; i++){
		//s+=QString::number(i+1);
		//s+=". ";
		if(this->days[i]>=0){
			s+=r.daysOfTheWeek[this->days[i]];
			s+=" ";
		}
		if(this->hours[i]>=0){
			s+=r.hoursOfTheDay[this->hours[i]];
		}
		s+=";  ";
	}
	s+="\n";

	s+=(QObject::tr("Weight (percentage)=%1").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

double ConstraintActivitiesPreferredStartingTimes::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	assert(r.internalStructureComputed);

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		int tmp;
	
		for(int i=0; i<this->nActivities; i++){
			tmp=0;
			int ai=this->activitiesIndices[i];
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d=c.times[ai]%r.nDaysPerWeek; //the day when this activity was scheduled
				int h=c.times[ai]/r.nDaysPerWeek; //the hour
				int i;
				for(i=0; i<this->nPreferredStartingTimes; i++){
					if(this->days[i]>=0 && this->days[i]!=d)
						continue;
					if(this->hours[i]>=0 && this->hours[i]!=h)
						continue;
					break;
				}
				if(i==this->nPreferredStartingTimes){
					tmp=1;
					//if(r.internalActivitiesList[ai].parity==PARITY_WEEKLY) //for weekly activities, double the conflicts
					//	tmp=2;
				}
			}
			nbroken+=tmp;
		}
	}
	else{
		nbroken=0;
		int tmp;
	
		for(int i=0; i<this->nActivities; i++){
			tmp=0;
			int ai=this->activitiesIndices[i];
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d=c.times[ai]%r.nDaysPerWeek; //the day when this activity was scheduled
				int h=c.times[ai]/r.nDaysPerWeek; //the hour
				int i;
				for(i=0; i<this->nPreferredStartingTimes; i++){
					if(this->days[i]>=0 && this->days[i]!=d)
						continue;
					if(this->hours[i]>=0 && this->hours[i]!=h)
						continue;
					break;
				}
				if(i==this->nPreferredStartingTimes){
					tmp=1;
					//if(r.internalActivitiesList[ai].parity==PARITY_WEEKLY) //for weekly activities, double the conflicts
					//	tmp=2;
				}
			}
			nbroken+=tmp;
			if(conflictsString!=NULL && tmp>0){
				QString s=QObject::tr("Time constraint activities preferred starting times broken"
				 " for activity with id=%1, "
				 " increases conflicts total by %2")
				 .arg(r.internalActivitiesList[ai].id)
				 .arg(weightPercentage/100*tmp);


		 
				s+=" ";
				QString tn;
				foreach(QString t, r.internalActivitiesList[ai].teachersNames){
					tn+=t;
					tn+=" ";
				}
				QString sn;
				foreach(QString t, r.internalActivitiesList[ai].studentsNames){
					sn+=t;
					sn+=" ";
				}
				s+="(";
				s+=QObject::tr("teachers %1, students sets %2, subject %3")
				 .arg(tn)
				 .arg(sn)
				 .arg(r.internalActivitiesList[ai].subjectName);
				if(r.internalActivitiesList[ai].activityTagName!="")
					s+=QObject::tr(", activity tag %4").arg(r.internalActivitiesList[ai].activityTagName);
	
				s+=")";



				 
				dl.append(s);
				cl.append(weightPercentage/100*tmp);
			
				*conflictsString+= s+"\n";
			}
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return nbroken * weightPercentage;
}

bool ConstraintActivitiesPreferredStartingTimes::isRelatedToActivity(Rules& r, Activity* a)
{
	QStringList::iterator it;

	//check if this activity has the corresponding teacher
	if(this->teacherName!=""){
		it = a->teachersNames.find(this->teacherName);
		if(it==a->teachersNames.end())
			return false;
	}
	//check if this activity has the corresponding students
	if(this->studentsName!=""){
		bool commonStudents=false;
		foreach(QString st, a->studentsNames){
			if(r.studentsSetsRelated(st, this->studentsName)){
				commonStudents=true;
				break;
			}
		}
		if(!commonStudents)
			return false;

		//it = a->studentsNames.find(this->studentsName);
		//if(it==a->studentsNames.end())
		//	return false;
	}
	//check if this activity has the corresponding subject
	if(this->subjectName!="" && a->subjectName!=this->subjectName)
		return false;
	//check if this activity has the corresponding activity tag
	if(this->activityTagName!="" && a->activityTagName!=this->activityTagName)
		return false;

	return true;
}

bool ConstraintActivitiesPreferredStartingTimes::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintActivitiesPreferredStartingTimes::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintActivitiesPreferredStartingTimes::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintActivitiesPreferredStartingTimes::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/
		
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintSubactivitiesPreferredStartingTimes::ConstraintSubactivitiesPreferredStartingTimes()
	: TimeConstraint()
{
	this->type = CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES;
}

ConstraintSubactivitiesPreferredStartingTimes::ConstraintSubactivitiesPreferredStartingTimes(double wp, int compNo, QString te,
	QString st, QString su, QString sut, int nPT, int d[], int h[])
	: TimeConstraint(wp)
{
	this->componentNumber=compNo;
	this->teacherName=te;
	this->subjectName=su;
	this->activityTagName=sut;
	this->studentsName=st;
	this->nPreferredStartingTimes=nPT;
	assert(nPT<=MAX_N_CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES);
	for(int i=0; i<nPT; i++){
		this->days[i]=d[i];
		this->hours[i]=h[i];
	}
	this->type=CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES;
}

bool ConstraintSubactivitiesPreferredStartingTimes::computeInternalStructure(Rules& r)
{
	//assert(this->teacherName!="" || this->studentsName!="" || this->subjectName!="" || this->subjectTagName!="");

	this->nActivities=0;

	QStringList::iterator it;
	Activity* act;
	int i;
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		
		if(!act->representsComponentNumber(this->componentNumber))
			continue;

		//check if this activity has the corresponding teacher
		if(this->teacherName!=""){
			it = act->teachersNames.find(this->teacherName);
			if(it==act->teachersNames.end())
				continue;
		}
		//check if this activity has the corresponding students
		if(this->studentsName!=""){
			bool commonStudents=false;
			foreach(QString st, act->studentsNames)
				if(r.studentsSetsRelated(st, studentsName)){
					commonStudents=true;
					break;
				}
		
			/*it = act->studentsNames.find(this->studentsName);
			if(it==act->studentsNames.end())
				continue;*/
			if(!commonStudents)
				continue;
		}
		//check if this activity has the corresponding subject
		if(this->subjectName!="" && act->subjectName!=this->subjectName){
				continue;
		}
		//check if this activity has the corresponding activity tag
		if(this->activityTagName!="" && act->activityTagName!=this->activityTagName){
				continue;
		}
	
		assert(this->nActivities < MAX_ACTIVITIES);	
		this->activitiesIndices[this->nActivities++]=i;
		
		//cout<<endl;
		//cout<<"activity with id == "<<act->id<<" corresponds to constraint subactivities preferred starting times"<<endl;
		//cout<<qPrintable(this->getDescription(r))<<endl;
	}

	//////////////////////	
	for(int k=0; k<nPreferredStartingTimes; k++){
		if(this->days[k] >= r.nDaysPerWeek){
			QMessageBox::information(NULL, QObject::tr("FET information"),
			 QObject::tr("Constraint subactivities preferred starting times is wrong because it refers to removed day. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
		if(this->hours[k] == r.nHoursPerDay){
			QMessageBox::information(NULL, QObject::tr("FET information"),
			 QObject::tr("Constraint subactivities preferred starting times is wrong because a preferred hour is too late (after the last acceptable slot). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
		if(this->hours[k] > r.nHoursPerDay){
			QMessageBox::information(NULL, QObject::tr("FET information"),
			 QObject::tr("Constraint subactivities preferred starting times is wrong because it refers to removed hour. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
	}
	///////////////////////
	
	if(this->nActivities>0)
		return true;
	else{
		QMessageBox::warning(NULL, QObject::tr("FET error in data"), 
			QObject ::tr("Following constraint is wrong (refers to no activities. Please correct it):\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}
}

bool ConstraintSubactivitiesPreferredStartingTimes::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintSubactivitiesPreferredStartingTimes::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	//Q_UNUSED(r);
	//if(&r==NULL)
	//	;
	QString s="<ConstraintSubactivitiesPreferredStartingTimes>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	//if(this->teacherName!="")
	s+="	<Component_Number>"+QString::number(this->componentNumber)+"</Component_Number>\n";
		s+="	<Teacher_Name>"+protect(this->teacherName)+"</Teacher_Name>\n";
	//if(this->studentsName!="")
		s+="	<Students_Name>"+protect(this->studentsName)+"</Students_Name>\n";
	//if(this->subjectName!="")
		s+="	<Subject_Name>"+protect(this->subjectName)+"</Subject_Name>\n";
	//if(this->subjectTagName!="")
		s+="	<Activity_Tag_Name>"+protect(this->activityTagName)+"</Activity_Tag_Name>\n";
	s+="	<Number_of_Preferred_Starting_Times>"+QString::number(this->nPreferredStartingTimes)+"</Number_of_Preferred_Starting_Times>\n";
	for(int i=0; i<nPreferredStartingTimes; i++){
		s+="	<Preferred_Starting_Time>\n";
		if(this->days[i]>=0)
			s+="		<Preferred_Starting_Day>"+protect(r.daysOfTheWeek[this->days[i]])+"</Preferred_Starting_Day>\n";
		if(this->hours[i]>=0)
			s+="		<Preferred_Starting_Hour>"+protect(r.hoursOfTheDay[this->hours[i]])+"</Preferred_Starting_Hour>\n";
		s+="	</Preferred_Starting_Time>\n";
	}
	s+="</ConstraintSubactivitiesPreferredStartingTimes>\n";
	return s;
}

QString ConstraintSubactivitiesPreferredStartingTimes::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	//Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s;
	s+=QObject::tr("Subactivities with ");
	
	s+=QObject::tr("component number=%1").arg(this->componentNumber);
	s+=", ";
	
	if(this->teacherName!="")
		s+=QObject::tr("teacher=%1, ").arg(this->teacherName);
	else
		s+=QObject::tr("all teachers, ");
	if(this->studentsName!="")
		s+=QObject::tr("students=%1, ").arg(this->studentsName);
	else
		s+=QObject::tr("all students, ");
	if(this->subjectName!="")
		s+=QObject::tr("subject=%1, ").arg(this->subjectName);
	else
		s+=QObject::tr("all subjects, ");
	if(this->activityTagName!="")
		s+=QObject::tr("activity tag=%1, ").arg(this->activityTagName);
	else
		s+=QObject::tr("all activity tags, ");
	s+=QObject::tr("must be scheduled starting at: ");
	for(int i=0; i<this->nPreferredStartingTimes; i++){
		//s+=QString::number(i+1);
		//s+=":";
		if(this->days[i]>=0){
			s+=r.daysOfTheWeek[this->days[i]];
			s+=" ";
		}
		if(this->hours[i]>=0){
			s+=r.hoursOfTheDay[this->hours[i]];
		}
		s+="; ";
	}

	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintSubactivitiesPreferredStartingTimes::getDetailedDescription(Rules& r)
{
	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Subactivities with:");s+="\n";

	s+=QObject::tr("Component number=%1").arg(this->componentNumber);s+="\n";

	if(this->teacherName!="")
		s+=QObject::tr("Teacher=%1\n").arg(this->teacherName);
	else
		s+=QObject::tr("All teachers\n");
	if(this->studentsName!="")
		s+=QObject::tr("Students=%1\n").arg(this->studentsName);
	else
		s+=QObject::tr("All students\n");
	if(this->subjectName!="")
		s+=QObject::tr("Subject=%1\n").arg(this->subjectName);
	else
		s+=QObject::tr("All subjects\n");
	if(this->activityTagName!="")
		s+=QObject::tr("Activity tag=%1\n").arg(this->activityTagName);
	else
		s+=QObject::tr("All activity tags\n");

	s+=QObject::tr("must be scheduled starting at:\n");
	for(int i=0; i<this->nPreferredStartingTimes; i++){
		//s+=QString::number(i+1);
		//s+=". ";
		if(this->days[i]>=0){
			s+=r.daysOfTheWeek[this->days[i]];
			s+=" ";
		}
		if(this->hours[i]>=0){
			s+=r.hoursOfTheDay[this->hours[i]];
		}
		s+=";  ";
	}
	s+="\n";

	s+=(QObject::tr("Weight (percentage)=%1").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

double ConstraintSubactivitiesPreferredStartingTimes::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	assert(r.internalStructureComputed);

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		int tmp;
	
		for(int i=0; i<this->nActivities; i++){
			tmp=0;
			int ai=this->activitiesIndices[i];
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d=c.times[ai]%r.nDaysPerWeek; //the day when this activity was scheduled
				int h=c.times[ai]/r.nDaysPerWeek; //the hour
				int i;
				for(i=0; i<this->nPreferredStartingTimes; i++){
					if(this->days[i]>=0 && this->days[i]!=d)
						continue;
					if(this->hours[i]>=0 && this->hours[i]!=h)
						continue;
					break;
				}
				if(i==this->nPreferredStartingTimes){
					tmp=1;
					//if(r.internalActivitiesList[ai].parity==PARITY_WEEKLY) //for weekly activities, double the conflicts
					//	tmp=2;
				}
			}
			nbroken+=tmp;
		}
	}
	else{
		nbroken=0;
		int tmp;
	
		for(int i=0; i<this->nActivities; i++){
			tmp=0;
			int ai=this->activitiesIndices[i];
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d=c.times[ai]%r.nDaysPerWeek; //the day when this activity was scheduled
				int h=c.times[ai]/r.nDaysPerWeek; //the hour
				int i;
				for(i=0; i<this->nPreferredStartingTimes; i++){
					if(this->days[i]>=0 && this->days[i]!=d)
						continue;
					if(this->hours[i]>=0 && this->hours[i]!=h)
						continue;
					break;
				}
				if(i==this->nPreferredStartingTimes){
					tmp=1;
					//if(r.internalActivitiesList[ai].parity==PARITY_WEEKLY) //for weekly activities, double the conflicts
					//	tmp=2;
				}
			}
			nbroken+=tmp;
			if(conflictsString!=NULL && tmp>0){
				QString s=QObject::tr("Time constraint subactivities preferred starting times broken"
				 " for activity with id=%1, "
				 " increases conflicts total by %2")
				 .arg(r.internalActivitiesList[ai].id)
				 .arg(weightPercentage/100*tmp);


		 
				s+=" ";
				QString tn;
				foreach(QString t, r.internalActivitiesList[ai].teachersNames){
					tn+=t;
					tn+=" ";
				}
				QString sn;
				foreach(QString t, r.internalActivitiesList[ai].studentsNames){
					sn+=t;
					sn+=" ";
				}
				s+="(";
				
				s+=QObject::tr("component number %1").arg(this->componentNumber);
				s+=", ";
				
				s+=QObject::tr("teachers %1, students sets %2, subject %3")
				 .arg(tn)
				 .arg(sn)
				 .arg(r.internalActivitiesList[ai].subjectName);
				if(r.internalActivitiesList[ai].activityTagName!="")
					s+=QObject::tr(", activity tag %4").arg(r.internalActivitiesList[ai].activityTagName);
	
				s+=")";



				 
				dl.append(s);
				cl.append(weightPercentage/100*tmp);
			
				*conflictsString+= s+"\n";
			}
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return nbroken * weightPercentage;
}

bool ConstraintSubactivitiesPreferredStartingTimes::isRelatedToActivity(Rules& r, Activity* a)
{
	if(!a->representsComponentNumber(this->componentNumber))
		return false;

	QStringList::iterator it;
	
	//check if this activity has the corresponding teacher
	if(this->teacherName!=""){
		it = a->teachersNames.find(this->teacherName);
		if(it==a->teachersNames.end())
			return false;
	}
	//check if this activity has the corresponding students
	if(this->studentsName!=""){
		bool commonStudents=false;
		foreach(QString st, a->studentsNames){
			if(r.studentsSetsRelated(st, this->studentsName)){
				commonStudents=true;
				break;
			}
		}
		if(!commonStudents)
			return false;

		//it = a->studentsNames.find(this->studentsName);
		//if(it==a->studentsNames.end())
		//	return false;
	}
	//check if this activity has the corresponding subject
	if(this->subjectName!="" && a->subjectName!=this->subjectName)
		return false;
	//check if this activity has the corresponding activity tag
	if(this->activityTagName!="" && a->activityTagName!=this->activityTagName)
		return false;

	return true;
}

bool ConstraintSubactivitiesPreferredStartingTimes::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintSubactivitiesPreferredStartingTimes::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintSubactivitiesPreferredStartingTimes::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintSubactivitiesPreferredStartingTimes::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/
		
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivitiesSameStartingHour::ConstraintActivitiesSameStartingHour()
	: TimeConstraint()
{
	type=CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR;
}

ConstraintActivitiesSameStartingHour::ConstraintActivitiesSameStartingHour(double wp, int nact, const int act[])
 : TimeConstraint(wp)
 {
	assert(nact>=2 && nact<=MAX_CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR);
	this->n_activities=nact;
	for(int i=0; i<nact; i++)
		this->activitiesId[i]=act[i];

	this->type=CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR;
}

bool ConstraintActivitiesSameStartingHour::computeInternalStructure(Rules &r)
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
	
	if(this->_n_activities<=1){
		QMessageBox::warning(NULL, QObject::tr("FET error in data"), 
			QObject ::tr("Following constraint is wrong (because you need 2 or more activities. Please correct it):\n%1").arg(this->getDetailedDescription(r)));
		//assert(0);
		return false;
	}

	return true;
}

void ConstraintActivitiesSameStartingHour::removeUseless(Rules& r)
{
	//remove the activitiesId which no longer exist (used after the deletion of an activity)

	for(int j=0; j<this->n_activities; j++)
		this->_activities[j]=-1;

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

bool ConstraintActivitiesSameStartingHour::hasInactiveActivities(Rules& r)
{
	int count=0;

	for(int i=0; i<this->n_activities; i++)
		if(r.inactiveActivities.contains(this->activitiesId[i]))
			count++;

	if(this->n_activities-count<=1)
		return true;
	else
		return false;
}

QString ConstraintActivitiesSameStartingHour::getXmlDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="<ConstraintActivitiesSameStartingHour>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Number_of_Activities>"+QString::number(this->n_activities)+"</Number_of_Activities>\n";
	for(int i=0; i<this->n_activities; i++)
		s+="	<Activity_Id>"+QString::number(this->activitiesId[i])+"</Activity_Id>\n";
	s+="</ConstraintActivitiesSameStartingHour>\n";
	return s;
}

QString ConstraintActivitiesSameStartingHour::getDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s;
	s+=QObject::tr("Activities same starting hour");s+=", ";
	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
	s+=(QObject::tr("NA:%1").arg(this->n_activities));s+=", ";
	for(int i=0; i<this->n_activities; i++){
		s+=(QObject::tr("ID:%1").arg(this->activitiesId[i]));s+=", ";
	}

	return s;
}

QString ConstraintActivitiesSameStartingHour::getDetailedDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s;
	
	s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Activities must have the same starting hour");s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	s+=(QObject::tr("Number of activities=%1").arg(this->n_activities));s+="\n";
	for(int i=0; i<this->n_activities; i++){
		s+=(QObject::tr("Activity with id=%1").arg(this->activitiesId[i]));
		
		//* write the teachers, subject and students sets
		//added in version 5.1.10
		int ai;
		for(ai=0; ai<r.activitiesList.size(); ai++)
			if(r.activitiesList[ai]->id==this->activitiesId[i])
				break;
		if(ai==r.activitiesList.size()){
			s+=QObject::tr(" Invalid (inexistent) activity id");
			s+="\n";
			return s;
		}
		assert(ai<r.activitiesList.size());
		s+=" ( ";
	
		s+=QObject::tr("T: ");
		int k=0;
		foreach(QString ss, r.activitiesList[ai]->teachersNames){
			if(k>0)
				s+=" ,";
			s+=ss;
			k++;
		}
	
		s+=QObject::tr(" , S: ");
		s+=r.activitiesList[ai]->subjectName;
	
		if(r.activitiesList[ai]->activityTagName!="")
			s+=QObject::tr(" , AT: ", "Activity tag")+r.activitiesList[ai]->activityTagName;
	
		s+=QObject::tr(" , St: ");
		k=0;
		foreach(QString ss, r.activitiesList[ai]->studentsNames){
			if(k>0)
				s+=",";
			s+=ss;
			k++;
		}
	
		s+=" )";
		//* end section

		s+="\n";
	}

	return s;
}

double ConstraintActivitiesSameStartingHour::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	assert(r.internalStructureComputed);

	int nbroken;

	//We do not use the matrices 'subgroupsMatrix' nor 'teachersMatrix'.

	//sum the differences in the scheduled hour for all pairs of activities.

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int i=1; i<this->_n_activities; i++){
			int t1=c.times[this->_activities[i]];
			if(t1!=UNALLOCATED_TIME){
				//int day1=t1%r.nDaysPerWeek;
				int hour1=t1/r.nDaysPerWeek;
				for(int j=0; j<i; j++){
					int t2=c.times[this->_activities[j]];
					if(t2!=UNALLOCATED_TIME){
						//int day2=t2%r.nDaysPerWeek;
						int hour2=t2/r.nDaysPerWeek;
						int tmp=0;

						//activity weekly - counts as double
						/*if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY &&
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 4 * abs(hour1-hour2);
						else if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY ||
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 2 * abs(hour1-hour2);
						else*/
						
						//	tmp = abs(hour1-hour2);
						if(hour1!=hour2)
							tmp=1;

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
			int t1=c.times[this->_activities[i]];
			if(t1!=UNALLOCATED_TIME){
				//int day1=t1%r.nDaysPerWeek;
				int hour1=t1/r.nDaysPerWeek;
				for(int j=0; j<i; j++){
					int t2=c.times[this->_activities[j]];
					if(t2!=UNALLOCATED_TIME){
						//int day2=t2%r.nDaysPerWeek;
						int hour2=t2/r.nDaysPerWeek;
						int tmp=0;

						//activity weekly - counts as double
						/*if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY &&
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 4 * abs(hour1-hour2);
						else if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY ||
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 2 * abs(hour1-hour2);
						else*/
						
						//	tmp = abs(hour1-hour2);						
						if(hour1!=hour2)
							tmp=1;

						nbroken+=tmp;

						if(tmp>0 && conflictsString!=NULL){
							QString s=QObject::tr("Time constraint activities same starting hour broken, because activity with id=%1 is not at the same hour with activity with id=%2")
							 .arg(this->activitiesId[i])
							 .arg(this->activitiesId[j]);
							s+=", ";
							s+=QObject::tr("conflicts factor increase=%1").arg(tmp*weightPercentage/100);
							
							dl.append(s);
							cl.append(tmp*weightPercentage/100);
						
							*conflictsString+= s+"\n";
						}
					}
				}
			}
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintActivitiesSameStartingHour::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);

	for(int i=0; i<this->n_activities; i++)
		if(this->activitiesId[i]==a->id)
			return true;
	return false;
}

bool ConstraintActivitiesSameStartingHour::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintActivitiesSameStartingHour::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintActivitiesSameStartingHour::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintActivitiesSameStartingHour::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/
		
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivitiesSameStartingDay::ConstraintActivitiesSameStartingDay()
	: TimeConstraint()
{
	type=CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY;
}

ConstraintActivitiesSameStartingDay::ConstraintActivitiesSameStartingDay(double wp, int nact, const int act[])
 : TimeConstraint(wp)
 {
	assert(nact>=2 && nact<=MAX_CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY);
	this->n_activities=nact;
	for(int i=0; i<nact; i++)
		this->activitiesId[i]=act[i];

	this->type=CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY;
}

bool ConstraintActivitiesSameStartingDay::computeInternalStructure(Rules &r)
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
	
	if(this->_n_activities<=1){
		QMessageBox::warning(NULL, QObject::tr("FET error in data"), 
			QObject ::tr("Following constraint is wrong (because you need 2 or more activities. Please correct it):\n%1").arg(this->getDetailedDescription(r)));
		//assert(0);
		return false;
	}

	return true;
}

void ConstraintActivitiesSameStartingDay::removeUseless(Rules& r)
{
	//remove the activitiesId which no longer exist (used after the deletion of an activity)

	for(int j=0; j<this->n_activities; j++)
		this->_activities[j]=-1;

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

bool ConstraintActivitiesSameStartingDay::hasInactiveActivities(Rules& r)
{
	int count=0;

	for(int i=0; i<this->n_activities; i++)
		if(r.inactiveActivities.contains(this->activitiesId[i]))
			count++;

	if(this->n_activities-count<=1)
		return true;
	else
		return false;
}

QString ConstraintActivitiesSameStartingDay::getXmlDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="<ConstraintActivitiesSameStartingDay>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Number_of_Activities>"+QString::number(this->n_activities)+"</Number_of_Activities>\n";
	for(int i=0; i<this->n_activities; i++)
		s+="	<Activity_Id>"+QString::number(this->activitiesId[i])+"</Activity_Id>\n";
	s+="</ConstraintActivitiesSameStartingDay>\n";
	return s;
}

QString ConstraintActivitiesSameStartingDay::getDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s;
	s+=QObject::tr("Activities same starting day");s+=", ";
	s+=(QObject::tr("WP:%1\%", "Weight percentage").arg(this->weightPercentage));s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
	s+=(QObject::tr("NA:%1", "Number of activities").arg(this->n_activities));s+=", ";
	for(int i=0; i<this->n_activities; i++){
		s+=(QObject::tr("ID:%1").arg(this->activitiesId[i]));s+=", ";
	}

	return s;
}

QString ConstraintActivitiesSameStartingDay::getDetailedDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s;
	
	s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Activities must have the same starting day");s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	s+=(QObject::tr("Number of activities=%1").arg(this->n_activities));s+="\n";
	for(int i=0; i<this->n_activities; i++){
		s+=(QObject::tr("Activity with id=%1").arg(this->activitiesId[i]));
		
		//* write the teachers, subject and students sets
		//added in version 5.1.10
		int ai;
		for(ai=0; ai<r.activitiesList.size(); ai++)
			if(r.activitiesList[ai]->id==this->activitiesId[i])
				break;
		if(ai==r.activitiesList.size()){
			s+=QObject::tr(" Invalid (inexistent) activity id");
			s+="\n";
			return s;
		}
		assert(ai<r.activitiesList.size());
		s+=" ( ";
	
		s+=QObject::tr("T: ", "Teacher");
		int k=0;
		foreach(QString ss, r.activitiesList[ai]->teachersNames){
			if(k>0)
				s+=" ,";
			s+=ss;
			k++;
		}
	
		s+=QObject::tr(" , S: ", "Subject");
		s+=r.activitiesList[ai]->subjectName;
	
		if(r.activitiesList[ai]->activityTagName!="")
			s+=QObject::tr(" , AT: ", "Activity tag")+r.activitiesList[ai]->activityTagName;
	
		s+=QObject::tr(" , St: ", "Students");
		k=0;
		foreach(QString ss, r.activitiesList[ai]->studentsNames){
			if(k>0)
				s+=",";
			s+=ss;
			k++;
		}
	
		s+=" )";
		//* end section

		s+="\n";
	}

	return s;
}

double ConstraintActivitiesSameStartingDay::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	assert(r.internalStructureComputed);

	int nbroken;

	//We do not use the matrices 'subgroupsMatrix' nor 'teachersMatrix'.

	//sum the differences in the scheduled hour for all pairs of activities.

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int i=1; i<this->_n_activities; i++){
			int t1=c.times[this->_activities[i]];
			if(t1!=UNALLOCATED_TIME){
				int day1=t1%r.nDaysPerWeek;
				//int hour1=t1/r.nDaysPerWeek;
				for(int j=0; j<i; j++){
					int t2=c.times[this->_activities[j]];
					if(t2!=UNALLOCATED_TIME){
						int day2=t2%r.nDaysPerWeek;
						//int hour2=t2/r.nDaysPerWeek;
						int tmp=0;

						if(day1!=day2)
							tmp=1;

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
			int t1=c.times[this->_activities[i]];
			if(t1!=UNALLOCATED_TIME){
				int day1=t1%r.nDaysPerWeek;
				//int hour1=t1/r.nDaysPerWeek;
				for(int j=0; j<i; j++){
					int t2=c.times[this->_activities[j]];
					if(t2!=UNALLOCATED_TIME){
						int day2=t2%r.nDaysPerWeek;
						//int hour2=t2/r.nDaysPerWeek;
						int tmp=0;

						if(day1!=day2)
							tmp=1;

						nbroken+=tmp;

						if(tmp>0 && conflictsString!=NULL){
							QString s=QObject::tr("Time constraint activities same starting day broken, because activity with id=%1 is not in the same day with activity with id=%2")
							 .arg(this->activitiesId[i])
							 .arg(this->activitiesId[j]);
							s+=", ";
							s+=QObject::tr("conflicts factor increase=%1").arg(tmp*weightPercentage/100);
							
							dl.append(s);
							cl.append(tmp*weightPercentage/100);
						
							*conflictsString+= s+"\n";
						}
					}
				}
			}
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintActivitiesSameStartingDay::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);

	for(int i=0; i<this->n_activities; i++)
		if(this->activitiesId[i]==a->id)
			return true;
	return false;
}

bool ConstraintActivitiesSameStartingDay::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintActivitiesSameStartingDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintActivitiesSameStartingDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintActivitiesSameStartingDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/
		
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

Constraint2ActivitiesConsecutive::Constraint2ActivitiesConsecutive()
	: TimeConstraint()
{
	this->type = CONSTRAINT_2_ACTIVITIES_CONSECUTIVE;
}

Constraint2ActivitiesConsecutive::Constraint2ActivitiesConsecutive(double wp, int firstActId, int secondActId)
	: TimeConstraint(wp)
{
	this->firstActivityId = firstActId;
	this->secondActivityId=secondActId;
	this->type = CONSTRAINT_2_ACTIVITIES_CONSECUTIVE;
}

bool Constraint2ActivitiesConsecutive::computeInternalStructure(Rules& r)
{
	Activity* act;
	int i;
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		if(act->id==this->firstActivityId)
			break;
	}
	
	if(i==r.nInternalActivities){	
		//assert(0);
		QMessageBox::warning(NULL, QObject::tr("FET error in data"), 
			QObject ::tr("Following constraint is wrong (refers to inexistent activity ids):\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}

	this->firstActivityIndex=i;	

	////////
	
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		if(act->id==this->secondActivityId)
			break;
	}
	
	if(i==r.nInternalActivities){	
		//assert(0);
		QMessageBox::warning(NULL, QObject::tr("FET error in data"), 
			QObject ::tr("Following constraint is wrong (refers to inexistent activity ids):\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}

	this->secondActivityIndex=i;
	
	if(firstActivityIndex==secondActivityIndex){	
		//assert(0);
		QMessageBox::warning(NULL, QObject::tr("FET error in data"), 
			QObject ::tr("Following constraint is wrong (refers to same activities):\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}
	assert(firstActivityIndex!=secondActivityIndex);
	
	return true;
}

bool Constraint2ActivitiesConsecutive::hasInactiveActivities(Rules& r)
{
	if(r.inactiveActivities.contains(this->firstActivityId))
		return true;
	if(r.inactiveActivities.contains(this->secondActivityId))
		return true;
	return false;
}

QString Constraint2ActivitiesConsecutive::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s="<Constraint2ActivitiesConsecutive>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<First_Activity_Id>"+QString::number(this->firstActivityId)+"</First_Activity_Id>\n";
	s+="	<Second_Activity_Id>"+QString::number(this->secondActivityId)+"</Second_Activity_Id>\n";
	s+="</Constraint2ActivitiesConsecutive>\n";
	return s;
}

QString Constraint2ActivitiesConsecutive::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s;
	
	s=QObject::tr("Constraint 2 activities consecutive: ");
	
	s+=QObject::tr("first act. id:%1").arg(this->firstActivityId);
	s+=", ";
	s+=QObject::tr("second act. id:%1").arg(this->secondActivityId);
	s+=", ";	
	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);

	return s;
}

QString Constraint2ActivitiesConsecutive::getDetailedDescription(Rules& r)
{
	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Constraint 2 activities consecutive (second activity must be placed immediately after the first"
	 " activity, in the same day, possibly separated by breaks)"); s+="\n";
	
	s+=QObject::tr("First activity id=%1").arg(this->firstActivityId);

	//////////////////
	//* write the teachers, subject and students sets
	int ai;
	for(ai=0; ai<r.activitiesList.size(); ai++)
		if(r.activitiesList[ai]->id==this->firstActivityId)
			break;
	if(ai==r.activitiesList.size()){
		s+=QObject::tr(" Invalid (inexistent) activity id for first activity");
		s+="\n";
		return s;
	}
	assert(ai<r.activitiesList.size());
	s+=" (";
	
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
	
	s+=")";
	s+="\n";
	/////////////////////

	s+=QObject::tr("Second activity id=%1").arg(this->secondActivityId);
	
	//////////////////
	//* write the teachers, subject and students sets
	for(ai=0; ai<r.activitiesList.size(); ai++)
		if(r.activitiesList[ai]->id==this->secondActivityId)
			break;
	if(ai==r.activitiesList.size()){
		s+=QObject::tr(" Invalid (inexistent) activity id for second activity");
		s+="\n";
		return s;
	}
	assert(ai<r.activitiesList.size());
	s+=" (";
	
	s+=QObject::tr("T:");
	k=0;
	foreach(QString ss, r.activitiesList[ai]->teachersNames){
		if(k>0)
			s+=",";
		s+=ss;
		k++;
	}
	
	s+=QObject::tr(",S:");
	s+=r.activitiesList[ai]->subjectName;
	
	if(r.activitiesList[ai]->activityTagName!="")
		s+=QObject::tr(",AT:")+r.activitiesList[ai]->activityTagName;
	
	s+=QObject::tr(",St:");
	k=0;
	foreach(QString ss, r.activitiesList[ai]->studentsNames){
		if(k>0)
			s+=",";
		s+=ss;
		k++;
	}
	
	s+=")";
	s+="\n";
	/////////////////////

	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";

	return s;
}

double Constraint2ActivitiesConsecutive::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	assert(r.internalStructureComputed);

	nbroken=0;
	if(c.times[this->firstActivityIndex]!=UNALLOCATED_TIME && c.times[this->secondActivityIndex]!=UNALLOCATED_TIME){
		int fd=c.times[this->firstActivityIndex]%r.nDaysPerWeek; //the day when first activity was scheduled
		int fh=c.times[this->firstActivityIndex]/r.nDaysPerWeek; //the hour
		int sd=c.times[this->secondActivityIndex]%r.nDaysPerWeek; //the day when second activity was scheduled
		int sh=c.times[this->secondActivityIndex]/r.nDaysPerWeek; //the hour
		
		//cout<<"fd=="<<fd<<", fh=="<<fh<<", sd=="<<sd<<", sh=="<<sh<<endl;
		
		if(fd!=sd)
			nbroken=1;
		else if(fh+r.internalActivitiesList[this->firstActivityIndex].duration>sh)
			nbroken=1;
		else if(fd==sd){
			int h;
			int d=fd;
			assert(d==sd);
			for(h=fh+r.internalActivitiesList[this->firstActivityIndex].duration; h<r.nHoursPerDay; h++)
				if(!breakDayHour[d][h])
					break;
					
			assert(h<=sh);	
				
			if(h!=sh)
				nbroken=1;
		}
	}
	
	assert(nbroken==0 || nbroken==1);

	if(conflictsString!=NULL && nbroken>0){
		QString s=QObject::tr("Time constraint 2 activities consecutive broken for first activity with id=%1 and "
		 "second activity with id=%2, increases conflicts total by %3")
		 .arg(this->firstActivityId)
		 .arg(this->secondActivityId)
		 .arg(weightPercentage/100*nbroken);

		dl.append(s);
		cl.append(weightPercentage/100*nbroken);
	
		*conflictsString+= s+"\n";
	}
	
	if(weightPercentage==100)
		assert(nbroken==0);
	return nbroken * weightPercentage/100;
}

bool Constraint2ActivitiesConsecutive::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);

	if(this->firstActivityId==a->id)
		return true;
	if(this->secondActivityId==a->id)
		return true;
	return false;
}

bool Constraint2ActivitiesConsecutive::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool Constraint2ActivitiesConsecutive::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool Constraint2ActivitiesConsecutive::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool Constraint2ActivitiesConsecutive::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/
		
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

Constraint2ActivitiesOrdered::Constraint2ActivitiesOrdered()
	: TimeConstraint()
{
	this->type = CONSTRAINT_2_ACTIVITIES_ORDERED;
}

Constraint2ActivitiesOrdered::Constraint2ActivitiesOrdered(double wp, int firstActId, int secondActId)
	: TimeConstraint(wp)
{
	this->firstActivityId = firstActId;
	this->secondActivityId=secondActId;
	this->type = CONSTRAINT_2_ACTIVITIES_ORDERED;
}

bool Constraint2ActivitiesOrdered::computeInternalStructure(Rules& r)
{
	Activity* act;
	int i;
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		if(act->id==this->firstActivityId)
			break;
	}
	
	if(i==r.nInternalActivities){	
		//assert(0);
		QMessageBox::warning(NULL, QObject::tr("FET error in data"), 
			QObject ::tr("Following constraint is wrong (refers to inexistent activity ids):\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}

	this->firstActivityIndex=i;	

	////////
	
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		if(act->id==this->secondActivityId)
			break;
	}
	
	if(i==r.nInternalActivities){	
		//assert(0);
		QMessageBox::warning(NULL, QObject::tr("FET error in data"), 
			QObject ::tr("Following constraint is wrong (refers to inexistent activity ids):\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}

	this->secondActivityIndex=i;
	
	if(firstActivityIndex==secondActivityIndex){	
		//assert(0);
		QMessageBox::warning(NULL, QObject::tr("FET error in data"), 
			QObject ::tr("Following constraint is wrong (refers to same activities):\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}
	assert(firstActivityIndex!=secondActivityIndex);
	
	return true;
}

bool Constraint2ActivitiesOrdered::hasInactiveActivities(Rules& r)
{
	if(r.inactiveActivities.contains(this->firstActivityId))
		return true;
	if(r.inactiveActivities.contains(this->secondActivityId))
		return true;
	return false;
}

QString Constraint2ActivitiesOrdered::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s="<Constraint2ActivitiesOrdered>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<First_Activity_Id>"+QString::number(this->firstActivityId)+"</First_Activity_Id>\n";
	s+="	<Second_Activity_Id>"+QString::number(this->secondActivityId)+"</Second_Activity_Id>\n";
	s+="</Constraint2ActivitiesOrdered>\n";
	return s;
}

QString Constraint2ActivitiesOrdered::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s;
	
	s=QObject::tr("Constraint 2 activities ordered:");
	s+=" ";
	
	s+=QObject::tr("first act. id:%1").arg(this->firstActivityId);
	s+=", ";
	s+=QObject::tr("second act. id:%1").arg(this->secondActivityId);
	s+=", ";	
	s+=QObject::tr("WP:%1\%", "Weight percentage").arg(this->weightPercentage);

	return s;
}

QString Constraint2ActivitiesOrdered::getDetailedDescription(Rules& r)
{
	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Constraint 2 activities ordered (second activity must be placed at any time after the first"
	 " activity)"); s+="\n";
	
	s+=QObject::tr("First activity id=%1").arg(this->firstActivityId);

	//////////////////
	//* write the teachers, subject and students sets
	int ai;
	for(ai=0; ai<r.activitiesList.size(); ai++)
		if(r.activitiesList[ai]->id==this->firstActivityId)
			break;
	if(ai==r.activitiesList.size()){
		s+=QObject::tr(" Invalid (inexistent) activity id for first activity");
		s+="\n";
		return s;
	}
	assert(ai<r.activitiesList.size());
	s+=" (";
	
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
	
	s+=")";
	s+="\n";
	/////////////////////

	s+=QObject::tr("Second activity id=%1").arg(this->secondActivityId);
	
	//////////////////
	//* write the teachers, subject and students sets
	for(ai=0; ai<r.activitiesList.size(); ai++)
		if(r.activitiesList[ai]->id==this->secondActivityId)
			break;
	if(ai==r.activitiesList.size()){
		s+=QObject::tr(" Invalid (inexistent) activity id for second activity");
		s+="\n";
		return s;
	}
	assert(ai<r.activitiesList.size());
	s+=" (";
	
	s+=QObject::tr("T:");
	k=0;
	foreach(QString ss, r.activitiesList[ai]->teachersNames){
		if(k>0)
			s+=",";
		s+=ss;
		k++;
	}
	
	s+=QObject::tr(",S:");
	s+=r.activitiesList[ai]->subjectName;
	
	if(r.activitiesList[ai]->activityTagName!="")
		s+=QObject::tr(",AT:")+r.activitiesList[ai]->activityTagName;
	
	s+=QObject::tr(",St:");
	k=0;
	foreach(QString ss, r.activitiesList[ai]->studentsNames){
		if(k>0)
			s+=",";
		s+=ss;
		k++;
	}
	
	s+=")";
	s+="\n";
	/////////////////////

	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";

	return s;
}

double Constraint2ActivitiesOrdered::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	assert(r.internalStructureComputed);

	nbroken=0;
	if(c.times[this->firstActivityIndex]!=UNALLOCATED_TIME && c.times[this->secondActivityIndex]!=UNALLOCATED_TIME){
		int fd=c.times[this->firstActivityIndex]%r.nDaysPerWeek; //the day when first activity was scheduled
		int fh=c.times[this->firstActivityIndex]/r.nDaysPerWeek
		  + r.internalActivitiesList[this->firstActivityIndex].duration-1; //the end hour of first activity
		int sd=c.times[this->secondActivityIndex]%r.nDaysPerWeek; //the day when second activity was scheduled
		int sh=c.times[this->secondActivityIndex]/r.nDaysPerWeek; //the start hour of second activity
		
		if(!(fd<sd || (fd==sd && fh<sh)))
			nbroken=1;
	}
	
	assert(nbroken==0 || nbroken==1);

	if(conflictsString!=NULL && nbroken>0){
		QString s=QObject::tr("Time constraint 2 activities ordered broken for first activity with id=%1 and "
		 "second activity with id=%2, increases conflicts total by %3")
		 .arg(this->firstActivityId)
		 .arg(this->secondActivityId)
		 .arg(weightPercentage/100*nbroken);

		dl.append(s);
		cl.append(weightPercentage/100*nbroken);
	
		*conflictsString+= s+"\n";
	}
	
	if(weightPercentage==100)
		assert(nbroken==0);
	return nbroken * weightPercentage/100;
}

bool Constraint2ActivitiesOrdered::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);

	if(this->firstActivityId==a->id)
		return true;
	if(this->secondActivityId==a->id)
		return true;
	return false;
}

bool Constraint2ActivitiesOrdered::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool Constraint2ActivitiesOrdered::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool Constraint2ActivitiesOrdered::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool Constraint2ActivitiesOrdered::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/
		
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivityEndsStudentsDay::ConstraintActivityEndsStudentsDay()
	: TimeConstraint()
{
	this->type = CONSTRAINT_ACTIVITY_ENDS_STUDENTS_DAY;
}

ConstraintActivityEndsStudentsDay::ConstraintActivityEndsStudentsDay(double wp, int actId)
	: TimeConstraint(wp)
{
	this->activityId = actId;
	this->type = CONSTRAINT_ACTIVITY_ENDS_STUDENTS_DAY;
}

bool ConstraintActivityEndsStudentsDay::computeInternalStructure(Rules& r)
{
	Activity* act;
	int i;
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		if(act->id==this->activityId)
			break;
	}
	
	if(i==r.nInternalActivities){	
		//assert(0);
		QMessageBox::warning(NULL, QObject::tr("FET error in data"), 
			QObject ::tr("Following constraint is wrong (because it refers to invalid activity id. Please correct (maybe removing it is a solution)):\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}

	this->activityIndex=i;	
	return true;
}

bool ConstraintActivityEndsStudentsDay::hasInactiveActivities(Rules& r)
{
	if(r.inactiveActivities.contains(this->activityId))
		return true;
	return false;
}

QString ConstraintActivityEndsStudentsDay::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s="<ConstraintActivityEndsStudentsDay>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Activity_Id>"+QString::number(this->activityId)+"</Activity_Id>\n";
	s+="</ConstraintActivityEndsStudentsDay>\n";
	return s;
}

QString ConstraintActivityEndsStudentsDay::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	//Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s;
	s+=(QObject::tr("Act. id:%1 must end students' day").arg(this->activityId));
	
	//* write the teachers, subject and students sets
	//added in version 4.1.4
	int ai;
	for(ai=0; ai<r.activitiesList.size(); ai++)
		if(r.activitiesList[ai]->id==this->activityId)
			break;
	if(ai==r.activitiesList.size()){
		s+=QObject::tr(" Invalid (inexistent) activity id");
		return s;
	}
	assert(ai<r.activitiesList.size());
	s+=" (";
	
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
	
	s+=")";
	//* end section
	
	s+=", ";

	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));

	return s;
}

QString ConstraintActivityEndsStudentsDay::getDetailedDescription(Rules& r)
{
	QString s=QObject::tr("Time constraint");s+="\n";
	s+=(QObject::tr("Activity with id=%1 must end students' day").arg(this->activityId));
	
	//* write the teachers, subject and students sets
	//added in version 4.1.4
	int ai;
	for(ai=0; ai<r.activitiesList.size(); ai++)
		if(r.activitiesList[ai]->id==this->activityId)
			break;
	if(ai==r.activitiesList.size()){
		s+=QObject::tr(" Invalid (inexistent) activity id");
		s+="\n";
		return s;
	}
	assert(ai<r.activitiesList.size());
	s+=" (";
	
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
	
	s+=")";
	//* end section

	s+="\n";

	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

double ConstraintActivityEndsStudentsDay::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	assert(r.internalStructureComputed);

	nbroken=0;
	if(c.times[this->activityIndex]!=UNALLOCATED_TIME){
		int d=c.times[this->activityIndex]%r.nDaysPerWeek; //the day when this activity was scheduled
		int h=c.times[this->activityIndex]/r.nDaysPerWeek; //the hour
		
		int i=this->activityIndex;
		for(int j=0; j<r.internalActivitiesList[i].iSubgroupsList.count(); j++){
			int sb=r.internalActivitiesList[i].iSubgroupsList.at(j);
			for(int hh=h+r.internalActivitiesList[i].duration; hh<r.nHoursPerDay; hh++)
				if(subgroupsMatrix[sb][d][hh]>0){
					nbroken=1;
					break;
				}
			if(nbroken)
				break;
		}
	}

	if(conflictsString!=NULL && nbroken>0){
		QString s=QObject::tr("Time constraint activity ends students' day broken for activity with id=%1, increases conflicts total by %2")
		 .arg(this->activityId)
		 .arg(weightPercentage/100*nbroken);

		dl.append(s);
		cl.append(weightPercentage/100*nbroken);
	
		*conflictsString+= s+"\n";
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return nbroken * weightPercentage/100;
}

bool ConstraintActivityEndsStudentsDay::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);

	if(this->activityId==a->id)
		return true;
	return false;
}

bool ConstraintActivityEndsStudentsDay::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintActivityEndsStudentsDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintActivityEndsStudentsDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintActivityEndsStudentsDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/
		
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersMinHoursDaily::ConstraintTeachersMinHoursDaily()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHERS_MIN_HOURS_DAILY;
}

ConstraintTeachersMinHoursDaily::ConstraintTeachersMinHoursDaily(double wp, int minhours)
 : TimeConstraint(wp)
 {
	assert(minhours>0);
	this->minHoursDaily=minhours;

	this->type=CONSTRAINT_TEACHERS_MIN_HOURS_DAILY;
}

bool ConstraintTeachersMinHoursDaily::computeInternalStructure(Rules& r)
{
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;
	
	return true;
}

bool ConstraintTeachersMinHoursDaily::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeachersMinHoursDaily::getXmlDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="<ConstraintTeachersMinHoursDaily>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Minimum_Hours_Daily>"+QString::number(this->minHoursDaily)+"</Minimum_Hours_Daily>\n";
	s+="</ConstraintTeachersMinHoursDaily>\n";
	return s;
}

QString ConstraintTeachersMinHoursDaily::getDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s;
	s+=(QObject::tr("Teachers min %1 hours daily").arg(this->minHoursDaily));s+=", ";
	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));

	return s;
}

QString ConstraintTeachersMinHoursDaily::getDetailedDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=(QObject::tr("Teachers must not have less than %1 hours daily").arg(this->minHoursDaily));s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	s+=QObject::tr("Note: FET is smart enough to use this constraint only on working days of the teachers");
	s+="\n";

	return s;
}

double ConstraintTeachersMinHoursDaily::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int i=0; i<r.nInternalTeachers; i++){
			for(int d=0; d<r.nDaysPerWeek; d++){
				int n_hours_daily=0;
				for(int h=0; h<r.nHoursPerDay; h++)
					if(teachersMatrix[i][d][h]>0)
						n_hours_daily++;

				if(n_hours_daily>0 && n_hours_daily<this->minHoursDaily){
					nbroken++;
				}
			}
		}
	}
	//with logging
	else{
		nbroken=0;
		for(int i=0; i<r.nInternalTeachers; i++){
			for(int d=0; d<r.nDaysPerWeek; d++){
				int n_hours_daily=0;
				for(int h=0; h<r.nHoursPerDay; h++)
					if(teachersMatrix[i][d][h]>0)
						n_hours_daily++;

				if(n_hours_daily>0 && n_hours_daily<this->minHoursDaily){
					nbroken++;

					if(conflictsString!=NULL){
						QString s=(QObject::tr(
						 "Time constraint teachers min %1 hours daily broken for teacher %2, on day %3, length=%4.")
						 .arg(QString::number(this->minHoursDaily))
						 .arg(r.internalTeachersList[i]->name)
						 .arg(r.daysOfTheWeek[d])
						 .arg(n_hours_daily)
						 )
						 +
						 " "
						 +
						 (QObject::tr("This increases the conflicts total by %1").arg(QString::number(weightPercentage/100)));
						
						dl.append(s);
						cl.append(weightPercentage/100);
					
						*conflictsString+= s+"\n";
					}
				}
			}
		}
	}

	/*int na=0;
	for(int i=0; i<r.nInternalActivities; i++)
		if(c.times[i]!=UNALLOCATED_TIME)
			na++;*/
	if(c.nPlacedActivities==r.nInternalActivities)
	//if(na==r.nInternalActivities)
		//does not work for partial solutions
		if(weightPercentage==100)	
			assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintTeachersMinHoursDaily::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(a);
	Q_UNUSED(r);
	//if(a)
	//	;

	return false;
}

bool ConstraintTeachersMinHoursDaily::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return true;
}

bool ConstraintTeachersMinHoursDaily::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintTeachersMinHoursDaily::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintTeachersMinHoursDaily::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMinHoursDaily::ConstraintTeacherMinHoursDaily()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHER_MIN_HOURS_DAILY;
}

ConstraintTeacherMinHoursDaily::ConstraintTeacherMinHoursDaily(double wp, int minhours, const QString& teacher)
 : TimeConstraint(wp)
 {
	assert(minhours>0);
	this->minHoursDaily=minhours;
	this->teacherName=teacher;

	this->type=CONSTRAINT_TEACHER_MIN_HOURS_DAILY;
}

bool ConstraintTeacherMinHoursDaily::computeInternalStructure(Rules& r)
{
	this->teacher_ID=r.searchTeacher(this->teacherName);
	assert(this->teacher_ID>=0);
	return true;
}

bool ConstraintTeacherMinHoursDaily::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeacherMinHoursDaily::getXmlDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="<ConstraintTeacherMinHoursDaily>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Teacher_Name>"+protect(this->teacherName)+"</Teacher_Name>\n";
	s+="	<Minimum_Hours_Daily>"+QString::number(this->minHoursDaily)+"</Minimum_Hours_Daily>\n";
	s+="</ConstraintTeacherMinHoursDaily>\n";
	return s;
}

QString ConstraintTeacherMinHoursDaily::getDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s;
	s+=(QObject::tr("Teacher min %1 hours daily").arg(this->minHoursDaily));s+=", ";
	s+=QObject::tr("TN:%1").arg(this->teacherName);s+=", ";
	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));

	return s;
}

QString ConstraintTeacherMinHoursDaily::getDetailedDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Teacher %1 must not have less than %2 hours daily").arg(this->teacherName).arg(this->minHoursDaily);s+="\n";
	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";
	s+=QObject::tr("Note: FET is smart enough to use this constraint only on working days of the teacher");
	s+="\n";

	return s;
}

double ConstraintTeacherMinHoursDaily::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		int i=this->teacher_ID;
		for(int d=0; d<r.nDaysPerWeek; d++){
			int n_hours_daily=0;
			for(int h=0; h<r.nHoursPerDay; h++)
				if(teachersMatrix[i][d][h]>0)
					n_hours_daily++;

			if(n_hours_daily>0 && n_hours_daily<this->minHoursDaily){
				nbroken++;
			}
		}
	}
	//with logging
	else{
		nbroken=0;
		int i=this->teacher_ID;
		for(int d=0; d<r.nDaysPerWeek; d++){
			int n_hours_daily=0;
			for(int h=0; h<r.nHoursPerDay; h++)
				if(teachersMatrix[i][d][h]>0)
					n_hours_daily++;

			if(n_hours_daily>0 && n_hours_daily<this->minHoursDaily){
				nbroken++;

				if(conflictsString!=NULL){
					QString s=(QObject::tr(
					 "Time constraint teacher min %1 hours daily broken for teacher %2, on day %3, length=%4.")
					 .arg(QString::number(this->minHoursDaily))
					 .arg(r.internalTeachersList[i]->name)
					 .arg(r.daysOfTheWeek[d])
					 .arg(n_hours_daily)
					 )
					 +" "
					 +
					 (QObject::tr("This increases the conflicts total by %1").arg(QString::number(weightPercentage/100)));
						
					dl.append(s);
					cl.append(weightPercentage/100);
				
					*conflictsString+= s+"\n";
				}
			}
		}
	}

	/*int na=0;
	for(int i=0; i<r.nInternalActivities; i++)
		if(c.times[i]!=UNALLOCATED_TIME)
			na++;*/
	if(c.nPlacedActivities==r.nInternalActivities)
	//if(na==r.nInternalActivities)
		//does not work for partial solutions
		if(weightPercentage==100)
			assert(nbroken==0);
			
	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherMinHoursDaily::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);
	//if(a)
	//	;

	return false;
}

bool ConstraintTeacherMinHoursDaily::isRelatedToTeacher(Teacher* t)
{
	if(this->teacherName==t->name)
		return true;
	return false;
}

bool ConstraintTeacherMinHoursDaily::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintTeacherMinHoursDaily::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintTeacherMinHoursDaily::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherIntervalMaxDaysPerWeek::ConstraintTeacherIntervalMaxDaysPerWeek()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHER_INTERVAL_MAX_DAYS_PER_WEEK;
}

ConstraintTeacherIntervalMaxDaysPerWeek::ConstraintTeacherIntervalMaxDaysPerWeek(double wp, int maxnd, QString tn, int sh, int eh)
	 : TimeConstraint(wp)
{
	this->teacherName = tn;
	this->maxDaysPerWeek=maxnd;
	this->type=CONSTRAINT_TEACHER_INTERVAL_MAX_DAYS_PER_WEEK;
	this->startHour=sh;
	this->endHour=eh;
	assert(sh<eh);
	assert(sh>=0);
}

bool ConstraintTeacherIntervalMaxDaysPerWeek::computeInternalStructure(Rules& r)
{
	this->teacher_ID=r.searchTeacher(this->teacherName);
	assert(this->teacher_ID>=0);
	if(this->startHour>=this->endHour){
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Constraint teacher interval max days per week is wrong because start hour >= end hour."
		 " Please correct it. Constraint is:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
	if(this->startHour<0){
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Constraint teacher interval max days per week is wrong because start hour < first hour or the day."
		 " Please correct it. Constraint is:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
	if(this->endHour>r.nHoursPerDay){
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Constraint teacher interval max days per week is wrong because end hour > number of hours per day."
		 " Please correct it. Constraint is:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
	return true;
}

bool ConstraintTeacherIntervalMaxDaysPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeacherIntervalMaxDaysPerWeek::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="<ConstraintTeacherIntervalMaxDaysPerWeek>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Teacher_Name>"+protect(this->teacherName)+"</Teacher_Name>\n";
	s+="	<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->startHour])+"</Interval_Start_Hour>\n";
	if(this->endHour < r.nHoursPerDay){
		s+="	<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->endHour])+"</Interval_End_Hour>\n";
	}
	else{
		s+="	<Interval_End_Hour></Interval_End_Hour>\n";
		s+="	<!-- Interval_End_Hour void means the end of the day (which has no name) -->\n";
	}
	s+="	<Max_Days_Per_Week>"+QString::number(this->maxDaysPerWeek)+"</Max_Days_Per_Week>\n";
	s+="</ConstraintTeacherIntervalMaxDaysPerWeek>\n";
	return s;
}

QString ConstraintTeacherIntervalMaxDaysPerWeek::getDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Teacher interval max days per week");s+=", ";
	s+=(QObject::tr("WP:%1\%", "Abbreviation for weight percentage").arg(this->weightPercentage));s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
	s+=(QObject::tr("T:%1", "Abbreviation for teacher").arg(this->teacherName));s+=", ";
	s+=QObject::tr("ISH:%1", "Abbreviation for interval start hour").arg(r.hoursOfTheDay[this->startHour]);
	s+=", ";
	if(this->endHour<r.nHoursPerDay)
		s+=QObject::tr("IEH:%1", "Abbreviation for interval end hour").arg(r.hoursOfTheDay[this->endHour]);
	else
		s+=QObject::tr("IEH:%1", "Abbreviation for interval end hour").arg(QObject::tr("End of day"));
	s+=", ";
	s+=(QObject::tr("MD:%1", "Abbreviation for max days").arg(this->maxDaysPerWeek));

	return s;
}

QString ConstraintTeacherIntervalMaxDaysPerWeek::getDetailedDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Teacher interval max. days per week");s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	s+=(QObject::tr("Teacher=%1").arg(this->teacherName));s+="\n";
	s+=QObject::tr("Interval start hour=%1").arg(r.hoursOfTheDay[this->startHour]);s+="\n";

	if(this->endHour<r.nHoursPerDay)
		s+=QObject::tr("Interval end hour=%1").arg(r.hoursOfTheDay[this->endHour]);
	else
		s+=QObject::tr("Interval end hour=%1").arg(QObject::tr("End of day"));
	s+="\n";

	s+=(QObject::tr("Max. days per week=%1").arg(this->maxDaysPerWeek));s+="\n";

	return s;
}

double ConstraintTeacherIntervalMaxDaysPerWeek::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString *conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken;
	
	int t=this->teacher_ID;

	nbroken=0;
	bool ocDay[MAX_DAYS_PER_WEEK];
	for(int d=0; d<r.nDaysPerWeek; d++){
		ocDay[d]=false;
		for(int h=startHour; h<endHour; h++){
			if(teachersMatrix[t][d][h]>0){
				ocDay[d]=true;
			}
		}
	}
	int nOcDays=0;
	for(int d=0; d<r.nDaysPerWeek; d++)
		if(ocDay[d])
			nOcDays++;
	if(nOcDays > this->maxDaysPerWeek){
		nbroken+=nOcDays-this->maxDaysPerWeek;

		if(nbroken>0){
			QString s= QObject::tr("Time constraint teacher interval max days per week broken for teacher: %1, allowed %2 days, required %3 days.")
			 .arg(r.internalTeachersList[t]->name)
			 .arg(this->maxDaysPerWeek)
			 .arg(nOcDays);
			s+=" ";
			s += QObject::tr("This increases the conflicts total by %1")
			 .arg(nbroken*weightPercentage/100);
			 
			dl.append(s);
			cl.append(nbroken*weightPercentage/100);
		
			*conflictsString += s+"\n";
		}
	}
	//cout<<"teacher = "<<qPrintable(this->teacherName)<<", nOcDays = "<<nOcDays<<", max days per week = "<<this->maxDaysPerWeek<<endl;

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherIntervalMaxDaysPerWeek::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);
	//if(a)
	//	;

	return false;
}

bool ConstraintTeacherIntervalMaxDaysPerWeek::isRelatedToTeacher(Teacher* t)
{
	if(this->teacherName==t->name)
		return true;
	return false;
}

bool ConstraintTeacherIntervalMaxDaysPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintTeacherIntervalMaxDaysPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintTeacherIntervalMaxDaysPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersIntervalMaxDaysPerWeek::ConstraintTeachersIntervalMaxDaysPerWeek()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHERS_INTERVAL_MAX_DAYS_PER_WEEK;
}

ConstraintTeachersIntervalMaxDaysPerWeek::ConstraintTeachersIntervalMaxDaysPerWeek(double wp, int maxnd, int sh, int eh)
	 : TimeConstraint(wp)
{
	this->maxDaysPerWeek=maxnd;
	this->type=CONSTRAINT_TEACHERS_INTERVAL_MAX_DAYS_PER_WEEK;
	this->startHour=sh;
	this->endHour=eh;
	assert(sh<eh);
	assert(sh>=0);
}

bool ConstraintTeachersIntervalMaxDaysPerWeek::computeInternalStructure(Rules& r)
{
	//this->teacher_ID=r.searchTeacher(this->teacherName);
	//assert(this->teacher_ID>=0);
	if(this->startHour>=this->endHour){
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Constraint teacher interval max days per week is wrong because start hour >= end hour."
		 " Please correct it. Constraint is:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
	if(this->startHour<0){
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Constraint teacher interval max days per week is wrong because start hour < first hour or the day."
		 " Please correct it. Constraint is:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
	if(this->endHour>r.nHoursPerDay){
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Constraint teacher interval max days per week is wrong because end hour > number of hours per day."
		 " Please correct it. Constraint is:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
	return true;
}

bool ConstraintTeachersIntervalMaxDaysPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintTeachersIntervalMaxDaysPerWeek::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="<ConstraintTeachersIntervalMaxDaysPerWeek>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Teacher_Name>"+protect(this->teacherName)+"</Teacher_Name>\n";
	s+="	<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->startHour])+"</Interval_Start_Hour>\n";
	if(this->endHour < r.nHoursPerDay){
		s+="	<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->endHour])+"</Interval_End_Hour>\n";
	}
	else{
		s+="	<Interval_End_Hour></Interval_End_Hour>\n";
		s+="	<!-- Interval_End_Hour void means the end of the day (which has no name) -->\n";
	}
	s+="	<Max_Days_Per_Week>"+QString::number(this->maxDaysPerWeek)+"</Max_Days_Per_Week>\n";
	s+="</ConstraintTeachersIntervalMaxDaysPerWeek>\n";
	return s;
}

QString ConstraintTeachersIntervalMaxDaysPerWeek::getDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Teachers interval max days per week");s+=", ";
	s+=(QObject::tr("WP:%1\%", "Abbreviation for weight percentage").arg(this->weightPercentage));s+=", ";
	//s+=(QObject::tr("T:%1").arg(this->teacherName));s+=", ";
	s+=QObject::tr("ISH:%1", "Abbreviation for interval start hour").arg(r.hoursOfTheDay[this->startHour]);
	s+=", ";
	if(this->endHour<r.nHoursPerDay)
		s+=QObject::tr("IEH:%1", "Abbreviation for interval end hour").arg(r.hoursOfTheDay[this->endHour]);
	else
		s+=QObject::tr("IEH:%1", "Abbreviation for interval end hour").arg(QObject::tr("End of day"));
	s+=", ";
	s+=(QObject::tr("MD:%1", "Abbreviation for max days").arg(this->maxDaysPerWeek));

	return s;
}

QString ConstraintTeachersIntervalMaxDaysPerWeek::getDetailedDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Teachers interval max. days per week");s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Teacher=%1").arg(this->teacherName));s+="\n";
	s+=QObject::tr("Interval start hour=%1").arg(r.hoursOfTheDay[this->startHour]);s+="\n";

	if(this->endHour<r.nHoursPerDay)
		s+=QObject::tr("Interval end hour=%1").arg(r.hoursOfTheDay[this->endHour]);
	else
		s+=QObject::tr("Interval end hour=%1").arg(QObject::tr("End of day"));
	s+="\n";

	s+=(QObject::tr("Max. days per week=%1").arg(this->maxDaysPerWeek));s+="\n";

	return s;
}

double ConstraintTeachersIntervalMaxDaysPerWeek::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString *conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	for(int t=0; t<r.nInternalTeachers; t++){
		bool ocDay[MAX_DAYS_PER_WEEK];
		for(int d=0; d<r.nDaysPerWeek; d++){
			ocDay[d]=false;
			for(int h=startHour; h<endHour; h++){
				if(teachersMatrix[t][d][h]>0){
					ocDay[d]=true;
				}
			}
		}
		int nOcDays=0;
		for(int d=0; d<r.nDaysPerWeek; d++)
			if(ocDay[d])
				nOcDays++;
		if(nOcDays > this->maxDaysPerWeek){
			nbroken+=nOcDays-this->maxDaysPerWeek;

			if(nOcDays-this->maxDaysPerWeek>0){
				QString s= QObject::tr("Time constraint teachers interval max days per week broken for teacher: %1, allowed %2 days, required %3 days.")
				 .arg(r.internalTeachersList[t]->name)
				 .arg(this->maxDaysPerWeek)
				 .arg(nOcDays);
				s+=" ";
				s += QObject::tr("This increases the conflicts total by %1")
				 .arg((nOcDays-this->maxDaysPerWeek)*weightPercentage/100);
				 
				dl.append(s);
				cl.append((nOcDays-this->maxDaysPerWeek)*weightPercentage/100);
			
				*conflictsString += s+"\n";
			}
		}
		//cout<<"teacher = "<<qPrintable(r.internalTeachersList[t]->name)<<", nOcDays = "<<nOcDays<<", max days per week = "<<this->maxDaysPerWeek<<endl;
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintTeachersIntervalMaxDaysPerWeek::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);
	//if(a)
	//	;

	return false;
}

bool ConstraintTeachersIntervalMaxDaysPerWeek::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return true;
}

bool ConstraintTeachersIntervalMaxDaysPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintTeachersIntervalMaxDaysPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintTeachersIntervalMaxDaysPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetIntervalMaxDaysPerWeek::ConstraintStudentsSetIntervalMaxDaysPerWeek()
	: TimeConstraint()
{
	this->type=CONSTRAINT_STUDENTS_SET_INTERVAL_MAX_DAYS_PER_WEEK;
}

ConstraintStudentsSetIntervalMaxDaysPerWeek::ConstraintStudentsSetIntervalMaxDaysPerWeek(double wp, int maxnd, QString sn, int sh, int eh)
	 : TimeConstraint(wp)
{
	this->students = sn;
	this->maxDaysPerWeek=maxnd;
	this->type=CONSTRAINT_STUDENTS_SET_INTERVAL_MAX_DAYS_PER_WEEK;
	this->startHour=sh;
	this->endHour=eh;
	assert(sh<eh);
	assert(sh>=0);
}

bool ConstraintStudentsSetIntervalMaxDaysPerWeek::computeInternalStructure(Rules& r)
{
	if(this->startHour>=this->endHour){
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Constraint teacher interval max days per week is wrong because start hour >= end hour."
		 " Please correct it. Constraint is:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
	if(this->startHour<0){
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Constraint teacher interval max days per week is wrong because start hour < first hour or the day."
		 " Please correct it. Constraint is:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
	if(this->endHour>r.nHoursPerDay){
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Constraint teacher interval max days per week is wrong because end hour > number of hours per day."
		 " Please correct it. Constraint is:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}

	/////////
	StudentsSet* ss=r.searchAugmentedStudentsSet(this->students);

	if(ss==NULL){
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Constraint students set interval max days per week is wrong because it refers to inexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}	

	assert(ss);

	//this->nSubgroups=0;
	this->iSubgroupsList.clear();
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		/*for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
			if(r.internalSubgroupsList[tmp]->name == ss->name)
				break;*/
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		//assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
		//this->subgroups[this->nSubgroups++]=tmp;
		if(!this->iSubgroupsList.contains(tmp))
			this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			/*for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
				if(r.internalSubgroupsList[tmp]->name == sts->name)
					break;*/
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			//assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
			//this->subgroups[this->nSubgroups++]=tmp;
			if(!this->iSubgroupsList.contains(tmp))
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
				/*for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
					if(r.internalSubgroupsList[tmp]->name == sts->name)
						break;*/
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				//assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
				//this->subgroups[this->nSubgroups++]=tmp;
				if(!this->iSubgroupsList.contains(tmp))
					this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);
		
	return true;
}

bool ConstraintStudentsSetIntervalMaxDaysPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsSetIntervalMaxDaysPerWeek::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="<ConstraintStudentsSetIntervalMaxDaysPerWeek>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Students>"+protect(this->students)+"</Students>\n";
	s+="	<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->startHour])+"</Interval_Start_Hour>\n";
	if(this->endHour < r.nHoursPerDay){
		s+="	<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->endHour])+"</Interval_End_Hour>\n";
	}
	else{
		s+="	<Interval_End_Hour></Interval_End_Hour>\n";
		s+="	<!-- Interval_End_Hour void means the end of the day (which has no name) -->\n";
	}
	s+="	<Max_Days_Per_Week>"+QString::number(this->maxDaysPerWeek)+"</Max_Days_Per_Week>\n";
	s+="</ConstraintStudentsSetIntervalMaxDaysPerWeek>\n";
	return s;
}

QString ConstraintStudentsSetIntervalMaxDaysPerWeek::getDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Students set interval max days per week");s+=", ";
	s+=(QObject::tr("WP:%1\%", "Abbreviation for weight percentage").arg(this->weightPercentage));s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
	s+=(QObject::tr("S:%1", "Abbreviation for students").arg(this->students));s+=", ";
	s+=QObject::tr("ISH:%1", "Abbreviation for interval start hour").arg(r.hoursOfTheDay[this->startHour]);
	s+=", ";
	if(this->endHour<r.nHoursPerDay)
		s+=QObject::tr("IEH:%1", "Abbreviation for interval end hour").arg(r.hoursOfTheDay[this->endHour]);
	else
		s+=QObject::tr("IEH:%1", "Abbreviation for interval end hour").arg(QObject::tr("End of day"));
	s+=", ";
	s+=(QObject::tr("MD:%1", "Abbreviation for max days").arg(this->maxDaysPerWeek));

	return s;
}

QString ConstraintStudentsSetIntervalMaxDaysPerWeek::getDetailedDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Students set interval max. days per week");s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	s+=(QObject::tr("Students set=%1").arg(this->students));s+="\n";
	s+=QObject::tr("Interval start hour=%1").arg(r.hoursOfTheDay[this->startHour]);s+="\n";

	if(this->endHour<r.nHoursPerDay)
		s+=QObject::tr("Interval end hour=%1").arg(r.hoursOfTheDay[this->endHour]);
	else
		s+=QObject::tr("Interval end hour=%1").arg(QObject::tr("End of day"));
	s+="\n";

	s+=(QObject::tr("Max. days per week=%1").arg(this->maxDaysPerWeek));s+="\n";

	return s;
}

double ConstraintStudentsSetIntervalMaxDaysPerWeek::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString *conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken;
	
	nbroken=0;
	
	foreach(int sbg, this->iSubgroupsList){
		bool ocDay[MAX_DAYS_PER_WEEK];
		for(int d=0; d<r.nDaysPerWeek; d++){
			ocDay[d]=false;
			for(int h=startHour; h<endHour; h++){
				if(subgroupsMatrix[sbg][d][h]>0){
					ocDay[d]=true;
				}
			}
		}
		int nOcDays=0;
		for(int d=0; d<r.nDaysPerWeek; d++)
			if(ocDay[d])
				nOcDays++;
		if(nOcDays > this->maxDaysPerWeek){
			nbroken+=nOcDays-this->maxDaysPerWeek;

			if((nOcDays-this->maxDaysPerWeek)>0){
				QString s= QObject::tr("Time constraint students set interval max days per week broken for subgroup: %1, allowed %2 days, required %3 days.")
				 .arg(r.internalSubgroupsList[sbg]->name)
				 .arg(this->maxDaysPerWeek)
				 .arg(nOcDays);
				s+=" ";
				s += QObject::tr("This increases the conflicts total by %1")
				 .arg((nOcDays-this->maxDaysPerWeek)*weightPercentage/100);
			 
				dl.append(s);
				cl.append((nOcDays-this->maxDaysPerWeek)*weightPercentage/100);
		
				*conflictsString += s+"\n";
			}
		}
		//cout<<"subgroup = "<<qPrintable(r.internalSubgroupsList[sbg]->name)<<", nOcDays = "<<nOcDays<<", max days per week = "<<this->maxDaysPerWeek<<endl;
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsSetIntervalMaxDaysPerWeek::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);
	//if(a)
	//	;

	return false;
}

bool ConstraintStudentsSetIntervalMaxDaysPerWeek::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	return false;
}

bool ConstraintStudentsSetIntervalMaxDaysPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintStudentsSetIntervalMaxDaysPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintStudentsSetIntervalMaxDaysPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.studentsSetsRelated(this->students, s->name);
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsIntervalMaxDaysPerWeek::ConstraintStudentsIntervalMaxDaysPerWeek()
	: TimeConstraint()
{
	this->type=CONSTRAINT_STUDENTS_INTERVAL_MAX_DAYS_PER_WEEK;
}

ConstraintStudentsIntervalMaxDaysPerWeek::ConstraintStudentsIntervalMaxDaysPerWeek(double wp, int maxnd, int sh, int eh)
	 : TimeConstraint(wp)
{
	//this->students = sn;
	this->maxDaysPerWeek=maxnd;
	this->type=CONSTRAINT_STUDENTS_INTERVAL_MAX_DAYS_PER_WEEK;
	this->startHour=sh;
	this->endHour=eh;
	assert(sh<eh);
	assert(sh>=0);
}

bool ConstraintStudentsIntervalMaxDaysPerWeek::computeInternalStructure(Rules& r)
{
	if(this->startHour>=this->endHour){
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Constraint teacher interval max days per week is wrong because start hour >= end hour."
		 " Please correct it. Constraint is:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
	if(this->startHour<0){
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Constraint teacher interval max days per week is wrong because start hour < first hour or the day."
		 " Please correct it. Constraint is:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
	if(this->endHour>r.nHoursPerDay){
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Constraint teacher interval max days per week is wrong because end hour > number of hours per day."
		 " Please correct it. Constraint is:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}

	return true;
}

bool ConstraintStudentsIntervalMaxDaysPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintStudentsIntervalMaxDaysPerWeek::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s="<ConstraintStudentsIntervalMaxDaysPerWeek>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
//	s+="	<Students>"+protect(this->students)+"</Students>\n";
	s+="	<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->startHour])+"</Interval_Start_Hour>\n";
	if(this->endHour < r.nHoursPerDay){
		s+="	<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->endHour])+"</Interval_End_Hour>\n";
	}
	else{
		s+="	<Interval_End_Hour></Interval_End_Hour>\n";
		s+="	<!-- Interval_End_Hour void means the end of the day (which has no name) -->\n";
	}
	s+="	<Max_Days_Per_Week>"+QString::number(this->maxDaysPerWeek)+"</Max_Days_Per_Week>\n";
	s+="</ConstraintStudentsIntervalMaxDaysPerWeek>\n";
	return s;
}

QString ConstraintStudentsIntervalMaxDaysPerWeek::getDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Students interval max days per week");s+=", ";
	s+=(QObject::tr("WP:%1\%", "Abbreviation for weight percentage").arg(this->weightPercentage));s+=", ";
	//s+=(QObject::tr("S:%1", "Abbreviation for students").arg(this->students));s+=", ";
	s+=QObject::tr("ISH:%1", "Abbreviation for interval start hour").arg(r.hoursOfTheDay[this->startHour]);
	s+=", ";
	if(this->endHour<r.nHoursPerDay)
		s+=QObject::tr("IEH:%1", "Abbreviation for interval end hour").arg(r.hoursOfTheDay[this->endHour]);
	else
		s+=QObject::tr("IEH:%1", "Abbreviation for interval end hour").arg(QObject::tr("End of day"));
	s+=", ";
	s+=(QObject::tr("MD:%1", "Abbreviation for max days").arg(this->maxDaysPerWeek));

	return s;
}

QString ConstraintStudentsIntervalMaxDaysPerWeek::getDetailedDescription(Rules& r){
	Q_UNUSED(r);
	//if(&r!=NULL)
	//	;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Students interval max. days per week");s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Students set=%1").arg(this->students));s+="\n";
	s+=QObject::tr("Interval start hour=%1").arg(r.hoursOfTheDay[this->startHour]);s+="\n";

	if(this->endHour<r.nHoursPerDay)
		s+=QObject::tr("Interval end hour=%1").arg(r.hoursOfTheDay[this->endHour]);
	else
		s+=QObject::tr("Interval end hour=%1").arg(QObject::tr("End of day"));
	s+="\n";

	s+=(QObject::tr("Max. days per week=%1").arg(this->maxDaysPerWeek));s+="\n";

	return s;
}

double ConstraintStudentsIntervalMaxDaysPerWeek::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString *conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken;
	
	nbroken=0;
	
	for(int sbg=0; sbg<r.nInternalSubgroups; sbg++){
	//foreach(int sbg, this->iSubgroupsList){
		bool ocDay[MAX_DAYS_PER_WEEK];
		for(int d=0; d<r.nDaysPerWeek; d++){
			ocDay[d]=false;
			for(int h=startHour; h<endHour; h++){
				if(subgroupsMatrix[sbg][d][h]>0){
					ocDay[d]=true;
				}
			}
		}
		int nOcDays=0;
		for(int d=0; d<r.nDaysPerWeek; d++)
			if(ocDay[d])
				nOcDays++;
		if(nOcDays > this->maxDaysPerWeek){
			nbroken+=nOcDays-this->maxDaysPerWeek;

			if((nOcDays-this->maxDaysPerWeek)>0){
				QString s= QObject::tr("Time constraint students interval max days per week broken for subgroup: %1, allowed %2 days, required %3 days.")
				 .arg(r.internalSubgroupsList[sbg]->name)
				 .arg(this->maxDaysPerWeek)
				 .arg(nOcDays);
				s+=" ";
				s += QObject::tr("This increases the conflicts total by %1")
				 .arg((nOcDays-this->maxDaysPerWeek)*weightPercentage/100);
			 
				dl.append(s);
				cl.append((nOcDays-this->maxDaysPerWeek)*weightPercentage/100);
		
				*conflictsString += s+"\n";
			}
		}
		//cout<<"subgroup = "<<qPrintable(r.internalSubgroupsList[sbg]->name)<<", nOcDays = "<<nOcDays<<", max days per week = "<<this->maxDaysPerWeek<<endl;
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsIntervalMaxDaysPerWeek::isRelatedToActivity(Rules& r, Activity* a)
{
	Q_UNUSED(r);
	Q_UNUSED(a);
	//if(a)
	//	;

	return false;
}

bool ConstraintStudentsIntervalMaxDaysPerWeek::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	return false;
}

bool ConstraintStudentsIntervalMaxDaysPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintStudentsIntervalMaxDaysPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintStudentsIntervalMaxDaysPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivitiesEndStudentsDay::ConstraintActivitiesEndStudentsDay()
	: TimeConstraint()
{
	this->type = CONSTRAINT_ACTIVITIES_END_STUDENTS_DAY;
}

ConstraintActivitiesEndStudentsDay::ConstraintActivitiesEndStudentsDay(double wp, QString te,
	QString st, QString su, QString sut)
	: TimeConstraint(wp)
{
	this->teacherName=te;
	this->subjectName=su;
	this->activityTagName=sut;
	this->studentsName=st;
	this->type=CONSTRAINT_ACTIVITIES_END_STUDENTS_DAY;
}

bool ConstraintActivitiesEndStudentsDay::computeInternalStructure(Rules& r)
{
	//assert(this->teacherName!="" || this->studentsName!="" || this->subjectName!="" || this->subjectTagName!="");

	this->nActivities=0;

	QStringList::iterator it;
	Activity* act;
	int i;
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];

		//check if this activity has the corresponding teacher
		if(this->teacherName!=""){
			it = act->teachersNames.find(this->teacherName);
			if(it==act->teachersNames.end())
				continue;
		}
		//check if this activity has the corresponding students
		if(this->studentsName!=""){
			bool commonStudents=false;
			foreach(QString st, act->studentsNames)
				if(r.studentsSetsRelated(st, studentsName)){
					commonStudents=true;
					break;
				}
		
			/*it = act->studentsNames.find(this->studentsName);
			if(it==act->studentsNames.end())
				continue;*/
			if(!commonStudents)
				continue;
		}
		//check if this activity has the corresponding subject
		if(this->subjectName!="" && act->subjectName!=this->subjectName){
				continue;
		}
		//check if this activity has the corresponding activity tag
		if(this->activityTagName!="" && act->activityTagName!=this->activityTagName){
				continue;
		}
	
		assert(this->nActivities < MAX_ACTIVITIES);	
		this->activitiesIndices[this->nActivities++]=i;
	}

	if(this->nActivities>0)
		return true;
	else{
		QMessageBox::warning(NULL, QObject::tr("FET error in data"), 
			QObject ::tr("Following constraint is wrong (refers to no activities. Please correct it):\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}
}

bool ConstraintActivitiesEndStudentsDay::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintActivitiesEndStudentsDay::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s="<ConstraintActivitiesEndStudentsDay>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	//if(this->teacherName!="")
		s+="	<Teacher_Name>"+protect(this->teacherName)+"</Teacher_Name>\n";
	//if(this->studentsName!="")
		s+="	<Students_Name>"+protect(this->studentsName)+"</Students_Name>\n";
	//if(this->subjectName!="")
		s+="	<Subject_Name>"+protect(this->subjectName)+"</Subject_Name>\n";
	//if(this->subjectTagName!="")
		s+="	<Activity_Tag_Name>"+protect(this->activityTagName)+"</Activity_Tag_Name>\n";
	s+="</ConstraintActivitiesEndStudentsDay>\n";
	return s;
}

QString ConstraintActivitiesEndStudentsDay::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	Q_UNUSED(r);
	//if(&r==NULL)
	//	;

	QString s;
	s+=QObject::tr("Activities with ");
	if(this->teacherName!="")
		s+=QObject::tr("teacher=%1, ").arg(this->teacherName);
	else
		s+=QObject::tr("all teachers, ");
	if(this->studentsName!="")
		s+=QObject::tr("students=%1, ").arg(this->studentsName);
	else
		s+=QObject::tr("all students, ");
	if(this->subjectName!="")
		s+=QObject::tr("subject=%1, ").arg(this->subjectName);
	else
		s+=QObject::tr("all subjects, ");
	if(this->activityTagName!="")
		s+=QObject::tr("activity tag=%1, ").arg(this->activityTagName);
	else
		s+=QObject::tr("all activity tags, ");
	s+=QObject::tr("must end students' day");
	s+=", ";

	s+=(QObject::tr("WP:%1\%", "Abbreviation for Weight Percentage").arg(this->weightPercentage));
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintActivitiesEndStudentsDay::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Activities with:");s+="\n";

	if(this->teacherName!="")
		s+=QObject::tr("Teacher=%1\n").arg(this->teacherName);
	else
		s+=QObject::tr("All teachers\n");
	if(this->studentsName!="")
		s+=QObject::tr("Students=%1\n").arg(this->studentsName);
	else
		s+=QObject::tr("All students\n");
	if(this->subjectName!="")
		s+=QObject::tr("Subject=%1\n").arg(this->subjectName);
	else
		s+=QObject::tr("All subjects\n");
	if(this->activityTagName!="")
		s+=QObject::tr("Activity tag=%1\n").arg(this->activityTagName);
	else
		s+=QObject::tr("All activity tags\n");

	s+=QObject::tr("must end students' day");
	s+="\n";

	s+=(QObject::tr("Weight (percentage)=%1").arg(this->weightPercentage));s+="\n";

	return s;
}

double ConstraintActivitiesEndStudentsDay::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(!c.teachersMatrixReady || !c.subgroupsMatrixReady){
		c.teachersMatrixReady=true;
		c.subgroupsMatrixReady=true;
	//if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.changedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		//crt_chrom=&c;
		//crt_rules=&r;
		
		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	assert(r.internalStructureComputed);

	for(int kk=0; kk<this->nActivities; kk++){
		int tmp=0;
		int ai=this->activitiesIndices[kk];
	
		if(c.times[ai]!=UNALLOCATED_TIME){
			int d=c.times[ai]%r.nDaysPerWeek; //the day when this activity was scheduled
			int h=c.times[ai]/r.nDaysPerWeek; //the hour
		
			for(int j=0; j<r.internalActivitiesList[ai].iSubgroupsList.count(); j++){
				int sb=r.internalActivitiesList[ai].iSubgroupsList.at(j);
				for(int hh=h+r.internalActivitiesList[ai].duration; hh<r.nHoursPerDay; hh++)
					if(subgroupsMatrix[sb][d][hh]>0){
						nbroken++;
						tmp=1;
						break;
					}
				if(tmp>0)
					break;
			}

			if(conflictsString!=NULL && tmp>0){
				QString s=QObject::tr("Time constraint activities end students' day broken for activity with id=%1, increases conflicts total by %2")
				 .arg(r.internalActivitiesList[ai].id)
				 .arg(weightPercentage/100*tmp);

				dl.append(s);
				cl.append(weightPercentage/100*tmp);
	
				*conflictsString+= s+"\n";
			}
		}
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return nbroken * weightPercentage/100;
}

bool ConstraintActivitiesEndStudentsDay::isRelatedToActivity(Rules& r, Activity* a)
{
	QStringList::iterator it;

	//check if this activity has the corresponding teacher
	if(this->teacherName!=""){
		it = a->teachersNames.find(this->teacherName);
		if(it==a->teachersNames.end())
			return false;
	}
	//check if this activity has the corresponding students
	if(this->studentsName!=""){
		bool commonStudents=false;
		foreach(QString st, a->studentsNames){
			if(r.studentsSetsRelated(st, this->studentsName)){
				commonStudents=true;
				break;
			}
		}
		if(!commonStudents)
			return false;

		//it = a->studentsNames.find(this->studentsName);
		//if(it==a->studentsNames.end())
		//	return false;
	}
	//check if this activity has the corresponding subject
	if(this->subjectName!="" && a->subjectName!=this->subjectName)
		return false;
	//check if this activity has the corresponding activity tag
	if(this->activityTagName!="" && a->activityTagName!=this->activityTagName)
		return false;

	return true;
}

bool ConstraintActivitiesEndStudentsDay::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	//if(t)
	//	;

	return false;
}

bool ConstraintActivitiesEndStudentsDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintActivitiesEndStudentsDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);
	//if(s)
	//	;

	return false;
}

bool ConstraintActivitiesEndStudentsDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	/*if(s)
		;
	if(&r)
		;*/
		
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
