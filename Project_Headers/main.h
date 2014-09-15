
#include "list.h" 
#include "MC9S08AC60.h"

#define   uchar  unsigned char
#define   uint   unsigned int

uchar  agn_int7037t;									// �����ʼ��7037ʧ�� �Ժ�ÿ10�����³�ʼ��1��
uchar  rst_3s;
uchar  fsyc;
unsigned int uuuu;

uchar  timebase,ms10,sec1s,min60m;						// 

#define SL_STATE_NUMBER	3

enum  RL_state{received=0, rl_empty=1};				// R ����  ״̬  0�Ѿ��յ�  1��
enum  SL_state{needsend=0, waitACK=1, sl_empty=2};		// S ����        0��Ҫ����  1��ACK  2��

volatile list_head	receive_list[2];		 			// ���ձ� ��ͷ
volatile list_head	send_list[SL_STATE_NUMBER];			// ���ͱ� ��ͷ

 
uint   ad_value;
uchar  ad_tmp,ad_tmpbf;									// �����¶�

uchar  volume,volumeBF,mic_volume,mic_volumeBF;			// ����

uchar  pwer_t;											// ��Դ��������1��    
uint   rst_armt;										// 5����û���յ�arm����
uchar  SD_sec,SD_min,SD_hour,SD_day,SD_mth,SD_year;	// ʱ��1302
uchar  T1302_year,T1302_mth,T1302_week,T1302_day,T1302_hour,T1302_min,T1302_sec;

uchar  safe_zt[8],safe_bf[8],pot_a,pa_sm_t;			// ����״̬
uchar  safe_jj,safe_jjbf,safe_jtm,safe_jbz;

uchar  kScr_hlbz;										// У׼��Ŀ
uint   kScr_t;

uchar  rut_zt,tlphn_zt;									// ��·״̬ �绰�ֻ�״̬

uchar  ledzt[8],Rlayzt[8];								// ledָʾ��״̬ �̵���״̬

uchar  Telarm[20],Telmdl[20];							// �绰���뻺���� [0]�泤��
uchar  Dtmfarm[20],Dtmfmdl[20];						// Dtmf���뻺����

uchar  bw_armbf,bw_telbf;								// 
//--------------------------------------------------------
// Arm ͨ��
volatile uchar rsvbufi[35],rsvbufm[35],rsv_p,rsvover,headf5bz,rsv_ack;// ���ڽ���	

volatile uchar Sndbuf[5][35],snd_p,snd_frm,snd_BWenNo;			// ����Sndbuf[5][31]�Ǽ�ʱ��
// Sndbuf[x][6]�������  Sndbuf[x][27] = 0xfd  Sndbuf[x][snd321]���͵�123���� Sndbuf[x][30]��ʱ200ms  

//---------------------------------------------------------
// �绰ģ��
volatile uchar RxTelbufi[30], RxTelbufm[30],RxTel_p,RxTelov,RxTelf5bz;// ���ڽ���	

volatile uchar TxTelbuf[5][33],Tel_p,Tel_frm,Tel_BWenNo;		// ����Sndbuf[5][31]�Ǽ�ʱ��
// Sndbuf[x][6]�������  Sndbuf[x][27] = 0xfd  Sndbuf[x][snd321]���͵�123���� Sndbuf[x][30]��ʱ200ms  




#define		_PIN_TE_RX	 			PTFD_PTFD0 		// ��绰ģ��ͨ�ţ�����
#define		_PIN_TE_TX	 			PTFD_PTFD1 		// ��绰ģ��ͨ��  ���

#define		_PIN_5465_UD_ 			PTCD_PTCD4 		// �������� ���ֵ�λ��
#define		_PIN_5465_CS_ 			PTCD_PTCD6		// �������� ���ֵ�λ��

#define		_STATE_51_RESET			0				//  
#define		_STATE_51_NORMAL		1				//  

#define  	_PIN_CkScreen	 		PTBD_PTBD4		// ���� ��� У׼��Ļ

#define  	_PIN_RESET_51 			PTDD_PTDD0		// ��λ arm

#define 	_PIN_AW60_UART_EN		PTFD_PTFD6		// 2031  Ԥ��

//----------------------------------------------------
// LED ָʾ��
#define  	_PIN_LED_MR	 		PTCD_PTCD2			// ����        
#define  	_PIN_LED_JJAF 		PTCD_PTCD0			// ����ָʾ��  
#define  	_PIN_LED_TX	 		PTCD_PTCD1			// ͨ��       

  
// ��������
#define	    SAFETY_SENSOR_0	    PTAD_PTAD0			// ���� 8· ����
#define	    SAFETY_SENSOR_1		PTAD_PTAD1
#define	    SAFETY_SENSOR_2		PTAD_PTAD2
#define	    SAFETY_SENSOR_3		PTAD_PTAD3
#define	    SAFETY_SENSOR_4		PTAD_PTAD4
#define	    SAFETY_SENSOR_5		PTAD_PTAD5
#define	    SAFETY_SENSOR_6		PTAD_PTAD6
#define	    SAFETY_SENSOR_7		PTAD_PTAD7
 

#define	MAX_CHANGE_VOL_TIMES		3				// ��� �ı� ���� ����

