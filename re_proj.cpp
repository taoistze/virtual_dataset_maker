#include "re_proj.h"

double cal_dis(Point3d pt_1, Point3d pt_2)
{
	return (pt_1.x - pt_2.x) * (pt_1.x - pt_2.x) + (pt_1.y - pt_2.y) * (pt_1.y - pt_2.y) + (pt_1.z - pt_2.z) * (pt_1.z - pt_2.z);
}

re_proj::re_proj(string data_path)
{
	ifstream keypts;
	keypts.open(data_path);
	//first,read the total num of keypoints
	string data;
	keypts >> data;
	int total_num_kpts = atoi(data.c_str());
	//load keeypoints from the file
	for (int i = 0; i < total_num_kpts; i++)
	{
		Point3d pt;
		keypts >> data;
		pt.x= atof(data.c_str())*0.0254;
		keypts >> data;
		pt.y = atof(data.c_str()) * 0.0254;
		keypts >> data;
		pt.z = atof(data.c_str()) * 0.0254;
		pts_3d.push_back(pt);

	}
}

Point3d re_proj::cal_pts_on_camera_cord(Mat R, Mat T, Point3f pts_on_world_cord)
{
	Point3d pts_carmra_cord = { 0.,0.,0. };
	pts_carmra_cord.x = R.at<double>(0, 0) * pts_on_world_cord.x + R.at<double>(0, 1) * pts_on_world_cord.y + R.at<double>(0, 2) * pts_on_world_cord.z + T.at<double>(0);
	pts_carmra_cord.y = R.at<double>(1, 0) * pts_on_world_cord.x + R.at<double>(1, 1) * pts_on_world_cord.y + R.at<double>(1, 2) * pts_on_world_cord.z + T.at<double>(1);
	pts_carmra_cord.z = R.at<double>(2, 0) * pts_on_world_cord.x + R.at<double>(2, 1) * pts_on_world_cord.y + R.at<double>(2, 2) * pts_on_world_cord.z + T.at<double>(2);
	return pts_carmra_cord;
}

void re_proj::back_proj(Mat R, Mat T, InputArray img, double focal, double dx, int ROWS, int COLS, int index)
{
	pts_2d.clear();
	/*for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			cout << R.at<double>(i, j) << "	";
		}
		cout << endl;
	}
	for (int i = 0; i < 3; i++)
		cout << T.at<double>(i) << endl;*/
	Mat img_copy;
	img.copyTo(img_copy);
	int total_num = pts_3d.size();
	for (int i = 0; i < total_num; i++)
	{
		Point3d pts_i_camera = cal_pts_on_camera_cord(R, T, pts_3d[i]);//将点坐标转到相机系
		//将点坐标转到图像系
		//将点转到图像系
		Point2d pts_pic_cord;
		pts_pic_cord.x = pts_i_camera.x / pts_i_camera.z * (focal) / dx + COLS / 2;
		pts_pic_cord.y = pts_i_camera.y / pts_i_camera.z * (focal) / dx + ROWS / 2;
		//画图
		//circle(img_copy, pts_pic_cord, 1, Scalar(0., 0., 255.));
		//putText(img_copy, to_string(i + 1), pts_pic_cord, FONT_HERSHEY_COMPLEX, 0.5, Scalar(0., 0., 255.), 0.5, 8);
		pts_2d.push_back(pts_pic_cord);
		//imshow("img_back", img_copy);
		//waitKey(0);

	}
	////将坐标原点画出来
	//Point3d pts_i_camera = cal_pts_on_camera_cord(R, T, Point3d(0.,0.,0.));//将点坐标转到相机系
	//	//将点坐标转到图像系
	//	//将点转到图像系
	//Point2d pts_pic_cord;
	//pts_pic_cord.x = pts_i_camera.x / pts_i_camera.z * (focal) / dx + COLS / 2;
	//pts_pic_cord.y = pts_i_camera.y / pts_i_camera.z * (focal) / dx + ROWS / 2;
	////画图
	//circle(img_copy, pts_pic_cord, 1, Scalar(0., 0., 255.));
	//putText(img_copy, to_string(0), pts_pic_cord, FONT_HERSHEY_COMPLEX, 0.5, Scalar(0., 0., 255.), 0.5, 8);
	//namedWindow("img_back", WINDOW_NORMAL);
	//imshow("img_back"+to_string(index), img_copy);
	//waitKey(0.1);
}

