/***************************************************************************
                          modifyconstraintteachersmaxhoursdailyrealdaysform.cpp  -  description
                             -------------------
    begin                : 2021
    copyright            : (C) 2021 by Liviu Lalescu
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

#include "modifyconstraintteachersmaxhoursdailyrealdaysform.h"
#include "timeconstraint.h"

ModifyConstraintTeachersMaxHoursDailyRealDaysForm::ModifyConstraintTeachersMaxHoursDailyRealDaysForm(QWidget* parent, ConstraintTeachersMaxHoursDailyRealDays* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, &QPushButton::clicked, this, &ModifyConstraintTeachersMaxHoursDailyRealDaysForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyConstraintTeachersMaxHoursDailyRealDaysForm::cancel);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	updateMaxHoursSpinBox();
	
	maxHoursSpinBox->setValue(ctr->maxHoursDaily);
}

ModifyConstraintTeachersMaxHoursDailyRealDaysForm::~ModifyConstraintTeachersMaxHoursDailyRealDaysForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintTeachersMaxHoursDailyRealDaysForm::updateMaxHoursSpinBox(){
	maxHoursSpinBox->setMinimum(1);
	maxHoursSpinBox->setMaximum(2*gt.rules.nHoursPerDay);
}

void ModifyConstraintTeachersMaxHoursDailyRealDaysForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage)"));
		return;
	}

	if(weight<100.0){
		int t=QMessageBox::warning(this, tr("FET warning"),
			tr("You selected a weight less than 100%. The generation algorithm is not perfectly optimized to work with such weights (even"
			 " if in practice it might work well). It is recommended to work only with 100% weights for these constraints. Are you sure you want to continue?"),
			 QMessageBox::Yes | QMessageBox::Cancel);
		if(t==QMessageBox::Cancel)
			return;
	}

	int max_hours=maxHoursSpinBox->value();

	QString oldcs=this->_ctr->getDetailedDescription(gt.rules);

	this->_ctr->weightPercentage=weight;
	this->_ctr->maxHoursDaily=max_hours;

	QString newcs=this->_ctr->getDetailedDescription(gt.rules);
	gt.rules.addUndoPoint(tr("Modified the constraint:\n\n%1\ninto\n\n%2").arg(oldcs).arg(newcs));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintTeachersMaxHoursDailyRealDaysForm::cancel()
{
	this->close();
}
