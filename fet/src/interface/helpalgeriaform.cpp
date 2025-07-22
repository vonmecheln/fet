/***************************************************************************
                          helpalgeriaform.cpp  -  description
                             -------------------
    begin                : September 2, 2011
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

#include "helpalgeriaform.h"

#include "timetable_defs.h"

#include <QCoreApplication>

HelpAlgeriaForm::HelpAlgeriaForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	connect(closePushButton, &QPushButton::clicked, this, &HelpAlgeriaForm::close);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QString s=QString("");
	
	s+=tr("This help by %1, last modified on %2.", "%1 is a person").arg("Liviu Lalescu").arg(tr("2 July 2025"));
	s+="\n\n";

	s+=tr("FET mornings-afternoons with unrestricted mornings/afternoons for teachers was originally designed for Algerian schools (as requested by the user %1)"
	 ", but it can be used in other institutions working in two shifts where teachers can work both in the morning and in the evening on the same day.").arg("aissa");

	s+="\n\n";

	s+=tr("Please define days in FET to be double of the real days, that is if your week has "
	 "5 days, define 10 FET days. First day is for morning, second day is for afternoon, and so on.");

	s+="\n\n";

	s+=tr("Very important advice about the constraint min days between activities: probably you will need to "
	 "add min days = 1 for all constraints. I modified the source code and min 1 day means "
	 "that the activities must be on different real days, so it cannot be that one activity "
	 "is in the morning and another is in the afternoon. If you need constraint to be always respected, "
	 "please use 100% weight. If you allow weight under 100% and select consecutive if on the same day, "
	 "then activities must be either in the morning or exclusively in the afternoon.");

	s+="\n\n";

	s+=tr("Min 1 day means that the activities cannot be in same REAL day (so they can be "
	"on Monday afternoon and Tuesday morning, but not both on Monday).");

	s+="\n\n";

	s+=tr("Min 2 days means that the activities must be 2 REAL days apart (so they "
	 "can be on Monday afternoon and Wednesday morning, but not on Monday and Tuesday).");

	s+="\n\n";

	s+=tr("If your data is too difficult (impossible), maybe you can deactivate consecutive if on the same day for"
	 " all constraints min days between activities (from the modify multiple constraints at once dialog,"
	 " which can be opened from the constraints min days between activities dialog).");

	s+="\n\n";

	s+=tr("Constraint max days between activities considers real days.");
	s+="\n\n";

	s+=tr("The constraint students (set) min hours daily without allowing empty days is for real days, and for nonempty FET days (half days)"
	 " it respects the minimum number of hours daily.");
	
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

HelpAlgeriaForm::~HelpAlgeriaForm()
{
	restoreFETDialogGeometry(this);
}
