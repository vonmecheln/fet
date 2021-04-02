/***************************************************************************
                          subactivitiesform.cpp  -  description
                             -------------------
    begin                : 2009
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

//#include <iostream>
//using namespace std;

#include "timetable_defs.h"
#include "fet.h"
#include "timetable.h"

#include "studentsset.h"

#include "subactivitiesform.h"
#include "modifysubactivityform.h"

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

SubactivitiesForm::SubactivitiesForm()
{
    setupUi(this);
    
    modifySubactivityPushButton->setDefault(true);
    
    subactivitiesListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(subactivitiesListWidget, SIGNAL(currentRowChanged(int)), this /*SubactivitiesForm_template*/, SLOT(subactivityChanged()));
    connect(closePushButton, SIGNAL(clicked()), this /*SubactivitiesForm_template*/, SLOT(close()));
    connect(teachersComboBox, SIGNAL(activated(QString)), this /*SubactivitiesForm_template*/, SLOT(filterChanged()));
    connect(studentsComboBox, SIGNAL(activated(QString)), this /*SubactivitiesForm_template*/, SLOT(studentsFilterChanged()));
    connect(subjectsComboBox, SIGNAL(activated(QString)), this /*SubactivitiesForm_template*/, SLOT(filterChanged()));
    connect(modifySubactivityPushButton, SIGNAL(clicked()), this /*SubactivitiesForm_template*/, SLOT(modifySubactivity()));
    connect(activityTagsComboBox, SIGNAL(activated(QString)), this /*SubactivitiesForm_template*/, SLOT(filterChanged()));
    connect(subactivitiesListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this /*SubactivitiesForm_template*/, SLOT(modifySubactivity()));
    connect(recursiveCheckBox, SIGNAL(toggled(bool)), this /*SubactivitiesForm_template*/, SLOT(studentsFilterChanged()));

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

	this->filterChanged();
}

SubactivitiesForm::~SubactivitiesForm()
{
}

bool SubactivitiesForm::filterOk(Activity* act)
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
			if(showedStudents.contains(*it)){
				//if(*it == stn){
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

void SubactivitiesForm::studentsFilterChanged()
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

void SubactivitiesForm::filterChanged()
{
	int nacts=0, nsubacts=0, nh=0;
	int ninact=0, ninacth=0;


	QString s;
	subactivitiesListWidget->clear();
	visibleSubactivitiesList.clear();
	
	int k=0;
	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		Activity* act=gt.rules.activitiesList[i];
		if(this->filterOk(act)){
			s=act->getDescription(gt.rules);
			visibleSubactivitiesList.append(act);
			subactivitiesListWidget->addItem(s);
			k++;
			
			if(USE_GUI_COLORS && !act->active)
				subactivitiesListWidget->item(k-1)->setBackground(subactivitiesListWidget->palette().alternateBase());
			
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
		"Please leave space between fields, so that they are better visible").arg(nsubacts-ninact).arg(nsubacts));
	totalTextLabel->setText(tr("Dur: %1 / %2", "Dur means duration, %1 is the duration of active activities, %2 is the duration of total activities."
		"Please leave space between fields, so that they are better visible").arg(nh-ninacth).arg(nh));
	
	subactivitiesListWidget->setCurrentRow(0);
	
	if(subactivitiesListWidget->count()<=0)
		subactivityTextEdit->setText("");
}

void SubactivitiesForm::modifySubactivity()
{
	int ind=subactivitiesListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subactivity"));
		return;
	}
	
	assert(ind<visibleSubactivitiesList.count());
	
	int valv=subactivitiesListWidget->verticalScrollBar()->value();
	int valh=subactivitiesListWidget->horizontalScrollBar()->value();
	
	Activity* act=visibleSubactivitiesList[ind];
	assert(act!=NULL);
	
	ModifySubactivityForm modifySubactivityForm(act->id, act->activityGroupId);
	int t;
	t=modifySubactivityForm.exec();

	if(t==QDialog::Accepted){
		//cout<<"Acc"<<endl;
		filterChanged();
		
		subactivitiesListWidget->verticalScrollBar()->setValue(valv);
		subactivitiesListWidget->horizontalScrollBar()->setValue(valh);

		if(ind>=subactivitiesListWidget->count())
			ind=subactivitiesListWidget->count()-1;
		if(ind>=0)
			subactivitiesListWidget->setCurrentRow(ind);
	}
	else{
		//cout<<"Rej"<<endl;
		assert(t==QDialog::Rejected);
	}
}

void SubactivitiesForm::subactivityChanged()
{
	int index=subactivitiesListWidget->currentRow();

	if(index<0){
		subactivityTextEdit->setText(tr("Invalid activity"));
		return;
	}
	if(index>=visibleSubactivitiesList.count()){
		subactivityTextEdit->setText(tr("Invalid activity"));
		return;
	}

	QString s;
	Activity* act=visibleSubactivitiesList[index];

	assert(act!=NULL);
	s=act->getDetailedDescriptionWithConstraints(gt.rules);
	subactivityTextEdit->setText(s);
}

void SubactivitiesForm::help()
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
	s+=tr("To modify a subactivity, you can also double click it.");
	s+="\n\n";
	s+=tr("Show related: if you select this, there will be listed subactivities for groups and subgroups contained also in the current set (if the current set"
		" is a year or a group) and also higher ranked year or group (if the current set is a group or a subgroup).");
	
	LongTextMessageBox::largeInformation(this, tr("FET Help"), s);
}
