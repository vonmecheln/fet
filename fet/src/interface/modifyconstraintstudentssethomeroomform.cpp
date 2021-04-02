/***************************************************************************
                          modifyconstraintstudentssethomeroomform.cpp  -  description
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

#include "modifyconstraintstudentssethomeroomform.h"
#include "spaceconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <q3table.h>

#include <QDesktopWidget>

ModifyConstraintStudentsSetHomeRoomForm::ModifyConstraintStudentsSetHomeRoomForm(ConstraintStudentsSetHomeRoom* ctr)
{
    setupUi(this);

    connect(cancelPushButton, SIGNAL(clicked()), this /*ModifyConstraintStudentsSetHomeRoomForm_template*/, SLOT(cancel()));
    connect(okPushButton, SIGNAL(clicked()), this /*ModifyConstraintStudentsSetHomeRoomForm_template*/, SLOT(ok()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);

	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);

	QSize tmp5=roomsComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	
	this->_ctr=ctr;
	
	//compulsoryCheckBox->setChecked(ctr->compulsory);
	weightLineEdit->setText(QString::number(ctr->weightPercentage));

	updateStudentsComboBox();
	updateRoomsComboBox();
}

ModifyConstraintStudentsSetHomeRoomForm::~ModifyConstraintStudentsSetHomeRoomForm()
{
}

void ModifyConstraintStudentsSetHomeRoomForm::updateStudentsComboBox(){
	int i=0, j=-1;
	studentsComboBox->clear();
	for(int m=0; m<gt.rules.yearsList.size(); m++){
		StudentsYear* sty=gt.rules.yearsList[m];
		studentsComboBox->insertItem(sty->name);
		if(sty->name==this->_ctr->studentsName)
			j=i;
		i++;
		for(int n=0; n<sty->groupsList.size(); n++){
			StudentsGroup* stg=sty->groupsList[n];
			studentsComboBox->insertItem(stg->name);
			if(stg->name==this->_ctr->studentsName)
				j=i;
			i++;
			for(int p=0; p<stg->subgroupsList.size(); p++){
				StudentsSubgroup* sts=stg->subgroupsList[p];
				studentsComboBox->insertItem(sts->name);
				if(sts->name==this->_ctr->studentsName)
					j=i;
				i++;
			}
		}
	}
	assert(j>=0);
	studentsComboBox->setCurrentItem(j);
}

void ModifyConstraintStudentsSetHomeRoomForm::updateRoomsComboBox()
{
	int i=0, j=-1;
	roomsComboBox->clear();
	for(int k=0; k<gt.rules.roomsList.size(); k++){
		Room* rm=gt.rules.roomsList[k];
		roomsComboBox->insertItem(rm->name);
		if(rm->name==this->_ctr->roomName)
			j=i;
		i++;
	}
	assert(j>=0);
	roomsComboBox->setCurrentItem(j);
}

void ModifyConstraintStudentsSetHomeRoomForm::cancel()
{
	this->close();
}

void ModifyConstraintStudentsSetHomeRoomForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight"));
		return;
	}

	QString students=studentsComboBox->currentText();
	assert(gt.rules.searchStudentsSet(students)!=NULL);

	int i=roomsComboBox->currentItem();
	if(i<0 || roomsComboBox->count()<=0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid room"));
		return;
	}
	QString room=roomsComboBox->currentText();

	this->_ctr->weightPercentage=weight;
	this->_ctr->roomName=room;
	this->_ctr->studentsName=students;

	gt.rules.internalStructureComputed=false;
	
	this->close();
}
