/***************************************************************************
                          studentsstatisticform.cpp  -  description
                             -------------------
    begin                : March 25, 2006
    copyright            : (C) 2006 by Lalescu Liviu
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

#include "studentsstatisticsform.h"

#include "timetable_defs.h"
#include "timetable.h"

#include "fet.h"

#include <QString>
#include <QStringList>

#include <QDesktopWidget>

#include <QHash>

#include <QProgressDialog>

#include <QMessageBox>
#include <QApplication>

extern QApplication* pqapplication;

#include <iostream>
using namespace std;

//QHash<QString, int> onlyExactHours; //for each students set, only the hours from each activity which has exactly the same set
//does not include related sets.

//QHash<QString, int> onlyExactActivities;


//QHash<QString, int> allStudentsSets; //int is type
QSet<QString> allStudentsSets;


QHash<QString, int> allHours; //for each students set, only the hours from each activity which has exactly the same set
//does not include related sets.

QHash<QString, int> allActivities;

QSet<QString> related; //related to current set

QSet<QString> relatedSubgroups;
QSet<QString> relatedGroups;
QSet<QString> relatedYears;

StudentsStatisticsForm::StudentsStatisticsForm()
{
	setupUi(this);

	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
		
	connect(showYearsCheckBox, SIGNAL(toggled(bool)), this, SLOT(checkBoxesModified()));
	connect(showGroupsCheckBox, SIGNAL(toggled(bool)), this, SLOT(checkBoxesModified()));
	connect(showSubgroupsCheckBox, SIGNAL(toggled(bool)), this, SLOT(checkBoxesModified()));

	////////////	
	/*onlyExactHours.clear();
	onlyExactActivities.clear();
	
	foreach(Activity* act, gt.rules.activitiesList) if(act->active){
		foreach(QString stud, act->studentsNames){
			//hours
			int crt=0;
			if(onlyExactHours.contains(stud))
				crt=onlyExactHours.value(stud);
			onlyExactHours.insert(stud, crt+act->duration);

			//n activities
			crt=0;
			if(onlyExactActivities.contains(stud))
				crt=onlyExactActivities.value(stud);
			onlyExactActivities.insert(stud, crt+1);
		}
	}*/
	////////////

	////////////
	/*allStudentsSets.clear();
	foreach(StudentsYear* year, gt.rules.yearsList){
		allStudentsSets.insert(year->name, STUDENTS_YEAR);
		foreach(StudentsGroup* group, year->groupsList){
			allStudentsSets.insert(group->name, STUDENTS_GROUP);
			foreach(StudentsSubgroup* subgroup, group->subgroupsList){
				allStudentsSets.insert(subgroup->name, STUDENTS_SUBGROUP);
			}
		}
	}*/
	allStudentsSets.clear();
	foreach(StudentsYear* year, gt.rules.yearsList){
		allStudentsSets.insert(year->name);
		foreach(StudentsGroup* group, year->groupsList){
			allStudentsSets.insert(group->name);
			foreach(StudentsSubgroup* subgroup, group->subgroupsList){
				allStudentsSets.insert(subgroup->name);
			}
		}
	}
	///////////
	
	///////////
	allHours.clear();
	allActivities.clear();	
	
	QProgressDialog progress(this);
	progress.setLabelText(tr("Computing ... please wait"));
	progress.setRange(0, allStudentsSets.count());
	progress.setModal(true);
						
	int ttt=0;
							
	foreach(QString set, allStudentsSets){
		progress.setValue(ttt);
		pqapplication->processEvents();
		if(progress.wasCanceled()){
			QMessageBox::information(NULL, tr("FET information"), tr("Canceled"));
			showYearsCheckBox->setDisabled(true);
			showGroupsCheckBox->setDisabled(true);
			showSubgroupsCheckBox->setDisabled(true);
			return;
		}
		ttt++;
	
		relatedSubgroups.clear();
		
		relatedGroups.clear();
		
		relatedYears.clear();
		
		related.clear();
		
		foreach(StudentsYear* year, gt.rules.yearsList){
			bool y=false;
			if(year->name==set)
				y=true;
			if(y)
				relatedYears.insert(year->name);
			foreach(StudentsGroup* group, year->groupsList){
				bool g=false;
				if(group->name==set)
					g=true;
				if(y || g)
					relatedGroups.insert(group->name);
				foreach(StudentsSubgroup* subgroup, group->subgroupsList){
					if(y || g || subgroup->name==set)
						relatedSubgroups.insert(subgroup->name);
				}
			}
		}
		
		foreach(StudentsYear* year, gt.rules.yearsList){
			if(relatedYears.contains(year->name))
				related.insert(year->name);
			foreach(StudentsGroup* group, year->groupsList){
				if(relatedGroups.contains(group->name)){
					related.insert(year->name);
					related.insert(group->name);
				}
				foreach(StudentsSubgroup* subgroup, group->subgroupsList){
					if(relatedSubgroups.contains(subgroup->name)){
						related.insert(year->name);
						related.insert(group->name);
						related.insert(subgroup->name);
					}
				}
			}
		}
		
		/*foreach(StudentsYear* year, gt.rules.yearsList){
			if(year->name==set)
				related.insert(year->name);
			foreach(StudentsGroup* group, year->groupsList){
				if(group->name==set){
					related.insert(group->name);
					related.insert(year->name);
				}
				if(year->name==set)
					related.insert(group->name);
				foreach(StudentsSubgroup* subgroup, group->subgroupsList){
					if(subgroup->name==set){
						related.insert(subgroup->name);
						related.insert(group->name);
						related.insert(year->name);
					}
					if(group->name==set)
						related.insert(subgroup->name);
					if(year->name==set)
						related.insert(subgroup->name);
				}
			}
		}*/
		
		int nh=0;
		int na=0;
		
		/*foreach(QString rel, related){
			//int nh=allHours.value(set);
			nh+=onlyExactHours.value(rel);
			//allHours.insert(set, nh);
			
			//int na=allActivities.value(set);
			na+=onlyExactActivities.value(rel);
			//allActivities.insert(set, na);
		}*/
		
		foreach(Activity* act, gt.rules.activitiesList) if(act->active){
			foreach(QString stud, act->studentsNames){
				if(related.contains(stud)){
					nh += act->duration;
					na ++;
					
					break;
				}
			}
		}
		
		allHours.insert(set, nh);
		allActivities.insert(set, na);
	}
	////////////

	progress.setValue(allStudentsSets.count());
	
	checkBoxesModified();	
}

