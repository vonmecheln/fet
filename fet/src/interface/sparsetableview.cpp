/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************
                      sparsetableview.cpp  -  description
                             -------------------
    begin                : 2010
    copyright            : (C) 2010 by Liviu Lalescu
                         : https://lalescu.ro/liviu/
 ***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "sparsetableview.h"

#include "timetable_defs.h"

#include <QHeaderView>

#include <QHash>
#include <QPair>

#include <QSize>

//#include <QString>
//#include <QColor>
//#include <QPalette>

SparseTableView::SparseTableView() : QTableView()
{
	this->setModel(&model);
}

int SparseTableView::maxHorizontalHeaderSectionSize()
{
	return this->width()/3;
}

void SparseTableView::resizeToFit()
{
	QHash<int, int> columnSizes;
	QHash<int, int> rowSizes;
	
	QHash<QPair<int, int>, QString>::const_iterator i=model.items.constBegin();
	while(i!=model.items.constEnd()){
		const QPair<int, int>& pair=i.key();
		
		if(!this->isRowHidden(pair.first) && !this->isColumnHidden(pair.second)){
			//QString str=i.value();
			
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
			QAbstractItemDelegate* delegate=this->itemDelegateForIndex(model.index(pair.first, pair.second));
#else
			QAbstractItemDelegate* delegate=this->itemDelegate(model.index(pair.first, pair.second));
#endif
			
			//QSize size=delegate->sizeHint(this->viewOptions(), model.index(pair.first, pair.second));
			QStyleOptionViewItem option;
			option.initFrom(this);
			QSize size=delegate->sizeHint(option, model.index(pair.first, pair.second));
			
			if(size.width() > columnSizes.value(pair.second, 0))
				columnSizes.insert(pair.second, size.width());
			if(size.height() > rowSizes.value(pair.first, 0))
				rowSizes.insert(pair.first, size.height());
		}
		
		i++;
	}
	
	horizontalSizesUntruncated.clear();
	
	for(int i=0; i<model.columnCount(); i++){
		int k=columnSizes.value(i, 0);
		if(this->showGrid())
			k++;
		if(k<this->horizontalHeader()->sectionSizeHint(i))
			k=this->horizontalHeader()->sectionSizeHint(i);
		
		//not too wide
		horizontalSizesUntruncated.append(k);
		
		if(k>maxHorizontalHeaderSectionSize()){
			k=maxHorizontalHeaderSectionSize();
		}
		
		this->horizontalHeader()->resizeSection(i, k);
	}

	for(int i=0; i<model.rowCount(); i++){
		int k=rowSizes.value(i, 0);
		if(this->showGrid())
			k++;
		if(k<this->verticalHeader()->sectionSizeHint(i))
			k=this->verticalHeader()->sectionSizeHint(i);
		this->verticalHeader()->resizeSection(i, k);
	}
}

/*void SparseTableView::itWasResized()
{
	assert(model.columnCount()==horizontalSizesUntruncated.count());
	for(int i=0; i<model.columnCount(); i++){
		int size=horizontalSizesUntruncated.at(i);
		
		if(size>maxHorizontalHeaderSectionSize())
			size=maxHorizontalHeaderSectionSize();
		
		if(this->horizontalHeader()->sectionSize(i)!=size)
			this->horizontalHeader()->resizeSection(i, size);
	}
}*/

void SparseTableView::resizeEvent(QResizeEvent* event)
{
	QTableView::resizeEvent(event);
	
	//So that if user resizes horizontal header and scroll bar appears, no automatic resize is done.
	if(event){
		if(event->size().width()==event->oldSize().width())
			return;
	}
	
	//this->itWasResized();
	if(model.columnCount()!=horizontalSizesUntruncated.count()){ //do NOT remove this test, or you will get assertion failed a few lines below, when user clicks "Swap axes".
	//This situation can appear if user clicks "Swap axes", and it is taken care of then, so we can return in this case, safely assuming that the table will be redrawn correctly.
		//cout<<"different sizes"<<endl;
		return;
	}
	assert(model.columnCount()==horizontalSizesUntruncated.count());
	for(int i=0; i<model.columnCount(); i++){
		int size=horizontalSizesUntruncated.at(i);
		
		if(size>maxHorizontalHeaderSectionSize())
			size=maxHorizontalHeaderSectionSize();
		
		if(this->horizontalHeader()->sectionSize(i)!=size)
			this->horizontalHeader()->resizeSection(i, size);
	}
}

void SparseTableView::allTableChanged()
{
	model.allItemsChanged();
	model.allHeadersChanged();
}
