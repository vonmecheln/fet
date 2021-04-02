//
//
// C++ Interface: $MODULE$
//
// Description: 
//
//
// Author: Lalescu Liviu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SUBJECTTAGSFORM_H
#define SUBJECTTAGSFORM_H

#include "subjecttagsform_template.h"

class SubjectTagsForm : public SubjectTagsForm_template
{
public:
	SubjectTagsForm();

	~SubjectTagsForm();

	void addSubjectTag();
	void removeSubjectTag();
	void renameSubjectTag();
	void sortSubjectTags();
	
	void subjectTagChanged(int index);
	
	void activateSubjectTag();
	void deactivateSubjectTag();
};

#endif
