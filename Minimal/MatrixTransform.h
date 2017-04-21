//
//  MatrixTransform.hpp
//  CSE167Proj3
//
//  Created by Tommy Yang on 11/2/16.
//  Copyright © 2016 tOMG. All rights reserved.
//

#ifndef MatrixTransform_hpp
#define MatrixTransform_hpp

#include <stdio.h>
#include "Group.h"

class MatrixTransform : public Group {
public:
    glm::mat4 M;
    MatrixTransform(glm::mat4 M);
    ~MatrixTransform();
    void draw(glm::mat4 C);
    void rotate(float angle, glm::vec3 axis);
    void scale(float mult);
    void translate(float x, float y, float z);
    void update();
    float deg;
    glm::vec3 axis;
    glm::vec3 move;
	glm::vec3 pos;
};

#endif /* MatrixTransform_hpp */
