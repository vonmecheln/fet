/***************************************************************************
                          helptermsform.cpp  -  description
                             -------------------
    begin                : 2021
    copyright            : (C) 2021 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "helptermsform.h"

#include "timetable_defs.h"

HelpTermsForm::HelpTermsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	closePushButton->setDefault(true);
	
	plainTextEdit->setReadOnly(true);

	connect(closePushButton, &QPushButton::clicked, this, &HelpTermsForm::close);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	setText();
}

HelpTermsForm::~HelpTermsForm()
{
	saveFETDialogGeometry(this);
}

void HelpTermsForm::setText()
{
	QString s;
	
	s+=tr("Instructions for the terms mode (Finland).");
	s+="\n\n";
	s+=tr("Last modified on %1.", "%1 is a date").arg(tr("2 July 2025"));
	s+="\n\n";
	s+=tr("The terms mode was suggested, chronologically, by these users: %1 and %2. You can follow a forum discussion about it here: %3",
	 "%1 and %2 are two persons, %3 is an internet link").arg("mathmake").arg("mikkojoo")
	 .arg("https://lalescu.ro/liviu/fet/forum/index.php?topic=4387.0");
	s+="\n\n";
	s+=tr("The main ideas:");
	s+="\n\n";
	s+=tr("There are nTerms (say 5) terms, and nDaysPerTerm (say 5) days each. So there are nTerms x nDaysPerTerm (say 5 x 5 = 25) FET days.");
	s+=" ";
	s+=tr("The terms are in order, one after another, and one day after another.");
	s+="\n\n";
	s+=tr("In this mode you can use some additional constraints referring to terms.");

	plainTextEdit->setPlainText(s);
}
