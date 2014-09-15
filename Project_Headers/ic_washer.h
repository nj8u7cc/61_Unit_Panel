
//����״̬��

#define  searching  0;
#define  reporting 1;

#define uchar unsigned char
#define uint unsigned int

//FM1702��Ҫ�õ��ĳ����ͼĴ�������
//��������
#define FALSE 0
#define TRUE 1

//FM1702������
#define Transceive 0x1E   //���ͽ�������
#define Transmit   0x1a   //��������
#define ReadE2   0x03  //��FM1702EEPROM����
#define WriteE2  0x01  //дFM1702EEPROM����
#define	Authent1	0x0c  //��֤������֤���̵�1��
#define 	Authent2	0x14	//��֤������֤���̵�2��
#define 	LoadKeyE2	0x0b	//�������EEPROM���Ƶ�KEY����
#define	LoadKey		0x19	//�������FIFI���Ƶ�KEY����

#define	Receive	0x16	//������յ�·

#define	RF_TimeOut	0x2f  //���������ӳ�ʱ��
#define	Req	0x01
#define	Sel	0x02



//FM1702��ַ����
#define	Page_Sel	0x00   	//ҳѡ�Ĵ���
#define	Command	0x01	//ָ��Ĵ���
#define	FIFODate	0x02	//64byteFIFO����������Ĵ���
#define	PrimaryStatus	 0x03	//����������������FIFO�ı�ʶλ
#define	FIFOLength	0x04 	//��ǰFIFO�� byte��
#define	SecondaryStatus	0x05	// ����״̬��ʶ�Ĵ���
#define	InterruptEn	0x06	//�ж�ʹ��/��ֹ����
#define	InterruptRq	0x07	//�ж������ʶ

#define	Control	0x09	//���ֿ��Ʊ�ʶ
#define	ErrorFlag	0x0a	//��һ��ָ�����������ʶ
#define	CollPos		0x0b	//��⵽�ĵ�һ����ͻλ��λ��
#define	TimerValue	0x0c	//��ǰTimerֵ
#define	CRCResultLSB	0x0d	//CRCЭ��������8λ
#define	CRCResultMSB	0x0e	//CRCЭ��������8λ
#define	BitFraming	0x0f	//��������bit�����ʽ

#define	TxControl	0x11	//���������ƼĴ���
#define	CWConductance	0x12	//ѡ�����TX1��TX2�������ߵ��迹
#define	ModWidth	0x15	//ѡ���ز����ƿ��

#define	RXControl1	0x19	//���������ƼĴ���
#define	DecoderControl	0x1a	//������ƼĴ���
#define	BitPhase	0x1b	//�����������ͽ�����ʱ�����
#define	Rxthreshold	0x1c	//ѡ��bit�����Xֵ
#define	RxControl2	0x1e	//������Ƽ�ѡ�����Դ
#define	ClockQControl	0x1f	//ʱ�Ӳ�������

#define	 RxWait	0x21	//ѡ����ͽ���֮���ʱ��
#define	ChannelRedundancy	0x22	//ѡ������У�������ģʽ
#define	CRCPresetLSB	0x23	//CRCԤ�üĴ�����8λ
#define	CRCPresetMSB	0x24
#define	MFOUTSelect	0x26	//ѡ��MFOUT�ź�Դ

#define	FIFOLevel	0x29	//����FIFO�������
#define	TimerClock	0x2a	//ѡ��Timerʱ�ӵķ�Ƶ
#define	TimerControl	0x2b	//ѡ��TIMER ����/ֹͣ����
#define	TimerReload		0x2c	//TimerԤ��ֵ
#define	IRQPinConfig	0x2d	//IRQ �������

//#define	CryptoSelect		0x31	//��֤ģʽѡ��
#define	TypeSH		0x31	//��֤ģʽѡ��

//��Ƶ��ͨѶ�����붨��
#define	RF_CMD_REQUEST_STD	0x26
#define	RF_CMD_REQUEST_ALL	0x52
#define	RF_CMD_ANTICOL		0x93
#define	RF_CMD_SELECT		0x93
#define	RF_CMD_AUTH_LA	0x60
#define	RF_CMD_AUTH_LB	0x61
#define	RF_CMD_READ		0x30
#define	RF_CMD_WRITE		0xa0
#define	RF_CMD_INC			0xc1
#define	RF_CMD_DEC		0xc0
#define	RF_CMD_RESTORE	0xc2
#define	RF_CMD_TRANSFER	0xb0
#define	RF_CMD_HALT		0x50

//Status Values
#define	ALL		0x01
#define	KEYB	0x04
#define	KEYA	0x00
#define	_AB		0x40
#define	CRC_A	1
#define	CRC_B	2

#define	CRC_OK	0
#define	CRC_ERR	1

#define	BCC_OK		0
#define	BCC_ERR	1

