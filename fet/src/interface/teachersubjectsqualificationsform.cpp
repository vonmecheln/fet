/***************************************************************************
                          teachersubjectsqualificationsform.cpp  -  description
                             -------------------
    begin                : 2016
    copyright            : (C) 2016 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include <QMessageBox>

#include "longtextmessagebox.h"

#include "teachersubjectsqualificationsform.h"
#include "teacher.h"

#include <QListWidget>
#include <QAbstractItemView>
#include <QSet>

#include <list>
#include <iterator>

TeacherSubjectsQualificationsForm::TeacherSubjectsQualificationsForm(QWidget* parent, Teacher* teacher): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	subjectsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	selectedSubjectsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(cancelPushButton, &QPushButton::clicked, this, &TeacherSubjectsQualificationsForm::close);
	connect(okPushButton, &QPushButton::clicked, this, &TeacherSubjectsQualificationsForm::ok);
	connect(subjectsListWidget, &QListWidget::itemDoubleClicked, this, &TeacherSubjectsQualificationsForm::addSubject);
	connect(selectedSubjectsListWidget, &QListWidget::itemDoubleClicked, this, &TeacherSubjectsQualificationsForm::removeSubject);
	connect(clearPushButton, &QPushButton::clicked, this, &TeacherSubjectsQualificationsForm::clear);
	connect(addAllSubjectsPushButton, &QPushButton::clicked, this, &TeacherSubjectsQualificationsForm::addAllSubjects);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QSet<QString> allSubjects;
	for(Subject* subject : std::as_const(gt.rules.subjectsList)){
		allSubjects.insert(subject->name);
		subjectsListWidget->addItem(subject->name);
	}
	
	for(const QString& subject : std::as_const(teacher->qualifiedSubjectsList)){
		assert(allSubjects.contains(subject));
		selectedSubjectsListWidget->addItem(subject);
	}
	
	teacherTextLabel->setText(tr("Teacher: %1").arg(teacher->name));
	
	_teacher=teacher;
}

TeacherSubjectsQualificationsForm::~TeacherSubjectsQualificationsForm()
{
	saveFETDialogGeometry(this);
}

void TeacherSubjectsQualificationsForm::ok()
{
	//!!!Do not use the commented code below!!!
	//!!!If we used the commented code below (which worked well with QLinkedList, but not with std::list) we would get segmentation fault.
	//Possible steps to reproduce: press OK in this dialog, then rename or remove a subject from this list of qualified subjects.
	/*std::list<QString> newSubjectsList;
	QHash<QString, std::list<QString>::iterator> newSubjectsHash;
	for(int i=0; i<selectedSubjectsListWidget->count(); i++){
		newSubjectsList.push_back(selectedSubjectsListWidget->item(i)->text());
		newSubjectsHash.insert(selectedSubjectsListWidget->item(i)->text(), std::prev(newSubjectsList.end()));
	}
	
	_teacher->qualifiedSubjectsList=newSubjectsList;
	_teacher->qualifiedSubjectsHash=newSubjectsHash;*/

	QString so;
	for(const QString& s : std::as_const(_teacher->qualifiedSubjectsList))
		so+=s+translatedCommaSpace();
	so.chop(translatedCommaSpace().size());

	_teacher->qualifiedSubjectsList.clear();
	_teacher->qualifiedSubjectsHash.clear();
	for(int i=0; i<selectedSubjectsListWidget->count(); i++){
		_teacher->qualifiedSubjectsList.push_back(selectedSubjectsListWidget->item(i)->text());
		_teacher->qualifiedSubjectsHash.insert(selectedSubjectsListWidget->item(i)->text(), std::prev(_teacher->qualifiedSubjectsList.end()));
	}

	QString sn;
	for(const QString& s : std::as_const(_teacher->qualifiedSubjectsList))
		sn+=s+translatedCommaSpace();
	sn.chop(translatedCommaSpace().size());

	gt.rules.addUndoPoint(tr("Changed the list of qualified subjects for teacher %1 from:\n%2\nto\n%3.").arg(_teacher->name).arg(so).arg(sn));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void TeacherSubjectsQualificationsForm::addSubject()
{
	if(subjectsListWidget->currentRow()<0)
		return;
	QString sbName=subjectsListWidget->currentItem()->text();
	assert(sbName!="");
	int i;
	//duplicate?
	for(i=0; i<selectedSubjectsListWidget->count(); i++)
		if(sbName==selectedSubjectsListWidget->item(i)->text())
			break;
	if(i<selectedSubjectsListWidget->count())
		return;
	selectedSubjectsListWidget->addItem(sbName);
	selectedSubjectsListWidget->setCurrentRow(selectedSubjectsListWidget->count()-1);
}

void TeacherSubjectsQualificationsForm::removeSubject()
{
	if(selectedSubjectsListWidget->currentRow()<0 || selectedSubjectsListWidget->count()<=0)
		return;
	int tmp=selectedSubjectsListWidget->currentRow();
	
	selectedSubjectsListWidget->setCurrentRow(-1);
	QListWidgetItem* item=selectedSubjectsListWidget->takeItem(tmp);
	delete item;
	if(tmp<selectedSubjectsListWidget->count())
		selectedSubjectsListWidget->setCurrentRow(tmp);
	else
		selectedSubjectsListWidget->setCurrentRow(selectedSubjectsListWidget->count()-1);
}

void TeacherSubjectsQualificationsForm::clear()
{
	selectedSubjectsListWidget->clear();
}

void TeacherSubjectsQualificationsForm::addAllSubjects()
{
	selectedSubjectsListWidget->clear();
	
	for(int i=0; i<subjectsListWidget->count(); i++)
		selectedSubjectsListWidget->addItem(subjectsListWidget->item(i)->text());
	
	if(selectedSubjectsListWidget->count()>0)
		selectedSubjectsListWidget->setCurrentRow(selectedSubjectsListWidget->count()-1);
}
