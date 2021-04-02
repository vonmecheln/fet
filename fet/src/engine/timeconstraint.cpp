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

#include "genetictimetable_defs.h"
#include "timeconstraint.h"
#include "rules.h"
#include "timechromosome.h"
#include "activity.h"
#include "teacher.h"
#include "subject.h"
#include "subjecttag.h"
#include "studentsset.h"

#include <qstring.h>

#include <iostream>
using namespace std;

#define yesNo(x)				((x)==0?"no":"yes")
#define yesNoTranslated(x)		((x)==0?QObject::tr("no"):QObject::tr("yes"))

#define minimu(x,y)	((x)<(y)?(x):(y))
#define maximu(x,y)	((x)>(y)?(x):(y))

static TimeChromosome* crt_chrom=NULL;
static Rules* crt_rules=NULL;

//The following 2 matrices are kept to make the computation faster
//They are calculated only at the beginning of the computation of the fitness
//of a certain chromosome. All the other fitness calculation functions assume these
//matrices keep the correct information for the current chromosome. This has to be improved
//in the near future, by some other assert functions or other (more intelligent) methods
//For the moment, we only keep the current chromosome's address for verification
static int16 subgroupsMatrix[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
static int16 teachersMatrix[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

//array used for ConstraintTeachersSubgroupsMaxHoursDaily
static int16 teachersSubgroupsDays[MAX_TEACHERS][MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK];

//array used for ConstraintTeachersSubjectTagsMaxHoursContinuously
//and ConstraintTeachersSubjectTagMaxHoursContinuously
static int16 teachersSubjectTags[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

static int teachers_conflicts=-1;
static int subgroups_conflicts=-1;

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

TimeConstraint::TimeConstraint()
{
	type=CONSTRAINT_GENERIC_TIME;
}

TimeConstraint::~TimeConstraint()
{
}

TimeConstraint::TimeConstraint(double w, bool c)
{
	weight=w;
	assert(w<=100.0);
	compulsory=c;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

ConstraintBasicCompulsoryTime::ConstraintBasicCompulsoryTime(): TimeConstraint()
{
	this->type=CONSTRAINT_BASIC_COMPULSORY_TIME;
	this->compulsory=true;
}

ConstraintBasicCompulsoryTime::ConstraintBasicCompulsoryTime(double w): TimeConstraint(w, true)
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
	s += "	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";
	s+=yesNo(this->compulsory);
	s+="</Compulsory>\n";
	s += "</ConstraintBasicCompulsoryTime>\n";
	return s;
}

QString ConstraintBasicCompulsoryTime::getDescription(Rules& r)
{
	if(&r!=NULL)
		;

	return QObject::tr("Basic compulsory constraints (time)") +
		" " + QObject::tr("W:%1").arg(this->weight) +
		" " + QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory));
;
}

QString ConstraintBasicCompulsoryTime::getDetailedDescription(Rules& r)
{
	if(&r!=NULL)
		;

	QString s=QObject::tr("These are the basic compulsory constraints\n"
		"(referring to time allocation) for any timetable\n");

	s+=QObject::tr("Weight=%1").arg(this->weight);s+="\n";
	s+=QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory));s+="\n";
	s+=QObject::tr("The basic time constraints try to avoid:\n");
	s+=QObject::tr("- unallocated activities\n");
	s+=QObject::tr("- activities scheduled too late\n");
	s+=QObject::tr("- teachers assigned to more than one activity simultaneously\n");
	s+=QObject::tr("- students assigned to more than one activity simultaneously\n");

	return s;
}

//critical function here - must be optimized for speed
int ConstraintBasicCompulsoryTime::fitness(TimeChromosome& c, Rules& r, QString* conflictsString){
	assert(r.internalStructureComputed);

	int teachersConflicts, subgroupsConflicts;

	//This constraint fitness calculation routine is called firstly,
	//so we can compute the teacher and subgroups conflicts faster this way.
	if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.timeChangedForMatrixCalculation){
		subgroupsConflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachersConflicts = c.getTeachersMatrix(r, teachersMatrix);

		crt_chrom = &c;
		crt_rules = &r;
		
		c.timeChangedForMatrixCalculation=false;
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
					if(r.internalActivitiesList[i].parity==PARITY_WEEKLY)
						tmp=2;
					else{
						assert(r.internalActivitiesList[i].parity==PARITY_FORTNIGHTLY);
						tmp=1;
					}
					late += (h+dd-r.nHoursPerDay) * tmp * r.internalActivitiesList[i].nSubgroups;
					//multiplied with 2 for weekly activities and with the number
					//of subgroups implied, for seeing the importance of the
					//activity
				}
			}
		}

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
					(*conflictsString) += QObject::tr("Time constraint basic compulsory");
					(*conflictsString) += ": ";
					(*conflictsString) += QObject::tr("unallocated activity with id=%1").arg(r.internalActivitiesList[i].id);
					(*conflictsString) += QObject::tr(" - this increases the conflicts total by %1")
						.arg(weight * /*r.internalActivitiesList[i].duration*r.internalActivitiesList[i].nSubgroups * */10000);
					(*conflictsString) += "\n";
				}
			}
			else{
				//Calculates the number of activities that are scheduled too late (in fact we
				//calculate a function that increases as the activity is getting late)
				int h=c.times[i]/r.nDaysPerWeek;
				dd=r.internalActivitiesList[i].duration;
				if(h+dd>r.nHoursPerDay){
					int tmp;
					if(r.internalActivitiesList[i].parity==PARITY_WEEKLY)
						tmp=2;
					else{
						assert(r.internalActivitiesList[i].parity==PARITY_FORTNIGHTLY);
						tmp=1;
					}
					late += (h+dd-r.nHoursPerDay) * tmp * r.internalActivitiesList[i].nSubgroups;
					//multiplied with 2 for weekly activities and with the number
					//of subgroups implied, for seeing the importance of the
					//activity

					if(conflictsString!=NULL){
						(*conflictsString)+=QObject::tr("Time constraint basic compulsory");
						(*conflictsString)+=": ";
						(*conflictsString)+=QObject::tr("activity with id=%1 is late.")
							.arg(r.internalActivitiesList[i].id);
						(*conflictsString)+=" ";
						(*conflictsString)+=QObject::tr("This increases the conflicts total by %1")
							.arg((h+dd-r.nHoursPerDay)*tmp*r.internalActivitiesList[i].nSubgroups*weight);
						(*conflictsString)+="\n";
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
					int tmp=teachersMatrix[i][j][k]-2;
					if(tmp>0){
						if(conflictsString!=NULL){
							(*conflictsString)+=QObject::tr("Time constraint basic compulsory");
							(*conflictsString)+=": ";
							(*conflictsString)+=QObject::tr("teacher with name %1 has more than one allocated activity on day %2, hour %3")
								.arg(r.internalTeachersList[i]->name)
								.arg(r.daysOfTheWeek[j])
								.arg(r.hoursOfTheDay[k]);
							(*conflictsString)+=". ";
							(*conflictsString)+=QObject::tr("This increases the conflicts total by %1")
								.arg(tmp*weight);
							(*conflictsString)+="\n";
						}
						nte+=tmp;
					}
				}

		//Calculates the number of subgroups exhaustion (a subgroup cannot attend two
		//activities at the same time)
		nse=0;
		for(i=0; i<r.nInternalSubgroups; i++)
			for(int j=0; j<r.nDaysPerWeek; j++)
				for(int k=0; k<r.nHoursPerDay; k++){
					int tmp=subgroupsMatrix[i][j][k]-2;
					if(tmp>0){
						if(conflictsString!=NULL){
							*conflictsString+=QObject::tr("Time constraint basic compulsory");
							*conflictsString+=": ";
							*conflictsString+=QObject::tr("subgroup %1 has more than one allocated activity on day %2, hour %3")
								.arg(r.internalSubgroupsList[i]->name)
								.arg(r.daysOfTheWeek[j])
								.arg(r.hoursOfTheDay[k]);
							*conflictsString+=". ";
							*conflictsString+=QObject::tr("This increases the conflicts total by %1")
								.arg((subgroupsMatrix[i][j][k]-2)*weight);
							*conflictsString+="\n";
						}
						nse += tmp;
					}
				}
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

	return int (ceil ( weight * (unallocated + late + nte + nse) ) ); //conflicts factor
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

ConstraintTeacherNotAvailable::ConstraintTeacherNotAvailable(double w, bool c, const QString& tn, int day, int start_hour, int end_hour)
	: TimeConstraint(w, c)
{
	this->teacherName=tn;
	this->d=day;
	this->h1=start_hour;
	this->h2=end_hour;
	this->type=CONSTRAINT_TEACHER_NOT_AVAILABLE;
}

QString ConstraintTeacherNotAvailable::getXmlDescription(Rules& r){
	QString s="<ConstraintTeacherNotAvailable>\n";
	s+="	<Weight>"+QString::number(weight)+"</Weight>\n";
	s+="	<Compulsory>";
	s+=yesNo(this->compulsory);
	s+="</Compulsory>\n";
	s+="	<Teacher_Name>"+protect(this->teacherName)+"</Teacher_Name>\n";
	s+="	<Day>"+protect(r.daysOfTheWeek[this->d])+"</Day>\n";
	s+="	<Start_Hour>"+protect(r.hoursOfTheDay[this->h1])+"</Start_Hour>\n";
	s+="	<End_Hour>"+protect(r.hoursOfTheDay[this->h2])+"</End_Hour>\n";
	s+="</ConstraintTeacherNotAvailable>\n";
	return s;
}

QString ConstraintTeacherNotAvailable::getDescription(Rules& r){
	QString s=QObject::tr("Teacher not available");s+=",";
	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
	s+=(QObject::tr("T:%1").arg(this->teacherName));s+=", ";
	s+=(QObject::tr("D:%1").arg(r.daysOfTheWeek[this->d]));s+=", ";
	s+=(QObject::tr("SH:%1").arg(r.hoursOfTheDay[this->h1]));s+=", ";
	s+=(QObject::tr("EH:%1").arg(r.hoursOfTheDay[this->h2]));

	return s;
}

QString ConstraintTeacherNotAvailable::getDetailedDescription(Rules& r){
	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Teacher not available");s+="\n";
	s+=(QObject::tr("Weight=%1").arg(this->weight));s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
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
int ConstraintTeacherNotAvailable::fitness(TimeChromosome& c, Rules& r, QString *conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.timeChangedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		crt_chrom=&c;
		crt_rules=&r;
		
		c.timeChangedForMatrixCalculation=false;
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
					*conflictsString+=
						(QObject::tr("Time constraint teacher not available broken for teacher %1 on day %2, hour %3")
						.arg(r.internalTeachersList[i]->name)
						.arg(r.daysOfTheWeek[j])
						.arg(r.hoursOfTheDay[k]));
					*conflictsString += ". ";
					*conflictsString += (QObject::tr("This increases the conflicts total by %1").arg(teachersMatrix[i][j][k]*weight));
					*conflictsString += "\n";
				}
				nbroken+=teachersMatrix[i][j][k];
			}
	}

	return int (ceil ( weight * nbroken ) );
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

ConstraintStudentsSetNotAvailable::ConstraintStudentsSetNotAvailable(double w, bool c, const QString& sn, int day, int start_hour, int end_hour)
	 : TimeConstraint(w, c){
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
	s+="	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
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
	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
	s+=(QObject::tr("S:%1").arg(this->students));s+=", ";
	s+=(QObject::tr("D:%1").arg(r.daysOfTheWeek[this->d]));s+=", ";
	s+=(QObject::tr("SH:%1").arg(r.hoursOfTheDay[this->h1]));s+=", ";
	s+=(QObject::tr("EH:%1").arg(r.hoursOfTheDay[this->h2]));

	return s;
}

QString ConstraintStudentsSetNotAvailable::getDetailedDescription(Rules& r){
	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Students set not available");s+="\n";
	s+=(QObject::tr("Weight=%1").arg(this->weight));s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	s+=(QObject::tr("Students=%1").arg(this->students));s+="\n";
	s+=(QObject::tr("Day=%1").arg(r.daysOfTheWeek[this->d]));s+="\n";
	s+=(QObject::tr("Start hour=%1").arg(r.hoursOfTheDay[this->h1]));s+="\n";
	s+=(QObject::tr("End hour=%1").arg(r.hoursOfTheDay[this->h2]));s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int ConstraintStudentsSetNotAvailable::fitness(TimeChromosome& c, Rules& r, QString *conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.timeChangedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		crt_chrom=&c;
		crt_rules=&r;
		
		c.timeChangedForMatrixCalculation=false;
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
			int i=this->subgroups[m], j=d;
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
						*conflictsString += QObject::tr("Time constraint students not available");
						*conflictsString += " ";
						*conflictsString += (QObject::tr("broken for subgroup: %1 on day %2, hour %3")
							.arg(r.internalSubgroupsList[i]->name)
							.arg(r.daysOfTheWeek[j])
							.arg(r.hoursOfTheDay[k]));
						*conflictsString += ". ";
						*conflictsString += (QObject::tr("This increases the conflicts total by %1")
							.arg(subgroupsMatrix[i][j][k]*weight));
						*conflictsString += "\n";
					}
					nbroken+=subgroupsMatrix[i][j][k];
				}
		}
	}

	return int (ceil ( weight * nbroken ) );
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

ConstraintActivitiesSameStartingTime::ConstraintActivitiesSameStartingTime(double w, bool c, int nact, const int act[])
 : TimeConstraint(w, c)
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
	
	if(this->_n_activities<=1)
		return false;

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
	s+="	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
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
	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
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
	s+=(QObject::tr("Weight=%1").arg(this->weight));s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	s+=(QObject::tr("Number of activities=%1").arg(this->n_activities));s+="\n";
	for(int i=0; i<this->n_activities; i++){
		s+=(QObject::tr("Activity with id=%1").arg(this->activitiesId[i]));s+="\n";
	}

	return s;
}

