/***************************************************************************
                          constraintactivitiessamestartingtimeform.cpp  -  description
                             -------------------
    begin                : 23 June 2004
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

#include "longtextmessagebox.h"

#include "constraintactivitiessamestartingtimeform.h"
#include "addconstraintactivitiessamestartingtimeform.h"
#include "modifyconstraintactivitiessamestartingtimeform.h"

#include <QDesktopWidget>

ConstraintActivitiesSameStartingTimeForm::ConstraintActivitiesSameStartingTimeForm()
{
    setupUi(this);

    connect(addConstraintPushButton, SIGNAL(clicked()), this /*ConstraintActivitiesSameStartingTimeForm_template*/, SLOT(addConstraint()));
    connect(removeConstraintPushButton, SIGNAL(clicked()), this /*ConstraintActivitiesSameStartingTimeForm_template*/, SLOT(removeConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this /*ConstraintActivitiesSameStartingTimeForm_template*/, SLOT(close()));
    connect(constraintsListBox, SIGNAL(highlighted(int)), this /*ConstraintActivitiesSameStartingTimeForm_template*/, SLOT(constraintChanged(int)));
    connect(modifyConstraintPushButton, SIGNAL(clicked()), this /*ConstraintActivitiesSameStartingTimeForm_template*/, SLOT(modifyConstraint()));
    connect(constraintsListBox, SIGNAL(selected(QString)), this /*ConstraintActivitiesSameStartingTimeForm_template*/, SLOT(modifyConstraint()));
    connect(teachersComboBox, SIGNAL(activated(QString)), this /*ConstraintActivitiesSameStartingTimeForm_template*/, SLOT(filterChanged()));
    connect(studentsComboBox, SIGNAL(activated(QString)), this /*ConstraintActivitiesSameStartingTimeForm_template*/, SLOT(filterChanged()));
    connect(subjectsComboBox, SIGNAL(activated(QString)), this /*ConstraintActivitiesSameStartingTimeForm_template*/, SLOT(filterChanged()));
    connect(activityTagsComboBox, SIGNAL(activated(QString)), this /*ConstraintActivitiesSameStartingTimeForm_template*/, SLOT(filterChanged()));
    connect(helpPushButton, SIGNAL(clicked()), this /*ConstraintActivitiesSameStartingTimeForm_template*/, SLOT(help()));


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
	
	//this->refreshConstraintsListBox();

/////////////
	teachersComboBox->insertItem("");
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		teachersComboBox->insertItem(tch->name);
	}
	teachersComboBox->setCurrentItem(0);

	subjectsComboBox->insertItem("");
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* sb=gt.rules.subjectsList[i];
		subjectsComboBox->insertItem(sb->name);
	}
	subjectsComboBox->setCurrentItem(0);

	activityTagsComboBox->insertItem("");
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* st=gt.rules.activityTagsList[i];
		activityTagsComboBox->insertItem(st->name);
	}
	activityTagsComboBox->setCurrentItem(0);

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
	studentsComboBox->setCurrentItem(0);
///////////////

	this->filterChanged();
}

ConstraintActivitiesSameStartingTimeForm::~ConstraintActivitiesSameStartingTimeForm()
{
}

void ConstraintActivitiesSameStartingTimeForm::refreshConstraintsListBox()
{
	this->visibleConstraintsList.clear();
	constraintsListBox->clear();
	for(int i=0; i<gt.rules.timeConstraintsList.size(); i++){
		TimeConstraint* ctr=gt.rules.timeConstraintsList[i];
		if(filterOk(ctr)){
			QString s;
			s=ctr->getDescription(gt.rules);
			visibleConstraintsList.append(ctr);
			constraintsListBox->insertItem(s);
		}
	}

	constraintsListBox->setCurrentItem(0);
	this->constraintChanged(constraintsListBox->currentItem());
}

/*bool ConstraintActivitiesSameStartingTimeForm::filterOk(TimeConstraint* ctr)
{
	if(ctr->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME)
		return true;
	else
		return false;
}*/

