/***************************************************************************
                          modifyconstraintstudentssetnotavailableform.cpp  -  description
                             -------------------
    begin                : Feb 10, 2005
    copyright            : (C) 2005 by Lalescu Liviu
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

#include "modifyconstraintstudentssetnotavailableform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

#define yesNo(x)	((x)==0?QObject::tr("no"):QObject::tr("yes"))

ModifyConstraintStudentsSetNotAvailableForm::ModifyConstraintStudentsSetNotAvailableForm(ConstraintStudentsSetNotAvailable* ctr)
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);
	
	this->_ctr=ctr;
	
	//compulsoryCheckBox->setChecked(ctr->compulsory);
	weightLineEdit->setText(QString::number(ctr->weightPercentage));
	
	updatePeriodGroupBox();
	updateStudentsComboBox();

	dayComboBox->setCurrentItem(ctr->d);
	startHourComboBox->setCurrentItem(ctr->h1);
	endHourComboBox->setCurrentItem(ctr->h2);

	constraintChanged();
}

ModifyConstraintStudentsSetNotAvailableForm::~ModifyConstraintStudentsSetNotAvailableForm()
{
}

void ModifyConstraintStudentsSetNotAvailableForm::updateStudentsComboBox(){
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

void ModifyConstraintStudentsSetNotAvailableForm::updatePeriodGroupBox(){
	startHourComboBox->clear();
	for(int i=0; i<=gt.rules.nHoursPerDay; i++)
		startHourComboBox->insertItem(gt.rules.hoursOfTheDay[i]);

	endHourComboBox->clear();
	for(int i=0; i<=gt.rules.nHoursPerDay; i++)
		endHourComboBox->insertItem(gt.rules.hoursOfTheDay[i]);

	dayComboBox->clear();
	for(int i=0; i<gt.rules.nDaysPerWeek; i++)
		dayComboBox->insertItem(gt.rules.daysOfTheWeek[i]);
}

void ModifyConstraintStudentsSetNotAvailableForm::constraintChanged()
{
	QString s;
	s+=QObject::tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=QObject::tr(QString("Weight (percentage)=%1\%").arg(weight));
	s+="\n";

	/*bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;
	s+=QObject::tr("Compulsory=%1").arg(yesNo(compulsory));
	s+="\n";*/

	s+=QObject::tr("Students set not available");
	s+="\n";
	s+=QObject::tr("Students set=%1").arg(studentsComboBox->currentText());
	s+="\n";

	int day=dayComboBox->currentItem();
	if(day<0 || day>gt.rules.nDaysPerWeek){
		s+=QObject::tr("Invalid day");
		s+="\n";
	}
	else{
		s+=QObject::tr("Day:%1").arg(dayComboBox->currentText());
		s+="\n";
	}

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

void ModifyConstraintStudentsSetNotAvailableForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<100.0 || weight>100.0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid weight (percentage). It has to be 100"));
		return;
	}

	/*bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;*/

	int day=dayComboBox->currentItem();
	if(day<0 || day>gt.rules.nDaysPerWeek){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid day"));
		return;
	}
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

	QString students_name=studentsComboBox->currentText();
	StudentsSet* s=gt.rules.searchStudentsSet(students_name);
	if(s==NULL){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid students set"));
		return;
	}

	this->_ctr->weightPercentage=weight;
	//this->_ctr->compulsory=compulsory;
	this->_ctr->students=students_name;
	this->_ctr->d=day;
	this->_ctr->h1=startHour;
	this->_ctr->h2=endHour;

	gt.rules.internalStructureComputed=false;
	
	this->close();
}

void ModifyConstraintStudentsSetNotAvailableForm::cancel()
{
	this->close();
}
