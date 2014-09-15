
//���Ŷ���
//uchar PA_DR;
//uchar PA_DDR;
//void SetPAState(uchar bitindex, uchar level);
//void SetPBState(uchar bitindex, uchar level);

//////////////////////////////////////////////////////////////////////////////
//��������
//////////////////////////////////////////////////////////////////////////////
#define WR  PTBD_PTBD6 //6 PTBD_PTB6
#define RST PTBD_PTBD5 //5
#define CS  PTBD_PTBD4 //4
#define RD  PTBD_PTBD3 //3
#define A2  PTBD_PTBD2 //2
#define A1  PTBD_PTBD1 //1
#define A0  PTBD_PTBD0 //0
#define D0  PTAD_PTAD0 //0
#define D1  PTAD_PTAD1 //1
#define D2  PTAD_PTAD2 //2
//����:Request
//����:�ú���ʵ�ֶԷ���FM1702 ������Χ֮�ڵĿ�Ƭ��Request����
//����:mode:ALL (�������FM1702������Χ֮�ڵĿ�Ƭ)
//	STD(�����FM1702������Χ֮�ڴ���HALT״̬�Ŀ�Ƭ)
//���:FM1702_NOTAGERR:�޿�
//	     FM1702_OK:Ӧ����ȷ
//	     FM1702_REQERR:Ӧ�����
//uchar R_temp;
uchar Request(uchar mode)
{
	uchar temp;

	write_cmd(CRCPresetLSB,0x63);
	write_cmd(CWConductance,0x3f);
	buffer[0] = mode;                  //Request ģʽѡ��
	write_cmd(BitFraming,0x07);        //����7 bit
	write_cmd(ChannelRedundancy,0x03); //�ر�CRC
	temp = read_cmd(Control);
	temp = temp & (0xf7);
	write_cmd(Control,temp);       	   //����CRYPTO1
	temp = Command_Send(1,buffer,Transceive);
	//Read_FIFO(buffer);
	if (temp == FALSE)
	{
		return FM1702_NOTAGERR;
	}
	
	temp = Read_FIFO(buffer);
	temp = Judge_Req(buffer);
	if (temp == TRUE)
	{
		tagtype[0] = buffer[0];
		tagtype[1] = buffer[1];
		return FM1702_OK;
	}
	
	return FM1702_REQERR;
}

//����:AntiColl
//����:�ú���ʵ�ֶԷ���FM1702������Χ֮�ڵĿ�Ƭ�ķ���ͻ���
//����:N/A
//���:FM1702_NOTAGERR:�޿�
//		FM1702_BYTECOUNTERR:�����ֽڴ���
//		FM1702_SERNRERR:��Ƭ���к�Ӧ�����
//		FM1702_OK:��ƬӦ����ȷ
uchar AntiColl(void)
{
	uchar temp;
	uchar i;
	uchar row,col;
	uchar pre_row;

	row =0;
	col =0;
	pre_row =0;

	write_cmd(CRCPresetLSB,  0x63);
	write_cmd(CWConductance, 0x3f);
	//write_cmd(ModConductance,0x3f);   //��FM1702���޴˼Ĵ���
	buffer[0] = RF_CMD_ANTICOL;
	buffer[1] = 0x20;
	write_cmd(ChannelRedundancy,0x03);  //�ر�CRC,����żУ��
	temp = Command_Send(2, buffer, Transceive);

	for(;;)
	{
		if (temp==FALSE)
		{
			return (FM1702_NOTAGERR);
		}
		temp = read_cmd(ErrorFlag);
		temp = read_cmd(FIFOLength);
		if (temp ==0)
		{
			return FM1702_BYTECOUNTERR;
		}
		
		temp = Read_FIFO(buffer);
		Save_UID(row, col, temp);   //�����յ���UID����UID������
	    //	Show_UID();		//��ʾUID
		temp = read_cmd(ErrorFlag);	//�жϽ����Ƿ���ȷ
		temp = temp&0x01;
		if (temp ==0x00)
		{
			temp = Check_UID();
			if (temp == FALSE)
			{
				return (FM1702_SERNRERR);
			}
			return FM1702_OK;
		}
		else
		{
			temp = read_cmd(CollPos);
			row = (temp/8);
			col = (temp%8);
			buffer[0] = RF_CMD_ANTICOL;
			Set_BitFraming(row+pre_row, col);
			pre_row = pre_row+row;
			for (i=0; i<pre_row+1; i++)
			{
				buffer[i+2] = UID[i];
			}
			if (col != 0x00)
			{
				row = pre_row +1;
			}
			else
			{
				row = pre_row;
			}
			temp = Command_Send(row+2, buffer, Transceive);
		}
	}
}

