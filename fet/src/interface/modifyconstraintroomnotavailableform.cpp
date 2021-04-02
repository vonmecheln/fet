/***************************************************************************
                          modifyconstraintroomnotavailableform.cpp  -  description
                             -------------------
    begin                : Feb 13, 2005
    copyright            : (C) 2005 by Lalescu Liviu
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

#include "modifyconstraintroomnotavailableform.h"
#include "spaceconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <q3table.h>

#include <QDesktopWidget>

ModifyConstraintRoomNotAvailableForm::ModifyConstraintRoomNotAvailableForm(ConstraintRoomNotAvailable* ctr)
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	this->_ctr=ctr;
	
	//compulsoryCheckBox->setChecked(ctr->compulsory);
	weightLineEdit->setText(QString::number(ctr->weightPercentage));

	updateRoomsComboBox();

	startHourComboBox->clear();
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		startHourComboBox->insertItem(gt.rules.hoursOfTheDay[i]);
	startHourComboBox->setCurrentItem(ctr->h1);

	endHourComboBox->clear();
	for(int i=0; i<=gt.rules.nHoursPerDay; i++)
		endHourComboBox->insertItem(gt.rules.hoursOfTheDay[i]);
	endHourComboBox->setCurrentItem(ctr->h2);

	dayComboBox->clear();
	for(int i=0; i<gt.rules.nDaysPerWeek; i++)
		dayComboBox->insertItem(gt.rules.daysOfTheWeek[i]);
	dayComboBox->setCurrentItem(ctr->d);
}

ModifyConstraintRoomNotAvailableForm::~ModifyConstraintRoomNotAvailableForm()
{
}

void ModifyConstraintRoomNotAvailableForm::updateRoomsComboBox()
{
	int i=0, j=-1;
	roomsComboBox->clear();
	for(int k=0; k<gt.rules.roomsList.size(); k++){
		Room* room=gt.rules.roomsList[k];
		roomsComboBox->insertItem(room->getDescription());
		if(room->name==this->_ctr->roomName)
			j=i;
		i++;
	}
	assert(j>=0);
	roomsComboBox->setCurrentItem(j);
}

void ModifyConstraintRoomNotAvailableForm::cancel()
{
	this->close();
}

void ModifyConstraintRoomNotAvailableForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<=0.0 || weight>100){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid weight"));
		return;
	}

/*	bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;*/

	int i=roomsComboBox->currentItem();
	if(i<0 || roomsComboBox->count()<=0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid room"));
		return;
	}
	Room* room=gt.rules.roomsList.at(i);

	int day=dayComboBox->currentItem();
	if(day<0 || day>=gt.rules.nDaysPerWeek + 1){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid day"));
		return;
	}
	int startHour=startHourComboBox->currentItem();
	if(startHour<-1 || startHour>=gt.rules.nHoursPerDay){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid start hour"));
		return;
	}
	int endHour=endHourComboBox->currentItem();
	if(endHour<=0 || endHour>gt.rules.nHoursPerDay){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid end hour"));
		return;
	}
	if(endHour<=startHour){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("End hour <= start hour - impossible"));
		return;
	}

	this->_ctr->weightPercentage=weight;
//	this->_ctr->compulsory=compulsory;
	this->_ctr->roomName=room->name;
	this->_ctr->d=day;
	this->_ctr->h1=startHour;
	this->_ctr->h2=endHour;

	gt.rules.internalStructureComputed=false;
	
	this->close();
}
