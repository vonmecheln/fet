/***************************************************************************
                          addconstraintstudentssetactivitytagmaxhourscontinuouslyform.cpp  -  description
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

#include "addconstraintstudentssetactivitytagmaxhourscontinuouslyform.h"
#include "timeconstraint.h"

#include <QDesktopWidget>

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

AddConstraintStudentsSetActivityTagMaxHoursContinuouslyForm::AddConstraintStudentsSetActivityTagMaxHoursContinuouslyForm()
{
	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	maxHoursSpinBox->setMinValue(1);
	maxHoursSpinBox->setMaxValue(gt.rules.nHoursPerDay);
	maxHoursSpinBox->setValue(gt.rules.nHoursPerDay);

	updateStudentsSetComboBox();
	
	updateActivityTagsComboBox();
}

AddConstraintStudentsSetActivityTagMaxHoursContinuouslyForm::~AddConstraintStudentsSetActivityTagMaxHoursContinuouslyForm()
{
}

void AddConstraintStudentsSetActivityTagMaxHoursContinuouslyForm::updateStudentsSetComboBox()
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

void AddConstraintStudentsSetActivityTagMaxHoursContinuouslyForm::updateActivityTagsComboBox()
{
	foreach(ActivityTag* at, gt.rules.activityTagsList)
		activityTagsComboBox->insertItem(at->name);

	constraintChanged();
}

void AddConstraintStudentsSetActivityTagMaxHoursContinuouslyForm::constraintChanged()
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

	int maxHours=maxHoursSpinBox->value();
	if(maxHours>=0){
		s+=QObject::tr("Max. hours:%1").arg(maxHours);
		s+="\n";
	}

	currentConstraintTextEdit->setText(s);
}

void AddConstraintStudentsSetActivityTagMaxHoursContinuouslyForm::addCurrentConstraint()
{
	TimeConstraint *ctr=NULL;

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid weight"));
		return;
	}

	int maxHours=maxHoursSpinBox->value();

	QString students_name=studentsComboBox->currentText();
	StudentsSet* s=gt.rules.searchStudentsSet(students_name);
	if(s==NULL){
		QMessageBox::warning(this, QObject::tr("FET warning"),
			QObject::tr("Invalid students set"));
		return;
	}
	
	QString activityTagName=activityTagsComboBox->currentText();
	int acttagindex=gt.rules.searchActivityTag(activityTagName);
	if(acttagindex<0){
		QMessageBox::warning(this, QObject::tr("FET warning"), QObject::tr("Invalid activity tag"));
		return;
	}

	ctr=new ConstraintStudentsSetActivityTagMaxHoursContinuously(weight, maxHours, students_name, activityTagName);

	bool tmp2=gt.rules.addTimeConstraint(ctr);
	if(tmp2)
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Constraint added"));
	else{
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Constraint NOT added - please report error"));
		delete ctr;
	}
}
