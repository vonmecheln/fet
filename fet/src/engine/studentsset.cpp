//
//
// C++ Implementation: $MODULE$
//
// Description:
//
//
// Author: Liviu Lalescu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "studentsset.h"
#include "rules.h"
#include "genetictimetable.h"

extern GeneticTimetable gt;

StudentsSet::StudentsSet()
{
	this->type=STUDENTS_SET;
	this->numberOfStudents=0;
}

StudentsSet::~StudentsSet()
{
}

StudentsYear::StudentsYear()
	: StudentsSet()
{
	this->type=STUDENTS_YEAR;
}

StudentsYear::~StudentsYear()
{
	//it is possible that the removed group to be in another year

	while(!groupsList.isEmpty()){
		StudentsGroup* g=groupsList[0];
		
		foreach(StudentsYear* year, gt.rules.yearsList)
			if(year!=this)
				for(int i=0; i<year->groupsList.size(); i++)
					if(year->groupsList[i]==g)
						year->groupsList[i]=NULL;
	
		if(g!=NULL)
			delete groupsList.takeFirst();
		else
			groupsList.removeFirst();
	}
}

QString StudentsYear::getXmlDescription()
{
	QString s="";
	s+="<Year>\n";
	s+="<Name>"+protect(this->name)+"</Name>\n";
	s+="<Number_of_Students>"+QString::number(this->numberOfStudents)+"</Number_of_Students>\n";
	for(int i=0; i<this->groupsList.size(); i++){
		StudentsGroup* stg=this->groupsList[i];
		s+=stg->getXmlDescription();
	}
	s+="</Year>\n";

	return s;
}

QString StudentsYear::getDescription()
{
	QString s="";
	s+=QObject::tr("Yn:");
	s+=this->name;
	s+=",";
	s+=QObject::tr("No:");
	s+=QString::number(this->numberOfStudents);

	return s;
}

QString StudentsYear::getDetailedDescription()
{
	QString s="";
	s+=QObject::tr("Students set - year");
	s+="\n";
	s+=QObject::tr("Year name=");
	s+=this->name;
	s+="\n";
	s+=QObject::tr("Number of students=");
	s+=QString::number(this->numberOfStudents);
	s+="\n";

	return s;
}

