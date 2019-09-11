
#include <hidef.h>      /* common defines and macros */
#include <MC9S12XS128.h>     /* derivative information */
#include <math.h>
#include "derivative.h"
#include "LCD.h"
#define uchar unsigned char
#define uint unsigned int

#define  servo_max        3390+450//460        // 200           4460+875
#define  servo_mid         3390

#define  servo_min         3390-450//480        // 200           4460-875
#define d_len        30//60 //20  //60   // 70  // ��ǰλ��ƫ�����d_len��ƫ��ƽ��������
#define d_amp       0.1//0.1//0.1 //4     //  5/2 03D item coef
#define SPEED_DANG_NUM 100


#define VV_MAX             250 	//1000 	�ٶ�PID���������ֵ
#define VV_MIN 	         -250     //-1000 	�ٶ�PID��������Сֵ
#define VV_DEADLINE  1	  //�ٶ�PID������������Χ

#define mkp   8.5//8.5// 6.46   //p���ٽ�ֵ��С�ڵ���9.23   60%~70%          
#define mki   0.183//0.183 //0.168//0.45//i��ֵ  С�ڵ���0.33   150%~180%	          
#define mkd   0.002//0.002//0.003/ 0.024  //d��ֵ    С��0.25            30%

#define mkp0   8.5//8.5// 6.46   //p���ٽ�ֵ��С�ڵ���9.23   60%~70%          
#define mki0   0.183//0.183//0.168//0.45//i��ֵ  С�ڵ���0.33   150%~180%	          
#define mkd0   0.002//0.002//0.003// 0.024  //d��ֵ    С��0.25            30%

float pp = 0;

#define cs         45             // 40����  Сת��ʱû�в���
#define cs1         80             // 80����  Сת��ʱû�в���
#define cc         1.5             // 1.5����ϵ������
#define cc1         0.8             //0.8����ϵ������

uint now_speed0;
uint now_speed;
int speed_ept0;
int speed_ept;
int a_m[7] = 0;  //Һ��������
int b_m[7] = 0;
int s_m[7] = 0;
int e_m[7] = 0;
int a_d[7] = 0;
int x;
int f;
int w;
int Pulse_count = 0;//���̷���ֵ
int Pulse_count0 = 0;
int ideal_speed;
int ideal_speed0;
int pre_error = 0;
int pre_error0 = 0;
int ppre_error = 0;
int ppre_error0 = 0;
int speed;
int speed0;
//int speed_buff[5]; 
uint speed_tab[SPEED_DANG_NUM] = { 0 };
int sadd = 0, sadd_cnt = 0;
uint LOW_SPEED = 0;
uint HIGH_SPEED = 0;
int car_driver = 0, car_driver0 = 0;
float ad_div = 0;
float pre_div;
unsigned int  juli = 0;
float juli1;
int wxstop;
int  position;
uint Servo_Dir, preServo_Dir;
int stop_flag = 0, stop_cnt = 0, slope_flag = 0, START_LINE = 0;
int slope_flag1 = 0, slope_flag2 = 0, slope_flag3 = 0;
int pcnt1 = 0, pcnt2 = 0;
int stop_finish;
uint ad_result[7] = { 0 }, ad_result10[6] = { 0 };
/***********************duojiPxiang*******************************/
//int P_seg1=90,P_seg2=210,P_seg3=290,P_seg4=350,P_max=450;
int P_seg1 = 90, P_seg2 = 155, P_seg3 = 205, P_seg4 = 290, P_max = 500;

