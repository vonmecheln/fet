//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2005 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include "institutionnameform.h"

#include <QMessageBox>

extern Timetable gt;

extern bool generation_running;
extern bool generation_running_multi;

InstitutionNameForm::InstitutionNameForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	okPushButton->setDefault(true);

	connect(okPushButton, &QPushButton::clicked, this, &InstitutionNameForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &InstitutionNameForm::cancel);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	institutionNameLineEdit->setText(gt.rules.institutionName);
	institutionNameLineEdit->selectAll();
	institutionNameLineEdit->setFocus();
}

InstitutionNameForm::~InstitutionNameForm()
{
	saveFETDialogGeometry(this);
}

void InstitutionNameForm::ok()
{
	if(!generation_running && !generation_running_multi){
		QString oin=gt.rules.institutionName;
	
		gt.rules.setInstitutionName(institutionNameLineEdit->text());
		
		gt.rules.addUndoPoint(tr("Changed the institution name from %1 to %2.").arg(oin).arg(gt.rules.institutionName));
	}
	else{
		QMessageBox::information(this, tr("FET information"),
			tr("Cannot update institution name during generation."
			" Please stop the generation before this."));
		return;
	}

	this->close();
}

void InstitutionNameForm::cancel()
{
	this->close();
}
