/***************************************************************************
                          modifyconstraintstudentsminafternoonsperweekform.cpp  -  description
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

#include "modifyconstraintstudentsminafternoonsperweekform.h"
#include "timeconstraint.h"

ModifyConstraintStudentsMinAfternoonsPerWeekForm::ModifyConstraintStudentsMinAfternoonsPerWeekForm(QWidget* parent, ConstraintStudentsMinAfternoonsPerWeek* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, &QPushButton::clicked, this, &ModifyConstraintStudentsMinAfternoonsPerWeekForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyConstraintStudentsMinAfternoonsPerWeekForm::cancel);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	updateMinAfternoonsSpinBox();

	minAfternoonsSpinBox->setValue(ctr->minAfternoonsPerWeek);
}

ModifyConstraintStudentsMinAfternoonsPerWeekForm::~ModifyConstraintStudentsMinAfternoonsPerWeekForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintStudentsMinAfternoonsPerWeekForm::updateMinAfternoonsSpinBox(){
	minAfternoonsSpinBox->setMinimum(0);
	minAfternoonsSpinBox->setMaximum(gt.rules.nDaysPerWeek/2);
}

void ModifyConstraintStudentsMinAfternoonsPerWeekForm::ok()
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

	int min_afternoons=minAfternoonsSpinBox->value();

	this->_ctr->weightPercentage=weight;
	this->_ctr->minAfternoonsPerWeek=min_afternoons;

	QString newcs=this->_ctr->getDetailedDescription(gt.rules);
	gt.rules.addUndoPoint(tr("Modified the constraint:\n\n%1\ninto\n\n%2").arg(oldcs).arg(newcs));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintStudentsMinAfternoonsPerWeekForm::cancel()
{
	this->close();
}
