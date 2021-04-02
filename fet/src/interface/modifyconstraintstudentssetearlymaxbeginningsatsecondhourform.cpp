/***************************************************************************
                          modifyconstraintstudentssetearlymaxbeginningsatsecondhourform.cpp  -  description
                             -------------------
    begin                : July 18, 2007
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

#include "modifyconstraintstudentssetearlymaxbeginningsatsecondhourform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

//#define yesNo(x)	((x)==0?tr("no"):tr("yes"))

ModifyConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm::ModifyConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm(ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour* ctr)
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
	
	maxBeginningsSpinBox->setMinValue(0);
	maxBeginningsSpinBox->setMaxValue(gt.rules.nDaysPerWeek);
	maxBeginningsSpinBox->setValue(ctr->maxBeginningsAtSecondHour);
}

ModifyConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm::~ModifyConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm()
{
}

void ModifyConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm::constraintChanged()
{
	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	s+=tr("Students set=%1").arg(studentsComboBox->currentText());
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=tr("Weight (percentage)=%1").arg(weight);
	s+="\n";

	/*bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;
	s+=tr("Compulsory=%1").arg(yesNo(compulsory));
	s+="\n";*/

	//s+=tr("Students set must begin activities as early as possible (permitted by not available and break)");
	//s+="\n";
	s+=tr("Students set must begin activities early, with maximum %1 beginnings at the second available hour, per week (not available and break not counted)")
	 .arg(maxBeginningsSpinBox->value());
  	s+="\n";
			
	currentConstraintTextEdit->setText(s);
}

void ModifyConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm::ok()
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
			tr("Invalid weight (percentage) - it must be 100%"));
		return;
	}

	QString students_name=studentsComboBox->currentText();
	StudentsSet* s=gt.rules.searchStudentsSet(students_name);
	if(s==NULL){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid students set"));
		return;
	}

	/*bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;*/

	this->_ctr->students=students_name;
	this->_ctr->weightPercentage=weight;
	//this->_ctr->compulsory=compulsory;
	
	this->_ctr->maxBeginningsAtSecondHour=maxBeginningsSpinBox->value();
	
	gt.rules.internalStructureComputed=false;
	
	this->close();
}

void ModifyConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm::cancel()
{
	this->close();
}
