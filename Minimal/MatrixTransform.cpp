//
//  MatrixTransform.cpp
//  CSE167Proj3
//
//  Created by Tommy Yang on 11/2/16.
//  Copyright © 2016 tOMG. All rights reserved.
//

#include "MatrixTransform.h"

MatrixTransform::MatrixTransform(glm::mat4 M)
{
    this->M = M;
    this->deg = 0.0f;
    this->axis = glm::vec3(0.0f, 1.0f, 0.0f);
    this->move = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec4 tmp_pos = M * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	this->pos = glm::vec3(tmp_pos.x, tmp_pos.y, tmp_pos.z);
}

void MatrixTransform::draw(glm::mat4 C)
{

    glm::mat4 M_new = C * M;

    Group::draw(M_new);
}
void MatrixTransform::draw(glm::mat4 C, GLint shaderProgram, glm::mat4 P, glm::mat4 V)
{

	glm::mat4 M_new = C * M;

	Group::draw(M_new, shaderProgram, P, V);
}

void MatrixTransform::rotate(float angle, glm::vec3 axis)
{
    float matX = this->M[3][0];
    float matY = this->M[3][1];
    float matZ = this->M[3][2];

    glm::mat4 to_origin_matrix =
    { 1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        -matX, -matY, -matZ, 1 };

    glm::mat4 reset_matrix =
    { 1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        matX, matY, matZ, 1 };

    this->M = to_origin_matrix * this->M;

    this->M = glm::rotate(glm::mat4(1.0f), angle / 180.0f * glm::pi<float>(), axis) * this->M;

    this->M = reset_matrix * this->M;
}

void MatrixTransform::scale(float mult)
{
    glm::mat4 scale_matrix =
    { mult, 0, 0, 0,
        0, mult, 0, 0,
        0, 0, mult, 0,
        0, 0, 0, 1 };

    float matX = this->M[3][0];
    float matY = this->M[3][1];
    float matZ = this->M[3][2];

    glm::mat4 to_origin_matrix =
    { 1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        -matX, -matY, -matZ, 1 };

    glm::mat4 reset_matrix =
    { 1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        matX, matY, matZ, 1 };

    this->M = to_origin_matrix * this->M;
    this->M = scale_matrix * this->M;
    this->M = reset_matrix * this->M;
}

void MatrixTransform::translate(float x, float y, float z)
{
    glm::mat4 matrix =
    { 1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        x, y, z, 1 };

    this->M = matrix * this->M;
}

void MatrixTransform::update()
{
    this->rotate(this->deg, this->axis);
    
	this->pos = this->pos + this->move;
	this->translate((this->move).x, (this->move).y, (this->move).z);
	
	//printf("%lf %lf %lf\n", this->pos.x, this->pos.y, this->pos.z);
	if (this->pos.x > 10.0 || this->pos.x < -10.0) {
		this->move.x = -this->move.x;
		this->pos = this->pos + this->move;
		this->translate((this->move).x, (this->move).y, (this->move).z);
	}
	if (this->pos.y > 10.0 || this->pos.y < -10.0) {
		this->move.y = -this->move.y;
		this->pos = this->pos + this->move;
		this->translate((this->move).x, (this->move).y, (this->move).z);
	}
	if (this->pos.z > 0.0 || this->pos.z < -20.0) {
		this->move.z = -this->move.z;
		this->pos = this->pos + this->move;
		this->translate((this->move).x, (this->move).y, (this->move).z);
	}
	
}
