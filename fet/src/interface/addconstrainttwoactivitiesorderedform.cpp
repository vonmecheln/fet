/***************************************************************************
                          addconstrainttwoactivitiesorderedform.cpp  -  description
                             -------------------
    begin                : 2 Apr 2008
    copyright            : (C) 2008 by Lalescu Liviu
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

#include "longtextmessagebox.h"

#include "addconstrainttwoactivitiesorderedform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

AddConstraintTwoActivitiesOrderedForm::AddConstraintTwoActivitiesOrderedForm()
{
    setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this /*AddConstraintTwoActivitiesOrderedForm_template*/, SLOT(constraintChanged()));
    connect(addConstraintPushButton, SIGNAL(clicked()), this /*AddConstraintTwoActivitiesOrderedForm_template*/, SLOT(addCurrentConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this /*AddConstraintTwoActivitiesOrderedForm_template*/, SLOT(close()));
//    connect(firstActivitiesComboBox, SIGNAL(activated(QString)), this /*AddConstraintTwoActivitiesOrderedForm_template*/, SLOT(constraintChanged()));
//    connect(secondActivitiesComboBox, SIGNAL(activated(QString)), this /*AddConstraintTwoActivitiesOrderedForm_template*/, SLOT(constraintChanged()));
    connect(teachersComboBox, SIGNAL(activated(QString)), this /*AddConstraintTwoActivitiesOrderedForm_template*/, SLOT(filterChanged()));
    connect(studentsComboBox, SIGNAL(activated(QString)), this /*AddConstraintTwoActivitiesOrderedForm_template*/, SLOT(filterChanged()));
    connect(subjectsComboBox, SIGNAL(activated(QString)), this /*AddConstraintTwoActivitiesOrderedForm_template*/, SLOT(filterChanged()));
    connect(activityTagsComboBox, SIGNAL(activated(QString)), this /*AddConstraintTwoActivitiesOrderedForm_template*/, SLOT(filterChanged()));


	//setWindowFlags(Qt::Window);
	//setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	/*QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	QSize tmp3=subjectsComboBox->minimumSizeHint();
	Q_UNUSED(tmp3);
	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	
	QSize tmp5=firstActivitiesComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	QSize tmp6=secondActivitiesComboBox->minimumSizeHint();
	Q_UNUSED(tmp6);
	
	firstActivitiesComboBox->setMaximumWidth(maxRecommendedWidth(this));
	secondActivitiesComboBox->setMaximumWidth(maxRecommendedWidth(this));
	
	teachersComboBox->insertItem("");
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		teachersComboBox->insertItem(tch->name);
	}
	teachersComboBox->setCurrentItem(0);

	subjectsComboBox->insertItem("");
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* sb=gt.rules.subjectsList[i];
		subjectsComboBox->insertItem(sb->name);
	}
	subjectsComboBox->setCurrentItem(0);

	activityTagsComboBox->insertItem("");
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* st=gt.rules.activityTagsList[i];
		activityTagsComboBox->insertItem(st->name);
	}
	activityTagsComboBox->setCurrentItem(0);

	studentsComboBox->insertItem("");
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* sty=gt.rules.yearsList[i];
		studentsComboBox->insertItem(sty->name);
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			studentsComboBox->insertItem(stg->name);
			for(int k=0; k<stg->subgroupsList.size(); k++){
				StudentsSubgroup* sts=stg->subgroupsList[k];
				studentsComboBox->insertItem(sts->name);
			}
		}
	}
	studentsComboBox->setCurrentItem(0);

	updateActivitiesComboBox();
}

AddConstraintTwoActivitiesOrderedForm::~AddConstraintTwoActivitiesOrderedForm()
{
}

bool AddConstraintTwoActivitiesOrderedForm::filterOk(Activity* act)
{
	QString tn=teachersComboBox->currentText();
	QString stn=studentsComboBox->currentText();
	QString sbn=subjectsComboBox->currentText();
	QString sbtn=activityTagsComboBox->currentText();
	int ok=true;

	//teacher
	if(tn!=""){
		bool ok2=false;
		for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++)
			if(*it == tn){
				ok2=true;
				break;
			}
		if(!ok2)
			ok=false;
	}

	//subject
	if(sbn!="" && sbn!=act->subjectName)
		ok=false;
		
	//activity tag
