//
//  Geode.cpp
//  CSE167Proj3
//
//  Created by Tommy Yang on 11/2/16.
//  Copyright © 2016 tOMG. All rights reserved.
//

#include "Geode.h"

Geode::Geode() {
    toWorld = glm::mat4(1.0f);
}

void Geode::draw(glm::mat4 C) {
    toWorld = C;
}

void Geode::draw(glm::mat4 C, GLint shaderProgram, glm::mat4 P, glm::mat4 V) {
	toWorld = C;
}