/***************************************************************************
                          constraintteachermaxtwoconsecutiveafternoonsform.cpp  -  description
                             -------------------
    begin                : Oct 15, 2015
    copyright            : (C) 2015 by Liviu Lalescu
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

#include "constraintteachermaxtwoconsecutiveafternoonsform.h"
#include "addconstraintteachermaxtwoconsecutiveafternoonsform.h"
#include "modifyconstraintteachermaxtwoconsecutiveafternoonsform.h"

#include <QListWidget>
#include <QScrollBar>
#include <QAbstractItemView>

ConstraintTeacherMaxTwoConsecutiveAfternoonsForm::ConstraintTeacherMaxTwoConsecutiveAfternoonsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	currentConstraintTextEdit->setReadOnly(true);
	
	modifyConstraintPushButton->setDefault(true);

	constraintsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(constraintsListWidget, &QListWidget::currentRowChanged, this, &ConstraintTeacherMaxTwoConsecutiveAfternoonsForm::constraintChanged);
	connect(addConstraintPushButton, &QPushButton::clicked, this, &ConstraintTeacherMaxTwoConsecutiveAfternoonsForm::addConstraint);
	connect(closePushButton, &QPushButton::clicked, this, &ConstraintTeacherMaxTwoConsecutiveAfternoonsForm::close);
	connect(removeConstraintPushButton, &QPushButton::clicked, this, &ConstraintTeacherMaxTwoConsecutiveAfternoonsForm::removeConstraint);
	connect(modifyConstraintPushButton, &QPushButton::clicked, this, &ConstraintTeacherMaxTwoConsecutiveAfternoonsForm::modifyConstraint);
	connect(constraintsListWidget, &QListWidget::itemDoubleClicked, this, &ConstraintTeacherMaxTwoConsecutiveAfternoonsForm::modifyConstraint);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	
	teachersComboBox->addItem("");
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		teachersComboBox->addItem(tch->name);
	}

	this->filterChanged();

	connect(teachersComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ConstraintTeacherMaxTwoConsecutiveAfternoonsForm::filterChanged);
}

ConstraintTeacherMaxTwoConsecutiveAfternoonsForm::~ConstraintTeacherMaxTwoConsecutiveAfternoonsForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeacherMaxTwoConsecutiveAfternoonsForm::filterOk(TimeConstraint* ctr)
{
	if(ctr->type==CONSTRAINT_TEACHER_MAX_TWO_CONSECUTIVE_AFTERNOONS){
		ConstraintTeacherMaxTwoConsecutiveAfternoons* c=(ConstraintTeacherMaxTwoConsecutiveAfternoons*) ctr;
		return c->teacherName==teachersComboBox->currentText() || teachersComboBox->currentText()=="";
	}
	else
		return false;
}

void ConstraintTeacherMaxTwoConsecutiveAfternoonsForm::filterChanged()
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
		constraintChanged(-1);
}

void ConstraintTeacherMaxTwoConsecutiveAfternoonsForm::constraintChanged(int index)
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

void ConstraintTeacherMaxTwoConsecutiveAfternoonsForm::addConstraint()
{
	AddConstraintTeacherMaxTwoConsecutiveAfternoonsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();

	filterChanged();
	
	constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
}

void ConstraintTeacherMaxTwoConsecutiveAfternoonsForm::modifyConstraint()
{
	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();

	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintTeacherMaxTwoConsecutiveAfternoonsForm form(this, (ConstraintTeacherMaxTwoConsecutiveAfternoons*)ctr);
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

void ConstraintTeacherMaxTwoConsecutiveAfternoonsForm::removeConstraint()
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
