/***************************************************************************
                          constraintsubactivitiespreferredstartingtimesform.cpp  -  description
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

#include "longtextmessagebox.h"

#include "constraintsubactivitiespreferredstartingtimesform.h"
#include "addconstraintsubactivitiespreferredstartingtimesform.h"
#include "modifyconstraintsubactivitiespreferredstartingtimesform.h"

ConstraintSubactivitiesPreferredStartingTimesForm::ConstraintSubactivitiesPreferredStartingTimesForm()
{
    setupUi(this);

    connect(constraintsListBox, SIGNAL(highlighted(int)), this /*ConstraintSubactivitiesPreferredStartingTimesForm_template*/, SLOT(constraintChanged(int)));
    connect(addConstraintPushButton, SIGNAL(clicked()), this /*ConstraintSubactivitiesPreferredStartingTimesForm_template*/, SLOT(addConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this /*ConstraintSubactivitiesPreferredStartingTimesForm_template*/, SLOT(close()));
    connect(removeConstraintPushButton, SIGNAL(clicked()), this /*ConstraintSubactivitiesPreferredStartingTimesForm_template*/, SLOT(removeConstraint()));
    connect(modifyConstraintPushButton, SIGNAL(clicked()), this /*ConstraintSubactivitiesPreferredStartingTimesForm_template*/, SLOT(modifyConstraint()));
    connect(constraintsListBox, SIGNAL(selected(QString)), this /*ConstraintSubactivitiesPreferredStartingTimesForm_template*/, SLOT(modifyConstraint()));
    connect(helpPushButton, SIGNAL(clicked()), this /*ConstraintSubactivitiesPreferredStartingTimesForm_template*/, SLOT(help()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	this->refreshConstraintsListBox();
}

ConstraintSubactivitiesPreferredStartingTimesForm::~ConstraintSubactivitiesPreferredStartingTimesForm()
{
}

void ConstraintSubactivitiesPreferredStartingTimesForm::refreshConstraintsListBox()
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

bool ConstraintSubactivitiesPreferredStartingTimesForm::filterOk(TimeConstraint* ctr)
{
	if(ctr->type==CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES)
		return true;
	else
		return false;
}

void ConstraintSubactivitiesPreferredStartingTimesForm::constraintChanged(int index)
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

void ConstraintSubactivitiesPreferredStartingTimesForm::addConstraint()
{
	AddConstraintSubactivitiesPreferredStartingTimesForm form;
	form.exec();

	this->refreshConstraintsListBox();
	
	constraintsListBox->setCurrentItem(constraintsListBox->count()-1);
}

void ConstraintSubactivitiesPreferredStartingTimesForm::modifyConstraint()
{
	int i=constraintsListBox->currentItem();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintSubactivitiesPreferredStartingTimesForm form((ConstraintSubactivitiesPreferredStartingTimes*)ctr);
	form.exec();

	this->refreshConstraintsListBox();
	
	constraintsListBox->setCurrentItem(i);
}

void ConstraintSubactivitiesPreferredStartingTimesForm::removeConstraint()
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

void ConstraintSubactivitiesPreferredStartingTimesForm::help()
{
	LongTextMessageBox::largeInformation(this, tr("FET help"), tr(
	 "This constraint is used to specify that for some components of a type of"
	" activities, for instance for Mathematics activities, you need that from say 4-5"
	" per week, at least the first 2 to start early (say in the first 4 hours).")+
	+"\n\n"+
	tr("This is simple: just add 2 constraints of this type, with the component"
	" number 1, (then 2) and the corresponding subject (you can also"
	" define teacher, students set and activity tag, of course)."
	 )
	 +"\n\n"+
	 tr("Note: if you have Maths with 4 and 5 splits and want to constrain 2 subactivities for those with 4 per week and 3 subactivities"
	 " for those with 5 per week, you can add constraints only for Maths with components 3, 4 and 5 (nice trick)")
	 +"\n\n"+
	 tr("Note: if an activity does not belong to a larger group (is a single activity),"
	 " it is considered to have component number 1. As an advice, if you have activities of say Biology splitted into 1"
	 " or 2 per week and want to constrain only one component if there are 2 per week, add a constraint for Biology with component number=2")
	 +"\n\n"+
	 tr("Note: if a teacher or a students set cannot have lessons in some slots because of this constraint, gaps will be counted (if you have max gaps constraints)"
 	  ". So be careful if you add this constraint for all sub-activities, for only a teacher or for only a students set"));
}
