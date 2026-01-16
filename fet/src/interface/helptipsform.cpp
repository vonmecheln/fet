/***************************************************************************
                          helptipsform.cpp  -  description
                             -------------------
    begin                : Feb 20, 2005
    copyright            : (C) 2005 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "helptipsform.h"

#include "timetable_defs.h"

HelpTipsForm::HelpTipsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	closePushButton->setDefault(true);
	
	plainTextEdit->setReadOnly(true);

	connect(closePushButton, &QPushButton::clicked, this, &HelpTipsForm::close);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	setText();
}

HelpTipsForm::~HelpTipsForm()
{
	saveFETDialogGeometry(this);
}

void HelpTipsForm::setText()
{
	QString s;
	
	s+=tr("Important tips.");
	s+="\n\n";
	s+=tr("Last modified on %1.").arg(tr("19 August 2009"));
	
	s+="\n\n";
	
	s+=tr("1) Say you add a split activity or you add a constraint min days between activities (say na activities),"
	 " when your week has nd days. Do not add a constraint with na>nd (number of activities strictly greater than number of days per week).");
	s+="\n\n";
	s+=tr("The correct way: if you selected consecutive if on the same day, consider activities having a longer duration."
	 " Instead of adding activities 1+1+1+1+1+1+1 in a 5 days week, add them as 2+2+1+1+1. If you didn't select "
	 "consecutive if on the same day, add 1+1+1+1+1 firstly and 1+1 after that.");
	s+="\n\n";
	s+=tr("The generation will be much faster and easier using this approach (more than 10 times faster sometimes).");
	
	s+="\n\n";
	
	s+=tr("2) After adding constraints of type activities same starting time or same starting day or max 0 days between activities "
	 "or max 0 half days between activities (in the Mornings-Afternoons mode), if you have 'bad' redundant constraints (see below), "
	 "it is necessary to apply the advanced function of removing redundant constraints of type min days between activities (and min "
	 "half days between activities, in the Mornings-Afternoons mode). The redundant constraints of type min (half) days between "
	 "activities with weight < 100% are considered 'bad' for the generation, because they can make the timetable too difficult, "
	 "their weights being 'amplified'. These constraints will be removed by this advanced function. The redundant constraints of type min (half) "
	 "days between activities with weight 100% are considered 'good' for the generation and will not be removed by this advanced function.");
	s+="\n\n";
	s+=tr("For example, if we have these activities: A1 (at the same time with B1 and C1), A2 (at the same time with B2 and C2) and "
	 "A3 (at the same time with B3 and C3). "
	 "You will normally have 3 or 6 constraints min days between activities (the first one or two for A1,A2,A3 and then the next one or two "
	 "for B1,B2,B3 and then the next one or two for C1,C2,C3). Assume that these 3 or 6 constraints have weights < 100%. Then these 3 or 6 "
	 "constraints will result in a much stronger overall constraint, which might make the timetable too difficult to find. If you have say "
	 "4 groups of activities (A, B, C, D), the timetable will be even more difficult to find. "
	 "The correct way in this case (if the weights of the min days between activities constraints is < 100%) is to leave only the "
	 "constraints referring to the first group of activities (A). You will have a much faster generation (maybe 10 times faster or more). "
	 "But if the weights of the min days between activities constraints are equal with 100%, then it is better to keep all of them, even "
	 "if they are redundant, because in this case FET will reject earlier the incorrect placements.");
	s+="\n\n";
	s+=tr("PS: Suppose there are 2 constraints for activities A1,2,3 and 2 constraints for activities B1,2,3 and 2 "
	 "constraints for activities C1,2,3. If A and B and C are simultaneous, remove only the constraints referring to B "
	 "and C (but leave both for A, if for instance you have min 2 days with 95% and min 1 day with 100%).");
	s+="\n\n";
	s+=tr("More details: the combination of 2 or more redundant constraints min (half) days between activities with weights < 100% implies "
	 "a much stronger constraint. From 3 redundant constraints with 95%, you get one with 100%-5%*5%*5%=99.9875%, which is not what you want. "
	 "But if the weights of the min (half) days between activities constraints are 100%, this useful redundancy will help guide FET faster "
	 "towards a solution.");
	s+="\n\n";
	s+=tr("Do not consider redundant constraints with the same activities but different number of days "
	 "(you may want to add min 2 days 95% and min 1 day 95%, which gives in 95% cases the 2 days will be "
	 "respected and in 99.75% cases the 1 day will be respected).");
	s+="\n\n";
	s+=tr("The other constraints (like preferred time(s) constraints) do not have the problem of redundancy like min "
	 "days constraints, so no need to take care about them.");
	s+="\n\n";
	s+=tr("It is IMPORTANT to remove the 'bad' redundant min (half) days between activities constraints after you have input "
	 "the data, before generating (for instance, apply this after adding more constraints of type same starting time/day/max 0 (half) days between activities "
	 "and before generating). Any modification of the min (half) days between activities constraints should be followed by this removal of the 'bad' redundant "
	 "min (half) days between activities constraints (well, not all modifications, but it does not hurt to do it more often). If you modify more constraints "
	 "at once or apply the advanced function of spreading the activities over the week, it is important to remove redundant constraints. If you have "
	 "no 'bad' redundant constraints, there is no need to worry about this, but applying the advanced function of removing redundant constraints will "
	 "not hurt.");
	
	s+="\n\n";
	s+=tr("3) About the constraints of type two activities grouped, two activities consecutive and three activities grouped:"
	 " It is a bad practice to add such constraints if the involved activities are also constrained not to be on the same day"
	 " by constraints min days between activities.");
	s+="\n\n";
	s+=tr("If A1 and A2 are constrained not to be on the same day with 95% weight or any other weight, it is a bad practice "
	"to add a constraint grouped or consecutive to them. If they are constrained with weight 100% not to be on the same day, "
	"the timetable is impossible; if the weight is below 100%, the timetable is more difficult to find than using the correct way."
	" The correct way would probably be to consider A1 and A2 = a single activity A12', or to modify the related constraint "
	"min days between activities. Or maybe you can find other ways.");
	
	plainTextEdit->setPlainText(s);
}
