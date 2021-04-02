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
#include "activitytagsform.h"
//#include "fetmainform.h"
#include "studentsset.h"
#include "teacher.h"
#include "subject.h"
#include "activitytag.h"

#include <q3listbox.h>
#include <qinputdialog.h>
#include <q3textedit.h>

#include <QDesktopWidget>

#include <QMessageBox>

ActivityTagsForm::ActivityTagsForm()
 : ActivityTagsForm_template()
{
    setupUi(this);

    connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(addActivityTagPushButton, SIGNAL(clicked()), this, SLOT(addActivityTag()));
    connect(removeActivityTagPushButton, SIGNAL(clicked()), this, SLOT(removeActivityTag()));
    connect(renameActivityTagPushButton, SIGNAL(clicked()), this, SLOT(renameActivityTag()));
    connect(sortActivityTagsPushButton, SIGNAL(clicked()), this, SLOT(sortActivityTags()));
    connect(activityTagsListBox, SIGNAL(highlighted(int)), this, SLOT(activityTagChanged(int)));
    connect(activateActivityTagPushButton, SIGNAL(clicked()), this, SLOT(activateActivityTag()));
    connect(deactivateActivityTagPushButton, SIGNAL(clicked()), this, SLOT(deactivateActivityTag()));
    connect(activityTagsListBox, SIGNAL(selected(QString)), this, SLOT(renameActivityTag()));
    connect(helpPushButton, SIGNAL(clicked()), this, SLOT(help()));

	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
		
	activityTagsListBox->clear();
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* sbt=gt.rules.activityTagsList[i];
		activityTagsListBox->insertItem(sbt->name);
	}
		
	if(activityTagsListBox->count()>0){
		activityTagsListBox->setCurrentItem(0);
		this->activityTagChanged(0);
	}
}


ActivityTagsForm::~ActivityTagsForm()
{
}

void ActivityTagsForm::addActivityTag()
{
	bool ok = FALSE;
	ActivityTag* sbt=new ActivityTag();
	sbt->name = QInputDialog::getText( tr("User input"), tr("Please enter activity tag's name") ,
                    QLineEdit::Normal, QString::null, &ok, this );

	if ( ok && !((sbt->name).isEmpty()) ){
		// user entered something and pressed OK
		if(!gt.rules.addActivityTag(sbt)){
			QMessageBox::information( this, tr("Activity tag insertion dialog"),
				tr("Could not insert item. Must be a duplicate"));
			delete sbt;
		}
		else{
			activityTagsListBox->insertItem(sbt->name);
			activityTagsListBox->setCurrentItem(activityTagsListBox->count()-1);
			this->activityTagChanged(activityTagsListBox->count()-1);
		}
	}
	else
		delete sbt;// user entered nothing or pressed Cancel
}

void ActivityTagsForm::removeActivityTag()
{
	int i=activityTagsListBox->currentItem();
	if(activityTagsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected activity tag"));
		return;
	}

	QString text=activityTagsListBox->currentText();
	int activity_tag_ID=gt.rules.searchActivityTag(text);
	if(activity_tag_ID<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected activity tag"));
		return;
	}

	if(QMessageBox::warning( this, tr("FET"),
		tr("Are you sure you want to delete this activity tag?"),
		tr("Yes"), tr("No"), 0, 0, 1 ) == 1)
		return;

	int tmp=gt.rules.removeActivityTag(text);
	if(tmp){
		activityTagsListBox->removeItem(activityTagsListBox->currentItem());
		if((uint)(i)>=activityTagsListBox->count())
			i=activityTagsListBox->count()-1;
		activityTagsListBox->setCurrentItem(i);
		this->activityTagChanged(i);
	}
}

void ActivityTagsForm::renameActivityTag()
{
	int i=activityTagsListBox->currentItem();
	if(activityTagsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected activity tag"));
		return;
	}
	
	QString initialActivityTagName=activityTagsListBox->currentText();

	int activity_tag_ID=gt.rules.searchActivityTag(initialActivityTagName);
	if(activity_tag_ID<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected activity tag"));
		return;
	}

	bool ok = FALSE;
	QString finalActivityTagName;
	finalActivityTagName = QInputDialog::getText( tr("User input"), tr("Please enter new activity tag's name") ,
                    QLineEdit::Normal, initialActivityTagName, &ok, this );

	if ( ok && !(finalActivityTagName.isEmpty()) ){
		// user entered something and pressed OK
		if(gt.rules.searchActivityTag(finalActivityTagName)>=0){
			QMessageBox::information( this, tr("Activity tag insertion dialog"),
				tr("Could not modify item. New name must be a duplicate"));
		}
		else{
			gt.rules.modifyActivityTag(initialActivityTagName, finalActivityTagName);
			activityTagsListBox->changeItem(finalActivityTagName, i);
		}
	}
}

void ActivityTagsForm::sortActivityTags()
{
	gt.rules.sortActivityTagsAlphabetically();

	activityTagsListBox->clear();
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* sbt=gt.rules.activityTagsList[i];
		activityTagsListBox->insertItem(sbt->name);
	}
}

void ActivityTagsForm::activityTagChanged(int index)
{
	if(index<0){
		currentActivityTagTextEdit->setText(tr("Invalid activity tag"));
		return;
	}
	
	ActivityTag* st=gt.rules.activityTagsList.at(index);
	assert(st);
	QString s=st->getDetailedDescriptionWithConstraints(gt.rules);
	currentActivityTagTextEdit->setText(s);
}

void ActivityTagsForm::activateActivityTag()
{
	if(activityTagsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected activity tag"));
		return;
	}

	QString text=activityTagsListBox->currentText();
	int count=gt.rules.activateActivityTag(text);
	QMessageBox::information(this, tr("FET information"), tr("Activated a number of %1 activities").arg(count));
}

void ActivityTagsForm::deactivateActivityTag()
{
	if(activityTagsListBox->currentItem()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected activity tag"));
		return;
	}

	QString text=activityTagsListBox->currentText();
	int count=gt.rules.deactivateActivityTag(text);
	QMessageBox::information(this, tr("FET information"), tr("De-activated a number of %1 activities").arg(count));
}

void ActivityTagsForm::help()
{
	QMessageBox::information(this, tr("FET help on activity tags"), 
	 tr("Activity tag is a field which can be used or not, depending on your wish (optional field)."
	 " It is designed to help you with some constraints. Each activity has a possible empty list of activity tags"
	 " (if you don't use activity tags, the list will be empty)"));
}
