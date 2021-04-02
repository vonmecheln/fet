//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu <Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
// Copyright (C) 2003 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "timetable_defs.h"
#include "fet.h"
#include "roomsform.h"
#include "addroomform.h"
#include "modifyroomform.h"

#include "roommakeeditvirtualform.h"

#include "longtextmessagebox.h"

#include <QMessageBox>

#include <QListWidget>
#include <QScrollBar>
#include <QAbstractItemView>

#include <QSplitter>
#include <QSettings>
#include <QObject>
#include <QMetaObject>

extern const QString COMPANY;
extern const QString PROGRAM;

extern bool students_schedule_ready;
extern bool rooms_schedule_ready;
extern bool teachers_schedule_ready;

RoomsForm::RoomsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	currentRoomTextEdit->setReadOnly(true);
	
	modifyRoomPushButton->setDefault(true);
	
	roomsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(addRoomPushButton, SIGNAL(clicked()), this, SLOT(addRoom()));
	connect(removeRoomPushButton, SIGNAL(clicked()), this, SLOT(removeRoom()));
	connect(roomsListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(roomChanged(int)));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(modifyRoomPushButton, SIGNAL(clicked()), this, SLOT(modifyRoom()));

	connect(moveRoomUpPushButton, SIGNAL(clicked()), this, SLOT(moveRoomUp()));
	connect(moveRoomDownPushButton, SIGNAL(clicked()), this, SLOT(moveRoomDown()));

	connect(sortRoomsPushButton, SIGNAL(clicked()), this, SLOT(sortRooms()));
	connect(roomsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(modifyRoom()));

	connect(commentsPushButton, SIGNAL(clicked()), this, SLOT(comments()));

	connect(makeRealPushButton, SIGNAL(clicked()), this, SLOT(makeReal()));
	connect(makeEditVirtualPushButton, SIGNAL(clicked()), this, SLOT(makeEditVirtual()));
	connect(helpPushButton, SIGNAL(clicked()), this, SLOT(help()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	//restore splitter state
	QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/splitter-state")))
		splitter->restoreState(settings.value(this->metaObject()->className()+QString("/splitter-state")).toByteArray());
	
	this->filterChanged();
}

RoomsForm::~RoomsForm()
{
	saveFETDialogGeometry(this);
	//save splitter state
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/splitter-state"), splitter->saveState());
}

bool RoomsForm::filterOk(Room* rm)
{
	Q_UNUSED(rm);

	bool ok=true;
	return ok;
}

void RoomsForm::filterChanged()
{
	QString s;
	roomsListWidget->clear();
	visibleRoomsList.clear();
	for(int i=0; i<gt.rules.roomsList.size(); i++){
		Room* rm=gt.rules.roomsList[i];
		if(this->filterOk(rm)){
			//s=rm->getDescription();
			//roomsListWidget->addItem(s);
			if(rm->isVirtual==false)
				roomsListWidget->addItem(rm->name);
			else
				roomsListWidget->addItem(tr("V: %1", "V means virtual room, %1 is the name of the room").arg(rm->name));
			visibleRoomsList.append(rm);
		}
	}
	
	if(roomsListWidget->count()>0)
		roomsListWidget->setCurrentRow(0);
	else
		roomChanged(-1);
}

void RoomsForm::addRoom()
{
	AddRoomForm addRoomForm(this);
	setParentAndOtherThings(&addRoomForm, this);
	addRoomForm.exec();
	
	filterChanged();
	
	roomsListWidget->setCurrentRow(roomsListWidget->count()-1);
}

void RoomsForm::removeRoom()
{
	int ind=roomsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected room"));
		return;
	}
	
	Room* rm=visibleRoomsList.at(ind);
	assert(rm!=nullptr);

	if(QMessageBox::warning( this, tr("FET"),
		tr("Are you sure you want to delete this room and all related constraints?"),
		tr("Yes"), tr("No"), 0, 0, 1 ) == 1)
		return;
		
	int nv=0;
	int nvtotal=0;
	if(rm->isVirtual==false){
		for(Room* rr : qAsConst(gt.rules.roomsList)){
			bool met=false;
			if(rr->isVirtual==true){
				assert(rr->name!=rm->name);
				for(const QStringList& tl : qAsConst(rr->realRoomsSetsList)){
					if(tl.contains(rm->name)){
						nvtotal++;
						met=true;
					}
				}
			}
			if(met)
				nv++;
		}
	}
	if(nv>0 || nvtotal>0){
		assert(nv>0 && nvtotal>0);
		
		if(QMessageBox::warning( this, tr("FET"),
			tr("This is a real room. If you remove it, it will affect %1 virtual rooms."
			" The real room is met %2 times overall in the lists of sets of the virtual rooms."
			" Do you really want to remove it?").arg(nv).arg(nvtotal),
			tr("Yes"), tr("No"), 0, 0, 1 ) == 1)
			return;
	}

	bool tmp=gt.rules.removeRoom(rm->name);
	assert(tmp);

	visibleRoomsList.removeAt(ind);
	roomsListWidget->setCurrentRow(-1);
	QListWidgetItem* item=roomsListWidget->takeItem(ind);
	delete item;

	if(ind>=roomsListWidget->count())
		ind=roomsListWidget->count()-1;
	if(ind>=0)
		roomsListWidget->setCurrentRow(ind);
	else
		currentRoomTextEdit->setPlainText(QString(""));
}

