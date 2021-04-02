/***************************************************************************
                          addconstraintstudentssetearlymaxbeginningsatsecondhourform.cpp  -  description
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

#include "addconstraintstudentssetearlymaxbeginningsatsecondhourform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

//#define yesNo(x)	((x)==0?tr("no"):tr("yes"))

AddConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm::AddConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm()
{
	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
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

	maxBeginningsSpinBox->setMinValue(0);
	maxBeginningsSpinBox->setMaxValue(gt.rules.nDaysPerWeek);
	maxBeginningsSpinBox->setValue(0);
	
	constraintChanged();
}

AddConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm::~AddConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm()
{
}

void AddConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm::constraintChanged()
{
	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	s+=tr("Students set=%1").arg(studentsComboBox->currentText());
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=tr("Weight percentage=%1\%").arg(weight);
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

void AddConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm::addCurrentConstraint()
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

	ctr=new ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour(weight/*, compulsory*/, maxBeginningsSpinBox->value(), students_name);

	bool tmp2=gt.rules.addTimeConstraint(ctr);
	if(tmp2)
		QMessageBox::information(this, tr("FET information"),
			tr("Constraint added."));
	else{
		QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - please report error"));
		delete ctr;
	}
}
