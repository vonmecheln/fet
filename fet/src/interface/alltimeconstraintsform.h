/***************************************************************************
                          alltimeconstraintsform.h  -  description
                             -------------------
    begin                : Feb 10, 2005
    copyright            : (C) 2005 by Lalescu Liviu
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

#ifndef ALLTIMECONSTRAINTSFORM_H
#define ALLTIMECONSTRAINTSFORM_H

#include "ui_alltimeconstraintsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include "advancedfilterform.h"

#include <q3combobox.h>
#include <qmessagebox.h>
#include <q3groupbox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <q3textedit.h>
#include <q3listbox.h>

/*
typedef enum {ALL, ANY} ALLANY;
typedef enum {DESCRIPTION=0, DETDESCRIPTION=1} DESCRDETDESCR;
typedef enum {CONTAINS=0, DOESNOTCONTAIN=1, REGEXP=2, NOTREGEXP=3} CONTAINSORNOT;*/

class AllTimeConstraintsForm : public QDialog, Ui::AllTimeConstraintsForm_template  {
	Q_OBJECT
	
private:
	AdvancedFilterForm* filterForm;

	QList<TimeConstraint*> visibleTimeConstraintsList;
	
	static bool filterInitialized;

	static bool all; //all or any, true means all, false means any
	static QList<int> descrDetDescr;
	static QList<int> contains;
	static QStringList text;
	static bool caseSensitive;
	
	bool useFilter;
	
public:
	AllTimeConstraintsForm();
	~AllTimeConstraintsForm();
	
	bool filterOk(TimeConstraint* ctr);
	
	void filterChanged();
	
public slots:
	void constraintChanged();
	void modifyConstraint();
	void removeConstraint();
	void filter(bool active);
};

#endif
