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
    setupUi(this);

    connect(addYearPushButton, SIGNAL(clicked()), this /*YearsForm_template*/, SLOT(addYear()));
    connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(removeYearPushButton, SIGNAL(clicked()), this, SLOT(removeYear()));
    connect(yearsListBox, SIGNAL(highlighted(int)), this, SLOT(yearChanged()));
    connect(modifyYearPushButton, SIGNAL(clicked()), this, SLOT(modifyYear()));
    connect(sortYearsPushButton, SIGNAL(clicked()), this, SLOT(sortYears()));
    connect(activateStudentsPushButton, SIGNAL(clicked()), this, SLOT(activateStudents()));
    connect(deactivateStudentsPushButton, SIGNAL(clicked()), this, SLOT(deactivateStudents()));
    connect(divisionsPushButton, SIGNAL(clicked()), this, SLOT(divideYear()));
    connect(yearsListBox, SIGNAL(selected(QString)), this, SLOT(modifyYear()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	yearsListBox->clear();
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* year=gt.rules.yearsList[i];
		yearsListBox->insertItem(year->name);
	}
	
	if(yearsListBox->count()>0){
		yearsListBox->setCurrentItem(0);
		yearsListBox->setSelected(yearsListBox->currentItem(), true);
	}
}


YearsForm::~YearsForm()
{
}

void YearsForm::addYear()
{
	AddStudentsYearForm form;
	form.exec();

	yearsListBox->clear();
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* year=gt.rules.yearsList[i];
		yearsListBox->insertItem(year->name);
	}
	
	int i=yearsListBox->count()-1;
	if(i>=0){
		yearsListBox->setCurrentItem(i);
		yearsListBox->setSelected(i, true);
	}
}

void YearsForm::removeYear()
{
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListBox->currentText();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	if(QMessageBox::warning( this, tr("FET"),
		tr("Are you sure you want to delete year %1 and all related groups, subgroups, activities and constraints?").arg(yearName),
		tr("Yes"), tr("No"), 0, 0, 1 ) == 1)
		return;

	bool tmp=gt.rules.removeYear(yearName);
	assert(tmp);
	if(tmp)
		yearsListBox->removeItem(yearsListBox->currentItem());

	if(yearsListBox->currentItem()>=0 && yearsListBox->count()>0)
		yearsListBox->setSelected(yearsListBox->currentItem(), true);
	else
		detailsTextEdit->setText("");
}

void YearsForm::yearChanged()
{
	if(yearsListBox->currentItem()<0){
		detailsTextEdit->setText("");
		return;
	}
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

	if(yearsListBox->count()>0){
		yearsListBox->setCurrentItem(0);
		yearsListBox->setSelected(0, true);
	}
}

void YearsForm::modifyYear()
{
	int ti=yearsListBox->topItem();

	int ci=yearsListBox->currentItem();
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListBox->currentText();
	int numberOfStudents=gt.rules.searchStudentsSet(yearName)->numberOfStudents;

	ModifyStudentsYearForm form(yearName, numberOfStudents);
	form.exec();

	yearsListBox->clear();
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* year=gt.rules.yearsList[i];
		yearsListBox->insertItem(year->name);
	}

	yearsListBox->setTopItem(ti);

	yearsListBox->setCurrentItem(ci);
}

void YearsForm::activateStudents()
{
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	
	QString yearName=yearsListBox->currentText();
	int count=gt.rules.activateStudents(yearName);
	QMessageBox::information(this, tr("FET information"), tr("Activated a number of %1 activities").arg(count));
}

void YearsForm::deactivateStudents()
{
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	
	QString yearName=yearsListBox->currentText();
	int count=gt.rules.deactivateStudents(yearName);
	QMessageBox::information(this, tr("FET information"), tr("De-activated a number of %1 activities").arg(count));
}

void YearsForm::divideYear()
{
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	
	QString yearName=yearsListBox->currentText();
	
	SplitYearForm form(yearName);
	form.exec();
}
