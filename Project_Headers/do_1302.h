

extern void  NopDelay(unsigned int num);

/*****************************************************************
*****        DS1302 COMMAND START              *************
*****************************************************************/
#define	 _REGISTER_DS1302_SEC_					(0x80)|(0x0<<1)
#define	 _REGISTER_DS1302_MIN_					(0x80)|(0x1<<1)
#define	 _REGISTER_DS1302_HR_					(0x80)|(0x2<<1)
#define	 _REGISTER_DS1302_DATE_					(0x80)|(0x3<<1)
#define	 _REGISTER_DS1302_MONTH_				(0x80)|(0x4<<1)
#define	 _REGISTER_DS1302_DAY_					(0x80)|(0x5<<1)
#define	 _REGISTER_DS1302_YEAR_					(0x80)|(0x6<<1)
#define	 _REGISTER_DS1302_CONTROL_				(0x80)|(0x7<<1)
#define	 _REGISTER_DS1302_TRICKLE_CHARGER_		(0x80)|(0x8<<1)
#define	 _REGISTER_DS1302_CLOCK_BURST_			(0x80)|(0x1F<<1)

#define	 _RAM_DS1302_RAM(i)_					(0xC0)|(i<<1)
#define  _RAM_DS1302_BURST_						(0xC0)|(0x1F<<1)


//#define 	io_ds1302clk 	p2_3
//#define 	io_ds1302rst 	p2_2
//#define 	io_ds1302dat 	p2_1


#define		io_ds1302clk 			PTGD_PTGD0		// G0
#define		io_ds1302dat 			PTGD_PTGD1		// G1
#define		io_ds1302rst 			PTGD_PTGD2		// G2 时钟芯片 



//===========================================
#define  bitset(name,bitnumber) ( (name) |= 1<<(bitnumber) )
#define  bitclr(name,bitnumber) ( (name) &= ~(1<<bitnumber) )



void nop(void) 
{
	uchar i;   
	for(i=0;i<10;i++)
		;
}

//==================================================================== 
//	复位    reset the ds1302
//----------------------------------------------------------
void ds1302_reset(void)
{	  
	PTGDD |= 0x02;	
	nop();
	io_ds1302clk = 0; 
	nop();
	io_ds1302rst = 0; 
	nop();
	io_ds1302rst = 1; 
	nop();
}

//==================================================================== 
//	写       write a single byte data into ds1302
//----------------------------------------------------------
void ds1302_write_byte(uchar data_ds1302)
{	
	uchar i;
	//pd2_1 = 1;
	PTGDD |= 0x02;
	for(i=0;i<=7;i++)
	{ 
		io_ds1302clk = 0; 
		nop();
		if( (data_ds1302&0x01)==0 ) 
			io_ds1302dat = 0; 
		else 
			io_ds1302dat = 1; 
		nop(); 
		io_ds1302clk = 1; 
		nop();
		data_ds1302	= data_ds1302 >> 1; 
		nop();
	}
}

//==================================================================== 
//	读       read a single byte data from ds1302
//----------------------------------------------------------
uchar ds1302_read_byte(void)
{	
	uchar i,Data_ds1302r;
	Data_ds1302r = 0; 
	//pd2_1 = 0;	
	PTGDD &= 0xfd;
	
	for(i=0;i<=7;i++)
	{
		nop(); 
		io_ds1302clk =	0; 
		nop();
		if(io_ds1302dat) 
			bitset(Data_ds1302r,i); 
		nop();
		io_ds1302clk = 1; 
		nop();
	}
	return Data_ds1302r; 
}