void re_proj::get_vis_data(InputArray datas_3d, int ROWS, int COLS)//参数为设置的图像大小
{
	double theta_dis = 0;
	vis_data.clear();
	//首先获取图像大小
	int src_width = datas_3d.cols();
	int src_height = datas_3d.rows();//注意这个参数
	int total_num = pts_3d.size();
	for (int i = 0; i < total_num; i++)
	{
		/*if (i >= 3 & i <= 18)
			theta_dis = 1e-2;
		else
			theta_dis = 1.f;*/
		theta_dis = 0.5;
		//cout << i + 1 << "	" ;
		if (whether_vis(src_width, src_height, ROWS, COLS, pts_2d[i], pts_3d[i], datas_3d, theta_dis, 5))
			vis_data.push_back(1);
		else
			vis_data.push_back(0);
	}
}

bool re_proj::whether_vis(int width_src, int height_src, int ROWS, int COLS, Point2d pt_2d, Point3d pt_3d, InputArray datas_3d, double theta_dis, int size)
{
	//首先将点进行缩放，opencv坐标系：右x，下y
	double x = pt_2d.x;//对应col，width
	double y = pt_2d.y;//对应raw，height
	//计算缩放后的点,注意这些点不是整数
	x = x / COLS * width_src;
	y = y / ROWS * height_src;
	//将点向下取整
	int x_int = ceil(x);
	int y_int = ceil(y);
	//获取深度信息
	Mat datas_3d_mat = datas_3d.getMat();
	//bool visi = false;
	double dis = 10000;
	Point3d nearest_pt;
	for (int i = x_int - size; i < x_int + size; i++)
	{
		for (int j = y_int - size; j < y_int + size; j++)
		{
			Point3d nearest_pt_3d = (Point3d(datas_3d_mat.at<Vec4f>(j, i)[0], datas_3d_mat.at<Vec4f>(j, i)[1], datas_3d_mat.at<Vec4f>(j, i)[2]));
			if (cal_dis(nearest_pt_3d, pt_3d) < dis)//
			{
				dis = cal_dis(nearest_pt_3d, pt_3d);
				nearest_pt = nearest_pt_3d;
			}

		}

	}
	//cout << dis <<"		"<<nearest_pt.x<<","<< nearest_pt.y << "," << nearest_pt.z << "," << endl;
	if (dis < theta_dis)
		return true;
	return false;

}

void re_proj::show_vis_result(InputArray img, int index)
{
	Mat img_mat = img.getMat();
	int total_num = pts_3d.size();
	for (int i = 0; i < total_num; i++)
	{
		if (vis_data[i] == 1)
		{
			circle(img_mat, pts_2d[i], 4, Scalar(255., 0., 0.));
			putText(img_mat, to_string(i + 1), pts_2d[i], FONT_HERSHEY_COMPLEX, 0.5, Scalar(0., 0., 255.), 0.5, 8);
			//cout << i + 1 << endl;
		}

	}
	imshow("vis_result" + to_string(index), img_mat);
	waitKey(0);
}

