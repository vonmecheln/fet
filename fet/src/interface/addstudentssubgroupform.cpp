/***************************************************************************
                          addstudentssubgroupform.cpp  -  description
                             -------------------
    begin                : Sat Jan 24 2004
    copyright            : (C) 2004 by Lalescu Liviu
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

#include "addstudentssubgroupform.h"

#include <qlineedit.h>

#include <QDesktopWidget>

AddStudentsSubgroupForm::AddStudentsSubgroupForm()
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	nameLineEdit->selectAll();
	nameLineEdit->setFocus();
}

AddStudentsSubgroupForm::~AddStudentsSubgroupForm()
{
}

void AddStudentsSubgroupForm::addStudentsSubgroup()
{
	if(nameLineEdit->text().isEmpty()){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Incorrect name"));
		return;
	}
	QString subgroupName=nameLineEdit->text();
	QString yearName=yearNameLineEdit->text();
	QString groupName=groupNameLineEdit->text();

	if(gt.rules.searchSubgroup(yearName, groupName, subgroupName)>=0){
		QMessageBox::information( this, QObject::tr("Subgroup insertion dialog"),
			QObject::tr("Could not insert item. Must be a duplicate"));
		return;
	}
	StudentsSet* ss=gt.rules.searchStudentsSet(subgroupName);
	StudentsSubgroup* sts;
	if(ss!=NULL && ss->type==STUDENTS_YEAR){
		QMessageBox::information( this, QObject::tr("Subgroup insertion dialog"),
			QObject::tr("This name is taken for a year - please consider another name"));
		return;
	}
	if(ss!=NULL && ss->type==STUDENTS_GROUP){
		QMessageBox::information( this, QObject::tr("Subgroup insertion dialog"),
			QObject::tr("This name is taken for a group - please consider another name"));
		return;
	}
	if(ss!=NULL){ //already existing subgroup, but in other group. Several groups share the same subgroup.
		assert(ss->type==STUDENTS_SUBGROUP);
		if(QMessageBox::warning( this, QObject::tr("FET"),
			QObject::tr("This subgroup already exists, but in another group\n"
			"If you insert current subgroup to current group, that\n"
			"means that some groups share the same subgroup (overlap)\n"
			"If you want to make a new subgroup, independent,\n"
			"please abort now and give it another name\n"),
			QObject::tr("Add"), QObject::tr("Abort"), 0, 0, 1 ) == 1)
			return;

		numberSpinBox->setValue(ss->numberOfStudents);
		sts=(StudentsSubgroup*)ss;
	}
	else{
		sts=new StudentsSubgroup();
		sts->name=subgroupName;
		sts->numberOfStudents=numberSpinBox->value();
	}
	gt.rules.addSubgroup(yearName, groupName, sts);
	QMessageBox::information(this, QObject::tr("Subgroup insertion dialog"),
		QObject::tr("Subgroup added"));

	nameLineEdit->selectAll();
	nameLineEdit->setFocus();
}
