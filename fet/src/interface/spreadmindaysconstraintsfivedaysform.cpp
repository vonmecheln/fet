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

#include <QHash>
#include <QList>

#include <QMessageBox>

#include <QPushButton>
#include <QCheckBox>
#include <QTextEdit>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "matrix.h"

#include "spreadmindaysconstraintsfivedaysform.h"

#include "longtextmessagebox.h"

#include "timetable.h"

#include <algorithm>
//using namespace std;

extern Timetable gt;

SpreadMinDaysConstraintsFiveDaysForm::SpreadMinDaysConstraintsFiveDaysForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	okPushButton->setDefault(true);
	
	connect(okPushButton, &QPushButton::clicked, this, &SpreadMinDaysConstraintsFiveDaysForm::wasAccepted);
	connect(cancelPushButton, &QPushButton::clicked, this, &SpreadMinDaysConstraintsFiveDaysForm::wasCanceled);
	connect(helpPushButton, &QPushButton::clicked, this, &SpreadMinDaysConstraintsFiveDaysForm::help);
	
	spread2CheckBox->setChecked(false);
	spread3CheckBox->setChecked(false);
	spread4OrMoreCheckBox->setChecked(true);

	connect(spread2CheckBox, &QCheckBox::toggled, this, &SpreadMinDaysConstraintsFiveDaysForm::spread2CheckBox_toggled);
	connect(spread3CheckBox, &QCheckBox::toggled, this, &SpreadMinDaysConstraintsFiveDaysForm::spread3CheckBox_toggled);
	connect(spread4OrMoreCheckBox, &QCheckBox::toggled, this, &SpreadMinDaysConstraintsFiveDaysForm::spread4OrMoreCheckBox_toggled);
	
	spread2CheckBox_toggled();
	spread3CheckBox_toggled();
	spread4OrMoreCheckBox_toggled();
}

SpreadMinDaysConstraintsFiveDaysForm::~SpreadMinDaysConstraintsFiveDaysForm()
{
	saveFETDialogGeometry(this);
}

