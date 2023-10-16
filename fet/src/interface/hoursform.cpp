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
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include "lockunlock.h"

#include "hoursform.h"

#include <QMessageBox>
#include <QListWidget>
#include <QListWidgetItem>

#include <QInputDialog>

#include <QHash>

extern Timetable gt;

extern bool students_schedule_ready;
extern bool teachers_schedule_ready;
extern bool rooms_schedule_ready;

HoursForm::HoursForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	hoursListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(nHoursSpinBox, SIGNAL(valueChanged(int)), this, SLOT(numberOfHoursChanged()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));
	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(insertHourPushButton, SIGNAL(clicked()), this, SLOT(insertHour()));
	connect(modifyHourPushButton, SIGNAL(clicked()), this, SLOT(modifyHour()));
	connect(removeHourPushButton, SIGNAL(clicked()), this, SLOT(removeHour()));
	connect(hoursListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(modifyHour()));

	disconnect(nHoursSpinBox, SIGNAL(valueChanged(int)), this, SLOT(numberOfHoursChanged()));
	nHoursSpinBox->setMinimum(1);
	nHoursSpinBox->setMaximum(MAX_HOURS_PER_DAY);
	nHoursSpinBox->setValue(gt.rules.nHoursPerDay);
	realNames.clear();
	for(int i=0; i<gt.rules.nHoursPerDay; i++){
		realNames.append(gt.rules.hoursOfTheDay[i]);
		hoursListWidget->addItem(tr("%1. %2", "%1 is the hours index, %2 is the hours name").arg(i+1).arg(gt.rules.hoursOfTheDay[i]));
	}
	if(hoursListWidget->count()>=1)
		hoursListWidget->setCurrentRow(0);
	connect(nHoursSpinBox, SIGNAL(valueChanged(int)), this, SLOT(numberOfHoursChanged()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
}

HoursForm::~HoursForm()
{
	saveFETDialogGeometry(this);
}

void HoursForm::insertHour()
{
	if(nHoursSpinBox->value()==nHoursSpinBox->maximum()){
		QMessageBox::warning(this, tr("FET information"), tr("Maximum number of hours reached"));
		return;
	}
	int i=hoursListWidget->currentRow();
	if(i<0)
		i=hoursListWidget->count();
	
	realNames.insert(i, tr("Hour %1").arg(i+1));
	hoursListWidget->insertItem(i, tr("%1. %2", "%1 is the hour index, %2 is the hour name").arg(i+1).arg(tr("Hour %1").arg(i+1)));
	for(int j=i+1; j<hoursListWidget->count(); j++)
		hoursListWidget->item(j)->setText(tr("%1. %2", "%1 is the hour index, %2 is the hour name").arg(j+1).arg(realNames.at(j)));

	disconnect(nHoursSpinBox, SIGNAL(valueChanged(int)), this, SLOT(numberOfHoursChanged()));
	int j=nHoursSpinBox->value();
	nHoursSpinBox->setValue(j+1);
	connect(nHoursSpinBox, SIGNAL(valueChanged(int)), this, SLOT(numberOfHoursChanged()));
}

void HoursForm::modifyHour()
{
	int i=hoursListWidget->currentRow();
	if(hoursListWidget->count()<=0 || i<0 || i>=hoursListWidget->count()){
		QMessageBox::warning(this, tr("FET information"), tr("Invalid selected hour"));
		return;
	}
	QString oldName=realNames.at(i);
	bool ok=false;
	QString newName=QInputDialog::getText(this, tr("Rename hour"), tr("Please enter the new name of this hour"),
	 QLineEdit::Normal, oldName, &ok);
	if(ok && !newName.isEmpty()){
		hoursListWidget->item(i)->setText(tr("%1. %2", "%1 is the hour index, %2 is the hour name").arg(i+1).arg(newName));
		realNames[i]=newName;
	}
}

void HoursForm::removeHour()
{
	int i=hoursListWidget->currentRow();
	if(i>=0 && i<hoursListWidget->count() && hoursListWidget->count()>=2){
		hoursListWidget->setCurrentRow(-1);
		QListWidgetItem* item=hoursListWidget->takeItem(i);
		delete item;
		realNames.removeAt(i);
		if(i<hoursListWidget->count())
			hoursListWidget->setCurrentRow(i);
		else
			hoursListWidget->setCurrentRow(hoursListWidget->count()-1);
		for(int j=i; j<hoursListWidget->count(); j++)
			hoursListWidget->item(j)->setText(tr("%1. %2", "%1 is the hour index, %2 is the hour name").arg(j+1).arg(realNames[j]));

		disconnect(nHoursSpinBox, SIGNAL(valueChanged(int)), this, SLOT(numberOfHoursChanged()));
		int j=nHoursSpinBox->value();
		nHoursSpinBox->setValue(j-1);
		connect(nHoursSpinBox, SIGNAL(valueChanged(int)), this, SLOT(numberOfHoursChanged()));
	}
	else if(hoursListWidget->count()==1){
		QMessageBox::warning(this, tr("FET information"), tr("The number of hours must be at least 1"));
	}
	else{
		QMessageBox::warning(this, tr("FET information"), tr("Invalid selected hour"));
	}
}

void HoursForm::numberOfHoursChanged()
{
	int nv=nHoursSpinBox->value();
	int cnt=hoursListWidget->count();
	if(nv<cnt){
		hoursListWidget->setCurrentRow(-1);
		for(int i=cnt-1; i>=nv; i--){
			QListWidgetItem* item=hoursListWidget->takeItem(i);
			delete item;
			realNames.removeLast();
		}
		hoursListWidget->setCurrentRow(hoursListWidget->count()-1);
	}
	else if(nv>cnt){
		for(int i=cnt; i<nv; i++){
			realNames.append(tr("Hour %1").arg(i+1));
			hoursListWidget->addItem(tr("%1. %2", "%1 is the hour index, %2 is the hour name").arg(i+1).arg(tr("Hour %1").arg(i+1)));
		}
	}
}

void HoursForm::ok()
{
	int nHours=hoursListWidget->count();

	QHash<QString, int> hashHourIndex;
	for(int i=0; i<nHours; i++){
		QString s=realNames.at(i);

		if(s.isEmpty()){
			QMessageBox::warning(this, tr("FET information"), tr("Empty names not allowed (the hour number %1 has an empty name).").arg(i+1));
			return;
		}

		if(hashHourIndex.contains(s)){
			int j=hashHourIndex.value(s);
			QMessageBox::warning(this, tr("FET information"),
			 tr("Duplicate names not allowed (the hour number %1 has the same name as the hour number %2).").arg(i+1).arg(j+1));
			return;
		}
		else{
			hashHourIndex.insert(s, i);
		}
	}

	//2011-10-18
	int cnt_mod=0;
	int cnt_rem=0;
	int oldHours=gt.rules.nHoursPerDay;
	gt.rules.nHoursPerDay=nHours;
	
	for(TimeConstraint* tc : std::as_const(gt.rules.timeConstraintsList))
		if(tc->hasWrongDayOrHour(gt.rules)){
			if(tc->canRepairWrongDayOrHour(gt.rules))
				cnt_mod++;
			else
				cnt_rem++;
		}

	for(SpaceConstraint* sc : std::as_const(gt.rules.spaceConstraintsList))
		if(sc->hasWrongDayOrHour(gt.rules)){
			if(sc->canRepairWrongDayOrHour(gt.rules))
				cnt_mod++;
			else
				cnt_rem++;
		}
	
	gt.rules.nHoursPerDay=oldHours;
	
	if(cnt_mod>0 || cnt_rem>0){
		QString s=QString("");
		if(cnt_rem>0){
			s+=tr("%1 constraints will be removed.", "%1 is the number of constraints").arg(cnt_rem);
			s+=" ";
		}
		if(cnt_mod>0){
			s+=tr("%1 constraints will be modified.", "%1 is the number of constraints").arg(cnt_mod);
			s+=" ";
		}
		s+=tr("Do you want to continue?");

		int res=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Yes|QMessageBox::Cancel);
		
		if(res==QMessageBox::Cancel)
			return;

		teachers_schedule_ready=false;
		students_schedule_ready=false;
		rooms_schedule_ready=false;
		
		int _oldHours=gt.rules.nHoursPerDay;
		gt.rules.nHoursPerDay=nHours;

		//time
		QList<TimeConstraint*> toBeRemovedTime;
		for(TimeConstraint* tc : std::as_const(gt.rules.timeConstraintsList)){
			if(tc->hasWrongDayOrHour(gt.rules)){
				bool tmp=tc->canRepairWrongDayOrHour(gt.rules);
				if(tmp){
					int tmp2=tc->repairWrongDayOrHour(gt.rules);
					assert(tmp2);
				}
				else{
					toBeRemovedTime.append(tc);
				}
			}
		}
		bool recomputeTime=false;

		if(toBeRemovedTime.count()>0){
			for(TimeConstraint* tc : std::as_const(toBeRemovedTime)){
				if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME)
					recomputeTime=true;
				bool tmp=gt.rules.removeTimeConstraint(tc);
				assert(tmp);
			}
		}
		//////

		//space
		QList<SpaceConstraint*> toBeRemovedSpace;
		for(SpaceConstraint* sc : std::as_const(gt.rules.spaceConstraintsList)){
			if(sc->hasWrongDayOrHour(gt.rules)){
				bool tmp=sc->canRepairWrongDayOrHour(gt.rules);
				if(tmp){
					int tmp2=sc->repairWrongDayOrHour(gt.rules);
					assert(tmp2);
				}
				else{
					toBeRemovedSpace.append(sc);
				}
			}
		}
		
		bool recomputeSpace=false;

		if(toBeRemovedSpace.count()>0){
			for(SpaceConstraint* sc : std::as_const(toBeRemovedSpace)){
				if(sc->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM)
					recomputeSpace=true;
				bool tmp=gt.rules.removeSpaceConstraint(sc);
				assert(tmp);
			}
		}
		//////

		gt.rules.nHoursPerDay=_oldHours;

		if(recomputeTime){
			LockUnlock::computeLockedUnlockedActivitiesOnlyTime();
		}
		if(recomputeSpace){
			assert(0);
			LockUnlock::computeLockedUnlockedActivitiesOnlySpace();
		}
		if(recomputeTime || recomputeSpace){
			LockUnlock::increaseCommunicationSpinBox();
		}
	}
	else{
		teachers_schedule_ready=false;
		students_schedule_ready=false;
		rooms_schedule_ready=false;
	}
	////////////

	gt.rules.nHoursPerDay=nHours;
	gt.rules.hoursOfTheDay.clear();
	for(int i=0; i<nHours; i++)
		gt.rules.hoursOfTheDay.append(realNames.at(i));
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);

	this->close();
}

void HoursForm::cancel()
{
	this->close();
}
