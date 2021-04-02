/***************************************************************************
                          activitiesform.h  -  description
                             -------------------
    begin                : Wed Apr 23 2003
    copyright            : (C) 2003 by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ACTIVITIESFORM_H
#define ACTIVITIESFORM_H

#include "activitiesform_template.h"

class ActivitiesForm : public ActivitiesForm_template  {
public:
	ActivitiesList visibleActivitiesList;

	ActivitiesForm();
	~ActivitiesForm();

	void addActivity();
	void removeActivity();
	void modifyActivity();
	void activityChanged(int index);
	void filterChanged();
	bool filterOk(Activity* act);
};

#endif
