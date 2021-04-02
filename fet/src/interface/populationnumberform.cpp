//
//
// C++ Implementation: $MODULE$
//
// Description: 
//
//
// Author: Lalescu Liviu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "genetictimetable_defs.h"
#include "genetictimetable.h"
#include "fet.h"

#include "populationnumberform.h"

#include <qslider.h>
#include <qlabel.h>

#include <QDesktopWidget>

PopulationNumberForm::PopulationNumberForm()
 : PopulationNumberForm_template()
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	this->p_n=population_number;

	populationNumberSlider->setMaxValue(MAX_POPULATION_SIZE);
	populationNumberSlider->setValue(population_number);
	populationNumberSlider->setMinValue(3);

	QString s=QObject::tr("Population number (power of search)");
	s+="\n";
	s+=QString::number(population_number);
	populationNumberTextLabel->setText(s);
}


PopulationNumberForm::~PopulationNumberForm()
{
}

void PopulationNumberForm::populationNumberChanged()
{
	this->p_n=populationNumberSlider->value();

	QString s=QObject::tr("Population number (power of search)");
	s+="\n";
	s+=QString::number(this->p_n);
	populationNumberTextLabel->setText(s);
}

void PopulationNumberForm::cancel()
{
	this->close();
}

void PopulationNumberForm::ok()
{
	population_number=this->p_n;

	this->close();
}
