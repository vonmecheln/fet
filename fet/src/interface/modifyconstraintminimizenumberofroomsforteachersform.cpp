/***************************************************************************
                          modifyconstraintminimizenumberofroomsforteachersform.cpp  -  description
                             -------------------
    begin                : 12 July 2005
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

#include "modifyconstraintminimizenumberofroomsforteachersform.h"
#include "spaceconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <q3table.h>

#include <QDesktopWidget>

ModifyConstraintMinimizeNumberOfRoomsForTeachersForm::ModifyConstraintMinimizeNumberOfRoomsForTeachersForm(ConstraintMinimizeNumberOfRoomsForTeachers* ctr)
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
}

ModifyConstraintMinimizeNumberOfRoomsForTeachersForm::~ModifyConstraintMinimizeNumberOfRoomsForTeachersForm()
{
}

void ModifyConstraintMinimizeNumberOfRoomsForTeachersForm::cancel()
{
	this->close();
}

void ModifyConstraintMinimizeNumberOfRoomsForTeachersForm::ok()
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

	this->_ctr->weight=weight;
	this->_ctr->compulsory=compulsory;

	gt.rules.internalStructureComputed=false;
	
	this->close();
}
