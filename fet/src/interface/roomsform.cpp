//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
// Copyright (C) 2003 Liviu Lalescu <http://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
//
//

#include "timetable_defs.h"
#include "fet.h"
#include "roomsform.h"
#include "addroomform.h"
#include "modifyroomform.h"

#include <QInputDialog>

#include <QMessageBox>

RoomsForm::RoomsForm()
 : RoomsForm_template()
{
    setupUi(this);

    connect(addRoomPushButton, SIGNAL(clicked()), this /*RoomsForm_template*/, SLOT(addRoom()));
    connect(removeRoomPushButton, SIGNAL(clicked()), this /*RoomsForm_template*/, SLOT(removeRoom()));
    connect(roomsListBox, SIGNAL(highlighted(int)), this /*RoomsForm_template*/, SLOT(roomChanged(int)));
    connect(closePushButton, SIGNAL(clicked()), this /*RoomsForm_template*/, SLOT(close()));
    connect(modifyRoomPushButton, SIGNAL(clicked()), this /*RoomsForm_template*/, SLOT(modifyRoom()));
    connect(sortRoomsPushButton, SIGNAL(clicked()), this /*RoomsForm_template*/, SLOT(sortRooms()));
    connect(roomsListBox, SIGNAL(selected(QString)), this /*RoomsForm_template*/, SLOT(modifyRoom()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	this->filterChanged();
}


RoomsForm::~RoomsForm()
{
}

bool RoomsForm::filterOk(Room* rm)
{
	Q_UNUSED(rm);
	//if(rm!=NULL)
	//	;

	bool ok=true;

	return ok;
}

void RoomsForm::filterChanged()
{
	QString s;
	roomsListBox->clear();
	visibleRoomsList.clear();
	for(int i=0; i<gt.rules.roomsList.size(); i++){
		Room* rm=gt.rules.roomsList[i];
		if(this->filterOk(rm)){
			s=rm->getDescription();
			visibleRoomsList.append(rm);
			roomsListBox->insertItem(s);
		}
	}
	roomChanged(roomsListBox->currentItem());
}

void RoomsForm::addRoom()
{
	int ind=roomsListBox->currentItem();

	AddRoomForm addRoomForm;
	addRoomForm.exec();
	
	filterChanged();
	
	//roomsListBox->setCurrentItem(ind);
	Q_UNUSED(ind);
	int i=roomsListBox->count()-1;
	if(i>=0){
		roomsListBox->setCurrentItem(i);
		roomChanged(i);
	}
}

void RoomsForm::removeRoom()
{
	int ind=roomsListBox->currentItem();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected room"));
		return;
	}
	
	Room* rm=visibleRoomsList.at(ind);
	assert(rm!=NULL);

	if(QMessageBox::warning( this, tr("FET"),
		tr("Are you sure you want to delete this room and all related constraints?"),
		tr("Yes"), tr("No"), 0, 0, 1 ) == 1)
		return;

	bool tmp=gt.rules.removeRoom(rm->name);
	assert(tmp);

	filterChanged();
	
	if((uint)(ind)>=roomsListBox->count())
		ind=roomsListBox->count()-1;
	roomsListBox->setCurrentItem(ind);
}

void RoomsForm::roomChanged(int index)
{
	if(index<0){
		//currentRoomTextEdit->setText(tr("Invalid room"));
		currentRoomTextEdit->setText("");
		return;
	}

	QString s;
	Room* room=visibleRoomsList.at(index);

	assert(room!=NULL);
	s=room->getDetailedDescriptionWithConstraints(gt.rules);
	currentRoomTextEdit->setText(s);
}

void RoomsForm::sortRooms()
{
	gt.rules.sortRoomsAlphabetically();

	filterChanged();
}

void RoomsForm::modifyRoom()
{
	int ci=roomsListBox->currentItem();
	if(ci<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected room"));
		return;
	}
	
	Room* rm=visibleRoomsList.at(ci);
	ModifyRoomForm form(rm->name, rm->building, rm->capacity);
	form.exec();

	filterChanged();
	
	roomsListBox->setCurrentItem(ci);
}
