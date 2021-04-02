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

#include "ui_activitytagsform_template.h"

class ActivityTagsForm : public QDialog, Ui::ActivityTagsForm_template
{
	Q_OBJECT
	
public:
	ActivityTagsForm();

	~ActivityTagsForm();

public slots:
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
