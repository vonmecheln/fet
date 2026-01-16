/***************************************************************************
                          settingsstyleandcolorschemeform.cpp  -  description
                             -------------------
    begin                : 2024
    copyright            : (C) 2024 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include "settingsstyleandcolorschemeform.h"

#include <QStyleFactory>
#include <QStringList>
#include <QString>

#include <QMessageBox>

SettingsStyleAndColorSchemeForm::SettingsStyleAndColorSchemeForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	connect(okPushButton, &QPushButton::clicked, this, &SettingsStyleAndColorSchemeForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &SettingsStyleAndColorSchemeForm::cancel);
	
	int i=0;
	int j=0;
	stylesComboBox->addItem(tr("Default", "It refers to a style"));
	//Remember that the saved style may no longer be correct, or the saved style may be corrupt.
	//if(INTERFACE_STYLE=="")
	//	j=0;
	i++;
	QStringList sl=QStyleFactory::keys();
	for(const QString& s : std::as_const(sl)){
		stylesComboBox->addItem(s);
		if(QString::compare(INTERFACE_STYLE, s, Qt::CaseInsensitive)==0)
			j=i;
		i++;
	}
	stylesComboBox->setCurrentIndex(j);
	
	if(INTERFACE_COLOR_SCHEME=="automatic")
		automaticRadioButton->setChecked(true);
	else if(INTERFACE_COLOR_SCHEME=="light")
		lightRadioButton->setChecked(true);
	else if(INTERFACE_COLOR_SCHEME=="dark")
		darkRadioButton->setChecked(true);
	else
		assert(0);
}

SettingsStyleAndColorSchemeForm::~SettingsStyleAndColorSchemeForm()
{
	saveFETDialogGeometry(this);
}

void SettingsStyleAndColorSchemeForm::ok()
{
	if(stylesComboBox->currentIndex()<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Please select a style."));
		return;
	}

	if(!automaticRadioButton->isChecked() && !lightRadioButton->isChecked() && !darkRadioButton->isChecked()){
		QMessageBox::warning(this, tr("FET warning"), tr("Please select a color scheme."));
		return;
	}

	if(stylesComboBox->currentIndex()==0)
		INTERFACE_STYLE="";
	else
		INTERFACE_STYLE=stylesComboBox->currentText();
	
	if(automaticRadioButton->isChecked())
		INTERFACE_COLOR_SCHEME="automatic";
	else if(lightRadioButton->isChecked())
		INTERFACE_COLOR_SCHEME="light";
	else if(darkRadioButton->isChecked())
		INTERFACE_COLOR_SCHEME="dark";
	else
		assert(0);
	
	this->accept();
}

void SettingsStyleAndColorSchemeForm::cancel()
{
	this->reject();
}
