#ifndef _LINE_H_
#define _LINE_H_

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
// Use of degrees is deprecated. Use radians instead.
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Geode.h"

class Line : public Geode
{
public:
	Line();
	~Line();

	glm::mat4 toWorld;

	void draw(GLuint);
	void draw(glm::mat4 C, GLint shaderProgram, glm::mat4 P, glm::mat4 V);
	void update();
	void spin(float);

	// These variables are needed for the shader program
	GLuint VBO, VAO;
	GLuint uProjection, uModel, uView;

	bool pressed = false;
};

// Define the coordinates and indices needed to draw the cube. Note that it is not necessary
// to use a 2-dimensional array, since the layout in memory is the same as a 1-dimensional array.
// This just looks nicer since it's easy to tell what coordinates/indices belong where.
const GLfloat vertices[2][3] = {
	{0.0, 0.0,  0.0}, {0.0, 0.0, -100.0}
};

#endif

