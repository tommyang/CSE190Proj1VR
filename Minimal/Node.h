//
//  Node.h
//  CSE167Proj3
//
//  Created by Tommy Yang on 11/2/16.
//  Copyright © 2016 tOMG. All rights reserved.
//

#ifndef Node_h
#define Node_h

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

class Node {
public:
    virtual void draw(glm::mat4 C) = 0;
	virtual void draw(glm::mat4 C, GLint shaderProgram, glm::mat4 P, glm::mat4 V) = 0;
    virtual void update() = 0;
};

#endif /* Node_h */
