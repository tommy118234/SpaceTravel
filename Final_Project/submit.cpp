/*********************************************************
FILE : submit.cpp (csci3260 2016-2017 Final Project)
*********************************************************/
/*********************************************************
Student Information
Student ID:11550 33056
Student Name:TSUI Wai Yin
*********************************************************/

#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include "Dependencies\glm\glm.hpp"
#include "Dependencies\glm\gtc\matrix_transform.hpp"
#include "Dependencies\glm\gtc\type_ptr.hpp"
#include "Dependencies\SOIL\src\SOIL.h"
#include "camera.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
using glm::vec3;
using glm::mat4;
Camera camera(glm::vec3(0.0f, 5.0f, 30.0f));
GLint programID;
GLint Planet_programID;
GLint LightBox_programID;
GLint Instance_programID;
GLint Skybox_programID;
GLint Jeep_programID;
GLfloat ScreenWidth = 800.0, ScreenHeight = 600.0;
GLuint Texture[10];
GLuint drawSize[10];
//Define Vao, Vbo and Interaction parameters
GLuint planet_vao;
GLuint jeep_vao;
GLuint lightbox_vao;
GLuint skybox_vao;
GLuint instance_vao;
//VBO size not related to No. of Objects to Render
GLuint planet_vbo[3];
GLuint jeep_vbo[3];
GLuint lightbox_vbo[3];
GLuint skybox_vbo[1];
GLuint instance_vbo[3];
// Light control Setup
float diffuse_delta = 0.05f;
int diffuse_press_num = 0;
float specular_delta = 0.05f;
int specular_press_num = 0;
// Mouse Movement Setup
int prev_mouse_X = 0;
int prev_mouse_Y = 0;
bool first_time = TRUE;
float a_delta = 0.1f;
float a_press_num = 0.0;
float b_delta = 0.1f;
float b_press_num = 0.0;
int move_flag = TRUE;
GLint deltaX = 0;
GLint deltaY = 0;
//Skybox Setup
GLuint space_cubemapTexture;
GLuint sea_cubemapTexture;
//Instance Rendering Setup
mat4* modelMatrix;
GLuint amount = 1000;
// Switch View Point Setup
float camera_x = 0;
float camera_y = 5;
float camera_z = 30;
float offset_x = a_delta*a_press_num;
float offset_y = b_delta*b_press_num;
float offset_z = 0;
mat4 ViewMatrix = glm::lookAt(vec3(camera_x, camera_y, camera_z), // Camera Position in World Space
	vec3(0, 0, 0), // looks at the origin
	vec3(0, 1, 0)  // Head is up,(0,-1,0) to look upside-down)
);
// Lightbox movement along Z axis Setup
vec3 Ambient(0.5f, 0.5f, 0.5f);
vec3 lightposition(0.0f, 10.0f, -50.0f);
vec3 eyeposition(0.0f, 5.0f , 10.0f);
vec3 orientation(0.0f, 1.0f, 0.0f);
//  Travel Movement Setup
bool travel_flag = false;
vec3 start(-5.0f, 0.0f, 5.0f);
vec3 finish(13.0f, 5.0f, 5.0f);
float speed = 0.005;
vec3 motion = (finish - start)*speed;
float f_count = 0;
float accel = 1;
GLfloat skyboxVertices[] =
{
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	1.0f, -1.0f,  1.0f
};

GLuint loadCubemap(vector<const GLchar*> faces)
{
	int width, height;
	unsigned char* image;
	GLuint textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < faces.size(); i++)
	{
		image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	return textureID;
}
bool checkStatus(
	GLuint objectID,
	PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
	PFNGLGETSHADERINFOLOGPROC getInfoLogFunc,
	GLenum statusType)
{
	GLint status;
	objectPropertyGetterFunc(objectID, statusType, &status);
	if (status != GL_TRUE)
	{
		GLint infoLogLength;
		objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
		cout << buffer << endl;

		delete[] buffer;
		return false;
	}
	return true;
}
bool checkShaderStatus(GLuint shaderID)
{
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}
bool checkProgramStatus(GLuint programID)
{
	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}
