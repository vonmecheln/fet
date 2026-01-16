//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2021 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "timetable_defs.h"

#include "getmodefornewfileform.h"

#include "longtextmessagebox.h"

#include "generate.h"

#include "rules.h"

#include <QMessageBox>

extern const QString COMPANY;
extern const QString PROGRAM;

//extern MRG32k3a rng;
extern Generate gen;

GetModeForNewFileForm::GetModeForNewFileForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	okPushButton->setDefault(true);
	
	connect(helpPushButton, &QPushButton::clicked, this, &GetModeForNewFileForm::help);
	connect(okPushButton, &QPushButton::clicked, this, &GetModeForNewFileForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &GetModeForNewFileForm::cancel);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	officialRadioButton->setChecked(false);
	morningsAfternoonsRadioButton->setChecked(false);
	blockPlanningRadioButton->setChecked(false);
	termsRadioButton->setChecked(false);
}

GetModeForNewFileForm::~GetModeForNewFileForm()
{
	saveFETDialogGeometry(this);
}

void GetModeForNewFileForm::help()
{
	QString s=QString("");
	
	s+=tr("Mode: official. This is the usual mode for timetabling. The week has days and hours, and you allocate activities with a subject, teacher(s), students to"
	 " time slots and rooms");
	s+="\n\n";
	s+=tr("Mode: mornings-afternoons. This mode assumes that the number of FET days is the double of the real number of days per week, such that the first FET day"
	 " is the morning of the first real day, the second FET day is the afternoon of the first real day, the third FET day is the morning of the second real day,"
	 " and so on. This mode was developed with suggestions from the users of FET from Morocco and Algeria. It may be used in other countries as well.");
	s+="\n\n";
	s+=tr("Mode: block-planning. This mode assigns activities with a subject and students to FET hours (real-life time slots, day and hour) and to FET days"
	 " (real-life teachers). Be careful, the basic time and space constraints are much stronger, and each students set may have a single occupied column from"
	 " all the columns of a row.");
	s+="\n\n";
	s+=tr("Mode: terms. This mode was developed with suggestions from the users of FET from Finland, but it might be used in other countries as well. The data"
	 " has a number of terms and a number of days in each term. The total number of days must be the product of these two numbers. The week begins with the first term's days,"
	 " continues with the second term, and so on.");

	LongTextMessageBox::largeInformation(this, tr("FET Help"), s);
}

void GetModeForNewFileForm::ok()
{
	if(officialRadioButton->isChecked()){
		mode=OFFICIAL;
	}
	else if(morningsAfternoonsRadioButton->isChecked()){
		mode=MORNINGS_AFTERNOONS;
	}
	else if(blockPlanningRadioButton->isChecked()){
		QString s=tr("WARNING: In the block-planning mode, the basic time and space constraints are modified to consider that the FET hours are real-life time slots."
		 " Consequently, they are much stronger and each row (FET hour, real-life time slot) for a students subgroup / room may have a single occupied slot out of all"
		 " the FET days (real-life teachers). This might result in an impossible timetable if you intend to do usual timetabling.");
		s+="\n\n";
		s+=tr("Are you sure you want to continue?");
		
		QMessageBox::StandardButton ret=QMessageBox::question(this, tr("FET confirmation"), s, QMessageBox::Yes|QMessageBox::Cancel);
		if(ret==QMessageBox::Cancel)
			return;
		
		mode=BLOCK_PLANNING;
	}
	else if(termsRadioButton->isChecked()){
		mode=TERMS;
	}
	else{
		QMessageBox::warning(this, tr("FET warning"), tr("Please select a mode."));
		return;
	}
	
	this->accept();
}

void GetModeForNewFileForm::cancel()
{
	this->reject();
}
