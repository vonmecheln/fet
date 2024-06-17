//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2003 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "studentsset.h"
#include "rules.h"
#include "timetable.h"

//#include <QDataStream>

extern Timetable gt;

/*QDataStream& operator<<(QDataStream& stream, const StudentsYear& year)
{
	stream<<year.name;
	stream<<year.numberOfStudents;
	stream<<year.type;

	stream<<year.comments;

	int ng=year.groupsList.count(); //convert count() to int
	stream<<ng;
	for(int i=0; i<ng; i++)
		stream<<*year.groupsList.at(i);

	stream<<year.divisions;
	stream<<year.separator; //The separator when dividing a year by categories.

	return stream;
}

QDataStream& operator>>(QDataStream& stream, StudentsYear& year){
	stream>>year.name;
	stream>>year.numberOfStudents;
	stream>>year.type;

	stream>>year.comments;

	int cntg;
	stream>>cntg;
	for(int i=0; i<cntg; i++){
		StudentsGroup* group=new StudentsGroup;
		stream>>*group;
		year.groupsList.append(group);
	}

	stream>>year.divisions;
	stream>>year.separator; //The separator when dividing a year by categories.

	return stream;
}

QDataStream& operator<<(QDataStream& stream, const StudentsGroup& group)
{
	stream<<group.name;
	stream<<group.numberOfStudents;
	stream<<group.type;

	stream<<group.comments;

	int ns=group.subgroupsList.count(); //convert count() to int
	stream<<ns;
	for(int i=0; i<ns; i++)
		stream<<*group.subgroupsList.at(i);

	return stream;
}

QDataStream& operator>>(QDataStream& stream, StudentsGroup& group)
{
	stream>>group.name;
	stream>>group.numberOfStudents;
	stream>>group.type;

	stream>>group.comments;

	int cnts;
	stream>>cnts;
	for(int i=0; i<cnts; i++){
		StudentsSubgroup* subgroup=new StudentsSubgroup;
		stream>>*subgroup;
		group.subgroupsList.append(subgroup);
	}

	return stream;
}

QDataStream& operator<<(QDataStream& stream, const StudentsSubgroup& subgroup)
{
	stream<<subgroup.name;
	stream<<subgroup.numberOfStudents;
	stream<<subgroup.type;

	stream<<subgroup.comments;

	return stream;
}

QDataStream& operator>>(QDataStream& stream, StudentsSubgroup& subgroup)
{
	stream>>subgroup.name;
	stream>>subgroup.numberOfStudents;
	stream>>subgroup.type;

	stream>>subgroup.comments;

	return stream;
}*/

StudentsSet::StudentsSet()
{
	longName=QString("");
	code=QString("");

	this->type=STUDENTS_SET;
	this->numberOfStudents=0;
	comments=QString("");
}

StudentsSet::~StudentsSet()
{
}

StudentsYear::StudentsYear()
	: StudentsSet()
{
	this->type=STUDENTS_YEAR;
	
	divisions.clear();
	separator=QString(" ");

	indexInAugmentedYearsList=-1;
}

StudentsYear::~StudentsYear()
{
}

QString StudentsYear::getXmlDescription()
{
	QString s="";
	s+="<Year>\n";
	s+="	<Name>"+protect(this->name)+"</Name>\n";
	s+="	<Long_Name>"+protect(this->longName)+"</Long_Name>\n";
	s+="	<Code>"+protect(this->code)+"</Code>\n";
	s+="	<Number_of_Students>"+CustomFETString::number(this->numberOfStudents)+"</Number_of_Students>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	
	s+="	<!-- The information regarding categories, divisions of each category, and separator is only used in the divide year automatically by categories dialog. -->\n";
	s+="	<Number_of_Categories>"+QString::number(divisions.count())+"</Number_of_Categories>\n";
	for(const QStringList& tl : std::as_const(divisions)){
		s+="	<Category>\n";
		s+="		<Number_of_Divisions>"+QString::number(tl.count())+"</Number_of_Divisions>\n";
		for(const QString& dn : std::as_const(tl))
			s+="		<Division>"+protect(dn)+"</Division>\n";
		s+="	</Category>\n";
	}
	s+="	<Separator>"+protect(separator)+"</Separator>\n";
	
	for(int i=0; i<this->groupsList.size(); i++){
		StudentsGroup* stg=this->groupsList[i];
		s+=stg->getXmlDescription();
	}
	s+="</Year>\n";

	return s;
}

QString StudentsYear::getDescription()
{
	QString s;
	s+=tr("YN:%1", "Year (short) name").arg(this->name);
	s+=", ";
	s+=tr("YLN:%1", "Year long name").arg(this->longName);
	s+=", ";
	s+=tr("YC:%1", "Year code").arg(this->code);
	s+=", ";
	s+=tr("NoS:%1", "Number of students").arg(this->numberOfStudents);
	
	QString end=QString("");
	if(!comments.isEmpty())
		end=QString(", ")+tr("C: %1", "Comments").arg(comments);

	return s+end;
}