void re_proj::cal_frame(InputArray datas_3d, int ROWS, int COLS)
{
	for (int i = 0; i < 12; i++)
	{
		frame.push_back(Point2d(0., 0.));
	}
	//用于根据获取的三维信息计算frame
	//首先获取图像大小
	int src_width = datas_3d.cols();
	int src_height = datas_3d.rows();//注意这个参数
	//frame包括，1-18；19-20-4；21-22-5；7-8-6-9；11-12-10-13；16-17-14-15
	//对于1-18这个frame，直接调用重投影点
	frame[0] = pts_2d[0];
	frame[1] = pts_2d[17];
	/**************************************处理19-20-4，右侧机翼******************************************/
	//分别计算19-4,20-4，中间点连线的遮挡情况，然后查看那两个的k想接近作为遮挡情况的表示
	//处理19-4
	Point3d pt_bottom_3d = pts_3d[19 - 1];
	Point3d pt_top_3d = pts_3d[4 - 1];
	Point2d pt_bottom_2d = pts_2d[19 - 1];
	Point2d pt_top_2d = pts_2d[4 - 1];
	//遍历找到可见点
	float k1 = 0;
	for (k1 = 0; k1 <= 1; k1 = k1 + 0.01)//采样间隔为0.01
	{
		Point2d pt_2d = pt_bottom_2d + k1 * (pt_top_2d - pt_bottom_2d);//当前点2d坐标
		Point3d pt_3d = pt_bottom_3d + k1 * (pt_top_3d - pt_bottom_3d);//当前点2d坐标
		//判断这个点是否可见
		if (whether_vis(src_width, src_height, ROWS, COLS, pt_2d, pt_3d, datas_3d, 0.5f, 1))
			break;
	}
	//处理20-4
	pt_bottom_3d = pts_3d[20 - 1];
	pt_top_3d = pts_3d[4 - 1];
	pt_bottom_2d = pts_2d[20 - 1];
	pt_top_2d = pts_2d[4 - 1];
	//遍历找到可见点
	float k2 = 0;
	for (k2 = 0; k2 <= 1; k2 = k2 + 0.01)//采样间隔为0.01
	{
		Point2d pt_2d = pt_bottom_2d + k2 * (pt_top_2d - pt_bottom_2d);//当前点2d坐标
		Point3d pt_3d = pt_bottom_3d + k2 * (pt_top_3d - pt_bottom_3d);//当前点2d坐标
		//判断这个点是否可见
		if (whether_vis(src_width, src_height, ROWS, COLS, pt_2d, pt_3d, datas_3d, 0.5f, 1))
			break;
	}
	//处理中间
	pt_bottom_3d = (pts_3d[19 - 1] + pts_3d[20 - 1]) / 2;
	pt_top_3d = pts_3d[4 - 1];
	pt_bottom_2d = (pts_2d[19 - 1] + pts_2d[20 - 1]) / 2;
	pt_top_2d = pts_2d[4 - 1];
	//遍历找到可见点
	float k3 = 0;
	for (k3 = 00; k3 <= 1; k3 = k3 + 0.01)//采样间隔为0.01
	{
		Point2d pt_2d = pt_bottom_2d + k3 * (pt_top_2d - pt_bottom_2d);//当前点2d坐标
		Point3d pt_3d = pt_bottom_3d + k3 * (pt_top_3d - pt_bottom_3d);//当前点2d坐标
		//判断这个点是否可见
		if (whether_vis(src_width, src_height, ROWS, COLS, pt_2d, pt_3d, datas_3d, 0.5f, 1))
			break;
	}
	float k = min(min(k1, k2), k3);//k为三个中最小的，即为最接近bottom的点
	if (abs(k - 1) < 1e-3)
	{
		frame[3 - 1] = Point2d(0., 0.);
		frame[4 - 1] = Point2d(0., 0.);
	}
	else
	{
		frame[4 - 1] = pt_top_2d;
		frame[3 - 1] = pt_bottom_2d + k * (pt_top_2d - pt_bottom_2d);
	}
	/**************************************处理21-22-5,左侧机翼******************************************/
	//分别计算22-5,21-5，中间点连线的遮挡情况，然后查看那两个的k想接近作为遮挡情况的表示
	//处理22-5
	pt_bottom_3d = pts_3d[22 - 1];
	pt_top_3d = pts_3d[5 - 1];
	pt_bottom_2d = pts_2d[22 - 1];
	pt_top_2d = pts_2d[5 - 1];
	//遍历找到可见点
	k1 = 0;
	for (k1 = 0; k1 <= 1; k1 = k1 + 0.01)//采样间隔为0.01
	{
		Point2d pt_2d = pt_bottom_2d + k1 * (pt_top_2d - pt_bottom_2d);//当前点2d坐标
		Point3d pt_3d = pt_bottom_3d + k1 * (pt_top_3d - pt_bottom_3d);//当前点2d坐标
		//判断这个点是否可见
		if (whether_vis(src_width, src_height, ROWS, COLS, pt_2d, pt_3d, datas_3d, 0.5f, 1))
			break;
	}
	//处理21-5
	pt_bottom_3d = pts_3d[21 - 1];
	pt_top_3d = pts_3d[5 - 1];
	pt_bottom_2d = pts_2d[21 - 1];
	pt_top_2d = pts_2d[5 - 1];
	//遍历找到可见点
	k2 = 0;
	for (k2 = 0; k2 <= 1; k2 = k2 + 0.01)//采样间隔为0.01
	{
		Point2d pt_2d = pt_bottom_2d + k2 * (pt_top_2d - pt_bottom_2d);//当前点2d坐标
		Point3d pt_3d = pt_bottom_3d + k2 * (pt_top_3d - pt_bottom_3d);//当前点2d坐标
		//判断这个点是否可见
		if (whether_vis(src_width, src_height, ROWS, COLS, pt_2d, pt_3d, datas_3d, 0.5f, 1))
			break;
	}
	//处理中间
	pt_bottom_3d = (pts_3d[22 - 1] + pts_3d[21 - 1]) / 2;
	pt_top_3d = pts_3d[5 - 1];
	pt_bottom_2d = (pts_2d[22 - 1] + pts_2d[21 - 1]) / 2;
	pt_top_2d = pts_2d[5 - 1];
	//遍历找到可见点
	k3 = 0;
	for (k3 = 00; k3 <= 1; k3 = k3 + 0.01)//采样间隔为0.01
	{
		Point2d pt_2d = pt_bottom_2d + k3 * (pt_top_2d - pt_bottom_2d);//当前点2d坐标
		Point3d pt_3d = pt_bottom_3d + k3 * (pt_top_3d - pt_bottom_3d);//当前点2d坐标
		//判断这个点是否可见
		if (whether_vis(src_width, src_height, ROWS, COLS, pt_2d, pt_3d, datas_3d, 0.5f, 1))
			break;
	}
	k = min(min(k1, k2), k3);//k为三个中最小的，即为最接近bottom的点
	if (abs(k - 1) < 1e-3)
	{
		frame[5 - 1] = Point2d(0., 0.);
		frame[6 - 1] = Point2d(0., 0.);
	}
	else
	{
		frame[6 - 1] = pt_top_2d;
		frame[5 - 1] = pt_bottom_2d + k * (pt_top_2d - pt_bottom_2d);
	}
	/**************************************处理7-8-6-9,右侧尾翼******************************************/
	//分别计算6-7,8-9，中间点连线的遮挡情况，然后查看那两个的k想接近作为遮挡情况的表示
	//处理6-7
	pt_bottom_3d = pts_3d[7 - 1];
	pt_top_3d = pts_3d[6 - 1];
	pt_bottom_2d = pts_2d[7 - 1];
	pt_top_2d = pts_2d[6 - 1];
	//遍历找到可见点
	k1 = 0;
	for (k1 = 0; k1 <= 1; k1 = k1 + 0.01)//采样间隔为0.01
	{
		Point2d pt_2d = pt_bottom_2d + k1 * (pt_top_2d - pt_bottom_2d);//当前点2d坐标
		Point3d pt_3d = pt_bottom_3d + k1 * (pt_top_3d - pt_bottom_3d);//当前点2d坐标
		//判断这个点是否可见
		if (whether_vis(src_width, src_height, ROWS, COLS, pt_2d, pt_3d, datas_3d, 0.5f, 1))
			break;
	}
	//处理8-9
	pt_bottom_3d = pts_3d[8 - 1];
	pt_top_3d = pts_3d[9 - 1];
	pt_bottom_2d = pts_2d[8 - 1];
	pt_top_2d = pts_2d[9 - 1];
	//遍历找到可见点
	k2 = 0;
	for (k2 = 0; k2 <= 1; k2 = k2 + 0.01)//采样间隔为0.01
	{
		Point2d pt_2d = pt_bottom_2d + k2 * (pt_top_2d - pt_bottom_2d);//当前点2d坐标
		Point3d pt_3d = pt_bottom_3d + k2 * (pt_top_3d - pt_bottom_3d);//当前点2d坐标
		//判断这个点是否可见
		if (whether_vis(src_width, src_height, ROWS, COLS, pt_2d, pt_3d, datas_3d, 0.5f, 1))
			break;
	}
	//处理中间
	pt_bottom_3d = (pts_3d[7 - 1] + pts_3d[8 - 1]) / 2;
	pt_top_3d = (pts_3d[6 - 1] + pts_3d[9 - 1]) / 2;
	pt_bottom_2d = (pts_2d[7 - 1] + pts_2d[8 - 1]) / 2;
	pt_top_2d = (pts_2d[6 - 1] + pts_2d[9 - 1]) / 2;
	//遍历找到可见点
	k3 = 0;
	for (k3 = 00; k3 <= 1; k3 = k3 + 0.01)//采样间隔为0.01
	{
		Point2d pt_2d = pt_bottom_2d + k3 * (pt_top_2d - pt_bottom_2d);//当前点2d坐标
		Point3d pt_3d = pt_bottom_3d + k3 * (pt_top_3d - pt_bottom_3d);//当前点2d坐标
		//判断这个点是否可见
		if (whether_vis(src_width, src_height, ROWS, COLS, pt_2d, pt_3d, datas_3d, 0.5f, 1))
			break;
	}
	k = min(min(k1, k2), k3);//k为三个中最小的，即为最接近bottom的点
	if (abs(k - 1) < 1e-3)
	{
		frame[8 - 1] = Point2d(0., 0.);
		frame[7 - 1] = Point2d(0., 0.);
	}
	else
	{
		frame[8 - 1] = pt_top_2d;
		frame[7 - 1] = pt_bottom_2d + k * (pt_top_2d - pt_bottom_2d);
	}
	/**************************************处理11-12-10-13,左侧尾翼******************************************/
	//分别计算10-11,12-13，中间点连线的遮挡情况，然后查看那两个的k想接近作为遮挡情况的表示
	//处理10-11
	pt_bottom_3d = pts_3d[10];
	pt_top_3d = pts_3d[9];
	pt_bottom_2d = pts_2d[10];
	pt_top_2d = pts_2d[9];
	//遍历找到可见点
	k1 = 0;
	for (k1 = 0; k1 <= 1; k1 = k1 + 0.01)//采样间隔为0.01
	{
		Point2d pt_2d = pt_bottom_2d + k1 * (pt_top_2d - pt_bottom_2d);//当前点2d坐标
		Point3d pt_3d = pt_bottom_3d + k1 * (pt_top_3d - pt_bottom_3d);//当前点2d坐标
		//判断这个点是否可见
		if (whether_vis(src_width, src_height, ROWS, COLS, pt_2d, pt_3d, datas_3d, 0.5f, 1))
			break;
	}
	//处理12-13
	pt_bottom_3d = pts_3d[11];
	pt_top_3d = pts_3d[12];
	pt_bottom_2d = pts_2d[11];
	pt_top_2d = pts_2d[12];
	//遍历找到可见点
	k2 = 0;
	for (k2 = 0; k2 <= 1; k2 = k2 + 0.01)//采样间隔为0.01
	{
		Point2d pt_2d = pt_bottom_2d + k2 * (pt_top_2d - pt_bottom_2d);//当前点2d坐标
		Point3d pt_3d = pt_bottom_3d + k2 * (pt_top_3d - pt_bottom_3d);//当前点2d坐标
		//判断这个点是否可见
		if (whether_vis(src_width, src_height, ROWS, COLS, pt_2d, pt_3d, datas_3d, 0.5f, 1))
			break;
	}
	//处理中间
	pt_bottom_3d = (pts_3d[11 - 1] + pts_3d[12 - 1]) / 2;
	pt_top_3d = (pts_3d[10 - 1] + pts_3d[13 - 1]) / 2;
	pt_bottom_2d = (pts_2d[11 - 1] + pts_2d[12 - 1]) / 2;
	pt_top_2d = (pts_2d[10 - 1] + pts_2d[13 - 1]) / 2;
	//遍历找到可见点
	k3 = 0;
	for (k3 = 0; k3 <= 1; k3 = k3 + 0.01)//采样间隔为0.01
	{
		Point2d pt_2d = pt_bottom_2d + k3 * (pt_top_2d - pt_bottom_2d);//当前点2d坐标
		Point3d pt_3d = pt_bottom_3d + k3 * (pt_top_3d - pt_bottom_3d);//当前点2d坐标
		//判断这个点是否可见
		if (whether_vis(src_width, src_height, ROWS, COLS, pt_2d, pt_3d, datas_3d, 0.5f, 1))
			break;
	}
	k = min(min(k1, k2), k3);//k为三个中最小的，即为最接近bottom的点
	if (abs(k - 1) < 1e-3)
	{
		frame[8] = Point2d(0., 0.);
		frame[9] = Point2d(0., 0.);
	}
	else
	{
		frame[10 - 1] = pt_top_2d;
		frame[9 - 1] = pt_bottom_2d + k * (pt_top_2d - pt_bottom_2d);
	}
	/**************************************处理16-17-14-15,竖尾翼******************************************/
	//尾翼特殊对待
	//分别计算14-17,15-16，中间点连线的遮挡情况，然后查看那两个的k想接近作为遮挡情况的表示
	//处理14-17
	pt_bottom_3d = pts_3d[16];
	pt_top_3d = pts_3d[13];
	pt_bottom_2d = pts_2d[16];
	pt_top_2d = pts_2d[16];
	//遍历找到可见点
	k1 = 0;
	for (k1 = 0; k1 <= 1; k1 = k1 + 0.01)//采样间隔为0.01
	{
		Point2d pt_2d = pt_bottom_2d + k1 * (pt_top_2d - pt_bottom_2d);//当前点2d坐标
		Point3d pt_3d = pt_bottom_3d + k1 * (pt_top_3d - pt_bottom_3d);//当前点2d坐标
		//判断这个点是否可见
		if (whether_vis(src_width, src_height, ROWS, COLS, pt_2d, pt_3d, datas_3d, 0.5f, 1))
			break;
	}
	//处理15-16
	pt_bottom_3d = pts_3d[15];
	pt_top_3d = pts_3d[14];
	pt_bottom_2d = pts_2d[15];
	pt_top_2d = pts_2d[14];
	//遍历找到可见点
	k2 = 0;
	for (k2 = 0; k2 <= 1; k2 = k2 + 0.01)//采样间隔为0.01
	{
		Point2d pt_2d = pt_bottom_2d + k2 * (pt_top_2d - pt_bottom_2d);//当前点2d坐标
		Point3d pt_3d = pt_bottom_3d + k2 * (pt_top_3d - pt_bottom_3d);//当前点2d坐标
		//判断这个点是否可见
		if (whether_vis(src_width, src_height, ROWS, COLS, pt_2d, pt_3d, datas_3d, 0.5f, 1))
			break;
	}
	//处理中间
	pt_bottom_3d = (pts_3d[15] + pts_3d[16]) / 2;
	pt_top_3d = (pts_3d[13] + pts_3d[14]) / 2;
	pt_bottom_2d = (pts_2d[15] + pts_2d[16]) / 2;
	pt_top_2d = (pts_2d[13] + pts_2d[14]) / 2;
	//遍历找到可见点
	k3 = 0;
	for (k3 = 00; k3 <= 1; k3 = k3 + 0.01)//采样间隔为0.01
	{
		Point2d pt_2d = pt_bottom_2d + k3 * (pt_top_2d - pt_bottom_2d);//当前点2d坐标
		Point3d pt_3d = pt_bottom_3d + k3 * (pt_top_3d - pt_bottom_3d);//当前点2d坐标
		//判断这个点是否可见
		if (whether_vis(src_width, src_height, ROWS, COLS, pt_2d, pt_3d, datas_3d, 0.5f, 1))
			break;
	}
	k = min(min(k1, k2), k3);//k为三个中最小的，即为最接近bottom的点
	if (abs(k - 1) < 1e-3)
	{
		frame[10] = Point2d(0., 0.);
		frame[11] = Point2d(0., 0.);
	}
	else
	{
		frame[11] = pt_top_2d;
		frame[10] = pt_bottom_2d + k * (pt_top_2d - pt_bottom_2d);
	}





}

