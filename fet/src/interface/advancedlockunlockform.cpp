/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************
                          advancedlockunlockform.cpp  -  description
                             -------------------
    begin                : Dec 2008
    copyright            : (C) by Liviu Lalescu (https://lalescu.ro/liviu/) and Volker Dirr (https://www.timetabling.de/)
 ***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include <QtGlobal>
#include <QSizePolicy>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#else
#include <QRegExp>
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include <QMessageBox>

#include <QBrush>
#include <QPalette>

#include <QProgressDialog>

#include <algorithm>
//using namespace std;

#include "lockunlock.h"
#include "advancedlockunlockform.h"
#include "fetmainform.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "solution.h"

#include "fet.h"

#include "longtextmessagebox.h"

extern bool students_schedule_ready;
extern bool teachers_schedule_ready;
extern bool rooms_buildings_schedule_ready;

extern Solution best_solution;

extern Timetable gt;

/*extern QSet<int> idsOfLockedTime;
extern QSet<int> idsOfLockedSpace;
extern QSet<int> idsOfPermanentlyLockedTime;
extern QSet<int> idsOfPermanentlyLockedSpace;*/

const int MIN_WIDTH=400;
const int MIN_HEIGHT=200;

const QString lockDaySettingsString=QString("AdvancedLockUnlockFormLockDay");
const QString unlockDaySettingsString=QString("AdvancedLockUnlockFormUnlockDay");
const QString lockAllSettingsString=QString("AdvancedLockUnlockFormLockAll");
const QString unlockAllSettingsString=QString("AdvancedLockUnlockFormUnlockAll");
const QString lockEndStudentsDaySettingsString=QString("AdvancedLockUnlockFormLockEndStudentsDay");
const QString unlockEndStudentsDaySettingsString=QString("AdvancedLockUnlockFormUnlockEndStudentsDay");
const QString lockActivityTagSettingsString=QString("AdvancedLockUnlockFormLockActivityTag");
const QString unlockActivityTagSettingsString=QString("AdvancedLockUnlockFormUnlockActivityTag");
///
const QString lockAdvancedFilterSettingsString=QString("AdvancedLockUnlockFormLockAdvancedFilter");
const QString unlockAdvancedFilterSettingsString=QString("AdvancedLockUnlockFormUnlockAdvancedFilter");

const QString lockDayConfirmationSettingsString=QString("AdvancedLockUnlockFormLockDayConfirmation");
const QString unlockDayConfirmationSettingsString=QString("AdvancedLockUnlockFormUnlockDayConfirmation");
const QString lockAllConfirmationSettingsString=QString("AdvancedLockUnlockFormLockAllConfirmation");
const QString unlockAllConfirmationSettingsString=QString("AdvancedLockUnlockFormUnlockAllConfirmation");
const QString lockEndStudentsDayConfirmationSettingsString=QString("AdvancedLockUnlockFormLockEndStudentsDayConfirmation");
const QString unlockEndStudentsDayConfirmationSettingsString=QString("AdvancedLockUnlockFormUnlockEndStudentsDayConfirmation");
const QString lockActivityTagConfirmationSettingsString=QString("AdvancedLockUnlockFormLockActivityTagConfirmation");
const QString unlockActivityTagConfirmationSettingsString=QString("AdvancedLockUnlockFormUnlockActivityTagConfirmation");
///
const QString lockAdvancedFilterConfirmationSettingsString=QString("AdvancedLockUnlockFormLockAdvancedFilterConfirmation");
const QString unlockAdvancedFilterConfirmationSettingsString=QString("AdvancedLockUnlockFormUnlockAdvancedFilterConfirmation");

extern const QString COMPANY;
extern const QString PROGRAM;

//The order is important: we must have DESCRIPTION < DETDESCRIPTION < DETDESCRIPTIONWITHCONSTRAINTS, because we use std::stable_sort to put
//the hopefully simpler/faster/easier to check filters first.
const int DESCRIPTION=0;
const int DETDESCRIPTION=1;
const int DETDESCRIPTIONWITHCONSTRAINTS=2;

const int CONTAINS=0;
const int DOESNOTCONTAIN=1;
const int REGEXP=2;
const int NOTREGEXP=3;

bool compareTimeConstraintsActivityPreferredStartingTimeActivitiesIds(TimeConstraint* a, TimeConstraint* b)
{
	assert(a->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME);
	ConstraintActivityPreferredStartingTime* apst=(ConstraintActivityPreferredStartingTime*)a;

	assert(b->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME);
	ConstraintActivityPreferredStartingTime* bpst=(ConstraintActivityPreferredStartingTime*)b;
	
	return apst->activityId < bpst->activityId;
}

bool compareSpaceConstraintsActivityPreferredRoomActivitiesIds(SpaceConstraint* a, SpaceConstraint* b)
{
	assert(a->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM);
	ConstraintActivityPreferredRoom* apr=(ConstraintActivityPreferredRoom*)a;

	assert(b->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM);
	ConstraintActivityPreferredRoom* bpr=(ConstraintActivityPreferredRoom*)b;
	
	return apr->activityId < bpr->activityId;
}

void AdvancedLockUnlockForm::lockDay(QWidget* parent)
{
	if(!students_schedule_ready || !teachers_schedule_ready || !rooms_buildings_schedule_ready){
		return;
	}
	
	QStringList days;
	for(int j=0; j<gt.rules.nDaysPerWeek; j++)
		days<<gt.rules.daysOfTheWeek[j];
	assert(days.size()!=0);

	//New Dialog
	QDialog taDialog(parent);
	taDialog.setWindowTitle(tr("FET - Lock activities of a day"));

	QVBoxLayout* taMainLayout=new QVBoxLayout(&taDialog);

	QLabel* taLabel=new QLabel();
	taLabel->setWordWrap(true);
	taLabel->setText(tr("All activities of the selected day will be locked")+"\n\n"+tr("Please select the day to lock:"));
	taLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

	QComboBox* taLW=new QComboBox();
	
	QSize tmp=taLW->minimumSizeHint();
	Q_UNUSED(tmp);
	
	taLW->addItems(days);
	taLW->setCurrentIndex(0);

	QPushButton* tapb1=new QPushButton(tr("Cancel"));
	QPushButton* tapb2=new QPushButton(tr("OK"));
	
	QHBoxLayout* buttons=new QHBoxLayout();
	buttons->addStretch();
	buttons->addWidget(tapb1);
	buttons->addWidget(tapb2);
	
	////////
	QGroupBox* selectorsGroupBox=new QGroupBox();
	selectorsGroupBox->setTitle(tr("Lock"));
	
	QHBoxLayout* groupLayout=new QHBoxLayout(selectorsGroupBox);
	
	QCheckBox* timeCheckBox=new QCheckBox();
	timeCheckBox->setChecked(true);
	timeCheckBox->setText(tr("Time"));
	QCheckBox* spaceCheckBox=new QCheckBox();
	spaceCheckBox->setChecked(true);
	spaceCheckBox->setText(tr("Space"));

	groupLayout->addWidget(timeCheckBox);
	groupLayout->addWidget(spaceCheckBox);
	groupLayout->addStretch();
	/////////
	
	taMainLayout->addWidget(taLabel);
	taMainLayout->addWidget(taLW);
	taMainLayout->addStretch();
	taMainLayout->addWidget(selectorsGroupBox);
	taMainLayout->addStretch();
	taMainLayout->addLayout(buttons);

	connect(tapb2, &QPushButton::clicked, &taDialog, &QDialog::accept);
	connect(tapb1, &QPushButton::clicked, &taDialog, &QDialog::reject);

	tapb2->setDefault(true);
	tapb2->setFocus();
	
	int w=taDialog.sizeHint().width();
	int h=taDialog.sizeHint().height();
	if(w<MIN_WIDTH)
		w=MIN_WIDTH;
	if(h<MIN_HEIGHT)
		h=MIN_HEIGHT;
	taDialog.resize(w,h);
	centerWidgetOnScreen(&taDialog);
	restoreFETDialogGeometry(&taDialog, lockDaySettingsString);
	int ok=taDialog.exec();
	saveFETDialogGeometry(&taDialog, lockDaySettingsString);
	if(ok==QDialog::Rejected)
		return;
		
	bool lockTime=timeCheckBox->isChecked();
	bool lockSpace=spaceCheckBox->isChecked();

	int selectedDayInt=taLW->currentIndex();
	assert(selectedDayInt>=0 && selectedDayInt<gt.rules.nDaysPerWeek);

	QString addedTimeConstraintsString;
	QString notAddedTimeConstraintsString;
	
	QList<ConstraintActivityPreferredStartingTime*> addedTimeConstraints;
	QList<ConstraintActivityPreferredStartingTime*> notAddedTimeConstraints;
	
	QString addedSpaceConstraintsString;
	QString notAddedSpaceConstraintsString;
	
	QList<ConstraintActivityPreferredRoom*> addedSpaceConstraints;
	QList<ConstraintActivityPreferredRoom*> notAddedSpaceConstraints;
	
	int addedTime=0, notAddedTime=0;
	int addedSpace=0, notAddedSpace=0;

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		if(best_solution.times[i]!=UNALLOCATED_TIME){
			assert(best_solution.times[i]>=0 && best_solution.times[i]<gt.rules.nHoursPerWeek);
			int d=best_solution.times[i]%gt.rules.nDaysPerWeek;
			int h=best_solution.times[i]/gt.rules.nDaysPerWeek;
			
			ConstraintActivityPreferredStartingTime* newTimeCtr=nullptr;
			
			ConstraintActivityPreferredRoom* newSpaceCtr=nullptr;
			
			if(d==selectedDayInt && lockTime){
				newTimeCtr=new ConstraintActivityPreferredStartingTime(100.0, gt.rules.internalActivitiesList[i].id, d, h, false);
			}
			
			if(d==selectedDayInt && lockSpace){
				if(best_solution.rooms[i]!=UNALLOCATED_SPACE && best_solution.rooms[i]!=UNSPECIFIED_ROOM){
					QStringList tl;
					if(gt.rules.internalRoomsList[best_solution.rooms[i]]->isVirtual==false)
						assert(best_solution.realRoomsList[i].isEmpty());
					else
						for(int rr : std::as_const(best_solution.realRoomsList[i]))
							tl.append(gt.rules.internalRoomsList[rr]->name);
					
					newSpaceCtr=new ConstraintActivityPreferredRoom(100.0, gt.rules.internalActivitiesList[i].id, gt.rules.internalRoomsList[best_solution.rooms[i]]->name, tl, false);
				}
			}
			
			int aid=gt.rules.internalActivitiesList[i].id;
			
			if(newTimeCtr!=nullptr){
				bool add=true;
				
				for(ConstraintActivityPreferredStartingTime* tc : gt.rules.apstHash.value(aid, QSet<ConstraintActivityPreferredStartingTime*>())){
					if((*tc) == (*newTimeCtr)){
						add=false;
						break;
					}
				}
				
				if(add){
					addedTimeConstraintsString+=newTimeCtr->getDetailedDescription(gt.rules)+"\n";
					addedTimeConstraints.append(newTimeCtr);
					
					addedTime++;
				}
				else{
					notAddedTimeConstraintsString+=newTimeCtr->getDetailedDescription(gt.rules)+"\n";
					notAddedTimeConstraints.append(newTimeCtr);
					
					notAddedTime++;
				}
			}

			if(newSpaceCtr!=nullptr){
				bool add=true;
				
				for(ConstraintActivityPreferredRoom* tc : gt.rules.aprHash.value(aid, QSet<ConstraintActivityPreferredRoom*>())){
					if((*tc) == (*newSpaceCtr)){
						add=false;
						break;
					}
				}
				
				if(add){
					addedSpaceConstraintsString+=newSpaceCtr->getDetailedDescription(gt.rules)+"\n";
					addedSpaceConstraints.append(newSpaceCtr);
	 				
	 				addedSpace++;
				}
				else{
					notAddedSpaceConstraintsString+=newSpaceCtr->getDetailedDescription(gt.rules)+"\n";
					notAddedSpaceConstraints.append(newSpaceCtr);
					
					notAddedSpace++;
				}
			}
		}
	}
	
	////////////
	//last confirmation dialog
	QDialog lastConfirmationDialog(&taDialog);
	lastConfirmationDialog.setWindowTitle(tr("Last confirmation needed"));

	QVBoxLayout* lastMainLayout=new QVBoxLayout(&lastConfirmationDialog);

	QPushButton* lastpb1=new QPushButton(tr("Cancel"));
	QPushButton* lastpb2=new QPushButton(tr("OK"));
	QHBoxLayout* lastButtons=new QHBoxLayout();
	lastButtons->addStretch();
	lastButtons->addWidget(lastpb1);
	lastButtons->addWidget(lastpb2);
	
	////texts
	QVBoxLayout* left=new QVBoxLayout();
	QVBoxLayout* right=new QVBoxLayout();
	QHBoxLayout* all=new QHBoxLayout();
	all->addLayout(left);
	all->addLayout(right);
	
	QTextEdit* addTim=new QTextEdit();
	addTim->setReadOnly(true);
	addTim->setPlainText(addedTimeConstraintsString);
	QLabel* labAddTim=new QLabel(tr("These time constraints will be added"));
	labAddTim->setWordWrap(true);
	labAddTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labAddTim);
	left->addWidget(addTim);

	QTextEdit* notAddTim=new QTextEdit();
	notAddTim->setReadOnly(true);
	notAddTim->setPlainText(notAddedTimeConstraintsString);
	QLabel* labNotAddTim=new QLabel(tr("These time constraints will NOT be added"));
	labNotAddTim->setWordWrap(true);
	labNotAddTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labNotAddTim);
	left->addWidget(notAddTim);

	QTextEdit* addSpa=new QTextEdit();
	addSpa->setReadOnly(true);
	addSpa->setPlainText(addedSpaceConstraintsString);
	QLabel* labAddSpa=new QLabel(tr("These space constraints will be added"));
	labAddSpa->setWordWrap(true);
	labAddSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labAddSpa);
	right->addWidget(addSpa);
	
	QTextEdit* notAddSpa=new QTextEdit();
	notAddSpa->setReadOnly(true);
	notAddSpa->setPlainText(notAddedSpaceConstraintsString);
	QLabel* labNotAddSpa=new QLabel(tr("These space constraints will NOT be added"));
	labNotAddSpa->setWordWrap(true);
	labNotAddSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labNotAddSpa);
	right->addWidget(notAddSpa);
	////end texts
	
	////////
	lastMainLayout->addLayout(all);
	//lastMainLayout->addStretch();
	lastMainLayout->addLayout(lastButtons);

	connect(lastpb2, &QPushButton::clicked, &lastConfirmationDialog, &QDialog::accept);
	connect(lastpb1, &QPushButton::clicked, &lastConfirmationDialog, &QDialog::reject);

	lastpb2->setDefault(true);
	lastpb2->setFocus();
	
	int lw=lastConfirmationDialog.sizeHint().width();
	int lh=lastConfirmationDialog.sizeHint().height();
	lastConfirmationDialog.resize(lw,lh);
	centerWidgetOnScreen(&lastConfirmationDialog);
	restoreFETDialogGeometry(&lastConfirmationDialog, lockDayConfirmationSettingsString);
	ok=lastConfirmationDialog.exec();
	saveFETDialogGeometry(&lastConfirmationDialog, lockDayConfirmationSettingsString);
	if(!ok)
		return;
	////////////
	
	for(TimeConstraint* tc : std::as_const(addedTimeConstraints)){
		bool t=gt.rules.addTimeConstraint(tc);
		assert(t);
	}
	addedTimeConstraints.clear();
	for(TimeConstraint* tc : std::as_const(notAddedTimeConstraints)){
		delete tc;
	}
	notAddedTimeConstraints.clear();
	
	for(SpaceConstraint* sc : std::as_const(addedSpaceConstraints)){
		bool t=gt.rules.addSpaceConstraint(sc);
		assert(t);
	}
	addedSpaceConstraints.clear();
	for(SpaceConstraint* sc : std::as_const(notAddedSpaceConstraints)){
		delete sc;
	}
	notAddedSpaceConstraints.clear();
	
	QMessageBox::information(&lastConfirmationDialog, tr("FET information"), tr("There were added %1 locking time constraints and"
		" %2 locking space constraints. There were not added %3 locking time constraints and %4 locking space constraints, because"
		" these activities were already locked").arg(addedTime).arg(addedSpace).arg(notAddedTime).arg(notAddedSpace));

	if(addedTime>0 || addedSpace>0)
		gt.rules.addUndoPoint(tr("Locked the activities of the day %1, by adding %2 locking time constraints and %3 locking space constraints.")
		 .arg(gt.rules.daysOfTheWeek[selectedDayInt]).arg(addedTime).arg(addedSpace));
	
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	LockUnlock::increaseCommunicationSpinBox();
}

/*void AdvancedLockUnlockForm::unlockDay(QWidget* parent)
{
	if(!students_schedule_ready || !teachers_schedule_ready || !rooms_buildings_schedule_ready){
		return;
	}
	
	QStringList days;
	for(int j=0; j<gt.rules.nDaysPerWeek; j++)
		days<<gt.rules.daysOfTheWeek[j];
	assert(days.size()!=0);

	//New Dialog
	QDialog taDialog(parent);
	taDialog.setWindowTitle(tr("FET - Unlock activities of a day"));

	QVBoxLayout* taMainLayout=new QVBoxLayout(&taDialog);

	QLabel* taLabel=new QLabel();
	taLabel->setWordWrap(true);
	taLabel->setText(tr("All activities of the selected day will be unlocked (those which are not permanently locked)")+"\n\n"+tr("Please select the day to unlock:"));
	taLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

	QComboBox* taLW=new QComboBox();
	
	QSize tmp2=taLW->minimumSizeHint();
	Q_UNUSED(tmp2);
	
	taLW->addItems(days);
	taLW->setCurrentIndex(0);

	QPushButton* tapb1=new QPushButton(tr("Cancel"));
	QPushButton* tapb2=new QPushButton(tr("OK"));
	
	QHBoxLayout* buttons=new QHBoxLayout();
	buttons->addStretch();
	buttons->addWidget(tapb1);
	buttons->addWidget(tapb2);
	
	////////
	QGroupBox* selectorsGroupBox=new QGroupBox();
	selectorsGroupBox->setTitle(tr("Unlock"));
	
	QHBoxLayout* groupLayout=new QHBoxLayout(selectorsGroupBox);
	
	QCheckBox* timeCheckBox=new QCheckBox();
	timeCheckBox->setChecked(true);
	timeCheckBox->setText(tr("Time"));
	QCheckBox* spaceCheckBox=new QCheckBox();
	spaceCheckBox->setChecked(true);
	spaceCheckBox->setText(tr("Space"));

	groupLayout->addWidget(timeCheckBox);
	groupLayout->addWidget(spaceCheckBox);
	groupLayout->addStretch();
	/////////
	
	taMainLayout->addWidget(taLabel);
	taMainLayout->addWidget(taLW);
	taMainLayout->addStretch();
	taMainLayout->addWidget(selectorsGroupBox);
	taMainLayout->addStretch();
	taMainLayout->addLayout(buttons);

	QObj ect::connect(tapb2, SIG NAL(clicked()), &taDialog, SL OT(accept()));
	QObj ect::connect(tapb1, SIG NAL(clicked()), &taDialog, SL OT(reject()));

	tapb2->setDefault(true);
	tapb2->setFocus();
	
	int w=taDialog.sizeHint().width();
	int h=taDialog.sizeHint().height();
	if(w<MIN_WIDTH)
		w=MIN_WIDTH;
	if(h<MIN_HEIGHT)
		h=MIN_HEIGHT;
	taDialog.resize(w,h);
	centerWidgetOnScreen(&taDialog);
	restoreFETDialogGeometry(&taDialog, unlockDaySettingsString);
	bool ok=taDialog.exec();
	saveFETDialogGeometry(&taDialog, unlockDaySettingsString);
	if(!ok)
		return;
		
	bool unlockTime=timeCheckBox->isChecked();
	bool unlockSpace=spaceCheckBox->isChecked();

	int selectedDayInt=taLW->currentIndex();
	assert(selectedDayInt>=0 && selectedDayInt<gt.rules.nDaysPerWeek);

	int removedTime=0, notRemovedTime=0;
	int removedSpace=0, notRemovedSpace=0;
	
	QList<int> lockedActivitiesIds;

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		if(best_solution.times[i]!=UNALLOCATED_TIME){
			assert(best_solution.times[i]>=0 && best_solution.times[i]<gt.rules.nHoursPerWeek);
			int d=best_solution.times[i]%gt.rules.nDaysPerWeek;
			//int h=best_solution.times[i]/gt.rules.nDaysPerWeek;
			
			if(d==selectedDayInt){
				lockedActivitiesIds.append(gt.rules.internalActivitiesList[i].id);
			}
		}
	}

	QString removedTimeConstraintsString;
	QString notRemovedTimeConstraintsString;

	QList<TimeConstraint*> removedTimeConstraints;
	QList<TimeConstraint*> notRemovedTimeConstraints;
	
	QString removedSpaceConstraintsString;
	QString notRemovedSpaceConstraintsString;

	QList<SpaceConstraint*> removedSpaceConstraints;
	QList<SpaceConstraint*> notRemovedSpaceConstraints;
	
	if(unlockTime){
		for(int aid : std::as_const(lockedActivitiesIds)){
			for(ConstraintActivityPreferredStartingTime* c : gt.rules.apstHash.value(aid, QSet<ConstraintActivityPreferredStartingTime*>())){
				assert(aid==c->activityId);
				if(c->weightPercentage==100.0 && c->day>=0 && c->hour>=0){
//					if(c->day!=selectedDayInt){
						//QMessageBox::warning(&taDialog, tr("FET warning"), tr("Incorrect data - time constraint is incorrect - please regenerate the timetable. Please report possible bug."));
						//above test is not good???
//					}
					//assert(c->day==selectedDayInt);
					
					if(!c->permanentlyLocked){
						removedTimeConstraints.append((TimeConstraint*)c);
						removedTimeConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						removedTime++;
					}
					else{
						notRemovedTimeConstraints.append((TimeConstraint*)c);
						notRemovedTimeConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						notRemovedTime++;
					}
				}
			}
		}
	}

	if(unlockSpace){
		for(int aid : std::as_const(lockedActivitiesIds)){
			for(ConstraintActivityPreferredRoom* c : gt.rules.aprHash.value(aid, QSet<ConstraintActivityPreferredRoom*>())){
				assert(aid==c->activityId);
				if(c->weightPercentage==100.0){
					if(!c->permanentlyLocked){
						removedSpaceConstraints.append((SpaceConstraint*)c);
						removedSpaceConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						removedSpace++;
					}
					else{
						notRemovedSpaceConstraints.append((SpaceConstraint*)c);
						notRemovedSpaceConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						notRemovedSpace++;
					}
				}
			}
		}
	}

	////////////
	//last confirmation dialog
	QDialog lastConfirmationDialog(&taDialog);
	lastConfirmationDialog.setWindowTitle(tr("Last confirmation needed"));

	QVBoxLayout* lastMainLayout=new QVBoxLayout(&lastConfirmationDialog);

	QPushButton* lastpb1=new QPushButton(tr("Cancel"));
	QPushButton* lastpb2=new QPushButton(tr("OK"));
	QHBoxLayout* lastButtons=new QHBoxLayout();
	lastButtons->addStretch();
	lastButtons->addWidget(lastpb1);
	lastButtons->addWidget(lastpb2);
	
	////texts
	QVBoxLayout* left=new QVBoxLayout();
	QVBoxLayout* right=new QVBoxLayout();
	QHBoxLayout* all=new QHBoxLayout();
	all->addLayout(left);
	all->addLayout(right);
	
	QTextEdit* remTim=new QTextEdit();
	remTim->setReadOnly(true);
	remTim->setPlainText(removedTimeConstraintsString);
	QLabel* labRemTim=new QLabel(tr("These time constraints will be removed"));
	labRemTim->setWordWrap(true);
	labRemTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labRemTim);
	left->addWidget(remTim);

	QTextEdit* notRemTim=new QTextEdit();
	notRemTim->setReadOnly(true);
	notRemTim->setPlainText(notRemovedTimeConstraintsString);
	QLabel* labNotRemTim=new QLabel(tr("These time constraints will NOT be removed"));
	labNotRemTim->setWordWrap(true);
	labNotRemTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labNotRemTim);
	left->addWidget(notRemTim);

	QTextEdit* remSpa=new QTextEdit();
	remSpa->setReadOnly(true);
	remSpa->setPlainText(removedSpaceConstraintsString);
	QLabel* labRemSpa=new QLabel(tr("These space constraints will be removed"));
	labRemSpa->setWordWrap(true);
	labRemSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labRemSpa);
	right->addWidget(remSpa);
	
	QTextEdit* notRemSpa=new QTextEdit();
	notRemSpa->setReadOnly(true);
	notRemSpa->setPlainText(notRemovedSpaceConstraintsString);
	QLabel* labNotRemSpa=new QLabel(tr("These space constraints will NOT be removed"));
	labNotRemSpa->setWordWrap(true);
	labNotRemSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labNotRemSpa);
	right->addWidget(notRemSpa);
	////end texts
	
	////////
	lastMainLayout->addLayout(all);
	//lastMainLayout->addStretch();
	lastMainLayout->addLayout(lastButtons);

	QObj ect::connect(lastpb2, SIG NAL(clicked()), &lastConfirmationDialog, SL OT(accept()));
	QObj ect::connect(lastpb1, SIG NAL(clicked()), &lastConfirmationDialog, SL OT(reject()));

	lastpb2->setDefault(true);
	lastpb2->setFocus();
	
	int lw=lastConfirmationDialog.sizeHint().width();
	int lh=lastConfirmationDialog.sizeHint().height();
	lastConfirmationDialog.resize(lw,lh);
	centerWidgetOnScreen(&lastConfirmationDialog);
	restoreFETDialogGeometry(&lastConfirmationDialog, unlockDayConfirmationSettingsString);
	ok=lastConfirmationDialog.exec();
	saveFETDialogGeometry(&lastConfirmationDialog, unlockDayConfirmationSettingsString);
	if(!ok)
		return;
	////////////
	
	bool t=gt.rules.removeTimeConstraints(removedTimeConstraints);
	assert(t);
	
	removedTimeConstraints.clear();
	notRemovedTimeConstraints.clear();

	t=gt.rules.removeSpaceConstraints(removedSpaceConstraints);
	assert(t);
	
	removedSpaceConstraints.clear();
	notRemovedSpaceConstraints.clear();

	QMessageBox::information(&lastConfirmationDialog, tr("FET information"), tr("There were removed %1 locking time constraints and"
		" %2 locking space constraints. There were not removed %3 locking time constraints and %4 locking space constraints, because"
		" these activities were permanently locked").arg(removedTime).arg(removedSpace).arg(notRemovedTime).arg(notRemovedSpace));
	
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	LockUnlock::increaseCommunicationSpinBox();
}*/

