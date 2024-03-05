/***************************************************************************
                          constraintstudentssetmaxbuildingchangesperrealdayinintervalform.cpp  -  description
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

#include "constraintstudentssetmaxbuildingchangesperrealdayinintervalform.h"
#include "addconstraintstudentssetmaxbuildingchangesperrealdayinintervalform.h"
#include "modifyconstraintstudentssetmaxbuildingchangesperrealdayinintervalform.h"

#include <QListWidget>
#include <QScrollBar>
#include <QAbstractItemView>

ConstraintStudentsSetMaxBuildingChangesPerRealDayInIntervalForm::ConstraintStudentsSetMaxBuildingChangesPerRealDayInIntervalForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	currentConstraintTextEdit->setReadOnly(true);
	
	modifyConstraintPushButton->setDefault(true);

	constraintsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(constraintsListWidget, &QListWidget::currentRowChanged, this, &ConstraintStudentsSetMaxBuildingChangesPerRealDayInIntervalForm::constraintChanged);
	connect(addConstraintPushButton, &QPushButton::clicked, this, &ConstraintStudentsSetMaxBuildingChangesPerRealDayInIntervalForm::addConstraint);
	connect(closePushButton, &QPushButton::clicked, this, &ConstraintStudentsSetMaxBuildingChangesPerRealDayInIntervalForm::close);
	connect(removeConstraintPushButton, &QPushButton::clicked, this, &ConstraintStudentsSetMaxBuildingChangesPerRealDayInIntervalForm::removeConstraint);
	connect(modifyConstraintPushButton, &QPushButton::clicked, this, &ConstraintStudentsSetMaxBuildingChangesPerRealDayInIntervalForm::modifyConstraint);

	connect(constraintsListWidget, &QListWidget::itemDoubleClicked, this, &ConstraintStudentsSetMaxBuildingChangesPerRealDayInIntervalForm::modifyConstraint);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	
	populateStudentsComboBox(studentsComboBox, QString(""), true);

	this->filterChanged();

	connect(studentsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ConstraintStudentsSetMaxBuildingChangesPerRealDayInIntervalForm::filterChanged);
}

ConstraintStudentsSetMaxBuildingChangesPerRealDayInIntervalForm::~ConstraintStudentsSetMaxBuildingChangesPerRealDayInIntervalForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintStudentsSetMaxBuildingChangesPerRealDayInIntervalForm::filterOk(SpaceConstraint* ctr)
{
	if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL){
		ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval* c=(ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval*) ctr;
		return c->studentsName==studentsComboBox->currentText() || studentsComboBox->currentText()=="";
	}
	else
		return false;
}

void ConstraintStudentsSetMaxBuildingChangesPerRealDayInIntervalForm::filterChanged()
{
	this->visibleConstraintsList.clear();
	constraintsListWidget->clear();
	for(int i=0; i<gt.rules.spaceConstraintsList.size(); i++){
		SpaceConstraint* ctr=gt.rules.spaceConstraintsList[i];
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

void ConstraintStudentsSetMaxBuildingChangesPerRealDayInIntervalForm::constraintChanged(int index)
{
	if(index<0){
		currentConstraintTextEdit->setPlainText("");
		return;
	}
	assert(index<this->visibleConstraintsList.size());
	SpaceConstraint* ctr=this->visibleConstraintsList.at(index);
	assert(ctr!=nullptr);
	currentConstraintTextEdit->setPlainText(ctr->getDetailedDescription(gt.rules));
}

void ConstraintStudentsSetMaxBuildingChangesPerRealDayInIntervalForm::addConstraint()
{
	AddConstraintStudentsSetMaxBuildingChangesPerRealDayInIntervalForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();

	filterChanged();
	
	constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
}

void ConstraintStudentsSetMaxBuildingChangesPerRealDayInIntervalForm::modifyConstraint()
{
	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();

	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	SpaceConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintStudentsSetMaxBuildingChangesPerRealDayInIntervalForm form(this, (ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval*)ctr);
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

void ConstraintStudentsSetMaxBuildingChangesPerRealDayInIntervalForm::removeConstraint()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	SpaceConstraint* ctr=this->visibleConstraintsList.at(i);
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

		gt.rules.removeSpaceConstraint(ctr);

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