//critical function here - must be optimized for speed
int ConstraintActivitiesSameStartingTime::fitness(TimeChromosome& c, Rules& r, QString* conflictsString)
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
						if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY &&
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 4 * (abs(day1-day2) + abs(hour1-hour2));
						else if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY ||
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 2 * (abs(day1-day2) + abs(hour1-hour2));
						else
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
						if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY &&
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 4 * (abs(day1-day2) + abs(hour1-hour2));
						else if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY ||
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 2 * (abs(day1-day2) + abs(hour1-hour2));
						else
							tmp = abs(day1-day2) + abs(hour1-hour2);

						nbroken+=tmp;

						if(tmp>0 && conflictsString!=NULL){
							*conflictsString+=(QObject::tr("Time constraint activities same time broken, because activity with id=%1 is not at the same time with activity with id=%2")
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

ConstraintActivitiesNotOverlapping::ConstraintActivitiesNotOverlapping(double w, bool c, int nact, const int act[])
 : TimeConstraint(w, c)
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
	
	if(this->_n_activities<=1)
		return false;

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
	s+="	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
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
	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
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
	s+=(QObject::tr("Weight=%1").arg(this->weight));s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	s+=(QObject::tr("Number of activities=%1").arg(this->n_activities));s+="\n";
	for(int i=0; i<this->n_activities; i++){
		s+=(QObject::tr("Activity with id=%1").arg(this->activitiesId[i]));s+="\n";
	}

	return s;
}

//critical function here - must be optimized for speed
int ConstraintActivitiesNotOverlapping::fitness(TimeChromosome& c, Rules& r, QString* conflictsString)
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
						if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY &&
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tt = 4 * tt;
						else if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY ||
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tt = 2 * tt;
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
						if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY &&
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 4 * tmp;
						else if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY ||
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 2 * tmp;
						/*else
							tmp = tmp;*/

						nbroken+=tmp;

						if(tt>0 && conflictsString!=NULL){
							*conflictsString+=QObject::tr("Time constraint activities not overlapping");
							*conflictsString+=" ";
							*conflictsString+=QObject::tr("broken:");
							*conflictsString+=" ";
							*conflictsString+=(QObject::tr("activity with id=%1 overlaps with activity with id=%2 on a number of %3 periods")
								.arg(this->activitiesId[i])
								.arg(this->activitiesId[j])
								.arg(tt));
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

ConstraintMinNDaysBetweenActivities::ConstraintMinNDaysBetweenActivities(double w, bool c, int nact, const int act[], int n)
 : TimeConstraint(w, c)
 {
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
	if(this->compulsory!=c.compulsory)
		return false;
	if(this->weight!=c.weight)
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
	
	if(this->_n_activities<=1)
		return false;

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
	s+="	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
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
	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
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
	s+=(QObject::tr("Weight=%1").arg(this->weight));s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	s+=(QObject::tr("Number of activities=%1").arg(this->n_activities));s+="\n";
	for(int i=0; i<this->n_activities; i++){
		s+=(QObject::tr("Activity with id=%1").arg(this->activitiesId[i]));s+="\n";
	}
	s+=(QObject::tr("Minimum number of days=%1").arg(this->minDays));s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int ConstraintMinNDaysBetweenActivities::fitness(TimeChromosome& c, Rules& r, QString* conflictsString)
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
				int duration1=r.internalActivitiesList[this->_activities[i]].duration;

				for(int j=0; j<i; j++){
					int t2=c.times[this->_activities[j]];
					if(t2!=UNALLOCATED_TIME){
						int day2=t2%r.nDaysPerWeek;
						int duration2=r.internalActivitiesList[this->_activities[j]].duration;
					
						int tmp;
						int tt=0;
						int dist=abs(day1-day2);
						if(dist<minDays)
							tt=minDays-dist;
						
						//conflicts increase as the activities are longer
						tt*=duration1*duration2;
					
						tmp=tt;
	
						//activity weekly - counts as double
						if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY &&
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 4 * tmp;
						else if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY ||
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 2 * tmp;
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
				int duration1=r.internalActivitiesList[this->_activities[i]].duration;

				for(int j=0; j<i; j++){
					int t2=c.times[this->_activities[j]];
					if(t2!=UNALLOCATED_TIME){
						int day2=t2%r.nDaysPerWeek;
						int duration2=r.internalActivitiesList[this->_activities[j]].duration;
					
						int tmp;
						int tt=0;
						int dist=abs(day1-day2);
						if(dist<minDays)
							tt=minDays-dist;
						
						//conflicts increase as the activities are longer
						tt*=duration1*duration2;
					
						tmp=tt;
	
						//activity weekly - counts as double
						if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY &&
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 4 * tmp;
						else if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY ||
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 2 * tmp;
						/*else
							tmp = tmp;*/

						nbroken+=tmp;

						if(tt>0 && conflictsString!=NULL){
							*conflictsString+=QObject::tr("Time constraint min n days between activities");
							*conflictsString+=" ";
							*conflictsString+=QObject::tr("broken:");
							*conflictsString+=" ";
							*conflictsString+=(QObject::tr("activity with id=%1 conflicts with activity with id=%2 on %3 hours")
								.arg(this->activitiesId[i])
								.arg(this->activitiesId[j])
								.arg(tt));
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

ConstraintTeachersMaxHoursContinuously::ConstraintTeachersMaxHoursContinuously()
	: TimeConstraint()
{
	type=CONSTRAINT_TEACHERS_MAX_HOURS_CONTINUOUSLY;
}

ConstraintTeachersMaxHoursContinuously::ConstraintTeachersMaxHoursContinuously(double w, bool c, int maxhours)
 : TimeConstraint(w, c)
 {
	assert(maxhours>0);
	this->maxHoursContinuously=maxhours;

	this->type=CONSTRAINT_TEACHERS_MAX_HOURS_CONTINUOUSLY;
}

bool ConstraintTeachersMaxHoursContinuously::computeInternalStructure(Rules& r)
{
	if(&r!=NULL)
		;
	return true;
}

QString ConstraintTeachersMaxHoursContinuously::getXmlDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s="<ConstraintTeachersMaxHoursContinuously>\n";
	s+="	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Maximum_Hours_Continuously>"+QString::number(this->maxHoursContinuously)+"</Maximum_Hours_Continuously>\n";
	s+="</ConstraintTeachersMaxHoursContinuously>\n";
	return s;
}

QString ConstraintTeachersMaxHoursContinuously::getDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s;
	s+=(QObject::tr("Teachers max %1 hours continuously").arg(this->maxHoursContinuously));s+=", ";
	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintTeachersMaxHoursContinuously::getDetailedDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=(QObject::tr("Teachers must not have more than %1 hours continuously").arg(this->maxHoursContinuously));s+="\n";
	s+=(QObject::tr("Weight=%1").arg(this->weight));s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int ConstraintTeachersMaxHoursContinuously::fitness(TimeChromosome& c, Rules& r, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.timeChangedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		crt_chrom=&c;
		crt_rules=&r;
		
		c.timeChangedForMatrixCalculation=false;
	}

	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int i=0; i<r.nInternalTeachers; i++){
			for(int d=0; d<r.nDaysPerWeek; d++){
				int n_cont_hours=0;
				for(int h=0; h<r.nHoursPerDay; h++){
					if(teachersMatrix[i][d][h]>0){
						n_cont_hours++;
						if(n_cont_hours>this->maxHoursContinuously)
							nbroken+=2; //Fortnightly activities are treated here as weekly ones, for the sake of simplicity
					}
					else
						n_cont_hours=0;
				}
			}
		}
	}
	//with logging
	else{
		nbroken=0;
		for(int i=0; i<r.nInternalTeachers; i++){
			for(int d=0; d<r.nDaysPerWeek; d++){
				int n_cont_hours=0;
				for(int h=0; h<r.nHoursPerDay; h++){
					if(teachersMatrix[i][d][h]>0){
						n_cont_hours++;
						if(n_cont_hours>this->maxHoursContinuously){
							nbroken+=2; //Fortnightly activities are treated here as weekly ones, for the sake of simplicity

							if(conflictsString!=NULL){
								(*conflictsString)+=(QObject::tr(
									"Time constraint teacher no more than %1 hours continuously broken for teacher %2, on day %3, hour %4")
									.arg(QString::number(this->maxHoursContinuously))
									.arg(r.internalTeachersList[i]->name)
									.arg(r.daysOfTheWeek[d])
									.arg(QString::number(h))
									)
									+
								 	(QObject::tr(". This increases the conflicts total by %1").arg(QString::number(2*weight)))
									+ "\n";
							}
						}
					}
					else{
						n_cont_hours=0;
					}
				}
			}
		}
	}

	return int (ceil ( weight * nbroken ) );
}

bool ConstraintTeachersMaxHoursContinuously::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintTeachersMaxHoursContinuously::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return true;
}

bool ConstraintTeachersMaxHoursContinuously::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintTeachersMaxHoursContinuously::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintTeachersMaxHoursContinuously::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
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

ConstraintTeachersMaxHoursDaily::ConstraintTeachersMaxHoursDaily(double w, bool c, int maxhours)
 : TimeConstraint(w, c)
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
	s+="	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Maximum_Hours_Daily>"+QString::number(this->maxHoursDaily)+"</Maximum_Hours_Daily>\n";
	s+="</ConstraintTeachersMaxHoursDaily>\n";
	return s;
}

QString ConstraintTeachersMaxHoursDaily::getDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s;
	s+=(QObject::tr("Teachers max %1 hours daily").arg(this->maxHoursDaily));s+=", ";
	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintTeachersMaxHoursDaily::getDetailedDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=(QObject::tr("Teachers must not have more than %1 hours daily").arg(this->maxHoursDaily));s+="\n";
	s+=(QObject::tr("Weight=%1").arg(this->weight));s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int ConstraintTeachersMaxHoursDaily::fitness(TimeChromosome& c, Rules& r, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.timeChangedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		crt_chrom=&c;
		crt_rules=&r;
		
		c.timeChangedForMatrixCalculation=false;
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
					nbroken+=2; //Fortnightly activities are treated here as weekly ones, for the sake of simplicity
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
					nbroken+=2; //Fortnightly activities are treated here as weekly ones, for the sake of simplicity

					if(conflictsString!=NULL){
						(*conflictsString)+=(QObject::tr(
							"Time constraint teacher max %1 hours daily broken for teacher %2, on day %3.")
							.arg(QString::number(this->maxHoursDaily))
							.arg(r.internalTeachersList[i]->name)
							.arg(r.daysOfTheWeek[d])
							)
							+
							(QObject::tr(". This increases the conflicts total by %1").arg(QString::number(2*weight)))
							+ "\n";
					}
				}
			}
		}
	}

	return int (ceil ( weight * nbroken ) );
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

ConstraintTeachersMinHoursDaily::ConstraintTeachersMinHoursDaily()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHERS_MIN_HOURS_DAILY;
}

ConstraintTeachersMinHoursDaily::ConstraintTeachersMinHoursDaily(double w, bool c, int minhours)
 : TimeConstraint(w, c)
 {
	assert(minhours>0);
	this->minHoursDaily=minhours;

	this->type=CONSTRAINT_TEACHERS_MIN_HOURS_DAILY;
}

bool ConstraintTeachersMinHoursDaily::computeInternalStructure(Rules& r)
{
	if(&r!=NULL)
		;
	return true;
}

QString ConstraintTeachersMinHoursDaily::getXmlDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s="<ConstraintTeachersMinHoursDaily>\n";
	s+="	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Minimum_Hours_Daily>"+QString::number(this->minHoursDaily)+"</Minimum_Hours_Daily>\n";
	s+="</ConstraintTeachersMinHoursDaily>\n";
	return s;
}

QString ConstraintTeachersMinHoursDaily::getDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s;
	s+=(QObject::tr("Teachers min %1 hours daily").arg(this->minHoursDaily));s+=", ";
	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintTeachersMinHoursDaily::getDetailedDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=(QObject::tr("Teachers must not have less than %1 hours daily").arg(this->minHoursDaily));s+="\n";
	s+=(QObject::tr("Weight=%1").arg(this->weight));s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int ConstraintTeachersMinHoursDaily::fitness(TimeChromosome& c, Rules& r, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.timeChangedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		crt_chrom=&c;
		crt_rules=&r;
		
		c.timeChangedForMatrixCalculation=false;
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

				if(n_hours_daily<this->minHoursDaily)
					nbroken+=2; //Fortnightly activities are treated here as weekly ones, for the sake of simplicity
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

				if(n_hours_daily<this->minHoursDaily){
					nbroken+=2; //Fortnightly activities are treated here as weekly ones, for the sake of simplicity

					if(conflictsString!=NULL){
						(*conflictsString)+=(QObject::tr(
							"Time constraint teacher min %1 hours daily broken for teacher %2, on day %3.")
							.arg(QString::number(this->minHoursDaily))
							.arg(r.internalTeachersList[i]->name)
							.arg(r.daysOfTheWeek[d])
							)
							+
							(QObject::tr(". This increases the conflicts total by %1").arg(QString::number(2*weight)))
							+ "\n";
					}
				}
			}
		}
	}

	return int (ceil ( weight * nbroken ) );
}

bool ConstraintTeachersMinHoursDaily::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintTeachersMinHoursDaily::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return true;
}

bool ConstraintTeachersMinHoursDaily::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintTeachersMinHoursDaily::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintTeachersMinHoursDaily::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersSubgroupsMaxHoursDaily::ConstraintTeachersSubgroupsMaxHoursDaily()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHERS_SUBGROUPS_MAX_HOURS_DAILY;
}

ConstraintTeachersSubgroupsMaxHoursDaily::ConstraintTeachersSubgroupsMaxHoursDaily(double w, bool c, int maxhours)
 : TimeConstraint(w, c)
 {
	assert(maxhours>0);
	this->maxHoursDaily=maxhours;
	this->type=CONSTRAINT_TEACHERS_SUBGROUPS_MAX_HOURS_DAILY;
}

bool ConstraintTeachersSubgroupsMaxHoursDaily::computeInternalStructure(Rules& r)
{
	if(&r!=NULL)
		;
	return true;
}

QString ConstraintTeachersSubgroupsMaxHoursDaily::getXmlDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s="<ConstraintTeachersSubgroupsMaxHoursDaily>\n";
	s+="	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Maximum_Hours_Daily>"+QString::number(this->maxHoursDaily)+"</Maximum_Hours_Daily>\n";
	s+="</ConstraintTeachersSubgroupsMaxHoursDaily>\n";
	return s;
}

QString ConstraintTeachersSubgroupsMaxHoursDaily::getDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s;
	s+=(QObject::tr("Teachers-subgroups max %1 hours in a day").arg(this->maxHoursDaily));s+=", ";
	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
	s+=QObject::tr("Warning: this constraint dramatically slows down the execution of the program");

	return s;
}