void AdvancedLockUnlockForm::lockEndStudentsDay(QWidget* parent)
{
	if(!students_schedule_ready || !teachers_schedule_ready || !rooms_buildings_schedule_ready){
		return;
	}
	
	//New Dialog
	QDialog taDialog(parent);
	taDialog.setWindowTitle(tr("FET - Lock all activities which end each students set's day"));

	QVBoxLayout* taMainLayout=new QVBoxLayout(&taDialog);

	QLabel* taLabel=new QLabel();
	taLabel->setWordWrap(true);
	taLabel->setText(tr("All activities which end each students set's day will be locked"));
	taLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

	QPushButton* tapb1=new QPushButton(tr("Cancel"));
	QPushButton* tapb2=new QPushButton(tr("OK"));
	
	QHBoxLayout* buttons=new QHBoxLayout();
	buttons->addStretch();
	buttons->addWidget(tapb1);
	buttons->addWidget(tapb2);
	
	////////
	QGroupBox* selectorsGroupBox=new QGroupBox();
	selectorsGroupBox->setTitle(tr("Lock"));
	
	QHBoxLayout* groupLayout=new QHBoxLayout(selectorsGroupBox);
	
	QCheckBox* timeCheckBox=new QCheckBox();
	timeCheckBox->setChecked(true);
	timeCheckBox->setText(tr("Time"));
	QCheckBox* spaceCheckBox=new QCheckBox();
	spaceCheckBox->setChecked(true);
	spaceCheckBox->setText(tr("Space"));

	groupLayout->addWidget(timeCheckBox);
	groupLayout->addWidget(spaceCheckBox);
	groupLayout->addStretch();
	/////////
	
	taMainLayout->addWidget(taLabel);
	taMainLayout->addStretch();
	taMainLayout->addWidget(selectorsGroupBox);
	taMainLayout->addStretch();
	taMainLayout->addLayout(buttons);

	connect(tapb2, &QPushButton::clicked, &taDialog, &QDialog::accept);
	connect(tapb1, &QPushButton::clicked, &taDialog, &QDialog::reject);

	tapb2->setDefault(true);
	tapb2->setFocus();
	
	int w=taDialog.sizeHint().width();
	int h=taDialog.sizeHint().height();
	if(w<MIN_WIDTH)
		w=MIN_WIDTH;
	if(h<MIN_HEIGHT)
		h=MIN_HEIGHT;
	taDialog.resize(w,h);
	centerWidgetOnScreen(&taDialog);
	restoreFETDialogGeometry(&taDialog, lockEndStudentsDaySettingsString);
	int ok=taDialog.exec();
	saveFETDialogGeometry(&taDialog, lockEndStudentsDaySettingsString);
	if(ok==QDialog::Rejected)
		return;
		
	bool lockTime=timeCheckBox->isChecked();
	bool lockSpace=spaceCheckBox->isChecked();

	QString addedTimeConstraintsString;
	QString notAddedTimeConstraintsString;
	
	QList<ConstraintActivityPreferredStartingTime*> addedTimeConstraints;
	QList<ConstraintActivityPreferredStartingTime*> notAddedTimeConstraints;
	
	QString addedSpaceConstraintsString;
	QString notAddedSpaceConstraintsString;
	
	QList<ConstraintActivityPreferredRoom*> addedSpaceConstraints;
	QList<ConstraintActivityPreferredRoom*> notAddedSpaceConstraints;
	
	int addedTime=0, notAddedTime=0;
	int addedSpace=0, notAddedSpace=0;
	
	QList<int> activitiesIdsList;
	//QList<int> activitiesIndexList;
	QHash<int, int> activitiesIndexHash;
	QSet<int> activitiesIdsSet;
	for(int sg=0; sg<gt.rules.nInternalSubgroups; sg++){
		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			for(int h=gt.rules.nHoursPerDay-1; h>=0; h--){
				int ai=students_timetable_weekly[sg][d][h];
				
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					
					if(!activitiesIdsSet.contains(act->id)){
						activitiesIdsList.append(act->id);
						activitiesIdsSet.insert(act->id);
						assert(!activitiesIndexHash.contains(act->id));
						activitiesIndexHash.insert(act->id, ai);
					}
				
					break;
				}
			}
		}
	}
	
	std::stable_sort(activitiesIdsList.begin(), activitiesIdsList.end());

	assert(activitiesIdsList.count()==activitiesIndexHash.count());
	for(int q=0; q<activitiesIdsList.count(); q++){
		int id=activitiesIdsList.at(q);
		int ai=activitiesIndexHash.value(id, -1);
		assert(ai>=0);
		assert(gt.rules.internalActivitiesList[ai].id==id);

		assert(best_solution.times[ai]!=UNALLOCATED_TIME);
		if(best_solution.times[ai]!=UNALLOCATED_TIME){
			assert(best_solution.times[ai]>=0 && best_solution.times[ai]<gt.rules.nHoursPerWeek);
			int d=best_solution.times[ai]%gt.rules.nDaysPerWeek;
			int h=best_solution.times[ai]/gt.rules.nDaysPerWeek;
			
			ConstraintActivityPreferredStartingTime* newTimeCtr=nullptr;
			
			ConstraintActivityPreferredRoom* newSpaceCtr=nullptr;
			
			if(lockTime){
				newTimeCtr=new ConstraintActivityPreferredStartingTime(100.0, id, d, h, false);
			}
			
			if(lockSpace){
				if(best_solution.rooms[ai]!=UNALLOCATED_SPACE && best_solution.rooms[ai]!=UNSPECIFIED_ROOM){
					QStringList tl;
					if(gt.rules.internalRoomsList[best_solution.rooms[ai]]->isVirtual==false)
						assert(best_solution.realRoomsList[ai].isEmpty());
					else
						for(int rr : std::as_const(best_solution.realRoomsList[ai]))
							tl.append(gt.rules.internalRoomsList[rr]->name);

					newSpaceCtr=new ConstraintActivityPreferredRoom(100.0, id, gt.rules.internalRoomsList[best_solution.rooms[ai]]->name, tl, false);
				}
			}
			
			if(newTimeCtr!=nullptr){
				bool add=true;

				for(ConstraintActivityPreferredStartingTime* tc : gt.rules.apstHash.value(id, QSet<ConstraintActivityPreferredStartingTime*>())){
					if((*tc) == (*newTimeCtr)){
						add=false;
						break;
					}
				}
				
				if(add){
					addedTimeConstraintsString+=newTimeCtr->getDetailedDescription(gt.rules)+"\n";
					addedTimeConstraints.append(newTimeCtr);
	 				
	 				addedTime++;
				}
				else{
					notAddedTimeConstraintsString+=newTimeCtr->getDetailedDescription(gt.rules)+"\n";
					notAddedTimeConstraints.append(newTimeCtr);
					
					notAddedTime++;
				}
			}

			if(newSpaceCtr!=nullptr){
				bool add=true;

				for(ConstraintActivityPreferredRoom* tc : gt.rules.aprHash.value(id, QSet<ConstraintActivityPreferredRoom*>())){
					if((*tc) == (*newSpaceCtr)){
						add=false;
						break;
					}
				}
				
				if(add){
					addedSpaceConstraintsString+=newSpaceCtr->getDetailedDescription(gt.rules)+"\n";
					addedSpaceConstraints.append(newSpaceCtr);
	 				
	 				addedSpace++;
				}
				else{
					notAddedSpaceConstraintsString+=newSpaceCtr->getDetailedDescription(gt.rules)+"\n";
					notAddedSpaceConstraints.append(newSpaceCtr);
					
					notAddedSpace++;
				}
			}
		}
	}
	
	////////////
	//last confirmation dialog
	QDialog lastConfirmationDialog(&taDialog);
	lastConfirmationDialog.setWindowTitle(tr("Last confirmation needed"));

	QVBoxLayout* lastMainLayout=new QVBoxLayout(&lastConfirmationDialog);

	QPushButton* lastpb1=new QPushButton(tr("Cancel"));
	QPushButton* lastpb2=new QPushButton(tr("OK"));
	QHBoxLayout* lastButtons=new QHBoxLayout();
	lastButtons->addStretch();
	lastButtons->addWidget(lastpb1);
	lastButtons->addWidget(lastpb2);
	
	////texts
	QVBoxLayout* left=new QVBoxLayout();
	QVBoxLayout* right=new QVBoxLayout();
	QHBoxLayout* all=new QHBoxLayout();
	all->addLayout(left);
	all->addLayout(right);
	
	QTextEdit* addTim=new QTextEdit();
	addTim->setReadOnly(true);
	addTim->setPlainText(addedTimeConstraintsString);
	QLabel* labAddTim=new QLabel(tr("These time constraints will be added"));
	labAddTim->setWordWrap(true);
	labAddTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labAddTim);
	left->addWidget(addTim);

	QTextEdit* notAddTim=new QTextEdit();
	notAddTim->setReadOnly(true);
	notAddTim->setPlainText(notAddedTimeConstraintsString);
	QLabel* labNotAddTim=new QLabel(tr("These time constraints will NOT be added"));
	labNotAddTim->setWordWrap(true);
	labNotAddTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labNotAddTim);
	left->addWidget(notAddTim);

	QTextEdit* addSpa=new QTextEdit();
	addSpa->setReadOnly(true);
	addSpa->setPlainText(addedSpaceConstraintsString);
	QLabel* labAddSpa=new QLabel(tr("These space constraints will be added"));
	labAddSpa->setWordWrap(true);
	labAddSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labAddSpa);
	right->addWidget(addSpa);
	
	QTextEdit* notAddSpa=new QTextEdit();
	notAddSpa->setReadOnly(true);
	notAddSpa->setPlainText(notAddedSpaceConstraintsString);
	QLabel* labNotAddSpa=new QLabel(tr("These space constraints will NOT be added"));
	labNotAddSpa->setWordWrap(true);
	labNotAddSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labNotAddSpa);
	right->addWidget(notAddSpa);
	////end texts
	
	////////
	lastMainLayout->addLayout(all);
	//lastMainLayout->addStretch();
	lastMainLayout->addLayout(lastButtons);

	connect(lastpb2, &QPushButton::clicked, &lastConfirmationDialog, &QDialog::accept);
	connect(lastpb1, &QPushButton::clicked, &lastConfirmationDialog, &QDialog::reject);

	lastpb2->setDefault(true);
	lastpb2->setFocus();
	
	int lw=lastConfirmationDialog.sizeHint().width();
	int lh=lastConfirmationDialog.sizeHint().height();
	lastConfirmationDialog.resize(lw,lh);
	centerWidgetOnScreen(&lastConfirmationDialog);
	restoreFETDialogGeometry(&lastConfirmationDialog, lockEndStudentsDayConfirmationSettingsString);
	ok=lastConfirmationDialog.exec();
	saveFETDialogGeometry(&lastConfirmationDialog, lockEndStudentsDayConfirmationSettingsString);
	if(!ok)
		return;
	////////////
	
	for(TimeConstraint* tc : std::as_const(addedTimeConstraints)){
		bool t=gt.rules.addTimeConstraint(tc);
		assert(t);
	}
	addedTimeConstraints.clear();
	for(TimeConstraint* tc : std::as_const(notAddedTimeConstraints)){
		delete tc;
	}
	notAddedTimeConstraints.clear();
	
	for(SpaceConstraint* sc : std::as_const(addedSpaceConstraints)){
		bool t=gt.rules.addSpaceConstraint(sc);
		assert(t);
	}
	addedSpaceConstraints.clear();
	for(SpaceConstraint* sc : std::as_const(notAddedSpaceConstraints)){
		delete sc;
	}
	notAddedSpaceConstraints.clear();
	
	QMessageBox::information(&lastConfirmationDialog, tr("FET information"), tr("There were added %1 locking time constraints and"
		" %2 locking space constraints. There were not added %3 locking time constraints and %4 locking space constraints, because"
		" these activities were already locked").arg(addedTime).arg(addedSpace).arg(notAddedTime).arg(notAddedSpace));

	if(addedTime>0 || addedSpace>0)
		gt.rules.addUndoPoint(tr("Locked the activities which end students' days, by adding %1 locking time constraints and %2 locking space constraints.")
		 .arg(addedTime).arg(addedSpace));
	
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	LockUnlock::increaseCommunicationSpinBox();
}

void AdvancedLockUnlockForm::unlockEndStudentsDay(QWidget* parent)
{
	if(!students_schedule_ready || !teachers_schedule_ready || !rooms_buildings_schedule_ready){
		return;
	}
	
	//New Dialog
	QDialog taDialog(parent);
	taDialog.setWindowTitle(tr("FET - Unlock all activities which end each students set's day"));

	QVBoxLayout* taMainLayout=new QVBoxLayout(&taDialog);

	QLabel* taLabel=new QLabel();
	taLabel->setWordWrap(true);
	taLabel->setText(tr("All activities which end each students set's day will be unlocked"));
	taLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

	QPushButton* tapb1=new QPushButton(tr("Cancel"));
	QPushButton* tapb2=new QPushButton(tr("OK"));
	
	QHBoxLayout* buttons=new QHBoxLayout();
	buttons->addStretch();
	buttons->addWidget(tapb1);
	buttons->addWidget(tapb2);
	
	////////
	QGroupBox* selectorsGroupBox=new QGroupBox();
	selectorsGroupBox->setTitle(tr("Unlock"));
	
	QHBoxLayout* groupLayout=new QHBoxLayout(selectorsGroupBox);
	
	QCheckBox* timeCheckBox=new QCheckBox();
	timeCheckBox->setChecked(true);
	timeCheckBox->setText(tr("Time"));
	QCheckBox* spaceCheckBox=new QCheckBox();
	spaceCheckBox->setChecked(true);
	spaceCheckBox->setText(tr("Space"));

	groupLayout->addWidget(timeCheckBox);
	groupLayout->addWidget(spaceCheckBox);
	groupLayout->addStretch();
	/////////
	
	taMainLayout->addWidget(taLabel);
	taMainLayout->addStretch();
	taMainLayout->addWidget(selectorsGroupBox);
	taMainLayout->addStretch();
	taMainLayout->addLayout(buttons);

	connect(tapb2, &QPushButton::clicked, &taDialog, &QDialog::accept);
	connect(tapb1, &QPushButton::clicked, &taDialog, &QDialog::reject);

	tapb2->setDefault(true);
	tapb2->setFocus();
	
	int w=taDialog.sizeHint().width();
	int h=taDialog.sizeHint().height();
	if(w<MIN_WIDTH)
		w=MIN_WIDTH;
	if(h<MIN_HEIGHT)
		h=MIN_HEIGHT;
	taDialog.resize(w,h);
	centerWidgetOnScreen(&taDialog);
	restoreFETDialogGeometry(&taDialog, unlockEndStudentsDaySettingsString);
	int ok=taDialog.exec();
	saveFETDialogGeometry(&taDialog, unlockEndStudentsDaySettingsString);
	if(ok==QDialog::Rejected)
		return;
		
	bool unlockTime=timeCheckBox->isChecked();
	bool unlockSpace=spaceCheckBox->isChecked();

	int removedTime=0, notRemovedTime=0;
	int removedSpace=0, notRemovedSpace=0;
	
	QSet<QString> virtualRooms;
	for(Room* rm : std::as_const(gt.rules.roomsList))
		if(rm->isVirtual==true)
			virtualRooms.insert(rm->name);

	QList<int> activitiesIdsList;
	QSet<int> activitiesIdsSet;
	for(int sg=0; sg<gt.rules.nInternalSubgroups; sg++){
		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			for(int h=gt.rules.nHoursPerDay-1; h>=0; h--){
				int ai=students_timetable_weekly[sg][d][h];
				
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					
					if(!activitiesIdsSet.contains(act->id)){
						activitiesIdsSet.insert(act->id);
						activitiesIdsList.append(act->id);
					}
				
					break;
				}
			}
		}
	}
	
	std::stable_sort(activitiesIdsList.begin(), activitiesIdsList.end());

	QString removedTimeConstraintsString;
	QString notRemovedTimeConstraintsString;

	QList<TimeConstraint*> removedTimeConstraints;
	QList<TimeConstraint*> notRemovedTimeConstraints;
	
	QString removedSpaceConstraintsString;
	QString notRemovedSpaceConstraintsString;

	QList<SpaceConstraint*> removedSpaceConstraints;
	QList<SpaceConstraint*> notRemovedSpaceConstraints;
	
	if(unlockTime){
		for(int id : std::as_const(activitiesIdsList)){
			for(ConstraintActivityPreferredStartingTime* c : gt.rules.apstHash.value(id, QSet<ConstraintActivityPreferredStartingTime*>())){
				assert(id==c->activityId);
				if(c->weightPercentage==100.0 && c->day>=0 && c->hour>=0){
					if(!c->permanentlyLocked){
						removedTimeConstraints.append((TimeConstraint*)c);
						removedTimeConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						removedTime++;
					}
					else{
						notRemovedTimeConstraints.append((TimeConstraint*)c);
						notRemovedTimeConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						notRemovedTime++;
					}
				}
			}
		}
	}

	if(unlockSpace){
		for(int id : std::as_const(activitiesIdsList)){
			for(ConstraintActivityPreferredRoom* c : gt.rules.aprHash.value(id, QSet<ConstraintActivityPreferredRoom*>())){
				assert(id==c->activityId);
				if(c->weightPercentage==100.0 &&
				 (!virtualRooms.contains(c->roomName) || (virtualRooms.contains(c->roomName) && !c->preferredRealRoomsNames.isEmpty()))){
					if(!c->permanentlyLocked){
						removedSpaceConstraints.append((SpaceConstraint*)c);
						removedSpaceConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						removedSpace++;
					}
					else{
						notRemovedSpaceConstraints.append((SpaceConstraint*)c);
						notRemovedSpaceConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						notRemovedSpace++;
					}
				}
			}
		}
	}

	////////////
	//last confirmation dialog
	QDialog lastConfirmationDialog(&taDialog);
	lastConfirmationDialog.setWindowTitle(tr("Last confirmation needed"));

	QVBoxLayout* lastMainLayout=new QVBoxLayout(&lastConfirmationDialog);

	QPushButton* lastpb1=new QPushButton(tr("Cancel"));
	QPushButton* lastpb2=new QPushButton(tr("OK"));
	QHBoxLayout* lastButtons=new QHBoxLayout();
	lastButtons->addStretch();
	lastButtons->addWidget(lastpb1);
	lastButtons->addWidget(lastpb2);
	
	////texts
	QVBoxLayout* left=new QVBoxLayout();
	QVBoxLayout* right=new QVBoxLayout();
	QHBoxLayout* all=new QHBoxLayout();
	all->addLayout(left);
	all->addLayout(right);
	
	QTextEdit* remTim=new QTextEdit();
	remTim->setReadOnly(true);
	remTim->setPlainText(removedTimeConstraintsString);
	QLabel* labRemTim=new QLabel(tr("These time constraints will be removed"));
	labRemTim->setWordWrap(true);
	labRemTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labRemTim);
	left->addWidget(remTim);

	QTextEdit* notRemTim=new QTextEdit();
	notRemTim->setReadOnly(true);
	notRemTim->setPlainText(notRemovedTimeConstraintsString);
	QLabel* labNotRemTim=new QLabel(tr("These time constraints will NOT be removed"));
	labNotRemTim->setWordWrap(true);
	labNotRemTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labNotRemTim);
	left->addWidget(notRemTim);

	QTextEdit* remSpa=new QTextEdit();
	remSpa->setReadOnly(true);
	remSpa->setPlainText(removedSpaceConstraintsString);
	QLabel* labRemSpa=new QLabel(tr("These space constraints will be removed"));
	labRemSpa->setWordWrap(true);
	labRemSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labRemSpa);
	right->addWidget(remSpa);
	
	QTextEdit* notRemSpa=new QTextEdit();
	notRemSpa->setReadOnly(true);
	notRemSpa->setPlainText(notRemovedSpaceConstraintsString);
	QLabel* labNotRemSpa=new QLabel(tr("These space constraints will NOT be removed"));
	labNotRemSpa->setWordWrap(true);
	labNotRemSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labNotRemSpa);
	right->addWidget(notRemSpa);
	////end texts
	
	////////
	lastMainLayout->addLayout(all);
	//lastMainLayout->addStretch();
	lastMainLayout->addLayout(lastButtons);

	connect(lastpb2, &QPushButton::clicked, &lastConfirmationDialog, &QDialog::accept);
	connect(lastpb1, &QPushButton::clicked, &lastConfirmationDialog, &QDialog::reject);

	lastpb2->setDefault(true);
	lastpb2->setFocus();
	
	int lw=lastConfirmationDialog.sizeHint().width();
	int lh=lastConfirmationDialog.sizeHint().height();
	lastConfirmationDialog.resize(lw,lh);
	centerWidgetOnScreen(&lastConfirmationDialog);
	restoreFETDialogGeometry(&lastConfirmationDialog, unlockEndStudentsDayConfirmationSettingsString);
	ok=lastConfirmationDialog.exec();
	saveFETDialogGeometry(&lastConfirmationDialog, unlockEndStudentsDayConfirmationSettingsString);
	if(!ok)
		return;
	////////////
	
	bool t=gt.rules.removeTimeConstraints(removedTimeConstraints);
	assert(t);
	
	removedTimeConstraints.clear();
	notRemovedTimeConstraints.clear();

	t=gt.rules.removeSpaceConstraints(removedSpaceConstraints);
	assert(t);
	
	removedSpaceConstraints.clear();
	notRemovedSpaceConstraints.clear();

	QMessageBox::information(&lastConfirmationDialog, tr("FET information"), tr("There were removed %1 locking time constraints and"
		" %2 locking space constraints. There were not removed %3 locking time constraints and %4 locking space constraints, because"
		" these activities were permanently locked").arg(removedTime).arg(removedSpace).arg(notRemovedTime).arg(notRemovedSpace));

	if(removedTime>0 || removedSpace>0)
		gt.rules.addUndoPoint(tr("Unlocked the activities which end students' days, by removing %1 locking time constraints and %2 locking space constraints.")
		 .arg(removedTime).arg(removedSpace));
	
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	LockUnlock::increaseCommunicationSpinBox();
}

