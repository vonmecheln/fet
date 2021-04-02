//
//
// C++ Implementation: $MODULE$
//
// Description: 
//
//
// Author: Lalescu Liviu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "timetableshowconflictsspaceform.h"

#include "genetictimetable.h"

#include <q3textedit.h>
#include <qstring.h>

#include <QDesktopWidget>

extern GeneticTimetable gt;
extern QString spaceConflictsString;

TimetableShowConflictsSpaceForm::TimetableShowConflictsSpaceForm()
 : TimetableShowConflictsSpaceForm_template()
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	conflictsTextEdit->setText(spaceConflictsString);
}


TimetableShowConflictsSpaceForm::~TimetableShowConflictsSpaceForm()
{
}
