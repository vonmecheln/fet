/***************************************************************************
                          modifyroomform.cpp  -  description
                             -------------------
    begin                : Feb 12, 2005
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

#include "modifyroomform.h"

#include <qlineedit.h>
#include <q3combobox.h>

#include <QDesktopWidget>

ModifyRoomForm::ModifyRoomForm(const QString& initialRoomName, const QString& initialRoomType, const QString& initialRoomBuilding, int initialRoomCapacity)
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	this->_initialRoomName=initialRoomName;
	this->_initialRoomType=initialRoomType;
	this->_initialRoomBuilding=initialRoomBuilding;
	this->_initialRoomCapacity=initialRoomCapacity;
	capacitySpinBox->setValue(initialRoomCapacity);
	nameLineEdit->setText(initialRoomName);
	nameLineEdit->selectAll();
	nameLineEdit->setFocus();
	
	typesComboBox->clear();
	typesComboBox->setDuplicatesEnabled(false);
	int i=0, j=-1;
	for(int ri=0; ri<gt.rules.roomsList.size(); ri++){
		Room* rm=gt.rules.roomsList[ri];
		int k;
		for(k=0; k<typesComboBox->count(); k++)
			if(typesComboBox->text(k)==rm->type)
				break;
		if(k==typesComboBox->count()){
			typesComboBox->insertItem(rm->type);

			if(rm->type==initialRoomType)
				j=i;
				
			i++;
		}
	}
	assert(j>=0);
	typesComboBox->setCurrentItem(j);

	i=0;j=-1;
	buildingsComboBox->clear();
	buildingsComboBox->insertItem("");
	if(initialRoomBuilding=="")
		j=i;
	i++;
	for(int k=0; k<gt.rules.buildingsList.size(); k++, i++){
		buildingsComboBox->insertItem(gt.rules.buildingsList.at(i)->name);
		if(gt.rules.buildingsList.at(i)->name==initialRoomBuilding)
			j=i;
	}
	assert(j>=0);
	buildingsComboBox->setCurrentItem(j);
}

ModifyRoomForm::~ModifyRoomForm()
{
}

void ModifyRoomForm::cancel()
{
	this->close();
}

void ModifyRoomForm::ok()
{
	if(nameLineEdit->text().isEmpty()){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Incorrect name"));
		return;
	}
	if(typesComboBox->currentText().isEmpty()){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Incorrect type"));
		return;
	}
	if(buildingsComboBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Incorrect building"));
		return;
	}
	if(this->_initialRoomName!=nameLineEdit->text() && gt.rules.searchRoom(nameLineEdit->text())>=0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Name existing - please choose another"));
		return;
	}
	
	bool t=gt.rules.modifyRoom(this->_initialRoomName, nameLineEdit->text(), typesComboBox->currentText(), buildingsComboBox->currentText(), capacitySpinBox->value());
	assert(t);
	
	this->close();
}