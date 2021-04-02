/***************************************************************************
                          activitiesform.cpp  -  description
                             -------------------
    begin                : Wed Apr 23 2003
    copyright            : (C) 2003 by Lalescu Liviu
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

//#include <iostream>
//using namespace std;

#include "timetable_defs.h"
#include "fet.h"
#include "timetable.h"

#include "studentsset.h"

#include "activitiesform.h"
#include "addactivityform.h"
#include "modifyactivityform.h"

#include <qstring.h>
//#include <q3listbox.h>
#include <qmessagebox.h>
//#include <q3textedit.h>

#include <QTextEdit>
#include <QListWidget>

#include <QScrollBar>

#include <QAbstractItemView>

#include <QDesktopWidget>

#include "longtextmessagebox.h"

#include <QBrush>
#include <QPalette>
//#include <QApplication>

ActivitiesForm::ActivitiesForm()
{
    setupUi(this);
    
    modifyActivityPushButton->setDefault(true);
    
    activitiesListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(activitiesListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(activityChanged()));
    connect(addActivityPushButton, SIGNAL(clicked()), this, SLOT(addActivity()));
    connect(removeActivityPushButton, SIGNAL(clicked()), this, SLOT(removeActivity()));
    connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(teachersComboBox, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
    connect(studentsComboBox, SIGNAL(activated(QString)), this, SLOT(studentsFilterChanged()));
    connect(subjectsComboBox, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
    connect(modifyActivityPushButton, SIGNAL(clicked()), this, SLOT(modifyActivity()));
    connect(activityTagsComboBox, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
    connect(activitiesListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(modifyActivity()));
    connect(recursiveCheckBox, SIGNAL(toggled(bool)), this, SLOT(studentsFilterChanged()));

    connect(helpPushButton, SIGNAL(clicked()), this, SLOT(help()));

	//setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	centerWidgetOnScreen(this);
	/*QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	
	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	QSize tmp3=subjectsComboBox->minimumSizeHint();
	Q_UNUSED(tmp3);
	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);

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
	
	showedStudents.clear();
	showedStudents.insert("");
	//this->studentsFilterChanged();
	
	this->filterChanged();
}

ActivitiesForm::~ActivitiesForm()
{
}

bool ActivitiesForm::filterOk(Activity* act)
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
	//if(sbtn!="" && sbtn!=act->activityTagName)
	if(sbtn!="" && !act->activityTagsNames.contains(sbtn))
		ok=false;
		
	//students
	if(stn!=""){
		bool ok2=false;
		for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
			//if(*it == stn){
			if(showedStudents.contains(*it)){
				ok2=true;
				break;
			}
		if(!ok2)
			ok=false;
	}
	else{
		assert(showedStudents.count()==1);
		assert(showedStudents.contains(""));
	}
	
	return ok;
}

void ActivitiesForm::studentsFilterChanged()
{
	bool showContained=recursiveCheckBox->isChecked();
	
	showedStudents.clear();
	
	if(!showContained){
		showedStudents.insert(studentsComboBox->currentText());
	}
	else{
		if(studentsComboBox->currentText()=="")
			showedStudents.insert("");
		else{
			//down
			StudentsSet* set=gt.rules.searchStudentsSet(studentsComboBox->currentText());
			assert(set!=NULL);
			if(set->type==STUDENTS_YEAR){
				StudentsYear* year=(StudentsYear*)set;
				showedStudents.insert(year->name);
				foreach(StudentsGroup* group, year->groupsList){
					showedStudents.insert(group->name);
					foreach(StudentsSubgroup* subgroup, group->subgroupsList)
						showedStudents.insert(subgroup->name);
				}
			}
			else if(set->type==STUDENTS_GROUP){
				StudentsGroup* group=(StudentsGroup*) set;
				showedStudents.insert(group->name);
				foreach(StudentsSubgroup* subgroup, group->subgroupsList)
					showedStudents.insert(subgroup->name);
			}
			else if(set->type==STUDENTS_SUBGROUP){
				StudentsSubgroup* subgroup=(StudentsSubgroup*) set;
				showedStudents.insert(subgroup->name);
			}
			else
				assert(0);
				
			//up
			QString crt=studentsComboBox->currentText();
			foreach(StudentsYear* year, gt.rules.yearsList){
				foreach(StudentsGroup* group, year->groupsList){
					if(group->name==crt){
						showedStudents.insert(year->name);
					}
					foreach(StudentsSubgroup* subgroup, group->subgroupsList){
						if(subgroup->name==crt){
							showedStudents.insert(year->name);
							showedStudents.insert(group->name);
						}
					}
				}
			}
		}
	}
	
	filterChanged();
}

void ActivitiesForm::filterChanged()
{
	int nacts=0, nsubacts=0, nh=0;
	int ninact=0, ninacth=0;

	QString s;
	activitiesListWidget->clear();
	visibleActivitiesList.clear();
	
	int k=0;
	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		Activity* act=gt.rules.activitiesList[i];
		if(this->filterOk(act)){
			s=act->getDescription(gt.rules);
			visibleActivitiesList.append(act);
			activitiesListWidget->addItem(s);
			k++;
			
			if(USE_GUI_COLORS && !act->active)
				activitiesListWidget->item(k-1)->setBackground(activitiesListWidget->palette().alternateBase());
			
			if(act->id==act->activityGroupId || act->activityGroupId==0)
				nacts++;
			nsubacts++;
			
			nh+=act->duration;
			
			if(!act->active){
				ninact++;
				ninacth+=act->duration;
			}
		}
	}
	
	assert(nsubacts-ninact>=0);
	assert(nh-ninacth>=0);
	activeTextLabel->setText(tr("No: %1 / %2", "No means number, %1 is the number of active activities, %2 is the number of total activities."
		" Please leave spaces between fields, so that they are better visible").arg(nsubacts-ninact).arg(nsubacts));
	totalTextLabel->setText(tr("Dur: %1 / %2", "Dur means duration, %1 is the duration of active activities, %2 is the duration of total activities."
		" Please leave spaces between fields, so that they are better visible").arg(nh-ninacth).arg(nh));
	
	activitiesListWidget->setCurrentRow(0);
	//activitiesListWidget->setSelected(0, true);
	
	if(activitiesListWidget->count()<=0)
		activityTextEdit->setText("");
}

void ActivitiesForm::addActivity()
{
	int nInitialActs=gt.rules.activitiesList.count();

	AddActivityForm addActivityForm;
	addActivityForm.exec();

	if(gt.rules.activitiesList.count()!=nInitialActs){
		assert(gt.rules.activitiesList.count()>nInitialActs);

		//rebuild the activities list box
		filterChanged();
	
		int ind=activitiesListWidget->count()-1;
		if(ind>=0)
			activitiesListWidget->setCurrentRow(ind);
	}
}

void ActivitiesForm::modifyActivity()
{
	int ind=activitiesListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected activity"));
		return;
	}
	
	assert(ind<visibleActivitiesList.count());
	
	int valv=activitiesListWidget->verticalScrollBar()->value();
	int valh=activitiesListWidget->horizontalScrollBar()->value();

	Activity* act=visibleActivitiesList[ind];
	assert(act!=NULL);
	
	QStringList teachers=act->teachersNames;
	bool diffTeachers=false;
	
	QString subject=act->subjectName;
	bool diffSubject=false;
	
	QStringList activityTags=act->activityTagsNames;
	bool diffActivityTags=false;
	
	QStringList students=act->studentsNames;
	bool diffStudents=false;
	
	int nTotalStudents=act->nTotalStudents;
	bool diffNTotalStudents=false;
	
	bool computeNTotalStudents=act->computeNTotalStudents;
	bool diffComputeNTotalStudents=false;
	
	if(act->isSplit()){
		int nSplit=0;
		for(int i=0; i<gt.rules.activitiesList.size(); i++){
			Activity* act2=gt.rules.activitiesList[i];
			if(act2->activityGroupId==act->activityGroupId){
				nSplit++;
				
				if(teachers!=act2->teachersNames)
					diffTeachers=true;
				if(subject!=act2->subjectName)
					diffSubject=true;
				if(activityTags!=act2->activityTagsNames)
					diffActivityTags=true;
				if(students!=act2->studentsNames)
					diffStudents=true;
				if( /* !computeNTotalStudents && !act2->computeNTotalStudents && */ nTotalStudents!=act2->nTotalStudents )
					diffNTotalStudents=true;
				if(computeNTotalStudents!=act2->computeNTotalStudents)
					diffComputeNTotalStudents=true;
			}
			if(nSplit>10){
				QMessageBox::warning(this, tr("FET information"),
					tr("Cannot modify this large activity, because it contains more than %1 activities. "
					"If you really need that, please talk to the author").arg(10));
				return;
			}
		}
		
		if(diffTeachers || diffSubject || diffActivityTags || diffStudents || diffNTotalStudents || diffComputeNTotalStudents){
			QStringList s;
			if(diffTeachers)
				s.append(tr("different teachers"));
			if(diffSubject)
				s.append(tr("different subject"));
			if(diffActivityTags)
				s.append(tr("different activity tags"));
			if(diffStudents)
				s.append(tr("different students"));
			if(diffComputeNTotalStudents)
				s.append(tr("different boolean variable 'must compute n total students'"));
			if(diffNTotalStudents)
				s.append(tr("different number of students"));
				
			QString s2;
			s2+=tr("The current split activity has subactivities which were individually modified. It is recommended to abort now"
			 " and modify individual subactivities from the corresponding menu. Otherwise you will modify the fields for all the subactivities"
			 " from this larger split activity.");
			s2+="\n\n";
			s2+=tr("The fields which are different are: %1").arg(s.join(", "));
				
			int t=QMessageBox::warning(this, tr("FET warning"), s2, tr("Abort"), tr("Continue"), QString(), 1, 0);
			
			if(t==0)
				return;
		}
	}
	
	ModifyActivityForm modifyActivityForm(act->id, act->activityGroupId);
	int t;
	t=modifyActivityForm.exec();
	
	if(t==QDialog::Accepted){
		//cout<<"AAcc"<<endl;
		filterChanged();
	
		activitiesListWidget->verticalScrollBar()->setValue(valv);
		activitiesListWidget->horizontalScrollBar()->setValue(valh);

		if(ind >= activitiesListWidget->count())
			ind = activitiesListWidget->count()-1;
		if(ind>=0)
			activitiesListWidget->setCurrentRow(ind);
	}
	else{
		//cout<<"ARej"<<endl;
		assert(t==QDialog::Rejected);
	}
}

