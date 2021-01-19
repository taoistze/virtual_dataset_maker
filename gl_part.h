#pragma once
#ifndef __GL_PART_H__
#define __GL_PART_H__

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>//使用四元数的库
#include <glm/gtx/quaternion.hpp>

#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;
#include "shader_m.h"//着色器类
#include "model.h"//模型加载类
#include "light.h"

class gl_part//opengl渲染模型部分
{
public:
	glm::mat4 model_mat;
	glm::mat4 projection_mat;
	GLFWwindow* window;//opengl窗口指针
	gl_part() ;//构造函数
	//void load_model(string model_path);
	int create_win(string win_name, int src_width, int src_height,bool src, string model_path);//创建窗口
	//GLFWframebuffersizefun framebuffer_size_callback(GLFWwindow* window, int width, int height);//窗口大小回调函数
	void load_model(string model_path);
	Model model_my;//成员对象
	void render_pic(string fs_path, string vs_path, Mat view_mat_cv, double focal, int SCR_height, int SCR_width, double depth, double cellsize,int index);//渲染函数
	void set_model_mat(Mat model_mat);
	void cal_projectionmat(double focal, int SCR_height, int SCR_width, float znear,float zfar, double cellsize);//计算projection mat
	void get_3d_pts(string fs_path, string vs_path, Mat view_mat_cv, double focal, int SCR_height, int SCR_width, double depth, double cellsize,OutputArray datas_3d);//获得图像对应的3D点的坐标，从而为下面的选可见点提供数据
	Mat pic_8uc3;
	
};

#endif