/***************************************************************************
                          helponimperfectconstraints.cpp  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
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
	
	s+=tr("The imperfect constraints are these: students (set) max gaps per day, students (set) / teacher(s) max gaps per real day,"
	 " teacher(s) or students (set) activity tag max/min hours daily / real days, and students (set) / teacher(s) max gaps per week for real days"
	 " (depending on the chosen mode).");

	s+="\n\n";
	
	s+=tr("You are advised to leave the imperfect constraints as a last resort if you really need them. Generate the timetable with all other constraints"
	 " and only after that you may try them. The imperfect constraints may make your timetable too hard to find or even impossible, in some conditions.");
	
	s+="\n\n";
	
	s+=tr("Notation: ATS = affected teacher or students set - the teacher or students specified in an imperfect constraint.");

	s+="\n\n";
	
	s+=tr("Students max gaps per day: it is not fully optimized for use with other constraints for students: min/max hours daily. It is also not tested thoroughly.");
	
	s+="\n\n";
	
	s+=tr("Students (set) / teacher(s) max gaps per week for real days (available depending on the mode): it is not fully optimized for use with other constraints for students/teachers:"
	 " max hours daily (for a real day). It is also not tested thoroughly. These 4 constraints were introduced on 30 July 2020. They represent the total sum of gaps allowed"
	 " over the whole week, where the gaps are computed for each complete real day (morning+afternoon).");

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
