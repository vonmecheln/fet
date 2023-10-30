/***************************************************************************
                          timetableviewteacherstimehorizontalform.cpp  -  description
                             -------------------
    begin                : 2023
    copyright            : (C) 2023 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include <QHeaderView>
#include <QTableWidget>
#include <QTableWidgetItem>

#include <QAbstractItemView>

#include <QBrush>
#include <QColor>

#include <QPainter>

#include <QSettings>

#include "teachersnotavailabletimestimehorizontalform.h"
#include "timeconstraint.h"

#define YES		(QString("X"))
#define NO		(QString(" "))

extern const QString COMPANY;
extern const QString PROGRAM;

extern const int MINIMUM_WIDTH_SPIN_BOX_VALUE;
extern const int MINIMUM_HEIGHT_SPIN_BOX_VALUE;

void TeachersNotAvailableTimesTimeHorizontalDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QStyledItemDelegate::paint(painter, option, index);

	//int day=index.column()/gt.rules.nHoursPerDay;
	//int hour=index.column()%gt.rules.nHoursPerDay;
	int hour=index.column()%nColumns;

	/*if(day>=0 && day<gt.rules.nDaysPerWeek-1 && hour==gt.rules.nHoursPerDay-1){
		QPen pen(painter->pen());
		pen.setWidth(2);
		painter->setPen(pen);
		painter->drawLine(option.rect.topRight(), option.rect.bottomRight());
	}*/

	/*assert(table!=nullptr);
	QBrush bg(table->item(index.row(), index.column())->background());
	QPen pen(painter->pen());

	double brightness = bg.color().redF()*0.299 + bg.color().greenF()*0.587 + bg.color().blueF()*0.114;
	if (brightness<0.5)
		pen.setColor(Qt::white);
	else
		pen.setColor(Qt::black);

	painter->setPen(pen);*/

	if(hour==0)
		painter->drawLine(option.rect.topLeft(), option.rect.bottomLeft());
	if(hour==nColumns-1)
		painter->drawLine(option.rect.topRight(), option.rect.bottomRight());

	if(index.row()==0)
		painter->drawLine(option.rect.topLeft(), option.rect.topRight());
	if(index.row()==nRows-1)
		painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
}