void AD_ctrl(void);
void Steer(void);
int Cal_P_Item(void);
int Cal_D_Item(int length);
void road_Judgement(void);
void Speed_Control(void);
int abs(int x);
void PWM_init_motor0(void);
void PWM_init_motor1(void);
void PWM_rudder_init(void);
void steer_control(void);
void Dly_us(int jj);
void RD_TSL(void);
void dongtaiyuzhi(void);
void zhangai(void);
uint ADV0[128] = { 0 }, ADV_0[128] = { 0 };     //�������飬���ڴ�Ųɼ���������ֵ
uint CCD1 = 0, CCD2 = 0, pre_CCD1 = 0, pre_CCD2 = 0, total = 0;
uint slope_cnt = 0, zhang_cnt = 0, zai_cnt = 0, ceju = 0, ceju0 = 0;
volatile int s2w_flag = 0, sza_flag = 0, szal_flag = 0, za_flag, zai_flag = 0, lsza_flag = 0, straight_flag = 0, long_straight_flag = 0, ls2w_flag = 0, lls2w_flag = 0,
angleflag = 1, st_flag = 0, lst_flag = 0, po_flag = 0, zhang_flag, pozhi_flag = 0, poqi_flag = 0, zj_flag = 0, lzj_flag = 0;
uint start = 0, start_cnt = 0;
volatile int po_cnt = 0;
uint k_cnt;
int k = 0;
int l2 = 0, r2 = 0, lock = 0;
int pit_cnt = 0;
int pre_PTS_PTS4;
int pre_PTS_PTS2;
int tingche = 0;
uint time;
int kaiguan = 0, start_flag = 0;
int r2, l2;
int hongwai0, hongwai1, hongwai2;
int shizi1 = 0;
unsigned int time1 = 0;
unsigned int pulse;
int time2 = 0;
uint array_ad0[10], array_ad1[10], array_ad2[10], array_ad3[10], array_ad4[10], array_ad5[10];
int speed_buff[5];
void ATD_Process(void);
/**************************
////////���໷��ʼ��///////
****************************/
void SetBusCLK_40M()
{
	CLKSEL = 0x00;             //ʱ��ѡ��Ĵ��� ����ʱ��ģʽ
	PLLCTL_PLLON = 1;          //���໷��·����λ
	SYNR = 0x04;               //SYNR=4
	REFDV = 0x01;              //REFDV=1   PLLCLK=2*OSCCLK*(SYNR+1)/(REFDV+1)  OSCCLK=16M BusCLK=PLLCLK/2
	POSTDIV = 0x00;
	_asm(nop);                 //��ʱ����
	_asm(nop);                 //��ʱ����
	while (!(CRGFLG_LOCK == 1));  //���໷ʱ���ȶ�
	CLKSEL_PLLSEL = 1;            //���໷ʹ��
}
/**************************
////////�����ʼ��///////
****************************/
void PWM_rudder_init(void)
{
	PWME_PWME2 = 0;
	PWME_PWME3 = 0;
	PWMPRCLK_PCKB = 2;     //������ʱ�ӽ���Ԥ��Ƶ������ʱ��Ϊ40M��0000 0010 4��Ƶ  Clock A=10Mhz           
	PWMCLK_PCLK3 = 1;      //��SAΪ��ʱ��Դ    (0,1,4,5��ѡʱ��A��SA)
	PWMSCLB = 2;           //SA��Ƶ   Clock SA=Clock A/(2*PWMSCLA)
	PWMCTL_CON23 = 1;
	PWMPOL_PPOL3 = 1;      //�����ط�ת  ��ʼΪ�ߵ�ƽ
	PWMCAE_CAE3 = 0;       //��������                       
	PWMPER23 = 50000;      //���ΪƵ��Ϊ Clock SA/PWMPER01 �Ĳ�   
	PWMCNT23 = 0X00;       //ͨ����������0 
	PWME_PWME3 = 1;        //ͨ��ʹ�ܣ�1ͨ��Ϊ���ͨ�� 


}
/**************************
////////�����ʼ��///////
****************************/
void PWM_init_motor0(void)
{
	PWME_PWME0 = 0;
	PWMPRCLK_PCKA = 2;   //������ʱ�ӽ���Ԥ��Ƶ������ʱ��Ϊ40M��0000 0010 4��Ƶ  Clock A=10Mhz
	PWMCLK_PCLK0 = 1;    //��SBΪ��ʱ��Դ    (0,1,4,5��ѡʱ��A��SA)
	PWMSCLA = 2;         //SB��Ƶ   Clock SB=Clock B/(2*PWMSCLA)
	PWMPOL_PPOL0 = 1;    //�����ط�ת  ��ʼΪ�ߵ�ƽ
	PWMCAE_CAE0 = 0;     //��������                       
	PWMPER0 = 250;       //���ΪƵ��Ϊ Clock SB/PWMPER01 �Ĳ�
	PWMCNT0 = 0X00;      //ͨ����������0 
	PWME_PWME0 = 1;      //ͨ��ʹ�ܣ�1ͨ��Ϊ���ͨ��

	PWME_PWME1 = 0;
	PWMPRCLK_PCKA = 2;   //������ʱ�ӽ���Ԥ��Ƶ������ʱ��Ϊ40M��0000 0010 4��Ƶ  Clock A=10Mhz           
	PWMCLK_PCLK1 = 1;    //��SAΪ��ʱ��Դ    (0,1,4,5��ѡʱ��A��SA)
	PWMSCLA = 2;         //SA��Ƶ   Clock SA=Clock A/(2*PWMSCLA)
	PWMPOL_PPOL1 = 1;    //�����ط�ת  ��ʼΪ�ߵ�ƽ
	PWMCAE_CAE1 = 0;     //��������                        
	PWMPER1 = 250;       //���ΪƵ��Ϊ Clock SA/PWMPER01 �Ĳ�   
	PWMCNT1 = 0X00;      //ͨ����������0 
	PWME_PWME1 = 1;      //ͨ��ʹ�ܣ�1ͨ��Ϊ���ͨ�� 

}
void PWM_init_motor1(void)
{
	PWME_PWME4 = 0;
	PWMPRCLK_PCKA = 2;   //������ʱ�ӽ���Ԥ��Ƶ������ʱ��Ϊ40M��0000 0010 4��Ƶ  Clock A=10Mhz           
	PWMCLK_PCLK4 = 1;    //��SBΪ��ʱ��Դ    (0,1,4,5��ѡʱ��A��SA)
	PWMSCLA = 2;         //SB��Ƶ   Clock SB=Clock B/(2*PWMSCLA)
	PWMPOL_PPOL4 = 1;    //�����ط�ת  ��ʼΪ�ߵ�ƽ
	PWMCAE_CAE4 = 0;     //��������                       
	PWMPER4 = 250;       //���ΪƵ��Ϊ Clock SB/PWMPER01 �Ĳ�   
	PWMCNT4 = 0X00;      //ͨ����������0 
	PWME_PWME4 = 1;      //ͨ��ʹ�ܣ�1ͨ��Ϊ���ͨ��

	PWME_PWME5 = 0;
	PWMPRCLK_PCKA = 2;   //������ʱ�ӽ���Ԥ��Ƶ������ʱ��Ϊ40M��0000 0010 4��Ƶ  Clock A=10Mhz           
	PWMCLK_PCLK5 = 1;    //��SAΪ��ʱ��Դ    (0,1,4,5��ѡʱ��A��SA)
	PWMSCLA = 2;         //SA��Ƶ   Clock SA=Clock A/(2*PWMSCLA)
	PWMPOL_PPOL5 = 1;    //�����ط�ת  ��ʼΪ�ߵ�ƽ
	PWMCAE_CAE5 = 0;     //��������                        
	PWMPER5 = 250;       //���ΪƵ��Ϊ Clock SA/PWMPER01 �Ĳ�   
	PWMCNT5 = 0X00;      //ͨ����������0 
	PWME_PWME5 = 1;      //ͨ��ʹ�ܣ�1ͨ��Ϊ���ͨ�� 


}
void AD_Init(void)
{
	ATD0DIEN = 0x00;   //��ֹ��������
	ATD0CTL1 = 0x20;   //ѡ��ADͨ��Ϊ�ⲿ����,12λ����,����ǰ���ŵ�  
	ATD0CTL2 = 0x40;   //��־λ�Զ����㣬��ֹ�ⲿ����, ��ֹ�ж�     
	ATD0CTL3 = 0x80;   //�Ҷ����޷���,ÿ��ת��4������, No FIFO, Freezeģʽ�¼���ת     
	ATD0CTL4 = 0x00;   //����ʱ��Ϊ4��ADʱ������,PRS=9,ATDClock=40/(2*(9+1))2MHz  
	ATD0CTL5 = 0x30;   //����ͨ����ֹ,����ת��2��ͨ�� ,��ͨ��ת������ʼͨ��Ϊ0ת�� 

}



/**************************
////////���뿪��///////
****************************/
void boman()
{
	if (PTM_PTM1 == 1 && PTM_PTM2 == 1) {         //1
		HIGH_SPEED = 55;

	}
	if (PTM_PTM1 == 1 && PTM_PTM2 == 0) {          // 2
		HIGH_SPEED = 50;

	}
	if (PTM_PTM1 == 0 && PTM_PTM2 == 1) {          // 2
		HIGH_SPEED = 45;

	}

	if (PTM_PTM1 == 0 && PTM_PTM2 == 0) {          // 2
		HIGH_SPEED = 40;

	}


	if (PTM_PTM3 == 1 && PTM_PTM4 == 1) {

		LOW_SPEED = 35;

	}

	if (PTM_PTM3 == 1 && PTM_PTM4 == 0) {

		LOW_SPEED = 30;
	}

	if (PTM_PTM3 == 0 && PTM_PTM4 == 1){

		LOW_SPEED = 25;
	}
	if (PTM_PTM3 == 0 && PTM_PTM4 == 0){
		LOW_SPEED = 20;
	}

}

/**************************************************************************/
//**Function name:			filter0,filter1,filter2,filter3
//**Description:            ��·ADͨ��5���˲�
//**Input parameters:		get_value(��·ad����ֵ)
//**Output parameters:		array_ad0[],array_ad1[],array_ad2[],array_ad2[]
//**Returned value:			uint�͸�·�˲����
/**************************************************************************/
uint filter0(uint get_value)
{
	static int i = 0;
	uchar j, k;
	uint count, sum = 0, temp;
	array_ad0[i++] = get_value;
	if (i == 5)
		i = 0;
	for (j = 0; j < 5; j++)//�������5�����ݴ�С��������
	{
		for (k = 0; k<j; k++)
		{
			if (array_ad0[k]>array_ad0[k + 1])
			{
				temp = array_ad0[k];
				array_ad0[k] = array_ad0[k + 1];
				array_ad0[k + 1] = temp;
			}
		}
	}
	for (count = 1; count < 4; count++)//ȥ�����ֵ��Сֵ��ƽ��
		sum = sum + array_ad0[count];
	return (uint)(sum / 3);
}

