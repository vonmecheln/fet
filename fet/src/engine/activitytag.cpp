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
#include "activitytag.h"
#include "rules.h"

ActivityTag::ActivityTag()
{
}

ActivityTag::~ActivityTag()
{
}

QString ActivityTag::getXmlDescription()
{
	QString s="<Activity_Tag>\n";
	s+="	<Name>"+protect(this->name)+"</Name>\n";
	s+="</Activity_Tag>\n";

	return s;
}

QString ActivityTag::getDetailedDescription()
{
	QString s=QObject::tr("Activity tag");
	s+="\n";
	s+=QObject::tr("Name=%1").arg(this->name);
	s+="\n";

	return s;
}

QString ActivityTag::getDetailedDescriptionWithConstraints(Rules& r)
{
	QString s=this->getDetailedDescription();

	s+="--------------------------------------------------\n";
	s+=QObject::tr("Time constraints directly related to this activity tag:");
	s+="\n";
	for(int i=0; i<r.timeConstraintsList.size(); i++){
		TimeConstraint* c=r.timeConstraintsList[i];
		if(c->isRelatedToActivityTag(this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}

	s+="--------------------------------------------------\n";
	s+=QObject::tr("Space constraints directly related to this activity tag:");
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
	return st1->name < st2->name;
}