void SpreadMinDaysConstraintsFiveDaysForm::wasAccepted()
{
	double weight4;
	QString tmp=weight4LineEdit->text();
	weight_sscanf(tmp, "%lf", &weight4);
	if(weight4<0.0 || weight4>100.0){
		QMessageBox::warning(this, tr("FET information"),
		 tr("Invalid weight (percentage) for all split activities - must be real number >=0.0 and <=100.0"));
		return;
	}

	double weight2;
	tmp=weight2LineEdit->text();
	weight_sscanf(tmp, "%lf", &weight2);
	if(spread2CheckBox->isChecked() && (weight2<0.0 || weight2>100.0)){
		QMessageBox::warning(this, tr("FET information"),
		 tr("Invalid weight (percentage) for activities split into 2 components - must be real number >=0.0 and <=100.0"));
		return;
	}

	double weight3;
	tmp=weight3LineEdit->text();
	weight_sscanf(tmp, "%lf", &weight3);
	if(spread3CheckBox->isChecked() && (weight3<0.0 || weight3>100.0)){
		QMessageBox::warning(this, tr("FET information"),
		 tr("Invalid weight (percentage) for activities split into 3 components - must be real number >=0.0 and <=100.0"));
		return;
	}

	bool spread2=spread2CheckBox->isChecked();
	bool spread3=spread3CheckBox->isChecked();
	bool spread4OrMore=spread4OrMoreCheckBox->isChecked();
	
	if(!spread4OrMore){
		QMessageBox::critical(this, tr("FET bug"), tr("You found a probable bug in FET - min 1 day should be selected automatically for "
		 "all split activities. Please report error. FET will now abort current operation"));
		return;
	}
	assert(spread4OrMore);
	
	QHash<int, int> activitiesRepresentantIds; //first integer is the id, second is the index in the lists

	Matrix1D<QList<int>> activitiesForRepresentant;
	activitiesForRepresentant.resize(gt.rules.activitiesList.count());
	
	int nActs=0;
	
	for(Activity* act : std::as_const(gt.rules.activitiesList)){
		if(act->activityGroupId==0){
			assert(!activitiesRepresentantIds.contains(act->id));
			activitiesRepresentantIds.insert(act->id, nActs);
			activitiesForRepresentant[nActs].clear();
			activitiesForRepresentant[nActs].append(act->id);
			
			nActs++;
		}
		else{
			if(activitiesRepresentantIds.contains(act->activityGroupId)){
				int k=activitiesRepresentantIds.value(act->activityGroupId);
				assert(!activitiesForRepresentant[k].contains(act->id));
				activitiesForRepresentant[k].append(act->id);
			}
			else{
				activitiesRepresentantIds.insert(act->activityGroupId, nActs);
				activitiesForRepresentant[nActs].clear();
				activitiesForRepresentant[nActs].append(act->id);
				
				nActs++;
			}
		}
	}
	
	QHash<int, int> activityGroupIdHash;
	
	for(Activity* act : std::as_const(gt.rules.activitiesList))
		activityGroupIdHash.insert(act->id, act->activityGroupId);
	
	for(int i=0; i<nActs; i++){
		//qSort(activitiesForRepresentant[i]);
		std::stable_sort(activitiesForRepresentant[i].begin(), activitiesForRepresentant[i].end());
		int fid=activitiesForRepresentant[i].at(0);
		assert(activityGroupIdHash.contains(fid));
		int gid=activityGroupIdHash.value(fid);
		if(gid>0){
			assert(activitiesRepresentantIds.contains(gid));
			assert(activitiesRepresentantIds.value(gid)==i);
		}
		else
			assert(activitiesForRepresentant[i].count()==1);
	}
	
	QList<int> moreThanDaysPerWeek;
	for(int i=0; i<nActs; i++){
		QList<int> cl=activitiesForRepresentant[i];
		assert(cl.count()>=1);
		if((gt.rules.mode!=MORNINGS_AFTERNOONS && cl.count()>gt.rules.nDaysPerWeek) ||
		 (gt.rules.mode==MORNINGS_AFTERNOONS && cl.count()>gt.rules.nDaysPerWeek/2))
			moreThanDaysPerWeek.append(i);
	}
	if(moreThanDaysPerWeek.count()>=1){
		QString s;
		if(gt.rules.mode!=MORNINGS_AFTERNOONS){
			s=tr("Warning: there are activities which are divided into more subactivities than the number of days per week.");
		}
		else{
			s=tr("Warning: there are activities which are divided into more subactivities than the number of real days per week.");
		}
		s+=" ";
		s+=tr("These activities are listed below.");
		s+=" ";
		s+=tr("It is not recommended to add a constraint min 1 days between activities for such activities.");
		s+=" ";
		s+=tr("A workaround is to divide them again (remove and add them again), with less divisions, and come back to this dialog.");
		s+=" ";
		if(gt.rules.mode!=MORNINGS_AFTERNOONS){
			s+=tr("If you want them consecutive if on the same day, reduce the number of subactivities from the larger split activity and increase the duration"
			 " of some subactivities (you will have number_of_days_per_week subactivities in a single larger split activity)."
			 " If not, add two or more larger split activities.");
		}
		else{
			s+=tr("If you want them consecutive if on the same day, reduce the number of subactivities from the larger split activity and increase the duration"
			 " of some subactivities (you will have number_of_real_days_per_week subactivities in a single larger split activity)."
			 " If not, add two or more larger split activities.");
			s+=" ";
			s+=tr("(Alternatively, if the number of subactivities is at most equal to the number_of_half_days_per_week and you want to add a constraint"
			 " min 1 half days between activities for them then, after spreading the activities with this dialog, modify the constraints accordingly."
			 " There will be no constraints min half days between activities removed or added after using this dialog.)");
		}
		s+="\n\n";
		for(int i=0; i<nActs; i++){
			QList<int> cl=activitiesForRepresentant[i];
			assert(cl.count()>=1);
			if((gt.rules.mode!=MORNINGS_AFTERNOONS && cl.count()>gt.rules.nDaysPerWeek) ||
			 (gt.rules.mode==MORNINGS_AFTERNOONS && cl.count()>gt.rules.nDaysPerWeek/2)){
				QStringList lst;
				for(int ai : std::as_const(cl))
					lst.append(QString::number(ai));
				s+=tr("Number of activities: %1, activities ids: %2.").arg(cl.count()).arg(lst.join(translatedCommaSpace()));
				if(i<nActs-1)
					s+="\n";
			}
		}
		
		int res=LongTextMessageBox::largeConfirmation(this, tr("FET warning"), s, tr("Continue"), tr("Cancel"), QString(), 0, 1);
		if(res==1)
			return;
	}
	
	QList<ConstraintMinDaysBetweenActivities*> constraintsToBeRemoved;
	
	for(TimeConstraint* tc : std::as_const(gt.rules.timeConstraintsList)){
		if(tc->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES){
			ConstraintMinDaysBetweenActivities* mdc=(ConstraintMinDaysBetweenActivities*)tc;
			
			//find representative
			int reprIndex=-1;
			
			bool toBeRemoved=true;
			
			for(int i=0; i<mdc->n_activities; i++){
				if(!activityGroupIdHash.contains(mdc->activitiesIds[i])){
					QMessageBox::critical(this, tr("FET bug"), tr("You found a probable bug in FET - constraint %1\ncontains invalid activity id %2\n"
					 "\nPlease report error. FET will now abort current operation").arg(mdc->getDetailedDescription(gt.rules)).arg(mdc->activitiesIds[i]));
					return;
				}
				assert(activityGroupIdHash.contains(mdc->activitiesIds[i]));
				if(reprIndex==-1)
					reprIndex=activityGroupIdHash.value(mdc->activitiesIds[i]);
				else if(reprIndex!=activityGroupIdHash.value(mdc->activitiesIds[i])){
					toBeRemoved=false;
					break;
				}
			}
			
			if(reprIndex==0)
				toBeRemoved=false;
			
			if(toBeRemoved)
				constraintsToBeRemoved.append(mdc);
		}
	}
	
	bool consecutiveIfSameDay=consecutiveIfSameDayCheckBox->isChecked();
	
	QList<ConstraintMinDaysBetweenActivities*> addedConstraints;
	
	for(int i=0; i<nActs; i++){
		ConstraintMinDaysBetweenActivities* c1;
		ConstraintMinDaysBetweenActivities* c2;
		ConstraintMinDaysBetweenActivities* c3;
		c1=nullptr;
		c2=nullptr;
		c3=nullptr;
		
		QList<int> cl=activitiesForRepresentant[i];
		assert(cl.count()>=1);
		
		if(cl.count()>=2){
			assert(spread4OrMore);

			int n_acts;
			QList<int> acts;
			//int acts[MAX_CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES];
			
			n_acts=cl.count();
			acts.clear();
			for(int k=0; k<cl.count(); k++){
				//acts[k]=cl.at(k);
				acts.append(cl.at(k));
			}
			c1=new ConstraintMinDaysBetweenActivities(weight4, consecutiveIfSameDay, n_acts, acts, 1);
		}
		if(cl.count()==3 && spread3){
			int aloneComponent=-1, notAloneComp1=-1, notAloneComp2=-1;
			if(type123RadioButton->isChecked()){
				aloneComponent=1;
				notAloneComp1=2;
				notAloneComp2=3;
			}
			else if(type213RadioButton->isChecked()){
				aloneComponent=2;
				notAloneComp1=1;
				notAloneComp2=3;
			}
			else if(type312RadioButton->isChecked()){
				aloneComponent=3;
				notAloneComp1=1;
				notAloneComp2=2;
			}
			else{
				QMessageBox::information(this, tr("FET information"), tr("Please select the isolated component"));
				assert(c1!=nullptr);
				delete c1;
				return;
			}
			
			aloneComponent--;
			notAloneComp1--;
			notAloneComp2--;
		
			int n_acts;
			//int acts[10];
			QList<int> acts;
			
			n_acts=2;
			acts.clear();
			//acts[0]=cl.at(aloneComponent);
			acts.append(cl.at(aloneComponent));
			//acts[1]=cl.at(notAloneComp1);
			acts.append(cl.at(notAloneComp1));
			
			c2=new ConstraintMinDaysBetweenActivities(weight3, consecutiveIfSameDay, n_acts, acts, 2);

			//////////

			n_acts=2;
			acts.clear();
			//acts[0]=cl.at(aloneComponent);
			acts.append(cl.at(aloneComponent));
			//acts[1]=cl.at(notAloneComp2);
			acts.append(cl.at(notAloneComp2));
			
			c3=new ConstraintMinDaysBetweenActivities(weight3, consecutiveIfSameDay, n_acts, acts, 2);
		}
		if(cl.count()==2 && spread2){
			int n_acts;
			
			QList<int> acts;
			//int acts[10];
			
			n_acts=2;
			acts.clear();
			//acts[0]=cl.at(0);
			acts.append(cl.at(0));
			//acts[1]=cl.at(1);
			acts.append(cl.at(1));
			
			assert(c2==nullptr);
			c2=new ConstraintMinDaysBetweenActivities(weight2, consecutiveIfSameDay, n_acts, acts, 2);
		}
	
		if(c1!=nullptr)
			addedConstraints.append(c1);
		if(c2!=nullptr)
			addedConstraints.append(c2);
		if(c3!=nullptr)
			addedConstraints.append(c3);
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
	QTextEdit* addedText=new QTextEdit();
	
	QString s=tr("The following time constraints will be removed:");
	s+="\n\n";
	for(ConstraintMinDaysBetweenActivities* ctr : std::as_const(constraintsToBeRemoved)){
		s+=ctr->getDetailedDescription(gt.rules);
		s+="\n";
	}
	
	removedText->setPlainText(s);
	removedText->setReadOnly(true);
	
	s=tr("The following time constraints will be added:");
	s+="\n\n";
	for(ConstraintMinDaysBetweenActivities* ctr : std::as_const(addedConstraints)){
		s+=ctr->getDetailedDescription(gt.rules);
		s+="\n";
	}
	
	addedText->setPlainText(s);
	addedText->setReadOnly(true);
	
	top->addWidget(removedText);
	top->addWidget(addedText);
	
	top->addLayout(hl);
	
	//dialog.addLayout(top);
	
	const QString settingsName=QString("SpreadMinDaysBetweenActivitiesConstraintsLastConfirmationForm");

	dialog.resize(600, 500);
	centerWidgetOnScreen(&dialog);
	restoreFETDialogGeometry(&dialog, settingsName);
	
	acceptPB->setFocus();
	acceptPB->setDefault(true);
	
	setParentAndOtherThings(&dialog, this);
	int res=dialog.exec();
	saveFETDialogGeometry(&dialog, settingsName);
	
	if(res==QDialog::Rejected){
		constraintsToBeRemoved.clear();

		for(ConstraintMinDaysBetweenActivities* ctr : std::as_const(addedConstraints)){
			delete ctr;
		}
		addedConstraints.clear();
		
		return;
	}

	assert(res==QDialog::Accepted);
	
	bool dataChanged=false;
	
	//better
	QList<TimeConstraint*> removedList;
	for(ConstraintMinDaysBetweenActivities* mdc : std::as_const(constraintsToBeRemoved))
		removedList.append((TimeConstraint*)mdc);
	if(removedList.count()>0)
		dataChanged=true;
	bool t=gt.rules.removeTimeConstraints(removedList);
	assert(t);
	removedList.clear();
	constraintsToBeRemoved.clear();
	
	for(ConstraintMinDaysBetweenActivities* tc : std::as_const(addedConstraints)){
		bool t=gt.rules.addTimeConstraint(tc);
		if(!t){
			QMessageBox::critical(this, tr("FET bug"), tr("You found a probable bug in FET - trying to add constraint %1, "
			 "but it is already existing. Please report error. FET will now continue operation").arg(tc->getDetailedDescription(gt.rules)));
		}
		else{
			if(!dataChanged){
				dataChanged=true;
			}
		}
	}
	
	addedConstraints.clear();
	
	QString s2=tr("Spreading of activities operation completed successfully");
	s2+="\n\n";
	s2+=tr("NOTE: If you are using constraints of type activities same starting time or activities same starting day or max 0 days between "
	 "activities or max 0 half days between activities (in the Mornings-Afternoons mode), it is important (after the current operation) "
	 "to apply the operation of removing constraints which are 'bad' (redundant and with weight < 100%).")
	 +" "+tr("Read Help/Important tips - tip 2) for details.");
	QMessageBox::information(this, tr("FET information"), s2);
	
	if(dataChanged){
		QString su=tr("Spreaded the activities evenly over the week:");
		su+=QString("\n");
		
		su+=tr("Consecutive if on the same day=%1.").arg(consecutiveIfSameDayCheckBox->isChecked()?tr("yes"):tr("no"));
		su+=QString("\n");
		
		assert(spread4OrMoreCheckBox->isChecked());
		su+=tr("All split activities should be at least 1 day apart with weight=%1%.").arg(CustomFETString::number(weight4)/*LineEdit->text()*/);
		su+=QString("\n");

		if(spread2CheckBox->isChecked()){
			su+=tr("Activities split into 2 components should be at least 2 days apart with weight=%1%.").arg(CustomFETString::number(weight2)/*LineEdit->text()*/);
			su+=QString("\n");
		}
		
		if(spread3CheckBox->isChecked()){
			su+=tr("Activities split into 3 components should not be on 3 consecutive days with weight=%1%.").arg(CustomFETString::number(weight3)/*LineEdit->text()*/);
			su+=QString(" ");
			if(type123RadioButton->isChecked()){
				su+=tr("The isolated component is number 1.");
				su+=QString("\n");
			}
			else if(type213RadioButton->isChecked()){
				su+=tr("The isolated component is number 2.");
				su+=QString("\n");
			}
			else if(type312RadioButton->isChecked()){
				su+=tr("The isolated component is number 3.");
				su+=QString("\n");
			}
			else{
				assert(0);
			}
		}
		
		gt.rules.addUndoPoint(su);
	}
	
	this->accept();
}

void SpreadMinDaysConstraintsFiveDaysForm::wasCanceled()
{
	this->reject();
}

void SpreadMinDaysConstraintsFiveDaysForm::help()
{
	QString s;
	
	s+=tr("Help on spreading the activities over the week:");
	
	s+="\n\n";
	
	s+=tr("How to choose the weights in this dialog:");
	
	s+="\n\n";

	s+=tr("Weights (percentages) of newly added constraints min days between activities - recommended between 95.0%-100.0% "
	"(maybe lower on those split into 3). Make weights 100.0% if the constraints need to be respected all the time."
	" It is recommended to enable the check boxes for activities split into 2 or 3 components (not to be in consecutive days), "
	"if your data is still possible to solve. You may use a progressive approach. Example of weights: 90.0%, 95.0%, 99.0%, 99.75%, 100.0%.");

	LongTextMessageBox::largeInformation(this, tr("FET help"), s);
}

void SpreadMinDaysConstraintsFiveDaysForm::spread2CheckBox_toggled()
{
	weight2LineEdit->setEnabled(spread2CheckBox->isChecked());
	weight2Label->setEnabled(spread2CheckBox->isChecked());
}

void SpreadMinDaysConstraintsFiveDaysForm::spread3CheckBox_toggled()
{
	weight3LineEdit->setEnabled(spread3CheckBox->isChecked());
	weight3Label->setEnabled(spread3CheckBox->isChecked());
	aloneGroupBox->setEnabled(spread3CheckBox->isChecked());
}

void SpreadMinDaysConstraintsFiveDaysForm::spread4OrMoreCheckBox_toggled()
{
	int k=spread4OrMoreCheckBox->isChecked();
	if(!k){
		spread4OrMoreCheckBox->setChecked(true);
		QMessageBox::information(this, tr("FET information"), tr("This box must remain checked, so that split activities"
		 " are not on the same day (with the probability you write below)"));
	}
}