uint filter1(uint get_value)
{
	static int i = 0;
	uchar j, k;
	uint count, sum = 0, temp;
	array_ad1[i++] = get_value;
	if (i == 5)
		i = 0;
	for (j = 0; j < 5; j++)//�������5�����ݴ�С��������
	{
		for (k = 0; k<j; k++)
		{
			if (array_ad1[k]>array_ad1[k + 1])
			{
				temp = array_ad1[k];
				array_ad1[k] = array_ad1[k + 1];
				array_ad1[k + 1] = temp;
			}
		}
	}
	for (count = 1; count < 4; count++)//ȥ�����ֵ��Сֵ��ƽ��
		sum = sum + array_ad1[count];
	return (uint)(sum / 3);
}

uint filter2(uint get_value)
{
	static int i = 0;
	uchar j, k;
	uint count, sum = 0, temp;
	array_ad2[i++] = get_value;
	if (i == 5)
		i = 0;
	for (j = 0; j < 5; j++)//�������5�����ݴ�С��������
	{
		for (k = 0; k<j; k++)
		{
			if (array_ad2[k]>array_ad2[k + 1])
			{
				temp = array_ad2[k];
				array_ad2[k] = array_ad2[k + 1];
				array_ad2[k + 1] = temp;
			}
		}
	}
	for (count = 1; count < 4; count++)//ȥ�����ֵ��Сֵ��ƽ��
		sum = sum + array_ad2[count];
	return (uint)(sum / 3);
}

uint filter3(uint get_value)
{
	static int i = 0;
	uchar j, k;
	uint count, sum = 0, temp;
	array_ad3[i++] = get_value;
	if (i == 5)
		i = 0;
	for (j = 0; j < 5; j++)//�������5�����ݴ�С��������
	{
		for (k = 0; k<j; k++)
		{
			if (array_ad3[k]>array_ad3[k + 1])
			{
				temp = array_ad3[k];
				array_ad3[k] = array_ad3[k + 1];
				array_ad3[k + 1] = temp;
			}
		}
	}
	for (count = 1; count < 4; count++)//ȥ�����ֵ��Сֵ��ƽ��
		sum = sum + array_ad3[count];
	return (uint)(sum / 3);
}

uint filter4(uint get_value)
{
	static int i = 0;
	uchar j, k;
	uint count, sum = 0, temp;
	array_ad4[i++] = get_value;
	if (i == 5)
		i = 0;
	for (j = 0; j < 5; j++)//�������5�����ݴ�С��������
	{
		for (k = 0; k<j; k++)
		{
			if (array_ad4[k]>array_ad4[k + 1])
			{
				temp = array_ad4[k];
				array_ad4[k] = array_ad4[k + 1];
				array_ad4[k + 1] = temp;
			}
		}
	}
	for (count = 1; count < 4; count++)//ȥ�����ֵ��Сֵ��ƽ��
		sum = sum + array_ad4[count];
	return (uint)(sum / 3);
}


uint filter5(uint get_value)
{
	static int i = 0;
	uchar j, k;
	uint count, sum = 0, temp;
	array_ad5[i++] = get_value;
	if (i == 5)
		i = 0;
	for (j = 0; j < 5; j++)//�������5�����ݴ�С��������
	{
		for (k = 0; k<j; k++)
		{
			if (array_ad5[k]>array_ad5[k + 1])
			{
				temp = array_ad5[k];
				array_ad5[k] = array_ad5[k + 1];
				array_ad5[k + 1] = temp;
			}
		}
	}
	for (count = 1; count < 4; count++)//ȥ�����ֵ��Сֵ��ƽ��
		sum = sum + array_ad5[count];
	return (uint)(sum / 3);
}

int  Speed_Filter(int get_speed)
{
	static int j = 0;
	int count;
	int  sum = 0;
	speed_buff[j++] = get_speed;
	if (j == 5)	j = 0;
	for (count = 0; count < 5; count++)
		sum = sum + speed_buff[count];
	return (sum / 5);

}

/*********************************************************************************************************
** Function name:           ATD_Process
** Descriptions:            5·AD�ź�3�ε����˲�
** input parameters:        filterx��int x��
** Output parameters:       atd[x]
** Returned value:          NONE
*********************************************************************************************************/
void ATD_Process(void)
{
	while (!ATD0STAT0_SCF);  //�ȴ�ת�����
	ad_result[0] = filter0(ATD0DR0);
	//ad_result[1]=filter1(ATD0DR1);

	/////ATD0DR2����ʹ��/////
	ad_result[2] = filter1(ATD0DR1);
	ad_result[3] = filter2(ATD0DR2);
	ad_result[4] = ATD0DR3;
	// ad_result[5]=ATD0DR2;   //zuo
	//ad_result[6]=ATD0DR3;
	//you

}

//*************************************************************************/
//**Function name:		  AD_Process1
//**Description:          ����һ�µ�ԴAD���ݴ���(),ȷ��ֵ���,�δ󴫸���,�������ֵ           
//**Input parameters:	  NONE
//**Output parameters:	  NONE
//**Returned value:		  NONE
//*************************************************************************/
void steer_control(void)
{

	////////****************�㷨����*************////////////////
	int i = 0, j = 0, l1 = -1, r1 = -1;

	int shizi = 0;
	for (i = 63; i > 0; i--)
	{
		l1 = l1 + 1;
		l2 = l1;
		if (ADV_0[i - 1] == 0 && ADV_0[i - 2] == 0 && ADV_0[i - 3] == 0 && ADV_0[i - 4] == 0) break;
	}

	for (j = 64; j < 127; j++)
	{
		r1 = r1 + 1;
		r2 = r1;
		if (ADV_0[j + 1] == 0 && ADV_0[j + 2] == 0 && ADV_0[j + 3] == 0 && ADV_0[j + 4] == 0) break;
	}

	ad_div = ((float)r1 - (float)l1) / 50;
	ad_div = ad_div * 500;
	ad_div = ad_div + (ad_div - pre_div) * 1 / 30;
	ad_div = ad_div * 4 / 5;

	if (r1 > 55 || l1 > 55 || lock == 1)  // ||shizi==1
	{
		if ((r1 - l1)<-45)
			ad_div = -450;

		if ((r1 - l1)>45)

			ad_div = 450;
		lock = 1;
	}


	if (r1 > 45 && l1 > 45)
	{
		shizi = 1;

	}

	if (shizi)
	{
		ad_div = pre_div;
	}
	if (r1 + l1<25)
		ad_div = pre_div;

	if (r1<38 && r1>25 && l1>25 && l1<38 && CCD2>85)
	{
		lock = 0;
		shizi = 0;
	}

}
void AD_ctrl(void)
{
	uint angle, pre_angle;

	steer_control();
	pre_div = ad_div;
	if (lock == 1)
		ad_div = pre_div;
	if ((ad_div - pre_div) > 300 || (ad_div - pre_div) < -300)
		angle = pre_angle;

	position = (int)ad_div;

	Steer();

	// angle=servo_mid+ad_div;     //  �Ȳ�����
	angle = Servo_Dir;

	if (angle >= servo_max)
	{
		angle = servo_max;
	}
	if (angle < servo_min)
	{
		angle = servo_min;
	}
	pre_angle = angle;


	PWMDTY23 = angle;


}

