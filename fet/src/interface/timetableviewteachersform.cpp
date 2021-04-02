/***************************************************************************
                          timetableviewteachersform.cpp  -  description
                             -------------------
    begin                : Wed May 14 2003
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

#include "timetableviewteachersform.h"

#include "timetable_defs.h"
#include "timetable.h"
#include "solution.h"

#include "fet.h"

#include <q3combobox.h>
#include <qmessagebox.h>
#include <q3groupbox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <q3table.h>
#include <qapplication.h>
#include <q3textedit.h>
#include <qstring.h>
#include <q3listbox.h>
#include <qlabel.h>
#include <q3table.h>

#include <QDesktopWidget>

extern bool students_schedule_ready;
extern bool teachers_schedule_ready;

extern Solution best_solution;

extern bool simulation_running;

TimetableViewTeachersForm::TimetableViewTeachersForm()
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	teachersListBox->clear();
	for(int i=0; i<gt.rules.nInternalTeachers; i++)
		teachersListBox->insertItem(gt.rules.internalTeachersList[i]->name);
	teacherChanged(teachersListBox->currentText());
}

TimetableViewTeachersForm::~TimetableViewTeachersForm()
{
}

void TimetableViewTeachersForm::teacherChanged(const QString &teacherName)
{
	if(teacherName==QString::null)
		return;

	int teacherId=gt.rules.searchTeacher(teacherName);
	if(teacherId<0)
		return;

	updateTeachersTimetableTable();
}

void TimetableViewTeachersForm::updateTeachersTimetableTable(){
	assert(students_schedule_ready && teachers_schedule_ready);

	QString s;
	QString teachername;

	if(teachersListBox->currentText()==QString::null)
		return;

	teachername = teachersListBox->currentText();

	s = teachername;
	teacherNameTextLabel->setText(s);

	assert(gt.rules.initialized);
	teachersTimetableTable->setNumRows(gt.rules.nHoursPerDay);
	teachersTimetableTable->setNumCols(gt.rules.nDaysPerWeek);
	for(int j=0; j<gt.rules.nDaysPerWeek; j++)
		//teachersTimetableTable->setText(0, j+1, gt.rules.daysOfTheWeek[j]);
		teachersTimetableTable->horizontalHeader()->setLabel(j, gt.rules.daysOfTheWeek[j]);
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		//teachersTimetableTable->setText(i+1, 0, gt.rules.hoursOfTheDay[i]);
		teachersTimetableTable->verticalHeader()->setLabel(i, gt.rules.hoursOfTheDay[i]);

	int teacher=gt.rules.searchTeacher(teachername);
	if(teacher<0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("The teacher's timetable cannot be printed,\nbecause the teacher is invalid"));
	}
	else{
		for(int j=0; j<gt.rules.nHoursPerDay; j++){
			for(int k=0; k<gt.rules.nDaysPerWeek; k++){
				s = "";
				int ai=teachers_timetable_weekly[teacher][k][j]; //activity index
				//Activity* act=gt.rules.activitiesList.at(ai);
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					assert(act!=NULL);
					s += act->subjectName+" "+act->subjectTagName+"\n"; //added in version 3_9_16, on 16 Oct. 2004; suggested by Nicholas Robinson
					for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
						s += (*it) + " ";

					int r=best_solution.rooms[ai];
					if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
						s+=" ";
						s+=QObject::tr("R:")+gt.rules.internalRoomsList[r]->name;
					}
				}
				teachersTimetableTable->setText(j, k, s);
			}
		}
		for(int i=0; i<gt.rules.nHoursPerDay; i++) //added in version 3_9_16, on 16 Oct. 2004
			teachersTimetableTable->adjustRow(i);
	}
}

void TimetableViewTeachersForm::detailActivity(int row, int col){
	assert(students_schedule_ready && teachers_schedule_ready);

	QString s;
	QString teachername;

	if(teachersListBox->currentText()==QString::null)
		return;

	teachername = teachersListBox->currentText();

	s = teachername;

	teacherNameTextLabel->setText(s);

	int teacher=gt.rules.searchTeacher(teachername);
	if(teacher<0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("The teacher is invalid"));
	}
	else{
		int j=row;
		int k=col;
		s = "";
		if(j>=0 && k>=0){
			int ai=teachers_timetable_weekly[teacher][k][j]; //activity index
			//Activity* act=gt.rules.activitiesList.at(ai);
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				assert(act!=NULL);
				//s += act->getDetailedDescriptionWithConstraints(gt.rules);
				s += act->getDetailedDescription(gt.rules);

				//int r=rooms_timetable_weekly[teacher][k][j];
				int r=best_solution.rooms[ai];
				if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
					s+="\n";
					s+=QObject::tr("Room: ")+gt.rules.internalRoomsList[r]->name;
					s+="\n";
				}
			}
		}
		detailsTextEdit->setText(s);
	}
}

void TimetableViewTeachersForm::lock()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	//find teacher index
	QString teachername;

	if(teachersListBox->currentText()==QString::null)
		return;

	teachername = teachersListBox->currentText();
	int i=gt.rules.searchTeacher(teachername);

	Solution* tc=&best_solution;

	//lock selected activities
	for(int j=0; j<gt.rules.nHoursPerDay; j++){
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			if(teachersTimetableTable->isSelected(j, k)){
				int ai=teachers_timetable_weekly[i][k][j];
				if(ai!=UNALLOCATED_ACTIVITY){
					int time=tc->times[ai];
					int hour=time/gt.rules.nDaysPerWeek;
					int day=time%gt.rules.nDaysPerWeek;
					//Activity* act=gt.rules.activitiesList.at(ai);
					Activity* act=&gt.rules.internalActivitiesList[ai];
					ConstraintActivityPreferredTime* ctr=new ConstraintActivityPreferredTime(100.0, act->id, day, hour);
					bool t=gt.rules.addTimeConstraint(ctr);
					if(t)
						QMessageBox::information(this, QObject::tr("FET information"), 
						 QObject::tr("Added the following constraint:\n%1").arg(ctr->getDetailedDescription(gt.rules)));
					else{
						QMessageBox::warning(this, QObject::tr("FET information"), 
						 QObject::tr("Constraint\n%1 NOT added - duplicate").arg(ctr->getDetailedDescription(gt.rules)));
						delete ctr;
					}
				}
			}
		}
	}
}
