/***************************************************************************
                          addconstraintsubjectsubjecttagrequireequipmentsform.cpp  -  description
                             -------------------
    begin                : Feb 22, 2005
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

#include "addconstraintsubjectsubjecttagrequireequipmentsform.h"
#include "spaceconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <q3table.h>

#include <QDesktopWidget>

AddConstraintSubjectSubjectTagRequireEquipmentsForm::AddConstraintSubjectSubjectTagRequireEquipmentsForm()
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	updateSubjectsComboBox();
	updateSubjectTagsComboBox();
	updateEquipmentsListBox();
	requiredEquipmentsListBox->clear();
}

AddConstraintSubjectSubjectTagRequireEquipmentsForm::~AddConstraintSubjectSubjectTagRequireEquipmentsForm()
{
}

void AddConstraintSubjectSubjectTagRequireEquipmentsForm::updateSubjectsComboBox()
{
	subjectsComboBox->clear();
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* sb=gt.rules.subjectsList[i];
		subjectsComboBox->insertItem(sb->name);
	}
}

void AddConstraintSubjectSubjectTagRequireEquipmentsForm::updateSubjectTagsComboBox()
{
	subjectTagsComboBox->clear();
	for(int i=0; i<gt.rules.subjectTagsList.size(); i++){
		SubjectTag* st=gt.rules.subjectTagsList[i];
		subjectTagsComboBox->insertItem(st->name);
	}
}

void AddConstraintSubjectSubjectTagRequireEquipmentsForm::updateEquipmentsListBox()
{
	allEquipmentsListBox->clear();
	for(int i=0; i<gt.rules.equipmentsList.size(); i++){
		Equipment* eq=gt.rules.equipmentsList[i];
		allEquipmentsListBox->insertItem(eq->name);
	}
}

void AddConstraintSubjectSubjectTagRequireEquipmentsForm::addConstraint()
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

	int i=subjectsComboBox->currentItem();
	if(i<0 || subjectsComboBox->count()<=0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid subject"));
		return;
	}
	QString subject=subjectsComboBox->currentText();

	i=subjectTagsComboBox->currentItem();
	if(i<0 || subjectTagsComboBox->count()<=0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid subject tag"));
		return;
	}
	QString subjectTag=subjectTagsComboBox->currentText();

	if(requiredEquipmentsListBox->count()==0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Empty list of required equipments"));
		return;
	}

	ctr=new ConstraintSubjectSubjectTagRequireEquipments(weight, compulsory, subject, subjectTag);

	for(uint i=0; i<requiredEquipmentsListBox->count(); i++)
		((ConstraintSubjectSubjectTagRequireEquipments*)ctr)->addRequiredEquipment(requiredEquipmentsListBox->text(i));

	bool tmp2=gt.rules.addSpaceConstraint(ctr);
	if(tmp2){
		QString s=QObject::tr("Constraint added:");
		s+="\n";
		s+=ctr->getDetailedDescription(gt.rules);
		QMessageBox::information(this, QObject::tr("FET information"), s);
	}
	else{
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Constraint NOT added - error ?"));
		delete ctr;
	}
}

void AddConstraintSubjectSubjectTagRequireEquipmentsForm::addEquipment()
{
	if(allEquipmentsListBox->currentItem()<0)
		return;
	QString equip=allEquipmentsListBox->currentText();
	assert(equip!="");
	uint i;
	//duplicate?
	for(i=0; i<requiredEquipmentsListBox->count(); i++)
		if(equip==requiredEquipmentsListBox->text(i))
			break;
	if(i<requiredEquipmentsListBox->count())
		return;
	requiredEquipmentsListBox->insertItem(equip);
}

void AddConstraintSubjectSubjectTagRequireEquipmentsForm::removeEquipment()
{
	if(requiredEquipmentsListBox->currentItem()<0 || requiredEquipmentsListBox->count()<=0)
		return;
	requiredEquipmentsListBox->removeItem(requiredEquipmentsListBox->currentItem());
}
