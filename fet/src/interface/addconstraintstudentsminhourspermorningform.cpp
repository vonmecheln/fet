/***************************************************************************
                          addconstraintstudentsminhourspermorningform.cpp  -  description
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

#include "addconstraintstudentsminhourspermorningform.h"
#include "timeconstraint.h"

AddConstraintStudentsMinHoursPerMorningForm::AddConstraintStudentsMinHoursPerMorningForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, &QPushButton::clicked, this, &AddConstraintStudentsMinHoursPerMorningForm::addCurrentConstraint);
	connect(closePushButton, &QPushButton::clicked, this, &AddConstraintStudentsMinHoursPerMorningForm::close);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	if(gt.rules.mode==MORNINGS_AFTERNOONS || ENABLE_STUDENTS_MIN_HOURS_PER_MORNING_WITH_ALLOW_EMPTY_MORNINGS)
		allowLabel->setText(tr("Advanced usage: enabled"));
	else
		allowLabel->setText(tr("Advanced usage: not enabled"));
	
	minHoursSpinBox->setMinimum(1);
	minHoursSpinBox->setMaximum(gt.rules.nHoursPerDay);
	minHoursSpinBox->setValue(2);
	
	allowEmptyMorningsCheckBox->setChecked(true);

	connect(allowEmptyMorningsCheckBox, &QCheckBox::toggled, this, &AddConstraintStudentsMinHoursPerMorningForm::allowEmptyMorningsCheckBox_toggled);
	
	allowEmptyMorningsCheckBox_toggled();
}

AddConstraintStudentsMinHoursPerMorningForm::~AddConstraintStudentsMinHoursPerMorningForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintStudentsMinHoursPerMorningForm::addCurrentConstraint()
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
			tr("Invalid weight (percentage) - it has to be 100%"));
		return;
	}
	
	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		if(!ENABLE_STUDENTS_MIN_HOURS_PER_MORNING_WITH_ALLOW_EMPTY_MORNINGS && allowEmptyMorningsCheckBox->isChecked()){
			QMessageBox::warning(this, tr("FET warning"), tr("Empty mornings for students min hours per morning constraints are not enabled. You must enable them from the Settings->Advanced menu."));
			return;
		}
		
		//2021-03-26 - I think I commented out this check because the user might combine this constraint with a min hours daily constraint.
		/*if(allowEmptyMorningsCheckBox->isChecked() && minHoursSpinBox->value()<2){
			QMessageBox::warning(this, tr("FET warning"), tr("If you allow empty mornings, the min hours must be at least 2 (to make it a non-trivial constraint)"));
			return;
		}*/
	}
	
	int minHours=minHoursSpinBox->value();

	ctr=new ConstraintStudentsMinHoursPerMorning(weight, minHours, allowEmptyMorningsCheckBox->isChecked());

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

void AddConstraintStudentsMinHoursPerMorningForm::allowEmptyMorningsCheckBox_toggled()
{
	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		bool k=allowEmptyMorningsCheckBox->isChecked();
		
		if(k && !ENABLE_STUDENTS_MIN_HOURS_PER_MORNING_WITH_ALLOW_EMPTY_MORNINGS){
			allowEmptyMorningsCheckBox->setChecked(false);
			QString s=tr("Advanced usage is not enabled. To be able to select 'Allow empty mornings' for the constraints of type min hours per morning for students, you must enable the option from the Settings->Advanced menu.",
				"'Allow empty mornings' is an option which the user can enable and then he can select it.");
			s+="\n\n";
			s+=tr("Explanation: only select this option if your institution allows empty mornings for students and a timetable is possible with empty mornings for students."
				" Otherwise, it is IMPERATIVE (for performance reasons) to not select this option (or FET may not be able to find a timetable).");
			s+="\n\n";
			s+=tr("Use with caution.");
			QMessageBox::information(this, tr("FET information"), s);
		}
	}
}
