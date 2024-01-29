/***************************************************************************
                          modifyconstraintteacherminhoursdailyform.cpp  -  description
                             -------------------
    begin                : Sept 21, 2007
    copyright            : (C) 2007 by Liviu Lalescu
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

#include "modifyconstraintteacherminhoursdailyform.h"
#include "timeconstraint.h"

ModifyConstraintTeacherMinHoursDailyForm::ModifyConstraintTeacherMinHoursDailyForm(QWidget* parent, ConstraintTeacherMinHoursDaily* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, &QPushButton::clicked, this, &ModifyConstraintTeacherMinHoursDailyForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyConstraintTeacherMinHoursDailyForm::cancel);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	allowEmptyDaysCheckBox->setChecked(ctr->allowEmptyDays);
	
	connect(allowEmptyDaysCheckBox, &QCheckBox::toggled, this, &ModifyConstraintTeacherMinHoursDailyForm::allowEmptyDaysCheckBoxToggled); //after setChecked(...)
	
	updateMinHoursSpinBox();
	
	minHoursSpinBox->setValue(ctr->minHoursDaily);

	teachersComboBox->clear();
	int i=0, j=-1;
	for(int k=0; k<gt.rules.teachersList.size(); k++, i++){
		Teacher* tch=gt.rules.teachersList[k];
		teachersComboBox->addItem(tch->name);
		if(tch->name==this->_ctr->teacherName)
			j=i;
	}
	assert(j>=0);
	teachersComboBox->setCurrentIndex(j);
}

ModifyConstraintTeacherMinHoursDailyForm::~ModifyConstraintTeacherMinHoursDailyForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintTeacherMinHoursDailyForm::updateMinHoursSpinBox(){
	minHoursSpinBox->setMinimum(2);
	minHoursSpinBox->setMaximum(gt.rules.nHoursPerDay);	
}

void ModifyConstraintTeacherMinHoursDailyForm::ok()
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

	if(!allowEmptyDaysCheckBox->isChecked()){
		if(gt.rules.mode!=MORNINGS_AFTERNOONS){
			QMessageBox::warning(this, tr("FET information"), tr("Allow empty days check box must be checked. If you need to not allow empty days for the teachers, "
				"please use the constraint teachers min days per week"));
			return;
		}
		else{
			QMessageBox::warning(this, tr("FET information"), tr("Allow empty days check box must be checked. If you need to not allow empty days for a teacher, "
				"please use the constraint teacher min days per week (but the min days per week constraint is for real days. You can also use the "
				"constraints teacher min mornings/afternoons per week.)"));
			return;
		}
	}

	int min_hours=minHoursSpinBox->value();

	QString teacher_name=teachersComboBox->currentText();
	int teacher_ID=gt.rules.searchTeacher(teacher_name);
	if(teacher_ID<0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid teacher"));
		return;
	}

	QString oldcs=this->_ctr->getDetailedDescription(gt.rules);

	this->_ctr->weightPercentage=weight;
	this->_ctr->minHoursDaily=min_hours;
	this->_ctr->teacherName=teacher_name;
	
	this->_ctr->allowEmptyDays=allowEmptyDaysCheckBox->isChecked();

	QString newcs=this->_ctr->getDetailedDescription(gt.rules);
	gt.rules.addUndoPoint(tr("Modified the constraint:\n\n%1\ninto\n\n%2").arg(oldcs).arg(newcs));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintTeacherMinHoursDailyForm::cancel()
{
	this->close();
}

void ModifyConstraintTeacherMinHoursDailyForm::allowEmptyDaysCheckBoxToggled()
{
	bool k=allowEmptyDaysCheckBox->isChecked();

	if(!k){
		allowEmptyDaysCheckBox->setChecked(true);
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			QMessageBox::information(this, tr("FET information"), tr("This check box must remain checked. If you really need to not allow empty days for the teachers,"
				" please use constraint teachers min days per week"));
		else
			QMessageBox::information(this, tr("FET information"), tr("This check box must remain checked. If you really need to not allow empty days for this teacher,"
				" please use constraint teacher min days per week (but the min days per week constraint is for real days. You can also use the "
				"constraints teacher min mornings/afternoons per week.)"));
	}
}
