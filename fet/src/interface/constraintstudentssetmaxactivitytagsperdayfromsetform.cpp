/***************************************************************************
                          constraintstudentssetmaxactivitytagsperdayfromsetform.cpp  -  description
                             -------------------
    begin                : 2024
    copyright            : (C) 2024 by Liviu Lalescu
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

#include "constraintstudentssetmaxactivitytagsperdayfromsetform.h"
#include "addconstraintstudentssetmaxactivitytagsperdayfromsetform.h"
#include "modifyconstraintstudentssetmaxactivitytagsperdayfromsetform.h"

#include <QListWidget>
#include <QScrollBar>
#include <QAbstractItemView>

ConstraintStudentsSetMaxActivityTagsPerDayFromSetForm::ConstraintStudentsSetMaxActivityTagsPerDayFromSetForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	currentConstraintTextEdit->setReadOnly(true);
	
	modifyConstraintPushButton->setDefault(true);

	connect(constraintsListWidget, &QListWidget::currentRowChanged, this, &ConstraintStudentsSetMaxActivityTagsPerDayFromSetForm::constraintChanged);
	connect(addConstraintPushButton, &QPushButton::clicked, this, &ConstraintStudentsSetMaxActivityTagsPerDayFromSetForm::addConstraint);
	connect(closePushButton, &QPushButton::clicked, this, &ConstraintStudentsSetMaxActivityTagsPerDayFromSetForm::close);
	connect(removeConstraintPushButton, &QPushButton::clicked, this, &ConstraintStudentsSetMaxActivityTagsPerDayFromSetForm::removeConstraint);
	connect(modifyConstraintPushButton, &QPushButton::clicked, this, &ConstraintStudentsSetMaxActivityTagsPerDayFromSetForm::modifyConstraint);
	connect(constraintsListWidget, &QListWidget::itemDoubleClicked, this, &ConstraintStudentsSetMaxActivityTagsPerDayFromSetForm::modifyConstraint);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	
	populateStudentsComboBox(studentsComboBox, QString(""), true);

	activityTagsComboBox->clear();
	activityTagsComboBox->addItem("");
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* at=gt.rules.activityTagsList[i];
		activityTagsComboBox->addItem(at->name);
	}

	this->filterChanged();

	connect(studentsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ConstraintStudentsSetMaxActivityTagsPerDayFromSetForm::filterChanged);
	connect(activityTagsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ConstraintStudentsSetMaxActivityTagsPerDayFromSetForm::filterChanged);
}

ConstraintStudentsSetMaxActivityTagsPerDayFromSetForm::~ConstraintStudentsSetMaxActivityTagsPerDayFromSetForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsSetMaxActivityTagsPerDayFromSetForm::filterOk(TimeConstraint* ctr)
{
	if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET){
		ConstraintStudentsSetMaxActivityTagsPerDayFromSet* ct=(ConstraintStudentsSetMaxActivityTagsPerDayFromSet*) ctr;
		return (studentsComboBox->currentText()=="" || ct->students==studentsComboBox->currentText()) &&
		 (activityTagsComboBox->currentText()=="" || ct->tagsList.contains(activityTagsComboBox->currentText()));
	}
	else
		return false;
}

void ConstraintStudentsSetMaxActivityTagsPerDayFromSetForm::filterChanged()
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

void ConstraintStudentsSetMaxActivityTagsPerDayFromSetForm::constraintChanged(int index)
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

void ConstraintStudentsSetMaxActivityTagsPerDayFromSetForm::addConstraint()
{
	AddConstraintStudentsSetMaxActivityTagsPerDayFromSetForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();

	filterChanged();
	
	constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
}

void ConstraintStudentsSetMaxActivityTagsPerDayFromSetForm::modifyConstraint()
{
	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();

	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintStudentsSetMaxActivityTagsPerDayFromSetForm form(this, (ConstraintStudentsSetMaxActivityTagsPerDayFromSet*)ctr);
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

void ConstraintStudentsSetMaxActivityTagsPerDayFromSetForm::removeConstraint()
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