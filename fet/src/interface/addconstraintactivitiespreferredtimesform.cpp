/***************************************************************************
                          addconstraintactivitiespreferredtimesform.cpp  -  description
                             -------------------
    begin                : 15 May 2004
    copyright            : (C) 2004 by Lalescu Liviu
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

#include "addconstraintactivitiespreferredtimesform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <q3table.h>

#include <QDesktopWidget>

#define YES	(QObject::tr("Yes"))
#define NO	(QObject::tr("No"))

AddConstraintActivitiesPreferredTimesForm::AddConstraintActivitiesPreferredTimesForm()
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	updateTeachersComboBox();
	updateStudentsComboBox();
	updateSubjectsComboBox();
	updateSubjectTagsComboBox();

	preferredTimesTable->setNumRows(gt.rules.nHoursPerDay);
	preferredTimesTable->setNumCols(gt.rules.nDaysPerWeek);

	for(int j=0; j<gt.rules.nDaysPerWeek; j++)
		//preferredTimesTable->setText(0, j+1, gt.rules.daysOfTheWeek[j]);
		preferredTimesTable->horizontalHeader()->setLabel(j, gt.rules.daysOfTheWeek[j]);
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		//preferredTimesTable->setText(i+1, 0, gt.rules.hoursOfTheDay[i]);
		preferredTimesTable->verticalHeader()->setLabel(i, gt.rules.hoursOfTheDay[i]);

	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			preferredTimesTable->setText(i, j, NO);
}

AddConstraintActivitiesPreferredTimesForm::~AddConstraintActivitiesPreferredTimesForm()
{
}

void AddConstraintActivitiesPreferredTimesForm::tableClicked(int row, int col, int button, const QPoint& mousePos)
{
	Q_UNUSED(button);
	Q_UNUSED(mousePos);
	//if(&button!=NULL && &mousePos!=NULL)
	//	; //to avoid "unused parameter" compiler warning

	//row--; col--;
	if(row>=0 && row<gt.rules.nHoursPerDay && col>=0 && col<gt.rules.nDaysPerWeek){
		QString s=preferredTimesTable->text(row, col);
		if(s==YES)
			s=NO;
		else{
			assert(s==NO);
			s=YES;
		}
		preferredTimesTable->setText(row, col, s);
	}
}

void AddConstraintActivitiesPreferredTimesForm::updateTeachersComboBox(){
	teachersComboBox->clear();
	teachersComboBox->insertItem("");
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* t=gt.rules.teachersList[i];
		teachersComboBox->insertItem(t->name);
	}
}

void AddConstraintActivitiesPreferredTimesForm::updateStudentsComboBox(){
	studentsComboBox->clear();
	studentsComboBox->insertItem("");
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
}

void AddConstraintActivitiesPreferredTimesForm::updateSubjectsComboBox(){
	subjectsComboBox->clear();
	subjectsComboBox->insertItem("");
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* s=gt.rules.subjectsList[i];
		subjectsComboBox->insertItem(s->name);
	}
}

void AddConstraintActivitiesPreferredTimesForm::updateSubjectTagsComboBox(){
	subjectTagsComboBox->clear();
	subjectTagsComboBox->insertItem("");
	for(int i=0; i<gt.rules.subjectTagsList.size(); i++){
		SubjectTag* s=gt.rules.subjectTagsList[i];
		subjectTagsComboBox->insertItem(s->name);
	}
}

void AddConstraintActivitiesPreferredTimesForm::addConstraint()
{
	TimeConstraint *ctr=NULL;

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid weight (percentage)"));
		return;
	}

	/*bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;*/

	QString teacher=teachersComboBox->currentText();
	if(teacher!="")
		assert(gt.rules.searchTeacher(teacher)>=0);

	QString students=studentsComboBox->currentText();
	if(students!="")
		assert(gt.rules.searchStudentsSet(students)!=NULL);

	QString subject=subjectsComboBox->currentText();
	if(subject!="")
		assert(gt.rules.searchSubject(subject)>=0);
		
	QString subjectTag=subjectTagsComboBox->currentText();
	if(subjectTag!="")
		assert(gt.rules.searchSubjectTag(subjectTag)>=0);
		
	if(teacher=="" && students=="" && subject=="" && subjectTag==""){
		int t=QMessageBox::question(this, tr("FET question"),
		 tr("Are you sure you want to add this constraint for all activities?"
		 " (no teacher, students, subject or subject tag specified)"),
		 QMessageBox::Yes, QMessageBox::Cancel);
						 
		if(t==QMessageBox::Cancel)
				return;
	}

	int days[MAX_N_CONSTRAINT_ACTIVITIES_PREFERRED_TIMES];
	int hours[MAX_N_CONSTRAINT_ACTIVITIES_PREFERRED_TIMES];
	int n=0;
	for(int j=0; j<gt.rules.nDaysPerWeek; j++)
		for(int i=0; i<gt.rules.nHoursPerDay; i++)
			if(preferredTimesTable->text(i, j)==YES){
				if(n>=MAX_N_CONSTRAINT_ACTIVITIES_PREFERRED_TIMES){
					QString s=QObject::tr("Not enough slots (too many \"Yes\" values).");
					s+="\n";
					s+=QObject::tr("Please increase the variable MAX_N_CONSTRAINT_ACTIVITIES_PREFERRED_TIMES");
					s+="\n";
					s+=QObject::tr("Currently, it is %1").arg(MAX_N_CONSTRAINT_ACTIVITIES_PREFERRED_TIMES);
					QMessageBox::warning(this, QObject::tr("FET information"), s);
					
					return;
				}
				
				days[n]=j;
				hours[n]=i;
				n++;
			}

	if(n<=0){
		int t=QMessageBox::question(this, tr("FET question"),
		 tr("Warning: 0 slots selected. Are you sure?"),
		 QMessageBox::Yes, QMessageBox::Cancel);
						 
		if(t==QMessageBox::Cancel)
				return;
	}

	ctr=new ConstraintActivitiesPreferredTimes(weight, /*compulsory,*/ teacher, students, subject, subjectTag, n, days, hours);

	bool tmp2=gt.rules.addTimeConstraint(ctr);
	if(tmp2){
		QString s=QObject::tr("Constraint added:");
		s+="\n";
		s+=ctr->getDetailedDescription(gt.rules);
		QMessageBox::information(this, QObject::tr("FET information"), s);
	}
	else{
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Constraint NOT added - duplicate?"));
		delete ctr;
	}
}

#undef YES
#undef NO
