
****************�޸ĺ�ǵ���main.c�ļ����޸Ĺ̼�����**************

20180920 �Ѿ�������������׶Ρ�


20181009 �齭���Գ������⣺
		 1����Ƶ�ķ��䲨Լ1.5�׿�����������·��䲨��ز�������󣩣�
		 2����Ϊ���䲨�Ȼز���̫�࣬�����а���ͳ������������ ��totalwdth > pFilstWave->chrct[2+0*5]/3�����¼��㲻����Ƶˮ��
		 ��Ƶ���䲨̫���ԭ���д����ң��Ż�����ͳ�Ƶ��������ơ���Ƶ���䲨̫���ԭ���ǻ��������������⡣
	
20181012 �����������ʱ��ѹĬ��Ϊ12.50�����⣬��ʼ����ɺ���е�ѹ����������Get_PowerDcv������
		 ���ʹ��ʱ�䳬��ע��ʱ��ʱ��ʹ��ʱ�仹�����ӵ�����
		 
20181019 ����������Ź����ܡ�

20181022 ���⣺���ע�������ᵼ��ע��ʱ���ʹ��ʱ����ң�ԭ���ǰ�ע��ʱ��Sys_CB.Time.str_reg_time����ֱ�Ӵ���Decrypt����ʱע���������
		 �»ᵼ�µ���ע��ʱ���ʹ��ʱ����ң�����������ѽ��ܺ����������ע��ʱ��ŵ�һ���ֲ������У�������ܳɹ�����Sys_CB.Time.str_reg_time��

20181030 ���⣺�״θ�оƬ���ع̼���ע��ʱ���ȡ����	����������ж��Ƿ�Ϊ�״���д�̼�����������ע��ʱ��ǿ�����㡣

20181206 �޸ķ���������Ƶ�ʡ�

