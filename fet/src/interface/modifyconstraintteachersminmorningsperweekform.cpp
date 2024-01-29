/***************************************************************************
                          modifyconstraintteachersminmorningsperweekform.cpp  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Liviu Lalescu
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



#include "modifyconstraintteachersminmorningsperweekform.h"
#include "timeconstraint.h"

#include <QMessageBox>

ModifyConstraintTeachersMinMorningsPerWeekForm::ModifyConstraintTeachersMinMorningsPerWeekForm(QWidget* parent, ConstraintTeachersMinMorningsPerWeek* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, &QPushButton::clicked, this, &ModifyConstraintTeachersMinMorningsPerWeekForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyConstraintTeachersMinMorningsPerWeekForm::cancel);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	updateMinMorningsSpinBox();
	
	minMorningsSpinBox->setValue(ctr->minMorningsPerWeek);
}

ModifyConstraintTeachersMinMorningsPerWeekForm::~ModifyConstraintTeachersMinMorningsPerWeekForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintTeachersMinMorningsPerWeekForm::updateMinMorningsSpinBox(){
	minMorningsSpinBox->setMinimum(1);
	minMorningsSpinBox->setMaximum(gt.rules.nDaysPerWeek/2);
}

void ModifyConstraintTeachersMinMorningsPerWeekForm::ok()
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

	int min_mornings=minMorningsSpinBox->value();

	QString oldcs=this->_ctr->getDetailedDescription(gt.rules);

	this->_ctr->weightPercentage=weight;
	this->_ctr->minMorningsPerWeek=min_mornings;

	QString newcs=this->_ctr->getDetailedDescription(gt.rules);
	gt.rules.addUndoPoint(tr("Modified the constraint:\n\n%1\ninto\n\n%2").arg(oldcs).arg(newcs));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintTeachersMinMorningsPerWeekForm::cancel()
{
	this->close();
}