//	if(sbtn!="" && sbtn!=act->activityTagName)
	if(sbtn!="" && !act->activityTagsNames.contains(sbtn))
		ok=false;
		
	//students
	if(stn!=""){
		bool ok2=false;
		for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
			if(*it == stn){
				ok2=true;
				break;
			}
		if(!ok2)
			ok=false;
	}
	
	return ok;
}

void AddConstraintTwoActivitiesOrderedForm::updateActivitiesComboBox(){
	firstActivitiesComboBox->clear();
	firstActivitiesList.clear();

	secondActivitiesComboBox->clear();
	secondActivitiesList.clear();
	
	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		Activity* act=gt.rules.activitiesList[i];
		
		if(filterOk(act)){
			firstActivitiesComboBox->insertItem(act->getDescription(gt.rules));
			this->firstActivitiesList.append(act->id);

			secondActivitiesComboBox->insertItem(act->getDescription(gt.rules));
			this->secondActivitiesList.append(act->id);
		}
	}

	constraintChanged();
}

void AddConstraintTwoActivitiesOrderedForm::filterChanged()
{
	this->updateActivitiesComboBox();
}

void AddConstraintTwoActivitiesOrderedForm::constraintChanged()
{/*
	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=tr("Weight (percentage)=%1\%").arg(weight);
	s+="\n";

	s+=tr("two activities ordered");
	s+=" ";
	s+=tr("(activity 2 must be after activity 1, separated by any number of days or hours)");
	s+="\n";

	int tmp2=firstActivitiesComboBox->currentItem();
	assert(tmp2<firstActivitiesList.size());
	assert(tmp2<gt.rules.activitiesList.size());
	if(tmp2<0){
		s+=tr("Invalid first activity");
		s+="\n";
	}
	else{
		int fid=firstActivitiesList.at(tmp2);
		s+=tr("First activity id=%1").arg(fid);
		s+="\n";
	}

	int tmp3=secondActivitiesComboBox->currentItem();
	assert(tmp3<secondActivitiesList.size());
	assert(tmp3<gt.rules.activitiesList.size());
	if(tmp3<0){
		s+=tr("Invalid second activity");
		s+="\n";
	}
	else{
		int sid=secondActivitiesList.at(tmp3);
		s+=tr("Second activity id=%1").arg(sid);
		s+="\n";
	}

	currentConstraintTextEdit->setText(s);*/
}

void AddConstraintTwoActivitiesOrderedForm::addCurrentConstraint()
{
	TimeConstraint *ctr=NULL;

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage)"));
		return;
	}

	int fid;
	int tmp2=firstActivitiesComboBox->currentItem();
	assert(tmp2<gt.rules.activitiesList.size());
	assert(tmp2<firstActivitiesList.size());
	if(tmp2<0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid activity"));
		return;
	}
	else
		fid=firstActivitiesList.at(tmp2);
	
	int sid;
	int tmp3=secondActivitiesComboBox->currentItem();
	assert(tmp3<gt.rules.activitiesList.size());
	assert(tmp3<secondActivitiesList.size());
	if(tmp3<0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid activity"));
		return;
	}
	else
		sid=secondActivitiesList.at(tmp3);
		
	if(sid==fid){
		QMessageBox::warning(this, tr("FET information"),
			tr("Same activities - impossible"));
		return;
	}
	
	ctr=new ConstraintTwoActivitiesOrdered(weight, fid, sid);

	bool tmp4=gt.rules.addTimeConstraint(ctr);
	if(tmp4)
		LongTextMessageBox::information(this, tr("FET information"),
			tr("Constraint added:")+"\n\n"+ctr->getDetailedDescription(gt.rules));
	else{
		QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - error?"));
		delete ctr;
	}
}
