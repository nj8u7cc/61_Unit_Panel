
#ifndef __H_PROTOCOL__
  #define __H_PROTOCOL__
  #include "typedefine.h"
  #include "list.h"

  #define   snd321     29
  #define   snd200ms   30

//=====================================
// �ӻ�-->Arm  ָ��
#define    Answer_zt      0x02				// ��ѯ���� 
#define    ACK_slvtx	  0x03 				// ACK
#define    Safe_ztai      0x09				// ����
#define    Tmp_zt         0x0A				// �¶�
#define    Tlph_com       0x0F				// �绰
#define    Rout_zt        0x10				// ��·
#define    Dtmf_com       0x11 				// DTMF
#define    Pwer_offkey    0x12 				// ���¹ػ�����
#define    Check_Screen   0x17 				//
 
// Arm-->�ӻ�  ָ��
#define	   HOST_QUERY			0X01		// ��ѯ
#define	   HOST_ACK				0x03		// ��������ACK
#define	   HOST_SHUTDOWN		0x04		// �ػ�
#define    HOST_SETTIME			0x05		// �趨ʱ��
#define	   HOST_VOLUME			0x06		// ����
#define	   HOST_Led				0x07		// ָʾ�ƿ���
#define	   HOST_Rly				0x08		// �̵�������
#define	   HOST_TakPhn			0x0B		// ժ��
#define	   HOST_Hangphn			0x0C		// �һ�
#define	   HOST_Dial			0x0D		// ����
#define	   HOST_Dtmf			0x0E		// ����DTMF
#define	   HOST_RstArm			0x13		// ��λarm
#define	   HOST_EchoCoOn		0x14		// ����������
#define	   HOST_EchoCOff		0x15		// ���������ر�
#define	   HOST_DvcType			0x16		// �����豸����
#define	   HOST_MIC_VOLUME		0x18		// Mic����

#define	   COMMAND_7037			0x21		// 


extern uchar volume;
extern uchar volumeBF;
extern uchar mic_volume;
extern uchar mic_volumeBF;
void volume_dwq_to( _UBYTE count);
void reset_host(void);
void get_current_time(void);
void set_7037_mic_volume(_UBYTE vol);

//=====================================================================
// �绰ģ�鷢����Ƭ��
#define		TEL_answer					0X52			// �绰ģ��ظ�
#define		TEL_ack						0X53			// ACK
#define		TEL_come_call				0X57			// ����
#define		TEL_dtmf					0X58			// DTMF
#define		TEL_routstat				0X59			// ��·״̬

// ��Ƭ�������绰ģ��
#define		CTEL_ask					0X51			// ��ѯ4
#define		CTEL_open					0X54			// ժ��
#define		CTEL_halloff				0X55			// �һ�
#define		CTEL_dial					0X56			// ����
#define     CDtmf_dial                  0x5f            // DTMF����


//=============================================================
// ���䷢�͵�֡  ��������
void  send2host(void)
{
	uchar i;
	if(snd_frm<6)
		return;
	// Sndbuf[x][6]�������  Sndbuf[x][27] = 0xfd  Sndbuf[x][snd321]���͵�123���� Sndbuf[x][snd200ms]��ʱ200ms 
	for(i=0;i<5;i++)
	{
		if(Sndbuf[i][snd321]>0)
		{
			if(Sndbuf[i][snd200ms]>20)
			{
				SCI2C2_TE = 1;	   				// ʹ�ܴ���  TIE TCIE RIE ILIE TE RE RWU
				SCI2C2_TCIE = 1;				// ���ͽ��� �ж�ʹ��
				
				Sndbuf[i][snd321]--;
				Sndbuf[i][snd200ms] = 0;
				snd_frm = i; 
				snd_p = 0;
				
				SCI2D = 0xf5;		  			// pChar[ (*pPos)++ ];		
				return;
			}
		}
	}
}




