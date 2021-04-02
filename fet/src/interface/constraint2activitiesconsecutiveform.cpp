/***************************************************************************
                          constraint2activitiesconsecutiveform.cpp  -  description
                             -------------------
    begin                : Aug 21, 2007
    copyright            : (C) 2007 by Lalescu Liviu
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

#include "constraint2activitiesconsecutiveform.h"
#include "addconstraint2activitiesconsecutiveform.h"
#include "modifyconstraint2activitiesconsecutiveform.h"

#include <QDesktopWidget>

#include <QMessageBox>

#include <q3listbox.h>
#include <q3combobox.h>
#include <q3groupbox.h>

Constraint2ActivitiesConsecutiveForm::Constraint2ActivitiesConsecutiveForm()
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	this->filterChanged();
}

Constraint2ActivitiesConsecutiveForm::~Constraint2ActivitiesConsecutiveForm()
{
}

bool Constraint2ActivitiesConsecutiveForm::filterOk(TimeConstraint* ctr)
{
	if(ctr->type==CONSTRAINT_2_ACTIVITIES_CONSECUTIVE)
		return true;
	else
		return false;
}

void Constraint2ActivitiesConsecutiveForm::filterChanged()
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

void Constraint2ActivitiesConsecutiveForm::constraintChanged(int index)
{
	if(index<0)
		return;
	assert(index<this->visibleConstraintsList.size());
	TimeConstraint* ctr=this->visibleConstraintsList.at(index);
	assert(ctr!=NULL);
	currentConstraintTextEdit->setText(ctr->getDetailedDescription(gt.rules));
}

void Constraint2ActivitiesConsecutiveForm::addConstraint()
{
	AddConstraint2ActivitiesConsecutiveForm *form=new AddConstraint2ActivitiesConsecutiveForm();
	form->exec();

	filterChanged();
	
	constraintsListBox->setCurrentItem(constraintsListBox->count()-1);
}

void Constraint2ActivitiesConsecutiveForm::modifyConstraint()
{
	int i=constraintsListBox->currentItem();
	if(i<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraint2ActivitiesConsecutiveForm *form = new ModifyConstraint2ActivitiesConsecutiveForm((Constraint2ActivitiesConsecutive*)ctr);
	form->exec();

	filterChanged();
	constraintsListBox->setCurrentItem(i);
}

void Constraint2ActivitiesConsecutiveForm::removeConstraint()
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
