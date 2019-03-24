/*********************************************************************************
JJ_fly by TIVA
�ļ�����control.c
���������Ƴ���
���ߣ�¬���
ʱ�䣺2015.11
**********************************************************************************/
#include "imu_new.h"
#include "mpu6050.h"
#include "math.h"
#include "PWM.h"
#include "control.h"
#include "capture.h"
#include "mymath.h"
#include "US100.h"
#include "flow.h"
float T;//����
int thr;//����
int motor_1,motor_2,motor_3,motor_4;//��·���pwm���
float error_angle_x,error_angle_y,error_angle_z;//�Ƕ����
float except_angle_x,except_angle_y,except_angle_z;//�����Ƕ�
float offset_angle_x,offset_angle_y,offset_angle_z;//�ǶȲ���
float i_error_angle_x,i_error_angle_y,i_error_angle_z;//�Ƕ�������ֵ
float i_error_angle_x_max,i_error_angle_y_max,i_error_angle_z_max;//�����޷����ֵ
float i_error_angle_x_min,i_error_angle_y_min,i_error_angle_z_min;//�����޷���Сֵ
float d_error_angle_x,d_error_angle_y,d_error_angle_z;//�Ƕ����΢��ֵ
float control_1_x,control_1_y,control_1_z;//�⻷pid���
float old_error_angle_x,old_error_angle_y,old_error_angle_z;//��ʷ����
float kp_angle_x,ki_angle_x,kd_angle_x,kp_angle_y,ki_angle_y,kd_angle_y,kp_angle_z,ki_angle_z,kd_angle_z;//�⻷pid����

float error_aspeed_x,error_aspeed_y,error_aspeed_z;//���ٶ����
float except_aspeed_x,except_aspeed_y,except_aspeed_z;//�������ٶ�
float offset_aspeed_x,offset_aspeed_y,offset_aspeed_z;//���ٶȲ���
float i_error_aspeed_x,i_error_aspeed_y,i_error_aspeed_z;//���ٶ�������ֵ
float i_error_aspeed_x_max,i_error_aspeed_y_max,i_error_aspeed_z_max;//�����޷����ֵ
float i_error_aspeed_x_min,i_error_aspeed_y_min,i_error_aspeed_z_min;//�����޷���Сֵ
float d_error_aspeed_x,d_error_aspeed_y,d_error_aspeed_z;//���ٶ����΢��ֵ
float control_2_x,control_2_y,control_2_z;//�ڻ�pid���
float old_error_aspeed_x,old_error_aspeed_y,old_error_aspeed_z;//��ʷ����
float kp_aspeed_x,ki_aspeed_x,kd_aspeed_x,kp_aspeed_y,ki_aspeed_y,kd_aspeed_y,kp_aspeed_z,ki_aspeed_z,kd_aspeed_z;//�ڻ�pid����
int THR_keep;

