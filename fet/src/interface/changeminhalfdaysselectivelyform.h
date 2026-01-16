/***************************************************************************
                          changeminhalfdaysselectivelyform.h  -  description
                             -------------------
    begin                : 2022
    copyright            : (C) 2022 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef CHANGEMINHALFDAYSSELECTIVELYFORM_H
#define CHANGEMINHALFDAYSSELECTIVELYFORM_H

#include "ui_changeminhalfdaysselectivelyform_template.h"
#include "timetable_defs.h"

class QLineEdit;
class QComboBox;
class QSpinBox;
class QString;

class ChangeMinHalfDaysSelectivelyForm : public QDialog, Ui::ChangeMinHalfDaysSelectivelyForm_template  {
	Q_OBJECT

public:
	ChangeMinHalfDaysSelectivelyForm(QWidget* parent);
	~ChangeMinHalfDaysSelectivelyForm();
	
	double oldWeight;
	int oldDays;
	int oldConsecutive;
	QString oldConsecutiveString;
	int oldNActs;
	
	double newWeight;
	int newDays;
	int newConsecutive;
	QString newConsecutiveString;

public Q_SLOTS:
	void ok();
	void cancel();
};

#endif
