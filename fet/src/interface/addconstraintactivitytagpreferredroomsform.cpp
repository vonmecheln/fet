/***************************************************************************
                          addconstraintactivitytagpreferredroomsform.cpp  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QMessageBox>

#include <cstdio>

#include "longtextmessagebox.h"

#include "addconstraintactivitytagpreferredroomsform.h"
#include "spaceconstraint.h"

AddConstraintActivityTagPreferredRoomsForm::AddConstraintActivityTagPreferredRoomsForm()
{
    setupUi(this);

//    connect(addPushButton, SIGNAL(clicked()), this, SLOT(addRoom()));
//    connect(removePushButton, SIGNAL(clicked()), this, SLOT(removeRoom()));
    connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(addConstraintPushButton, SIGNAL(clicked()), this, SLOT(addConstraint()));
    connect(roomsListBox, SIGNAL(selected(QString)), this, SLOT(addRoom()));
    connect(selectedRoomsListBox, SIGNAL(selected(QString)), this, SLOT(removeRoom()));

    connect(clearPushButton, SIGNAL(clicked()), this, SLOT(clear()));

	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);

	updateRoomsListBox();
	
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* sb=gt.rules.activityTagsList[i];
		activityTagsComboBox->insertItem(sb->name);
	}
}

AddConstraintActivityTagPreferredRoomsForm::~AddConstraintActivityTagPreferredRoomsForm()
{
}

void AddConstraintActivityTagPreferredRoomsForm::updateRoomsListBox()
{
	roomsListBox->clear();
	selectedRoomsListBox->clear();

	for(int i=0; i<gt.rules.roomsList.size(); i++){
		Room* rm=gt.rules.roomsList[i];
		roomsListBox->insertItem(rm->name);
	}
}

void AddConstraintActivityTagPreferredRoomsForm::addConstraint()
{
	SpaceConstraint *ctr=NULL;

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight"));
		return;
	}

	if(selectedRoomsListBox->count()==0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Empty list of selected rooms"));
		return;
	}
	if(selectedRoomsListBox->count()==1){
		QMessageBox::warning(this, tr("FET information"),
			tr("Only one selected room - please use constraint activity tag preferred room if you want a single room"));
		return;
	}
	
/*	if(subjectsComboBox->currentItem()<0 || subjectsComboBox->count()<=0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid selected subject"));
		return;	
	}
	QString subject=subjectsComboBox->currentText();*/
	
	if(activityTagsComboBox->currentItem()<0 || activityTagsComboBox->count()<=0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid selected activity tag"));
		return;	
	}
	QString activityTag=activityTagsComboBox->currentText();
	
	QStringList roomsList;
	for(uint i=0; i<selectedRoomsListBox->count(); i++)
		roomsList.append(selectedRoomsListBox->text(i));
	
	ctr=new ConstraintActivityTagPreferredRooms(weight, /* subject,*/ activityTag, roomsList);
	bool tmp2=gt.rules.addSpaceConstraint(ctr);
	
	if(tmp2){
		QString s=tr("Constraint added:");
		s+="\n\n";
		s+=ctr->getDetailedDescription(gt.rules);
		LongTextMessageBox::information(this, tr("FET information"), s);
	}
	else{
		QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - please report error"));
		delete ctr;
	}
}

void AddConstraintActivityTagPreferredRoomsForm::addRoom()
{
	if(roomsListBox->currentItem()<0)
		return;
	QString rmName=roomsListBox->currentText();
	assert(rmName!="");
	uint i;
	//duplicate?
	for(i=0; i<selectedRoomsListBox->count(); i++)
		if(rmName==selectedRoomsListBox->text(i))
			break;
	if(i<selectedRoomsListBox->count())
		return;
	selectedRoomsListBox->insertItem(rmName);
}

void AddConstraintActivityTagPreferredRoomsForm::removeRoom()
{
	if(selectedRoomsListBox->currentItem()<0 || selectedRoomsListBox->count()<=0)
		return;		
	selectedRoomsListBox->removeItem(selectedRoomsListBox->currentItem());
}

void AddConstraintActivityTagPreferredRoomsForm::clear()
{
	selectedRoomsListBox->clear();
}
