/***************************************************************************
                          addconstraintstudentssetminhoursdailyform.cpp  -  description
                             -------------------
    begin                : July 19, 2007
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

#include <QMessageBox>

#include <cstdio>

#include "longtextmessagebox.h"

#include "addconstraintstudentssetminhoursdailyform.h"
#include "timeconstraint.h"


AddConstraintStudentsSetMinHoursDailyForm::AddConstraintStudentsSetMinHoursDailyForm()
{
    setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this /*AddConstraintStudentsSetMinHoursDailyForm_template*/, SLOT(constraintChanged()));
    connect(addConstraintPushButton, SIGNAL(clicked()), this /*AddConstraintStudentsSetMinHoursDailyForm_template*/, SLOT(addCurrentConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this /*AddConstraintStudentsSetMinHoursDailyForm_template*/, SLOT(close()));
//    connect(studentsComboBox, SIGNAL(activated(QString)), this /*AddConstraintStudentsSetMinHoursDailyForm_template*/, SLOT(constraintChanged()));
//    connect(minHoursSpinBox, SIGNAL(valueChanged(int)), this /*AddConstraintStudentsSetMinHoursDailyForm_template*/, SLOT(constraintChanged()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	if(ENABLE_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS)
		allowLabel->setText(tr("Advanced usage: enabled"));
	else
		allowLabel->setText(tr("Advanced usage: not enabled"));
	
	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);

	minHoursSpinBox->setMinValue(1);
	minHoursSpinBox->setMaxValue(gt.rules.nHoursPerDay);
	minHoursSpinBox->setValue(1);

	updateStudentsSetComboBox();
}

AddConstraintStudentsSetMinHoursDailyForm::~AddConstraintStudentsSetMinHoursDailyForm()
{
}

void AddConstraintStudentsSetMinHoursDailyForm::updateStudentsSetComboBox()
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

void AddConstraintStudentsSetMinHoursDailyForm::constraintChanged()
{/*
	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=tr("Weight (percentage)=%1").arg(weight);
	s+="\n";

	s+=tr("Students set min hours daily");
	s+="\n";
	s+=tr("Students set=%1").arg(studentsComboBox->currentText());
	s+="\n";

	int minHours=minHoursSpinBox->value();
	if(minHours>=0){
		s+=tr("Min. hours:%1").arg(minHours);
		s+="\n";
	}

	currentConstraintTextEdit->setText(s);*/
}

void AddConstraintStudentsSetMinHoursDailyForm::addCurrentConstraint()
{
	TimeConstraint *ctr=NULL;

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight"));
		return;
	}
	if(weight!=100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight - it has to be 100%"));
		return;
	}

	if(!ENABLE_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS && allowEmptyDaysCheckBox->isChecked()){
		QMessageBox::warning(this, tr("FET warning"), tr("Empty days for students min hours daily constraints are not enabled. You must enable them from the Settings->Advanced menu."));
		return;
	}

	if(allowEmptyDaysCheckBox->isChecked() && minHoursSpinBox->value()<2){
		QMessageBox::warning(this, tr("FET warning"), tr("If you allow empty days, the min hours must be at least 2 (to make it a non-trivial constraint)"));
		return;
	}

	/*bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;*/

	int minHours=minHoursSpinBox->value();

	QString students_name=studentsComboBox->currentText();
	StudentsSet* s=gt.rules.searchStudentsSet(students_name);
	if(s==NULL){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid students set"));
		return;
	}

	ctr=new ConstraintStudentsSetMinHoursDaily(weight, /*compulsory,*/ minHours, students_name, allowEmptyDaysCheckBox->isChecked());

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

void AddConstraintStudentsSetMinHoursDailyForm::on_allowEmptyDaysCheckBox_toggled()
{
	bool k=allowEmptyDaysCheckBox->isChecked();
		
	if(k && !ENABLE_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS){
		allowEmptyDaysCheckBox->setChecked(false);
		QString s=tr("Advanced usage is not enabled. To be able to select 'Allow empty days' for the constraints of type min hours daily for students, you must enable the option from the Settings->Advanced menu.",
			"'Allow empty days' is an option which the user can enable and then he can select it.");
		s+="\n\n";
		s+=tr("Explanation: only select this option if your institution allows empty days for students and a timetable is possible with empty days for students."
			" Otherwise, it is IMPERATIVE (for performance reasons) to not select this option (or FET may not be able to find a timetable).");
		s+="\n\n";
		s+=tr("Use with caution.");
		QMessageBox::information(this, tr("FET information"), s);
	}
}
