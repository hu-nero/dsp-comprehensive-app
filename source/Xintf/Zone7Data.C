/********************************************************************************************/
//DataLog数据缓冲区
#pragma DATA_SECTION(DataLog_Buf, "ZONE7DATA")
int DataLog_Buf[10000][18];//缓存触发数据
#pragma DATA_SECTION(DataLog_SendBuf, "ZONE7DATA")
unsigned int DataLog_SendBuf[180000];//send缓冲区数据
/*******************************************************************************************/
void Zone7_Init(void)
{
	long i,j;
	for(i=0;i<18;i++)
	for(j=0;j<10000;j++)
	  DataLog_Buf[j][i]=0xFFFF;
	  
	for(i=0;i<180000;i++)
	  DataLog_SendBuf[i]=0xFFFF;
}
/******************************************************************************************/