void Steer(void)              /* ǰ�ֶ��ת�� */
{
	uint Dir_Tmp;
	int P, D;
	uint pre_dir;
	// ppre_dir=pre_dir; 

	pre_dir = Servo_Dir;                // (int16)

	P = Cal_P_Item();

	D = Cal_D_Item(d_len);

	Dir_Tmp = servo_mid + P + D*d_amp;         //d�� ,d_amp�ɵ�P++D*d_amp 


	if (Dir_Tmp<servo_min)
	{
		Dir_Tmp = servo_min;
	}
	else if (Dir_Tmp>servo_max)
	{
		Dir_Tmp = servo_max;
	}

	Servo_Dir = (int)Dir_Tmp;

}

int Cal_P_Item(void)
{
	int P_part;

	P_part = abs(position);

	if (position > 0)               //���
	{
		if (P_part < 20)
		{
			return 0;
		}
		else if (P_part < P_seg1)
		{
			return -(P_part * 1 / 6);
		}
		else if (P_part < P_seg2)
		{
			return -(P_seg1 * 1 / 6 + (P_part - P_seg1) * 1 / 3);
		}
		else if (P_part < P_seg3)
		{
			return -(P_seg1 * 1 / 6 + (P_seg2 - P_seg1) * 1 / 3 + (P_part - P_seg2) * 2 / 5);
		}
		else if (P_part < P_seg4)
		{
			return -(P_seg1 * 1 / 6 + (P_seg2 - P_seg1) * 1 / 3 + (P_seg3 - P_seg2) * 2 / 5 + (P_part - P_seg3) * 7 / 4);
		}
		else                     //���������Χ��P ���
		{
			if ((P_seg1 * 1 / 6 + (P_seg2 - P_seg1) * 1 / 3 + (P_seg3 - P_seg2) * 2 / 5 + (P_part - P_seg3) * 7 / 4) < P_max)
			{
				return -(P_seg1 * 1 / 6 + (P_seg2 - P_seg1) * 1 / 3 + (P_seg3 - P_seg2) * 2 / 5 + (P_part - P_seg3) * 7 / 4);
			}
			else
			{
				return -P_max;
			}

		}
	}
	else // �Ҳ�
	{
		if (P_part < 20)
		{
			return 0;
		}
		if (P_part < P_seg1)
		{
			return (P_part * 1 / 6);
		}
		else if (P_part < P_seg2)
		{
			return (P_seg1 * 1 / 6 + (P_part - P_seg1) * 1 / 3);
		}
		else if (P_part < P_seg3)
		{
			return (P_seg1 * 1 / 6 + (P_seg2 - P_seg1) * 1 / 3 + (P_part - P_seg2) * 2 / 5);
		}
		else if (P_part < P_seg4)
		{
			return (P_seg1 * 1 / 6 + (P_seg2 - P_seg1) * 1 / 3 + (P_seg3 - P_seg2) * 2 / 5 + (P_part - P_seg3) * 7 / 4);
		}
		else //���������Χ��P ���
		{
			if ((P_seg1 * 1 / 6 + (P_seg2 - P_seg1) * 1 / 3 + (P_seg3 - P_seg2) * 2 / 5 + (P_part - P_seg3) * 7 / 4) < P_max)
			{
				return (P_seg1 * 1 / 6 + (P_seg2 - P_seg1) * 1 / 3 + (P_seg3 - P_seg2) * 2 / 5 + (P_part - P_seg3) * 7 / 4);
			}
			else
			{
				return P_max;
			}
		}
	}
}
int Cal_D_Item(int length)       /* ǰ�ֶ��D����� */
{


	int Dcal_D, err_1, err_2, q_l_1, q_l_2, q_l_3;
	static int Z1[102], queue_index = 0;                     /* ��ʷ���� */
	//ѭ������
	Z1[queue_index] = position * 4 / 5;                           //Z1[queue_index] = position*3/4;

	queue_index = (queue_index + 1) % 102;

	q_l_1 = queue_index - 1;

	if (q_l_1 < 0)
	{
		q_l_1 = 102 + q_l_1;
	}

	q_l_2 = queue_index - 2;
	if (q_l_2 < 0)
	{
		q_l_2 = 102 + q_l_2;
	}

	q_l_3 = queue_index - 3;
	if (q_l_3 < 0)
	{
		q_l_3 = 102 + q_l_3;
	}
	err_1 = (Z1[q_l_1] + Z1[q_l_2] + Z1[q_l_3]) / 3;

	q_l_1 = queue_index - 1 - length;
	if (q_l_1 < 0)
	{
		q_l_1 = 102 + q_l_1;
	}

	q_l_2 = queue_index - 2 - length;
	if (q_l_2 < 0)
	{
		q_l_2 = 102 + q_l_2;
	}

	q_l_3 = queue_index - 3 - length;
	if (q_l_3 < 0)
	{
		q_l_3 = 102 + q_l_3;
	}

	err_2 = (Z1[q_l_1] + Z1[q_l_2] + Z1[q_l_3]) / 3;
	Dcal_D = err_1 - err_2;

	return Dcal_D;
}

int abs(int x)
{
	if (x < 0)
		return -x;
	else
		return x;
}

void Init_speed_tab(void)
{
	int i;
	for (i = 0; i < SPEED_DANG_NUM; i++)
	{ /********�ϻ�**************/
		//     speed_tab[i] =HIGH_SPEED-(int)((long)(i-SPEED_DANG_NUM+1)*(long)(i-SPEED_DANG_NUM+1)*(long)(HIGH_SPEED-LOW_SPEED)/
		//               (long)(SPEED_DANG_NUM-1)/(long)(SPEED_DANG_NUM-1));
		speed_tab[i] = HIGH_SPEED - (int)((long)i*(long)i*(long)(HIGH_SPEED - LOW_SPEED) /
			(long)(SPEED_DANG_NUM - 1) / (long)(SPEED_DANG_NUM - 1));
		/********�»�**************/
		//speed_tab[i] =LOW_SPEED+(int)((long)(i-SPEED_DANG_NUM+1)*(long)(i-SPEED_DANG_NUM+1)*(long)(HIGH_SPEED-LOW_SPEED)/
		//(long)(SPEED_DANG_NUM-1)/(long)(SPEED_DANG_NUM-1));
	}
}

void MOTOR_forward(int mt2, int mt3)
{
	int s;
	s = mt2 - mt3;

	if (s > -2 && s < 2)
	{
		if (mt2 >= 0)
		{
			PWMDTY0 = 0;
			PWMDTY1 = mt2;                          //  MCF_PWM_PWMDTY0=(uint8)(mt2);                                                                               //  MCF_PWM_PWMDTY1=0;

			PWMDTY5 = 0;
			PWMDTY4 = mt2;
		}
		else
		{
			mt2 = -mt2;
			PWMDTY0 = mt2;
			PWMDTY1 = 0;
			PWMDTY5 = mt2;
			PWMDTY4 = 0;                             // MCF_PWM_PWMDTY1=(uint8)(mt2);                                                              // MCF_PWM_PWMDTY0=0;       
		}
	}
	else
	{
		if (mt2 >= 0)
		{
			PWMDTY0 = 0;
			PWMDTY1 = mt2;                          //  MCF_PWM_PWMDTY0=(uint8)(mt2);                                                                               //  MCF_PWM_PWMDTY1=0;
		}
		else
		{
			mt2 = -mt2;
			PWMDTY0 = mt2;
			PWMDTY1 = 0;                       // MCF_PWM_PWMDTY1=(uint8)(mt2);                                                              // MCF_PWM_PWMDTY0=0;       
		}

		if (mt3 >= 0)
		{
			PWMDTY5 = 0;
			PWMDTY4 = mt3;                             //  MCF_PWM_PWMDTY0=(uint8)(mt2);                                                                            //  MCF_PWM_PWMDTY1=0;
		}
		else
		{
			mt3 = -mt3;
			PWMDTY5 = mt3;
			PWMDTY4 = 0;                        // MCF_PWM_PWMDTY1=(uint8)(mt2);                                                             // MCF_PWM_PWMDTY0=0;       
		}

	}
}