QString ConstraintTeachersSubgroupsMaxHoursDaily::getDetailedDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=(QObject::tr("Teachers must not teach to any subgroup more than %1 hours in a day").arg(this->maxHoursDaily));s+="\n";
	s+=(QObject::tr("Weight=%1").arg(this->weight));s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	s+=QObject::tr("Please be very careful: this restriction dramatically slows down the automatic generation of the timetable");s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int ConstraintTeachersSubgroupsMaxHoursDaily::fitness(TimeChromosome& c, Rules& r, QString* conflictsString)
{
	int nbroken=0;

	//The matrix will keep all the durations on a day
	for(int tch=0; tch<r.nInternalTeachers; tch++)
		for(int s=0; s<r.nInternalSubgroups; s++)
			for(int d=0; d<r.nDaysPerWeek; d++)
				teachersSubgroupsDays[tch][s][d]=0;

	int i;
	//without logging
	if(conflictsString==NULL){
		for(i=0; i<r.nInternalActivities; i++){
			int tim=c.times[i];
			if(tim!=UNALLOCATED_TIME){
				int d=tim%r.nDaysPerWeek; //the day when this activity was scheduled
				Activity* act=&r.internalActivitiesList[i];
				for(int ti=0; ti<act->nTeachers; ti++){
					int tch = act->teachers[ti];
					for(int j=0; j < act->nSubgroups; j++){
						int s = act->subgroups[j];
						int tmp;
						if(act->parity==PARITY_WEEKLY)
							tmp=(teachersSubgroupsDays[tch][s][d]+=2*act->duration);
						else
							tmp=(teachersSubgroupsDays[tch][s][d]+=act->duration);
						tmp -= 2*this->maxHoursDaily;
						if(tmp > 0){
							nbroken += tmp;
							teachersSubgroupsDays[tch][s][d] = 2*this->maxHoursDaily;
						}
					}
				}
			}
		}
	}
	//with logging
	else{
		for(i=0; i<r.nInternalActivities; i++){
			int tim=c.times[i];
			if(tim!=UNALLOCATED_TIME){
				int d=tim%r.nDaysPerWeek; //the day when this activity was scheduled
				Activity* act=&r.internalActivitiesList[i];
				for(int ti=0; ti<act->nTeachers; ti++){
					int tch = act->teachers[ti];
					for(int j=0; j < act->nSubgroups; j++){
						int s = act->subgroups[j];
						int tmp;
						if(act->parity==PARITY_WEEKLY)
							tmp=(teachersSubgroupsDays[tch][s][d]+=2*act->duration);
						else
							tmp=(teachersSubgroupsDays[tch][s][d]+=act->duration);
						if(tmp>2*this->maxHoursDaily){
							nbroken+=tmp-2*this->maxHoursDaily;
							teachersSubgroupsDays[tch][s][d]=2*this->maxHoursDaily;

							if(conflictsString!=NULL){
								*conflictsString+=QObject::tr("Time constraint teachers subgroups max hours daily broken ");
								*conflictsString+=QString("because teacher %1 teaches subgroup %2 more than %3 hours on a day")
									.arg(r.internalTeachersList[tch]->name)
									.arg(r.internalSubgroupsList[s]->name)
									.arg(QString::number(this->maxHoursDaily));
								*conflictsString+="; ";
								*conflictsString+=QObject::tr("This increases the conflicts total by")+
									QString::number((tmp-2*this->maxHoursDaily)*weight)+
									"\n";
							}
						}
					}
				}
			}
		}
	}

	return int (ceil ( weight * nbroken ) );
}

bool ConstraintTeachersSubgroupsMaxHoursDaily::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintTeachersSubgroupsMaxHoursDaily::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return true;
}

bool ConstraintTeachersSubgroupsMaxHoursDaily::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintTeachersSubgroupsMaxHoursDaily::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintTeachersSubgroupsMaxHoursDaily::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMaxDaysPerWeek::ConstraintTeacherMaxDaysPerWeek()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHER_MAX_DAYS_PER_WEEK;
}

ConstraintTeacherMaxDaysPerWeek::ConstraintTeacherMaxDaysPerWeek(double w, bool c, int maxnd, QString tn)
	 : TimeConstraint(w, c)
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
	s+="	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Teacher_Name>"+protect(this->teacherName)+"</Teacher_Name>\n";
	s+="	<Max_Days_Per_Week>"+QString::number(this->maxDaysPerWeek)+"</Max_Days_Per_Week>\n";
	s+="</ConstraintTeacherMaxDaysPerWeek>\n";
	return s;
}

QString ConstraintTeacherMaxDaysPerWeek::getDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s=QObject::tr("Teacher max days per week");s+=", ";
	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
	s+=(QObject::tr("T:%1").arg(this->teacherName));s+=", ";
	s+=(QObject::tr("MD:%1").arg(this->maxDaysPerWeek));

	return s;
}

QString ConstraintTeacherMaxDaysPerWeek::getDetailedDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Teacher max. days per week");s+="\n";
	s+=(QObject::tr("Weight=%1").arg(this->weight));s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	s+=(QObject::tr("Teacher=%1").arg(this->teacherName));s+="\n";
	s+=(QObject::tr("Max. days per week=%1").arg(this->maxDaysPerWeek));s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int ConstraintTeacherMaxDaysPerWeek::fitness(TimeChromosome& c, Rules& r, QString *conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.timeChangedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		crt_chrom=&c;
		crt_rules=&r;
		
		c.timeChangedForMatrixCalculation=false;
	}

	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		//count sort
		int t=this->teacher_ID;
		int nd[MAX_HOURS_PER_DAY * 2 + 1];
		for(int h=0; h<=2*r.nHoursPerDay; h++)
			nd[h]=0;
		for(int d=0; d<r.nDaysPerWeek; d++){
			int nh=0;
			for(int h=0; h<r.nHoursPerDay; h++)
				nh += teachersMatrix[t][d][h]>=2 ? 2 : teachersMatrix[t][d][h];
			nd[nh]++;
		}
		//return the minimum occupied days which do not respect this constraint
		int i = r.nDaysPerWeek - this->maxDaysPerWeek;
		for(int k=0; k<=2*r.nHoursPerDay; k++){
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
		int nd[MAX_HOURS_PER_DAY * 2 + 1];
		for(int h=0; h<=2*r.nHoursPerDay; h++)
			nd[h]=0;
		for(int d=0; d<r.nDaysPerWeek; d++){
			int nh=0;
			for(int h=0; h<r.nHoursPerDay; h++)
				nh += teachersMatrix[t][d][h]>=2 ? 2 : teachersMatrix[t][d][h];
			nd[nh]++;
		}
		//return the minimum occupied days which do not respect this constraint
		int i = r.nDaysPerWeek - this->maxDaysPerWeek;
		for(int k=0; k<=2*r.nHoursPerDay; k++){
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
			*conflictsString += QObject::tr("Time constraint teacher max days per week broken for");
			*conflictsString += " ";
			*conflictsString += QObject::tr("teacher: %1.")
				.arg(r.internalTeachersList[t]->name);
			*conflictsString += QObject::tr("This increases the conflicts total by %1")
				.arg(nbroken*weight);
			*conflictsString += "\n";
		}
	}

	return int (ceil ( weight * nbroken ) );
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

ConstraintTeachersNoGaps::ConstraintTeachersNoGaps()
	: TimeConstraint()
{
	this->type = CONSTRAINT_TEACHERS_NO_GAPS;
}

ConstraintTeachersNoGaps::ConstraintTeachersNoGaps(double w, bool c)
	: TimeConstraint(w, c)
{
	this->type = CONSTRAINT_TEACHERS_NO_GAPS;
}

bool ConstraintTeachersNoGaps::computeInternalStructure(Rules& r)
{
	if(&r!=NULL)
		;

	/*do nothing*/
	return true;
}

QString ConstraintTeachersNoGaps::getXmlDescription(Rules& r){
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s="<ConstraintTeachersNoGaps>\n";
	s+="	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="</ConstraintTeachersNoGaps>\n";
	return s;
}

QString ConstraintTeachersNoGaps::getDescription(Rules& r){
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s;
	s+=QObject::tr("Teachers no gaps");s+=", ";
	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintTeachersNoGaps::getDetailedDescription(Rules& r){
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Teachers must not have gaps");s+="\n";
	s+=(QObject::tr("Weight=%1").arg(this->weight));s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int ConstraintTeachersNoGaps::fitness(TimeChromosome& c, Rules& r, QString* conflictsString)
{ //returns a number equal to the number of windows of teachers (in hours), multiplied by the weight
	//The current subroutine can be improved to keep track of the seniority of the teacher
	//Simply multiply by a number representing the teacher's importance in the place marked with a '*'
	//And remember to change appropriately the overall weight of the restriction (minimize it)
	//But I think for this I have to introduce double weights and fitnesses instead of int

	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.timeChangedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		crt_chrom=&c;
		crt_rules=&r;
		
		c.timeChangedForMatrixCalculation=false;
	}
	
	int tw;
	int tmp;
	int i, j, k;

	//without logging
	if(conflictsString==NULL){
		//First week
		tw=0; //teacher windows
		for(i=0; i<r.nInternalTeachers; i++)
			for(j=0; j<r.nDaysPerWeek; j++){
				for(k=0; k<r.nHoursPerDay; k++)
					if(teachersMatrix[i][j][k]>0)
						break;
				tmp=0;
				for(; k<r.nHoursPerDay; k++)
					if(teachersMatrix[i][j][k]>0){
						tw+=tmp;
						tmp=0;
					}
					else
						tmp++;
			}

		//Second week
		for(i=0; i<r.nInternalTeachers; i++)
			for(j=0; j<r.nDaysPerWeek; j++){
				for(k=0; k<r.nHoursPerDay; k++)
					if(teachersMatrix[i][j][k]>1)
						break;
				tmp=0;
				for(; k<r.nHoursPerDay; k++)
					if(teachersMatrix[i][j][k]>1){
						tw+=tmp;
						tmp=0;
					}
					else
						tmp++;
			}
	}
	//with logging
	else{
		//First week
		tw=0; //teacher windows
		for(i=0; i<r.nInternalTeachers; i++)
			for(j=0; j<r.nDaysPerWeek; j++){
				for(k=0; k<r.nHoursPerDay; k++)
					if(teachersMatrix[i][j][k]>0)
						break;

				tmp=0;
				for(; k<r.nHoursPerDay; k++)
					if(teachersMatrix[i][j][k]>0){
						tw+=tmp;

						if(conflictsString!=NULL && tmp!=0){
							*conflictsString+=
								(QObject::tr("Time constraint teachers no gaps broken: teacher: %1, week 1, day: %2, before hour: %3, lenght=%4\n")
								.arg(r.internalTeachersList[i]->name)
								.arg(r.daysOfTheWeek[j])
								.arg(r.hoursOfTheDay[k])
								.arg(tmp));
						}

						tmp=0;
					}
					else
						tmp++;
			}

		//Second week
		for(i=0; i<r.nInternalTeachers; i++)
			for(j=0; j<r.nDaysPerWeek; j++){
				for(k=0; k<r.nHoursPerDay; k++)
					if(teachersMatrix[i][j][k]>1)
						break;
				tmp=0;
				for(; k<r.nHoursPerDay; k++)
					if(teachersMatrix[i][j][k]>1){
						tw+=tmp;

						if(conflictsString!=NULL && tmp!=0){
							*conflictsString+=
								(QObject::tr("Time constraint teachers no gaps broken: teacher: %1, week 2, day: %2, before hour: %3, lenght=%4\n")
								.arg(r.internalTeachersList[i]->name)
								.arg(r.daysOfTheWeek[j])
								.arg(r.hoursOfTheDay[k])
								.arg(tmp));
						}

						tmp=0;
					}
					else
						tmp++;
			}
	}

	return int (ceil(weight * tw));
}

bool ConstraintTeachersNoGaps::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintTeachersNoGaps::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return true;
}

bool ConstraintTeachersNoGaps::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintTeachersNoGaps::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintTeachersNoGaps::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
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

ConstraintBreak::ConstraintBreak(double w, bool c, int day, int start_hour, int end_hour)
	: TimeConstraint(w, c)
{
	this->d = day;
	this->h1 = start_hour;
	this->h2 = end_hour;
	this->type = CONSTRAINT_BREAK;
}

QString ConstraintBreak::getXmlDescription(Rules& r){
	QString s="<ConstraintBreak>\n";
	s+="	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Day>"+protect(r.daysOfTheWeek[this->d])+"</Day>\n";
	s+="	<Start_Hour>"+protect(r.hoursOfTheDay[this->h1])+"</Start_Hour>\n";
	s+="	<End_Hour>"+protect(r.hoursOfTheDay[this->h2])+"</End_Hour>\n";
	s+="</ConstraintBreak>\n";
	return s;
}

QString ConstraintBreak::getDescription(Rules& r){
	QString s;
	s+=QObject::tr("Break");s+=", ";
	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
	s+=(QObject::tr("D:%1").arg(r.daysOfTheWeek[this->d]));s+=", ";
	s+=(QObject::tr("SH:%1").arg(r.hoursOfTheDay[this->h1]));s+=", ";
	s+=(QObject::tr("EH:%1").arg(r.hoursOfTheDay[this->h2]));

	return s;
}

QString ConstraintBreak::getDetailedDescription(Rules& r){
	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Break");s+="\n";
	s+=(QObject::tr("Weight=%1").arg(this->weight));s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
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
int ConstraintBreak::fitness(TimeChromosome& c, Rules& r, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.timeChangedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		crt_chrom=&c;
		crt_rules=&r;
		
		c.timeChangedForMatrixCalculation=false;
	}

	//For the moment, this function sums the number of hours each teacher
	//is teaching in this break period.
	//This function consideres all the hours, I mean if there are for example 5 weekly courses
	//scheduled on that hour (which is already a broken hard restriction - we only
	//are allowed 1 weekly course for a certain teacher at a certain hour) we calculate
	//5 broken restrictions for this break period.
	//TODO: decide if it is better to consider only 2 or 10 as a return value in this particular case
	//(currently it is 10)
	int j=this->d;
	int nbroken;
	
	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int i=0; i<r.nInternalTeachers; i++)
			for(int k=this->h1; k<this->h2; k++)
				if(teachersMatrix[i][j][k]>0) //I can get rid of this "if". Should I?
					nbroken+=teachersMatrix[i][j][k];
	}
	//with logging
	else{
		nbroken=0;
		for(int i=0; i<r.nInternalTeachers; i++)
			for(int k=this->h1; k<this->h2; k++)
				if(teachersMatrix[i][j][k]>0){
					if(conflictsString!=NULL){
						*conflictsString+=
							(QObject::tr("Time constraint break not respected for teacher %1, on day %2, hour %3")
							.arg(r.internalTeachersList[i]->name)
							.arg(r.daysOfTheWeek[j])
							.arg(r.hoursOfTheDay[k]))
							;
						*conflictsString+=". ";
						*conflictsString+=QObject::tr("This increases the conflicts total by");
						*conflictsString+=" "+QString::number(teachersMatrix[i][j][k]*weight)+
						 "\n";
					}

					nbroken+=teachersMatrix[i][j][k];
				}
	}

	return int (ceil ( weight * nbroken ) );
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

