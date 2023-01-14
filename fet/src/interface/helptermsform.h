/***************************************************************************
                          helptermsform.h  -  description
                             -------------------
    begin                : 2021
    copyright            : (C) 2021 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef HELPTERMSFORM_H
#define HELPTERMSFORM_H

#include "ui_helptermsform_template.h"

class HelpTermsForm : public QDialog, public Ui::HelpTermsForm_template
{
	Q_OBJECT
	
public:
	HelpTermsForm(QWidget* parent);
	~HelpTermsForm();
	
	void setText();
};

#endif