void MOTORforward_Control(void)
{

	if ((ideal_speed - Pulse_count) >= 15 || (Pulse_count - ideal_speed) >= 15)          //bang_bang
	{
		if (ideal_speed > Pulse_count)
		{
			car_driver = 250;
		}
		else
		{
			car_driver = -250;
		}
	}
	else
	{
		car_driver = motor_ctrl2(ideal_speed, Pulse_count);                                                                //   V_PIDCalc(&sPID)  ; 
	}
	if ((ideal_speed0 - Pulse_count0) >= 15 || (Pulse_count0 - ideal_speed0) >= 15)          //bang_bang
	{
		if (ideal_speed0 > Pulse_count0)
		{
			car_driver0 = 250;
		}
		else
		{
			car_driver0 = -250;
		}
	}
	else
	{
		car_driver0 = motor_ctrl20(ideal_speed0, Pulse_count0);                                                                //   V_PIDCalc(&sPID)  ; 
	}

	MOTOR_forward(car_driver, car_driver0);

}



/*************************�ٶȿ���*****************/

int motor_ctrl2(int ideal_speed, uint Pulse_count)
{
	int error;
	int d_error;
	int dd_error;

	error = ideal_speed - Pulse_count;	          //ƫ�����(����)   �ٶȵ�ƫ��		
	d_error = error - pre_error;                  //ƫ�����(����)  �ٶȵ�ƫ����
	dd_error = d_error - ppre_error;              //ƫ�����(΢��)   

	pre_error = error;		                      //�洢��ǰƫ��
	ppre_error = d_error;

	if (Pulse_count <= 5)                          //PID��������    �����⳵�ڼ�ɲ��ʱ���ֵ�ת�Ĺؼ�����
	{
		speed = 250;
		pre_error = 0;
		ppre_error = 0;
	}

	if ((error < VV_DEADLINE) && (error > -VV_DEADLINE))
	{
		;            //do nothing
	}

	else
	{
		speed += (int)(mkp*d_error + mki*error + mkd*dd_error); //�ٶ�PID������ʽ 

	}

	if (speed >= VV_MAX) 		  //�ٶ�PID����ֹ����������
	{
		speed = VV_MAX;
		//	 car_driver=VV_MAX;
	}
	else if (speed <= VV_MIN)	//�ٶ�PID����ֹ���������� 
	{
		speed = VV_MIN;
		//  	  car_driver= VV_MIN; 
	}

	return (speed);


}


int motor_ctrl20(int ideal_speed0, uint Pulse_count0)
{
	int error;
	int d_error;
	int dd_error;

	error = ideal_speed0 - Pulse_count0;	    //ƫ�����(����)   �ٶȵ�ƫ��		
	d_error = error - pre_error0;               //ƫ�����(����)  �ٶȵ�ƫ����
	dd_error = d_error - ppre_error0;           //ƫ�����(΢��)   

	pre_error0 = error;		                    //�洢��ǰƫ��
	ppre_error0 = d_error;

	if (Pulse_count0 <= 5)                      //PID��������    �����⳵�ڼ�ɲ��ʱ���ֵ�ת�Ĺؼ�����
	{
		speed0 = 250;
		pre_error0 = 0;
		ppre_error0 = 0;
	}

	if ((error < VV_DEADLINE) && (error > -VV_DEADLINE))
	{
		;            //do nothing
	}

	else
	{
		speed0 += (int)(mkp0*d_error + mki0*error + mkd0*dd_error); //�ٶ�PID������ʽ 

	}

	if (speed0 >= VV_MAX) 		  //�ٶ�PID����ֹ����������
	{
		speed0 = VV_MAX;
		//	 car_driver=VV_MAX;
	}
	else if (speed0 <= VV_MIN)	  //�ٶ�PID����ֹ���������� 
	{
		speed0 = VV_MIN;
		//  	  car_driver= VV_MIN; 
	}

	return (speed0);

}


void road_Judgement(void)
{
	static int sza_cnt = 0, lsza_cnt = 0, straight_cnt = 0, s2w_cnt = 0, st_cnt = 0, lst_cnt = 0, ls2w_cnt = 0, lsb_cnt = 0, zj_cnt0 = 0, lzj_cnt = 0, po_cnt1 = 0, slope_cnt, sz_cnt = 0;
	static int long_straight_cnt = 0, zj_cnt = 0, pozhi_cnt = 0, za_cnt = 0, lls2w_cnt = 0;

	/***************�ϰ��жϣ������ֱ���жϣ�����һ��ֱ��ʱ���ж��ϰ�**************/
	//if(abs(position)>=80)StopFlag = 1;
	if (abs(position) <= 200)
	{
		if (zai_cnt < 40)      //800
			zai_cnt++;
		if (zai_cnt >= 40)
		{
			zai_flag = 1;
			zai_cnt = 0;
		}
	}
	else
	{
		zai_flag = 0;
		zai_cnt = 0;
	}
	/***************�µ��жϣ�ͬ��**************/
	if (abs(position) <= 170)
	{
		if (za_cnt < 25)      //800
			za_cnt++;
		if (za_cnt >= 25)
		{
			za_flag = 1;
			za_cnt = 0;
		}
	}
	else
	{
		za_flag = 0;
		za_cnt = 0;
	}



	/***************��ֱ�� **************/

	if (abs(position) <= 150)
	{
		if (straight_cnt < 50)      //800
			straight_cnt++;
		if (straight_cnt >= 50)
		{
			straight_flag = 1;
			straight_cnt = 0;

		}
	}
	else
	{
		straight_flag = 0;
		straight_cnt = 0;

	}


	if (straight_flag && abs(position) > 200)
	{
		straight_flag = 0;
		s2w_flag = 1;
		//StopFlag = 1;
	}


	if (s2w_flag)
	{
		s2w_cnt++;
		if (s2w_cnt >= 10)              //2000
		{
			s2w_cnt = 0;
			s2w_flag = 0;

		}
	}

	/***************��ֱ��**************/


	if (abs(position) <= 200)
	{
		if (long_straight_cnt < 150)
			long_straight_cnt++;

		if (long_straight_cnt >= 150)                  // 2000
		{
			long_straight_flag = 1;
			long_straight_cnt = 0;

		}
	}
	else
	{
		long_straight_flag = 0;
		long_straight_cnt = 0;
	}


	if (long_straight_flag && abs(position) > 180)                                      //180
	{
		long_straight_flag = 0;
		ls2w_flag = 1;
		// wxstop=1; 
	}


	if (ls2w_flag)
	{
		ls2w_cnt++;
		if (ls2w_cnt >= 8)    //     180  180
		{
			ls2w_cnt = 0;
			ls2w_flag = 0;

		}
	}


	if (r2 > 45 && l2 > 45)
	{
		shizi1 = 1;
		sz_cnt = 0;
	}
	if (shizi1)
	{
		sz_cnt++;
		if (sz_cnt >= 4)
			shizi1 = 0;
	}



	/***************��⵽�µ��������ϰ�**************/
	if (PTM_PTM6 == 1)
	{

		if (l2 > 35 && l2<45 && r2>35 && r2 > 45 && za_flag)

		{
			slope_flag = 1;
			PORTB_PB0 = 0;                                   //wxstop=1;
		}


		if (slope_flag)
		{

			PWMDTY23 = servo_mid;
			pozhi_flag = 1;
			poqi_flag = 1;
			wxstop == 0;
			slope_cnt++;
			if (slope_cnt >= 30)    //     180  180
			{
				slope_cnt = 0;
				slope_flag = 0;
			}
		}
		if (pozhi_flag)  //pozhi_flag
		{
			PWMDTY23 = servo_mid;
			zai_cnt = 0;
			pozhi_cnt++;
			wxstop == 0;
			if (pozhi_cnt > 40)
				pozhi_cnt = 40;
			if (pozhi_cnt > 40 && l2<38 && r2<38)
			{
				pozhi_cnt = 0;
				pozhi_flag = 0;
				PORTB_PB0 = 1;
			}
		}

		/******�µ���־������*******/
		if (l2>35 && l2<45 && r2>35 && r2>45 && za_flag)

		{
			slope_flag1 = 1;
		}

		else
			slope_flag1 = 0;
	}

}




