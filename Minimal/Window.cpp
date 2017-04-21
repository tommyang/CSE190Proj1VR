#include <random>
#include <stdlib.h>
#include "window.h"
#include "Model.h"
#include "Group.h"
#include "MatrixTransform.h"

const char* window_title = "GLFW Starter Project";
Model * factory;
Group * co2Group;
std::vector<MatrixTransform *> co2Group_mt;
MatrixTransform * co2_1_tr;
MatrixTransform * co2_2_tr;
MatrixTransform * co2_3_tr;
MatrixTransform * co2_4_tr;
MatrixTransform * co2_5_tr;
Group * o2Group;
Model * co2;
Model * o2;
GLint Window::currentShader;
GLint shaderProgram;

// On some systems you need to change this to the absolute path
#define VERTEX_SHADER_PATH "../shader.vert"
#define FRAGMENT_SHADER_PATH "../shader.frag"
#define VERTEX_SHADER2_PATH "../shader2.vert"
#define FRAGMENT_SHADER2_PATH "../shader2.frag"

// Default camera parameters
glm::vec3 cam_pos(0.0f, 0.0f, 20.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

int Window::width;
int Window::height;
std::default_random_engine generator;
time_t Window::last_co2_time;
glm::mat4 Window::P;
glm::mat4 Window::V;

void Window::initialize_objects()
{
	// cube = new Cube();
	// Load the shader program. Make sure you have the correct filepath up top
	// shaderProgram = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
	shaderProgram = LoadShaders(VERTEX_SHADER2_PATH, FRAGMENT_SHADER2_PATH);

	factory = new Model("C:/Users/tiyang/Desktop/CSE190Project1/assets/factory1/factory1.obj");
	co2 = new Model("C:/Users/tiyang/Desktop/CSE190Project1/assets/co2/co2.obj");
	o2 = new Model("C:/Users/tiyang/Desktop/CSE190Project1/assets/o2/o2.obj");

	co2Group = new Group();
	for (int i = 0; i < 5; i++) {
		create_co2();
	}
	last_co2_time = time(0);
}

void Window::create_co2() {
	std::uniform_real_distribution<float> plus_minus_one_dist(-1.0, 1.0);
	std::uniform_real_distribution<float> plus_one_dist(0.0, 1.0);
	MatrixTransform* mt = new MatrixTransform(glm::translate(glm::mat4(1.0f), glm::vec3(plus_minus_one_dist(generator) * 10.0f, plus_minus_one_dist(generator) * 10.0f, plus_one_dist(generator) * -10.0f)));
	mt->deg = plus_minus_one_dist(generator);
	mt->axis = glm::vec3(plus_minus_one_dist(generator), plus_minus_one_dist(generator), plus_minus_one_dist(generator));
	mt->move = glm::vec3(plus_minus_one_dist(generator) / 20.0f, plus_one_dist(generator) / 20.0f, plus_minus_one_dist(generator) / 20.0f); // upwards
	mt->scale(0.5f);
	mt->addChild(co2);
	co2Group->addChild(mt);
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
	// delete(cube);
	glDeleteProgram(shaderProgram);
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ // Because Apple hates comforming to standards
	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		fprintf(stderr, "Either GLFW is not installed or your graphics card does not support modern OpenGL.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
	Window::width = width;
	Window::height = height;
	// Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!
	glViewport(0, 0, width, height);

	if (height > 0)
	{
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

void Window::idle_callback()
{
	// Call the update function the cube
	// cube->update();
	co2Group->update();
}

void Window::display_callback(GLFWwindow* window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use the shader of programID
	glUseProgram(shaderProgram);
	currentShader = shaderProgram;

	time_t cur_time = time(0);
	double seconds = difftime(cur_time, last_co2_time);
	if ( seconds >= 1.0) {
		Window::last_co2_time = cur_time;
		create_co2();
	}

	// light
	glm::vec3 pointLightPosition;
	pointLightPosition = glm::vec3(10.0f, 10.0f, 5.0f);
	glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[0].position"), pointLightPosition.x, pointLightPosition.y, pointLightPosition.z);
    glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[0].ambient"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[0].diffuse"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[0].specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "pointLight[0].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "pointLight[0].linear"), 0.2f); // 0.09
    glUniform1f(glGetUniformLocation(shaderProgram, "pointLight[0].quadratic"), 0.032f); // 0.032

    pointLightPosition = glm::vec3(10.0f, -10.0f, 5.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[1].position"), pointLightPosition.x, pointLightPosition.y, pointLightPosition.z);
    glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[1].ambient"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[1].diffuse"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[1].specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "pointLight[1].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "pointLight[1].linear"), 0.2f); // 0.09
    glUniform1f(glGetUniformLocation(shaderProgram, "pointLight[1].quadratic"), 0.0f); // 0.032

    pointLightPosition = glm::vec3(-10.0f, 10.0f, 5.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[2].position"), pointLightPosition.x, pointLightPosition.y, pointLightPosition.z);
    glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[2].ambient"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[2].diffuse"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[2].specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "pointLight[2].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "pointLight[2].linear"), 0.2f); // 0.09
    glUniform1f(glGetUniformLocation(shaderProgram, "pointLight[2].quadratic"), 0.0f); // 0.032

    pointLightPosition = glm::vec3(-10.0f, -10.0f, 5.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[3].position"), pointLightPosition.x, pointLightPosition.y, pointLightPosition.z);
    glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[3].ambient"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[3].diffuse"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "pointLight[3].specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "pointLight[3].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "pointLight[3].linear"), 0.2f); // 0.09
    glUniform1f(glGetUniformLocation(shaderProgram, "pointLight[3].quadratic"), 0.0f); // 0.032

	// Render the cube
	// cube->draw(shaderProgram);
	factory->draw(shaderProgram);
	co2Group->draw(glm::mat4(1.0f));

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press
	if (action == GLFW_PRESS)
	{
		// Check if escape was pressed
		if (key == GLFW_KEY_ESCAPE)
		{
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}
}