QString StudentsYear::getDetailedDescriptionWithConstraints(Rules& r)
{
	QString s=this->getDetailedDescription();

	s+="--------------------------------------------------\n";
	s+=QObject::tr("Time constraints directly related to this students year:");
	s+="\n";
	for(int i=0; i<r.timeConstraintsList.size(); i++){
		TimeConstraint* c=r.timeConstraintsList[i];
		if(c->isRelatedToStudentsSet(r, this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}

	s+="--------------------------------------------------\n";
	s+=QObject::tr("Space constraints directly related to this students year:");
	s+="\n";
	for(int i=0; i<r.spaceConstraintsList.size(); i++){
		SpaceConstraint* c=r.spaceConstraintsList[i];
		if(c->isRelatedToStudentsSet(r, this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}
	s+="--------------------------------------------------\n";

	return s;
}


StudentsGroup::StudentsGroup()
	: StudentsSet()
{
	this->type=STUDENTS_GROUP;
}

StudentsGroup::~StudentsGroup()
{
	while(!subgroupsList.isEmpty()){
		StudentsSubgroup* s=subgroupsList[0];
		
		foreach(StudentsYear* year, gt.rules.yearsList)
			foreach(StudentsGroup* group, year->groupsList)
				if(group!=this)
					for(int i=0; i<group->subgroupsList.size(); i++)
						if(group->subgroupsList[i]==s)
							group->subgroupsList[i]=NULL;
	
		if(s!=NULL)
			delete subgroupsList.takeFirst();
		else
			subgroupsList.removeFirst();
	}
}

QString StudentsGroup::getXmlDescription()
{
	QString s="";
	s+="	<Group>\n";
	s+="	<Name>"+protect(this->name)+"</Name>\n";
	s+="	<Number_of_Students>"+QString::number(this->numberOfStudents)+"</Number_of_Students>\n";
	for(int i=0; i<this->subgroupsList.size(); i++){
		StudentsSubgroup* sts=this->subgroupsList[i];
		s+=sts->getXmlDescription();
	}
	s+="	</Group>\n";

	return s;
}

QString StudentsGroup::getDescription()
{
	QString s="";
	s+=QObject::tr("Gn:");
	s+=this->name;
	s+=",";
	s+=QObject::tr("No:");
	s+=QString::number(this->numberOfStudents);

	return s;
}

QString StudentsGroup::getDetailedDescription()
{
	QString s="";
	s+=QObject::tr("Students set - group");
	s+="\n";
	s+=QObject::tr("Group name=");
	s+=this->name;
	s+="\n";
	s+=QObject::tr("Number of students=");
	s+=QString::number(this->numberOfStudents);
	s+="\n";

	return s;
}

QString StudentsGroup::getDetailedDescriptionWithConstraints(Rules& r)
{
	QString s=this->getDetailedDescription();

	s+="--------------------------------------------------\n";
	s+=QObject::tr("Time constraints directly related to this students group:");
	s+="\n";
	for(int i=0; i<r.timeConstraintsList.size(); i++){
		TimeConstraint* c=r.timeConstraintsList[i];
		if(c->isRelatedToStudentsSet(r, this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}

	s+="--------------------------------------------------\n";
	s+=QObject::tr("Space constraints directly related to this students group:");
	s+="\n";
	for(int i=0; i<r.spaceConstraintsList.size(); i++){
		SpaceConstraint* c=r.spaceConstraintsList[i];
		if(c->isRelatedToStudentsSet(r, this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}
	s+="--------------------------------------------------\n";

	return s;
}


StudentsSubgroup::StudentsSubgroup()
	: StudentsSet()
{
	this->type=STUDENTS_SUBGROUP;
}

StudentsSubgroup::~StudentsSubgroup()
{
}

QString StudentsSubgroup::getXmlDescription()
{
	QString s="";
	s+="		<Subgroup>\n";
	s+="		<Name>"+protect(this->name)+"</Name>\n";
	s+="		<Number_of_Students>"+QString::number(this->numberOfStudents)+"</Number_of_Students>\n";
	s+="		</Subgroup>\n";

	return s;
}

QString StudentsSubgroup::getDescription()
{
	QString s="";
	s+=QObject::tr("Sgn:");
	s+=this->name;
	s+=",";
	s+=QObject::tr("No:");
	s+=QString::number(this->numberOfStudents);

	return s;
}

QString StudentsSubgroup::getDetailedDescription()
{
	QString s="";
	s+=QObject::tr("Students set - subgroup");
	s+="\n";
	s+=QObject::tr("Subgroup name=");
	s+=this->name;
	s+="\n";
	s+=QObject::tr("Number of students=");
	s+=QString::number(this->numberOfStudents);
	s+="\n";

	return s;
}

QString StudentsSubgroup::getDetailedDescriptionWithConstraints(Rules& r)
{
	QString s=this->getDetailedDescription();

	s+="--------------------------------------------------\n";
	s+=QObject::tr("Time constraints directly related to this students subgroup:");
	s+="\n";
	for(int i=0; i<r.timeConstraintsList.size(); i++){
		TimeConstraint* c=r.timeConstraintsList[i];
		if(c->isRelatedToStudentsSet(r, this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}

	s+="--------------------------------------------------\n";
	s+=QObject::tr("Space constraints directly related to this students subgroup:");
	s+="\n";
	for(int i=0; i<r.spaceConstraintsList.size(); i++){
		SpaceConstraint* c=r.spaceConstraintsList[i];
		if(c->isRelatedToStudentsSet(r, this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}
	s+="--------------------------------------------------\n";

	return s;
}

int yearsAscending(const StudentsYear* y1, const StudentsYear* y2)
{
	return y1->name < y2->name;
}

int groupsAscending(const StudentsGroup* g1, const StudentsGroup* g2)
{
	return g1->name < g2->name;
}

int subgroupsAscending(const StudentsSubgroup* s1, const StudentsSubgroup* s2)
{
	return s1->name < s2->name;
}