void Speed_Control(void)
{

	int sp_dang;

	// dir =  absabs(position)/4;

	sp_dang = abs(position) / 5;        //12 ϵ�������     

	if (sp_dang >= SPEED_DANG_NUM)
	{
		sp_dang = SPEED_DANG_NUM - 1;
	}

	speed_ept = speed_tab[sp_dang];
	speed_ept0 = speed_ept;//speed_tab[sp_dang];	



	if (sp_dang >= cs1) //cs,cs1,���ٷֶΣ�����ת���������
	{
		if (Servo_Dir > servo_mid)
		{
			speed_ept0 = speed_ept0;
			speed_ept = (int)(speed_ept*(500.0 + (float)(cs1 - cs) + (float)(sp_dang - cs1) / cc1) / 500); //����   5    
		}
		if (Servo_Dir<servo_mid)
		{
			speed_ept = speed_ept;
			speed_ept0 = (int)(speed_ept0*(500.0 + (float)(cs1 - cs) + (float)(sp_dang - cs1) / cc1) / 500);  	    //5     
		}
	}

	else  if (sp_dang >= cs)
	{
		if (Servo_Dir>servo_mid)
		{
			speed_ept0 = speed_ept0;
			speed_ept = (int)(speed_ept*(500.0 + (float)(sp_dang - cs) / cc) / 500);  	   //      3        +8  

		}
		if (Servo_Dir < servo_mid)
		{
			speed_ept = speed_ept;
			speed_ept0 = (int)(speed_ept0*(500.0 + (float)(sp_dang - cs) / cc) / 500); 	   //     3
		}

	}


	if (abs(Servo_Dir - preServo_Dir)<80)
	{
		sadd = 1;

		if (abs(Servo_Dir - servo_mid)>200 && abs(Servo_Dir - servo_mid) <= 500 && sadd == 1)
		{
			sadd_cnt++;
		}
		else
		{
			sadd = 0;
			sadd_cnt = 0;
		}


		if (sadd_cnt > 13)      //ֱ����������ٺ�ļ���
		{

			speed_ept = LOW_SPEED + ((sadd_cnt - 13) / 0.9)*((sadd_cnt - 13) / 0.9);          //+9+((500-(Servo_Dir-servo_mid))/20) 	
			speed_ept0 = speed_ept;

			if (Servo_Dir > servo_mid&&sp_dang > cs)
			{
				speed_ept0 = speed_ept0;
				speed_ept = (int)(speed_ept*(500.0 + (float)(sp_dang - cs)) / 500 + 10);

			}
			if (Servo_Dir<servo_mid&&sp_dang>cs)
			{
				speed_ept = speed_ept;
				speed_ept0 = (int)(speed_ept0*(500.0 + (float)(sp_dang - cs)) / 500 + 10);
			}

			if (Servo_Dir > servo_mid&&sp_dang > cs1)
			{
				speed_ept0 = speed_ept0;
				speed_ept = (int)(speed_ept*(500.0 + (float)(cs1 - cs) + (float)(sp_dang - cs1) / cc1) / 500 + 14);
			}
			if (Servo_Dir<servo_mid&&sp_dang>cs1)
			{
				speed_ept = speed_ept;
				speed_ept0 = (int)(speed_ept0*(500.0 + (float)(cs1 - cs) + (float)(sp_dang - cs1) / cc1) / 500 + 14);
			}

		}
		if (sadd_cnt > 16)
		{
			sadd_cnt = 16;
		}


		if (abs(Servo_Dir - servo_mid) >= 450 && sadd_cnt > 7)//����ʱ�Ĳ���
		{
			if (Servo_Dir > servo_mid)
			{
				speed_ept0 = speed_ept0;
				speed_ept = 1.2*speed_ept0;
			}
			if (Servo_Dir<servo_mid)
			{
				speed_ept = speed_ept;
				speed_ept0 = (uint)(1.2*speed_ept);
			}

		}
		if (speed_ept>95)
			speed_ept = 95;
		if (speed_ept0 > 95)
			speed_ept0 = 95;

	}
	else
	{
		sadd_cnt = 0;
		sadd = 0;
	}


	preServo_Dir = Servo_Dir;



	if (straight_flag)
	{
		speed_ept = HIGH_SPEED + 5;
		speed_ept0 = speed_ept;

	}



	if (long_straight_flag)
	{
		speed_ept = HIGH_SPEED + 10;    //+HIGH_SPEED/10; 	
		speed_ept0 = speed_ept;

	}


	if (s2w_flag)//��ֱ������       
	{
		speed_ept = now_speed - 15;//0; now_speed-5
		speed_ept0 = now_speed - 15;
		if (Servo_Dir > servo_mid)
		{
			speed_ept0 = now_speed0 - 10;       //8
			speed_ept = now_speed - 2;

		}
		if (Servo_Dir < servo_mid)
		{
			speed_ept = now_speed - 10;
			speed_ept0 = now_speed0 - 2;

			//8

		}
		if (speed_ept < 25)
			speed_ept = 25;
		if (speed_ept0 < 25)
			speed_ept0 = 25;

	}


	if (ls2w_flag) //��ֱ������
	{
		speed_ept = now_speed - 20;        //3
		speed_ept0 = now_speed0 - 20;      //3


		if (sp_dang <= cs1)
		{
			if (Servo_Dir > servo_mid)
			{
				speed_ept0 = speed_ept0 - 13;        //13
				speed_ept = (int)(speed_ept*(500.0 + (float)(cs1 - cs)) / 500 - 5); 	 //10        
			}
			if (Servo_Dir<servo_mid)
			{
				speed_ept = speed_ept - 13;          //13
				speed_ept0 = (int)(speed_ept0*(500.0 + (float)(cs1 - cs)) / 500 - 5);  //10	         
			}
		}
		else
		{
			if (Servo_Dir>servo_mid)
			{                                              //13
				speed_ept0 = speed_ept0 - 13;
				speed_ept = (int)(speed_ept*(500.0 + (float)(cs1 - cs) + (float)(sp_dang - cs1) / cc1) / 500 - 5);
			}
			if (Servo_Dir < servo_mid)
			{
				speed_ept = speed_ept - 13;                     //13
				speed_ept0 = (int)(speed_ept0*(500.0 + (float)(cs1 - cs) + (float)(sp_dang - cs1) / cc1) / 500 - 5);
			}
		}

		if (speed_ept < 10)
			speed_ept = 10;
		if (speed_ept0 < 10)
			speed_ept0 = 10;

		//   wxstop=1;
	}

	/*  if(shizi1)
	  {
	  speed_ept =speed_ept-15;                     //13
	  speed_ept0 =speed_ept;
	  if(now_speed<45)
	  {
	  speed_ept=45;
	  speed_ept0=45;
	  }
	  }
	  */
	if (slope_flag1) //�µ���־
	{

		speed_ept = now_speed - 15;
		speed_ept0 = speed_ept;
		if (now_speed < 45)
		{
			//  if( speed_ept<45)
			speed_ept = 45;
			// if( speed_ept0<45)
			speed_ept0 = 45;
		}



	}


	ideal_speed = speed_ept;
	ideal_speed0 = speed_ept0;
	MOTORforward_Control();

}