void RoomsForm::roomChanged(int index)
{
	if(index<0){
		currentRoomTextEdit->setPlainText("");
		return;
	}

	QString s;
	Room* room=visibleRoomsList.at(index);

	assert(room!=nullptr);
	s=room->getDetailedDescriptionWithConstraints(gt.rules);
	currentRoomTextEdit->setPlainText(s);
}

void RoomsForm::moveRoomUp()
{
	if(roomsListWidget->count()<=1)
		return;
	int i=roomsListWidget->currentRow();
	if(i<0 || i>=roomsListWidget->count())
		return;
	if(i==0)
		return;
		
	QString s1=roomsListWidget->item(i)->text();
	QString s2=roomsListWidget->item(i-1)->text();
	
	Room* rm1=gt.rules.roomsList.at(i);
	Room* rm2=gt.rules.roomsList.at(i-1);
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	roomsListWidget->item(i)->setText(s2);
	roomsListWidget->item(i-1)->setText(s1);
	
	gt.rules.roomsList[i]=rm2;
	gt.rules.roomsList[i-1]=rm1;
	
	//Begin bug fix on 2017-08-29
	Room* vr1=visibleRoomsList[i];
	Room* vr2=visibleRoomsList[i-1];
	visibleRoomsList[i]=vr2;
	visibleRoomsList[i-1]=vr1;
	//End bug fix
	
	roomsListWidget->setCurrentRow(i-1);
	roomChanged(i-1);
}

void RoomsForm::moveRoomDown()
{
	if(roomsListWidget->count()<=1)
		return;
	int i=roomsListWidget->currentRow();
	if(i<0 || i>=roomsListWidget->count())
		return;
	if(i==roomsListWidget->count()-1)
		return;
		
	QString s1=roomsListWidget->item(i)->text();
	QString s2=roomsListWidget->item(i+1)->text();
	
	Room* rm1=gt.rules.roomsList.at(i);
	Room* rm2=gt.rules.roomsList.at(i+1);
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;

	roomsListWidget->item(i)->setText(s2);
	roomsListWidget->item(i+1)->setText(s1);
	
	gt.rules.roomsList[i]=rm2;
	gt.rules.roomsList[i+1]=rm1;
	
	//Begin bug fix on 2017-08-29
	Room* vr1=visibleRoomsList[i];
	Room* vr2=visibleRoomsList[i+1];
	visibleRoomsList[i]=vr2;
	visibleRoomsList[i+1]=vr1;
	//End bug fix

	roomsListWidget->setCurrentRow(i+1);
	roomChanged(i+1);
}

void RoomsForm::sortRooms()
{
	gt.rules.sortRoomsAlphabetically();

	filterChanged();
}

void RoomsForm::modifyRoom()
{
	int valv=roomsListWidget->verticalScrollBar()->value();
	int valh=roomsListWidget->horizontalScrollBar()->value();

	int ci=roomsListWidget->currentRow();
	if(ci<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected room"));
		return;
	}
	
	Room* rm=visibleRoomsList.at(ci);
	ModifyRoomForm form(this, rm->name, rm->building, rm->capacity);
	setParentAndOtherThings(&form, this);
	form.exec();

	filterChanged();
	
	roomsListWidget->verticalScrollBar()->setValue(valv);
	roomsListWidget->horizontalScrollBar()->setValue(valh);

	if(ci>=roomsListWidget->count())
		ci=roomsListWidget->count()-1;

	if(ci>=0)
		roomsListWidget->setCurrentRow(ci);
}

void RoomsForm::comments()
{
	int ind=roomsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected room"));
		return;
	}
	
	Room* rm=gt.rules.roomsList[ind];
	assert(rm!=nullptr);

	QDialog getCommentsDialog(this);
	
	getCommentsDialog.setWindowTitle(tr("Room comments"));
	
	QPushButton* okPB=new QPushButton(tr("OK"));
	okPB->setDefault(true);
	QPushButton* cancelPB=new QPushButton(tr("Cancel"));
	
	connect(okPB, SIGNAL(clicked()), &getCommentsDialog, SLOT(accept()));
	connect(cancelPB, SIGNAL(clicked()), &getCommentsDialog, SLOT(reject()));

	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch();
	hl->addWidget(okPB);
	hl->addWidget(cancelPB);
	
	QVBoxLayout* vl=new QVBoxLayout();
	
	QPlainTextEdit* commentsPT=new QPlainTextEdit();
	commentsPT->setPlainText(rm->comments);
	commentsPT->selectAll();
	commentsPT->setFocus();
	
	vl->addWidget(commentsPT);
	vl->addLayout(hl);
	
	getCommentsDialog.setLayout(vl);
	
	const QString settingsName=QString("RoomCommentsDialog");
	
	getCommentsDialog.resize(500, 320);
	centerWidgetOnScreen(&getCommentsDialog);
	restoreFETDialogGeometry(&getCommentsDialog, settingsName);
	
	int t=getCommentsDialog.exec();
	saveFETDialogGeometry(&getCommentsDialog, settingsName);
	
	if(t==QDialog::Accepted){
		rm->comments=commentsPT->toPlainText();
	
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		roomChanged(ind);
	}
}

