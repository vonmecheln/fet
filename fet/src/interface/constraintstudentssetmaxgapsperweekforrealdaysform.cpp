/***************************************************************************
                          constraintstudentssetmaxgapsperweekforrealdaysform.cpp  -  description
                             -------------------
    begin                : 2020
    copyright            : (C) 2020 by Liviu Lalescu
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

#include "helponimperfectconstraints.h"

#include "longtextmessagebox.h"

#include "constraintstudentssetmaxgapsperweekforrealdaysform.h"
#include "addconstraintstudentssetmaxgapsperweekforrealdaysform.h"
#include "modifyconstraintstudentssetmaxgapsperweekforrealdaysform.h"

#include <QListWidget>
#include <QScrollBar>
#include <QAbstractItemView>

ConstraintStudentsSetMaxGapsPerWeekForRealDaysForm::ConstraintStudentsSetMaxGapsPerWeekForRealDaysForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	currentConstraintTextEdit->setReadOnly(true);

	modifyConstraintPushButton->setDefault(true);

	constraintsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(constraintsListWidget, &QListWidget::currentRowChanged, this, &ConstraintStudentsSetMaxGapsPerWeekForRealDaysForm::constraintChanged);
	connect(addConstraintPushButton, &QPushButton::clicked, this, &ConstraintStudentsSetMaxGapsPerWeekForRealDaysForm::addConstraint);
	connect(closePushButton, &QPushButton::clicked, this, &ConstraintStudentsSetMaxGapsPerWeekForRealDaysForm::close);
	connect(removeConstraintPushButton, &QPushButton::clicked, this, &ConstraintStudentsSetMaxGapsPerWeekForRealDaysForm::removeConstraint);
	connect(modifyConstraintPushButton, &QPushButton::clicked, this, &ConstraintStudentsSetMaxGapsPerWeekForRealDaysForm::modifyConstraint);
	connect(constraintsListWidget, &QListWidget::itemDoubleClicked, this, &ConstraintStudentsSetMaxGapsPerWeekForRealDaysForm::modifyConstraint);
	connect(helpPushButton, &QPushButton::clicked, this, &ConstraintStudentsSetMaxGapsPerWeekForRealDaysForm::help);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	
	populateStudentsComboBox(studentsComboBox, QString(""), true);

	this->filterChanged();

	connect(studentsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ConstraintStudentsSetMaxGapsPerWeekForRealDaysForm::filterChanged);
}

ConstraintStudentsSetMaxGapsPerWeekForRealDaysForm::~ConstraintStudentsSetMaxGapsPerWeekForRealDaysForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsSetMaxGapsPerWeekForRealDaysForm::filterOk(TimeConstraint* ctr)
{
	if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS){
		ConstraintStudentsSetMaxGapsPerWeekForRealDays* c=(ConstraintStudentsSetMaxGapsPerWeekForRealDays*) ctr;
		return c->students==studentsComboBox->currentText() || studentsComboBox->currentText()=="";
	}
	else
		return false;
}

void ConstraintStudentsSetMaxGapsPerWeekForRealDaysForm::filterChanged()
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

void ConstraintStudentsSetMaxGapsPerWeekForRealDaysForm::constraintChanged(int index)
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

void ConstraintStudentsSetMaxGapsPerWeekForRealDaysForm::addConstraint()
{
	AddConstraintStudentsSetMaxGapsPerWeekForRealDaysForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();

	filterChanged();
	
	constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
}

void ConstraintStudentsSetMaxGapsPerWeekForRealDaysForm::modifyConstraint()
{
	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();

	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintStudentsSetMaxGapsPerWeekForRealDaysForm form(this, (ConstraintStudentsSetMaxGapsPerWeekForRealDays*)ctr);
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

void ConstraintStudentsSetMaxGapsPerWeekForRealDaysForm::removeConstraint()
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

void ConstraintStudentsSetMaxGapsPerWeekForRealDaysForm::help()
{
	HelpOnImperfectConstraints::help(this);
}