void ActivitiesForm::removeActivity()
{
	int ind=activitiesListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected activity"));
		return;
	}
	
	assert(ind<visibleActivitiesList.count());

	int valv=activitiesListWidget->verticalScrollBar()->value();
	int valh=activitiesListWidget->horizontalScrollBar()->value();

	Activity* act=visibleActivitiesList[ind];
	assert(act!=NULL);

	QString s;
	/*
	if(!act->isSplit())
		s=tr("Removing activity:");
	else
		s=tr("Removing sub-activity:");*/
	s=tr("Remove activity?");
	s+="\n";
	if(act->isSplit())
		s+=tr("There will also be removed the related activities from the same larger split activity");
	s+="\n\n";
	s+=act->getDetailedDescription(gt.rules);
	s+="\n";

	switch( LongTextMessageBox::confirmation( this, tr("FET confirmation"),
	s, tr("Yes"), tr("No"), 0, 0, 1 ) ){
	case 0: // The user clicked the OK button or pressed Enter
		gt.rules.removeActivity(act->id, act->activityGroupId);
		filterChanged();
		break;
	case 1: // The user clicked the Cancel or pressed Escape
		break;
	}

	activitiesListWidget->verticalScrollBar()->setValue(valv);
	activitiesListWidget->horizontalScrollBar()->setValue(valh);

	if(ind >= activitiesListWidget->count())
		ind = activitiesListWidget->count()-1;
	if(ind>=0)
		activitiesListWidget->setCurrentRow(ind);
}