string readShaderCode(const char* fileName)
{
	ifstream meInput(fileName);
	if (!meInput.good())
	{
		cout << "File failed to load..." << fileName;
		exit(1);
	}
	return std::string(
		std::istreambuf_iterator<char>(meInput),
		std::istreambuf_iterator<char>()
	);
}
GLuint installShaders(string VertexShader, string FragmentShader)
{
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);	
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);	
	const GLchar* adapter[1];
	string temp = readShaderCode(VertexShader.c_str());	
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode(FragmentShader.c_str());	
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);
	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);
	programID = glCreateProgram();
	if (!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID))
		return programID;	
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);	
	return programID;	
}

void keyboard(unsigned char key, int x, int y)
{	
	if (key == 'a')
	{
		camera_x = 40;
		camera_y = 0;
		camera_z = 0;
		offset_x = 0;
		offset_y = 0;
		offset_z = 0;
		orientation= vec3(0.0f, 1.0f, 0.0f);
	}
	if (key == 's')
	{
		camera_x = 0;
		camera_y = 40;
		camera_z = 0;
		offset_x = 0;
		offset_y = 0;
		offset_z = 0;
		orientation = vec3(0.0f, 0.0f, -1.0f);
	}					 
	if (key == 'd')		 
	{					 
		camera_x = 20;	 
		camera_y = 20;	 
		camera_z = 20;	 
		offset_x = 0;
		offset_y = 0;
		offset_z = 0;
		orientation = vec3(-1.0f, 1.0f, -1.0f);
	}					 
	if (key == 'z')		 
	{					 
		camera_x = 0;	 
		camera_y = 5;	 
		camera_z = 30; 	 
		offset_x = 0;
		offset_y = 0;
		offset_z = 0;
		orientation = vec3(0.0f, 1.0f, 0.0f);
	}
	if (key == 'o')
	{		
		diffuse_delta -= 0.05;
	}
	if (key == 'p')
	{		
		diffuse_delta += 0.05;
	}
	if (key == 'n')
	{
		
		specular_delta -= 0.05;
	}
	if (key == 'm')
	{		
		specular_delta += 0.05;
	}
	if (key == 'T')
	{
		travel_flag = !travel_flag;
		start = vec3(+5.0f, 0.0f, 5.0f);
		f_count = 0;
		motion = (finish - start)*speed;
	}
	if (key == 27)exit(0);	
}
void PassiveMouse(int x, int y)
{
	if (first_time) {	
		first_time = FALSE;
		prev_mouse_X = x;
		prev_mouse_Y = y;
		return;
	}
	 deltaX = prev_mouse_X - x;
	 deltaY = prev_mouse_Y - y;
	 if (deltaX < -0.01f) { a_press_num += 10; }
	 if (deltaX >  0.01f) { a_press_num -= 10; }
	 if (deltaY < -0.01f) { b_press_num -= 10; }
	 if (deltaY >  0.01f) { b_press_num += 10; }	
	first_time = TRUE;
}
void wheel(int mouse_wheel,int direction,int x,int y)
{
	if (direction > 0) offset_z += 0.5;
	else offset_z -= 0.5;	
}
void movecar(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		accel += 0.01;
		motion = motion*accel;
		f_count += 1;
		break;
	case GLUT_KEY_DOWN:
		accel -= 0.01;
		motion = motion*accel;		
		f_count -= 1;
		break;		
	}
}

