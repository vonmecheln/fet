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
    setupUi(this);

    connect(yearsListBox, SIGNAL(highlighted(QString)), this /*SubgroupsForm_template*/, SLOT(yearChanged(QString)));
    connect(groupsListBox, SIGNAL(highlighted(QString)), this /*SubgroupsForm_template*/, SLOT(groupChanged(QString)));
    connect(addSubgroupPushButton, SIGNAL(clicked()), this /*SubgroupsForm_template*/, SLOT(addSubgroup()));
    connect(removeSubgroupPushButton, SIGNAL(clicked()), this /*SubgroupsForm_template*/, SLOT(removeSubgroup()));
    connect(closePushButton, SIGNAL(clicked()), this /*SubgroupsForm_template*/, SLOT(close()));
    connect(subgroupsListBox, SIGNAL(highlighted(QString)), this /*SubgroupsForm_template*/, SLOT(subgroupChanged(QString)));
    connect(modifySubgroupPushButton, SIGNAL(clicked()), this /*SubgroupsForm_template*/, SLOT(modifySubgroup()));
    connect(removeSubgroupPushButton_2, SIGNAL(clicked()), this /*SubgroupsForm_template*/, SLOT(sortSubgroups()));
    connect(activateStudentsPushButton, SIGNAL(clicked()), this /*SubgroupsForm_template*/, SLOT(activateStudents()));
    connect(deactivateStudentsPushButton, SIGNAL(clicked()), this /*SubgroupsForm_template*/, SLOT(deactivateStudents()));
    connect(subgroupsListBox, SIGNAL(selected(QString)), this /*SubgroupsForm_template*/, SLOT(modifySubgroup()));


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
	else{
		groupsListBox->clear();
		subgroupsListBox->clear();
	}
}

SubgroupsForm::~SubgroupsForm()
{
}

void SubgroupsForm::addSubgroup()
{
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListBox->currentText();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	if(groupsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}
	QString groupName=groupsListBox->currentText();
	int groupIndex=gt.rules.searchGroup(yearName, groupName);
	assert(groupIndex>=0);

	AddStudentsSubgroupForm form(yearName, groupName);
	//form.yearNameLineEdit->setText(yearName);
	//form.groupNameLineEdit->setText(groupName);
	form.exec();

	groupChanged(groupsListBox->currentText());
	
	int i=subgroupsListBox->count()-1;
	if(i>=0){
		subgroupsListBox->setCurrentItem(i);
		subgroupsListBox->setSelected(i, true);
	}
}

void SubgroupsForm::removeSubgroup()
{
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListBox->currentText();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	if(groupsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}
	QString groupName=groupsListBox->currentText();
	int groupIndex=gt.rules.searchGroup(yearName, groupName);
	assert(groupIndex>=0);

	if(subgroupsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subgroup"));
		return;
	}
	QString subgroupName=subgroupsListBox->currentText();
	int subgroupIndex=gt.rules.searchSubgroup(yearName, groupName, subgroupName);
	if(subgroupIndex<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subgroup"));
		return;
	}

	if(QMessageBox::warning( this, tr("FET"),
		tr("Are you sure you want to delete subgroup %1 and all related activities and constraints?").arg(subgroupName),
		tr("Yes"), tr("No"), 0, 0, 1 ) == 1)
		return;

	bool tmp=gt.rules.removeSubgroup(yearName, groupName, subgroupName);
	assert(tmp);
	if(tmp)
		subgroupsListBox->removeItem(subgroupsListBox->currentItem());

	if(gt.rules.searchStudentsSet(subgroupName)!=NULL)
		QMessageBox::information( this, tr("FET"), tr("This subgroup still exists into another group. "
		"The related activities and constraints were not removed"));

	if(subgroupsListBox->count()>0){
		subgroupsListBox->setSelected(subgroupsListBox->currentItem(), true);
	}
	else{
		subgroupTextEdit->setText("");
	}
}

