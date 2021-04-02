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
    setupUi(this);

    connect(yearsListBox, SIGNAL(highlighted(QString)), this /*GroupsForm_template*/, SLOT(yearChanged(QString)));
    connect(addGroupPushButton, SIGNAL(clicked()), this /*GroupsForm_template*/, SLOT(addGroup()));
    connect(removeGroupPushButton, SIGNAL(clicked()), this /*GroupsForm_template*/, SLOT(removeGroup()));
    connect(closePushButton, SIGNAL(clicked()), this /*GroupsForm_template*/, SLOT(close()));
    connect(groupsListBox, SIGNAL(highlighted(QString)), this /*GroupsForm_template*/, SLOT(groupChanged(QString)));
    connect(modifyGroupPushButton, SIGNAL(clicked()), this /*GroupsForm_template*/, SLOT(modifyGroup()));
    connect(sortGroupsPushButton, SIGNAL(clicked()), this /*GroupsForm_template*/, SLOT(sortGroups()));
    connect(activateStudentsPushButton, SIGNAL(clicked()), this /*GroupsForm_template*/, SLOT(activateStudents()));
    connect(deactivateStudentsPushButton, SIGNAL(clicked()), this /*GroupsForm_template*/, SLOT(deactivateStudents()));
    connect(groupsListBox, SIGNAL(selected(QString)), this /*GroupsForm_template*/, SLOT(modifyGroup()));


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
		yearChanged(yearsListBox->currentText());
	}
	else
		groupsListBox->clear();
}


GroupsForm::~GroupsForm()
{
}

void GroupsForm::addGroup()
{
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListBox->currentText();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	AddStudentsGroupForm form(yearName);
	//form.yearNameLineEdit->setText(yearName);
	form.exec();

	yearChanged(yearsListBox->currentText());
	
	int i=groupsListBox->count()-1;
	if(i>=0){
		groupsListBox->setCurrentItem(i);
		groupsListBox->setSelected(i, true);
	}
}

void GroupsForm::removeGroup()
{
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	int yearIndex=gt.rules.searchYear(yearsListBox->currentText());
	assert(yearIndex>=0);

	if(groupsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}

	QString groupName=groupsListBox->currentText();
	int groupIndex=gt.rules.searchGroup(yearsListBox->currentText(), groupName);
	assert(groupIndex>=0);

	if(QMessageBox::warning( this, tr("FET"),
		tr("Are you sure you want to delete group %1 and all related subgroups, activities and constraints?").arg(groupName),
		tr("Yes"), tr("No"), 0, 0, 1 ) == 1)
		return;

	bool tmp=gt.rules.removeGroup(yearsListBox->currentText(), groupName);
	assert(tmp);
	if(tmp)
		groupsListBox->removeItem(groupsListBox->currentItem());

	if(gt.rules.searchStudentsSet(groupName)!=NULL)
		QMessageBox::information( this, tr("FET"), tr("This group still exists into another year. "
			"The related subgroups, activities and constraints were not removed"));

	if(groupsListBox->count()>0){
		groupsListBox->setSelected(groupsListBox->currentItem(), true);
	}
	else{
		groupTextEdit->setText("");
	}
}

void GroupsForm::yearChanged(const QString &yearName)
{
	int yearIndex=gt.rules.searchYear(yearName);
	if(yearIndex<0){
		groupsListBox->clear();
		groupTextEdit->setText("");
		return;
	}

	groupsListBox->clear();

	StudentsYear* sty=gt.rules.yearsList.at(yearIndex);
	for(int i=0; i<sty->groupsList.size(); i++){
		StudentsGroup* stg=sty->groupsList[i];
		groupsListBox->insertItem(stg->name);
	}

	if(groupsListBox->count()>0){
		groupsListBox->setCurrentItem(0);
		groupsListBox->setSelected(groupsListBox->currentItem(), true);
	}
	else
		groupTextEdit->setText("");
}

void GroupsForm::groupChanged(const QString &groupName)
{
	StudentsSet* ss=gt.rules.searchStudentsSet(groupName);
	if(ss==NULL){
		groupTextEdit->setText("");
		return;
	}
	StudentsGroup* sg=(StudentsGroup*)ss;
	groupTextEdit->setText(sg->getDetailedDescriptionWithConstraints(gt.rules));
}

void GroupsForm::sortGroups()
{
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
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
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	
	QString yearName=yearsListBox->currentText();
	
	int yearIndex=gt.rules.searchYear(yearsListBox->currentText());
	assert(yearIndex>=0);
	
	int ci=groupsListBox->currentItem();
	int ti=groupsListBox->topItem();
	if(groupsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}

	QString groupName=groupsListBox->currentText();
	int groupIndex=gt.rules.searchGroup(yearsListBox->currentText(), groupName);
	assert(groupIndex>=0);

	int numberOfStudents=gt.rules.searchStudentsSet(groupName)->numberOfStudents;
	
	ModifyStudentsGroupForm form(yearName, groupName, numberOfStudents);
	form.exec();

	yearChanged(yearsListBox->currentText());
	
	groupsListBox->setTopItem(ti);
	groupsListBox->setCurrentItem(ci);
}

void GroupsForm::activateStudents()
{
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	int yearIndex=gt.rules.searchYear(yearsListBox->currentText());
	assert(yearIndex>=0);

	if(groupsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}

	QString groupName=groupsListBox->currentText();
	int count=gt.rules.activateStudents(groupName);
	QMessageBox::information(this, tr("FET information"), tr("Activated a number of %1 activities").arg(count));
}

void GroupsForm::deactivateStudents()
{
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	int yearIndex=gt.rules.searchYear(yearsListBox->currentText());
	assert(yearIndex>=0);

	if(groupsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}

	QString groupName=groupsListBox->currentText();
	int count=gt.rules.deactivateStudents(groupName);
	QMessageBox::information(this, tr("FET information"), tr("De-activated a number of %1 activities").arg(count));
}