//����:Check_UID
//����:�ú���ʵ��
//����:
//���
// 5.4.5 select
//����: select_card
//����: �ú���ʵ�ֶԷ���FM1702������Χ֮�ڵ�ĳ�ſ�Ƭ����ѡ��
//���룺N/A
//�����FM1702_NOTAGERR:�޿�
//      FM1702_PARITYERR:��żУ���
//      FM1702_CRCERR:CRCУ���
//      FM1702_BYTECOUNTERR:�����ֽڴ���
//      FM1702_OK:Ӧ����ȷ
//      FM1702_SELERR:ѡ������
uchar Select_Card(void)
{
	uchar temp,i;
	
	write_cmd(CRCPresetLSB,  0x63);
	write_cmd(CWConductance, 0x3f);
	buffer[0] = RF_CMD_SELECT;
	buffer[1] = 0x70;
	for (i=0; i<5; i++)
	{
	    buffer[i+2] = UID[i];
	}
	
	// ChannelRedundancy=0x0f;                            //����CRC,��żУ��
	write_cmd(ChannelRedundancy, 0x0f);
	temp = Command_Send(7, buffer, Transceive);

	if (temp==FALSE)
	{
	   return(FM1702_NOTAGERR);
	}
	else
	{
	    // temp=ErrorFlag;
	    temp = read_cmd(ErrorFlag);
	    if ((temp&0x02)==0x02) return(FM1702_PARITYERR);
	    if ((temp&0x04)==0x04) return(FM1702_FRAMINGERR);
	    if ((temp&0x08)==0x08) return(FM1702_CRCERR);
	      //temp=FIFO_Length;
	    temp = read_cmd(FIFOLength);
	    if (temp!=1)
	    {
	    	return(FM1702_BYTECOUNTERR);
	    }
	    temp = Read_FIFO(buffer);                         //��FIFO�ж�ȡӦ����Ϣ
	    temp = *buffer;
	    if ((temp==0x08) || (temp==0x88) || (temp==0x53))     //�ж�Ӧ���ź��Ƿ���ȷ
	    {
	        return(FM1702_OK);
	    }
	    else
	    {
	        return(FM1702_SELERR);
	    }
	}
}


//��ĳһ��ַ��д������
//addr:Ҫд�ĵ�ַ��
//data:Ҫд������
//ʹ�ö����ĵ�ַ/���ݿڣ���д����ʱ��������:
//1.����ѡ��ҳ(page)�Ĵ�������дA0~A2Ϊ0��D0~D2Ϊҳ��ַ(0~7),D7=1 ��ʾʹ�ö���������
//  Ȼ��ִ��һ��д����
//2.��A0~A2��дĳһҳ�ĵ�ַ������Ҫд�������͵�D0~D7����ִ��һ��д����
//3.���
void  write_cmd(uchar addr,uchar data)
{
	uchar dat;
	uchar delay;
	
    PTADD = 0XFF;   //���
    
	WR = 1;
	RD = 1;
	for (delay=0; delay<10; delay++);
	CS = 0;
	for (delay=0; delay<10; delay++);

	A0 = 0;
	A1 = 0;
	A2 = 0;
	for (delay=0; delay<10; delay++);

	dat  = addr>>3;
	dat &= 0x07;
	dat |= 0x80;
	PTAD = dat;
	for (delay=0; delay<10; delay++);
	
    WR = 0;
 	for (delay=0; delay<10; delay++);
	WR = 1;
	for (delay=0; delay<10; delay++);
	CS = 1;
 	for (delay=0; delay<10; delay++);
	CS = 0;
	for (delay=0; delay<10; delay++);
	
	if ((addr&0x01)>0) A0=1; else A0=0;
	if ((addr&0x02)>0) A1=1; else A1=0;
	if ((addr&0x04)>0) A2=1; else A2=0;
	for (delay=0; delay<10; delay++);
	
	PTAD = data;

	WR = 0;
	for (delay=0; delay<10; delay++);
	WR = 1;
	for (delay=0; delay<10; delay++);
	CS = 1;
}


//��ĳһ��ַ�ж�����
/*
ʹ�ö����ĵ�ַ/���ݿڣ��ڶ�����ʱ��������:
1.����ѡ��ҳ(page)�Ĵ�������дA0~A2Ϊ0��D0~D2Ϊҳ��ַ(0~7),D7=1 ��ʾʹ�ö���������
  Ȼ��ִ��һ��д����
2.��A0~A2��дĳһҳ�ĵ�ַ��Ȼ��Ϳ��Դ�D0~D7��ȡ����
3.���
*/
uchar read_cmd(uchar addr)
{
	uchar dat;
	uchar delay;
	
	WR = 1;
	RD = 1;
	for (delay=0; delay<10; delay++);
	A0 = 0;
	A1 = 0;
	A2 = 0;
	for (delay=0; delay<10; delay++);
	
	dat = addr>>3;
	dat &= 0x07;
	dat |= 0x80;
	PTAD = dat;
	for (delay=0; delay<10; delay++);
	CS = 0;
	for (delay=0; delay<10; delay++);
	WR = 0;
	for (delay=0; delay<10; delay++);
	WR = 1;
	for (delay=0; delay<10; delay++);
	CS = 1;
	
	if ((addr&0x01)>0) A0=1; else A0=0;
	if ((addr&0x02)>0) A1=1; else A1=0;
	if ((addr&0x04)>0) A2=1; else A2=0;
	for (delay=0; delay<10; delay++);
	
	PTADD = 0x00;
	for (delay=0; delay<10; delay++);
	CS = 0;
	for (delay=0; delay<10; delay++);
	RD = 0;
	for (delay=0; delay<10; delay++);
	dat =PTAD;
	PTADD = 0xFF;
	for (delay=0; delay<10; delay++);
	RD = 1;
	for (delay=0; delay<10; delay++);
	CS = 1;
	
	return dat;
}

