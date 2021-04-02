/***************************************************************************
                          modifyconstraintsubjectactivitytagpreferredroomform.cpp  -  description
                             -------------------
    begin                : 18 Aug 2007
    copyright            : (C) 2007 by Liviu Lalescu
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

#include "modifyconstraintsubjectactivitytagpreferredroomform.h"
#include "spaceconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <q3table.h>

#include <QDesktopWidget>

ModifyConstraintSubjectActivityTagPreferredRoomForm::ModifyConstraintSubjectActivityTagPreferredRoomForm(ConstraintSubjectActivityTagPreferredRoom* ctr)
{
    setupUi(this);

    connect(cancelPushButton, SIGNAL(clicked()), this /*ModifyConstraintSubjectActivityTagPreferredRoomForm_template*/, SLOT(cancel()));
    connect(okPushButton, SIGNAL(clicked()), this /*ModifyConstraintSubjectActivityTagPreferredRoomForm_template*/, SLOT(ok()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);

	QSize tmp3=subjectsComboBox->minimumSizeHint();
	Q_UNUSED(tmp3);
	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);

	QSize tmp5=roomsComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	
	this->_ctr=ctr;
	
	//compulsoryCheckBox->setChecked(ctr->compulsory);
	weightLineEdit->setText(QString::number(ctr->weightPercentage));

	updateSubjectsComboBox();
	updateActivityTagsComboBox();
	updateRoomsComboBox();
}

ModifyConstraintSubjectActivityTagPreferredRoomForm::~ModifyConstraintSubjectActivityTagPreferredRoomForm()
{
}

void ModifyConstraintSubjectActivityTagPreferredRoomForm::updateSubjectsComboBox()
{
	int i=0, j=-1;
	subjectsComboBox->clear();
	for(int k=0; k<gt.rules.subjectsList.size(); k++){
		Subject* sb=gt.rules.subjectsList[k];
		subjectsComboBox->insertItem(sb->name);
		if(sb->name==this->_ctr->subjectName)
			j=i;
		i++;
	}
	assert(j>=0);
	subjectsComboBox->setCurrentItem(j);
}

void ModifyConstraintSubjectActivityTagPreferredRoomForm::updateActivityTagsComboBox()
{
	int i=0, j=-1;
	activityTagsComboBox->clear();
	for(int k=0; k<gt.rules.activityTagsList.size(); k++){
		ActivityTag* sb=gt.rules.activityTagsList[k];
		activityTagsComboBox->insertItem(sb->name);
		if(sb->name==this->_ctr->activityTagName)
			j=i;
		i++;
	}
	assert(j>=0);
	activityTagsComboBox->setCurrentItem(j);
}

void ModifyConstraintSubjectActivityTagPreferredRoomForm::updateRoomsComboBox()
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

void ModifyConstraintSubjectActivityTagPreferredRoomForm::cancel()
{
	this->close();
}

void ModifyConstraintSubjectActivityTagPreferredRoomForm::ok()
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

	int i=subjectsComboBox->currentItem();
	if(i<0 || subjectsComboBox->count()<=0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid subject"));
		return;
	}
	QString subject=subjectsComboBox->currentText();

	i=activityTagsComboBox->currentItem();
	if(i<0 || activityTagsComboBox->count()<=0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid activity tag"));
		return;
	}
	QString activityTag=activityTagsComboBox->currentText();

	i=roomsComboBox->currentItem();
	if(i<0 || roomsComboBox->count()<=0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid room"));
		return;
	}
	QString room=roomsComboBox->currentText();

	this->_ctr->weightPercentage=weight;
	//this->_ctr->compulsory=compulsory;
	this->_ctr->roomName=room;
	this->_ctr->subjectName=subject;
	this->_ctr->activityTagName=activityTag;

	gt.rules.internalStructureComputed=false;
	
	this->close();
}