TeachersNotAvailableTimesTimeHorizontalForm::TeachersNotAvailableTimesTimeHorizontalForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	naTableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

	tnaMatrix.resize(gt.rules.teachersList.count(), gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	for(int t=0; t<gt.rules.teachersList.count(); t++){
		for(int d=0; d<gt.rules.nDaysPerWeek; d++)
			for(int h=0; h<gt.rules.nHoursPerDay; h++)
				tnaMatrix[t][d][h]=false;

		QSet<ConstraintTeacherNotAvailableTimes*> stc=gt.rules.tnatHash.value(gt.rules.teachersList.at(t)->name, QSet<ConstraintTeacherNotAvailableTimes*>());
		assert(stc.count()<=1);
		if(!stc.isEmpty()){
			ConstraintTeacherNotAvailableTimes* ctr=*stc.constBegin();
			for(int i=0; i<ctr->days.count(); i++){
				int d=ctr->days.at(i);
				int h=ctr->hours.at(i);
				tnaMatrix[t][d][h]=true;
			}
		}
	}

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(ok()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancel()));
	connect(naTableWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(itemClicked(QTableWidgetItem*)));
	connect(selectedPushButton, SIGNAL(clicked()), this, SLOT(selectedClicked()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	naTableWidget->setRowCount(gt.rules.teachersList.count());
	naTableWidget->setColumnCount(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	naTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	naTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
#else
	naTableWidget->verticalHeader()->setResizeMode(QHeaderView::Interactive);
	naTableWidget->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
#endif

	oldItemDelegate=naTableWidget->itemDelegate();
	newItemDelegate=new TeachersNotAvailableTimesTimeHorizontalDelegate(nullptr, naTableWidget->rowCount(), gt.rules.nHoursPerDay);
	naTableWidget->setItemDelegate(newItemDelegate);

	bool min2letters=false;
	for(int d=0; d<gt.rules.nDaysPerWeek; d++){
		if(gt.rules.daysOfTheWeek[d].size()>gt.rules.nHoursPerDay){
			min2letters=true;
			break;
		}
	}

	for(int d=0; d<gt.rules.nDaysPerWeek; d++){
		QString dayName=gt.rules.daysOfTheWeek[d];
		int t=dayName.size();
		int q=t/gt.rules.nHoursPerDay;
		int r=t%gt.rules.nHoursPerDay;
		QStringList list;
		
		if(q==0)
			q=1;
		
		for(int i=0; i<gt.rules.nHoursPerDay; i++){
			if(!min2letters){
				list.append(dayName.left(1));
				dayName.remove(0, 1);
			}
			else if(i<r || q<=1){
				assert(q>=1);
				list.append(dayName.left(q+1));
				dayName.remove(0, q+1);
			}
			else{
				list.append(dayName.left(q));
				dayName.remove(0, q);
			}
		}
	
		for(int h=0; h<gt.rules.nHoursPerDay; h++){
			QTableWidgetItem* item=new QTableWidgetItem(list.at(h)+"\n"+gt.rules.hoursOfTheDay[h]);
			item->setToolTip(gt.rules.daysOfTheWeek[d]+"\n"+gt.rules.hoursOfTheDay[h]);
			naTableWidget->setHorizontalHeaderItem(d*gt.rules.nHoursPerDay+h, item);
		}
	}
	for(int t=0; t<gt.rules.teachersList.count(); t++){
		QTableWidgetItem* item=new QTableWidgetItem(gt.rules.teachersList.at(t)->name);
		item->setToolTip(gt.rules.teachersList[t]->name);
		naTableWidget->setVerticalHeaderItem(t, item);
	}

	for(int t=0; t<gt.rules.teachersList.count(); t++){
		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			for(int h=0; h<gt.rules.nHoursPerDay; h++){
				QTableWidgetItem* item;
				if(tnaMatrix[t][d][h])
					item=new QTableWidgetItem(YES);
				else
					item=new QTableWidgetItem(NO);
				item->setTextAlignment(Qt::AlignCenter);
				item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
				colorItem(item);
				if(SHOW_TOOLTIPS_FOR_CONSTRAINTS_WITH_TABLES)
					item->setToolTip(gt.rules.teachersList.at(t)->name+QString("\n")+gt.rules.daysOfTheWeek[d]+QString("\n")+gt.rules.hoursOfTheDay[h]);
				naTableWidget->setItem(t, d*gt.rules.nHoursPerDay+h, item);
			}
		}
	}

	initialRecommendedHeight=naTableWidget->verticalHeader()->sectionSizeHint(0);

	int h;
	int w;

	QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/vertical-header-size"))){
		h=settings.value(this->metaObject()->className()+QString("/vertical-header-size")).toInt();
		if(h==0)
			h=MINIMUM_HEIGHT_SPIN_BOX_VALUE;
	}
	else{
		h=MINIMUM_HEIGHT_SPIN_BOX_VALUE;
	}
//	if(h==0)
//		h=initialRecommendedHeight;

	if(settings.contains(this->metaObject()->className()+QString("/horizontal-header-size"))){
		w=settings.value(this->metaObject()->className()+QString("/horizontal-header-size")).toInt();
		if(w==0)
			w=MINIMUM_WIDTH_SPIN_BOX_VALUE;
	}
	else{
		w=MINIMUM_WIDTH_SPIN_BOX_VALUE;
	}
//	if(w==0)
//		w=2*initialRecommendedHeight;

	widthSpinBox->setSuffix(QString(" ")+tr("px", "Abbreviation for pixels"));
	widthSpinBox->setMinimum(MINIMUM_WIDTH_SPIN_BOX_VALUE);
#if QT_VERSION >= QT_VERSION_CHECK(5,2,0)
	widthSpinBox->setMaximum(naTableWidget->verticalHeader()->maximumSectionSize());
#else
	widthSpinBox->setMaximum(maxScreenWidth(this));
#endif
	widthSpinBox->setValue(w);
	widthSpinBox->setSpecialValueText(tr("Automatic"));

	heightSpinBox->setSuffix(QString(" ")+tr("px", "Abbreviation for pixels"));
	heightSpinBox->setMinimum(MINIMUM_HEIGHT_SPIN_BOX_VALUE);
#if QT_VERSION >= QT_VERSION_CHECK(5,2,0)
	heightSpinBox->setMaximum(naTableWidget->verticalHeader()->maximumSectionSize());
#else
	heightSpinBox->setMaximum(maxScreenWidth(this));
#endif
	heightSpinBox->setValue(h);
	heightSpinBox->setSpecialValueText(tr("Automatic"));

	naTableWidget->horizontalHeader()->setMinimumSectionSize(MINIMUM_WIDTH_SPIN_BOX_VALUE);
	naTableWidget->verticalHeader()->setMinimumSectionSize(MINIMUM_HEIGHT_SPIN_BOX_VALUE);

	widthSpinBoxValueChanged();
	heightSpinBoxValueChanged();

	connect(widthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(widthSpinBoxValueChanged()));
	connect(heightSpinBox, SIGNAL(valueChanged(int)), this, SLOT(heightSpinBoxValueChanged()));
}

