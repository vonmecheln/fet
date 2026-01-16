/***************************************************************************
                          addroomform.cpp  -  description
                             -------------------
    begin                : Sun Jan 1 2004
    copyright            : (C) 2004 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "addroomform.h"

#include "longtextmessagebox.h"

#include <QMessageBox>

AddRoomForm::AddRoomForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	addRoomPushButton->setDefault(true);

	connect(closePushButton, &QPushButton::clicked, this, &AddRoomForm::close);
	connect(helpPushButton, &QPushButton::clicked, this, &AddRoomForm::help);
	connect(addRoomPushButton, &QPushButton::clicked, this, &AddRoomForm::addRoom);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp5=buildingsComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	
	buildingsComboBox->clear();
	buildingsComboBox->addItem("");
	for(int i=0; i<gt.rules.buildingsList.size(); i++)
		buildingsComboBox->addItem(gt.rules.buildingsList.at(i)->name);
	
	capacitySpinBox->setMinimum(1);
	capacitySpinBox->setMaximum(MAX_ROOM_CAPACITY);
	capacitySpinBox->setValue(MAX_ROOM_CAPACITY);
}

AddRoomForm::~AddRoomForm()
{
	saveFETDialogGeometry(this);
}

void AddRoomForm::addRoom()
{
	if(nameLineEdit->text().isEmpty()){
		QMessageBox::information(this, tr("FET information"), tr("Incorrect name"));
		return;
	}
	if(buildingsComboBox->currentIndex()<0){
		QMessageBox::information(this, tr("FET information"), tr("Incorrect building"));
		return;
	}
	Room* rm=new Room();
	rm->name=nameLineEdit->text();
	rm->building=buildingsComboBox->currentText();
	rm->capacity=capacitySpinBox->value();
	if(!gt.rules.addRoom(rm)){
		QMessageBox::information(this, tr("Room insertion dialog"),
			tr("Could not insert item. Must be a duplicate"));
		delete rm;
	}
	else{
		QMessageBox::information(this, tr("Room insertion dialog"),
			tr("Room added"));
			
		gt.rules.addUndoPoint(tr("Added the room %1.").arg(rm->name));
	}

	nameLineEdit->selectAll();
	nameLineEdit->setFocus();
}

void AddRoomForm::help()
{
	QString s;
	
	s=tr("It is advisable to generate the timetable without the rooms (or without rooms' constraints), then, if a solution is possible, to add rooms or rooms' constraints");
	
	s+="\n\n";
	
	s+=tr("Please note that each room can hold a single activity at a specified period. If you"
	 " have a very large room, which can hold more activities at one time, please add more rooms,"
	 " representing this larger room");
	
	LongTextMessageBox::largeInformation(this, tr("FET - help on adding room(s)"), s);
}