void SubgroupsForm::yearChanged(const QString &yearName)
{
	int yearIndex=gt.rules.searchYear(yearName);
	if(yearIndex<0){
		groupsListBox->clear();
		subgroupsListBox->clear();
		subgroupTextEdit->setText("");
		return;
	}
	StudentsYear* sty=gt.rules.yearsList.at(yearIndex);

	groupsListBox->clear();
	for(int i=0; i<sty->groupsList.size(); i++){
		StudentsGroup* stg=sty->groupsList[i];
		groupsListBox->insertItem(stg->name);
	}

	if(groupsListBox->count()>0){
		groupsListBox->setCurrentItem(0);
		groupsListBox->setSelected(groupsListBox->currentItem(), true);
		groupChanged(groupsListBox->currentText());
	}
	else{
		subgroupsListBox->clear();
		subgroupTextEdit->setText("");
	}
}

void SubgroupsForm::groupChanged(const QString &groupName)
{
	QString yearName=yearsListBox->currentText();
	int yearIndex=gt.rules.searchYear(yearName);
	if(yearIndex<0){
		return;
	}
	StudentsYear* sty=gt.rules.yearsList.at(yearIndex);
	int groupIndex=gt.rules.searchGroup(yearName, groupName);
	if(groupIndex<0){
		subgroupsListBox->clear();
		subgroupTextEdit->setText("");
		return;
	}

	StudentsGroup* stg=sty->groupsList.at(groupIndex);

	subgroupsListBox->clear();
	for(int i=0; i<stg->subgroupsList.size(); i++){
		StudentsSubgroup* sts=stg->subgroupsList[i];
		subgroupsListBox->insertItem(sts->name);
	}

	if(subgroupsListBox->count()>0){
		subgroupsListBox->setCurrentItem(0);
		subgroupsListBox->setSelected(subgroupsListBox->currentItem(), true);
	}
	else{
		subgroupsListBox->clear();
		subgroupTextEdit->setText("");
	}
}

void SubgroupsForm::subgroupChanged(const QString &subgroupName)
{
	StudentsSet* ss=gt.rules.searchStudentsSet(subgroupName);
	if(ss==NULL){
		subgroupTextEdit->setText("");
		return;
	}
	StudentsSubgroup* s=(StudentsSubgroup*)ss;
	subgroupTextEdit->setText(s->getDetailedDescriptionWithConstraints(gt.rules));
}

void SubgroupsForm::sortSubgroups()
{
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListBox->currentText();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	if(groupsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
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
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListBox->currentText();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	if(groupsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}
	QString groupName=groupsListBox->currentText();
	int groupIndex=gt.rules.searchGroup(yearName, groupName);
	assert(groupIndex>=0);

	int ci=subgroupsListBox->currentItem();
	int ti=subgroupsListBox->topItem();
	if(subgroupsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subgroup"));
		return;
	}
	QString subgroupName=subgroupsListBox->currentText();
	int subgroupIndex=gt.rules.searchSubgroup(yearName, groupName, subgroupName);
	if(subgroupIndex<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subgroup"));
		return;
	}
	
	int numberOfStudents=gt.rules.searchStudentsSet(subgroupName)->numberOfStudents;
	
	ModifyStudentsSubgroupForm form(yearName, groupName, subgroupName, numberOfStudents);
	form.exec();

	groupChanged(groupName);
	
	subgroupsListBox->setTopItem(ti);
	subgroupsListBox->setCurrentItem(ci);
}

void SubgroupsForm::activateStudents()
{
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListBox->currentText();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	if(groupsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}
	QString groupName=groupsListBox->currentText();
	int groupIndex=gt.rules.searchGroup(yearName, groupName);
	assert(groupIndex>=0);

	if(subgroupsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subgroup"));
		return;
	}
	
	QString subgroupName=subgroupsListBox->currentText();
	int count=gt.rules.activateStudents(subgroupName);
	QMessageBox::information(this, tr("FET information"), tr("Activated a number of %1 activities").arg(count));
}

void SubgroupsForm::deactivateStudents()
{
	if(yearsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListBox->currentText();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	if(groupsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}
	QString groupName=groupsListBox->currentText();
	int groupIndex=gt.rules.searchGroup(yearName, groupName);
	assert(groupIndex>=0);

	if(subgroupsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subgroup"));
		return;
	}
	
	QString subgroupName=subgroupsListBox->currentText();
	int count=gt.rules.deactivateStudents(subgroupName);
	QMessageBox::information(this, tr("FET information"), tr("De-activated a number of %1 activities").arg(count));
}
