/***************************************************************************
                          modifyconstraintteachermaxroomchangesperdayinintervalform.cpp  -  description
                             -------------------
    begin                : 2024
    copyright            : (C) 2024 by Liviu Lalescu
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

#include "modifyconstraintteachermaxroomchangesperdayinintervalform.h"

ModifyConstraintTeacherMaxRoomChangesPerDayInIntervalForm::ModifyConstraintTeacherMaxRoomChangesPerDayInIntervalForm(QWidget* parent, ConstraintTeacherMaxRoomChangesPerDayInInterval* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, &QPushButton::clicked, this, &ModifyConstraintTeacherMaxRoomChangesPerDayInIntervalForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyConstraintTeacherMaxRoomChangesPerDayInIntervalForm::cancel);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QSize tmp5=startHourComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	QSize tmp6=endHourComboBox->minimumSizeHint();
	Q_UNUSED(tmp6);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	
	this->_ctr=ctr;

	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));

	maxChangesSpinBox->setMinimum(0);
	maxChangesSpinBox->setMaximum(gt.rules.nHoursPerDay);
	maxChangesSpinBox->setValue(ctr->maxRoomChangesPerDay);

	startHourComboBox->clear();
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		startHourComboBox->addItem(gt.rules.hoursOfTheDay[i]);
	startHourComboBox->setCurrentIndex(ctr->intervalStart);

	endHourComboBox->clear();
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		endHourComboBox->addItem(gt.rules.hoursOfTheDay[i]);
	endHourComboBox->addItem(tr("End of day"));
	endHourComboBox->setCurrentIndex(ctr->intervalEnd);

	updateTeachersComboBox();
}

ModifyConstraintTeacherMaxRoomChangesPerDayInIntervalForm::~ModifyConstraintTeacherMaxRoomChangesPerDayInIntervalForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintTeacherMaxRoomChangesPerDayInIntervalForm::updateTeachersComboBox(){
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

void ModifyConstraintTeacherMaxRoomChangesPerDayInIntervalForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<100.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage). It has to be 100"));
		return;
	}

	int startHour=startHourComboBox->currentIndex();
	int endHour=endHourComboBox->currentIndex();
	if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
		QMessageBox::warning(this, tr("FET information"),
			tr("Start hour invalid"));
		return;
	}
	if(endHour<0 || endHour>gt.rules.nHoursPerDay){
		QMessageBox::warning(this, tr("FET information"),
			tr("End hour invalid"));
		return;
	}
	if(endHour-startHour<2){
		QMessageBox::warning(this, tr("FET information"),
			tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
		return;
	}

	QString teacher_name=teachersComboBox->currentText();
	int teacher_ID=gt.rules.searchTeacher(teacher_name);
	if(teacher_ID<0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid teacher"));
		return;
	}

	QString oldcs=this->_ctr->getDetailedDescription(gt.rules);

	this->_ctr->weightPercentage=weight;
	this->_ctr->maxRoomChangesPerDay=maxChangesSpinBox->value();
	this->_ctr->intervalStart=startHour;
	this->_ctr->intervalEnd=endHour;
	this->_ctr->teacherName=teacher_name;

	QString newcs=this->_ctr->getDetailedDescription(gt.rules);
	gt.rules.addUndoPoint(tr("Modified the constraint:\n\n%1\ninto\n\n%2").arg(oldcs).arg(newcs));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintTeacherMaxRoomChangesPerDayInIntervalForm::cancel()
{
	this->close();
}
