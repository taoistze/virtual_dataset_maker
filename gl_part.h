#pragma once
#ifndef __GL_PART_H__
#define __GL_PART_H__

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>//ʹ����Ԫ���Ŀ�
#include <glm/gtx/quaternion.hpp>

#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;
#include "shader_m.h"//��ɫ����
#include "model.h"//ģ�ͼ�����
#include "light.h"

class gl_part//opengl��Ⱦģ�Ͳ���
{
public:
	glm::mat4 model_mat;
	glm::mat4 projection_mat;
	GLFWwindow* window;//opengl����ָ��
	gl_part() ;//���캯��
	//void load_model(string model_path);
	int create_win(string win_name, int src_width, int src_height,bool src, string model_path);//��������
	//GLFWframebuffersizefun framebuffer_size_callback(GLFWwindow* window, int width, int height);//���ڴ�С�ص�����
	void load_model(string model_path);
	Model model_my;//��Ա����
	void render_pic(string fs_path, string vs_path, Mat view_mat_cv, double focal, int SCR_height, int SCR_width, double depth, double cellsize,int index);//��Ⱦ����
	void set_model_mat(Mat model_mat);
	void cal_projectionmat(double focal, int SCR_height, int SCR_width, float znear,float zfar, double cellsize);//����projection mat
	void get_3d_pts(string fs_path, string vs_path, Mat view_mat_cv, double focal, int SCR_height, int SCR_width, double depth, double cellsize,OutputArray datas_3d);//���ͼ���Ӧ��3D������꣬�Ӷ�Ϊ�����ѡ�ɼ����ṩ����
	Mat pic_8uc3;
	
};

#endif