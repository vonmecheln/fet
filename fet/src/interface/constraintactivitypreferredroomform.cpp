/***************************************************************************
                          constraintactivitypreferredroomform.cpp  -  description
                             -------------------
    begin                : Feb 13, 2005
    copyright            : (C) 2005 by Lalescu Liviu
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

#include "longtextmessagebox.h"

#include "constraintactivitypreferredroomform.h"
#include "addconstraintactivitypreferredroomform.h"
#include "modifyconstraintactivitypreferredroomform.h"

#include "lockunlock.h"

ConstraintActivityPreferredRoomForm::ConstraintActivityPreferredRoomForm()
{
    setupUi(this);

    connect(constraintsListBox, SIGNAL(highlighted(int)), this /*ConstraintActivityPreferredRoomForm_template*/, SLOT(constraintChanged(int)));
    connect(addConstraintPushButton, SIGNAL(clicked()), this /*ConstraintActivityPreferredRoomForm_template*/, SLOT(addConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this /*ConstraintActivityPreferredRoomForm_template*/, SLOT(close()));
    connect(removeConstraintPushButton, SIGNAL(clicked()), this /*ConstraintActivityPreferredRoomForm_template*/, SLOT(removeConstraint()));
    connect(modifyConstraintPushButton, SIGNAL(clicked()), this /*ConstraintActivityPreferredRoomForm_template*/, SLOT(modifyConstraint()));
    connect(roomsComboBox, SIGNAL(activated(QString)), this /*ConstraintActivityPreferredRoomForm_template*/, SLOT(filterChanged()));
    connect(constraintsListBox, SIGNAL(selected(QString)), this /*ConstraintActivityPreferredRoomForm_template*/, SLOT(modifyConstraint()));
    connect(teachersComboBox, SIGNAL(activated(QString)), this /*ConstraintActivityPreferredRoomForm_template*/, SLOT(filterChanged()));
    connect(studentsComboBox, SIGNAL(activated(QString)), this /*ConstraintActivityPreferredRoomForm_template*/, SLOT(filterChanged()));
    connect(subjectsComboBox, SIGNAL(activated(QString)), this /*ConstraintActivityPreferredRoomForm_template*/, SLOT(filterChanged()));
    connect(activityTagsComboBox, SIGNAL(activated(QString)), this /*ConstraintActivityPreferredRoomForm_template*/, SLOT(filterChanged()));


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
	
	QSize tmp5=roomsComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);

	roomsComboBox->insertItem("");
	for(int i=0; i<gt.rules.roomsList.size(); i++){
		Room* rm=gt.rules.roomsList[i];
		roomsComboBox->insertItem(rm->name);
	}

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

ConstraintActivityPreferredRoomForm::~ConstraintActivityPreferredRoomForm()
{
}

bool ConstraintActivityPreferredRoomForm::filterOk(SpaceConstraint* ctr)
{
	if(ctr->type!=CONSTRAINT_ACTIVITY_PREFERRED_ROOM)
		return false;
		
	ConstraintActivityPreferredRoom* c=(ConstraintActivityPreferredRoom*) ctr;
	
	QString tn=teachersComboBox->currentText();
	QString sbn=subjectsComboBox->currentText();
	QString sbtn=activityTagsComboBox->currentText();
	QString stn=studentsComboBox->currentText();
		
	bool found=true;
	
	int id=c->activityId;
	Activity* act=NULL;
	foreach(Activity* a, gt.rules.activitiesList)
		if(a->id==id)
			act=a;

	found=true;		
		
	if(act!=NULL){
		//teacher
		if(tn!=""){
			bool ok2=false;
			for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++)
				if(*it == tn){
					ok2=true;
					break;
				}
			if(!ok2)
				found=false;
		}

		//subject
		if(sbn!="" && sbn!=act->subjectName)
			found=false;
	
		//activity tag
//		if(sbtn!="" && sbtn!=act->activityTagName)
		if(sbtn!="" && !act->activityTagsNames.contains(sbtn))
			found=false;
	
		//students
		if(stn!=""){
			bool ok2=false;
			for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
				if(*it == stn){
					ok2=true;
					break;
			}
			if(!ok2)
				found=false;
		}
	}
	
	if(!found)
		return false;
	
	return c->roomName==roomsComboBox->currentText() || roomsComboBox->currentText()=="";
}

void ConstraintActivityPreferredRoomForm::filterChanged()
{
	this->visibleConstraintsList.clear();
	constraintsListBox->clear();
	for(int i=0; i<gt.rules.spaceConstraintsList.size(); i++){
		SpaceConstraint* ctr=gt.rules.spaceConstraintsList[i];
		if(filterOk(ctr)){
			visibleConstraintsList.append(ctr);
			constraintsListBox->insertItem(ctr->getDescription(gt.rules));
		}
	}
	
	constraintsListBox->setCurrentItem(0);
	this->constraintChanged(constraintsListBox->currentItem());

	/*if(visibleConstraintsList.count()>0)
		constraintChanged(0);
	else
		constraintChanged(-1);*/
}

void ConstraintActivityPreferredRoomForm::constraintChanged(int index)
{
	if(index<0){
		currentConstraintTextEdit->setText("");
		//currentConstraintTextEdit->setText(tr("Invalid constraint"));
		return;
	}
	assert(index<this->visibleConstraintsList.size());
	SpaceConstraint* ctr=this->visibleConstraintsList.at(index);
	assert(ctr!=NULL);
	currentConstraintTextEdit->setText(ctr->getDetailedDescription(gt.rules));
}

void ConstraintActivityPreferredRoomForm::addConstraint()
{
	AddConstraintActivityPreferredRoomForm form;
	form.exec();

	filterChanged();
	
	constraintsListBox->setCurrentItem(constraintsListBox->count()-1);
}

void ConstraintActivityPreferredRoomForm::modifyConstraint()
{
	int i=constraintsListBox->currentItem();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	SpaceConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintActivityPreferredRoomForm form((ConstraintActivityPreferredRoom*)ctr);
	form.exec();

	filterChanged();
	constraintsListBox->setCurrentItem(i);
}

void ConstraintActivityPreferredRoomForm::removeConstraint()
{
	int i=constraintsListBox->currentItem();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	SpaceConstraint* ctr=this->visibleConstraintsList.at(i);
	QString s;
	s=tr("Remove constraint?");
	s+="\n\n";
	s+=ctr->getDetailedDescription(gt.rules);
	//s+=tr("\nAre you sure?");

	switch( LongTextMessageBox::confirmation( this, tr("FET confirmation"),
		s, tr("Yes"), tr("No"), 0, 0, 1 ) ){
	case 0: // The user clicked the OK again button or pressed Enter
		gt.rules.removeSpaceConstraint(ctr);
		
		LockUnlock::computeLockedUnlockedActivitiesOnlySpace();
		LockUnlock::increaseCommunicationSpinBox();
		
		filterChanged();
		break;
	case 1: // The user clicked the Cancel or pressed Escape
		break;
	}
	
	if((uint)(i) >= constraintsListBox->count())
		i=constraintsListBox->count()-1;
	constraintsListBox->setCurrentItem(i);
}
