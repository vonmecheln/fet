/***************************************************************************
                          modifyconstraintroomtypenotallowedsubjectsform.cpp  -  description
                             -------------------
    begin                : Feb 13, 2005
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

#include "modifyconstraintroomtypenotallowedsubjectsform.h"
#include "spaceconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <q3table.h>

#include <QDesktopWidget>

ModifyConstraintRoomTypeNotAllowedSubjectsForm::ModifyConstraintRoomTypeNotAllowedSubjectsForm(ConstraintRoomTypeNotAllowedSubjects* ctr)
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	this->_ctr=ctr;
	
	compulsoryCheckBox->setChecked(ctr->compulsory);
	weightLineEdit->setText(QString::number(ctr->weight));

	updateTypesComboBox();
	updateSubjectsListBox();
	
	notAllowedSubjectsListBox->clear();
	for(QStringList::Iterator it=ctr->subjects.begin(); it!=ctr->subjects.end(); it++)
		notAllowedSubjectsListBox->insertItem(*it);
}

ModifyConstraintRoomTypeNotAllowedSubjectsForm::~ModifyConstraintRoomTypeNotAllowedSubjectsForm()
{
}

void ModifyConstraintRoomTypeNotAllowedSubjectsForm::updateTypesComboBox()
{
	int k=0, j=-1;
	typesComboBox->clear();
	for(int m=0; m<gt.rules.roomsList.size(); m++){
		Room* room=gt.rules.roomsList[m];
		//insert without duplicates
		int i;
		for(i=0; i<typesComboBox->count(); i++)
			if(typesComboBox->text(i)==room->type)
				break;
		if(i==typesComboBox->count()){
			typesComboBox->insertItem(room->type);
			if(room->type==this->_ctr->roomType)
				j=k;
			k++;
		}
	}
	typesComboBox->setCurrentItem(j);
}

void ModifyConstraintRoomTypeNotAllowedSubjectsForm::updateSubjectsListBox()
{
	subjectsListBox->clear();
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* sb=gt.rules.subjectsList[i];
		subjectsListBox->insertItem(sb->name);
	}
}

void ModifyConstraintRoomTypeNotAllowedSubjectsForm::cancel()
{
	this->close();
}

void ModifyConstraintRoomTypeNotAllowedSubjectsForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<0.0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid weight"));
		return;
	}

	bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;

	int i=typesComboBox->currentItem();
	if(i<0 || typesComboBox->count()<=0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid type"));
		return;
	}
	QString room_type=typesComboBox->currentText();

	if(notAllowedSubjectsListBox->count()==0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Empty list of not-allowed subjects"));
		return;
	}

	this->_ctr->weight=weight;
	this->_ctr->compulsory=compulsory;
	this->_ctr->roomType=room_type;
	this->_ctr->subjects.clear();
	for(uint i=0; i<notAllowedSubjectsListBox->count(); i++)
		this->_ctr->addNotAllowedSubject(notAllowedSubjectsListBox->text(i));
		
	gt.rules.internalStructureComputed=false;

	this->close();
}

void ModifyConstraintRoomTypeNotAllowedSubjectsForm::addSubject()
{
	if(subjectsListBox->currentItem()<0)
		return;
	QString subj=subjectsListBox->currentText();
	assert(subj!="");
	uint i;
	//duplicate?
	for(i=0; i<notAllowedSubjectsListBox->count(); i++)
		if(subj==notAllowedSubjectsListBox->text(i))
			break;
	if(i<notAllowedSubjectsListBox->count())
		return;
	notAllowedSubjectsListBox->insertItem(subj);
}

void ModifyConstraintRoomTypeNotAllowedSubjectsForm::removeSubject()
{
	if(notAllowedSubjectsListBox->currentItem()<0 || notAllowedSubjectsListBox->count()<=0)
		return;
	notAllowedSubjectsListBox->removeItem(notAllowedSubjectsListBox->currentItem());
}
