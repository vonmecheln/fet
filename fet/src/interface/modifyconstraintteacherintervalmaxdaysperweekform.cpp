/***************************************************************************
                          modifyconstraintteacherintervalmaxdaysperweekform.cpp  -  description
                             -------------------
    begin                : Mar 30, 2006
    copyright            : (C) 2006 by Lalescu Liviu
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

#include "modifyconstraintteacherintervalmaxdaysperweekform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

#define yesNo(x)	((x)==0?QObject::tr("no"):QObject::tr("yes"))

ModifyConstraintTeacherIntervalMaxDaysPerWeekForm::ModifyConstraintTeacherIntervalMaxDaysPerWeekForm(ConstraintTeacherIntervalMaxDaysPerWeek* ctr)
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	this->_ctr=ctr;
	
	compulsoryCheckBox->setChecked(ctr->compulsory);
	weightLineEdit->setText(QString::number(ctr->weight));
	
	updatePeriodGroupBox();
	updateTeachersComboBox();

	startHourComboBox->setCurrentItem(ctr->h1);
	endHourComboBox->setCurrentItem(ctr->h2);
	maxDaysSpinBox->setValue(ctr->n);

	constraintChanged();
}

ModifyConstraintTeacherIntervalMaxDaysPerWeekForm::~ModifyConstraintTeacherIntervalMaxDaysPerWeekForm()
{
}

void ModifyConstraintTeacherIntervalMaxDaysPerWeekForm::updateTeachersComboBox(){
	teachersComboBox->clear();
	int j=-1;
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* t=gt.rules.teachersList[i];
		teachersComboBox->insertItem(t->name);
		if(t->name==this->_ctr->teacher)
			j=i;
	}
	
	assert(j>=0);
	teachersComboBox->setCurrentItem(j);																

	constraintChanged();
}

void ModifyConstraintTeacherIntervalMaxDaysPerWeekForm::updatePeriodGroupBox(){
	startHourComboBox->clear();
	for(int i=0; i<=gt.rules.nHoursPerDay; i++)
		startHourComboBox->insertItem(gt.rules.hoursOfTheDay[i]);

	endHourComboBox->clear();
	for(int i=0; i<=gt.rules.nHoursPerDay; i++)
		endHourComboBox->insertItem(gt.rules.hoursOfTheDay[i]);

	maxDaysSpinBox->setMinValue(0);
	maxDaysSpinBox->setMaxValue(gt.rules.nDaysPerWeek);	
}

void ModifyConstraintTeacherIntervalMaxDaysPerWeekForm::constraintChanged()
{
	QString s;
	s+=QObject::tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=QObject::tr(QString("Weight=%1").arg(weight));
	s+="\n";

	bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;
	s+=QObject::tr("Compulsory=%1").arg(yesNo(compulsory));
	s+="\n";

	s+=QObject::tr("Teacher interval max days per week");
	s+="\n";
	s+=QObject::tr("Teacher=%1").arg(teachersComboBox->currentText());
	s+="\n";

	s+=QObject::tr("Max days=%1").arg(maxDaysSpinBox->value());
	s+="\n";

	int startHour=startHourComboBox->currentItem();
	if(startHour<0 || startHour>gt.rules.nHoursPerDay){
		s+=QObject::tr("Invalid start hour");
		s+="\n";
	}
	else{
		s+=QObject::tr("Start hour:%1").arg(startHourComboBox->currentText());
		s+="\n";
	}

	int endHour=endHourComboBox->currentItem();
	if(endHour<0 || endHour>gt.rules.nHoursPerDay){
		s+=QObject::tr("Invalid end hour");
		s+="\n";
	}
	else{
		s+=QObject::tr("End hour:%1").arg(endHourComboBox->currentText());
		s+="\n";
	}

	currentConstraintTextEdit->setText(s);
}

void ModifyConstraintTeacherIntervalMaxDaysPerWeekForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<0.0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid weight"));
		return;
	}

	bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;

	int startHour=startHourComboBox->currentItem();
	if(startHour<0 || startHour>gt.rules.nHoursPerDay){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid start hour"));
		return;
	}
	int endHour=-1;
	endHour=endHourComboBox->currentItem();
	if(endHour<0 || endHour>gt.rules.nHoursPerDay){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid end hour"));
		return;
	}
	if(endHour<=startHour){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("End hour <= start hour - impossible"));
		return;
	}

	QString teacher_name=teachersComboBox->currentText();
	int t=gt.rules.searchTeacher(teacher_name);
	if(t<0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid teacher"));
		return;
	}

	this->_ctr->weight=weight;
	this->_ctr->compulsory=compulsory;
	this->_ctr->teacher=teacher_name;
	this->_ctr->h1=startHour;
	this->_ctr->h2=endHour;
	this->_ctr->n=maxDaysSpinBox->value();

	gt.rules.internalStructureComputed=false;
	
	this->close();
}

void ModifyConstraintTeacherIntervalMaxDaysPerWeekForm::cancel()
{
	this->close();
}
