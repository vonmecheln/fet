//
//
// C++ Implementation: $MODULE$
//
// Description: 
//
//
// Author: Lalescu Liviu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include "commentsform.h"

#include <q3textedit.h>

#include <QMessageBox>

#include <QDesktopWidget>

extern Timetable gt;

extern bool simulation_running;

CommentsForm::CommentsForm()
 : CommentsForm_template()
{
	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	commentsTextEdit->setText(gt.rules.comments);
}

CommentsForm::~CommentsForm()
{
}

void CommentsForm::ok()
{
	if(!simulation_running)
		gt.rules.setComments(commentsTextEdit->text());
	else{
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Cannot update comments during simulation\n"
			"Please stop simulation before this"));
		return;
	}

	this->close();
}

void CommentsForm::cancel()
{
	this->close();
}
