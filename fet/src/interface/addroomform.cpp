/***************************************************************************
                          addroomform.cpp  -  description
                             -------------------
    begin                : Sun Jan 1 2004
    copyright            : (C) 2004 by Lalescu Liviu
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

#include "addroomform.h"

#include <qlineedit.h>
#include <q3combobox.h>

#include <QDesktopWidget>

#include <QMessageBox>

#include <QTextEdit>

AddRoomForm::AddRoomForm()
{
	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	/*typesComboBox->clear();
	typesComboBox->setDuplicatesEnabled(false);
	for(int i=0; i<gt.rules.roomsList.size(); i++){
		Room* rm=gt.rules.roomsList[i];
		int i;
		for(i=0; i<typesComboBox->count(); i++)
			if(typesComboBox->text(i)==rm->type)
				break;
		if(i==typesComboBox->count())
			typesComboBox->insertItem(rm->type);
	}*/

	buildingsComboBox->clear();
	buildingsComboBox->insertItem("");
	for(int i=0; i<gt.rules.buildingsList.size(); i++)
		buildingsComboBox->insertItem(gt.rules.buildingsList.at(i)->name);
		
	capacitySpinBox->setMinValue(1);
	capacitySpinBox->setMaxValue(MAX_ROOM_CAPACITY);
	capacitySpinBox->setValue(MAX_ROOM_CAPACITY);
}

AddRoomForm::~AddRoomForm()
{
}

void AddRoomForm::addRoom()
{
	if(nameLineEdit->text().isEmpty()){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Incorrect name"));
		return;
	}
	/*if(typesComboBox->currentText().isEmpty()){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Incorrect type"));
		return;
	}*/
	if(buildingsComboBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Incorrect building"));
		return;
	}
	Room* rm=new Room();
	rm->name=nameLineEdit->text();
	//rm->type=typesComboBox->currentText();
	rm->building=buildingsComboBox->currentText();
	rm->capacity=capacitySpinBox->value();
	if(!gt.rules.addRoom(rm)){
		QMessageBox::information(this, QObject::tr("Room insertion dialog"),
			QObject::tr("Could not insert item. Must be a duplicate"));
		delete rm;
	}
	else{
		QMessageBox::information(this, QObject::tr("Room insertion dialog"),
			QObject::tr("Room added"));
			
		//typesComboBox->insertItem(rm->type);
	}

	nameLineEdit->selectAll();
	nameLineEdit->setFocus();
}

void AddRoomForm::help()
{
	QString s;
	
	s=QObject::tr("It is advisable to only input special rooms, which are not available, or for activities with "
	 "special rooms needed (no need to input home rooms for teachers or students)\n\n"
	 "It is advisable to generate the timetable without the rooms, then, if a solution is possible, to add rooms"
	 );
	 
	s+="\n\n";
	
	s+=QObject::tr("Please note that each room can hold a single activity at a specified period. If you"
	 " have a very large room, which can hold more activities at one time, please add more rooms,"
	 " representing this larger room");
	
	//show the message in a dialog
	QDialog* dialog=new QDialog();
	
	dialog->setWindowTitle(QObject::tr("FET - help on adding room(s)"));

	QVBoxLayout* vl=new QVBoxLayout(dialog);
	QTextEdit* te=new QTextEdit();
	te->setPlainText(s);
	te->setReadOnly(true);
	QPushButton* pb=new QPushButton(QObject::tr("OK"));

	QHBoxLayout* hl=new QHBoxLayout(0);
	hl->addStretch(1);
	hl->addWidget(pb);

	vl->addWidget(te);
	vl->addLayout(hl);
	connect(pb, SIGNAL(clicked()), dialog, SLOT(close()));

	dialog->setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	//QDesktopWidget* desktop=QApplication::desktop();
	QRect rect = QApplication::desktop()->availableGeometry(dialog);
	int xx=rect.width()/2 - 350;
	int yy=rect.height()/2 - 250;
	dialog->setGeometry(xx, yy, 700, 500);

	dialog->exec();
}
