/***************************************************************************
                          helponn1n2n3.h  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Liviu Lalescu
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

#ifndef HELPONN1N2N3_H
#define HELPONN1N2N3_H

#include <QApplication>

class HelpOnN1N2N3{
	Q_DECLARE_TR_FUNCTIONS(HelpOnN1N2N3)
	
public:
	static void helpOnTeachersConstraintsDays(QWidget* parent);
	static void helpOnStudentsConstraintsDays(QWidget* parent);
	static void helpOnTeachersConstraintsRealDays(QWidget* parent);
	static void helpOnStudentsConstraintsRealDays(QWidget* parent);
};

#endif
