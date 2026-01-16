//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2019 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "timetable_defs.h"
#include "fet.h"
#include "roommakeeditvirtualform.h"

#include "longtextmessagebox.h"

#include <QMessageBox>

#include <QListWidget>
#include <QScrollBar>
#include <QAbstractItemView>

extern bool students_schedule_ready;
extern bool rooms_buildings_schedule_ready;
extern bool teachers_schedule_ready;

RoomMakeEditVirtualForm::RoomMakeEditVirtualForm(QWidget* parent, Room* _editedRoom): QDialog(parent)
{
	setupUi(this);
	
	okPushButton->setDefault(true);
	
	setsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	selectedRealRoomsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	allRealRoomsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	
	connect(setsListWidget, &QListWidget::currentRowChanged, this, &RoomMakeEditVirtualForm::setChanged);
	connect(addSetPushButton, &QPushButton::clicked, this, &RoomMakeEditVirtualForm::addSet);
	connect(removeSetPushButton, &QPushButton::clicked, this, &RoomMakeEditVirtualForm::removeSet);
	
	connect(selectedRealRoomsListWidget, &QListWidget::itemDoubleClicked, this, &RoomMakeEditVirtualForm::removeRoom);
	connect(clearPushButton, &QPushButton::clicked, this, &RoomMakeEditVirtualForm::clearRooms);
	
	connect(allRealRoomsListWidget, &QListWidget::itemDoubleClicked, this, &RoomMakeEditVirtualForm::addRoom);
	connect(allPushButton, &QPushButton::clicked, this, &RoomMakeEditVirtualForm::addAllRooms);
	
	connect(okPushButton, &QPushButton::clicked, this, &RoomMakeEditVirtualForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &RoomMakeEditVirtualForm::cancel);
	
	connect(helpPushButton, &QPushButton::clicked, this, &RoomMakeEditVirtualForm::help);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	editedRoom=_editedRoom;
	
	if(editedRoom->isVirtual)
		sets=editedRoom->realRoomsSetsList;
	else
		sets.clear();
	
	for(int i=0; i<sets.count(); i++)
		setsListWidget->addItem(tr("Set %1", "Set number %1 of real rooms").arg(i+1));
	setsListWidget->setCurrentRow(0);
	
	allRealRoomsListWidget->clear();
	for(Room* rm : std::as_const(gt.rules.roomsList))
		if(rm!=editedRoom)
			if(rm->isVirtual==false)
				allRealRoomsListWidget->addItem(rm->name);
	allRealRoomsListWidget->setCurrentRow(0);
}

RoomMakeEditVirtualForm::~RoomMakeEditVirtualForm()
{
	saveFETDialogGeometry(this);
}

void RoomMakeEditVirtualForm::setChanged()
{
	if(sets.count()==0){
		selectedRealRoomsListWidget->clear();
		return;
	}
	
	assert(sets.count()==setsListWidget->count());
	
	int si=setsListWidget->currentRow();
	if(si>=0){
		assert(si<setsListWidget->count());
		assert(si<sets.count());
		
		selectedRealRoomsListWidget->clear();
		for(const QString& tr : std::as_const(sets[si]))
			selectedRealRoomsListWidget->addItem(tr);
		
		selectedRealRoomsListWidget->setCurrentRow(0);
	}
}

void RoomMakeEditVirtualForm::addSet()
{
	sets.append(QStringList());
	setsListWidget->addItem(tr("Set %1", "Set number %1 of real rooms").arg(sets.count()));
	assert(sets.count()==setsListWidget->count());
	setsListWidget->setCurrentRow(setsListWidget->count()-1);
	setChanged();
}

