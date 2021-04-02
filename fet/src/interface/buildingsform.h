//
//
// C++ Interface: $MODULE$
//
// Description: 
//
//
// Author: Liviu Lalescu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BUILDINGSFORM_H
#define BUILDINGSFORM_H

#include "building.h"

#include "ui_buildingsform_template.h"

/**
@author Liviu Lalescu
*/
class BuildingsForm : public QDialog, Ui::BuildingsForm_template
{
	Q_OBJECT
	
public:
	BuildingsList visibleBuildingsList;

	BuildingsForm();
	~BuildingsForm();

	bool filterOk(Building* bu);

public slots:
	void addBuilding();
	void removeBuilding();
	void buildingChanged(int index);
	void sortBuildings();
	void modifyBuilding();
	void filterChanged();
};

#endif
