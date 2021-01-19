#pragma once
#ifndef __POSE_GIVEN_H__
#define __POSE_GIVEN_H__
//���ڸ���opengl��Ⱦ��Ҫ����̬����̬ͨ��euler�Ǹ���
#include <opencv2/opencv.hpp>
#include <iostream>


using namespace cv;
using namespace std;
#define PI 3.14159265
class pose_given
{
public:
	//��̬��
	double r_x=0;//������,x
	double r_y=0;//��ת��,y
	double r_z=0;//ƫ����,z
	//��ת����
	Mat rotation_mat = Mat::zeros(3,3,CV_64FC1);//row,col,type
	//opengl�����model mat
	Mat model_mat_gl = Mat::zeros(4, 4, CV_64FC1);
	//ƽ��ʸ��
	Mat translation_mat = Mat::zeros(3,1,CV_64FC1);
	//������ͶӰ
	Mat rotation_mat_back = Mat::zeros(3, 3, CV_64FC1);
	Mat translation_mat_back = Mat::zeros(3, 1, CV_64FC1);

	pose_given();
	void set_rot(double PITCH, double YAW, double ROLL);
	void eulertorotation();//��ŷ����תΪ��ת����,��ת˳��Ϊzxy����pitch yaw roll
	void set_tran(double tx,double ty,double tz);
	void cal_model();//ͨ��R��T����view_mat
	void pose_given_rotvector(Mat rot_vector);
	//void cal_projection(double focal);//ͨ���������projection_mat

};



#endif