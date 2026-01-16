/***************************************************************************
                          teachersstatisticform.cpp  -  description
                             -------------------
    begin                : March 25, 2006
    copyright            : (C) 2006 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "teachersstatisticsform.h"

#include "timetable_defs.h"
#include "timetable.h"

#include "fet.h"

#include <Qt>

#include <QString>
#include <QStringList>

#include <QTableWidget>
#include <QHeaderView>

#include <QSet>
#include <QHash>

TeachersStatisticsForm::TeachersStatisticsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	closeButton->setDefault(true);

	connect(highlightIncompleteTeachersCheckBox, &QCheckBox::toggled, this, &TeachersStatisticsForm::highlightIncompleteTeachersCheckBoxModified);
	connect(hideFullTeachersCheckBox, &QCheckBox::toggled, this, &TeachersStatisticsForm::hideFullTeachersCheckBoxModified);

	connect(closeButton, &QPushButton::clicked, this, &TeachersStatisticsForm::close);
	
	tableViewSetHighlightHeader(tableWidget);
	
	tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QHash<QString, QSet<Activity*>> activitiesForTeacher;
	
	for(Activity* act : std::as_const(gt.rules.activitiesList))
		if(act->active)
			for(const QString& teacherName : std::as_const(act->teachersNames)){
				QSet<Activity*> acts=activitiesForTeacher.value(teacherName, QSet<Activity*>());
				acts.insert(act);
				activitiesForTeacher.insert(teacherName, acts);
			}
	
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* t=gt.rules.teachersList[i];
		
		int nSubActivities=0;
		int nHours=0;
		
		QSet<Activity*> acts=activitiesForTeacher.value(t->name, QSet<Activity*>());
		
		for(Activity* act : std::as_const(acts)){
			if(act->active){
				nSubActivities++;
				nHours+=act->duration;
			}
			else{
				assert(0);
			}
		}
		
		names.append(t->name);
		subactivities.append(nSubActivities);
		durations.append(nHours);
		targets.append(t->targetNumberOfHours);

		if(nHours==t->targetNumberOfHours)
			hideFullTeacher.append(true);
		else
			hideFullTeacher.append(false);
	}

	hideFullTeachersCheckBoxModified();
}

TeachersStatisticsForm::~TeachersStatisticsForm()
{
	saveFETDialogGeometry(this);
}

void TeachersStatisticsForm::highlightIncompleteTeachersCheckBoxModified()
{
	hideFullTeachersCheckBoxModified();
}

void TeachersStatisticsForm::hideFullTeachersCheckBoxModified()
{
	tableWidget->clear();
	
	int n_rows=0;
	for(bool b : std::as_const(hideFullTeacher))
		if(!(hideFullTeachersCheckBox->isChecked() && b))
			n_rows++;

	tableWidget->setColumnCount(4);
	tableWidget->setRowCount(n_rows);
	
	QStringList columns;
	columns<<tr("Teacher");
	columns<<tr("No. of activities");
	columns<<tr("Duration");
	columns<<tr("Target duration", "It means the target duration of activities for each teacher");
	
	tableWidget->setHorizontalHeaderLabels(columns);

	int j=0;
	for(int i=0; i<gt.rules.teachersList.count(); i++){
		if(!(hideFullTeachersCheckBox->isChecked() && hideFullTeacher.at(i))){
			QTableWidgetItem* newItem=new QTableWidgetItem(names.at(i));
			newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			tableWidget->setItem(j, 0, newItem);

			newItem=new QTableWidgetItem(CustomFETString::number(subactivities.at(i)));
			newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			tableWidget->setItem(j, 1, newItem);
	
			newItem=new QTableWidgetItem(CustomFETString::number(durations.at(i)));
			newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			tableWidget->setItem(j, 2, newItem);
		
			newItem=new QTableWidgetItem(CustomFETString::number(targets.at(i)));
			newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			tableWidget->setItem(j, 3, newItem);
			
			if(highlightIncompleteTeachersCheckBox->isChecked() && !hideFullTeacher.at(i)){
				for(int k=0; k<4; k++){
					tableWidget->item(j, k)->setBackground(tableWidget->palette().highlight());
					tableWidget->item(j, k)->setForeground(tableWidget->palette().highlightedText());
					/*tableWidget->item(j, k)->setBackground(QBrush(QColorConstants::DarkGreen));
					tableWidget->item(j, k)->setForeground(QBrush(QColorConstants::DarkRed));*/
				}
			}
			
			j++;
		}
	}
	
	tableWidget->resizeColumnsToContents();
	tableWidget->resizeRowsToContents();
}