/*
����:Judge_Req
����:�ú���ʵ�ֶԿ�Ƭ��λӦ���źŵ��ж�
����:*buff,ָ��Ӧ�����ݵ�ָ��
���:TRUE����ƬӦ���ź���ȷ
	     FALSE����ƬӦ���źŴ���
ע: temp1=03;�Ϻ���׼TOKEN��
                 =04;MIFARE��׼8K
                 =05;MIFARE��׼TOKEN��
                 =53;�Ϻ���׼8K��
*/
uchar Judge_Req(uchar *buff)
{
	uchar temp1,temp2;

	temp1 = *buff;
	temp2 = *(buff+1);
	if (((temp1 ==0x03)||(temp1 ==0x04)||(temp1==0x05)||(temp1 ==0x53))&&(temp2 ==0x00))
	{
		return TRUE;
	}
	return FALSE;
}

/*
����:Check_UID
����:�ú���ʵ��
����:
���:
*/

/*
����:Check_UID
����:�ú���ʵ�ֶ��յ��Ŀ�Ƭ�����кŵ��ж�
����:N/A
���:TRUE ���к���ȷ
	      FALSE ���кŴ���
*/
uchar Check_UID(void)
{
	uchar temp;
	uchar i;

	temp =0x00;
	for (i=0;i<5;i++)
	{
		temp = temp^UID[i];
	}
	
	if (temp ==0)
	{
		return TRUE;
	}
	
	return FALSE;
}

/*
����:Save_UID
����:�ú���ʵ�ֱ��濨Ƭ�յ������к�
����: 	row:������ͻ����
	  	col:������ͻ����
	  	length:���յ� UID���ݳ���
���:N/A
*/
void Save_UID(uchar row,uchar col,uchar length)
{
	uchar i;
	uchar temp;
	uchar temp1;

	if ((row ==0x00)&&(col ==0x00))
	{
		for (i=0; i<length; i++)
		{
			UID[i] = buffer[i];
		}
	}
	else
	{
		temp = buffer[0];
		temp1 = UID[row -1];
		switch(col)
		{
			case 0:
				temp1 =0x00;
				row =row+1;
				break;
			case 1:
				temp =temp&0xFE;
				temp1 =temp1&0x01;
				break;
			case 2:
				temp =temp&0xFC;
				temp1 =temp1&0x03;
				break;
			case 3:
				temp =temp&0xF8;
				temp1 =temp1&0x07;
				break;
			case 4:
				temp =temp&0xF0;
				temp1 =temp1&0x0F;
				break;
			case 5:
				temp =temp&0xE0;
				temp1 =temp1&0x1F;
				break;
			case 6:
				temp =temp&0xC0;
				temp1 =temp1&0x3F;
				break;
			case 7:
				temp =temp&0x80;
				temp1 =temp1&0x7F;
				break;
			default:
				break;
		}
		buffer[0] = temp;
		UID[row-1] = temp1|temp;
		for (i=1; i<length; i++)
		{
			UID[row-1+i] = buffer[i];
		}
	}
}

/*
����:Set_BitFraming
����:�ú������ô��������ݵ��ֽ���
����:row:������ȵ���
		col:������ͻ����
���:N/A
*/
void Set_BitFraming(uchar row,uchar col)
{
	switch (row)
	{
		case 0:
			buffer[1] =0x20;
			break;
		case 1:
			buffer[1] =0x30;
			break;
		case 2:
			buffer[1] =0x40;
			break;
		case 3:
			buffer[1] =0x50;
			break;
		case 4:
			buffer[1] =0x60;
			break;
		default:
			break;
	}
	
	switch (col)
	{
		case 0:
			write_cmd(BitFraming,0x00);
			break;
		case 1:
			write_cmd(BitFraming,0x11);
			buffer[1] = (buffer[1]|0x01);
			break;
		case 2:
			write_cmd(BitFraming,0x22);
			buffer[1] = (buffer[1]|0x02);
			break;
		case 3:
			write_cmd(BitFraming,0x33);
			buffer[1] = (buffer[1]|0x03);
			break;
		case 4:
			write_cmd(BitFraming,0x44);
			buffer[1] = (buffer[1]|0x04);
			break;
		case 5:
			write_cmd(BitFraming,0x55);
			buffer[1] = (buffer[1]|0x05);
			break;
		case 6:
			write_cmd(BitFraming,0x66);
			buffer[1] = (buffer[1]|0x06);
			break;
		case 7:
			write_cmd(BitFraming,0x77);
			buffer[1] = (buffer[1]|0x07);
			break;
		default:
			break;
	}
}

//����ѡ��
uchar FM1702_Bus_Sel(void)
{
	uint i;
	
	write_cmd(Page_Sel,0x80);
	for (i=0;i<RF_TimeOut;i++)
	{
		if (read_cmd(Command)==0x00)
		{
			write_cmd(Page_Sel,0x00);
			return TRUE;
		}
	}
	return FALSE;
}

//FM1702��ʼ��
void Init_FM1702(void)
{
	uchar temp=0;
	uint i,j;

	RST = 0; //SetCTL(RST, 0);//MFRST =0;
	for (i=0; i<0x1fff; i++)
	{
	   j = j+1;
	   __RESET_WATCHDOG();
	}

	//����CommandΪ0x00
	j = 0;
	while ((j<0xff) && ((read_cmd(Command))!=0x00))   //�ȴ�Command =0,FM1702��λ�ɹ�
	//while ((read_cmd(Command))!=0x00)   //�ȴ�Command =0,FM1702��λ�ɹ�
	{
		j = j+1;
		__RESET_WATCHDOG();
	}
	
	temp = FM1702_Bus_Sel();      //FM1702 ����ѡ��
	write_cmd(TimerClock,0x0b);   //151us/per
	write_cmd(TimerControl,0x02); //���ͽ�������ʱ�������տ�ʼ�ض�ʱ��
	write_cmd(TimerReload,0x42);  //10ms��ʱ
	write_cmd(InterruptEn,0x7f);  //�������ж�
	write_cmd(InterruptRq,0x7f);
	write_cmd(MFOUTSelect,0x02);  //������
	write_cmd(TxControl,0x5b);    //����TX1 TX2

	write_cmd(0x3a,0x0a);         //��MFOUT
	write_cmd(TypeSH,0x00);
}

