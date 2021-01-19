//this code is used to make vis data for airplane pose estimation
#include "gl_part.h"
#include "pose_given.h"
#include <iostream>
#include "re_proj.h"
#include "config_load.h"
using namespace std;
int main()
{
	/****************************************************** some fucking config *********************************************************************************/
	string path_now = "..\\";//the main path
	string store_path = path_now + "vis_data";//output path of data
	string config_path = path_now + "config";//the config of the data,include the pose set,the model of aircraft
	//load the config of the img,which are width,height,focal,dx/dy
	vector<double> img_size=load_img_config(config_path+"\\img_set.config");
	string model_path = config_path + "\\engine_filled-n-37.5.obj";
	/****************************************************** init some fucking thing ********************************************************************************/
	pose_given pose_given_my;
	gl_part gl_part_my;
	gl_part_my.create_win("fxxk", int(img_size[0]), int(img_size[1]), true,model_path);
	re_proj re_proj_my(config_path+"\\key_pts_cords.config");
	/********************************************************* begin generate the vis data,the main process ************************************************************************************/
	//we interactive load the pose from the pose file and render the vis_data
	vector<double> pose_para;//init the pose para
	ifstream pose_set;
	pose_set.open(config_path+"\\pose_para.config");
	string data;
	pose_set >> data;//the first num in the pose_set file is the total num of the vis_data
	int total_data = atoi(data.c_str());
	int index = 0;
	for (int i = 0; i < total_data; i++)
	{
		pose_para.clear();//clear the pose para
		pose_set >> data;
		index = atoi(data.c_str());
		cout << "generate " << to_string(index) << " data " <<" of "<<total_data<< endl;
		//set the pose para in this inter
		for (int count = 0; count < 6; count++)
		{
			pose_set >> data;
			pose_para.push_back(atof(data.c_str()));
		}
		//set
		pose_given_my.set_rot(pose_para[0], pose_para[1], pose_para[2]);//
		pose_given_my.set_tran(pose_para[3], pose_para[4], pose_para[5]);
		pose_given_my.cal_model();
		/****************************** render the img  *******************************************************/
		//we first render to get the 2d rendered img
		gl_part_my.render_pic(config_path+"\\shader_model.fs", config_path+"\\shader_model.vs", pose_given_my.model_mat_gl, img_size[2], img_size[1], img_size[0], pose_para[5], img_size[3], index);//the rendered pic is in gl_part_my.pic_8uc3
		Mat render_data;//this is the rendered img
		gl_part_my.pic_8uc3.copyTo(render_data);
		//then we get the 3d data
		Mat rendered_3d_data;//this is the rendered 3d data ,which is correspondent to the rendered img
		gl_part_my.get_3d_pts(config_path + "\\2model.fs", config_path + "\\2model.vs", pose_given_my.model_mat_gl, img_size[2], img_size[1], img_size[0], pose_para[5], img_size[3], rendered_3d_data);//
		//then we get the mask,bbox of the whole plane and give the rendered img the background
		int rand_num = rand() % 533 + 1;//random select the background img
		Mat back_ground;
		vector<int> bbox = {  int(img_size[0]), int(img_size[1]),0,0 };//the bounding box
		back_ground = imread(config_path+"\\background\\"+to_string(rand_num)+".png");//select the bg img
		resize(back_ground, back_ground, Size(img_size[0], img_size[1]));//resize it to the setteled size
		Mat img_with_backgroud;
		Mat mask_aero(img_size[1], img_size[0],CV_8UC1);
		gl_part_my.pic_8uc3.copyTo(img_with_backgroud);
		for (int x = 0; x < img_size[0]; x++)
		{
			for (int y = 0; y < img_size[1]; y++)
			{
				if (rendered_3d_data.at<Vec4f>(y, x)(3) == 0)
				{
					img_with_backgroud.at<Vec3b>(y, x) = back_ground.at<Vec3b>(y, x);
					mask_aero.at<uchar>(y, x) = 1;
				}
				else//process to get the bbox
				{
					if (x < bbox[0])
						bbox[0] = max(0,x - 15);//i enlarge the bbox by 15 pixel
					if (x > bbox[2])
						bbox[2] = min(x + 15, int(img_size[0]));
					if (y < bbox[1])
						bbox[1] = max(0,y - 15);
					if (y > bbox[3])
						bbox[3] = min(y + 15, int(img_size[1]));
				}
			}
		}
		//now ,we reproj to get the kpts's pixel																																														 //reproj to get the kpts coord
		re_proj_my.back_proj(pose_given_my.rotation_mat_back, pose_given_my.translation_mat_back, gl_part_my.pic_8uc3, img_size[2], img_size[3], img_size[1], img_size[0], index);//repoj, the kpst is stored in re_proj_my.pts_2d
		re_proj_my.get_vis_data(rendered_3d_data, img_size[1], img_size[0]);//we got a vector to give whether the each kpts is visuable
		//now, we get the 3d projection of 3d_bbox
		vector<Point2d> bbox_3d_proj=re_proj_my.reproj_3d_bbox(pose_given_my.rotation_mat_back, pose_given_my.translation_mat_back, gl_part_my.pic_8uc3, img_size[2], img_size[3], img_size[1], img_size[0], index,config_path+"\\3d_bbox.config");
		//re_proj_my.show_vis_result(img_with_backgroud, index);//to show the final result
		/************************************** now ,we store the data  *******************************************/
		//we first store the 2d and 3d img
		imwrite(store_path+"\\"+to_string(index)+"_2d_img.jpg",img_with_backgroud);
		store_the_3d_data(rendered_3d_data, store_path + "\\" + to_string(index) + "_3d_img.3d");
		//then we store the mask
		imwrite(store_path + "\\" + to_string(index) + "_mask.jpg", mask_aero);
		//then we store the R,T,focal,dx,img_width,img_height,bbox,and kpts(notice we also store the whether the kpts is visable)
		ofstream data_store;
		string name_data;
		name_data = store_path +"\\"+ to_string(index) + "_2d_datas";
		data_store.open(name_data);
			//the R
		data_store << "rotation" << endl;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
				data_store << pose_given_my.rotation_mat_back.at<double>(i, j)<<"\t";
			data_store << endl;
		}
			//the T
		data_store << "translation" << endl;
		for (int i = 0; i < 3; i++)
		{
			data_store << pose_given_my.translation_mat_back.at<double>(i) << "\t";
		}
		data_store << endl;
			//focal,dx,img_width,img_height
		data_store << "focal, dx, img_width and img_height" << endl;
		for (int i = 0; i < 4; i++)
		{
			data_store << img_size[i] << "\t";
		}
		data_store << endl;
			//bbox
		data_store << "2d bbox" << endl;
		for (int i = 0; i < 4; i++)
		{
			data_store << bbox[i] << "\t";
		}
		data_store << endl;
			//3d bbox
		data_store << "3d bbox" << endl;
		for (int i = 0; i < 8; i++)
		{
			data_store << bbox_3d_proj[i].x << "\t" << bbox_3d_proj[i].y  << endl;
		}
		data_store << "kpts" << endl;
			//kpts
		for (int i = 0; i < re_proj_my.pts_2d.size(); i++)
		{
			data_store << re_proj_my.pts_2d[i].x << "\t" << re_proj_my.pts_2d[i].y << "\t" << re_proj_my.vis_data[i] << "\t" << endl;
		}
		data_store.close();
		
	}
	cout << "generate done!";
	
}