TeachersNotAvailableTimesTimeHorizontalForm::~TeachersNotAvailableTimesTimeHorizontalForm()
{
	saveFETDialogGeometry(this);

	QSettings settings(COMPANY, PROGRAM);

	if(heightSpinBox->value()<=MINIMUM_HEIGHT_SPIN_BOX_VALUE)
		settings.setValue(this->metaObject()->className()+QString("/vertical-header-size"), 0);
	else
		settings.setValue(this->metaObject()->className()+QString("/vertical-header-size"), heightSpinBox->value());

	if(widthSpinBox->value()<=MINIMUM_WIDTH_SPIN_BOX_VALUE)
		settings.setValue(this->metaObject()->className()+QString("/horizontal-header-size"), 0);
	else
		settings.setValue(this->metaObject()->className()+QString("/horizontal-header-size"), widthSpinBox->value());

	naTableWidget->setItemDelegate(oldItemDelegate);
	delete newItemDelegate;
}

void TeachersNotAvailableTimesTimeHorizontalForm::colorItem(QTableWidgetItem* item)
{
	if(USE_GUI_COLORS){
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
		if(item->text()==NO)
			item->setBackground(QBrush(QColorConstants::DarkGreen));
		else
			item->setBackground(QBrush(QColorConstants::DarkRed));
		item->setForeground(QBrush(QColorConstants::LightGray));
#else
		if(item->text()==NO)
			item->setBackground(QBrush(Qt::darkGreen));
		else
			item->setBackground(QBrush(Qt::darkRed));
		item->setForeground(QBrush(Qt::lightGray));
#endif
	}
}

void TeachersNotAvailableTimesTimeHorizontalForm::itemClicked(QTableWidgetItem* item)
{
	QString s=item->text();
	if(s==YES)
		s=NO;
	else{
		assert(s==NO);
		s=YES;
	}
	item->setText(s);
	colorItem(item);
}

void TeachersNotAvailableTimesTimeHorizontalForm::widthSpinBoxValueChanged()
{
	if(widthSpinBox->value()==MINIMUM_WIDTH_SPIN_BOX_VALUE)
		naTableWidget->horizontalHeader()->setDefaultSectionSize(2*initialRecommendedHeight);
	else
		naTableWidget->horizontalHeader()->setDefaultSectionSize(widthSpinBox->value());
}

void TeachersNotAvailableTimesTimeHorizontalForm::heightSpinBoxValueChanged()
{
	if(heightSpinBox->value()==MINIMUM_HEIGHT_SPIN_BOX_VALUE)
		naTableWidget->verticalHeader()->setDefaultSectionSize(initialRecommendedHeight);
	else
		naTableWidget->verticalHeader()->setDefaultSectionSize(heightSpinBox->value());
}

void TeachersNotAvailableTimesTimeHorizontalForm::selectedClicked()
{
	bool firstFound=false;
	bool firstIsNO;
	for(int t=0; t<gt.rules.teachersList.count(); t++){
		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			for(int h=0; h<gt.rules.nHoursPerDay; h++){
				QTableWidgetItem* it=naTableWidget->item(t, d*gt.rules.nHoursPerDay+h);
				if(it->isSelected()){
					if(!firstFound){
						firstFound=true;
						firstIsNO=(it->text()==NO);
					}
					if(firstIsNO)
						it->setText(YES);
					else
						it->setText(NO);
					colorItem(it);
				}
			}
		}
	}
}

void TeachersNotAvailableTimesTimeHorizontalForm::ok()
{
	for(int t=0; t<gt.rules.teachersList.count(); t++)
		for(int d=0; d<gt.rules.nDaysPerWeek; d++)
			for(int h=0; h<gt.rules.nHoursPerDay; h++)
				tnaMatrix[t][d][h]=(naTableWidget->item(t, d*gt.rules.nHoursPerDay+h)->text()==YES);

	QList<TimeConstraint*> tl;

	for(int t=0; t<gt.rules.teachersList.count(); t++){
		QList<int> daysList;
		QList<int> hoursList;

		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			for(int h=0; h<gt.rules.nHoursPerDay; h++){
				if(tnaMatrix[t][d][h]){
					daysList.append(d);
					hoursList.append(h);
				}
			}
		}

		QSet<ConstraintTeacherNotAvailableTimes*> stc=gt.rules.tnatHash.value(gt.rules.teachersList.at(t)->name, QSet<ConstraintTeacherNotAvailableTimes*>());
		assert(stc.count()<=1);
		if(!stc.isEmpty()){
			ConstraintTeacherNotAvailableTimes* ctr=*stc.constBegin();

			if(!daysList.isEmpty()){
				ctr->days=daysList;
				ctr->hours=hoursList;
			}
			else{
				tl.append(ctr);
			}
		}
		else{
			if(!daysList.isEmpty()){
				ConstraintTeacherNotAvailableTimes* ctr=new ConstraintTeacherNotAvailableTimes(100, gt.rules.teachersList.at(t)->name, daysList, hoursList);
				bool t=gt.rules.addTimeConstraint(ctr);
				assert(t);
			}
		}
	}

	if(!tl.isEmpty())
		gt.rules.removeTimeConstraints(tl);

	if(!gt.rules.teachersList.isEmpty()){
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);
	}

	this->close();
}

void TeachersNotAvailableTimesTimeHorizontalForm::cancel()
{
	this->close();
}
