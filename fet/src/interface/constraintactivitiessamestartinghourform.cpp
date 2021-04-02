/***************************************************************************
                          constraintactivitiessamestartinghourform.cpp  -  description
                             -------------------
    begin                : 15 Feb 2005
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

#include "longtextmessagebox.h"

#include "constraintactivitiessamestartinghourform.h"
#include "addconstraintactivitiessamestartinghourform.h"
#include "modifyconstraintactivitiessamestartinghourform.h"

#include <QDesktopWidget>

ConstraintActivitiesSameStartingHourForm::ConstraintActivitiesSameStartingHourForm()
{
    setupUi(this);

    connect(addConstraintPushButton, SIGNAL(clicked()), this /*ConstraintActivitiesSameStartingHourForm_template*/, SLOT(addConstraint()));
    connect(removeConstraintPushButton, SIGNAL(clicked()), this /*ConstraintActivitiesSameStartingHourForm_template*/, SLOT(removeConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this /*ConstraintActivitiesSameStartingHourForm_template*/, SLOT(close()));
    connect(constraintsListBox, SIGNAL(highlighted(int)), this /*ConstraintActivitiesSameStartingHourForm_template*/, SLOT(constraintChanged(int)));
    connect(modifyConstraintPushButton, SIGNAL(clicked()), this /*ConstraintActivitiesSameStartingHourForm_template*/, SLOT(modifyConstraint()));
    connect(constraintsListBox, SIGNAL(selected(QString)), this /*ConstraintActivitiesSameStartingHourForm_template*/, SLOT(modifyConstraint()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	this->refreshConstraintsListBox();
}

ConstraintActivitiesSameStartingHourForm::~ConstraintActivitiesSameStartingHourForm()
{
}

void ConstraintActivitiesSameStartingHourForm::refreshConstraintsListBox()
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

bool ConstraintActivitiesSameStartingHourForm::filterOk(TimeConstraint* ctr)
{
	if(ctr->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR)
		return true;
	else
		return false;
}

void ConstraintActivitiesSameStartingHourForm::constraintChanged(int index)
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

void ConstraintActivitiesSameStartingHourForm::addConstraint()
{
	AddConstraintActivitiesSameStartingHourForm form;
	form.exec();

	this->refreshConstraintsListBox();
	
	constraintsListBox->setCurrentItem(constraintsListBox->count()-1);
}

void ConstraintActivitiesSameStartingHourForm::modifyConstraint()
{
	int i=constraintsListBox->currentItem();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintActivitiesSameStartingHourForm form((ConstraintActivitiesSameStartingHour*)ctr);
	form.exec();

	this->refreshConstraintsListBox();
	
	constraintsListBox->setCurrentItem(i);
}

void ConstraintActivitiesSameStartingHourForm::removeConstraint()
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
