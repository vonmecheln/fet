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
#include "subjecttag.h"
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
static qint16 subgroupsMatrix[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
static qint16 teachersMatrix[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

static int teachers_conflicts=-1;
static int subgroups_conflicts=-1;

extern bool breakTime[MAX_HOURS_PER_WEEK];
extern bool breakDayHour[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

extern bool teacherNotAvailableTime[MAX_TEACHERS][MAX_HOURS_PER_WEEK];
extern bool teacherNotAvailableDayHour[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

extern bool subgroupNotAvailableTime[MAX_TOTAL_SUBGROUPS][MAX_HOURS_PER_WEEK];
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
	if(&r!=NULL)
		;
	/*do nothing*/
	return true;
}

QString ConstraintBasicCompulsoryTime::getXmlDescription(Rules& r)
{
	if(&r!=NULL)
		;

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
	if(&r!=NULL)
		;

	return QObject::tr("Basic compulsory constraints (time)") +
		" " + QObject::tr("WP:%1\%").arg(this->weightPercentage);
;
}

QString ConstraintBasicCompulsoryTime::getDetailedDescription(Rules& r)
{
	if(&r!=NULL)
		;

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

//critical function here - must be optimized for speed
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
					late += (h+dd-r.nHoursPerDay) * tmp * r.internalActivitiesList[i].nSubgroups;
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
					late += (h+dd-r.nHoursPerDay) * tmp * r.internalActivitiesList[i].nSubgroups;
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
						 .arg((h+dd-r.nHoursPerDay)*tmp*r.internalActivitiesList[i].nSubgroups*weightPercentage/100);
						s+="\n";
						
						dl.append(s);
						cl.append((h+dd-r.nHoursPerDay)*tmp*r.internalActivitiesList[i].nSubgroups*weightPercentage/100);

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

bool ConstraintBasicCompulsoryTime::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintBasicCompulsoryTime::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintBasicCompulsoryTime::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintBasicCompulsoryTime::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintBasicCompulsoryTime::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherNotAvailable::ConstraintTeacherNotAvailable()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHER_NOT_AVAILABLE;
}

ConstraintTeacherNotAvailable::ConstraintTeacherNotAvailable(double wp, const QString& tn, int day, int start_hour, int end_hour)
	: TimeConstraint(wp)
{
	this->teacherName=tn;
	this->d=day;
	this->h1=start_hour;
	this->h2=end_hour;
	this->type=CONSTRAINT_TEACHER_NOT_AVAILABLE;
}

QString ConstraintTeacherNotAvailable::getXmlDescription(Rules& r){
	QString s="<ConstraintTeacherNotAvailable>\n";
	s+="	<Weight_Percentage>"+QString::number(weightPercentage)+"</Weight_Percentage>\n";
	/*s+="	<Compulsory>";
	s+=yesNo(this->compulsory);
	s+="</Compulsory>\n";*/
	s+="	<Teacher_Name>"+protect(this->teacherName)+"</Teacher_Name>\n";
	s+="	<Day>"+protect(r.daysOfTheWeek[this->d])+"</Day>\n";
	s+="	<Start_Hour>"+protect(r.hoursOfTheDay[this->h1])+"</Start_Hour>\n";
	s+="	<End_Hour>"+protect(r.hoursOfTheDay[this->h2])+"</End_Hour>\n";
	s+="</ConstraintTeacherNotAvailable>\n";
	return s;
}

QString ConstraintTeacherNotAvailable::getDescription(Rules& r){
	QString s=QObject::tr("Teacher not available");s+=",";
	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
	s+=(QObject::tr("T:%1").arg(this->teacherName));s+=", ";
	s+=(QObject::tr("D:%1").arg(r.daysOfTheWeek[this->d]));s+=", ";
	s+=(QObject::tr("SH:%1").arg(r.hoursOfTheDay[this->h1]));s+=", ";
	s+=(QObject::tr("EH:%1").arg(r.hoursOfTheDay[this->h2]));

	return s;
}

QString ConstraintTeacherNotAvailable::getDetailedDescription(Rules& r){
	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Teacher not available");s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	s+=(QObject::tr("Teacher=%1").arg(this->teacherName));s+="\n";
	s+=(QObject::tr("Day=%1").arg(r.daysOfTheWeek[this->d]));s+="\n";
	s+=(QObject::tr("Start hour=%1").arg(r.hoursOfTheDay[this->h1]));s+="\n";
	s+=(QObject::tr("End hour=%1").arg(r.hoursOfTheDay[this->h2]));s+="\n";

	return s;
}

bool ConstraintTeacherNotAvailable::computeInternalStructure(Rules& r){
	this->teacher_ID=r.searchTeacher(this->teacherName);
	assert(this->teacher_ID>=0);
	return true;
}

//critical function here - must be optimized for speed
double ConstraintTeacherNotAvailable::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString *conflictsString)
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
	int i=this->teacher_ID;
	int j=this->d;
	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int k=h1; k<h2; k++)
			if(teachersMatrix[i][j][k]>0)
				//teachersMatrix[i][j][k]>=0 at anytime. Can I improve this if I get rid of the "if"?
				nbroken+=teachersMatrix[i][j][k];
	}
	//with logging
	else{
		nbroken=0;
		for(int k=h1; k<h2; k++)
			if(teachersMatrix[i][j][k]>0){
				if(conflictsString!=NULL){
					QString s=(QObject::tr("Time constraint teacher not available broken for teacher %1 on day %2, hour %3")
					 .arg(r.internalTeachersList[i]->name)
					 .arg(r.daysOfTheWeek[j])
					 .arg(r.hoursOfTheDay[k]));
					s += ". ";
					s += (QObject::tr("This increases the conflicts total by %1").arg(teachersMatrix[i][j][k]*weightPercentage/100));
				
					*conflictsString+= s+"\n";
					
					dl.append(s);
					cl.append(teachersMatrix[i][j][k]*weightPercentage/100);
				}
				nbroken+=teachersMatrix[i][j][k];
			}
	}

	if(weightPercentage==100.0)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherNotAvailable::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintTeacherNotAvailable::isRelatedToTeacher(Teacher* t)
{
	if(this->teacherName==t->name)
		return true;
	return false;
}

bool ConstraintTeacherNotAvailable::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintTeacherNotAvailable::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintTeacherNotAvailable::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetNotAvailable::ConstraintStudentsSetNotAvailable()
	: TimeConstraint()
{
	this->type=CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE;
}

ConstraintStudentsSetNotAvailable::ConstraintStudentsSetNotAvailable(double wp, const QString& sn, int day, int start_hour, int end_hour)
	 : TimeConstraint(wp){
	this->students = sn;
	this->d=day;
	this->h1=start_hour;
	this->h2=end_hour;
	this->type=CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE;
}

bool ConstraintStudentsSetNotAvailable::computeInternalStructure(Rules& r){
	StudentsSet* ss=r.searchStudentsSet(this->students);
	assert(ss);

	this->nSubgroups=0;
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
			if(r.internalSubgroupsList[tmp]->name == ss->name)
				break;
		assert(tmp<r.nInternalSubgroups);
		assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
		this->subgroups[this->nSubgroups++]=tmp;
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
				if(r.internalSubgroupsList[tmp]->name == sts->name)
					break;
			assert(tmp<r.nInternalSubgroups);
			assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
			this->subgroups[this->nSubgroups++]=tmp;
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
					if(r.internalSubgroupsList[tmp]->name == sts->name)
						break;
				assert(tmp<r.nInternalSubgroups);
				assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
				this->subgroups[this->nSubgroups++]=tmp;
			}
		}
	}
	else
		assert(0);
	return true;
}

