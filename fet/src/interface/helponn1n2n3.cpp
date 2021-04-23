/***************************************************************************
                          helponn1n2n3.cpp  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "helponn1n2n3.h"

#include "longtextmessagebox.h"

#include <QApplication>

void HelpOnN1N2N3::help(QWidget* parent)
{
	QString s=tr("These constraints ensure that a certain teacher (or all the teachers) do not have more than two activity tags per FET day (half real day)"
	 " from these three: N1, N2, N3. So, it is allowed to have N1 and N2, or N1 and N3, or N2 and N3, but not all three of them.");
	
	s+="\n\n";
	
	s+=tr("You need to add these three activity tags: N1, N2 and N3 (with these exact names) and then add them to (sub)activities, as needed.");
	
	LongTextMessageBox::largeInformation(parent, tr("FET information"), s);
}
