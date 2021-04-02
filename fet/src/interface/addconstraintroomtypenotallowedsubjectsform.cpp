/***************************************************************************
                          addconstraintroomtypenotallowedsubjectsform.cpp  -  description
                             -------------------
    begin                : Fri Jan 9 2004
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

#include "addconstraintroomtypenotallowedsubjectsform.h"
#include "spaceconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <q3table.h>

#include <QDesktopWidget>

AddConstraintRoomTypeNotAllowedSubjectsForm::AddConstraintRoomTypeNotAllowedSubjectsForm()
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);	

	updateTypesComboBox();
	updateSubjectsListBox();
	notAllowedSubjectsListBox->clear();
}

AddConstraintRoomTypeNotAllowedSubjectsForm::~AddConstraintRoomTypeNotAllowedSubjectsForm()
{
}

void AddConstraintRoomTypeNotAllowedSubjectsForm::updateTypesComboBox()
{
	typesComboBox->clear();
	for(int i=0; i<gt.rules.roomsList.size(); i++){
		Room* room=gt.rules.roomsList[i];
		//insert without duplicates
		int i;
		for(i=0; i<typesComboBox->count(); i++)
			if(typesComboBox->text(i)==room->type)
				break;
		if(i==typesComboBox->count())
			typesComboBox->insertItem(room->type);
	}
}

void AddConstraintRoomTypeNotAllowedSubjectsForm::updateSubjectsListBox()
{
	subjectsListBox->clear();
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* sb=gt.rules.subjectsList[i];
		subjectsListBox->insertItem(sb->name);
	}
}

void AddConstraintRoomTypeNotAllowedSubjectsForm::addConstraint()
{
	SpaceConstraint *ctr=NULL;

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

	ctr=new ConstraintRoomTypeNotAllowedSubjects(weight, compulsory, room_type);

	for(uint i=0; i<notAllowedSubjectsListBox->count(); i++)
		((ConstraintRoomTypeNotAllowedSubjects*)ctr)->addNotAllowedSubject(notAllowedSubjectsListBox->text(i));

	bool tmp2=gt.rules.addSpaceConstraint(ctr);
	if(tmp2){
		QString s=QObject::tr("Constraint added:");
		s+="\n";
		s+=ctr->getDetailedDescription(gt.rules);
		QMessageBox::information(this, QObject::tr("FET information"), s);
	}
	else{
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Constraint NOT added - duplicate type?"));
		delete ctr;
	}
}

void AddConstraintRoomTypeNotAllowedSubjectsForm::addSubject()
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

void AddConstraintRoomTypeNotAllowedSubjectsForm::removeSubject()
{
	if(notAllowedSubjectsListBox->currentItem()<0 || notAllowedSubjectsListBox->count()<=0)
		return;
	notAllowedSubjectsListBox->removeItem(notAllowedSubjectsListBox->currentItem());
}
