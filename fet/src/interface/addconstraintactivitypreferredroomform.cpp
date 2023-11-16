/***************************************************************************
                          addconstraintactivitypreferredroomform.cpp  -  description
                             -------------------
    begin                : 7 Feb 2005
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

#include <QSet>

#include "longtextmessagebox.h"

#include "addconstraintactivitypreferredroomform.h"

#include "lockunlock.h"

AddConstraintActivityPreferredRoomForm::AddConstraintActivityPreferredRoomForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(helpPushButton, SIGNAL(clicked()), this, SLOT(help()));
	connect(addConstraintPushButton, SIGNAL(clicked()), this, SLOT(addConstraint()));

	connect(selectedRealRoomsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(removeRealRoom()));
	connect(clearPushButton, SIGNAL(clicked()), this, SLOT(clearRealRooms()));
	connect(allRealRoomsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addRealRoom()));
	
	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	QSize tmp3=subjectsComboBox->minimumSizeHint();
	Q_UNUSED(tmp3);
	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	
	QSize tmp5=roomsComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	QSize tmp6=activitiesComboBox->minimumSizeHint();
	Q_UNUSED(tmp6);
	
	activitiesComboBox->setMaximumWidth(maxRecommendedWidth(this));
	
	teachersComboBox->addItem("");
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		teachersComboBox->addItem(tch->name);
	}
	teachersComboBox->setCurrentIndex(0);

	subjectsComboBox->addItem("");
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* sb=gt.rules.subjectsList[i];
		subjectsComboBox->addItem(sb->name);
	}
	subjectsComboBox->setCurrentIndex(0);

	activityTagsComboBox->addItem("");
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* st=gt.rules.activityTagsList[i];
		activityTagsComboBox->addItem(st->name);
	}
	activityTagsComboBox->setCurrentIndex(0);

	populateStudentsComboBox(studentsComboBox, QString(""), true);
	studentsComboBox->setCurrentIndex(0);
	
	filterChanged();
	updateRoomsComboBox();

	allRealRoomsListWidget->clear();
	for(Room* rm : std::as_const(gt.rules.roomsList))
		if(rm->isVirtual==false)
			allRealRoomsListWidget->addItem(rm->name);
	allRealRoomsListWidget->setCurrentRow(0);

	connect(teachersComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(filterChanged()));
	connect(studentsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(filterChanged()));
	connect(subjectsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(filterChanged()));
	connect(activityTagsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(filterChanged()));
}

AddConstraintActivityPreferredRoomForm::~AddConstraintActivityPreferredRoomForm()
{
	saveFETDialogGeometry(this);
}

bool AddConstraintActivityPreferredRoomForm::filterOk(Activity* act)
{
	QString tn=teachersComboBox->currentText();
	QString stn=studentsComboBox->currentText();
	QString sbn=subjectsComboBox->currentText();
	QString atn=activityTagsComboBox->currentText();
	int ok=true;

	//teacher
	if(tn!=""){
		bool ok2=false;
		for(QStringList::const_iterator it=act->teachersNames.constBegin(); it!=act->teachersNames.constEnd(); it++)
			if(*it == tn){
				ok2=true;
				break;
			}
		if(!ok2)
			ok=false;
	}

	//subject
	if(sbn!="" && sbn!=act->subjectName)
		ok=false;
		
	//activity tag
	if(atn!="" && !act->activityTagsNames.contains(atn))
		ok=false;
		
	//students
	if(stn!=""){
		bool ok2=false;
		for(QStringList::const_iterator it=act->studentsNames.constBegin(); it!=act->studentsNames.constEnd(); it++)
			if(*it == stn){
				ok2=true;
				break;
			}
		if(!ok2)
			ok=false;
	}
	
	return ok;
}

void AddConstraintActivityPreferredRoomForm::filterChanged(){
	activitiesComboBox->clear();
	activitiesList.clear();
	
	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		Activity* act=gt.rules.activitiesList[i];
		
		if(filterOk(act)){
			activitiesComboBox->addItem(act->getDescription(gt.rules));
			this->activitiesList.append(act->id);
		}
	}
}

void AddConstraintActivityPreferredRoomForm::updateRoomsComboBox()
{
	roomsComboBox->clear();
	for(int i=0; i<gt.rules.roomsList.size(); i++){
		Room* rm=gt.rules.roomsList[i];
		roomsComboBox->addItem(rm->name);
	}
}

void AddConstraintActivityPreferredRoomForm::removeRealRoom()
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

void AddConstraintActivityPreferredRoomForm::clearRealRooms()
{
	selectedRealRoomsListWidget->clear();
}

void AddConstraintActivityPreferredRoomForm::addRealRoom()
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

void AddConstraintActivityPreferredRoomForm::addConstraint()
{
	SpaceConstraint *ctr=nullptr;

	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage)"));
		return;
	}

	int id;
	int tmp2=activitiesComboBox->currentIndex();
	//assert(tmp2<gt.rules.activitiesList.size());
	//assert(tmp2<activitiesList.size());
	if(tmp2<0 || tmp2>=gt.rules.activitiesList.size() || tmp2>=activitiesList.size()){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid activity"));
		return;
	}
	else
		id=activitiesList.at(tmp2);
		
	/*int i=activitiesComboBox->currentIndex();
	if(i<0 || activitiesComboBox->count()<=0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid activity"));
		return;
	}
	Activity* act=gt.rules.activitiesList.at(i);*/

	int i=roomsComboBox->currentIndex();
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
		for(const QStringList& tl : std::as_const(rm->realRoomsSetsList))
			for(const QString& s : std::as_const(tl))
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

	ctr=new ConstraintActivityPreferredRoom(weight, id, room, lst, permLockedCheckBox->isChecked());

	bool tmp3=gt.rules.addSpaceConstraint(ctr);
	if(tmp3){
		QString s=tr("Constraint added:");
		s+="\n\n";
		s+=ctr->getDetailedDescription(gt.rules);
		LongTextMessageBox::information(this, tr("FET information"), s);

		gt.rules.addUndoPoint(tr("Added the constraint:\n\n%1").arg(ctr->getDetailedDescription(gt.rules)));

		/*if(permLockedCheckBox->isChecked()) wrong, must take care of weight==100.0
			idsOfPermanentlyLockedSpace.insert(id);
		else
			idsOfLockedSpace.insert(id);*/
		LockUnlock::computeLockedUnlockedActivitiesOnlySpace(); //safer
		LockUnlock::increaseCommunicationSpinBox();
	}
	else{
		QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - it must be a duplicate"));
		delete ctr;
	}
}

void AddConstraintActivityPreferredRoomForm::help()
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
