/***************************************************************************
                          modifyconstraintactivitypreferredroomsform.cpp  -  description
                             -------------------
    begin                : March 28, 2005
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

#include "modifyconstraintactivitypreferredroomsform.h"

#include <QListWidget>
#include <QAbstractItemView>

ModifyConstraintActivityPreferredRoomsForm::ModifyConstraintActivityPreferredRoomsForm(QWidget* parent, ConstraintActivityPreferredRooms* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);
	
	roomsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	selectedRoomsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyConstraintActivityPreferredRoomsForm::cancel);
	connect(okPushButton, &QPushButton::clicked, this, &ModifyConstraintActivityPreferredRoomsForm::ok);
	connect(roomsListWidget, &QListWidget::itemDoubleClicked, this, &ModifyConstraintActivityPreferredRoomsForm::addRoom);
	connect(selectedRoomsListWidget, &QListWidget::itemDoubleClicked, this, &ModifyConstraintActivityPreferredRoomsForm::removeRoom);
	connect(clearPushButton, &QPushButton::clicked, this, &ModifyConstraintActivityPreferredRoomsForm::clear);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp5=activitiesComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	
	activitiesComboBox->setMaximumWidth(maxRecommendedWidth(this));
	
	updateRoomsListWidget();

	int j=-1, i=0;
	for(int k=0; k<gt.rules.activitiesList.size(); k++){
		Activity* ac=gt.rules.activitiesList[k];
		activitiesComboBox->addItem(ac->getDescription(gt.rules));
		if(ac->id==ctr->activityId){
			assert(j==-1);
			j=i;
		}
		i++;
	}
	assert(j>=0);
	activitiesComboBox->setCurrentIndex(j);
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	for(QStringList::const_iterator it=ctr->roomsNames.constBegin(); it!=ctr->roomsNames.constEnd(); it++)
		selectedRoomsListWidget->addItem(*it);
}

ModifyConstraintActivityPreferredRoomsForm::~ModifyConstraintActivityPreferredRoomsForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintActivityPreferredRoomsForm::updateRoomsListWidget()
{
	roomsListWidget->clear();
	selectedRoomsListWidget->clear();

	for(int i=0; i<gt.rules.roomsList.size(); i++){
		Room* rm=gt.rules.roomsList[i];
		roomsListWidget->addItem(rm->name);
	}
}

void ModifyConstraintActivityPreferredRoomsForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight"));
		return;
	}

	if(selectedRoomsListWidget->count()==0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Empty list of selected rooms"));
		return;
	}
	/*if(selectedRoomsListWidget->count()==1){
		QMessageBox::warning(this, tr("FET information"),
			tr("Only one selected room - please use constraint activity preferred room if you want a single room"));
		return;
	}*/
	
	if(activitiesComboBox->currentIndex()<0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid selected activity"));
		return;
	}

	QString oldcs=this->_ctr->getDetailedDescription(gt.rules);

	int id=gt.rules.activitiesList.at(activitiesComboBox->currentIndex())->id;
	
	QStringList roomsList;
	for(int i=0; i<selectedRoomsListWidget->count(); i++)
		roomsList.append(selectedRoomsListWidget->item(i)->text());
	
	this->_ctr->weightPercentage=weight;
	this->_ctr->activityId=id;
	this->_ctr->roomsNames=roomsList;
	
	QString newcs=this->_ctr->getDetailedDescription(gt.rules);
	gt.rules.addUndoPoint(tr("Modified the constraint:\n\n%1\ninto\n\n%2").arg(oldcs).arg(newcs));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintActivityPreferredRoomsForm::cancel()
{
	this->close();
}

void ModifyConstraintActivityPreferredRoomsForm::addRoom()
{
	if(roomsListWidget->currentRow()<0)
		return;
	QString rmName=roomsListWidget->currentItem()->text();
	assert(rmName!="");
	int i;
	//duplicate?
	for(i=0; i<selectedRoomsListWidget->count(); i++)
		if(rmName==selectedRoomsListWidget->item(i)->text())
			break;
	if(i<selectedRoomsListWidget->count())
		return;
	selectedRoomsListWidget->addItem(rmName);
	selectedRoomsListWidget->setCurrentRow(selectedRoomsListWidget->count()-1);
}

void ModifyConstraintActivityPreferredRoomsForm::removeRoom()
{
	if(selectedRoomsListWidget->currentRow()<0 || selectedRoomsListWidget->count()<=0)
		return;
	int tmp=selectedRoomsListWidget->currentRow();
	
	selectedRoomsListWidget->setCurrentRow(-1);
	QListWidgetItem* item=selectedRoomsListWidget->takeItem(tmp);
	delete item;
	if(tmp<selectedRoomsListWidget->count())
		selectedRoomsListWidget->setCurrentRow(tmp);
	else
		selectedRoomsListWidget->setCurrentRow(selectedRoomsListWidget->count()-1);
}

void ModifyConstraintActivityPreferredRoomsForm::clear()
{
	selectedRoomsListWidget->clear();
}