QString ConstraintStudentsSetNotAvailable::getXmlDescription(Rules& r){
	QString s="<ConstraintStudentsSetNotAvailable>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Students>"+protect(this->students)+"</Students>\n";
	s+="	<Day>"+protect(r.daysOfTheWeek[this->d])+"</Day>\n";
	s+="	<Start_Hour>"+protect(r.hoursOfTheDay[this->h1])+"</Start_Hour>\n";
	s+="	<End_Hour>"+protect(r.hoursOfTheDay[this->h2])+"</End_Hour>\n";
	s+="</ConstraintStudentsSetNotAvailable>\n";
	return s;
}

QString ConstraintStudentsSetNotAvailable::getDescription(Rules& r){
	QString s;
	s=QObject::tr("Students set not available");s+=", ";
	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
	s+=(QObject::tr("S:%1").arg(this->students));s+=", ";
	s+=(QObject::tr("D:%1").arg(r.daysOfTheWeek[this->d]));s+=", ";
	s+=(QObject::tr("SH:%1").arg(r.hoursOfTheDay[this->h1]));s+=", ";
	s+=(QObject::tr("EH:%1").arg(r.hoursOfTheDay[this->h2]));

	return s;
}

QString ConstraintStudentsSetNotAvailable::getDetailedDescription(Rules& r){
	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Students set not available");s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	s+=(QObject::tr("Students=%1").arg(this->students));s+="\n";
	s+=(QObject::tr("Day=%1").arg(r.daysOfTheWeek[this->d]));s+="\n";
	s+=(QObject::tr("Start hour=%1").arg(r.hoursOfTheDay[this->h1]));s+="\n";
	s+=(QObject::tr("End hour=%1").arg(r.hoursOfTheDay[this->h2]));s+="\n";

	return s;
}

//critical function here - must be optimized for speed
double ConstraintStudentsSetNotAvailable::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString *conflictsString)
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

	//Calculates the number of hours when the students' set is supposed to be attend the course,
	//but it is not available
	//This function consideres all the hours, I mean if there are for example 5 weekly courses
	//scheduled on that hour (which is already a broken hard restriction - we only
	//are allowed 1 weekly course for a certain students' year at a certain hour) we calculate
	//5 broken restrictions for that function.
	//TODO: decide if it is better to consider only 2 or 10 as a return value in that case
	//(currently it is 10)
	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int m=0; m<this->nSubgroups; m++){
			int i=this->subgroups[m];
			int j=d;
			for(int k=h1; k<h2; k++)
				//subgroupsMatrix[i][j][k]>=0 at anytime. Can I improve this if I get rid of the "if"?
				if(subgroupsMatrix[i][j][k]>0)
					nbroken+=subgroupsMatrix[i][j][k];
		}
	}
	//with logging
	else{
		nbroken=0;
		for(int m=0; m<this->nSubgroups; m++){
			int i=this->subgroups[m], j=d;
			for(int k=h1; k<h2; k++)
				if(subgroupsMatrix[i][j][k]>0){
					if(conflictsString!=NULL){
						QString s= QObject::tr("Time constraint students not available");
						s += " ";
						s += (QObject::tr("broken for subgroup: %1 on day %2, hour %3")
						 .arg(r.internalSubgroupsList[i]->name)
						 .arg(r.daysOfTheWeek[j])
						 .arg(r.hoursOfTheDay[k]));
						s += ". ";
						s += (QObject::tr("This increases the conflicts total by %1")
						 .arg(subgroupsMatrix[i][j][k]*weightPercentage/100));
						 
						dl.append(s);
						cl.append(subgroupsMatrix[i][j][k]*weightPercentage/100);
					
						*conflictsString += s+"\n";
					}
					nbroken+=subgroupsMatrix[i][j][k];
				}
		}
	}

	if(weightPercentage==100.0)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsSetNotAvailable::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintStudentsSetNotAvailable::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintStudentsSetNotAvailable::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintStudentsSetNotAvailable::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintStudentsSetNotAvailable::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
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

QString ConstraintActivitiesSameStartingTime::getXmlDescription(Rules& r){
	if(&r!=NULL)
		;

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
	if(&r!=NULL)
		;

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
	if(&r!=NULL)
		;

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

//critical function here - must be optimized for speed
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

						nbroken+=tmp;

						if(tmp>0 && conflictsString!=NULL){
							QString s=QObject::tr("Time constraint activities same time broken, because activity with id=%1 is not at the same time with activity with id=%2")
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

bool ConstraintActivitiesSameStartingTime::isRelatedToActivity(Activity* a)
{
	for(int i=0; i<this->n_activities; i++)
		if(this->activitiesId[i]==a->id)
			return true;
	return false;
}

bool ConstraintActivitiesSameStartingTime::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintActivitiesSameStartingTime::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintActivitiesSameStartingTime::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintActivitiesSameStartingTime::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

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

QString ConstraintActivitiesNotOverlapping::getXmlDescription(Rules& r){
	if(&r!=NULL)
		;

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
	if(&r!=NULL)
		;

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
	if(&r!=NULL)
		;

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

//critical function here - must be optimized for speed
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

bool ConstraintActivitiesNotOverlapping::isRelatedToActivity(Activity* a)
{
	for(int i=0; i<this->n_activities; i++)
		if(this->activitiesId[i]==a->id)
			return true;
	return false;
}

bool ConstraintActivitiesNotOverlapping::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintActivitiesNotOverlapping::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintActivitiesNotOverlapping::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintActivitiesNotOverlapping::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

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

QString ConstraintMinNDaysBetweenActivities::getXmlDescription(Rules& r){
	if(&r!=NULL)
		;

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
	if(&r!=NULL)
		;

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
	if(&r!=NULL)
		;

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

//critical function here - must be optimized for speed
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
							s+=(QObject::tr("activity with id=%1 conflicts with activity with id=%2 on %3 hours")
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
							s+=", ";
							s+=(QObject::tr("teachers2 %1, students sets2 %2, subject2 %3")
							 .arg(tn2)
							 .arg(sn2)
							 .arg(r.internalActivitiesList[this->_activities[j]].subjectName));
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

bool ConstraintMinNDaysBetweenActivities::isRelatedToActivity(Activity* a)
{
	for(int i=0; i<this->n_activities; i++)
		if(this->activitiesId[i]==a->id)
			return true;
	return false;
}

bool ConstraintMinNDaysBetweenActivities::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintMinNDaysBetweenActivities::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintMinNDaysBetweenActivities::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintMinNDaysBetweenActivities::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

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
	if(&r!=NULL)
		;
	return true;
}

QString ConstraintTeachersMaxHoursDaily::getXmlDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s="<ConstraintTeachersMaxHoursDaily>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Maximum_Hours_Daily>"+QString::number(this->maxHoursDaily)+"</Maximum_Hours_Daily>\n";
	s+="</ConstraintTeachersMaxHoursDaily>\n";
	return s;
}

QString ConstraintTeachersMaxHoursDaily::getDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s;
	s+=(QObject::tr("Teachers max %1 hours daily").arg(this->maxHoursDaily));s+=", ";
	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));//s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintTeachersMaxHoursDaily::getDetailedDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=(QObject::tr("Teachers must not have more than %1 hours daily").arg(this->maxHoursDaily));s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

//critical function here - must be optimized for speed
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
						 "Time constraint teacher max %1 hours daily broken for teacher %2, on day %3.")
						 .arg(QString::number(this->maxHoursDaily))
						 .arg(r.internalTeachersList[i]->name)
						 .arg(r.daysOfTheWeek[d])
						 )
						 +
						 (QObject::tr(". This increases the conflicts total by %1").arg(QString::number(weightPercentage/100)));
						
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

