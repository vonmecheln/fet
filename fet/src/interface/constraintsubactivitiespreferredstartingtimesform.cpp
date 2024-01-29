/***************************************************************************
                          constraintsubactivitiespreferredstartingtimesform.cpp  -  description
                             -------------------
    begin                : 2008
    copyright            : (C) 2008 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include <QMessageBox>

#include "longtextmessagebox.h"

#include "constraintsubactivitiespreferredstartingtimesform.h"
#include "addconstraintsubactivitiespreferredstartingtimesform.h"
#include "modifyconstraintsubactivitiespreferredstartingtimesform.h"

#include <QListWidget>
#include <QScrollBar>
#include <QAbstractItemView>

ConstraintSubactivitiesPreferredStartingTimesForm::ConstraintSubactivitiesPreferredStartingTimesForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	currentConstraintTextEdit->setReadOnly(true);
	
	modifyConstraintPushButton->setDefault(true);

	constraintsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(constraintsListWidget, &QListWidget::currentRowChanged, this, &ConstraintSubactivitiesPreferredStartingTimesForm::constraintChanged);
	connect(addConstraintPushButton, &QPushButton::clicked, this, &ConstraintSubactivitiesPreferredStartingTimesForm::addConstraint);
	connect(closePushButton, &QPushButton::clicked, this, &ConstraintSubactivitiesPreferredStartingTimesForm::close);
	connect(removeConstraintPushButton, &QPushButton::clicked, this, &ConstraintSubactivitiesPreferredStartingTimesForm::removeConstraint);
	connect(modifyConstraintPushButton, &QPushButton::clicked, this, &ConstraintSubactivitiesPreferredStartingTimesForm::modifyConstraint);
	connect(constraintsListWidget, &QListWidget::itemDoubleClicked, this, &ConstraintSubactivitiesPreferredStartingTimesForm::modifyConstraint);
	connect(helpPushButton, &QPushButton::clicked, this, &ConstraintSubactivitiesPreferredStartingTimesForm::help);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	this->filterChanged();
}

ConstraintSubactivitiesPreferredStartingTimesForm::~ConstraintSubactivitiesPreferredStartingTimesForm()
{
	saveFETDialogGeometry(this);
}

void ConstraintSubactivitiesPreferredStartingTimesForm::filterChanged()
{
	this->visibleConstraintsList.clear();
	constraintsListWidget->clear();
	for(int i=0; i<gt.rules.timeConstraintsList.size(); i++){
		TimeConstraint* ctr=gt.rules.timeConstraintsList[i];
		if(filterOk(ctr)){
			QString s;
			s=ctr->getDescription(gt.rules);
			visibleConstraintsList.append(ctr);
			constraintsListWidget->addItem(s);
		}
	}

	if(constraintsListWidget->count()>0)
		constraintsListWidget->setCurrentRow(0);
	else
		this->constraintChanged(-1);
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
		currentConstraintTextEdit->setPlainText("");
		return;
	}
	QString s;
	assert(index<this->visibleConstraintsList.size());
	TimeConstraint* ctr=this->visibleConstraintsList.at(index);
	assert(ctr!=nullptr);
	s=ctr->getDetailedDescription(gt.rules);
	currentConstraintTextEdit->setPlainText(s);
}

void ConstraintSubactivitiesPreferredStartingTimesForm::addConstraint()
{
	AddConstraintSubactivitiesPreferredStartingTimesForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();

	this->filterChanged();
	
	constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
}

void ConstraintSubactivitiesPreferredStartingTimesForm::modifyConstraint()
{
	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();

	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintSubactivitiesPreferredStartingTimesForm form(this, (ConstraintSubactivitiesPreferredStartingTimes*)ctr);
	setParentAndOtherThings(&form, this);
	form.exec();

	this->filterChanged();
	
	constraintsListWidget->verticalScrollBar()->setValue(valv);
	constraintsListWidget->horizontalScrollBar()->setValue(valh);

	if(i>=constraintsListWidget->count())
		i=constraintsListWidget->count()-1;

	if(i>=0)
		constraintsListWidget->setCurrentRow(i);
	else
		this->constraintChanged(-1);
}

void ConstraintSubactivitiesPreferredStartingTimesForm::removeConstraint()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);
	QString s;
	s=tr("Remove constraint?");
	s+="\n\n";
	s+=ctr->getDetailedDescription(gt.rules);
	
	QListWidgetItem* item;

	QString oc;

	switch( LongTextMessageBox::confirmation( this, tr("FET confirmation"),
		s, tr("Yes"), tr("No"), QString(), 0, 1 ) ){
	case 0: // The user clicked the OK button or pressed Enter
		oc=ctr->getDetailedDescription(gt.rules);

		gt.rules.removeTimeConstraint(ctr);

		gt.rules.addUndoPoint(tr("Removed the constraint:\n\n%1").arg(oc));
		
		visibleConstraintsList.removeAt(i);
		constraintsListWidget->setCurrentRow(-1);
		item=constraintsListWidget->takeItem(i);
		delete item;
		
		break;
	case 1: // The user clicked the Cancel button or pressed Escape
		break;
	}

	if(i>=constraintsListWidget->count())
		i=constraintsListWidget->count()-1;
	if(i>=0)
		constraintsListWidget->setCurrentRow(i);
	else
		this->constraintChanged(-1);
}

void ConstraintSubactivitiesPreferredStartingTimesForm::help()
{
	LongTextMessageBox::largeInformation(this, tr("FET help"), tr(
	 "This constraint is used to specify that for some components of a type of"
	" activities, for instance for math activities, you need that from say 4-5"
	" per week, at least the first 2 to start early (say in the first 4 hours).")+
	+"\n\n"+
	tr("This is simple: just add 2 constraints of this type, with the component"
	" number 1, (then 2) and the corresponding subject (you can also"
	" define teacher, students set and activity tag, of course)."
	 )
	 +"\n\n"+
	 tr("Note: if you have math with 4 and 5 splits and want to constrain 2 subactivities for those with 4 per week and 3 subactivities"
	 " for those with 5 per week, you can add constraints only for math with components 3, 4 and 5 (nice trick)")
	 +"\n\n"+
	 tr("Note: if an activity does not belong to a larger group (is a single activity),"
	 " it is considered to have component number 1. As an advice, if you have activities of say biology split into 1"
	 " or 2 per week and want to constrain only one component if there are 2 per week, add a constraint for biology with component number=2")
	 +"\n\n"+
	 tr("Note: if a teacher or a students set cannot have activities in some slots because of this constraint, gaps will be counted (if you have max gaps constraints)"
 	  ". So be careful if you add this constraint for all subactivities, for only a teacher or for only a students set"));
}
