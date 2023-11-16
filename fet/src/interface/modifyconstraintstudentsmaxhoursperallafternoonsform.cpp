/***************************************************************************
                          modifyconstraintstudentsmaxhoursperallafternoonsform.cpp  -  description
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

#include "modifyconstraintstudentsmaxhoursperallafternoonsform.h"
#include "timeconstraint.h"

ModifyConstraintStudentsMaxHoursPerAllAfternoonsForm::ModifyConstraintStudentsMaxHoursPerAllAfternoonsForm(QWidget* parent, ConstraintStudentsMaxHoursPerAllAfternoons* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	updateMaxHoursSpinBox();
	
	maxHoursSpinBox->setValue(ctr->maxHoursPerAllAfternoons);
}

ModifyConstraintStudentsMaxHoursPerAllAfternoonsForm::~ModifyConstraintStudentsMaxHoursPerAllAfternoonsForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintStudentsMaxHoursPerAllAfternoonsForm::updateMaxHoursSpinBox(){
	maxHoursSpinBox->setMinimum(0);
	maxHoursSpinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay/2);
}

void ModifyConstraintStudentsMaxHoursPerAllAfternoonsForm::ok()
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

	QString oldcs=this->_ctr->getDetailedDescription(gt.rules);

	int max_hours=maxHoursSpinBox->value();

	this->_ctr->weightPercentage=weight;
	this->_ctr->maxHoursPerAllAfternoons=max_hours;
	
	QString newcs=this->_ctr->getDetailedDescription(gt.rules);
	gt.rules.addUndoPoint(tr("Modified the constraint:\n\n%1\ninto\n\n%2").arg(oldcs).arg(newcs));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintStudentsMaxHoursPerAllAfternoonsForm::cancel()
{
	this->close();
}
