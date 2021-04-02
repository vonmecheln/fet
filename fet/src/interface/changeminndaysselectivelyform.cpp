/***************************************************************************
                          changeminndaysselectivelyform.cpp  -  description
                             -------------------
    begin                : July 30, 2008
    copyright            : (C) 2008 by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "changeminndaysselectivelyform.h"

#include "fet.h"

#include <QMessageBox>

ChangeMinNDaysSelectivelyForm::ChangeMinNDaysSelectivelyForm()
{
	setupUi(this);
	
	oldConsecutiveComboBox->clear();
	oldConsecutiveComboBox->addItem(tr("Any"));
	oldConsecutiveComboBox->addItem(tr("Yes"));
	oldConsecutiveComboBox->addItem(tr("No"));
	oldConsecutiveComboBox->setCurrentIndex(0);
	
	oldDaysSpinBox->setMinValue(-1);
	oldDaysSpinBox->setMaxValue(gt.rules.nDaysPerWeek);
	oldDaysSpinBox->setValue(-1);

	newConsecutiveComboBox->clear();
	newConsecutiveComboBox->addItem(tr("No change"));
	newConsecutiveComboBox->addItem(tr("Yes"));
	newConsecutiveComboBox->addItem(tr("No"));
	newConsecutiveComboBox->setCurrentIndex(0);
	
	newDaysSpinBox->setMinValue(-1);
	newDaysSpinBox->setMaxValue(gt.rules.nDaysPerWeek);
	newDaysSpinBox->setValue(-1);
	
	oldNActsSpinBox->setMinValue(-1);
	oldNActsSpinBox->setMaxValue(99);
	oldNActsSpinBox->setValue(-1);
}

ChangeMinNDaysSelectivelyForm::~ChangeMinNDaysSelectivelyForm()
{

}

void ChangeMinNDaysSelectivelyForm::on_okPushButton_clicked()
{
	enum {ANY=0, YES=1, NO=2};
	enum {NOCHANGE=0};

	oldWeight=oldDays=oldConsecutive=oldNActs=-2;
	newWeight=newDays=newConsecutive=-2;

	QString oldWeightS=oldWeightLineEdit->text();
	sscanf(oldWeightS, "%lf", &oldWeight);
	if(!(oldWeight==-1 || (oldWeight>=0.0 && oldWeight<=100.0))){
		QMessageBox::warning(this, tr("FET warning"), tr("Old weight must be -1 or both >=0 and <=100"));
		return;
	}
	
	QString newWeightS=newWeightLineEdit->text();
	sscanf(newWeightS, "%lf", &newWeight);
	if(!(newWeight==-1 || (newWeight>=0.0 && newWeight<=100.0))){
		QMessageBox::warning(this, tr("FET warning"), tr("New weight must be -1 or both >=0 and <=100"));
		return;
	}
	
	oldDays=oldDaysSpinBox->value();
	if(!(oldDays==-1 || (oldDays>=1 && oldDays<=gt.rules.nDaysPerWeek))){
		QMessageBox::warning(this, tr("FET warning"), tr("Old min n days must be -1 or both >=1 and <=n_days_per_week"));
		return;
	}
	
	newDays=newDaysSpinBox->value();
	if(!(newDays==-1 || (newDays>=1 && newDays<=gt.rules.nDaysPerWeek))){
		QMessageBox::warning(this, tr("FET warning"), tr("New min n days must be -1 or both >=1 and <=n_days_per_week"));
		return;
	}
	
	oldConsecutive=oldConsecutiveComboBox->currentIndex();
	assert(oldConsecutive>=0 && oldConsecutive<=2);

	newConsecutive=newConsecutiveComboBox->currentIndex();
	assert(newConsecutive>=0 && newConsecutive<=2);

	oldNActs=oldNActsSpinBox->value();
	if(!(oldNActs==-1 || oldNActs>=1)){
		QMessageBox::warning(this, tr("FET warning"), tr("Old n_acts must be -1 or >=1"));
		return;
	}
	
	this->accept();
}

void ChangeMinNDaysSelectivelyForm::on_cancelPushButton_clicked()
{
	this->reject();
}
