/***************************************************************************
                          modifyconstraintteacheractivitytagminhoursdailyform.cpp  -  description
                             -------------------
    begin                : 2019
    copyright            : (C) 2019 by Liviu Lalescu
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

#include "modifyconstraintteacheractivitytagminhoursdailyform.h"
#include "timeconstraint.h"

ModifyConstraintTeacherActivityTagMinHoursDailyForm::ModifyConstraintTeacherActivityTagMinHoursDailyForm(QWidget* parent, ConstraintTeacherActivityTagMinHoursDaily* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	updateMinHoursSpinBox();
	
	minHoursSpinBox->setValue(ctr->minHoursDaily);
	
	allowEmptyDaysCheckBox->setChecked(ctr->allowEmptyDays);

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
	
	j=-1;
	activityTagsComboBox->clear();
	for(int k=0; k<gt.rules.activityTagsList.count(); k++){
		ActivityTag* at=gt.rules.activityTagsList.at(k);
		activityTagsComboBox->addItem(at->name);
		if(at->name==this->_ctr->activityTagName)
			j=k;
	}
	assert(j>=0);
	activityTagsComboBox->setCurrentIndex(j);
}

ModifyConstraintTeacherActivityTagMinHoursDailyForm::~ModifyConstraintTeacherActivityTagMinHoursDailyForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintTeacherActivityTagMinHoursDailyForm::updateMinHoursSpinBox(){
	minHoursSpinBox->setMinimum(1);
	minHoursSpinBox->setMaximum(gt.rules.nHoursPerDay);
}

void ModifyConstraintTeacherActivityTagMinHoursDailyForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<100.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage). It has to be 100"));
		return;
	}

	int min_hours=minHoursSpinBox->value();

	QString teacher_name=teachersComboBox->currentText();
	int teacher_ID=gt.rules.searchTeacher(teacher_name);
	if(teacher_ID<0){
		QMessageBox::warning(this, tr("FET warning"),
			tr("Invalid teacher"));
		return;
	}
	
	QString activityTagName=activityTagsComboBox->currentText();
	int ati=gt.rules.searchActivityTag(activityTagName);
	if(ati<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid activity tag"));
		return;
	}

	if(allowEmptyDaysCheckBox->isChecked() && minHoursSpinBox->value()==1){
		QMessageBox::warning(this, tr("FET warning"), tr("Allow empty days is selected and min hours daily is 1, so this would be a useless constraint."));
		return;
	}
	
	this->_ctr->weightPercentage=weight;
	this->_ctr->minHoursDaily=min_hours;
	this->_ctr->teacherName=teacher_name;
	this->_ctr->activityTagName=activityTagName;
	this->_ctr->allowEmptyDays=allowEmptyDaysCheckBox->isChecked();

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintTeacherActivityTagMinHoursDailyForm::cancel()
{
	this->close();
}