/*
����:Read_FIFO
����:�ú���ʵ�ִ�FM1702��FIFO�ж���X bytes����
����:buff,ָ��������ݵ�ָ��
���:N/A
*/
uchar Read_FIFO(uchar *buff)
{
	uchar temp;
	uchar i;

	temp = read_cmd(FIFOLength);
	if (temp ==0)
	{
		return 0;
	}
	if (temp >=24)
	{  		 //temp =255ʱ���������ѭ��
		temp = 24;	//�������FIFO_LengthԽ���ж�
	}			//
	for (i=0;i<temp;i++)
	{
		*(buff+i) = read_cmd(FIFODate);
	}
	return temp;
}

/*
����:Write_FIFO
����:�ú���ʵ����FM1702��FIFOд��Xbytes ������
����: count,��д���ֽڵĳ���
            buff,ָ���д�����ݵ�ָ��
 ���:N/A
*/
void Write_FIFO(uchar count,uchar *buff)
{
	uchar i;
	for (i=0;i<count;i++)
	{
		__RESET_WATCHDOG();
		write_cmd(FIFODate,*(buff+i));
	}
}

/*
����:Clear_FIFO
����:�ú���ʵ�����FM1702��FIFO������
����:N/A
���:TRUE,FIFO�����
	     FALSE,FIFOδ�����
*/
uchar Clear_FIFO(void)
{
	uchar temp;
	uint i;

	temp = read_cmd(Control);  //���FIFO
	temp = (temp | 0x01);
	write_cmd(Control, temp);
	for (i=0; i<RF_TimeOut; i++)
	{					//���FIFO�Ƿ����
		temp = read_cmd(FIFOLength);
		if (temp==0)
		{
			return TRUE;
		}
	}
	
	return FALSE;
}

/*
����:Command_Send
����:�ú���ʵ����FM1702��������Ĺ���
����:count,����������ĳ���
	     buff,ָ����������ݵ�ָ��
	     Comm_Set,������
���:TRUE,�����ȷִ��
	     FALSE,����ִ�д���
*/
uchar Command_Send(uchar count,uchar *buff,uchar Comm_Set)
{
	uint j;
	uchar temp,temp1;
	
	//InterruptRq = 0x7f;
	write_cmd(Command, 0x00);
	temp =Clear_FIFO();
	Write_FIFO(count, buff);
	
	//RxControl2 = 0x01;
	temp = read_cmd(MFOUTSelect);
	write_cmd(Command, Comm_Set);   //����ִ��
	
	for (j=0; j<RF_TimeOut; j++)   		//�������ִ�з�
	{
		__RESET_WATCHDOG();
		temp = read_cmd(MFOUTSelect);
		temp = read_cmd(Command);
		temp1 = (read_cmd(InterruptRq)) & 0x80;
		if ((temp ==0x00) || (temp1==0x80))
		{
			//RxControl2 =0x41;
			return TRUE;
		}
	}
	
	//RxControl2 =0x41;
	return FALSE;
}

/*
//////////////////////////////////////////////////////////////////////////////
//��������
//////////////////////////////////////////////////////////////////////////////
void read_data(void)
{
	uchar temp,temp1;
 
	CHECK_PWD();
  	temp1 = shanqu&0xff;
  	temp = Authentication(UID,temp1,KEYA);  //������֤
  	if (temp == FM1702_OK)
  	{
   		temp = MIF_READ(buffer,temp1*4+block);
  	    if (temp == FM1702_OK)
   		{
     		send_data_tx();   //��ȡ���ݳɹ�
    	}
  	    else
   		{
   		    ack_send_tx(UART_RD_ERR);  //��ȡ���ݴ���
   		}
  	}
    else
  	{
  	    ack_send_tx(UART_AUTH_ERR);    //��֤����
  	}
}*/
/*
void Read_Card_Pro(void)
{
	//����ָ���������Ϣ
  	shanqu= RX_BUFF_TMP[4];
  	shanqu <<=8;
  	shanqu |= RX_BUFF_TMP[5];
  	block = RX_BUFF_TMP[6];
  	if ((shanqu>15)||(block>3))
    {
        //��������ų���16���ų���3,�������
    	ack_send_tx(UART_VAL_ERR);
	}
	else
	{
		read_data();
	}
}*/
/*
void write_data(void)
{
	uchar i,temp,temp1;
    uchar data_b[16];
    
	CHECK_PWD();
	temp1 = shanqu&0xff;
  	temp = Authentication(UID,temp1,KEYA);  //������֤
  	if (temp == FM1702_OK)
    {
    		for (i=0;i<16;i++)
		  	{
			      data_b[i] = RX_BUFF_TMP[7+i];
		  	}
		    temp = MIF_Write(data_b,temp1*4+block);

		    if (temp == FM1702_OK)
			{
			      ack_send_tx(UART_WR_OK);  //д���ݳɹ�
			}
		    else
			{
			      ack_send_tx(UART_WR_ERR);  //д���ݴ���
			}
	}
	else
	{
		    ack_send_tx(UART_AUTH_ERR);    //��֤����
	}
}*/
/*
void write_pro_2(void)
{
	uchar i, UID_SUM;

	UID_SUM =0;
	for (i=0;i<4;i++)
	{
	    UID_SUM += UID[i];
	}
	if (RX_BUFF_TMP[23]!= UID_SUM)
  	{
		  ack_send_tx(UART_CARD_ERR);
  	}
	else
	{
		shanqu= RX_BUFF_TMP[4];
		shanqu <<=8;
		shanqu |= RX_BUFF_TMP[5];
		block = RX_BUFF_TMP[6];
		if ((shanqu>15)||(block>=3)||(RX_BUFF_TMP[2]<20))   //��3��Ϊ������,��ʱ��ֹд,����С��20���Լ�Ҳ��д
		{      //��������ų���16���ų���3,�������
			  ack_send_tx(UART_VAL_ERR);
		}
		else
		{

  			write_data();
		}
 	}
}

void ack_send_tx(uchar ack_da)
{
	TX_BUFF[0] = 0xff;//֡ͷ
	TX_BUFF[1] = 0xff;//֡ͷ
	TX_BUFF[2] = 0x01;//֡����12
	TX_BUFF[3] = 0xaa;//
	TX_BUFF[4] = ack_da;//
	TX_BUFF[5] = TX_BUFF[2]+TX_BUFF[3]+TX_BUFF[4];//

  //RZKDevWrite(SERIAL1,(char*)TX_BUFF,6);
}*/

