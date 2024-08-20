/***************************************************************************
                          modifyconstraintstudentssetminhourspermorningform.cpp  -  description
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

#include "modifyconstraintstudentssetminhourspermorningform.h"
#include "timeconstraint.h"

ModifyConstraintStudentsSetMinHoursPerMorningForm::ModifyConstraintStudentsSetMinHoursPerMorningForm(QWidget* parent, ConstraintStudentsSetMinHoursPerMorning* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, &QPushButton::clicked, this, &ModifyConstraintStudentsSetMinHoursPerMorningForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyConstraintStudentsSetMinHoursPerMorningForm::cancel);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));

	//assert(ctr->allowEmptyDays==true);
	allowEmptyMorningsCheckBox->setChecked(ctr->allowEmptyMornings);
	
	updateStudentsComboBox(parent);

	minHoursSpinBox->setMinimum(1);
	minHoursSpinBox->setMaximum(gt.rules.nHoursPerDay);
	minHoursSpinBox->setValue(ctr->minHoursPerMorning);
}

ModifyConstraintStudentsSetMinHoursPerMorningForm::~ModifyConstraintStudentsSetMinHoursPerMorningForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintStudentsSetMinHoursPerMorningForm::updateStudentsComboBox(QWidget* parent){
	int j=populateStudentsComboBox(studentsComboBox, this->_ctr->students);
	if(j<0)
		showWarningForInvisibleSubgroupConstraint(parent, this->_ctr->students);
	else
		assert(j>=0);
	studentsComboBox->setCurrentIndex(j);
}

void ModifyConstraintStudentsSetMinHoursPerMorningForm::ok()
{
	if(studentsComboBox->currentIndex()<0){
		showWarningCannotModifyConstraintInvisibleSubgroupConstraint(this, this->_ctr->students);
		return;
	}

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
		//2021-03-26 - I think I commented out this check because the user might combine this constraint with a min hours daily constraint.
		/*if(allowEmptyMorningsCheckBox->isChecked() && minHoursSpinBox->value()<2){
			QMessageBox::warning(this, tr("FET warning"), tr("If you allow empty mornings, the min hours must be at least 2 (to make it a non-trivial constraint)"));
			return;
		}*/
	}

	QString students_name=studentsComboBox->currentText();
	StudentsSet* s=gt.rules.searchStudentsSet(students_name);
	if(s==nullptr){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid students set"));
		return;
	}

	QString oldcs=this->_ctr->getDetailedDescription(gt.rules);

	this->_ctr->weightPercentage=weight;
	this->_ctr->students=students_name;
	this->_ctr->minHoursPerMorning=minHoursSpinBox->value();

	this->_ctr->allowEmptyMornings=allowEmptyMorningsCheckBox->isChecked();

	QString newcs=this->_ctr->getDetailedDescription(gt.rules);
	gt.rules.addUndoPoint(tr("Modified the constraint:\n\n%1\ninto\n\n%2").arg(oldcs).arg(newcs));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintStudentsSetMinHoursPerMorningForm::cancel()
{
	this->close();
}
