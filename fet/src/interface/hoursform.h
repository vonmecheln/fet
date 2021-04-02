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
#ifndef HOURSFORM_H
#define HOURSFORM_H

#include "hoursform_template.h"

class HoursForm : public HoursForm_template
{
public:
	HoursForm();

	~HoursForm();

	void hoursChanged();
	void ok();
	void cancel();
};

#endif
