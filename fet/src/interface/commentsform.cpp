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

#include "commentsform.h"

#include <QMessageBox>

extern Timetable gt;

extern bool generation_running;
extern bool generation_running_multi;

CommentsForm::CommentsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	//!!!Do NOT set commentsTextEdit read only
	
	okPushButton->setDefault(true);

	connect(okPushButton, &QPushButton::clicked, this, &CommentsForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &CommentsForm::cancel);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	commentsTextEdit->setPlainText(gt.rules.comments);
	commentsTextEdit->selectAll();
	commentsTextEdit->setFocus();
}

CommentsForm::~CommentsForm()
{
	saveFETDialogGeometry(this);
}

void CommentsForm::ok()
{
	if(!generation_running && !generation_running_multi){
		QString oc=gt.rules.comments;
	
		gt.rules.setComments(commentsTextEdit->toPlainText());
		
		gt.rules.addUndoPoint(tr("Changed the comments from %1 to %2.").arg(oc).arg(gt.rules.comments));
	}
	else{
		QMessageBox::information(this, tr("FET information"),
			tr("Cannot update the comments during generation."
			" Please stop the generation before this."));
		return;
	}

	this->close();
}

void CommentsForm::cancel()
{
	this->close();
}
