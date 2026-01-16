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

#include "subject.h"
#include "rules.h"

#include <QDataStream>

QDataStream& operator<<(QDataStream& stream, const Subject& sbj)
{
	stream<<sbj.name;
	stream<<sbj.longName;
	stream<<sbj.code;
	stream<<sbj.comments;

	return stream;
}

QDataStream& operator>>(QDataStream& stream, Subject& sbj)
{
	stream>>sbj.name;
	stream>>sbj.longName;
	stream>>sbj.code;
	stream>>sbj.comments;

	return stream;
}

Subject::Subject()
{
	longName=QString("");
	code=QString("");

	comments=QString("");
}

Subject::~Subject()
{
}

QString Subject::getXmlDescription()
{
	QString s=IL2+"<Subject>\n";
	s+=IL3+"<Name>"+protect(this->name)+"</Name>\n";
	s+=IL3+"<Long_Name>"+protect(this->longName)+"</Long_Name>\n";
	s+=IL3+"<Code>"+protect(this->code)+"</Code>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</Subject>\n";

	return s;
}

QString Subject::getDescription()
{
	QString s=tr("N:%1", "The (short) name of the subject").arg(name);
	s+=translatedCommaSpace();
	s+=tr("LN:%1", "The long name of the subject").arg(longName);
	s+=translatedCommaSpace();
	s+=tr("C:%1", "The code of the subject").arg(code);
	
	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);
	
	return s+end;
}

QString Subject::getDetailedDescription()
{
	QString s=tr("Subject");
	s+="\n";
	s+=tr("Name=%1", "The (short) name of the subject").arg(this->name);
	s+="\n";
	s+=tr("Long name=%1", "The long name of the subject").arg(this->longName);
	s+="\n";
	s+=tr("Code=%1", "The code of the subject").arg(this->code);
	s+="\n";

	//Has comments?
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

QString Subject::getDetailedDescriptionWithConstraints(Rules& r)
{
	QString s=this->getDetailedDescription();

	s+="--------------------------------------------------\n";
	s+=tr("Time constraints directly related to this subject:");
	s+="\n";
	for(int i=0; i<r.timeConstraintsList.size(); i++){
		TimeConstraint* c=r.timeConstraintsList[i];
		if(c->isRelatedToSubject(this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}

	s+="--------------------------------------------------\n";
	s+=tr("Space constraints directly related to this subject:");
	s+="\n";
	for(int i=0; i<r.spaceConstraintsList.size(); i++){
		SpaceConstraint* c=r.spaceConstraintsList[i];
		if(c->isRelatedToSubject(this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}
	s+="--------------------------------------------------\n";

	return s;
}

int subjectsAscending(const Subject* s1, const Subject* s2)
{
	//return s1->name < s2->name;
	
	//by Rodolfo Ribeiro Gomes
	return s1->name.localeAwareCompare(s2->name)<0;
}

int subjectsCodesAscending(const Subject* s1, const Subject* s2)
{
	//return s1->name < s2->name;
	
	//by Rodolfo Ribeiro Gomes
	return s1->code.localeAwareCompare(s2->code)<0;
}
