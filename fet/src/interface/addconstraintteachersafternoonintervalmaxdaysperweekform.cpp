/***************************************************************************
                          addconstraintteachersafternoonintervalmaxdaysperweekform.cpp  -  description
                             -------------------
    begin                : 2020
    copyright            : (C) 2020 by Lalescu Liviu
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

#include "longtextmessagebox.h"

#include "addconstraintteachersafternoonintervalmaxdaysperweekform.h"
#include "timeconstraint.h"

AddConstraintTeachersAfternoonIntervalMaxDaysPerWeekForm::AddConstraintTeachersAfternoonIntervalMaxDaysPerWeekForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, SIGNAL(clicked()), this, SLOT(addCurrentConstraint()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QSize tmp5=startHourComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	QSize tmp6=endHourComboBox->minimumSizeHint();
	Q_UNUSED(tmp6);
	
	updateMaxDaysSpinBox();
	updateStartHoursComboBox();
	updateEndHoursComboBox();
}

AddConstraintTeachersAfternoonIntervalMaxDaysPerWeekForm::~AddConstraintTeachersAfternoonIntervalMaxDaysPerWeekForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintTeachersAfternoonIntervalMaxDaysPerWeekForm::updateMaxDaysSpinBox(){
	maxDaysSpinBox->setMinimum(0);
	maxDaysSpinBox->setMaximum(gt.rules.nDaysPerWeek/2);
	maxDaysSpinBox->setValue(gt.rules.nDaysPerWeek/2);
}

void AddConstraintTeachersAfternoonIntervalMaxDaysPerWeekForm::updateStartHoursComboBox()
{
	startHourComboBox->clear();
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		startHourComboBox->addItem(gt.rules.hoursOfTheDay[i]);
	startHourComboBox->setCurrentIndex(gt.rules.nHoursPerDay-1);
}

void AddConstraintTeachersAfternoonIntervalMaxDaysPerWeekForm::updateEndHoursComboBox()
{
	endHourComboBox->clear();
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		endHourComboBox->addItem(gt.rules.hoursOfTheDay[i]);
	endHourComboBox->addItem(tr("End of day"));
	endHourComboBox->setCurrentIndex(gt.rules.nHoursPerDay);
}

void AddConstraintTeachersAfternoonIntervalMaxDaysPerWeekForm::addCurrentConstraint()
{
	TimeConstraint *ctr=nullptr;

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

	int max_days=maxDaysSpinBox->value();

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

	ctr=new ConstraintTeachersAfternoonIntervalMaxDaysPerWeek(weight, max_days, startHour, endHour);

	bool tmp2=gt.rules.addTimeConstraint(ctr);
	if(tmp2)
		LongTextMessageBox::information(this, tr("FET information"),
			tr("Constraint added:")+"\n\n"+ctr->getDetailedDescription(gt.rules));
	else{
		QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - please report error"));
		delete ctr;
	}
}