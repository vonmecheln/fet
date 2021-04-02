//
//
// C++ Implementation: $MODULE$
//
// Description:
//
//
// Author: Lalescu Liviu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "spreadminndaysconstraints5daysform.h"

#include "timetable.h"

extern Timetable gt;

#include <QHash>
#include <QList>

#include <QMessageBox>

#include <QPushButton>
#include <QCheckBox>
#include <QTextEdit>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>

SpreadMinNDaysConstraints5DaysForm::SpreadMinNDaysConstraints5DaysForm()
{
	setupUi(this);

	centerWidgetOnScreen(this);
	
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(wasAccepted()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(wasCanceled()));
	
	spread2CheckBox->setChecked(false);
	spread3CheckBox->setChecked(false);
}

SpreadMinNDaysConstraints5DaysForm::~SpreadMinNDaysConstraints5DaysForm()
{

}

void SpreadMinNDaysConstraints5DaysForm::wasAccepted()
{
	double weight4;
	QString tmp=weight4LineEdit->text();
	sscanf(tmp, "%lf", &weight4);
	if(weight4<0.0 || weight4>100.0){
		QMessageBox::warning(this, QObject::tr("FET information"),
		 QObject::tr("Invalid weight (percentage) for all split activities - must be real number >=0.0 and <=100.0"));
		return;
	}

	double weight2;
	tmp=weight2LineEdit->text();
	sscanf(tmp, "%lf", &weight2);
	if(spread2CheckBox->isChecked() && (weight2<0.0 || weight2>100.0)){
		QMessageBox::warning(this, QObject::tr("FET information"),
		 QObject::tr("Invalid weight (percentage) for activities split into 2 components - must be real number >=0.0 and <=100.0"));
		return;
	}

	double weight3;
	tmp=weight3LineEdit->text();
	sscanf(tmp, "%lf", &weight3);
	if(spread3CheckBox->isChecked() && (weight3<0.0 || weight3>100.0)){
		QMessageBox::warning(this, QObject::tr("FET information"),
		 QObject::tr("Invalid weight (percentage) for activities split into 3 components - must be real number >=0.0 and <=100.0"));
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
	QList<int> activitiesForRepresentant[MAX_ACTIVITIES];
	
	int nActs=0;
	
	foreach(Activity* act, gt.rules.activitiesList){
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
	
	foreach(Activity* act, gt.rules.activitiesList)
		activityGroupIdHash.insert(act->id, act->activityGroupId);
	
	for(int i=0; i<nActs; i++){
		qSort(activitiesForRepresentant[i]);
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
	
	QList<ConstraintMinNDaysBetweenActivities*> constraintsToBeRemoved;
	
	foreach(TimeConstraint* tc, gt.rules.timeConstraintsList){
		if(tc->type==CONSTRAINT_MIN_N_DAYS_BETWEEN_ACTIVITIES){
			ConstraintMinNDaysBetweenActivities* mdc=(ConstraintMinNDaysBetweenActivities*) tc;
			
			//find representant
			int reprIndex=-1;
			
			bool toBeRemoved=true;
			
			for(int i=0; i<mdc->n_activities; i++){
				if(!activityGroupIdHash.contains(mdc->activitiesId[i])){
					QMessageBox::critical(this, tr("FET bug"), tr("You found a probable bug in FET - constraint %1\ncontains invalid activity id %2\n"
					 "\nPlease report error. FET will now abort current operation").arg(mdc->getDetailedDescription(gt.rules)).arg(mdc->activitiesId[i]));
					return;
				}
				assert(activityGroupIdHash.contains(mdc->activitiesId[i]));
				if(reprIndex==-1)
					reprIndex=activityGroupIdHash.value(mdc->activitiesId[i]);
				else if(reprIndex!=activityGroupIdHash.value(mdc->activitiesId[i])){
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
	
	QList<ConstraintMinNDaysBetweenActivities*> addedConstraints;
	
	for(int i=0; i<nActs; i++){
		ConstraintMinNDaysBetweenActivities* c1;
		ConstraintMinNDaysBetweenActivities* c2;
		ConstraintMinNDaysBetweenActivities* c3;
		c1=NULL;
		c2=NULL;
		c3=NULL;
		
		QList<int> cl=activitiesForRepresentant[i];
		assert(cl.count()>=1);
		
		if(cl.count()>=2){
			assert(spread4OrMore);

			int n_acts=cl.count();
			int acts[MAX_CONSTRAINT_MIN_N_DAYS_BETWEEN_ACTIVITIES];
			
			for(int k=0; k<cl.count(); k++){
				acts[k]=cl.at(k);
			}
			c1=new ConstraintMinNDaysBetweenActivities(weight4, consecutiveIfSameDay, n_acts, acts, 1);
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
				return;
			}
				
			aloneComponent--;
			notAloneComp1--;
			notAloneComp2--;
		
			int n_acts;
			int acts[10];
			
			n_acts=2;
			
			acts[0]=cl.at(aloneComponent);
			acts[1]=cl.at(notAloneComp1);
				
			c2=new ConstraintMinNDaysBetweenActivities(weight3, consecutiveIfSameDay, n_acts, acts, 2);

			//////////

			n_acts=2;
			
			acts[0]=cl.at(aloneComponent);
			acts[1]=cl.at(notAloneComp2);
				
			c3=new ConstraintMinNDaysBetweenActivities(weight3, consecutiveIfSameDay, n_acts, acts, 2);
		}
		if(cl.count()==2 && spread2){
			int n_acts=2;
			
			int acts[10];
			
			acts[0]=cl.at(0);
			acts[1]=cl.at(1);
			
			c2=new ConstraintMinNDaysBetweenActivities(weight2, consecutiveIfSameDay, n_acts, acts, 2);
		}
	
		if(c1!=NULL)
			addedConstraints.append(c1);
		if(c2!=NULL)
			addedConstraints.append(c2);
		if(c3!=NULL)
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
	
	QObject::connect(acceptPB, SIGNAL(clicked()), &dialog, SLOT(accept()));
	QObject::connect(cancelPB, SIGNAL(clicked()), &dialog, SLOT(reject()));
	
	QTextEdit* removedText=new QTextEdit();
	QTextEdit* addedText=new QTextEdit();
	
	QString s=tr("The following time constraints will be removed:");
	s+="\n\n";
	foreach(ConstraintMinNDaysBetweenActivities* ctr, constraintsToBeRemoved){
		s+=ctr->getDetailedDescription(gt.rules);
		s+="\n";
	}
	
	removedText->setText(s);
	
	removedText->setReadOnly(true);
	
	s=tr("The following time constraints will be added:");
	s+="\n\n";
	foreach(ConstraintMinNDaysBetweenActivities* ctr, addedConstraints){
		s+=ctr->getDetailedDescription(gt.rules);
		s+="\n";
	}
	
	addedText->setText(s);
	
	addedText->setReadOnly(true);
	
	top->addWidget(removedText);
	top->addWidget(addedText);
	
	top->addLayout(hl);
	
	//dialog.addLayout(top);

	dialog.resize(600, 500);
	centerWidgetOnScreen(&dialog);
	
	acceptPB->setFocus();
	acceptPB->setDefault(true);
	
	int res=dialog.exec();
	
	if(res==QDialog::Rejected){
		constraintsToBeRemoved.clear();

		foreach(ConstraintMinNDaysBetweenActivities* ctr, addedConstraints){
			delete ctr;
		}
		addedConstraints.clear();
		
		return;
	}

	assert(res==QDialog::Accepted);
	
	foreach(ConstraintMinNDaysBetweenActivities* mdc, constraintsToBeRemoved){
		int t=gt.rules.timeConstraintsList.removeAll(mdc);
		assert(t==1);
	}
	gt.rules.internalStructureComputed=false;
	
	foreach(ConstraintMinNDaysBetweenActivities* mdc, constraintsToBeRemoved)
		delete mdc;
		
	constraintsToBeRemoved.clear();
		
	foreach(ConstraintMinNDaysBetweenActivities* tc, addedConstraints){
		bool t=gt.rules.addTimeConstraint(tc);
		if(!t){
			QMessageBox::critical(this, tr("FET bug"), tr("You found a probable bug in FET - trying to add constraint %1, "
			 "but it is already existing. Please report error. FET will now continue operation").arg(tc->getDetailedDescription(gt.rules)));
		}
	}
		
	addedConstraints.clear();
	
	QString s2=tr("Spreading of activities operation completed successfully");
	s2+="\n\n";
	s2+=tr("NOTE: If you are using constraints of type activities same starting time or activities same starting day, it is important"
	 " (after current operation) to apply the operation of removing redundant constraints.")
	 +" "+tr("Read Help/Important tips - tip 2) for details.");
	QMessageBox::information(this, tr("FET information"), s2);
	
	this->accept();
}

void SpreadMinNDaysConstraints5DaysForm::wasCanceled()
{
	this->reject();
}

void SpreadMinNDaysConstraints5DaysForm::on_spread2CheckBox_toggled()
{
	weight2LineEdit->setEnabled(spread2CheckBox->isChecked());
	weight2Label->setEnabled(spread2CheckBox->isChecked());
}

void SpreadMinNDaysConstraints5DaysForm::on_spread3CheckBox_toggled()
{
	weight3LineEdit->setEnabled(spread3CheckBox->isChecked());
	weight3Label->setEnabled(spread3CheckBox->isChecked());
	aloneGroupBox->setEnabled(spread3CheckBox->isChecked());
}

void SpreadMinNDaysConstraints5DaysForm::on_spread4OrMoreCheckBox_toggled()
{
	int k=spread4OrMoreCheckBox->isChecked();
	if(!k){
		spread4OrMoreCheckBox->setChecked(true);
		QMessageBox::information(this, tr("FET information"), tr("This box must remain checked, so that split activities"
		 " are not in the same day (with the probability you write below)"));
	}
}
