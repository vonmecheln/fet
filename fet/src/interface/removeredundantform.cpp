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

#include "removeredundantform.h"

#include "timetable.h"

extern Timetable gt;

#include <QHash>
#include <QMultiHash>
#include <QList>
#include <QSet>
#include <QQueue>

#include <QMessageBox>

#include <QPushButton>
#include <QCheckBox>
#include <QTextEdit>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <algorithm>
//using namespace std;

RemoveRedundantForm::RemoveRedundantForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	if(gt.rules.mode!=MORNINGS_AFTERNOONS)
		minHalfDaysGroupBox->setEnabled(false);
	
	okPushButton->setDefault(true);
	
	connect(okPushButton, &QPushButton::clicked, this, &RemoveRedundantForm::wasAccepted);
	connect(cancelPushButton, &QPushButton::clicked, this, &RemoveRedundantForm::wasCanceled);
	
	removeRedundantCheckBox->setChecked(true);
	removeRedundantHalfCheckBox->setChecked(true);
	
	connect(removeRedundantCheckBox, &QCheckBox::toggled, this, &RemoveRedundantForm::removeRedundantCheckBox_toggled);
	connect(removeRedundantHalfCheckBox, &QCheckBox::toggled, this, &RemoveRedundantForm::removeRedundantHalfCheckBox_toggled);
	
	removeRedundantCheckBox_toggled();
	removeRedundantHalfCheckBox_toggled();
}

RemoveRedundantForm::~RemoveRedundantForm()
{
	saveFETDialogGeometry(this);
}

