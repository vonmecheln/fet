/***************************************************************************
                          modifystudentsyearform.cpp  -  description
                             -------------------
    begin                : Feb 8, 2005
    copyright            : (C) 2005 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include <Qt>

#include <QMessageBox>

#include "modifystudentsyearform.h"

#include "longtextmessagebox.h"

ModifyStudentsYearForm::ModifyStudentsYearForm(QWidget* parent, const QString& initialYearName, int initialNumberOfStudents): QDialog(parent)
{
	setupUi(this);
	
	okPushButton->setDefault(true);

	connect(okPushButton, &QPushButton::clicked, this, &ModifyStudentsYearForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyStudentsYearForm::cancel);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	numberSpinBox->setMaximum(MAX_ROOM_CAPACITY);
	numberSpinBox->setMinimum(0);
	numberSpinBox->setValue(0);

	this->_initialYearName=initialYearName;
	this->_initialNumberOfStudents=initialNumberOfStudents;
	numberSpinBox->setValue(initialNumberOfStudents);
	nameLineEdit->setText(initialYearName);
	nameLineEdit->selectAll();
	nameLineEdit->setFocus();
}

ModifyStudentsYearForm::~ModifyStudentsYearForm()
{
	saveFETDialogGeometry(this);
}

void ModifyStudentsYearForm::cancel()
{
	this->close();
}

void ModifyStudentsYearForm::ok()
{
	if(nameLineEdit->text().isEmpty()){
		QMessageBox::information(this, tr("FET information"), tr("Incorrect name"));
		return;
	}
	if(this->_initialYearName!=nameLineEdit->text() && gt.rules.searchStudentsSet(nameLineEdit->text())!=nullptr){
		QMessageBox::information(this, tr("FET information"), tr("Name existing - please choose another"));

		nameLineEdit->selectAll();
		nameLineEdit->setFocus();

		return;
	}

	if(this->_initialYearName==nameLineEdit->text()){
		QString od=tr("Year name=%1\nNumber of students=%2").arg(this->_initialYearName).arg(this->_initialNumberOfStudents);

		bool t=gt.rules.modifyStudentsSet(this->_initialYearName, nameLineEdit->text(), numberSpinBox->value());
		assert(t);

		QString nd=tr("Year name=%1\nNumber of students=%2").arg(nameLineEdit->text()).arg(numberSpinBox->value());
		gt.rules.addUndoPoint(tr("The year with the description:\n\n%1\nwas modified into\n\n%2").arg(od).arg(nd));
	}
	else{
		//rename groups and subgroups by Volker Dirr (start)
		//prepare checks
		QSet<QString> oldNames;
		for(StudentsYear* year : std::as_const(gt.rules.yearsList)){
			oldNames<<year->name;
			for(StudentsGroup* group : std::as_const(year->groupsList)){
				oldNames<<group->name;
				for(StudentsSubgroup* subgroup : std::as_const(group->subgroupsList)){
					oldNames<<subgroup->name;
				}
			}
		}
		
		//do checks
		QString wontBeRenamed1;
		QString wontBeRenamed2;
		QString wontBeRenamed3;
		QString willBeRenamed;
		QSet<QString> alreadyRenamed;
		QHash<QString, QString> oldAndNewStudentsSetNamesForRenaming;
		for(StudentsYear* year : std::as_const(gt.rules.yearsList)){
			if(this->_initialYearName != year->name){
				for(StudentsGroup* group : std::as_const(year->groupsList)){
					if(group->name.left(this->_initialYearName.length())==this->_initialYearName){
						wontBeRenamed1+=tr("%1 in %2", "For instance group '1 a' in year '1'").arg(group->name).arg(year->name)+"\n";
						//It's correct for example if there is year "1" and year "10"
					}
					for(StudentsSubgroup* subgroup : std::as_const(group->subgroupsList)){
						if(subgroup->name.left(this->_initialYearName.length())==this->_initialYearName){
							wontBeRenamed1+=tr("%1 in %2 in %3", "For instance subgroup '1 a DE' in group '1 a' in year '1'").arg(subgroup->name).arg(group->name).arg(year->name)+"\n";
							//It's correct for example if there is year "1" and year "10"
						}
					}
				}
			} else {
				for(StudentsGroup* group : std::as_const(year->groupsList)){
					for(StudentsSubgroup* subgroup : std::as_const(group->subgroupsList)){
						if(subgroup->name.left(this->_initialYearName.length())!=this->_initialYearName){
							wontBeRenamed2+=tr("%1 in %2 in %3", "For instance subgroup '1 a DE' in group '1 a' in year '1'").arg(subgroup->name).arg(group->name).arg(year->name)+"\n";
						} else {
							QString tmpName=subgroup->name;
							tmpName.remove(0, this->_initialYearName.length());
							assert(!tmpName.isEmpty());
							QString newName=nameLineEdit->text();
							newName.append(tmpName);
							if(oldNames.contains(newName)){
								wontBeRenamed3+=newName+"\n";
							} else {
								if(!alreadyRenamed.contains(newName)){
									alreadyRenamed<<newName;
									willBeRenamed+=subgroup->name+" -> "+newName+"\n";
									assert(subgroup->name!=newName);
									oldAndNewStudentsSetNamesForRenaming.insert(subgroup->name, newName);
								}
							}
						}
					}
					if(group->name.left(this->_initialYearName.length())!=this->_initialYearName){
						wontBeRenamed2+=tr("%1 in %2", "For instance group '1 a' in year '1'").arg(group->name).arg(year->name)+"\n";
					} else {
						QString tmpName=group->name;
						tmpName.remove(0, this->_initialYearName.length());
						assert(!tmpName.isEmpty());
						QString newName=nameLineEdit->text();
						newName.append(tmpName);
						if(oldNames.contains(newName)){
							wontBeRenamed3+=newName+"\n";
						} else {
							if(!alreadyRenamed.contains(newName)){
								alreadyRenamed<<newName;
								willBeRenamed+=group->name+" -> "+newName+"\n";
								assert(group->name!=newName);
								oldAndNewStudentsSetNamesForRenaming.insert(group->name, newName);
							}
						}
					}
				}
				assert(!alreadyRenamed.contains(nameLineEdit->text()));
				assert(year->name!=nameLineEdit->text());
			}
		}
		
		bool warningsOrRenaming=false;
		
		if(!wontBeRenamed1.isEmpty()){
			wontBeRenamed1.prepend(tr("The following groups and subgroups of other years won't be renamed, even if they start with the same year name:")+"\n\n");
			warningsOrRenaming=true;
		}
		if(!wontBeRenamed2.isEmpty()){
			wontBeRenamed2.prepend(tr("The following groups and subgroups of this year won't be renamed, because they don't start with the year name:")+"\n\n");
			warningsOrRenaming=true;
		}
		if(!wontBeRenamed3.isEmpty()){
			wontBeRenamed3.prepend(tr("The following groups and subgroups of this year won't be renamed, because their computed new name already exists:")+"\n\n");
			warningsOrRenaming=true;
		}
		if(!willBeRenamed.isEmpty()){
			willBeRenamed.prepend(tr("The following groups and subgroups of this year will be renamed:")+"\n\n");
			warningsOrRenaming=true;
		}
		
		QString message="";
		message+=tr("FET can also try to rename the groups and the subgroups of this year. Should they be renamed?");
		message+=QString("\n\n");
		message+=tr("Please check the following information carefully:");
		message+=QString("\n\n");
		if(!wontBeRenamed1.isEmpty()){
			message+=wontBeRenamed1;
			message+=QString("\n");
		}
		if(!wontBeRenamed2.isEmpty()){
			message+=wontBeRenamed2;
			message+=QString("\n");
		}
		if(!wontBeRenamed3.isEmpty()){
			message+=wontBeRenamed3;
			message+=QString("\n");
		}
		if(!willBeRenamed.isEmpty()){
			message+=willBeRenamed;
			message+=QString("\n");
		}
		message=message.trimmed();
		
		//int result=LongTextMessageBox::largeConfirmation( this, tr("FET question"), message, tr("Yes"), tr("No"), QString(), 0 , 1 );
		
		/*QMessageBox msgBox(this);
		msgBox.setTextFormat(Qt::PlainText);
		msgBox.setWindowTitle(tr("FET question"));
		msgBox.setText(tr("FET can also try to rename the groups and the subgroups of this year. Should they be renamed?"));
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Yes);
		msgBox.setEscapeButton(QMessageBox::Cancel);
		msgBox.setDetailedText(message);
		int result = msgBox.exec();
		
		if(result==QMessageBox::Yes){
			bool t=gt.rules.modifyStudentsSets(oldAndNewStudentsSetNamesForRenaming);
			assert(t);
			t=gt.rules.modifyStudentsSet(this->_initialYearName, nameLineEdit->text(), numberSpinBox->value());
			assert(t);
		} else if(result==QMessageBox::No) {
		//rename groups and subgroups by Volker Dirr (end)
			bool t=gt.rules.modifyStudentsSet(this->_initialYearName, nameLineEdit->text(), numberSpinBox->value());
			assert(t);
		} else {
			assert(result==QMessageBox::Cancel);
			return;
		}*/
		
		int result;
		if(warningsOrRenaming){
			result=LongTextMessageBox::largeConfirmationWithDimensionsThreeButtonsYesNoCancel( this, tr("FET question"), message, tr("Yes"), tr("No"), tr("Cancel"), 0 , 2 );
		} else result=QMessageBox::No;

		if(result==QMessageBox::Yes){
			QString od=tr("Year name=%1\nNumber of students=%2").arg(this->_initialYearName).arg(this->_initialNumberOfStudents);

			bool t=gt.rules.modifyStudentsSets(oldAndNewStudentsSetNamesForRenaming);
			assert(t);
			t=gt.rules.modifyStudentsSet(this->_initialYearName, nameLineEdit->text(), numberSpinBox->value());
			assert(t);

			QString nd=tr("Year name=%1\nNumber of students=%2").arg(nameLineEdit->text()).arg(numberSpinBox->value());
			gt.rules.addUndoPoint(tr("The year (some or all of its groups and subgroups were renamed to match its new name) "
			 "with description:\n\n%1\nwas modified into\n\n%2").arg(od).arg(nd));
		} else if(result==QMessageBox::No) {
		//rename groups and subgroups by Volker Dirr (end)
			QString od=tr("Year name=%1\nNumber of students=%2").arg(this->_initialYearName).arg(this->_initialNumberOfStudents);

			bool t=gt.rules.modifyStudentsSet(this->_initialYearName, nameLineEdit->text(), numberSpinBox->value());
			assert(t);

			QString nd=tr("Year name=%1\nNumber of students=%2").arg(nameLineEdit->text()).arg(numberSpinBox->value());
			gt.rules.addUndoPoint(tr("The year with the description:\n\n%1\nwas modified into\n\n%2").arg(od).arg(nd));
		} else {
			assert(result==QMessageBox::Cancel);
			return;
		}
	}

	this->close();
}
