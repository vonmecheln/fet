/***************************************************************************
                          subactivitiesform.h  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Lalescu Liviu
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

#ifndef SUBACTIVITIESFORM_H
#define SUBACTIVITIESFORM_H

#include "subactivitiesform_template.h"

class SubactivitiesForm : public SubactivitiesForm_template  {
public:
	ActivitiesList visibleSubactivitiesList;

	SubactivitiesForm();
	~SubactivitiesForm();

	void modifySubactivity();
	void subactivityChanged(int index);
	void filterChanged();
	bool filterOk(Activity* act);
};

#endif
