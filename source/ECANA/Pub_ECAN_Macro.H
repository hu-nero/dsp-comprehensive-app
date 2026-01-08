#ifndef _THIS_FILE_
#define _THIS_FILE_
/********************************************************************************/
//�����������û������޸�
//CANOPEN defination,MPR2NPR-MPR
#define TPDO1 0x0000  //001,1
#define RPDO1 0x0000  //010,0
#define TPDO2 0x280  //010,1
#define RPDO2 0x300  //011,0
#define TPDO3 0x380  //011,1
#define RPDO3 0x400  //100,0
#define TPDO4 0x480  //100,1
#define RPDO4 0x500  //101,0
#define TSDO  0x580  //101,1
#define RSDO  0x600  //110,0
#define NMTEC 0x700  //111,0
/********************************************************************************/
union CANSendCtrl//���Ϳ����ֶ���
{
	int CtrlWord;
	struct
	{
		unsigned MailID:11;//������
		unsigned RTR:1;     //Զ��֡��־
		unsigned Unused:2;  //Ԥ��
		unsigned SendFlag:1;//��Ҫ���ͣ�����PDO��������
		unsigned ACKFlag:1; //��ҪӦ�𣬽���SDO����
	}CtrlBit;
};
/********************************************************************************/
struct CANPDO //PDO���鶨��
{
	union CANSendCtrl Ctrl;//���Ϳ���
	unsigned int DataLength;        //�����ֽڳ���
	unsigned int *TimeInterval;     //����ʱ����
	unsigned int *PDataBuff[4];     //��Ч���
};
///////////////////////////////////////////////////////
/*
struct CANPDO1 //PDO���鶨��
{
	unsigned int DataLength;        //�����ֽڳ���
	unsigned int *TimeInterval;     //����ʱ����
	unsigned int *PDataBuff[4];     //��Ч���
};
struct MessSumm
{
	union CANSendCtrl Ctrl;//���Ϳ���
	const struct CANPDO1 *PDO;
};
*/
/*************************************************************************************/
struct CANSDO  //SDO���鶨��
{
	unsigned int DataLength;//�����ֽڳ���
	unsigned int Subindex;//���������?����
	union 
	{
	  unsigned int ParaWord;
	  struct
	  {
	    unsigned WriteEnable:1;//1����д
		  unsigned ReadEnable:1;//1���ɶ�	
		  unsigned ByteBum:2;//�ֽڸ���00��4���ֽڣ�01��3���ֽڣ�10��2���ֽڣ�11��1���ֽ�
		  unsigned Reserved1:4;	
		  unsigned CycleAccessFlag:2;//00������Ҫ��01���ڶ���10����д
		  unsigned InitAccessEnable:2;//00������Ҫ��01����ʼ������10����ʼ��д
		  unsigned MemoryFlag:3;//000��RAM��001��EEPROM
		  unsigned ACKFlag:1;	
		  //MemoryFlagֻ�ڵ�0��ʹ�ã�����������ݵĴ洢����
		  //������ÿ����Ա������
	  }ParaBit;
	}Para;//���Ͳ���
	unsigned int *Object;//���Ͷ���
};
//////////////////////////////////////////////
struct SDOSummaryType//SDO������������ܳ�һ�ű�
{
	unsigned int Index;
	struct CANSDO *Obj;
};

/*********************************************************************/
#endif







	





