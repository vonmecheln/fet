/***************************************************************************
                          modifyconstraintactivitypreferredroomform.cpp  -  description
                             -------------------
    begin                : 13 Feb 2005
    copyright            : (C) 2005 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include <QMessageBox>

#include "modifyconstraintactivitypreferredroomform.h"
#include "spaceconstraint.h"

#include "lockunlock.h"

#include "longtextmessagebox.h"

ModifyConstraintActivityPreferredRoomForm::ModifyConstraintActivityPreferredRoomForm(QWidget* parent, ConstraintActivityPreferredRoom* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));
	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(helpPushButton, SIGNAL(clicked()), this, SLOT(help()));

	connect(selectedRealRoomsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(removeRealRoom()));
	connect(clearPushButton, SIGNAL(clicked()), this, SLOT(clearRealRooms()));
	connect(allRealRoomsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addRealRoom()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp5=roomsComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	QSize tmp6=activitiesComboBox->minimumSizeHint();
	Q_UNUSED(tmp6);
	
	activitiesComboBox->setMaximumWidth(maxRecommendedWidth(this));
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	permLockedCheckBox->setChecked(this->_ctr->permanentlyLocked);

	updateActivitiesComboBox();
	updateRoomsComboBox();

	allRealRoomsListWidget->clear();
	for(Room* rm : qAsConst(gt.rules.roomsList))
		if(rm->isVirtual==false)
			allRealRoomsListWidget->addItem(rm->name);
	allRealRoomsListWidget->setCurrentRow(0);

	selectedRealRoomsListWidget->clear();
	for(const QString& rrn : qAsConst(_ctr->preferredRealRoomsNames))
		selectedRealRoomsListWidget->addItem(rrn);
	selectedRealRoomsListWidget->setCurrentRow(0);
}

ModifyConstraintActivityPreferredRoomForm::~ModifyConstraintActivityPreferredRoomForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintActivityPreferredRoomForm::updateActivitiesComboBox()
{
	int i=0, j=-1;
	activitiesComboBox->clear();
	for(int k=0; k<gt.rules.activitiesList.size(); k++){
		Activity* act=gt.rules.activitiesList[k];
		activitiesComboBox->addItem(act->getDescription(gt.rules));
		if(act->id==this->_ctr->activityId)
			j=i;
		i++;
	}
	assert(j>=0);
	activitiesComboBox->setCurrentIndex(j);
}

void ModifyConstraintActivityPreferredRoomForm::updateRoomsComboBox()
{
	int i=0, j=-1;
	roomsComboBox->clear();
	for(int k=0; k<gt.rules.roomsList.size(); k++){
		Room* rm=gt.rules.roomsList[k];
		roomsComboBox->addItem(rm->name);
		if(rm->name==this->_ctr->roomName)
			j=i;
		i++;
	}
	assert(j>=0);
	roomsComboBox->setCurrentIndex(j);
}

void ModifyConstraintActivityPreferredRoomForm::removeRealRoom()
{
	int ind=selectedRealRoomsListWidget->currentRow();
	if(ind<0 || ind>=selectedRealRoomsListWidget->count()){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected real room."));
		return;
	}

	QListWidgetItem* item=selectedRealRoomsListWidget->takeItem(ind);
	delete item;

	if(ind>=selectedRealRoomsListWidget->count())
		ind=selectedRealRoomsListWidget->count()-1;
	selectedRealRoomsListWidget->setCurrentRow(ind);
}

void ModifyConstraintActivityPreferredRoomForm::clearRealRooms()
{
	selectedRealRoomsListWidget->clear();
}

void ModifyConstraintActivityPreferredRoomForm::addRealRoom()
{
	int ind=allRealRoomsListWidget->currentRow();
	if(ind<0 || ind>=allRealRoomsListWidget->count()){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected real room."));
		return;
	}

	bool exists=false;
	for(int i=0; i<selectedRealRoomsListWidget->count(); i++)
		if(selectedRealRoomsListWidget->item(i)->text()==allRealRoomsListWidget->item(ind)->text()){
			exists=true;
			break;
		}
	
	if(!exists){
		selectedRealRoomsListWidget->addItem(allRealRoomsListWidget->item(ind)->text());
		selectedRealRoomsListWidget->setCurrentRow(selectedRealRoomsListWidget->count()-1);
	}
}

void ModifyConstraintActivityPreferredRoomForm::cancel()
{
	this->close();
}

void ModifyConstraintActivityPreferredRoomForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight"));
		return;
	}

	int i=activitiesComboBox->currentIndex();
	if(i<0 || activitiesComboBox->count()<=0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid activity"));
		return;
	}
	Activity* act=gt.rules.activitiesList.at(i);

	i=roomsComboBox->currentIndex();
	if(i<0 || roomsComboBox->count()<=0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid room"));
		return;
	}
	QString room=roomsComboBox->currentText();

	int ri=gt.rules.searchRoom(room);
	assert(ri>=0);
	Room* rm=gt.rules.roomsList.at(ri);
	
	if(rm->isVirtual==false){
		if(selectedRealRoomsListWidget->count()>0){
			QMessageBox::warning(this, tr("FET information"), tr("The preferred room of the activity is a real room, not a virtual one."
			 " This implies that the selected real rooms list should be empty."));
			return;
		}
	}
	else{
		if(selectedRealRoomsListWidget->count()>0 && weight<100.0){
			QMessageBox::warning(this, tr("FET information"), tr("If the preferred room is virtual and the list of real rooms is not empty,"
			 " the weight percentage must be exactly 100%."));
			return;
		}
		
		if(selectedRealRoomsListWidget->count()>0 && rm->realRoomsSetsList.count()!=selectedRealRoomsListWidget->count()){
			QMessageBox::warning(this, tr("FET information"), tr("The preferred room of the activity is a virtual room."
			 " This implies that the number of selected real rooms in the list should either be zero or equal to the"
			 " number of sets of real rooms of the preferred virtual room, which is %1.").arg(rm->realRoomsSetsList.count()));
			return;
		}
		
		QSet<QString> rrs;
		for(const QStringList& tl : qAsConst(rm->realRoomsSetsList))
			for(const QString& s : qAsConst(tl))
				if(!rrs.contains(s))
					rrs.insert(s);
		
		QStringList incorrectList;
		for(int i=0; i<selectedRealRoomsListWidget->count(); i++){
			bool found=false;
			QString rrn=selectedRealRoomsListWidget->item(i)->text();
			if(rrs.contains(rrn))
				found=true;
				
			if(!found)
				incorrectList.append(rrn);
		}
		if(!incorrectList.isEmpty()){
			switch(LongTextMessageBox::confirmation(this, tr("FET information"), tr("The selected real rooms: %1 are not found in the sets of sets of real rooms of the"
			 " selected preferred virtual room. This is probably wrong. Are you sure you want to add this constraint?").arg(incorrectList.join(", ")),
			 tr("Yes"), tr("No"), QString(), 0, 1)){
			case 0:
				break;
			case 1:
				return;
			}
		}
	}
	
	QStringList lst;
	for(int i=0; i<selectedRealRoomsListWidget->count(); i++)
		lst.append(selectedRealRoomsListWidget->item(i)->text());

	this->_ctr->weightPercentage=weight;
	this->_ctr->roomName=room;
	
	if(_ctr->activityId!=act->id){
		int oldId=_ctr->activityId;
		int newId=act->id;
		
		QSet<ConstraintActivityPreferredRoom*> cs=gt.rules.aprHash.value(oldId, QSet<ConstraintActivityPreferredRoom*>());
		assert(cs.contains(_ctr));
		cs.remove(_ctr);
		gt.rules.aprHash.insert(oldId, cs);
		
		cs=gt.rules.aprHash.value(newId, QSet<ConstraintActivityPreferredRoom*>());
		assert(!cs.contains(_ctr));
		cs.insert(_ctr);
		gt.rules.aprHash.insert(newId, cs);
	
		this->_ctr->activityId=act->id;
	}
	
	this->_ctr->permanentlyLocked=permLockedCheckBox->isChecked();
	
	_ctr->preferredRealRoomsNames=lst;

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	LockUnlock::computeLockedUnlockedActivitiesOnlySpace();
	LockUnlock::increaseCommunicationSpinBox();
	
	this->close();
}

void ModifyConstraintActivityPreferredRoomForm::help()
{
	QString s;
	
	s+=tr("A room can be real (the simplest and the most used scenario) or virtual. You can read more about this in the rooms dialog, by clicking the Help button there.");
	s+="\n\n";
	s+=tr("If the preferred room selected in the combo box is real, the list of selected real rooms must remain empty.");
	s+="\n\n";
	s+=tr("If the preferred room selected in the combo box is virtual, you can select also the list of real rooms to be allocated to the "
	 "selected activity (if the preferred room selected in the combo box is virtual and the list of selected real rooms is not empty, the "
	 "weight of the constraint must be 100.0%).");
	
	LongTextMessageBox::largeInformation(this, tr("FET help"), s);
}
