/***************************************************************************
                          modifyconstraintstudentssetintervalmaxdaysperweekform.cpp  -  description
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

#include "modifyconstraintstudentssetintervalmaxdaysperweekform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

ModifyConstraintStudentsSetIntervalMaxDaysPerWeekForm::ModifyConstraintStudentsSetIntervalMaxDaysPerWeekForm(ConstraintStudentsSetIntervalMaxDaysPerWeek* ctr)
{
    setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this /*ModifyConstraintStudentsSetIntervalMaxDaysPerWeekForm_template*/, SLOT(constraintChanged()));
    connect(okPushButton, SIGNAL(clicked()), this /*ModifyConstraintStudentsSetIntervalMaxDaysPerWeekForm_template*/, SLOT(ok()));
    connect(cancelPushButton, SIGNAL(clicked()), this /*ModifyConstraintStudentsSetIntervalMaxDaysPerWeekForm_template*/, SLOT(cancel()));
//    connect(studentsComboBox, SIGNAL(activated(QString)), this /*ModifyConstraintStudentsSetIntervalMaxDaysPerWeekForm_template*/, SLOT(constraintChanged()));
//    connect(maxDaysSpinBox, SIGNAL(valueChanged(int)), this /*ModifyConstraintStudentsSetIntervalMaxDaysPerWeekForm_template*/, SLOT(constraintChanged()));
//    connect(startHourComboBox, SIGNAL(activated(QString)), this /*ModifyConstraintStudentsSetIntervalMaxDaysPerWeekForm_template*/, SLOT(constraintChanged()));
//    connect(endHourComboBox, SIGNAL(activated(QString)), this /*ModifyConstraintStudentsSetIntervalMaxDaysPerWeekForm_template*/, SLOT(constraintChanged()));


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
	
	this->_ctr=ctr;
	
	//compulsoryCheckBox->setChecked(ctr->compulsory);
	weightLineEdit->setText(QString::number(ctr->weightPercentage));
	
	updateMaxDaysSpinBox();
	updateStudentsComboBox();
	
	maxDaysSpinBox->setValue(ctr->maxDaysPerWeek);
	
	for(int i=0; i<gt.rules.nHoursPerDay; i++){
		startHourComboBox->insertItem(gt.rules.hoursOfTheDay[i]);
	}
	startHourComboBox->setCurrentItem(ctr->startHour);

	for(int i=0; i<gt.rules.nHoursPerDay; i++){
		endHourComboBox->insertItem(gt.rules.hoursOfTheDay[i]);
	}
	endHourComboBox->insertItem(tr("End of day"));
	endHourComboBox->setCurrentItem(ctr->endHour);

	constraintChanged();
}

ModifyConstraintStudentsSetIntervalMaxDaysPerWeekForm::~ModifyConstraintStudentsSetIntervalMaxDaysPerWeekForm()
{
}

void ModifyConstraintStudentsSetIntervalMaxDaysPerWeekForm::updateStudentsComboBox(){
	studentsComboBox->clear();
	int i=0, j=-1;
	for(int m=0; m<gt.rules.yearsList.size(); m++){
		StudentsYear* sty=gt.rules.yearsList[m];
		studentsComboBox->insertItem(sty->name);
		if(sty->name==this->_ctr->students)
			j=i;
		i++;
		for(int n=0; n<sty->groupsList.size(); n++){
			StudentsGroup* stg=sty->groupsList[n];
			studentsComboBox->insertItem(stg->name);
			if(stg->name==this->_ctr->students)
				j=i;
			i++;
			for(int p=0; p<stg->subgroupsList.size(); p++){
				StudentsSubgroup* sts=stg->subgroupsList[p];
				studentsComboBox->insertItem(sts->name);
				if(sts->name==this->_ctr->students)
					j=i;
				i++;
			}
		}
	}
	assert(j>=0);
	studentsComboBox->setCurrentItem(j);																

	constraintChanged();
}

void ModifyConstraintStudentsSetIntervalMaxDaysPerWeekForm::updateMaxDaysSpinBox(){
	maxDaysSpinBox->setMinValue(0);
	maxDaysSpinBox->setMaxValue(gt.rules.nDaysPerWeek);	
}

void ModifyConstraintStudentsSetIntervalMaxDaysPerWeekForm::constraintChanged()
{/*
	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=tr("Weight (percentage)=%1\%").arg(weight);
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

void ModifyConstraintStudentsSetIntervalMaxDaysPerWeekForm::ok()
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

	this->_ctr->weightPercentage=weight;
	//this->_ctr->compulsory=compulsory;
	this->_ctr->maxDaysPerWeek=max_days;
	this->_ctr->students=students_name;

	this->_ctr->startHour=startHour;
	this->_ctr->endHour=endHour;

	gt.rules.internalStructureComputed=false;
	
	this->close();
}

void ModifyConstraintStudentsSetIntervalMaxDaysPerWeekForm::cancel()
{
	this->close();
}
