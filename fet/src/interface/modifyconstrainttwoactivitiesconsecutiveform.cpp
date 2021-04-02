/***************************************************************************
                          modifyconstrainttwoactivitiesconsecutiveform.cpp  -  description
                             -------------------
    begin                : Aug 21, 2007
    copyright            : (C) 2007 by Lalescu Liviu
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

#include "modifyconstrainttwoactivitiesconsecutiveform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

ModifyConstraintTwoActivitiesConsecutiveForm::ModifyConstraintTwoActivitiesConsecutiveForm(ConstraintTwoActivitiesConsecutive* ctr)
{
    setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this /*ModifyConstraintTwoActivitiesConsecutiveForm_template*/, SLOT(constraintChanged()));
    connect(okPushButton, SIGNAL(clicked()), this /*ModifyConstraintTwoActivitiesConsecutiveForm_template*/, SLOT(ok()));
    connect(cancelPushButton, SIGNAL(clicked()), this /*ModifyConstraintTwoActivitiesConsecutiveForm_template*/, SLOT(cancel()));
//    connect(firstActivitiesComboBox, SIGNAL(activated(QString)), this /*ModifyConstraintTwoActivitiesConsecutiveForm_template*/, SLOT(constraintChanged()));
//    connect(secondActivitiesComboBox, SIGNAL(activated(QString)), this /*ModifyConstraintTwoActivitiesConsecutiveForm_template*/, SLOT(constraintChanged()));


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
	
	firstActivitiesComboBox->setMaximumWidth(maxRecommendedWidth(this));
	secondActivitiesComboBox->setMaximumWidth(maxRecommendedWidth(this));
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(QString::number(ctr->weightPercentage));

	updateActivitiesComboBox();

	constraintChanged();
}

ModifyConstraintTwoActivitiesConsecutiveForm::~ModifyConstraintTwoActivitiesConsecutiveForm()
{
}

bool ModifyConstraintTwoActivitiesConsecutiveForm::filterOk(Activity* act)
{
	Q_UNUSED(act);

	int ok=true;
	
	return ok;
}

void ModifyConstraintTwoActivitiesConsecutiveForm::filterChanged()
{
	this->updateActivitiesComboBox();
}

void ModifyConstraintTwoActivitiesConsecutiveForm::updateActivitiesComboBox(){
	firstActivitiesComboBox->clear();
	firstActivitiesList.clear();

	secondActivitiesComboBox->clear();
	secondActivitiesList.clear();
	
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

	constraintChanged();
}

void ModifyConstraintTwoActivitiesConsecutiveForm::constraintChanged()
{/*
	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=tr("Weight (percentage)=%1\%").arg(weight);
	s+="\n";

	s+=tr("two activities consecutive");
	s+=" ";
	s+=tr("(activity 2 must be immediately after activity 1, in the same day, possibly separated by breaks)");
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

	currentConstraintTextEdit->setText(s);*/
}

void ModifyConstraintTwoActivitiesConsecutiveForm::ok()
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

	if(sid==fid){
		QMessageBox::warning(this, tr("FET information"),
			tr("Same activities - impossible"));
		return;
	}
	
	this->_ctr->weightPercentage=weight;
	this->_ctr->firstActivityId=fid;
	this->_ctr->secondActivityId=sid;
	
	gt.rules.internalStructureComputed=false;

	this->close();
}

void ModifyConstraintTwoActivitiesConsecutiveForm::cancel()
{
	this->close();
}
