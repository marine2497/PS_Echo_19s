
****************修改后记得在main.c文件中修改固件日期**************

20180920 已经进入样机评审阶段。


20181009 珠江测试出现问题：
		 1、高频的发射波约1.5米宽，（正常情况下发射波与回波宽度相差不大）；
		 2、因为发射波比回波大太多，程序中包络统计脉宽有限制 （totalwdth > pFilstWave->chrct[2+0*5]/3）导致计算不到高频水深
		 高频发射波太大的原因有待查找，优化包络统计的脉宽限制。高频发射波太大的原因是换能器出现了问题。
	
20181012 解决开机测量时电压默认为12.50的问题，初始化完成后进行电压采样，加入Get_PowerDcv函数。
		 解决使用时间超过注册时间时，使用时间还会增加的问题
		 
20181019 加入独立看门狗功能。

20181022 问题：如果注册码错误会导致注册时间和使用时间错乱，原因是把注册时间Sys_CB.Time.str_reg_time变量直接传到Decrypt函数时注册码错的情况
		 下会导致导致注册时间和使用时间错乱，解决方法：把解密函数解出来的注册时间放到一个局部变量中，如果解密成功则复制Sys_CB.Time.str_reg_time。

20181030 问题：首次给芯片下载固件，注册时间读取有误！	解决方法：判断是否为首次烧写固件，如果是则把注册时间强制清零。

20181206 修改蜂鸣器报警频率。

