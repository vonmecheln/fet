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

#include "hoursform.h"

#include <qspinbox.h>
#include <qmessagebox.h>
#include <qlineedit.h>

#include <QDesktopWidget>

extern GeneticTimetable gt;

QLineEdit* hoursNames[31];
int nHours;

HoursForm::HoursForm()
 : HoursForm_template()
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	nHours=gt.rules.nHoursPerDay;
	hoursNames[0]=hour1LineEdit;
	hoursNames[1]=hour2LineEdit;
	hoursNames[2]=hour3LineEdit;
	hoursNames[3]=hour4LineEdit;
	hoursNames[4]=hour5LineEdit;
	hoursNames[5]=hour6LineEdit;
	hoursNames[6]=hour7LineEdit;
	hoursNames[7]=hour8LineEdit;
	hoursNames[8]=hour9LineEdit;
	hoursNames[9]=hour10LineEdit;
	hoursNames[10]=hour11LineEdit;
	hoursNames[11]=hour12LineEdit;
	hoursNames[12]=hour13LineEdit;
	hoursNames[13]=hour14LineEdit;
	hoursNames[14]=hour15LineEdit;
	hoursNames[15]=hour16LineEdit;
	hoursNames[16]=hour17LineEdit;
	hoursNames[17]=hour18LineEdit;
	hoursNames[18]=hour19LineEdit;
	hoursNames[19]=hour20LineEdit;
	hoursNames[20]=hour21LineEdit;
	hoursNames[21]=hour22LineEdit;
	hoursNames[22]=hour23LineEdit;
	hoursNames[23]=hour24LineEdit;
	hoursNames[24]=hour25LineEdit;
	hoursNames[25]=hour26LineEdit;
	hoursNames[26]=hour27LineEdit;
	hoursNames[27]=hour28LineEdit;
	hoursNames[28]=hour29LineEdit;
	hoursNames[29]=hour30LineEdit;
	hoursNames[30]=hour31LineEdit;

	hoursSpinBox->setMinValue(1);
	hoursSpinBox->setMaxValue(30);
	hoursSpinBox->setValue(gt.rules.nHoursPerDay);

	for(int i=0; i<=30; i++)
		if(i<=nHours){
			hoursNames[i]->setEnabled(true);
			hoursNames[i]->setText(gt.rules.hoursOfTheDay[i]);
		}
		else
			hoursNames[i]->setDisabled(true);
}


HoursForm::~HoursForm()
{
}

void HoursForm::hoursChanged()
{
	nHours=hoursSpinBox->value();
	assert(nHours <= MAX_HOURS_PER_DAY);
	for(int i=0; i<=30; i++)
		if(i<=nHours)
			hoursNames[i]->setEnabled(true);
		else
			hoursNames[i]->setDisabled(true);
}

void HoursForm::ok()
{
	for(int i=0; i<=nHours; i++)
		if(hoursNames[i]->text()==""){
			QMessageBox::warning(this, QObject::tr("FET information"),
				QObject::tr("Empty names not allowed\n"));
			return;
		}
	for(int i=0; i<nHours; i++)
		for(int j=i+1; j<=nHours; j++)
			if(hoursNames[i]->text()==hoursNames[j]->text()){
				QMessageBox::warning(this, QObject::tr("FET information"),
					QObject::tr("Duplicates not allowed\n"));
				return;
			}

	QMessageBox::information(this, QObject::tr("FET information"),
		QObject::tr("Please note that FET will NOT take care\n"
		"of old constraints using erased hours\n"
		"(only renamed hours will be handled correctly)\n"));

	gt.rules.nHoursPerDay=nHours;
	for(int i=0; i<=nHours; i++)
		gt.rules.hoursOfTheDay[i]=hoursNames[i]->text();

	this->close();
}

void HoursForm::cancel()
{
	this->close();
}