bool ConstraintTeachersMaxHoursDaily::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintTeachersMaxHoursDaily::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return true;
}

bool ConstraintTeachersMaxHoursDaily::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintTeachersMaxHoursDaily::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintTeachersMaxHoursDaily::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

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

QString ConstraintTeacherMaxHoursDaily::getXmlDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s="<ConstraintTeacherMaxHoursDaily>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Teacher_Name>"+protect(this->teacherName)+"</Teacher_Name>\n";
	s+="	<Maximum_Hours_Daily>"+QString::number(this->maxHoursDaily)+"</Maximum_Hours_Daily>\n";
	s+="</ConstraintTeacherMaxHoursDaily>\n";
	return s;
}

QString ConstraintTeacherMaxHoursDaily::getDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s;
	s+=(QObject::tr("Teacher max %1 hours daily").arg(this->maxHoursDaily));s+=", ";
	s+=QObject::tr("TN:%1").arg(this->teacherName);s+=", ";
	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));//s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintTeacherMaxHoursDaily::getDetailedDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Teacher %1 must not have more than %2 hours daily").arg(this->teacherName).arg(this->maxHoursDaily);s+="\n";
	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

//critical function here - must be optimized for speed
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
					 "Time constraint teacher max %1 hours daily broken for teacher %2, on day %3.")
					 .arg(QString::number(this->maxHoursDaily))
					 .arg(r.internalTeachersList[i]->name)
					 .arg(r.daysOfTheWeek[d])
					 )
					 +
					 (QObject::tr(". This increases the conflicts total by %1").arg(QString::number(weightPercentage/100)));
						
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

bool ConstraintTeacherMaxHoursDaily::isRelatedToActivity(Activity* a)
{
	if(a)
		;

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
	if(s)
		;

	return false;
}

bool ConstraintTeacherMaxHoursDaily::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintTeacherMaxHoursDaily::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

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

QString ConstraintTeacherMaxDaysPerWeek::getXmlDescription(Rules& r)
{
	if(&r!=NULL)
		;

	QString s="<ConstraintTeacherMaxDaysPerWeek>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Teacher_Name>"+protect(this->teacherName)+"</Teacher_Name>\n";
	s+="	<Max_Days_Per_Week>"+QString::number(this->maxDaysPerWeek)+"</Max_Days_Per_Week>\n";
	s+="</ConstraintTeacherMaxDaysPerWeek>\n";
	return s;
}

QString ConstraintTeacherMaxDaysPerWeek::getDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s=QObject::tr("Teacher max days per week");s+=", ";
	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
	s+=(QObject::tr("T:%1").arg(this->teacherName));s+=", ";
	s+=(QObject::tr("MD:%1").arg(this->maxDaysPerWeek));

	return s;
}

QString ConstraintTeacherMaxDaysPerWeek::getDetailedDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Teacher max. days per week");s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	s+=(QObject::tr("Teacher=%1").arg(this->teacherName));s+="\n";
	s+=(QObject::tr("Max. days per week=%1").arg(this->maxDaysPerWeek));s+="\n";

	return s;
}

//critical function here - must be optimized for speed
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

bool ConstraintTeacherMaxDaysPerWeek::isRelatedToActivity(Activity* a)
{
	if(a)
		;

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
	if(s)
		;

	return false;
}

bool ConstraintTeacherMaxDaysPerWeek::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintTeacherMaxDaysPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

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
	if(&r!=NULL)
		;

	/*do nothing*/
	return true;
}

QString ConstraintTeachersMaxGapsPerWeek::getXmlDescription(Rules& r){
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s="<ConstraintTeachersMaxGapsPerWeek>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Max_Gaps>"+QString::number(this->maxGaps)+"</Max_Gaps>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="</ConstraintTeachersMaxGapsPerWeek>\n";
	return s;
}

QString ConstraintTeachersMaxGapsPerWeek::getDescription(Rules& r){
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s;
	s+=QObject::tr("Teachers max gaps per week");s+=", ";
	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);s+=", ";
	s+=QObject::tr("MG:%1").arg(this->maxGaps);
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintTeachersMaxGapsPerWeek::getDetailedDescription(Rules& r){
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Teachers max gaps per week");s+="\n";
	s+=QObject::tr("Max gaps per week:%1").arg(this->maxGaps); s+="\n";
	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

//critical function here - must be optimized for speed
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

	//if(weightPercentage==100)
	//	assert(totalGaps==0); for partial solutions this rule might be broken
	return weightPercentage/100 * totalGaps;
}

bool ConstraintTeachersMaxGapsPerWeek::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintTeachersMaxGapsPerWeek::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return true;
}

bool ConstraintTeachersMaxGapsPerWeek::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintTeachersMaxGapsPerWeek::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintTeachersMaxGapsPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

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

QString ConstraintTeacherMaxGapsPerWeek::getXmlDescription(Rules& r){
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

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
	if(&r==NULL)
		;

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
	if(&r==NULL)
		;

	QString s=QObject::tr("Time constraint"); s+="\n";
	s+=QObject::tr("Teacher max gaps per week"); s+="\n";
	s+=QObject::tr("Teacher: %1").arg(this->teacherName); s+="\n";
	s+=QObject::tr("Max gaps per week: %1").arg(this->maxGaps); s+="\n";
	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage); s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

//critical function here - must be optimized for speed
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

	//if(weightPercentage==100)
	//	assert(totalGaps==0); for partial solutions this rule might be broken
	return weightPercentage/100 * totalGaps;
}

bool ConstraintTeacherMaxGapsPerWeek::isRelatedToActivity(Activity* a)
{
	if(a)
		;

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
	if(s)
		;

	return false;
}

bool ConstraintTeacherMaxGapsPerWeek::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintTeacherMaxGapsPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintBreak::ConstraintBreak()
	: TimeConstraint()
{
	this->type = CONSTRAINT_BREAK;
}

ConstraintBreak::ConstraintBreak(double wp, int day, int start_hour, int end_hour)
	: TimeConstraint(wp)
{
	this->d = day;
	this->h1 = start_hour;
	this->h2 = end_hour;
	this->type = CONSTRAINT_BREAK;
}

QString ConstraintBreak::getXmlDescription(Rules& r){
	QString s="<ConstraintBreak>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Day>"+protect(r.daysOfTheWeek[this->d])+"</Day>\n";
	s+="	<Start_Hour>"+protect(r.hoursOfTheDay[this->h1])+"</Start_Hour>\n";
	s+="	<End_Hour>"+protect(r.hoursOfTheDay[this->h2])+"</End_Hour>\n";
	s+="</ConstraintBreak>\n";
	return s;
}

QString ConstraintBreak::getDescription(Rules& r){
	QString s;
	s+=QObject::tr("Break");s+=", ";
	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
	s+=(QObject::tr("D:%1").arg(r.daysOfTheWeek[this->d]));s+=", ";
	s+=(QObject::tr("SH:%1").arg(r.hoursOfTheDay[this->h1]));s+=", ";
	s+=(QObject::tr("EH:%1").arg(r.hoursOfTheDay[this->h2]));

	return s;
}

QString ConstraintBreak::getDetailedDescription(Rules& r){
	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Break");s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	s+=(QObject::tr("Day=%1").arg(r.daysOfTheWeek[this->d]));s+="\n";
	s+=(QObject::tr("Start hour=%1").arg(r.hoursOfTheDay[this->h1]));s+="\n";
	s+=(QObject::tr("End hour=%1").arg(r.hoursOfTheDay[this->h2]));s+="\n";

	return s;
}