void AdvancedLockUnlockForm::lockAll(QWidget* parent)
{
	if(!students_schedule_ready || !teachers_schedule_ready || !rooms_buildings_schedule_ready){
		return;
	}
	
	//New Dialog
	QDialog taDialog(parent);
	taDialog.setWindowTitle(tr("FET - Lock all activities in the timetable"));

	QVBoxLayout* taMainLayout=new QVBoxLayout(&taDialog);

	QLabel* taLabel=new QLabel();
	taLabel->setWordWrap(true);
	taLabel->setText(tr("All activities in the current timetable will be locked"));
	taLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

	QPushButton* tapb1=new QPushButton(tr("Cancel"));
	QPushButton* tapb2=new QPushButton(tr("OK"));
	
	QHBoxLayout* buttons=new QHBoxLayout();
	buttons->addStretch();
	buttons->addWidget(tapb1);
	buttons->addWidget(tapb2);
	
	////////
	QGroupBox* selectorsGroupBox=new QGroupBox();
	selectorsGroupBox->setTitle(tr("Lock"));
	
	QHBoxLayout* groupLayout=new QHBoxLayout(selectorsGroupBox);
	
	QCheckBox* timeCheckBox=new QCheckBox();
	timeCheckBox->setChecked(true);
	timeCheckBox->setText(tr("Time"));
	QCheckBox* spaceCheckBox=new QCheckBox();
	spaceCheckBox->setChecked(true);
	spaceCheckBox->setText(tr("Space"));

	groupLayout->addWidget(timeCheckBox);
	groupLayout->addWidget(spaceCheckBox);
	groupLayout->addStretch();
	/////////
	
	taMainLayout->addWidget(taLabel);
	taMainLayout->addStretch();
	taMainLayout->addWidget(selectorsGroupBox);
	taMainLayout->addStretch();
	taMainLayout->addLayout(buttons);

	connect(tapb2, &QPushButton::clicked, &taDialog, &QDialog::accept);
	connect(tapb1, &QPushButton::clicked, &taDialog, &QDialog::reject);

	tapb2->setDefault(true);
	tapb2->setFocus();
	
	int w=taDialog.sizeHint().width();
	int h=taDialog.sizeHint().height();
	if(w<MIN_WIDTH)
		w=MIN_WIDTH;
	if(h<MIN_HEIGHT)
		h=MIN_HEIGHT;
	taDialog.resize(w,h);
	centerWidgetOnScreen(&taDialog);
	restoreFETDialogGeometry(&taDialog, lockAllSettingsString);
	int ok=taDialog.exec();
	saveFETDialogGeometry(&taDialog, lockAllSettingsString);
	if(ok==QDialog::Rejected)
		return;
		
	bool lockTime=timeCheckBox->isChecked();
	bool lockSpace=spaceCheckBox->isChecked();

	QString addedTimeConstraintsString;
	QString notAddedTimeConstraintsString;
	
	QList<ConstraintActivityPreferredStartingTime*> addedTimeConstraints;
	QList<ConstraintActivityPreferredStartingTime*> notAddedTimeConstraints;
	
	QString addedSpaceConstraintsString;
	QString notAddedSpaceConstraintsString;
	
	QList<ConstraintActivityPreferredRoom*> addedSpaceConstraints;
	QList<ConstraintActivityPreferredRoom*> notAddedSpaceConstraints;
	
	int addedTime=0, notAddedTime=0;
	int addedSpace=0, notAddedSpace=0;

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		if(best_solution.times[i]!=UNALLOCATED_TIME){
			assert(best_solution.times[i]>=0 && best_solution.times[i]<gt.rules.nHoursPerWeek);
			int d=best_solution.times[i]%gt.rules.nDaysPerWeek;
			int h=best_solution.times[i]/gt.rules.nDaysPerWeek;
			
			int id=gt.rules.internalActivitiesList[i].id;

			ConstraintActivityPreferredStartingTime* newTimeCtr=nullptr;
			
			ConstraintActivityPreferredRoom* newSpaceCtr=nullptr;
			
			if(lockTime){
				newTimeCtr=new ConstraintActivityPreferredStartingTime(100.0, gt.rules.internalActivitiesList[i].id, d, h, false);
			}
			
			if(lockSpace){
				if(best_solution.rooms[i]!=UNALLOCATED_SPACE && best_solution.rooms[i]!=UNSPECIFIED_ROOM){
					QStringList tl;
					if(gt.rules.internalRoomsList[best_solution.rooms[i]]->isVirtual==false)
						assert(best_solution.realRoomsList[i].isEmpty());
					else
						for(int rr : std::as_const(best_solution.realRoomsList[i]))
							tl.append(gt.rules.internalRoomsList[rr]->name);

					newSpaceCtr=new ConstraintActivityPreferredRoom(100.0, gt.rules.internalActivitiesList[i].id, gt.rules.internalRoomsList[best_solution.rooms[i]]->name, tl, false);
				}
			}
			
			
			if(newTimeCtr!=nullptr){
				bool add=true;
				
				for(ConstraintActivityPreferredStartingTime* tc : gt.rules.apstHash.value(id, QSet<ConstraintActivityPreferredStartingTime*>())){
					if((*tc) == (*newTimeCtr)){
						add=false;
						break;
					}
				}
				
				if(add){
					addedTimeConstraintsString+=newTimeCtr->getDetailedDescription(gt.rules)+"\n";
					addedTimeConstraints.append(newTimeCtr);
	 				
	 				addedTime++;
				}
				else{
					notAddedTimeConstraintsString+=newTimeCtr->getDetailedDescription(gt.rules)+"\n";
					notAddedTimeConstraints.append(newTimeCtr);
					
					notAddedTime++;
				}
			}

			if(newSpaceCtr!=nullptr){
				bool add=true;

				for(ConstraintActivityPreferredRoom* tc : gt.rules.aprHash.value(id, QSet<ConstraintActivityPreferredRoom*>())){
					if((*tc) == (*newSpaceCtr)){
						add=false;
						break;
					}
				}
				
				if(add){
					addedSpaceConstraintsString+=newSpaceCtr->getDetailedDescription(gt.rules)+"\n";
					addedSpaceConstraints.append(newSpaceCtr);
	 				
	 				addedSpace++;
				}
				else{
					notAddedSpaceConstraintsString+=newSpaceCtr->getDetailedDescription(gt.rules)+"\n";
					notAddedSpaceConstraints.append(newSpaceCtr);
					
					notAddedSpace++;
				}
			}
		}
	}
	
	////////////
	//last confirmation dialog
	QDialog lastConfirmationDialog(&taDialog);
	lastConfirmationDialog.setWindowTitle(tr("Last confirmation needed"));

	QVBoxLayout* lastMainLayout=new QVBoxLayout(&lastConfirmationDialog);

	QPushButton* lastpb1=new QPushButton(tr("Cancel"));
	QPushButton* lastpb2=new QPushButton(tr("OK"));
	QHBoxLayout* lastButtons=new QHBoxLayout();
	lastButtons->addStretch();
	lastButtons->addWidget(lastpb1);
	lastButtons->addWidget(lastpb2);
	
	////texts
	QVBoxLayout* left=new QVBoxLayout();
	QVBoxLayout* right=new QVBoxLayout();
	QHBoxLayout* all=new QHBoxLayout();
	all->addLayout(left);
	all->addLayout(right);
	
	QTextEdit* addTim=new QTextEdit();
	addTim->setReadOnly(true);
	addTim->setPlainText(addedTimeConstraintsString);
	QLabel* labAddTim=new QLabel(tr("These time constraints will be added"));
	labAddTim->setWordWrap(true);
	labAddTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labAddTim);
	left->addWidget(addTim);

	QTextEdit* notAddTim=new QTextEdit();
	notAddTim->setReadOnly(true);
	notAddTim->setPlainText(notAddedTimeConstraintsString);
	QLabel* labNotAddTim=new QLabel(tr("These time constraints will NOT be added"));
	labNotAddTim->setWordWrap(true);
	labNotAddTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labNotAddTim);
	left->addWidget(notAddTim);

	QTextEdit* addSpa=new QTextEdit();
	addSpa->setReadOnly(true);
	addSpa->setPlainText(addedSpaceConstraintsString);
	QLabel* labAddSpa=new QLabel(tr("These space constraints will be added"));
	labAddSpa->setWordWrap(true);
	labAddSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labAddSpa);
	right->addWidget(addSpa);
	
	QTextEdit* notAddSpa=new QTextEdit();
	notAddSpa->setReadOnly(true);
	notAddSpa->setPlainText(notAddedSpaceConstraintsString);
	QLabel* labNotAddSpa=new QLabel(tr("These space constraints will NOT be added"));
	labNotAddSpa->setWordWrap(true);
	labNotAddSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labNotAddSpa);
	right->addWidget(notAddSpa);
	////end texts
	
	////////
	lastMainLayout->addLayout(all);
	//lastMainLayout->addStretch();
	lastMainLayout->addLayout(lastButtons);

	connect(lastpb2, &QPushButton::clicked, &lastConfirmationDialog, &QDialog::accept);
	connect(lastpb1, &QPushButton::clicked, &lastConfirmationDialog, &QDialog::reject);

	lastpb2->setDefault(true);
	lastpb2->setFocus();
	
	int lw=lastConfirmationDialog.sizeHint().width();
	int lh=lastConfirmationDialog.sizeHint().height();
	lastConfirmationDialog.resize(lw,lh);
	centerWidgetOnScreen(&lastConfirmationDialog);
	restoreFETDialogGeometry(&lastConfirmationDialog, lockAllConfirmationSettingsString);
	ok=lastConfirmationDialog.exec();
	saveFETDialogGeometry(&lastConfirmationDialog, lockAllConfirmationSettingsString);
	if(!ok)
		return;
	////////////
	
	for(TimeConstraint* tc : std::as_const(addedTimeConstraints)){
		bool t=gt.rules.addTimeConstraint(tc);
		assert(t);
	}
	addedTimeConstraints.clear();
	for(TimeConstraint* tc : std::as_const(notAddedTimeConstraints)){
		delete tc;
	}
	notAddedTimeConstraints.clear();
	
	for(SpaceConstraint* sc : std::as_const(addedSpaceConstraints)){
		bool t=gt.rules.addSpaceConstraint(sc);
		assert(t);
	}
	addedSpaceConstraints.clear();
	for(SpaceConstraint* sc : std::as_const(notAddedSpaceConstraints)){
		delete sc;
	}
	notAddedSpaceConstraints.clear();
	
	QMessageBox::information(&lastConfirmationDialog, tr("FET information"), tr("There were added %1 locking time constraints and"
		" %2 locking space constraints. There were not added %3 locking time constraints and %4 locking space constraints, because"
		" these activities were already locked").arg(addedTime).arg(addedSpace).arg(notAddedTime).arg(notAddedSpace));

	if(addedTime>0 || addedSpace>0)
		gt.rules.addUndoPoint(tr("Locked all the activities in the timetable, by adding %1 locking time constraints and %2 locking space constraints.")
		 .arg(addedTime).arg(addedSpace));
	
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	LockUnlock::increaseCommunicationSpinBox();

	//cout<<"isc=="<<gt.rules.internalStructureComputed<<endl;
}

/*void AdvancedLockUnlockForm::unlockAll(QWidget* parent)
{
	if(!students_schedule_ready || !teachers_schedule_ready || !rooms_buildings_schedule_ready){
		return;
	}
	
	//New Dialog
	QDialog taDialog(parent);
	taDialog.setWindowTitle(tr("FET - Unlock all activities of the current timetable"));

	QVBoxLayout* taMainLayout=new QVBoxLayout(&taDialog);

	QLabel* taLabel=new QLabel();
	taLabel->setWordWrap(true);
	taLabel->setText(tr("All activities of the current timetable will be unlocked (those which are not permanently locked)"));
	taLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

	QPushButton* tapb1=new QPushButton(tr("Cancel"));
	QPushButton* tapb2=new QPushButton(tr("OK"));
	
	QHBoxLayout* buttons=new QHBoxLayout();
	buttons->addStretch();
	buttons->addWidget(tapb1);
	buttons->addWidget(tapb2);
	
	////////
	QGroupBox* selectorsGroupBox=new QGroupBox();
	selectorsGroupBox->setTitle(tr("Unlock"));
	
	QHBoxLayout* groupLayout=new QHBoxLayout(selectorsGroupBox);
	
	QCheckBox* timeCheckBox=new QCheckBox();
	timeCheckBox->setChecked(true);
	timeCheckBox->setText(tr("Time"));
	QCheckBox* spaceCheckBox=new QCheckBox();
	spaceCheckBox->setChecked(true);
	spaceCheckBox->setText(tr("Space"));

	groupLayout->addWidget(timeCheckBox);
	groupLayout->addWidget(spaceCheckBox);
	groupLayout->addStretch();
	/////////
	
	taMainLayout->addWidget(taLabel);
	taMainLayout->addStretch();
	taMainLayout->addWidget(selectorsGroupBox);
	taMainLayout->addStretch();
	taMainLayout->addLayout(buttons);

	QObj ect::connect(tapb2, SIG NAL(clicked()), &taDialog, SL OT(accept()));
	QObj ect::connect(tapb1, SIG NAL(clicked()), &taDialog, SL OT(reject()));

	tapb2->setDefault(true);
	tapb2->setFocus();
	
	int w=taDialog.sizeHint().width();
	int h=taDialog.sizeHint().height();
	if(w<MIN_WIDTH)
		w=MIN_WIDTH;
	if(h<MIN_HEIGHT)
		h=MIN_HEIGHT;
	taDialog.resize(w,h);
	centerWidgetOnScreen(&taDialog);
	restoreFETDialogGeometry(&taDialog, unlockAllSettingsString);
	bool ok=taDialog.exec();
	saveFETDialogGeometry(&taDialog, unlockAllSettingsString);
	if(!ok)
		return;
		
	bool unlockTime=timeCheckBox->isChecked();
	bool unlockSpace=spaceCheckBox->isChecked();

	int removedTime=0, notRemovedTime=0;
	int removedSpace=0, notRemovedSpace=0;
	
	QList<int> lockedActivitiesIds;

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		if(best_solution.times[i]!=UNALLOCATED_TIME){
			assert(best_solution.times[i]>=0 && best_solution.times[i]<gt.rules.nHoursPerWeek);
			lockedActivitiesIds.append(gt.rules.internalActivitiesList[i].id);
		}
	}

	QString removedTimeConstraintsString;
	QString notRemovedTimeConstraintsString;

	QList<TimeConstraint*> removedTimeConstraints;
	QList<TimeConstraint*> notRemovedTimeConstraints;
	
	QString removedSpaceConstraintsString;
	QString notRemovedSpaceConstraintsString;

	QList<SpaceConstraint*> removedSpaceConstraints;
	QList<SpaceConstraint*> notRemovedSpaceConstraints;
	
	if(unlockTime){
		for(int id : std::as_const(lockedActivitiesIds)){
			for(ConstraintActivityPreferredStartingTime* c : gt.rules.apstHash.value(id, QSet<ConstraintActivityPreferredStartingTime*>())){
				assert(id==c->activityId);
				if(c->weightPercentage==100.0 && c->day>=0 && c->hour>=0){
					if(!c->permanentlyLocked){
						removedTimeConstraints.append((TimeConstraint*)c);
						removedTimeConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						removedTime++;
					}
					else{
						notRemovedTimeConstraints.append((TimeConstraint*)c);
						notRemovedTimeConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						notRemovedTime++;
					}
				}
			}
		}
	}

	if(unlockSpace){
		for(int id : std::as_const(lockedActivitiesIds)){
			for(ConstraintActivityPreferredRoom* c : gt.rules.aprHash.value(id, QSet<ConstraintActivityPreferredRoom*>())){
				assert(id==c->activityId);
				if(c->weightPercentage==100.0){
					if(!c->permanentlyLocked){
						removedSpaceConstraints.append((SpaceConstraint*)c);
						removedSpaceConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						removedSpace++;
					}
					else{
						notRemovedSpaceConstraints.append((SpaceConstraint*)c);
						notRemovedSpaceConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						notRemovedSpace++;
					}
				}
			}
		}
	}

	////////////
	//last confirmation dialog
	QDialog lastConfirmationDialog(&taDialog);
	lastConfirmationDialog.setWindowTitle(tr("Last confirmation needed"));

	QVBoxLayout* lastMainLayout=new QVBoxLayout(&lastConfirmationDialog);

	QPushButton* lastpb1=new QPushButton(tr("Cancel"));
	QPushButton* lastpb2=new QPushButton(tr("OK"));
	QHBoxLayout* lastButtons=new QHBoxLayout();
	lastButtons->addStretch();
	lastButtons->addWidget(lastpb1);
	lastButtons->addWidget(lastpb2);
	
	////texts
	QVBoxLayout* left=new QVBoxLayout();
	QVBoxLayout* right=new QVBoxLayout();
	QHBoxLayout* all=new QHBoxLayout();
	all->addLayout(left);
	all->addLayout(right);
	
	QTextEdit* remTim=new QTextEdit();
	remTim->setReadOnly(true);
	remTim->setPlainText(removedTimeConstraintsString);
	QLabel* labRemTim=new QLabel(tr("These time constraints will be removed"));
	labRemTim->setWordWrap(true);
	labRemTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labRemTim);
	left->addWidget(remTim);

	QTextEdit* notRemTim=new QTextEdit();
	notRemTim->setReadOnly(true);
	notRemTim->setPlainText(notRemovedTimeConstraintsString);
	QLabel* labNotRemTim=new QLabel(tr("These time constraints will NOT be removed"));
	labNotRemTim->setWordWrap(true);
	labNotRemTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labNotRemTim);
	left->addWidget(notRemTim);

	QTextEdit* remSpa=new QTextEdit();
	remSpa->setReadOnly(true);
	remSpa->setPlainText(removedSpaceConstraintsString);
	QLabel* labRemSpa=new QLabel(tr("These space constraints will be removed"));
	labRemSpa->setWordWrap(true);
	labRemSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labRemSpa);
	right->addWidget(remSpa);
	
	QTextEdit* notRemSpa=new QTextEdit();
	notRemSpa->setReadOnly(true);
	notRemSpa->setPlainText(notRemovedSpaceConstraintsString);
	QLabel* labNotRemSpa=new QLabel(tr("These space constraints will NOT be removed"));
	labNotRemSpa->setWordWrap(true);
	labNotRemSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labNotRemSpa);
	right->addWidget(notRemSpa);
	////end texts
	
	////////
	lastMainLayout->addLayout(all);
	//lastMainLayout->addStretch();
	lastMainLayout->addLayout(lastButtons);

	QObj ect::connect(lastpb2, SIG NAL(clicked()), &lastConfirmationDialog, SL OT(accept()));
	QObj ect::connect(lastpb1, SIG NAL(clicked()), &lastConfirmationDialog, SL OT(reject()));

	lastpb2->setDefault(true);
	lastpb2->setFocus();
	
	int lw=lastConfirmationDialog.sizeHint().width();
	int lh=lastConfirmationDialog.sizeHint().height();
	lastConfirmationDialog.resize(lw,lh);
	centerWidgetOnScreen(&lastConfirmationDialog);
	restoreFETDialogGeometry(&lastConfirmationDialog, unlockAllConfirmationSettingsString);
	ok=lastConfirmationDialog.exec();
	saveFETDialogGeometry(&lastConfirmationDialog, unlockAllConfirmationSettingsString);
	if(!ok)
		return;
	////////////
	
	bool t=gt.rules.removeTimeConstraints(removedTimeConstraints);
	assert(t);
	
	removedTimeConstraints.clear();
	notRemovedTimeConstraints.clear();

	t=gt.rules.removeSpaceConstraints(removedSpaceConstraints);
	assert(t);
	
	removedSpaceConstraints.clear();
	notRemovedSpaceConstraints.clear();

	QMessageBox::information(&lastConfirmationDialog, tr("FET information"), tr("There were removed %1 locking time constraints and"
		" %2 locking space constraints. There were not removed %3 locking time constraints and %4 locking space constraints, because"
		" these activities were permanently locked").arg(removedTime).arg(removedSpace).arg(notRemovedTime).arg(notRemovedSpace));
	
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	LockUnlock::increaseCommunicationSpinBox();
	
	//cout<<"isc=="<<gt.rules.internalStructureComputed<<endl;
}*/

