/***************************************************************************
                          addstudentsgroupform.cpp  -  description
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

#include "addstudentsgroupform.h"

#include <qlineedit.h>

#include <QDesktopWidget>

#include <QMessageBox>

AddStudentsGroupForm::AddStudentsGroupForm(const QString& yearName)
{
    setupUi(this);

    connect(addStudentsYearPushButton, SIGNAL(clicked()), this /*addStudentsGroupForm_template*/, SLOT(addStudentsGroup()));
    connect(closePushButton, SIGNAL(clicked()), this/*addStudentsGroupForm_template*/, SLOT(close()));

	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
		
	yearNameLineEdit->setText(yearName);

	nameLineEdit->selectAll();
	nameLineEdit->setFocus();
	
	numberSpinBox->setMaxValue(MAX_ROOM_CAPACITY);
	numberSpinBox->setMinValue(0);
	numberSpinBox->setValue(0);
}

AddStudentsGroupForm::~AddStudentsGroupForm()
{
}

void AddStudentsGroupForm::addStudentsGroup()
{
	StudentsGroup* sg;

	if(nameLineEdit->text().isEmpty()){
		QMessageBox::information(this, tr("FET information"), tr("Incorrect name"));
		return;
	}
	QString yearName=yearNameLineEdit->text();
	QString groupName=nameLineEdit->text();

	if(gt.rules.searchGroup(yearName, groupName)>=0){
		QMessageBox::information( this, tr("Group insertion dialog"),
			tr("Could not insert item. Duplicate"));
		return;
	}
	StudentsSet* ss=gt.rules.searchStudentsSet(groupName);
	if(ss!=NULL && ss->type==STUDENTS_YEAR){
		QMessageBox::information( this, tr("Group insertion dialog"),
			tr("This name is taken for a year - please consider another name"));
		return;
	}
	if(ss!=NULL && ss->type==STUDENTS_SUBGROUP){
		QMessageBox::information( this, tr("Group insertion dialog"),
			tr("This name is taken for a subgroup - please consider another name"));
		return;
	}
	if(ss!=NULL){ //already existing group, but in other year. It is the same group.
		assert(ss->type==STUDENTS_GROUP);
		if(QMessageBox::warning( this, tr("FET"),
			tr("This group already exists, but in another year\n"
			"If you insert current group to current year, that\n"
			"means that some years share the same group (overlap)\n"
			"If you want to make a new group, independent,\n"
			"please abort now and give it another name\n"),
			tr("Add"),tr("Abort"), 0, 0, 1 ) == 1)
			return;

		numberSpinBox->setValue(ss->numberOfStudents);
		sg=(StudentsGroup*)ss;
	}
	else{
		sg=new StudentsGroup();
		sg->name=groupName;
		sg->numberOfStudents=numberSpinBox->value();
	}

	gt.rules.addGroup(yearName, sg);
	QMessageBox::information(this, tr("Group insertion dialog"),
		tr("Group added"));

	nameLineEdit->selectAll();
	nameLineEdit->setFocus();
}
