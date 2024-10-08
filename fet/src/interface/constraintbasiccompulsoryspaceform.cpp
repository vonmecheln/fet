/***************************************************************************
                          constraintbasiccompulsoryspaceform.cpp  -  description
                             -------------------
    begin                : Feb 13, 2005
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

#include "constraintbasiccompulsoryspaceform.h"
#include "addconstraintbasiccompulsoryspaceform.h"
#include "modifyconstraintbasiccompulsoryspaceform.h"

#include <QListWidget>
#include <QScrollBar>
#include <QAbstractItemView>

ConstraintBasicCompulsorySpaceForm::ConstraintBasicCompulsorySpaceForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	currentConstraintTextEdit->setReadOnly(true);
	
	modifyConstraintPushButton->setDefault(true);

	connect(constraintsListWidget, &QListWidget::currentRowChanged, this, &ConstraintBasicCompulsorySpaceForm::constraintChanged);
	connect(addConstraintPushButton, &QPushButton::clicked, this, &ConstraintBasicCompulsorySpaceForm::addConstraint);
	connect(closePushButton, &QPushButton::clicked, this, &ConstraintBasicCompulsorySpaceForm::close);
	connect(removeConstraintPushButton, &QPushButton::clicked, this, &ConstraintBasicCompulsorySpaceForm::removeConstraint);
	connect(modifyConstraintPushButton, &QPushButton::clicked, this, &ConstraintBasicCompulsorySpaceForm::modifyConstraint);
	connect(constraintsListWidget, &QListWidget::itemDoubleClicked, this, &ConstraintBasicCompulsorySpaceForm::modifyConstraint);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	this->filterChanged();
}

ConstraintBasicCompulsorySpaceForm::~ConstraintBasicCompulsorySpaceForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintBasicCompulsorySpaceForm::filterOk(SpaceConstraint* ctr)
{
	if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_SPACE)
		return true;
	else
		return false;
}

void ConstraintBasicCompulsorySpaceForm::filterChanged()
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

void ConstraintBasicCompulsorySpaceForm::constraintChanged(int index)
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

void ConstraintBasicCompulsorySpaceForm::addConstraint()
{
	AddConstraintBasicCompulsorySpaceForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();

	filterChanged();
	
	constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
}

void ConstraintBasicCompulsorySpaceForm::modifyConstraint()
{
	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();

	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	SpaceConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintBasicCompulsorySpaceForm form(this, (ConstraintBasicCompulsorySpace*)ctr);
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

void ConstraintBasicCompulsorySpaceForm::removeConstraint()
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

	int lres=LongTextMessageBox::confirmation( this, tr("FET confirmation"),
		s, tr("Yes"), tr("No"), QString(), 0, 1 );
		
	if(lres==0){
		//The user clicked the OK button or pressed Enter
		
		assert(ctr->type==CONSTRAINT_BASIC_COMPULSORY_SPACE);
		
		QString s=tr("Do you really want to remove the basic compulsory space constraint?");
		s+=" ";
		s+=tr("You cannot generate a timetable without this constraint.");
		s+="\n\n";
		s+=tr("Note: you can add again a constraint of this type from the menu Data -> Space constraints -> "
			"Miscellaneous -> Basic compulsory space constraints.");
		
		QMessageBox::StandardButton wr=QMessageBox::warning(this, tr("FET warning"), s,
			QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
		
		if(wr==QMessageBox::Yes){
			oc=ctr->getDetailedDescription(gt.rules);

			gt.rules.removeSpaceConstraint(ctr);

			gt.rules.addUndoPoint(tr("Removed the constraint:\n\n%1").arg(oc));

			visibleConstraintsList.removeAt(i);
			constraintsListWidget->setCurrentRow(-1);
			item=constraintsListWidget->takeItem(i);
			delete item;
		}
	}
	//else if(lres==1){
		//The user clicked the Cancel button or pressed Escape
	//}
	
	if(i>=constraintsListWidget->count())
		i=constraintsListWidget->count()-1;
	if(i>=0)
		constraintsListWidget->setCurrentRow(i);
	else
		this->constraintChanged(-1);
}
