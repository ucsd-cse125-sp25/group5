////////////////////////////////////////
// Joint.cpp
////////////////////////////////////////

#include <iostream>
#include <vector>
#include <string>
#include "Joint.h"
#include "Tokenizer.h"
#include "Cube.h"
#include "DOF.h"
#include "core.h"
#include "Window.h"
#include "Skin.h"

Joint::Joint(Skeleton* jointsskel, Joint * parent) {
	this->skely = jointsskel;
	this->box = new Cube;
	this->parent = parent;
	skely->joints.push_back(this);

	boxmin = glm::vec3(-1, -1, -1);
	boxmax = glm::vec3(1, 1, 1);
	offset = glm::vec3(0, 0, 0);
	rotxlimit = glm::vec2(-100000, 100000);
	rotylimit = glm::vec2(-100000, 100000);
	rotzlimit = glm::vec2(-100000, 100000);
	pose = glm::vec3(0.0f, 0.0f, 0.0f);
	L = glm::mat4(1.0f);
	if (!parent) { //questionable
		this->parent = this;
	}
	W = glm::mat4(1.0f);
	xDof = new DOF();
	yDof = new DOF();
	zDof = new DOF();

}


void Joint::setTranslation(const glm::vec3& translation) {
	/*L = glm::translate(L, translation);*/
	this->offset = glm::vec4(translation, 1.0f);

}

void Joint::setRotation(const glm::vec3& rotation) {
	this->xDof->SetValue(rotation.x);
	this->yDof->SetValue(rotation.y);
	this->zDof->SetValue(rotation.z);
	//glm::vec4 translation = L[3];
	//L = glm::eulerAngleZYX(this->zDof->GetValue(), this->yDof->GetValue(), this->xDof->GetValue());
	//L[3] = translation;
	//W = parent->GetWorldMatrix() * L;
}

void Joint::Update(glm::mat4& parent) {


	// std::cout << "before dof set" << std::endl;

//	this->xDof->SetValue(pose.x);
//	this->yDof->SetValue(pose.y);
//	this->zDof->SetValue(pose.z);
	this->xDof->SetMinMax(rotxlimit.x, rotxlimit.y);
	this->yDof->SetMinMax(rotylimit.x, rotylimit.y);
	this->zDof->SetMinMax(rotzlimit.x, rotzlimit.y);
	// std::cout << "after dof set before matrices" << std::endl;

//	this->xDof->SetValue(20.0f);
//	std::cout << "Inside Joint: xDof for " << this->name << " is " << this->xDof->GetValue() << std::endl;


	L = glm::eulerAngleZYX(this->zDof->GetValue(), this->yDof->GetValue(), this->xDof->GetValue());
	L[3] = glm::vec4(offset, 1.0f);
	W = parent * L;

	/*for (const auto& child : children) {
		child->Update(W);
	}*/

	box->update(boxmin, boxmax, W);

	for (auto it = children.begin(); it < children.end(); it++) {
		(*it)->Update(W);
	}
	// Compute local matrix L
	// Compute world matrix W
	// Recursively call Update() on children
}

void Joint::Draw(GLuint shader) {
	//Cube* box3 = new Cube(this->boxmin, this->boxmax);
	// Cube* box3 = new Cube;
	//box = new Cube();
	//box->draw(viewProjMtx * W, shader);
    //box->draw(viewProjMtx, shader);
   // this->box2->draw(viewProjMtx, shader);


	for (auto it = children.begin(); it < children.end(); it++) {
		(*it)->Draw(shader);
	}

	// Draw oriented box with OpenGL
	// Recursively call Draw() on children
}

glm::mat4 Joint::GetWorldMatrix() {
	return W;
}
void Joint::AddChild(Joint * child) {
	this->children.push_back(child);
}

bool Joint::Load(Tokenizer & t) {
	t.GetToken(this->name);
	t.FindToken("{");
	int childCount = 0;
	while (1) {
		char temp[256];
		t.GetToken(temp);
		// std::cout << temp << std::endl;

		if (strcmp(temp, "offset") == 0) {
			offset.x = t.GetFloat();
			offset.y = t.GetFloat();
			offset.z = t.GetFloat();
			// std::cout << "offset " << std::endl;
		}
		else if (strcmp(temp, "boxmin") == 0) {
			boxmin.x = t.GetFloat();
			boxmin.y = t.GetFloat();
			boxmin.z = t.GetFloat();
		}
		else if (strcmp(temp, "boxmax") == 0) {
			boxmax.x = t.GetFloat();
			boxmax.y = t.GetFloat();
			boxmax.z = t.GetFloat();
		}
		else if (strcmp(temp, "rotxlimit") == 0) {
			rotxlimit.x = t.GetFloat();
			rotxlimit.y = t.GetFloat();
		}
		else if (strcmp(temp, "rotylimit") == 0) {
			rotylimit.x = t.GetFloat();
			rotylimit.y = t.GetFloat();
		}
		else if (strcmp(temp, "rotzlimit") == 0) {
			rotzlimit.x = t.GetFloat();
			rotzlimit.y = t.GetFloat();
		}
		else if (strcmp(temp, "pose") == 0) {
			pose.x = t.GetFloat();
			pose.y = t.GetFloat();
			pose.z = t.GetFloat();
			this->xDof->SetValue(pose.x);
			this->yDof->SetValue(pose.y);
			this->zDof->SetValue(pose.z);

			// std::cout << "pose " << pose.x << " " << pose.y << " " << pose.z << std::endl;
		}
		else if (strcmp(temp, "balljoint") == 0) {

			childCount++;
			Joint* jnt = new Joint(this->skely, this);
			jnt->Load(t);
			AddChild(jnt);
		}
		else if (strcmp(temp, "}") == 0) {
		/*	if (box == nullptr) {
				box = new Cube(boxmin, boxmax);
				// std::cout << "done with one cube" << std::endl;
			}*/
			return true;
		}
		else t.SkipLine(); // Unrecognized token
	} 
				// std::cout << "define just one cube" << std::endl;
				// box = new Cube(boxmin, boxmax);
				// box = new Cube;
				// std::cout << "define just one cube 2" << std::endl;
	return true;
}