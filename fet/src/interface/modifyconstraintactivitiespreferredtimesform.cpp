
/***************************************************************************
                          modifyconstraintactivitiespreferredtimesform.cpp  -  description
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

#include "modifyconstraintactivitiespreferredtimesform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <q3table.h>

#include <QDesktopWidget>

#define YES	(QObject::tr("Yes"))
#define NO	(QObject::tr("No"))

ModifyConstraintActivitiesPreferredTimesForm::ModifyConstraintActivitiesPreferredTimesForm(ConstraintActivitiesPreferredTimes* ctr)
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	this->_ctr=ctr;

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

	bool currentMatrix[MAX_HOURS_PER_DAY][MAX_DAYS_PER_WEEK];
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			currentMatrix[i][j]=false;
	for(int k=0; k<ctr->nPreferredTimes; k++){
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

ModifyConstraintActivitiesPreferredTimesForm::~ModifyConstraintActivitiesPreferredTimesForm()
{
}

void ModifyConstraintActivitiesPreferredTimesForm::tableClicked(int row, int col, int button, const QPoint& mousePos)
{
	if(&button!=NULL && &mousePos!=NULL)
		; //to avoid "unused parameter" compiler warning

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

void ModifyConstraintActivitiesPreferredTimesForm::updateTeachersComboBox(){
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

void ModifyConstraintActivitiesPreferredTimesForm::updateStudentsComboBox(){
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

void ModifyConstraintActivitiesPreferredTimesForm::updateSubjectsComboBox(){
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

void ModifyConstraintActivitiesPreferredTimesForm::updateSubjectTagsComboBox(){
	int i=0, j=-1;
	subjectTagsComboBox->clear();
	subjectTagsComboBox->insertItem("");
	if(this->_ctr->subjectTagName=="")
		j=i;
	i++;
	for(int k=0; k<gt.rules.subjectTagsList.size(); k++){
		SubjectTag* s=gt.rules.subjectTagsList[k];
		subjectTagsComboBox->insertItem(s->name);
		if(s->name==this->_ctr->subjectTagName)
			j=i;
		i++;
	}
	assert(j>=0);
	subjectTagsComboBox->setCurrentItem(j);
}

void ModifyConstraintActivitiesPreferredTimesForm::ok()
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
	this->_ctr->nPreferredTimes=n;
	for(int i=0; i<n; i++){
		this->_ctr->days[i]=days[i];
		this->_ctr->hours[i]=hours[i];
	}

	gt.rules.internalStructureComputed=false;
	
	this->close();
}

void ModifyConstraintActivitiesPreferredTimesForm::cancel()
{
	this->close();
}

#undef YES
#undef NO
