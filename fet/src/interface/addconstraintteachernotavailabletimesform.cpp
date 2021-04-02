/***************************************************************************
                          addconstraintteachernotavailabletimesform.cpp  -  description
                             -------------------
    begin                : Feb 10, 2005
    copyright            : (C) 2005 by Lalescu Liviu
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

#include "addconstraintteachernotavailabletimesform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

#define YES	(QObject::tr("Not available", "Please keep translation short"))
#define NO	(QObject::tr("Available", "Please keep translation short"))

AddConstraintTeacherNotAvailableTimesForm::AddConstraintTeacherNotAvailableTimesForm()
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	updateTeachersComboBox();

	notAllowedTimesTable->setNumRows(gt.rules.nHoursPerDay);
	notAllowedTimesTable->setNumCols(gt.rules.nDaysPerWeek);

	for(int j=0; j<gt.rules.nDaysPerWeek; j++)
		notAllowedTimesTable->horizontalHeader()->setLabel(j, gt.rules.daysOfTheWeek[j]);
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		notAllowedTimesTable->verticalHeader()->setLabel(i, gt.rules.hoursOfTheDay[i]);

	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			notAllowedTimesTable->setText(i, j, NO);
}

AddConstraintTeacherNotAvailableTimesForm::~AddConstraintTeacherNotAvailableTimesForm()
{
}

void AddConstraintTeacherNotAvailableTimesForm::updateTeachersComboBox(){
	teachersComboBox->clear();
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		teachersComboBox->insertItem(tch->name);
	}
}

void AddConstraintTeacherNotAvailableTimesForm::tableClicked(int row, int col, int button, const QPoint& mousePos)
{
	Q_UNUSED(button);
	Q_UNUSED(mousePos);

	if(row>=0 && row<gt.rules.nHoursPerDay && col>=0 && col<gt.rules.nDaysPerWeek){
		QString s=notAllowedTimesTable->text(row, col);
		if(s==YES)
			s=NO;
		else{
			assert(s==NO);
			s=YES;
		}
		notAllowedTimesTable->setText(row, col, s);
	}
}

void AddConstraintTeacherNotAvailableTimesForm::addCurrentConstraint()
{
	TimeConstraint *ctr=NULL;

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<100.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage). It has to be 100"));
		return;
	}

	QString teacher_name=teachersComboBox->currentText();
	int teacher_ID=gt.rules.searchTeacher(teacher_name);
	if(teacher_ID<0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid teacher"));
		return;
	}

	QList<int> days;
	QList<int> hours;
	for(int j=0; j<gt.rules.nDaysPerWeek; j++)
		for(int i=0; i<gt.rules.nHoursPerDay; i++)
			if(notAllowedTimesTable->text(i, j)==YES){
				days.append(j);
				hours.append(i);
			}

	ctr=new ConstraintTeacherNotAvailableTimes(weight, teacher_name, days, hours);

	bool tmp2=gt.rules.addTimeConstraint(ctr);
	if(tmp2)
		QMessageBox::information(this, tr("FET information"),
			tr("Constraint added"));
	else{
		QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - there must be another constraint of this "
			  "type referring to the same teacher. Please edit that one"));
		delete ctr;
	}
}

void AddConstraintTeacherNotAvailableTimesForm::help()
{
	QString s;
	
	s=tr("This constraint does not induce gaps for teachers. If a teacher has lessons"
	 " before and after a not available period, gaps will not be counted.");
	 
	s+="\n\n";
	 
	s+=tr("If you really need to use weight under 100%, you can use activities preferred times with"
	 " only the teacher specified, but this might generate problems, as possible gaps will be"
	 " counted and you may obtain an impossible timetable.");

	QMessageBox::information(this, tr("FET help"), s);
}
