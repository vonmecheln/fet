/***************************************************************************
                          helpalgeriaform.h  -  description
                             -------------------
    begin                : September 2, 2011
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

#ifndef HELPALGERIAFORM_H
#define HELPALGERIAFORM_H

#include "ui_helpalgeriaform_template.h"

class HelpAlgeriaForm : public QDialog, public Ui::HelpAlgeriaForm_template
{
	Q_OBJECT
	
public: 
	HelpAlgeriaForm(QWidget* parent);
	~HelpAlgeriaForm();
};

#endif