//�߶ȿ���
unsigned char mode_height;
unsigned char mode_land;
float error_height , except_height;
float kp_height,ki_height,kd_height,ki_height_ready;
float ctrl_height_out,error_height,i_error_height,d_error_height,old_error_height;
unsigned char error_flag;
unsigned char ctrl_height;
unsigned char fly_low;
float fly_thr;//��������
//����
float except_x_angle;
float except_y_angle;
float kp_point,kd_point,ki_point,ki_dis;
float d_error_vx,d_error_vy,error_vx,error_vy,i_error_vx,i_error_vy;
float old_error_vx,old_error_vy;
unsigned char point_flag;
float i_error_X,i_error_Y;
float error_X,error_Y,d_error_X,d_error_Y,old_error_X,old_error_Y;
int buchang;
void control_init()
{
	//�趨pid����
	kp_angle_x=580;
	ki_angle_x=5;
	kd_angle_x=0;
	kp_angle_y=580;
	ki_angle_y=5;
	kd_angle_y=0;
	kp_angle_z=700;
	ki_angle_z=0;
	kd_angle_z=0;

	kp_aspeed_x=42;
	ki_aspeed_x=0;
	kd_aspeed_x=1500;
	kp_aspeed_y=42;
	ki_aspeed_y=0;
	kd_aspeed_y=1500;
	kp_aspeed_z=300;
	ki_aspeed_z=0;
	kd_aspeed_z=400;
	//�趨�����޷�
	i_error_angle_x_max=1200;
	i_error_angle_y_max=1200;
	i_error_angle_z_max=1200;

	i_error_angle_x_min=-1200;
	i_error_angle_y_min=-1200;
	i_error_angle_z_min=-1200;

	i_error_aspeed_x_max=400;
	i_error_aspeed_y_max=400;
	i_error_aspeed_z_max=2000;

	i_error_aspeed_x_min=-400;
	i_error_aspeed_y_min=-400;
	i_error_aspeed_z_min=-2000;
	//�趨����
	except_height=9.0f;
	kp_height=120;
	kd_height=1000;
	ki_height=11;
	ki_height_ready=38;
	T=5;//pwmˢ��Ϊ5ms
	//�������
	kp_point=150;
	ki_point=30;
	kd_point=750;
	buchang=15;
}
void control_1()//x roll,y pitch,z yaw
{
	//�õ��Ƕ����
	error_angle_x = except_angle_x + offset_angle_x - Q_ANGLE_X;
	error_angle_y = except_angle_y + offset_angle_y - Q_ANGLE_Y;
	error_angle_z = except_angle_z + offset_angle_z - Q_ANGLE_Z;
	if(error_angle_x<=0.1f&&error_angle_x>=-0.1f)
	{
		error_angle_x=0;
	}
	if(error_angle_y<=0.1f&&error_angle_y>=-0.1f)
	{
		error_angle_y=0;
	}
	if(error_angle_z<=0.2f&&error_angle_z>=-0.2f)
	{
		error_angle_z=0;
	}
	if(error_angle_z>50.0f||error_angle_z<-50.0f)
	{
		error_angle_z=0;
	}
	//�Ƕ�������
	i_error_angle_x = (i_error_angle_x + error_angle_x);
	i_error_angle_y = (i_error_angle_y + error_angle_y);
	i_error_angle_z = (i_error_angle_z + error_angle_z);
	//�Ƕ��������޷�
	if(i_error_angle_x > i_error_angle_x_max) i_error_angle_x = i_error_angle_x_max;
	if(i_error_angle_x < i_error_angle_x_min) i_error_angle_x = i_error_angle_x_min;
	if(i_error_angle_y > i_error_angle_y_max) i_error_angle_y = i_error_angle_y_max;
	if(i_error_angle_y < i_error_angle_y_min) i_error_angle_y = i_error_angle_y_min;
	if(i_error_angle_z > i_error_angle_z_max) i_error_angle_z = i_error_angle_z_max;
	if(i_error_angle_z < i_error_angle_z_min) i_error_angle_z = i_error_angle_z_min;
	//�Ƕ����΢��
	d_error_angle_x = (error_angle_x - old_error_angle_x) / T;
	d_error_angle_y = (error_angle_y - old_error_angle_y) / T;
	d_error_angle_z = (error_angle_z - old_error_angle_z) / T;
	//�⻷pid���
	control_1_x = kp_angle_x * error_angle_x + ki_angle_x * i_error_angle_x / 100 + kd_angle_x * d_error_angle_x;
	control_1_y = kp_angle_y * error_angle_y + ki_angle_y * i_error_angle_y / 100+ kd_angle_y * d_error_angle_y;
	control_1_z = kp_angle_z * error_angle_z + ki_angle_z * i_error_angle_z / 100+ kd_angle_z * d_error_angle_z;
	//������ʷ����
	old_error_angle_x = error_angle_x;
	old_error_angle_y = error_angle_y;
	old_error_angle_z = error_angle_z;
	if(CH_2<CH_2_MID-2||CH_2>CH_2_MID+2)
	{
		control_1_z=0;//�������������
		except_angle_z=Q_ANGLE_Z;
	}
	else
	{
		error_Y=0;
	}
}
void control_2()//x roll,y pitch,z yaw
{
	//�õ����ٶ����
	error_aspeed_x = except_aspeed_x + offset_aspeed_x - MPU6050_GYRO_LAST_X*Gyro_G;
	error_aspeed_y = except_aspeed_y + offset_aspeed_y - MPU6050_GYRO_LAST_Y*Gyro_G;
	error_aspeed_z = except_aspeed_z + offset_aspeed_z - MPU6050_GYRO_LAST_Z/100;
	//���ٶ�������
	i_error_aspeed_x = (i_error_aspeed_x + error_aspeed_x)/10000;
	i_error_aspeed_y = (i_error_aspeed_y + error_aspeed_y)/10000;
	i_error_aspeed_z = (i_error_aspeed_z + error_aspeed_z)/10000;

	//���ٶ��������޷�
	if(i_error_aspeed_x > i_error_aspeed_x_max) i_error_aspeed_x = i_error_aspeed_x_max;
	if(i_error_aspeed_x < i_error_aspeed_x_min) i_error_aspeed_x = i_error_aspeed_x_min;
	if(i_error_aspeed_y > i_error_aspeed_y_max) i_error_aspeed_y = i_error_aspeed_y_max;
	if(i_error_aspeed_y < i_error_aspeed_y_min) i_error_aspeed_y = i_error_aspeed_y_min;
	if(i_error_aspeed_z > i_error_aspeed_z_max) i_error_aspeed_z = i_error_aspeed_z_max;
	if(i_error_aspeed_z < i_error_aspeed_z_min) i_error_aspeed_z = i_error_aspeed_z_min;

	//���ٶ����΢��
	d_error_aspeed_x = (error_aspeed_x - old_error_aspeed_x) / T;
	d_error_aspeed_y = (error_aspeed_y - old_error_aspeed_y) / T;
	d_error_aspeed_z = (error_aspeed_z - old_error_aspeed_z) / T;
	//�ڻ�pid���
	control_2_x = kp_aspeed_x * error_aspeed_x + ki_aspeed_x * i_error_aspeed_x + kd_aspeed_x * d_error_aspeed_x;
	control_2_y = kp_aspeed_y * error_aspeed_y + ki_aspeed_y * i_error_aspeed_y + kd_aspeed_y * d_error_aspeed_y;
	control_2_z = kp_aspeed_z * error_aspeed_z + ki_aspeed_z * i_error_aspeed_z + kd_aspeed_z * d_error_aspeed_z;
	//������ʷ����
	old_error_aspeed_x = error_aspeed_x;
	old_error_aspeed_y = error_aspeed_y;
	old_error_aspeed_z = error_aspeed_z;
}
//һ�����ͨ���ı�߶������ֳ���Iʵ��
//�����֮ǰ��Iȡ�ıȽϴ�ki_height_ready�������ж����֮��Iȡ�ıȽ�С��ki_height��
void control_height()
{
		//�߶Ȼ�
		if(mode_land==0)
		{
			except_height=(CH_1-CH_1_MIN)/5.0f+0.4f;
		}
		//�õ��߶����
		error_height = except_height - US100_Alt;

		//�߶����΢��
		d_error_height = (error_height - old_error_height);
		if(US100_Alt<2.5f&&fly_low==0)
		{
			//�߶�������
			i_error_height = (i_error_height + ki_height_ready*error_height);
			fly_thr=0;
		}
		//��ɸ߶��ж�Ϊ20cm
		if(US100_Alt>2.5f&&fly_low==0)
		{
			fly_low=1;
			fly_thr=i_error_height;
			i_error_height=0;
		}
		if(fly_low==1)
		{
			i_error_height = (i_error_height + ki_height*error_height);
		}
		//�߶��������޷�
		if(i_error_height > 16000.0f) i_error_aspeed_x = 16000.0f;
		if(i_error_height < -16000.0f) i_error_aspeed_x = -16000.0f;
		//�߶Ȼ�
		if(mode_land==0)
		{
			ctrl_height_out = fly_thr+kp_height * error_height + i_error_height + kd_height * d_error_height;
		}
		else
		{
			ctrl_height_out = fly_thr+kp_height * error_height + i_error_height + 200.0f * d_error_height;
		}
		//������ʷ����
		old_error_height = error_height;
}
void land()//һ������
{
	if(mode_land==1)
	{
		//if(except_height>1.0f)
		{
			except_height=except_height-0.8f;
		}
		/*if(US100_Alt<2.5f)
		{
			fly_thr=fly_thr-100;
		}*/

	}
}
//����ADNS3080�Ŀ��ƶ���ĳ�������3080�������ܴ󣬲��˲��û��ֿ���
void control_point()//�������
{
	error_vx=vx-127;
	error_vy=vy-127;

	if(error_vx>-2&&error_vx<2)
		error_vx=0;
	if(error_vy>-2&&error_vy<2)
		error_vy=0;
	d_error_vx = (error_vx - old_error_vx);
	d_error_vy = (error_vy - old_error_vy);
    if(point_flag==1)
    {
		i_error_vx=i_error_vx+error_vx;
		i_error_vy=i_error_vy+error_vy;
    }
	if(i_error_vx>50) i_error_vx=50;
	if(i_error_vx<-50) i_error_vx=-50;
	if(i_error_vy>50) i_error_vy=50;
	if(i_error_vy<-50) i_error_vy=-50;
	except_x_angle=kp_point*0.01f*error_vx+kd_point*0.01f*d_error_vx+ki_point*0.001f*i_error_vx;
	except_y_angle=kp_point*0.01f*error_vy+kd_point*0.01f*d_error_vy+ki_point*0.01f*i_error_vy;

	if(except_x_angle>3.0f) except_x_angle=3.0f;
	if(except_x_angle<-3.0f) except_x_angle=-3.0f;
	if(except_y_angle>3.0f) except_y_angle=3.0f;
	if(except_y_angle<-3.0f) except_y_angle=-3.0f;

	old_error_vx=error_vx;
	old_error_vy=error_vy;
}
void control_point_1()//�������1
{
	error_X=X-30+(Q_ANGLE_Y-offset_angle_y)*buchang*US100_Alt/50;
	error_Y=Y-40+(Q_ANGLE_X-offset_angle_x)*buchang*US100_Alt/50;
	if(US100_Alt<0.6f)
	{
		error_X=0;
		error_Y=0;
	}
	if(Y==0)
	{
		error_Y=0;
	}
	if(X==0)
	{
		error_X=0;
	}
	/*if(error_vx>-2&&error_vx<2)
		error_vx=0;
	if(error_vy>-2&&error_vy<2)
		error_vy=0;*/
	d_error_X = (error_X - old_error_X);
	d_error_Y = (error_Y - old_error_Y);
	if(US100_Alt>2.0f)
	{
		i_error_X+=error_X;
		i_error_Y+=error_Y;
	}
	except_x_angle=kp_point*0.001f*error_X+kd_point*0.001f*d_error_X+ki_point*0.0001f*i_error_X;
	except_y_angle=kp_point*0.001f*error_Y+kd_point*0.001f*d_error_Y+ki_point*0.0001f*i_error_Y;

	if(except_x_angle>2.0f) except_x_angle=2.0f;
	if(except_x_angle<-2.0f) except_x_angle=-2.0f;
	if(except_y_angle>2.0f) except_y_angle=2.0f;
	if(except_y_angle<-2.0f) except_y_angle=-2.0f;

	old_error_X=error_X;
	old_error_Y=error_Y;
}
void control()
{
	except_angle_x=(CH_5-CH_5_MID)/1.0f*(-1)-except_y_angle;
	except_angle_y=(CH_6-CH_6_MID)/1.0f-except_x_angle;
	except_aspeed_z=(CH_2-CH_2_MID)*4.0f*(-1);//z����ƽ��ٶ�
	control_1();//�⻷
	control_2();//�ڻ�
	if(CH_1<CH_1_MIN||CH_1>CH_1_MAX)//�����쳣ֵ�ų�
	{
		CH_1=CH_1_MIN;
	}

	if(CH_3<CH_3_HEIGHT+5&&CH_3>CH_3_HEIGHT-5)//�������ݴ�10���������ش���߽�����ͬʱz�Ὺʼ����
	{
		/////����ģʽ
		mode_height=1;//��������ģʽ
		motor_1 = 20000 + ctrl_height_out - control_1_x - control_1_y - control_1_z + control_2_x - control_2_y + control_2_z;
		motor_2 = 20000 + ctrl_height_out + control_1_x - control_1_y + control_1_z - control_2_x - control_2_y - control_2_z;
		motor_3 = 20000 + ctrl_height_out + control_1_x + control_1_y - control_1_z - control_2_x + control_2_y + control_2_z;
		motor_4 = 20000 + ctrl_height_out - control_1_x + control_1_y + control_1_z + control_2_x + control_2_y - control_2_z;
	}
	else if(CH_3<CH_3_HAND+5&&CH_3>CH_3_HAND-5)//�������ݴ�10���������ش���߽�����ͬʱz�Ὺʼ����
	{
		/////�ֶ�ģʽ
		if(mode_height==0)
		{
			motor_1 = 20000 +(CH_1 - CH_1_MIN) * 200 - control_1_x - control_1_y - control_1_z + control_2_x - control_2_y + control_2_z;
			motor_2 = 20000 +(CH_1 - CH_1_MIN) * 200 + control_1_x - control_1_y + control_1_z - control_2_x - control_2_y - control_2_z;
			motor_3 = 20000 +(CH_1 - CH_1_MIN) * 200 + control_1_x + control_1_y - control_1_z - control_2_x + control_2_y + control_2_z;
			motor_4 = 20000 +(CH_1 - CH_1_MIN) * 200 - control_1_x + control_1_y + control_1_z + control_2_x + control_2_y - control_2_z;

		}
		else if(mode_height==1)//���ߵ�ʱ���������ش���������ģʽ
		{
			mode_land=1;//��������ģʽ
			motor_1 = 20000 + ctrl_height_out - control_1_x - control_1_y - control_1_z + control_2_x - control_2_y + control_2_z;
			motor_2 = 20000 + ctrl_height_out + control_1_x - control_1_y + control_1_z - control_2_x - control_2_y - control_2_z;
			motor_3 = 20000 + ctrl_height_out + control_1_x + control_1_y - control_1_z - control_2_x + control_2_y + control_2_z;
			motor_4 = 20000 + ctrl_height_out - control_1_x + control_1_y + control_1_z + control_2_x + control_2_y - control_2_z;

		}
		//
	}
	else //δ����������pwm�����z����ٶ�
	{
		motor_1 = 20000;
		motor_2 = 20000;
		motor_3 = 20000;
		motor_4 = 20000;
		GYRO_I_Z=0;//�����ǻ�������
		error_flag=0;
		i_error_aspeed_z=0;
		Az=0;
		i_error_height=0;
		fly_thr=0;
		fly_low=0;
		except_angle_z=0;
		mode_land=0;
		mode_height=0;
		i_error_vx=0;
		i_error_vy=0;
		point_flag=0;
		//offset_angle_x = Q_ANGLE_X;
		//offset_angle_y = Q_ANGLE_Y;
		X=0;
		Y=0;
	}
	if(CH_1<CH_1_MIN+2)//���Ŵ���ͽ�ֹ���
	{
		motor_1 = 20000;
		motor_2 = 20000;
		motor_3 = 20000;
		motor_4 = 20000;
		i_error_aspeed_x=0;
		i_error_aspeed_y=0;
		i_error_angle_x=0;
		i_error_angle_y=0;
		i_error_X=0;
		i_error_Y=0;
	}
	if(US100_Alt<1.8f&&mode_land==1)
	{
		error_flag=1;//�رյ��
		mode_land=0;
		mode_height=0;
	}
	if(Q_ANGLE_X>50 || Q_ANGLE_X<-50 || Q_ANGLE_Y>50 || Q_ANGLE_Y<-50 )
	{
		error_flag=1;
	}
	if(error_flag==1)//�Ƕȳ����쳣ֵ
	{
		motor_1 = 20000;
		motor_2 = 20000;
		motor_3 = 20000;
		motor_4 = 20000;
	}

	//pwm���޷���20000��Ӧ1ms��40000��Ӧ2ms
	if(motor_1<20000) motor_1=20000;
	if(motor_1>40000) motor_1=40000;
	if(motor_2<20000) motor_2=20000;
	if(motor_2>40000) motor_2=40000;
	if(motor_3<20000) motor_3=20000;
	if(motor_3>40000) motor_3=40000;
	if(motor_4<20000) motor_4=20000;
	if(motor_4>40000) motor_4=40000;
	//ˢ��pwmֵ
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, motor_2);//m2
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_1, motor_1);//m1
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_2, motor_4);//m4
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_3, motor_3);//m3
}