void ActivitiesForm::activityChanged()
{
	int index=activitiesListWidget->currentRow();
	
	if(index<0){
		activityTextEdit->setText(tr("Invalid activity"));
		return;
	}
	if(index>=visibleActivitiesList.count()){
		activityTextEdit->setText(tr("Invalid activity"));
		return;
	}

	QString s;
	Activity* act=visibleActivitiesList[index];

	assert(act!=NULL);
	s=act->getDetailedDescriptionWithConstraints(gt.rules);
	activityTextEdit->setText(s);
}

void ActivitiesForm::help()
{
	QString s;
	
	s+=tr("Useful instructions/tips:");
	s+="\n\n";
	
	s+=tr("Above the (sub)activities list, we have 2 labels, containing 4 numbers. The first label contains text: No: a / b. The first number a is the"
		" number of active (sub)activities (we number each individual subactivity as 1), while the second number b is the number of total (sub)activities."
		" The second label contains text: Dur: c / d. The third number c is the duration of active (sub)activities, in periods"
		" (or FET hours), while the fourth number d is the duration of total (sub)activities, in periods (or FET hours)."
		" So, No means number and Dur means duration.");
	s+="\n\n";
	s+=tr("Example: No: 100 / 102, Dur: 114 / 117. They represent: 100 - the number of active (sub)activities,"
		" then 102 - the number of total (sub)activities,"
		" 114 - the duration of active activities (in periods or FET hours) and 117 - the duration of total activities"
		" (in periods or FET hours). In this example we have 2 inactive activities with their combined duration being 3 periods.");
	
	s+="\n\n";
	s+=tr("Explanation of the short description of an activity: first comes the id."
		" If the activity is inactive, an X follows. Then the duration. Then, if the activity is split, a slash and the total duration."
		" Then teachers, subject, activity tag (if it is not void) and students. Then the number of students (if specified).");
	s+="\n\n";
	s+=tr("The activities which are inactive:");
	s+="\n";
	s+=" -";
	s+=tr("have an X mark after the id.");
	s+="\n";
/*	s+=" -";
	s+=tr("are shown with lowercase letters.");
	s+="\n";*/
	s+=" -";
	s+=tr("if you use colors in interface (see Settings/Interface menu), they will appear with different background color.");
	s+="\n\n";
	s+=tr("To modify an activity, you can also double click it.");
	s+="\n\n";
	s+=tr("Show related: if you select this, there will be listed activities for groups and subgroups contained also in the current set (if the current set"
		" is a year or a group) and also higher ranked year or group (if the current set is a group or a subgroup).");
	
	LongTextMessageBox::largeInformation(this, tr("FET Help"), s);
}
