/***************************************************************************
                          constraintteachermaxthreeconsecutivedaysform.cpp  -  description
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

#include "constraintteachermaxthreeconsecutivedaysform.h"
#include "addconstraintteachermaxthreeconsecutivedaysform.h"
#include "modifyconstraintteachermaxthreeconsecutivedaysform.h"

#include <QListWidget>
#include <QScrollBar>
#include <QAbstractItemView>

ConstraintTeacherMaxThreeConsecutiveDaysForm::ConstraintTeacherMaxThreeConsecutiveDaysForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	currentConstraintTextEdit->setReadOnly(true);
	
	modifyConstraintPushButton->setDefault(true);

	constraintsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(constraintsListWidget, &QListWidget::currentRowChanged, this, &ConstraintTeacherMaxThreeConsecutiveDaysForm::constraintChanged);
	connect(addConstraintPushButton, &QPushButton::clicked, this, &ConstraintTeacherMaxThreeConsecutiveDaysForm::addConstraint);
	connect(closePushButton, &QPushButton::clicked, this, &ConstraintTeacherMaxThreeConsecutiveDaysForm::close);
	connect(helpPushButton, &QPushButton::clicked, this, &ConstraintTeacherMaxThreeConsecutiveDaysForm::help);
	connect(removeConstraintPushButton, &QPushButton::clicked, this, &ConstraintTeacherMaxThreeConsecutiveDaysForm::removeConstraint);
	connect(modifyConstraintPushButton, &QPushButton::clicked, this, &ConstraintTeacherMaxThreeConsecutiveDaysForm::modifyConstraint);

	connect(constraintsListWidget, &QListWidget::itemDoubleClicked, this, &ConstraintTeacherMaxThreeConsecutiveDaysForm::modifyConstraint);

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

	connect(teachersComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ConstraintTeacherMaxThreeConsecutiveDaysForm::filterChanged);
}

ConstraintTeacherMaxThreeConsecutiveDaysForm::~ConstraintTeacherMaxThreeConsecutiveDaysForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeacherMaxThreeConsecutiveDaysForm::filterOk(TimeConstraint* ctr)
{
	if(ctr->type==CONSTRAINT_TEACHER_MAX_THREE_CONSECUTIVE_DAYS){
		ConstraintTeacherMaxThreeConsecutiveDays* c=(ConstraintTeacherMaxThreeConsecutiveDays*) ctr;
		return c->teacherName==teachersComboBox->currentText() || teachersComboBox->currentText()=="";
	}
	else
		return false;
}

void ConstraintTeacherMaxThreeConsecutiveDaysForm::filterChanged()
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

void ConstraintTeacherMaxThreeConsecutiveDaysForm::constraintChanged(int index)
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

void ConstraintTeacherMaxThreeConsecutiveDaysForm::addConstraint()
{
	AddConstraintTeacherMaxThreeConsecutiveDaysForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();

	filterChanged();
	
	constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
}

void ConstraintTeacherMaxThreeConsecutiveDaysForm::modifyConstraint()
{
	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();

	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintTeacherMaxThreeConsecutiveDaysForm form(this, (ConstraintTeacherMaxThreeConsecutiveDays*)ctr);
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

void ConstraintTeacherMaxThreeConsecutiveDaysForm::removeConstraint()
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

void ConstraintTeacherMaxThreeConsecutiveDaysForm::help()
{
	QString s=tr("This constraint was suggested by %1, %2, and other users on this FET forum topic: %3.", "%1 and %2 are two persons, %3 is an internet link")
	 .arg("Benahmed Abdelkrim").arg("Hiba Hadi")
	 .arg("https://lalescu.ro/liviu/fet/forum/index.php?topic=4990.0");
	s+="\n\n";
	s+=tr("It was designed to disallow a teacher to have activities in two consecutive real days, morning and afternoon on both,"
	 " which can be tiring. If you can allow the teacher to have activities only on the afternoon of a real day (half day 1), then the whole next real day"
	 " (both morning and afternoon, half days 2 and 3), then only on the next morning (half day 4), you can select the exception option."
	 " In this case only a succession of these four half-days: morning-afternoon-morning-afternoon is NOT allowed by the algorithm.");
	LongTextMessageBox::information(this, tr("FET help"), s);
}
