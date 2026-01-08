#define TPDO1 0x0000  //001,1
#define TPDO2 0x280  //010,1
#define TPDO3 0x380  //011,1
#define TPDO4 0x480  //100,1
//#define TPDO5 0x580  //101,1
//#define TPDO6 0x680  //110,1
//#define TPDO7 0x780  //111,1
//#define TPDO8 0x880  //100,1
//#define TPDO9 0x980  //100,1
//#define TPDO10 0xA80  //100,1

#define RPDO1 0x0000  //010,0
#define RPDO2 0x300  //011,0
#define RPDO3 0x400  //100,0
#define RPDO4 0x500  //101,0

#define TSDO 0x580  //101,1
#define RSDO 0x600  //110,0

#define NODE_GUARD 0x700
/***************************************************************************/
union CANSendCtrl
{
	int CtrlWord;
	struct
	{
		unsigned FunCode:11;
		unsigned RTR:1;
		unsigned Unused:2;
		unsigned SendFlag:1;
		unsigned ACKFlag:1;
	}CtrlBit;
};
///////////PDO defination/////////////////
struct CANPDO
{
	union CANSendCtrl Ctrl;
	int DataLength;             //������ݳ���
	int *TimeInterval;
	unsigned int *PDataBuff[4];
};
/////////SDO defination///////////////////
struct CANSDO
{
	int DataLength;
	int Subindex;
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
	}Para;
	unsigned int *Object;
};
////////////////////////////////////////
struct SDOSummaryType
{
	int Index;
	struct CANSDO *Obj;
};
/************************************************************************/
