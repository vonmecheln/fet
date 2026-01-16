/***************************************************************************
                          helpinstructionsform.h  -  description
                             -------------------
    begin                : July 19, 2007
    copyright            : (C) 2007 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef HELPINSTRUCTIONSFORM_H
#define HELPINSTRUCTIONSFORM_H

#include "ui_helpinstructionsform_template.h"

class HelpInstructionsForm : public QDialog, public Ui::HelpInstructionsForm_template
{
	Q_OBJECT
	
public:
	HelpInstructionsForm(QWidget* parent);
	~HelpInstructionsForm();
	
	void setText();
};

#endif