//              volstate = {MAX_CHANGE_VOL_TIMES, 0, 0, 0, {0,2,6,20,21,22,23,24,25,26,27,28,29,30,31} };
//              volstate = {MAX_CHANGE_VOL_TIMES, 0, 0, 0,  {0,2,6,20,21,22,23,24,25,26,27,28,29,30,31} };
// ���ֵ�λ�������  ��5�������ѹ
// 1--0.595   2--0.898   3--1.090   4--1.224   5--1.321   6--1.395   7--1.453   8--1.500   9--1.538
//10--1.569  11--1.595  12--1.617  13--1.634  14--1.649  15--1.661  16--1.673  20--1.687  25--1.650


typedef struct _TimerCount_ 
{
	volatile _UINT	delay1ms;
	_UINT	reset_delay;
	_UINT	sensor_delay;
} _TimerCount_;
volatile _TimerCount_ time_counter = { 0, 0, 0};

//uchar MenQianJi;

// 1-0.129  2-0.230  3-[0.308]4-0.386  5-[0.442]
// 6-0.499  7-0.547  8-0.588  9-0.625 10-0.658
//11-0.689 12-0.717 13-0.742 14-0.765 15-0.787
//16-0.807 17-0.826 18-0.844 19-0.862 20-0.878
//21-0.894 22-0.909 23-0.924 24-0.939 25-0.955
//26-0.970 27-0.986 28-1.003 29-1.022 30-1.043   31-1.066

// 1-15������
const unsigned char Volv_lvel[15][2] = 
{
	{0,	0x16},				// 0
	
	{18,0x00},				// 1
	{19,0x00},
	{20,0x00},
	{21,0x00},		
	{22,0x00},	
	
	{23,0x00},				// 6		
	{24,0x00},				// 7
	{25,0x00},	                         
	{26,0x00},	
	{27,0x00},				// 10
	{28,0x00},				//
};

 const unsigned char Volv_lvel_99999[15][2] = 
{
	{0,	0x16},				// 0
	
	{20,0x00},				// 1                 
	{21,0x00},
	{22,0x00},
	{23,0x00},		
	{24,0x00},	
	
	{25,0x00},				// 6		
	{26,0x00},				// 7
	{27,0x00},	
	{28,0x00},	
	{29,0x00},				// 10
	{29,0x00},				//
};

// ��ͬͨ���豸[��ǰ��]���������� �� mic�����Ȳ�ͬ
const unsigned char Dvc_Lbalvl[] = {0x1C,0x1F,0x1D,0x1E,0x1F,0x00,0x01,0x03,0x05,0x07,0x09};
const unsigned char Dvc_miclvl[] = {0x00,0x1F,0x00,0x01,0x07,0x00,0x01,0x1f,0x00,0x01,0x1f,0x00,0x01,0x1f,0x00,0x01,0x1f,0x00,0x01,0x1f,0x00};

// 1 �����ն�	  2 ������    3 ��Ԫ�ſڻ�    4 С�ſڻ�     5 ������     06 Զ�̿����ֻ�
// 7 Զ����ҳ���� 8 �绰      9 С���ſڻ�   10 ��ҵ�����  11 ����       12 ���ֱ����ſڻ�
//13 ���ַֻ�    14 �ƶ��ն� 15 ����������   16 ���пͻ���  17 ͣ�������������
//18 ����        19 ��ʵ�����ſڻ�           20 9000 �ն�CPU2
// 0x16[-20]  0x17[-18]  0x18[-16]  0x19[-14]  0x1A[-12]  0x1B[-10]  0x1C[-8]  0x1D[-6]  0x1E[-4]  0x1F[-2]
// 0x00[0]    0x01[2]    0x02[4]    0x03[6]    0x04[8]    0x05[10]   0x06[12]  0x07[14]  0x08[16]  0x09[18]  0x0A[20]

//#pragma CONST_SEG  DEFAULT_ROM
_UINT  temp_mv[] =
{   
	//981, 979, 977, 974, 971, 968, 965, //962, 959, 955,	//-39--30
	
	//962, 959, 955,				   						//-32  -30
	//951, 947, 
	943, 939, 935, 931, 926, 922, 917, 912, 				//-29--20						 
	907, 901, 895, 890, 884, 877, 871, 865, 858, 852, 		//-19--10
	845, 837, 830, 822, 815, 807, 798, 790, 782, 774, 		//-9-0
	
	759, 756, 748, 739, 729, 720, 711, 701, 692, 682, 		//0-10
	672, 662, 653, 643, 633, 623, 613, 603, 593, 583, 		//11-20
	572, 562, 553, 543, 533, 523, 513, 503, 494, 484, 		//21-30
	
	474, 465, 456, 446, 437, 428, 419, 410, 401, 393, 		//31-40
	384, 376, 367, 359, 351, 343, 335, 328, 320, 313, 		//41-50
	306, 299, 292, 285, 278, 272, 265, 259, 252, 246, 		//51-60
	241, 235, 229, 224, 219, 213, 208, 203, 198, 193, 		//61-70
	189, 184, 179, 175, 171, 167, 163, 159, 155, 151  		//71-80
};



void saving_clock(void);


_UBYTE   _global_error_;
_UBYTE   msgflag;

// ����led��ʾ0123��״̬�Ļ������±�   	// ���� ֹͣ ͨ�� ���� , δ�� ͨѶ ���� ����
#define   Tongxun   	2
#define   Baojing   	1
#define   Mianrao		0



// 1 ��ѯʱ ʱ�ӵ�����δ��Ϊ������ ����BCD��
// 2 ACK   01  02
// 3 �ػ�ָ��ִ��


