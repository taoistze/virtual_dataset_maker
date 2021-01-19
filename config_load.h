#pragma once
#ifndef __CONFIG_LOAD_H__
#define __CONFIG_LOAD_H__
#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;
vector<double> load_img_config(std::string img_config_file_path);
void store_the_3d_data(Mat data_3d, string path);//this fun is used to save the 3d data
void load_the_3d_data(string path,OutputArray data_3d);//this fun is used to load the 3d data
vector<double> data_to_vec(string data);//this code is used to tranform the data to vector

#endif