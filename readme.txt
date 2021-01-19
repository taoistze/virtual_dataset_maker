//this coe is used to generate the vis data for training the pose estimation model
//the code is implemented through c++ opencv opengl
//the copyright belongs to runze_fan, taoistze@163.com

1. Framework
the code is in the visual_dataset_maker file
the generated data is in the vis_data file
and the config needed in the project is in the config file

2.How to use this code 
first change the configs in the config file, then run this code in debug(i highly recommend you do not run the code in release, it may cause some fucking diffuse mistakes)

3.Detailed description of the config
img_set.config: in this file, you can set the image width and height, focal and dx of the camera
3d_bbox.config: in this file ,you give the 3d bbox of the object by giving the max and min coordnite, the order is x_max,x_min,y_max,y_min,z_max,z_min
pose_para.config: in this file ,you set the pose of the vis_data to be generated; first, you give the total num of the data you will generate, then in each line, you first give the index of the data and then give 
the pose by euler(ang_x,ang_y,ang_z) and t(tx,ty,tz)
key_pts_cords.config: in this file, you give the keypoints' 3d coordinates which will be projection to the img (we finally give the 2d coordinate the these kpts)
background file: in this file, we select 553 back ground img, these images will use as the background of the rendered img

4.Output of the code
the output is in the vis_data file, and it contain 4 part:
first: 2d_img.jpg, it is the rendered img
seconf: mask.jpg, it is the mask of the img
third: 3d_img.jpg, it is a file contain the 3d coordinate of the 2d_img, and in each tuple, it is a vector<double>(4), which is the 3d coordiante of this pixel and bool_value(whether this pixel is foreground or background)
		i also write a fun to laod this 3d_img.3d: void load_the_3d_data(string path, OutputArray data_3d), this fun take the file name as input and output a mat contain the data(you can see it by img_watch), you can directly call this fun
		this fun is implemented in the config_load.cpp
fourth: it contain the 2d data: rotation mat, translation mat,focal, dx, img_width, img_height,2d bbox,3d bbox and kpts

5. How to config the enviroment to run this code 
first, you need to config the enviroment of opencv, it is very easy
then, you need to config the enviroment of opengl, you can refer LearnOpengl, it is very helpful
third, do not forget to config these enviroment in you project

6. Some advice
if you fell it is very slow to generate the code, it is because the terrible efficiency of the fun to generate the 3d_img.3d writing by me and i do not optimize this fun actually. So if you do not need the 3d data, i highly recommend you to comment it out
To be specific, comment the line 95 of main.cpp: store_the_3d_data(rendered_3d_data, store_path + "\\" + to_string(index) + "_3d_img.3d");
Also, do not forget to put your in the config file ,and change the model_path in the code,you need to change the model in main.cpp(line 16) to our model path

if you have any trouble in config opengl or your project, please contact me 