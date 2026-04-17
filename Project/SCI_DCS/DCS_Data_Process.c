#include "Custom_DCS.h"
#include "Main\Variable.H"
#include "SCI_DCS\Sci_DCS_config.H"

/***************************************************************************************************/
void Modbus_Send_To_Dcs(void)
{
	memcpy(DcsInputRegBuf, &DcsInputSum, sizeof(DcsInputSum));
}

void Modbus_Recv_For_Dcs(void)
{
	memcpy(&DcsHoldSum, DcsHoldRegBuf, sizeof(DcsHoldSum));
}