ConstraintStudentsNoGaps::ConstraintStudentsNoGaps(double w, bool c)
	: TimeConstraint(w, c)
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
	s+="	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
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
	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintStudentsNoGaps::getDetailedDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Students must not have gaps");s+="\n";
	s+=(QObject::tr("Weight=%1").arg(this->weight));s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int ConstraintStudentsNoGaps::fitness(TimeChromosome& c, Rules& r, QString* conflictsString)
{
	//returns a number equal to the number of windows of the subgroups (in hours)

	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.timeChangedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		crt_chrom=&c;
		crt_rules=&r;
		
		c.timeChangedForMatrixCalculation=false;
	}

	int windows;
	int tmp;
	int i;

	//without logging
	if(conflictsString==NULL){
		//this part is for the first week (including the first 2 week activity)
		windows=0;
		for(i=0; i<r.nInternalSubgroups; i++)
			for(int j=0; j<r.nDaysPerWeek; j++){
				int k;
				tmp=0;
				for(k=0; k<r.nHoursPerDay; k++)
					if(subgroupsMatrix[i][j][k]>0)
						break;
				for(; k<r.nHoursPerDay; k++)
					if(subgroupsMatrix[i][j][k]>0){
						windows+=tmp;
						tmp=0;
					}
					else
						//tmp++;
						tmp=1;
			}

		//this part is only for the second week (including the second 2 week activity, if any).
		for(i=0; i<r.nInternalSubgroups; i++)
			for(int j=0; j<r.nDaysPerWeek; j++){
				int k;
				tmp=0;
				for(k=0; k<r.nHoursPerDay; k++)
					if(subgroupsMatrix[i][j][k]>1)
						break;
				for(; k<r.nHoursPerDay; k++)
					if(subgroupsMatrix[i][j][k]>1){
						windows+=tmp;
						tmp=0;
					}
					else
						//tmp++;
						tmp=1;
			}
	}
	//with logging
	else{
		//this part is for the first week (including the first 2 week activity)
		windows=0;
		for(i=0; i<r.nInternalSubgroups; i++)
			for(int j=0; j<r.nDaysPerWeek; j++){
				int k;
				tmp=0;
				for(k=0; k<r.nHoursPerDay; k++)
					if(subgroupsMatrix[i][j][k]>0)
						break;
				for(; k<r.nHoursPerDay; k++)
					if(subgroupsMatrix[i][j][k]>0){
						windows+=tmp;

						if(tmp>0 && conflictsString!=NULL){
							*conflictsString+=
								(QObject::tr("Time constraint students no gaps broken for subgroup: %1, on week 1, day: %2, before hour: %3, lenght=%4\n")
								.arg(r.internalSubgroupsList[i]->name)
								.arg(r.daysOfTheWeek[j])
								.arg(r.hoursOfTheDay[k])
								.arg(tmp));
						}
						tmp=0;
					}
					else
						//tmp++;
						tmp=1;
			}

		//this part is only for the second week (including the second 2 week activity, if any).
		for(i=0; i<r.nInternalSubgroups; i++)
			for(int j=0; j<r.nDaysPerWeek; j++){
				int k;
				tmp=0;
				for(k=0; k<r.nHoursPerDay; k++)
					if(subgroupsMatrix[i][j][k]>1)
						break;
				for(; k<r.nHoursPerDay; k++)
					if(subgroupsMatrix[i][j][k]>1){
						windows+=tmp;

						if(tmp>0 && conflictsString!=NULL){
							*conflictsString+=
								(QObject::tr("Time constraint students no gaps broken for subgroup: %1, on week 2, day: %2, before hour: %3, lenght=%4\n")
								.arg(r.internalSubgroupsList[i]->name)
								.arg(r.daysOfTheWeek[j])
								.arg(r.hoursOfTheDay[k])
								.arg(tmp));
						}
						tmp=0;
					}
					else
						//tmp++;
						tmp=1;
			}
	}
		
	//windows: 2 represent a weely window, 1 represents a fortnightly window
	return int (ceil(weight * windows));
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

ConstraintStudentsSetNoGaps::ConstraintStudentsSetNoGaps(double w, bool c, const QString& st )
	: TimeConstraint(w, c)
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
	s+="	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
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
	s+=(QObject::tr("W:%1").arg(this->weight)); s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory))); s+=", ";
	s+=(QObject::tr("St:%1").arg(this->students));

	return s;
}

QString ConstraintStudentsSetNoGaps::getDetailedDescription(Rules& r){
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Students set must not have gaps");s+="\n";
	s+=(QObject::tr("Weight=%1").arg(this->weight));s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	s+=(QObject::tr("Students=%1").arg(this->students)); s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int ConstraintStudentsSetNoGaps::fitness(TimeChromosome& c, Rules& r, QString* conflictsString)
{
	//returns a number equal to the number of windows of the subgroups (in hours)

	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.timeChangedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		crt_chrom=&c;
		crt_rules=&r;
		
		c.timeChangedForMatrixCalculation=false;
	}
	
	int windows;
	int tmp;
	
	//without logging
	if(conflictsString==NULL){
		//this part is for the first week (including the first 2 week activity)
		windows=0;
		for(int sg=0; sg<this->nSubgroups; sg++){
			int i=this->subgroups[sg];
			for(int j=0; j<r.nDaysPerWeek; j++){
				int k;
				tmp=0;
				for(k=0; k<r.nHoursPerDay; k++)
					if(subgroupsMatrix[i][j][k]>0)
						break;
				for(; k<r.nHoursPerDay; k++)
					if(subgroupsMatrix[i][j][k]>0){
						windows+=tmp;
						tmp=0;
					}
					else
						tmp++;
			}
		}

		//this part is only for the second week (including the second 2 week activity, if any).
		for(int sg=0; sg<this->nSubgroups; sg++){
			int i=this->subgroups[sg];
			for(int j=0; j<r.nDaysPerWeek; j++){
				int k;
				tmp=0;
				for(k=0; k<r.nHoursPerDay; k++)
					if(subgroupsMatrix[i][j][k]>1)
						break;
				for(; k<r.nHoursPerDay; k++)
					if(subgroupsMatrix[i][j][k]>1){
						windows+=tmp;
						tmp=0;
					}
					else
						tmp++;
			}
		}
	}
	//with logging
	else{
		//this part is for the first week (including the first 2 week activity)
		windows=0;
		for(int sg=0; sg<this->nSubgroups; sg++){
			int i=this->subgroups[sg];
			for(int j=0; j<r.nDaysPerWeek; j++){
				int k;
				tmp=0;
				for(k=0; k<r.nHoursPerDay; k++)
					if(subgroupsMatrix[i][j][k]>0)
						break;
				for(; k<r.nHoursPerDay; k++)
					if(subgroupsMatrix[i][j][k]>0){
						windows+=tmp;

						if(tmp>0 && conflictsString!=NULL){
							*conflictsString+=
								(QObject::tr("Time constraint students set no gaps broken for subgroup: %1, on week 1, day: %2, before hour: %3, lenght=%4\n")
								.arg(r.internalSubgroupsList[i]->name)
								.arg(r.daysOfTheWeek[j])
								.arg(r.hoursOfTheDay[k])
								.arg(tmp));
						}
						tmp=0;
					}
					else
						tmp++;
			}
		}

		//this part is only for the second week (including the second 2 week activity, if any).
		for(int sg=0; sg<this->nSubgroups; sg++){
			int i=this->subgroups[sg];
			for(int j=0; j<r.nDaysPerWeek; j++){
				int k;
				tmp=0;
				for(k=0; k<r.nHoursPerDay; k++)
					if(subgroupsMatrix[i][j][k]>1)
						break;
				for(; k<r.nHoursPerDay; k++)
					if(subgroupsMatrix[i][j][k]>1){
						windows+=tmp;

						if(tmp>0 && conflictsString!=NULL){
							*conflictsString+=
								(QObject::tr("Time constraint students no gaps broken for subgroup: %1, on week 2, day: %2, before hour: %3, lenght=%4\n")
								.arg(r.internalSubgroupsList[i]->name)
								.arg(r.daysOfTheWeek[j])
								.arg(r.hoursOfTheDay[k])
								.arg(tmp));
						}
						tmp=0;
					}
					else
						tmp++;
			}
		}
	}

	//gaps: 2 represents a weekly gap, 1 represents a fortnightly gap
	return int (ceil(weight * windows));
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

ConstraintStudentsEarly::ConstraintStudentsEarly(double w, bool compulsory)
	: TimeConstraint(w, compulsory)
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
	s+="	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="</ConstraintStudentsEarly>\n";
	return s;
}

QString ConstraintStudentsEarly::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s;
	s+=QObject::tr("Students early");s+=", ";
	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintStudentsEarly::getDetailedDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Students must begin as early as possible their courses");s+="\n";
	s+=(QObject::tr("Weight=%1").arg(this->weight));s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int ConstraintStudentsEarly::fitness(TimeChromosome& c, Rules& r, QString* conflictsString)
{
	//considers the condition that the hours of subgroups begin as early as possible

	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.timeChangedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		crt_chrom=&c;
		crt_rules=&r;
		
		c.timeChangedForMatrixCalculation=false;
	}
	
	int free;
	int i;
	
	//treating fortnightly activities as weekly ones

	if(conflictsString==NULL){ //without logging
		free=0; //number of free hours before starting the courses
		for(i=0; i<r.nInternalSubgroups; i++)
			for(int j=0; j<r.nDaysPerWeek; j++){
				int k;
				int weekly=0;
				for(k=0; k<r.nHoursPerDay && subgroupsMatrix[i][j][k]==0; k++)
					weekly++;
				if(k<r.nHoursPerDay) //this day is not empty
					free += 2*weekly;
				//else
					//empty day
			}
	}
	else{ //with logging
		free=0; //number of free hours before starting the courses
		for(i=0; i<r.nInternalSubgroups; i++)
			for(int j=0; j<r.nDaysPerWeek; j++){
				int k;
				int weekly=0;
				for(k=0; k<r.nHoursPerDay && subgroupsMatrix[i][j][k]==0; k++)
					weekly++;
				if(k<r.nHoursPerDay && weekly>0){ //this day is not empty
					free+=2*weekly;
					*conflictsString+=
						QString("Constraint students early broken for subgroup %1, on day %2, increases conflicts total by %3\n")
						.arg(r.internalSubgroupsList[i]->name)
						.arg(r.daysOfTheWeek[j])
						.arg(2*weekly*weight);
				}
				//else
					//empty day
			}
	}

	return int(ceil(weight * free));
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

ConstraintStudentsNHoursDaily::ConstraintStudentsNHoursDaily()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_N_HOURS_DAILY;
	this->maxHoursDaily = this->minHoursDaily=-1;
}

ConstraintStudentsNHoursDaily::ConstraintStudentsNHoursDaily(double w, bool c, int maxnh, int minnh)
	: TimeConstraint(w, c)
{
	this->maxHoursDaily = maxnh;
	this->minHoursDaily = minnh;
	this->type = CONSTRAINT_STUDENTS_N_HOURS_DAILY;
}

bool ConstraintStudentsNHoursDaily::computeInternalStructure(Rules& r)
{
	if(&r!=NULL)
		;

	/*do nothing*/
	
	return true;
}

QString ConstraintStudentsNHoursDaily::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s="<ConstraintStudentsNHoursDaily>\n";
	s+="	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	if(this->maxHoursDaily>=0)
		s+="	<MaxHoursDaily>"+QString::number(this->maxHoursDaily)+"</MaxHoursDaily>\n";
	if(this->minHoursDaily>=0)
		s+="	<MinHoursDaily>"+QString::number(this->minHoursDaily)+"</MinHoursDaily>\n";
	s+="</ConstraintStudentsNHoursDaily>\n";
	return s;
}

QString ConstraintStudentsNHoursDaily::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s;
	s+=QObject::tr("Students n hours daily");s+=", ";
	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
	if(this->maxHoursDaily>=0)
		s+=(QObject::tr("MH:%1").arg(this->maxHoursDaily));
	if(this->minHoursDaily>=0)
		s+=(QObject::tr("mH:%1").arg(this->minHoursDaily));

	return s;
}

QString ConstraintStudentsNHoursDaily::getDetailedDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("All students sets must have the recommended number of hours daily");s+="\n";
	s+=(QObject::tr("Weight=%1").arg(this->weight));s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	if(this->maxHoursDaily>=0){
		s+=(QObject::tr("Maximum recommended hours daily=%1").arg(this->maxHoursDaily));
		s+="\n";
	}
	if(this->minHoursDaily>=0){
		s+=(QObject::tr("Minimum recommended hours daily=%1").arg(this->minHoursDaily));
		s+="\n";
	}

	return s;
}

