/***************************************************************************
                          constraintteacherhomeroomsform.cpp  -  description
                             -------------------
    begin                : 8 April 2005
    copyright            : (C) 2005 by Liviu Lalescu
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

#include "constraintteacherhomeroomsform.h"
#include "addconstraintteacherhomeroomsform.h"
#include "modifyconstraintteacherhomeroomsform.h"

#include <QListWidget>
#include <QScrollBar>
#include <QAbstractItemView>

ConstraintTeacherHomeRoomsForm::ConstraintTeacherHomeRoomsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	currentConstraintTextEdit->setReadOnly(true);
	
	modifyConstraintPushButton->setDefault(true);

	constraintsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(addConstraintPushButton, &QPushButton::clicked, this, &ConstraintTeacherHomeRoomsForm::addConstraint);
	connect(removeConstraintPushButton, &QPushButton::clicked, this, &ConstraintTeacherHomeRoomsForm::removeConstraint);
	connect(closePushButton, &QPushButton::clicked, this, &ConstraintTeacherHomeRoomsForm::close);
	connect(constraintsListWidget, &QListWidget::currentRowChanged, this, &ConstraintTeacherHomeRoomsForm::constraintChanged);
	connect(modifyConstraintPushButton, &QPushButton::clicked, this, &ConstraintTeacherHomeRoomsForm::modifyConstraint);
	connect(constraintsListWidget, &QListWidget::itemDoubleClicked, this, &ConstraintTeacherHomeRoomsForm::modifyConstraint);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	
	QSize tmp5=roomsComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	
	teachersComboBox->addItem("");
	for(Teacher* tch : std::as_const(gt.rules.teachersList))
		teachersComboBox->addItem(tch->name);
	
	roomsComboBox->addItem("");
	for(Room* rm : std::as_const(gt.rules.roomsList))
		roomsComboBox->addItem(rm->name);

	this->filterChanged();

	connect(teachersComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ConstraintTeacherHomeRoomsForm::filterChanged);
	connect(roomsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ConstraintTeacherHomeRoomsForm::filterChanged);
}

ConstraintTeacherHomeRoomsForm::~ConstraintTeacherHomeRoomsForm()
{
	saveFETDialogGeometry(this);
}

void ConstraintTeacherHomeRoomsForm::filterChanged()
{
	this->visibleConstraintsList.clear();
	constraintsListWidget->clear();
	for(int i=0; i<gt.rules.spaceConstraintsList.size(); i++){
		SpaceConstraint* ctr=gt.rules.spaceConstraintsList[i];
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

bool ConstraintTeacherHomeRoomsForm::filterOk(SpaceConstraint* ctr)
{
	if(ctr->type==CONSTRAINT_TEACHER_HOME_ROOMS){
		ConstraintTeacherHomeRooms* c=(ConstraintTeacherHomeRooms*)ctr;
		return (c->teacherName==teachersComboBox->currentText() || teachersComboBox->currentText()=="")
		  && (roomsComboBox->currentText()=="" || c->roomsNames.contains(roomsComboBox->currentText()));
	}
	else
		return false;
}

void ConstraintTeacherHomeRoomsForm::constraintChanged(int index)
{
	if(index<0){
		currentConstraintTextEdit->setPlainText("");
		return;
	}
	QString s;
	assert(index<this->visibleConstraintsList.size());
	SpaceConstraint* ctr=this->visibleConstraintsList.at(index);
	assert(ctr!=nullptr);
	s=ctr->getDetailedDescription(gt.rules);
	currentConstraintTextEdit->setPlainText(s);
}

void ConstraintTeacherHomeRoomsForm::addConstraint()
{
	AddConstraintTeacherHomeRoomsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();

	this->filterChanged();
	
	constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
}

void ConstraintTeacherHomeRoomsForm::modifyConstraint()
{
	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();

	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	SpaceConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintTeacherHomeRoomsForm form(this, (ConstraintTeacherHomeRooms*)ctr);
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

void ConstraintTeacherHomeRoomsForm::removeConstraint()
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
