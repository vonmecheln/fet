/***************************************************************************
                          modifystudentsgroupform.cpp  -  description
                             -------------------
    begin                : Feb 8, 2005
    copyright            : (C) 2005 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include <QMessageBox>

#include "modifystudentsgroupform.h"

ModifyStudentsGroupForm::ModifyStudentsGroupForm(QWidget* parent, const QString& yearName, const QString& initialGroupName, int initialNumberOfStudents): QDialog(parent)
{
	setupUi(this);
	
	okPushButton->setDefault(true);

	connect(okPushButton, &QPushButton::clicked, this, &ModifyStudentsGroupForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyStudentsGroupForm::cancel);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	numberSpinBox->setMaximum(MAX_ROOM_CAPACITY);
	numberSpinBox->setMinimum(0);
	numberSpinBox->setValue(0);
	
	//this->_yearName=yearName;
	this->_initialGroupName=initialGroupName;
	this->_initialNumberOfStudents=initialNumberOfStudents;
	numberSpinBox->setValue(initialNumberOfStudents);
	yearNameLineEdit->setText(yearName);
	nameLineEdit->setText(initialGroupName);
	nameLineEdit->selectAll();
	nameLineEdit->setFocus();
}

ModifyStudentsGroupForm::~ModifyStudentsGroupForm()
{
	saveFETDialogGeometry(this);
}

void ModifyStudentsGroupForm::cancel()
{
	this->close();
}

void ModifyStudentsGroupForm::ok()
{
	if(nameLineEdit->text().isEmpty()){
		QMessageBox::information(this, tr("FET information"), tr("Incorrect name"));
		return;
	}
	//QString yearName=yearNameLineEdit->text();
	QString groupName=nameLineEdit->text();
	
	if(this->_initialGroupName!=groupName && gt.rules.searchStudentsSet(groupName)!=nullptr){
		QMessageBox::information(this, tr("FET information"),
		 tr("Name exists. If you would like to make more years to contain a group (overlapping years),"
		 " please remove current group (FET will unfortunately remove all related activities and constraints)"
		 " and add a new group with desired name in current year."
		 " I know this is not an elegant procedure, I'll try to fix that in the future."));

		nameLineEdit->selectAll();
		nameLineEdit->setFocus();
		
		return;
	}
	
	QString od=tr("Group name=%1\nNumber of students=%2").arg(this->_initialGroupName).arg(this->_initialNumberOfStudents);
	
	bool t=gt.rules.modifyStudentsSet(this->_initialGroupName, groupName, numberSpinBox->value());
	assert(t);

	QString nd=tr("Group name=%1\nNumber of students=%2").arg(groupName).arg(numberSpinBox->value());
	gt.rules.addUndoPoint(tr("The group with the description:\n\n%1\nwas modified into\n\n%2").arg(od).arg(nd));
	
	this->close();
}
