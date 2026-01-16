/***************************************************************************
                          modifybuildingform.cpp  -  description
                             -------------------
    begin                : Feb 11, 2008
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

#include "modifybuildingform.h"

#include <QMessageBox>

ModifyBuildingForm::ModifyBuildingForm(QWidget* parent, const QString& initialBuildingName): QDialog(parent)
{
	setupUi(this);
	
	okPushButton->setDefault(true);

	connect(okPushButton, &QPushButton::clicked, this, &ModifyBuildingForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyBuildingForm::cancel);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	this->_initialBuildingName=initialBuildingName;

	nameLineEdit->setText(initialBuildingName);
	nameLineEdit->selectAll();
	nameLineEdit->setFocus();
}

ModifyBuildingForm::~ModifyBuildingForm()
{
	saveFETDialogGeometry(this);
}

void ModifyBuildingForm::cancel()
{
	this->close();
}

void ModifyBuildingForm::ok()
{
	if(nameLineEdit->text().isEmpty()){
		QMessageBox::information(this, tr("FET information"), tr("Incorrect name"));
		return;
	}
	if(this->_initialBuildingName!=nameLineEdit->text() && gt.rules.searchBuilding(nameLineEdit->text())>=0){
		QMessageBox::information(this, tr("FET information"), tr("Name existing - please choose another"));
		return;
	}
	
	bool t=gt.rules.modifyBuilding(this->_initialBuildingName, nameLineEdit->text());
	assert(t);
	
	gt.rules.addUndoPoint(tr("Modified the building %1 to %2.").arg(this->_initialBuildingName).arg(nameLineEdit->text()));
	
	this->close();
}
