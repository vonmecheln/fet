/***************************************************************************
                          addconstraintactivitytagsnotoverlappingform.cpp  -  description
                             -------------------
    begin                : 2019
    copyright            : (C) 2019 by Liviu Lalescu
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

#include "addconstraintactivitytagsnotoverlappingform.h"

#include <QListWidget>
#include <QAbstractItemView>
#include <QScrollBar>

AddConstraintActivityTagsNotOverlappingForm::AddConstraintActivityTagsNotOverlappingForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	activityTagsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	notOverlappingActivityTagsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(addConstraintPushButton, SIGNAL(clicked()), this, SLOT(addConstraint()));
	connect(activityTagsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addActivityTag()));
	connect(addAllActivityTagsPushButton, SIGNAL(clicked()), this, SLOT(addAllActivityTags()));
	connect(notOverlappingActivityTagsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(removeActivityTag()));
	connect(clearPushButton, SIGNAL(clicked()), this, SLOT(clear()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* st=gt.rules.activityTagsList[i];
		activityTagsListWidget->addItem(st->name);
	}

	notOverlappingActivityTagsListWidget->clear();
}

AddConstraintActivityTagsNotOverlappingForm::~AddConstraintActivityTagsNotOverlappingForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintActivityTagsNotOverlappingForm::addConstraint()
{
	TimeConstraint *ctr=nullptr;

	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage)"));
		return;
	}

	if(notOverlappingActivityTagsListWidget->count()==0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Empty list of not overlapping activity tags"));
		return;
	}
	if(notOverlappingActivityTagsListWidget->count()==1){
		QMessageBox::warning(this, tr("FET information"),
			tr("Only one selected activity tag"));
		return;
	}
	
	QStringList atl;
	for(int i=0; i<notOverlappingActivityTagsListWidget->count(); i++)
		atl.append(notOverlappingActivityTagsListWidget->item(i)->text());
	
	ctr=new ConstraintActivityTagsNotOverlapping(weight, atl);
	bool tmp2=gt.rules.addTimeConstraint(ctr);
	
	if(tmp2){
		QString s=tr("Constraint added:");
		s+="\n\n";
		s+=ctr->getDetailedDescription(gt.rules);
		LongTextMessageBox::information(this, tr("FET information"), s);

		gt.rules.addUndoPoint(tr("Added the constraint:\n\n%1").arg(ctr->getDetailedDescription(gt.rules)));
	}
	else{
		QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - please report error"));
		delete ctr;
	}
}

void AddConstraintActivityTagsNotOverlappingForm::addActivityTag()
{
	if(activityTagsListWidget->currentRow()<0)
		return;
	
	QString actTagName=activityTagsListWidget->currentItem()->text();
	assert(actTagName!="");
	int i;
	//duplicate?
	for(i=0; i<notOverlappingActivityTagsListWidget->count(); i++)
		if(actTagName==notOverlappingActivityTagsListWidget->item(i)->text())
			break;
	if(i<notOverlappingActivityTagsListWidget->count())
		return;
	notOverlappingActivityTagsListWidget->addItem(actTagName);
	notOverlappingActivityTagsListWidget->setCurrentRow(notOverlappingActivityTagsListWidget->count()-1);
}

void AddConstraintActivityTagsNotOverlappingForm::addAllActivityTags()
{
	for(int tmp=0; tmp<activityTagsListWidget->count(); tmp++){
		QString actTagName=activityTagsListWidget->item(tmp)->text();
		assert(actTagName!="");
		int i;
		//duplicate?
		for(i=0; i<notOverlappingActivityTagsListWidget->count(); i++)
			if(notOverlappingActivityTagsListWidget->item(i)->text()==actTagName)
				break;
		if(i<notOverlappingActivityTagsListWidget->count())
			continue;
		
		notOverlappingActivityTagsListWidget->addItem(actTagName);
	}
	
	notOverlappingActivityTagsListWidget->setCurrentRow(notOverlappingActivityTagsListWidget->count()-1);
}

void AddConstraintActivityTagsNotOverlappingForm::removeActivityTag()
{
	if(notOverlappingActivityTagsListWidget->currentRow()<0 || notOverlappingActivityTagsListWidget->count()<=0)
		return;
	int tmp=notOverlappingActivityTagsListWidget->currentRow();
	
	notOverlappingActivityTagsListWidget->setCurrentRow(-1);
	QListWidgetItem* item=notOverlappingActivityTagsListWidget->takeItem(tmp);
	delete item;
	if(tmp<notOverlappingActivityTagsListWidget->count())
		notOverlappingActivityTagsListWidget->setCurrentRow(tmp);
	else
		notOverlappingActivityTagsListWidget->setCurrentRow(notOverlappingActivityTagsListWidget->count()-1);
}

void AddConstraintActivityTagsNotOverlappingForm::clear()
{
	notOverlappingActivityTagsListWidget->clear();
}