void AdvancedLockUnlockForm::unlockAllWithoutTimetable(QWidget* parent)
{
	/*if(!students_schedule_ready || !teachers_schedule_ready || !rooms_buildings_schedule_ready){
		return;
	}*/
	
	QSet<QString> virtualRooms;
	for(Room* rm : std::as_const(gt.rules.roomsList))
		if(rm->isVirtual==true)
			virtualRooms.insert(rm->name);
	
	//New Dialog
	QDialog taDialog(parent);
	taDialog.setWindowTitle(tr("FET - Unlock all activities without a generated timetable"));

	QVBoxLayout* taMainLayout=new QVBoxLayout(&taDialog);

	QLabel* taLabel=new QLabel();
	taLabel->setWordWrap(true);
	taLabel->setText(tr("All activities will be unlocked (those which are not permanently locked)"));
	taLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

	QPushButton* tapb1=new QPushButton(tr("Cancel"));
	QPushButton* tapb2=new QPushButton(tr("OK"));
	
	QHBoxLayout* buttons=new QHBoxLayout();
	buttons->addStretch();
	buttons->addWidget(tapb1);
	buttons->addWidget(tapb2);
	
	////////
	QGroupBox* selectorsGroupBox=new QGroupBox();
	selectorsGroupBox->setTitle(tr("Unlock"));
	
	QHBoxLayout* groupLayout=new QHBoxLayout(selectorsGroupBox);
	
	QCheckBox* timeCheckBox=new QCheckBox();
	timeCheckBox->setChecked(true);
	timeCheckBox->setText(tr("Time"));
	QCheckBox* spaceCheckBox=new QCheckBox();
	spaceCheckBox->setChecked(true);
	spaceCheckBox->setText(tr("Space"));

	groupLayout->addWidget(timeCheckBox);
	groupLayout->addWidget(spaceCheckBox);
	groupLayout->addStretch();
	/////////
	
	taMainLayout->addWidget(taLabel);
	taMainLayout->addStretch();
	taMainLayout->addWidget(selectorsGroupBox);
	taMainLayout->addStretch();
	taMainLayout->addLayout(buttons);

	connect(tapb2, &QPushButton::clicked, &taDialog, &QDialog::accept);
	connect(tapb1, &QPushButton::clicked, &taDialog, &QDialog::reject);

	tapb2->setDefault(true);
	tapb2->setFocus();
	
	int w=taDialog.sizeHint().width();
	int h=taDialog.sizeHint().height();
	if(w<MIN_WIDTH)
		w=MIN_WIDTH;
	if(h<MIN_HEIGHT)
		h=MIN_HEIGHT;
	taDialog.resize(w,h);
	centerWidgetOnScreen(&taDialog);
	restoreFETDialogGeometry(&taDialog, unlockAllSettingsString);
	int ok=taDialog.exec();
	saveFETDialogGeometry(&taDialog, unlockAllSettingsString);
	if(ok==QDialog::Rejected)
		return;
		
	bool unlockTime=timeCheckBox->isChecked();
	bool unlockSpace=spaceCheckBox->isChecked();

	int removedTime=0, notRemovedTime=0;
	int removedSpace=0, notRemovedSpace=0;
	
	/*QList<int> lockedActivitiesIds;

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		if(best_solution.times[i]!=UNALLOCATED_TIME){
			assert(best_solution.times[i]>=0 && best_solution.times[i]<gt.rules.nHoursPerWeek);
			lockedActivitiesIds.append(gt.rules.internalActivitiesList[i].id);
		}
	}*/

	QString removedTimeConstraintsString;
	QString notRemovedTimeConstraintsString;

	QList<TimeConstraint*> removedTimeConstraints;
	QList<TimeConstraint*> notRemovedTimeConstraints;
	
	QString removedSpaceConstraintsString;
	QString notRemovedSpaceConstraintsString;

	QList<SpaceConstraint*> removedSpaceConstraints;
	QList<SpaceConstraint*> notRemovedSpaceConstraints;
	
	if(unlockTime)
		for(TimeConstraint* tc : std::as_const(gt.rules.timeConstraintsList))
			if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME){
				ConstraintActivityPreferredStartingTime* c=(ConstraintActivityPreferredStartingTime*)tc;
				if(tc->weightPercentage==100.0 && c->day>=0 && c->hour>=0){
					if(!c->permanentlyLocked){
						removedTimeConstraints.append((TimeConstraint*)c);
						//removedTimeConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						removedTime++;
					}
					else{
						notRemovedTimeConstraints.append((TimeConstraint*)c);
						//notRemovedTimeConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						notRemovedTime++;
					}
				}
			}
	
	if(unlockSpace)
		for(SpaceConstraint* sc : std::as_const(gt.rules.spaceConstraintsList))
			if(sc->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
				ConstraintActivityPreferredRoom* c=(ConstraintActivityPreferredRoom*)sc;
				if(c->weightPercentage==100.0 &&
				 (!virtualRooms.contains(c->roomName) || (virtualRooms.contains(c->roomName) && !c->preferredRealRoomsNames.isEmpty()))){
					if(!c->permanentlyLocked){
						removedSpaceConstraints.append((SpaceConstraint*)c);
						//removedSpaceConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						removedSpace++;
					}
					else{
						notRemovedSpaceConstraints.append((SpaceConstraint*)c);
						//notRemovedSpaceConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						notRemovedSpace++;
					}
				}
			}
			
	std::stable_sort(removedTimeConstraints.begin(), removedTimeConstraints.end(), compareTimeConstraintsActivityPreferredStartingTimeActivitiesIds);
	for(TimeConstraint* tc : std::as_const(removedTimeConstraints))
		removedTimeConstraintsString+=tc->getDetailedDescription(gt.rules)+"\n";
		
	std::stable_sort(notRemovedTimeConstraints.begin(), notRemovedTimeConstraints.end(), compareTimeConstraintsActivityPreferredStartingTimeActivitiesIds);
	for(TimeConstraint* tc : std::as_const(notRemovedTimeConstraints))
		notRemovedTimeConstraintsString+=tc->getDetailedDescription(gt.rules)+"\n";

	std::stable_sort(removedSpaceConstraints.begin(), removedSpaceConstraints.end(), compareSpaceConstraintsActivityPreferredRoomActivitiesIds);
	for(SpaceConstraint* sc : std::as_const(removedSpaceConstraints))
		removedSpaceConstraintsString+=sc->getDetailedDescription(gt.rules)+"\n";

	std::stable_sort(notRemovedSpaceConstraints.begin(), notRemovedSpaceConstraints.end(), compareSpaceConstraintsActivityPreferredRoomActivitiesIds);
	for(SpaceConstraint* sc : std::as_const(notRemovedSpaceConstraints))
		notRemovedSpaceConstraintsString+=sc->getDetailedDescription(gt.rules)+"\n";

	////////////
	//last confirmation dialog
	QDialog lastConfirmationDialog(&taDialog);
	lastConfirmationDialog.setWindowTitle(tr("Last confirmation needed"));

	QVBoxLayout* lastMainLayout=new QVBoxLayout(&lastConfirmationDialog);

	QPushButton* lastpb1=new QPushButton(tr("Cancel"));
	QPushButton* lastpb2=new QPushButton(tr("OK"));
	QHBoxLayout* lastButtons=new QHBoxLayout();
	lastButtons->addStretch();
	lastButtons->addWidget(lastpb1);
	lastButtons->addWidget(lastpb2);
	
	////texts
	QVBoxLayout* left=new QVBoxLayout();
	QVBoxLayout* right=new QVBoxLayout();
	QHBoxLayout* all=new QHBoxLayout();
	all->addLayout(left);
	all->addLayout(right);
	
	QTextEdit* remTim=new QTextEdit();
	remTim->setReadOnly(true);
	remTim->setPlainText(removedTimeConstraintsString);
	QLabel* labRemTim=new QLabel(tr("These time constraints will be removed"));
	labRemTim->setWordWrap(true);
	labRemTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labRemTim);
	left->addWidget(remTim);

	QTextEdit* notRemTim=new QTextEdit();
	notRemTim->setReadOnly(true);
	notRemTim->setPlainText(notRemovedTimeConstraintsString);
	QLabel* labNotRemTim=new QLabel(tr("These time constraints will NOT be removed"));
	labNotRemTim->setWordWrap(true);
	labNotRemTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labNotRemTim);
	left->addWidget(notRemTim);

	QTextEdit* remSpa=new QTextEdit();
	remSpa->setReadOnly(true);
	remSpa->setPlainText(removedSpaceConstraintsString);
	QLabel* labRemSpa=new QLabel(tr("These space constraints will be removed"));
	labRemSpa->setWordWrap(true);
	labRemSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labRemSpa);
	right->addWidget(remSpa);
	
	QTextEdit* notRemSpa=new QTextEdit();
	notRemSpa->setReadOnly(true);
	notRemSpa->setPlainText(notRemovedSpaceConstraintsString);
	QLabel* labNotRemSpa=new QLabel(tr("These space constraints will NOT be removed"));
	labNotRemSpa->setWordWrap(true);
	labNotRemSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labNotRemSpa);
	right->addWidget(notRemSpa);
	////end texts
	
	////////
	lastMainLayout->addLayout(all);
	//lastMainLayout->addStretch();
	lastMainLayout->addLayout(lastButtons);

	connect(lastpb2, &QPushButton::clicked, &lastConfirmationDialog, &QDialog::accept);
	connect(lastpb1, &QPushButton::clicked, &lastConfirmationDialog, &QDialog::reject);

	lastpb2->setDefault(true);
	lastpb2->setFocus();
	
	int lw=lastConfirmationDialog.sizeHint().width();
	int lh=lastConfirmationDialog.sizeHint().height();
	lastConfirmationDialog.resize(lw,lh);
	centerWidgetOnScreen(&lastConfirmationDialog);
	restoreFETDialogGeometry(&lastConfirmationDialog, unlockAllConfirmationSettingsString);
	ok=lastConfirmationDialog.exec();
	saveFETDialogGeometry(&lastConfirmationDialog, unlockAllConfirmationSettingsString);
	if(!ok)
		return;
	////////////
	
	bool t=gt.rules.removeTimeConstraints(removedTimeConstraints);
	assert(t);
	
	removedTimeConstraints.clear();
	notRemovedTimeConstraints.clear();

	t=gt.rules.removeSpaceConstraints(removedSpaceConstraints);
	assert(t);
	
	removedSpaceConstraints.clear();
	notRemovedSpaceConstraints.clear();

	QMessageBox::information(&lastConfirmationDialog, tr("FET information"), tr("There were removed %1 locking time constraints and"
		" %2 locking space constraints. There were not removed %3 locking time constraints and %4 locking space constraints, because"
		" these activities were permanently locked").arg(removedTime).arg(removedSpace).arg(notRemovedTime).arg(notRemovedSpace));

	if(removedTime>0 || removedSpace>0)
		gt.rules.addUndoPoint(tr("Unlocked all the activities in the timetable, by removing %1 locking time constraints and %2 locking space constraints.")
		 .arg(removedTime).arg(removedSpace));
	
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	LockUnlock::increaseCommunicationSpinBox();
	
	//cout<<"isc=="<<gt.rules.internalStructureComputed<<endl;
}

void AdvancedLockUnlockForm::unlockDayWithoutTimetable(QWidget* parent)
{
	/*if(!students_schedule_ready || !teachers_schedule_ready || !rooms_buildings_schedule_ready){
		return;
	}*/
	
	QSet<QString> virtualRooms;
	for(Room* rm : std::as_const(gt.rules.roomsList))
		if(rm->isVirtual==true)
			virtualRooms.insert(rm->name);
	
	QStringList days;
	for(int j=0; j<gt.rules.nDaysPerWeek; j++)
		days<<gt.rules.daysOfTheWeek[j];
	assert(days.size()!=0);

	//New Dialog
	QDialog taDialog(parent);
	taDialog.setWindowTitle(tr("FET - Unlock activities of a day"));

	QVBoxLayout* taMainLayout=new QVBoxLayout(&taDialog);

	QLabel* taLabel=new QLabel();
	taLabel->setWordWrap(true);
	taLabel->setText(tr("All activities of the selected day will be unlocked (those which are not permanently locked)")+"\n\n"+tr("Please select the day to unlock:"));
	taLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

	QComboBox* taLW=new QComboBox();
	
	QSize tmp2=taLW->minimumSizeHint();
	Q_UNUSED(tmp2);
	
	taLW->addItems(days);
	taLW->setCurrentIndex(0);

	QPushButton* tapb1=new QPushButton(tr("Cancel"));
	QPushButton* tapb2=new QPushButton(tr("OK"));
	
	QHBoxLayout* buttons=new QHBoxLayout();
	buttons->addStretch();
	buttons->addWidget(tapb1);
	buttons->addWidget(tapb2);
	
	////////
	QGroupBox* selectorsGroupBox=new QGroupBox();
	selectorsGroupBox->setTitle(tr("Unlock"));
	
	QHBoxLayout* groupLayout=new QHBoxLayout(selectorsGroupBox);
	
	QCheckBox* timeCheckBox=new QCheckBox();
	timeCheckBox->setChecked(true);
	timeCheckBox->setText(tr("Time"));
	QCheckBox* spaceCheckBox=new QCheckBox();
	spaceCheckBox->setChecked(true);
	spaceCheckBox->setText(tr("Space"));

	groupLayout->addWidget(timeCheckBox);
	groupLayout->addWidget(spaceCheckBox);
	groupLayout->addStretch();
	/////////
	
	taMainLayout->addWidget(taLabel);
	taMainLayout->addWidget(taLW);
	taMainLayout->addStretch();
	taMainLayout->addWidget(selectorsGroupBox);
	taMainLayout->addStretch();
	taMainLayout->addLayout(buttons);

	connect(tapb2, &QPushButton::clicked, &taDialog, &QDialog::accept);
	connect(tapb1, &QPushButton::clicked, &taDialog, &QDialog::reject);

	tapb2->setDefault(true);
	tapb2->setFocus();
	
	int w=taDialog.sizeHint().width();
	int h=taDialog.sizeHint().height();
	if(w<MIN_WIDTH)
		w=MIN_WIDTH;
	if(h<MIN_HEIGHT)
		h=MIN_HEIGHT;
	taDialog.resize(w,h);
	centerWidgetOnScreen(&taDialog);
	restoreFETDialogGeometry(&taDialog, unlockDaySettingsString);
	int ok=taDialog.exec();
	saveFETDialogGeometry(&taDialog, unlockDaySettingsString);
	if(ok==QDialog::Rejected)
		return;
		
	bool unlockTime=timeCheckBox->isChecked();
	bool unlockSpace=spaceCheckBox->isChecked();

	int selectedDayInt=taLW->currentIndex();
	assert(selectedDayInt>=0 && selectedDayInt<gt.rules.nDaysPerWeek);

	int removedTime=0, notRemovedTime=0;
	int removedSpace=0, notRemovedSpace=0;
	
	/*QList<int> lockedActivitiesIds;

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		if(best_solution.times[i]!=UNALLOCATED_TIME){
			assert(best_solution.times[i]>=0 && best_solution.times[i]<gt.rules.nHoursPerWeek);
			int d=best_solution.times[i]%gt.rules.nDaysPerWeek;
			//int h=best_solution.times[i]/gt.rules.nDaysPerWeek;
			
			if(d==selectedDayInt){
				lockedActivitiesIds.append(gt.rules.internalActivitiesList[i].id);
			}
		}
	}*/

	QString removedTimeConstraintsString;
	QString notRemovedTimeConstraintsString;

	QList<TimeConstraint*> removedTimeConstraints;
	QList<TimeConstraint*> notRemovedTimeConstraints;
	
	QString removedSpaceConstraintsString;
	QString notRemovedSpaceConstraintsString;

	QList<SpaceConstraint*> removedSpaceConstraints;
	QList<SpaceConstraint*> notRemovedSpaceConstraints;
	
	QSet<int> actsSet;
	
	if(unlockTime || unlockSpace) // ||unlockSpace is added on 2018-06-26, correcting a bug if the user only selected Space and the timetable was not generated.
		for(TimeConstraint* tc : std::as_const(gt.rules.timeConstraintsList))
			if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME){
				ConstraintActivityPreferredStartingTime* c=(ConstraintActivityPreferredStartingTime*)tc;
				if(c->weightPercentage==100.0 && c->day>=0 && c->hour>=0 && c->day==selectedDayInt){
					actsSet.insert(c->activityId);
					if(unlockTime){
						if(!c->permanentlyLocked){
							removedTimeConstraints.append((TimeConstraint*)c);
							//removedTimeConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
							removedTime++;
						}
						else{
							notRemovedTimeConstraints.append((TimeConstraint*)c);
							//notRemovedTimeConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
							notRemovedTime++;
						}
					}
				}
			}
			
	if(unlockSpace)
		for(SpaceConstraint* sc : std::as_const(gt.rules.spaceConstraintsList))
			if(sc->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
				ConstraintActivityPreferredRoom* c=(ConstraintActivityPreferredRoom*) sc;
				if(c->weightPercentage==100.0 && actsSet.contains(c->activityId) &&
				 (!virtualRooms.contains(c->roomName) || (virtualRooms.contains(c->roomName) && !c->preferredRealRoomsNames.isEmpty()))){
					if(!c->permanentlyLocked){
						removedSpaceConstraints.append((SpaceConstraint*)c);
						//removedSpaceConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						removedSpace++;
					}
					else{
						notRemovedSpaceConstraints.append((SpaceConstraint*)c);
						//notRemovedSpaceConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						notRemovedSpace++;
					}
				}
			}

	std::stable_sort(removedTimeConstraints.begin(), removedTimeConstraints.end(), compareTimeConstraintsActivityPreferredStartingTimeActivitiesIds);
	for(TimeConstraint* tc : std::as_const(removedTimeConstraints))
		removedTimeConstraintsString+=tc->getDetailedDescription(gt.rules)+"\n";
		
	std::stable_sort(notRemovedTimeConstraints.begin(), notRemovedTimeConstraints.end(), compareTimeConstraintsActivityPreferredStartingTimeActivitiesIds);
	for(TimeConstraint* tc : std::as_const(notRemovedTimeConstraints))
		notRemovedTimeConstraintsString+=tc->getDetailedDescription(gt.rules)+"\n";

	std::stable_sort(removedSpaceConstraints.begin(), removedSpaceConstraints.end(), compareSpaceConstraintsActivityPreferredRoomActivitiesIds);
	for(SpaceConstraint* sc : std::as_const(removedSpaceConstraints))
		removedSpaceConstraintsString+=sc->getDetailedDescription(gt.rules)+"\n";

	std::stable_sort(notRemovedSpaceConstraints.begin(), notRemovedSpaceConstraints.end(), compareSpaceConstraintsActivityPreferredRoomActivitiesIds);
	for(SpaceConstraint* sc : std::as_const(notRemovedSpaceConstraints))
		notRemovedSpaceConstraintsString+=sc->getDetailedDescription(gt.rules)+"\n";

	////////////
	//last confirmation dialog
	QDialog lastConfirmationDialog(&taDialog);
	lastConfirmationDialog.setWindowTitle(tr("Last confirmation needed"));

	QVBoxLayout* lastMainLayout=new QVBoxLayout(&lastConfirmationDialog);

	QPushButton* lastpb1=new QPushButton(tr("Cancel"));
	QPushButton* lastpb2=new QPushButton(tr("OK"));
	QHBoxLayout* lastButtons=new QHBoxLayout();
	lastButtons->addStretch();
	lastButtons->addWidget(lastpb1);
	lastButtons->addWidget(lastpb2);
	
	////texts
	QVBoxLayout* left=new QVBoxLayout();
	QVBoxLayout* right=new QVBoxLayout();
	QHBoxLayout* all=new QHBoxLayout();
	all->addLayout(left);
	all->addLayout(right);
	
	QTextEdit* remTim=new QTextEdit();
	remTim->setReadOnly(true);
	remTim->setPlainText(removedTimeConstraintsString);
	QLabel* labRemTim=new QLabel(tr("These time constraints will be removed"));
	labRemTim->setWordWrap(true);
	labRemTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labRemTim);
	left->addWidget(remTim);

	QTextEdit* notRemTim=new QTextEdit();
	notRemTim->setReadOnly(true);
	notRemTim->setPlainText(notRemovedTimeConstraintsString);
	QLabel* labNotRemTim=new QLabel(tr("These time constraints will NOT be removed"));
	labNotRemTim->setWordWrap(true);
	labNotRemTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labNotRemTim);
	left->addWidget(notRemTim);

	QTextEdit* remSpa=new QTextEdit();
	remSpa->setReadOnly(true);
	remSpa->setPlainText(removedSpaceConstraintsString);
	QLabel* labRemSpa=new QLabel(tr("These space constraints will be removed"));
	labRemSpa->setWordWrap(true);
	labRemSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labRemSpa);
	right->addWidget(remSpa);
	
	QTextEdit* notRemSpa=new QTextEdit();
	notRemSpa->setReadOnly(true);
	notRemSpa->setPlainText(notRemovedSpaceConstraintsString);
	QLabel* labNotRemSpa=new QLabel(tr("These space constraints will NOT be removed"));
	labNotRemSpa->setWordWrap(true);
	labNotRemSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labNotRemSpa);
	right->addWidget(notRemSpa);
	////end texts
	
	////////
	lastMainLayout->addLayout(all);
	//lastMainLayout->addStretch();
	lastMainLayout->addLayout(lastButtons);

	connect(lastpb2, &QPushButton::clicked, &lastConfirmationDialog, &QDialog::accept);
	connect(lastpb1, &QPushButton::clicked, &lastConfirmationDialog, &QDialog::reject);

	lastpb2->setDefault(true);
	lastpb2->setFocus();
	
	int lw=lastConfirmationDialog.sizeHint().width();
	int lh=lastConfirmationDialog.sizeHint().height();
	lastConfirmationDialog.resize(lw,lh);
	centerWidgetOnScreen(&lastConfirmationDialog);
	restoreFETDialogGeometry(&lastConfirmationDialog, unlockDayConfirmationSettingsString);
	ok=lastConfirmationDialog.exec();
	saveFETDialogGeometry(&lastConfirmationDialog, unlockDayConfirmationSettingsString);
	if(!ok)
		return;
	////////////
	
	bool t=gt.rules.removeTimeConstraints(removedTimeConstraints);
	assert(t);
	
	removedTimeConstraints.clear();
	notRemovedTimeConstraints.clear();

	t=gt.rules.removeSpaceConstraints(removedSpaceConstraints);
	assert(t);
	
	removedSpaceConstraints.clear();
	notRemovedSpaceConstraints.clear();

	QMessageBox::information(&lastConfirmationDialog, tr("FET information"), tr("There were removed %1 locking time constraints and"
		" %2 locking space constraints. There were not removed %3 locking time constraints and %4 locking space constraints, because"
		" these activities were permanently locked").arg(removedTime).arg(removedSpace).arg(notRemovedTime).arg(notRemovedSpace));

	if(removedTime>0 || removedSpace>0)
		gt.rules.addUndoPoint(tr("Unlocked the activities of the day %1, by removing %2 locking time constraints and %3 locking space constraints.")
		 .arg(gt.rules.daysOfTheWeek[selectedDayInt]).arg(removedTime).arg(removedSpace));
	
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	LockUnlock::increaseCommunicationSpinBox();
}

