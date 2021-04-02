/***************************************************************************
                          modifyconstraintthreeactivitiesgroupedform.cpp  -  description
                             -------------------
    begin                : Aug 14, 2009
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

#include "modifyconstraintthreeactivitiesgroupedform.h"
#include "timeconstraint.h"

ModifyConstraintThreeActivitiesGroupedForm::ModifyConstraintThreeActivitiesGroupedForm(ConstraintThreeActivitiesGrouped* ctr)
{
    setupUi(this);

    connect(okPushButton, SIGNAL(clicked()), this /*ModifyConstraintThreeActivitiesGroupedForm_template*/, SLOT(ok()));
    connect(cancelPushButton, SIGNAL(clicked()), this /*ModifyConstraintThreeActivitiesGroupedForm_template*/, SLOT(cancel()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);

	QSize tmp5=firstActivitiesComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	QSize tmp6=secondActivitiesComboBox->minimumSizeHint();
	Q_UNUSED(tmp6);
	QSize tmp7=thirdActivitiesComboBox->minimumSizeHint();
	Q_UNUSED(tmp7);
	
	firstActivitiesComboBox->setMaximumWidth(maxRecommendedWidth(this));
	secondActivitiesComboBox->setMaximumWidth(maxRecommendedWidth(this));
	thirdActivitiesComboBox->setMaximumWidth(maxRecommendedWidth(this));
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(QString::number(ctr->weightPercentage));

	updateActivitiesComboBox();

	//constraintChanged();
}

ModifyConstraintThreeActivitiesGroupedForm::~ModifyConstraintThreeActivitiesGroupedForm()
{
}

bool ModifyConstraintThreeActivitiesGroupedForm::filterOk(Activity* act)
{
	Q_UNUSED(act);

	int ok=true;
	
	return ok;
}

void ModifyConstraintThreeActivitiesGroupedForm::filterChanged()
{
	this->updateActivitiesComboBox();
}

void ModifyConstraintThreeActivitiesGroupedForm::updateActivitiesComboBox(){
	firstActivitiesComboBox->clear();
	firstActivitiesList.clear();

	secondActivitiesComboBox->clear();
	secondActivitiesList.clear();

	thirdActivitiesComboBox->clear();
	thirdActivitiesList.clear();
	
	int i=0, j=-1;
	for(int k=0; k<gt.rules.activitiesList.size(); k++){
		Activity* act=gt.rules.activitiesList[k];
		if(filterOk(act)){
			firstActivitiesComboBox->insertItem(act->getDescription(gt.rules));
			this->firstActivitiesList.append(act->id);

			if(act->id==this->_ctr->firstActivityId)
				j=i;
				
			i++;
		}
	}
	//assert(j>=0); only first time
	firstActivitiesComboBox->setCurrentItem(j);

	i=0, j=-1;
	for(int k=0; k<gt.rules.activitiesList.size(); k++){
		Activity* act=gt.rules.activitiesList[k];
		if(filterOk(act)){
			secondActivitiesComboBox->insertItem(act->getDescription(gt.rules));
			this->secondActivitiesList.append(act->id);

			if(act->id==this->_ctr->secondActivityId)
				j=i;
				
			i++;
		}
	}
	//assert(j>=0); only first time
	secondActivitiesComboBox->setCurrentItem(j);

	i=0, j=-1;
	for(int k=0; k<gt.rules.activitiesList.size(); k++){
		Activity* act=gt.rules.activitiesList[k];
		if(filterOk(act)){
			thirdActivitiesComboBox->insertItem(act->getDescription(gt.rules));
			this->thirdActivitiesList.append(act->id);

			if(act->id==this->_ctr->thirdActivityId)
				j=i;
				
			i++;
		}
	}
	//assert(j>=0); only first time
	thirdActivitiesComboBox->setCurrentItem(j);

	//constraintChanged();
}

/*void ModifyConstraintThreeActivitiesGroupedForm::constraintChanged()
{
	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=tr("Weight (percentage)=%1\%").arg(weight);
	s+="\n";

	s+=tr("3 activities grouped");
	s+=" ";
	s+=tr("(activities must be in the same day, one following the other, in any order, possibly separated by breaks)");
	s+="\n";
	
	int fid;
	int tmp2=firstActivitiesComboBox->currentItem();
	assert(tmp2<gt.rules.activitiesList.size());
	assert(tmp2<firstActivitiesList.size());
	if(tmp2<0){
		s+=tr("Invalid activity");
		s+="\n";
	}
	else{
		fid=firstActivitiesList.at(tmp2);
		s+=tr("First activity id=%1").arg(fid);
		s+="\n";
	}

	int sid;
	int tmp3=secondActivitiesComboBox->currentItem();
	assert(tmp3<gt.rules.activitiesList.size());
	assert(tmp3<secondActivitiesList.size());
	if(tmp3<0){
		s+=tr("Invalid second activity");
		s+="\n";
	}
	else{
		sid=secondActivitiesList.at(tmp3);
		s+=tr("Second activity id=%1").arg(sid);
		s+="\n";
	}

	int tid;
	int tmp4=thirdActivitiesComboBox->currentItem();
	assert(tmp4<gt.rules.activitiesList.size());
	assert(tmp4<thirdActivitiesList.size());
	if(tmp4<0){
		s+=tr("Invalid third activity");
		s+="\n";
	}
	else{
		tid=thirdActivitiesList.at(tmp4);
		s+=tr("Third activity id=%1").arg(tid);
		s+="\n";
	}

	currentConstraintTextEdit->setText(s);
}*/

void ModifyConstraintThreeActivitiesGroupedForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage)"));
		return;
	}

	int tmp2=firstActivitiesComboBox->currentItem();
	assert(tmp2<gt.rules.activitiesList.size());
	assert(tmp2<firstActivitiesList.size());
	if(tmp2<0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid first activity"));
		return;
	}
	int fid=firstActivitiesList.at(tmp2);
	
	int tmp3=secondActivitiesComboBox->currentItem();
	assert(tmp3<gt.rules.activitiesList.size());
	assert(tmp3<secondActivitiesList.size());
	if(tmp3<0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid second activity"));
		return;
	}
	int sid=secondActivitiesList.at(tmp3);

	int tmp4=thirdActivitiesComboBox->currentItem();
	assert(tmp4<gt.rules.activitiesList.size());
	assert(tmp4<thirdActivitiesList.size());
	if(tmp4<0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid third activity"));
		return;
	}
	int tid=thirdActivitiesList.at(tmp4);

	if(sid==fid || sid==tid || fid==tid){
		QMessageBox::warning(this, tr("FET information"),
			tr("Same activities - impossible"));
		return;
	}
	
	this->_ctr->weightPercentage=weight;
	this->_ctr->firstActivityId=fid;
	this->_ctr->secondActivityId=sid;
	this->_ctr->thirdActivityId=tid;
	
	gt.rules.internalStructureComputed=false;

	this->close();
}

void ModifyConstraintThreeActivitiesGroupedForm::cancel()
{
	this->close();
}