void re_proj::show_frame(InputArray img)
{
	//展示frame
	Mat image;
	img.copyTo(image);
	for (int i = 0; i < 6; i++)
	{
		line(image, Point(int(frame[2 * i].x), int(frame[2 * i].y)), Point(int(frame[2 * i + 1].x), int(frame[2 * i + 1].y)), Scalar(0, 0, 255));
	}
	imshow("frame", image);
	waitKey(0);
}

vector<Point2d> re_proj::reproj_3d_bbox(Mat R, Mat T, InputArray img, double focal, double dx, int ROWS, int COLS, int index,string bbox_3d_path)
{
	//we first to load the 3d bbox
	ifstream keypts;
	keypts.open(bbox_3d_path);
	vector<double> minmax_3d;
	string data;
	for (int i = 0; i < 6; i++)
	{
		keypts >> data;
		minmax_3d.push_back(atof(data.c_str())*0.0254);
	}
	//then we cal the 3d bbox coord
	vector<Point3d> bbox_3d(8);
	for (int i = 0; i < 8; i++)
	{
		if (i % 2 == 0)
			bbox_3d[i].z = minmax_3d[5];//z_min
		else
			bbox_3d[i].z = minmax_3d[4];//z_max
		if (i<4)
			bbox_3d[i].x = minmax_3d[1];//x_min
		else
			bbox_3d[i].x = minmax_3d[0];//x_max
		if (i < 2)
			bbox_3d[i].y = minmax_3d[3];//y_min
		else if (i<4)
			bbox_3d[i].y = minmax_3d[2];//y_max
		else if (i<6)
			bbox_3d[i].y = minmax_3d[3];//y_min
		else
			bbox_3d[i].y = minmax_3d[2];//y_max
	}
	vector<Point2d> bbox_2d;
	//then we reproj the bbox
	for (int i = 0; i < 8; i++)
	{
		Point3d pts_i_camera = cal_pts_on_camera_cord(R, T, bbox_3d[i]);//将点坐标转到相机系
		//将点坐标转到图像系
		//将点转到图像系
		Point2d pts_pic_cord;
		pts_pic_cord.x = pts_i_camera.x / pts_i_camera.z * (focal) / dx + COLS / 2;
		pts_pic_cord.y = pts_i_camera.y / pts_i_camera.z * (focal) / dx + ROWS / 2;
		////画图
		//Mat img_copy;
		//img.copyTo(img_copy);
		//circle(img_copy, pts_pic_cord, 3, Scalar(0., 0., 255.));
		//putText(img_copy, to_string(i + 1), pts_pic_cord, FONT_HERSHEY_COMPLEX, 0.5, Scalar(0., 0., 255.), 0.5, 8);
		bbox_2d.push_back(pts_pic_cord);
		/*imshow("img_back", img_copy);
		waitKey(0);*/

	}
	return bbox_2d;
		

}
