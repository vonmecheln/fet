/***************************************************************************
                          addconstraintactivitiespreferredstartingtimesform.cpp  -  description
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

#include "addconstraintactivitiespreferredstartingtimesform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <q3table.h>

#include <QDesktopWidget>

#define YES	(QObject::tr("Allowed", "Please keep translation short"))
#define NO	(QObject::tr("Not allowed", "Please keep translation short"))

AddConstraintActivitiesPreferredStartingTimesForm::AddConstraintActivitiesPreferredStartingTimesForm()
{
	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	updateTeachersComboBox();
	updateStudentsComboBox();
	updateSubjectsComboBox();
	updateActivityTagsComboBox();

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
			preferredTimesTable->setText(i, j, YES);
}

AddConstraintActivitiesPreferredStartingTimesForm::~AddConstraintActivitiesPreferredStartingTimesForm()
{
}

void AddConstraintActivitiesPreferredStartingTimesForm::setAllSlotsAllowed()
{
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			preferredTimesTable->setText(i, j, YES);
}

void AddConstraintActivitiesPreferredStartingTimesForm::setAllSlotsNotAllowed()
{
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			preferredTimesTable->setText(i, j, NO);
}

void AddConstraintActivitiesPreferredStartingTimesForm::tableClicked(int row, int col, int button, const QPoint& mousePos)
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

void AddConstraintActivitiesPreferredStartingTimesForm::updateTeachersComboBox(){
	teachersComboBox->clear();
	teachersComboBox->insertItem("");
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* t=gt.rules.teachersList[i];
		teachersComboBox->insertItem(t->name);
	}
}

void AddConstraintActivitiesPreferredStartingTimesForm::updateStudentsComboBox(){
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

void AddConstraintActivitiesPreferredStartingTimesForm::updateSubjectsComboBox(){
	subjectsComboBox->clear();
	subjectsComboBox->insertItem("");
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* s=gt.rules.subjectsList[i];
		subjectsComboBox->insertItem(s->name);
	}
}

void AddConstraintActivitiesPreferredStartingTimesForm::updateActivityTagsComboBox(){
	activityTagsComboBox->clear();
	activityTagsComboBox->insertItem("");
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* s=gt.rules.activityTagsList[i];
		activityTagsComboBox->insertItem(s->name);
	}
}

void AddConstraintActivitiesPreferredStartingTimesForm::addConstraint()
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
		
	QString activityTag=activityTagsComboBox->currentText();
	if(activityTag!="")
		assert(gt.rules.searchActivityTag(activityTag)>=0);
		
	if(teacher=="" && students=="" && subject=="" && activityTag==""){
		int t=QMessageBox::question(this, tr("FET question"),
		 tr("You specified all the activities. This might be a small problem: if you specify"
		  " a not allowed slot between two allowed slots, this not allowed slot will"
		  " be counted as a gap in the teachers' and students' timetable.\n\n"
		  " The best practice would be to use constraint break times.\n\n"
		  " If you need weight under 100%, then you can use this constraint, but be careful"
		  " not to obtain an impossible timetable (if your teachers/students are constrained on gaps"
		  " or early gaps and if you leave a not allowed slot between 2 allowed slots or"
		  " a not allowed slot early in the day and more allowed slots after it,"
		  " this possible gap might be counted in teachers' and students' timetable)")
		  +"\n\n"+tr("Do you want to add current constraint?"),
		 QMessageBox::Yes, QMessageBox::Cancel);
						 
		if(t==QMessageBox::Cancel)
				return;
	}

	if(teacher!="" && students=="" && subject=="" && activityTag==""){
		int t=QMessageBox::question(this, tr("FET question"),
		 tr("You specified only the teacher. This might be a small problem: if you specify"
		  " a not allowed slot between two allowed slots, this not allowed slot will"
		  " be counted as a gap in the teacher's timetable.\n\n"
		  " The best practice would be to use constraint teacher not available times.\n\n"
		  " If you need weight under 100%, then you can use this constraint, but be careful"
		  " not to obtain an impossible timetable (if your teacher is constrained on gaps"
		  " and if you leave a not allowed slot between 2 allowed slots, this possible"
		  " gap might be counted in teacher's timetable)")
		  +"\n\n"+tr("Do you want to add current constraint?"),
		 QMessageBox::Yes, QMessageBox::Cancel);
						 
		if(t==QMessageBox::Cancel)
				return;
	}
	if(teacher=="" && students!="" && subject=="" && activityTag==""){
		int t=QMessageBox::question(this, tr("FET question"),
		 tr("You specified only the students set. This might be a small problem: if you specify"
		  " a not allowed slot between two allowed slots (or a not allowed slot before allowed slots),"
		  " this not allowed slot will"
		  " be counted as a gap (or early gap) in the students' timetable.\n\n"
		  " The best practice would be to use constraint students set not available times.\n\n"
		  " If you need weight under 100%, then you can use this constraint, but be careful"
		  " not to obtain an impossible timetable (if your students set is constrained on gaps or early gaps"
		  " and if you leave a not allowed slot between 2 allowed slots (or a not allowed slot before allowed slots), this possible"
		  " gap might be counted in students' timetable)")
		  +"\n\n"+tr("Do you want to add current constraint?"),
		 QMessageBox::Yes, QMessageBox::Cancel);
						 
		if(t==QMessageBox::Cancel)
				return;
	}

	int days[MAX_N_CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES];
	int hours[MAX_N_CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES];
	int n=0;
	for(int j=0; j<gt.rules.nDaysPerWeek; j++)
		for(int i=0; i<gt.rules.nHoursPerDay; i++)
			if(preferredTimesTable->text(i, j)==YES){
				if(n>=MAX_N_CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES){
					QString s=QObject::tr("Not enough slots (too many \"Yes\" values).");
					s+="\n";
					s+=QObject::tr("Please increase the variable MAX_N_CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES");
					s+="\n";
					s+=QObject::tr("Currently, it is %1").arg(MAX_N_CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES);
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

	ctr=new ConstraintActivitiesPreferredStartingTimes(weight, /*compulsory,*/ teacher, students, subject, activityTag, n, days, hours);

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