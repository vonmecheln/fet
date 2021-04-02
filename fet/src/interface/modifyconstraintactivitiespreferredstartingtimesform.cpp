/***************************************************************************
                          modifyconstraintactivitiespreferredstartingtimesform.cpp  -  description
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

#include "modifyconstraintactivitiespreferredstartingtimesform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <q3table.h>

#include <QDesktopWidget>

#define YES	(QObject::tr("Allowed", "Please keep translation short"))
#define NO	(QObject::tr("Not allowed", "Please keep translation short"))

ModifyConstraintActivitiesPreferredStartingTimesForm::ModifyConstraintActivitiesPreferredStartingTimesForm(ConstraintActivitiesPreferredStartingTimes* ctr)
{
	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	this->_ctr=ctr;

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

	bool currentMatrix[MAX_HOURS_PER_DAY][MAX_DAYS_PER_WEEK];
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			currentMatrix[i][j]=false;
	for(int k=0; k<ctr->nPreferredStartingTimes; k++){
		if(ctr->hours[k]==-1 || ctr->days[k]==-1)
			assert(0);
		int i=ctr->hours[k];
		int j=ctr->days[k];
		currentMatrix[i][j]=true;
	}

	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			if(!currentMatrix[i][j])
				preferredTimesTable->setText(i, j, NO);
			else
				preferredTimesTable->setText(i, j, YES);
				
	//compulsoryCheckBox->setChecked(ctr->compulsory);
	weightLineEdit->setText(QString::number(ctr->weightPercentage));
}

ModifyConstraintActivitiesPreferredStartingTimesForm::~ModifyConstraintActivitiesPreferredStartingTimesForm()
{
}

void ModifyConstraintActivitiesPreferredStartingTimesForm::setAllSlotsAllowed()
{
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			preferredTimesTable->setText(i, j, YES);
}

void ModifyConstraintActivitiesPreferredStartingTimesForm::setAllSlotsNotAllowed()
{
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			preferredTimesTable->setText(i, j, NO);
}

void ModifyConstraintActivitiesPreferredStartingTimesForm::tableClicked(int row, int col, int button, const QPoint& mousePos)
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

void ModifyConstraintActivitiesPreferredStartingTimesForm::updateTeachersComboBox(){
	int i=0, j=-1;
	teachersComboBox->clear();
	teachersComboBox->insertItem("");
	if(this->_ctr->teacherName=="")
		j=i;
	i++;
	for(int k=0; k<gt.rules.teachersList.size(); k++){
		Teacher* t=gt.rules.teachersList[k];
		teachersComboBox->insertItem(t->name);
		if(t->name==this->_ctr->teacherName)
			j=i;
		i++;
	}
	assert(j>=0);
	teachersComboBox->setCurrentItem(j);
}

void ModifyConstraintActivitiesPreferredStartingTimesForm::updateStudentsComboBox(){
	int i=0, j=-1;
	studentsComboBox->clear();
	studentsComboBox->insertItem("");
	if(this->_ctr->studentsName=="")
		j=i;
	i++;
	for(int m=0; m<gt.rules.yearsList.size(); m++){
		StudentsYear* sty=gt.rules.yearsList[m];
		studentsComboBox->insertItem(sty->name);
		if(sty->name==this->_ctr->studentsName)
			j=i;
		i++;
		for(int n=0; n<sty->groupsList.size(); n++){
			StudentsGroup* stg=sty->groupsList[n];
			studentsComboBox->insertItem(stg->name);
			if(stg->name==this->_ctr->studentsName)
				j=i;
			i++;
			for(int p=0; p<stg->subgroupsList.size(); p++){
				StudentsSubgroup* sts=stg->subgroupsList[p];
				studentsComboBox->insertItem(sts->name);
				if(sts->name==this->_ctr->studentsName)
					j=i;
				i++;
			}
		}
	}
	assert(j>=0);
	studentsComboBox->setCurrentItem(j);
}

void ModifyConstraintActivitiesPreferredStartingTimesForm::updateSubjectsComboBox(){
	int i=0, j=-1;
	subjectsComboBox->clear();
	subjectsComboBox->insertItem("");
	if(this->_ctr->subjectName=="")
		j=i;
	i++;
	for(int k=0; k<gt.rules.subjectsList.size(); k++){
		Subject* s=gt.rules.subjectsList[k];
		subjectsComboBox->insertItem(s->name);
		if(s->name==this->_ctr->subjectName)
			j=i;
		i++;
	}
	assert(j>=0);
	subjectsComboBox->setCurrentItem(j);
}

void ModifyConstraintActivitiesPreferredStartingTimesForm::updateActivityTagsComboBox(){
	int i=0, j=-1;
	activityTagsComboBox->clear();
	activityTagsComboBox->insertItem("");
	if(this->_ctr->activityTagName=="")
		j=i;
	i++;
	for(int k=0; k<gt.rules.activityTagsList.size(); k++){
		ActivityTag* s=gt.rules.activityTagsList[k];
		activityTagsComboBox->insertItem(s->name);
		if(s->name==this->_ctr->activityTagName)
			j=i;
		i++;
	}
	assert(j>=0);
	activityTagsComboBox->setCurrentItem(j);
}

void ModifyConstraintActivitiesPreferredStartingTimesForm::ok()
{
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
					QMessageBox::critical(this, QObject::tr("FET information"), s);
					
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

	this->_ctr->weightPercentage=weight;
	//this->_ctr->compulsory=compulsory;
	this->_ctr->teacherName=teacher;
	this->_ctr->studentsName=students;
	this->_ctr->subjectName=subject;
	this->_ctr->activityTagName=activityTag;
	this->_ctr->nPreferredStartingTimes=n;
	for(int i=0; i<n; i++){
		this->_ctr->days[i]=days[i];
		this->_ctr->hours[i]=hours[i];
	}

	gt.rules.internalStructureComputed=false;
	
	this->close();
}

void ModifyConstraintActivitiesPreferredStartingTimesForm::cancel()
{
	this->close();
}

#undef YES
#undef NO
