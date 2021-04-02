//
//
// C++ Implementation: $MODULE$
//
// Description:
//
//
// Author: Lalescu Liviu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"
#include "subjecttagsform.h"
//#include "fetmainform.h"
#include "studentsset.h"
#include "teacher.h"
#include "subject.h"
#include "subjecttag.h"

#include <q3listbox.h>
#include <qinputdialog.h>
#include <q3textedit.h>

#include <QDesktopWidget>

#include <QMessageBox>

SubjectTagsForm::SubjectTagsForm()
 : SubjectTagsForm_template()
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	subjectTagsListBox->clear();
	for(int i=0; i<gt.rules.subjectTagsList.size(); i++){
		SubjectTag* sbt=gt.rules.subjectTagsList[i];
		subjectTagsListBox->insertItem(sbt->name);
	}
		
	if(subjectTagsListBox->count()>0){
		subjectTagsListBox->setCurrentItem(0);
		this->subjectTagChanged(0);
	}
}


SubjectTagsForm::~SubjectTagsForm()
{
}

void SubjectTagsForm::addSubjectTag()
{
	bool ok = FALSE;
	SubjectTag* sbt=new SubjectTag();
	sbt->name = QInputDialog::getText( QObject::tr("User input"), QObject::tr("Please enter subject tag's name") ,
                    QLineEdit::Normal, QString::null, &ok, this );

	if ( ok && !((sbt->name).isEmpty()) ){
		// user entered something and pressed OK
		if(!gt.rules.addSubjectTag(sbt)){
			QMessageBox::information( this, QObject::tr("Subject tag insertion dialog"),
				QObject::tr("Could not insert item. Must be a duplicate"));
			delete sbt;
		}
		else{
			subjectTagsListBox->insertItem(sbt->name);
			subjectTagsListBox->setCurrentItem(subjectTagsListBox->count()-1);
			this->subjectTagChanged(subjectTagsListBox->count()-1);
		}
	}
	else
		delete sbt;// user entered nothing or pressed Cancel
}

void SubjectTagsForm::removeSubjectTag()
{
	int i=subjectTagsListBox->currentItem();
	if(subjectTagsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected subject tag"));
		return;
	}

	QString text=subjectTagsListBox->currentText();
	int subject_tag_ID=gt.rules.searchSubjectTag(text);
	if(subject_tag_ID<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected subject tag"));
		return;
	}

	if(QMessageBox::warning( this, QObject::tr("FET"),
		QObject::tr("Are you sure you want to delete this subject tag?\n"),
		QObject::tr("Yes"), QObject::tr("No"), 0, 0, 1 ) == 1)
		return;

	int tmp=gt.rules.removeSubjectTag(text);
	if(tmp){
		subjectTagsListBox->removeItem(subjectTagsListBox->currentItem());
		if((uint)(i)>=subjectTagsListBox->count())
			i=subjectTagsListBox->count()-1;
		subjectTagsListBox->setCurrentItem(i);
		this->subjectTagChanged(i);
	}
}

void SubjectTagsForm::renameSubjectTag()
{
	int i=subjectTagsListBox->currentItem();
	if(subjectTagsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected subject tag"));
		return;
	}
	
	QString initialSubjectTagName=subjectTagsListBox->currentText();

	int subject_tag_ID=gt.rules.searchSubjectTag(initialSubjectTagName);
	if(subject_tag_ID<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected subject tag"));
		return;
	}

	bool ok = FALSE;
	QString finalSubjectTagName;
	finalSubjectTagName = QInputDialog::getText( QObject::tr("User input"), QObject::tr("Please enter new subject tag's name") ,
                    QLineEdit::Normal, initialSubjectTagName, &ok, this );

	if ( ok && !(finalSubjectTagName.isEmpty()) ){
		// user entered something and pressed OK
		if(gt.rules.searchSubjectTag(finalSubjectTagName)>=0){
			QMessageBox::information( this, QObject::tr("Subject tag insertion dialog"),
				QObject::tr("Could not modify item. New name must be a duplicate"));
		}
		else{
			gt.rules.modifySubjectTag(initialSubjectTagName, finalSubjectTagName);
			subjectTagsListBox->changeItem(finalSubjectTagName, i);
		}
	}
}

void SubjectTagsForm::sortSubjectTags()
{
	gt.rules.sortSubjectTagsAlphabetically();

	subjectTagsListBox->clear();
	for(int i=0; i<gt.rules.subjectTagsList.size(); i++){
		SubjectTag* sbt=gt.rules.subjectTagsList[i];
		subjectTagsListBox->insertItem(sbt->name);
	}
}

void SubjectTagsForm::subjectTagChanged(int index)
{
	if(index<0){
		currentSubjectTagTextEdit->setText(QObject::tr("Invalid subject tag"));
		return;
	}
	
	SubjectTag* st=gt.rules.subjectTagsList.at(index);
	assert(st);
	QString s=st->getDetailedDescriptionWithConstraints(gt.rules);
	currentSubjectTagTextEdit->setText(s);
}

void SubjectTagsForm::activateSubjectTag()
{
	if(subjectTagsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected subject tag"));
		return;
	}

	QString text=subjectTagsListBox->currentText();
	int count=gt.rules.activateSubjectTag(text);
	QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Activated a number of %1 activities").arg(count));
}

void SubjectTagsForm::deactivateSubjectTag()
{
	if(subjectTagsListBox->currentItem()<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected subject tag"));
		return;
	}

	QString text=subjectTagsListBox->currentText();
	int count=gt.rules.deactivateSubjectTag(text);
	QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("De-activated a number of %1 activities").arg(count));
}

void SubjectTagsForm::help()
{
	QMessageBox::information(this, QObject::tr("FET help on subject tags"), 
	 QObject::tr("Subject tag is a field which can be used or not, depending on your wish (optional field)."
	 " It is designed to help you with some constraints. Each activity has a possible empty subject tag"
	 " (if you don't use subject tags, it will be empty)"));
}
