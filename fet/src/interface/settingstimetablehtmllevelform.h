//
//
// C++ Interface: $MODULE$
//
// Description:
//
//
// Author: Lalescu Liviu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SETTINGSTIMETABLEHTMLLEVELFORM_H
#define SETTINGSTIMETABLEHTMLLEVELFORM_H

#include "ui_settingstimetablehtmllevelform_template.h"

class SettingsTimetableHtmlLevelForm:public QDialog, Ui::SettingsTimetableHtmlLevelForm_template
{
	Q_OBJECT

public:
	SettingsTimetableHtmlLevelForm();
	~SettingsTimetableHtmlLevelForm();

public slots:
	void on_okPushButton_clicked();
	void on_cancelPushButton_clicked();
};

#endif
