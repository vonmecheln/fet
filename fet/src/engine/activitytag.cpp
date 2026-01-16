//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2005 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "activitytag.h"
#include "rules.h"

#include <QDataStream>

QDataStream& operator<<(QDataStream& stream, const ActivityTag& at)
{
	stream<<at.name;
	stream<<at.longName;
	stream<<at.code;
	stream<<at.printable;
	stream<<at.comments;

	return stream;
}

QDataStream& operator>>(QDataStream& stream, ActivityTag& at)
{
	stream>>at.name;
	stream>>at.longName;
	stream>>at.code;
	stream>>at.printable;
	stream>>at.comments;

	return stream;
}

ActivityTag::ActivityTag()
{
	longName=QString("");
	code=QString("");

	printable=true;
	comments=QString("");
}

ActivityTag::~ActivityTag()
{
}

QString ActivityTag::getXmlDescription()
{
	QString s=IL2+"<Activity_Tag>\n";

	s+=IL3+"<Name>"+protect(this->name)+"</Name>\n";
	s+=IL3+"<Long_Name>"+protect(this->longName)+"</Long_Name>\n";
	s+=IL3+"<Code>"+protect(this->code)+"</Code>\n";
	
	s+=IL3+"<Printable>";
	if(this->printable)
		s+="true";
	else
		s+="false";
	s+="</Printable>\n";
	
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</Activity_Tag>\n";

	return s;
}

QString ActivityTag::getDescription()
{
	QString s=tr("N:%1", "The (short) name of the activity tag").arg(name);
	s+=translatedCommaSpace();

	s+=tr("LN:%1", "The long name of the activity tag").arg(longName);
	s+=translatedCommaSpace();

	s+=tr("C:%1", "The code of the activity tag").arg(code);
	s+=translatedCommaSpace();

	QString printableYesNo;
	if(this->printable)
		printableYesNo=tr("yes");
	else
		printableYesNo=tr("no");
	s+=tr("P:%1", "Whether the activity tag is Printable - can be true or false").arg(printableYesNo);
	
	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);
	
	return s+end;
}

QString ActivityTag::getDetailedDescription()
{
	QString s=tr("Activity tag");
	s+="\n";
	s+=tr("Name=%1", "The (short) name of the activity tag").arg(this->name);
	s+="\n";

	s+=tr("Long name=%1", "The long name of the activity tag").arg(this->longName);
	s+="\n";

	s+=tr("Code=%1", "The code of the activity tag").arg(this->code);
	s+="\n";

	QString printableYesNo;
	if(this->printable)
		printableYesNo=tr("yes");
	else
		printableYesNo=tr("no");
	s+=tr("Printable=%1", "Whether the activity tag is Printable - can be true or false").arg(printableYesNo);
	s+="\n";

	//Has comments?
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

QString ActivityTag::getDetailedDescriptionWithConstraints(Rules& r)
{
	QString s=this->getDetailedDescription();

	s+="--------------------------------------------------\n";
	s+=tr("Time constraints directly related to this activity tag:");
	s+="\n";
	for(int i=0; i<r.timeConstraintsList.size(); i++){
		TimeConstraint* c=r.timeConstraintsList[i];
		if(c->isRelatedToActivityTag(this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}

	s+="--------------------------------------------------\n";
	s+=tr("Space constraints directly related to this activity tag:");
	s+="\n";
	for(int i=0; i<r.spaceConstraintsList.size(); i++){
		SpaceConstraint* c=r.spaceConstraintsList[i];
		if(c->isRelatedToActivityTag(this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}
	s+="--------------------------------------------------\n";

	return s;
}

int activityTagsAscending(const ActivityTag* st1, const ActivityTag* st2)
{
	//return st1->name < st2->name;
	
	//by Rodolfo Ribeiro Gomes
	return st1->name.localeAwareCompare(st2->name)<0;
}

int activityTagsCodesAscending(const ActivityTag* st1, const ActivityTag* st2)
{
	//return st1->name < st2->name;
	
	//by Rodolfo Ribeiro Gomes
	return st1->code.localeAwareCompare(st2->code)<0;
}
