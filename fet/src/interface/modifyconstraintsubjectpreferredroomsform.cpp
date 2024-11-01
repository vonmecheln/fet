/***************************************************************************
                          modifyconstraintsubjectpreferredroomsform.cpp  -  description
                             -------------------
    begin                : April 8, 2005
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

#include "modifyconstraintsubjectpreferredroomsform.h"

#include <QListWidget>
#include <QAbstractItemView>

ModifyConstraintSubjectPreferredRoomsForm::ModifyConstraintSubjectPreferredRoomsForm(QWidget* parent, ConstraintSubjectPreferredRooms* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	roomsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	selectedRoomsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyConstraintSubjectPreferredRoomsForm::cancel);
	connect(okPushButton, &QPushButton::clicked, this, &ModifyConstraintSubjectPreferredRoomsForm::ok);
	connect(roomsListWidget, &QListWidget::itemDoubleClicked, this, &ModifyConstraintSubjectPreferredRoomsForm::addRoom);
	connect(selectedRoomsListWidget, &QListWidget::itemDoubleClicked, this, &ModifyConstraintSubjectPreferredRoomsForm::removeRoom);
	connect(clearPushButton, &QPushButton::clicked, this, &ModifyConstraintSubjectPreferredRoomsForm::clear);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp3=subjectsComboBox->minimumSizeHint();
	Q_UNUSED(tmp3);
	
	updateRoomsListWidget();
	
	int i=0, j=-1;
	for(int k=0; k<gt.rules.subjectsList.size(); k++){
		Subject* sb=gt.rules.subjectsList[k];
		subjectsComboBox->addItem(sb->name);
		if(ctr->subjectName==sb->name){
			assert(j==-1);
			j=i;
		}
		i++;
	}
	assert(j>=0);
	subjectsComboBox->setCurrentIndex(j);
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	for(QStringList::const_iterator it=ctr->roomsNames.constBegin(); it!=ctr->roomsNames.constEnd(); it++)
		selectedRoomsListWidget->addItem(*it);
}

ModifyConstraintSubjectPreferredRoomsForm::~ModifyConstraintSubjectPreferredRoomsForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintSubjectPreferredRoomsForm::updateRoomsListWidget()
{
	roomsListWidget->clear();
	selectedRoomsListWidget->clear();

	for(int i=0; i<gt.rules.roomsList.size(); i++){
		Room* rm=gt.rules.roomsList[i];
		roomsListWidget->addItem(rm->name);
	}
}

void ModifyConstraintSubjectPreferredRoomsForm::ok()
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
			tr("Only one selected room - please use constraint subject preferred room if you want a single room"));
		return;
	}*/
	
	if(subjectsComboBox->currentIndex()<0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid selected subject"));
		return;
	}
	QString subject=subjectsComboBox->currentText();
	
	QStringList roomsList;
	for(int i=0; i<selectedRoomsListWidget->count(); i++)
		roomsList.append(selectedRoomsListWidget->item(i)->text());

	QString oldcs=this->_ctr->getDetailedDescription(gt.rules);

	this->_ctr->weightPercentage=weight;
	this->_ctr->subjectName=subject;
	this->_ctr->roomsNames=roomsList;
	
	QString newcs=this->_ctr->getDetailedDescription(gt.rules);
	gt.rules.addUndoPoint(tr("Modified the constraint:\n\n%1\ninto\n\n%2").arg(oldcs).arg(newcs));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintSubjectPreferredRoomsForm::cancel()
{
	this->close();
}

void ModifyConstraintSubjectPreferredRoomsForm::addRoom()
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

void ModifyConstraintSubjectPreferredRoomsForm::removeRoom()
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

void ModifyConstraintSubjectPreferredRoomsForm::clear()
{
	selectedRoomsListWidget->clear();
}
