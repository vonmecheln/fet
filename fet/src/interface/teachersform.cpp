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
#include "teachersform.h"
#include "fetmainform.h"
#include "teacher.h"

#include <q3listbox.h>
#include <qinputdialog.h>
#include <q3textedit.h>

#include <QDesktopWidget>

TeachersForm::TeachersForm()
 : TeachersForm_template()
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	teachersListBox->clear();
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		teachersListBox->insertItem(tch->name);
	}
		
	if(teachersListBox->count()>0){
		teachersListBox->setCurrentItem(0);
		this->teacherChanged(0);
	}
}


TeachersForm::~TeachersForm()
{
}

void TeachersForm::addTeacher()
{
	bool ok = FALSE;
	Teacher* tch=new Teacher();
	tch->name = QInputDialog::getText( QObject::tr("User input"), QObject::tr("Please enter teacher's name") ,
                    QLineEdit::Normal, QString::null, &ok, this );

	if ( ok && !((tch->name).isEmpty()) ){
		// user entered something and pressed OK
		if(!gt.rules.addTeacher(tch)){
			QMessageBox::information( this, QObject::tr("Teacher insertion dialog"),
				QObject::tr("Could not insert item. Must be a duplicate"));
			delete tch;
		}
		else{
			teachersListBox->insertItem(tch->name);
			teachersListBox->setCurrentItem(teachersListBox->count()-1);
			this->teacherChanged(teachersListBox->count()-1);
		}
	}
	else
		delete tch;// user entered nothing or pressed Cancel
}

void TeachersForm::removeTeacher()
{
	int i=teachersListBox->currentItem();
	if(teachersListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected teacher"));
		return;
	}

	QString text=teachersListBox->currentText();
	int teacher_ID=gt.rules.searchTeacher(text);
	if(teacher_ID<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected teacher"));
		return;
	}

	if(QMessageBox::warning( this, QObject::tr("FET"),
		QObject::tr("Are you sure you want to delete this teacher and all related activities and constraints?\n"),
		QObject::tr("Yes"), QObject::tr("No"), 0, 0, 1 ) == 1)
		return;

	int tmp=gt.rules.removeTeacher(text);
	if(tmp){
		teachersListBox->removeItem(teachersListBox->currentItem());
		if((uint)(i)>=teachersListBox->count())
			i=teachersListBox->count()-1;
		teachersListBox->setCurrentItem(i);
		this->teacherChanged(i);		
	}
}

void TeachersForm::renameTeacher()
{
	int i=teachersListBox->currentItem();
	if(teachersListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected teacher"));
		return;
	}

	QString initialTeacherName=teachersListBox->currentText();
	int teacher_ID=gt.rules.searchTeacher(initialTeacherName);
	if(teacher_ID<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected teacher"));
		return;
	}

	bool ok = FALSE;
	QString finalTeacherName;
	finalTeacherName = QInputDialog::getText( QObject::tr("User input"), QObject::tr("Please enter new teacher's name") ,
                    QLineEdit::Normal, initialTeacherName, &ok, this );

	if ( ok && !(finalTeacherName.isEmpty())){
		// user entered something and pressed OK
		if(gt.rules.searchTeacher(finalTeacherName)>=0){
			QMessageBox::information( this, QObject::tr("Teacher insertion dialog"),
				QObject::tr("Could not modify item. New name must be a duplicate"));
		}
		else{
			gt.rules.modifyTeacher(initialTeacherName, finalTeacherName);
			teachersListBox->changeItem(finalTeacherName, i);
		}
	}
}

void TeachersForm::sortTeachers()
{
	gt.rules.sortTeachersAlphabetically();

	teachersListBox->clear();
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		teachersListBox->insertItem(tch->name);
	}
}

void TeachersForm::teacherChanged(int index)
{
	if(index<0){
		currentTeacherTextEdit->setText(QObject::tr("Invalid teacher"));
		return;
	}
	
	Teacher* t=gt.rules.teachersList.at(index);
	assert(t);
	QString s=t->getDetailedDescriptionWithConstraints(gt.rules);
	currentTeacherTextEdit->setText(s);
}

void TeachersForm::activateTeacher()
{
	if(teachersListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected teacher"));
		return;
	}

	QString teacherName=teachersListBox->currentText();
	int count=gt.rules.activateTeacher(teacherName);
	QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Activated a number of %1 activities").arg(count));
}

void TeachersForm::deactivateTeacher()
{
	if(teachersListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected teacher"));
		return;
	}

	QString teacherName=teachersListBox->currentText();
	int count=gt.rules.deactivateTeacher(teacherName);
	QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("De-activated a number of %1 activities").arg(count));
}
