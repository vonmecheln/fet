/***************************************************************************
                          constraintstudentsmorningsearlymaxbeginningsatsecondhourform.cpp  -  description
                             -------------------
    begin                : 2021
    copyright            : (C) 2021 by Liviu Lalescu
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

#include "constraintstudentsmorningsearlymaxbeginningsatsecondhourform.h"
#include "addconstraintstudentsmorningsearlymaxbeginningsatsecondhourform.h"
#include "modifyconstraintstudentsmorningsearlymaxbeginningsatsecondhourform.h"

#include <QListWidget>
#include <QScrollBar>
#include <QAbstractItemView>

ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHourForm::ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHourForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	currentConstraintTextEdit->setReadOnly(true);

	modifyConstraintPushButton->setDefault(true);

	constraintsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(constraintsListWidget, &QListWidget::currentRowChanged, this, &ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHourForm::constraintChanged);
	connect(addConstraintPushButton, &QPushButton::clicked, this, &ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHourForm::addConstraint);
	connect(closePushButton, &QPushButton::clicked, this, &ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHourForm::close);
	connect(removeConstraintPushButton, &QPushButton::clicked, this, &ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHourForm::removeConstraint);
	connect(modifyConstraintPushButton, &QPushButton::clicked, this, &ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHourForm::modifyConstraint);
	connect(constraintsListWidget, &QListWidget::itemDoubleClicked, this, &ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHourForm::modifyConstraint);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	this->filterChanged();
}

ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHourForm::~ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHourForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHourForm::filterOk(TimeConstraint* ctr)
{
	if(ctr->type==CONSTRAINT_STUDENTS_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR)
		return true;
	else
		return false;
}

void ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHourForm::filterChanged()
{
	this->visibleConstraintsList.clear();
	constraintsListWidget->clear();
	for(int i=0; i<gt.rules.timeConstraintsList.size(); i++){
		TimeConstraint* ctr=gt.rules.timeConstraintsList[i];
		if(filterOk(ctr)){
			visibleConstraintsList.append(ctr);
			constraintsListWidget->addItem(ctr->getDescription(gt.rules));
		}
	}
	
	if(constraintsListWidget->count()>0)
		constraintsListWidget->setCurrentRow(0);
	else
		this->constraintChanged(-1);
}

void ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHourForm::constraintChanged(int index)
{
	if(index<0){
		currentConstraintTextEdit->setPlainText("");
		return;
	}
	assert(index<this->visibleConstraintsList.size());
	TimeConstraint* ctr=this->visibleConstraintsList.at(index);
	assert(ctr!=nullptr);
	currentConstraintTextEdit->setPlainText(ctr->getDetailedDescription(gt.rules));
}

void ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHourForm::addConstraint()
{
	AddConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHourForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();

	filterChanged();
	
	constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
}

void ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHourForm::modifyConstraint()
{
	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();

	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHourForm form(this, (ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHour*)ctr);
	setParentAndOtherThings(&form, this);
	form.exec();

	filterChanged();

	constraintsListWidget->verticalScrollBar()->setValue(valv);
	constraintsListWidget->horizontalScrollBar()->setValue(valh);

	if(i>=constraintsListWidget->count())
		i=constraintsListWidget->count()-1;

	if(i>=0)
		constraintsListWidget->setCurrentRow(i);
	else
		this->constraintChanged(-1);
}

void ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHourForm::removeConstraint()
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