void RoomsForm::help()
{
	QString s;
	
	s=tr("Virtual rooms were suggested by the user %1 on the FET forum (you can follow the discussion on the internet page %2)."
	 " They can be useful in two situations (but you might think of more uses):")
	 .arg("math").arg("https://lalescu.ro/liviu/fet/forum/index.php?topic=4249.0");
	s+="\n\n";
	s+=tr("1) If you want an activity to take place in more real rooms.");
	s+="\n\n";
	s+=tr("2) If you want an activity to take place, say, either in a large real room or in three smaller real rooms.");
	s+="\n\n";
	s+=tr("More details about defining and using virtual rooms are shown in the dialog of making/editing virtual rooms, if you click the Help button there.");

	LongTextMessageBox::largeInformation(this, tr("FET Help"), s);
}

void RoomsForm::makeReal()
{
	int valv=roomsListWidget->verticalScrollBar()->value();
	int valh=roomsListWidget->horizontalScrollBar()->value();

	int ci=roomsListWidget->currentRow();
	if(ci<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected room"));
		return;
	}
	
	Room* rm=visibleRoomsList.at(ci);
	
	if(rm->isVirtual==false){
		QMessageBox::information(this, tr("FET information"), tr("The selected room is already real."));
		return;
	}
	
	for(SpaceConstraint* ctr : qAsConst(gt.rules.spaceConstraintsList))
		if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
			ConstraintActivityPreferredRoom* c=(ConstraintActivityPreferredRoom*)ctr;
			
			if(c->roomName==rm->name && c->preferredRealRoomsNames.count()>0){
				QMessageBox::information(this, tr("FET information"), tr("This virtual room is used in at least a constraint of type"
				 " activity preferred room, having specified a nonempty list of preferred real rooms. It cannot thus be made real"
				 " directly. Firstly you need to edit/remove that/those constraints (converting this virtual room to a real one will"
				 " be possible if you clear the preferred real rooms list for each such constraint)."));
				return;
			}
		}
	
	if(QMessageBox::warning( this, tr("FET confirmation"),
		tr("Are you sure you want to make this room real? This will erase the list of sets of real rooms for this virtual room."),
		tr("Yes"), tr("No"), 0, 0, 1 ) == 1)
		return;
	
	rm->isVirtual=false;
	rm->realRoomsSetsList.clear();
	
	////////
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_schedule_ready=false;
	////////
	
	filterChanged();
	
	roomsListWidget->verticalScrollBar()->setValue(valv);
	roomsListWidget->horizontalScrollBar()->setValue(valh);

	if(ci>=roomsListWidget->count())
		ci=roomsListWidget->count()-1;

	if(ci>=0)
		roomsListWidget->setCurrentRow(ci);
}

void RoomsForm::makeEditVirtual()
{
	int valv=roomsListWidget->verticalScrollBar()->value();
	int valh=roomsListWidget->horizontalScrollBar()->value();

	int ci=roomsListWidget->currentRow();
	if(ci<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected room"));
		return;
	}
	
	Room* rm=visibleRoomsList.at(ci);
	
	QStringList vlcr; //virtual rooms containing rm
	if(rm->isVirtual==false)
		for(Room* rm2 : qAsConst(gt.rules.roomsList))
			if(rm2!=rm)
				if(rm2->isVirtual==true)
					for(const QStringList& tl : qAsConst(rm2->realRoomsSetsList))
						if(tl.contains(rm->name)){
							vlcr.append(rm2->name);
							break;
						}
						
	if(!vlcr.isEmpty()){
		QMessageBox::information(this, tr("FET information"), tr("This real room is contained in %1 other virtual rooms, so it cannot be made virtual"
		 " (because a virtual room can only contain real rooms).").arg(vlcr.count()));
		return;
	}

	if(rm->isVirtual==false)
		for(SpaceConstraint* ctr : qAsConst(gt.rules.spaceConstraintsList))
			if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
				ConstraintActivityPreferredRoom* c=(ConstraintActivityPreferredRoom*)ctr;
				
				if(c->preferredRealRoomsNames.contains(rm->name)){
					QMessageBox::information(this, tr("FET information"), tr("This real room is used in at least a constraint of type"
					 " activity preferred room, in the list of preferred real rooms. It cannot thus be made virtual"
					 " directly. Firstly you need to edit/remove that/those constraints."));
					return;
				}
			}
	
	RoomMakeEditVirtualForm form(this, rm);
	setParentAndOtherThings(&form, this);
	form.exec();
	
	filterChanged();
	
	roomsListWidget->verticalScrollBar()->setValue(valv);
	roomsListWidget->horizontalScrollBar()->setValue(valh);

	if(ci>=roomsListWidget->count())
		ci=roomsListWidget->count()-1;

	if(ci>=0)
		roomsListWidget->setCurrentRow(ci);
}
