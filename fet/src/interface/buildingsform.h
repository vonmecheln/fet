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

#include "buildingsform_template.h"

/**
@author Liviu Lalescu
*/
class BuildingsForm : public BuildingsForm_template
{
public:
	BuildingsList visibleBuildingsList;

	BuildingsForm();
	~BuildingsForm();

	void addBuilding();
	void removeBuilding();
	void modifyBuilding();
	void sortBuildings();
	void buildingChanged(int index);
	void filterChanged();
	bool filterOk(Building* bu);
};

#endif
