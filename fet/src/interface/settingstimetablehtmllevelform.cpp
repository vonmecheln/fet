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

#include "timetable_defs.h"

#include "settingstimetablehtmllevelform.h"

extern QApplication* pqapplication;

#include <QDesktopWidget>

#include <QMessageBox>

SettingsTimetableHtmlLevelForm::SettingsTimetableHtmlLevelForm()
{
	setupUi(this);
	
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);
	
	if(TIMETABLE_HTML_LEVEL==0)
		level0RadioButton->setChecked(true);
	else if(TIMETABLE_HTML_LEVEL==1)
		level1RadioButton->setChecked(true);
	else if(TIMETABLE_HTML_LEVEL==2)
		level2RadioButton->setChecked(true);
	else if(TIMETABLE_HTML_LEVEL==3)
		level3RadioButton->setChecked(true);
	else if(TIMETABLE_HTML_LEVEL==4)
		level4RadioButton->setChecked(true);
	else if(TIMETABLE_HTML_LEVEL==5)
		level5RadioButton->setChecked(true);
}

SettingsTimetableHtmlLevelForm::~SettingsTimetableHtmlLevelForm()
{
}

void SettingsTimetableHtmlLevelForm::on_okPushButton_clicked()
{
	int level=-1;
	if(level0RadioButton->isChecked())
		level=0;
	else if(level1RadioButton->isChecked())
		level=1;
	else if(level2RadioButton->isChecked())
		level=2;
	else if(level3RadioButton->isChecked())
		level=3;
	else if(level4RadioButton->isChecked())
		level=4;
	else if(level5RadioButton->isChecked())
		level=5;
	else{
		QMessageBox::warning(this, tr("FET warning"), tr("No level selected"));
		return;
	}
	
	assert(level>=0);

	if(level>=3){
		int t=QMessageBox::warning(this, tr("FET warning"), tr("This level might generate very large timetables, maybe 1 MB per file"
		 " and 10 MB for all files of a timetable or even more."
		 " Are you sure you have enough disk space?"),
		 QMessageBox::Yes, QMessageBox::Cancel);
								 		 
		if(t==QMessageBox::Cancel)
			return;
	}
	
	TIMETABLE_HTML_LEVEL=level;
	
	this->close();
}

void SettingsTimetableHtmlLevelForm::on_cancelPushButton_clicked()
{
	this->close();
}
