/***************************************************************************
                          modifyconstraintteachersmaxhoursdailyinintervalform.cpp  -  description
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

#include "modifyconstraintteachersmaxhoursdailyinintervalform.h"

ModifyConstraintTeachersMaxHoursDailyInIntervalForm::ModifyConstraintTeachersMaxHoursDailyInIntervalForm(QWidget* parent, ConstraintTeachersMaxHoursDailyInInterval* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, &QPushButton::clicked, this, &ModifyConstraintTeachersMaxHoursDailyInIntervalForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyConstraintTeachersMaxHoursDailyInIntervalForm::cancel);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QSize tmp5=startHourComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	QSize tmp6=endHourComboBox->minimumSizeHint();
	Q_UNUSED(tmp6);

	this->_ctr=ctr;

	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));

	maxHoursSpinBox->setMinimum(0);
	maxHoursSpinBox->setMaximum(gt.rules.nHoursPerDay);
	maxHoursSpinBox->setValue(ctr->maxHoursDaily);

	startHourComboBox->clear();
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		startHourComboBox->addItem(gt.rules.hoursOfTheDay[i]);
	startHourComboBox->setCurrentIndex(ctr->startHour);

	endHourComboBox->clear();
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		endHourComboBox->addItem(gt.rules.hoursOfTheDay[i]);
	endHourComboBox->addItem(tr("End of day"));
	endHourComboBox->setCurrentIndex(ctr->endHour);
}

ModifyConstraintTeachersMaxHoursDailyInIntervalForm::~ModifyConstraintTeachersMaxHoursDailyInIntervalForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintTeachersMaxHoursDailyInIntervalForm::ok()
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
	if(endHour<=startHour){
		QMessageBox::warning(this, tr("FET information"),
			tr("Start hour cannot be greater or equal than end hour"));
		return;
	}

	QString oldcs=this->_ctr->getDetailedDescription(gt.rules);

	this->_ctr->weightPercentage=weight;
	this->_ctr->maxHoursDaily=maxHoursSpinBox->value();
	this->_ctr->startHour=startHour;
	this->_ctr->endHour=endHour;

	QString newcs=this->_ctr->getDetailedDescription(gt.rules);
	gt.rules.addUndoPoint(tr("Modified the constraint:\n\n%1\ninto\n\n%2").arg(oldcs).arg(newcs));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintTeachersMaxHoursDailyInIntervalForm::cancel()
{
	this->close();
}
