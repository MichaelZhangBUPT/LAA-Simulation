#pragma once
#include "commondef.h"
#include "HighLayerController.h"

namespace LTESim
{
	class HighLayerControllerMaker
	{
	public:
		typedef boost::shared_ptr<HighLayerController> HIGHLAYERCONTROLLERPTR;
	public:
		HIGHLAYERCONTROLLERPTR CreateHighLayerController( int iID, double dPosX = 0.0, double dPosY = 0.0 );
	};
}