/***************************************************************************
                          addconstraintstudentssetintervalmaxdaysperweekform.cpp  -  description
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

#include "addconstraintstudentssetintervalmaxdaysperweekform.h"
#include "timeconstraint.h"

AddConstraintStudentsSetIntervalMaxDaysPerWeekForm::AddConstraintStudentsSetIntervalMaxDaysPerWeekForm()
{
     setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this /*AddConstraintStudentsSetIntervalMaxDaysPerWeekForm_template*/, SLOT(constraintChanged()));
    connect(addConstraintPushButton, SIGNAL(clicked()), this /*AddConstraintStudentsSetIntervalMaxDaysPerWeekForm_template*/, SLOT(addCurrentConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this /*AddConstraintStudentsSetIntervalMaxDaysPerWeekForm_template*/, SLOT(close()));
//    connect(studentsComboBox, SIGNAL(activated(QString)), this /*AddConstraintStudentsSetIntervalMaxDaysPerWeekForm_template*/, SLOT(constraintChanged()));
//    connect(maxDaysSpinBox, SIGNAL(valueChanged(int)), this /*AddConstraintStudentsSetIntervalMaxDaysPerWeekForm_template*/, SLOT(constraintChanged()));
//    connect(startHourComboBox, SIGNAL(activated(QString)), this /*AddConstraintStudentsSetIntervalMaxDaysPerWeekForm_template*/, SLOT(constraintChanged()));
//    connect(endHourComboBox, SIGNAL(activated(QString)), this /*AddConstraintStudentsSetIntervalMaxDaysPerWeekForm_template*/, SLOT(constraintChanged()));

	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);

	QSize tmp5=startHourComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	QSize tmp6=endHourComboBox->minimumSizeHint();
	Q_UNUSED(tmp6);

	updateMaxDaysSpinBox();
	updateStudentsComboBox();
	updateStartHoursComboBox();
	updateEndHoursComboBox();
}

AddConstraintStudentsSetIntervalMaxDaysPerWeekForm::~AddConstraintStudentsSetIntervalMaxDaysPerWeekForm()
{
}

void AddConstraintStudentsSetIntervalMaxDaysPerWeekForm::updateStudentsComboBox()
{
	studentsComboBox->clear();	
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* sty=gt.rules.yearsList[i];
		studentsComboBox->insertItem(sty->name);
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			studentsComboBox->insertItem(stg->name);
			for(int k=0; k<stg->subgroupsList.size(); k++){
				StudentsSubgroup* sts=stg->subgroupsList[k];
				studentsComboBox->insertItem(sts->name);
			}
		}
	}

	constraintChanged();
}

void AddConstraintStudentsSetIntervalMaxDaysPerWeekForm::updateMaxDaysSpinBox(){
	maxDaysSpinBox->setMinValue(0);
	maxDaysSpinBox->setMaxValue(gt.rules.nDaysPerWeek);
	maxDaysSpinBox->setValue(gt.rules.nDaysPerWeek);
}

void AddConstraintStudentsSetIntervalMaxDaysPerWeekForm::updateStartHoursComboBox()
{
	startHourComboBox->clear();
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		startHourComboBox->insertItem(gt.rules.hoursOfTheDay[i]);
	startHourComboBox->setCurrentItem(gt.rules.nHoursPerDay-1);
	
	constraintChanged();
}

void AddConstraintStudentsSetIntervalMaxDaysPerWeekForm::updateEndHoursComboBox()
{
	endHourComboBox->clear();
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		endHourComboBox->insertItem(gt.rules.hoursOfTheDay[i]);
	endHourComboBox->insertItem(tr("End of day"));
	endHourComboBox->setCurrentItem(gt.rules.nHoursPerDay);
	
	constraintChanged();
}

void AddConstraintStudentsSetIntervalMaxDaysPerWeekForm::constraintChanged()
{/*
	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=tr("Weight (percentage)=%1").arg(weight);
	s+="\n";

	s+=tr("Students set interval max days per week");
	s+="\n";
	s+=tr("Students set=%1").arg(studentsComboBox->currentText());
	s+="\n";

	s+=tr("Interval start hour=%1").arg(startHourComboBox->currentText());
	s+="\n";

	s+=tr("Interval end hour=%1").arg(endHourComboBox->currentText());
	s+="\n";

	s+=tr("Max days per week=%1").arg(maxDaysSpinBox->value());
	s+="\n";

	currentConstraintTextEdit->setText(s);*/
}

void AddConstraintStudentsSetIntervalMaxDaysPerWeekForm::addCurrentConstraint()
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

	QString students_name=studentsComboBox->currentText();
	StudentsSet* s=gt.rules.searchStudentsSet(students_name);
	if(s==NULL){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid students set"));
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

	ctr=new ConstraintStudentsSetIntervalMaxDaysPerWeek(weight, /*compulsory,*/ max_days, students_name, startHour, endHour);

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