/*
//����λ���������濨��״̬
void report_card(void)
{
	uchar i;
	TX_BUFF[0] = 0xff;//֡ͷ
	TX_BUFF[1] = 0xff;//֡ͷ
	TX_BUFF[2] = 0x08;//֡����12
	TX_BUFF[3] = 0xcc;//

	TX_BUFF[4] = 0x00;//
	TX_BUFF[5] = 0x00;//
	TX_BUFF[6] = 0x00;//
//	if (FLAG_F2.bit.PWD_SET )
  if (mark_pwd_set ==1) 
		TX_BUFF[7] = 0x01;//
	else
		TX_BUFF[7] = 0x00;//
	TX_BUFF[8] = UID[0];//
	TX_BUFF[9] = UID[1];//
	TX_BUFF[10] = UID[2];//
	TX_BUFF[11] = UID[3];//

	TX_BUFF[12] =0;
	for (i=2;i<=11;i++)
		TX_BUFF[12] +=TX_BUFF[i];

        //RZKDevWrite(SERIAL1,(char*)TX_BUFF,12);

}*/

/*
void reques_again(void)
{
	uchar i;

	TX_BUFF[0] = 0xff;//֡ͷ
	TX_BUFF[1] = 0xff;//֡ͷ
	TX_BUFF[2] = 0x08;//֡����12
	TX_BUFF[3] = 0x77;//

	TX_BUFF[4] = 0x00;//
	TX_BUFF[5] = 0x00;//
	TX_BUFF[6] = 0x00;//
	if (mark_pwd_set == 1)
		TX_BUFF[7] = 0x01;//
	else
		TX_BUFF[7] = 0x00;//
	TX_BUFF[8] = UID[0];//
	TX_BUFF[9] = UID[1];//
	TX_BUFF[10] = UID[2];//
	TX_BUFF[11] = UID[3];//

	TX_BUFF[12] =0;
	for (i=2;i<=11;i++)
		TX_BUFF[12] +=TX_BUFF[i];

       //RZKDevWrite(SERIAL1,(char*)TX_BUFF,13);
}*/
/*
void send_data_tx(void)
{
	uchar i;

	TX_BUFF[0] = 0xff;//֡ͷ
	TX_BUFF[1] = 0xff;//֡ͷ
	TX_BUFF[2] = 17;//֡����17
	TX_BUFF[3] = 0x66;//
	TX_BUFF[4] = 0x00;// ACK
	for (i=0;i<16;i++)
	{
		TX_BUFF[5+i] = buffer[i];
	}

	TX_BUFF[21] =0;
	for (i=2;i<=20;i++)	TX_BUFF[21] +=TX_BUFF[i];

	//RZKDevWrite(SERIAL1,(char*)TX_BUFF,22);
}

//�ղ���
void blank_send(void)
{
	TX_BUFF[0] = 0xff;//֡ͷ
	TX_BUFF[1] = 0xff;//֡ͷ
	TX_BUFF[2] = 0x02;//֡����12
	TX_BUFF[3] = 0x88;//

	TX_BUFF[4] = 0x00;//
	TX_BUFF[5] = 0x00;//
	TX_BUFF[6] = 0x8a;//

        //RZKDevWrite(SERIAL1,(char*)TX_BUFF,7);
}*/

/*
����:Write_E2
����:�ú���ʵ����FM1702��д������
����:lsb,EE��ַ(���ֽ�)
		msb,EE��ַ(���ֽ�)
		count,��д������EE���ֽڸ���
		buff,ָ���д�����ݵ�ָ��
���:	TRUE��EE������ȷд��
		FALSE��EE����д������
*/
/*
uchar Write_E2(uchar lsb,uchar msb,uchar count,uchar *buff)
{
	uchar temp,i;

	for (i=0;i<count;i++)
		{
		*(buff+count-i+2) = *(buff-i+count);
		}
	*buff =lsb;
	*(buff+1) =msb;
	temp = Command_Send(count+2,buff,WriteE2);
	temp = read_cmd(SecondaryStatus);
	temp = temp &0x40;
	if (temp ==0x40)
		{
		return TRUE;
		}
	return FALSE;
}*/

