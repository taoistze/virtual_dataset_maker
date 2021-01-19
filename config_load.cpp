//this code is to load the config
#include "config_load.h"
vector<double> load_img_config(std::string img_config_file_path)
{
	ifstream i_file;
	i_file.open(img_config_file_path);
	std::vector<double> img_config;
	string data;
	i_file >> data;//load width
	img_config.push_back(atof(data.c_str()));
	i_file >> data;//load height
	img_config.push_back(atof(data.c_str()));
	i_file >> data;//load focal
	img_config.push_back(atof(data.c_str()));
	i_file >> data;//load dx/dy
	img_config.push_back(atof(data.c_str()));
	return img_config;
}

void store_the_3d_data(Mat data_3d, string path)
{
	Mat img;
	data_3d.copyTo(img);
	ofstream data_store;
	data_store.open(path);
	data_store << img.cols << "\t" << img.rows << endl;//first we store the img size
	//then we store the data, the 3d data in the same pixel,we seperate it with ",",different pixel with \t
	for (int x = 0; x < img.cols; x++)
	{
		for (int y = 0; y < img.rows; y++)
		{
			for (int k = 0; k < 4; k++)
				data_store << img.at<Vec4f>(y, x)(k) << ",";
			data_store << "\t";			
		}
		data_store << endl;
	}
	data_store.close();

}
void load_the_3d_data(string path, OutputArray data_3d)
{
	ifstream data_load;
	data_load.open(path);
	//first, we load the data size
	int img_width, img_height;
	string data;
	data_load >> data;
	img_width = atoi(data.c_str());
	data_load >> data;
	img_height = atoi(data.c_str());
	//then,we load the data
	cv::Mat data_depth(img_height, img_width, CV_32FC4);
	for (int x = 0; x < img_width; x++)
	{
		for (int y = 0; y < img_height; y++)
		{
			vector<float> fxxk;
			data_load>>data;
			char* p = (char*)data.data();
			char* pch;
			char* ptr = NULL;
			pch = strtok_s(p,",",&ptr);
			while (pch != NULL)
			{
				fxxk.push_back(atof(pch));
				pch = strtok_s(NULL, ",", &ptr);
			}
			for (int k = 0; k < 4; k++)
				data_depth.at<Vec4f>(y, x)(k) = fxxk[k];
		}
	}
	data_depth.copyTo(data_3d);

}


