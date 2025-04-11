////////////////////////////////////////
// Skeleton.cpp
////////////////////////////////////////

#include <iostream>
#include <vector>
#include <string>
#include "Skeleton.h"
#include "Tokenizer.h"
#include "Joint.h"

Skeleton::Skeleton() {
}

void Skeleton::doSkel() {
	this->doSkeleton = true;
}

std::vector<char*> Skeleton::JNamelist() {
	std::vector<char*> Jlist;

	// Iterate through Joints and fill array
	//Jlist.push_back("Joint 0 Name");
	//Jlist.push_back("Joint 1 Name");
	int i = 0;
	for (auto it = this->joints.begin(); it != this->joints.end(); it++) {

		Jlist.push_back(this->joints.at(i)->name);
		i++;
	}
	// Iterate through Joints and fill array

	return Jlist;
} // End JNamelist


bool Skeleton::Load(const char* file) {
	if (!doSkeleton) return false;

	std::cout << file << std::endl;
	Tokenizer token;
	token.Open(file);

	// Parse tree
	root = new Joint(this, NULL);
	char garbage[32];
	token.GetToken(garbage);
	root->Load(token);
	// Finish
	token.Close();
	return true;
	
}

void Skeleton::update() {
	if (!doSkeleton) return;

	if (root) {
		root->Update(glm::mat4(1.0f));
	}

	
}// –(traverse tree& compute all joint matrices)


void Skeleton::draw(const glm::mat4& viewProjMtx, GLuint shader) {
	if (!doSkeleton) return;

	if (root) {
		root->Draw(viewProjMtx, shader);
	}
	
}