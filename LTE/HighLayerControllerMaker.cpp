#include "commondef.h"
#include "HighLayerControllerMaker.h"
#include "HighLayerController.h"

namespace LTESim
{
	HighLayerControllerMaker::HIGHLAYERCONTROLLERPTR HighLayerControllerMaker::CreateHighLayerController( int iID, double dPosX /* = 0.0 */, double dPosY /* = 0.0 */ )
	{
		HIGHLAYERCONTROLLERPTR pController( new HighLayerController( iID, dPosX, dPosY ) );
		pController->Initialize();
		return pController;
	}
}