/**************************
////////�����ۼ�����ʼ��///
****************************/

void PA_Init(void)
{

	PACTL_PAEN = 0;                 // PACTL = 0x40; 
	PACNT = 0x00;
	PACTL_PAMOD = 0;
	PACTL_PEDGE = 0;
	PACTL_PAEN = 1;
}

/**************************
////////PIT��ʼ��///
****************************/
void initPIT(void)//��ʱ�жϳ�ʼ������20ms ��ʱ�ж�����
{

	PITCFLMT_PITE = 0;                      /* ��ֹ PIT                   */
	PITCE_PCE0 = 1;                         /* ʹ��ͨ�� 0                 */
	PITMUX = 0X00;                          /* ch0 ���ӵ�΢������         */
	//��ʱ����=(PITMTLD0+1)*(PITLD0+1)/40Mhz=1ms
	PITMTLD0 = 200 - 1;                     /* ʱ�ӻ���40M  �üĴ�����������PITģ���е�8λ��������ֵ����ʵ��24λ�ļ������趨ֵΪ0��255��Χ              */
	PITLD0 = 200 - 1;                       /* ���������ֵ���趨         */
	PITINTE_PINTE0 = 1;                     /* ʹ��ͨ��0�ж�              */
	PITCFLMT_PITE = 1;                      /* ʹ�� PIT                   */
}
/**************************
////////SPI��ʼ��///
****************************/
void SPI0_Init()
{

	SPI0BR = 0x77; //����SPI�����ʣ���Ƶϵ��Ϊ2048
	SPI0CR1 = 0x;  //����SPI������ʽ������ģʽ���������жϣ�ʱ�ӿ��е͵�ƽ

}

/**************************
////////SPI0�������ݳ�ʼ��///
****************************/
void SPI0_Receive_Data()
{
	while (!SPI0SR_SPIF)
	{
		;
	}
}

/**************************
////// ��ʱ������cnt*1ms��////////////
****************************/
void delay(uint cnt) {
	uint loop_i, loop_j;
	for (loop_i = 0; loop_i < cnt; loop_i++) {
		loop_j = 0x1300;
		while (loop_j--);
	}
}

void zhangai(void)			//�ϰ���⴦��
{
	uint d = 0;
	if (!poqi_flag)
	{
		if (zai_flag && ((r2<15 && l2>25 && l2 < 35) || (l2<15 && r2>25 && r2 < 35)))//&&ad_result[7]<1400
		{

			if (r2<15)
			{
				for (d = 2000; d>0; d--)
				{

					//3750
					PWMDTY23 = servo_mid + 350;

				}
				for (d = 2000; d > 0; d--)
				{


					PWMDTY23 = servo_mid;
				}
			}
			if (l2<15)

			{

				for (d = 2000; d>0; d--)
				{

					//3750
					PWMDTY23 = servo_mid - 350;
				}
				for (d = 2000; d > 0; d--)
				{

					;              //3750
					PWMDTY23 = servo_mid;

				}
			}

		}

	}

	/*     if(!poqi_flag)
		 {
		 if(zai_flag&&((r2<15&&l2>25&&l2<35)||(l2<15&&r2>25&&r2<35)))
		 {

		 if(r2<15)
		 {
		 for(d=2000;d>0;d--)
		 {


		 PWMDTY23=servo_mid+450;
		 }
		 for(d=2000;d>0;d--)
		 {

		 //PORTB_PB0=1;
		 PWMDTY23=servo_mid;
		 }
		 }
		 if(l2<15)
		 {

		 for(d=2000;d>0;d--)
		 {


		 PWMDTY23=servo_mid-450;
		 }
		 for(d=2000;d>0;d--)
		 {


		 PWMDTY23=servo_mid;

		 }
		 }

		 }

		 }*/
}


/**************************
////// ������////////////
****************************/

void main()
{

	DisableInterrupts;

	SetBusCLK_40M();

	// PTTRR=0x30;
	PWM_rudder_init();
	PWM_init_motor0();
	PWM_init_motor1();
	AD_Init();



	initPIT();
	PA_Init();
	///////////////IO��////////////////////// 
	PORTB = 0xff;
	DDRB = 0x01;
	PORTA = 0xff;
	DDRA = 0xff;
	PTH = 0xff;
	DDRH = 0x00;//H������Ϊ����
	DDRM = 0x00;
	DDRS = 0X00;//S������Ϊ��������
	PORTE = 0xff;
	DDRE = 0xff;
	PORTK = 0xef;
	DDRK = 0xff;
	PUCR = 0X10;//ѡ��PWM��Ӧ�� 

	IRQCR = 0X00;
	LCD_init(); //��ʼ��LCDģ��
	PWMDTY23 = servo_mid;
	delay(1000);

	boman();
	Init_speed_tab();
	SPI0_Init();
	EnableInterrupts;

	for (;;)
	{
		ATD_Process();




		// RD_TSL();//��ȡ���Դ���������ֵ
		// dongtaiyuzhi();             

		// zhangai(); 

		//AD_ctrl();
		if (time2 > 3 && time2 < 7)
		{
			PORTK_PK4 = 1;
			PORTB_PB0 = 0;
		}
		else
		{
			PORTK_PK4 = 0;
			PORTB_PB0 = 1;
		}
		time2 = time1*0.001;

		// zhangai();
		//SPI0_Receive_Data();
		//ad_div=SPI0DR;         //��Ȧ��ֵ
		//AD_ctrl();          //����������������


		start_flag = 1;
		if (start_flag)
		{


			if (time > 6000)
			{

				if ((PORTB_PB1 == 0 && PORTB_PB6 == 0) || (PORTB_PB2 == 0 && PORTB_PB6 == 0) || (PORTB_PB2 == 0 && PORTB_PB5 == 0) || (PORTB_PB3 == 0 && PORTB_PB6 == 0) || (PORTB_PB4 == 0 && PORTB_PB2 == 0) || (PORTB_PB4 == 0 && PORTB_PB1 == 0))
				{
					wxstop = 1;
					//PORTB_PB0=0;
				}

			}



			if (wxstop == 1)
			{

				road_Judgement();
				if (now_speed > 10 && stop_finish == 0)
				{
					ideal_speed = 0;
					ideal_speed0 = 0;
					MOTORforward_Control();
				}
				else
				{
					stop_cnt++;

					PWMDTY0 = 0;
					PWMDTY1 = 0;
					PWMDTY4 = 0;
					PWMDTY5 = 0;
				}

				if (stop_cnt > 10)             //        4500
				{
					stop_finish = 1;
					stop_cnt = 0;
					//PORTB_PB0=0;
				}

			}
			else
			{
				//road_Judgement(); 

				if (start == 0)
				{
					PWMDTY0 = 0;
					PWMDTY1 = 40;
					PWMDTY4 = 40;
					PWMDTY5 = 0;
					start_cnt++;
					if (start_cnt > 60)//4000
					{
						start_cnt = 0;
						start = 1;
					}



				}
				else
				{
					// PORTB_PB0=0;           
					//  Speed_Control();
					/*       if(Servo_Dir>servo_mid+450)
						  {
						  PWMDTY0=0;
						  PWMDTY1=2.7*speed_ept;
						  }
						  if(Servo_Dir<servo_mid-450)
						  {
						  PWMDTY4=0;
						  PWMDTY5=2.7*speed_ept0;
						  }  */


					PWMDTY0 = 0;
					PWMDTY1 = 40;
					PWMDTY4 = 40;
					PWMDTY5 = 0;

				}


			}
		}

	}

}


