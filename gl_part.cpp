#include "gl_part.h"
//Model model_my;
gl_part::gl_part()//���캯��,��������glfw
{
	//��ʼ��glfw������
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
}

int gl_part::create_win(string win_name,int src_width,int src_height,bool scr,string model_path)
{
	window = glfwCreateWindow(src_width, src_height, win_name.c_str(), NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	//glfwSetFramebufferSizeCallback(window, gl_part::framebuffer_size_callback);
	// glad: ������Ӧ��ָ��
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	if (scr)
	{
		//֡����
		int maxFBWidth = src_width;
		int maxFBHeight = src_height;
		//frame buffer
		unsigned int osFrameBuffer;
		glGenFramebuffers(1, &osFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, osFrameBuffer);
		unsigned int osDepthBuffer;
		glGenRenderbuffers(1, &osDepthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, osDepthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA32F, maxFBWidth, maxFBHeight);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, osDepthBuffer);

		GLuint attachments[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(sizeof(attachments) / sizeof(*attachments), attachments);

		unsigned int osRBO;
		glGenTextures(1, &osRBO);
		glBindTexture(GL_TEXTURE_2D, osRBO);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, maxFBWidth, maxFBHeight, 0,
			GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
		);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, osRBO, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		if (glad_glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	}
	//����ģ��
	cout << "model is loading,please wait..." << endl;
	this->load_model(model_path);
	cout << "model loaded done" << endl;
}

void gl_part::render_pic(string fs_path, string vs_path,Mat view_mat_cv, double focal, int SCR_height, int SCR_width, double depth, double cellsize,int index)
{
	float znear = focal;
	float zfar = depth + 100;
	set_model_mat(view_mat_cv);
	cal_projectionmat(focal,SCR_height,SCR_width,znear,zfar,cellsize);
	//����ģ����ɫ��
	Shader shader_model(vs_path.c_str(), fs_path.c_str());
	//���ù���
	Light light_my;
	//��Ⱦ�������ã����ð�͸��,ע���޸�shader_model.fs�е�alphaֵ
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//����shader
	shader_model.use();
	//���ù���
	shader_model.setVec3("light.diffuse", light_my.diffuse);
	shader_model.setVec3("light.position", light_my.position);
	//��Ⱦѭ��
	glEnable(GL_DEPTH_TEST);
	// ��Ⱦ
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);//���ñ�����ɫ
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//������̬
	shader_model.setMat4("model", model_mat);
	shader_model.setMat4("projection", projection_mat);
	//��Ⱦģ��

	int screen_w;
	int screen_h;
	int pixel_w;
	int pixel_h;
	glfwGetWindowSize(window, &screen_w, &screen_h);
	glfwGetFramebufferSize(window, &pixel_w, &pixel_h);
	model_my.Draw(shader_model);//��ͼ
	//�����ݶ�����
	cv::Mat data_depth(screen_h, screen_w, CV_32FC4);//row,col
	glReadPixels(0, 0, screen_w, screen_h, GL_RGBA, GL_FLOAT, data_depth.data);
	cv::Mat data_resize(SCR_height,SCR_width, CV_32FC4);
	cv::resize(data_depth, data_resize, Size(SCR_width, SCR_height), 0, 0, INTER_LINEAR);
	//ע������ϵ��opengl����ϵ��ԭ�������½ǣ�x���ң�y����;cvϵ��ԭ�������Ͻǣ�x���ң�y����;opencv��ɫΪbgr��gl����ɫΪrgb
	cv::Mat pic_cv(SCR_height, SCR_width, CV_32FC3);
		//��������ͼ����������
	for (int x = 0; x < SCR_width; x++)
	{
		for (int y = 0; y < SCR_height; y++)
		{
			//pic_cv.at<Vec3f>(y,x) = Vec3f(data_resize.at<Vec4f>(SCR_height-y-1,x )[2], data_resize.at<Vec4f>(SCR_height - y - 1, x)[1], data_resize.at<Vec4f>(SCR_height - y - 1, x)[0]);
			pic_cv.at<Vec3f>(y, x) = Vec3f(data_resize.at<Vec4f>(y, x)[2], data_resize.at<Vec4f>(y, x)[1], data_resize.at<Vec4f>(y, x)[0]);
		}
	}
	pic_cv.convertTo(pic_8uc3,CV_8UC3,255);
}





void gl_part::set_model_mat(Mat model_mat_cv)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			model_mat[i][j] = model_mat_cv.at<double>(i, j);
		}
		//cout << endl;

	}
}
void gl_part::cal_projectionmat(double focal, int SCR_height, int SCR_width, float znear, float zfar, double cellsize)
{
	float fov = atan2((double)(SCR_height / 2) * cellsize, focal) * 2;
	//projection_mat= glm::perspective(glm::radians(fov), (float)(double)((double)(SCR_width/2) / (double)(SCR_height/2)), znear, zfar);
	projection_mat = glm::perspective((fov), (float)(double)((double)(SCR_width / 2) / (double)(SCR_height / 2)), znear, zfar);
	//for (int i = 0; i < 4; i++)
	//{
	//	for (int j = 0; j < 4; j++)
	//		//cout << projection_mat[i][j] << "	";
	//	//cout << endl;
	//}

}

void gl_part::load_model(string model_path)
{
	model_my.loadModel(model_path);
}

void gl_part::get_3d_pts(string fs_path, string vs_path, Mat view_mat_cv, double focal, int SCR_height, int SCR_width, double depth, double cellsize,OutputArray datas_3d)
{
	float znear = focal;
	float zfar = depth + 100;
	set_model_mat(view_mat_cv);
	cal_projectionmat(focal, SCR_height, SCR_width, znear, zfar, cellsize);
	//����ģ����ɫ��
	Shader shader_model(vs_path.c_str(), fs_path.c_str());
	//���ù���
	Light light_my;
	//��Ⱦ�������ã����ð�͸��,ע���޸�shader_model.fs�е�alphaֵ
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//����shader
	shader_model.use();
	//���ù���
	shader_model.setVec3("light.diffuse", light_my.diffuse);
	shader_model.setVec3("light.position", light_my.position);
	//��Ⱦѭ��
	glEnable(GL_DEPTH_TEST);
	// ��Ⱦ
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);//���ñ�����ɫ
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//������̬
	shader_model.setMat4("model", model_mat);
	shader_model.setMat4("projection", projection_mat);
	//��Ⱦģ��

	int screen_w;
	int screen_h;
	int pixel_w;
	int pixel_h;
	glfwGetWindowSize(window, &screen_w, &screen_h);
	glfwGetFramebufferSize(window, &pixel_w, &pixel_h);
	model_my.Draw(shader_model);//��ͼ
	//�����ݶ�����
	cv::Mat data_depth(screen_h, screen_w, CV_32FC4);//row,col
	glReadPixels(0, 0, screen_w, screen_h, GL_RGBA, GL_FLOAT, data_depth.data);
	resize(data_depth, data_depth, Size(SCR_width, SCR_height), 0, 0, INTER_LINEAR);
	data_depth.copyTo(datas_3d);
}