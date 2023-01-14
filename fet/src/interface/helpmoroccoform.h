/***************************************************************************
                          helpmoroccoform.h  -  description
                             -------------------
    begin                : August 1, 2011
    copyright            : (C) 2011 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef HELPMOROCCOFORM_H
#define HELPMOROCCOFORM_H

#include "ui_helpmoroccoform_template.h"

class HelpMoroccoForm : public QDialog, public Ui::HelpMoroccoForm_template
{
	Q_OBJECT
	
public: 
	HelpMoroccoForm(QWidget* parent);
	~HelpMoroccoForm();
};

#endif