bool ConstraintBreak::computeInternalStructure(Rules& r)
{
	if(&r!=NULL)
		;
		
	return true;
}

//critical function here - must be optimized for speed
double ConstraintBreak::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
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
	
	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		
		int begintimebreak=this->d+r.nDaysPerWeek*this->h1;
		int endtimebreak=this->d+r.nDaysPerWeek*this->h2;
		
		for(int i=0; i<r.nInternalActivities; i++){
			int dayact=c.times[i]%r.nDaysPerWeek;
			if(dayact==this->d){
				int begintimeact=c.times[i];
				int endtimeact=c.times[i]+r.nDaysPerWeek*r.internalActivitiesList[i].duration;
			
				if(!(endtimeact<=begintimebreak || begintimeact>=endtimebreak)){ //they intersect
					nbroken++;
				}
			}
		}
	}
	//with logging
	else{
		nbroken=0;
		
		int begintimebreak=this->d+r.nDaysPerWeek*this->h1;
		int endtimebreak=this->d+r.nDaysPerWeek*this->h2;
		
		for(int i=0; i<r.nInternalActivities; i++){
			int dayact=c.times[i]%r.nDaysPerWeek;
			if(dayact==this->d){
				int begintimeact=c.times[i];
				int endtimeact=c.times[i]+r.nDaysPerWeek*r.internalActivitiesList[i].duration;
			
				if(!(endtimeact<=begintimebreak || begintimeact>=endtimebreak)){ //they intersect
					/*cout<<"Activity with id=="<<r.internalActivitiesList[i].id<<", duration=="<<r.internalActivitiesList[i].duration
					 <<" is scheduled on day "<<qPrintable(r.daysOfTheWeek[begintimeact%r.nDaysPerWeek])<<", hour "<<
					 qPrintable(r.hoursOfTheDay[begintimeact/r.nDaysPerWeek])<<endl;*/
				
					nbroken++;

					if(conflictsString!=NULL){
						QString s=QObject::tr("Time constraint break not respected for activity with id %1, on day %2")
							.arg(r.internalActivitiesList[i].id)
							.arg(r.daysOfTheWeek[dayact]);
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
	}

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintBreak::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintBreak::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintBreak::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintBreak::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintBreak::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsNoGaps::ConstraintStudentsNoGaps()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_NO_GAPS;
}

ConstraintStudentsNoGaps::ConstraintStudentsNoGaps(double wp)
	: TimeConstraint(wp)
{
	this->type = CONSTRAINT_STUDENTS_NO_GAPS;
}

bool ConstraintStudentsNoGaps::computeInternalStructure(Rules& r)
{
	if(&r!=NULL)
		;

	/*do nothing*/
	return true;
}

QString ConstraintStudentsNoGaps::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s="<ConstraintStudentsNoGaps>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="</ConstraintStudentsNoGaps>\n";
	return s;
}

QString ConstraintStudentsNoGaps::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s;
	s+=QObject::tr("Students no gaps");s+=", ";
	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintStudentsNoGaps::getDetailedDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Students must not have gaps");s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	s+=QObject::tr("Please note that for each subgroup, you must have: 1. only no gaps or 2. no gaps and early, with the same weight percentage");
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

//critical function here - must be optimized for speed
double ConstraintStudentsNoGaps::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
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

	windows=0;
	for(i=0; i<r.nInternalSubgroups; i++)
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

					if(tmp>0 && conflictsString!=NULL){
						QString s=QObject::tr("Time constraint students no gaps broken for subgroup: %1, on day: %2, before hour: %3, lenght=%4, conflicts increase=%5")
						 .arg(r.internalSubgroupsList[i]->name)
						 .arg(r.daysOfTheWeek[j])
						 .arg(r.hoursOfTheDay[k])
						 .arg(tmp)
						 .arg(tmp*weightPercentage/100);
						 
						dl.append(s);
						cl.append(tmp*weightPercentage/100);
					
						*conflictsString+= s+"\n";
					}
					tmp=0;
				}
				else
					tmp++;
			}
		}

	//if(weightPercentage==100)    for partial solutions it might be broken
	//	assert(windows==0);		
	return weightPercentage/100 * windows;
}

bool ConstraintStudentsNoGaps::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintStudentsNoGaps::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintStudentsNoGaps::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintStudentsNoGaps::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintStudentsNoGaps::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetNoGaps::ConstraintStudentsSetNoGaps()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_SET_NO_GAPS;
}

ConstraintStudentsSetNoGaps::ConstraintStudentsSetNoGaps(double wp, const QString& st )
	: TimeConstraint(wp)
{
	this->type = CONSTRAINT_STUDENTS_SET_NO_GAPS;
	this->students = st;
}

bool ConstraintStudentsSetNoGaps::computeInternalStructure(Rules& r){
	StudentsSet* ss=r.searchStudentsSet(this->students);
	assert(ss);

	this->nSubgroups=0;
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
			if(r.internalSubgroupsList[tmp]->name == ss->name)
				break;
		assert(tmp<r.nInternalSubgroups);
		assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
		this->subgroups[this->nSubgroups++]=tmp;
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
				if(r.internalSubgroupsList[tmp]->name == sts->name)
					break;
			assert(tmp<r.nInternalSubgroups);
			assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
			this->subgroups[this->nSubgroups++]=tmp;
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
					if(r.internalSubgroupsList[tmp]->name == sts->name)
						break;
				assert(tmp<r.nInternalSubgroups);
				assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
				this->subgroups[this->nSubgroups++]=tmp;
			}
		}
	}
	else
		assert(0);
		
	return true;
}

QString ConstraintStudentsSetNoGaps::getXmlDescription(Rules& r){
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s="<ConstraintStudentsSetNoGaps>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Students>"; s+=protect(this->students); s+="</Students>\n";
	s+="</ConstraintStudentsSetNoGaps>\n";
	return s;
}

QString ConstraintStudentsSetNoGaps::getDescription(Rules& r){
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s;
	s+=QObject::tr("Students set no gaps"); s+=", ";
	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage)); s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory))); s+=", ";
	s+=(QObject::tr("St:%1").arg(this->students));

	return s;
}

QString ConstraintStudentsSetNoGaps::getDetailedDescription(Rules& r){
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Students set must not have gaps");s+="\n";
	s+=(QObject::tr("Weight (percentage)=%1").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	s+=QObject::tr("Students=%1").arg(this->students); s+="\n";
	s+=QObject::tr("Please note that for each subgroup, you must have: 1. only no gaps or 2. no gaps and early, with the same weight percentage");

	return s;
}

//critical function here - must be optimized for speed
double ConstraintStudentsSetNoGaps::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
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
	
	windows=0;
	for(int sg=0; sg<this->nSubgroups; sg++){
		int i=this->subgroups[sg];
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

					if(tmp>0 && conflictsString!=NULL){
						QString s=QObject::tr("Time constraint students set no gaps broken for subgroup: %1, on day: %2, before hour: %3, lenght=%4, conflicts increase=%5")
						 .arg(r.internalSubgroupsList[i]->name)
						 .arg(r.daysOfTheWeek[j])
						 .arg(r.hoursOfTheDay[k])
						 .arg(tmp)
						 .arg(weightPercentage/100*tmp);
						 
						dl.append(s);
						cl.append(weightPercentage/100*tmp);
					
						*conflictsString+= s+"\n";
					}
					tmp=0;
				}
				else
					tmp++;
			}
		}
	}

	//if(weightPercentage==100)     for partial solutions it might be broken
	//	assert(windows==0);
	return weightPercentage/100 * windows;
}

