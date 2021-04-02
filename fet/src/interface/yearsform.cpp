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

#include "addstudentsyearform.h"
#include "modifystudentsyearform.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"
#include "yearsform.h"
#include "studentsset.h"

#include "splityearform.h"

#include <q3listbox.h>
#include <qinputdialog.h>
#include <qstring.h>

#include <QDesktopWidget>

#include <QMessageBox>

YearsForm::YearsForm()
 : YearsForm_template()
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	yearsListBox->clear();
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* year=gt.rules.yearsList[i];
		yearsListBox->insertItem(year->name);
	}
}


YearsForm::~YearsForm()
{
}

void YearsForm::addYear()
{
	AddStudentsYearForm* form=new AddStudentsYearForm();
	form->exec();

	yearsListBox->clear();
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* year=gt.rules.yearsList[i];
		yearsListBox->insertItem(year->name);
	}
}

void YearsForm::removeYear()
{
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListBox->currentText();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	if(QMessageBox::warning( this, QObject::tr("FET"),
		QObject::tr("Are you sure you want to delete this year and all related groups, subgroups, activities and constraints?\n"),
		QObject::tr("Yes"), QObject::tr("No"), 0, 0, 1 ) == 1)
		return;

	bool tmp=gt.rules.removeYear(yearName);
	assert(tmp);
	if(tmp)
		yearsListBox->removeItem(yearsListBox->currentItem());
	this->show();
}

void YearsForm::yearChanged()
{
	if(yearsListBox->currentItem()<0)
		return;
	StudentsYear* sty=gt.rules.yearsList.at(yearsListBox->currentItem());
	detailsTextEdit->setText(sty->getDetailedDescriptionWithConstraints(gt.rules));
}

void YearsForm::sortYears()
{
	gt.rules.sortYearsAlphabetically();

	yearsListBox->clear();
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* year=gt.rules.yearsList[i];
		yearsListBox->insertItem(year->name);
	}
}

void YearsForm::modifyYear()
{
	int ci=yearsListBox->currentItem();
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListBox->currentText();
	int numberOfStudents=gt.rules.searchStudentsSet(yearName)->numberOfStudents;

	ModifyStudentsYearForm* form=new ModifyStudentsYearForm(yearName, numberOfStudents);
	form->exec();

	yearsListBox->clear();
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* year=gt.rules.yearsList[i];
		yearsListBox->insertItem(year->name);
	}

	yearsListBox->setCurrentItem(ci);
}

void YearsForm::activateStudents()
{
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected year"));
		return;
	}
	
	QString yearName=yearsListBox->currentText();
	int count=gt.rules.activateStudents(yearName);
	QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Activated a number of %1 activities").arg(count));
}

void YearsForm::deactivateStudents()
{
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected year"));
		return;
	}
	
	QString yearName=yearsListBox->currentText();
	int count=gt.rules.deactivateStudents(yearName);
	QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("De-activated a number of %1 activities").arg(count));
}

void YearsForm::divideYear()
{
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected year"));
		return;
	}
	
	QString yearName=yearsListBox->currentText();	
	
	SplitYearForm* form=new SplitYearForm(yearName);
	form->exec();
}
