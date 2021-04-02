/***************************************************************************
                          addconstraintteacherintervalmaxdaysperweekform.cpp  -  description
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

#include <QMessageBox>

#include <cstdio>

#include "longtextmessagebox.h"

#include "addconstraintteacherintervalmaxdaysperweekform.h"
#include "timeconstraint.h"

AddConstraintTeacherIntervalMaxDaysPerWeekForm::AddConstraintTeacherIntervalMaxDaysPerWeekForm()
{
    setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this /*AddConstraintTeacherIntervalMaxDaysPerWeekForm_template*/, SLOT(constraintChanged()));
    connect(addConstraintPushButton, SIGNAL(clicked()), this /*AddConstraintTeacherIntervalMaxDaysPerWeekForm_template*/, SLOT(addCurrentConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this /*AddConstraintTeacherIntervalMaxDaysPerWeekForm_template*/, SLOT(close()));
//    connect(teachersComboBox, SIGNAL(activated(QString)), this /*AddConstraintTeacherIntervalMaxDaysPerWeekForm_template*/, SLOT(constraintChanged()));
//    connect(maxDaysSpinBox, SIGNAL(valueChanged(int)), this /*AddConstraintTeacherIntervalMaxDaysPerWeekForm_template*/, SLOT(constraintChanged()));
//    connect(startHourComboBox, SIGNAL(activated(QString)), this /*AddConstraintTeacherIntervalMaxDaysPerWeekForm_template*/, SLOT(constraintChanged()));
//    connect(endHourComboBox, SIGNAL(activated(QString)), this /*AddConstraintTeacherIntervalMaxDaysPerWeekForm_template*/, SLOT(constraintChanged()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	
	QSize tmp5=startHourComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	QSize tmp6=endHourComboBox->minimumSizeHint();
	Q_UNUSED(tmp6);
	
	updateMaxDaysSpinBox();
	updateTeachersComboBox();
	updateStartHoursComboBox();
	updateEndHoursComboBox();
}

AddConstraintTeacherIntervalMaxDaysPerWeekForm::~AddConstraintTeacherIntervalMaxDaysPerWeekForm()
{
}

void AddConstraintTeacherIntervalMaxDaysPerWeekForm::updateTeachersComboBox(){
	teachersComboBox->clear();
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		teachersComboBox->insertItem(tch->name);
	}

	constraintChanged();
}

void AddConstraintTeacherIntervalMaxDaysPerWeekForm::updateMaxDaysSpinBox(){
	maxDaysSpinBox->setMinValue(0);
	maxDaysSpinBox->setMaxValue(gt.rules.nDaysPerWeek);
	maxDaysSpinBox->setValue(gt.rules.nDaysPerWeek);
}

void AddConstraintTeacherIntervalMaxDaysPerWeekForm::updateStartHoursComboBox()
{
	startHourComboBox->clear();
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		startHourComboBox->insertItem(gt.rules.hoursOfTheDay[i]);
	startHourComboBox->setCurrentItem(gt.rules.nHoursPerDay-1);
	
	constraintChanged();
}

void AddConstraintTeacherIntervalMaxDaysPerWeekForm::updateEndHoursComboBox()
{
	endHourComboBox->clear();
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		endHourComboBox->insertItem(gt.rules.hoursOfTheDay[i]);
	endHourComboBox->insertItem(tr("End of day"));
	endHourComboBox->setCurrentItem(gt.rules.nHoursPerDay);
	
	constraintChanged();
}

void AddConstraintTeacherIntervalMaxDaysPerWeekForm::constraintChanged()
{/*
	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=tr("Weight (percentage)=%1").arg(weight);
	s+="\n";

	s+=tr("Teacher interval max days per week");
	s+="\n";
	s+=tr("Teacher=%1").arg(teachersComboBox->currentText());
	s+="\n";

	s+=tr("Interval start hour=%1").arg(startHourComboBox->currentText());
	s+="\n";

	s+=tr("Interval end hour=%1").arg(endHourComboBox->currentText());
	s+="\n";

	s+=tr("Max days per week=%1").arg(maxDaysSpinBox->value());
	s+="\n";

	currentConstraintTextEdit->setText(s);*/
}

void AddConstraintTeacherIntervalMaxDaysPerWeekForm::addCurrentConstraint()
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

	QString teacher_name=teachersComboBox->currentText();
	int teacher_ID=gt.rules.searchTeacher(teacher_name);
	if(teacher_ID<0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid teacher"));
		return;
	}
	
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

	ctr=new ConstraintTeacherIntervalMaxDaysPerWeek(weight, /*compulsory,*/ max_days, teacher_name, startHour, endHour);

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
