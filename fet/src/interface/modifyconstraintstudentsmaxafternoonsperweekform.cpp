/***************************************************************************
                          modifyconstraintstudentsmaxafternoonsperweekform.cpp  -  description
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

#include "modifyconstraintstudentsmaxafternoonsperweekform.h"
#include "timeconstraint.h"

ModifyConstraintStudentsMaxAfternoonsPerWeekForm::ModifyConstraintStudentsMaxAfternoonsPerWeekForm(QWidget* parent, ConstraintStudentsMaxAfternoonsPerWeek* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	updateMaxAfternoonsSpinBox();

	maxAfternoonsSpinBox->setValue(ctr->maxAfternoonsPerWeek);
}

ModifyConstraintStudentsMaxAfternoonsPerWeekForm::~ModifyConstraintStudentsMaxAfternoonsPerWeekForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintStudentsMaxAfternoonsPerWeekForm::updateMaxAfternoonsSpinBox(){
	maxAfternoonsSpinBox->setMinimum(0);
	maxAfternoonsSpinBox->setMaximum(gt.rules.nDaysPerWeek/2);
}

void ModifyConstraintStudentsMaxAfternoonsPerWeekForm::ok()
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

	int max_afternoons=maxAfternoonsSpinBox->value();

	this->_ctr->weightPercentage=weight;
	this->_ctr->maxAfternoonsPerWeek=max_afternoons;

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintStudentsMaxAfternoonsPerWeekForm::cancel()
{
	this->close();
}