bool ConstraintStudentsSetNoGaps::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintStudentsSetNoGaps::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintStudentsSetNoGaps::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintStudentsSetNoGaps::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintStudentsSetNoGaps::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.studentsSetsRelated(this->students, s->name);
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsEarly::ConstraintStudentsEarly()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_EARLY;
}

ConstraintStudentsEarly::ConstraintStudentsEarly(double wp)
	: TimeConstraint(wp)
{
	this->type = CONSTRAINT_STUDENTS_EARLY;
}

bool ConstraintStudentsEarly::computeInternalStructure(Rules& r)
{
	if(&r!=NULL)
		;

	/*do nothing*/
	
	return true;
}

QString ConstraintStudentsEarly::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s="<ConstraintStudentsEarly>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="</ConstraintStudentsEarly>\n";
	return s;
}

QString ConstraintStudentsEarly::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s;
	s+=QObject::tr("Students must begin their courses as early as possible (permitted by breaks and students not available)");s+=", ";
	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));//s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintStudentsEarly::getDetailedDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Students must begin their courses as early as possible (permitted by breaks and students not available)");s+="\n";
	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";
	s+=QObject::tr("Please note that for each subgroup, you must have: 1. only no gaps or 2. no gaps and early, with the same weight percentage");
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

//critical function here - must be optimized for speed
double ConstraintStudentsEarly::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
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
	
	int free;
	int i;
	
	free=0; //number of free hours before starting the courses
	for(i=0; i<r.nInternalSubgroups; i++)
		for(int j=0; j<r.nDaysPerWeek; j++){
			int k;
			int weekly=0;
			for(k=0; k<r.nHoursPerDay && subgroupsMatrix[i][j][k]==0; k++)
				if(!breakDayHour[j][k] && !subgroupNotAvailableDayHour[i][j][k])
					weekly++;
			if(k<r.nHoursPerDay && weekly>0){ //this day is not empty
				free+=weekly;
				
				if(conflictsString!=NULL){
					QString s=QObject::tr("Constraint students early broken for subgroup %1, on day %2, increases conflicts total by %3")
					 .arg(r.internalSubgroupsList[i]->name)
					 .arg(r.daysOfTheWeek[j])
					 .arg(weekly*weightPercentage/100);
					 
					dl.append(s);
					cl.append(weekly*weightPercentage/100);
						
					*conflictsString+= s+"\n";
				}
			}
			//else
				//empty day or early assignment
		}

	//if(weightPercentage==100)    might be broken for partial solutions
	//	assert(free==0);
	return weightPercentage/100 * free;
}

bool ConstraintStudentsEarly::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintStudentsEarly::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintStudentsEarly::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintStudentsEarly::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintStudentsEarly::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetEarly::ConstraintStudentsSetEarly()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_SET_EARLY;
}

ConstraintStudentsSetEarly::ConstraintStudentsSetEarly(double wp, const QString& students)
	: TimeConstraint(wp)
{
	this->type = CONSTRAINT_STUDENTS_SET_EARLY;
	this->students=students;
}

bool ConstraintStudentsSetEarly::computeInternalStructure(Rules& r)
{
	StudentsSet* ss=r.searchStudentsSet(this->students);
	assert(ss);

	this->nSubgroups=0;
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
			if(r.internalSubgroupsList[tmp]->name == ss->name)
				break;
		assert(tmp<r.nInternalSubgroups);
		assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
		this->subgroups[this->nSubgroups++]=tmp;
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
				if(r.internalSubgroupsList[tmp]->name == sts->name)
					break;
			assert(tmp<r.nInternalSubgroups);
			assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
			this->subgroups[this->nSubgroups++]=tmp;
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
					if(r.internalSubgroupsList[tmp]->name == sts->name)
						break;
				assert(tmp<r.nInternalSubgroups);
				assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
				this->subgroups[this->nSubgroups++]=tmp;
			}
		}
	}
	else
		assert(0);
	return true;
}

QString ConstraintStudentsSetEarly::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s="<ConstraintStudentsSetEarly>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+="	<Students>"+protect(this->students)+"</Students>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="</ConstraintStudentsSetEarly>\n";
	return s;
}

QString ConstraintStudentsSetEarly::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s;
	s+=QObject::tr("Students set must begin their courses as early as possible (permitted by breaks and students not available)");s+=", ";
	s+=QObject::tr("S:%1").arg(this->students); s+=", ";
	s+=QObject::tr("WP:%1\%").arg(this->weightPercentage);//s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintStudentsSetEarly::getDetailedDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Students set must begin their courses as early as possible (permitted by breaks and students not available)");s+="\n";
	s+=QObject::tr("Students set=%1").arg(this->students); s+="\n";
	s+=QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage);s+="\n";
	s+=QObject::tr("Please note that for each subgroup, you must have: 1. only no gaps or 2. no gaps and early, with the same weight percentage");
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

//critical function here - must be optimized for speed
double ConstraintStudentsSetEarly::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
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
	
	int free;
	int i;
	
	free=0; //number of free hours before starting the courses
	for(int q=0; q<this->nSubgroups; q++){
	//for(i=0; i<r.nInternalSubgroups; i++)
		i=this->subgroups[q];
		for(int j=0; j<r.nDaysPerWeek; j++){
			int k;
			int weekly=0;
			for(k=0; k<r.nHoursPerDay && subgroupsMatrix[i][j][k]==0; k++)
				if(!breakDayHour[j][k] && !subgroupNotAvailableDayHour[i][j][k])
					weekly++;
			if(k<r.nHoursPerDay && weekly>0){ //this day is not empty
				free+=weekly;
				
				if(conflictsString!=NULL){
					QString s=QObject::tr("Constraint students early broken for subgroup %1, on day %2, increases conflicts total by %3")
					 .arg(r.internalSubgroupsList[i]->name)
					 .arg(r.daysOfTheWeek[j])
					 .arg(weekly*weightPercentage/100);
				 
					dl.append(s);
					cl.append(weekly*weightPercentage/100);
					
					*conflictsString+= s+"\n";
				}
			}
			//else
				//empty day or early assignment
		}
	}

	//if(weightPercentage==100)      might be broken for partial solutions
	//	assert(free==0);
	return weightPercentage/100 * free;
}

bool ConstraintStudentsSetEarly::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintStudentsSetEarly::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintStudentsSetEarly::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintStudentsSetEarly::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintStudentsSetEarly::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
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
	if(&r!=NULL)
		;

	/*do nothing*/
	
	return true;
}

QString ConstraintStudentsMaxHoursDaily::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

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
	if(&r==NULL)
		;

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
	if(&r==NULL)
		;

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

