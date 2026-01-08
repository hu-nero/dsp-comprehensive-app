#include "Version.H"
#include "ParaDefine.h"
#include "main\Variable.H"
#include "MessageParse.h"
/*******************************************************************************/
void Monitor_Software_Read(unsigned int *PTxBuf,unsigned int *PTxTail)
{
	Uint16 uiVerSenDataBuf[80];               /*版本信息数据缓冲区*/
	int M_CtrlWord_Temp=0;
  int i;
  unsigned int length=0;

	for(i=0;i<RTM_VER_SIZE;i++)
	  uiVerSenDataBuf[i]=RTM_File_Ver[i+1];
	uiVerSenDataBuf[RTM_VER_SIZE-1]&=0x00FF;
	for(i=RTM_VER_SIZE;i<16;i++)
	  uiVerSenDataBuf[i]=0;
	
	for(i=0;i<TYPE_VER_SIZE;i++)
	  uiVerSenDataBuf[i+16]=Conv_Type_Ver[i+1];
	uiVerSenDataBuf[TYPE_VER_SIZE+15]&=0x00FF;
	for(i=TYPE_VER_SIZE;i<32;i++)
	  uiVerSenDataBuf[i+16]=0;
	
	for(i=0;i<LIST_VER_SIZE;i++)
	  uiVerSenDataBuf[i+32]=Conv_List_Ver[i+1];
	for(i=LIST_VER_SIZE;i<16;i++)
	  uiVerSenDataBuf[i+32]=0;  

	for(i=48;i<80;i++)
	  uiVerSenDataBuf[i]=0;    
	  
	M_CtrlWord_Temp=7<<8;
	length=80;
  	
  Monitor_Tx_Protocol(PTxBuf,PTxTail,0x00800000,length,uiVerSenDataBuf,0,M_CtrlWord_Temp);
}
//===========================================================================
// No more.
//===========================================================================
