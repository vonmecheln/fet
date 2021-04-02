/***************************************************************************
                          modifyconstraintroomnotavailabletimesform.cpp  -  description
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

#include "modifyconstraintroomnotavailabletimesform.h"
#include "spaceconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <q3table.h>

#include <QDesktopWidget>

#define YES	(QObject::tr("Not available", "Please keep translation short"))
#define NO	(QObject::tr("Available", "Please keep translation short"))

ModifyConstraintRoomNotAvailableTimesForm::ModifyConstraintRoomNotAvailableTimesForm(ConstraintRoomNotAvailableTimes* ctr)
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	this->_ctr=ctr;
	
	weightLineEdit->setText(QString::number(ctr->weightPercentage));

	updateRoomsComboBox();

	notAllowedTimesTable->setNumRows(gt.rules.nHoursPerDay);
	notAllowedTimesTable->setNumCols(gt.rules.nDaysPerWeek);

	for(int j=0; j<gt.rules.nDaysPerWeek; j++)
		notAllowedTimesTable->horizontalHeader()->setLabel(j, gt.rules.daysOfTheWeek[j]);
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		notAllowedTimesTable->verticalHeader()->setLabel(i, gt.rules.hoursOfTheDay[i]);

	bool currentMatrix[MAX_HOURS_PER_DAY][MAX_DAYS_PER_WEEK];
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			currentMatrix[i][j]=false;			
	assert(ctr->days.count()==ctr->hours.count());	
	for(int k=0; k<ctr->days.count(); k++){
		if(ctr->hours.at(k)==-1 || ctr->days.at(k)==-1)
			assert(0);
		int i=ctr->hours.at(k);
		int j=ctr->days.at(k);
		currentMatrix[i][j]=true;
	}

	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			if(!currentMatrix[i][j])
				notAllowedTimesTable->setText(i, j, NO);
			else
				notAllowedTimesTable->setText(i, j, YES);
}

ModifyConstraintRoomNotAvailableTimesForm::~ModifyConstraintRoomNotAvailableTimesForm()
{
}

void ModifyConstraintRoomNotAvailableTimesForm::updateRoomsComboBox()
{
	int i=0, j=-1;
	roomsComboBox->clear();
	for(int k=0; k<gt.rules.roomsList.size(); k++){
		Room* room=gt.rules.roomsList[k];
		roomsComboBox->insertItem(room->getDescription());
		if(room->name==this->_ctr->room)
			j=i;
		i++;
	}
	assert(j>=0);
	roomsComboBox->setCurrentItem(j);
}

void ModifyConstraintRoomNotAvailableTimesForm::tableClicked(int row, int col, int button, const QPoint& mousePos)
{
	Q_UNUSED(button);
	Q_UNUSED(mousePos);

	if(row>=0 && row<gt.rules.nHoursPerDay && col>=0 && col<gt.rules.nDaysPerWeek){
		QString s=notAllowedTimesTable->text(row, col);
		if(s==YES)
			s=NO;
		else{
			assert(s==NO);
			s=YES;
		}
		notAllowedTimesTable->setText(row, col, s);
	}
}

void ModifyConstraintRoomNotAvailableTimesForm::cancel()
{
	this->close();
}

void ModifyConstraintRoomNotAvailableTimesForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<=0.0 || weight>100){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid weight"));
		return;
	}

	int i=roomsComboBox->currentItem();
	if(i<0 || roomsComboBox->count()<=0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid room"));
		return;
	}
	
	Room* room=gt.rules.roomsList.at(i);

	foreach(SpaceConstraint* c, gt.rules.spaceConstraintsList)
		if(c!=this->_ctr && c->type==CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES){
			ConstraintRoomNotAvailableTimes* cc=(ConstraintRoomNotAvailableTimes*)c;
			if(cc->room==room->name){
				QMessageBox::warning(this, QObject::tr("FET information"),
					QObject::tr("A constraint of this type exists for the same room - cannot proceed"));
				return;
			}
		}
	
	this->_ctr->weightPercentage=weight;
	this->_ctr->room=room->name;

	QList<int> days;
	QList<int> hours;
	for(int j=0; j<gt.rules.nDaysPerWeek; j++)
		for(int i=0; i<gt.rules.nHoursPerDay; i++)
			if(notAllowedTimesTable->text(i, j)==YES){
				days.append(j);
				hours.append(i);
			}

	this->_ctr->days=days;
	this->_ctr->hours=hours;

	gt.rules.internalStructureComputed=false;
	
	this->close();
}
