/***************************************************************************
                          modifyconstraintteachersminhourspermorningform.cpp  -  description
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

#include "modifyconstraintteachersminhourspermorningform.h"
#include "timeconstraint.h"

ModifyConstraintTeachersMinHoursPerMorningForm::ModifyConstraintTeachersMinHoursPerMorningForm(QWidget* parent, ConstraintTeachersMinHoursPerMorning* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, &QPushButton::clicked, this, &ModifyConstraintTeachersMinHoursPerMorningForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyConstraintTeachersMinHoursPerMorningForm::cancel);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	allowEmptyMorningsCheckBox->setChecked(ctr->allowEmptyMornings);
	
	connect(allowEmptyMorningsCheckBox, &QCheckBox::toggled, this, &ModifyConstraintTeachersMinHoursPerMorningForm::allowEmptyMorningsCheckBoxToggled); //after setChecked(...)
	
	updateMinHoursSpinBox();
	
	minHoursSpinBox->setValue(ctr->minHoursPerMorning);
}

ModifyConstraintTeachersMinHoursPerMorningForm::~ModifyConstraintTeachersMinHoursPerMorningForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintTeachersMinHoursPerMorningForm::updateMinHoursSpinBox(){
	minHoursSpinBox->setMinimum(2);
	minHoursSpinBox->setMaximum(gt.rules.nHoursPerDay);
}

void ModifyConstraintTeachersMinHoursPerMorningForm::ok()
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

	if(!allowEmptyMorningsCheckBox->isChecked()){
		QMessageBox::warning(this, tr("FET information"), tr("Allow empty mornings check box must be checked. If you need to not allow empty mornings for the teachers, "
			"please use the constraint teachers min mornings per week."));
		return;
	}

	int min_hours=minHoursSpinBox->value();

	QString oldcs=this->_ctr->getDetailedDescription(gt.rules);

	this->_ctr->weightPercentage=weight;
	this->_ctr->minHoursPerMorning=min_hours;
	
	this->_ctr->allowEmptyMornings=allowEmptyMorningsCheckBox->isChecked();

	QString newcs=this->_ctr->getDetailedDescription(gt.rules);
	gt.rules.addUndoPoint(tr("Modified the constraint:\n\n%1\ninto\n\n%2").arg(oldcs).arg(newcs));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintTeachersMinHoursPerMorningForm::cancel()
{
	this->close();
}

void ModifyConstraintTeachersMinHoursPerMorningForm::allowEmptyMorningsCheckBoxToggled()
{
	bool k=allowEmptyMorningsCheckBox->isChecked();

	if(!k){
		allowEmptyMorningsCheckBox->setChecked(true);
		QMessageBox::information(this, tr("FET information"), tr("This check box must remain checked. If you really need to not allow empty mornings for the teachers,"
			" please use constraint teachers min mornings per week."));
	}
}
