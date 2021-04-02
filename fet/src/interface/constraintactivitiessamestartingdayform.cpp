/***************************************************************************
                          constraintactivitiessamestartingdayform.cpp  -  description
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

#include <QMessageBox>

#include "longtextmessagebox.h"

#include "constraintactivitiessamestartingdayform.h"
#include "addconstraintactivitiessamestartingdayform.h"
#include "modifyconstraintactivitiessamestartingdayform.h"

ConstraintActivitiesSameStartingDayForm::ConstraintActivitiesSameStartingDayForm()
{
    setupUi(this);

    connect(addConstraintPushButton, SIGNAL(clicked()), this /*ConstraintActivitiesSameStartingDayForm_template*/, SLOT(addConstraint()));
    connect(removeConstraintPushButton, SIGNAL(clicked()), this /*ConstraintActivitiesSameStartingDayForm_template*/, SLOT(removeConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this /*ConstraintActivitiesSameStartingDayForm_template*/, SLOT(close()));
    connect(constraintsListBox, SIGNAL(highlighted(int)), this /*ConstraintActivitiesSameStartingDayForm_template*/, SLOT(constraintChanged(int)));
    connect(modifyConstraintPushButton, SIGNAL(clicked()), this /*ConstraintActivitiesSameStartingDayForm_template*/, SLOT(modifyConstraint()));
    connect(constraintsListBox, SIGNAL(selected(QString)), this /*ConstraintActivitiesSameStartingDayForm_template*/, SLOT(modifyConstraint()));
    connect(helpPushButton, SIGNAL(clicked()), this /*ConstraintActivitiesSameStartingDayForm_template*/, SLOT(help()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	this->refreshConstraintsListBox();
}

ConstraintActivitiesSameStartingDayForm::~ConstraintActivitiesSameStartingDayForm()
{
}

void ConstraintActivitiesSameStartingDayForm::refreshConstraintsListBox()
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

bool ConstraintActivitiesSameStartingDayForm::filterOk(TimeConstraint* ctr)
{
	if(ctr->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY)
		return true;
	else
		return false;
}

void ConstraintActivitiesSameStartingDayForm::constraintChanged(int index)
{
	if(index<0){
		currentConstraintTextEdit->setText("");
		return;
	}
	QString s;
	assert(index<this->visibleConstraintsList.size());
	TimeConstraint* ctr=this->visibleConstraintsList.at(index);
	assert(ctr!=NULL);
	s=ctr->getDetailedDescription(gt.rules);
	currentConstraintTextEdit->setText(s);
}

void ConstraintActivitiesSameStartingDayForm::addConstraint()
{
	AddConstraintActivitiesSameStartingDayForm form;
	form.exec();

	this->refreshConstraintsListBox();
	
	constraintsListBox->setCurrentItem(constraintsListBox->count()-1);
}

void ConstraintActivitiesSameStartingDayForm::modifyConstraint()
{
	int i=constraintsListBox->currentItem();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintActivitiesSameStartingDayForm form((ConstraintActivitiesSameStartingDay*)ctr);
	form.exec();

	this->refreshConstraintsListBox();
	
	constraintsListBox->setCurrentItem(i);
}

void ConstraintActivitiesSameStartingDayForm::removeConstraint()
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

void ConstraintActivitiesSameStartingDayForm::help()
{
	QString s;

	s+=tr("IMPORTANT: after adding such constraints, it is necessary (otherwise generation might be impossible) to remove redundant constraints"
	" min days between activities. If you are sure that you don't have redundant constraints, you can skip this step, but it doesn't hurt to do it as a precaution."
	" Also, you don't have to do that after each added constraint, but only once after adding more constraints of this type."
	" Please read Help/Important tips - tip number 2 for details");

	LongTextMessageBox::largeInformation(this, tr("FET help"), s);
}
