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

#include "removeredundantform.h"

#include "timetable.h"

extern Timetable gt;

#include <QHash>
#include <QList>
#include <QQueue>

#include <QMessageBox>

#include <QPushButton>
#include <QCheckBox>
#include <QTextEdit>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>

RemoveRedundantForm::RemoveRedundantForm()
{
	setupUi(this);

	centerWidgetOnScreen(this);
	
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(wasAccepted()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(wasCanceled()));
}

RemoveRedundantForm::~RemoveRedundantForm()
{

}

void RemoveRedundantForm::wasAccepted()
{
	QMultiHash<int, int> adjMatrix;

	foreach(TimeConstraint* tc, gt.rules.timeConstraintsList){
		if(tc->weightPercentage==100.0){
			if(tc->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME){
				ConstraintActivitiesSameStartingTime* cst=(ConstraintActivitiesSameStartingTime*) tc;
				
				for(int i=1; i<cst->n_activities; i++){
					adjMatrix.insert(cst->activitiesId[0], cst->activitiesId[i]);
					adjMatrix.insert(cst->activitiesId[i], cst->activitiesId[0]);
				}
			}
			else if(tc->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY){
				ConstraintActivitiesSameStartingDay* csd=(ConstraintActivitiesSameStartingDay*) tc;

				for(int i=1; i<csd->n_activities; i++){
					adjMatrix.insert(csd->activitiesId[0], csd->activitiesId[i]);
					adjMatrix.insert(csd->activitiesId[i], csd->activitiesId[0]);
				}
			}
		}
	}
	
	QHash<int, int> repr;
	
	QQueue<int> queue;

	foreach(Activity* act, gt.rules.activitiesList){
		int start=act->id;
		
		if(repr.value(start, -1)==-1){ //not visited
			repr.insert(start, start);
			queue.enqueue(start);
			while(!queue.isEmpty()){
				int crtHead=queue.dequeue();
				assert(repr.value(crtHead, -1)==start);
				QList<int> neighList=adjMatrix.values(crtHead);
				foreach(int neigh, neighList){
					if(repr.value(neigh, -1)==-1){
						queue.enqueue(neigh);
						repr.insert(neigh, start);
					}
					else{
						assert(repr.value(neigh, -1)==start);
					}
				}
			}
		}
	}
	
	QList<ConstraintMinNDaysBetweenActivities*> mdcList;
	
	foreach(TimeConstraint* tc, gt.rules.timeConstraintsList){
		if(tc->type==CONSTRAINT_MIN_N_DAYS_BETWEEN_ACTIVITIES){
			mdcList.prepend((ConstraintMinNDaysBetweenActivities*)tc); //inverse order, so earlier activities are not removed (the older ones are)
		}
	}

	QList<ConstraintMinNDaysBetweenActivities*> toBeRemovedList;
	
	for(int i=0; i<mdcList.count()-1; i++){
		ConstraintMinNDaysBetweenActivities* c1=mdcList.at(i);
		
		QList<int> a1List;
		for(int k=0; k<c1->n_activities; k++){
			int m=repr.value(c1->activitiesId[k], -1);
			assert(m>0);
			a1List.append(m);
		}
		
		qSort(a1List);
		
		for(int j=i+1; j<mdcList.count(); j++){
			ConstraintMinNDaysBetweenActivities* c2=mdcList.at(j);

			QList<int> a2List;
			for(int k=0; k<c2->n_activities; k++){
				int m=repr.value(c2->activitiesId[k], -1);
				assert(m>0);
				a2List.append(m);
			}
			
			qSort(a2List);
			
			bool equal=true;
			
			if(a1List.count()!=a2List.count())
				equal=false;
			if(a1List!=a2List)
				equal=false;
			if(c1->minDays!=c2->minDays)
				equal=false;
				
			//if(c1->weightPercentage!=c2->weightPercentage)
			//	equal=false;			
			//if(c1->consecutiveIfSameDay!=c2->consecutiveIfSameDay)
			//	equal=false;
			if(c1->weightPercentage > c2->weightPercentage){
				if(!c1->consecutiveIfSameDay && c2->consecutiveIfSameDay){
					equal=false;
				}
			}
			else if(c1->weightPercentage < c2->weightPercentage){
				if(c1->consecutiveIfSameDay && !c2->consecutiveIfSameDay){
					equal=false;
				}
			}
			else{
				//
			}
			
			if(equal){
				if(c1->weightPercentage > c2->weightPercentage){
					ConstraintMinNDaysBetweenActivities* tmp;
					tmp=mdcList[i];
					mdcList[i]=mdcList[j];
					mdcList[j]=tmp;

					c1=mdcList.at(i);
					c2=mdcList.at(j);
				}
				if(c1->weightPercentage==c2->weightPercentage && c1->consecutiveIfSameDay && !c2->consecutiveIfSameDay){
					ConstraintMinNDaysBetweenActivities* tmp;
					tmp=mdcList[i];
					mdcList[i]=mdcList[j];
					mdcList[j]=tmp;

					c1=mdcList.at(i);
					c2=mdcList.at(j);
				}
				
				assert( ! (c1->consecutiveIfSameDay && !c2->consecutiveIfSameDay) );
				assert(c1->weightPercentage <= c2->weightPercentage);
				
				int kk=0;
				for(kk=0; kk<toBeRemovedList.count(); kk++)
					if(toBeRemovedList.at(kk)->activitiesId[0] >= c1->activitiesId[0])
						break;
				toBeRemovedList.insert(kk, c1);
				
				//toBeRemovedList.prepend(c1);
				break;
			}
		}
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
	
	QString s=tr("The following time constraints will be inactivated (their weight will be made 0%):");
	s+="\n\n";
	foreach(ConstraintMinNDaysBetweenActivities* ctr, toBeRemovedList){
		if(ctr->weightPercentage>0.0){
			s+=ctr->getDetailedDescription(gt.rules);
			s+="\n";
			s+=tr("will be inactivated, by making its weight 0%");
			s+="\n\n";
			s+="---------------------------------";
			s+="\n\n";
		}
	}
	
	removedText->setText(s);
	
	removedText->setReadOnly(true);
	
	
	top->addWidget(removedText);
	
	top->addLayout(hl);
	
	
	dialog.resize(600, 400);
	centerWidgetOnScreen(&dialog);
	
	acceptPB->setFocus();
	acceptPB->setDefault(true);
	
	int res=dialog.exec();
	
	if(res==QDialog::Rejected){
		toBeRemovedList.clear();

		return;
	}

	assert(res==QDialog::Accepted);
	
/*	foreach(ConstraintMinNDaysBetweenActivities* mdc, toBeRemovedList){
		int t=gt.rules.timeConstraintsList.removeAll(mdc);
		assert(t==1);
	}
	gt.rules.internalStructureComputed=false;
	
	foreach(ConstraintMinNDaysBetweenActivities* mdc, toBeRemovedList)
		delete mdc;*/
	gt.rules.internalStructureComputed=false;
	
	foreach(ConstraintMinNDaysBetweenActivities* mdc, toBeRemovedList)
		mdc->weightPercentage=0.0;
		
	toBeRemovedList.clear();
		
	this->accept();
}

void RemoveRedundantForm::wasCanceled()
{
	this->reject();
}

void RemoveRedundantForm::on_removeRedundantCheckBox_toggled()
{
	int k=removeRedundantCheckBox->isChecked();
	if(!k){
		removeRedundantCheckBox->setChecked(true);
		QMessageBox::information(this, tr("FET information"), tr("This box must remain checked, so that you can remove"
		 " redundant constraints of type min n days between activities"));
	}
}
