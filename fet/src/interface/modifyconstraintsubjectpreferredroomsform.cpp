/***************************************************************************
                          modifyconstraintsubjectpreferredroomsform.cpp  -  description
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

#include "modifyconstraintsubjectpreferredroomsform.h"
#include "spaceconstraint.h"

ModifyConstraintSubjectPreferredRoomsForm::ModifyConstraintSubjectPreferredRoomsForm(ConstraintSubjectPreferredRooms* ctr)
{
    setupUi(this);

//    connect(addPushButton, SIGNAL(clicked()), this /*ModifyConstraintSubjectPreferredRoomsForm_template*/, SLOT(addRoom()));
//    connect(removePushButton, SIGNAL(clicked()), this /*ModifyConstraintSubjectPreferredRoomsForm_template*/, SLOT(removeRoom()));
    connect(cancelPushButton, SIGNAL(clicked()), this /*ModifyConstraintSubjectPreferredRoomsForm_template*/, SLOT(cancel()));
    connect(okPushButton, SIGNAL(clicked()), this /*ModifyConstraintSubjectPreferredRoomsForm_template*/, SLOT(ok()));
    connect(roomsListBox, SIGNAL(selected(QString)), this /*ModifyConstraintSubjectPreferredRoomsForm_template*/, SLOT(addRoom()));
    connect(selectedRoomsListBox, SIGNAL(selected(QString)), this /*ModifyConstraintSubjectPreferredRoomsForm_template*/, SLOT(removeRoom()));

    connect(clearPushButton, SIGNAL(clicked()), this, SLOT(clear()));

	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);

	QSize tmp3=subjectsComboBox->minimumSizeHint();
	Q_UNUSED(tmp3);
	
	updateRoomsListBox();
	
	int i=0, j=-1;
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
	subjectsComboBox->setCurrentItem(j);
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(QString::number(ctr->weightPercentage));
	//compulsoryCheckBox->setChecked(ctr->compulsory);
	
	for(QStringList::Iterator it=ctr->roomsNames.begin(); it!=ctr->roomsNames.end(); it++)
		selectedRoomsListBox->insertItem(*it);
}

ModifyConstraintSubjectPreferredRoomsForm::~ModifyConstraintSubjectPreferredRoomsForm()
{
}

void ModifyConstraintSubjectPreferredRoomsForm::updateRoomsListBox()
{
	roomsListBox->clear();
	selectedRoomsListBox->clear();

	for(int i=0; i<gt.rules.roomsList.size(); i++){
		Room* rm=gt.rules.roomsList[i];
		roomsListBox->insertItem(rm->name);
	}
}

void ModifyConstraintSubjectPreferredRoomsForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight"));
		return;
	}

/*	bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;*/

	if(selectedRoomsListBox->count()==0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Empty list of selected rooms"));
		return;
	}
	if(selectedRoomsListBox->count()==1){
		QMessageBox::warning(this, tr("FET information"),
			tr("Only one selected room - please use constraint subject preferred room if you want a single room"));
		return;
	}
	
	if(subjectsComboBox->currentItem()<0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid selected subject"));
		return;
	}
	QString subject=subjectsComboBox->currentText();
	
	QStringList roomsList;
	for(uint i=0; i<selectedRoomsListBox->count(); i++)
		roomsList.append(selectedRoomsListBox->text(i));
	
	this->_ctr->weightPercentage=weight;
//	this->_ctr->compulsory=compulsory;
	this->_ctr->subjectName=subject;
	this->_ctr->roomsNames=roomsList;
	
	gt.rules.internalStructureComputed=false;
	
	this->close();
}

void ModifyConstraintSubjectPreferredRoomsForm::cancel()
{
	this->close();
}

void ModifyConstraintSubjectPreferredRoomsForm::addRoom()
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

void ModifyConstraintSubjectPreferredRoomsForm::removeRoom()
{
	if(selectedRoomsListBox->currentItem()<0 || selectedRoomsListBox->count()<=0)
		return;		
	selectedRoomsListBox->removeItem(selectedRoomsListBox->currentItem());
}

void ModifyConstraintSubjectPreferredRoomsForm::clear()
{
	selectedRoomsListBox->clear();
}
