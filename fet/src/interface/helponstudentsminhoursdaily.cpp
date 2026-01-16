/***************************************************************************
                          helponstudentsminhoursdaily.cpp  -  description
                             -------------------
    begin                : 2010
    copyright            : (C) 2010 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "helponstudentsminhoursdaily.h"

#include "longtextmessagebox.h"

#include "rules.h"
#include "timetable.h"

#include <QApplication>

extern Timetable gt;

void HelpOnStudentsMinHoursDaily::help(QWidget* parent)
{
	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		QString s=tr("Important information about the constraints of type students (set) min hours daily:");
	
		s+="\n\n";
	
		s+=tr("Be very careful: if your school does not allow empty days for students or if a timetable does not exist with empty days for students, you MUST"
			" keep the constraints the usual way, without allowing empty days.");
		s+=" ";
		s+=tr("The reason is performance: speed of generation and the ability of FET to find a solution.");
		s+=" ";
		s+=tr("If you allow empty days to these constraints and a solution exists only with non-empty days, FET might not be able to find it.");
		s+="\n\n";
		s+=tr("For normal schools or high-schools, probably you won't need to enable this option. Maybe for universities you will need to enable this option.");
		s+="\n\n";
		s+=tr("So, remember: don't allow empty days unless you need it.");
	
		LongTextMessageBox::largeInformation(parent, tr("FET information"), s);
	}
	else{
		QString s=tr("Important information about the constraints of type students (set) min hours daily:");
	
		s+="\n\n";
		
		s+=tr("If you select not allowed empty days, it means not allowed empty real days. Empty days (half days,"
		 " FET days) are allowed. Each day (half day, FET day) will have either zero hours, or the minimum selected hours daily.");
	
		LongTextMessageBox::largeInformation(parent, tr("FET information"), s);
	}
}