void RoomMakeEditVirtualForm::removeSet()
{
	int ind=setsListWidget->currentRow();
	if(ind<0 || ind>=setsListWidget->count()){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected set."));
		return;
	}
	
	QMessageBox::StandardButton t=QMessageBox::question(this, tr("FET confirmation"), tr("Are you sure you want to remove the set number %1?").arg(ind+1)+"\n\n"+
	 tr("Note: The sets after the removed one will be renamed, so that all the sets are numbered from 1 to the number of sets."),
	 QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
	if(t==QMessageBox::No)
		return;
	
	assert(setsListWidget->count()==sets.count());
	assert(sets.count()>0);
	sets.removeAt(ind);
	disconnect(setsListWidget, &QListWidget::currentRowChanged, this, &RoomMakeEditVirtualForm::setChanged);
	QListWidgetItem* item=setsListWidget->takeItem(ind);
	connect(setsListWidget, &QListWidget::currentRowChanged, this, &RoomMakeEditVirtualForm::setChanged);
	delete item;

	assert(setsListWidget->count()==sets.count());

	for(int i=ind; i<setsListWidget->count(); i++)
		setsListWidget->item(i)->setText(tr("Set %1", "Set number %1 of real rooms").arg(i+1));

	int k=setsListWidget->currentRow();
	if(ind>=setsListWidget->count())
		ind=setsListWidget->count()-1;
	setsListWidget->setCurrentRow(ind);
	if(ind==k)
		setChanged();
}

void RoomMakeEditVirtualForm::removeRoom()
{
	int ind=selectedRealRoomsListWidget->currentRow();
	if(ind<0 || ind>=selectedRealRoomsListWidget->count()){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected real room."));
		return;
	}

	int si=setsListWidget->currentRow();
	assert(si>=0 && si<sets.count());
	
	assert(ind<sets.at(si).count());
	
	assert(sets.at(si).at(ind)==selectedRealRoomsListWidget->item(ind)->text());
	sets[si].removeAt(ind);

	QListWidgetItem* item=selectedRealRoomsListWidget->takeItem(ind);
	delete item;

	if(ind>=selectedRealRoomsListWidget->count())
		ind=selectedRealRoomsListWidget->count()-1;
	selectedRealRoomsListWidget->setCurrentRow(ind);
}

void RoomMakeEditVirtualForm::clearRooms()
{
	if(sets.count()==0)
		return;
	int si=setsListWidget->currentRow();
	assert(si>=0 && si<sets.count());
	
	sets[si].clear();
	selectedRealRoomsListWidget->clear();
}

void RoomMakeEditVirtualForm::addRoom()
{
	int ind=allRealRoomsListWidget->currentRow();
	if(ind<0 || ind>=allRealRoomsListWidget->count()){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected real room."));
		return;
	}

	if(sets.count()==0)
		return;
	int si=setsListWidget->currentRow();
	assert(si>=0 && si<sets.count());
	
	bool exists=false;
	for(int i=0; i<selectedRealRoomsListWidget->count(); i++)
		if(selectedRealRoomsListWidget->item(i)->text()==allRealRoomsListWidget->item(ind)->text()){
			exists=true;
			break;
		}
	assert(exists==(sets.at(si).contains(allRealRoomsListWidget->item(ind)->text())));
	
	if(!exists){
		sets[si].append(allRealRoomsListWidget->item(ind)->text());
		selectedRealRoomsListWidget->addItem(allRealRoomsListWidget->item(ind)->text());
		selectedRealRoomsListWidget->setCurrentRow(selectedRealRoomsListWidget->count()-1);
	}
}

void RoomMakeEditVirtualForm::addAllRooms()
{
	if(sets.count()==0)
		return;
	int si=setsListWidget->currentRow();
	assert(si>=0 && si<sets.count());
	
	for(int ind=0; ind<allRealRoomsListWidget->count(); ind++){
		bool exists=false;
		for(int i=0; i<selectedRealRoomsListWidget->count(); i++)
			if(selectedRealRoomsListWidget->item(i)->text()==allRealRoomsListWidget->item(ind)->text()){
				exists=true;
				break;
			}
		assert(exists==(sets.at(si).contains(allRealRoomsListWidget->item(ind)->text())));
	
		if(!exists){
			sets[si].append(allRealRoomsListWidget->item(ind)->text());
			selectedRealRoomsListWidget->addItem(allRealRoomsListWidget->item(ind)->text());
		}
	}
	selectedRealRoomsListWidget->setCurrentRow(selectedRealRoomsListWidget->count()-1);
}

void RoomMakeEditVirtualForm::cancel()
{
	close();
}

void RoomMakeEditVirtualForm::ok()
{
	if(sets.count()<2){
		QMessageBox::information(this, tr("FET information"), tr("You must input at least two sets of real rooms, otherwise you wouldn't need this virtual room."));
		return;
	}
	int i=0;
	for(const QStringList& tl : std::as_const(sets)){
		if(tl.count()==0){
			QMessageBox::information(this, tr("FET information"), tr("The set of real rooms number %1 has no real rooms - incorrect,"
			 " because each set must be nonvoid.").arg(i+1));
			return;
		}
		i++;
	}

	bool oldvirt=editedRoom->isVirtual;
	QString od=editedRoom->getDetailedDescription();

	editedRoom->realRoomsSetsList=sets;
	if(editedRoom->isVirtual==false)
		editedRoom->isVirtual=true;
	
	QString nd=editedRoom->getDetailedDescription();
	if(oldvirt)
		gt.rules.addUndoPoint(tr("The room\n\n%1\nchanged its real rooms sets, becoming:\n\n%2",
		 "%1 is the old room description, %2 is the new room description").arg(od).arg(nd));
	else
		gt.rules.addUndoPoint(tr("The room\n\n%1\nwas made virtual into:\n%2",
		 "%1 is the old room description, %2 is the new room description").arg(od).arg(nd));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_buildings_schedule_ready=false;
	
	close();
}

void RoomMakeEditVirtualForm::help()
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
	s+=tr("How does this work: Each virtual room has a SET of sets of real rooms. If an activity is placed in this virtual room,"
	 " FET will choose for the SET a real room from each set. The chosen real rooms must all be different.",
	 "Explanation for the translators: the SET incompases more sets. Use capital letters for"
	 " 'SET' and lowercase letters for 'set'");
	s+=" ";
	s+=tr("Warning: If it is impossible to choose a different real room from each set, the timetable will be impossible.");
	s+="\n\n";
	s+=tr("Example: We have a virtual room VR and four real rooms Room1... Room4. "
	 "SET is: set1 (Room1, Room2, Room3, Room4), set2 (Room1, Room2), and set3 (Room1, Room2). A solution would be "
	 "Room4 from set1, Room1 from set2, and Room2 from set3.");
	s+="\n\n";
	s+=tr("Important notes:");
	s+="\n\n";
	s+=tr("1) If an activity has a virtual preferred room, the considered capacity of the virtual room is that specified for that room,"
	 " not the sum of capacities of the chosen real rooms.");
	s+="\n\n";
	s+=tr("2) If an activity has a virtual preferred room, the building of this activity will be considered that specified for the virtual room,"
	 " not (one of) those of the chosen real rooms.");
	s+="\n\n";
	s+=tr("3) If an activity is assigned to a virtual room, then in the constraints of type 'activities same room if consecutive' and"
	 " 'activities occupy max different rooms' the room of this activity will considered to be the virtual room, not (one of) the chosen real rooms.");
	s+="\n\n";
	s+=tr("4) If an activity is assigned to a virtual room, consequently to some real rooms of this virtual room, the constraints"
	 " of type room not available times for the virtual room and for the chosen real rooms will all be respected"
	 " (allowing the constraints to be broken if their weight is lower than 100%).");
	s+="\n\n";
	s+=tr("Note that the order of sets in a virtual room's SET or the order of real rooms in each set does not matter."
	 " So, in this dialog, if you remove a set, the sets after it will be renamed"
	 " so that the sets are always Set 1... Set n, retaining their rooms (if you have for example Set 1, Set 2, Set 3, and Set 4, and you remove Set 2,"
	 " FET will keep Set 1, and rename Set 3 to Set 2 (keeping the real rooms of Set 3) and Set 4 to Set 3 (keeping the real rooms of Set 4)).");
	
	LongTextMessageBox::largeInformation(this, tr("FET Help"), s);
}