/////////////////////////////////////////////////////
//��Ƭ������������

/*
EEPROM��ַ��Ÿ�ʽ
���	���ַ	byte��ַ	��дȨ��	����
0		0			00~0F		r				��Ʒ��Ϣ��

1		1			10~1F		r/w				�Ĵ�����λ��ʼֵ�����
2		2			20~2F		r/w				�Ĵ�����λ��ʼֵ�����

3		3			30~3F		r/w				�Ĵ�����ʼֵ�����
~
~
7		7			70~7F		r/w				�Ĵ�����ʼֵ�����

8		8			80~8F		w				��������
~
~
31		1F			1F0~1FF		w				��������

IC�������뱣���ַΪ(huanghuaming�趨)
block   ��ַ
0         80~8F
1	    90~9F
~
~
14	    160~16F
15	    170~17F
*/
/*
///////////////////////////////////////////////////////////////////////
//                      C O D E   K E Y S
//    ���������Ÿ�ʽ��λ
///////////////////////////////////////////////////////////////////////
uchar M500HostCodeKey(uchar *uncoded,uchar *coded)      // 6 bytes key value uncoded                                                                        // 12 bytes key value coded
{

   uchar  cnt = 0;
   uchar  ln  = 0;     // low nibble
   uchar  hn  = 0;     // high nibble

   for (cnt = 0; cnt < 6; cnt++)
   	{
      ln = uncoded[cnt] & 0x0F;
      hn = uncoded[cnt] >> 4;
      coded[cnt * 2 + 1] = (~ln << 4) | ln;
      coded[cnt * 2 ] = (~hn << 4) | hn;
   	}
   return 1;
}

//����:Load_Key
//����:�ú���ʵ�ְѻ�����������д��EE
//����:	Block_Adr  ���,��ΧΪ0~63
//		buff  �������ݵĻ�����
//���:TRUE:����װ�سɹ�
//		FALSE:����װ��ʧ��
uchar Load_Key(uchar Secnr2,uchar *buff)
{
	uchar temp,lsb,msb,keybuff[16];
	//Secnr=Block_Adr/4;  //����������


	temp = M500HostCodeKey(buff,keybuff);
	temp = Secnr2*12;    //����EEPROM���������ַ
	if (temp >= 0x80)
		{  //���������ŵ�ַ
		lsb = temp -0x80;
		msb = 0x01;
		}
	else
		{
		msb =0;
		lsb =temp + 0x80;
		}
	temp =Write_E2(lsb,msb,12,keybuff);

	if (temp ==1)
		return TRUE;
	else return FALSE;
	//return temp;
}

//����:Load_keyE2
//����:�ú���ʵ�ְ�E2���������FM1702��keybuffer��
//����:Secnr:EE��ʼ��ַ
//���:TRUE:����װ�سɹ�
//		FALSE:����װ��ʧ��
//�Ӵ˺������Կ���,����Ĵ�ȡ��Χ��:   //HHM
// KEYA:��0x80��ʼ,һ������12�ֽ�
//KEYB:��0x140��ʼ,һ������12�ֽ�
uchar Load_keyE2_CPY(uchar Secnr,uchar Mode)
{
	uchar temp;
	uchar msb=0;
	uchar lsb =0;

	temp = Secnr*12;
	if (Mode ==0)
		{
		if (temp >= 0x80)
			{  //���������ŵ�ַ
			lsb = temp -0x80;
			msb = 0x01;
			}
		else
			{
			msb =0;
			lsb =temp + 0x80;
			}
		}
	else
		{
		msb =0x01;
		lsb = temp+0x40;
		}

	buffer[0] = lsb;
	buffer[1] = msb;
	temp = Command_Send(2,buffer,LoadKeyE2);
	temp = ((read_cmd(ErrorFlag))&0x40);
	if (temp==0x40)
		{
		return FALSE;
		}
	return TRUE;
}
*/

