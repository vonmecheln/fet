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

#include "activityplanningconfirmationform.h"

#include "timetable_defs.h"

ActivityPlanningConfirmationForm::ActivityPlanningConfirmationForm()
{
	setupUi(this);

	centerWidgetOnScreen(this);
	
	QString s;
	
	s+=tr("Please read carefully the description below:");
	s+="\n\n";
	s+=tr("This function is new and not thoroughly tested. Please report any problems.");
	s+=" ";
	s+=tr("It might be a good idea to backup your current data file before proceeding.");
	s+="\n\n";
	s+=tr("This is a simple activity planning dialog. You need to add all teachers, "
		"subjects and the students structure before you can work with it. It "
		"is also recommended to add the necessary activity tags before using this dialog.");
	s+="\n\n";
	s+=tr("The main work will be done in the upper table with the students-subjects "
		"matrix. Select an 'action' from the right side and activate a table cell to do that "
		"action (activate with the mouse double-click or click, depending on your platform, or by pressing Enter). "
		"The number of hours is summed in the heading, so "
		"you can always check if your planning fits your needs.");
	s+="\n\n";
	s+=tr("You can also double click the tables heading to do an action to the whole row/column.");
	s+="\n\n";
	s+=tr("You can change size of the tables by using the splitter between the tables.");
	s+="\n\n";
	s+=tr("Pseudo activities are (sometimes) difficult to see in the tables, so "
		"you can view them quickly with the corresponding push button.");
	s+="\n\n";
	s+=tr("You have also a facility to hide/show the buttons on the right. This is useful "
		"if you need more space for the tables. With the buttons shown (default), there is available a "
		"'Hide buttons' command. When you press it, all the other buttons will be hidden and you will have only "
		"one small button remaining. Press it and all the buttons will be shown again.");
	s+=" ";
	s+=tr("If the buttons are hidden, when you will double-click or click the mouse on the tables or press Enter "
		"on a table cell, the command that will be done is the one which was selected when the buttons were not hidden.");
	
	textBrowser->setText(s);
}

ActivityPlanningConfirmationForm::~ActivityPlanningConfirmationForm()
{

}