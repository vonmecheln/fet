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
#include "subjecttag.h"
#include "rules.h"

SubjectTag::SubjectTag()
{
}

SubjectTag::~SubjectTag()
{
}

QString SubjectTag::getXmlDescription()
{
	QString s="<Subject_Tag>\n";
	s+="	<Name>"+protect(this->name)+"</Name>\n";
	s+="</Subject_Tag>\n";

	return s;
}

QString SubjectTag::getDetailedDescription()
{
	QString s=QObject::tr("Subject tag");
	s+="\n";
	s+=QObject::tr("Name=%1").arg(this->name);
	s+="\n";

	return s;
}

QString SubjectTag::getDetailedDescriptionWithConstraints(Rules& r)
{
	QString s=this->getDetailedDescription();

	s+="--------------------------------------------------\n";
	s+=QObject::tr("Time constraints directly related to this subject tag:");
	s+="\n";
	for(int i=0; i<r.timeConstraintsList.size(); i++){
		TimeConstraint* c=r.timeConstraintsList[i];
		if(c->isRelatedToSubjectTag(this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}

	s+="--------------------------------------------------\n";
	s+=QObject::tr("Space constraints directly related to this subject tag:");
	s+="\n";
	for(int i=0; i<r.spaceConstraintsList.size(); i++){
		SpaceConstraint* c=r.spaceConstraintsList[i];
		if(c->isRelatedToSubjectTag(this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}
	s+="--------------------------------------------------\n";

	return s;
}

int subjectTagsAscending(const SubjectTag* st1, const SubjectTag* st2)
{
	return st1->name < st2->name;
}
