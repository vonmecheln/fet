/***************************************************************************
                          modifyconstraintteachersminhoursperafternoonform.cpp  -  description
                             -------------------
    begin                : 2022
    copyright            : (C) 2022 by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
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

#include "modifyconstraintteachersminhoursperafternoonform.h"
#include "timeconstraint.h"

ModifyConstraintTeachersMinHoursPerAfternoonForm::ModifyConstraintTeachersMinHoursPerAfternoonForm(QWidget* parent, ConstraintTeachersMinHoursPerAfternoon* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	allowEmptyAfternoonsCheckBox->setChecked(ctr->allowEmptyAfternoons);
	
	connect(allowEmptyAfternoonsCheckBox, SIGNAL(toggled(bool)), this, SLOT(allowEmptyAfternoonsCheckBoxToggled())); //after setChecked(...)
	
	updateMinHoursSpinBox();
	
	minHoursSpinBox->setValue(ctr->minHoursPerAfternoon);
}

ModifyConstraintTeachersMinHoursPerAfternoonForm::~ModifyConstraintTeachersMinHoursPerAfternoonForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintTeachersMinHoursPerAfternoonForm::updateMinHoursSpinBox(){
	minHoursSpinBox->setMinimum(2);
	minHoursSpinBox->setMaximum(gt.rules.nHoursPerDay);
}

void ModifyConstraintTeachersMinHoursPerAfternoonForm::ok()
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

	if(!allowEmptyAfternoonsCheckBox->isChecked()){
		QMessageBox::warning(this, tr("FET information"), tr("Allow empty afternoons check box must be checked. If you need to not allow empty afternoons for the teachers, "
			"please use the constraint teachers min afternoons per week."));
		return;
	}

	int min_hours=minHoursSpinBox->value();

	this->_ctr->weightPercentage=weight;
	this->_ctr->minHoursPerAfternoon=min_hours;
	
	this->_ctr->allowEmptyAfternoons=allowEmptyAfternoonsCheckBox->isChecked();

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintTeachersMinHoursPerAfternoonForm::cancel()
{
	this->close();
}

void ModifyConstraintTeachersMinHoursPerAfternoonForm::allowEmptyAfternoonsCheckBoxToggled()
{
	bool k=allowEmptyAfternoonsCheckBox->isChecked();

	if(!k){
		allowEmptyAfternoonsCheckBox->setChecked(true);
		QMessageBox::information(this, tr("FET information"), tr("This check box must remain checked. If you really need to not allow empty afternoons for the teachers,"
			" please use constraint teachers min afternoons per week."));
	}
}
