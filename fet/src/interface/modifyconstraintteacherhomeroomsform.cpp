/***************************************************************************
                          modifyconstraintteacherhomeroomform.cpp  -  description
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

#include "modifyconstraintteacherhomeroomsform.h"

#include <QListWidget>
#include <QAbstractItemView>

ModifyConstraintTeacherHomeRoomsForm::ModifyConstraintTeacherHomeRoomsForm(QWidget* parent, ConstraintTeacherHomeRooms* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);
	
	roomsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	selectedRoomsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyConstraintTeacherHomeRoomsForm::cancel);
	connect(okPushButton, &QPushButton::clicked, this, &ModifyConstraintTeacherHomeRoomsForm::ok);
	connect(roomsListWidget, &QListWidget::itemDoubleClicked, this, &ModifyConstraintTeacherHomeRoomsForm::addRoom);
	connect(selectedRoomsListWidget, &QListWidget::itemDoubleClicked, this, &ModifyConstraintTeacherHomeRoomsForm::removeRoom);
	connect(clearPushButton, &QPushButton::clicked, this, &ModifyConstraintTeacherHomeRoomsForm::clear);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	
	updateRoomsListWidget();
	
	this->_ctr=ctr;
	
	///////teacher
	teachersComboBox->clear();
	int i=0, j=-1;
	for(int k=0; k<gt.rules.teachersList.size(); k++, i++){
		Teacher* tch=gt.rules.teachersList[k];
			teachersComboBox->addItem(tch->name);
				if(tch->name==this->_ctr->teacherName)
					j=i;
	}
	assert(j>=0);
	teachersComboBox->setCurrentIndex(j);
	///////////////
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	for(QStringList::const_iterator it=ctr->roomsNames.constBegin(); it!=ctr->roomsNames.constEnd(); it++)
		selectedRoomsListWidget->addItem(*it);
}

ModifyConstraintTeacherHomeRoomsForm::~ModifyConstraintTeacherHomeRoomsForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintTeacherHomeRoomsForm::updateRoomsListWidget()
{
	roomsListWidget->clear();
	selectedRoomsListWidget->clear();

	for(int i=0; i<gt.rules.roomsList.size(); i++){
		Room* rm=gt.rules.roomsList[i];
		roomsListWidget->addItem(rm->name);
	}
}

void ModifyConstraintTeacherHomeRoomsForm::ok()
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
			tr("Only one selected room - please use constraint teacher home room if you want a single room"));
		return;
	}*/

	QString teacher=teachersComboBox->currentText();
	assert(gt.rules.searchTeacher(teacher)>=0);

	QStringList roomsList;
	for(int i=0; i<selectedRoomsListWidget->count(); i++)
		roomsList.append(selectedRoomsListWidget->item(i)->text());

	QString oldcs=this->_ctr->getDetailedDescription(gt.rules);

	this->_ctr->weightPercentage=weight;

	this->_ctr->teacherName=teacher;

	this->_ctr->roomsNames=roomsList;
	
	QString newcs=this->_ctr->getDetailedDescription(gt.rules);
	gt.rules.addUndoPoint(tr("Modified the constraint:\n\n%1\ninto\n\n%2").arg(oldcs).arg(newcs));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintTeacherHomeRoomsForm::cancel()
{
	this->close();
}

void ModifyConstraintTeacherHomeRoomsForm::addRoom()
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

void ModifyConstraintTeacherHomeRoomsForm::removeRoom()
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

void ModifyConstraintTeacherHomeRoomsForm::clear()
{
	selectedRoomsListWidget->clear();
}
