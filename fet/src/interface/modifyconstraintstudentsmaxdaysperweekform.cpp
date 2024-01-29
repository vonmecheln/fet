/***************************************************************************
                          modifyconstraintstudentsmaxdaysperweekform.cpp  -  description
                             -------------------
    begin                : 2013
    copyright            : (C) 2013 by Liviu Lalescu
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

#include "modifyconstraintstudentsmaxdaysperweekform.h"
#include "timeconstraint.h"

ModifyConstraintStudentsMaxDaysPerWeekForm::ModifyConstraintStudentsMaxDaysPerWeekForm(QWidget* parent, ConstraintStudentsMaxDaysPerWeek* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, &QPushButton::clicked, this, &ModifyConstraintStudentsMaxDaysPerWeekForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyConstraintStudentsMaxDaysPerWeekForm::cancel);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	updateMaxDaysSpinBox();

	maxDaysSpinBox->setValue(ctr->maxDaysPerWeek);
}

ModifyConstraintStudentsMaxDaysPerWeekForm::~ModifyConstraintStudentsMaxDaysPerWeekForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintStudentsMaxDaysPerWeekForm::updateMaxDaysSpinBox(){
	maxDaysSpinBox->setMinimum(1);
	maxDaysSpinBox->setMaximum(gt.rules.nDaysPerWeek);
}

void ModifyConstraintStudentsMaxDaysPerWeekForm::ok()
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
			tr("Invalid weight (percentage) - it has to be 100%"));
		return;
	}

	QString oldcs=this->_ctr->getDetailedDescription(gt.rules);

	int max_days=maxDaysSpinBox->value();

	this->_ctr->weightPercentage=weight;
	this->_ctr->maxDaysPerWeek=max_days;

	QString newcs=this->_ctr->getDetailedDescription(gt.rules);
	gt.rules.addUndoPoint(tr("Modified the constraint:\n\n%1\ninto\n\n%2").arg(oldcs).arg(newcs));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintStudentsMaxDaysPerWeekForm::cancel()
{
	this->close();
}
