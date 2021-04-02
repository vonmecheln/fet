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

#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include "hoursform.h"

#include <qspinbox.h>
#include <qmessagebox.h>
#include <qlineedit.h>

#include <QDesktopWidget>

extern Timetable gt;

QLineEdit* hoursNames[60];
int nHours;

HoursForm::HoursForm()
 : HoursForm_template()
{
    setupUi(this);

    connect(hoursSpinBox, SIGNAL(valueChanged(int)), this /*HoursForm_template*/, SLOT(hoursChanged()));
    connect(okPushButton, SIGNAL(clicked()), this /*HoursForm_template*/, SLOT(ok()));
    connect(cancelPushButton, SIGNAL(clicked()), this /*HoursForm_template*/, SLOT(cancel()));

	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
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

	hoursNames[31]=hour32LineEdit;
	hoursNames[32]=hour33LineEdit;
	hoursNames[33]=hour34LineEdit;
	hoursNames[34]=hour35LineEdit;
	hoursNames[35]=hour36LineEdit;
	hoursNames[36]=hour37LineEdit;
	hoursNames[37]=hour38LineEdit;
	hoursNames[38]=hour39LineEdit;
	hoursNames[39]=hour40LineEdit;
	hoursNames[40]=hour41LineEdit;

	hoursNames[41]=hour42LineEdit;
	hoursNames[42]=hour43LineEdit;
	hoursNames[43]=hour44LineEdit;
	hoursNames[44]=hour45LineEdit;
	hoursNames[45]=hour46LineEdit;
	hoursNames[46]=hour47LineEdit;
	hoursNames[47]=hour48LineEdit;
	hoursNames[48]=hour49LineEdit;
	hoursNames[49]=hour50LineEdit;
	hoursNames[50]=hour51LineEdit;

	hoursNames[51]=hour52LineEdit;
	hoursNames[52]=hour53LineEdit;
	hoursNames[53]=hour54LineEdit;
	hoursNames[54]=hour55LineEdit;
	hoursNames[55]=hour56LineEdit;
	hoursNames[56]=hour57LineEdit;
	hoursNames[57]=hour58LineEdit;
	hoursNames[58]=hour59LineEdit;
	hoursNames[59]=hour60LineEdit;

	hoursSpinBox->setMinValue(1);
	hoursSpinBox->setMaxValue(60);
	hoursSpinBox->setValue(gt.rules.nHoursPerDay);

	for(int i=0; i<60; i++)
		if(i<nHours){
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
	for(int i=0; i<60; i++)
		if(i<nHours)
			hoursNames[i]->setEnabled(true);
		else
			hoursNames[i]->setDisabled(true);
}

void HoursForm::ok()
{
	for(int i=0; i<nHours; i++)
		if(hoursNames[i]->text()==""){
			QMessageBox::warning(this, tr("FET information"),
				tr("Empty names not allowed"));
			return;
		}
	for(int i=0; i<nHours-1; i++)
		for(int j=i+1; j<nHours; j++)
			if(hoursNames[i]->text()==hoursNames[j]->text()){
				QMessageBox::warning(this, tr("FET information"),
					tr("Duplicates not allowed"));
				return;
			}

	QMessageBox::information(this, tr("FET information"),
		tr("Please note that FET will NOT take care "
		"of old constraints using erased hours "
		"(only renamed hours will be handled correctly)"));
				
	/*int t=QMessageBox::question(this, tr("FET question"),
		tr("Are you sure that the number of working periods per day is %1? (there were lots of misunderstandings here)")
		.arg(nHours),
		QMessageBox::Yes, QMessageBox::Cancel
		);		
	if(t==QMessageBox::Cancel)
		return;*/

	//remove old names
	for(int i=nHours; i<gt.rules.nHoursPerDay; i++)
		gt.rules.hoursOfTheDay[i]="";
		
	gt.rules.nHoursPerDay=nHours;
	for(int i=0; i<nHours; i++)
		gt.rules.hoursOfTheDay[i]=hoursNames[i]->text();
		
	gt.rules.nHoursPerWeek=gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek; //not needed
	gt.rules.internalStructureComputed=false;

	assert(gt.rules.nHoursPerDay<=MAX_HOURS_PER_DAY);
		
	this->close();
}

void HoursForm::cancel()
{
	this->close();
}
