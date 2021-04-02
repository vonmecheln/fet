/***************************************************************************
                          modifyconstraintstudentsethomeroomform.cpp  -  description
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

#include "modifyconstraintstudentssethomeroomsform.h"
#include "spaceconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <q3table.h>

#include <QDesktopWidget>

ModifyConstraintStudentsSetHomeRoomsForm::ModifyConstraintStudentsSetHomeRoomsForm(ConstraintStudentsSetHomeRooms* ctr)
{
    setupUi(this);

//    connect(addPushButton, SIGNAL(clicked()), this /*ModifyConstraintStudentsSetHomeRoomsForm_template*/, SLOT(addRoom()));
//    connect(removePushButton, SIGNAL(clicked()), this /*ModifyConstraintStudentsSetHomeRoomsForm_template*/, SLOT(removeRoom()));
    connect(cancelPushButton, SIGNAL(clicked()), this /*ModifyConstraintStudentsSetHomeRoomsForm_template*/, SLOT(cancel()));
    connect(okPushButton, SIGNAL(clicked()), this /*ModifyConstraintStudentsSetHomeRoomsForm_template*/, SLOT(ok()));
    connect(roomsListBox, SIGNAL(selected(QString)), this /*ModifyConstraintStudentsSetHomeRoomsForm_template*/, SLOT(addRoom()));
    connect(selectedRoomsListBox, SIGNAL(selected(QString)), this /*ModifyConstraintStudentsSetHomeRoomsForm_template*/, SLOT(removeRoom()));

    connect(clearPushButton, SIGNAL(clicked()), this, SLOT(clear()));
    
	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);

	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	
	updateRoomsListBox();
	
	this->_ctr=ctr;
	
	/*int i=0, j=-1;
	for(int k=0; k<gt.rules.subjectsList.size(); k++){
		Subject* sb=gt.rules.subjectsList[k];
		subjectsComboBox->insertItem(sb->name);
		if(ctr->subjectName==sb->name){
			assert(j==-1);
			j=i;
		}
		i++;
	}
	assert(j>=0);
	subjectsComboBox->setCurrentItem(j);*/

/////////
	//students
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
/////////
	
	weightLineEdit->setText(QString::number(ctr->weightPercentage));
	//compulsoryCheckBox->setChecked(ctr->compulsory);
	
	for(QStringList::Iterator it=ctr->roomsNames.begin(); it!=ctr->roomsNames.end(); it++)
		selectedRoomsListBox->insertItem(*it);
}

ModifyConstraintStudentsSetHomeRoomsForm::~ModifyConstraintStudentsSetHomeRoomsForm()
{
}

void ModifyConstraintStudentsSetHomeRoomsForm::updateRoomsListBox()
{
	roomsListBox->clear();
	selectedRoomsListBox->clear();

	for(int i=0; i<gt.rules.roomsList.size(); i++){
		Room* rm=gt.rules.roomsList[i];
		roomsListBox->insertItem(rm->name);
	}
}

void ModifyConstraintStudentsSetHomeRoomsForm::ok()
{
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
			tr("Only one selected room - please use constraint students set home room if you want a single room"));
		return;
	}

	QString students=studentsComboBox->currentText();
	assert(gt.rules.searchStudentsSet(students)!=NULL);

	QStringList roomsList;
	for(uint i=0; i<selectedRoomsListBox->count(); i++)
		roomsList.append(selectedRoomsListBox->text(i));
	
	this->_ctr->weightPercentage=weight;

	this->_ctr->studentsName=students;

	this->_ctr->roomsNames=roomsList;
	
	gt.rules.internalStructureComputed=false;
	
	this->close();
}

void ModifyConstraintStudentsSetHomeRoomsForm::cancel()
{
	this->close();
}

void ModifyConstraintStudentsSetHomeRoomsForm::addRoom()
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

void ModifyConstraintStudentsSetHomeRoomsForm::removeRoom()
{
	if(selectedRoomsListBox->currentItem()<0 || selectedRoomsListBox->count()<=0)
		return;		
	selectedRoomsListBox->removeItem(selectedRoomsListBox->currentItem());
}

void ModifyConstraintStudentsSetHomeRoomsForm::clear()
{
	selectedRoomsListBox->clear();
}
