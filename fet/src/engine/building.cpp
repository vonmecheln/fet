//
//
// C++ Implementation: $MODULE$
//
// Description: 
//
//
// Author: Liviu Lalescu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "building.h"
#include "rules.h"

Building::Building()
{
}

Building::~Building()
{
}

QString Building::getDescription()
{
	QString s=QObject::tr("N:");
	s+=this->name;

	return s;
}

QString Building::getDetailedDescription()
{
	QString s=QObject::tr("Name=");
	s+=this->name;
	s+="\n";

	return s;
}

QString Building::getXmlDescription()
{
	QString s="<Building>\n";
	s+="	<Name>"+protect(this->name)+"</Name>\n";
	s+="</Building>\n";

	return s;
}

QString Building::getDetailedDescriptionWithConstraints(Rules& r)
{
	QString s=this->getDetailedDescription();

	s+="--------------------------------------------------\n";
	s+=QObject::tr("Space constraints directly related to this building:");
	s+="\n";
	for(int i=0; i<r.spaceConstraintsList.size(); i++){
		SpaceConstraint* c=r.spaceConstraintsList[i];
		if(c->isRelatedToBuilding(this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}
	s+="--------------------------------------------------\n";

	return s;
}

int buildingsAscending (const Building* b1, const Building* b2)
{
	return b1->name < b2->name;
}
