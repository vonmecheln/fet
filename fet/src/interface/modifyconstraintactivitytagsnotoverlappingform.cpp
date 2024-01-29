/***************************************************************************
                          modifyconstraintactivitytagsnotoverlappingform.cpp  -  description
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

#include "modifyconstraintactivitytagsnotoverlappingform.h"
#include "fet.h"

#include <QListWidget>
#include <QAbstractItemView>
#include <QScrollBar>

ModifyConstraintActivityTagsNotOverlappingForm::ModifyConstraintActivityTagsNotOverlappingForm(QWidget* parent, ConstraintActivityTagsNotOverlapping* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	activityTagsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	notOverlappingActivityTagsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyConstraintActivityTagsNotOverlappingForm::cancel);
	connect(okPushButton, &QPushButton::clicked, this, &ModifyConstraintActivityTagsNotOverlappingForm::ok);
	connect(activityTagsListWidget, &QListWidget::itemDoubleClicked, this, &ModifyConstraintActivityTagsNotOverlappingForm::addActivityTag);
	connect(addAllActivityTagsPushButton, &QPushButton::clicked, this, &ModifyConstraintActivityTagsNotOverlappingForm::addAllActivityTags);
	connect(notOverlappingActivityTagsListWidget, &QListWidget::itemDoubleClicked, this, &ModifyConstraintActivityTagsNotOverlappingForm::removeActivityTag);
	connect(clearPushButton, &QPushButton::clicked, this, &ModifyConstraintActivityTagsNotOverlappingForm::clear);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	this->_ctr=ctr;
	
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* st=gt.rules.activityTagsList[i];
		activityTagsListWidget->addItem(st->name);
	}
	
	notOverlappingActivityTagsListWidget->clear();
	for(const QString& at : std::as_const(ctr->activityTagsNames))
		this->notOverlappingActivityTagsListWidget->addItem(at);
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
}

ModifyConstraintActivityTagsNotOverlappingForm::~ModifyConstraintActivityTagsNotOverlappingForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintActivityTagsNotOverlappingForm::ok()
{
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

	QString oldcs=this->_ctr->getDetailedDescription(gt.rules);

	QStringList atl;
	for(int i=0; i<notOverlappingActivityTagsListWidget->count(); i++)
		atl.append(notOverlappingActivityTagsListWidget->item(i)->text());

	this->_ctr->weightPercentage=weight;
	this->_ctr->activityTagsNames=atl;
	
	QString newcs=this->_ctr->getDetailedDescription(gt.rules);
	gt.rules.addUndoPoint(tr("Modified the constraint:\n\n%1\ninto\n\n%2").arg(oldcs).arg(newcs));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintActivityTagsNotOverlappingForm::cancel()
{
	this->close();
}

void ModifyConstraintActivityTagsNotOverlappingForm::addActivityTag()
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

void ModifyConstraintActivityTagsNotOverlappingForm::addAllActivityTags()
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

void ModifyConstraintActivityTagsNotOverlappingForm::removeActivityTag()
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

void ModifyConstraintActivityTagsNotOverlappingForm::clear()
{
	notOverlappingActivityTagsListWidget->clear();
}