bool ConstraintActivitiesSameStartingTimeForm::filterOk(TimeConstraint* ctr)
{
	if(ctr->type!=CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME)
		return false;
		
	ConstraintActivitiesSameStartingTime* c=(ConstraintActivitiesSameStartingTime*) ctr;
	
	QString tn=teachersComboBox->currentText();
	QString sbn=subjectsComboBox->currentText();
	QString sbtn=activityTagsComboBox->currentText();
	QString stn=studentsComboBox->currentText();
	
	if(tn=="" && sbn=="" && sbtn=="" && stn=="")
		return true;
	
	bool foundTeacher=false, foundStudents=false, foundSubject=false, foundActivityTag=false;
		
	for(int i=0; i<c->n_activities; i++){
		//bool found=true;
	
		int id=c->activitiesId[i];
		Activity* act=NULL;
		foreach(Activity* a, gt.rules.activitiesList)
			if(a->id==id)
				act=a;
		
		if(act!=NULL){
			//teacher
			if(tn!=""){
				bool ok2=false;
				for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++)
					if(*it == tn){
						ok2=true;
						break;
					}
				if(ok2)
					foundTeacher=true;
				//if(!ok2)
				//	found=false;
			}
			else
				foundTeacher=true;

			//subject
			if(sbn!="" && sbn!=act->subjectName)
				;
			else
				//found=false;
				foundSubject=true;
		
			//activity tag
//			if(sbtn!="" && sbtn!=act->activityTagName)
			if(sbtn!="" && !act->activityTagsNames.contains(sbtn))
				;
			else
				//found=false;
				foundActivityTag=true;
		
			//students
			if(stn!=""){
				bool ok2=false;
				for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
					if(*it == stn){
						ok2=true;
						break;
				}
				//if(!ok2)
				//	found=false;
				if(ok2)
					foundStudents=true;
			}
			else
				foundStudents=true;
		}
		
		//if(found)
		//	return true;
	}
	
	if(foundTeacher && foundStudents && foundSubject && foundActivityTag)
		return true;
	else
		return false;
}

void ConstraintActivitiesSameStartingTimeForm::filterChanged()
{
	this->visibleConstraintsList.clear();
	constraintsListBox->clear();
	for(int i=0; i<gt.rules.timeConstraintsList.size(); i++){
		TimeConstraint* ctr=gt.rules.timeConstraintsList[i];
		if(filterOk(ctr)){
			visibleConstraintsList.append(ctr);
			constraintsListBox->insertItem(ctr->getDescription(gt.rules));
		}
	}
	if(visibleConstraintsList.count()>0)
		constraintChanged(0);
	else
		constraintChanged(-1);
}

void ConstraintActivitiesSameStartingTimeForm::constraintChanged(int index)
{
	if(index<0)
		return;
	QString s;
	assert(index<this->visibleConstraintsList.size());
	TimeConstraint* ctr=this->visibleConstraintsList.at(index);
	assert(ctr!=NULL);
	s=ctr->getDetailedDescription(gt.rules);
	currentConstraintTextEdit->setText(s);
}

void ConstraintActivitiesSameStartingTimeForm::addConstraint()
{
	AddConstraintActivitiesSameStartingTimeForm form;
	form.exec();

	this->refreshConstraintsListBox();
	
	constraintsListBox->setCurrentItem(constraintsListBox->count()-1);
}

void ConstraintActivitiesSameStartingTimeForm::modifyConstraint()
{
	int i=constraintsListBox->currentItem();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintActivitiesSameStartingTimeForm form((ConstraintActivitiesSameStartingTime*)ctr);
	form.exec();

	this->refreshConstraintsListBox();
	
	constraintsListBox->setCurrentItem(i);
}

void ConstraintActivitiesSameStartingTimeForm::removeConstraint()
{
	int i=constraintsListBox->currentItem();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);
	QString s;
	s=tr("Remove constraint?");
	s+="\n\n";
	s+=ctr->getDetailedDescription(gt.rules);
	//s+=tr("\nAre you sure?");

	switch( LongTextMessageBox::confirmation( this, tr("FET confirmation"),
		s, tr("Yes"), tr("No"), 0, 0, 1 ) ){
	case 0: // The user clicked the OK again button or pressed Enter
		gt.rules.removeTimeConstraint(ctr);
		this->refreshConstraintsListBox();
		break;
	case 1: // The user clicked the Cancel or pressed Escape
		break;
	}

	if((uint)(i) >= constraintsListBox->count())
		i=constraintsListBox->count()-1;
	constraintsListBox->setCurrentItem(i);
}

void ConstraintActivitiesSameStartingTimeForm::help()
{
	QString s;

	s+=tr("IMPORTANT: after adding such constraints, it is necessary (otherwise generation might be impossible) to remove redundant constraints"
	" min days between activities. If you are sure that you don't have redundant constraints, you can skip this step, but it doesn't hurt to do it as a precaution."
	" Also, you don't have to do that after each added constraint, but only once after adding more constraints of this type."
	" Please read Help/Important tips - tip number 2 for details");

	LongTextMessageBox::largeInformation(this, tr("FET help"), s);
}
