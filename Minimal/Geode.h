//
//  Geode.hpp
//  CSE167Proj3
//
//  Created by Tommy Yang on 11/2/16.
//  Copyright © 2016 tOMG. All rights reserved.
//

#ifndef Geode_hpp
#define Geode_hpp

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

#include "Node.h"

class Geode	: public Node {
public:
    // OBJObject *toDraw;
    glm::mat4 toWorld;

    Geode();
    void draw(glm::mat4 C);
	void draw(glm::mat4 C, GLint shaderProgram, glm::mat4 P, glm::mat4 V);
    virtual void update() = 0;
};

#endif /* Geode_hpp */