//critical function here - must be optimized for speed
int ConstraintStudentsNHoursDaily::fitness(TimeChromosome& c, Rules& r, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.timeChangedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		crt_chrom=&c;
		crt_rules=&r;
		
		c.timeChangedForMatrixCalculation=false;
	}

	int tmp;
	int too_much;
	int too_little;

	//without logging
	if(conflictsString==NULL){	
		too_much=0;
		too_little=0;
		for(int i=0; i<r.nInternalSubgroups; i++)
			for(int j=0; j<r.nDaysPerWeek; j++){
				tmp=0;
				for(int k=0; k<r.nHoursPerDay; k++){
					//Here we want to see if we have a weekly activity or a 2 weeks activity
					//We don't do tmp+=subgroupsMatrix[i][j][k] because we already counted this as a hard hitness
					if(subgroupsMatrix[i][j][k]==1)
						tmp++;

					else if(subgroupsMatrix[i][j][k]>=2)
						tmp+=2;
				}
				if(tmp > 2 * this->maxHoursDaily){ //we would like no more than maxHoursDaily hours per day.
					//we multiply by two because all other clashes are multiplied by 2 (because we consider 2 weeks)
					too_much += tmp - 2 * this->maxHoursDaily;
				}
				else if(tmp < 2 * this->minHoursDaily){ //we would like no less than minHoursDaily hours per day.
					//we multiply by two because all other clashes are multiplied by 2 (because we consider 2 weeks)
					too_little += - tmp + 2 * this->minHoursDaily;
				}
			}
	}
	//with logging
	else{
		too_much=0;
		too_little=0;
		for(int i=0; i<r.nInternalSubgroups; i++)
			for(int j=0; j<r.nDaysPerWeek; j++){
				tmp=0;
				for(int k=0; k<r.nHoursPerDay; k++){
					//Here we want to see if we have a weekly activity or a 2 weeks activity
					//We don't do tmp+=subgroupsMatrix[i][j][k] because we already counted this as a hard hitness
					if(subgroupsMatrix[i][j][k]==1)
						tmp++;

					else if(subgroupsMatrix[i][j][k]>=2)
						tmp+=2;
				}
				if(tmp > 2 * this->maxHoursDaily){ //we would like no more than maxHoursDaily hours per day.
					//we multiply by two because all other clashes are multiplied by 2 (because we consider 2 weeks)
					too_much += tmp - 2 * this->maxHoursDaily;

					if(conflictsString!=NULL && tmp!=0){
						*conflictsString+=
							(QObject::tr("Time constraint students n hours daily broken for subgroup: %1, day: %2, 2*lenght=%3\n")
							.arg(r.internalSubgroupsList[i]->name)
							.arg(r.daysOfTheWeek[j])
							.arg(QString::number(tmp)));
					}
				}
				else if(tmp < 2 * this->minHoursDaily){ //we would like no less than minHoursDaily hours per day.
					//we multiply by two because all other clashes are multiplied by 2 (because we consider 2 weeks)
					too_little += - tmp + 2 * this->minHoursDaily;

					if(conflictsString!=NULL && tmp!=0){
						*conflictsString+=
							(QObject::tr("Time constraint students n hours daily broken for subgroup: %1, day: %2, 2*lenght=%3\n")
							.arg(r.internalSubgroupsList[i]->name)
							.arg(r.daysOfTheWeek[j])
							.arg(QString::number(tmp)));
					}
				}
			}
	}

	return int(ceil((too_much+too_little) * weight));
}

bool ConstraintStudentsNHoursDaily::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintStudentsNHoursDaily::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintStudentsNHoursDaily::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintStudentsNHoursDaily::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintStudentsNHoursDaily::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetNHoursDaily::ConstraintStudentsSetNHoursDaily()
	: TimeConstraint()
{
	this->type = CONSTRAINT_STUDENTS_SET_N_HOURS_DAILY;
	this->maxHoursDaily = this->minHoursDaily = -1;
}

ConstraintStudentsSetNHoursDaily::ConstraintStudentsSetNHoursDaily(double w, bool c, int maxnh, int minnh, QString s)
	: TimeConstraint(w, c)
{
	this->maxHoursDaily = maxnh;
	this->minHoursDaily = minnh;
	this->students = s;
	this->type = CONSTRAINT_STUDENTS_SET_N_HOURS_DAILY;
}

QString ConstraintStudentsSetNHoursDaily::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s="<ConstraintStudentsSetNHoursDaily>\n";
	s+="	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<MaxHoursDaily>"+QString::number(this->maxHoursDaily)+"</MaxHoursDaily>\n";
	s+="	<MinHoursDaily>"+QString::number(this->minHoursDaily)+"</MinHoursDaily>\n";
	s+="	<Students>"+protect(this->students)+"</Students>\n";
	s+="</ConstraintStudentsSetNHoursDaily>\n";
	return s;
}

QString ConstraintStudentsSetNHoursDaily::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s;
	s+=QObject::tr("Students set n hours daily");s+=", ";
	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
	if(this->maxHoursDaily>=0)
		s+=(QObject::tr("MH:%1").arg(this->maxHoursDaily));
	if(this->minHoursDaily>=0)
		s+=(QObject::tr("mH:%1").arg(this->minHoursDaily));
	s+=(QObject::tr("St:%1").arg(this->students));

	return s;
}

QString ConstraintStudentsSetNHoursDaily::getDetailedDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Students set must respect the number of hours daily");s+="\n";
	s+=(QObject::tr("Weight=%1").arg(this->weight));s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	if(this->maxHoursDaily>=0)
		s+=(QObject::tr("Maximum recommended hours daily=%1").arg(this->maxHoursDaily));s+="\n";
	if(this->minHoursDaily>=0)
		s+=(QObject::tr("Minimum recommended hours daily=%1").arg(this->minHoursDaily));s+="\n";
	s+=(QObject::tr("Students set=%1").arg(this->students));s+="\n";

	return s;
}

bool ConstraintStudentsSetNHoursDaily::computeInternalStructure(Rules &r)
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
int ConstraintStudentsSetNHoursDaily::fitness(TimeChromosome& c, Rules& r, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.timeChangedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		crt_chrom=&c;
		crt_rules=&r;
		
		c.timeChangedForMatrixCalculation=false;
	}

	int tmp;
	int too_much;
	int too_little;

	//without logging
	if(conflictsString==NULL){
		too_much=0;
		too_little=0;
		for(int sg=0; sg<this->nSubgroups; sg++){
			int i=subgroups[sg];
			for(int j=0; j<r.nDaysPerWeek; j++){
				tmp=0;
				for(int k=0; k<r.nHoursPerDay; k++){
					//Here we want to see if we have a weekly activity or a 2 weeks activity
					//We don't do tmp+=subgroupsMatrix[i][j][k] because we already counted this as a hard hitness
					if(subgroupsMatrix[i][j][k]==1)
						tmp++;

					else if(subgroupsMatrix[i][j][k]>=2)
						tmp+=2;
				}
				if(tmp > 2 * this->maxHoursDaily){ //we would like no more than max_hours_daily hours per day.
					//we multiply by two because all other clashes are multiplied by 2 (because we consider 2 weeks)
					too_much += tmp - 2 * this->maxHoursDaily;
				}
				if(tmp < 2 * this->minHoursDaily){ //we would like no less than min_hours_daily hours per day.
					//we multiply by two because all other clashes are multiplied by 2 (because we consider 2 weeks)
					too_little += -tmp + 2 * this->minHoursDaily;
				}
			}
		}
	}
	//with logging
	else{
		too_much=0;
		too_little=0;
		for(int sg=0; sg<this->nSubgroups; sg++){
			int i=subgroups[sg];
			for(int j=0; j<r.nDaysPerWeek; j++){
				tmp=0;
				for(int k=0; k<r.nHoursPerDay; k++){
					//Here we want to see if we have a weekly activity or a 2 weeks activity
					//We don't do tmp+=subgroupsMatrix[i][j][k] because we already counted this as a hard hitness
					if(subgroupsMatrix[i][j][k]==1)
						tmp++;

					else if(subgroupsMatrix[i][j][k]>=2)
						tmp+=2;
				}
				if(tmp > 2 * this->maxHoursDaily){ //we would like no more than max_hours_daily hours per day.
					//we multiply by two because all other clashes are multiplied by 2 (because we consider 2 weeks)
					too_much += tmp - 2 * this->maxHoursDaily;

					if(conflictsString!=NULL && tmp!=0){
						*conflictsString+=
							(QObject::tr("Time constraint students set n hours daily broken for subgroup: %1, day: %2, 2*lenght=%3\n")
							.arg(r.internalSubgroupsList[i]->name)
							.arg(r.daysOfTheWeek[j])
							.arg(QString::number(tmp)));
					}
				}
				if(tmp < 2 * this->minHoursDaily){ //we would like no less than min_hours_daily hours per day.
					//we multiply by two because all other clashes are multiplied by 2 (because we consider 2 weeks)
					too_little += -tmp + 2 * this->minHoursDaily;

					if(conflictsString!=NULL && tmp!=0){
						*conflictsString+=
							(QObject::tr("Time constraint students set n hours daily broken for subgroup: %1, day: %2, 2*lenght=%3\n")
							.arg(r.internalSubgroupsList[i]->name)
							.arg(r.daysOfTheWeek[j])
							.arg(QString::number(tmp)));
					}
				}
			}
		}
	}

	return int(ceil((too_much+too_little) * weight));
}

bool ConstraintStudentsSetNHoursDaily::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintStudentsSetNHoursDaily::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintStudentsSetNHoursDaily::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintStudentsSetNHoursDaily::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintStudentsSetNHoursDaily::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.studentsSetsRelated(this->students, s->name);
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetIntervalMaxDaysPerWeek::ConstraintStudentsSetIntervalMaxDaysPerWeek()
	: TimeConstraint()
{
	this->type=CONSTRAINT_STUDENTS_SET_INTERVAL_MAX_DAYS_PER_WEEK;
}

ConstraintStudentsSetIntervalMaxDaysPerWeek::ConstraintStudentsSetIntervalMaxDaysPerWeek(double w, bool c, const QString& sn, int start_hour, int end_hour, int n_intervals)
	 : TimeConstraint(w, c)
{
	this->students = sn;
	this->h1=start_hour;
	this->h2=end_hour;
	this->n=n_intervals;
	this->type=CONSTRAINT_STUDENTS_SET_INTERVAL_MAX_DAYS_PER_WEEK;
}

bool ConstraintStudentsSetIntervalMaxDaysPerWeek::computeInternalStructure(Rules& r)
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
		assert(this->nSubgroups<MAX_SUBGROUPS_PER_ACTIVITY);
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
			assert(this->nSubgroups<MAX_SUBGROUPS_PER_ACTIVITY);
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
				assert(this->nSubgroups<MAX_SUBGROUPS_PER_ACTIVITY);
				this->subgroups[this->nSubgroups++]=tmp;
			}
		}
	}
	else
		assert(0);
		
	return true;
}

QString ConstraintStudentsSetIntervalMaxDaysPerWeek::getXmlDescription(Rules& r)
{
	QString s="<ConstraintStudentsSetIntervalMaxDaysPerWeek>\n";
	s+="	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Students>"+protect(this->students)+"</Students>\n";
	s+="	<Start_Hour>"+protect(r.hoursOfTheDay[this->h1])+"</Start_Hour>\n";
	s+="	<End_Hour>"+protect(r.hoursOfTheDay[this->h2])+"</End_Hour>\n";
	s+="	<Max_Intervals>"+QString::number(this->n)+"</Max_Intervals>\n";
	s+="</ConstraintStudentsSetIntervalMaxDaysPerWeek>\n";
	return s;
}

QString ConstraintStudentsSetIntervalMaxDaysPerWeek::getDescription(Rules& r)
{
	QString s;
	s=QObject::tr("Students set interval max days per week");s+=", ";
	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
	s+=(QObject::tr("S:%1").arg(this->students));s+=", ";
	s+=(QObject::tr("SH:%1").arg(r.hoursOfTheDay[this->h1]));s+=", ";
	s+=(QObject::tr("EH:%1").arg(r.hoursOfTheDay[this->h2]));s+=", ";
	s+=(QObject::tr("MI:%1").arg(this->n));

	return s;
}

