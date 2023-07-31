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

#include "teacher.h"
#include "rules.h"

Teacher::Teacher()
{
	targetNumberOfHours=0;
	comments=QString("");
	qualifiedSubjectsList.clear();
	qualifiedSubjectsHash.clear();

	morningsAfternoonsBehavior=TEACHER_MORNINGS_AFTERNOONS_BEHAVIOR_NOT_INITIALIZED;
}

Teacher::~Teacher()
{
}

QString Teacher::getXmlDescription(const Rules& r)
{
	QString s="<Teacher>\n";
	s+="	<Name>"+protect(this->name)+"</Name>\n";

	if(r.mode==MORNINGS_AFTERNOONS){
		s+="	<Mornings_Afternoons_Behavior>";
		if(morningsAfternoonsBehavior==TEACHER_UNRESTRICTED_MORNINGS_AFTERNOONS)
			s+="Unrestricted";
		else if(morningsAfternoonsBehavior==TEACHER_MORNING_OR_EXCLUSIVELY_AFTERNOON)
			s+="Exclusive";
		else if(morningsAfternoonsBehavior==TEACHER_ONE_DAY_EXCEPTION)
			s+="One day exception";
		else if(morningsAfternoonsBehavior==TEACHER_TWO_DAYS_EXCEPTION)
			s+="Two days exception";
		else if(morningsAfternoonsBehavior==TEACHER_THREE_DAYS_EXCEPTION)
			s+="Three days exception";
		else if(morningsAfternoonsBehavior==TEACHER_FOUR_DAYS_EXCEPTION)
			s+="Four days exception";
		else if(morningsAfternoonsBehavior==TEACHER_FIVE_DAYS_EXCEPTION)
			s+="Five days exception";
		else
			assert(0);
		s+="</Mornings_Afternoons_Behavior>\n";
	}

	s+="	<Target_Number_of_Hours>"+CustomFETString::number(targetNumberOfHours)+"</Target_Number_of_Hours>\n";
	s+="	<Qualified_Subjects>\n";
	for(const QString& sbj : qAsConst(qualifiedSubjectsList))
		s+="		<Qualified_Subject>"+protect(sbj)+"</Qualified_Subject>\n";
	s+="	</Qualified_Subjects>\n";
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	s+="</Teacher>\n";

	return s;
}

QString Teacher::getDescription(const Rules& r)
{
	QString s=tr("N:%1", "The name of the teacher").arg(name);

	if(r.mode==MORNINGS_AFTERNOONS){
		QString mab;
		if(morningsAfternoonsBehavior==TEACHER_UNRESTRICTED_MORNINGS_AFTERNOONS)
			mab=tr("UNR", "Unrestricted mornings/afternoons");
		else if(morningsAfternoonsBehavior==TEACHER_MORNING_OR_EXCLUSIVELY_AFTERNOON)
			mab=tr("EXCL", "Exclusive mornings/afternoons");
		else if(morningsAfternoonsBehavior==TEACHER_ONE_DAY_EXCEPTION)
			mab=tr("1DE", "One day exception");
		else if(morningsAfternoonsBehavior==TEACHER_TWO_DAYS_EXCEPTION)
			mab=tr("2DE", "Two days exception");
		else if(morningsAfternoonsBehavior==TEACHER_THREE_DAYS_EXCEPTION)
			mab=tr("3DE", "Three days exception");
		else if(morningsAfternoonsBehavior==TEACHER_FOUR_DAYS_EXCEPTION)
			mab=tr("4DE", "Four days exception");
		else if(morningsAfternoonsBehavior==TEACHER_FIVE_DAYS_EXCEPTION)
			mab=tr("5DE", "Five days exception");
		else
			assert(0);
		s+=", ";
		s+=tr("MAB: %1", "Mornings-afternoons behavior").arg(mab);
	}
	
	QString end=QString("");
	if(!comments.isEmpty())
		end=", "+tr("C: %1", "Comments").arg(comments);
	
	return s+end;
}

QString Teacher::getDetailedDescription(const Rules& r)
{
	QString s=tr("Teacher");
	s+="\n";
	s+=tr("Name=%1", "The name of the teacher").arg(this->name);
	s+="\n";

	if(r.mode==MORNINGS_AFTERNOONS){
		QString mab;
		if(morningsAfternoonsBehavior==TEACHER_UNRESTRICTED_MORNINGS_AFTERNOONS)
			mab=tr("Unrestricted mornings/afternoons");
		else if(morningsAfternoonsBehavior==TEACHER_MORNING_OR_EXCLUSIVELY_AFTERNOON)
			mab=tr("Exclusive mornings/afternoons");
		else if(morningsAfternoonsBehavior==TEACHER_ONE_DAY_EXCEPTION)
			mab=tr("One day exception");
		else if(morningsAfternoonsBehavior==TEACHER_TWO_DAYS_EXCEPTION)
			mab=tr("Two days exception");
		else if(morningsAfternoonsBehavior==TEACHER_THREE_DAYS_EXCEPTION)
			mab=tr("Three days exception");
		else if(morningsAfternoonsBehavior==TEACHER_FOUR_DAYS_EXCEPTION)
			mab=tr("Four days exception");
		else if(morningsAfternoonsBehavior==TEACHER_FIVE_DAYS_EXCEPTION)
			mab=tr("Five days exception");
		else
			assert(0);
		s+=tr("Mornings-afternoons behavior=%1").arg(mab);
		s+="\n";
	}
	
	s+=tr("Target number of hours=%1", "The target number of hours for the teacher").arg(targetNumberOfHours);
	s+="\n";

	s+=tr("Qualified subjects:", "The list of qualified subjects for a teacher");
	s+="\n";
	for(const QString& sbj : qAsConst(qualifiedSubjectsList)){
		s+=sbj;
		s+="\n";
	}

	//Has comments?
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

QString Teacher::getDetailedDescriptionWithConstraints(Rules& r)
{
	QString s=this->getDetailedDescription(r);

	s+="--------------------------------------------------\n";
	s+=tr("Time constraints directly related to this teacher:");
	s+="\n";
	for(int i=0; i<r.timeConstraintsList.size(); i++){
		TimeConstraint* c=r.timeConstraintsList[i];
		if(c->isRelatedToTeacher(this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}

	s+="--------------------------------------------------\n";
	s+=tr("Space constraints directly related to this teacher:");
	s+="\n";
	for(int i=0; i<r.spaceConstraintsList.size(); i++){
		SpaceConstraint* c=r.spaceConstraintsList[i];
		if(c->isRelatedToTeacher(this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}
	s+="--------------------------------------------------\n";

	return s;
}

int teachersAscending(const Teacher* t1, const Teacher* t2)
{
	//return t1->name < t2->name;
	
	//by Rodolfo Ribeiro Gomes
	return t1->name.localeAwareCompare(t2->name)<0;
}
