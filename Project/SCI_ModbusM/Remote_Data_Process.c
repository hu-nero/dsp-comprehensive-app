#include "Custom_Remote.h"
#include "Main\Variable.H"
#include "SCI_ModbusS\Sci_Modbus_config.H"
#include "Main\Comm_Variable.H"

/***************************************************************************************************/
void Modbus_Send_To_Rem(void)
{
	memcpy(RemInputRegBuf, &RemInputSum, sizeof(RemInputSum));
}

void Modbus_Recv_For_Rem(void)
{
	memcpy(&RemHoldSum, RemHoldRegBuf, sizeof(RemHoldSum));
	SysTime_Year = RemHoldSum.year;
	SysTime_Month = RemHoldSum.month;
	SysTime_Day = RemHoldSum.day;
	SysTime_Hour = RemHoldSum.hour;
	SysTime_Minute = RemHoldSum.minute;
	SysTime_Second = RemHoldSum.second;
}
