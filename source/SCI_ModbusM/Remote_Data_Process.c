#include "Custom_Remote.h"
#include "Main\Variable.H"
#include "SCI_Modbus\Sci_Modbus_config.H"

/***************************************************************************************************/
void Modbus_Send_To_Rem(void)
{
	memcpy(RemInputRegBuf, &RemInputSum, sizeof(RemInputSum));
}

void Modbus_Recv_For_Rem(void)
{
	memcpy(&RemHoldSum, RemHoldRegBuf, sizeof(RemHoldSum));
}