QString ConstraintStudentsSetIntervalMaxDaysPerWeek::getDetailedDescription(Rules& r)
{
	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Students set interval max. days per week");s+="\n";
	s+=(QObject::tr("Weight=%1").arg(this->weight));s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	s+=(QObject::tr("Students=%1").arg(this->students));s+="\n";
	s+=(QObject::tr("Start hour=%1").arg(r.hoursOfTheDay[this->h1]));s+="\n";
	s+=(QObject::tr("End hour=%1").arg(r.hoursOfTheDay[this->h2]));s+="\n";
	s+=(QObject::tr("Max. intervals=%1").arg(this->n));s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int ConstraintStudentsSetIntervalMaxDaysPerWeek::fitness(TimeChromosome& c, Rules& r, QString *conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.timeChangedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		crt_chrom=&c;
		crt_rules=&r;
		
		c.timeChangedForMatrixCalculation=false;
	}

	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		//count sort
		for(int m=0; m<this->nSubgroups; m++){
			int s=this->subgroups[m];
			int nd[MAX_HOURS_PER_DAY * 2 + 1];
			for(int h=0; h<=2*r.nHoursPerDay; h++)
				nd[h]=0;
			int n_total_intervals=0;
			for(int d=0; d<r.nDaysPerWeek; d++){
				int nh=0;
				for(int h=this->h1; h<this->h2; h++)
					nh += subgroupsMatrix[s][d][h]>=2 ? 2 : subgroupsMatrix[s][d][h];
				nd[nh]++;
				n_total_intervals++;
			}
			//return the minimum intervals which do not respect this constraint
			int i = n_total_intervals - this->n;
			for(int k=0; k<=2*r.nHoursPerDay; k++){
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
	}
	//with logging
	else{
		nbroken=0;
		//count sort
		for(int m=0; m<this->nSubgroups; m++){
			int s=this->subgroups[m];
			int nd[MAX_HOURS_PER_DAY * 2 + 1];
			for(int h=0; h<=2*r.nHoursPerDay; h++)
				nd[h]=0;
			int n_total_intervals=0;
			for(int d=0; d<r.nDaysPerWeek; d++){
				int nh=0;
				for(int h=this->h1; h<this->h2; h++)
					nh += subgroupsMatrix[s][d][h]>=2 ? 2 : subgroupsMatrix[s][d][h];
				nd[nh]++;
				n_total_intervals++;
			}
			//return the minimum intervals which do not respect this constraint
			int i = n_total_intervals - this->n;
			int nbroken_partial=0;
			for(int k=0; k<=2*r.nHoursPerDay; k++){
				if(nd[k]>0){
					if(i>nd[k]){
						i-=nd[k];
						nbroken_partial+=nd[k]*k;
					}
					else{
						nbroken_partial+=i*k;
						break;
					}
				}
			}

			*conflictsString += QObject::tr("Time constraint students set interval max days per week broken for");
			*conflictsString += " ";
			*conflictsString += QObject::tr("subgroup: %1.")
				.arg(r.internalSubgroupsList[s]->name);
			*conflictsString += QObject::tr("This increases the conflicts total by %1")
				.arg(nbroken_partial*weight);
			*conflictsString += "\n";

			nbroken+=nbroken_partial;
		}
	}

	return int (ceil ( weight * nbroken ) );
}

bool ConstraintStudentsSetIntervalMaxDaysPerWeek::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintStudentsSetIntervalMaxDaysPerWeek::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintStudentsSetIntervalMaxDaysPerWeek::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintStudentsSetIntervalMaxDaysPerWeek::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintStudentsSetIntervalMaxDaysPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.studentsSetsRelated(this->students, s->name);
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherIntervalMaxDaysPerWeek::ConstraintTeacherIntervalMaxDaysPerWeek()
	: TimeConstraint()
{
	this->type=CONSTRAINT_TEACHER_INTERVAL_MAX_DAYS_PER_WEEK;
}

ConstraintTeacherIntervalMaxDaysPerWeek::ConstraintTeacherIntervalMaxDaysPerWeek(double w, bool c, const QString& tn, int start_hour, int end_hour, int n_intervals)
	 : TimeConstraint(w, c)
{
	this->teacher = tn;
	this->h1=start_hour;
	this->h2=end_hour;
	this->n=n_intervals;
	this->type=CONSTRAINT_TEACHER_INTERVAL_MAX_DAYS_PER_WEEK;
}

bool ConstraintTeacherIntervalMaxDaysPerWeek::computeInternalStructure(Rules& r)
{
	int tmp;	
	for(tmp=0; tmp<r.nInternalTeachers; tmp++)
		if(r.internalTeachersList[tmp]->name==this->teacher){
			this->teacherIndex=tmp;
			break;
		}
	assert(tmp<r.nInternalTeachers);

	return true;
}

QString ConstraintTeacherIntervalMaxDaysPerWeek::getXmlDescription(Rules& r)
{
	QString s="<ConstraintTeacherIntervalMaxDaysPerWeek>\n";
	s+="	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Teacher>"+protect(this->teacher)+"</Teacher>\n";
	s+="	<Start_Hour>"+protect(r.hoursOfTheDay[this->h1])+"</Start_Hour>\n";
	s+="	<End_Hour>"+protect(r.hoursOfTheDay[this->h2])+"</End_Hour>\n";
	s+="	<Max_Intervals>"+QString::number(this->n)+"</Max_Intervals>\n";
	s+="</ConstraintTeacherIntervalMaxDaysPerWeek>\n";
	return s;
}

QString ConstraintTeacherIntervalMaxDaysPerWeek::getDescription(Rules& r)
{
	QString s;
	s=QObject::tr("Teacher interval max days per week");s+=", ";
	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
	s+=(QObject::tr("T:%1").arg(this->teacher));s+=", ";
	s+=(QObject::tr("SH:%1").arg(r.hoursOfTheDay[this->h1]));s+=", ";
	s+=(QObject::tr("EH:%1").arg(r.hoursOfTheDay[this->h2]));s+=", ";
	s+=(QObject::tr("MI:%1").arg(this->n));

	return s;
}

QString ConstraintTeacherIntervalMaxDaysPerWeek::getDetailedDescription(Rules& r)
{
	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Teacher interval max. days per week");s+="\n";
	s+=(QObject::tr("Weight=%1").arg(this->weight));s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	s+=(QObject::tr("Teacher=%1").arg(this->teacher));s+="\n";
	s+=(QObject::tr("Start hour=%1").arg(r.hoursOfTheDay[this->h1]));s+="\n";
	s+=(QObject::tr("End hour=%1").arg(r.hoursOfTheDay[this->h2]));s+="\n";
	s+=(QObject::tr("Max. intervals=%1").arg(this->n));s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int ConstraintTeacherIntervalMaxDaysPerWeek::fitness(TimeChromosome& c, Rules& r, QString *conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.timeChangedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		crt_chrom=&c;
		crt_rules=&r;
		
		c.timeChangedForMatrixCalculation=false;
	}

	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		//count sort
		int t=this->teacherIndex;
		int nd[MAX_HOURS_PER_DAY * 2 + 1];
		for(int h=0; h<=2*r.nHoursPerDay; h++)
			nd[h]=0;
		int n_total_intervals=0;
		for(int d=0; d<r.nDaysPerWeek; d++){
			int nh=0;
			for(int h=this->h1; h<this->h2; h++)
				nh+=teachersMatrix[t][d][h]>=2 ? 2 : teachersMatrix[t][d][h];
			nd[nh]++;
			n_total_intervals++;
		}
		//return the minimum intervals which do not respect this constraint
		int i = n_total_intervals - this->n;
		for(int k=0; k<=2*r.nHoursPerDay; k++){
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
		int t=this->teacherIndex;
		int nd[MAX_HOURS_PER_DAY * 2 + 1];
		for(int h=0; h<=2*r.nHoursPerDay; h++)
			nd[h]=0;
		int n_total_intervals=0;
		for(int d=0; d<r.nDaysPerWeek; d++){
			int nh=0;
			for(int h=this->h1; h<this->h2; h++)
				nh+=teachersMatrix[t][d][h]>=2 ? 2 : teachersMatrix[t][d][h];
			nd[nh]++;
			n_total_intervals++;
		}
		//return the minimum intervals which do not respect this constraint
		int i = n_total_intervals - this->n;
		for(int k=0; k<=2*r.nHoursPerDay; k++){
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
	
		*conflictsString += QObject::tr("Time constraint teacher interval max days per week broken for");
		*conflictsString += " ";
		*conflictsString += QObject::tr("teacher: %1.")
			.arg(r.internalTeachersList[t]->name);
		*conflictsString += QObject::tr("This increases the conflicts total by %1")
			.arg(nbroken*weight);
		*conflictsString += "\n";
	}

	return int (ceil ( weight * nbroken ) );
}

bool ConstraintTeacherIntervalMaxDaysPerWeek::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintTeacherIntervalMaxDaysPerWeek::isRelatedToTeacher(Teacher* t)
{
	if(t->name == this->teacher)
		return true;

	return false;
}

bool ConstraintTeacherIntervalMaxDaysPerWeek::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintTeacherIntervalMaxDaysPerWeek::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintTeacherIntervalMaxDaysPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;
		
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivityPreferredTime::ConstraintActivityPreferredTime()
	: TimeConstraint()
{
	this->type = CONSTRAINT_ACTIVITY_PREFERRED_TIME;
}

ConstraintActivityPreferredTime::ConstraintActivityPreferredTime(double w, bool c, int actId, int d, int h)
	: TimeConstraint(w, c)
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
	if(this->compulsory!=c.compulsory)
		return false;
	if(this->weight!=c.weight)
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
	
	if(i==r.nInternalActivities)
		return false;

	this->activityIndex=i;	
	return true;
}

QString ConstraintActivityPreferredTime::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s="<ConstraintActivityPreferredTime>\n";
	s+="	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
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

	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

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

	s+=(QObject::tr("Weight=%1").arg(this->weight));s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int ConstraintActivityPreferredTime::fitness(TimeChromosome& c, Rules& r, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.timeChangedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		crt_chrom=&c;
		crt_rules=&r;
		
		c.timeChangedForMatrixCalculation=false;
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
		if(r.internalActivitiesList[this->activityIndex].parity==PARITY_WEEKLY) //for weekly activities, double the conflicts
			nbroken*=2;
	}

	if(conflictsString!=NULL && nbroken>0){
		*conflictsString+=
			(QObject::tr("Time constraint activity preferred time broken for activity with id=%1, increases conflicts total by %2\n")
			.arg(this->activityId)
			.arg(weight*nbroken));
	}

	return int(ceil(nbroken * weight));
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

ConstraintActivityPreferredTimes::ConstraintActivityPreferredTimes(double w, bool c, int actId, int nPT, int d[], int h[])
	: TimeConstraint(w, c)
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

	if(i==r.nInternalActivities)
		return false;

	this->activityIndex=i;	
	return true;
}

QString ConstraintActivityPreferredTimes::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s="<ConstraintActivityPreferredTimes>\n";
	s+="	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
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

	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

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

	s+=(QObject::tr("Weight=%1").arg(this->weight));s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int ConstraintActivityPreferredTimes::fitness(TimeChromosome& c, Rules& r, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.timeChangedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		crt_chrom=&c;
		crt_rules=&r;
		
		c.timeChangedForMatrixCalculation=false;
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
			if(r.internalActivitiesList[this->activityIndex].parity==PARITY_WEEKLY) //for weekly activities, double the conflicts
				nbroken*=2;
		}
	}

	if(conflictsString!=NULL && nbroken>0){
		*conflictsString+=
			(QObject::tr("Time constraint activity preferred times broken for activity with id=%1, increases conflicts total by %2\n")
			.arg(this->activityId)
			.arg(weight*nbroken));
	}

	return int(ceil(nbroken * weight));
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

ConstraintActivityEndsDay::ConstraintActivityEndsDay()
	: TimeConstraint()
{
	this->type=CONSTRAINT_ACTIVITY_ENDS_DAY;
}

ConstraintActivityEndsDay::ConstraintActivityEndsDay(double w, bool c, int actId)
	: TimeConstraint(w, c)
{
	this->activityId=actId;
	this->type=CONSTRAINT_ACTIVITY_ENDS_DAY;
}

bool ConstraintActivityEndsDay::computeInternalStructure(Rules& r)
{
	Activity* act;
	int i;
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		if(act->id==this->activityId)
			break;
	}
	
	if(i==r.nInternalActivities)
		return false;

	this->activityIndex=i;	
	return true;
}

QString ConstraintActivityEndsDay::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s="<ConstraintActivityEndsDay>\n";
	s+="	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Activity_Id>"+QString::number(this->activityId)+"</Activity_Id>\n";
	s+="</ConstraintActivityEndsDay>\n";
	return s;
}

QString ConstraintActivityEndsDay::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s;
	s+=(QObject::tr("Act. id:%1").arg(this->activityId));s+=", ";
	s+=QObject::tr("must end the day");
	s+=", ";

	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString ConstraintActivityEndsDay::getDetailedDescription(Rules& r)
{
	if(&r!=NULL)
		;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=(QObject::tr("Activity with id=%1").arg(this->activityId));s+="\n";
	s+=QObject::tr("must be the last in a certain day");s+="\n";
		
	s+=(QObject::tr("Weight=%1").arg(this->weight));s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

//critical function here - must be optimized for speed
//TODO: this calculation can be improved, by precomputation, using
//rather the "times" array than the teachers and subgroups matrix.
//Only for many constraints of this kind (improbable)
int ConstraintActivityEndsDay::fitness(TimeChromosome& c, Rules& r, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.timeChangedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		crt_chrom=&c;
		crt_rules=&r;
		
		c.timeChangedForMatrixCalculation=false;
	}

	int nbroken;

	assert(r.internalStructureComputed);

	nbroken=0;
	if(c.times[this->activityIndex]!=UNALLOCATED_TIME){
		int d=c.times[this->activityIndex]%r.nDaysPerWeek; //the day when this activity was scheduled
		int h=c.times[this->activityIndex]/r.nDaysPerWeek; //the hour

		//the last activity for subgroups		
		for(int s=0; s<r.nInternalSubgroups; s++)
			for(int i=h+r.internalActivitiesList[this->activityIndex].duration; i<r.nHoursPerDay; i++)
				nbroken += subgroupsMatrix[s][d][i]>=2 ? 2 : subgroupsMatrix[s][d][i];

		//the last activity for teachers
		for(int t=0; t<r.nInternalTeachers; t++)
			for(int i=h+r.internalActivitiesList[this->activityIndex].duration; i<r.nHoursPerDay; i++)
				nbroken += subgroupsMatrix[t][d][i]>=2 ? 2 : subgroupsMatrix[t][d][i];

		if(r.internalActivitiesList[this->activityIndex].parity==PARITY_WEEKLY) //for weekly activities, double the conflicts
			nbroken*=2;
	}

	if(conflictsString!=NULL && nbroken>0){
		*conflictsString+=
			(QObject::tr("Time constraint activity ends day broken for activity with id=%1, increases conflicts total by %2\n")
			.arg(this->activityId)
			.arg(weight*nbroken));
	}

	return int(ceil(nbroken * weight));
}

bool ConstraintActivityEndsDay::isRelatedToActivity(Activity* a)
{
	if(this->activityId==a->id)
		return true;
	return false;
}

bool ConstraintActivityEndsDay::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool ConstraintActivityEndsDay::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintActivityEndsDay::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool ConstraintActivityEndsDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;
		
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

Constraint2ActivitiesConsecutive::Constraint2ActivitiesConsecutive()
	: TimeConstraint()
{
	this->type = CONSTRAINT_2_ACTIVITIES_CONSECUTIVE;
}

Constraint2ActivitiesConsecutive::Constraint2ActivitiesConsecutive(double w, bool c, int firstActId, int secondActId)
	: TimeConstraint(w, c)
{
	this->firstActivityId = firstActId;
	this->secondActivityId = secondActId;
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
	if(i==r.nInternalActivities)
		return false;

	this->firstActivityIndex=i;
	
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		if(act->id==this->secondActivityId)
			break;
	}
	if(i==r.nInternalActivities)
		return false;

	this->secondActivityIndex=i;
	
	return true;
}