//critical function here - must be optimized for speed
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

	//without logging
	if(conflictsString==NULL){	
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
					too_much += tmp - this->maxHoursDaily;
				}
			}
	}
	//with logging
	else{
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
					too_much += tmp - this->maxHoursDaily;

					if(conflictsString!=NULL){
						QString s=QObject::tr("Time constraint students max hours daily broken for subgroup: %1, day: %2, lenght=%3, conflict increase=%4")
						 .arg(r.internalSubgroupsList[i]->name)
						 .arg(r.daysOfTheWeek[j])
						 .arg(QString::number(tmp))
						 .arg(weightPercentage/100*(tmp-this->maxHoursDaily));
						 
						dl.append(s);
						cl.append(weightPercentage/100*(tmp-this->maxHoursDaily));
					
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

bool ConstraintStudentsMaxHoursDaily::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintStudentsMaxHoursDaily::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintStudentsMaxHoursDaily::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintStudentsMaxHoursDaily::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintStudentsMaxHoursDaily::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

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

QString ConstraintStudentsSetMaxHoursDaily::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

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
	if(&r==NULL)
		;

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
	s+=(QObject::tr("St:%1").arg(this->students));

	return s;
}

QString ConstraintStudentsSetMaxHoursDaily::getDetailedDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

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
	StudentsSet* ss=r.searchStudentsSet(this->students);
	assert(ss);

	this->nSubgroups=0;
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
			if(r.internalSubgroupsList[tmp]->name == ss->name)
				break;
		assert(tmp<r.nInternalSubgroups);
		assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
		this->subgroups[this->nSubgroups++]=tmp;
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
				if(r.internalSubgroupsList[tmp]->name == sts->name)
					break;
			assert(tmp<r.nInternalSubgroups);
			assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
			this->subgroups[this->nSubgroups++]=tmp;
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
					if(r.internalSubgroupsList[tmp]->name == sts->name)
						break;
				assert(tmp<r.nInternalSubgroups);
				assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
				this->subgroups[this->nSubgroups++]=tmp;
			}
		}
	}
	else
		assert(0);
		
	return true;
}

//critical function here - must be optimized for speed
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

	//without logging
	if(conflictsString==NULL){
		too_much=0;
		//too_little=0;
		for(int sg=0; sg<this->nSubgroups; sg++){
			int i=subgroups[sg];
			for(int j=0; j<r.nDaysPerWeek; j++){
				tmp=0;
				for(int k=0; k<r.nHoursPerDay; k++){
					//Here we want to see if we have a weekly activity or a 2 weeks activity
					//We don't do tmp+=subgroupsMatrix[i][j][k] because we already counted this as a hard hitness
					if(subgroupsMatrix[i][j][k]>=1)
						tmp++;
				}
				if(this->maxHoursDaily>=0 && tmp > this->maxHoursDaily){ //we would like no more than max_hours_daily hours per day.
					too_much += tmp - this->maxHoursDaily;
				}
			}
		}
	}
	//with logging
	else{
		too_much=0;
		//too_little=0;
		for(int sg=0; sg<this->nSubgroups; sg++){
			int i=subgroups[sg];
			for(int j=0; j<r.nDaysPerWeek; j++){
				tmp=0;
				for(int k=0; k<r.nHoursPerDay; k++){
					//Here we want to see if we have a weekly activity or a 2 weeks activity
					//We don't do tmp+=subgroupsMatrix[i][j][k] because we already counted this as a hard hitness
					if(subgroupsMatrix[i][j][k]>=1)
						tmp++;
				}
				if(this->maxHoursDaily>=0 && tmp > this->maxHoursDaily){ //we would like no more than max_hours_daily hours per day.
					too_much += tmp - this->maxHoursDaily;

					if(conflictsString!=NULL){
						QString s=QObject::tr("Time constraint students set max hours daily broken for subgroup: %1, day: %2, lenght=%3, conflicts increase=%4")
						 .arg(r.internalSubgroupsList[i]->name)
						 .arg(r.daysOfTheWeek[j])
						 .arg(QString::number(tmp))
						 .arg((tmp-this->maxHoursDaily)*weightPercentage/100);
						 
						dl.append(s);
						cl.append((tmp-this->maxHoursDaily)*weightPercentage/100);
					
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

bool ConstraintStudentsSetMaxHoursDaily::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintStudentsSetMaxHoursDaily::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintStudentsSetMaxHoursDaily::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintStudentsSetMaxHoursDaily::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintStudentsSetMaxHoursDaily::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
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
	if(&r!=NULL)
		;

	/*do nothing*/
	
	return true;
}

QString ConstraintStudentsMinHoursDaily::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

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
	if(&r==NULL)
		;

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
	if(&r==NULL)
		;

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

	return s;
}

//critical function here - must be optimized for speed
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
			if(this->minHoursDaily>=0 && tmp < this->minHoursDaily){ //we would like no more than maxHoursDaily hours per day.
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

	assert(too_little>=0);
	//if(weightPercentage==100) does not work for partial solutions
	//	assert(too_little==0);
	return too_little * weightPercentage/100;
}

bool ConstraintStudentsMinHoursDaily::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintStudentsMinHoursDaily::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintStudentsMinHoursDaily::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintStudentsMinHoursDaily::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintStudentsMinHoursDaily::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

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

QString ConstraintStudentsSetMinHoursDaily::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

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
	if(&r==NULL)
		;

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
	s+=(QObject::tr("St:%1").arg(this->students));

	return s;
}

QString ConstraintStudentsSetMinHoursDaily::getDetailedDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

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

	return s;
}

bool ConstraintStudentsSetMinHoursDaily::computeInternalStructure(Rules &r)
{
	StudentsSet* ss=r.searchStudentsSet(this->students);
	assert(ss);

	this->nSubgroups=0;
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
			if(r.internalSubgroupsList[tmp]->name == ss->name)
				break;
		assert(tmp<r.nInternalSubgroups);
		assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
		this->subgroups[this->nSubgroups++]=tmp;
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
				if(r.internalSubgroupsList[tmp]->name == sts->name)
					break;
			assert(tmp<r.nInternalSubgroups);
			assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
			this->subgroups[this->nSubgroups++]=tmp;
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				for(tmp=0; tmp<=r.nInternalSubgroups; tmp++)
					if(r.internalSubgroupsList[tmp]->name == sts->name)
						break;
				assert(tmp<r.nInternalSubgroups);
				assert(this->nSubgroups<MAX_SUBGROUPS_PER_CONSTRAINT);
				this->subgroups[this->nSubgroups++]=tmp;
			}
		}
	}
	else
		assert(0);
		
	return true;
}

