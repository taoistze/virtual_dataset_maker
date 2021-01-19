#pragma once
//������ͶӰ
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;
class re_proj 
{
public:
	re_proj(string data_path);
	void back_proj(Mat R, Mat T, InputArray img, double focal, double dx, int ROWS, int COLS, int index);//��ͶӰ����
	Point3d cal_pts_on_camera_cord(Mat R, Mat T, Point3f pts_on_world_cord);
	vector<Point2d> pts_2d;//ͶӰ�õ���2d������
	vector<Point3d> pts_3d;//3D������
	void get_vis_data(InputArray datas_3d, int ROWS, int COLS);//���ڼ����Ƿ�ɼ�
	bool whether_vis(int width_src, int height_src, int ROWS, int COLS, Point2d pt_2d, Point3d pt_3d, InputArray datas_3d, double theta_dis, int size);//�����ж�������Ƿ�ɼ�
	vector<int> vis_data;
	void show_vis_result(InputArray img, int index);//չʾ�ɼ���
	void cal_frame(InputArray datas_3d, int ROWS, int COLS);//���ڼ���frame
	vector<Point2d> frame;
	void show_frame(InputArray img);//����չʾframe
	vector<Point2d> reproj_3d_bbox(Mat R, Mat T, InputArray img, double focal, double dx, int ROWS, int COLS, int index, string bbox_3d);//this fun is used to cal the projection of 3d bbox



};