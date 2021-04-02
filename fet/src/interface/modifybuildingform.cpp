/***************************************************************************
                          modifybuildingform.cpp  -  description
                             -------------------
    begin                : Feb 11, 2008
    copyright            : (C) 2008 by Lalescu Liviu
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

#include "modifybuildingform.h"

#include <qlineedit.h>
#include <q3combobox.h>

#include <QDesktopWidget>

#include <QMessageBox>

ModifyBuildingForm::ModifyBuildingForm(const QString& initialBuildingName)
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	this->_initialBuildingName=initialBuildingName;

	nameLineEdit->setText(initialBuildingName);
	nameLineEdit->selectAll();
	nameLineEdit->setFocus();
}

ModifyBuildingForm::~ModifyBuildingForm()
{
}

void ModifyBuildingForm::cancel()
{
	this->close();
}

void ModifyBuildingForm::ok()
{
	if(nameLineEdit->text().isEmpty()){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Incorrect name"));
		return;
	}
	if(this->_initialBuildingName!=nameLineEdit->text() && gt.rules.searchBuilding(nameLineEdit->text())>=0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Name existing - please choose another"));
		return;
	}
	
	bool t=gt.rules.modifyBuilding(this->_initialBuildingName, nameLineEdit->text());
	assert(t);
	
	this->close();
}
