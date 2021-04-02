//
//
// C++ Implementation: $MODULE$
//
// Description: 
//
//
// Author: Liviu Lalescu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "timetable_defs.h"
#include "fet.h"
#include "roomsform.h"
#include "addroomform.h"
#include "modifyroomform.h"

#include <q3listbox.h>
#include <qinputdialog.h>

#include <QDesktopWidget>

#include <QMessageBox>

RoomsForm::RoomsForm()
 : RoomsForm_template()
{
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

	AddRoomForm* addRoomForm=new AddRoomForm();
	addRoomForm->exec();
	
	filterChanged();
	
	roomsListBox->setCurrentItem(ind);
}

void RoomsForm::removeRoom()
{
	int ind=roomsListBox->currentItem();
	if(ind<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected room"));
		return;
	}
	
	Room* rm=visibleRoomsList.at(ind);
	assert(rm!=NULL);

	if(QMessageBox::warning( this, QObject::tr("FET"),
		QObject::tr("Are you sure you want to delete this room and all related constraints?\n"),
		QObject::tr("Yes"), QObject::tr("No"), 0, 0, 1 ) == 1)
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
		currentRoomTextEdit->setText(QObject::tr("Invalid room"));
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
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected room"));
		return;
	}
	
	Room* rm=visibleRoomsList.at(ci);
	ModifyRoomForm* form=new ModifyRoomForm(rm->name, rm->building, rm->capacity);
	form->exec();

	filterChanged();
	
	roomsListBox->setCurrentItem(ci);
}