//critical function here - must be optimized for speed
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
	for(int sg=0; sg<this->nSubgroups; sg++){
		int i=subgroups[sg];
		for(int j=0; j<r.nDaysPerWeek; j++){
			tmp=0;
			for(int k=0; k<r.nHoursPerDay; k++){
				if(subgroupsMatrix[i][j][k]>=1)
					tmp++;
			}
			if(this->minHoursDaily>=0 && tmp < this->minHoursDaily){ //we would like no more than max_hours_daily hours per day.
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
	//if(weightPercentage==100) does not work for partial solutions
	//	assert(too_much==0);
	return too_little * weightPercentage;
}

bool ConstraintStudentsSetMinHoursDaily::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintStudentsSetMinHoursDaily::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintStudentsSetMinHoursDaily::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintStudentsSetMinHoursDaily::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintStudentsSetMinHoursDaily::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.studentsSetsRelated(this->students, s->name);
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivityPreferredTime::ConstraintActivityPreferredTime()
	: TimeConstraint()
{
	this->type = CONSTRAINT_ACTIVITY_PREFERRED_TIME;
}

ConstraintActivityPreferredTime::ConstraintActivityPreferredTime(double wp, int actId, int d, int h)
	: TimeConstraint(wp)
{
	this->activityId = actId;
	this->day = d;
	this->hour = h;
	this->type = CONSTRAINT_ACTIVITY_PREFERRED_TIME;
}

bool ConstraintActivityPreferredTime::operator==(ConstraintActivityPreferredTime& c){
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

bool ConstraintActivityPreferredTime::computeInternalStructure(Rules& r)
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
			QObject ::tr("Following constraint is wrong (because you need 2 or more activities. Please correct it):\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}

	this->activityIndex=i;	
	return true;
}

QString ConstraintActivityPreferredTime::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s="<ConstraintActivityPreferredTime>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Activity_Id>"+QString::number(this->activityId)+"</Activity_Id>\n";
	if(this->day>=0)
		s+="	<Preferred_Day>"+protect(r.daysOfTheWeek[this->day])+"</Preferred_Day>\n";
	if(this->hour>=0)
		s+="	<Preferred_Hour>"+protect(r.hoursOfTheDay[this->hour])+"</Preferred_Hour>\n";
	s+="</ConstraintActivityPreferredTime>\n";
	return s;
}

QString ConstraintActivityPreferredTime::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s;
	s+=(QObject::tr("Act. id:%1").arg(this->activityId));
	
	//* write the teachers, subject and students sets
	//added in version 4.1.4
	int ai;
	for(ai=0; ai<r.activitiesList.size(); ai++)
		if(r.activitiesList[ai]->id==this->activityId)
			break;
	if(ai==r.activitiesList.size())
		cout<<"this->activityId=="<<this->activityId<<endl;
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
	s+=QObject::tr("must be scheduled at: ");
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

QString ConstraintActivityPreferredTime::getDetailedDescription(Rules& r)
{
	QString s=QObject::tr("Time constraint");s+="\n";
	s+=(QObject::tr("Activity with id=%1").arg(this->activityId));
	
	//* write the teachers, subject and students sets
	//added in version 4.1.4
	int ai;
	for(ai=0; ai<r.activitiesList.size(); ai++)
		if(r.activitiesList[ai]->id==this->activityId)
			break;
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
	s+=QObject::tr("must be scheduled at: ");
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

//critical function here - must be optimized for speed
double ConstraintActivityPreferredTime::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
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

	if(conflictsString!=NULL && nbroken>0){
		QString s=QObject::tr("Time constraint activity preferred time broken for activity with id=%1, increases conflicts total by %2")
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

bool ConstraintActivityPreferredTime::isRelatedToActivity(Activity* a)
{
	if(this->activityId==a->id)
		return true;
	return false;
}

bool ConstraintActivityPreferredTime::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintActivityPreferredTime::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintActivityPreferredTime::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintActivityPreferredTime::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;
		
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivityPreferredTimes::ConstraintActivityPreferredTimes()
	: TimeConstraint()
{
	this->type = CONSTRAINT_ACTIVITY_PREFERRED_TIMES;
}

ConstraintActivityPreferredTimes::ConstraintActivityPreferredTimes(double wp, int actId, int nPT, int d[], int h[])
	: TimeConstraint(wp)
{
	this->activityId=actId;
	this->nPreferredTimes=nPT;
	assert(nPT<=MAX_N_CONSTRAINT_ACTIVITY_PREFERRED_TIMES);
	for(int i=0; i<nPT; i++){
		this->days[i]=d[i];
		this->hours[i]=h[i];
	}
	this->type=CONSTRAINT_ACTIVITY_PREFERRED_TIMES;
}

bool ConstraintActivityPreferredTimes::computeInternalStructure(Rules& r)
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
			QObject ::tr("Following constraint is wrong (because you need 2 or more activities. Please correct it):\n%1").arg(this->getDetailedDescription(r)));
		//assert(0);
		return false;
	}

	this->activityIndex=i;	
	return true;
}

QString ConstraintActivityPreferredTimes::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s="<ConstraintActivityPreferredTimes>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Activity_Id>"+QString::number(this->activityId)+"</Activity_Id>\n";
	s+="	<Number_of_Preferred_Times>"+QString::number(this->nPreferredTimes)+"</Number_of_Preferred_Times>\n";
	for(int i=0; i<nPreferredTimes; i++){
		s+="	<Preferred_Time>\n";
		if(this->days[i]>=0)
			s+="		<Preferred_Day>"+protect(r.daysOfTheWeek[this->days[i]])+"</Preferred_Day>\n";
		if(this->hours[i]>=0)
			s+="		<Preferred_Hour>"+protect(r.hoursOfTheDay[this->hours[i]])+"</Preferred_Hour>\n";
		s+="	</Preferred_Time>\n";
	}
	s+="</ConstraintActivityPreferredTimes>\n";
	return s;
}

