/***************************************************************************
                          addconstraintstudentssetafternoonsearlymaxbeginningsatsecondhourform.cpp  -  description
                             -------------------
    begin                : 2020
    copyright            : (C) 2020 by Liviu Lalescu
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

#include <QMessageBox>

#include "longtextmessagebox.h"

#include "addconstraintstudentssetafternoonsearlymaxbeginningsatsecondhourform.h"
#include "timeconstraint.h"

AddConstraintStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHourForm::AddConstraintStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHourForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, &QPushButton::clicked, this, &AddConstraintStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHourForm::addCurrentConstraint);
	connect(closePushButton, &QPushButton::clicked, this, &AddConstraintStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHourForm::close);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	
	populateStudentsComboBox(studentsComboBox);

	maxBeginningsSpinBox->setMinimum(0);
	maxBeginningsSpinBox->setMaximum(gt.rules.nDaysPerWeek/2);
	maxBeginningsSpinBox->setValue(0);
}

AddConstraintStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHourForm::~AddConstraintStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHourForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHourForm::addCurrentConstraint()
{
	TimeConstraint *ctr=nullptr;

	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage)"));
		return;
	}
	if(weight!=100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage) - it must be 100%"));
		return;
	}

	QString students_name=studentsComboBox->currentText();
	StudentsSet* s=gt.rules.searchStudentsSet(students_name);
	if(s==nullptr){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid students set"));
		return;
	}

	ctr=new ConstraintStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHour(weight, maxBeginningsSpinBox->value(), students_name);

	bool tmp2=gt.rules.addTimeConstraint(ctr);
	if(tmp2){
		LongTextMessageBox::information(this, tr("FET information"),
			tr("Constraint added:")+"\n\n"+ctr->getDetailedDescription(gt.rules));

		gt.rules.addUndoPoint(tr("Added the constraint:\n\n%1").arg(ctr->getDetailedDescription(gt.rules)));
	}
	else{
		QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - please report error"));
		delete ctr;
	}
}