bool loadOBJ(const char * path,	std::vector<glm::vec3> & out_vertices,	std::vector<glm::vec2> & out_uvs,std::vector<glm::vec3> & out_normals) {
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;
	
	FILE * file = fopen(path, "r");
	if (file == NULL) {
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

				   // else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for (unsigned int i = 0; i<vertexIndices.size(); i++) {

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];		
		unsigned int uvIndex = uvIndices[i];		
		unsigned int normalIndex = normalIndices[i];		

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);

	}

	return true;
}
GLuint loadBMP_custom(const char * imagepath) {
	printf("Reading image %s\n", imagepath);
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;
	unsigned char * data;

	FILE * file = fopen(imagepath, "rb");
	if (!file) { printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar(); return 0; }

	if (fread(header, 1, 54, file) != 54) {
		printf("Not a correct BMP file\n");
		return 0;
	}
	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return 0;
	}
	if (*(int*)&(header[0x1E]) != 0) { printf("Not a correct BMP file\n");    return 0; }
	if (*(int*)&(header[0x1C]) != 24) { printf("Not a correct BMP file\n");    return 0; }

	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);
	if (imageSize == 0)    imageSize = width*height * 3; 
	if (dataPos == 0)      dataPos = 54; 

	data = new unsigned char[imageSize];
	fread(data, 1, imageSize, file);
	fclose(file);
	//Create one OpenGL texture and set the texture parameter 	
	GLuint textureID;
	glGenTextures(1, &textureID);
	//"Bind" new texture:all future texture function will modify this
	glBindTexture(GL_TEXTURE_2D, textureID);
	//Give Image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	//OpenGL has now copied the data. Free local data space
	delete[] data;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	return textureID;
}
void send_planet_data_to_gl() {		
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res = loadOBJ("planet.obj", vertices, uvs, normals);
	drawSize[0] = vertices.size();
	//Create VAO [Planet]
	glGenVertexArrays(1, &planet_vao);	
	glBindVertexArray(planet_vao);
	//Create VBO [Planet]
	glGenBuffers(3, planet_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, planet_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, planet_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, planet_vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);	
}
void send_jeep_data_to_gl() {	
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res = loadOBJ("jeep.obj", vertices, uvs, normals);
	drawSize[1] = vertices.size();
	//Create VAO [Jeep]			
	glGenVertexArrays(1, &jeep_vao);
	glBindVertexArray(jeep_vao);
	//Create VBO [Jeep]
	glGenBuffers(3, jeep_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, jeep_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, jeep_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, jeep_vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
}
void send_lightbox_data_to_gl() {	
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res = loadOBJ("box.obj", vertices, uvs, normals);
	drawSize[2] = vertices.size();
	//Create VAO [LightBox]		
	glGenVertexArrays(1, &lightbox_vao);
	glBindVertexArray(lightbox_vao);
	//Create VBO [LightBox]
	glGenBuffers(3, lightbox_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, lightbox_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, lightbox_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, lightbox_vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
}
void send_instance_data_to_gl() {	
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res = loadOBJ("rock.obj", vertices, uvs, normals);
	drawSize[3] = vertices.size();
	//Create VAO [Instance]		
	glGenVertexArrays(1, &instance_vao);
	glBindVertexArray(instance_vao);
	//Create VBO [Instance]
	glGenBuffers(3, instance_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, instance_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, instance_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, instance_vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
}
void send_skybox_data_to_gl() {	
	glGenVertexArrays(1, &skybox_vao);
	glGenBuffers(1, skybox_vbo);
	glBindVertexArray(skybox_vao);
	glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER,  sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);
}
void CreateRand_Model() 
{
	modelMatrix = new glm::mat4[amount];
	srand(glutGet(GLUT_ELAPSED_TIME));
	GLfloat radius = 18.0f;
	GLfloat offset = 0.8f;
	GLfloat displacement;
	for (GLuint i = 0; i < amount; i++)
	{
		mat4 model;
		GLfloat angle = (GLfloat)i / (GLfloat)amount * 360.0f;
		displacement = (rand() % (GLint)(2 * offset * 200)) / 100.0f - offset;
		GLfloat x = sin(angle) * radius + displacement;
		displacement = (rand() % (GLint)(2 * offset * 200)) / 100.0f - offset;
		GLfloat y = displacement * 0.6f + 1;
		displacement = (rand() % (GLint)(2 * offset * 200)) / 100.0f - offset;
		GLfloat z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x , y, z));
		GLfloat scale = (rand() % 10) / 70.0f + 0.07;
		model = glm::scale(model, glm::vec3(scale));		
		modelMatrix[i] = model;
	}
}
GLuint load_skybox_space() {
	vector<const GLchar*> space_faces;
	space_faces.push_back("skybox/space/right.bmp");
	space_faces.push_back("skybox/space/left.bmp");
	space_faces.push_back("skybox/space/top.bmp");
	space_faces.push_back("skybox/space/bottom.bmp");
	space_faces.push_back("skybox/space/back.bmp");
	space_faces.push_back("skybox/space/front.bmp");
	return loadCubemap(space_faces);	
}
void load_texture() {
	Texture[0] = loadBMP_custom("earth.bmp");
	Texture[1] = loadBMP_custom("earth_normal.bmp");
	Texture[2] = loadBMP_custom("jeep_texture.bmp");
	Texture[3] = loadBMP_custom("stone_texture.bmp");
	Texture[4] = loadBMP_custom("stone_normal.bmp");
	Texture[5] = loadBMP_custom("brickwall_texture.bmp");
	Texture[6] = loadBMP_custom("box.bmp");
	Texture[7] = loadBMP_custom("jupiter.bmp");
	space_cubemapTexture = load_skybox_space();	
}
void sendDataToOpenGL()
{	// Send Planet geometry data to OpenGL
	send_planet_data_to_gl();
	// Send Jeep geometry data to OpenGL
	send_jeep_data_to_gl();
	// Send LightBox geometry data to OpenGL
	send_lightbox_data_to_gl();
	// Send SkyBox data to OpenGL
	send_skybox_data_to_gl();
	// Send Instance(Stones) data to OpenGL
	send_instance_data_to_gl();
	// Load all textures
	load_texture();
	CreateRand_Model();
}
void draw_planet(mat4 Projection, mat4 ViewMatrix) {
	glUseProgram(Planet_programID);
	GLint AmbientLocation = glGetUniformLocation(Planet_programID, "ambientLight");	
	glUniform3fv(AmbientLocation, 1, &Ambient[0]);
	GLint lightpositionLocation = glGetUniformLocation(Planet_programID, "lightposition");	
	glUniform3fv(lightpositionLocation, 1, &lightposition[0]);	
	GLint eyepositionLocation = glGetUniformLocation(Planet_programID, "eyeposition");	
	glUniform3fv(eyepositionLocation, 1, &eyeposition[0]);
	GLint diffusecontrolLocation = glGetUniformLocation(Planet_programID, "diffuse_brightness_control");
	glUniform1f(diffusecontrolLocation, diffuse_delta);
	GLint specularcontrolLocation = glGetUniformLocation(Planet_programID, "diffuse_brightness_control");
	glUniform1f(specularcontrolLocation, specular_delta);
	//Bind different texture
	GLuint TextureID_0 = glGetUniformLocation(Planet_programID, "sampler");
	GLuint TextureID_1 = glGetUniformLocation(Planet_programID, "sampler2");
	//Draw First Planet	
	glBindVertexArray(planet_vao);
	mat4 Model = glm::translate(mat4(1.0f), vec3(15.0f, 5.0f, 5.0f));	
	mat4 Scale = glm::scale(Model, vec3(1.0f));
	mat4 Rotate = glm::rotate(mat4(), ((GLfloat)glutGet(GLUT_ELAPSED_TIME)*0.002f), vec3(0, 1, 0));
	mat4 Trans = Scale * Rotate;
	GLuint TransLocation = glGetUniformLocation(Planet_programID, "Trans");
	glUniformMatrix4fv(TransLocation, 1, GL_FALSE, &Trans[0][0]);
	glm::mat4 MVP = Projection * ViewMatrix * Scale * Rotate;
	GLuint MVPLocation = glGetUniformLocation(Planet_programID, "MVP");
	glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &MVP[0][0]);
	//Multiple Mapping for planet B(Mars)	
	glUniform1i(glGetUniformLocation(Planet_programID, "multiple_mapping_flag"), true);
	glUniform1i(glGetUniformLocation(Planet_programID, "normal_mapping_flag"), false);
	//Bind texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture[5]);
	glUniform1i(TextureID_0, 0);
	//Bind texture in Texture Unit 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Texture[7]);
	glUniform1i(TextureID_1, 1);
	glDrawArrays(GL_TRIANGLES, 0, drawSize[0]);
	//Draw Second Planet		
	Model = glm::translate(mat4(1.0f), vec3(-5.0f, 0.0f, 5.0f));
	Scale = glm::scale(Model, vec3(2.0f));
	Rotate = glm::rotate(mat4(), ((GLfloat)glutGet(GLUT_ELAPSED_TIME)*0.001f), vec3(0, 1, 0));
	Trans = Scale * Rotate;
	TransLocation = glGetUniformLocation(Planet_programID, "Trans");
	glUniformMatrix4fv(TransLocation, 1, GL_FALSE, &Trans[0][0]);
	MVP = Projection * ViewMatrix * Scale * Rotate;
	MVPLocation = glGetUniformLocation(Planet_programID, "MVP");
	glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &MVP[0][0]);
	//Normal Mapping for planet A(Earth)	
	glUniform1i(glGetUniformLocation(Planet_programID, "multiple_mapping_flag"), false);
	glUniform1i(glGetUniformLocation(Planet_programID, "normal_mapping_flag"), true);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, Texture[0]);
	glUniform1i(TextureID_0, 2);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, Texture[1]);
	glUniform1i(TextureID_1, 3);
	glDrawArrays(GL_TRIANGLES, 0, drawSize[0]);
	//Draw Third Planet	(Moon)	
	mat4 a_Model = glm::rotate(Model, ((GLfloat)glutGet(GLUT_ELAPSED_TIME)*0.001f), vec3(0, 1, 0));//around Second planet 
	a_Model = glm::translate(a_Model, vec3(-10.0f, 0.0f, 5.0f));	
	a_Model = glm::scale(a_Model, vec3(0.4f));	
	mat4 moon_Rotate = glm::rotate(mat4(), ((GLfloat)glutGet(GLUT_ELAPSED_TIME)*0.005f), vec3(0, 1, 0));//self-rotate
	Trans = a_Model*moon_Rotate;
	TransLocation = glGetUniformLocation(Planet_programID, "Trans");
	glUniformMatrix4fv(TransLocation, 1, GL_FALSE, &Trans[0][0]);
	MVP = Projection * ViewMatrix * a_Model * moon_Rotate;
	MVPLocation = glGetUniformLocation(Planet_programID, "MVP");
	glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &MVP[0][0]);	
	glUniform1i(glGetUniformLocation(Planet_programID, "multiple_mapping_flag"), false);
	glUniform1i(glGetUniformLocation(Planet_programID, "normal_mapping_flag"), false);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, Texture[3]);
	glUniform1i(TextureID_0, 4);
	glDrawArrays(GL_TRIANGLES, 0, drawSize[0]);
}	
void draw_jeep(mat4 Projection,mat4 ViewMatrix) {		
	glUseProgram(Jeep_programID);	
	GLint AmbientLocation = glGetUniformLocation(Jeep_programID, "ambientLight");	
	glUniform3fv(AmbientLocation, 1, &Ambient[0]);
	GLint lightpositionLocation = glGetUniformLocation(Jeep_programID, "lightposition");	
	glUniform3fv(lightpositionLocation, 1, &lightposition[0]);
	GLint eyepositionLocation = glGetUniformLocation(Jeep_programID, "eyeposition");	
	glUniform3fv(eyepositionLocation, 1, &eyeposition[0]);
	GLint diffusecontrolLocation = glGetUniformLocation(Jeep_programID, "diffuse_brightness_control");
	glUniform1f(diffusecontrolLocation, diffuse_delta);
	GLint specularcontrolLocation = glGetUniformLocation(Jeep_programID, "diffuse_brightness_control");
	glUniform1f(specularcontrolLocation, specular_delta);	
	GLuint TextureID_0 = glGetUniformLocation(Jeep_programID, "sampler");
	//Draw Jeep	
	glBindVertexArray(jeep_vao);	
	mat4 Model = glm::translate(mat4(1.0f), vec3(-5.0f, 0.0f, 5.0f));	
	mat4 a_Model;
	if (!travel_flag) {
		a_Model = glm::rotate(Model, ((GLfloat)glutGet(GLUT_ELAPSED_TIME)*0.003f), vec3(0, 1, 0));
		a_Model = glm::translate(a_Model, vec3(-5.0f, 0.0f, 5.0f));
	}	
	
	if (f_count == 1 / speed) {
		travel_flag = false;		
		Model = glm::translate(mat4(1.0f), vec3(15.0f, 5.0f, 5.0f));
		a_Model = glm::rotate(Model, ((GLfloat)glutGet(GLUT_ELAPSED_TIME)*0.004f), vec3(0, 1, 0));
		a_Model = glm::translate(a_Model, vec3(-2.0f, 0.0f, 5.0f));		
		
	}
	if (travel_flag) {	
		a_Model = glm::translate(Model, start);
		start += motion;		
		f_count += 1.0;
	}		
	a_Model = glm::scale(a_Model, vec3(0.5f));	
	mat4 Trans = a_Model;
	GLuint TransLocation = glGetUniformLocation(Jeep_programID, "Trans");
	glUniformMatrix4fv(TransLocation, 1, GL_FALSE, &Trans[0][0]);
	glm::mat4 MVP = Projection * ViewMatrix *  a_Model;
	GLuint MVPLocation = glGetUniformLocation(Jeep_programID, "MVP");
	glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &MVP[0][0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture[2]);
	glUniform1i(TextureID_0, 0);	
	glDrawArrays(GL_TRIANGLES, 0, drawSize[1]);
}
void draw_lightbox(mat4 Projection, mat4 ViewMatrix) {
	glUseProgram(LightBox_programID);
	GLint AmbientLocation = glGetUniformLocation(LightBox_programID, "ambientLight");	
	glUniform3fv(AmbientLocation, 1, &Ambient[0]);
	GLint lightpositionLocation = glGetUniformLocation(LightBox_programID, "lightposition");	
	glUniform3fv(lightpositionLocation, 1, &lightposition[0]);
	GLint eyepositionLocation = glGetUniformLocation(LightBox_programID, "eyeposition");	
	glUniform3fv(eyepositionLocation, 1, &eyeposition[0]);	
	GLuint TextureID_0 = glGetUniformLocation(LightBox_programID, "sampler");
	//Draw Light Box	
	glBindVertexArray(lightbox_vao);
	mat4 Model = glm::translate(mat4(1.0f), lightposition);
	glm::mat4 MVP = Projection * ViewMatrix *  Model;
	GLuint MVPLocation = glGetUniformLocation(LightBox_programID, "MVP");
	glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &MVP[0][0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture[6]);
	glUniform1i(TextureID_0, 0);
	glDrawArrays(GL_TRIANGLES, 0, drawSize[2]);
}
void draw_instance(mat4 Projection, mat4 ViewMatrix) {
	glUseProgram(Instance_programID);
	for (GLuint i =0; i < amount; i++)
	{	
		glm::mat4 Model = glm::translate(mat4(1.0f), vec3(15.0f, 5.0f, 5.0f));
		mat4 a_Model = glm::rotate(Model, ((GLfloat)glutGet(GLUT_ELAPSED_TIME)*0.001f), vec3(0, 1, 0));//around Second planet 
		a_Model = glm::translate(a_Model, vec3(0.0f, 0.0f, 0.0f));
		a_Model = glm::scale(a_Model, vec3(0.4f));		
		mat4 MVP = Projection * ViewMatrix * a_Model * modelMatrix[i];
		GLuint MVPLocation = glGetUniformLocation(Instance_programID, "MVP");
		glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &MVP[0][0]);		
		glBindVertexArray(instance_vao);
		GLuint TextureID_0 = glGetUniformLocation(Instance_programID, "sampler");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture[3]);
		glUniform1i(TextureID_0, 0);
		glDrawArrays(GL_TRIANGLES, 0, drawSize[3]);
	}
}
void draw_skybox() {			
	glDepthMask(GL_FALSE);	
	glUseProgram(Skybox_programID);
	//Draw Skybox	
	GLuint Skb_ModelLocation = glGetUniformLocation(Skybox_programID, "M");
	mat4 Skb_ModelMatrix = mat4(1.0f);
	glUniformMatrix4fv(Skb_ModelLocation, 1, GL_FALSE, &Skb_ModelMatrix[0][0]);
	// Remove Translation component of view matrix
	mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	
	mat4 projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(Skybox_programID, "view"), 1, GL_FALSE,glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(Skybox_programID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	// skybox cube
	glBindVertexArray(skybox_vao);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(Skybox_programID, "skybox"), 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, space_cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthMask(GL_TRUE);
}
void paintGL(void)
{	
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);	
	//Lighting and Projection and View setting
	offset_x = a_delta*a_press_num;
	offset_y = b_delta*b_press_num;
	ViewMatrix = glm::lookAt(vec3(camera_x+offset_x, camera_y + offset_y, camera_z+ offset_z), // Camera Position in World Space
		vec3(0 + offset_x, 0 + offset_y, 0), // looks at the origin
		orientation); // Head is up		
	mat4 Projection = glm::perspective(45.0f, ScreenWidth / ScreenHeight, 0.01f, 100.0f);	
	lightposition += vec3(0, 0, 0.05);
	eyeposition += vec3(0, 0, 0.05);	
	if (lightposition.z > 100.0f)
	{
		lightposition = vec3(0.0f, 10.0f, -50.0f);
		eyeposition = vec3(0.0f, 5.0f, 10.0f);
	}		
	draw_skybox();
	draw_planet(Projection,ViewMatrix);	
	draw_jeep(Projection, ViewMatrix);
	draw_lightbox(Projection, ViewMatrix);		
	draw_instance(Projection, ViewMatrix);
	glFlush();
	glutPostRedisplay();
}
void initializedGL(void) 
{
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);		
	sendDataToOpenGL();
}
int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);	
	//Create a window with title specified
	glutInitWindowSize(ScreenWidth,ScreenHeight);
	glutCreateWindow("Final Project");
	initializedGL();
	Planet_programID = installShaders("planet_vs.glsl", "planet_fs.glsl");
	Jeep_programID = installShaders("jeep_vs.glsl", "jeep_fs.glsl");
	LightBox_programID = installShaders("instance_vs.glsl", "instance_fs.glsl");
	Skybox_programID = installShaders("skybox_vs.glsl", "skybox_fs.glsl");
	Instance_programID = installShaders("instance_vs.glsl", "instance_fs.glsl");
	glutDisplayFunc(paintGL);	
	glutPassiveMotionFunc(PassiveMouse);
	glutKeyboardFunc(keyboard);
	glutMouseWheelFunc(wheel);
	glutSpecialFunc(movecar);
	glutMainLoop();
}