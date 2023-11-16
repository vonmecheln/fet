/***************************************************************************
                          modifyconstraintteachersmaxafternoonsperweekform.cpp  -  description
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

#include <QMessageBox>

#include "modifyconstraintteachersmaxafternoonsperweekform.h"
#include "timeconstraint.h"

ModifyConstraintTeachersMaxAfternoonsPerWeekForm::ModifyConstraintTeachersMaxAfternoonsPerWeekForm(QWidget* parent, ConstraintTeachersMaxAfternoonsPerWeek* ctr): QDialog(parent)
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

ModifyConstraintTeachersMaxAfternoonsPerWeekForm::~ModifyConstraintTeachersMaxAfternoonsPerWeekForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintTeachersMaxAfternoonsPerWeekForm::updateMaxAfternoonsSpinBox(){
	maxAfternoonsSpinBox->setMinimum(1);
	maxAfternoonsSpinBox->setMaximum(gt.rules.nDaysPerWeek/2);
}

void ModifyConstraintTeachersMaxAfternoonsPerWeekForm::ok()
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

	int max_afternoons=maxAfternoonsSpinBox->value();

	this->_ctr->weightPercentage=weight;
	this->_ctr->maxAfternoonsPerWeek=max_afternoons;

	QString newcs=this->_ctr->getDetailedDescription(gt.rules);
	gt.rules.addUndoPoint(tr("Modified the constraint:\n\n%1\ninto\n\n%2").arg(oldcs).arg(newcs));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintTeachersMaxAfternoonsPerWeekForm::cancel()
{
	this->close();
}
