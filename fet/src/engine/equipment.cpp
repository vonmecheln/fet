//
//
// C++ Interface: $MODULE$
//
// Description: 
//
//
// Author: Liviu Lalescu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "equipment.h"
#include "rules.h"

Equipment::Equipment()
{
}

Equipment::~Equipment()
{
}

QString Equipment::getDescription()
{
	QString s=QObject::tr("N:%1").arg(this->name);
	
	return s;
}

QString Equipment::getDetailedDescription()
{
	QString s=QObject::tr("Name=%1").arg(this->name);
	s+="\n";
	
	return s;
}

QString Equipment::getXmlDescription()
{
	QString s="<Equipment>\n";
	s+="	<Name>"; s+=protect(this->name); s+="</Name>\n";
	s+="</Equipment>\n";
	
	return s;
}

QString Equipment::getDetailedDescriptionWithConstraints(Rules& r)
{
	QString s=this->getDetailedDescription();

	s+="--------------------------------------------------\n";
	s+=QObject::tr("Space constraints directly related to this equipment:");
	s+="\n";
	for(int i=0; i<r.spaceConstraintsList.size(); i++){
		SpaceConstraint* c=r.spaceConstraintsList[i];
		if(c->isRelatedToEquipment(this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}
	s+="--------------------------------------------------\n";

	return s;
}

int equipmentsAscending(const Equipment* e1, const Equipment* e2)
{
	return e1->name < e2->name;	
}
