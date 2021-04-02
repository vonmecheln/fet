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

ModifyStudentsGroupForm::ModifyStudentsGroupForm(const QString& yearName, const QString& initialGroupName, int initialNumberOfStudents)
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

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
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Name existing - please choose another"));
		return;
	}
	
	bool t=gt.rules.modifyGroup(this->_yearName, this->_initialGroupName, groupName, numberSpinBox->value());	
	assert(t);
	
	this->close();
}
