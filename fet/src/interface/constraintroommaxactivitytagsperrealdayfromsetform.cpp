/***************************************************************************
                          constraintroommaxactivitytagsperrealdayfromsetform.cpp  -  description
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

#include "constraintroommaxactivitytagsperrealdayfromsetform.h"
#include "addconstraintroommaxactivitytagsperrealdayfromsetform.h"
#include "modifyconstraintroommaxactivitytagsperrealdayfromsetform.h"

#include <QListWidget>
#include <QScrollBar>
#include <QAbstractItemView>

ConstraintRoomMaxActivityTagsPerRealDayFromSetForm::ConstraintRoomMaxActivityTagsPerRealDayFromSetForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	currentConstraintTextEdit->setReadOnly(true);
	
	modifyConstraintPushButton->setDefault(true);

	connect(constraintsListWidget, &QListWidget::currentRowChanged, this, &ConstraintRoomMaxActivityTagsPerRealDayFromSetForm::constraintChanged);
	connect(addConstraintPushButton, &QPushButton::clicked, this, &ConstraintRoomMaxActivityTagsPerRealDayFromSetForm::addConstraint);
	connect(closePushButton, &QPushButton::clicked, this, &ConstraintRoomMaxActivityTagsPerRealDayFromSetForm::close);
	connect(removeConstraintPushButton, &QPushButton::clicked, this, &ConstraintRoomMaxActivityTagsPerRealDayFromSetForm::removeConstraint);
	connect(modifyConstraintPushButton, &QPushButton::clicked, this, &ConstraintRoomMaxActivityTagsPerRealDayFromSetForm::modifyConstraint);
	connect(constraintsListWidget, &QListWidget::itemDoubleClicked, this, &ConstraintRoomMaxActivityTagsPerRealDayFromSetForm::modifyConstraint);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp5=roomsComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	
	roomsComboBox->addItem("");
	for(int i=0; i<gt.rules.roomsList.size(); i++){
		Room* rm=gt.rules.roomsList[i];
		roomsComboBox->addItem(rm->name);
	}

	activityTagsComboBox->clear();
	activityTagsComboBox->addItem("");
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* at=gt.rules.activityTagsList[i];
		activityTagsComboBox->addItem(at->name);
	}

	this->filterChanged();

	connect(roomsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ConstraintRoomMaxActivityTagsPerRealDayFromSetForm::filterChanged);
	connect(activityTagsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ConstraintRoomMaxActivityTagsPerRealDayFromSetForm::filterChanged);
}

ConstraintRoomMaxActivityTagsPerRealDayFromSetForm::~ConstraintRoomMaxActivityTagsPerRealDayFromSetForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintRoomMaxActivityTagsPerRealDayFromSetForm::filterOk(SpaceConstraint* ctr)
{
	if(ctr->type==CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET){
		ConstraintRoomMaxActivityTagsPerRealDayFromSet* ct=(ConstraintRoomMaxActivityTagsPerRealDayFromSet*) ctr;
		return (roomsComboBox->currentText()=="" || ct->room==roomsComboBox->currentText()) &&
		 (activityTagsComboBox->currentText()=="" || ct->tagsList.contains(activityTagsComboBox->currentText()));
	}
	else
		return false;
}

void ConstraintRoomMaxActivityTagsPerRealDayFromSetForm::filterChanged()
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

void ConstraintRoomMaxActivityTagsPerRealDayFromSetForm::constraintChanged(int index)
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

void ConstraintRoomMaxActivityTagsPerRealDayFromSetForm::addConstraint()
{
	AddConstraintRoomMaxActivityTagsPerRealDayFromSetForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();

	filterChanged();
	
	constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
}

void ConstraintRoomMaxActivityTagsPerRealDayFromSetForm::modifyConstraint()
{
	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();

	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	SpaceConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintRoomMaxActivityTagsPerRealDayFromSetForm form(this, (ConstraintRoomMaxActivityTagsPerRealDayFromSet*)ctr);
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

void ConstraintRoomMaxActivityTagsPerRealDayFromSetForm::removeConstraint()
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