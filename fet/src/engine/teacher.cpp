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
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "teacher.h"
#include "rules.h"

#include <QDataStream>

QDataStream& operator<<(QDataStream& stream, const Teacher& tch)
{
	stream<<tch.name;
	stream<<tch.longName;
	stream<<tch.code;
	stream<<tch.morningsAfternoonsBehavior;
	stream<<tch.targetNumberOfHours;
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
	stream<<QList<QString>(tch.qualifiedSubjectsList.cbegin(), tch.qualifiedSubjectsList.cend());
#else
	stream<<QList<QString>::fromStdList(tch.qualifiedSubjectsList);
#endif
	stream<<tch.comments;

	return stream;
}

QDataStream& operator>>(QDataStream& stream, Teacher& tch)
{
	stream>>tch.name;
	stream>>tch.longName;
	stream>>tch.code;
	stream>>tch.morningsAfternoonsBehavior;
	stream>>tch.targetNumberOfHours;

	QList<QString> tl;
	stream>>tl;
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
	tch.qualifiedSubjectsList=std::list<QString>(tl.constBegin(), tl.constEnd());
#else
	tch.qualifiedSubjectsList=tl.toStdList();
#endif

	stream>>tch.comments;

	return stream;
}

Teacher::Teacher()
{
	longName=QString("");
	code=QString("");

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
	QString s=IL2+"<Teacher>\n";
	s+=IL3+"<Name>"+protect(this->name)+"</Name>\n";
	s+=IL3+"<Long_Name>"+protect(this->longName)+"</Long_Name>\n";
	s+=IL3+"<Code>"+protect(this->code)+"</Code>\n";

	if(r.mode==MORNINGS_AFTERNOONS){
		s+=IL3+"<Mornings_Afternoons_Behavior>";
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

	s+=IL3+"<Target_Number_of_Hours>"+CustomFETString::number(targetNumberOfHours)+"</Target_Number_of_Hours>\n";
	s+=IL3+"<Qualified_Subjects>\n";
	for(const QString& sbj : std::as_const(qualifiedSubjectsList))
		s+=IL4+"<Qualified_Subject>"+protect(sbj)+"</Qualified_Subject>\n";
	s+=IL3+"</Qualified_Subjects>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</Teacher>\n";

	return s;
}

QString Teacher::getDescription(const Rules& r)
{
	QString s=tr("N:%1", "The (short) name of the teacher").arg(name);
	s+=translatedCommaSpace();
	s+=tr("LN:%1", "The long name of the teacher").arg(longName);
	s+=translatedCommaSpace();
	s+=tr("C:%1", "The code of the teacher").arg(code);

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
		s+=translatedCommaSpace();
		s+=tr("MAB: %1", "Mornings-afternoons behavior").arg(mab);
	}
	
	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);
	
	return s+end;
}

QString Teacher::getDetailedDescription(const Rules& r)
{
	QString s=tr("Teacher");
	s+="\n";
	s+=tr("Name=%1", "The (short) name of the teacher").arg(this->name);
	s+="\n";
	s+=tr("Long name=%1", "The long name of the teacher").arg(this->longName);
	s+="\n";
	s+=tr("Code=%1", "The code of the teacher").arg(this->code);
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
	for(const QString& sbj : std::as_const(qualifiedSubjectsList)){
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

QString Teacher::getDetailedDescriptionWithNumberOfActiveHours(const Rules& r, const QHash<QString, int>& activeHoursHash)
{
	QString s=tr("Teacher");
	s+="\n";
	s+=tr("Name=%1", "The (short) name of the teacher").arg(this->name);
	s+="\n";
	s+=tr("Long name=%1", "The long name of the teacher").arg(this->longName);
	s+="\n";
	s+=tr("Code=%1", "The code of the teacher").arg(this->code);
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
	
	assert(activeHoursHash.contains(this->name));
	int acthours=activeHoursHash.value(this->name);
	s+=tr("Number of active hours=%1", "The total duration of the active activities for the teacher").arg(acthours);
	s+="\n";

	s+=tr("Target number of hours=%1", "The target number of hours for the teacher").arg(targetNumberOfHours);
	s+="\n";

	s+=tr("Qualified subjects:", "The list of qualified subjects for a teacher");
	s+="\n";
	for(const QString& sbj : std::as_const(qualifiedSubjectsList)){
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

QString Teacher::getDetailedDescriptionWithConstraintsAndNumberOfActiveHours(Rules& r, const QHash<QString, int>& activeHoursHash)
{
	QString s=this->getDetailedDescriptionWithNumberOfActiveHours(r, activeHoursHash);

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

int teachersCodesAscending(const Teacher* t1, const Teacher* t2)
{
	//return t1->name < t2->name;
	
	//by Rodolfo Ribeiro Gomes
	return t1->code.localeAwareCompare(t2->code)<0;
}

void Teacher::recomputeQualifiedSubjectsHash()
{
	qualifiedSubjectsHash.clear();
	//I don't know what it does not work with const_iterator, cbegin(), and cend().
	for(std::list<QString>::iterator it=qualifiedSubjectsList.begin(); it!=qualifiedSubjectsList.end(); it++){
		assert(!qualifiedSubjectsHash.contains(*it));
		qualifiedSubjectsHash.insert(*it, it);
	}
}
