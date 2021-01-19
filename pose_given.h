#pragma once
#ifndef __POSE_GIVEN_H__
#define __POSE_GIVEN_H__
//用于给出opengl渲染需要的姿态，姿态通过euler角给出
#include <opencv2/opencv.hpp>
#include <iostream>


using namespace cv;
using namespace std;
#define PI 3.14159265
class pose_given
{
public:
	//姿态角
	double r_x=0;//俯仰角,x
	double r_y=0;//滚转角,y
	double r_z=0;//偏航角,z
	//旋转矩阵
	Mat rotation_mat = Mat::zeros(3,3,CV_64FC1);//row,col,type
	//opengl所需的model mat
	Mat model_mat_gl = Mat::zeros(4, 4, CV_64FC1);
	//平移矢量
	Mat translation_mat = Mat::zeros(3,1,CV_64FC1);
	//用于重投影
	Mat rotation_mat_back = Mat::zeros(3, 3, CV_64FC1);
	Mat translation_mat_back = Mat::zeros(3, 1, CV_64FC1);

	pose_given();
	void set_rot(double PITCH, double YAW, double ROLL);
	void eulertorotation();//将欧拉角转为旋转矩阵,旋转顺序为zxy，即pitch yaw roll
	void set_tran(double tx,double ty,double tz);
	void cal_model();//通过R，T计算view_mat
	void pose_given_rotvector(Mat rot_vector);
	//void cal_projection(double focal);//通过焦距计算projection_mat

};



#endif