/***************************************************************************
                          addconstraintteacherhomeroomsform.cpp  -  description
                             -------------------
    begin                : April 8, 2005
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

#include <QMessageBox>

#include <cstdio>

#include "longtextmessagebox.h"

#include "addconstraintteacherhomeroomsform.h"
#include "spaceconstraint.h"

AddConstraintTeacherHomeRoomsForm::AddConstraintTeacherHomeRoomsForm()
{
    setupUi(this);

//    connect(addPushButton, SIGNAL(clicked()), this /*AddConstraintTeacherHomeRoomsForm_template*/, SLOT(addRoom()));
//    connect(removePushButton, SIGNAL(clicked()), this /*AddConstraintTeacherHomeRoomsForm_template*/, SLOT(removeRoom()));
    connect(closePushButton, SIGNAL(clicked()), this /*AddConstraintTeacherHomeRoomsForm_template*/, SLOT(close()));
    connect(addConstraintPushButton, SIGNAL(clicked()), this /*AddConstraintTeacherHomeRoomsForm_template*/, SLOT(addConstraint()));
    connect(roomsListBox, SIGNAL(selected(QString)), this /*AddConstraintTeacherHomeRoomsForm_template*/, SLOT(addRoom()));
    connect(selectedRoomsListBox, SIGNAL(selected(QString)), this /*AddConstraintTeacherHomeRoomsForm_template*/, SLOT(removeRoom()));

    connect(clearPushButton, SIGNAL(clicked()), this, SLOT(clear()));

	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	
	updateRoomsListBox();

	teachersComboBox->clear();
	foreach(Teacher* tch, gt.rules.teachersList)
		teachersComboBox->insertItem(tch->name);
}

AddConstraintTeacherHomeRoomsForm::~AddConstraintTeacherHomeRoomsForm()
{
}

void AddConstraintTeacherHomeRoomsForm::updateRoomsListBox()
{
	roomsListBox->clear();
	selectedRoomsListBox->clear();

	for(int i=0; i<gt.rules.roomsList.size(); i++){
		Room* rm=gt.rules.roomsList[i];
		roomsListBox->insertItem(rm->name);
	}
}

void AddConstraintTeacherHomeRoomsForm::addConstraint()
{
	SpaceConstraint *ctr=NULL;

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight"));
		return;
	}

	if(selectedRoomsListBox->count()==0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Empty list of selected rooms"));
		return;
	}
	if(selectedRoomsListBox->count()==1){
		QMessageBox::warning(this, tr("FET information"),
			tr("Only one selected room - please use constraint teacher home room if you want a single room"));
		return;
	}

	QString teacher=teachersComboBox->currentText();
	assert(gt.rules.searchTeacher(teacher)>=0);

	QStringList roomsList;
	for(uint i=0; i<selectedRoomsListBox->count(); i++)
		roomsList.append(selectedRoomsListBox->text(i));
	
	ctr=new ConstraintTeacherHomeRooms(weight, teacher, roomsList);
	bool tmp2=gt.rules.addSpaceConstraint(ctr);
	
	if(tmp2){
		QString s=tr("Constraint added:");
		s+="\n\n";
		s+=ctr->getDetailedDescription(gt.rules);
		LongTextMessageBox::information(this, tr("FET information"), s);
	}
	else{
		QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - please report error"));
		delete ctr;
	}
}

void AddConstraintTeacherHomeRoomsForm::addRoom()
{
	if(roomsListBox->currentItem()<0)
		return;
	QString rmName=roomsListBox->currentText();
	assert(rmName!="");
	uint i;
	//duplicate?
	for(i=0; i<selectedRoomsListBox->count(); i++)
		if(rmName==selectedRoomsListBox->text(i))
			break;
	if(i<selectedRoomsListBox->count())
		return;
	selectedRoomsListBox->insertItem(rmName);
}

void AddConstraintTeacherHomeRoomsForm::removeRoom()
{
	if(selectedRoomsListBox->currentItem()<0 || selectedRoomsListBox->count()<=0)
		return;		
	selectedRoomsListBox->removeItem(selectedRoomsListBox->currentItem());
}

void AddConstraintTeacherHomeRoomsForm::clear()
{
	selectedRoomsListBox->clear();
}
