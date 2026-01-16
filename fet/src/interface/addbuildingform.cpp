/***************************************************************************
                          addbuildingform.cpp  -  description
                             -------------------
    begin                : Feb 11 2008
    copyright            : (C) 2008 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "addbuildingform.h"

#include <QMessageBox>

AddBuildingForm::AddBuildingForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	addBuildingPushButton->setDefault(true);

	connect(addBuildingPushButton, &QPushButton::clicked, this, &AddBuildingForm::addBuilding);
	connect(closePushButton, &QPushButton::clicked, this, &AddBuildingForm::close);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
}

AddBuildingForm::~AddBuildingForm()
{
	saveFETDialogGeometry(this);
}

void AddBuildingForm::addBuilding()
{
	if(nameLineEdit->text().isEmpty()){
		QMessageBox::information(this, tr("FET information"), tr("Incorrect name"));
		return;
	}
	Building* bu=new Building();
	bu->name=nameLineEdit->text();
	if(!gt.rules.addBuilding(bu)){
		QMessageBox::information(this, tr("Room insertion dialog"),
			tr("Could not insert item. Must be a duplicate"));
		delete bu;
	}
	else{
		QMessageBox::information(this, tr("Building insertion dialog"),
			tr("Building added"));
			
		gt.rules.addUndoPoint(tr("Added the building %1.").arg(bu->name));
	}

	nameLineEdit->selectAll();
	nameLineEdit->setFocus();
}