/*
// 5.4.6 AUTHENTICATION
//���ƣ�Authentication
//���ܣ��ú���ʵ��������֤�Ĺ���
//���룺UID: ��Ƭ���кŵ�ַ
//      SecNR: ������
//      mode: ģʽ
//�����FM1702_NOTAGERR:�޿�
//      FM1702_PARITYERR:��żУ���
//      FM1702_CRCERR:CRCУ���
//      FM1702_OK:Ӧ����ȷ
//      FM1702_AUTHERR:Ȩ����֤�д�
uchar  Authentication(uchar  *UID,uchar SecNR,uchar mode)
{
     uchar  i;
     uchar  temp,temp1;

     	//CRCPresetLSB=0x63;
     	//CWConductance=0x3f;
   //  MonConductance=0x3f;
	  write_cmd(CRCPresetLSB,0x63);
	  write_cmd(CWConductance,0x3f);
    // temp1=Control;
    temp1 = read_cmd(Control);
     temp1=temp1&0xf7;
     //Control=temp1;
     write_cmd(Control,temp1);

     if (mode==1)                             //AUTHENT1
         buffer[0]=RF_CMD_AUTH_LB;
     else
         buffer[0]=RF_CMD_AUTH_LA;
     buffer[1]=SecNR*4+3;
     for (i=0;i<4;i++)
     {
        buffer[2+i]=UID[i];
     }
     //ChannelRedundancy=0x0f;                  //����CRC,��żУ��
     write_cmd(ChannelRedundancy,0x0f);
     temp1 =0;
     temp1 = Command_Send(6,buffer,Authent1);
     if (temp1==FALSE)
     {
        return FM1702_NOTAGERR;
     }
    // temp=ErrorFlag;
    temp = read_cmd(ErrorFlag);
     if ((temp&0x02)==0x02)
          return FM1702_PARITYERR;
     if ((temp&0x04)==0x04)
          return FM1702_FRAMINGERR;
     if ((temp&0x08)==0x08)
          return FM1702_CRCERR;

     temp1= 0;
     temp1=Command_Send(0,buffer,Authent2);       //AUTHENT2
     if (temp1==FALSE)
     {
        return  FM1702_NOTAGERR;
     }
     temp=ErrorFlag;
    temp = read_cmd(ErrorFlag);
     if ((temp&0x02)==0x02)
        return  FM1702_PARITYERR;
     if ((temp&0x04)==0x04)
        return  FM1702_FRAMINGERR;
     if ((temp&0x08)==0x08)
        return  FM1702_CRCERR;
     //temp1=Control;
     temp1 = read_cmd(Control);
     temp1=temp1&0x08;                         //Cryptolon=1,��֤ͨ��
     if (temp1==0x08)
     {
        return FM1702_OK;
     }
     return  FM1702_AUTHERR;
   }

//5.4.7 READ
//���ƣ�MIF_Read
//���ܣ��ú���ʵ�ֶ�MIFARE �������ֵ
//����: buff: �������׵�ַ
//      Block_Adr: ���ַ
//�����FM1702_NOTAGERR: �޿�
//      FM1702_PARITYERR:��żУ���
//      FM1702_CRCERR:CRC�����
//      FM1702_BYTECOUNTERR:�����ֽڴ���
//      FM1702_OK:Ӧ����ȷ
uchar MIF_READ(uchar  *buff,uchar Block_Adr)
{
	uchar  temp;

   write_cmd(CRCPresetLSB,0x63);
   write_cmd(CWConductance,0x3f);
   write_cmd(ChannelRedundancy,0x0f);
   buff[0]=RF_CMD_READ;
   buff[1]=Block_Adr;
   temp=Command_Send(2,buff,Transceive);
   if (temp==0)
   {
        return FM1702_NOTAGERR;
   }
   
   temp = read_cmd(ErrorFlag);
   if ((temp&0x02)==0x02) return FM1702_PARITYERR;
   if ((temp&0x04)==0x04) return FM1702_FRAMINGERR;
   if ((temp&0x08)==0x08) return FM1702_CRCERR;
   	
   temp = read_cmd(FIFOLength);
   if (temp==0x10)                        //8k �������ݳ���Ϊ16
   {
     temp = Read_FIFO(buff);
     return FM1702_OK;
   }
   else if (temp==0x04)                   //Token �������ݳ���Ϊ16
   {
     temp = Read_FIFO(buff);
     return FM1702_OK;
   }
   else
   {
      return FM1702_BYTECOUNTERR;
   }
}

// 5.4.8  WRITE
//����: MIF_Write
//���ܣ��ú���ʵ��дMIFARE �������ֵ
//���룺buff: �������׵�ַ
//      Block_Adr:���ַ
//�����FM1702_MOTAGERR:�޿�
//      FM1702_BYTECOUNTERR:�����ֽڴ���
//      FM1702_NOTAUTHERR:δ��Ȩ����֤
//      FM1702_EMPTY:�����������
//      FM1702_CRCERR:CRCУ���
//      FM1702_PARITYERR:��żУ���
//      FM1702_WRITEERR:д�������ݳ���
//      FM1702_OK:Ӧ����ȷ
uchar MIF_Write(uchar  *buff,uchar Block_Adr)
{
      uchar  temp;
      uchar  *F_buff;

 	  write_cmd(CRCPresetLSB,0x63);
	  write_cmd(CWConductance,0x3f);
      F_buff=buff+0x10;
      write_cmd(ChannelRedundancy,0x07);
      *F_buff=RF_CMD_WRITE;
      *(F_buff+1)=Block_Adr;
      temp=Command_Send(2,F_buff,Transceive);
      if (temp==FALSE)
      {
          return(FM1702_NOTAGERR);
      }
      
      temp = read_cmd(FIFOLength);
      if (temp==0)
      {
          return(FM1702_BYTECOUNTERR);
      }
      temp = Read_FIFO(F_buff);
      temp=*F_buff;
      switch(temp)
      {
        case 0x00: return(FM1702_NOTAUTHERR);   //��ʱ���ε�д����
        case 0x04: return(FM1702_EMPTY);
        case 0x0a: break;
        case 0x01: return(FM1702_CRCERR);
        case 0x05: return(FM1702_PARITYERR);
        default:   return(FM1702_WRITEERR);
       }
      temp=Command_Send(16,buff,Transceive);
      if (temp==TRUE)
      {
        return(FM1702_OK);
      }
      else
      {
       // temp=ErrorFlag;
       temp = read_cmd(ErrorFlag);
        if ((temp&0x02)==0x02)
            return(FM1702_PARITYERR);
        else if ((temp&0x04)==0x04)
            return(FM1702_FRAMINGERR);
        else if ((temp&0x08)==0x08)
            return(FM1702_CRCERR);
        else
            return(FM1702_WRITEERR);
      }
}

//����:pwd_convert
//����:�ú���ʵ�������û�����
//          ��ʹ��key_buff�е�˳��,
//          ��shanqu, block,UID[4]��3�����û������������key_pwd
//          ����:
//shanqu block = 0x05 0x03,�ϳ��ֺ��ö����Ʊ�ʾΪ:
//           0 0 0 0 0 1 0 1  0 0 0 0 0 0 1 1
//˳�� f e d c b a 9 8  7 6 5 4 3 2 1 0
//ʹ���û���Ϊ:(����ֻ�����������ֻ���)
//           1 0 3 2 5 4 7 6  9 8 b a d c f e
//���û�������ݱ�Ϊ:
//           1 1 0 0 0 0 0 0  0 1 0 1 0 0 0 0    =0xc050
//    UID= C6 FB 21 81
// C6 FB=1 1 0 0 0 1 1 0 1 1 1 1 1 0 1 1
// ת=   1 1 1 0 1 1 1 1 1 0 0 1 0 0 1 1    =0xef93
//21 81=0 0 1 0 0 0 0 1  1 0 0 0 0 0 0 1
// ת=   0 1 0 0 0 0 1 0  0 1 0 0 1 0 0 0    =0x4248
//����:shanqu, block,UID[4],key_buff[8]
//���:key_pwd[6]
void pwd_convert(void)
{
	uchar key_temp2[16];
	uchar i,j,shift,key_temp[16];
	uint temp,shift_t;

	for (i=0; i<6; i++)
		key_pwd[i] = 0;

//����ת�������Ϳ��
//����ʹ�ÿ���Ϊ����,��Ϊÿ��������4��,�����붼��ͬ
//�����ݶ����λ����������ͬ
//	temp = (shanqu<<8)|block;
	temp = (shanqu<<8)|(shanqu&0xff);
	shift_t = 0x01;
	for (i=0;i<16;i++)
	{
		key_temp[i] = (temp>>i)&shift_t;    //��16λ���ݱ��浽key_temp��
	}

	for (i=0, j=7; i<16; j--,i++)
	{       //����KEY_BUFF�е�˳��������������
		key_temp2[i++] = key_temp[key_buff[j]&0x0f];
		key_temp2[i] = key_temp[(key_buff[j]&0xf0)>>4];
	}

	shift = 0x01;
	for (i=0; i<8; i++)
	{   //�������е������������
		if (key_temp2[i])
			key_pwd[1] |= shift;
		shift <<=1;
	}

	key_pwd[0] = key_pwd[1];

    //�ڶ�ת�����ŵĵ���λ
	temp = (UID[0]<<8)|UID[1];
	shift_t = 0x01;
	for (i=0; i<16; i++)
	{
		key_temp[i] = (temp>>i)&shift_t;    //��16λ���ݱ��浽key_temp��
	}
	
	for (i=0,j=7;i<16;j--,i++)
	{
		key_temp2[i++] = key_temp[key_buff[j]&0x0f];
		key_temp2[i] = key_temp[(key_buff[j]&0xf0)>>4];
	}

	shift =0x01;
	for (i=0;i<8;i++)
	{
		if (key_temp2[i])
			key_pwd[3] |= shift;
		shift <<=1;
	}

	shift =0x01;
	for (i=8;i<16;i++)
	{
		if (key_temp2[i])
			key_pwd[2] |= shift;
		shift <<=1;
	}

    //����ת�����ŵĸ���λ
	temp = (UID[2]<<8)|UID[3];
	shift_t = 0x01;
	for (i=0;i<16;i++)
		key_temp[i] = (temp>>i)&shift_t;    //��16λ���ݱ��浽key_temp��

	for (i=0,j=7;i<16;j--,i++)
	{
		key_temp2[i++] = key_temp[key_buff[j]&0x0f];
		key_temp2[i] = key_temp[(key_buff[j]&0xf0)>>4];
	}

	shift =0x01;
	for (i=0;i<8;i++)
	{
		if (key_temp2[i])
			key_pwd[5] |= shift;
		shift <<=1;
	}

	shift =0x01;
	for (i=8;i<16;i++)
	{
		if (key_temp2[i])
			key_pwd[4] |= shift;
		shift <<=1;
	}
}

//����:load_pwd
//����:�ú���ʵ�������û��ͼ��س���
//����:
//���:
void load_pwd(uchar pwd_type)
{
	uchar temp;//,key_temp[6];

    //	for (temp=0;temp<6;temp++)
    //		key_temp[temp] = key[temp];
  	if (pwd_type==old_pwd)
  	{
		temp = Load_Key(1, key);
  	}
 	else
	{
	    pwd_convert();
	    temp = Load_Key(1, key_pwd);
	}

	temp = Load_keyE2_CPY(1, KEYA);  //��EE������浽keybuffer
}

/////////////////////////////////////////////////////////////////////////
//
//4mSʱ�䴦����򣬵��ñ���������ڲ��ɴ���4mS
//
////////////////////////////////////////////////////////////////////////
void CHECK_PWD(void)
{
	uchar i,j;
		
	for (i=0;i<5;i++)
	{
	    if (UID_TMP[i]!=UID[i])
	    {
			for (j=0;j<5;j++)
				UID_TMP[j]=UID[j];
			break;
		}
	}
		
	if ((i!=5) || (shanqu_tmp!=shanqu))
	{
	 	shanqu_tmp =shanqu;
	    load_pwd(new_pwd);
	}
}*/