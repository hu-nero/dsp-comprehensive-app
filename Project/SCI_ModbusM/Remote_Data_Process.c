#include "Custom_Remote.h"
#include "Main\Variable.H"
#include "SCI_ModbusS\Sci_Modbus_config.H"
#include "Main\Comm_Variable.H"

/***************************************************************************************************/
void Modbus_Send_To_Rem(void)
{
	//memcpy(RemInputRegBuf, &RemInputSum, sizeof(RemInputSum));
    RemInputRegBuf[0] = RemInputSum.StaWord1.DataInt;
    RemInputRegBuf[1] = RemInputSum.StaWord2.DataInt;
    RemInputRegBuf[2] = RemInputSum.FltWord1.DataInt;
    RemInputRegBuf[3] = RemInputSum.FltWord2.DataInt;
    RemInputRegBuf[4] = RemInputSum.AlmWord1.DataInt;
    RemInputRegBuf[5] = RemInputSum.AlmWord2.DataInt;
    RemInputRegBuf[6] = RemInputSum.UabSl;
    RemInputRegBuf[7] = RemInputSum.UbcSl;
    RemInputRegBuf[8] = RemInputSum.UcaSl;
    RemInputRegBuf[9] = RemInputSum.UabPar;
    RemInputRegBuf[10] = RemInputSum.UbcPar;
    RemInputRegBuf[11] = RemInputSum.UcaPar;
    RemInputRegBuf[12] = RemInputSum.UaOut;
    RemInputRegBuf[13] = RemInputSum.UbOut;
    RemInputRegBuf[14] = RemInputSum.UcOut;
    RemInputRegBuf[15] = RemInputSum.UabOut;
    RemInputRegBuf[16] = RemInputSum.UbcOut;
    RemInputRegBuf[17] = RemInputSum.UcaOut;
    RemInputRegBuf[18] = RemInputSum.IaOut;
    RemInputRegBuf[19] = RemInputSum.IbOut;
    RemInputRegBuf[20] = RemInputSum.IcOut;
    RemInputRegBuf[21] = RemInputSum.P_Sys;
    RemInputRegBuf[22] = RemInputSum.Q_Sys;
    RemInputRegBuf[23] = RemInputSum.S_Sys;
    RemInputRegBuf[24] = RemInputSum.PF_Sys;
    RemInputRegBuf[25] = RemInputSum.Ublk;
    RemInputRegBuf[26] = RemInputSum.Iblk;
    RemInputRegBuf[27] = RemInputSum.FLwater;
}

void Modbus_Recv_For_Rem(void)
{
	//memcpy(&RemHoldSum, RemHoldRegBuf, sizeof(RemHoldSum));
    RemHoldSum.CtrlWord.all = RemHoldRegBuf[0];
    RemHoldSum.year = RemHoldRegBuf[1];
    RemHoldSum.month = RemHoldRegBuf[2];
    RemHoldSum.day = RemHoldRegBuf[3];
    RemHoldSum.hour = RemHoldRegBuf[4];
    RemHoldSum.minute = RemHoldRegBuf[5];
    RemHoldSum.second = RemHoldRegBuf[6];

	SysTime_Year = RemHoldSum.year;
	SysTime_Month = RemHoldSum.month;
	SysTime_Day = RemHoldSum.day;
	SysTime_Hour = RemHoldSum.hour;
	SysTime_Minute = RemHoldSum.minute;
	SysTime_Second = RemHoldSum.second;
}
