/***************************************************************************
                          addequipmentform.cpp  -  description
                             -------------------
    begin                : Thu Jan 29 2004
    copyright            : (C) 2004 by Lalescu Liviu
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

#include "addequipmentform.h"

#include <qlineedit.h>

#include <QDesktopWidget>

AddEquipmentForm::AddEquipmentForm()
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);
}

AddEquipmentForm::~AddEquipmentForm()
{
}

void AddEquipmentForm::addEquipment()
{
	if(nameLineEdit->text().isEmpty()){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Incorrect name"));
		return;
	}
	Equipment* eq=new Equipment();
	eq->name=nameLineEdit->text();
	if(!gt.rules.addEquipment(eq)){
		QMessageBox::information(this, QObject::tr("Equipment insertion dialog"),
			QObject::tr("Could not insert item. Must be a duplicate"));
		delete eq;
	}
	else{
		QMessageBox::information(this, QObject::tr("Equipment insertion dialog"),
			QObject::tr("Equipment added"));
	}

	nameLineEdit->selectAll();
	nameLineEdit->setFocus();
}
