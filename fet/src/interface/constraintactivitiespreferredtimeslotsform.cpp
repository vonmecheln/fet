/* **************************************************************************
                          constraintactivitiespreferredtimeslotsform.cpp  -  description
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

#include "longtextmessagebox.h"

#include "constraintactivitiespreferredtimeslotsform.h"
#include "addconstraintactivitiespreferredtimeslotsform.h"
#include "modifyconstraintactivitiespreferredtimeslotsform.h"

#include <QDesktopWidget>

ConstraintActivitiesPreferredTimeSlotsForm::ConstraintActivitiesPreferredTimeSlotsForm()
{
    setupUi(this);

    connect(constraintsListBox, SIGNAL(highlighted(int)), this /*ConstraintActivitiesPreferredTimeSlotsForm_template*/, SLOT(constraintChanged(int)));
    connect(addConstraintPushButton, SIGNAL(clicked()), this /*ConstraintActivitiesPreferredTimeSlotsForm_template*/, SLOT(addConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this /*ConstraintActivitiesPreferredTimeSlotsForm_template*/, SLOT(close()));
    connect(removeConstraintPushButton, SIGNAL(clicked()), this /*ConstraintActivitiesPreferredTimeSlotsForm_template*/, SLOT(removeConstraint()));
    connect(modifyConstraintPushButton, SIGNAL(clicked()), this /*ConstraintActivitiesPreferredTimeSlotsForm_template*/, SLOT(modifyConstraint()));
    connect(constraintsListBox, SIGNAL(selected(QString)), this /*ConstraintActivitiesPreferredTimeSlotsForm_template*/, SLOT(modifyConstraint()));

	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	this->refreshConstraintsListBox();
}

ConstraintActivitiesPreferredTimeSlotsForm::~ConstraintActivitiesPreferredTimeSlotsForm()
{
}

void ConstraintActivitiesPreferredTimeSlotsForm::refreshConstraintsListBox()
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

bool ConstraintActivitiesPreferredTimeSlotsForm::filterOk(TimeConstraint* ctr)
{
	if(ctr->type==CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS)
		return true;
	else
		return false;
}

void ConstraintActivitiesPreferredTimeSlotsForm::constraintChanged(int index)
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

void ConstraintActivitiesPreferredTimeSlotsForm::addConstraint()
{
	AddConstraintActivitiesPreferredTimeSlotsForm form;
	form.exec();

	this->refreshConstraintsListBox();

	constraintsListBox->setCurrentItem(constraintsListBox->count()-1);
}

void ConstraintActivitiesPreferredTimeSlotsForm::modifyConstraint()
{
	int i=constraintsListBox->currentItem();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintActivitiesPreferredTimeSlotsForm form((ConstraintActivitiesPreferredTimeSlots*)ctr);
	form.exec();

	this->refreshConstraintsListBox();
	
	constraintsListBox->setCurrentItem(i);
}

void ConstraintActivitiesPreferredTimeSlotsForm::removeConstraint()
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