//==================================================================== 
//	读    get current time & date
//----------------------------------------------------------
void get_current_time(void)		
{
	uchar hh1,mm1,hh2,mm2,bzz; 
	ds1302_reset();
	ds1302_write_byte(0b10111111);
	T1302_sec = ds1302_read_byte();			
	mm1 = ds1302_read_byte();	
	hh1 = ds1302_read_byte();
	T1302_day = ds1302_read_byte();	 
	T1302_mth = ds1302_read_byte();	
	T1302_week= ds1302_read_byte();		// 这个week指的是1911年的，不是2011年的，所以不准
	T1302_year= ds1302_read_byte(); 	
	//current_date= ds1302_read_byte();	//ri
	//current_date= ds1302_read_byte();	//yue
	//current_date= ds1302_read_byte();	//zhou
	io_ds1302rst = 0;  
	                                      
	ds1302_reset();
	ds1302_write_byte(0b10111111);
	T1302_sec = ds1302_read_byte();			
	mm2 = ds1302_read_byte();	
	hh2 = ds1302_read_byte(); 
	io_ds1302rst = 0;  
	
	bzz = 0;
	if( (hh1==hh2)&&(mm1==mm2) )
	{
		if( (hh1&0xf0)==0 )
		{
			if( (hh1&0x0f)>=0x0a )
				bzz = 10;
		}
		else if( (hh1&0xf0)==0x10 )
		{
			if( (hh1&0x0f)>=0x0a )
				bzz = 10;
		}
		else if( (hh1&0xf0)==0x20 )
		{
			if( (hh1&0x0f)>=0x04 )
				bzz = 10;
		}
		else
			bzz = 10;
		
		if( (mm1&0xf0)<0x60 )
		{
			if( (mm1&0x0f)>=0x0a )
				bzz = 10;
		} 
		else
			bzz = 10;
	}
	else
		bzz = 5;

	if(bzz==0)
	{
		T1302_min = mm1;
		T1302_hour= hh1; 
	} 
	else if(bzz==10)
	{
		SD_hour = 0x00; 	
		//saving_clokh();					//储存 北京时间 
		SD_min = 0x00; 
		//saving_clokm();					//储存 
	}	
}

//========================================================== 
// 初始化函数
//----------------------------------------------------------
//时钟设定为24小时
void pow_1302init(void)
{
	uchar hour,min,second;	
	ds1302_reset();
	ds1302_write_byte(0b10111111);		//读时钟,Burst模式
	second= ds1302_read_byte();			//秒
	min  = ds1302_read_byte();			//分
	hour = ds1302_read_byte();			//时	
	io_ds1302rst = 0;
	second &= 0b01111111;
	hour &= 0b00111111;
	
	ds1302_reset();
	ds1302_write_byte(0b10001110);
	ds1302_write_byte(0b00000000);		//去写保护
	ds1302_reset();								
	ds1302_write_byte(0b10000000);				
	ds1302_write_byte(second);			//写时钟 地址0-秒
	ds1302_reset();								 
	ds1302_write_byte(0b10000100);		//写时钟 地址2-时
	ds1302_write_byte(hour);	
	ds1302_reset();
	ds1302_write_byte(0b10001110);
	ds1302_write_byte(0b10000000);		//使能写保护	
}
  

// 0秒 1分 2时 3日 4月 5周 6年
//时钟 hour                     
void saving_clock(void)
{
	uchar ss0,ss1,ss2; 
	
	ds1302_reset();
	ds1302_write_byte(0b10001110);				//去写保护
	ds1302_write_byte(0b00000000);	 
	
	ds1302_reset();
	ds1302_write_byte(0b10000000);				// 0-秒 
	ss0 = SD_sec/10;
	ss1 = SD_sec%10;
	ss2 = (ss0<<4)|ss1;
	ds1302_write_byte(ss2);		
	
	ds1302_reset();
	ds1302_write_byte(0b10000010);				// 1-分	
	ss0 = SD_min/10;
	ss1 = SD_min%10;
	ss2 = (ss0<<4)|ss1;
	ds1302_write_byte(ss2);						
	
	ds1302_reset();
	ds1302_write_byte(0b10000100);				// 2-时 地址
	ss0 = SD_hour/10;
	ss1 = SD_hour%10;
	ss2 = (ss0<<4)|ss1;
	ds1302_write_byte(ss2);     				 
	
	ds1302_reset();
	ds1302_write_byte(0b10000110);				// 3-日 地址
	ss0 = SD_day/10;
	ss1 = SD_day%10;
	ss2 = (ss0<<4)|ss1;
	ds1302_write_byte(ss2);     				 
	
	ds1302_reset();
	ds1302_write_byte(0b10001000);				// 4-月 地址
	ss0 = SD_mth/10;
	ss1 = SD_mth%10;
	ss2 = (ss0<<4)|ss1;
	ds1302_write_byte(ss2);     			 
	
	ds1302_reset();
	ds1302_write_byte(0b10001100);				// 6-年 地址
	ss0 = SD_year/10;
	ss1 = SD_year%10;
	ss2 = (ss0<<4)|ss1;
	ds1302_write_byte(ss2);     				 
	
	ds1302_reset();
	ds1302_write_byte(0b10001110);
	ds1302_write_byte(0b10000000);				//启动写保护
	io_ds1302rst = 0; 
}