void AdvancedLockUnlockForm::lockActivityTag(QWidget* parent)
{
	if(!students_schedule_ready || !teachers_schedule_ready || !rooms_buildings_schedule_ready){
		return;
	}
	
	QStringList activityTags;
	assert(gt.rules.activityTagsList.count()==gt.rules.nInternalActivityTags);
	for(ActivityTag* at : std::as_const(gt.rules.activityTagsList))
		activityTags.append(at->name);
	if(activityTags.size()==0){
		QMessageBox::warning(parent, tr("FET warning"), tr("You entered this dialog, but you have no activity tags. This should not happen."
		 " Please report possible bug. Now FET will abandon the current operation and return to the main dialog, so that you don't lose data."));
		return;
	}
	assert(activityTags.size()!=0);

	//New Dialog
	QDialog taDialog(parent);
	taDialog.setWindowTitle(tr("FET - Lock activities which have attached a certain activity tag"));

	QVBoxLayout* taMainLayout=new QVBoxLayout(&taDialog);

	QLabel* taLabel=new QLabel();
	taLabel->setWordWrap(true);
	taLabel->setText(tr("All activities which have attached the selected activity tag will be locked")+"\n\n"+tr("Please select the activity tag to lock:"));
	taLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

	QComboBox* taLW=new QComboBox();
	
	QSize tmp=taLW->minimumSizeHint();
	Q_UNUSED(tmp);
	
	taLW->addItems(activityTags);
	taLW->setCurrentIndex(0);

	QPushButton* tapb1=new QPushButton(tr("Cancel"));
	QPushButton* tapb2=new QPushButton(tr("OK"));
	
	QHBoxLayout* buttons=new QHBoxLayout();
	buttons->addStretch();
	buttons->addWidget(tapb1);
	buttons->addWidget(tapb2);
	
	////////
	QGroupBox* selectorsGroupBox=new QGroupBox();
	selectorsGroupBox->setTitle(tr("Lock"));
	
	QHBoxLayout* groupLayout=new QHBoxLayout(selectorsGroupBox);
	
	QCheckBox* timeCheckBox=new QCheckBox();
	timeCheckBox->setChecked(true);
	timeCheckBox->setText(tr("Time"));
	QCheckBox* spaceCheckBox=new QCheckBox();
	spaceCheckBox->setChecked(true);
	spaceCheckBox->setText(tr("Space"));

	groupLayout->addWidget(timeCheckBox);
	groupLayout->addWidget(spaceCheckBox);
	groupLayout->addStretch();
	/////////
	
	taMainLayout->addWidget(taLabel);
	taMainLayout->addWidget(taLW);
	taMainLayout->addStretch();
	taMainLayout->addWidget(selectorsGroupBox);
	taMainLayout->addStretch();
	taMainLayout->addLayout(buttons);

	connect(tapb2, &QPushButton::clicked, &taDialog, &QDialog::accept);
	connect(tapb1, &QPushButton::clicked, &taDialog, &QDialog::reject);

	tapb2->setDefault(true);
	tapb2->setFocus();
	
	int w=taDialog.sizeHint().width();
	int h=taDialog.sizeHint().height();
	if(w<MIN_WIDTH)
		w=MIN_WIDTH;
	if(h<MIN_HEIGHT)
		h=MIN_HEIGHT;
	taDialog.resize(w,h);
	centerWidgetOnScreen(&taDialog);
	restoreFETDialogGeometry(&taDialog, lockActivityTagSettingsString);
	int ok=taDialog.exec();
	saveFETDialogGeometry(&taDialog, lockActivityTagSettingsString);
	if(ok==QDialog::Rejected)
		return;
		
	bool lockTime=timeCheckBox->isChecked();
	bool lockSpace=spaceCheckBox->isChecked();

	int selectedActivityTagInt=taLW->currentIndex();
	assert(selectedActivityTagInt>=0 && selectedActivityTagInt<gt.rules.nInternalActivityTags);

	QString addedTimeConstraintsString;
	QString notAddedTimeConstraintsString;
	
	QList<ConstraintActivityPreferredStartingTime*> addedTimeConstraints;
	QList<ConstraintActivityPreferredStartingTime*> notAddedTimeConstraints;
	
	QString addedSpaceConstraintsString;
	QString notAddedSpaceConstraintsString;
	
	QList<ConstraintActivityPreferredRoom*> addedSpaceConstraints;
	QList<ConstraintActivityPreferredRoom*> notAddedSpaceConstraints;
	
	int addedTime=0, notAddedTime=0;
	int addedSpace=0, notAddedSpace=0;

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		Activity* act=&gt.rules.internalActivitiesList[i];
		if(best_solution.times[i]!=UNALLOCATED_TIME){
			assert(best_solution.times[i]>=0 && best_solution.times[i]<gt.rules.nHoursPerWeek);
			int d=best_solution.times[i]%gt.rules.nDaysPerWeek;
			int h=best_solution.times[i]/gt.rules.nDaysPerWeek;
			
			ConstraintActivityPreferredStartingTime* newTimeCtr=nullptr;
			
			ConstraintActivityPreferredRoom* newSpaceCtr=nullptr;
			
			if(act->iActivityTagsSet.contains(selectedActivityTagInt) && lockTime){
				newTimeCtr=new ConstraintActivityPreferredStartingTime(100.0, gt.rules.internalActivitiesList[i].id, d, h, false);
			}
			
			if(act->iActivityTagsSet.contains(selectedActivityTagInt) && lockSpace){
				if(best_solution.rooms[i]!=UNALLOCATED_SPACE && best_solution.rooms[i]!=UNSPECIFIED_ROOM){
					QStringList tl;
					if(gt.rules.internalRoomsList[best_solution.rooms[i]]->isVirtual==false)
						assert(best_solution.realRoomsList[i].isEmpty());
					else
						for(int rr : std::as_const(best_solution.realRoomsList[i]))
							tl.append(gt.rules.internalRoomsList[rr]->name);

					newSpaceCtr=new ConstraintActivityPreferredRoom(100.0, gt.rules.internalActivitiesList[i].id, gt.rules.internalRoomsList[best_solution.rooms[i]]->name, tl, false);
				}
			}
			
			int aid=gt.rules.internalActivitiesList[i].id;
			
			if(newTimeCtr!=nullptr){
				bool add=true;
				
				for(ConstraintActivityPreferredStartingTime* tc : gt.rules.apstHash.value(aid, QSet<ConstraintActivityPreferredStartingTime*>())){
					if((*tc) == (*newTimeCtr)){
						add=false;
						break;
					}
				}
				
				if(add){
					addedTimeConstraintsString+=newTimeCtr->getDetailedDescription(gt.rules)+"\n";
					addedTimeConstraints.append(newTimeCtr);
					
					addedTime++;
				}
				else{
					notAddedTimeConstraintsString+=newTimeCtr->getDetailedDescription(gt.rules)+"\n";
					notAddedTimeConstraints.append(newTimeCtr);
					
					notAddedTime++;
				}
			}

			if(newSpaceCtr!=nullptr){
				bool add=true;
				
				for(ConstraintActivityPreferredRoom* tc : gt.rules.aprHash.value(aid, QSet<ConstraintActivityPreferredRoom*>())){
					if((*tc) == (*newSpaceCtr)){
						add=false;
						break;
					}
				}
				
				if(add){
					addedSpaceConstraintsString+=newSpaceCtr->getDetailedDescription(gt.rules)+"\n";
					addedSpaceConstraints.append(newSpaceCtr);
	 				
	 				addedSpace++;
				}
				else{
					notAddedSpaceConstraintsString+=newSpaceCtr->getDetailedDescription(gt.rules)+"\n";
					notAddedSpaceConstraints.append(newSpaceCtr);
					
					notAddedSpace++;
				}
			}
		}
	}
	
	////////////
	//last confirmation dialog
	QDialog lastConfirmationDialog(&taDialog);
	lastConfirmationDialog.setWindowTitle(tr("Last confirmation needed"));

	QVBoxLayout* lastMainLayout=new QVBoxLayout(&lastConfirmationDialog);

	QPushButton* lastpb1=new QPushButton(tr("Cancel"));
	QPushButton* lastpb2=new QPushButton(tr("OK"));
	QHBoxLayout* lastButtons=new QHBoxLayout();
	lastButtons->addStretch();
	lastButtons->addWidget(lastpb1);
	lastButtons->addWidget(lastpb2);
	
	////texts
	QVBoxLayout* left=new QVBoxLayout();
	QVBoxLayout* right=new QVBoxLayout();
	QHBoxLayout* all=new QHBoxLayout();
	all->addLayout(left);
	all->addLayout(right);
	
	QTextEdit* addTim=new QTextEdit();
	addTim->setReadOnly(true);
	addTim->setPlainText(addedTimeConstraintsString);
	QLabel* labAddTim=new QLabel(tr("These time constraints will be added"));
	labAddTim->setWordWrap(true);
	labAddTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labAddTim);
	left->addWidget(addTim);

	QTextEdit* notAddTim=new QTextEdit();
	notAddTim->setReadOnly(true);
	notAddTim->setPlainText(notAddedTimeConstraintsString);
	QLabel* labNotAddTim=new QLabel(tr("These time constraints will NOT be added"));
	labNotAddTim->setWordWrap(true);
	labNotAddTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labNotAddTim);
	left->addWidget(notAddTim);

	QTextEdit* addSpa=new QTextEdit();
	addSpa->setReadOnly(true);
	addSpa->setPlainText(addedSpaceConstraintsString);
	QLabel* labAddSpa=new QLabel(tr("These space constraints will be added"));
	labAddSpa->setWordWrap(true);
	labAddSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labAddSpa);
	right->addWidget(addSpa);
	
	QTextEdit* notAddSpa=new QTextEdit();
	notAddSpa->setReadOnly(true);
	notAddSpa->setPlainText(notAddedSpaceConstraintsString);
	QLabel* labNotAddSpa=new QLabel(tr("These space constraints will NOT be added"));
	labNotAddSpa->setWordWrap(true);
	labNotAddSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labNotAddSpa);
	right->addWidget(notAddSpa);
	////end texts
	
	////////
	lastMainLayout->addLayout(all);
	//lastMainLayout->addStretch();
	lastMainLayout->addLayout(lastButtons);

	connect(lastpb2, &QPushButton::clicked, &lastConfirmationDialog, &QDialog::accept);
	connect(lastpb1, &QPushButton::clicked, &lastConfirmationDialog, &QDialog::reject);

	lastpb2->setDefault(true);
	lastpb2->setFocus();
	
	int lw=lastConfirmationDialog.sizeHint().width();
	int lh=lastConfirmationDialog.sizeHint().height();
	lastConfirmationDialog.resize(lw,lh);
	centerWidgetOnScreen(&lastConfirmationDialog);
	restoreFETDialogGeometry(&lastConfirmationDialog, lockActivityTagConfirmationSettingsString);
	ok=lastConfirmationDialog.exec();
	saveFETDialogGeometry(&lastConfirmationDialog, lockActivityTagConfirmationSettingsString);
	if(!ok)
		return;
	////////////
	
	for(TimeConstraint* tc : std::as_const(addedTimeConstraints)){
		bool t=gt.rules.addTimeConstraint(tc);
		assert(t);
	}
	addedTimeConstraints.clear();
	for(TimeConstraint* tc : std::as_const(notAddedTimeConstraints)){
		delete tc;
	}
	notAddedTimeConstraints.clear();
	
	for(SpaceConstraint* sc : std::as_const(addedSpaceConstraints)){
		bool t=gt.rules.addSpaceConstraint(sc);
		assert(t);
	}
	addedSpaceConstraints.clear();
	for(SpaceConstraint* sc : std::as_const(notAddedSpaceConstraints)){
		delete sc;
	}
	notAddedSpaceConstraints.clear();
	
	QMessageBox::information(&lastConfirmationDialog, tr("FET information"), tr("There were added %1 locking time constraints and"
		" %2 locking space constraints. There were not added %3 locking time constraints and %4 locking space constraints, because"
		" these activities were already locked").arg(addedTime).arg(addedSpace).arg(notAddedTime).arg(notAddedSpace));

	if(addedTime>0 || addedSpace>0)
		gt.rules.addUndoPoint(tr("Locked the activities with the activity tag %1, by adding %2 locking time constraints and %3 locking space constraints.")
		 .arg(gt.rules.activityTagsList[selectedActivityTagInt]->name).arg(addedTime).arg(addedSpace));
	
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	LockUnlock::increaseCommunicationSpinBox();
}

/*void AdvancedLockUnlockForm::unlockActivityTag(QWidget* parent)
{
	if(!students_schedule_ready || !teachers_schedule_ready || !rooms_buildings_schedule_ready){
		return;
	}
	
	QStringList activityTags;
	assert(gt.rules.activityTagsList.count()==gt.rules.nInternalActivityTags);
	for(ActivityTag* at : std::as_const(gt.rules.activityTagsList))
		activityTags.append(at->name);
	if(activityTags.size()==0){
		QMessageBox::warning(parent, tr("FET warning"), tr("You entered this dialog, but you have no activity tags. This should not happen."
		 " Please report possible bug. Now FET will abandon the current operation and return to the main dialog, so that you don't lose data."));
		return;
	}
	assert(activityTags.size()!=0);

	//New Dialog
	QDialog taDialog(parent);
	taDialog.setWindowTitle(tr("FET - Unlock activities which have attached a certain activity tag"));

	QVBoxLayout* taMainLayout=new QVBoxLayout(&taDialog);

	QLabel* taLabel=new QLabel();
	taLabel->setWordWrap(true);
	taLabel->setText(tr("All activities which have attached the selected activity tag will be unlocked (those which are not permanently locked)")+"\n\n"+
	 tr("Please select the activity tag to unlock:"));
	taLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

	QComboBox* taLW=new QComboBox();
	
	QSize tmp2=taLW->minimumSizeHint();
	Q_UNUSED(tmp2);
	
	taLW->addItems(activityTags);
	taLW->setCurrentIndex(0);

	QPushButton* tapb1=new QPushButton(tr("Cancel"));
	QPushButton* tapb2=new QPushButton(tr("OK"));
	
	QHBoxLayout* buttons=new QHBoxLayout();
	buttons->addStretch();
	buttons->addWidget(tapb1);
	buttons->addWidget(tapb2);
	
	////////
	QGroupBox* selectorsGroupBox=new QGroupBox();
	selectorsGroupBox->setTitle(tr("Unlock"));
	
	QHBoxLayout* groupLayout=new QHBoxLayout(selectorsGroupBox);
	
	QCheckBox* timeCheckBox=new QCheckBox();
	timeCheckBox->setChecked(true);
	timeCheckBox->setText(tr("Time"));
	QCheckBox* spaceCheckBox=new QCheckBox();
	spaceCheckBox->setChecked(true);
	spaceCheckBox->setText(tr("Space"));

	groupLayout->addWidget(timeCheckBox);
	groupLayout->addWidget(spaceCheckBox);
	groupLayout->addStretch();
	/////////
	
	taMainLayout->addWidget(taLabel);
	taMainLayout->addWidget(taLW);
	taMainLayout->addStretch();
	taMainLayout->addWidget(selectorsGroupBox);
	taMainLayout->addStretch();
	taMainLayout->addLayout(buttons);

	QObj ect::connect(tapb2, SIG NAL(clicked()), &taDialog, SL OT(accept()));
	QObj ect::connect(tapb1, SIG NAL(clicked()), &taDialog, SL OT(reject()));

	tapb2->setDefault(true);
	tapb2->setFocus();
	
	int w=taDialog.sizeHint().width();
	int h=taDialog.sizeHint().height();
	if(w<MIN_WIDTH)
		w=MIN_WIDTH;
	if(h<MIN_HEIGHT)
		h=MIN_HEIGHT;
	taDialog.resize(w,h);
	centerWidgetOnScreen(&taDialog);
	restoreFETDialogGeometry(&taDialog, unlockActivityTagSettingsString);
	bool ok=taDialog.exec();
	saveFETDialogGeometry(&taDialog, unlockActivityTagSettingsString);
	if(!ok)
		return;
		
	bool unlockTime=timeCheckBox->isChecked();
	bool unlockSpace=spaceCheckBox->isChecked();

	int selectedActivityTagInt=taLW->currentIndex();
	assert(selectedActivityTagInt>=0 && selectedActivityTagInt<gt.rules.nInternalActivityTags);

	int removedTime=0, notRemovedTime=0;
	int removedSpace=0, notRemovedSpace=0;
	
	QList<int> lockedActivitiesIds;

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		if(best_solution.times[i]!=UNALLOCATED_TIME){
			assert(best_solution.times[i]>=0 && best_solution.times[i]<gt.rules.nHoursPerWeek);
			//int d=best_solution.times[i]%gt.rules.nDaysPerWeek;
			//int h=best_solution.times[i]/gt.rules.nDaysPerWeek;
			
			if(gt.rules.internalActivitiesList[i].iActivityTagsSet.contains(selectedActivityTagInt)){
				lockedActivitiesIds.append(gt.rules.internalActivitiesList[i].id);
			}
		}
	}

	QString removedTimeConstraintsString;
	QString notRemovedTimeConstraintsString;

	QList<TimeConstraint*> removedTimeConstraints;
	QList<TimeConstraint*> notRemovedTimeConstraints;
	
	QString removedSpaceConstraintsString;
	QString notRemovedSpaceConstraintsString;

	QList<SpaceConstraint*> removedSpaceConstraints;
	QList<SpaceConstraint*> notRemovedSpaceConstraints;
	
	if(unlockTime){
		for(int aid : std::as_const(lockedActivitiesIds)){
			for(ConstraintActivityPreferredStartingTime* c : gt.rules.apstHash.value(aid, QSet<ConstraintActivityPreferredStartingTime*>())){
				assert(aid==c->activityId);
				if(c->weightPercentage==100.0 && c->day>=0 && c->hour>=0){
					if(!c->permanentlyLocked){
						removedTimeConstraints.append((TimeConstraint*)c);
						removedTimeConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						removedTime++;
					}
					else{
						notRemovedTimeConstraints.append((TimeConstraint*)c);
						notRemovedTimeConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						notRemovedTime++;
					}
				}
			}
		}
	}

	if(unlockSpace){
		for(int aid : std::as_const(lockedActivitiesIds)){
			for(ConstraintActivityPreferredRoom* c : gt.rules.aprHash.value(aid, QSet<ConstraintActivityPreferredRoom*>())){
				assert(aid==c->activityId);
				if(c->weightPercentage==100.0){
					if(!c->permanentlyLocked){
						removedSpaceConstraints.append((SpaceConstraint*)c);
						removedSpaceConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						removedSpace++;
					}
					else{
						notRemovedSpaceConstraints.append((SpaceConstraint*)c);
						notRemovedSpaceConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						notRemovedSpace++;
					}
				}
			}
		}
	}

	////////////
	//last confirmation dialog
	QDialog lastConfirmationDialog(&taDialog);
	lastConfirmationDialog.setWindowTitle(tr("Last confirmation needed"));

	QVBoxLayout* lastMainLayout=new QVBoxLayout(&lastConfirmationDialog);

	QPushButton* lastpb1=new QPushButton(tr("Cancel"));
	QPushButton* lastpb2=new QPushButton(tr("OK"));
	QHBoxLayout* lastButtons=new QHBoxLayout();
	lastButtons->addStretch();
	lastButtons->addWidget(lastpb1);
	lastButtons->addWidget(lastpb2);
	
	////texts
	QVBoxLayout* left=new QVBoxLayout();
	QVBoxLayout* right=new QVBoxLayout();
	QHBoxLayout* all=new QHBoxLayout();
	all->addLayout(left);
	all->addLayout(right);
	
	QTextEdit* remTim=new QTextEdit();
	remTim->setReadOnly(true);
	remTim->setPlainText(removedTimeConstraintsString);
	QLabel* labRemTim=new QLabel(tr("These time constraints will be removed"));
	labRemTim->setWordWrap(true);
	labRemTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labRemTim);
	left->addWidget(remTim);

	QTextEdit* notRemTim=new QTextEdit();
	notRemTim->setReadOnly(true);
	notRemTim->setPlainText(notRemovedTimeConstraintsString);
	QLabel* labNotRemTim=new QLabel(tr("These time constraints will NOT be removed"));
	labNotRemTim->setWordWrap(true);
	labNotRemTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labNotRemTim);
	left->addWidget(notRemTim);

	QTextEdit* remSpa=new QTextEdit();
	remSpa->setReadOnly(true);
	remSpa->setPlainText(removedSpaceConstraintsString);
	QLabel* labRemSpa=new QLabel(tr("These space constraints will be removed"));
	labRemSpa->setWordWrap(true);
	labRemSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labRemSpa);
	right->addWidget(remSpa);
	
	QTextEdit* notRemSpa=new QTextEdit();
	notRemSpa->setReadOnly(true);
	notRemSpa->setPlainText(notRemovedSpaceConstraintsString);
	QLabel* labNotRemSpa=new QLabel(tr("These space constraints will NOT be removed"));
	labNotRemSpa->setWordWrap(true);
	labNotRemSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labNotRemSpa);
	right->addWidget(notRemSpa);
	////end texts
	
	////////
	lastMainLayout->addLayout(all);
	//lastMainLayout->addStretch();
	lastMainLayout->addLayout(lastButtons);

	QObj ect::connect(lastpb2, SIG NAL(clicked()), &lastConfirmationDialog, SL OT(accept()));
	QObj ect::connect(lastpb1, SIG NAL(clicked()), &lastConfirmationDialog, SL OT(reject()));

	lastpb2->setDefault(true);
	lastpb2->setFocus();
	
	int lw=lastConfirmationDialog.sizeHint().width();
	int lh=lastConfirmationDialog.sizeHint().height();
	lastConfirmationDialog.resize(lw,lh);
	centerWidgetOnScreen(&lastConfirmationDialog);
	restoreFETDialogGeometry(&lastConfirmationDialog, unlockActivityTagConfirmationSettingsString);
	ok=lastConfirmationDialog.exec();
	saveFETDialogGeometry(&lastConfirmationDialog, unlockActivityTagConfirmationSettingsString);
	if(!ok)
		return;
	////////////
	
	bool t=gt.rules.removeTimeConstraints(removedTimeConstraints);
	assert(t);
	
	removedTimeConstraints.clear();
	notRemovedTimeConstraints.clear();

	t=gt.rules.removeSpaceConstraints(removedSpaceConstraints);
	assert(t);
	
	removedSpaceConstraints.clear();
	notRemovedSpaceConstraints.clear();

	QMessageBox::information(&lastConfirmationDialog, tr("FET information"), tr("There were removed %1 locking time constraints and"
		" %2 locking space constraints. There were not removed %3 locking time constraints and %4 locking space constraints, because"
		" these activities were permanently locked").arg(removedTime).arg(removedSpace).arg(notRemovedTime).arg(notRemovedSpace));
	
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	LockUnlock::increaseCommunicationSpinBox();
}*/

