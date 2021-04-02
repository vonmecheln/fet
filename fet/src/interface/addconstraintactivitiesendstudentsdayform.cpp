/***************************************************************************
                          addconstraintactivitiesendstudentsdayform.cpp  -  description
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

#include <QMessageBox>

#include <cstdio>

#include "longtextmessagebox.h"

#include "addconstraintactivitiesendstudentsdayform.h"
#include "timeconstraint.h"

AddConstraintActivitiesEndStudentsDayForm::AddConstraintActivitiesEndStudentsDayForm()
{
    setupUi(this);

    connect(addConstraintPushButton, SIGNAL(clicked()), this, SLOT(addConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));

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
}

AddConstraintActivitiesEndStudentsDayForm::~AddConstraintActivitiesEndStudentsDayForm()
{
}

void AddConstraintActivitiesEndStudentsDayForm::updateTeachersComboBox(){
	teachersComboBox->clear();
	teachersComboBox->insertItem("");
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* t=gt.rules.teachersList[i];
		teachersComboBox->insertItem(t->name);
	}
}

void AddConstraintActivitiesEndStudentsDayForm::updateStudentsComboBox(){
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

void AddConstraintActivitiesEndStudentsDayForm::updateSubjectsComboBox(){
	subjectsComboBox->clear();
	subjectsComboBox->insertItem("");
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* s=gt.rules.subjectsList[i];
		subjectsComboBox->insertItem(s->name);
	}
}

void AddConstraintActivitiesEndStudentsDayForm::updateActivityTagsComboBox(){
	activityTagsComboBox->clear();
	activityTagsComboBox->insertItem("");
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* s=gt.rules.activityTagsList[i];
		activityTagsComboBox->insertItem(s->name);
	}
}

void AddConstraintActivitiesEndStudentsDayForm::addConstraint()
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
			tr("Invalid weight (percentage) - must be 100%"));
		return;
	}

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
		
	ctr=new ConstraintActivitiesEndStudentsDay(weight, teacher, students, subject, activityTag);

	bool tmp2=gt.rules.addTimeConstraint(ctr);
	if(tmp2){
		QString s=tr("Constraint added:");
		s+="\n\n";
		s+=ctr->getDetailedDescription(gt.rules);
		LongTextMessageBox::information(this, tr("FET information"), s);
	}
	else{
		QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - please report error"));
		delete ctr;
	}
}

