/***************************************************************************
                          addstudentsgroupform.cpp  -  description
                             -------------------
    begin                : Sat Jan 24 2004
    copyright            : (C) 2004 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "addstudentsgroupform.h"

#include <QMessageBox>

AddStudentsGroupForm::AddStudentsGroupForm(QWidget* parent, const QString& yearName): QDialog(parent)
{
	setupUi(this);
	
	addStudentsGroupPushButton->setDefault(true);

	connect(addStudentsGroupPushButton, &QPushButton::clicked, this, &AddStudentsGroupForm::addStudentsGroup);
	connect(closePushButton, &QPushButton::clicked, this, &AddStudentsGroupForm::close);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
		
	yearNameLineEdit->setText(yearName);

	nameLineEdit->selectAll();
	nameLineEdit->setFocus();
	
	numberSpinBox->setMaximum(MAX_ROOM_CAPACITY);
	numberSpinBox->setMinimum(0);
	numberSpinBox->setValue(0);
}

AddStudentsGroupForm::~AddStudentsGroupForm()
{
	saveFETDialogGeometry(this);
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
			tr("Could not insert item. Must be a duplicate"));

		nameLineEdit->selectAll();
		nameLineEdit->setFocus();

		return;
	}
	StudentsSet* ss=gt.rules.searchStudentsSet(groupName);
	if(ss!=nullptr && ss->type==STUDENTS_YEAR){
		QMessageBox::information( this, tr("Group insertion dialog"),
			tr("This name is taken for a year - please consider another name"));

		nameLineEdit->selectAll();
		nameLineEdit->setFocus();

		return;
	}
	if(ss!=nullptr && ss->type==STUDENTS_SUBGROUP){
		QMessageBox::information( this, tr("Group insertion dialog"),
			tr("This name is taken for a subgroup - please consider another name"));

		nameLineEdit->selectAll();
		nameLineEdit->setFocus();

		return;
	}
	if(ss!=nullptr){ //already existing group, but in other year. It is the same group.
		assert(ss->type==STUDENTS_GROUP);
		/*if(QMessageBox::warning( this, tr("FET"),
			tr("This group already exists, but in another year. "
			"If you insert current group to current year, that "
			"means that some years share the same group (overlap). "
			"If you want to make a new group, independent, "
			"please abort now and give it another name.")+"\n\n"+tr("Note: the number of students for the added group will be the number of students of the already existing group"
			" (you can modify the number of students in the modify group dialog)."),
			tr("Add"),tr("Abort"), QString(), 0, 1 ) == 1){

			nameLineEdit->selectAll();
			nameLineEdit->setFocus();

			return;
		}*/
		if(QMessageBox::warning( this, tr("FET"),
			tr("This group already exists, but in another year. "
			"If you insert current group to current year, that "
			"means that some years share the same group (overlap). "
			"If you want to make a new group, independent, "
			"please abort now and give it another name.")+"\n\n"+tr("Note: the number of students for the added group will be the number of students of the already existing group"
			" (you can modify the number of students in the modify group dialog)."),
			QMessageBox::Ok | QMessageBox::Cancel ) == QMessageBox::Cancel){

			nameLineEdit->selectAll();
			nameLineEdit->setFocus();

			return;
		}

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
		
	gt.rules.addUndoPoint(tr("Added the group %1 in the year %2.").arg(sg->name).arg(yearName));

	nameLineEdit->selectAll();
	nameLineEdit->setFocus();
}
