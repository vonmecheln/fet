/***************************************************************************
                          constraintactivitiesendstudentsdayform.cpp  -  description
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

#include "constraintactivitiesendstudentsdayform.h"
#include "addconstraintactivitiesendstudentsdayform.h"
#include "modifyconstraintactivitiesendstudentsdayform.h"

#include <QListWidget>
#include <QScrollBar>
#include <QAbstractItemView>

ConstraintActivitiesEndStudentsDayForm::ConstraintActivitiesEndStudentsDayForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	currentConstraintTextEdit->setReadOnly(true);

	modifyConstraintPushButton->setDefault(true);

	constraintsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(constraintsListWidget, &QListWidget::currentRowChanged, this, &ConstraintActivitiesEndStudentsDayForm::constraintChanged);
	connect(addConstraintPushButton, &QPushButton::clicked, this, &ConstraintActivitiesEndStudentsDayForm::addConstraint);
	connect(closePushButton, &QPushButton::clicked, this, &ConstraintActivitiesEndStudentsDayForm::close);
	connect(removeConstraintPushButton, &QPushButton::clicked, this, &ConstraintActivitiesEndStudentsDayForm::removeConstraint);
	connect(modifyConstraintPushButton, &QPushButton::clicked, this, &ConstraintActivitiesEndStudentsDayForm::modifyConstraint);
	connect(constraintsListWidget, &QListWidget::itemDoubleClicked, this, &ConstraintActivitiesEndStudentsDayForm::modifyConstraint);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	this->filterChanged();
}

ConstraintActivitiesEndStudentsDayForm::~ConstraintActivitiesEndStudentsDayForm()
{
	saveFETDialogGeometry(this);
}

void ConstraintActivitiesEndStudentsDayForm::filterChanged()
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

bool ConstraintActivitiesEndStudentsDayForm::filterOk(TimeConstraint* ctr)
{
	if(ctr->type==CONSTRAINT_ACTIVITIES_END_STUDENTS_DAY)
		return true;
	else
		return false;
}

void ConstraintActivitiesEndStudentsDayForm::constraintChanged(int index)
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

void ConstraintActivitiesEndStudentsDayForm::addConstraint()
{
	AddConstraintActivitiesEndStudentsDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();

	this->filterChanged();
	
	constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
}

void ConstraintActivitiesEndStudentsDayForm::modifyConstraint()
{
	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();

	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintActivitiesEndStudentsDayForm form(this, (ConstraintActivitiesEndStudentsDay*)ctr);
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

void ConstraintActivitiesEndStudentsDayForm::removeConstraint()
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
