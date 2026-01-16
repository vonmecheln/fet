/***************************************************************************
                          helpblockplanningform.h  -  description
                             -------------------
    begin                : 2020
    copyright            : (C) 2020 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef HELPBLOCKPLANNINGFORM_H
#define HELPBLOCKPLANNINGFORM_H

#include "ui_helpblockplanningform_template.h"

class HelpBlockPlanningForm : public QDialog, public Ui::HelpBlockPlanningForm_template
{
	Q_OBJECT
	
public:
	HelpBlockPlanningForm(QWidget* parent);
	~HelpBlockPlanningForm();
	
	void setText();
};

#endif