// ׼�� Ӧ��/���� ���ݰ� 
void  prepare_sndarm_pack(uchar rg)
{
	uchar i,j,sum; 
	uchar t1,t2; 
	uint yy;
	// Sndbuf[x][6]�������  Sndbuf[x][27] = 0xfd  Sndbuf[x][snd321]���͵�123���� Sndbuf[x][snd200ms]��ʱ200ms  
	j = 10;
	for(i=0;i<5;i++)
	{
		if(Sndbuf[i][snd321]==0)
		{
			j = i;
			break;
		}
	}
	if(j==10)										// ���з��ͻ���������
		return;
	
	Sndbuf[j][0] = 0xf5;	
	Sndbuf[j][1] = 0x00;							// ����
	Sndbuf[j][2] = 0x00;//pot_a; 
	Sndbuf[j][3] = 0x00;
	Sndbuf[j][4] = 0x00;
	Sndbuf[j][5] = 0x00;							// Ԥ��
	snd_BWenNo++;
	if(snd_BWenNo==0)
		snd_BWenNo = 1;
	Sndbuf[j][6] = snd_BWenNo;						// ������� 
	Sndbuf[j][7] = rg;								// ָ��   
	
	switch(rg)
	{
		case  Answer_zt:            				// ��ѯ���� 
			Sndbuf[j][1] = 0x1B;					// ����
			Sndbuf[j][6] = rsv_ack;				// �յ���arm�����
			Sndbuf[j][8] = rut_zt;					// ��·״̬
			Sndbuf[j][9] = ad_tmp;					// �����¶�
			Sndbuf[j][10] = tlphn_zt;				// �绰�ֻ�״̬ 
			
			yy = 2000; 
			yy += (T1302_year>>4)*10; 
			yy += (T1302_year&0x0f); 			 
			Sndbuf[j][11] = yy&0xff;			   	// ������ ʱ���� 
			Sndbuf[j][12] = yy>>8;		
			Sndbuf[j][13] = T1302_mth;		 		
			Sndbuf[j][14] = T1302_day;			 	
			Sndbuf[j][15] = T1302_hour;			
			Sndbuf[j][16] = T1302_min;			 	
			Sndbuf[j][17] = T1302_sec;				// ����״̬
			for(i=0;i<5;i++)
			{
				t1 = Sndbuf[j][13+i]>>4;
				t2 = Sndbuf[j][13+i]&0x0f;
				Sndbuf[j][13+i] = t1*10 + t2;		// ��ʱ��BCD��ת��Ϊ16����
			}
			for(i=0;i<8;i++)
				Sndbuf[j][18+i] = safe_zt[i]; 
			Sndbuf[j][26] = safe_jj; 				// ��������
			Sndbuf[j][snd321] = 1;					// ���Է���1��
			break;   
		
		case ACK_slvtx:								// ACK
			Sndbuf[j][1] = 0x09;	
			Sndbuf[j][6] = rsv_ack;				// �յ���arm�����
			Sndbuf[j][8] = 2;						// 2��ʾ����
			Sndbuf[j][snd321] = 1;					// ���Է���1��
			break; 
		
		case  Safe_ztai:              				// ����  
			Sndbuf[j][1] = 0x11;					// ����
			for(i=0;i<8;i++)
				Sndbuf[j][8+i] = safe_zt[i];  
			Sndbuf[j][16] = safe_jj; 				// ������ť
			
			Sndbuf[j][snd321] = 1;					// ���Է���3��
			break; 
		
		case Pwer_offkey:							// ���µ�Դ����
		case Check_Screen:							// ������Ļ
			Sndbuf[j][1] = 0x08;	  
			Sndbuf[j][snd321] = 3;					// ���Է���3��
			break;
	}
	if(Sndbuf[j][1]>0)
	{ 
		Sndbuf[j][snd200ms] = 240;					// ��ʱ���Է���
		sum = 0;
		for(i=1;i<Sndbuf[j][1];i++)				// F5 F5 08 00 00 00 00 01 01 08 FD
			sum ^= Sndbuf[j][i];
		i = Sndbuf[j][1];
		Sndbuf[j][i] = sum;
		Sndbuf[j][i+1] = 0xfd;
	}
}
 
 
 
//-----------------------------------------------------------------------------------------
// �����յ�led״̬ת��Ϊ0 1 2 3���Ա�����ledzt[]
uchar led_rx_onoff(uchar rz,uchar no)
{
	uchar i;
	i = no*2;							// 0  1 2 3
	return (rz>>i)&0x03;
}

