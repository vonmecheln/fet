/***************************************************************************
                          modifystudentssubgroupform.cpp  -  description
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

#include "modifystudentssubgroupform.h"

ModifyStudentsSubgroupForm::ModifyStudentsSubgroupForm(QWidget* parent, const QString& yearName, const QString& groupName, const QString& initialSubgroupName, int initialNumberOfStudents): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, &QPushButton::clicked, this, &ModifyStudentsSubgroupForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyStudentsSubgroupForm::cancel);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	numberSpinBox->setMaximum(MAX_ROOM_CAPACITY);
	numberSpinBox->setMinimum(0);
	numberSpinBox->setValue(0);

//	this->_yearName=yearName;
//	this->_groupName=groupName;
	this->_initialSubgroupName=initialSubgroupName;
	this->_initialNumberOfStudents=initialNumberOfStudents;

	numberSpinBox->setValue(initialNumberOfStudents);
	
	yearNameLineEdit->setText(yearName);
	groupNameLineEdit->setText(groupName);
	nameLineEdit->setText(initialSubgroupName);
	nameLineEdit->selectAll();
	nameLineEdit->setFocus();
}

ModifyStudentsSubgroupForm::~ModifyStudentsSubgroupForm()
{
	saveFETDialogGeometry(this);
}

void ModifyStudentsSubgroupForm::cancel()
{
	this->close();
}

void ModifyStudentsSubgroupForm::ok()
{
	if(nameLineEdit->text().isEmpty()){
		QMessageBox::information(this, tr("FET information"), tr("Incorrect name"));
		return;
	}
	QString subgroupName=nameLineEdit->text();
	//QString yearName=yearNameLineEdit->text();
	//QString groupName=groupNameLineEdit->text();
	
	if(this->_initialSubgroupName!=subgroupName && gt.rules.searchStudentsSet(subgroupName)!=nullptr){
		QMessageBox::information(this, tr("FET information"),
		 tr("Name exists. If you would like to make more groups to contain a subgroup (overlapping groups),"
 		 " please remove current subgroup (FET will unfortunately remove all related activities and constraints)"
		 " and add a new subgroup with desired name in current group."
 		 " I know this is not an elegant procedure, I'll try to fix that in the future."));
		
		nameLineEdit->selectAll();
		nameLineEdit->setFocus();

		return;
	}

	QString od=tr("Subgroup name=%1\nNumber of students=%2").arg(this->_initialSubgroupName).arg(this->_initialNumberOfStudents);

	bool t=gt.rules.modifyStudentsSet(this->_initialSubgroupName, subgroupName, numberSpinBox->value());
	assert(t);

	QString nd=tr("Subgroup name=%1\nNumber of students=%2").arg(subgroupName).arg(numberSpinBox->value());
	gt.rules.addUndoPoint(tr("The subgroup with description:\n\n%1\nwas modified into\n\n%2").arg(od).arg(nd));

	this->close();
}
