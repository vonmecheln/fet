//
//
// Description: This file is part of FET
//
//
// Author: Lalescu Liviu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
// Copyright (C) 2003 Liviu Lalescu <http://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
//
//

#include "timetableshowconflictsform.h"

#include "timetable.h"

#include <QString>

extern Timetable gt;
extern QString conflictsStringTitle;
extern QString conflictsString;

TimetableShowConflictsForm::TimetableShowConflictsForm()
 : TimetableShowConflictsForm_template()
{
    setupUi(this);

    connect(closePushButton, SIGNAL(clicked()), this /*TimetableShowConflictsForm_template*/, SLOT(close()));


	//setWindowFlags(Qt::Window);
	//setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
/*	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	setWindowTitle(conflictsStringTitle);
	conflictsTextEdit->setText(conflictsString);
}


TimetableShowConflictsForm::~TimetableShowConflictsForm()
{
}