QString ConstraintActivityPreferredTimes::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s;
	s+=(QObject::tr("Act. id:%1").arg(this->activityId));
	
	//* write the teachers, subject and students sets
	//added in version 4.1.4
	int ai;
	for(ai=0; ai<r.activitiesList.size(); ai++)
		if(r.activitiesList[ai]->id==this->activityId)
			break;
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
	s+=QObject::tr("must be scheduled at: ");
	for(int i=0; i<this->nPreferredTimes; i++){
		s+=QString::number(i+1);
		s+=":";
		if(this->days[i]>=0){
			s+=r.daysOfTheWeek[this->days[i]];
			s+=" ";
		}
		if(this->hours[i]>=0){
			s+=r.hoursOfTheDay[this->hours[i]];
		}
		s+=";";
	}

	s+=(QObject::tr("WP:%1").arg(this->weightPercentage));//s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintActivityPreferredTimes::getDetailedDescription(Rules& r)
{
	QString s=QObject::tr("Time constraint");s+="\n";
	s+=(QObject::tr("Activity with id=%1").arg(this->activityId));
	
	//* write the teachers, subject and students sets
	//added in version 4.1.4
	int ai;
	for(ai=0; ai<r.activitiesList.size(); ai++)
		if(r.activitiesList[ai]->id==this->activityId)
			break;
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
	s+=QObject::tr("must be scheduled at:\n");
	for(int i=0; i<this->nPreferredTimes; i++){
		s+=QString::number(i+1);
		s+=". ";
		if(this->days[i]>=0){
			s+=r.daysOfTheWeek[this->days[i]];
			s+=" ";
		}
		if(this->hours[i]>=0){
			s+=r.hoursOfTheDay[this->hours[i]];
		}
		s+="\n";
	}

	s+=(QObject::tr("Weight (percentage)=%1\%").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

//critical function here - must be optimized for speed
double ConstraintActivityPreferredTimes::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
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
		for(i=0; i<this->nPreferredTimes; i++){
			if(this->days[i]>=0 && this->days[i]!=d)
				continue;
			if(this->hours[i]>=0 && this->hours[i]!=h)
				continue;
			break;
		}
		if(i==this->nPreferredTimes){
			nbroken=1;
			/*if(r.internalActivitiesList[this->activityIndex].parity==PARITY_WEEKLY) //for weekly activities, double the conflicts
				nbroken*=2;*/
		}
	}

	if(conflictsString!=NULL && nbroken>0){
		QString s=QObject::tr("Time constraint activity preferred times broken for activity with id=%1, increases conflicts total by %2")
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

bool ConstraintActivityPreferredTimes::isRelatedToActivity(Activity* a)
{
	if(this->activityId==a->id)
		return true;
	return false;
}

bool ConstraintActivityPreferredTimes::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintActivityPreferredTimes::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintActivityPreferredTimes::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintActivityPreferredTimes::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;
		
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivitiesPreferredTimes::ConstraintActivitiesPreferredTimes()
	: TimeConstraint()
{
	this->type = CONSTRAINT_ACTIVITIES_PREFERRED_TIMES;
}

ConstraintActivitiesPreferredTimes::ConstraintActivitiesPreferredTimes(double wp, QString te,
	QString st, QString su, QString sut, int nPT, int d[], int h[])
	: TimeConstraint(wp)
{
	this->teacherName=te;
	this->subjectName=su;
	this->subjectTagName=sut;
	this->studentsName=st;
	this->nPreferredTimes=nPT;
	assert(nPT<=MAX_N_CONSTRAINT_ACTIVITIES_PREFERRED_TIMES);
	for(int i=0; i<nPT; i++){
		this->days[i]=d[i];
		this->hours[i]=h[i];
	}
	this->type=CONSTRAINT_ACTIVITIES_PREFERRED_TIMES;
}

bool ConstraintActivitiesPreferredTimes::computeInternalStructure(Rules& r)
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
		//check if this activity has the corresponding subject tag
		if(this->subjectTagName!="" && act->subjectTagName!=this->subjectTagName){
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

QString ConstraintActivitiesPreferredTimes::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s="<ConstraintActivitiesPreferredTimes>\n";
	s+="	<Weight_Percentage>"+QString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	//s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	//if(this->teacherName!="")
		s+="	<Teacher_Name>"+protect(this->teacherName)+"</Teacher_Name>\n";
	//if(this->studentsName!="")
		s+="	<Students_Name>"+protect(this->studentsName)+"</Students_Name>\n";
	//if(this->subjectName!="")
		s+="	<Subject_Name>"+protect(this->subjectName)+"</Subject_Name>\n";
	//if(this->subjectTagName!="")
		s+="	<Subject_Tag_Name>"+protect(this->subjectTagName)+"</Subject_Tag_Name>\n";
	s+="	<Number_of_Preferred_Times>"+QString::number(this->nPreferredTimes)+"</Number_of_Preferred_Times>\n";
	for(int i=0; i<nPreferredTimes; i++){
		s+="	<Preferred_Time>\n";
		if(this->days[i]>=0)
			s+="		<Preferred_Day>"+protect(r.daysOfTheWeek[this->days[i]])+"</Preferred_Day>\n";
		if(this->hours[i]>=0)
			s+="		<Preferred_Hour>"+protect(r.hoursOfTheDay[this->hours[i]])+"</Preferred_Hour>\n";
		s+="	</Preferred_Time>\n";
	}
	s+="</ConstraintActivitiesPreferredTimes>\n";
	return s;
}

QString ConstraintActivitiesPreferredTimes::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

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
	if(this->subjectTagName!="")
		s+=QObject::tr("subject tag=%1, ").arg(this->subjectTagName);
	else
		s+=QObject::tr("all subject tags, ");
	s+=QObject::tr("must be scheduled at: ");
	for(int i=0; i<this->nPreferredTimes; i++){
		s+=QString::number(i+1);
		s+=":";
		if(this->days[i]>=0){
			s+=r.daysOfTheWeek[this->days[i]];
			s+=" ";
		}
		if(this->hours[i]>=0){
			s+=r.hoursOfTheDay[this->hours[i]];
		}
		s+=";";
	}

	s+=(QObject::tr("WP:%1\%").arg(this->weightPercentage));s+=", ";
	//s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintActivitiesPreferredTimes::getDetailedDescription(Rules& r)
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
	if(this->subjectTagName!="")
		s+=QObject::tr("Subject tag=%1\n").arg(this->subjectTagName);
	else
		s+=QObject::tr("All subject tags\n");

	s+=QObject::tr("must be scheduled at:\n");
	for(int i=0; i<this->nPreferredTimes; i++){
		s+=QString::number(i+1);
		s+=". ";
		if(this->days[i]>=0){
			s+=r.daysOfTheWeek[this->days[i]];
			s+=" ";
		}
		if(this->hours[i]>=0){
			s+=r.hoursOfTheDay[this->hours[i]];
		}
		s+="\n";
	}

	s+=(QObject::tr("Weight (percentage)=%1").arg(this->weightPercentage));s+="\n";
	//s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

//critical function here - must be optimized for speed
double ConstraintActivitiesPreferredTimes::fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString)
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
				for(i=0; i<this->nPreferredTimes; i++){
					if(this->days[i]>=0 && this->days[i]!=d)
						continue;
					if(this->hours[i]>=0 && this->hours[i]!=h)
						continue;
					break;
				}
				if(i==this->nPreferredTimes){
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
				for(i=0; i<this->nPreferredTimes; i++){
					if(this->days[i]>=0 && this->days[i]!=d)
						continue;
					if(this->hours[i]>=0 && this->hours[i]!=h)
						continue;
					break;
				}
				if(i==this->nPreferredTimes){
					tmp=1;
					//if(r.internalActivitiesList[ai].parity==PARITY_WEEKLY) //for weekly activities, double the conflicts
					//	tmp=2;
				}
			}
			nbroken+=tmp;
			if(conflictsString!=NULL && tmp>0){
				QString s=QObject::tr("Time constraint activities preferred times broken"
				 " for activity with id=%1, "
				 " increases conflicts total by %2")
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
	return nbroken * weightPercentage;
}

bool ConstraintActivitiesPreferredTimes::isRelatedToActivity(Activity* a)
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
		it = a->studentsNames.find(this->studentsName);
		if(it==a->studentsNames.end())
			return false;
	}
	//check if this activity has the corresponding subject
	if(this->subjectName!="" && a->subjectName!=this->subjectName)
		return false;
	//check if this activity has the corresponding subject tag
	if(this->subjectTagName!="" && a->subjectTagName!=this->subjectTagName)
		return false;

	return true;
}

bool ConstraintActivitiesPreferredTimes::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintActivitiesPreferredTimes::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintActivitiesPreferredTimes::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintActivitiesPreferredTimes::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;
		
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

QString ConstraintActivitiesSameStartingHour::getXmlDescription(Rules& r){
	if(&r!=NULL)
		;

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
	if(&r!=NULL)
		;

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
	if(&r!=NULL)
		;

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

//critical function here - must be optimized for speed
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
							QString s=QObject::tr("Time constraint activities same hour broken, because activity with id=%1 is not at the same hour with activity with id=%2")
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

bool ConstraintActivitiesSameStartingHour::isRelatedToActivity(Activity* a)
{
	for(int i=0; i<this->n_activities; i++)
		if(this->activitiesId[i]==a->id)
			return true;
	return false;
}

bool ConstraintActivitiesSameStartingHour::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintActivitiesSameStartingHour::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintActivitiesSameStartingHour::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintActivitiesSameStartingHour::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;
		
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

QString Constraint2ActivitiesConsecutive::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

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
	if(&r==NULL)
		;

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
	s+=QObject::tr("Constraint 2 activities consecutive (second activity must be placed after the first"
	 " activity, possibly separated by breaks)"); s+="\n";
	
	s+=QObject::tr("First activity id=%1").arg(this->firstActivityId);

	//////////////////
	//* write the teachers, subject and students sets
	int ai;
	for(ai=0; ai<r.activitiesList.size(); ai++)
		if(r.activitiesList[ai]->id==this->firstActivityId)
			break;
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

//critical function here - must be optimized for speed
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
		
		cout<<"fd=="<<fd<<", fh=="<<fh<<", sd=="<<sd<<", sh=="<<sh<<endl;
		
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

bool Constraint2ActivitiesConsecutive::isRelatedToActivity(Activity* a)
{
	if(this->firstActivityId==a->id)
		return true;
	if(this->secondActivityId==a->id)
		return true;
	return false;
}

bool Constraint2ActivitiesConsecutive::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool Constraint2ActivitiesConsecutive::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool Constraint2ActivitiesConsecutive::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool Constraint2ActivitiesConsecutive::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;
		
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

