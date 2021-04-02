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
#ifndef INSTITUTIONNAMEFORM_H
#define INSTITUTIONNAMEFORM_H

#include "institutionnameform_template.h"

class InstitutionNameForm : public InstitutionNameForm_template
{
	Q_OBJECT
public:
	InstitutionNameForm();

	~InstitutionNameForm();

public slots:
	void ok();
	void cancel();
};

#endif