QString Constraint2ActivitiesConsecutive::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s="<Constraint2ActivitiesConsecutive>\n";
	s+="	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
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

	QString s="2 activities consecutive: ";
	s+=(QObject::tr("act. id 1:%1").arg(this->firstActivityId));s+=", ";
	s+=(QObject::tr("act. id 2:%1").arg(this->secondActivityId));s+=", ";
	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString Constraint2ActivitiesConsecutive::getDetailedDescription(Rules& r)
{
	if(&r!=NULL)
		;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=(QObject::tr("Activity with id=%1").arg(this->firstActivityId));s+="\n";
	s+=(QObject::tr("must be followed (immediately) by activity with id=%1").arg(this->secondActivityId));s+="\n";
	s+=(QObject::tr("Weight=%1").arg(this->weight));s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int Constraint2ActivitiesConsecutive::fitness(TimeChromosome& c, Rules& r, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.timeChangedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		crt_chrom=&c;
		crt_rules=&r;
		
		c.timeChangedForMatrixCalculation=false;
	}

	int nbroken;

	assert(r.internalStructureComputed);

	nbroken=0;
	if(c.times[this->firstActivityIndex]!=UNALLOCATED_TIME && c.times[this->secondActivityIndex]!=UNALLOCATED_TIME){
		int d1=c.times[this->firstActivityIndex]%r.nDaysPerWeek; //the day when the first activity was scheduled
		int h1=c.times[this->firstActivityIndex]/r.nDaysPerWeek
			+r.internalActivitiesList[this->firstActivityIndex].duration; //the hour when the first activity ends
		int d2=c.times[this->secondActivityIndex]%r.nDaysPerWeek; //the day when the second activity was scheduled
		int h2=c.times[this->secondActivityIndex]/r.nDaysPerWeek; //the hour when the second activity begins

		nbroken+=abs(d1-d2);
		nbroken+=abs(h1-h2);
		if(r.internalActivitiesList[this->firstActivityIndex].parity==PARITY_WEEKLY) //for weekly activities, double the conflicts
			nbroken*=2;
		if(r.internalActivitiesList[this->secondActivityIndex].parity==PARITY_WEEKLY) //for weekly activities, double the conflicts
			nbroken*=2;
	}

	if(conflictsString!=NULL && nbroken>0){
		*conflictsString+=
			(QObject::tr("Time constraint 2 activities consecutive broken for activities with id=%1 and id=%2, increases conflicts total by %3\n")
			.arg(this->firstActivityId)
			.arg(this->secondActivityId)
			.arg(weight*nbroken));
	}

	return int(ceil(nbroken * weight));
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

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

Constraint2ActivitiesOrdered::Constraint2ActivitiesOrdered()
	: TimeConstraint()
{
	this->type = CONSTRAINT_2_ACTIVITIES_ORDERED;
}

Constraint2ActivitiesOrdered::Constraint2ActivitiesOrdered(double w, bool c, int firstActId, int secondActId)
	: TimeConstraint(w, c)
{
	this->firstActivityId = firstActId;
	this->secondActivityId = secondActId;
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
	if(i==r.nInternalActivities)
		return false;

	this->firstActivityIndex=i;
	
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		if(act->id==this->secondActivityId)
			break;
	}
	if(i==r.nInternalActivities)
		return false;

	this->secondActivityIndex=i;
	
	return true;
}

QString Constraint2ActivitiesOrdered::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s="<Constraint2ActivitiesOrdered>\n";
	s+="	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<First_Activity_Id>"+QString::number(this->firstActivityId)+"</First_Activity_Id>\n";
	s+="	<Second_Activity_Id>"+QString::number(this->secondActivityId)+"</Second_Activity_Id>\n";
	s+="</Constraint2ActivitiesOrdered>\n";
	return s;
}

QString Constraint2ActivitiesOrdered::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s="2 activities ordered: ";
	s+=(QObject::tr("act. id 1:%1").arg(this->firstActivityId));s+=", ";
	s+=(QObject::tr("act. id 2:%1").arg(this->secondActivityId));s+=", ";
	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString Constraint2ActivitiesOrdered::getDetailedDescription(Rules& r)
{
	if(&r!=NULL)
		;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=(QObject::tr("Activity with id=%1").arg(this->firstActivityId));s+="\n";
	s+=(QObject::tr("must be followed (at arbitrary distance) by activity with id=%1").arg(this->secondActivityId));s+="\n";
	s+=(QObject::tr("Weight=%1").arg(this->weight));s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int Constraint2ActivitiesOrdered::fitness(TimeChromosome& c, Rules& r, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.timeChangedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		crt_chrom=&c;
		crt_rules=&r;
		
		c.timeChangedForMatrixCalculation=false;
	}

	int nbroken;

	assert(r.internalStructureComputed);

	nbroken=0;
	if(c.times[this->firstActivityIndex]!=UNALLOCATED_TIME && c.times[this->secondActivityIndex]!=UNALLOCATED_TIME){
		int d1=c.times[this->firstActivityIndex]%r.nDaysPerWeek; //the day when the first activity was scheduled
		int h1=c.times[this->firstActivityIndex]/r.nDaysPerWeek
			+r.internalActivitiesList[this->firstActivityIndex].duration; //the hour when the first activity ends
		int d2=c.times[this->secondActivityIndex]%r.nDaysPerWeek; //the day when the second activity was scheduled
		int h2=c.times[this->secondActivityIndex]/r.nDaysPerWeek; //the hour when the second activity begins

		if(d1>d2 || d1==d2 && h1>h2)
			nbroken++;

		if(r.internalActivitiesList[this->firstActivityIndex].parity==PARITY_WEEKLY) //for weekly activities, double the conflicts
			nbroken*=2;
		if(r.internalActivitiesList[this->secondActivityIndex].parity==PARITY_WEEKLY) //for weekly activities, double the conflicts
			nbroken*=2;
	}

	if(conflictsString!=NULL && nbroken>0){
		*conflictsString+=
			(QObject::tr("Time constraint 2 activities ordered broken for activities with id=%1 and id=%2, increases conflicts total by %3\n")
			.arg(this->firstActivityId)
			.arg(this->secondActivityId)
			.arg(weight*nbroken));
	}

	return int(ceil(nbroken * weight));
}

bool Constraint2ActivitiesOrdered::isRelatedToActivity(Activity* a)
{
	if(this->firstActivityId==a->id)
		return true;
	if(this->secondActivityId==a->id)
		return true;
	return false;
}

bool Constraint2ActivitiesOrdered::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool Constraint2ActivitiesOrdered::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool Constraint2ActivitiesOrdered::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool Constraint2ActivitiesOrdered::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;
		
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

Constraint2ActivitiesGrouped::Constraint2ActivitiesGrouped()
	: TimeConstraint()
{
	this->type = CONSTRAINT_2_ACTIVITIES_GROUPED;
}

Constraint2ActivitiesGrouped::Constraint2ActivitiesGrouped(double w, bool c, int firstActId, int secondActId)
	: TimeConstraint(w, c)
{
	this->firstActivityId = firstActId;
	this->secondActivityId = secondActId;
	this->type = CONSTRAINT_2_ACTIVITIES_GROUPED;
}

bool Constraint2ActivitiesGrouped::computeInternalStructure(Rules& r)
{
	Activity* act;
	int i;
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		if(act->id==this->firstActivityId)
			break;
	}
	if(i==r.nInternalActivities)
		return false;

	this->firstActivityIndex=i;
	
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		if(act->id==this->secondActivityId)
			break;
	}
	if(i==r.nInternalActivities)
		return false;

	this->secondActivityIndex=i;
	
	return true;
}

QString Constraint2ActivitiesGrouped::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s="<Constraint2ActivitiesGrouped>\n";
	s+="	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<First_Activity_Id>"+QString::number(this->firstActivityId)+"</First_Activity_Id>\n";
	s+="	<Second_Activity_Id>"+QString::number(this->secondActivityId)+"</Second_Activity_Id>\n";
	s+="</Constraint2ActivitiesGrouped>\n";
	return s;
}

QString Constraint2ActivitiesGrouped::getDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s="2 activities grouped: ";
	s+=(QObject::tr("act. id 1:%1").arg(this->firstActivityId));s+=", ";
	s+=(QObject::tr("act. id 2:%1").arg(this->secondActivityId));s+=", ";
	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

	return s;
}

QString Constraint2ActivitiesGrouped::getDetailedDescription(Rules& r)
{
	if(&r!=NULL)
		;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=(QObject::tr("Activity with id=%1").arg(this->firstActivityId));s+="\n";
	s+=(QObject::tr("must be followed by/follow activity with id=%1").arg(this->secondActivityId));s+="\n";
	s+=(QObject::tr("Weight=%1").arg(this->weight));s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int Constraint2ActivitiesGrouped::fitness(TimeChromosome& c, Rules& r, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.timeChangedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		crt_chrom=&c;
		crt_rules=&r;
		
		c.timeChangedForMatrixCalculation=false;
	}

	int nbroken;

	assert(r.internalStructureComputed);

	nbroken=0;
	if(c.times[this->firstActivityIndex]!=UNALLOCATED_TIME && c.times[this->secondActivityIndex]!=UNALLOCATED_TIME){
		int d1=c.times[this->firstActivityIndex]%r.nDaysPerWeek; //the day when the first activity was scheduled
		int h1=c.times[this->firstActivityIndex]/r.nDaysPerWeek; //the hour when the first activity begins
		int dur1=r.internalActivitiesList[this->firstActivityIndex].duration; //duration of first activity
		int d2=c.times[this->secondActivityIndex]%r.nDaysPerWeek; //the day when the second activity was scheduled
		int h2=c.times[this->secondActivityIndex]/r.nDaysPerWeek; //the hour when the second activity begins
		int dur2=r.internalActivitiesList[this->secondActivityIndex].duration; //duration of second activity

		nbroken+=abs(d1-d2);
		
		if(h1>h2){
			if(h1 >= h2+dur2)
				nbroken += h1-(h2+dur2);
			else
				nbroken+=1;
		}
		else{
			if(h2 >= h1+dur1)
				nbroken += h2-(h1+dur1);
			else
				nbroken+=1;
		}

		if(r.internalActivitiesList[this->firstActivityIndex].parity==PARITY_WEEKLY) //for weekly activities, double the conflicts
			nbroken*=2;
		if(r.internalActivitiesList[this->secondActivityIndex].parity==PARITY_WEEKLY) //for weekly activities, double the conflicts
			nbroken*=2;
	}

	if(conflictsString!=NULL && nbroken>0){
		*conflictsString+=
			(QObject::tr("Time constraint 2 activities grouped broken for activities with id=%1 and id=%2, increases conflicts total by %3\n")
			.arg(this->firstActivityId)
			.arg(this->secondActivityId)
			.arg(weight*nbroken));
	}

	return int(ceil(nbroken * weight));
}

bool Constraint2ActivitiesGrouped::isRelatedToActivity(Activity* a)
{
	if(this->firstActivityId==a->id)
		return true;
	if(this->secondActivityId==a->id)
		return true;
	return false;
}

bool Constraint2ActivitiesGrouped::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return false;
}

bool Constraint2ActivitiesGrouped::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool Constraint2ActivitiesGrouped::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return false;
}

bool Constraint2ActivitiesGrouped::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
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

ConstraintActivitiesPreferredTimes::ConstraintActivitiesPreferredTimes(double w, bool c, QString te,
	QString st, QString su, QString sut, int nPT, int d[], int h[])
	: TimeConstraint(w, c)
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
			it = act->studentsNames.find(this->studentsName);
			if(it==act->studentsNames.end())
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
	else
		return false;
}

QString ConstraintActivitiesPreferredTimes::getXmlDescription(Rules& r)
{
	//to avoid non-used parameter warning
	if(&r==NULL)
		;

	QString s="<ConstraintActivitiesPreferredTimes>\n";
	s+="	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
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

	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));

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

	s+=(QObject::tr("Weight=%1").arg(this->weight));s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int ConstraintActivitiesPreferredTimes::fitness(TimeChromosome& c, Rules& r, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.timeChangedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		crt_chrom=&c;
		crt_rules=&r;
		
		c.timeChangedForMatrixCalculation=false;
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
					if(r.internalActivitiesList[ai].parity==PARITY_WEEKLY) //for weekly activities, double the conflicts
						tmp=2;
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
					if(r.internalActivitiesList[ai].parity==PARITY_WEEKLY) //for weekly activities, double the conflicts
						tmp=2;
				}
			}
			nbroken+=tmp;
			if(conflictsString!=NULL && tmp>0){
				*conflictsString+=
					QObject::tr("Time constraint activities preferred times broken"
						" for activity with id=%1, "
						" increases conflicts total by %2\n")
					.arg(r.internalActivitiesList[ai].id)
					.arg(weight*tmp);
			}
		}
	}

	return int(ceil(nbroken * weight));
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

ConstraintActivitiesSameStartingHour::ConstraintActivitiesSameStartingHour(double w, bool c, int nact, const int act[])
 : TimeConstraint(w, c)
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
	
	if(this->_n_activities<=1)
		return false;

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
	s+="	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
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
	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory)));s+=", ";
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
	s+=(QObject::tr("Weight=%1").arg(this->weight));s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));s+="\n";
	s+=(QObject::tr("Number of activities=%1").arg(this->n_activities));s+="\n";
	for(int i=0; i<this->n_activities; i++){
		s+=(QObject::tr("Activity with id=%1").arg(this->activitiesId[i]));s+="\n";
	}

	return s;
}

//critical function here - must be optimized for speed
int ConstraintActivitiesSameStartingHour::fitness(TimeChromosome& c, Rules& r, QString* conflictsString)
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
						if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY &&
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 4 * abs(hour1-hour2);
						else if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY ||
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 2 * abs(hour1-hour2);
						else
							tmp = abs(hour1-hour2);

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
						if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY &&
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 4 * abs(hour1-hour2);
						else if(r.internalActivitiesList[this->_activities[i]].parity==PARITY_WEEKLY ||
						 r.internalActivitiesList[this->_activities[j]].parity==PARITY_WEEKLY)
							tmp = 2 * abs(hour1-hour2);
						else
							tmp = abs(hour1-hour2);

						nbroken+=tmp;

						if(tmp>0 && conflictsString!=NULL){
							*conflictsString+=(QObject::tr("Time constraint activities same hour broken, because activity with id=%1 is not at the same hour with activity with id=%2")
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

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersSubjectTagsMaxHoursContinuously::ConstraintTeachersSubjectTagsMaxHoursContinuously()
	: TimeConstraint()
{
	type=CONSTRAINT_TEACHERS_SUBJECT_TAGS_MAX_HOURS_CONTINUOUSLY;
}

ConstraintTeachersSubjectTagsMaxHoursContinuously::ConstraintTeachersSubjectTagsMaxHoursContinuously(double w, bool c, int maxhours)
 : TimeConstraint(w, c)
 {
	assert(maxhours>0);
	this->maxHoursContinuously=maxhours;

	this->type=CONSTRAINT_TEACHERS_SUBJECT_TAGS_MAX_HOURS_CONTINUOUSLY;
}

bool ConstraintTeachersSubjectTagsMaxHoursContinuously::computeInternalStructure(Rules& r)
{
	if(&r)
		;
		
	return true;
}

QString ConstraintTeachersSubjectTagsMaxHoursContinuously::getXmlDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s="<ConstraintTeachersSubjectTagsMaxHoursContinuously>\n";
	s+="	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Maximum_Hours_Continuously>"+QString::number(this->maxHoursContinuously)+"</Maximum_Hours_Continuously>\n";
	s+="</ConstraintTeachersSubjectTagsMaxHoursContinuously>\n";
	return s;
}

QString ConstraintTeachersSubjectTagsMaxHoursContinuously::getDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s;
	s+=QObject::tr("Teachers subject tags max %1 hours in a row")
	 .arg(this->maxHoursContinuously);	 
	s+=", ";
	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory))); s+=", ";

	return s;
}

