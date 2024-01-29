/***************************************************************************
                          addconstraintstudentssetminhoursperafternoonform.cpp  -  description
                             -------------------
    begin                : 2022
    copyright            : (C) 2022 by Liviu Lalescu
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

#include "addconstraintstudentssetminhoursperafternoonform.h"
#include "timeconstraint.h"

AddConstraintStudentsSetMinHoursPerAfternoonForm::AddConstraintStudentsSetMinHoursPerAfternoonForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, &QPushButton::clicked, this, &AddConstraintStudentsSetMinHoursPerAfternoonForm::addCurrentConstraint);
	connect(closePushButton, &QPushButton::clicked, this, &AddConstraintStudentsSetMinHoursPerAfternoonForm::close);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	if(gt.rules.mode==MORNINGS_AFTERNOONS || ENABLE_STUDENTS_MIN_HOURS_PER_AFTERNOON_WITH_ALLOW_EMPTY_AFTERNOONS)
		allowLabel->setText(tr("Advanced usage: enabled"));
	else
		allowLabel->setText(tr("Advanced usage: not enabled"));
	
	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);

	minHoursSpinBox->setMinimum(1);
	minHoursSpinBox->setMaximum(gt.rules.nHoursPerDay);
	minHoursSpinBox->setValue(2);

	allowEmptyAfternoonsCheckBox->setChecked(true);

	updateStudentsSetComboBox();

	connect(allowEmptyAfternoonsCheckBox, &QCheckBox::toggled, this, &AddConstraintStudentsSetMinHoursPerAfternoonForm::allowEmptyAfternoonsCheckBox_toggled);
	
	allowEmptyAfternoonsCheckBox_toggled();
}

AddConstraintStudentsSetMinHoursPerAfternoonForm::~AddConstraintStudentsSetMinHoursPerAfternoonForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintStudentsSetMinHoursPerAfternoonForm::updateStudentsSetComboBox()
{
	populateStudentsComboBox(studentsComboBox);
}

void AddConstraintStudentsSetMinHoursPerAfternoonForm::addCurrentConstraint()
{
	TimeConstraint *ctr=nullptr;

	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight"));
		return;
	}
	if(weight!=100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight - it has to be 100%"));
		return;
	}

	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		if(!ENABLE_STUDENTS_MIN_HOURS_PER_AFTERNOON_WITH_ALLOW_EMPTY_AFTERNOONS && allowEmptyAfternoonsCheckBox->isChecked()){
			QMessageBox::warning(this, tr("FET warning"), tr("Empty afternoons for students min hours per afternoon constraints are not enabled. You must enable them from the Settings->Advanced menu."));
			return;
		}

		//2021-03-26 - I think I commented out this check because the user might combine this constraint with a min hours daily constraint.
		/*if(allowEmptyAfternoonsCheckBox->isChecked() && minHoursSpinBox->value()<2){
			QMessageBox::warning(this, tr("FET warning"), tr("If you allow empty afternoons, the min hours must be at least 2 (to make it a non-trivial constraint)"));
			return;
		}*/
	}

	int minHours=minHoursSpinBox->value();

	QString students_name=studentsComboBox->currentText();
	StudentsSet* s=gt.rules.searchStudentsSet(students_name);
	if(s==nullptr){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid students set"));
		return;
	}

	ctr=new ConstraintStudentsSetMinHoursPerAfternoon(weight, minHours, students_name, allowEmptyAfternoonsCheckBox->isChecked());

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

void AddConstraintStudentsSetMinHoursPerAfternoonForm::allowEmptyAfternoonsCheckBox_toggled()
{
	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		bool k=allowEmptyAfternoonsCheckBox->isChecked();
			
		if(k && !ENABLE_STUDENTS_MIN_HOURS_PER_AFTERNOON_WITH_ALLOW_EMPTY_AFTERNOONS){
			allowEmptyAfternoonsCheckBox->setChecked(false);
			QString s=tr("Advanced usage is not enabled. To be able to select 'Allow empty afternoons' for the constraints of type min hours per afternoon for students, you must enable the option from the Settings->Advanced menu.",
				"'Allow empty afternoons' is an option which the user can enable and then he can select it.");
			s+="\n\n";
			s+=tr("Explanation: only select this option if your institution allows empty afternoons for students and a timetable is possible with empty afternoons for students."
				" Otherwise, it is IMPERATIVE (for performance reasons) to not select this option (or FET may not be able to find a timetable).");
			s+="\n\n";
			s+=tr("Use with caution.");
			QMessageBox::information(this, tr("FET information"), s);
		}
	}
}