void AdvancedLockUnlockForm::unlockActivityTagWithoutTimetable(QWidget* parent)
{
	/*if(!students_schedule_ready || !teachers_schedule_ready || !rooms_buildings_schedule_ready){
		return;
	}*/

	QSet<QString> virtualRooms;
	for(Room* rm : std::as_const(gt.rules.roomsList))
		if(rm->isVirtual==true)
			virtualRooms.insert(rm->name);
	
	QStringList activityTags;
	//Wrong test below, because the timetable is not generated
	//assert(gt.rules.activityTagsList.count()==gt.rules.nInternalActivityTags);
	for(ActivityTag* at : std::as_const(gt.rules.activityTagsList))
		activityTags.append(at->name);
	if(activityTags.size()==0){
		QMessageBox::warning(parent, tr("FET warning"), tr("You entered this dialog, but you have no activity tags. This should not happen."
		 " Please report possible bug. Now FET will abandon the current operation and return to the main dialog, so that you don't lose data."));
		return;
	}
	assert(activityTags.size()!=0);

	//New Dialog
	QDialog taDialog(parent);
	taDialog.setWindowTitle(tr("FET - Unlock activities which have attached a certain activity tag"));

	QVBoxLayout* taMainLayout=new QVBoxLayout(&taDialog);

	QLabel* taLabel=new QLabel();
	taLabel->setWordWrap(true);
	taLabel->setText(tr("All activities which have attached the selected activity tag will be unlocked (those which are not permanently locked)")+"\n\n"+
	 tr("Please select the activity tag to unlock:"));
	taLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

	QComboBox* taLW=new QComboBox();
	
	QSize tmp2=taLW->minimumSizeHint();
	Q_UNUSED(tmp2);
	
	taLW->addItems(activityTags);
	taLW->setCurrentIndex(0);

	QPushButton* tapb1=new QPushButton(tr("Cancel"));
	QPushButton* tapb2=new QPushButton(tr("OK"));
	
	QHBoxLayout* buttons=new QHBoxLayout();
	buttons->addStretch();
	buttons->addWidget(tapb1);
	buttons->addWidget(tapb2);
	
	////////
	QGroupBox* selectorsGroupBox=new QGroupBox();
	selectorsGroupBox->setTitle(tr("Unlock"));
	
	QHBoxLayout* groupLayout=new QHBoxLayout(selectorsGroupBox);
	
	QCheckBox* timeCheckBox=new QCheckBox();
	timeCheckBox->setChecked(true);
	timeCheckBox->setText(tr("Time"));
	QCheckBox* spaceCheckBox=new QCheckBox();
	spaceCheckBox->setChecked(true);
	spaceCheckBox->setText(tr("Space"));

	groupLayout->addWidget(timeCheckBox);
	groupLayout->addWidget(spaceCheckBox);
	groupLayout->addStretch();
	/////////
	
	taMainLayout->addWidget(taLabel);
	taMainLayout->addWidget(taLW);
	taMainLayout->addStretch();
	taMainLayout->addWidget(selectorsGroupBox);
	taMainLayout->addStretch();
	taMainLayout->addLayout(buttons);

	connect(tapb2, &QPushButton::clicked, &taDialog, &QDialog::accept);
	connect(tapb1, &QPushButton::clicked, &taDialog, &QDialog::reject);

	tapb2->setDefault(true);
	tapb2->setFocus();
	
	int w=taDialog.sizeHint().width();
	int h=taDialog.sizeHint().height();
	if(w<MIN_WIDTH)
		w=MIN_WIDTH;
	if(h<MIN_HEIGHT)
		h=MIN_HEIGHT;
	taDialog.resize(w,h);
	centerWidgetOnScreen(&taDialog);
	restoreFETDialogGeometry(&taDialog, unlockActivityTagSettingsString);
	int ok=taDialog.exec();
	saveFETDialogGeometry(&taDialog, unlockActivityTagSettingsString);
	if(ok==QDialog::Rejected)
		return;
		
	bool unlockTime=timeCheckBox->isChecked();
	bool unlockSpace=spaceCheckBox->isChecked();
	//We need to consider non-internal data, because the timetable is not available
	//int selectedActivityTagInt=taLW->currentIndex();
	//assert(selectedActivityTagInt>=0 && selectedActivityTagInt<gt.rules.nInternalActivityTags);
	QString selectedActivityTagString=taLW->currentText();
	assert(selectedActivityTagString!=QString(""));

	int removedTime=0, notRemovedTime=0;
	int removedSpace=0, notRemovedSpace=0;
	
	QString removedTimeConstraintsString;
	QString notRemovedTimeConstraintsString;

	QList<TimeConstraint*> removedTimeConstraints;
	QList<TimeConstraint*> notRemovedTimeConstraints;
	
	QString removedSpaceConstraintsString;
	QString notRemovedSpaceConstraintsString;

	QList<SpaceConstraint*> removedSpaceConstraints;
	QList<SpaceConstraint*> notRemovedSpaceConstraints;
	
	QSet<int> actsSet;
	
	if(unlockTime || unlockSpace) // ||unlockSpace is added on 2018-06-26, correcting a bug if the user only selected Space and the timetable was not generated.
		for(TimeConstraint* tc : std::as_const(gt.rules.timeConstraintsList))
			if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME){
				ConstraintActivityPreferredStartingTime* c=(ConstraintActivityPreferredStartingTime*)tc;
				if(c->weightPercentage==100.0 && c->day>=0 && c->hour>=0){
					Activity* act=gt.rules.activitiesPointerHash.value(c->activityId, nullptr);
					assert(act!=nullptr);
					if(act->activityTagsNames.contains(selectedActivityTagString)){
						actsSet.insert(c->activityId);
						if(unlockTime){
							if(!c->permanentlyLocked){
								removedTimeConstraints.append((TimeConstraint*)c);
								//removedTimeConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
								removedTime++;
							}
							else{
								notRemovedTimeConstraints.append((TimeConstraint*)c);
								//notRemovedTimeConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
								notRemovedTime++;
							}
						}
					}
				}
			}
	
	if(unlockSpace)
		for(SpaceConstraint* sc : std::as_const(gt.rules.spaceConstraintsList))
			if(sc->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
				ConstraintActivityPreferredRoom* c=(ConstraintActivityPreferredRoom*) sc;
				if(c->weightPercentage==100.0 && actsSet.contains(c->activityId) &&
				 (!virtualRooms.contains(c->roomName) || (virtualRooms.contains(c->roomName) && !c->preferredRealRoomsNames.isEmpty()))){
					if(!c->permanentlyLocked){
						removedSpaceConstraints.append((SpaceConstraint*)c);
						//removedSpaceConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						removedSpace++;
					}
					else{
						notRemovedSpaceConstraints.append((SpaceConstraint*)c);
						//notRemovedSpaceConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						notRemovedSpace++;
					}
				}
			}

	std::stable_sort(removedTimeConstraints.begin(), removedTimeConstraints.end(), compareTimeConstraintsActivityPreferredStartingTimeActivitiesIds);
	for(TimeConstraint* tc : std::as_const(removedTimeConstraints))
		removedTimeConstraintsString+=tc->getDetailedDescription(gt.rules)+"\n";
		
	std::stable_sort(notRemovedTimeConstraints.begin(), notRemovedTimeConstraints.end(), compareTimeConstraintsActivityPreferredStartingTimeActivitiesIds);
	for(TimeConstraint* tc : std::as_const(notRemovedTimeConstraints))
		notRemovedTimeConstraintsString+=tc->getDetailedDescription(gt.rules)+"\n";

	std::stable_sort(removedSpaceConstraints.begin(), removedSpaceConstraints.end(), compareSpaceConstraintsActivityPreferredRoomActivitiesIds);
	for(SpaceConstraint* sc : std::as_const(removedSpaceConstraints))
		removedSpaceConstraintsString+=sc->getDetailedDescription(gt.rules)+"\n";

	std::stable_sort(notRemovedSpaceConstraints.begin(), notRemovedSpaceConstraints.end(), compareSpaceConstraintsActivityPreferredRoomActivitiesIds);
	for(SpaceConstraint* sc : std::as_const(notRemovedSpaceConstraints))
		notRemovedSpaceConstraintsString+=sc->getDetailedDescription(gt.rules)+"\n";

	////////////
	//last confirmation dialog
	QDialog lastConfirmationDialog(&taDialog);
	lastConfirmationDialog.setWindowTitle(tr("Last confirmation needed"));

	QVBoxLayout* lastMainLayout=new QVBoxLayout(&lastConfirmationDialog);

	QPushButton* lastpb1=new QPushButton(tr("Cancel"));
	QPushButton* lastpb2=new QPushButton(tr("OK"));
	QHBoxLayout* lastButtons=new QHBoxLayout();
	lastButtons->addStretch();
	lastButtons->addWidget(lastpb1);
	lastButtons->addWidget(lastpb2);
	
	////texts
	QVBoxLayout* left=new QVBoxLayout();
	QVBoxLayout* right=new QVBoxLayout();
	QHBoxLayout* all=new QHBoxLayout();
	all->addLayout(left);
	all->addLayout(right);
	
	QTextEdit* remTim=new QTextEdit();
	remTim->setReadOnly(true);
	remTim->setPlainText(removedTimeConstraintsString);
	QLabel* labRemTim=new QLabel(tr("These time constraints will be removed"));
	labRemTim->setWordWrap(true);
	labRemTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labRemTim);
	left->addWidget(remTim);

	QTextEdit* notRemTim=new QTextEdit();
	notRemTim->setReadOnly(true);
	notRemTim->setPlainText(notRemovedTimeConstraintsString);
	QLabel* labNotRemTim=new QLabel(tr("These time constraints will NOT be removed"));
	labNotRemTim->setWordWrap(true);
	labNotRemTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labNotRemTim);
	left->addWidget(notRemTim);

	QTextEdit* remSpa=new QTextEdit();
	remSpa->setReadOnly(true);
	remSpa->setPlainText(removedSpaceConstraintsString);
	QLabel* labRemSpa=new QLabel(tr("These space constraints will be removed"));
	labRemSpa->setWordWrap(true);
	labRemSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labRemSpa);
	right->addWidget(remSpa);
	
	QTextEdit* notRemSpa=new QTextEdit();
	notRemSpa->setReadOnly(true);
	notRemSpa->setPlainText(notRemovedSpaceConstraintsString);
	QLabel* labNotRemSpa=new QLabel(tr("These space constraints will NOT be removed"));
	labNotRemSpa->setWordWrap(true);
	labNotRemSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labNotRemSpa);
	right->addWidget(notRemSpa);
	////end texts
	
	////////
	lastMainLayout->addLayout(all);
	//lastMainLayout->addStretch();
	lastMainLayout->addLayout(lastButtons);

	connect(lastpb2, &QPushButton::clicked, &lastConfirmationDialog, &QDialog::accept);
	connect(lastpb1, &QPushButton::clicked, &lastConfirmationDialog, &QDialog::reject);

	lastpb2->setDefault(true);
	lastpb2->setFocus();
	
	int lw=lastConfirmationDialog.sizeHint().width();
	int lh=lastConfirmationDialog.sizeHint().height();
	lastConfirmationDialog.resize(lw,lh);
	centerWidgetOnScreen(&lastConfirmationDialog);
	restoreFETDialogGeometry(&lastConfirmationDialog, unlockActivityTagConfirmationSettingsString);
	ok=lastConfirmationDialog.exec();
	saveFETDialogGeometry(&lastConfirmationDialog, unlockActivityTagConfirmationSettingsString);
	if(!ok)
		return;
	////////////
	
	bool t=gt.rules.removeTimeConstraints(removedTimeConstraints);
	assert(t);
	
	removedTimeConstraints.clear();
	notRemovedTimeConstraints.clear();

	t=gt.rules.removeSpaceConstraints(removedSpaceConstraints);
	assert(t);
	
	removedSpaceConstraints.clear();
	notRemovedSpaceConstraints.clear();

	QMessageBox::information(&lastConfirmationDialog, tr("FET information"), tr("There were removed %1 locking time constraints and"
		" %2 locking space constraints. There were not removed %3 locking time constraints and %4 locking space constraints, because"
		" these activities were permanently locked").arg(removedTime).arg(removedSpace).arg(notRemovedTime).arg(notRemovedSpace));

	if(removedTime>0 || removedSpace>0)
		gt.rules.addUndoPoint(tr("Unlocked the activities with the activity tag %1, by removing %2 locking time constraints and %3 locking space constraints.")
		 .arg(selectedActivityTagString).arg(removedTime).arg(removedSpace));
	
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	LockUnlock::increaseCommunicationSpinBox();
}

//2023-01-13

LockAdvancedDialog::LockAdvancedDialog(QWidget* parent) : QDialog(parent)
{
	setWindowTitle(tr("FET - Lock the activities which correspond to the selected advanced filter"));
	
	numberLabel=new QLabel(QString("No: 1000 / 1000"));
	numberLabel->setAlignment(Qt::AlignCenter);
	durationLabel=new QLabel(QString("Dur: 1000 / 1000"));
	durationLabel->setAlignment(Qt::AlignCenter);
	QHBoxLayout* labelsLayout=new QHBoxLayout();
	labelsLayout->addWidget(numberLabel);
	labelsLayout->addWidget(durationLabel);

	QVBoxLayout* taMainLayout=new QVBoxLayout(this);

	QLabel* taLabel=new QLabel();
	taLabel->setWordWrap(true);
	taLabel->setText(tr("All the filtered activities will be locked"));
//	taLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	
	selectedActivitiesLW=new QListWidget();
	
	filterCheckBox=new QCheckBox(tr("Filter"));
	filterCheckBox->setChecked(false);
	QPushButton* helpPB=new QPushButton(tr("Help"));
	QPushButton* tapb1=new QPushButton(tr("Cancel"));
	QPushButton* tapb2=new QPushButton(tr("OK"));
	
	QHBoxLayout* buttons=new QHBoxLayout();
	buttons->addWidget(filterCheckBox);
	buttons->addStretch();
	buttons->addWidget(helpPB);
	buttons->addWidget(tapb1);
	buttons->addWidget(tapb2);
	
	////////
	QGroupBox* selectorsGroupBox=new QGroupBox();
	selectorsGroupBox->setTitle(tr("Lock"));
	
	QHBoxLayout* groupLayout=new QHBoxLayout(selectorsGroupBox);
	
	timeCheckBox=new QCheckBox();
	timeCheckBox->setChecked(true);
	timeCheckBox->setText(tr("Time"));
	spaceCheckBox=new QCheckBox();
	spaceCheckBox->setChecked(true);
	spaceCheckBox->setText(tr("Space"));

	groupLayout->addWidget(timeCheckBox);
	groupLayout->addWidget(spaceCheckBox);
	groupLayout->addStretch();
	/////////
	
	taMainLayout->addLayout(labelsLayout);
	taMainLayout->addWidget(taLabel);
	taMainLayout->addWidget(selectedActivitiesLW);
	taMainLayout->addWidget(selectorsGroupBox);
	taMainLayout->addLayout(buttons);

	connect(tapb2, &QPushButton::clicked, this, &LockAdvancedDialog::accept);
	connect(tapb1, &QPushButton::clicked, this, &LockAdvancedDialog::reject);
	connect(helpPB, &QPushButton::clicked, this, &LockAdvancedDialog::help);
	connect(filterCheckBox, &QCheckBox::toggled, this, &LockAdvancedDialog::filter);

	//////////////////
	QSettings settings(COMPANY, PROGRAM);

	QString settingsName="LockAdvancedDialog";
	
	all=settings.value(settingsName+"/all-conditions", "true").toBool();
	
	descrDetDescrDetDescrWithConstraints.clear();
	int n=settings.value(settingsName+"/number-of-descriptions", "1").toInt();
	for(int i=0; i<n; i++)
		descrDetDescrDetDescrWithConstraints.append(settings.value(settingsName+"/description/"+CustomFETString::number(i+1), CustomFETString::number(DESCRIPTION)).toInt());
	
	contains.clear();
	n=settings.value(settingsName+"/number-of-contains", "1").toInt();
	for(int i=0; i<n; i++)
		contains.append(settings.value(settingsName+"/contains/"+CustomFETString::number(i+1), CustomFETString::number(CONTAINS)).toInt());
	
	text.clear();
	n=settings.value(settingsName+"/number-of-texts", "1").toInt();
	for(int i=0; i<n; i++)
		text.append(settings.value(settingsName+"/text/"+CustomFETString::number(i+1), QString("")).toString());
	
	caseSensitive=settings.value(settingsName+"/case-sensitive", "false").toBool();
	
	assert(filterCheckBox->isChecked()==false);
	//////////////////
	
	tapb2->setDefault(true);
	tapb2->setFocus();
	
	int w=this->sizeHint().width();
	int h=this->sizeHint().height();
	if(w<MIN_WIDTH)
		w=MIN_WIDTH;
	if(h<MIN_HEIGHT)
		h=MIN_HEIGHT;
	this->resize(w,h);
	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this, lockAdvancedFilterSettingsString);
	
	filterChanged();
}

LockAdvancedDialog::~LockAdvancedDialog()
{
	saveFETDialogGeometry(this, lockAdvancedFilterSettingsString);

	QSettings settings(COMPANY, PROGRAM);

	QString settingsName="LockAdvancedDialog";
	
	settings.setValue(settingsName+"/all-conditions", all);
	
	settings.setValue(settingsName+"/number-of-descriptions", descrDetDescrDetDescrWithConstraints.count());
	settings.remove(settingsName+"/description");
	for(int i=0; i<descrDetDescrDetDescrWithConstraints.count(); i++)
		settings.setValue(settingsName+"/description/"+CustomFETString::number(i+1), descrDetDescrDetDescrWithConstraints.at(i));
	
	settings.setValue(settingsName+"/number-of-contains", contains.count());
	settings.remove(settingsName+"/contains");
	for(int i=0; i<contains.count(); i++)
		settings.setValue(settingsName+"/contains/"+CustomFETString::number(i+1), contains.at(i));
	
	settings.setValue(settingsName+"/number-of-texts", text.count());
	settings.remove(settingsName+"/text");
	for(int i=0; i<text.count(); i++)
		settings.setValue(settingsName+"/text/"+CustomFETString::number(i+1), text.at(i));
	
	settings.setValue(settingsName+"/case-sensitive", caseSensitive);
}

