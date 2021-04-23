/***************************************************************************
                          modifyconstraintteacherminhourspermorningform.cpp  -  description
                             -------------------
    begin                : 2019
    copyright            : (C) 2019 by Lalescu Liviu
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

#include "modifyconstraintteacherminhourspermorningform.h"
#include "timeconstraint.h"

ModifyConstraintTeacherMinHoursPerMorningForm::ModifyConstraintTeacherMinHoursPerMorningForm(QWidget* parent, ConstraintTeacherMinHoursPerMorning* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	allowEmptyMorningsCheckBox->setChecked(ctr->allowEmptyMornings);
	
	connect(allowEmptyMorningsCheckBox, SIGNAL(toggled(bool)), this, SLOT(allowEmptyMorningsCheckBoxToggled())); //after setChecked(...)
	
	updateMinHoursSpinBox();
	
	minHoursSpinBox->setValue(ctr->minHoursPerMorning);

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

ModifyConstraintTeacherMinHoursPerMorningForm::~ModifyConstraintTeacherMinHoursPerMorningForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintTeacherMinHoursPerMorningForm::updateMinHoursSpinBox(){
	minHoursSpinBox->setMinimum(2);
	minHoursSpinBox->setMaximum(gt.rules.nHoursPerDay);
}

void ModifyConstraintTeacherMinHoursPerMorningForm::ok()
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

	if(!allowEmptyMorningsCheckBox->isChecked()){
		QMessageBox::warning(this, tr("FET information"), tr("Allow empty mornings check box must be checked. If you need to not allow empty mornings for a teacher, "
			"please use the constraint teacher min mornings per week."));
		return;
	}

	int min_hours=minHoursSpinBox->value();

	QString teacher_name=teachersComboBox->currentText();
	int teacher_ID=gt.rules.searchTeacher(teacher_name);
	if(teacher_ID<0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid teacher"));
		return;
	}

	this->_ctr->weightPercentage=weight;
	this->_ctr->minHoursPerMorning=min_hours;
	this->_ctr->teacherName=teacher_name;
	
	this->_ctr->allowEmptyMornings=allowEmptyMorningsCheckBox->isChecked();

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintTeacherMinHoursPerMorningForm::cancel()
{
	this->close();
}

void ModifyConstraintTeacherMinHoursPerMorningForm::allowEmptyMorningsCheckBoxToggled()
{
	bool k=allowEmptyMorningsCheckBox->isChecked();

	if(!k){
		allowEmptyMorningsCheckBox->setChecked(true);
		QMessageBox::information(this, tr("FET information"), tr("This check box must remain checked. If you really need to not allow empty mornings for this teacher,"
			" please use constraint teacher min mornings per week."));
	}
}