void RemoveRedundantForm::wasAccepted()
{
	QMultiHash<int, int> adjMatrix1;
	QMultiHash<int, int> adjMatrix2;
	
	for(TimeConstraint* tc : std::as_const(gt.rules.timeConstraintsList)){
		if(tc->active && tc->weightPercentage==100.0){
			if(tc->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME){
				ConstraintActivitiesSameStartingTime* cst=(ConstraintActivitiesSameStartingTime*) tc;
				
				for(int i=1; i<cst->n_activities; i++){
					adjMatrix1.insert(cst->activitiesIds[0], cst->activitiesIds[i]);
					adjMatrix1.insert(cst->activitiesIds[i], cst->activitiesIds[0]);
					
					adjMatrix2.insert(cst->activitiesIds[0], cst->activitiesIds[i]);
					adjMatrix2.insert(cst->activitiesIds[i], cst->activitiesIds[0]);
				}
			}
			else if(tc->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY){
				ConstraintActivitiesSameStartingDay* csd=(ConstraintActivitiesSameStartingDay*) tc;
				
				for(int i=1; i<csd->n_activities; i++){
					adjMatrix1.insert(csd->activitiesIds[0], csd->activitiesIds[i]);
					adjMatrix1.insert(csd->activitiesIds[i], csd->activitiesIds[0]);
					
					adjMatrix2.insert(csd->activitiesIds[0], csd->activitiesIds[i]);
					adjMatrix2.insert(csd->activitiesIds[i], csd->activitiesIds[0]);
				}
			}
			else if(tc->type==CONSTRAINT_MAX_DAYS_BETWEEN_ACTIVITIES){
				ConstraintMaxDaysBetweenActivities* csd=(ConstraintMaxDaysBetweenActivities*) tc;
				if(csd->maxDays==0){
					for(int i=1; i<csd->n_activities; i++){
						adjMatrix1.insert(csd->activitiesIds[0], csd->activitiesIds[i]);
						adjMatrix1.insert(csd->activitiesIds[i], csd->activitiesIds[0]);
						
						//without adjMatrix2, because the two activities can be one in the morning
						//and the other one in the afternoon (max days between activities in the
						//Mornings-Afternoons mode refers to real days).
					}
				}
			}
			else if(tc->type==CONSTRAINT_MAX_HALF_DAYS_BETWEEN_ACTIVITIES){
				ConstraintMaxHalfDaysBetweenActivities* csd=(ConstraintMaxHalfDaysBetweenActivities*) tc;
				if(csd->maxDays==0){
					for(int i=1; i<csd->n_activities; i++){
						adjMatrix1.insert(csd->activitiesIds[0], csd->activitiesIds[i]);
						adjMatrix1.insert(csd->activitiesIds[i], csd->activitiesIds[0]);
						
						adjMatrix2.insert(csd->activitiesIds[0], csd->activitiesIds[i]);
						adjMatrix2.insert(csd->activitiesIds[i], csd->activitiesIds[0]);
					}
				}
			}
		}
	}
	
	QHash<int, int> repr1;
	QHash<int, int> repr2;
	
	QQueue<int> myQueue;
	
	for(Activity* act : std::as_const(gt.rules.activitiesList)){
		int start=act->id;
		
		if(repr1.value(start, -1)==-1){ //not visited
			repr1.insert(start, start);
			myQueue.enqueue(start);
			while(!myQueue.isEmpty()){
				int crtHead=myQueue.dequeue();
				assert(repr1.value(crtHead, -1)==start);
				QList<int> neighList=adjMatrix1.values(crtHead);
				for(int neigh : std::as_const(neighList)){
					if(repr1.value(neigh, -1)==-1){
						myQueue.enqueue(neigh);
						repr1.insert(neigh, start);
					}
					else{
						assert(repr1.value(neigh, -1)==start);
					}
				}
			}
		}
	}
	
	assert(myQueue.isEmpty());
	
	for(Activity* act : std::as_const(gt.rules.activitiesList)){
		int start=act->id;
		
		if(repr2.value(start, -1)==-1){ //not visited
			repr2.insert(start, start);
			myQueue.enqueue(start);
			while(!myQueue.isEmpty()){
				int crtHead=myQueue.dequeue();
				assert(repr2.value(crtHead, -1)==start);
				QList<int> neighList=adjMatrix2.values(crtHead);
				for(int neigh : std::as_const(neighList)){
					if(repr2.value(neigh, -1)==-1){
						myQueue.enqueue(neigh);
						repr2.insert(neigh, start);
					}
					else{
						assert(repr2.value(neigh, -1)==start);
					}
				}
			}
		}
	}
	
	assert(myQueue.isEmpty());
	
	//min days
	QList<ConstraintMinDaysBetweenActivities*> mdcList;
	
	for(TimeConstraint* tc : std::as_const(gt.rules.timeConstraintsList)){
		if(tc->active && tc->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES){
			mdcList.append((ConstraintMinDaysBetweenActivities*)tc);
		}
	}
	
	std::reverse(mdcList.begin(), mdcList.end()); //inverse order, so that we will remove the older constraints
	
	std::stable_sort(mdcList.begin(), mdcList.end(), [&crepr1=std::as_const(repr1)](ConstraintMinDaysBetweenActivities* a, ConstraintMinDaysBetweenActivities* b){
		QList<int> la1=a->activitiesIds;
		QList<int> la2;
		for(int k : std::as_const(la1)){
			int m=crepr1.value(k, -1);
			assert(m>0);
			la2.append(m);
		}
		std::stable_sort(la2.begin(), la2.end());
		
		QList<int> lb1=b->activitiesIds;
		QList<int> lb2;
		for(int k : std::as_const(lb1)){
			int m=crepr1.value(k, -1);
			assert(m>0);
			lb2.append(m);
		}
		std::stable_sort(lb2.begin(), lb2.end());
		
		return la2<lb2 || (la2==lb2 && a->minDays < b->minDays) || (la2==lb2 && a->minDays == b->minDays && a->weightPercentage < b->weightPercentage) ||
		 (la2==lb2 && a->minDays == b->minDays && a->weightPercentage == b->weightPercentage && !a->consecutiveIfSameDay && b->consecutiveIfSameDay);
		});
	
	QSet<ConstraintMinDaysBetweenActivities*> toBeRemovedSet;
	
	for(int i=0; i<mdcList.count()-1; i++){
		ConstraintMinDaysBetweenActivities* a=mdcList.at(i);
		QList<int> la1=a->activitiesIds;
		QList<int> la2;
		for(int k : std::as_const(la1)){
		int m=repr1.value(k, -1);
			assert(m>0);
			la2.append(m);
		}
		std::stable_sort(la2.begin(), la2.end());
		
		bool contained=false;
		bool with100=false;
		for(int j=i+1; j<mdcList.count(); j++){
			ConstraintMinDaysBetweenActivities* b=mdcList.at(j);
			QList<int> lb1=b->activitiesIds;
			QList<int> lb2;
			for(int k : std::as_const(lb1)){
				int m=repr1.value(k, -1);
				assert(m>0);
				lb2.append(m);
			}
			std::stable_sort(lb2.begin(), lb2.end());
			
			if(la2==lb2 && a->minDays==b->minDays && !(a->consecutiveIfSameDay && !b->consecutiveIfSameDay)){
				contained=true;
				if(b->weightPercentage==100.0)
					with100=true;
			}
			else if(la2==lb2 && a->minDays==b->minDays){
				if(b->weightPercentage==100.0)
					with100=true;
			}
			else if(la2!=lb2 || (la2==lb2 && a->minDays!=b->minDays)){
				break;
			}
		}
		
		if(contained && !with100)
			toBeRemovedSet.insert(a);
	}
	
	//min half days
	QList<ConstraintMinHalfDaysBetweenActivities*> mdhcList;
	
	for(TimeConstraint* tc : std::as_const(gt.rules.timeConstraintsList)){
		if(tc->active && tc->type==CONSTRAINT_MIN_HALF_DAYS_BETWEEN_ACTIVITIES){
			mdhcList.append((ConstraintMinHalfDaysBetweenActivities*)tc);
		}
	}
	
	std::reverse(mdhcList.begin(), mdhcList.end()); //inverse order, so that we will remove the older constraints
	
	std::stable_sort(mdhcList.begin(), mdhcList.end(), [&crepr2=std::as_const(repr2)](ConstraintMinHalfDaysBetweenActivities* a, ConstraintMinHalfDaysBetweenActivities* b){
		QList<int> la1=a->activitiesIds;
		QList<int> la2;
		for(int k : std::as_const(la1)){
			int m=crepr2.value(k, -1);
			assert(m>0);
			la2.append(m);
		}
		std::stable_sort(la2.begin(), la2.end());
		
		QList<int> lb1=b->activitiesIds;
		QList<int> lb2;
		for(int k : std::as_const(lb1)){
			int m=crepr2.value(k, -1);
			assert(m>0);
			lb2.append(m);
		}
		std::stable_sort(lb2.begin(), lb2.end());
		
		return la2<lb2 || (la2==lb2 && a->minDays < b->minDays) || (la2==lb2 && a->minDays == b->minDays && a->weightPercentage < b->weightPercentage) ||
		 (la2==lb2 && a->minDays == b->minDays && a->weightPercentage == b->weightPercentage && !a->consecutiveIfSameDay && b->consecutiveIfSameDay);
		});
	
	QSet<ConstraintMinHalfDaysBetweenActivities*> toBeRemovedHalfSet;
	
	for(int i=0; i<mdhcList.count()-1; i++){
		ConstraintMinHalfDaysBetweenActivities* a=mdhcList.at(i);
		QList<int> la1=a->activitiesIds;
		QList<int> la2;
		for(int k : std::as_const(la1)){
		int m=repr2.value(k, -1);
			assert(m>0);
			la2.append(m);
		}
		std::stable_sort(la2.begin(), la2.end());
		
		bool contained=false;
		bool with100=false;
		for(int j=i+1; j<mdhcList.count(); j++){
			ConstraintMinHalfDaysBetweenActivities* b=mdhcList.at(j);
			QList<int> lb1=b->activitiesIds;
			QList<int> lb2;
			for(int k : std::as_const(lb1)){
				int m=repr2.value(k, -1);
				assert(m>0);
				lb2.append(m);
			}
			std::stable_sort(lb2.begin(), lb2.end());
			
			if(la2==lb2 && a->minDays==b->minDays && !(a->consecutiveIfSameDay && !b->consecutiveIfSameDay)){
				contained=true;
				if(b->weightPercentage==100.0)
					with100=true;
			}
			else if(la2==lb2 && a->minDays==b->minDays){
				if(b->weightPercentage==100.0)
					with100=true;
			}
			else if(la2!=lb2 || (la2==lb2 && a->minDays!=b->minDays)){
				break;
			}
		}
		
		if(contained && !with100)
			toBeRemovedHalfSet.insert(a);
	}
	
	//both
	QList<TimeConstraint*> toBeRemovedCombinedList;
	for(TimeConstraint* tc : std::as_const(gt.rules.timeConstraintsList))
		if(tc->active && tc->weightPercentage>0.0){
			if(tc->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES && toBeRemovedSet.contains((ConstraintMinDaysBetweenActivities*)tc))
				toBeRemovedCombinedList.append(tc);
			else if(tc->type==CONSTRAINT_MIN_HALF_DAYS_BETWEEN_ACTIVITIES && toBeRemovedHalfSet.contains((ConstraintMinHalfDaysBetweenActivities*)tc))
				toBeRemovedCombinedList.append(tc);
		}
	
	///////////
	QDialog dialog(this);
	dialog.setWindowTitle(tr("Last confirmation needed"));
	
	QVBoxLayout* top=new QVBoxLayout(&dialog);
	QLabel* topLabel=new QLabel();
	topLabel->setText(tr("Operations that will be done:"));
	top->addWidget(topLabel);
	
	QPushButton* acceptPB=new QPushButton(tr("Accept"));
	QPushButton* cancelPB=new QPushButton(tr("Cancel"));
	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch();
	hl->addWidget(acceptPB);
	hl->addWidget(cancelPB);
	
	connect(acceptPB, &QPushButton::clicked, &dialog, &QDialog::accept);
	connect(cancelPB, &QPushButton::clicked, &dialog, &QDialog::reject);
	
	QTextEdit* removedText=new QTextEdit();
	
	QString s=tr("The following %1 time constraints will be inactivated (their weight will be made 0%):",
	 "%1 is the count of time constraints which will have their weight made 0%").arg(toBeRemovedCombinedList.count());
	s+="\n\n";
	for(TimeConstraint* ctr : std::as_const(toBeRemovedCombinedList)){
		s+=ctr->getDetailedDescription(gt.rules);
		s+="\n";
		s+=tr("will be inactivated, by making its weight 0%", "It refers to a constraint");
		s+="\n\n";
		s+="---------------------------------";
		s+="\n\n";
	}
	
	removedText->setPlainText(s);
	removedText->setReadOnly(true);
	
	top->addWidget(removedText);
	
	top->addLayout(hl);
	
	const QString settingsName=QString("RemoveRedundantConstraintsLastConfirmationForm");
	
	dialog.resize(600, 400);
	centerWidgetOnScreen(&dialog);
	restoreFETDialogGeometry(&dialog, settingsName);
	
	acceptPB->setFocus();
	acceptPB->setDefault(true);
	
	setParentAndOtherThings(&dialog, this);
	int res=dialog.exec();
	saveFETDialogGeometry(&dialog, settingsName);
	
	if(res==QDialog::Rejected){
		toBeRemovedCombinedList.clear();
		
		return;
	}
	
	assert(res==QDialog::Accepted);
	
	if(toBeRemovedCombinedList.count()>0){
		for(TimeConstraint* mdc : std::as_const(toBeRemovedCombinedList))
			mdc->weightPercentage=0.0;

		int cnt=toBeRemovedCombinedList.count();
		toBeRemovedCombinedList.clear();

		gt.rules.addUndoPoint(tr("Removed the redundant constraints, by making the weight of %1 time constraints equal with 0%.",
		 "%1 is the count of time constraints which had their weight made 0%").arg(cnt));

		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);
	}
	
	this->accept();
}

void RemoveRedundantForm::wasCanceled()
{
	this->reject();
}

void RemoveRedundantForm::removeRedundantCheckBox_toggled()
{
	int k=removeRedundantCheckBox->isChecked();
	if(!k){
		removeRedundantCheckBox->setChecked(true);
		QMessageBox::information(this, tr("FET information"), tr("This box must remain checked, so that you can remove"
		 " redundant constraints of type min days between activities"));
	}
}

void RemoveRedundantForm::removeRedundantHalfCheckBox_toggled()
{
	int k=removeRedundantHalfCheckBox->isChecked();
	if(!k){
		removeRedundantHalfCheckBox->setChecked(true);
		QMessageBox::information(this, tr("FET information"), tr("This box must remain checked, so that you can remove"
		 " redundant constraints of type min half days between activities"));
	}
}
