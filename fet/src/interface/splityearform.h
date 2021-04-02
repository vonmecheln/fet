/***************************************************************************
                          splityearform.h  -  description
                             -------------------
    begin                : 10 Aug 2007
    copyright            : (C) 2007 by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SPLITYEARFORM_H
#define SPLITYEARFORM_H

#include "ui_splityearform_template.h"

#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class SplitYearForm : public QDialog, Ui::SplitYearForm_template  {
	Q_OBJECT

public:
	QString year;
	
	SplitYearForm(const QString year);
	~SplitYearForm();

public slots:
	void ok();
	void numberOfCategoriesChanged();
	void category1Changed();
	void category2Changed();
	void category3Changed();
	
	void help();
	void reset();
};

#endif
