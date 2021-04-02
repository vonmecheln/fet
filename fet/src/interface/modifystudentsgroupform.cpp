/***************************************************************************
                          modifystudentsgroupform.cpp  -  description
                             -------------------
    begin                : Feb 8, 2005
    copyright            : (C) 2005 by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "modifystudentsgroupform.h"

#include <qlineedit.h>

#include <QDesktopWidget>

#include <QMessageBox>

ModifyStudentsGroupForm::ModifyStudentsGroupForm(const QString& yearName, const QString& initialGroupName, int initialNumberOfStudents)
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	numberSpinBox->setMaxValue(MAX_ROOM_CAPACITY);
	numberSpinBox->setMinValue(0);
	numberSpinBox->setValue(0);
				
	this->_yearName=yearName;
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
}

void ModifyStudentsGroupForm::cancel()
{
	this->close();
}

void ModifyStudentsGroupForm::ok()
{
	if(nameLineEdit->text().isEmpty()){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Incorrect name"));
		return;
	}
	QString yearName=yearNameLineEdit->text();
	QString groupName=nameLineEdit->text();
	
	if(this->_initialGroupName!=groupName && gt.rules.searchStudentsSet(groupName)!=NULL){
		QMessageBox::information(this, QObject::tr("FET information"), 
		 QObject::tr("Name exists. If you would like to make more years to contain a group (overlapping years),"
		 " please remove current group (FET will unfortunately remove all related activities and constraints)"
		 " and add a new group with desired name in current year."
		 " I know this is a not an elegant procedure, I'll try to fix that in the future."));
		return;
	}
	
	bool t=gt.rules.modifyGroup(this->_yearName, this->_initialGroupName, groupName, numberSpinBox->value());	
	assert(t);
	
	this->close();
}
