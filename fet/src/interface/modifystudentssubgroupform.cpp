/***************************************************************************
                          modifystudentssubgroupform.cpp  -  description
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

#include "modifystudentssubgroupform.h"

#include <qlineedit.h>

#include <QDesktopWidget>

#include <QMessageBox>

ModifyStudentsSubgroupForm::ModifyStudentsSubgroupForm(const QString& yearName, const QString& groupName, const QString& initialSubgroupName, int initialNumberOfStudents)
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	this->_yearName=yearName;
	this->_groupName=groupName;
	this->_initialSubgroupName=initialSubgroupName;
	
	numberSpinBox->setValue(initialNumberOfStudents);
	
	yearNameLineEdit->setText(yearName);
	groupNameLineEdit->setText(groupName);
	nameLineEdit->setText(initialSubgroupName);
	nameLineEdit->selectAll();
	nameLineEdit->setFocus();
}

ModifyStudentsSubgroupForm::~ModifyStudentsSubgroupForm()
{
}

void ModifyStudentsSubgroupForm::cancel()
{
	this->close();
}

void ModifyStudentsSubgroupForm::ok()
{
	if(nameLineEdit->text().isEmpty()){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Incorrect name"));
		return;
	}
	QString subgroupName=nameLineEdit->text();
	QString yearName=yearNameLineEdit->text();
	QString groupName=groupNameLineEdit->text();
	
	if(this->_initialSubgroupName!=subgroupName && gt.rules.searchStudentsSet(subgroupName)!=NULL){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Name existing - please choose another"));
		return;
	}

	bool t=gt.rules.modifySubgroup(this->_yearName, this->_groupName, this->_initialSubgroupName, subgroupName, numberSpinBox->value());
	assert(t);
	
	this->close();
}