bool LockAdvancedDialog::filterOk(Activity* act)
{
	if(!filterCheckBox->isChecked()){
		return true;
	}
	else{
		/////////////////////advanced filter
		assert(descrDetDescrDetDescrWithConstraints.count()==contains.count());
		assert(contains.count()==text.count());

		Qt::CaseSensitivity csens=Qt::CaseSensitive;
		if(!caseSensitive)
			csens=Qt::CaseInsensitive;

		QList<int> perm;
		for(int i=0; i<descrDetDescrDetDescrWithConstraints.count(); i++)
			perm.append(i);
		//Below we do a stable sorting, so that first inputted filters are hopefully filtering out more entries. This is written in the help when interrupting the
		//filtering of the activities, see the progress.wasCanceled() message.
		std::stable_sort(perm.begin(), perm.end(), [this](int a, int b){return descrDetDescrDetDescrWithConstraints.at(a)<descrDetDescrDetDescrWithConstraints.at(b);});
		for(int i=0; i<perm.count()-1; i++)
			assert(descrDetDescrDetDescrWithConstraints.at(perm.at(i))<=descrDetDescrDetDescrWithConstraints.at(perm.at(i+1)));

		int firstPosWithDescr=-1;
		int firstPosWithDetDescr=-1;
		int firstPosWithConstraints=-1;
		for(int i=0; i<perm.count(); i++){
			if(descrDetDescrDetDescrWithConstraints.at(perm.at(i))==DESCRIPTION && firstPosWithDescr==-1){
				firstPosWithDescr=i;
			}
			else if(descrDetDescrDetDescrWithConstraints.at(perm.at(i))==DETDESCRIPTION && firstPosWithDetDescr==-1){
				firstPosWithDetDescr=i;
			}
			else if(descrDetDescrDetDescrWithConstraints.at(perm.at(i))==DETDESCRIPTIONWITHCONSTRAINTS && firstPosWithConstraints==-1){
				firstPosWithConstraints=i;
			}
		}
		
		QString s=QString("");
		for(int i=0; i<perm.count(); i++){
			if(descrDetDescrDetDescrWithConstraints.at(perm.at(i))==DESCRIPTION){
				assert(firstPosWithDescr>=0);
				
				if(i==firstPosWithDescr)
					s=act->getDescription(gt.rules);
			}
			else if(descrDetDescrDetDescrWithConstraints.at(perm.at(i))==DETDESCRIPTION){
				assert(firstPosWithDetDescr>=0);
				
				if(i==firstPosWithDetDescr)
					s=act->getDetailedDescription(gt.rules);
			}
			else{
				assert(descrDetDescrDetDescrWithConstraints.at(perm.at(i))==DETDESCRIPTIONWITHCONSTRAINTS);
				
				assert(firstPosWithConstraints>=0);
				
				if(i==firstPosWithConstraints)
					s=act->getDetailedDescriptionWithConstraints(gt.rules); //warning: time consuming operation, goes through all the constraints and group activities in.initial order items.
			}
			
			bool okPartial=true; //We initialize okPartial to silence a MinGW 11.2.0 warning of type 'this variable might be used uninitialized'.
			
			QString t=text.at(perm.at(i));
			if(contains.at(perm.at(i))==CONTAINS){
				okPartial=s.contains(t, csens);
			}
			else if(contains.at(perm.at(i))==DOESNOTCONTAIN){
				okPartial=!(s.contains(t, csens));
			}
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
			else if(contains.at(perm.at(i))==REGEXP){
				QRegularExpression regExp(t);
				if(!caseSensitive)
					regExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
				okPartial=(regExp.match(s)).hasMatch();
			}
			else if(contains.at(perm.at(i))==NOTREGEXP){
				QRegularExpression regExp(t);
				if(!caseSensitive)
					regExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
				okPartial=!(regExp.match(s)).hasMatch();
			}
#else
			else if(contains.at(perm.at(i))==REGEXP){
				QRegExp regExp(t);
				regExp.setCaseSensitivity(csens);
				okPartial=(regExp.indexIn(s)>=0);
			}
			else if(contains.at(perm.at(i))==NOTREGEXP){
				QRegExp regExp(t);
				regExp.setCaseSensitivity(csens);
				okPartial=(regExp.indexIn(s)<0);
			}
#endif
			else
				assert(0);
				
			if(all && !okPartial)
				return false;
			else if(!all && okPartial)
				return true;
		}
		
		return all;
	}
}

void LockAdvancedDialog::filterChanged()
{
	int /*nacts=0,*/ nsubacts=0, nh=0;
	int ninact=0, ninacth=0;

	filteredActivitiesIndicesList.clear();
	selectedActivitiesLW->clear();
	
	QProgressDialog progress(this);
	progress.setWindowTitle(tr("Filtering the activities", "Title of a progress dialog"));
	progress.setLabelText(tr("Filtering the activities ... please wait"));
	progress.setRange(0, qMax(gt.rules.nInternalActivities, 1));
	progress.setModal(true);
	
	int k=0;

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		progress.setValue(i);
		if(progress.wasCanceled()){
			LongTextMessageBox::largeInformation(this, tr("FET warning"), tr("You canceled the filtering of the activities - the list of activities will be incomplete.")+QString(" ")+
			 tr("Note: if filtering of the activities takes too much, it might be because you are filtering on the detailed description with constraints of the activities,"
			 " which checks each activity against each time constraint, each space constraint, and each group activities in the initial order item, which might be too much."
			 " Please consider filtering on the description or detailed description of the activities, instead.")+QString("\n\n")+tr("Note: if you are using more filters,"
			 " like 'the activity should have a certain teacher', you might obtain much faster results if you filter on the description/detailed description of the activity"
			 " for the teacher and on the detailed description with constraints for other filters (meaning that you should prefer using the description/detailed description"
			 " instead of the detailed description with constraints, whenever it is possible).")+QString(" ")+tr("Also, the order of the detailed description with constraints"
			 " filters is important: you should put firstly the ones which filter out more activities (if you selected the 'All' radio button) or put firstly the ones which"
			 " accept immediately more activities (if you selected the 'Any' radio button)."));
			break;
		}
		
		if(best_solution.times[i]!=UNALLOCATED_TIME){
			Activity* act=&gt.rules.internalActivitiesList[i];
			if(filterOk(act)){

				filteredActivitiesIndicesList.append(i);
				selectedActivitiesLW->addItem(act->getDescription(gt.rules));
				
				k++;

				if(!act->active){
					selectedActivitiesLW->item(k-1)->setBackground(selectedActivitiesLW->palette().brush(QPalette::Disabled, QPalette::Window));
					selectedActivitiesLW->item(k-1)->setForeground(selectedActivitiesLW->palette().brush(QPalette::Disabled, QPalette::WindowText));
				}
				
				//if(act->id==act->activityGroupId || act->activityGroupId==0)
				//	nacts++;
				nsubacts++;
				
				nh+=act->duration;
				
				if(!act->active){
					ninact++;
					ninacth+=act->duration;
				}
			}
		}
	}

	progress.setValue(qMax(gt.rules.activitiesList.size(), 1));

	assert(nsubacts-ninact>=0);
	int NA=nsubacts-ninact;
	int NT=nsubacts;
	assert(nh-ninacth>=0);
	int DA=nh-ninacth;
	int DT=nh;
	numberLabel->setText(tr("No: %1 / %2", "No means number, %1 is the number of active activities, %2 is the total number of activities."
		" Please leave spaces between fields, so that they are better visible").arg(NA).arg(NT));
	durationLabel->setText(tr("Dur: %1 / %2", "Dur means duration, %1 is the duration of active activities, %2 is the total duration of activities."
		" Please leave spaces between fields, so that they are better visible").arg(DA).arg(DT));
}

void LockAdvancedDialog::filter(bool selected)
{
	if(!selected){
		filterChanged();
		return;
	}
	
	filterForm=new AdvancedFilterForm(this, tr("Advanced filter for activities"), true, all, descrDetDescrDetDescrWithConstraints, contains, text, caseSensitive, "ActivitiesLockAdvancedDialog");

	int t=filterForm->exec();
	if(t==QDialog::Accepted){
		if(filterForm->allRadio->isChecked())
			all=true;
		else if(filterForm->anyRadio->isChecked())
			all=false;
		else
			assert(0);
		
		caseSensitive=filterForm->caseSensitiveCheckBox->isChecked();
		
		descrDetDescrDetDescrWithConstraints.clear();
		contains.clear();
		text.clear();
		
		assert(filterForm->descrDetDescrDetDescrWithConstraintsComboBoxList.count()==filterForm->contNContReNReComboBoxList.count());
		assert(filterForm->descrDetDescrDetDescrWithConstraintsComboBoxList.count()==filterForm->textLineEditList.count());
		for(int i=0; i<filterForm->rows; i++){
			QComboBox* cb1=filterForm->descrDetDescrDetDescrWithConstraintsComboBoxList.at(i);
			QComboBox* cb2=filterForm->contNContReNReComboBoxList.at(i);
			QLineEdit* tl=filterForm->textLineEditList.at(i);
			
			descrDetDescrDetDescrWithConstraints.append(cb1->currentIndex());
			contains.append(cb2->currentIndex());
			text.append(tl->text());
		}
		
		filterChanged();
	}
	else{
		disconnect(filterCheckBox, &QCheckBox::toggled, this, &LockAdvancedDialog::filter);
		filterCheckBox->setChecked(false);
		connect(filterCheckBox, &QCheckBox::toggled, this, &LockAdvancedDialog::filter);
	}
	
	delete filterForm;
}

void LockAdvancedDialog::help()
{
	QString s;
	
	s+=tr("Above the activities list we have 2 labels, containing 4 numbers. The first label contains text: No: a / b. The first number a is the"
		" number of active activities (we number each individual subactivity as 1), while the second number b is the number of total activities."
		" The second label contains text: Dur: c / d. The third number c is the duration of active activities, in periods"
		" (or FET hours), while the fourth number d is the duration of total activities, in periods (or FET hours)."
		" So, No means number and Dur means duration.");
	s+="\n\n";
	s+=tr("Example: No: 100 / 102, Dur: 114 / 117. They represent: 100 - the number of active activities,"
		" then 102 - the number of total activities,"
		" 114 - the duration of active activities (in periods or FET hours) and 117 - the duration of total activities"
		" (in periods or FET hours). In this example we have 2 inactive activities with their combined duration being 3 periods.");
	
	LongTextMessageBox::largeInformation(this, tr("FET Help"), s);
}

void AdvancedLockUnlockForm::lockAdvancedFilter(QWidget* parent)
{
	if(!students_schedule_ready || !teachers_schedule_ready || !rooms_buildings_schedule_ready){
		return;
	}
	
	//New Dialog
	LockAdvancedDialog taDialog(parent);

	int ok=taDialog.exec();
	if(ok==QDialog::Rejected)
		return;
	
	bool lockTime=taDialog.timeCheckBox->isChecked();
	bool lockSpace=taDialog.spaceCheckBox->isChecked();

	QString addedTimeConstraintsString;
	QString notAddedTimeConstraintsString;
	
	QList<ConstraintActivityPreferredStartingTime*> addedTimeConstraints;
	QList<ConstraintActivityPreferredStartingTime*> notAddedTimeConstraints;
	
	QString addedSpaceConstraintsString;
	QString notAddedSpaceConstraintsString;
	
	QList<ConstraintActivityPreferredRoom*> addedSpaceConstraints;
	QList<ConstraintActivityPreferredRoom*> notAddedSpaceConstraints;
	
	int addedTime=0, notAddedTime=0;
	int addedSpace=0, notAddedSpace=0;

	for(int i : std::as_const(taDialog.filteredActivitiesIndicesList)){
		//Activity* act=&gt.rules.internalActivitiesList[i];
		if(best_solution.times[i]!=UNALLOCATED_TIME){
			assert(best_solution.times[i]>=0 && best_solution.times[i]<gt.rules.nHoursPerWeek);
			int d=best_solution.times[i]%gt.rules.nDaysPerWeek;
			int h=best_solution.times[i]/gt.rules.nDaysPerWeek;
			
			ConstraintActivityPreferredStartingTime* newTimeCtr=nullptr;
			
			ConstraintActivityPreferredRoom* newSpaceCtr=nullptr;
			
			if(lockTime){
				newTimeCtr=new ConstraintActivityPreferredStartingTime(100.0, gt.rules.internalActivitiesList[i].id, d, h, false);
			}
			
			if(lockSpace){
				if(best_solution.rooms[i]!=UNALLOCATED_SPACE && best_solution.rooms[i]!=UNSPECIFIED_ROOM){
					QStringList tl;
					if(gt.rules.internalRoomsList[best_solution.rooms[i]]->isVirtual==false)
						assert(best_solution.realRoomsList[i].isEmpty());
					else
						for(int rr : std::as_const(best_solution.realRoomsList[i]))
							tl.append(gt.rules.internalRoomsList[rr]->name);

					newSpaceCtr=new ConstraintActivityPreferredRoom(100.0, gt.rules.internalActivitiesList[i].id, gt.rules.internalRoomsList[best_solution.rooms[i]]->name, tl, false);
				}
			}
			
			int aid=gt.rules.internalActivitiesList[i].id;
			
			if(newTimeCtr!=nullptr){
				bool add=true;
				
				for(ConstraintActivityPreferredStartingTime* tc : gt.rules.apstHash.value(aid, QSet<ConstraintActivityPreferredStartingTime*>())){
					if((*tc) == (*newTimeCtr)){
						add=false;
						break;
					}
				}
				
				if(add){
					addedTimeConstraintsString+=newTimeCtr->getDetailedDescription(gt.rules)+"\n";
					addedTimeConstraints.append(newTimeCtr);
					
					addedTime++;
				}
				else{
					notAddedTimeConstraintsString+=newTimeCtr->getDetailedDescription(gt.rules)+"\n";
					notAddedTimeConstraints.append(newTimeCtr);
					
					notAddedTime++;
				}
			}

			if(newSpaceCtr!=nullptr){
				bool add=true;
				
				for(ConstraintActivityPreferredRoom* tc : gt.rules.aprHash.value(aid, QSet<ConstraintActivityPreferredRoom*>())){
					if((*tc) == (*newSpaceCtr)){
						add=false;
						break;
					}
				}
				
				if(add){
					addedSpaceConstraintsString+=newSpaceCtr->getDetailedDescription(gt.rules)+"\n";
					addedSpaceConstraints.append(newSpaceCtr);
					
					addedSpace++;
				}
				else{
					notAddedSpaceConstraintsString+=newSpaceCtr->getDetailedDescription(gt.rules)+"\n";
					notAddedSpaceConstraints.append(newSpaceCtr);
					
					notAddedSpace++;
				}
			}
		}
	}
	
	////////////
	//last confirmation dialog
	QDialog lastConfirmationDialog(&taDialog);
	lastConfirmationDialog.setWindowTitle(tr("Last confirmation needed"));

	QVBoxLayout* lastMainLayout=new QVBoxLayout(&lastConfirmationDialog);

	QPushButton* lastpb1=new QPushButton(tr("Cancel"));
	QPushButton* lastpb2=new QPushButton(tr("OK"));
	QHBoxLayout* lastButtons=new QHBoxLayout();
	lastButtons->addStretch();
	lastButtons->addWidget(lastpb1);
	lastButtons->addWidget(lastpb2);
	
	////texts
	QVBoxLayout* left=new QVBoxLayout();
	QVBoxLayout* right=new QVBoxLayout();
	QHBoxLayout* all=new QHBoxLayout();
	all->addLayout(left);
	all->addLayout(right);
	
	QTextEdit* addTim=new QTextEdit();
	addTim->setReadOnly(true);
	addTim->setPlainText(addedTimeConstraintsString);
	QLabel* labAddTim=new QLabel(tr("These time constraints will be added"));
	labAddTim->setWordWrap(true);
	labAddTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labAddTim);
	left->addWidget(addTim);

	QTextEdit* notAddTim=new QTextEdit();
	notAddTim->setReadOnly(true);
	notAddTim->setPlainText(notAddedTimeConstraintsString);
	QLabel* labNotAddTim=new QLabel(tr("These time constraints will NOT be added"));
	labNotAddTim->setWordWrap(true);
	labNotAddTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labNotAddTim);
	left->addWidget(notAddTim);

	QTextEdit* addSpa=new QTextEdit();
	addSpa->setReadOnly(true);
	addSpa->setPlainText(addedSpaceConstraintsString);
	QLabel* labAddSpa=new QLabel(tr("These space constraints will be added"));
	labAddSpa->setWordWrap(true);
	labAddSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labAddSpa);
	right->addWidget(addSpa);
	
	QTextEdit* notAddSpa=new QTextEdit();
	notAddSpa->setReadOnly(true);
	notAddSpa->setPlainText(notAddedSpaceConstraintsString);
	QLabel* labNotAddSpa=new QLabel(tr("These space constraints will NOT be added"));
	labNotAddSpa->setWordWrap(true);
	labNotAddSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labNotAddSpa);
	right->addWidget(notAddSpa);
	////end texts
	
	////////
	lastMainLayout->addLayout(all);
	//lastMainLayout->addStretch();
	lastMainLayout->addLayout(lastButtons);

	connect(lastpb2, &QPushButton::clicked, &lastConfirmationDialog, &QDialog::accept);
	connect(lastpb1, &QPushButton::clicked, &lastConfirmationDialog, &QDialog::reject);

	lastpb2->setDefault(true);
	lastpb2->setFocus();
	
	int lw=lastConfirmationDialog.sizeHint().width();
	int lh=lastConfirmationDialog.sizeHint().height();
	lastConfirmationDialog.resize(lw,lh);
	centerWidgetOnScreen(&lastConfirmationDialog);
	restoreFETDialogGeometry(&lastConfirmationDialog, lockActivityTagConfirmationSettingsString);
	ok=lastConfirmationDialog.exec();
	saveFETDialogGeometry(&lastConfirmationDialog, lockActivityTagConfirmationSettingsString);
	if(!ok)
		return;
	////////////
	
	for(TimeConstraint* tc : std::as_const(addedTimeConstraints)){
		bool t=gt.rules.addTimeConstraint(tc);
		assert(t);
	}
	addedTimeConstraints.clear();
	for(TimeConstraint* tc : std::as_const(notAddedTimeConstraints)){
		delete tc;
	}
	notAddedTimeConstraints.clear();
	
	for(SpaceConstraint* sc : std::as_const(addedSpaceConstraints)){
		bool t=gt.rules.addSpaceConstraint(sc);
		assert(t);
	}
	addedSpaceConstraints.clear();
	for(SpaceConstraint* sc : std::as_const(notAddedSpaceConstraints)){
		delete sc;
	}
	notAddedSpaceConstraints.clear();
	
	QMessageBox::information(&lastConfirmationDialog, tr("FET information"), tr("There were added %1 locking time constraints and"
		" %2 locking space constraints. There were not added %3 locking time constraints and %4 locking space constraints, because"
		" these activities were already locked").arg(addedTime).arg(addedSpace).arg(notAddedTime).arg(notAddedSpace));
	
	if(addedTime>0 || addedSpace>0)
		gt.rules.addUndoPoint(tr("Locked the activities with an advanced filter, by adding %1 locking time constraints and %2 locking space constraints.")
		 .arg(addedTime).arg(addedSpace));
	
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	LockUnlock::increaseCommunicationSpinBox();
}

UnlockAdvancedDialog::UnlockAdvancedDialog(QWidget* parent) : QDialog(parent)
{
	setWindowTitle(tr("FET - Unlock the activities which correspond to the selected advanced filter"));

	QVBoxLayout* taMainLayout=new QVBoxLayout(this);

	numberLabel=new QLabel(QString("No: 1000 / 1000"));
	numberLabel->setAlignment(Qt::AlignCenter);
	durationLabel=new QLabel(QString("Dur: 1000 / 1000"));
	durationLabel->setAlignment(Qt::AlignCenter);
	QHBoxLayout* labelsLayout=new QHBoxLayout();
	labelsLayout->addWidget(numberLabel);
	labelsLayout->addWidget(durationLabel);

	QLabel* taLabel=new QLabel();
	taLabel->setWordWrap(true);
	taLabel->setText(tr("All the filtered activities will be unlocked"));
	//taLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	
	selectedActivitiesLW=new QListWidget();
	
	filterCheckBox=new QCheckBox(tr("Filter"));
	filterCheckBox->setChecked(false);
	QPushButton* helpPB=new QPushButton(tr("Help"));
	QPushButton* tapb1=new QPushButton(tr("Cancel"));
	QPushButton* tapb2=new QPushButton(tr("OK"));
	
	QHBoxLayout* buttons=new QHBoxLayout();
	buttons->addWidget(filterCheckBox);
	buttons->addStretch();
	buttons->addWidget(helpPB);
	buttons->addWidget(tapb1);
	buttons->addWidget(tapb2);
	
	////////
	QGroupBox* selectorsGroupBox=new QGroupBox();
	selectorsGroupBox->setTitle(tr("Unlock"));
	
	QHBoxLayout* groupLayout=new QHBoxLayout(selectorsGroupBox);
	
	timeCheckBox=new QCheckBox();
	timeCheckBox->setChecked(true);
	timeCheckBox->setText(tr("Time"));
	spaceCheckBox=new QCheckBox();
	spaceCheckBox->setChecked(true);
	spaceCheckBox->setText(tr("Space"));

	groupLayout->addWidget(timeCheckBox);
	groupLayout->addWidget(spaceCheckBox);
	groupLayout->addStretch();
	/////////
	
	taMainLayout->addLayout(labelsLayout);
	taMainLayout->addWidget(taLabel);
	taMainLayout->addWidget(selectedActivitiesLW);
	taMainLayout->addWidget(selectorsGroupBox);
	taMainLayout->addLayout(buttons);

	connect(tapb2, &QPushButton::clicked, this, &UnlockAdvancedDialog::accept);
	connect(tapb1, &QPushButton::clicked, this, &UnlockAdvancedDialog::reject);
	connect(helpPB, &QPushButton::clicked, this, &UnlockAdvancedDialog::help);
	connect(filterCheckBox, &QCheckBox::toggled, this, &UnlockAdvancedDialog::filter);

	//////////////////
	QSettings settings(COMPANY, PROGRAM);

	QString settingsName="UnlockAdvancedDialog";
	
	all=settings.value(settingsName+"/all-conditions", "true").toBool();
	
	descrDetDescrDetDescrWithConstraints.clear();
	int n=settings.value(settingsName+"/number-of-descriptions", "1").toInt();
	for(int i=0; i<n; i++)
		descrDetDescrDetDescrWithConstraints.append(settings.value(settingsName+"/description/"+CustomFETString::number(i+1), CustomFETString::number(DESCRIPTION)).toInt());
	
	contains.clear();
	n=settings.value(settingsName+"/number-of-contains", "1").toInt();
	for(int i=0; i<n; i++)
		contains.append(settings.value(settingsName+"/contains/"+CustomFETString::number(i+1), CustomFETString::number(CONTAINS)).toInt());
	
	text.clear();
	n=settings.value(settingsName+"/number-of-texts", "1").toInt();
	for(int i=0; i<n; i++)
		text.append(settings.value(settingsName+"/text/"+CustomFETString::number(i+1), QString("")).toString());
	
	caseSensitive=settings.value(settingsName+"/case-sensitive", "false").toBool();
	
	assert(filterCheckBox->isChecked()==false);
	//////////////////
	
	tapb2->setDefault(true);
	tapb2->setFocus();
	
	int w=this->sizeHint().width();
	int h=this->sizeHint().height();
	if(w<MIN_WIDTH)
		w=MIN_WIDTH;
	if(h<MIN_HEIGHT)
		h=MIN_HEIGHT;
	this->resize(w,h);
	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this, unlockAdvancedFilterSettingsString);
	
	filterChanged();
}