20181207
		void TVG_Channel_One_Stop(void)
		{
			HAL_TIM_Base_Stop_IT(&htim4);
			Sys_CB_L.TVG_Dat_Index = 0;   原来为1 修改成0
		
20181213
		1、加入宏定义HF_20us, LF_50us，实现不同的采样频率切换。
		2、加入宏定义DEBUG_UART2，实现调试信息输出，采用UART2。
		3、UART_DEVICE.C 434行
			if(Sys_CB_Copy.Grade.grade != 0 && Sys_CB_Copy.Grade.grade<3)
			{
				Sys_CB_Copy.Grade.grade = 3;
				setCMD_ACK(	"@SIC,,GET,GRADE,", &Sys_CB_Copy.Grade.grade, Byte_Type);
			}
		以上代码造成自动测量时调整高低频参数时，档位会被设置成3挡，该代码的初衷是想解决当前水深超过档位时开启自动测量的情况。
		4、void Sounder_SetDat_Fill(char *p,uint8_t Channel) 修改该函数，使得自动测量下某些参数可调。 

20181214 在UART_DEVICE.C 586 行加入清除使用时间和注册时间代码，需要使用时去除注释即可。


20181218 auto_adjust.c 87行新增如下代码，为了在自动测量时档位由浅到深搜索水深。
		 		Sys_CB.Grade.grade = pautoadjust->searchlevel;
				setCMD_ACK(	"@SIC,,GET,GRADE,", &Sys_CB.Grade.grade, Byte_Type);
		 注释 UART_DEVICE.C 586 行代码

20181219 auto_adjust.c 282行
			if(pEcho_ProcCal->avestrenth < 95 修改为 if(pEcho_ProcCal->avestrenth < 100
			else if(pEcho_ProcCal->avestrenth >120 修改为 else if(pEcho_ProcCal->avestrenth >130
	

20181226
		自动测量模式下使用GET方式上传测量参数，使得在控制软件的参数界面可以看到变化。
		上传参数做了限定，避免参数与波形数据混在一起导致控制软件卡顿。

20181227
		上传数据加入限制
			if((Envelope_Index + (Envelope_Len>>1))>(UART1TXL_Buf_Len-50) || \
				   (validratio>(p_P_SYS_CB->UltraDat_Len * 0.3) && validratio>pCal_Deep->tick))	

		#define UART1TXH_Buf_Len	2500//5120
		#define UART1TXL_Buf_Len	1024//3072


		//发射波的结束点||水深起始点	往后数p_P_SYS_CB->UltraDat_Len * 0.15
		if((i < pCal_Deep->tick-1 || i >= pCal_Deep->tick) && validratio<(p_P_SYS_CB->UltraDat_Len * 0.15)) 
		{	
			validratio++;
			if(*(p_P_SYS_CB->p_UltraDat + i)&0xf0)
			{
				if(*(p_P_SYS_CB->p_UltraDat + i)>= 0xd0)
					*(p_P_SYS_CB->p_UltraDat + i) = 0xc0;
					
				if((Envelope_Len%2)==0)
				{
					*(p_P_SYS_CB->p_UART_Dat + Envelope_Index + (Envelope_Len>>1)) = *(p_P_SYS_CB->p_UltraDat + i)&0xf0;
				}
				else
				{
					*(p_P_SYS_CB->p_UART_Dat + Envelope_Index + (Envelope_Len>>1)) |= *(p_P_SYS_CB->p_UltraDat + i)>>4;
				}
				Envelope_Len++;			
				if(Envelope_Len==1)			// 只在第一个数的时候记录坐标
				{
					if(First_Valid_Index==0)
						First_Valid_Index = i;

					//if(i - First_Valid_Index>0)
					//	EnvelopeStart_Index = i - First_Valid_Index -pCal_Deep->errortick; //减去误差，保证水深线与波形一致
					//else
						EnvelopeStart_Index = i - First_Valid_Index;
				}
				
				if(p_P_SYS_CB->p_UART_Dat==UART1_TxBufL)		// 低频
				{
					//限制上传回波数量
					if((Envelope_Index + (Envelope_Len>>1))>(UART1TXL_Buf_Len-50))	// 防止越界
					{					
						break;
					}
				}
				else
				{
					//限制上传回波数量
					if((Envelope_Index + (Envelope_Len>>1))>(UART1TXH_Buf_Len-150))		// 防止越界
					{
						break;
					}				
				}
/*
				validratio++;			
				if(p_P_SYS_CB->p_UART_Dat==UART1_TxBufL)		// 低频
				{
					//限制上传回波数量
					if((Envelope_Index + (Envelope_Len>>1))>(UART1TXL_Buf_Len-50) || \
					   (validratio)>(p_P_SYS_CB->UltraDat_Len * 0.3))//30%			// 防止越界
					{					
						break;
					}
				}
				else
				{
					//限制上传回波数量
					if((Envelope_Index + (Envelope_Len>>1))>(UART1TXH_Buf_Len-150) || \
					   (validratio)>(p_P_SYS_CB->UltraDat_Len * 0.3))//30%			// 防止越界
					{
						break;
					}				
				}*/
			}
			else
			{
				if(Envelope_Len)
				{
					Envelope_Len >>= 1;				// 算出实际的数据字节数
					Envelope_Len += 4;				// 加上4个字节空间，即包字节数和偏移量
					*(p_P_SYS_CB->p_UART_Dat + Envelope_Index - 4) = Envelope_Len>>8;			// 当前包络采样点个数
					*(p_P_SYS_CB->p_UART_Dat + Envelope_Index - 3) = Envelope_Len;
					*(p_P_SYS_CB->p_UART_Dat + Envelope_Index - 2) = EnvelopeStart_Index>>8;	// 当前包络距离原点间隔周期数
					*(p_P_SYS_CB->p_UART_Dat + Envelope_Index - 1) = EnvelopeStart_Index;
					Envelope_Index += Envelope_Len;
					Envelope_Len = 0;
				}
			}
		
			
		}
		else if(i >= pCal_Deep->tick-1 && validratio>=(p_P_SYS_CB->UltraDat_Len * 0.15)) 
			validratio = 0;	
		



20190528 定制无人船固件
1、上电自动测量
2、@SIC,,GET,DATA.DEEP,OK,HF,123.45*CRC\r\n 
3、@SIC,,GET,DATA.DEEP,OK,LF,123.45*CRC\r\n
define Unmanned_ship

20190529 出现用户使用到30小时后注册失败。

2019-9-2	增加串口2，功能与串口1一致，用于海星2

2019-9-11   去掉手动测量功率限制


2019-09-23 修改采样电压 26.32x^3 -86.86x^2 +108.7x -36.75

2019-12-05 串口2输出增加4条指令，让网页与pc的参数同步，，，另外修改了regtime.c文件的flash操作，把存在的隐患修正