StudentsStatisticsForm::~StudentsStatisticsForm()
{
}

void StudentsStatisticsForm::checkBoxesModified()
{
	int nStudentsSets=0;
	foreach(StudentsYear* year, gt.rules.yearsList){
		if(showYearsCheckBox->isChecked())
			nStudentsSets++;
		foreach(StudentsGroup* group, year->groupsList){
			if(showGroupsCheckBox->isChecked())
				nStudentsSets++;
			foreach(StudentsSubgroup* subgroup, group->subgroupsList){
				Q_UNUSED(subgroup);
				//if(subgroup)
				//	;
				if(showSubgroupsCheckBox->isChecked())
					nStudentsSets++;
			}
		}
	}
	
	tableWidget->setColumnCount(3);
	tableWidget->setRowCount(nStudentsSets);
	
	QStringList columns;
	columns<<tr("Students set");
	columns<<tr("No. of activities");
	columns<<tr("No. of hours")+" ("+tr("periods")+")";
	
	tableWidget->setHorizontalHeaderLabels(columns);
	
	int currentStudentsSet=-1;
	foreach(StudentsYear* year, gt.rules.yearsList){
		if(showYearsCheckBox->isChecked()){
			currentStudentsSet++;		
			insertStudentsSet(year, currentStudentsSet);
		}
				
		foreach(StudentsGroup* group, year->groupsList){
			if(showGroupsCheckBox->isChecked()){
				currentStudentsSet++;
				insertStudentsSet(group, currentStudentsSet);
			}
			
			foreach(StudentsSubgroup* subgroup, group->subgroupsList){
				if(showSubgroupsCheckBox->isChecked()){
					currentStudentsSet++;
					insertStudentsSet(subgroup, currentStudentsSet);
				}
			}	
		}
	}
	
	tableWidget->resizeColumnsToContents();
	tableWidget->resizeRowsToContents();
}

void StudentsStatisticsForm::insertStudentsSet(StudentsSet* set, int row)
{
	QTableWidgetItem* newItem=new QTableWidgetItem(set->name);
	newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	tableWidget->setItem(row, 0, newItem);

	int	nSubActivities=0;
	int nHours=0;
	
	if(allHours.contains(set->name))
		nHours=allHours.value(set->name);
	else{
		cout<<qPrintable(set->name)<<endl;
		assert(0);
	}
		
	if(allActivities.contains(set->name))
		nSubActivities=allActivities.value(set->name);
	else
		assert(0);
		
	/*foreach(Activity* act, gt.rules.activitiesList)
		if(act->active)
			foreach(QString asn, act->studentsNames)
				if(gt.rules.studentsSetsRelated(asn, set->name)){
					nSubActivities++;
					nHours+=act->duration;
					break;
				}*/

	newItem=new QTableWidgetItem(QString::number(nSubActivities));
	newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	tableWidget->setItem(row, 1, newItem);

	newItem=new QTableWidgetItem(QString::number(nHours));
	newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	tableWidget->setItem(row, 2, newItem);
}