/*********************************���****************************************/
/*********************************���****************************************/
void Dly_us(int jj)
{
	int ii;
	for (ii = 0; ii < jj; ii++) asm("nop"); //300 100   170 50us   120 20us  150 30   180 60   
}

void RD_TSL(void)
{

	uint i = 0, j = 0, k = 0, t = 0, tslp = 1;

	PORTA_PA5 = 1;//TSL_CLK=1;//��ʼ��ƽ��
	PORTA_PA6 = 0;//TSL_SI=0; //��ʼ��ƽ��


	Dly_us(50); //�������ʱ  //300    

	PORTA_PA5 = 0;//TSL_CLK=0;//�½���
	PORTA_PA6 = 1;//TSL_SI=1; //������


	Dly_us(50); //������ʱ

	PORTA_PA5 = 1;//TSL_CLK=1;//��ʼ��ƽ��
	PORTA_PA6 = 0;//TSL_SI=0; //��ʼ��ƽ��

	Dly_us(50); //������ʱ      
	for (i = 1; i < 128; i++)
	{
		PORTA_PA5 = 0;//TSL_CLK=0;//�½���

		Dly_us(300); //���ݻ������ߵ����ȣ�������ʱ
		while (!ATD0STAT2L_CCF0);
		ADV0[tslp] = ATD0DR0L;  //ADC0-12CH�ɼ�,AN0,12bit


		total = total + ADV0[tslp];

		++tslp;
		PORTA_PA5 = 1;//TSL_CLK=1;//������ 

		Dly_us(50); //���ݻ������ߵ����ȣ�������ʱ 
	}

}

void dongtaiyuzhi(void)
{
	uint cnt = 0, cnts = 0, f = 0;
	long int m = 0, q = 0, total_low = 0, total_high = 0;
	long int avfor = 0, avback = 0, g1 = 0, g2 = 0;    //u32  long int
	int max1 = 0, min1 = 0, g = 0;
	if (ad_div<300 && ad_div>-300)
	{

		max1 = ADV0[5];
		min1 = ADV0[5];

		for (g = 5; g < 120; g++)
		{

			if (max1<ADV0[g] && ADV0[g]<250)
				max1 = ADV0[g];
			if (min1>ADV0[g] && ADV0[g]>3)
				min1 = ADV0[g];

		}
		CCD1 = (2 * max1 + min1) * 2 / 7;// 4 5/18        2 2/7        2 1/4

	}

	else
	{
		CCD1 = pre_CCD1;

	}

	for (f = 1; f<127; f++)
	{
		if (ADV0[f]>CCD1)
			ADV_0[f] = 1;
		else
			ADV_0[f] = 0;

	}

	pre_CCD1 = CCD1;
	CCD2 = CCD1;

}

/*********************************���****************************************/
/*********************************���****************************************/


#pragma CODE_SEG __NEAR_SEG NON_BANKED
void interrupt 66 PIT0(void)           //pit�ж� ���̲���ٶ�ֵ
{

	DisableInterrupts;

	pit_cnt++;

	if (pit_cnt == 2)
	{
		PACTL_PAEN = 0;
		Pulse_count = PACNT;
		Pulse_count0 = PTH & 0xff;
		pulse = Pulse_count;
		if (pulse > 112)     //һ�״�Լ��11200�����壬����112��ʾ112���������1���ף�ÿ��112������juli��1����
		{
			juli++;
			PACNT = 0;

		}
		PORTE |= (0x01 << 7);
		Dly_us(1);
		// PACNT=0x00; 
		PORTE &= ~(0x01 << 7);

		pit_cnt = 0;

		//pcnt1 = Pulse_count;       //������

		//pcnt2+= Pulse_count;
		// now_speed= Pulse_count;
		//now_speed0= Pulse_count0;
		//PACNT=0;
		PACTL_PAEN = 1;
	}
	time++;
	if (time > 6000)  //��ʱ������������⵽ͣ����ͣ��
		time = 7000;     //��ֹʱ��������



	k_cnt++;


	if (k_cnt == 200)                  //�����жϳ����������ʱ�䣬�ж�����ʱ��Խ��Խ��
	{
		LCD_P14x16Str(0, 3, "ʱ��");
		LCD_write_shu(35, 4, time2, 4);
		LCD_P14x16Str(0, 24, "����");
		LCD_write_shu(35, 1, juli, 4);
		/*
				LCD_write_shu(1,0,juli,4);
				LCD_write_shu(40,0,time2,4);
				LCD_write_shu(1,1,hongwai0,4);
				LCD_write_shu(40,1,hongwai1,4);
				LCD_write_shu(1,2,l2,4);
				LCD_write_shu(40,2,r2,4);
				LCD_write_shu(1,3,Pulse_count,4);
				LCD_write_shu(40,3,Pulse_count0,4);
				LCD_write_shu(1,4,LOW_SPEED,4);
				LCD_write_shu(40,4, HIGH_SPEED ,4);
				LCD_write_shu(1,5,hongwai2,6);
				LCD_write_shu(40,5,ad_result[6],5);   */
		k_cnt = 0;

	}
	PITTF_PTF0 = 1;  //pit��ʱ��־�Ĵ�����PTF0��ʾ��ʱ��0��ͨ��д1����
	time1++;        //ʱ�䣬 time2=time1*1000����������Һ��������ʾ�ļ�ʱʱ�䣬�������жϵĶ�ʱʱ����1ms

	EnableInterrupts;


}




#pragma CODE_SEG DEFAULT