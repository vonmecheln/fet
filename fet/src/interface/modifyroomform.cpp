/***************************************************************************
                          modifyroomform.cpp  -  description
                             -------------------
    begin                : Feb 12, 2005
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

#include "modifyroomform.h"

#include <QMessageBox>

ModifyRoomForm::ModifyRoomForm(QWidget* parent, const QString& initialRoomName, const QString& initialRoomBuilding, int initialRoomCapacity): QDialog(parent)
{
	setupUi(this);
	
	okPushButton->setDefault(true);

	connect(okPushButton, &QPushButton::clicked, this, &ModifyRoomForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyRoomForm::cancel);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp5=buildingsComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	
	capacitySpinBox->setMinimum(1);
	capacitySpinBox->setMaximum(MAX_ROOM_CAPACITY);
	capacitySpinBox->setValue(MAX_ROOM_CAPACITY);

	this->_initialRoomName=initialRoomName;
	this->_initialRoomBuilding=initialRoomBuilding;
	this->_initialRoomCapacity=initialRoomCapacity;
	capacitySpinBox->setValue(initialRoomCapacity);

	nameLineEdit->setText(initialRoomName);
	nameLineEdit->selectAll();
	nameLineEdit->setFocus();
	
	int i=0;
	int j=-1;
	buildingsComboBox->clear();
	buildingsComboBox->addItem("");
	if(initialRoomBuilding=="")
		j=i;
	i++;
	for(int k=0; k<gt.rules.buildingsList.size(); k++, i++){
		buildingsComboBox->addItem(gt.rules.buildingsList.at(k)->name);
		if(gt.rules.buildingsList.at(k)->name==initialRoomBuilding)
			j=i;
	}
	assert(j>=0);
	buildingsComboBox->setCurrentIndex(j);
}

ModifyRoomForm::~ModifyRoomForm()
{
	saveFETDialogGeometry(this);
}

void ModifyRoomForm::cancel()
{
	this->close();
}

void ModifyRoomForm::ok()
{
	if(nameLineEdit->text().isEmpty()){
		QMessageBox::information(this, tr("FET information"), tr("Incorrect name"));
		return;
	}
	if(buildingsComboBox->currentIndex()<0){
		QMessageBox::information(this, tr("FET information"), tr("Incorrect building"));
		return;
	}
	if(this->_initialRoomName!=nameLineEdit->text() && gt.rules.searchRoom(nameLineEdit->text())>=0){
		QMessageBox::information(this, tr("FET information"), tr("Name existing - please choose another"));
		return;
	}
	
	QString od=tr("Name=%1\nBuilding=%2\nCapacity=%3").arg(this->_initialRoomName).arg(this->_initialRoomBuilding).arg(this->_initialRoomCapacity)+QString("\n");
	
	bool t=gt.rules.modifyRoom(this->_initialRoomName, nameLineEdit->text(), buildingsComboBox->currentText(), capacitySpinBox->value());
	assert(t);

	QString nd=tr("Name=%1\nBuilding=%2\nCapacity=%3").arg(nameLineEdit->text()).arg(buildingsComboBox->currentText()).arg(capacitySpinBox->value())+QString("\n");
	gt.rules.addUndoPoint(tr("The room with description:\n\n%1\nwas modified into\n\n%2").arg(od).arg(nd));
	
	this->close();
}
