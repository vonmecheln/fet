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

StudentsStatisticsForm::StudentsStatisticsForm()
{
	setupUi(this);

	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);
	
	connect(showYearsCheckBox, SIGNAL(toggled(bool)), this, SLOT(checkBoxesModified()));
	connect(showGroupsCheckBox, SIGNAL(toggled(bool)), this, SLOT(checkBoxesModified()));
	connect(showSubgroupsCheckBox, SIGNAL(toggled(bool)), this, SLOT(checkBoxesModified()));
	
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
				if(subgroup)
					;
				if(showSubgroupsCheckBox->isChecked())
					nStudentsSets++;
			}
		}
	}
	
	tableWidget->setColumnCount(3);
	tableWidget->setRowCount(nStudentsSets);
	
	QStringList columns;
	columns<<QObject::tr("Students set");
	columns<<QObject::tr("No. of sub-activities");
	columns<<QObject::tr("No. of hours");
	
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
		
	foreach(Activity* act, gt.rules.activitiesList)
		if(act->active)
			foreach(QString asn, act->studentsNames)
				if(gt.rules.studentsSetsRelated(asn, set->name)){
					nSubActivities++;
					nHours+=act->duration;
					break;
				}

	newItem=new QTableWidgetItem(QString::number(nSubActivities));
	newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	tableWidget->setItem(row, 1, newItem);

	newItem=new QTableWidgetItem(QString::number(nHours));
	newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	tableWidget->setItem(row, 2, newItem);
}
