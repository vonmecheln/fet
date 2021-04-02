/***************************************************************************
                          addconstraintteachersintervalmaxdaysperweekform.cpp  -  description
                             -------------------
    begin                : 2008
    copyright            : (C) 2008 by Lalescu Liviu
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

#include "addconstraintteachersintervalmaxdaysperweekform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

AddConstraintTeachersIntervalMaxDaysPerWeekForm::AddConstraintTeachersIntervalMaxDaysPerWeekForm()
{
	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	updateMaxDaysSpinBox();
	updateStartHoursComboBox();
	updateEndHoursComboBox();
}

AddConstraintTeachersIntervalMaxDaysPerWeekForm::~AddConstraintTeachersIntervalMaxDaysPerWeekForm()
{
}

void AddConstraintTeachersIntervalMaxDaysPerWeekForm::updateMaxDaysSpinBox(){
	maxDaysSpinBox->setMinValue(0);
	maxDaysSpinBox->setMaxValue(gt.rules.nDaysPerWeek);
	maxDaysSpinBox->setValue(gt.rules.nDaysPerWeek);
}

void AddConstraintTeachersIntervalMaxDaysPerWeekForm::updateStartHoursComboBox()
{
	startHourComboBox->clear();
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		startHourComboBox->insertItem(gt.rules.hoursOfTheDay[i]);
	startHourComboBox->setCurrentItem(gt.rules.nHoursPerDay-1);
	
	constraintChanged();
}

void AddConstraintTeachersIntervalMaxDaysPerWeekForm::updateEndHoursComboBox()
{
	endHourComboBox->clear();
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		endHourComboBox->insertItem(gt.rules.hoursOfTheDay[i]);
	endHourComboBox->insertItem(tr("End of day"));
	endHourComboBox->setCurrentItem(gt.rules.nHoursPerDay);
	
	constraintChanged();
}

void AddConstraintTeachersIntervalMaxDaysPerWeekForm::constraintChanged()
{
	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=tr("Weight (percentage)=%1").arg(weight);
	s+="\n";

	s+=tr("Teachers interval max days per week");
	s+="\n";

	s+=tr("Interval start hour=%1").arg(startHourComboBox->currentText());
	s+="\n";

	s+=tr("Interval end hour=%1").arg(endHourComboBox->currentText());
	s+="\n";

	s+=tr("Max days per week=%1").arg(maxDaysSpinBox->value());
	s+="\n";

	currentConstraintTextEdit->setText(s);
}

void AddConstraintTeachersIntervalMaxDaysPerWeekForm::addCurrentConstraint()
{
	TimeConstraint *ctr=NULL;

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

	/*bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;*/

	int max_days=maxDaysSpinBox->value();

	int startHour=startHourComboBox->currentItem();
	int endHour=endHourComboBox->currentItem();
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

	ctr=new ConstraintTeachersIntervalMaxDaysPerWeek(weight, /*compulsory,*/ max_days, /*teacher_name,*/ startHour, endHour);

	bool tmp2=gt.rules.addTimeConstraint(ctr);
	if(tmp2)
		QMessageBox::information(this, tr("FET information"),
			tr("Constraint added"));
	else{
		QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - please report error"));
		delete ctr;
	}
}
