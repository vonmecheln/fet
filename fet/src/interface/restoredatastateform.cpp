/***************************************************************************
                          restoredatastateform.cpp  -  description
                             -------------------
    begin                : 2023
    copyright            : (C) 2023 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include "restoredatastateform.h"

#include <QPalette>

#include <QFont>

#include <QMessageBox>

#include <QDir>

#include <QDate>
#include <QTime>
#include <QLocale>

#include <QSettings>

#include <QByteArray>
#include <QDataStream>

extern const QString COMPANY;
extern const QString PROGRAM;

//in rules.cpp
extern int cntUndoRedoStackIterator;
extern std::list<QByteArray> oldRulesArchived; //.front() is oldest, .back() is newest
//extern std::list<QString> operationWhichWasDone; //as above
extern std::list<QByteArray> operationWhichWasDoneArchived; //as above
extern std::list<QPair<QDate, QTime>> operationDateTime; //as above
extern std::list<int> unarchivedSizes; //as above
//extern std::list<QString> stateFileName; //as above

extern int savedStateIterator;

RestoreDataStateForm::RestoreDataStateForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	restoreDataStateTextEdit->setReadOnly(true);

	restoreDataStateListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	QSettings settings(COMPANY, PROGRAM);
	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	//restore splitter state
	if(settings.contains(this->metaObject()->className()+QString("/splitter-state")))
		splitter->restoreState(settings.value(this->metaObject()->className()+QString("/splitter-state")).toByteArray());

	connect(restoreDataStateListWidget, &QListWidget::currentRowChanged, this, &RestoreDataStateForm::restoreDataStateListWidgetSelectionChanged);
	connect(buttonBox, &QDialogButtonBox::accepted, this, &RestoreDataStateForm::ok);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &RestoreDataStateForm::cancel);
	
	QLocale loc(FET_LANGUAGE_WITH_LOCALE);
	std::list<QByteArray>::const_iterator ita=oldRulesArchived.cbegin();
	std::list<int>::const_iterator itu=unarchivedSizes.cbegin();
	int membytes=0;
	int k=0;
	for(const QPair<QDate, QTime>& dt : std::as_const(operationDateTime)){
		QString s=QString::number(k+1)+QString(". ");
	
		k++;
		if(k==cntUndoRedoStackIterator){
			s+=tr("Current state:");
			s+=QString(" ");
		}
		
		if(k==savedStateIterator){
			s+=QString("[");
			s+=tr("Unmodified", "It means that the current data file was either created as a new file, opened from disk, saved, or saved as, and was not modified since then.");
			s+=QString("] ");
		}

		if(!dt.first.isValid() || !dt.second.isValid()){
			s+=tr("Corrupted date/time read from the memory or from the disk (but your data might be valid)")
			 +QString(" ")+tr("If the problem is caused by the history file saved on the disk, you might want to exit FET, remove the corresponding history file"
			 " ending in '%1', open FET again, and open your .fet data file again. Or just ignore the problem, until the history will be replaced with new, valid entries.")
			 .arg(SUFFIX_FILENAME_SAVE_HISTORY);
		}
		else{
			if(dt.first==QDate::currentDate())
				s+=tr("Today")+" "+loc.toString(dt.second, QLocale::ShortFormat);
			else
				s+=loc.toString(dt.first, QLocale::ShortFormat)+" "+loc.toString(dt.second, QLocale::ShortFormat);
		}
		
		s+=" (";
		assert(ita!=oldRulesArchived.cend());
		assert(itu!=unarchivedSizes.cend());
		s+=tr("Compressed size: %1 bytes, uncompressed size: %2 bytes").arg(loc.toString((*ita).size())).arg(loc.toString(*itu));
		membytes+=(*ita).size();
		s+=")";
		
		restoreDataStateListWidget->addItem(s);

		ita++;
		itu++;
	}

#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
	descriptionsArchivedBA=QList<QByteArray>(operationWhichWasDoneArchived.cbegin(), operationWhichWasDoneArchived.cend());
	//fileNames=QList<QString>(stateFileName.cbegin(), stateFileName.cend());
#else
	descriptionsArchivedBA=QList<QByteArray>::fromStdList(operationWhichWasDoneArchived);
	//fileNames=QList<QString>::fromStdList(stateFileName);
#endif

	assert(descriptionsArchivedBA.count()==restoreDataStateListWidget->count());

	if(restoreDataStateListWidget->count()>0){
		assert(cntUndoRedoStackIterator>0);
		assert(cntUndoRedoStackIterator<=restoreDataStateListWidget->count());
		
		restoreDataStateListWidget->setCurrentRow(cntUndoRedoStackIterator-1);
		restoreDataStateListWidget->item(cntUndoRedoStackIterator-1)->setBackground(restoreDataStateListWidget->palette().alternateBase());
		
		QFont font(restoreDataStateListWidget->item(cntUndoRedoStackIterator-1)->font());
		font.setBold(true);
		restoreDataStateListWidget->item(cntUndoRedoStackIterator-1)->setFont(font);
	}
	
	if(restoreDataStateListWidget->count()>0 && savedStateIterator>0){
		assert(savedStateIterator<=restoreDataStateListWidget->count());

		restoreDataStateListWidget->item(savedStateIterator-1)->setBackground(restoreDataStateListWidget->palette().alternateBase());

		QFont font(restoreDataStateListWidget->item(savedStateIterator-1)->font());
		font.setItalic(true);
		restoreDataStateListWidget->item(savedStateIterator-1)->setFont(font);
	}
	
	memoryConsumptionLabel->setText(tr("Memory consumption (sum) = %1 bytes").arg(loc.toString(membytes)));
	
	restoreDataStateListWidget->setFocus();
}

RestoreDataStateForm::~RestoreDataStateForm()
{
	saveFETDialogGeometry(this);
	//save splitter state
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/splitter-state"), splitter->saveState());
}

void RestoreDataStateForm::restoreDataStateListWidgetSelectionChanged()
{
	int i=restoreDataStateListWidget->currentRow();
	if(i>=0 && i<restoreDataStateListWidget->count()){
		QByteArray descriptionBA=qUncompress(descriptionsArchivedBA.at(i));
		//qUncompress(...) should have the same behavior with other (older, and also hopefully newer) versions of Qt, see this Qt function's doc.
		//We need this compatibility for the disk history, where the user might have saved the history on disk from a different Qt version.
		if(descriptionBA.isEmpty()){
			restoreDataStateTextEdit->setPlainText(tr("Corrupted operation details read from the memory or from the disk (but your data might be valid).")
			 +QString("\n\n")+tr("If the problem is caused by the history file saved on the disk, you might want to exit FET, remove the corresponding history file"
			 " ending in '%1', open FET again, and open your .fet data file again. Or just ignore the problem, until the history will be replaced with new, valid entries.")
			 .arg(SUFFIX_FILENAME_SAVE_HISTORY));
		}
		else{
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
			QDataStream dsd(&descriptionBA, QIODeviceBase::ReadOnly);
#else
			QDataStream dsd(&descriptionBA, QIODevice::ReadOnly);
#endif
			QString descr;
			dsd>>descr;
			restoreDataStateTextEdit->setPlainText(descr);
		}
	}
}

void RestoreDataStateForm::ok()
{
	int i=restoreDataStateListWidget->currentRow();
	if(i<0 || i>=restoreDataStateListWidget->count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid restore data point selected."));
		return;
	}

	int itb=cntUndoRedoStackIterator-i-1;
	if(itb==0){
		QMessageBox::information(this, tr("FET information"), tr("You selected to restore to the same state as the current one. Nothing will be done."));
	}
	else{
		/*if(INPUT_FILENAME_XML!=fileNames.at(i))
			QMessageBox::information(this, tr("FET information"), tr("Note: the name of your current file will be changed from %1 to %2.")
			 .arg(INPUT_FILENAME_XML.isEmpty() ? QString("Untitled") : QDir::toNativeSeparators(INPUT_FILENAME_XML))
			 .arg(fileNames.at(i).isEmpty() ? QString("Untitled") : QDir::toNativeSeparators(fileNames.at(i))));*/
		gt.rules.restoreState(this, itb);
	}

	this->close();
}

void RestoreDataStateForm::cancel()
{
	this->close();
}
