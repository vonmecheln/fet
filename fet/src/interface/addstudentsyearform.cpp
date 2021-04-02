/***************************************************************************
                          addstudentsyearform.cpp  -  description
                             -------------------
    begin                : Sat Jan 24 2004
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

#include "addstudentsyearform.h"

#include <qlineedit.h>

#include <QDesktopWidget>

AddStudentsYearForm::AddStudentsYearForm()
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);
}

AddStudentsYearForm::~AddStudentsYearForm()
{
}

void AddStudentsYearForm::addStudentsYear()
{
	if(nameLineEdit->text().isEmpty()){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Incorrect name"));
		return;
	}
	StudentsYear* sy=new StudentsYear();
	sy->name=nameLineEdit->text();
	sy->numberOfStudents=numberSpinBox->value();

	if(gt.rules.searchYear(sy->name) >=0 ){
		QMessageBox::information( this, QObject::tr("Year insertion dialog"),
		QObject::tr("Could not insert item. Must be a duplicate"));
		delete sy;
	}
	else{
		bool tmp=gt.rules.addYear(sy);
		assert(tmp);

		QMessageBox::information(this, QObject::tr("Year insertion dialog"),
			QObject::tr("Year added"));
	}

	nameLineEdit->selectAll();
	nameLineEdit->setFocus();
}
