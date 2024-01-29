/***************************************************************************
                          addconstrainstudentssetmaxhoursperallafternoonsform.cpp  -  description
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

#include "addconstraintstudentssetmaxhoursperallafternoonsform.h"
#include "timeconstraint.h"

AddConstraintStudentsSetMaxHoursPerAllAfternoonsForm::AddConstraintStudentsSetMaxHoursPerAllAfternoonsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, &QPushButton::clicked, this, &AddConstraintStudentsSetMaxHoursPerAllAfternoonsForm::addCurrentConstraint);
	connect(closePushButton, &QPushButton::clicked, this, &AddConstraintStudentsSetMaxHoursPerAllAfternoonsForm::close);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp1=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	
	updateMaxHoursSpinBox();

	populateStudentsComboBox(studentsComboBox);
}

AddConstraintStudentsSetMaxHoursPerAllAfternoonsForm::~AddConstraintStudentsSetMaxHoursPerAllAfternoonsForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintStudentsSetMaxHoursPerAllAfternoonsForm::updateMaxHoursSpinBox(){
	maxHoursSpinBox->setMinimum(0);
	maxHoursSpinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay/2);
	maxHoursSpinBox->setValue(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay/2);
}

void AddConstraintStudentsSetMaxHoursPerAllAfternoonsForm::addCurrentConstraint()
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
			tr("Invalid weight (percentage) - must be 100%"));
		return;
	}

	QString students_name=studentsComboBox->currentText();
	StudentsSet* s=gt.rules.searchStudentsSet(students_name);
	if(s==nullptr){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid students set"));
		return;
	}
	
	int max_hours=maxHoursSpinBox->value();

	ctr=new ConstraintStudentsSetMaxHoursPerAllAfternoons(weight, max_hours, students_name);

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