// Ӧ������  ����ת�沢����
void  respond2Host(void)
{
	uchar i,len,sum; 
	if(rsvover==0)
		return; 
	rsv_p = 0;
	for(i=0;i<32;i++)  
		rsvbufm[i] = rsvbufi[i];  
		
	for(i=0;i<32;i++) 
		rsvbufi[i] = 0;  								//  ����ջ�����
		
	rsvover = 0;
	
	len = rsvbufm[1];									// F5 F5 08 00 00 00 00 01 01 08 FD
	sum = 0;
	for(i=1;i<len;i++)
		sum ^= rsvbufm[i];
	if( (sum!=rsvbufm[len])||(rsvbufm[len+1]!=0xfd) )
		return;
	
	if(bw_armbf==rsvbufm[6])							// ���������ͬ�Ĳ�����
		return;
	bw_armbf = rsvbufm[6];
	
	rst_armt = 1;										// �յ�arm���ݱ�ʾarm����������������������  =0��ܿ췢�Ͱ�������
	
	rsv_ack =  rsvbufm[6];

	switch( rsvbufm[7] )
	{
		case HOST_QUERY:								// ��ѯ 
			prepare_sndarm_pack(Answer_zt);				// ׼����ѯ�ķ��Ͱ�
			break;
			
		case HOST_ACK:									// ��������ACK
			for(i=0;i<5;i++)							// F5 F5 09 00 00 00 00 01 03 01 0A FD
			{
				if(Sndbuf[i][6]==rsvbufm[6])			// ������� 
					Sndbuf[i][snd321] = 0; 
			}	// Sndbuf[x][snd321]���͵�123���� Sndbuf[x][snd200ms]��ʱ200ms   
			break; 
		
		case HOST_SHUTDOWN:								// �ػ�
			prepare_sndarm_pack(Answer_zt);				// F5 F5 08 00 00 00 00 01 04 0D FD
			rst_armt = 295;								// ��������Ͽ��ֱպ��ˣ�5�����������Arm
			uuuu = 5;
			break;
		
		//-----------------------------------------------------
		case HOST_VOLUME:								// ���� F5 F5 09 00 00 00 00 01 06 02 0C FD
			prepare_sndarm_pack(Answer_zt);				//      F5 F5 09 00 00 00 00 01 06 00 0E FD 
			volume = rsvbufm[8];						// 
			if(volume>10)
				volume = 10;
			if( (volume==0)&&(volumeBF!=volume) )
				init7037(); 	
			volumeBF = volume;
			volume_dwq_to(Volv_lvel[volume][0]);	//      F5 F5 09 00 00 00 00 01 06 0E 00 FD 
			break;
			
		case HOST_MIC_VOLUME:
			prepare_sndarm_pack(Answer_zt);
			mic_volume = rsvbufm[8];
			if(((mic_volume>=0)&&(mic_volume<=0x0A))||
					((mic_volume>=0x16)&&(mic_volume<=0x1F))){
				set_7037_mic_volume(mic_volume);
				mic_volumeBF = mic_volume;
			}
			break;
			
		case HOST_DvcType:								// �����豸����  ���Ͳ�ͬ�� MIC���治ͬ
			i = rsvbufm[8];
			prepare_sndarm_pack(Answer_zt);				// ACK 
			break;
		//-----------------------------------------------------------
		case HOST_Led:									// ָʾ�ƿ���
			prepare_sndarm_pack(Answer_zt);				// ACK   F5 F5 0C 00 00 00 00 01 07 01 01 00 0F 05 FD
			for(i=0;i<4;i++)
			{
				sum = led_rx_onoff(rsvbufm[10],i);		 // ���� ֹͣ ͨ�� ���� , δ�� ͨѶ ���� ����
				if( sum > 0 )
					ledzt[4+i] = sum;
				sum = led_rx_onoff(rsvbufm[11],i);
				if( sum > 0 )
					ledzt[i] = sum;
			}
			
			//if(ledzt[Baojing]==3)
			{
				//volume_dwq_to(Volv_lvel[10][0]);	//      F5 F5 09 00 00 00 00 01 06 0E 00 FD 
				//write7037byte(ADDRESS_ML7037_CR4,Volv_lvel[10][1]);
			}
			break;
		
		//-----------------------------------------------
		case HOST_RstArm:								// ��λarm
			prepare_sndarm_pack(Answer_zt);				// ACK 
			reset_host();
			break;
		
		case HOST_EchoCoOn:								// ����������
			prepare_sndarm_pack(Answer_zt);				// ACK 
			write7037byte(ADDRESS_ML7037_CR12,0x4C);
			break;
		
		case HOST_EchoCOff:								// ���������ر�
			prepare_sndarm_pack(Answer_zt);				// ACK 
			write7037byte(ADDRESS_ML7037_CR12,0x04);
			break;
		
		case COMMAND_7037:								// 
			switch(rsvbufm[8])
			{				
			 	case 3:									// POWER_DOWN_7037:
					power_down7037();
					break;
			 	case 4:									// INITIAL_7037:
					init7037();
					break; 
			}
			break; 
	} 
}
 


#endif



