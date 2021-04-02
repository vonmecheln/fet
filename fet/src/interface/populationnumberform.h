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
#ifndef POPULATIONNUMBERFORM_H
#define POPULATIONNUMBERFORM_H

#include "populationnumberform_template.h"

class PopulationNumberForm : public PopulationNumberForm_template
{
	int p_n;
	
public:
	PopulationNumberForm();

	~PopulationNumberForm();

	void populationNumberChanged();
	
	void ok();
	void cancel();
};

#endif
