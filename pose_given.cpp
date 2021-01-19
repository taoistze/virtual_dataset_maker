#include "pose_given.h"

pose_given::pose_given()//���캯��
{
	NULL;
}
void pose_given::eulertorotation()
{
	/*************��eulerתΪrotation mat*********/
	//�ֱ������������ת����
	Mat R_x = Mat::zeros(3, 3, CV_64FC1);
	Mat R_y = Mat::zeros(3, 3, CV_64FC1);
	Mat R_z = Mat::zeros(3, 3, CV_64FC1);
	//�ֱ�����������ת
		//������x����ת
	R_x.at<double>(0, 0) = 1;
	R_x.at<double>(1,1) = cos(r_x);
	R_x.at<double>(1,2) = -sin(r_x);
	R_x.at<double>(2,1) = sin(r_x);
	R_x.at<double>(2,2) = cos(r_x);
		//������y����ת
	R_y.at<double>(1,1) = 1;
	R_y.at<double>(0,0) = cos(r_y);
	R_y.at<double>(2,0) = -sin(r_y);
	R_y.at<double>(0,2) = sin(r_y);
	R_y.at<double>(2, 2) = cos(r_y);
		//������z����ת
	R_z.at<double>(2,2) = 1;
	R_z.at<double>(0,0) = cos(r_z);
	R_z.at<double>(0,1) = -sin(r_z);
	R_z.at<double>(1,0) = sin(r_z);
	R_z.at<double>(1,1) = cos(r_z);
	//������ת����
	rotation_mat = R_y * R_x * R_z;//����zxy��˳����ת
	rotation_mat_back = rotation_mat;
	
}
void pose_given::set_tran(double tx, double ty, double tz)//����tan
{
	translation_mat.at<double>(0, 0) = tx;
	translation_mat.at<double>(1, 0) = ty;
	translation_mat.at<double>(2, 0) = -tz;
	translation_mat.copyTo(translation_mat_back);
	translation_mat_back.at<double>(2, 0) = -translation_mat_back.at<double>(2, 0);
}
void pose_given::set_rot(double rot_x, double rot_y, double rot_z)
{
	r_x = rot_x /180*PI;
	r_y = rot_y /180*PI;
	r_z = rot_z /180*PI;
	eulertorotation();//cal the rotation mat
}
void pose_given::cal_model()
{
	Mat transform_mat = Mat::zeros(3, 3, CV_64FC1);//the transport mat from cv to gl
	transform_mat.at<double>(0, 0) = 1;
	transform_mat.at<double>(1, 1) = 1;
	transform_mat.at<double>(2, 2) = 1;
	Mat rotation_gl_mat = Mat::zeros(3, 3, CV_64FC1);//
	rotation_gl_mat = transform_mat * rotation_mat;//ת��
	translation_mat = transform_mat * translation_mat;

	//��ֵ��view_mat��ע��viewΪת�õ�
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
			model_mat_gl.at<double>(i,j) = rotation_gl_mat.at<double>(i, j);
	}
	//����T
	for (int i = 0; i < 3; i++)
		model_mat_gl.at<double>(3,i) = translation_mat.at<double>(i,0);
	model_mat_gl.at<double>(3, 3) = 1;

}
void pose_given::pose_given_rotvector(Mat rot_vector)
{
	Rodrigues(rot_vector, rotation_mat);//��תʸ��ת��ת����
}