UnlockAdvancedDialog::~UnlockAdvancedDialog()
{
	saveFETDialogGeometry(this, unlockAdvancedFilterSettingsString);
	
	QSettings settings(COMPANY, PROGRAM);

	QString settingsName="UnlockAdvancedDialog";
	
	settings.setValue(settingsName+"/all-conditions", all);
	
	settings.setValue(settingsName+"/number-of-descriptions", descrDetDescrDetDescrWithConstraints.count());
	settings.remove(settingsName+"/description");
	for(int i=0; i<descrDetDescrDetDescrWithConstraints.count(); i++)
		settings.setValue(settingsName+"/description/"+CustomFETString::number(i+1), descrDetDescrDetDescrWithConstraints.at(i));
	
	settings.setValue(settingsName+"/number-of-contains", contains.count());
	settings.remove(settingsName+"/contains");
	for(int i=0; i<contains.count(); i++)
		settings.setValue(settingsName+"/contains/"+CustomFETString::number(i+1), contains.at(i));
	
	settings.setValue(settingsName+"/number-of-texts", text.count());
	settings.remove(settingsName+"/text");
	for(int i=0; i<text.count(); i++)
		settings.setValue(settingsName+"/text/"+CustomFETString::number(i+1), text.at(i));
	
	settings.setValue(settingsName+"/case-sensitive", caseSensitive);
}

bool UnlockAdvancedDialog::filterOk(Activity* act)
{
	if(!filterCheckBox->isChecked()){
		return true;
	}
	else{
		/////////////////////advanced filter
		assert(descrDetDescrDetDescrWithConstraints.count()==contains.count());
		assert(contains.count()==text.count());

		Qt::CaseSensitivity csens=Qt::CaseSensitive;
		if(!caseSensitive)
			csens=Qt::CaseInsensitive;

		QList<int> perm;
		for(int i=0; i<descrDetDescrDetDescrWithConstraints.count(); i++)
			perm.append(i);
		//Below we do a stable sorting, so that first inputted filters are hopefully filtering out more entries. This is written in the help when interrupting the
		//filtering of the activities, see the progress.wasCanceled() message.
		std::stable_sort(perm.begin(), perm.end(), [this](int a, int b){return descrDetDescrDetDescrWithConstraints.at(a)<descrDetDescrDetDescrWithConstraints.at(b);});
		for(int i=0; i<perm.count()-1; i++)
			assert(descrDetDescrDetDescrWithConstraints.at(perm.at(i))<=descrDetDescrDetDescrWithConstraints.at(perm.at(i+1)));

		int firstPosWithDescr=-1;
		int firstPosWithDetDescr=-1;
		int firstPosWithConstraints=-1;
		for(int i=0; i<perm.count(); i++){
			if(descrDetDescrDetDescrWithConstraints.at(perm.at(i))==DESCRIPTION && firstPosWithDescr==-1){
				firstPosWithDescr=i;
			}
			else if(descrDetDescrDetDescrWithConstraints.at(perm.at(i))==DETDESCRIPTION && firstPosWithDetDescr==-1){
				firstPosWithDetDescr=i;
			}
			else if(descrDetDescrDetDescrWithConstraints.at(perm.at(i))==DETDESCRIPTIONWITHCONSTRAINTS && firstPosWithConstraints==-1){
				firstPosWithConstraints=i;
			}
		}
		
		QString s=QString("");
		for(int i=0; i<perm.count(); i++){
			if(descrDetDescrDetDescrWithConstraints.at(perm.at(i))==DESCRIPTION){
				assert(firstPosWithDescr>=0);
				
				if(i==firstPosWithDescr)
					s=act->getDescription(gt.rules);
			}
			else if(descrDetDescrDetDescrWithConstraints.at(perm.at(i))==DETDESCRIPTION){
				assert(firstPosWithDetDescr>=0);
				
				if(i==firstPosWithDetDescr)
					s=act->getDetailedDescription(gt.rules);
			}
			else{
				assert(descrDetDescrDetDescrWithConstraints.at(perm.at(i))==DETDESCRIPTIONWITHCONSTRAINTS);
				
				assert(firstPosWithConstraints>=0);
				
				if(i==firstPosWithConstraints)
					s=act->getDetailedDescriptionWithConstraints(gt.rules); //warning: time consuming operation, goes through all the constraints and group activities in.initial order items.
			}
			
			bool okPartial=true; //We initialize okPartial to silence a MinGW 11.2.0 warning of type 'this variable might be used uninitialized'.
			
			QString t=text.at(perm.at(i));
			if(contains.at(perm.at(i))==CONTAINS){
				okPartial=s.contains(t, csens);
			}
			else if(contains.at(perm.at(i))==DOESNOTCONTAIN){
				okPartial=!(s.contains(t, csens));
			}
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
			else if(contains.at(perm.at(i))==REGEXP){
				QRegularExpression regExp(t);
				if(!caseSensitive)
					regExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
				okPartial=(regExp.match(s)).hasMatch();
			}
			else if(contains.at(perm.at(i))==NOTREGEXP){
				QRegularExpression regExp(t);
				if(!caseSensitive)
					regExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
				okPartial=!(regExp.match(s)).hasMatch();
			}
#else
			else if(contains.at(perm.at(i))==REGEXP){
				QRegExp regExp(t);
				regExp.setCaseSensitivity(csens);
				okPartial=(regExp.indexIn(s)>=0);
			}
			else if(contains.at(perm.at(i))==NOTREGEXP){
				QRegExp regExp(t);
				regExp.setCaseSensitivity(csens);
				okPartial=(regExp.indexIn(s)<0);
			}
#endif
			else
				assert(0);
				
			if(all && !okPartial)
				return false;
			else if(!all && okPartial)
				return true;
		}
		
		return all;
	}
}

void UnlockAdvancedDialog::filterChanged()
{
	int /*nacts=0,*/ nsubacts=0, nh=0;
	int ninact=0, ninacth=0;

	filteredActivitiesIdsList.clear();
	selectedActivitiesLW->clear();
	
	QProgressDialog progress(this);
	progress.setWindowTitle(tr("Filtering the activities", "Title of a progress dialog"));
	progress.setLabelText(tr("Filtering the activities ... please wait"));
	progress.setRange(0, qMax(gt.rules.activitiesList.size(), 1));
	progress.setModal(true);

	int k=0;

	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		progress.setValue(i);
		if(progress.wasCanceled()){
			LongTextMessageBox::largeInformation(this, tr("FET warning"), tr("You canceled the filtering of the activities - the list of activities will be incomplete.")+QString(" ")+
			 tr("Note: if filtering of the activities takes too much, it might be because you are filtering on the detailed description with constraints of the activities,"
			 " which checks each activity against each time constraint, each space constraint, and each group activities in the initial order item, which might be too much."
			 " Please consider filtering on the description or detailed description of the activities, instead.")+QString("\n\n")+tr("Note: if you are using more filters,"
			 " like 'the activity should have a certain teacher', you might obtain much faster results if you filter on the description/detailed description of the activity"
			 " for the teacher and on the detailed description with constraints for other filters (meaning that you should prefer using the description/detailed description"
			 " instead of the detailed description with constraints, whenever it is possible).")+QString(" ")+tr("Also, the order of the detailed description with constraints"
			 " filters is important: you should put firstly the ones which filter out more activities (if you selected the 'All' radio button) or put firstly the ones which"
			 " accept immediately more activities (if you selected the 'Any' radio button)."));
			break;
		}
		
		Activity* act=gt.rules.activitiesList.at(i);
		if(filterOk(act)){
			filteredActivitiesIdsList.append(act->id);
			selectedActivitiesLW->addItem(act->getDescription(gt.rules));

			k++;

			if(!act->active){
				selectedActivitiesLW->item(k-1)->setBackground(selectedActivitiesLW->palette().brush(QPalette::Disabled, QPalette::Window));
				selectedActivitiesLW->item(k-1)->setForeground(selectedActivitiesLW->palette().brush(QPalette::Disabled, QPalette::WindowText));
			}
			
			//if(act->id==act->activityGroupId || act->activityGroupId==0)
			//	nacts++;
			nsubacts++;
			
			nh+=act->duration;
			
			if(!act->active){
				ninact++;
				ninacth+=act->duration;
			}
		}
	}

	progress.setValue(qMax(gt.rules.activitiesList.size(), 1));

	assert(nsubacts-ninact>=0);
	int NA=nsubacts-ninact;
	int NT=nsubacts;
	assert(nh-ninacth>=0);
	int DA=nh-ninacth;
	int DT=nh;
	numberLabel->setText(tr("No: %1 / %2", "No means number, %1 is the number of active activities, %2 is the total number of activities."
		" Please leave spaces between fields, so that they are better visible").arg(NA).arg(NT));
	durationLabel->setText(tr("Dur: %1 / %2", "Dur means duration, %1 is the duration of active activities, %2 is the total duration of activities."
		" Please leave spaces between fields, so that they are better visible").arg(DA).arg(DT));
}

void UnlockAdvancedDialog::filter(bool selected)
{
	if(!selected){
		filterChanged();
		return;
	}
	
	filterForm=new AdvancedFilterForm(this, tr("Advanced filter for activities"), true, all, descrDetDescrDetDescrWithConstraints, contains, text, caseSensitive, "ActivitiesUnlockAdvancedDialog");
	int t=filterForm->exec();
	if(t==QDialog::Accepted){
		if(filterForm->allRadio->isChecked())
			all=true;
		else if(filterForm->anyRadio->isChecked())
			all=false;
		else
			assert(0);
		
		caseSensitive=filterForm->caseSensitiveCheckBox->isChecked();
		
		descrDetDescrDetDescrWithConstraints.clear();
		contains.clear();
		text.clear();
		
		assert(filterForm->descrDetDescrDetDescrWithConstraintsComboBoxList.count()==filterForm->contNContReNReComboBoxList.count());
		assert(filterForm->descrDetDescrDetDescrWithConstraintsComboBoxList.count()==filterForm->textLineEditList.count());
		for(int i=0; i<filterForm->rows; i++){
			QComboBox* cb1=filterForm->descrDetDescrDetDescrWithConstraintsComboBoxList.at(i);
			QComboBox* cb2=filterForm->contNContReNReComboBoxList.at(i);
			QLineEdit* tl=filterForm->textLineEditList.at(i);
			
			descrDetDescrDetDescrWithConstraints.append(cb1->currentIndex());
			contains.append(cb2->currentIndex());
			text.append(tl->text());
		}
		
		filterChanged();
	}
	else{
		disconnect(filterCheckBox, &QCheckBox::toggled, this, &UnlockAdvancedDialog::filter);
		filterCheckBox->setChecked(false);
		connect(filterCheckBox, &QCheckBox::toggled, this, &UnlockAdvancedDialog::filter);
	}
	
	delete filterForm;
}

void UnlockAdvancedDialog::help()
{
	QString s;
	
	s+=tr("Above the activities list we have 2 labels, containing 4 numbers. The first label contains text: No: a / b. The first number a is the"
		" number of active activities (we number each individual subactivity as 1), while the second number b is the number of total activities."
		" The second label contains text: Dur: c / d. The third number c is the duration of active activities, in periods"
		" (or FET hours), while the fourth number d is the duration of total activities, in periods (or FET hours)."
		" So, No means number and Dur means duration.");
	s+="\n\n";
	s+=tr("Example: No: 100 / 102, Dur: 114 / 117. They represent: 100 - the number of active activities,"
		" then 102 - the number of total activities,"
		" 114 - the duration of active activities (in periods or FET hours) and 117 - the duration of total activities"
		" (in periods or FET hours). In this example we have 2 inactive activities with their combined duration being 3 periods.");
	
	LongTextMessageBox::largeInformation(this, tr("FET Help"), s);
}

void AdvancedLockUnlockForm::unlockAdvancedFilterWithoutTimetable(QWidget* parent)
{
	/*if(!students_schedule_ready || !teachers_schedule_ready || !rooms_buildings_schedule_ready){
		return;
	}*/
	QSet<QString> virtualRooms;
	for(Room* rm : std::as_const(gt.rules.roomsList))
		if(rm->isVirtual==true)
			virtualRooms.insert(rm->name);
	
	//New Dialog
	UnlockAdvancedDialog taDialog(parent);
	
	int ok=taDialog.exec();
	if(ok==QDialog::Rejected)
		return;
	
	bool unlockTime=taDialog.timeCheckBox->isChecked();
	bool unlockSpace=taDialog.spaceCheckBox->isChecked();
	//We need to consider non-internal data, because the timetable is not available
	//int selectedActivityTagInt=taLW->currentIndex();
	//assert(selectedActivityTagInt>=0 && selectedActivityTagInt<gt.rules.nInternalActivityTags);

	int removedTime=0, notRemovedTime=0;
	int removedSpace=0, notRemovedSpace=0;
	
	QString removedTimeConstraintsString;
	QString notRemovedTimeConstraintsString;

	QList<TimeConstraint*> removedTimeConstraints;
	QList<TimeConstraint*> notRemovedTimeConstraints;
	
	QString removedSpaceConstraintsString;
	QString notRemovedSpaceConstraintsString;

	QList<SpaceConstraint*> removedSpaceConstraints;
	QList<SpaceConstraint*> notRemovedSpaceConstraints;
	
	QSet<int> actsSet;
	
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
	QSet<int> filteredActivitiesIdsSet=QSet<int>(taDialog.filteredActivitiesIdsList.constBegin(), taDialog.filteredActivitiesIdsList.constEnd());
#else
	QSet<int> filteredActivitiesIdsSet=taDialog.filteredActivitiesIdsList.toSet();
#endif
	
	if(unlockTime || unlockSpace) // ||unlockSpace is added on 2018-06-26, correcting a bug if the user only selected Space and the timetable was not generated.
		for(TimeConstraint* tc : std::as_const(gt.rules.timeConstraintsList))
			if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME){
				ConstraintActivityPreferredStartingTime* c=(ConstraintActivityPreferredStartingTime*)tc;
				if(c->weightPercentage==100.0 && c->day>=0 && c->hour>=0){
					if(filteredActivitiesIdsSet.contains(c->activityId)){
						actsSet.insert(c->activityId);
						if(unlockTime){
							if(!c->permanentlyLocked){
								removedTimeConstraints.append((TimeConstraint*)c);
								//removedTimeConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
								removedTime++;
							}
							else{
								notRemovedTimeConstraints.append((TimeConstraint*)c);
								//notRemovedTimeConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
								notRemovedTime++;
							}
						}
					}
				}
			}
			
	if(unlockSpace)
		for(SpaceConstraint* sc : std::as_const(gt.rules.spaceConstraintsList))
			if(sc->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
				ConstraintActivityPreferredRoom* c=(ConstraintActivityPreferredRoom*) sc;
				if(c->weightPercentage==100.0 && actsSet.contains(c->activityId) &&
				 (!virtualRooms.contains(c->roomName) || (virtualRooms.contains(c->roomName) && !c->preferredRealRoomsNames.isEmpty()))){
					if(!c->permanentlyLocked){
						removedSpaceConstraints.append((SpaceConstraint*)c);
						//removedSpaceConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						removedSpace++;
					}
					else{
						notRemovedSpaceConstraints.append((SpaceConstraint*)c);
						//notRemovedSpaceConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						notRemovedSpace++;
					}
				}
			}

	std::stable_sort(removedTimeConstraints.begin(), removedTimeConstraints.end(), compareTimeConstraintsActivityPreferredStartingTimeActivitiesIds);
	for(TimeConstraint* tc : std::as_const(removedTimeConstraints))
		removedTimeConstraintsString+=tc->getDetailedDescription(gt.rules)+"\n";
		
	std::stable_sort(notRemovedTimeConstraints.begin(), notRemovedTimeConstraints.end(), compareTimeConstraintsActivityPreferredStartingTimeActivitiesIds);
	for(TimeConstraint* tc : std::as_const(notRemovedTimeConstraints))
		notRemovedTimeConstraintsString+=tc->getDetailedDescription(gt.rules)+"\n";

	std::stable_sort(removedSpaceConstraints.begin(), removedSpaceConstraints.end(), compareSpaceConstraintsActivityPreferredRoomActivitiesIds);
	for(SpaceConstraint* sc : std::as_const(removedSpaceConstraints))
		removedSpaceConstraintsString+=sc->getDetailedDescription(gt.rules)+"\n";

	std::stable_sort(notRemovedSpaceConstraints.begin(), notRemovedSpaceConstraints.end(), compareSpaceConstraintsActivityPreferredRoomActivitiesIds);
	for(SpaceConstraint* sc : std::as_const(notRemovedSpaceConstraints))
		notRemovedSpaceConstraintsString+=sc->getDetailedDescription(gt.rules)+"\n";

	////////////
	//last confirmation dialog
	QDialog lastConfirmationDialog(&taDialog);
	lastConfirmationDialog.setWindowTitle(tr("Last confirmation needed"));

	QVBoxLayout* lastMainLayout=new QVBoxLayout(&lastConfirmationDialog);

	QPushButton* lastpb1=new QPushButton(tr("Cancel"));
	QPushButton* lastpb2=new QPushButton(tr("OK"));
	QHBoxLayout* lastButtons=new QHBoxLayout();
	lastButtons->addStretch();
	lastButtons->addWidget(lastpb1);
	lastButtons->addWidget(lastpb2);
	
	////texts
	QVBoxLayout* left=new QVBoxLayout();
	QVBoxLayout* right=new QVBoxLayout();
	QHBoxLayout* all=new QHBoxLayout();
	all->addLayout(left);
	all->addLayout(right);
	
	QTextEdit* remTim=new QTextEdit();
	remTim->setReadOnly(true);
	remTim->setPlainText(removedTimeConstraintsString);
	QLabel* labRemTim=new QLabel(tr("These time constraints will be removed"));
	labRemTim->setWordWrap(true);
	labRemTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labRemTim);
	left->addWidget(remTim);

	QTextEdit* notRemTim=new QTextEdit();
	notRemTim->setReadOnly(true);
	notRemTim->setPlainText(notRemovedTimeConstraintsString);
	QLabel* labNotRemTim=new QLabel(tr("These time constraints will NOT be removed"));
	labNotRemTim->setWordWrap(true);
	labNotRemTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labNotRemTim);
	left->addWidget(notRemTim);

	QTextEdit* remSpa=new QTextEdit();
	remSpa->setReadOnly(true);
	remSpa->setPlainText(removedSpaceConstraintsString);
	QLabel* labRemSpa=new QLabel(tr("These space constraints will be removed"));
	labRemSpa->setWordWrap(true);
	labRemSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labRemSpa);
	right->addWidget(remSpa);
	
	QTextEdit* notRemSpa=new QTextEdit();
	notRemSpa->setReadOnly(true);
	notRemSpa->setPlainText(notRemovedSpaceConstraintsString);
	QLabel* labNotRemSpa=new QLabel(tr("These space constraints will NOT be removed"));
	labNotRemSpa->setWordWrap(true);
	labNotRemSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labNotRemSpa);
	right->addWidget(notRemSpa);
	////end texts
	
	////////
	lastMainLayout->addLayout(all);
	//lastMainLayout->addStretch();
	lastMainLayout->addLayout(lastButtons);

	connect(lastpb2, &QPushButton::clicked, &lastConfirmationDialog, &QDialog::accept);
	connect(lastpb1, &QPushButton::clicked, &lastConfirmationDialog, &QDialog::reject);

	lastpb2->setDefault(true);
	lastpb2->setFocus();
	
	int lw=lastConfirmationDialog.sizeHint().width();
	int lh=lastConfirmationDialog.sizeHint().height();
	lastConfirmationDialog.resize(lw,lh);
	centerWidgetOnScreen(&lastConfirmationDialog);
	restoreFETDialogGeometry(&lastConfirmationDialog, unlockActivityTagConfirmationSettingsString);
	ok=lastConfirmationDialog.exec();
	saveFETDialogGeometry(&lastConfirmationDialog, unlockActivityTagConfirmationSettingsString);
	if(!ok)
		return;
	////////////
	
	bool t=gt.rules.removeTimeConstraints(removedTimeConstraints);
	assert(t);
	
	removedTimeConstraints.clear();
	notRemovedTimeConstraints.clear();

	t=gt.rules.removeSpaceConstraints(removedSpaceConstraints);
	assert(t);
	
	removedSpaceConstraints.clear();
	notRemovedSpaceConstraints.clear();

	QMessageBox::information(&lastConfirmationDialog, tr("FET information"), tr("There were removed %1 locking time constraints and"
		" %2 locking space constraints. There were not removed %3 locking time constraints and %4 locking space constraints, because"
		" these activities were permanently locked").arg(removedTime).arg(removedSpace).arg(notRemovedTime).arg(notRemovedSpace));
	
	if(removedTime>0 || removedSpace>0)
		gt.rules.addUndoPoint(tr("Unlocked the activities with an advanced filter, by removing %1 locking time constraints and %2 locking space constraints.")
		 .arg(removedTime).arg(removedSpace));

	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	LockUnlock::increaseCommunicationSpinBox();
}
