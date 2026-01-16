/***************************************************************************
                          modifysubactivityform.cpp  -  description
                             -------------------
    begin                : Feb 9, 2005
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

#include "modifysubactivityform.h"
#include "teacher.h"
#include "subject.h"
#include "studentsset.h"

#include "activityplanningform.h"

#include <QMessageBox>

#include <QListWidget>
#include <QAbstractItemView>
#include <QScrollBar>

#include <QTextEdit>

#include <QSettings>
#include <QObject>
#include <QMetaObject>

extern const QString COMPANY;
extern const QString PROGRAM;

ModifySubactivityForm::ModifySubactivityForm(QWidget* parent, int id, int activityGroupId): QDialog(parent)
{
	setupUi(this);

	for(Teacher* tch : std::as_const(gt.rules.teachersList))
		teachersNamesSet.insert(tch->name);
	for(Subject* sbj : std::as_const(gt.rules.subjectsList))
		subjectsNamesSet.insert(sbj->name);
	for(ActivityTag* at : std::as_const(gt.rules.activityTagsList))
		activityTagsNamesSet.insert(at->name);

	allTeachersListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	selectedTeachersListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	allStudentsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	selectedStudentsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	allActivityTagsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	selectedActivityTagsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	QSettings settings(COMPANY, PROGRAM);

	showSubgroupsCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-subgroups-check-box-state"), "false").toBool());
	showGroupsCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-groups-check-box-state"), "true").toBool());
	showYearsCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-years-check-box-state"), "true").toBool());

	allTeachersRadioButton->setChecked(settings.value(this->metaObject()->className()+QString("/all-teachers-radio-button-state"), "true").toBool());
	qualifiedTeachersRadioButton->setChecked(settings.value(this->metaObject()->className()+QString("/qualified-teachers-radio-button-state"), "false").toBool());

	connect(cancelPushButton, &QPushButton::clicked, this, &ModifySubactivityForm::cancel);
	connect(okPushButton, &QPushButton::clicked, this, &ModifySubactivityForm::ok);
	connect(clearTeachersPushButton, &QPushButton::clicked, this, &ModifySubactivityForm::clearTeachers);
	connect(clearStudentsPushButton, &QPushButton::clicked, this, &ModifySubactivityForm::clearStudents);
	connect(allTeachersListWidget, &QListWidget::itemDoubleClicked, this, &ModifySubactivityForm::addTeacher);
	connect(selectedTeachersListWidget, &QListWidget::itemDoubleClicked, this, &ModifySubactivityForm::removeTeacher);
	connect(allStudentsListWidget, &QListWidget::itemDoubleClicked, this, &ModifySubactivityForm::addStudents);
	connect(selectedStudentsListWidget, &QListWidget::itemDoubleClicked, this, &ModifySubactivityForm::removeStudents);
	connect(clearActivityTagsPushButton, &QPushButton::clicked, this, &ModifySubactivityForm::clearActivityTags);
	connect(allActivityTagsListWidget, &QListWidget::itemDoubleClicked, this, &ModifySubactivityForm::addActivityTag);
	connect(selectedActivityTagsListWidget, &QListWidget::itemDoubleClicked, this, &ModifySubactivityForm::removeActivityTag);
	connect(showYearsCheckBox, &QCheckBox::toggled, this, &ModifySubactivityForm::showYearsChanged);
	connect(showGroupsCheckBox, &QCheckBox::toggled, this, &ModifySubactivityForm::showGroupsChanged);
	connect(showSubgroupsCheckBox, &QCheckBox::toggled, this, &ModifySubactivityForm::showSubgroupsChanged);
	connect(helpPushButton, &QPushButton::clicked, this, &ModifySubactivityForm::help);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp3=subjectsComboBox->minimumSizeHint();
	Q_UNUSED(tmp3);
	
	int splitIndex=0;
	int nSplit=0;
	bool found=false;
	
	this->_id=id;
	this->_activityGroupId=activityGroupId;
	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		Activity* act=gt.rules.activitiesList[i];
		if(act->activityGroupId==this->_activityGroupId && act->id==this->_id){
			this->_activity=act;
			splitIndex++;
			nSplit++;
			found=true;
		}
		else if(this->_activityGroupId!=0 && act->activityGroupId!=0 && act->activityGroupId==this->_activityGroupId){
			if(!found)
				splitIndex++;
			nSplit++;
		}
	}
	
	crtTextLabel->setText(tr("Current subactivity has split index %1 out of %2").arg(splitIndex).arg(nSplit)+
	 "<br />"+tr("The current data applies only to this subactivity"));

	this->_teachers=this->_activity->teachersNames;
	this->_subject = this->_activity->subjectName;
	this->_activityTags = this->_activity->activityTagsNames;
	this->_students=this->_activity->studentsNames;
	
	durationSpinBox->setMaximum(gt.rules.nHoursPerDay);
	durationSpinBox->setValue(this->_activity->duration);
	activeCheckBox->setChecked(this->_activity->active);

	nStudentsSpinBox->setMinimum(-1);
	nStudentsSpinBox->setMaximum(MAX_ROOM_CAPACITY);
	nStudentsSpinBox->setValue(-1);
	
	if(this->_activity->computeNTotalStudents==false)
		nStudentsSpinBox->setValue(this->_activity->nTotalStudents);
	
	updateStudentsListWidget();
	updateSubjectsComboBox();
	updateActivityTagsListWidget();

	//after updateSubjectsComboBox
	connect(subjectsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ModifySubactivityForm::updateAllTeachersListWidget);
	connect(allTeachersRadioButton, &QRadioButton::toggled, this, &ModifySubactivityForm::allTeachersRadioButtonToggled);
	connect(qualifiedTeachersRadioButton, &QRadioButton::toggled, this, &ModifySubactivityForm::qualifiedTeachersRadioButtonToggled);
	updateAllTeachersListWidget();
	
	selectedTeachersListWidget->clear();
	for(QStringList::const_iterator it=this->_teachers.constBegin(); it!=this->_teachers.constEnd(); it++)
		selectedTeachersListWidget->addItem(*it);

	selectedStudentsListWidget->clear();
	for(QStringList::const_iterator it=this->_students.constBegin(); it!=this->_students.constEnd(); it++)
		selectedStudentsListWidget->addItem(*it);
	
	okPushButton->setDefault(true);
	okPushButton->setFocus();
}

ModifySubactivityForm::~ModifySubactivityForm()
{
	saveFETDialogGeometry(this);

	QSettings settings(COMPANY, PROGRAM);

	settings.setValue(this->metaObject()->className()+QString("/show-subgroups-check-box-state"), showSubgroupsCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/show-groups-check-box-state"), showGroupsCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/show-years-check-box-state"), showYearsCheckBox->isChecked());

	settings.setValue(this->metaObject()->className()+QString("/qualified-teachers-radio-button-state"), qualifiedTeachersRadioButton->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/all-teachers-radio-button-state"), allTeachersRadioButton->isChecked());
}

void ModifySubactivityForm::allTeachersRadioButtonToggled(bool toggled)
{
	if(toggled)
		updateAllTeachersListWidget();
}

void ModifySubactivityForm::qualifiedTeachersRadioButtonToggled(bool toggled)
{
	if(toggled)
		updateAllTeachersListWidget();
}

void ModifySubactivityForm::updateAllTeachersListWidget()
{
	allTeachersListWidget->clear();
	
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		if(allTeachersRadioButton->isChecked() || subjectsComboBox->currentIndex()==-1){
			allTeachersListWidget->addItem(tch->name);
		}
		else{
			assert(qualifiedTeachersRadioButton->isChecked());
			assert(subjectsComboBox->currentText()!="");
			assert(subjectsNamesSet.contains(subjectsComboBox->currentText()));
			if(tch->qualifiedSubjectsHash.contains(subjectsComboBox->currentText())){
				allTeachersListWidget->addItem(tch->name);
			}
		}
	}
}

void ModifySubactivityForm::addTeacher()
{
	if(allTeachersListWidget->currentRow()<0 || allTeachersListWidget->currentRow()>=allTeachersListWidget->count())
		return;

	for(int i=0; i<selectedTeachersListWidget->count(); i++)
		if(selectedTeachersListWidget->item(i)->text()==allTeachersListWidget->currentItem()->text())
			return;

	selectedTeachersListWidget->addItem(allTeachersListWidget->currentItem()->text());
	selectedTeachersListWidget->setCurrentRow(selectedTeachersListWidget->count()-1);
}

void ModifySubactivityForm::removeTeacher()
{
	if(selectedTeachersListWidget->count()<=0 || selectedTeachersListWidget->currentRow()<0 ||
	 selectedTeachersListWidget->currentRow()>=selectedTeachersListWidget->count())
		return;

	int i=selectedTeachersListWidget->currentRow();
	selectedTeachersListWidget->setCurrentRow(-1);
	QListWidgetItem* item=selectedTeachersListWidget->takeItem(i);
	delete item;
	if(i<selectedTeachersListWidget->count())
		selectedTeachersListWidget->setCurrentRow(i);
	else
		selectedTeachersListWidget->setCurrentRow(selectedTeachersListWidget->count()-1);
}

void ModifySubactivityForm::addStudents()
{
	if(allStudentsListWidget->currentRow()<0 || allStudentsListWidget->currentRow()>=allStudentsListWidget->count())
		return;

	assert(canonicalStudentsSetsNames.count()==allStudentsListWidget->count());
	QString sn=canonicalStudentsSetsNames.at(allStudentsListWidget->currentRow());

	for(int i=0; i<selectedStudentsListWidget->count(); i++)
		if(selectedStudentsListWidget->item(i)->text()==sn)
			return;

	selectedStudentsListWidget->addItem(sn);
	selectedStudentsListWidget->setCurrentRow(selectedStudentsListWidget->count()-1);
}

void ModifySubactivityForm::removeStudents()
{
	if(selectedStudentsListWidget->count()<=0 || selectedStudentsListWidget->currentRow()<0 ||
	 selectedStudentsListWidget->currentRow()>=selectedStudentsListWidget->count())
		return;

	int i=selectedStudentsListWidget->currentRow();
	selectedStudentsListWidget->setCurrentRow(-1);
	QListWidgetItem* item=selectedStudentsListWidget->takeItem(i);
	delete item;
	if(i<selectedStudentsListWidget->count())
		selectedStudentsListWidget->setCurrentRow(i);
	else
		selectedStudentsListWidget->setCurrentRow(selectedStudentsListWidget->count()-1);
}

void ModifySubactivityForm::addActivityTag()
{
	if(allActivityTagsListWidget->currentRow()<0 || allActivityTagsListWidget->currentRow()>=allActivityTagsListWidget->count())
		return;

	for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
		if(selectedActivityTagsListWidget->item(i)->text()==allActivityTagsListWidget->currentItem()->text())
			return;

	selectedActivityTagsListWidget->addItem(allActivityTagsListWidget->currentItem()->text());
	selectedActivityTagsListWidget->setCurrentRow(selectedActivityTagsListWidget->count()-1);
}

void ModifySubactivityForm::removeActivityTag()
{
	if(selectedActivityTagsListWidget->count()<=0 || selectedActivityTagsListWidget->currentRow()<0 ||
	 selectedActivityTagsListWidget->currentRow()>=selectedActivityTagsListWidget->count())
		return;

	int i=selectedActivityTagsListWidget->currentRow();
	selectedActivityTagsListWidget->setCurrentRow(-1);
	QListWidgetItem* item=selectedActivityTagsListWidget->takeItem(i);
	delete item;
	if(i<selectedActivityTagsListWidget->count())
		selectedActivityTagsListWidget->setCurrentRow(i);
	else
		selectedActivityTagsListWidget->setCurrentRow(selectedActivityTagsListWidget->count()-1);
}

void ModifySubactivityForm::updateSubjectsComboBox()
{
	int i=0, j=-1;
	subjectsComboBox->clear();
	for(int k=0; k<gt.rules.subjectsList.size(); k++, i++){
		Subject* sbj=gt.rules.subjectsList[k];
		subjectsComboBox->addItem(sbj->name);
		if(sbj->name==this->_subject)
			j=i;
	}
	assert(j!=-1);
	subjectsComboBox->setCurrentIndex(j);
}

void ModifySubactivityForm::updateActivityTagsListWidget()
{
	allActivityTagsListWidget->clear();
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* at=gt.rules.activityTagsList[i];
		allActivityTagsListWidget->addItem(at->name);
	}
		
	selectedActivityTagsListWidget->clear();
	for(QStringList::const_iterator it=this->_activityTags.constBegin(); it!=this->_activityTags.constEnd(); it++)
		selectedActivityTagsListWidget->addItem(*it);
}

void ModifySubactivityForm::showYearsChanged()
{
	updateStudentsListWidget();
}

void ModifySubactivityForm::showGroupsChanged()
{
	updateStudentsListWidget();
}

void ModifySubactivityForm::showSubgroupsChanged()
{
	updateStudentsListWidget();
}

void ModifySubactivityForm::updateStudentsListWidget()
{
	const int INDENT=2;

	bool showYears=showYearsCheckBox->isChecked();
	bool showGroups=showGroupsCheckBox->isChecked();
	bool showSubgroups=showSubgroupsCheckBox->isChecked();

	allStudentsListWidget->clear();
	canonicalStudentsSetsNames.clear();
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* sty=gt.rules.yearsList[i];
		if(showYears){
			allStudentsListWidget->addItem(sty->name);
			canonicalStudentsSetsNames.append(sty->name);
		}
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			if(showGroups){
				QString begin=QString("");
				QString end=QString("");
				begin=QString(INDENT, ' ');
				allStudentsListWidget->addItem(begin+stg->name+end);
				canonicalStudentsSetsNames.append(stg->name);
			}
			if(showSubgroups) for(int k=0; k<stg->subgroupsList.size(); k++){
				StudentsSubgroup* sts=stg->subgroupsList[k];

				QString begin=QString("");
				QString end=QString("");
				begin=QString(2*INDENT, ' ');
				allStudentsListWidget->addItem(begin+sts->name+end);
				canonicalStudentsSetsNames.append(sts->name);
			}
		}
	}

	int q=allStudentsListWidget->verticalScrollBar()->minimum();
	allStudentsListWidget->verticalScrollBar()->setValue(q);
}

void ModifySubactivityForm::cancel()
{
	this->reject();
}

void ModifySubactivityForm::ok()
{
	//subject
	QString subject_name=subjectsComboBox->currentText();
	/*int subject_index=gt.rules.searchSubject(subject_name);
	if(subject_index<0){*/
	bool found=subjectsNamesSet.contains(subject_name);
	if(!found){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid subject"));
		return;
	}

	//activity tag
	QStringList activity_tags_names;
	for(int i=0; i<selectedActivityTagsListWidget->count(); i++){
		//assert(gt.rules.searchActivityTag(selectedActivityTagsListWidget->item(i)->text())>=0);
		assert(activityTagsNamesSet.contains(selectedActivityTagsListWidget->item(i)->text()));
		activity_tags_names.append(selectedActivityTagsListWidget->item(i)->text());
	}

	//teachers
	QStringList teachers_names;
	if(selectedTeachersListWidget->count()<=0){
		int t=QMessageBox::question(this, tr("FET question"),
		 tr("Do you really want to have the subactivity without teacher(s)?"),
		 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

		if(t==QMessageBox::No)
			return;
	}
	else{
		for(int i=0; i<selectedTeachersListWidget->count(); i++){
			//assert(gt.rules.searchTeacher(selectedTeachersListWidget->item(i)->text())>=0);
			assert(teachersNamesSet.contains(selectedTeachersListWidget->item(i)->text()));
			teachers_names.append(selectedTeachersListWidget->item(i)->text());
		}
	}

	//students
	int numberOfStudents=0;
	QStringList students_names;
	if(selectedStudentsListWidget->count()<=0){
		int t=QMessageBox::question(this, tr("FET question"),
		 tr("Do you really want to have the subactivity without student set(s)?"),
		  QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

		if(t==QMessageBox::No)
			return;
	}
	else{
		for(int i=0; i<selectedStudentsListWidget->count(); i++){
			//assert(gt.rules.searchStudentsSet(selectedStudentsListWidget->item(i)->text())!=nullptr);
			/*assert(numberOfStudentsHash.contains(selectedStudentsListWidget->item(i)->text()));
			numberOfStudents+=numberOfStudentsHash.value(selectedStudentsListWidget->item(i)->text());*/
			assert(gt.rules.permanentStudentsHash.contains(selectedStudentsListWidget->item(i)->text()));
			numberOfStudents+=gt.rules.permanentStudentsHash.value(selectedStudentsListWidget->item(i)->text())->numberOfStudents;
			students_names.append(selectedStudentsListWidget->item(i)->text());
		}
	}

	/*int total_number_of_students=0;
	for(QStringList::Iterator it=students_names.begin(); it!=students_names.end(); it++){
		StudentsSet* ss=gt.rules.searchStudentsSet(*it);
		assert(ss!=nullptr);
		total_number_of_students+=ss->numberOfStudents;
	}*/
	int total_number_of_students=numberOfStudents;

	QString od=this->_activity->getDetailedDescription(gt.rules);

	if(nStudentsSpinBox->value()==-1){
		gt.rules.modifySubactivity(this->_id, this->_activityGroupId, teachers_names, subject_name,
		 activity_tags_names,students_names, durationSpinBox->value(), activeCheckBox->isChecked(),
		 (nStudentsSpinBox->value()==-1), total_number_of_students);
	}
	else{
		gt.rules.modifySubactivity(this->_id, this->_activityGroupId, teachers_names, subject_name,
		 activity_tags_names,students_names, durationSpinBox->value(), activeCheckBox->isChecked(),
		 (nStudentsSpinBox->value()==-1), nStudentsSpinBox->value());
	}
	
	PlanningChanged::increasePlanningCommunicationSpinBox();
	
	QString nd=this->_activity->getDetailedDescription(gt.rules);
	gt.rules.addUndoPoint(tr("Modified the subactivity with id=%1.").arg(this->_id)
	 +QString("\n\n")+tr("The old description was:\n%1").arg(od)
	 +QString("\n")+tr("The new description was:\n%1", "It is 'was', not 'is', because it is a recorded history (undo/redo) point.").arg(nd));

	this->accept();
}

void ModifySubactivityForm::clearTeachers()
{
	selectedTeachersListWidget->clear();
}

void ModifySubactivityForm::clearStudents()
{
	selectedStudentsListWidget->clear();
}

void ModifySubactivityForm::clearActivityTags()
{
	selectedActivityTagsListWidget->clear();
}

void ModifySubactivityForm::help()
{
	QString s;
	
	s+=tr("Abbreviations in this dialog:");
	s+="\n\n";
	s+=tr("'Students' (the text near the spin box), means 'Number of students (-1 for automatic)'");
	s+="\n";
	s+=tr("'Qualified' means that only the teachers who are qualified to teach the selected subject will be shown in the 'Teachers' list.",
	 "Qualified refers to teachers");
	
	//show the message in a dialog
	QDialog dialog(this);
	
	dialog.setWindowTitle(tr("FET - help on modifying subactivity(ies)"));

	QVBoxLayout* vl=new QVBoxLayout(&dialog);
	QTextEdit* te=new QTextEdit();
	te->setPlainText(s);
	te->setReadOnly(true);
	QPushButton* pb=new QPushButton(tr("OK"));

	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch(1);
	hl->addWidget(pb);

	vl->addWidget(te);
	vl->addLayout(hl);
	connect(pb, &QPushButton::clicked, &dialog, &QDialog::close);

	dialog.resize(600,470);
	centerWidgetOnScreen(&dialog);

	setParentAndOtherThings(&dialog, this);
	dialog.exec();
}