QString StudentsYear::getDetailedDescription()
{
	QString s="";
	s+=tr("Students set - year");
	s+="\n";
	s+=tr("Year name=%1").arg(this->name);
	s+="\n";
	s+=tr("Year long name=%1").arg(this->longName);
	s+="\n";
	s+=tr("Year code=%1").arg(this->code);
	s+="\n";
	s+=tr("Number of students=%1").arg(this->numberOfStudents);
	s+="\n";

	//Has comments?
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

QString StudentsYear::getDetailedDescriptionWithConstraints(Rules& r)
{
	QString s=this->getDetailedDescription();

	s+="--------------------------------------------------\n";
	s+=tr("Time constraints directly related to this students year:");
	s+="\n";
	for(int i=0; i<r.timeConstraintsList.size(); i++){
		TimeConstraint* c=r.timeConstraintsList[i];
		if(c->isRelatedToStudentsSet(r, this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}

	s+="--------------------------------------------------\n";
	s+=tr("Space constraints directly related to this students year:");
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

	indexInInternalGroupsList=-1;
}

StudentsGroup::~StudentsGroup()
{
}

QString StudentsGroup::getXmlDescription()
{
	QString s="";
	s+="	<Group>\n";
	s+="		<Name>"+protect(this->name)+"</Name>\n";
	s+="		<Long_Name>"+protect(this->longName)+"</Long_Name>\n";
	s+="		<Code>"+protect(this->code)+"</Code>\n";
	s+="		<Number_of_Students>"+CustomFETString::number(this->numberOfStudents)+"</Number_of_Students>\n";
	s+="		<Comments>"+protect(comments)+"</Comments>\n";
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
	s+=tr("GN:%1", "Group (short) name").arg(this->name);
	s+=", ";
	s+=tr("GLN:%1", "Group long name").arg(this->longName);
	s+=", ";
	s+=tr("GC:%1", "Group code").arg(this->code);
	s+=", ";
	s+=tr("NoS:%1", "Number of students").arg(this->numberOfStudents);

	QString end=QString("");
	if(!comments.isEmpty())
		end=QString(", ")+tr("C: %1", "Comments").arg(comments);

	return s+end;
}

QString StudentsGroup::getDetailedDescription()
{
	QString s="";
	s+=tr("Students set - group");
	s+="\n";
	s+=tr("Group name=%1").arg(this->name);
	s+="\n";
	s+=tr("Group long name=%1").arg(this->longName);
	s+="\n";
	s+=tr("Group code=%1").arg(this->code);
	s+="\n";
	s+=tr("Number of students=%1").arg(this->numberOfStudents);
	s+="\n";

	//Has comments?
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

QString StudentsGroup::getDetailedDescriptionWithConstraints(Rules& r)
{
	QString s=this->getDetailedDescription();

	s+="--------------------------------------------------\n";
	s+=tr("Time constraints directly related to this students group:");
	s+="\n";
	for(int i=0; i<r.timeConstraintsList.size(); i++){
		TimeConstraint* c=r.timeConstraintsList[i];
		if(c->isRelatedToStudentsSet(r, this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}

	s+="--------------------------------------------------\n";
	s+=tr("Space constraints directly related to this students group:");
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
	
	indexInInternalSubgroupsList=-1;
}

StudentsSubgroup::~StudentsSubgroup()
{
}

QString StudentsSubgroup::getXmlDescription()
{
	QString s="";
	s+="		<Subgroup>\n";
	s+="			<Name>"+protect(this->name)+"</Name>\n";
	s+="			<Long_Name>"+protect(this->longName)+"</Long_Name>\n";
	s+="			<Code>"+protect(this->code)+"</Code>\n";
	s+="			<Number_of_Students>"+CustomFETString::number(this->numberOfStudents)+"</Number_of_Students>\n";
	s+="			<Comments>"+protect(comments)+"</Comments>\n";
	s+="		</Subgroup>\n";

	return s;
}

QString StudentsSubgroup::getDescription()
{
	QString s="";
	s+=tr("SgN:%1", "Subgroup (short) name").arg(this->name);
	s+=", ";
	s+=tr("SgLN:%1", "Subgroup long name").arg(this->longName);
	s+=", ";
	s+=tr("SgC:%1", "Subgroup code").arg(this->code);
	s+=", ";
	s+=tr("NoS:%1", "Number of students").arg(this->numberOfStudents);

	QString end=QString("");
	if(!comments.isEmpty())
		end=QString(", ")+tr("C: %1", "Comments").arg(comments);

	return s+end;
}

QString StudentsSubgroup::getDetailedDescription()
{
	QString s="";
	s+=tr("Students set - subgroup");
	s+="\n";
	s+=tr("Subgroup name=%1").arg(this->name);
	s+="\n";
	s+=tr("Subgroup long name=%1").arg(this->longName);
	s+="\n";
	s+=tr("Subgroup code=%1").arg(this->code);
	s+="\n";
	s+=tr("Number of students=%1").arg(this->numberOfStudents);
	s+="\n";

	//Has comments?
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

QString StudentsSubgroup::getDetailedDescriptionWithConstraints(Rules& r)
{
	QString s=this->getDetailedDescription();

	s+="--------------------------------------------------\n";
	s+=tr("Time constraints directly related to this students subgroup:");
	s+="\n";
	for(int i=0; i<r.timeConstraintsList.size(); i++){
		TimeConstraint* c=r.timeConstraintsList[i];
		if(c->isRelatedToStudentsSet(r, this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}

	s+="--------------------------------------------------\n";
	s+=tr("Space constraints directly related to this students subgroup:");
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
	//return y1->name < y2->name;
	
	//by Rodolfo Ribeiro Gomes
	return y1->name.localeAwareCompare(y2->name)<0;
}

int groupsAscending(const StudentsGroup* g1, const StudentsGroup* g2)
{
	//return g1->name < g2->name;
	
	//by Rodolfo Ribeiro Gomes
	return g1->name.localeAwareCompare(g2->name)<0;
}

int subgroupsAscending(const StudentsSubgroup* s1, const StudentsSubgroup* s2)
{
	//return s1->name < s2->name;
	
	//by Rodolfo Ribeiro Gomes
	return s1->name.localeAwareCompare(s2->name)<0;
}
