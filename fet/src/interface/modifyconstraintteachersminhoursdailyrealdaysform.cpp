/***************************************************************************
                          modifyconstraintteachersminhoursdailyrealdaysform.cpp  -  description
                             -------------------
    begin                : 2019
    copyright            : (C) 2019 by Liviu Lalescu
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

#include "modifyconstraintteachersminhoursdailyrealdaysform.h"
#include "timeconstraint.h"

ModifyConstraintTeachersMinHoursDailyRealDaysForm::ModifyConstraintTeachersMinHoursDailyRealDaysForm(QWidget* parent, ConstraintTeachersMinHoursDailyRealDays* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, &QPushButton::clicked, this, &ModifyConstraintTeachersMinHoursDailyRealDaysForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyConstraintTeachersMinHoursDailyRealDaysForm::cancel);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	allowEmptyDaysCheckBox->setChecked(ctr->allowEmptyDays);
	
	connect(allowEmptyDaysCheckBox, &QCheckBox::toggled, this, &ModifyConstraintTeachersMinHoursDailyRealDaysForm::allowEmptyDaysCheckBoxToggled); //after setChecked(...)
	
	updateMinHoursSpinBox();
	
	minHoursSpinBox->setValue(ctr->minHoursDaily);
}

ModifyConstraintTeachersMinHoursDailyRealDaysForm::~ModifyConstraintTeachersMinHoursDailyRealDaysForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintTeachersMinHoursDailyRealDaysForm::updateMinHoursSpinBox(){
	minHoursSpinBox->setMinimum(2);
	minHoursSpinBox->setMaximum(2*gt.rules.nHoursPerDay);
}

void ModifyConstraintTeachersMinHoursDailyRealDaysForm::ok()
{
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

	if(!allowEmptyDaysCheckBox->isChecked()){
		QMessageBox::warning(this, tr("FET information"), tr("Allow empty days check box must be checked. If you need to not allow empty days for the teachers, "
			"please use the constraint teachers min days per week"));
		return;
	}

	int min_hours=minHoursSpinBox->value();

	QString oldcs=this->_ctr->getDetailedDescription(gt.rules);

	this->_ctr->weightPercentage=weight;
	this->_ctr->minHoursDaily=min_hours;
	
	this->_ctr->allowEmptyDays=allowEmptyDaysCheckBox->isChecked();

	QString newcs=this->_ctr->getDetailedDescription(gt.rules);
	gt.rules.addUndoPoint(tr("Modified the constraint:\n\n%1\ninto\n\n%2").arg(oldcs).arg(newcs));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintTeachersMinHoursDailyRealDaysForm::cancel()
{
	this->close();
}

void ModifyConstraintTeachersMinHoursDailyRealDaysForm::allowEmptyDaysCheckBoxToggled()
{
	bool k=allowEmptyDaysCheckBox->isChecked();

	if(!k){
		allowEmptyDaysCheckBox->setChecked(true);
		QMessageBox::information(this, tr("FET information"), tr("This check box must remain checked. If you really need to not allow empty days for the teachers,"
			" please use constraint teachers min days per week"));
	}
}
