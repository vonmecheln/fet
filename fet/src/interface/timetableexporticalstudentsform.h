//
//
// C++ Interface: $MODULE$
//
// Description: 
//
//
// Author: Lalescu Liviu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TIMETABLEEXPORTICALSTUDENTSFORM_H
#define TIMETABLEEXPORTICALSTUDENTSFORM_H

#include "timetableexporticalstudentsform_template.h"

class TimetableExportiCalStudentsForm : public TimetableExportiCalStudentsForm_template
{
public:
	TimetableExportiCalStudentsForm();

	~TimetableExportiCalStudentsForm();

	void ok();
	void cancel();
};

#endif
