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

#include "building.h"
#include "rules.h"

#include <QDataStream>

QDataStream& operator<<(QDataStream& stream, const Building& bd)
{
	stream<<bd.name;
	stream<<bd.longName;
	stream<<bd.code;
	stream<<bd.comments;

	return stream;
}

QDataStream& operator>>(QDataStream& stream, Building& bd)
{
	stream>>bd.name;
	stream>>bd.longName;
	stream>>bd.code;
	stream>>bd.comments;

	return stream;
}

Building::Building()
{
	longName=QString("");
	code=QString("");

	comments=QString("");
}

Building::~Building()
{
}

void Building::computeInternalStructure(Rules& r)
{
	Q_UNUSED(r);
}

QString Building::getDescription()
{
	QString s=tr("N:%1", "The (short) name of the building").arg(name);
	
	s+=translatedCommaSpace();
	s+=tr("LN:%1", "The long name of the building").arg(longName);

	s+=translatedCommaSpace();
	s+=tr("C:%1", "The code of the building").arg(code);
	
	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);
	
	return s+end;
}

QString Building::getDetailedDescription()
{
	QString s=tr("Building");
	s+="\n";
	s+=tr("Name=%1", "The name of the building").arg(this->name);
	s+="\n";

	s+=tr("Long name=%1", "The long name of the building").arg(this->longName);
	s+="\n";

	s+=tr("Code=%1", "The code of the building").arg(this->code);
	s+="\n";

	//Has comments?
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

QString Building::getXmlDescription()
{
	QString s=IL2+"<Building>\n";
	s+=IL3+"<Name>"+protect(this->name)+"</Name>\n";
	s+=IL3+"<Long_Name>"+protect(this->longName)+"</Long_Name>\n";
	s+=IL3+"<Code>"+protect(this->code)+"</Code>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</Building>\n";

	return s;
}

QString Building::getDetailedDescriptionWithConstraints(Rules& r)
{
	Q_UNUSED(r);

	QString s=this->getDetailedDescription();

	/*s+="--------------------------------------------------\n";
	s+=tr("Space constraints directly related to this building:");
	s+="\n";
	for(int i=0; i<r.spaceConstraintsList.size(); i++){
		SpaceConstraint* c=r.spaceConstraintsList[i];
		if(c->isRelatedToBuilding(this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}
	s+="--------------------------------------------------\n";*/

	return s;
}

int buildingsAscending (const Building* b1, const Building* b2)
{
	//return b1->name < b2->name;
	
	//by Rodolfo Ribeiro Gomes
	return b1->name.localeAwareCompare(b2->name)<0;
}
