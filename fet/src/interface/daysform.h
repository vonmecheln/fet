//
//
// C++ Interface: $MODULE$
//
// Description: 
//
//
// Author: Lalescu Liviu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DAYSFORM_H
#define DAYSFORM_H

#include "daysform_template.h"

class DaysForm : public DaysForm_template
{
public:
	DaysForm();

	~DaysForm();

	void daysChanged();
	void ok();
	void cancel();
};

#endif
