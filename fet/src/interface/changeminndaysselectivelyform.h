/***************************************************************************
                          changeminndaysselectivelyform.h  -  description
                             -------------------
    begin                : July 30, 2008
    copyright            : (C) 2008 by Lalescu Liviu
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

#ifndef CHANGEMINNDAYSSELECTIVELYFORM_H
#define CHANGEMINNDAYSSELECTIVELYFORM_H

#include "ui_changeminndaysselectivelyform_template.h"
#include "timetable_defs.h"

class QLineEdit;
class QComboBox;
class QSpinBox;

class ChangeMinNDaysSelectivelyForm : public QDialog, Ui::ChangeMinNDaysSelectivelyForm_template  {
	Q_OBJECT

public:
	ChangeMinNDaysSelectivelyForm();
	~ChangeMinNDaysSelectivelyForm();
	
	double oldWeight;
	int oldDays;
	int oldConsecutive;
	int oldNActs;
	
	double newWeight;
	int newDays;
	int newConsecutive;

public slots:
	void on_okPushButton_clicked();
	void on_cancelPushButton_clicked();
	
//	QLineEdit* oldWeightLineEdit;
//	QLineEdit* newWeightLineEdit;
//	QSpinBox* oldDaysSpinBox;
//	QComboBox* oldConsecutiveComboBox;
};

#endif
