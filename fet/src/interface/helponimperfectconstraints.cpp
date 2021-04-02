/***************************************************************************
                          helponimperfectconstraints.cpp  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "helponimperfectconstraints.h"

#include "longtextmessagebox.h"

#include <QApplication>

void HelpOnImperfectConstraints::help(QWidget* parent, bool activityTagMinHoursDaily)
{
	QString s=tr("Important information about imperfect constraints:");
	
	s+="\n\n";
	
	s+=tr("The imperfect constraints are 10: students (set) max gaps per day and teacher(s) or students (set) activity tag max/min hours daily.");

	s+="\n\n";
	
	s+=tr("You are advised to leave the imperfect constraints as a last resort if you really need them. Generate the timetable with all other constraints"
	 " and only after that you may try them. The imperfect constraints may make your timetable too hard to find or even impossible, in some conditions.");
	
	s+="\n\n";
	
	s+=tr("Notation: ATS = affected teacher or students set - the teacher or students specified in an imperfect constraint.");

	s+="\n\n";
	
	s+=tr("Max gaps per day: it is not fully optimized for use with other constraints for students: min/max hours daily. It is also not tested thoroughly.");
	s+=" ";
	s+=tr("If your students need 0 maximum total gaps, do not use max gaps per day constraint, better use the safe and tested max gaps per week (with 0 gaps allowed).");
	
	s+="\n\n";
	
	s+=tr("Activity tag max/min hours daily: it is not fully optimized for this situation: if the ATS have max gaps constraints and the number of total available slots per week"
	 " for the ATS is much larger than the working hours per week and the ATS has many activities with the specified activity tag (more than 0.5-0.75 of total hours)"
	 ". In such cases, a remedy may be to add constraint ATS max/min hours daily (without specifying an activity tag).", "ATS means affected teacher or students set.");
	
	s+="\n\n";
	s+=tr("Read FAQ question 1/25 September 2009 for some more details about activity tag max/min hours daily constraint.");

	if(activityTagMinHoursDaily){
		s+="\n\n";
		s+=tr("IMPORTANT NOTE: If you need a solution without empty days or if a solution with empty days does not exist, please leave the option 'Allow"
		 " empty days' unselected, because the generation is more efficient this way.");
	}
	
	LongTextMessageBox::largeInformation(parent, tr("FET information"), s);
}