//����������붨��
#define	FM1702_OK	0	//��ȷ
#define	FM1702_NOTAGERR	1	//�޿�
#define	FM1702_CRCERR	2	//��ƬCRCУ�����
#define	FM1702_EMPTY	3	//��ֵ�������
#define	FM1702_AUTHERR	4	//��֤���ɹ�
#define	FM1702_PARITYERR	5	//��Ƭ��żУ�����
#define	FM1702_CODEERR	6	//ͨѶ����(BCCУ���)
#define	FM1702_SERNRERR	8	//��Ƭ���кŴ���
#define	FM1702_SELECTERR	9	//��Ƭ���ݳ����ֽڴ���
#define	FM1702_NOTAUTHERR	10	//��Ƭû��ͨ����֤
#define	FM1702_BITCOUTERR		11	//�ӿ�Ƭ���յ���λ������
#define	FM1702_BYTECOUNTERR	12	//�ӿ�Ƭ���յ���ֱ��������
#define	FM1702_RESTERR	13	//����RESTORE��������
#define	FM1702_TRANSERR	14	//����TRANSFER��������
#define	FM1702_WRITEERR	15	//����WRITE��������
#define	FM1702_INCRERR	16	//����INCREMENT��������
#define	FM1702_DECRERR	17	//����DECREMENT��������
#define	FM1702_READERR	18	//����READ��������
#define	FM1702_LOADKEYERR	19	//����LOADKEY��������
#define	FM1702_FRAMINGERR	20	//FM1702�����
#define	FM1702_REQERR		21	//����REQ��������
#define	FM1702_SELERR		22	//����SEL��������
#define	FM1702_ANTICOLLERR	23	//����ANTICOLL��������
#define	FM1702_INTIVALERR		24	//���ó�ʼ����������
#define	FM1702_READVALERR	25	//���ø߼�����ֵ��������
#define	FM1702_DESELECTERR	26
#define	FM1702_CMD_ERR		42	//�������

#define old_pwd 0    //ԭʼ����
#define new_pwd 1   //������

//����ͨѶ�����붨��
#define UART_WR_OK  0    //д��������ɹ�
#define UART_PWD_OK   0x10  //д�û��гɹ�
#define UART_BB_OK    0x20  //BB������ճɹ�
#define UART_HAND_OK  0x30  //��������ɹ�
#define UART_RSTPWD_OK 0x40  //��ԭ����ɹ�

#define UART_VAL_ERR  1  //�������Ŵ���
#define UART_RD_ERR 2   //�����ݴ���
#define UART_WR_ERR 3  //д���ݴ���
#define UART_AUTH_ERR 4 //������֤����
#define UART_PWD_ERR 5 //�����û��и�ʽ����
#define UART_NOPWD_ERR 6   //δд�����û��д���
#define UART_OLDPWD_ERR 7  //ʹ��ԭʼ������֤����
#define UART_WRPWD_ERR 8  //д�������
#define UART_CHESUM_ERR 9  //У��ʹ���
#define UART_NOCARD  0x0A  //FM1702_NOTAGERR   //�޿�

#define UART_BUSY 0x0B   //����æ,���ϸ�����û������
#define UART_TEST 0x0C   //�ϵ緢��һ��
#define UART_CARD_ERR   0x0D  //���Ŵ��󣬼���ǰ��������Ҫд�Ŀ��Ų�ͬ;
#define UART_NULL 0x0f   //��Ч����





uchar tagtype[2];	//��Ƭ��ʶ�ַ�


//FM1702��������
uchar buffer[20];	//FM1702����ͽ��ջ�����
uchar UID[5],UID_TMP[5];   //���к�

//��������
uchar  key[6]    = {0xFF, 0xFF, 0xff, 0xff, 0xFF, 0xFF};
uchar key_buff[8] ;//={0x10 , 0x32 , 0x54 , 0x76 , 0x98 , 0xba , 0xdc , 0xfe};   //���ڱ��������û����е�����
uchar key_pwd[6];   //����û��������


uint shanqu=5,shanqu_tmp=0;   //������,˫�ֽ�
uchar block;   //���ַ

uchar TX_BUFF[26],RX_BUFF[26];//,RX_BUFF_TMP[20];
uchar RX_BUFF_TMP[26];
unsigned char mark_pwd_set;
uchar g_uart2_sending;

//�����б�
void port_init(void);
void read_data(void);
void Read_Card_Pro(void);
void write_data(void);
void write_pro_2(void);
void ack_send_tx(uchar ack_da);
void report_card(void);
void reques_again(void);
void send_data_tx(void);
void blank_send(void);
void  write_cmd(uchar addr,uchar data);
uchar read_cmd(uchar addr);
uchar Judge_Req(uchar *buff);
uchar Check_UID(void);
void Save_UID(uchar row,uchar col,uchar length);
void Set_BitFraming(uchar row,uchar col);
uchar FM1702_Bus_Sel(void);
void Init_FM1702(void);
uchar Read_FIFO(uchar *buff);
void Write_FIFO(uchar count,uchar *buff);
uchar Clear_FIFO(void);
uchar Command_Send(uchar count,uchar *buff,uchar Comm_Set);
uchar Write_E2(uchar lsb,uchar msb,uchar count,uchar *buff);
uchar M500HostCodeKey(uchar *uncoded,uchar *coded) ;
uchar Load_Key(uchar Secnr2,uchar *buff);
uchar Load_keyE2_CPY(uchar Secnr,uchar Mode);
uchar Request(uchar mode);
uchar AntiColl(void);
uchar Select_Card(void);
uchar  Authentication(uchar  *UID,uchar SecNR,uchar mode);
uchar MIF_READ(uchar  *buff,uchar Block_Adr);
uchar MIF_Write(uchar  *buff,uchar Block_Adr);
void pwd_convert(void);
void load_pwd(uchar pwd_type);
void CHECK_PWD(void);
void NET_IC_PRO(void);


