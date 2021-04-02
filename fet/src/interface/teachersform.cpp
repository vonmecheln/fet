//
//
// Description: This file is part of FET
//
//
// Author: Lalescu Liviu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
// Copyright (C) 2003 Liviu Lalescu <http://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
//
//

#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"
#include "teachersform.h"
#include "teacher.h"

#include <QInputDialog>

#include <QMessageBox>

TeachersForm::TeachersForm()
 : TeachersForm_template()
{
    setupUi(this);

    connect(renameTeacherPushButton, SIGNAL(clicked()), this /*TeachersForm_template*/, SLOT(renameTeacher()));
    connect(closePushButton, SIGNAL(clicked()), this /*TeachersForm_template*/, SLOT(close()));
    connect(addTeacherPushButton, SIGNAL(clicked()), this /*TeachersForm_template*/, SLOT(addTeacher()));
    connect(sortTeachersPushButton, SIGNAL(clicked()), this /*TeachersForm_template*/, SLOT(sortTeachers()));
    connect(removeTeacherPushButton, SIGNAL(clicked()), this /*TeachersForm_template*/, SLOT(removeTeacher()));
    connect(teachersListBox, SIGNAL(highlighted(int)), this /*TeachersForm_template*/, SLOT(teacherChanged(int)));
    connect(activateTeacherPushButton, SIGNAL(clicked()), this /*TeachersForm_template*/, SLOT(activateTeacher()));
    connect(deactivateTeacherPushButton, SIGNAL(clicked()), this /*TeachersForm_template*/, SLOT(deactivateTeacher()));
    connect(teachersListBox, SIGNAL(selected(QString)), this /*TeachersForm_template*/, SLOT(renameTeacher()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	teachersListBox->clear();
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		teachersListBox->insertItem(tch->name);
	}
		
	if(teachersListBox->count()>0){
		teachersListBox->setCurrentItem(0);
		this->teacherChanged(0);
	}
}


TeachersForm::~TeachersForm()
{
}

void TeachersForm::addTeacher()
{
	bool ok = FALSE;
	Teacher* tch=new Teacher();
	tch->name = QInputDialog::getText( tr("User input"), tr("Please enter teacher's name") ,
                    QLineEdit::Normal, QString::null, &ok, this );

	if ( ok && !((tch->name).isEmpty()) ){
		// user entered something and pressed OK
		if(!gt.rules.addTeacher(tch)){
			QMessageBox::information( this, tr("Teacher insertion dialog"),
				tr("Could not insert item. Must be a duplicate"));
			delete tch;
		}
		else{
			teachersListBox->insertItem(tch->name);
			teachersListBox->setCurrentItem(teachersListBox->count()-1);
			this->teacherChanged(teachersListBox->count()-1);
		}
	}
	else
		delete tch;// user entered nothing or pressed Cancel
}

void TeachersForm::removeTeacher()
{
	int i=teachersListBox->currentItem();
	if(teachersListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected teacher"));
		return;
	}

	QString text=teachersListBox->currentText();
	int teacher_ID=gt.rules.searchTeacher(text);
	if(teacher_ID<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected teacher"));
		return;
	}

	if(QMessageBox::warning( this, tr("FET"),
		tr("Are you sure you want to delete this teacher and all related activities and constraints?"),
		tr("Yes"), tr("No"), 0, 0, 1 ) == 1)
		return;

	int tmp=gt.rules.removeTeacher(text);
	if(tmp){
		teachersListBox->removeItem(teachersListBox->currentItem());
		if((uint)(i)>=teachersListBox->count())
			i=teachersListBox->count()-1;
		teachersListBox->setCurrentItem(i);
		this->teacherChanged(i);		
	}
}

void TeachersForm::renameTeacher()
{
	int i=teachersListBox->currentItem();
	if(teachersListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected teacher"));
		return;
	}

	QString initialTeacherName=teachersListBox->currentText();
	int teacher_ID=gt.rules.searchTeacher(initialTeacherName);
	if(teacher_ID<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected teacher"));
		return;
	}

	bool ok = FALSE;
	QString finalTeacherName;
	finalTeacherName = QInputDialog::getText( tr("User input"), tr("Please enter new teacher's name") ,
                    QLineEdit::Normal, initialTeacherName, &ok, this );

	if ( ok && !(finalTeacherName.isEmpty())){
		// user entered something and pressed OK
		if(gt.rules.searchTeacher(finalTeacherName)>=0){
			QMessageBox::information( this, tr("Teacher modification dialog"),
				tr("Could not modify item. New name must be a duplicate"));
		}
		else{
			gt.rules.modifyTeacher(initialTeacherName, finalTeacherName);
			teachersListBox->changeItem(finalTeacherName, i);
		}
	}
}

void TeachersForm::sortTeachers()
{
	gt.rules.sortTeachersAlphabetically();

	teachersListBox->clear();
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		teachersListBox->insertItem(tch->name);
	}
}

void TeachersForm::teacherChanged(int index)
{
	if(index<0){
		//currentTeacherTextEdit->setText(tr("Invalid teacher"));
		currentTeacherTextEdit->setText("");
		return;
	}
	
	Teacher* t=gt.rules.teachersList.at(index);
	assert(t);
	QString s=t->getDetailedDescriptionWithConstraints(gt.rules);
	currentTeacherTextEdit->setText(s);
}

void TeachersForm::activateTeacher()
{
	if(teachersListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected teacher"));
		return;
	}

	QString teacherName=teachersListBox->currentText();
	int count=gt.rules.activateTeacher(teacherName);
	QMessageBox::information(this, tr("FET information"), tr("Activated a number of %1 activities").arg(count));
}

void TeachersForm::deactivateTeacher()
{
	if(teachersListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected teacher"));
		return;
	}

	QString teacherName=teachersListBox->currentText();
	int count=gt.rules.deactivateTeacher(teacherName);
	QMessageBox::information(this, tr("FET information"), tr("De-activated a number of %1 activities").arg(count));
}
