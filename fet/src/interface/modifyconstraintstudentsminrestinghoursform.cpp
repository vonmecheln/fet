/***************************************************************************
                          modifyconstraintstudentsminrestinghoursform.cpp  -  description
                             -------------------
    begin                : 2017
    copyright            : (C) 2017 by Liviu Lalescu
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

#include "modifyconstraintstudentsminrestinghoursform.h"
#include "timeconstraint.h"

ModifyConstraintStudentsMinRestingHoursForm::ModifyConstraintStudentsMinRestingHoursForm(QWidget* parent, ConstraintStudentsMinRestingHours* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, &QPushButton::clicked, this, &ModifyConstraintStudentsMinRestingHoursForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyConstraintStudentsMinRestingHoursForm::cancel);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	circularCheckBox->setChecked(ctr->circular);
	
	minRestingHoursSpinBox->setMinimum(1);
	minRestingHoursSpinBox->setMaximum(gt.rules.nHoursPerDay);
	minRestingHoursSpinBox->setValue(ctr->minRestingHours);
}

ModifyConstraintStudentsMinRestingHoursForm::~ModifyConstraintStudentsMinRestingHoursForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintStudentsMinRestingHoursForm::ok()
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
			tr("Invalid weight (percentage) - it must be 100%"));
		return;
	}

	QString oldcs=this->_ctr->getDetailedDescription(gt.rules);

	this->_ctr->weightPercentage=weight;
	
	this->_ctr->minRestingHours=minRestingHoursSpinBox->value();
	this->_ctr->circular=circularCheckBox->isChecked();

	QString newcs=this->_ctr->getDetailedDescription(gt.rules);
	gt.rules.addUndoPoint(tr("Modified the constraint:\n\n%1\ninto\n\n%2").arg(oldcs).arg(newcs));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintStudentsMinRestingHoursForm::cancel()
{
	this->close();
}
