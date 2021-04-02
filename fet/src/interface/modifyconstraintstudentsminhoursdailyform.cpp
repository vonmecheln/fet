/***************************************************************************
                          modifyconstraintstudentsminhoursdailyform.cpp  -  description
                             -------------------
    begin                : July 19, 2007
    copyright            : (C) 2007 by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QMessageBox>

#include <cstdio>

#include "modifyconstraintstudentsminhoursdailyform.h"
#include "timeconstraint.h"

ModifyConstraintStudentsMinHoursDailyForm::ModifyConstraintStudentsMinHoursDailyForm(ConstraintStudentsMinHoursDaily* ctr)
{
    setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this /*ModifyConstraintStudentsMinHoursDailyForm_template*/, SLOT(constraintChanged()));
    connect(okPushButton, SIGNAL(clicked()), this /*ModifyConstraintStudentsMinHoursDailyForm_template*/, SLOT(ok()));
    connect(cancelPushButton, SIGNAL(clicked()), this /*ModifyConstraintStudentsMinHoursDailyForm_template*/, SLOT(cancel()));
//    connect(minHoursSpinBox, SIGNAL(valueChanged(int)), this /*ModifyConstraintStudentsMinHoursDailyForm_template*/, SLOT(constraintChanged()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	this->_ctr=ctr;
	
	//compulsoryCheckBox->setChecked(ctr->compulsory);
	weightLineEdit->setText(QString::number(ctr->weightPercentage));
	
	allowEmptyDaysCheckBox->setChecked(ctr->allowEmptyDays);
	
	connect(allowEmptyDaysCheckBox, SIGNAL(toggled(bool)), this, SLOT(allowEmptyDaysCheckBoxToggled())); //after set checked!

	if(ENABLE_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS)
		allowLabel->setText(tr("Advanced usage: enabled"));
	else
		allowLabel->setText(tr("Advanced usage: not enabled"));
	
	minHoursSpinBox->setMinValue(1);
	minHoursSpinBox->setMaxValue(gt.rules.nHoursPerDay);
	minHoursSpinBox->setValue(ctr->minHoursDaily);
}

ModifyConstraintStudentsMinHoursDailyForm::~ModifyConstraintStudentsMinHoursDailyForm()
{
}

void ModifyConstraintStudentsMinHoursDailyForm::constraintChanged()
{/*
	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=tr("Weight (percentage)=%1\%").arg(weight);
	s+="\n";

	s+=tr("Students min hours daily");
	s+="\n";

	s+=tr("Min hours:%1").arg(minHoursSpinBox->value());
	s+="\n";

	currentConstraintTextEdit->setText(s);*/
}

void ModifyConstraintStudentsMinHoursDailyForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
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

	if(!ENABLE_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS && allowEmptyDaysCheckBox->isChecked()){
		QMessageBox::warning(this, tr("FET warning"), tr("Empty days for students min hours daily constraints are not enabled. You must enable them from the Settings->Advanced menu."));
		return;
	}

	if(allowEmptyDaysCheckBox->isChecked() && minHoursSpinBox->value()<2){
		QMessageBox::warning(this, tr("FET warning"), tr("If you allow empty days, the min hours must be at least 2 (to make it a non-trivial constraint)"));
		return;
	}

/*	bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;*/

	this->_ctr->weightPercentage=weight;
	//this->_ctr->compulsory=compulsory;
	//this->_ctr->minHoursDaily=minHoursSpinBox->value();
	this->_ctr->minHoursDaily=minHoursSpinBox->value();
	
	this->_ctr->allowEmptyDays=allowEmptyDaysCheckBox->isChecked();

	gt.rules.internalStructureComputed=false;
	
	this->close();
}

void ModifyConstraintStudentsMinHoursDailyForm::cancel()
{
	this->close();
}

void ModifyConstraintStudentsMinHoursDailyForm::allowEmptyDaysCheckBoxToggled()
{
	bool k=allowEmptyDaysCheckBox->isChecked();
	
	if(k && !ENABLE_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS){
		allowEmptyDaysCheckBox->setChecked(false);
		QString s=tr("Advanced usage is not enabled. To be able to select 'Allow empty days' for the constraints of type min hours daily for students, you must enable the option from the Settings->Advanced menu.",
			"'Allow empty days' is an option which the user can enable and then he can select it.");
		s+="\n\n";
		s+=tr("Explanation: only select this option if your institution allows empty days for students and a timetable is possible with empty days for students."
			" Otherwise, it is IMPERATIVE (for performance reasons) to not select this option (or FET may not be able to find a timetable).");
		s+="\n\n";
		s+=tr("Use with caution.");
		QMessageBox::information(this, tr("FET information"), s);
	}
}
