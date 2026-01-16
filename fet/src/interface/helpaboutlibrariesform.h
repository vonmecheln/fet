/***************************************************************************
                          helpaboutlibrariesform.h  -  description
                             -------------------
    begin                : 2019
    copyright            : (C) 2019 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef HELPABOUTLIBRARIESFORM_H
#define HELPABOUTLIBRARIESFORM_H

#include "ui_helpaboutlibrariesform_template.h"

class HelpAboutLibrariesForm : public QDialog, public Ui::HelpAboutLibrariesForm_template
{
	Q_OBJECT
	
public:
	HelpAboutLibrariesForm(QWidget* parent);
	~HelpAboutLibrariesForm();
};

#endif
