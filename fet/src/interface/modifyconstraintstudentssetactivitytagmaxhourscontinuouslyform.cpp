/***************************************************************************
                          modifyconstraintstudentssetactivitytagmaxhourscontinuouslyform.cpp  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Lalescu Liviu
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

#include "modifyconstraintstudentssetactivitytagmaxhourscontinuouslyform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

ModifyConstraintStudentsSetActivityTagMaxHoursContinuouslyForm::ModifyConstraintStudentsSetActivityTagMaxHoursContinuouslyForm(ConstraintStudentsSetActivityTagMaxHoursContinuously* ctr)
{
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
	
	updateStudentsComboBox();
	updateActivityTagsComboBox();

	maxHoursSpinBox->setMinValue(1);
	maxHoursSpinBox->setMaxValue(gt.rules.nHoursPerDay);
	maxHoursSpinBox->setValue(ctr->maxHoursContinuously);

	constraintChanged();
}

ModifyConstraintStudentsSetActivityTagMaxHoursContinuouslyForm::~ModifyConstraintStudentsSetActivityTagMaxHoursContinuouslyForm()
{
}

void ModifyConstraintStudentsSetActivityTagMaxHoursContinuouslyForm::updateStudentsComboBox(){
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

void ModifyConstraintStudentsSetActivityTagMaxHoursContinuouslyForm::updateActivityTagsComboBox()
{
	activityTagsComboBox->clear();
	int j=-1;
	for(int i=0; i<gt.rules.activityTagsList.count(); i++){
		ActivityTag* at=gt.rules.activityTagsList.at(i);
		activityTagsComboBox->insertItem(at->name);
		if(at->name==this->_ctr->activityTagName)
			j=i;
	}
	assert(j>=0);
	activityTagsComboBox->setCurrentItem(j);

	constraintChanged();
}

void ModifyConstraintStudentsSetActivityTagMaxHoursContinuouslyForm::constraintChanged()
{
	QString s;
	s+=QObject::tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=QObject::tr("Weight (percentage)=%1").arg(weight);
	s+="\n";

	s+=QObject::tr("Students set activity tag max hours continuously");
	s+="\n";
	s+=QObject::tr("Students set=%1").arg(studentsComboBox->currentText());
	s+="\n";
	s+=QObject::tr("Activity tag=%1").arg(activityTagsComboBox->currentText());
	s+="\n";

	s+=QObject::tr("Max hours:%1").arg(maxHoursSpinBox->value());
	s+="\n";

	currentConstraintTextEdit->setText(s);
}

void ModifyConstraintStudentsSetActivityTagMaxHoursContinuouslyForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, QObject::tr("FET warning"),
			QObject::tr("Invalid weight (percentage)"));
		return;
	}

	QString students_name=studentsComboBox->currentText();
	StudentsSet* s=gt.rules.searchStudentsSet(students_name);
	if(s==NULL){
		QMessageBox::warning(this, QObject::tr("FET warning"),
			QObject::tr("Invalid students set"));
		return;
	}
	
	QString activityTagName=activityTagsComboBox->currentText();
	int ati=gt.rules.searchActivityTag(activityTagName);
	if(ati<0){
		QMessageBox::warning(this, QObject::tr("FET warning"), QObject::tr("Invalid activity tag"));
		return;
	}

	this->_ctr->weightPercentage=weight;
	this->_ctr->students=students_name;
	this->_ctr->activityTagName=activityTagName;
	this->_ctr->maxHoursContinuously=maxHoursSpinBox->value();

	gt.rules.internalStructureComputed=false;
	
	this->close();
}

void ModifyConstraintStudentsSetActivityTagMaxHoursContinuouslyForm::cancel()
{
	this->close();
}
