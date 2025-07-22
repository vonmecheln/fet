/***************************************************************************
                          helpmoroccoform.cpp  -  description
                             -------------------
    begin                : August 1, 2011
    copyright            : (C) 2011 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "helpmoroccoform.h"

#include "timetable_defs.h"

#include <QCoreApplication>

HelpMoroccoForm::HelpMoroccoForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	connect(closePushButton, &QPushButton::clicked, this, &HelpMoroccoForm::close);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QString s=QString("");
	
	s+=tr("This help by %1 and %2, last modified on %3.", "%1 and %2 are two persons, %3 is a date").arg("Chafik Graiguer").arg("Liviu Lalescu").arg(tr("2 July 2025"));
	s+="\n\n";
	s+=tr("FET mornings-afternoons with exclusive mornings/afternoons for teachers was designed for institutions in Morocco, but might be also used in other"
	 " institutions working in two shifts, where teachers can work either in the morning or in the evening, but not both on the same day (with exceptions).");
	s+="\n\n";
	s+=QString("--------------------------------------");
	s+="\n\n";
	s+=tr("This is a version specially made for Preparatory and Secondary schools in Morocco; in other words,"
	 " it is suitable to 'colleges' and 'lyceums' in Morocco. It was requested by the user %1.").arg("Chafik Graiguer");
	s+="\n\n";
	s+=tr("These schools have a morning shift and an afternoon shift.");
	s+="\n\n";
	s+=QString("--------------------------------------");
	s+="\n\n";
	s+=tr("These are the words of user %1:").arg("Chafik Graiguer");
	s+="\n\n";
	s+=tr("This FET version fulfills the following requirements:");
	s+="\n\n";
	s+=tr("1 - Definition of a working school day:");
	s+="\n\n";
	s+="\t";
	s+=tr("A day is divided into two distinct periods:");
	s+="\n\n";
	s+="\t";
	s+=tr("- morning 08:00 - 12:00");
	s+="\n";
	s+="\t";
	s+=tr("- afternoon 14:00 - 18:00");
	s+="\n";
	s+="\t";
	s+=tr("- there is a lunch break 12:00 - 14:00");
	s+="\n\n";
	s+=tr("2 - Studying periods and gaps per day:");
	s+="\n\n";
	s+="\t";
	s+=tr("- Students can have gaps around lunch break, i.e. before or after lunch break (official FET version cannot tolerate this !!!)");
	s+="\n";
	s+="\t";
	s+=tr("- Students and teachers must have at least 2 hours per period (Empty periods are OK.)");
	s+="\n";
	s+="\t";
	s+=tr("- Teachers can only have activities either in the morning or in the afternoon. Never both.");
	s+="\n\n";
	s+=tr("3 - The key hint to use this version");
	s+="\n\n";
	s+=tr("We have 6 REAL days, with 8 working time slots");
	s+="\n\n";
	s+=tr("We should input 6x2 = 12 days, with 4 working time slots ONLY");
	s+="\n\n";
	s+=QString("--------------------------------------");
	s+="\n\n";
	s+=tr("Of course you can input less or more hours in each day and less or more days per week, as you need.");
	s+="\n\n";
	s+=tr("Features:");
	s+="\n\n";
	s+=tr("Intelligent min hours daily for students (can have days with 0 hours).");
	s+="\n\n";
	s+=tr("User must input an even number of days per week. The first FET day is morning real day 1, the second"
	 " FET day is afternoon real day 1, and so on (FET days are double than real days).");
	s+="\n\n";
	s+=tr("A teacher can have hours in first day or second day, but not both. Same for third and fourth, fifth and sixth, and so on."
	 " (in fact, there are 2*normal days, first is morning, second is afternoon).");
	s+="\n\n";
	s+=tr("Exceptions for teachers: they allow for some teachers to work in double morning+afternoon for a single day (1 exception)"
	 " or for 2 days (2 exception).");
	s+=" ";
	s+=tr("(Recently, it is also possible to allow 3, 4, or 5 days exceptions.)");
	s+="\n\n";
	s+=tr("Very important advice about the constraints of type min days between activities: probably you will need to"
	 " add min days = 1 for all the constraints. I modified the source code and min 1 day means"
	 " that the activities must be on different real days, so it cannot be that one activity"
	 " is in the morning and another is in the afternoon. If you need the constraint to be always respected,"
	 " please use 100% weight. If you allow weight under 100% and select consecutive if on the same day,"
	 " then the activities must be either in the morning or exclusively in the afternoon.");
	s+="\n\n";
	s+=tr("Min 1 day means that the activities cannot be in same REAL day (so they can be"
	 " on Monday afternoon and Tuesday morning, but not both on Monday).");
	s+="\n\n";
	s+=tr("Min 2 days means that the activities must be 2 REAL days apart (so they"
	 " can be on Monday afternoon and Wednesday morning, but not on Monday and Tuesday).");
	s+="\n\n";
	s+=tr("If your data is too difficult (impossible), maybe you can deactivate consecutive if on the same day for"
	 " all constraints min days between activities (from the modify multiple constraints at once dialog,"
	 " which can be opened from the constraints min days between activities dialog).");
	s+="\n\n";
	s+=tr("Constraint max days between activities considers real days.");
	s+="\n\n";
	s+=tr("The constraint students (set) min hours daily without allowing empty days is for real days, and for nonempty FET days (half days) "
	 "it respects the minimum number of hours daily.");

	s+="\n\n";

	s+=QCoreApplication::translate("HelpForMorningsAfternoons", "This question and answer by %1 and %2:", "%1 and %2 are two persons").arg("bachiri401").arg("Liviu Lalescu");
	s+="\n\n";
	s+=QCoreApplication::translate("HelpForMorningsAfternoons", "%1: In Algeria, we can not teach the same subject in the same real day. For example a subject divided into 3 activities (1+1+1)"
	 " must be on three different real days. The constraint min days between activities here is great (working for real days).", "%1 is the name of the person writing this paragraph").arg("bachiri401");
	s+="\n\n";
	s+=QCoreApplication::translate("HelpForMorningsAfternoons", "But for example we have students studying math and sport. We want math and sport not to be on the same half day but they can be on the same real day.");
	s+="\n\n";
	s+=QCoreApplication::translate("HelpForMorningsAfternoons", "%1 also said that for this problem he uses a trick, adding a constraint min gaps between the activities.",
	 "%1 is the name of a person").arg("bachiri401");
	s+="\n\n";
	s+=QCoreApplication::translate("HelpForMorningsAfternoons", "And indeed, %1 says this: I checked the code. There is no need for a new constraint. Please use a constraint min gaps between activities"
	 " with min gaps = the number of hours per half day (the maximum FET allows). Add all the math and sport activities for a students set for your example. It is implemented"
	 " efficiently for this case.", "%1 is the name of a person").arg("Liviu Lalescu");
	s+="\n\n";
	s+=QCoreApplication::translate("HelpForMorningsAfternoons", "Note: in FET version %1, suggested by %2, it was added a new type of constraint probably useful in this case:"
	 " min half days between activities, so that the trick above is now no longer necessary.", "%1 is the FET version number, %2 is a person").arg("6.4.0").arg("ngoctp29121982");

	textBrowser->setText(s);
}

HelpMoroccoForm::~HelpMoroccoForm()
{
	saveFETDialogGeometry(this);
}
