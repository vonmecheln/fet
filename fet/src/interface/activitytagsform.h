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

#ifndef ACTIVITYTAGSFORM_H
#define ACTIVITYTAGSFORM_H

#include "ui_activitytagsform_template.h"

class ActivityTagsForm : public QDialog, Ui::ActivityTagsForm_template
{
	Q_OBJECT
	
public:
	ActivityTagsForm(QWidget* parent);

	~ActivityTagsForm();

public Q_SLOTS:
	void addActivityTag();
	void removeActivityTag();
	void renameActivityTag();

	void moveActivityTagUp();
	void moveActivityTagDown();

	void sortActivityTags();
	
	void activityTagChanged(int index);
	
	void activateActivityTag();
	void deactivateActivityTag();
	
	void printableActivityTag();
	void notPrintableActivityTag();
	
	void longName();
	void code();
	void comments();
	
	void help();
};

#endif
