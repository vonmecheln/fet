/***************************************************************************
                          modifyconstraintteachersminafternoonsperweekform.cpp  -  description
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



#include "modifyconstraintteachersminafternoonsperweekform.h"
#include "timeconstraint.h"

#include <QMessageBox>

ModifyConstraintTeachersMinAfternoonsPerWeekForm::ModifyConstraintTeachersMinAfternoonsPerWeekForm(QWidget* parent, ConstraintTeachersMinAfternoonsPerWeek* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	updateMinAfternoonsSpinBox();
	
	minAfternoonsSpinBox->setValue(ctr->minAfternoonsPerWeek);
}

ModifyConstraintTeachersMinAfternoonsPerWeekForm::~ModifyConstraintTeachersMinAfternoonsPerWeekForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintTeachersMinAfternoonsPerWeekForm::updateMinAfternoonsSpinBox(){
	minAfternoonsSpinBox->setMinimum(1);
	minAfternoonsSpinBox->setMaximum(gt.rules.nDaysPerWeek/2);
}

void ModifyConstraintTeachersMinAfternoonsPerWeekForm::ok()
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

	int min_afternoons=minAfternoonsSpinBox->value();

	this->_ctr->weightPercentage=weight;
	this->_ctr->minAfternoonsPerWeek=min_afternoons;

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintTeachersMinAfternoonsPerWeekForm::cancel()
{
	this->close();
}
