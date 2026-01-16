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
#include "timetable.h"
#include "fet.h"

#include "lockunlock.h"

#include "hoursform.h"
#include "modifyhourform.h"
#include "modifyrealhourform.h"

#include <Qt>
#include <QShortcut>
#include <QKeySequence>

#include <QMessageBox>
#include <QListWidget>
#include <QListWidgetItem>

#include <QInputDialog>

#include <QHash>

extern Timetable gt;

extern bool students_schedule_ready;
extern bool teachers_schedule_ready;
extern bool rooms_buildings_schedule_ready;

HoursForm::HoursForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		realHoursLabel->setEnabled(false);
		realHoursListWidget->setEnabled(false);
		modifyRealHourPushButton->setEnabled(false);
	}

	okPushButton->setDefault(true);

	hoursListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(nHoursSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &HoursForm::numberOfHoursChanged);
	connect(cancelPushButton, &QPushButton::clicked, this, &HoursForm::cancel);
	connect(okPushButton, &QPushButton::clicked, this, &HoursForm::ok);
	connect(insertHourPushButton, &QPushButton::clicked, this, &HoursForm::insertHour);
	connect(modifyHourPushButton, &QPushButton::clicked, this, &HoursForm::modifyHour);
	connect(modifyRealHourPushButton, &QPushButton::clicked, this, &HoursForm::modifyRealHour);
	connect(removeHourPushButton, &QPushButton::clicked, this, &HoursForm::removeHour);

	connect(hoursListWidget, &QListWidget::itemDoubleClicked, this, &HoursForm::modifyHour);
	connect(realHoursListWidget, &QListWidget::itemDoubleClicked, this, &HoursForm::modifyRealHour);

	if(SHORTCUT_PLUS){
		QShortcut* addShortcut=new QShortcut(QKeySequence(Qt::Key_Plus), this);
		connect(addShortcut, &QShortcut::activated, [=]{insertHourPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	insertHourPushButton->setToolTip(QString("+"));
	}
	if(SHORTCUT_M){
		QShortcut* modifyShortcut=new QShortcut(QKeySequence(Qt::Key_M), this);
		connect(modifyShortcut, &QShortcut::activated, [=]{modifyHourPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	modifyHourPushButton->setToolTip(QString("M"));
	}
	if(SHORTCUT_DELETE){
		QShortcut* removeShortcut=new QShortcut(QKeySequence::Delete, this);
		connect(removeShortcut, &QShortcut::activated, [=]{removeHourPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	removeHourPushButton->setToolTip(QString("⌦"));
	}

	disconnect(nHoursSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &HoursForm::numberOfHoursChanged);
	nHoursSpinBox->setMinimum(1);
	nHoursSpinBox->setMaximum(MAX_HOURS_PER_DAY);
	nHoursSpinBox->setValue(gt.rules.nHoursPerDay);
	
	realNamesForHours.clear();
	realLongNamesForHours.clear();
	for(int i=0; i<gt.rules.nHoursPerDay; i++){
		realNamesForHours.append(gt.rules.hoursOfTheDay[i]);
		realLongNamesForHours.append(gt.rules.hoursOfTheDay_longNames[i]);
		hoursListWidget->addItem(tr("%1. N: %2, LN: %3", "%1 is the hour index, %2 is the (short) hour name, %3 is the long hour name")
		 .arg(i+1).arg(gt.rules.hoursOfTheDay[i]).arg(gt.rules.hoursOfTheDay_longNames[i]));
	}
	if(hoursListWidget->count()>=1)
		hoursListWidget->setCurrentRow(0);
	connect(nHoursSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &HoursForm::numberOfHoursChanged);

	if(gt.rules.mode==MORNINGS_AFTERNOONS){
		realNamesForRealHours.clear();
		realLongNamesForRealHours.clear();
		for(int i=0; i<gt.rules.nRealHoursPerDay; i++){
			realNamesForRealHours.append(gt.rules.realHoursOfTheDay[i]);
			realLongNamesForRealHours.append(gt.rules.realHoursOfTheDay_longNames[i]);
			realHoursListWidget->addItem(tr("%1. N: %2, LN: %3", "%1 is the real hour index, %2 is the (short) real hour name, %3 is the long real hour name")
			 .arg(i+1).arg(gt.rules.realHoursOfTheDay[i]).arg(gt.rules.realHoursOfTheDay_longNames[i]));
		}
	}
	numberOfRealHoursChanged();

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
	
	realNamesForHours.insert(i, tr("%1").arg(i+1));
	realLongNamesForHours.insert(i, tr("Hour %1").arg(i+1));
	hoursListWidget->insertItem(i, tr("%1. N: %2, LN: %3", "%1 is the hour index, %2 is the (short) hour name, %3 is the long hour name")
	 .arg(i+1).arg(tr("%1").arg(i+1)).arg(tr("Hour %1").arg(i+1)));
	for(int j=i+1; j<hoursListWidget->count(); j++)
		hoursListWidget->item(j)->setText(tr("%1. N: %2, LN: %3", "%1 is the hour index, %2 is the (short) hour name, %3 is the long hour name")
		 .arg(j+1).arg(realNamesForHours.at(j)).arg(realLongNamesForHours.at(j)));

	disconnect(nHoursSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &HoursForm::numberOfHoursChanged);
	int j=nHoursSpinBox->value();
	nHoursSpinBox->setValue(j+1);
	connect(nHoursSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &HoursForm::numberOfHoursChanged);
	
	numberOfRealHoursChanged();
}

void HoursForm::modifyHour()
{
	int i=hoursListWidget->currentRow();
	if(hoursListWidget->count()<=0 || i<0 || i>=hoursListWidget->count()){
		QMessageBox::warning(this, tr("FET information"), tr("Invalid selected hour"));
		return;
	}
	
	QString oldName=realNamesForHours.at(i);
	QString oldLongName=realLongNamesForHours.at(i);

	/*bool ok=false;
	QString newName=QInputDialog::getText(this, tr("Rename hour"), tr("Please enter the new name of this hour"),
	 QLineEdit::Normal, oldName, &ok);
	if(ok && !newName.isEmpty()){
		hoursListWidget->item(i)->setText(tr("%1. %2", "%1 is the hour index, %2 is the hour name").arg(i+1).arg(newName));
		realNamesForHours[i]=newName;
	}*/
	
	ModifyHourForm form(this, oldName, oldLongName);
	int res=form.exec();
	if(res==QDialog::Accepted){
		QString newName=form.name;
		QString newLongName=form.longName;
		
		hoursListWidget->item(i)->setText(tr("%1. N: %2, LN: %3", "%1 is the hour index, %2 is the (short) hour name, %3 is the long hour name")
		 .arg(i+1).arg(newName).arg(newLongName));
		realNamesForHours[i]=newName;
		realLongNamesForHours[i]=newLongName;
	}
}

void HoursForm::modifyRealHour()
{
	int i=realHoursListWidget->currentRow();
	if(realHoursListWidget->count()<=0 || i<0 || i>=realHoursListWidget->count()){
		QMessageBox::warning(this, tr("FET information"), tr("Invalid selected real hour"));
		return;
	}

	QString oldRealName=realNamesForRealHours.at(i);
	QString oldRealLongName=realLongNamesForRealHours.at(i);

	/*bool ok=false;
	QString newName=QInputDialog::getText(this, tr("Rename real hour"), tr("Please enter the new name of this real hour"),
	 QLineEdit::Normal, oldName, &ok);
	if(ok && !newName.isEmpty()){
		realHoursListWidget->item(i)->setText(tr("%1. %2", "%1 is the real hour index, %2 is the real hour name").arg(i+1).arg(newName));
		realNamesForRealHours[i]=newName;
	}*/
	
	ModifyRealHourForm form(this, oldRealName, oldRealLongName);
	int res=form.exec();
	if(res==QDialog::Accepted){
		QString newRealName=form.name;
		QString newRealLongName=form.longName;
		
		realHoursListWidget->item(i)->setText(tr("%1. N: %2, LN: %3", "%1 is the real hour index, %2 is the (short) real hour name, %3 is the long real hour name")
		 .arg(i+1).arg(newRealName).arg(newRealLongName));
		realNamesForRealHours[i]=newRealName;
		realLongNamesForRealHours[i]=newRealLongName;
	}
}

void HoursForm::removeHour()
{
	int i=hoursListWidget->currentRow();
	if(i>=0 && i<hoursListWidget->count() && hoursListWidget->count()>=2){
		hoursListWidget->setCurrentRow(-1);
		QListWidgetItem* item=hoursListWidget->takeItem(i);
		delete item;
		realNamesForHours.removeAt(i);
		realLongNamesForHours.removeAt(i);
		if(i<hoursListWidget->count())
			hoursListWidget->setCurrentRow(i);
		else
			hoursListWidget->setCurrentRow(hoursListWidget->count()-1);
		for(int j=i; j<hoursListWidget->count(); j++)
			hoursListWidget->item(j)->setText(tr("%1. N: %2, LN: %3", "%1 is the hour index, %2 is the (short) hour name, %3 is the long hour name")
			 .arg(j+1).arg(realNamesForHours[j]).arg(realLongNamesForHours[j]));

		disconnect(nHoursSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &HoursForm::numberOfHoursChanged);
		int j=nHoursSpinBox->value();
		nHoursSpinBox->setValue(j-1);
		connect(nHoursSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &HoursForm::numberOfHoursChanged);
		
		numberOfRealHoursChanged();
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
			realNamesForHours.removeLast();
			realLongNamesForHours.removeLast();
		}
		hoursListWidget->setCurrentRow(hoursListWidget->count()-1);
	}
	else if(nv>cnt){
		for(int i=cnt; i<nv; i++){
			realNamesForHours.append(tr("%1").arg(i+1));
			realLongNamesForHours.append(tr("Hour %1").arg(i+1));
			hoursListWidget->addItem(tr("%1. N: %2, LN: %3", "%1 is the hour index, %2 is the (short) hour name, %3 is the long hour name")
			 .arg(i+1).arg(tr("%1").arg(i+1)).arg(tr("Hour %1").arg(i+1)));
		}
	}
	
	numberOfRealHoursChanged();
}

void HoursForm::numberOfRealHoursChanged()
{
	if(gt.rules.mode!=MORNINGS_AFTERNOONS)
		return;

	int nv=nHoursSpinBox->value();
	int rnv=2*nv;
	realHoursLabel->setText(tr("Real hours: %1", "%1 is the number of real hours per week").arg(rnv));
	int rcnt=realHoursListWidget->count();
	if(rnv<rcnt){
		realHoursListWidget->setCurrentRow(-1);
		for(int i=rcnt-1; i>=rnv; i--){
			QListWidgetItem* item=realHoursListWidget->takeItem(i);
			delete item;
			realNamesForRealHours.removeLast();
			realLongNamesForRealHours.removeLast();
		}
		realHoursListWidget->setCurrentRow(realHoursListWidget->count()-1);
	}
	else if(rnv>rcnt){
		for(int i=rcnt; i<rnv; i++){
			realNamesForRealHours.append(tr("%1").arg(i+1));
			realLongNamesForRealHours.append(tr("Real hour %1").arg(i+1));
			realHoursListWidget->addItem(tr("%1. N: %2, LN: %3", "%1 is the real hour index, %2 is the (short) real hour name, %3 is the long real hour name")
			 .arg(i+1).arg(tr("%1").arg(i+1)).arg(tr("Real hour %1").arg(i+1)));
		}
	}
}

void HoursForm::ok()
{
	int nHours=hoursListWidget->count();

	QHash<QString, int> hashHourIndex;
	for(int i=0; i<nHours; i++){
		QString s=realNamesForHours.at(i);

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

	if(gt.rules.mode==MORNINGS_AFTERNOONS){
		QHash<QString, int> hashRealHourIndex;
		for(int i=0; i<2*nHours; i++){
			QString s=realNamesForRealHours.at(i);
			
			if(s.isEmpty()){
				QMessageBox::warning(this, tr("FET information"), tr("Empty names not allowed (the real hour number %1 has an empty name).").arg(i+1));
				return;
			}
			
			if(hashRealHourIndex.contains(s)){
				int j=hashRealHourIndex.value(s);
				QMessageBox::warning(this, tr("FET information"),
				 tr("Duplicate names not allowed (the real hour number %1 has the same name as the real hour number %2).").arg(i+1).arg(j+1));
				return;
			}
			else{
				hashRealHourIndex.insert(s, i);
			}
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
		rooms_buildings_schedule_ready=false;
		
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
		rooms_buildings_schedule_ready=false;
	}
	////////////

	QString oh=tr("Old number of hours: %1").arg(gt.rules.nHoursPerDay);
	oh+=QString("\n");
	oh+=tr("Old hours names:\n%1").arg(gt.rules.hoursOfTheDay.join("\n"));
	oh+=QString("\n");
	oh+=tr("Old hours long names:\n%1").arg(gt.rules.hoursOfTheDay_longNames.join("\n"));

	gt.rules.nHoursPerDay=nHours;
	gt.rules.hoursOfTheDay.clear();
	gt.rules.hoursOfTheDay_longNames.clear();
	for(int i=0; i<nHours; i++){
		gt.rules.hoursOfTheDay.append(realNamesForHours.at(i));
		gt.rules.hoursOfTheDay_longNames.append(realLongNamesForHours.at(i));
	}

	if(gt.rules.mode==MORNINGS_AFTERNOONS){
		oh+=QString("\n\n");
		oh+=tr("Old number of real hours: %1").arg(gt.rules.nRealHoursPerDay);
		oh+=QString("\n");
		oh+=tr("Old real hours names:\n%1").arg(gt.rules.realHoursOfTheDay.join("\n"));
		oh+=QString("\n");
		oh+=tr("Old real hours long names:\n%1").arg(gt.rules.realHoursOfTheDay_longNames.join("\n"));

		gt.rules.nRealHoursPerDay=2*nHours;
		gt.rules.realHoursOfTheDay.clear();
		gt.rules.realHoursOfTheDay_longNames.clear();
		for(int i=0; i<2*nHours; i++){
			gt.rules.realHoursOfTheDay.append(realNamesForRealHours.at(i));
			gt.rules.realHoursOfTheDay_longNames.append(realLongNamesForRealHours.at(i));
		}
	}

	QString nh=tr("New number of hours: %1").arg(gt.rules.nHoursPerDay);
	nh+=QString("\n");
	nh+=tr("New hours names:\n%1").arg(gt.rules.hoursOfTheDay.join("\n"));
	nh+=QString("\n");
	nh+=tr("New hours long names:\n%1").arg(gt.rules.hoursOfTheDay_longNames.join("\n"));

	if(gt.rules.mode==MORNINGS_AFTERNOONS){
		nh+=QString("\n\n");
		nh+=tr("New number of real hours: %1").arg(gt.rules.nRealHoursPerDay);
		nh+=QString("\n");
		nh+=tr("New real hours names:\n%1").arg(gt.rules.realHoursOfTheDay.join("\n"));
		nh+=QString("\n");
		nh+=tr("New real hours long names:\n%1").arg(gt.rules.realHoursOfTheDay_longNames.join("\n"));
	}
	
	if(gt.rules.mode!=MORNINGS_AFTERNOONS)
		gt.rules.addUndoPoint(tr("The number and/or the names/long names of the hours were changed from:\n\n%1\n\nto\n\n%2").arg(oh).arg(nh));
	else
		gt.rules.addUndoPoint(tr("The number and/or the names/long names of the hours/real hours were changed from:\n\n%1\n\nto\n\n%2").arg(oh).arg(nh));
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);

	this->close();
}

void HoursForm::cancel()
{
	this->close();
}
