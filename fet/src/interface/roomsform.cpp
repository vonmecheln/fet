//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2003 Liviu Lalescu <https://lalescu.ro/liviu/>
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
#include "roomsform.h"
#include "addroomform.h"
#include "modifyroomform.h"

#include "roommakeeditvirtualform.h"

#include "longtextmessagebox.h"

#include <QMessageBox>

#include <Qt>
#include <QShortcut>
#include <QKeySequence>

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
extern bool rooms_buildings_schedule_ready;
extern bool teachers_schedule_ready;

RoomsForm::RoomsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	currentRoomTextEdit->setReadOnly(true);
	
	modifyRoomPushButton->setDefault(true);
	
	roomsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(addRoomPushButton, &QPushButton::clicked, this, &RoomsForm::addRoom);
	connect(removeRoomPushButton, &QPushButton::clicked, this, &RoomsForm::removeRoom);

	connect(roomsListWidget, &QListWidget::currentRowChanged, this, &RoomsForm::roomChanged);
	connect(closePushButton, &QPushButton::clicked, this, &RoomsForm::close);
	connect(modifyRoomPushButton, &QPushButton::clicked, this, &RoomsForm::modifyRoom);

	connect(moveRoomUpPushButton, &QPushButton::clicked, this, &RoomsForm::moveRoomUp);
	connect(moveRoomDownPushButton, &QPushButton::clicked, this, &RoomsForm::moveRoomDown);

	connect(sortRoomsPushButton, &QPushButton::clicked, this, &RoomsForm::sortRooms);
	connect(roomsListWidget, &QListWidget::itemDoubleClicked, this, &RoomsForm::modifyRoom);

	connect(longNamePushButton, &QPushButton::clicked, this, &RoomsForm::longName);
	connect(codePushButton, &QPushButton::clicked, this, &RoomsForm::code);
	connect(commentsPushButton, &QPushButton::clicked, this, &RoomsForm::comments);

	connect(makeRealPushButton, &QPushButton::clicked, this, &RoomsForm::makeReal);
	connect(makeEditVirtualPushButton, &QPushButton::clicked, this, &RoomsForm::makeEditVirtual);
	connect(helpPushButton, &QPushButton::clicked, this, &RoomsForm::help);

	if(SHORTCUT_PLUS){
		QShortcut* addShortcut=new QShortcut(QKeySequence(Qt::Key_Plus), this);
		connect(addShortcut, &QShortcut::activated, [=]{addRoomPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	addRoomPushButton->setToolTip(QString("+"));
	}
	if(SHORTCUT_M){
		QShortcut* modifyShortcut=new QShortcut(QKeySequence(Qt::Key_M), this);
		connect(modifyShortcut, &QShortcut::activated, [=]{modifyRoomPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	modifyRoomPushButton->setToolTip(QString("M"));
	}
	if(SHORTCUT_DELETE){
		QShortcut* removeShortcut=new QShortcut(QKeySequence::Delete, this);
		connect(removeShortcut, &QShortcut::activated, [=]{removeRoomPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	removeRoomPushButton->setToolTip(QString("⌦"));
	}
	if(SHORTCUT_C){
		QShortcut* commentsShortcut=new QShortcut(QKeySequence(Qt::Key_C), this);
		connect(commentsShortcut, &QShortcut::activated, [=]{commentsPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	commentsPushButton->setToolTip(QString("C"));
	}
	if(SHORTCUT_U){
		QShortcut* upShortcut=new QShortcut(QKeySequence(Qt::Key_U), this);
		connect(upShortcut, &QShortcut::activated, [=]{moveRoomUpPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	moveRoomUpPushButton->setToolTip(QString("U"));
	}
	if(SHORTCUT_J){
		QShortcut* downShortcut=new QShortcut(QKeySequence(Qt::Key_J), this);
		connect(downShortcut, &QShortcut::activated, [=]{moveRoomDownPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	moveRoomDownPushButton->setToolTip(QString("J"));
	}

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
	 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes ) == QMessageBox::No)
		return;
		
	int nv=0;
	int nvtotal=0;
	if(rm->isVirtual==false){
		for(Room* rr : std::as_const(gt.rules.roomsList)){
			bool met=false;
			if(rr->isVirtual==true){
				assert(rr->name!=rm->name);
				for(const QStringList& tl : std::as_const(rr->realRoomsSetsList)){
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
			QMessageBox::Yes | QMessageBox::No ) == QMessageBox::No)
			return;
	}

	QString od=rm->name;

	bool tmp=gt.rules.removeRoom(rm->name);
	assert(tmp);

	gt.rules.addUndoPoint(tr("Removed the room %1.").arg(od));

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
	rooms_buildings_schedule_ready=false;

	roomsListWidget->item(i)->setText(s2);
	roomsListWidget->item(i-1)->setText(s1);
	
	gt.rules.roomsList[i]=rm2;
	gt.rules.roomsList[i-1]=rm1;

	gt.rules.addUndoPoint(tr("Moved the room %1 up.").arg(s1));

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
	rooms_buildings_schedule_ready=false;

	roomsListWidget->item(i)->setText(s2);
	roomsListWidget->item(i+1)->setText(s1);
	
	gt.rules.roomsList[i]=rm2;
	gt.rules.roomsList[i+1]=rm1;
	
	gt.rules.addUndoPoint(tr("Moved the room %1 down.").arg(s1));

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

	gt.rules.addUndoPoint(tr("Sorted the rooms."));

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
	
	connect(okPB, &QPushButton::clicked, &getCommentsDialog, &QDialog::accept);
	connect(cancelPB, &QPushButton::clicked, &getCommentsDialog, &QDialog::reject);

	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch();
	hl->addWidget(okPB);
	hl->addWidget(cancelPB);
	
	QVBoxLayout* vl=new QVBoxLayout();
	
	QTextEdit* commentsPT=new QTextEdit();
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
		QString od=rm->getDetailedDescription();

		rm->comments=commentsPT->toPlainText();
	
		gt.rules.addUndoPoint(tr("Modified the comments of the room\n\n%1\ninto\n\n%2").arg(od).arg(rm->comments));

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
	
	for(SpaceConstraint* ctr : std::as_const(gt.rules.spaceConstraintsList))
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
	 QMessageBox::Yes | QMessageBox::No ) == QMessageBox::No)
		return;

	QString od=rm->getDetailedDescription();

	rm->isVirtual=false;
	rm->realRoomsSetsList.clear();
	
	gt.rules.addUndoPoint(tr("Made the virtual room\n\n%1\nreal, into\n\n%2",
	 "%1 is the old room description, %2 is the new room description").arg(od).arg(rm->getDetailedDescription()));

	////////
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_buildings_schedule_ready=false;
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
		for(Room* rm2 : std::as_const(gt.rules.roomsList))
			if(rm2!=rm)
				if(rm2->isVirtual==true)
					for(const QStringList& tl : std::as_const(rm2->realRoomsSetsList))
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
		for(SpaceConstraint* ctr : std::as_const(gt.rules.spaceConstraintsList))
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

void RoomsForm::longName()
{
	int ind=roomsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected room"));
		return;
	}
	
	Room* rm=gt.rules.roomsList[ind];
	assert(rm!=nullptr);

	QDialog getLongNameDialog(this);
	
	getLongNameDialog.setWindowTitle(tr("Room long name"));
	
	QPushButton* okPB=new QPushButton(tr("OK"));
	okPB->setDefault(true);
	QPushButton* cancelPB=new QPushButton(tr("Cancel"));
	
	connect(okPB, &QPushButton::clicked, &getLongNameDialog, &QDialog::accept);
	connect(cancelPB, &QPushButton::clicked, &getLongNameDialog, &QDialog::reject);

	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch();
	hl->addWidget(okPB);
	hl->addWidget(cancelPB);
	
	QVBoxLayout* vl=new QVBoxLayout();
	
	QLineEdit* longNameLE=new QLineEdit();
	longNameLE->setText(rm->longName);
	longNameLE->selectAll();
	longNameLE->setFocus();
	
	vl->addWidget(longNameLE);
	vl->addLayout(hl);
	
	getLongNameDialog.setLayout(vl);
	
	const QString settingsName=QString("RoomLongNameDialog");
	
	getLongNameDialog.resize(300, 200);
	centerWidgetOnScreen(&getLongNameDialog);
	restoreFETDialogGeometry(&getLongNameDialog, settingsName);
	
	int t=getLongNameDialog.exec();
	saveFETDialogGeometry(&getLongNameDialog, settingsName);
	
	if(t==QDialog::Accepted){
		QString oln=rm->longName;
	
		rm->longName=longNameLE->text();
	
		gt.rules.addUndoPoint(tr("Changed the long name for the room %1 from\n%2\nto\n%3.").arg(rm->name).arg(oln).arg(rm->longName));
	
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		roomChanged(ind);
	}
}

void RoomsForm::code()
{
	int ind=roomsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected room"));
		return;
	}
	
	Room* rm=gt.rules.roomsList[ind];
	assert(rm!=nullptr);

	QDialog getCodeDialog(this);
	
	getCodeDialog.setWindowTitle(tr("Room code"));
	
	QPushButton* okPB=new QPushButton(tr("OK"));
	okPB->setDefault(true);
	QPushButton* cancelPB=new QPushButton(tr("Cancel"));
	
	connect(okPB, &QPushButton::clicked, &getCodeDialog, &QDialog::accept);
	connect(cancelPB, &QPushButton::clicked, &getCodeDialog, &QDialog::reject);

	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch();
	hl->addWidget(okPB);
	hl->addWidget(cancelPB);
	
	QVBoxLayout* vl=new QVBoxLayout();
	
	QLineEdit* codeLE=new QLineEdit();
	codeLE->setText(rm->code);
	codeLE->selectAll();
	codeLE->setFocus();
	
	vl->addWidget(codeLE);
	vl->addLayout(hl);
	
	getCodeDialog.setLayout(vl);
	
	const QString settingsName=QString("RoomCodeDialog");
	
	getCodeDialog.resize(300, 200);
	centerWidgetOnScreen(&getCodeDialog);
	restoreFETDialogGeometry(&getCodeDialog, settingsName);
	
	int t=getCodeDialog.exec();
	saveFETDialogGeometry(&getCodeDialog, settingsName);
	
	if(t==QDialog::Accepted){
		QString oc=rm->code;
	
		rm->code=codeLE->text();
	
		gt.rules.addUndoPoint(tr("Changed the code for the room %1 from\n%2\nto\n%3.").arg(rm->name).arg(oc).arg(rm->code));
	
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		roomChanged(ind);
	}
}
