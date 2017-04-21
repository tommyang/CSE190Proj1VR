//
//  Group.hpp
//  CSE167Proj3
//
//  Created by Tommy Yang on 11/2/16.
//  Copyright © 2016 tOMG. All rights reserved.
//

#ifndef Group_hpp
#define Group_hpp

#include "Node.h"
#include <list>

class Group: public Node {
public:
    std::list<Node*> children;
    
    Group();
    virtual void draw(glm::mat4 C);
	virtual void draw(glm::mat4 C, GLint shaderProgram, glm::mat4 P, glm::mat4 V);
    void update();
    void addChild(Node* node);
    void removeChild(Node* node);
};

#endif /* Group_hpp */
