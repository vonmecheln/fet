/***************************************************************************
                          constraintminndaysbetweenactivitiesform.cpp  -  description
                             -------------------
    begin                : Feb 11, 2005
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

#include "constraintminndaysbetweenactivitiesform.h"
#include "addconstraintminndaysbetweenactivitiesform.h"
#include "modifyconstraintminndaysbetweenactivitiesform.h"

#include <QDesktopWidget>

#include <QInputDialog>

ConstraintMinNDaysBetweenActivitiesForm::ConstraintMinNDaysBetweenActivitiesForm()
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	this->filterChanged();
}

ConstraintMinNDaysBetweenActivitiesForm::~ConstraintMinNDaysBetweenActivitiesForm()
{
}

bool ConstraintMinNDaysBetweenActivitiesForm::filterOk(TimeConstraint* ctr)
{
	if(ctr->type==CONSTRAINT_MIN_N_DAYS_BETWEEN_ACTIVITIES)
		return true;
	else
		return false;
}

void ConstraintMinNDaysBetweenActivitiesForm::filterChanged()
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

void ConstraintMinNDaysBetweenActivitiesForm::constraintChanged(int index)
{
	if(index<0)
		return;
	assert(index<this->visibleConstraintsList.size());
	TimeConstraint* ctr=this->visibleConstraintsList.at(index);
	assert(ctr!=NULL);
	currentConstraintTextEdit->setText(ctr->getDetailedDescription(gt.rules));
}

void ConstraintMinNDaysBetweenActivitiesForm::addConstraint()
{
	AddConstraintMinNDaysBetweenActivitiesForm *form=new AddConstraintMinNDaysBetweenActivitiesForm();
	form->exec();

	filterChanged();
	
	constraintsListBox->setCurrentItem(constraintsListBox->count()-1);
}

void ConstraintMinNDaysBetweenActivitiesForm::modifyConstraint()
{
	int i=constraintsListBox->currentItem();
	if(i<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintMinNDaysBetweenActivitiesForm *form
	 = new ModifyConstraintMinNDaysBetweenActivitiesForm((ConstraintMinNDaysBetweenActivities*)ctr);
	form->exec();

	filterChanged();
	constraintsListBox->setCurrentItem(i);
}

void ConstraintMinNDaysBetweenActivitiesForm::removeConstraint()
{
	int i=constraintsListBox->currentItem();
	if(i<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);
	QString s;
	s=QObject::tr("Removing constraint:\n");
	s+=ctr->getDetailedDescription(gt.rules);
	s+=QObject::tr("\nAre you sure?");

	switch( QMessageBox::warning( this, QObject::tr("FET warning"),
		s, QObject::tr("OK"), QObject::tr("Cancel"), 0, 0, 1 ) ){
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

void ConstraintMinNDaysBetweenActivitiesForm::changeAllWeights()
{
	bool ok = FALSE;
	QString s;
	s = QInputDialog::getText( QObject::tr("Modifying all weights for min n days"), QObject::tr("Warning: all min n days weights will be\n"
	 " changed to selected value. Are you sure?\n If yes, please enter weight for all constraints of\n"
	 " type min n days between activities") ,
     QLineEdit::Normal, QString::null, &ok, this );

	if ( ok && !s.isEmpty() ){
		double weight;
		sscanf(s, "%lf", &weight);
		if(weight<0.0 || weight>100.0){
			QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid weight (percentage)"));
			return;
		}

		foreach(TimeConstraint* tc, gt.rules.timeConstraintsList)
			if(tc->type==CONSTRAINT_MIN_N_DAYS_BETWEEN_ACTIVITIES)
				tc->weightPercentage=weight;

		gt.rules.internalStructureComputed=false;

		this->filterChanged();
	}
	else
		return;// user entered nothing or pressed Cancel
}
