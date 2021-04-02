/***************************************************************************
                          constraintmaxdaysbetweenactivitiesform.cpp  -  description
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

#include "longtextmessagebox.h"

#include "constraintmaxdaysbetweenactivitiesform.h"
#include "addconstraintmaxdaysbetweenactivitiesform.h"
#include "modifyconstraintmaxdaysbetweenactivitiesform.h"

#include <QDesktopWidget>

#include <QInputDialog>

ConstraintMaxDaysBetweenActivitiesForm::ConstraintMaxDaysBetweenActivitiesForm()
{
    setupUi(this);

    connect(constraintsListBox, SIGNAL(highlighted(int)), this /*ConstraintMaxDaysBetweenActivitiesForm_template*/, SLOT(constraintChanged(int)));
    connect(addConstraintPushButton, SIGNAL(clicked()), this /*ConstraintMaxDaysBetweenActivitiesForm_template*/, SLOT(addConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this /*ConstraintMaxDaysBetweenActivitiesForm_template*/, SLOT(close()));
    connect(removeConstraintPushButton, SIGNAL(clicked()), this /*ConstraintMaxDaysBetweenActivitiesForm_template*/, SLOT(removeConstraint()));
    connect(modifyConstraintPushButton, SIGNAL(clicked()), this /*ConstraintMaxDaysBetweenActivitiesForm_template*/, SLOT(modifyConstraint()));
    connect(constraintsListBox, SIGNAL(selected(QString)), this /*ConstraintMaxDaysBetweenActivitiesForm_template*/, SLOT(modifyConstraint()));
    connect(teachersComboBox, SIGNAL(activated(QString)), this /*ConstraintMaxDaysBetweenActivitiesForm_template*/, SLOT(filterChanged()));
    connect(studentsComboBox, SIGNAL(activated(QString)), this /*ConstraintMaxDaysBetweenActivitiesForm_template*/, SLOT(filterChanged()));
    connect(subjectsComboBox, SIGNAL(activated(QString)), this /*ConstraintMaxDaysBetweenActivitiesForm_template*/, SLOT(filterChanged()));
    connect(activityTagsComboBox, SIGNAL(activated(QString)), this /*ConstraintMaxDaysBetweenActivitiesForm_template*/, SLOT(filterChanged()));
    connect(helpPushButton, SIGNAL(clicked()), this /*ConstraintMaxDaysBetweenActivitiesForm_template*/, SLOT(help()));


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

ConstraintMaxDaysBetweenActivitiesForm::~ConstraintMaxDaysBetweenActivitiesForm()
{
}

bool ConstraintMaxDaysBetweenActivitiesForm::filterOk(TimeConstraint* ctr)
{
	if(ctr->type!=CONSTRAINT_MAX_DAYS_BETWEEN_ACTIVITIES)
		return false;
		
	ConstraintMaxDaysBetweenActivities* c=(ConstraintMaxDaysBetweenActivities*) ctr;
	
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

void ConstraintMaxDaysBetweenActivitiesForm::filterChanged()
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

void ConstraintMaxDaysBetweenActivitiesForm::constraintChanged(int index)
{
	if(index<0){
		currentConstraintTextEdit->setText(tr("Invalid constraint"));
	
		return;
	}
	assert(index<this->visibleConstraintsList.size());
	TimeConstraint* ctr=this->visibleConstraintsList.at(index);
	assert(ctr!=NULL);
	currentConstraintTextEdit->setText(ctr->getDetailedDescription(gt.rules));
}

void ConstraintMaxDaysBetweenActivitiesForm::addConstraint()
{
	AddConstraintMaxDaysBetweenActivitiesForm form;
	form.exec();

	filterChanged();
	
	constraintsListBox->setCurrentItem(constraintsListBox->count()-1);
}

void ConstraintMaxDaysBetweenActivitiesForm::modifyConstraint()
{
	int i=constraintsListBox->currentItem();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintMaxDaysBetweenActivitiesForm form((ConstraintMaxDaysBetweenActivities*)ctr);
	form.exec();

	filterChanged();
	constraintsListBox->setCurrentItem(i);
}

void ConstraintMaxDaysBetweenActivitiesForm::removeConstraint()
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
		filterChanged();
		break;
	case 1: // The user clicked the Cancel or pressed Escape
		break;
	}
	
	if((uint)(i) >= constraintsListBox->count())
		i=constraintsListBox->count()-1;
	constraintsListBox->setCurrentItem(i);
}

void ConstraintMaxDaysBetweenActivitiesForm::help()
{
	QString s;
	
	s+=tr("Help about the constraint max days between activities:");
	s+="\n\n";
	s+=tr("This constraint was suggested for the following situation: a user needed that activities A1, A2 and A3 to be in follower days"
	 " (like: A1 on Tuesday, A2 on Wednesday and A3 on Thursday. So, they must be in 3 consecutive days). This is simple: add a constraint"
	 " max days between activities for A1, A2 and A3, with max 2 days between them. It is supposed that these activities are constrained"
	 " not to be in the same day by a constraint min days between activities.");
	s+="\n\n";
	s+=tr("So, the general situation: this constraint ensures that between each pair from the selected activities, the distance in days is at most the selected value."
	" Distance = 1 day between a pair A1 and A2 means that A1 and A2 are in consecutive days (like Thursday and Friday)."
	" Distance = 3 days means that A1 and A2 are 3 days apart, for instance Monday and Thursday.");
	s+="\n\n";
	s+=tr("Another example: teacher T wants to ensure that his activities take place in at most 4 consecutive days (so, from Monday to Thursday or from"
	" Tuesday to Friday). Then, add all his activities and max days between them = 3.");

	LongTextMessageBox::largeInformation(this, tr("FET help"), s);
}
