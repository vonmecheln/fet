/***************************************************************************
                          helptipsform.h  -  description
                             -------------------
    begin                : Feb 20, 2005
    copyright            : (C) 2005 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef HELPTIPSFORM_H
#define HELPTIPSFORM_H

#include "ui_helptipsform_template.h"

class HelpTipsForm : public QDialog, public Ui::HelpTipsForm_template
{
	Q_OBJECT
	
public:
	HelpTipsForm(QWidget* parent);
	~HelpTipsForm();
	
	void setText();
};

#endif
