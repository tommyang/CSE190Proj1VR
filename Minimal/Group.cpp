//
//  Group.cpp
//  CSE167Proj3
//
//  Created by Tommy Yang on 11/2/16.
//  Copyright © 2016 tOMG. All rights reserved.
//

#include "Group.h"

Group::Group() {

}

void Group::draw(glm::mat4 C) {
    std::list<Node*>::iterator it;
    for (it = children.begin(); it != children.end(); ++it) {
        (*it)->draw(C);
    }
}

void Group::draw(glm::mat4 C, GLint shaderProgram, glm::mat4 P, glm::mat4 V) {
	std::list<Node*>::iterator it;
	for (it = children.begin(); it != children.end(); ++it) {
		(*it)->draw(C, shaderProgram, P, V);
	}
}

void Group::update() {
    std::list<Node*>::iterator it;
    for (it = children.begin(); it != children.end(); ++it) {
        (*it)->update();
    }
}

void Group::addChild(Node* node) {
    children.push_back(node);
}

void Group::removeChild(Node* node) {
    children.remove(node);
}
