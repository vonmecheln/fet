/***************************************************************************
                          modifyconstraintstudentsminrestinghoursbetweenmorningandafternoonform.cpp  -  description
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

#include "modifyconstraintstudentsminrestinghoursbetweenmorningandafternoonform.h"
#include "timeconstraint.h"

ModifyConstraintStudentsMinRestingHoursBetweenMorningAndAfternoonForm::ModifyConstraintStudentsMinRestingHoursBetweenMorningAndAfternoonForm(QWidget* parent, ConstraintStudentsMinRestingHoursBetweenMorningAndAfternoon* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	minRestingHoursSpinBox->setMinimum(1);
	minRestingHoursSpinBox->setMaximum(2*gt.rules.nHoursPerDay);
	minRestingHoursSpinBox->setValue(ctr->minRestingHours);
}

ModifyConstraintStudentsMinRestingHoursBetweenMorningAndAfternoonForm::~ModifyConstraintStudentsMinRestingHoursBetweenMorningAndAfternoonForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintStudentsMinRestingHoursBetweenMorningAndAfternoonForm::ok()
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

	this->_ctr->weightPercentage=weight;
	
	this->_ctr->minRestingHours=minRestingHoursSpinBox->value();

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintStudentsMinRestingHoursBetweenMorningAndAfternoonForm::cancel()
{
	this->close();
}
