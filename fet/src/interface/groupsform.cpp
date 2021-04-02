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
#include "addstudentsgroupform.h"
#include "modifystudentsgroupform.h"
#include "groupsform.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"
//#include "fetmainform.h"
#include "studentsset.h"

#include <q3listbox.h>
#include <qinputdialog.h>
#include <qstring.h>

#include <QDesktopWidget>

#include <QMessageBox>

GroupsForm::GroupsForm()
 : GroupsForm_template()
{
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

	yearChanged(yearsListBox->currentText());
}


GroupsForm::~GroupsForm()
{
}

void GroupsForm::addGroup()
{
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListBox->currentText();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	AddStudentsGroupForm* form=new AddStudentsGroupForm();
	form->yearNameLineEdit->setText(yearName);
	form->exec();

	yearChanged(yearsListBox->currentText());
}

void GroupsForm::removeGroup()
{
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected year"));
		return;
	}
	int yearIndex=gt.rules.searchYear(yearsListBox->currentText());
	assert(yearIndex>=0);

	if(groupsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected group"));
		return;
	}

	QString groupName=groupsListBox->currentText();
	int groupIndex=gt.rules.searchGroup(yearsListBox->currentText(), groupName);
	assert(groupIndex>=0);

	if(QMessageBox::warning( this, QObject::tr("FET"),
		QObject::tr("Are you sure you want to delete this group and all related subgroup, activities and constraints?\n"),
		QObject::tr("Yes"), QObject::tr("No"), 0, 0, 1 ) == 1)
		return;

	bool tmp=gt.rules.removeGroup(yearsListBox->currentText(), groupName);
	assert(tmp);
	if(tmp)
		groupsListBox->removeItem(groupsListBox->currentItem());

	if(gt.rules.searchStudentsSet(groupName)!=NULL)
		QMessageBox::information( this, QObject::tr("FET"), QObject::tr("This group still exists into another year\n"
			"The related subgroups, activities and constraints were not removed"));

	this->show();
}

void GroupsForm::yearChanged(const QString &yearName)
{
	int yearIndex=gt.rules.searchYear(yearName);
	if(yearIndex<0){
		if(gt.rules.yearsList.size()>0){
			yearIndex=0;
		}
		else
			return;
	}

	groupsListBox->clear();

	StudentsYear* sty=gt.rules.yearsList.at(yearIndex);
	for(int i=0; i<sty->groupsList.size(); i++){
		StudentsGroup* stg=sty->groupsList[i];
		groupsListBox->insertItem(stg->name);
	}
}

void GroupsForm::groupChanged(const QString &groupName)
{
	StudentsSet* ss=gt.rules.searchStudentsSet(groupName);
	if(ss==NULL)
		return;
	StudentsGroup* sg=(StudentsGroup*)ss;
	groupTextEdit->setText(sg->getDetailedDescriptionWithConstraints(gt.rules));
}

void GroupsForm::sortGroups()
{
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected year"));
		return;
	}
	int yearIndex=gt.rules.searchYear(yearsListBox->currentText());
	assert(yearIndex>=0);
	
	gt.rules.sortGroupsAlphabetically(yearsListBox->currentText());

	yearChanged(yearsListBox->currentText());
}

void GroupsForm::modifyGroup()
{
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected year"));
		return;
	}
	
	QString yearName=yearsListBox->currentText();
	
	int yearIndex=gt.rules.searchYear(yearsListBox->currentText());
	assert(yearIndex>=0);
	
	int ci=groupsListBox->currentItem();
	if(groupsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected group"));
		return;
	}

	QString groupName=groupsListBox->currentText();
	int groupIndex=gt.rules.searchGroup(yearsListBox->currentText(), groupName);
	assert(groupIndex>=0);

	int numberOfStudents=gt.rules.searchStudentsSet(groupName)->numberOfStudents;
	
	ModifyStudentsGroupForm* form=new ModifyStudentsGroupForm(yearName, groupName, numberOfStudents);
	form->exec();

	yearChanged(yearsListBox->currentText());
	
	groupsListBox->setCurrentItem(ci);
}

void GroupsForm::activateStudents()
{
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected year"));
		return;
	}
	int yearIndex=gt.rules.searchYear(yearsListBox->currentText());
	assert(yearIndex>=0);

	if(groupsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected group"));
		return;
	}

	QString groupName=groupsListBox->currentText();
	int count=gt.rules.activateStudents(groupName);
	QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Activated a number of %1 activities").arg(count));
}

void GroupsForm::deactivateStudents()
{
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected year"));
		return;
	}
	int yearIndex=gt.rules.searchYear(yearsListBox->currentText());
	assert(yearIndex>=0);

	if(groupsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected group"));
		return;
	}

	QString groupName=groupsListBox->currentText();
	int count=gt.rules.deactivateStudents(groupName);
	QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("De-activated a number of %1 activities").arg(count));
}
