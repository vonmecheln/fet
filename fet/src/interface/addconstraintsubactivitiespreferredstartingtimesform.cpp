/***************************************************************************
                          addconstraintsubactivitiespreferredstartingtimesform.cpp  -  description
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

#include "longtextmessagebox.h"

#include "addconstraintsubactivitiespreferredstartingtimesform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <q3table.h>

#include <QDesktopWidget>

#define YES	(QObject::tr("Allowed", "Please keep translation short"))
#define NO	(QObject::tr("Not allowed", "Please keep translation short"))

AddConstraintSubactivitiesPreferredStartingTimesForm::AddConstraintSubactivitiesPreferredStartingTimesForm()
{
	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	QSize tmp3=subjectsComboBox->minimumSizeHint();
	Q_UNUSED(tmp3);
	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	
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

	connect(preferredTimesTable->horizontalHeader(), SIGNAL(clicked(int)), this, SLOT(horizontalHeaderClicked(int)));
	connect(preferredTimesTable->verticalHeader(), SIGNAL(clicked(int)), this, SLOT(verticalHeaderClicked(int)));

	preferredTimesTable->setSelectionMode(Q3Table::NoSelection);
			
	componentNumberSpinBox->setMinValue(1);
	componentNumberSpinBox->setMaxValue(99);
	componentNumberSpinBox->setValue(1);
}

AddConstraintSubactivitiesPreferredStartingTimesForm::~AddConstraintSubactivitiesPreferredStartingTimesForm()
{
}

void AddConstraintSubactivitiesPreferredStartingTimesForm::horizontalHeaderClicked(int col)
{
	if(col>=0 && col<gt.rules.nDaysPerWeek){
		QString s=preferredTimesTable->text(0, col);
		if(s==YES)
			s=NO;
		else{
			assert(s==NO);
			s=YES;
		}

		for(int row=0; row<gt.rules.nHoursPerDay; row++){
			/*QString s=notAllowedTimesTable->text(row, col);
			if(s==YES)
				s=NO;
			else{
				assert(s==NO);
				s=YES;
			}*/
			preferredTimesTable->setText(row, col, s);
		}
	}
}

void AddConstraintSubactivitiesPreferredStartingTimesForm::verticalHeaderClicked(int row)
{
	if(row>=0 && row<gt.rules.nHoursPerDay){
		QString s=preferredTimesTable->text(row, 0);
		if(s==YES)
			s=NO;
		else{
			assert(s==NO);
			s=YES;
		}
	
		for(int col=0; col<gt.rules.nDaysPerWeek; col++){
			/*QString s=notAllowedTimesTable->text(row, col);
			if(s==YES)
				s=NO;
			else{
				assert(s==NO);
				s=YES;
			}*/
			preferredTimesTable->setText(row, col, s);
		}
	}
}

void AddConstraintSubactivitiesPreferredStartingTimesForm::setAllSlotsAllowed()
{
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			preferredTimesTable->setText(i, j, YES);
}

void AddConstraintSubactivitiesPreferredStartingTimesForm::setAllSlotsNotAllowed()
{
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			preferredTimesTable->setText(i, j, NO);
}

void AddConstraintSubactivitiesPreferredStartingTimesForm::tableClicked(int row, int col, int button, const QPoint& mousePos)
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

void AddConstraintSubactivitiesPreferredStartingTimesForm::updateTeachersComboBox(){
	teachersComboBox->clear();
	teachersComboBox->insertItem("");
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* t=gt.rules.teachersList[i];
		teachersComboBox->insertItem(t->name);
	}
}

void AddConstraintSubactivitiesPreferredStartingTimesForm::updateStudentsComboBox(){
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

void AddConstraintSubactivitiesPreferredStartingTimesForm::updateSubjectsComboBox(){
	subjectsComboBox->clear();
	subjectsComboBox->insertItem("");
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* s=gt.rules.subjectsList[i];
		subjectsComboBox->insertItem(s->name);
	}
}

void AddConstraintSubactivitiesPreferredStartingTimesForm::updateActivityTagsComboBox(){
	activityTagsComboBox->clear();
	activityTagsComboBox->insertItem("");
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* s=gt.rules.activityTagsList[i];
		activityTagsComboBox->insertItem(s->name);
	}
}

void AddConstraintSubactivitiesPreferredStartingTimesForm::addConstraint()
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
		
	/*
	if(teacher=="" && students=="" && subject=="" && activityTag==""){
		int t=QMessageBox::question(this, tr("FET question"),
		 tr("You specified all the sub-activities with split index %1 (no teacher, students set, subject or activity tag specified)"
		 ". Gaps in teachers' and students' timetable will be counted if using this constraint. Are you sure?")
		  +"\n\n"+tr("Do you want to add current constraint?"),
		 QMessageBox::Yes, QMessageBox::Cancel);
						 
		if(t==QMessageBox::Cancel)
				return;
	}

	if(teacher!="" && students=="" && subject=="" && activityTag==""){
		int t=QMessageBox::question(this, tr("FET question"),
		 tr("You specified only the teacher. Gaps in this teacher's timetable will be counted if using this constraint. Are you sure?")
		  +"\n\n"+tr("Do you want to add current constraint?"),
		 QMessageBox::Yes, QMessageBox::Cancel);
						 
		if(t==QMessageBox::Cancel)
				return;
	}
	if(teacher=="" && students!="" && subject=="" && activityTag==""){
		int t=QMessageBox::question(this, tr("FET question"),
		 tr("You specified only the students set. Gaps in this students set's timetable will be counted if using this constraint. Are you sure?")
		  +"\n\n"+tr("Do you want to add current constraint?"),
		 QMessageBox::Yes, QMessageBox::Cancel);
						 
		if(t==QMessageBox::Cancel)
				return;
	}*/

	int days[MAX_N_CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES];
	int hours[MAX_N_CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES];
	int n=0;
	for(int j=0; j<gt.rules.nDaysPerWeek; j++)
		for(int i=0; i<gt.rules.nHoursPerDay; i++)
			if(preferredTimesTable->text(i, j)==YES){
				if(n>=MAX_N_CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES){
					QString s=QObject::tr("Not enough slots (too many \"Yes\" values).");
					s+="\n";
					s+=QObject::tr("Please increase the variable MAX_N_CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES");
					s+="\n";
					s+=QObject::tr("Currently, it is %1").arg(MAX_N_CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES);
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

	ctr=new ConstraintSubactivitiesPreferredStartingTimes(weight, componentNumberSpinBox->value(),/*compulsory,*/ teacher, students, subject, activityTag, n, days, hours);

	bool tmp2=gt.rules.addTimeConstraint(ctr);
	if(tmp2){
		QString s=QObject::tr("Constraint added:");
		s+="\n\n";
		s+=ctr->getDetailedDescription(gt.rules);
		LongTextMessageBox::information(this, QObject::tr("FET information"), s);
	}
	else{
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Constraint NOT added - duplicate?"));
		delete ctr;
	}
}

#undef YES
#undef NO
