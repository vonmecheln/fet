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
#ifndef ACTIVITYTAGSFORM_H
#define ACTIVITYTAGSFORM_H

#include "activitytagsform_template.h"

class ActivityTagsForm : public ActivityTagsForm_template
{
public:
	ActivityTagsForm();

	~ActivityTagsForm();

	void addActivityTag();
	void removeActivityTag();
	void renameActivityTag();
	void sortActivityTags();
	
	void activityTagChanged(int index);
	
	void activateActivityTag();
	void deactivateActivityTag();
	
	void help();
};

#endif
