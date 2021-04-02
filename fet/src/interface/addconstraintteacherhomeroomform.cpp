/***************************************************************************
                          addconstraintteacherhomeroomform.cpp  -  description
                             -------------------
    begin                : 8 Apr 2005
    copyright            : (C) 2005 by Liviu Lalescu
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

#include "longtextmessagebox.h"

#include "addconstraintteacherhomeroomform.h"
#include "spaceconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <q3table.h>

#include <QDesktopWidget>

AddConstraintTeacherHomeRoomForm::AddConstraintTeacherHomeRoomForm()
{
    setupUi(this);

    connect(closePushButton, SIGNAL(clicked()), this /*AddConstraintTeacherHomeRoomForm_template*/, SLOT(close()));
    connect(addConstraintPushButton, SIGNAL(clicked()), this /*AddConstraintTeacherHomeRoomForm_template*/, SLOT(addConstraint()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	
	QSize tmp5=roomsComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	
	updateTeachersComboBox();
	updateRoomsComboBox();
}

AddConstraintTeacherHomeRoomForm::~AddConstraintTeacherHomeRoomForm()
{
}

void AddConstraintTeacherHomeRoomForm::updateTeachersComboBox(){
	teachersComboBox->clear();
	foreach(Teacher* tch, gt.rules.teachersList)
		teachersComboBox->insertItem(tch->name);
}

void AddConstraintTeacherHomeRoomForm::updateRoomsComboBox()
{
	roomsComboBox->clear();
	for(int i=0; i<gt.rules.roomsList.size(); i++){
		Room* rm=gt.rules.roomsList[i];
		roomsComboBox->insertItem(rm->name);
	}
}

void AddConstraintTeacherHomeRoomForm::addConstraint()
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

	QString teacher=teachersComboBox->currentText();
	assert(gt.rules.searchTeacher(teacher)>=0);

	int i=roomsComboBox->currentItem();
	if(i<0 || roomsComboBox->count()<=0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid room"));
		return;
	}
	QString room=roomsComboBox->currentText();

	ctr=new ConstraintTeacherHomeRoom(weight, teacher, room);

	bool tmp2=gt.rules.addSpaceConstraint(ctr);
	if(tmp2){
		QString s=tr("Constraint added:");
		s+="\n\n";
		s+=ctr->getDetailedDescription(gt.rules);
		LongTextMessageBox::information(this, tr("FET information"), s);
	}
	else{
		QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - error ?"));
		delete ctr;
	}
}
