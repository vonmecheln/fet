/***************************************************************************
                          constraintteachersintervalmaxdaysperweekform.cpp  -  description
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

#include "constraintteachersintervalmaxdaysperweekform.h"
#include "addconstraintteachersintervalmaxdaysperweekform.h"
#include "modifyconstraintteachersintervalmaxdaysperweekform.h"

#include <QDesktopWidget>

ConstraintTeachersIntervalMaxDaysPerWeekForm::ConstraintTeachersIntervalMaxDaysPerWeekForm()
{
	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	this->filterChanged();
}

ConstraintTeachersIntervalMaxDaysPerWeekForm::~ConstraintTeachersIntervalMaxDaysPerWeekForm()
{
}

bool ConstraintTeachersIntervalMaxDaysPerWeekForm::filterOk(TimeConstraint* ctr)
{
	if(ctr->type==CONSTRAINT_TEACHERS_INTERVAL_MAX_DAYS_PER_WEEK){
		return true;
	}
	else{
		return false;
	}
}

void ConstraintTeachersIntervalMaxDaysPerWeekForm::filterChanged()
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
}

void ConstraintTeachersIntervalMaxDaysPerWeekForm::constraintChanged(int index)
{
	if(index<0)
		return;
	assert(index<this->visibleConstraintsList.size());
	TimeConstraint* ctr=this->visibleConstraintsList.at(index);
	assert(ctr!=NULL);
	currentConstraintTextEdit->setText(ctr->getDetailedDescription(gt.rules));
}

void ConstraintTeachersIntervalMaxDaysPerWeekForm::addConstraint()
{
	AddConstraintTeachersIntervalMaxDaysPerWeekForm *form=new AddConstraintTeachersIntervalMaxDaysPerWeekForm();
	form->exec();

	filterChanged();
	
	constraintsListBox->setCurrentItem(constraintsListBox->count()-1);
}

void ConstraintTeachersIntervalMaxDaysPerWeekForm::modifyConstraint()
{
	int i=constraintsListBox->currentItem();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintTeachersIntervalMaxDaysPerWeekForm *form
	 = new ModifyConstraintTeachersIntervalMaxDaysPerWeekForm((ConstraintTeachersIntervalMaxDaysPerWeek*)ctr);
	form->exec();

	filterChanged();
	constraintsListBox->setCurrentItem(i);
}

void ConstraintTeachersIntervalMaxDaysPerWeekForm::removeConstraint()
{
	int i=constraintsListBox->currentItem();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);
	QString s;
	s=tr("Removing constraint:\n");
	s+=ctr->getDetailedDescription(gt.rules);
	s+=tr("\nAre you sure?");

	switch( QMessageBox::warning( this, tr("FET warning"),
		s, tr("OK"), tr("Cancel"), 0, 0, 1 ) ){
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
