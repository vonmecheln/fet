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
#include "subject.h"
#include "rules.h"

Subject::Subject()
{
}

Subject::~Subject()
{
}

QString Subject::getXmlDescription()
{
	QString s="<Subject>\n";
	s+="	<Name>"+protect(this->name)+"</Name>\n";
	s+="</Subject>\n";

	return s;
}

QString Subject::getDetailedDescription()
{
	QString s=tr("Subject");
	s+="\n";
	s+=tr("Name=%1", "The name of the subject").arg(this->name);
	s+="\n";

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
	return s1->name < s2->name;
}