QString ConstraintTeachersSubjectTagsMaxHoursContinuously::getDetailedDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Teachers subject tags max %1 hours in a row")
	 .arg(this->maxHoursContinuously);
	s+="\n";
	s+=(QObject::tr("Weight=%1").arg(this->weight));
	s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));
	s+="\n";
	s+=QObject::tr("Warning: this constraint dramatically slows down the execution of the program");
	s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int ConstraintTeachersSubjectTagsMaxHoursContinuously::fitness(TimeChromosome& c, Rules& r, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.timeChangedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		crt_chrom=&c;
		crt_rules=&r;
		
		c.timeChangedForMatrixCalculation=false;
	}

	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int st=0; st<r.subjectTagsList.size(); st++){
			for(int d=0; d<r.nDaysPerWeek; d++)
				for(int h=0; h<r.nHoursPerDay; h++)
					for(int t=0; t<r.nInternalTeachers; t++)
						teachersSubjectTags[t][d][h]=0;
						
			for(int i=0; i<r.nInternalActivities; i++)
				if(c.times[i]!=UNALLOCATED_TIME){
					Activity* act=&r.internalActivitiesList[i];
					if(act->subjectTagIndex!=st)
						continue;
					int hour=c.times[i]/r.nDaysPerWeek;
					int day=c.times[i]%r.nDaysPerWeek;
					for(int dd=0; dd<act->duration && hour+dd<r.nHoursPerDay; dd++)
						for(int it=0; it<act->nTeachers; it++){
							int tch=act->teachers[it];
							if(act->parity==PARITY_WEEKLY){
								teachersSubjectTags[tch][day][hour+dd]+=2;
							}
							else{
								assert(act->parity==PARITY_FORTNIGHTLY);
								teachersSubjectTags[tch][day][hour+dd]++;
							}
						}
				}
				
			for(int t=0; t<r.nInternalTeachers; t++){
				for(int d=0; d<r.nDaysPerWeek; d++){
					//Fortnightly activities are treated here as weekly ones, for the sake of simplicity					
					int n_cont_hours=0;
					for(int h=0; h<r.nHoursPerDay; h++){
						if(teachersSubjectTags[t][d][h]>0){
							n_cont_hours++;
							if(n_cont_hours>this->maxHoursContinuously){
								nbroken+=2;
							}
						}
						else
							n_cont_hours=0;
					}
				}
			}
		}
	}
	//with logging
	else{
		nbroken=0;
		for(int st=0; st<r.subjectTagsList.size(); st++){
			for(int d=0; d<r.nDaysPerWeek; d++)
				for(int h=0; h<r.nHoursPerDay; h++)
					for(int t=0; t<r.nInternalTeachers; t++)
						teachersSubjectTags[t][d][h]=0;
						
			for(int i=0; i<r.nInternalActivities; i++)
				if(c.times[i]!=UNALLOCATED_TIME){
					Activity* act=&r.internalActivitiesList[i];
					if(act->subjectTagIndex!=st)
						continue;
					int hour=c.times[i]/r.nDaysPerWeek;
					int day=c.times[i]%r.nDaysPerWeek;
					for(int dd=0; dd<act->duration && hour+dd<r.nHoursPerDay; dd++)
						for(int it=0; it<act->nTeachers; it++){
							int tch=act->teachers[it];
							if(act->parity==PARITY_WEEKLY){
								teachersSubjectTags[tch][day][hour+dd]+=2;
							}
							else{
								assert(act->parity==PARITY_FORTNIGHTLY);
								teachersSubjectTags[tch][day][hour+dd]++;
							}
						}
				}
				
			for(int t=0; t<r.nInternalTeachers; t++){
				for(int d=0; d<r.nDaysPerWeek; d++){
					//Fortnightly activities are treated here as weekly ones, for the sake of simplicity					
					int n_cont_hours=0;
					for(int h=0; h<r.nHoursPerDay; h++){
						if(teachersSubjectTags[t][d][h]>0){
							n_cont_hours++;
							if(n_cont_hours>this->maxHoursContinuously){
								nbroken+=2;

								if(conflictsString!=NULL){
									(*conflictsString)+=(QObject::tr(
										"Time constraint teachers subject tags max %1 hours in a row broken for subject tag %2, teacher %3, on day %4, hour %5")
										.arg(QString::number(this->maxHoursContinuously))
										.arg(r.subjectTagsList.at(st)->name)
										.arg(r.internalTeachersList[t]->name)
										.arg(r.daysOfTheWeek[d])
										.arg(r.hoursOfTheDay[h])
										)
										+
								 		(QObject::tr(". This increases the conflicts total by %1").arg(QString::number(2*weight)))
										+ "\n";
								}
							}
						}
						else
							n_cont_hours=0;
					}
				}
			}
		}
	}
	
	return int (ceil ( weight * nbroken ) );
}

bool ConstraintTeachersSubjectTagsMaxHoursContinuously::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintTeachersSubjectTagsMaxHoursContinuously::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return true;
}

bool ConstraintTeachersSubjectTagsMaxHoursContinuously::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintTeachersSubjectTagsMaxHoursContinuously::isRelatedToSubjectTag(SubjectTag* s)
{
	if(s)
		;

	return true;
}

bool ConstraintTeachersSubjectTagsMaxHoursContinuously::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersSubjectTagMaxHoursContinuously::ConstraintTeachersSubjectTagMaxHoursContinuously()
	: TimeConstraint()
{
	type=CONSTRAINT_TEACHERS_SUBJECT_TAG_MAX_HOURS_CONTINUOUSLY;
}

ConstraintTeachersSubjectTagMaxHoursContinuously::ConstraintTeachersSubjectTagMaxHoursContinuously(double w, bool c, int maxhours, const QString& subjecttag)
 : TimeConstraint(w, c)
 {
	assert(maxhours>0);
	this->maxHoursContinuously=maxhours;
	this->subjectTagName=subjecttag;

	this->type=CONSTRAINT_TEACHERS_SUBJECT_TAG_MAX_HOURS_CONTINUOUSLY;
}

bool ConstraintTeachersSubjectTagMaxHoursContinuously::computeInternalStructure(Rules& r)
{
	this->subjectTagIndex=r.searchSubjectTag(this->subjectTagName);
	return true;
}

QString ConstraintTeachersSubjectTagMaxHoursContinuously::getXmlDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s="<ConstraintTeachersSubjectTagMaxHoursContinuously>\n";
	s+="	<Weight>"+QString::number(this->weight)+"</Weight>\n";
	s+="	<Compulsory>";s+=yesNo(this->compulsory);s+="</Compulsory>\n";
	s+="	<Maximum_Hours_Continuously>"+QString::number(this->maxHoursContinuously)+"</Maximum_Hours_Continuously>\n";
	s+="	<Subject_Tag>"+protect(this->subjectTagName)+"</Subject_Tag>\n";
	s+="</ConstraintTeachersSubjectTagMaxHoursContinuously>\n";
	return s;
}

QString ConstraintTeachersSubjectTagMaxHoursContinuously::getDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s;
	s+=QObject::tr("Teachers subject tag %1 max %2 hours in a row")
	 .arg(this->subjectTagName)
	 .arg(this->maxHoursContinuously);	 
	s+=", ";
	s+=(QObject::tr("W:%1").arg(this->weight));s+=", ";
	s+=(QObject::tr("C:%1").arg(yesNoTranslated(this->compulsory))); s+=", ";

	return s;
}

QString ConstraintTeachersSubjectTagMaxHoursContinuously::getDetailedDescription(Rules& r){
	if(&r!=NULL)
		;

	QString s=QObject::tr("Time constraint");s+="\n";
	s+=QObject::tr("Teachers subject tag %1 max %2 hours in a row")
	 .arg(this->subjectTagName)
	 .arg(this->maxHoursContinuously);
	s+="\n";
	s+=(QObject::tr("Weight=%1").arg(this->weight));
	s+="\n";
	s+=(QObject::tr("Compulsory=%1").arg(yesNoTranslated(this->compulsory)));
	s+="\n";
	s+=QObject::tr("Warning: this constraint dramatically slows down the execution of the program");
	s+="\n";

	return s;
}

//critical function here - must be optimized for speed
int ConstraintTeachersSubjectTagMaxHoursContinuously::fitness(TimeChromosome& c, Rules& r, QString* conflictsString)
{
	//if the matrices subgroupsMatrix and teachersMatrix are already calculated, do not calculate them again!
	if(crt_chrom!=&c || crt_rules!=&r || subgroups_conflicts<0 || teachers_conflicts<0 || c.timeChangedForMatrixCalculation){
		subgroups_conflicts = c.getSubgroupsMatrix(r, subgroupsMatrix);
		teachers_conflicts = c.getTeachersMatrix(r, teachersMatrix);

		crt_chrom=&c;
		crt_rules=&r;
		
		c.timeChangedForMatrixCalculation=false;
	}

	int nbroken;

	//without logging
	if(conflictsString==NULL){
		nbroken=0;
		for(int d=0; d<r.nDaysPerWeek; d++)
			for(int h=0; h<r.nHoursPerDay; h++)
				for(int t=0; t<r.nInternalTeachers; t++)
					teachersSubjectTags[t][d][h]=0;
						
		for(int i=0; i<r.nInternalActivities; i++)
			if(c.times[i]!=UNALLOCATED_TIME){
				Activity* act=&r.internalActivitiesList[i];
				if(act->subjectTagIndex!=this->subjectTagIndex)
					continue;
				int hour=c.times[i]/r.nDaysPerWeek;
				int day=c.times[i]%r.nDaysPerWeek;
				for(int dd=0; dd<act->duration && hour+dd<r.nHoursPerDay; dd++)
					for(int it=0; it<act->nTeachers; it++){
						int tch=act->teachers[it];
						if(act->parity==PARITY_WEEKLY){
							teachersSubjectTags[tch][day][hour+dd]+=2;
						}
						else{
							assert(act->parity==PARITY_FORTNIGHTLY);
							teachersSubjectTags[tch][day][hour+dd]++;
						}
					}
			}
				
		for(int t=0; t<r.nInternalTeachers; t++){
			for(int d=0; d<r.nDaysPerWeek; d++){
				//Fortnightly activities are treated here as weekly ones, for the sake of simplicity					
				int n_cont_hours=0;
				for(int h=0; h<r.nHoursPerDay; h++){
					if(teachersSubjectTags[t][d][h]>0){
						n_cont_hours++;
						if(n_cont_hours>this->maxHoursContinuously){
							nbroken+=2;
						}
					}
					else
						n_cont_hours=0;
				}
			}
		}
	}
	//with logging
	else{
		nbroken=0;
		for(int d=0; d<r.nDaysPerWeek; d++)
			for(int h=0; h<r.nHoursPerDay; h++)
				for(int t=0; t<r.nInternalTeachers; t++)
					teachersSubjectTags[t][d][h]=0;
						
		for(int i=0; i<r.nInternalActivities; i++)
			if(c.times[i]!=UNALLOCATED_TIME){
				Activity* act=&r.internalActivitiesList[i];
				if(act->subjectTagIndex!=this->subjectTagIndex)
					continue;
				int hour=c.times[i]/r.nDaysPerWeek;
				int day=c.times[i]%r.nDaysPerWeek;
				for(int dd=0; dd<act->duration && hour+dd<r.nHoursPerDay; dd++)
					for(int it=0; it<act->nTeachers; it++){
						int tch=act->teachers[it];
						if(act->parity==PARITY_WEEKLY){
							teachersSubjectTags[tch][day][hour+dd]+=2;
						}
						else{
							assert(act->parity==PARITY_FORTNIGHTLY);
							teachersSubjectTags[tch][day][hour+dd]++;
						}
					}
			}
				
		for(int t=0; t<r.nInternalTeachers; t++){
			for(int d=0; d<r.nDaysPerWeek; d++){
				//Fortnightly activities are treated here as weekly ones, for the sake of simplicity					
				int n_cont_hours=0;
				for(int h=0; h<r.nHoursPerDay; h++){
					if(teachersSubjectTags[t][d][h]>0){
						n_cont_hours++;
						if(n_cont_hours>this->maxHoursContinuously){
							nbroken+=2;

							if(conflictsString!=NULL){
								(*conflictsString)+=(QObject::tr(
									"Time constraint teachers subject tag max %1 hours in a row broken for subject tag %2, teacher %3, on day %4, hour %5")
									.arg(QString::number(this->maxHoursContinuously))
									.arg(this->subjectTagName)
									.arg(r.internalTeachersList[t]->name)
									.arg(r.daysOfTheWeek[d])
									.arg(r.hoursOfTheDay[h])
									)
									+
							 		(QObject::tr(". This increases the conflicts total by %1").arg(QString::number(2*weight)))
									+ "\n";
							}
						}
					}
					else
						n_cont_hours=0;
				}
			}
		}
	}
	
	return int (ceil ( weight * nbroken ) );
}

bool ConstraintTeachersSubjectTagMaxHoursContinuously::isRelatedToActivity(Activity* a)
{
	if(a)
		;

	return false;
}

bool ConstraintTeachersSubjectTagMaxHoursContinuously::isRelatedToTeacher(Teacher* t)
{
	if(t)
		;

	return true;
}

bool ConstraintTeachersSubjectTagMaxHoursContinuously::isRelatedToSubject(Subject* s)
{
	if(s)
		;

	return false;
}

bool ConstraintTeachersSubjectTagMaxHoursContinuously::isRelatedToSubjectTag(SubjectTag* s)
{
	return s->name==this->subjectTagName;
}

bool ConstraintTeachersSubjectTagMaxHoursContinuously::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	if(s)
		;
	if(&r)
		;

	return false;
}
