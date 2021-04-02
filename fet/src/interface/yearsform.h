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
#ifndef YEARSFORM_H
#define YEARSFORM_H

#include "ui_yearsform_template.h"

class YearsForm : public QDialog, Ui::YearsForm_template
{
	Q_OBJECT
public:
	YearsForm();
	~YearsForm();

public slots:
	void addYear();
	void removeYear();
	void modifyYear();
	void sortYears();
	void yearChanged();
	
	void activateStudents();
	void deactivateStudents();
	
	void divideYear();
};

#endif
