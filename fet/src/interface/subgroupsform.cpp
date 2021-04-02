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
#include "addstudentssubgroupform.h"
#include "modifystudentssubgroupform.h"
#include "subgroupsform.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"
//#include "fetmainform.h"

#include <q3listbox.h>
#include <qinputdialog.h>

#include <QDesktopWidget>

#include <QMessageBox>

SubgroupsForm::SubgroupsForm()
 : SubgroupsForm_template()
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

	yearChanged(yearsListBox->currentText());
}

SubgroupsForm::~SubgroupsForm()
{
}

void SubgroupsForm::addSubgroup()
{
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListBox->currentText();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	if(groupsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected group"));
		return;
	}
	QString groupName=groupsListBox->currentText();
	int groupIndex=gt.rules.searchGroup(yearName, groupName);
	assert(groupIndex>=0);

	AddStudentsSubgroupForm* form=new AddStudentsSubgroupForm();
	form->yearNameLineEdit->setText(yearName);
	form->groupNameLineEdit->setText(groupName);
	form->exec();

	groupChanged(groupsListBox->currentText());
}

void SubgroupsForm::removeSubgroup()
{
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListBox->currentText();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	if(groupsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected group"));
		return;
	}
	QString groupName=groupsListBox->currentText();
	int groupIndex=gt.rules.searchGroup(yearName, groupName);
	assert(groupIndex>=0);

	if(subgroupsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected subgroup"));
		return;
	}
	QString subgroupName=subgroupsListBox->currentText();
	int subgroupIndex=gt.rules.searchSubgroup(yearName, groupName, subgroupName);
	if(subgroupIndex<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected subgroup"));
		return;
	}

	if(QMessageBox::warning( this, QObject::tr("FET"),
		QObject::tr("Are you sure you want to delete this subgroup and all related activities and constraints?\n"),
		QObject::tr("Yes"), QObject::tr("No"), 0, 0, 1 ) == 1)
		return;

	bool tmp=gt.rules.removeSubgroup(yearName, groupName, subgroupName);
	assert(tmp);
	if(tmp)
		subgroupsListBox->removeItem(subgroupsListBox->currentItem());
	this->show();
}

void SubgroupsForm::yearChanged(const QString &yearName)
{
	int yearIndex=gt.rules.searchYear(yearName);
	if(yearIndex<0){
		if(gt.rules.yearsList.size()>0)
			yearIndex=0;
		else	
			return;
	}
	StudentsYear* sty=gt.rules.yearsList.at(yearIndex);

	groupsListBox->clear();
	for(int i=0; i<sty->groupsList.size(); i++){
		StudentsGroup* stg=sty->groupsList[i];
		groupsListBox->insertItem(stg->name);
	}

	groupChanged(groupsListBox->currentText());
}

void SubgroupsForm::groupChanged(const QString &groupName)
{
	QString yearName=yearsListBox->currentText();
	int yearIndex=gt.rules.searchYear(yearName);
	if(yearIndex<0){
		if(gt.rules.yearsList.size()>0){
			yearIndex=0;
			yearName=gt.rules.yearsList.at(0)->name;
		}
		else	
			return;
	}
	StudentsYear* sty=gt.rules.yearsList.at(yearIndex);
	int groupIndex=gt.rules.searchGroup(yearName, groupName);
	if(groupIndex<0){
		if(sty->groupsList.size()>0)
			groupIndex=0;
		else
			return;
	}

	StudentsGroup* stg=sty->groupsList.at(groupIndex);

	subgroupsListBox->clear();
	for(int i=0; i<stg->subgroupsList.size(); i++){
		StudentsSubgroup* sts=stg->subgroupsList[i];
		subgroupsListBox->insertItem(sts->name);
	}
}

void SubgroupsForm::subgroupChanged(const QString &subgroupName)
{
	StudentsSet* ss=gt.rules.searchStudentsSet(subgroupName);
	if(ss==NULL)
		return;
	StudentsSubgroup* s=(StudentsSubgroup*)ss;
	subgroupTextEdit->setText(s->getDetailedDescriptionWithConstraints(gt.rules));
}

void SubgroupsForm::sortSubgroups()
{
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListBox->currentText();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	if(groupsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected group"));
		return;
	}
	QString groupName=groupsListBox->currentText();
	int groupIndex=gt.rules.searchGroup(yearName, groupName);
	assert(groupIndex>=0);
	
	gt.rules.sortSubgroupsAlphabetically(yearName, groupName);
	
	groupChanged(groupName);
}

void SubgroupsForm::modifySubgroup()
{
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListBox->currentText();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	if(groupsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected group"));
		return;
	}
	QString groupName=groupsListBox->currentText();
	int groupIndex=gt.rules.searchGroup(yearName, groupName);
	assert(groupIndex>=0);

	int ci=subgroupsListBox->currentItem();
	if(subgroupsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected subgroup"));
		return;
	}
	QString subgroupName=subgroupsListBox->currentText();
	int subgroupIndex=gt.rules.searchSubgroup(yearName, groupName, subgroupName);
	if(subgroupIndex<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected subgroup"));
		return;
	}
	
	int numberOfStudents=gt.rules.searchStudentsSet(subgroupName)->numberOfStudents;
	
	ModifyStudentsSubgroupForm* form=new ModifyStudentsSubgroupForm(yearName, groupName, subgroupName, numberOfStudents);
	form->exec();

	groupChanged(groupName);
	
	subgroupsListBox->setCurrentItem(ci);
}

void SubgroupsForm::activateStudents()
{
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListBox->currentText();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	if(groupsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected group"));
		return;
	}
	QString groupName=groupsListBox->currentText();
	int groupIndex=gt.rules.searchGroup(yearName, groupName);
	assert(groupIndex>=0);

	if(subgroupsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected subgroup"));
		return;
	}
	
	QString subgroupName=subgroupsListBox->currentText();
	int count=gt.rules.activateStudents(subgroupName);
	QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Activated a number of %1 activities").arg(count));
}

void SubgroupsForm::deactivateStudents()
{
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListBox->currentText();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	if(groupsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected group"));
		return;
	}
	QString groupName=groupsListBox->currentText();
	int groupIndex=gt.rules.searchGroup(yearName, groupName);
	assert(groupIndex>=0);

	if(subgroupsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected subgroup"));
		return;
	}
	
	QString subgroupName=subgroupsListBox->currentText();
	int count=gt.rules.deactivateStudents(subgroupName);
	QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("De-activated a number of %1 activities").arg(count));
}
