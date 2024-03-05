/***************************************************************************
                          changemindaysselectivelyform.h  -  description
                             -------------------
    begin                : July 30, 2008
    copyright            : (C) 2008 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef CHANGEMINDAYSSELECTIVELYFORM_H
#define CHANGEMINDAYSSELECTIVELYFORM_H

#include "ui_changemindaysselectivelyform_template.h"
#include "timetable_defs.h"

class QLineEdit;
class QComboBox;
class QSpinBox;
class QString;

class ChangeMinDaysSelectivelyForm : public QDialog, Ui::ChangeMinDaysSelectivelyForm_template  {
	Q_OBJECT

public:
	ChangeMinDaysSelectivelyForm(QWidget* parent);
	~ChangeMinDaysSelectivelyForm();
	
	double oldWeight;
	int oldDays;
	int oldConsecutive;
	QString oldConsecutiveString;
	int oldNActs;
	
	double newWeight;
	int newDays;
	int newConsecutive;
	QString newConsecutiveString;

public slots:
	void ok();
	void cancel();
};

#endif
