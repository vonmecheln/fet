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
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"
#include "subjectsform.h"
//#include "fetmainform.h"
#include "studentsset.h"
#include "teacher.h"
#include "subject.h"

#include <q3listbox.h>
#include <qinputdialog.h>
#include <q3textedit.h>

#include <QDesktopWidget>

#include <QMessageBox>

SubjectsForm::SubjectsForm()
 : SubjectsForm_template()
{
    setupUi(this);

    connect(closePushButton, SIGNAL(clicked()), this /*SubjectsForm_template*/, SLOT(close()));
    connect(addSubjectPushButton, SIGNAL(clicked()), this /*SubjectsForm_template*/, SLOT(addSubject()));
    connect(removeSubjectPushButton, SIGNAL(clicked()), this /*SubjectsForm_template*/, SLOT(removeSubject()));
    connect(renameSubjectPushButton, SIGNAL(clicked()), this /*SubjectsForm_template*/, SLOT(renameSubject()));
    connect(sortSubjectsPushButton, SIGNAL(clicked()), this /*SubjectsForm_template*/, SLOT(sortSubjects()));
    connect(subjectsListBox, SIGNAL(highlighted(int)), this /*SubjectsForm_template*/, SLOT(subjectChanged(int)));
    connect(activateSubjectPushButton, SIGNAL(clicked()), this /*SubjectsForm_template*/, SLOT(activateSubject()));
    connect(deactivateSubjectPushButton, SIGNAL(clicked()), this /*SubjectsForm_template*/, SLOT(deactivateSubject()));
    connect(subjectsListBox, SIGNAL(selected(QString)), this /*SubjectsForm_template*/, SLOT(renameSubject()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	subjectsListBox->clear();
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* sbj=gt.rules.subjectsList[i];
		subjectsListBox->insertItem(sbj->name);
	}
		
	if(subjectsListBox->count()>0){
		subjectsListBox->setCurrentItem(0);
		this->subjectChanged(0);
	}
}


SubjectsForm::~SubjectsForm()
{
}

void SubjectsForm::addSubject()
{
	bool ok = FALSE;
	Subject* sbj=new Subject();
	sbj->name = QInputDialog::getText( tr("User input"), tr("Please enter subject's name") ,
                    QLineEdit::Normal, QString::null, &ok, this );

	if ( ok && !((sbj->name).isEmpty()) ){
		// user entered something and pressed OK
		if(!gt.rules.addSubject(sbj)){
			QMessageBox::information( this, tr("Subject insertion dialog"),
				tr("Could not insert item. Must be a duplicate"));
			delete sbj;
		}
		else{
			subjectsListBox->insertItem(sbj->name);
			subjectsListBox->setCurrentItem(subjectsListBox->count()-1);
			this->subjectChanged(subjectsListBox->count()-1);
		}
	}
	else
		delete sbj;// user entered nothing or pressed Cancel
}

void SubjectsForm::removeSubject()
{
	int i=subjectsListBox->currentItem();
	if(subjectsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subject"));
		return;
	}

	QString text=subjectsListBox->currentText();
	int subject_ID=gt.rules.searchSubject(text);
	if(subject_ID<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subject"));
		return;
	}

	if(QMessageBox::warning( this, tr("FET"),
		tr("Are you sure you want to delete this subject and all related activities and constraints?"),
		tr("Yes"), tr("No"), 0, 0, 1 ) == 1)
		return;

	int tmp=gt.rules.removeSubject(text);
	if(tmp){
		subjectsListBox->removeItem(subjectsListBox->currentItem());
		if((uint)(i)>=subjectsListBox->count())
			i=subjectsListBox->count()-1;
		subjectsListBox->setCurrentItem(i);
		this->subjectChanged(i);
	}
}

void SubjectsForm::renameSubject()
{
	int i=subjectsListBox->currentItem();
	if(subjectsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subject"));
		return;
	}
	
	QString initialSubjectName=subjectsListBox->currentText();

	int subject_ID=gt.rules.searchSubject(initialSubjectName);
	if(subject_ID<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subject"));
		return;
	}

	bool ok = FALSE;
	QString finalSubjectName;
	finalSubjectName = QInputDialog::getText( tr("User input"), tr("Please enter new subject's name") ,
                    QLineEdit::Normal, initialSubjectName, &ok, this );

	if ( ok && !(finalSubjectName.isEmpty()) ){
		// user entered something and pressed OK
		if(gt.rules.searchSubject(finalSubjectName)>=0){
			QMessageBox::information( this, tr("Subject insertion dialog"),
				tr("Could not modify item. New name must be a duplicate"));
		}
		else{
			gt.rules.modifySubject(initialSubjectName, finalSubjectName);
			subjectsListBox->changeItem(finalSubjectName, i);
		}
	}
}

void SubjectsForm::sortSubjects()
{
	gt.rules.sortSubjectsAlphabetically();
	
	subjectsListBox->clear();
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* sbj=gt.rules.subjectsList[i];
		subjectsListBox->insertItem(sbj->name);
	}
}

void SubjectsForm::subjectChanged(int index)
{
	if(index<0){
		currentSubjectTextEdit->setText(tr("Invalid subject"));
		return;
	}
	
	Subject* sb=gt.rules.subjectsList.at(index);
	assert(sb);
	QString s=sb->getDetailedDescriptionWithConstraints(gt.rules);
	currentSubjectTextEdit->setText(s);
}

void SubjectsForm::activateSubject()
{
	if(subjectsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subject"));
		return;
	}
	
	QString subjectName=subjectsListBox->currentText();
	
	int count=gt.rules.activateSubject(subjectName);
	QMessageBox::information(this, tr("FET information"), tr("Activated a number of %1 activities").arg(count));
}

void SubjectsForm::deactivateSubject()
{
	if(subjectsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subject"));
		return;
	}
	
	QString subjectName=subjectsListBox->currentText();
	
	int count=gt.rules.deactivateSubject(subjectName);
	QMessageBox::information(this, tr("FET information"), tr("De-activated a number of %1 activities").arg(count));
}