20181207
		void TVG_Channel_One_Stop(void)
		{
			HAL_TIM_Base_Stop_IT(&htim4);
			Sys_CB_L.TVG_Dat_Index = 0;   ԭ��Ϊ1 �޸ĳ�0
		
20181213
		1������궨��HF_20us, LF_50us��ʵ�ֲ�ͬ�Ĳ���Ƶ���л���
		2������궨��DEBUG_UART2��ʵ�ֵ�����Ϣ���������UART2��
		3��UART_DEVICE.C 434��
			if(Sys_CB_Copy.Grade.grade != 0 && Sys_CB_Copy.Grade.grade<3)
			{
				Sys_CB_Copy.Grade.grade = 3;
				setCMD_ACK(	"@SIC,,GET,GRADE,", &Sys_CB_Copy.Grade.grade, Byte_Type);
			}
		���ϴ�������Զ�����ʱ�����ߵ�Ƶ����ʱ����λ�ᱻ���ó�3�����ô���ĳ�����������ǰˮ�����λʱ�����Զ������������
		4��void Sounder_SetDat_Fill(char *p,uint8_t Channel) �޸ĸú�����ʹ���Զ�������ĳЩ�����ɵ��� 

20181214 ��UART_DEVICE.C 586 �м������ʹ��ʱ���ע��ʱ����룬��Ҫʹ��ʱȥ��ע�ͼ��ɡ�


20181218 auto_adjust.c 87���������´��룬Ϊ�����Զ�����ʱ��λ��ǳ��������ˮ�
		 		Sys_CB.Grade.grade = pautoadjust->searchlevel;
				setCMD_ACK(	"@SIC,,GET,GRADE,", &Sys_CB.Grade.grade, Byte_Type);
		 ע�� UART_DEVICE.C 586 �д���

20181219 auto_adjust.c 282��
			if(pEcho_ProcCal->avestrenth < 95 �޸�Ϊ if(pEcho_ProcCal->avestrenth < 100
			else if(pEcho_ProcCal->avestrenth >120 �޸�Ϊ else if(pEcho_ProcCal->avestrenth >130
	

20181226
		�Զ�����ģʽ��ʹ��GET��ʽ�ϴ�����������ʹ���ڿ�������Ĳ���������Կ����仯��
		�ϴ����������޶�����������벨�����ݻ���һ���¿���������١�

20181227
		�ϴ����ݼ�������
			if((Envelope_Index + (Envelope_Len>>1))>(UART1TXL_Buf_Len-50) || \
				   (validratio>(p_P_SYS_CB->UltraDat_Len * 0.3) && validratio>pCal_Deep->tick))	

		#define UART1TXH_Buf_Len	2500//5120
		#define UART1TXL_Buf_Len	1024//3072


		//���䲨�Ľ�����||ˮ����ʼ��	������p_P_SYS_CB->UltraDat_Len * 0.15
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
				if(Envelope_Len==1)			// ֻ�ڵ�һ������ʱ���¼����
				{
					if(First_Valid_Index==0)
						First_Valid_Index = i;

					//if(i - First_Valid_Index>0)
					//	EnvelopeStart_Index = i - First_Valid_Index -pCal_Deep->errortick; //��ȥ����֤ˮ�����벨��һ��
					//else
						EnvelopeStart_Index = i - First_Valid_Index;
				}
				
				if(p_P_SYS_CB->p_UART_Dat==UART1_TxBufL)		// ��Ƶ
				{
					//�����ϴ��ز�����
					if((Envelope_Index + (Envelope_Len>>1))>(UART1TXL_Buf_Len-50))	// ��ֹԽ��
					{					
						break;
					}
				}
				else
				{
					//�����ϴ��ز�����
					if((Envelope_Index + (Envelope_Len>>1))>(UART1TXH_Buf_Len-150))		// ��ֹԽ��
					{
						break;
					}				
				}
/*
				validratio++;			
				if(p_P_SYS_CB->p_UART_Dat==UART1_TxBufL)		// ��Ƶ
				{
					//�����ϴ��ز�����
					if((Envelope_Index + (Envelope_Len>>1))>(UART1TXL_Buf_Len-50) || \
					   (validratio)>(p_P_SYS_CB->UltraDat_Len * 0.3))//30%			// ��ֹԽ��
					{					
						break;
					}
				}
				else
				{
					//�����ϴ��ز�����
					if((Envelope_Index + (Envelope_Len>>1))>(UART1TXH_Buf_Len-150) || \
					   (validratio)>(p_P_SYS_CB->UltraDat_Len * 0.3))//30%			// ��ֹԽ��
					{
						break;
					}				
				}*/
			}
			else
			{
				if(Envelope_Len)
				{
					Envelope_Len >>= 1;				// ���ʵ�ʵ������ֽ���
					Envelope_Len += 4;				// ����4���ֽڿռ䣬�����ֽ�����ƫ����
					*(p_P_SYS_CB->p_UART_Dat + Envelope_Index - 4) = Envelope_Len>>8;			// ��ǰ������������
					*(p_P_SYS_CB->p_UART_Dat + Envelope_Index - 3) = Envelope_Len;
					*(p_P_SYS_CB->p_UART_Dat + Envelope_Index - 2) = EnvelopeStart_Index>>8;	// ��ǰ�������ԭ����������
					*(p_P_SYS_CB->p_UART_Dat + Envelope_Index - 1) = EnvelopeStart_Index;
					Envelope_Index += Envelope_Len;
					Envelope_Len = 0;
				}
			}
		
			
		}
		else if(i >= pCal_Deep->tick-1 && validratio>=(p_P_SYS_CB->UltraDat_Len * 0.15)) 
			validratio = 0;	
		



20190528 �������˴��̼�
1���ϵ��Զ�����
2��@SIC,,GET,DATA.DEEP,OK,HF,123.45*CRC\r\n 
3��@SIC,,GET,DATA.DEEP,OK,LF,123.45*CRC\r\n
define Unmanned_ship

20190529 �����û�ʹ�õ�30Сʱ��ע��ʧ�ܡ�

2019-9-2	���Ӵ���2�������봮��1һ�£����ں���2

2019-9-11   ȥ���ֶ�������������


2019-09-23 �޸Ĳ�����ѹ 26.32x^3 -86.86x^2 +108.7x -36.75

2019-12-05 ����2�������4��ָ�����ҳ��pc�Ĳ���ͬ�������������޸���regtime.c�ļ���flash�������Ѵ��ڵ���������