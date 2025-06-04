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
#include "Skeleton.h"
#include "Skin.h"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

glm::mat4 ConvertMatrix(const aiMatrix4x4& from);

Joint::Joint(Skeleton* jointsskel, Joint * parent) {
	this->skely = jointsskel;
	//this->box = new Cube;
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
	if (!parent) { //questionable3
		this->parent = this;
	}
	preL = glm::mat4(1);
	draw = false;
	W = glm::mat4(1.0f);
	xDof = new DOF();
	yDof = new DOF();
	zDof = new DOF();

}

void Joint::Load(aiNode* node, std::unordered_map<aiNode*, aiBone*>* nodeToBone, Skin* skin) {

	strcpy(this->name, node->mName.C_Str());
	std::cout << this->name << " loaded!" << std::endl;
	skely->JNameMap.emplace(std::string(this->name), skely->joints.size() - 1);

	//if (std::string(this->name) == "rp_manuel_animated_001_dancing_hip") {
	//	xDof->SetValue(1.6f);
	//}

	//if (std::string(this->name) == "rp_manuel_animated_001_dancing_spine_01") {
	//	xDof->SetValue(-1.6f);
	//}

	preL = ConvertMatrix(node->mTransformation);
	std::cout << "joint array size during load: " << skely->joints.size() - 1 << std::endl;
	//std::cout << glm::to_string(preL) << std::endl;
	if (nodeToBone->count(node) > 0) {
		//std::cout << "Found node in nodeToBone: " << std::endl;
		aiBone* bone = (*nodeToBone)[node];
		invbind = ConvertMatrix(bone->mOffsetMatrix);
		draw = true;

		for (int i = 0; i < bone->mNumWeights; i++) {
			skin->sw[bone->mWeights[i].mVertexId]->jointIDs.push_back(skely->joints.size() - 1);
			skin->sw[bone->mWeights[i].mVertexId]->weights.push_back(bone->mWeights[i].mWeight);

			std::cout << "numweights: " << skin->sw[bone->mWeights[i].mVertexId]->jointIDs.size() << std::endl;
		}
	}


	/*
	std::cout << "Printing the children of the node above " << std::endl;
	std::cout << "Has this many children: " << node->mNumChildren << std::endl;
	for (int i = 0; i < node->mNumChildren; i++) {
		std::cout << node->mChildren[i]->mName.C_Str() << std::endl;
	}
	*/
	for (int i = 0; i < node->mNumChildren; i++) {
		//std::cout << node->mChildren[i]->mName.C_Str() << std::endl;
		Joint* jnt = new Joint(this->skely, this);
		jnt->Load(node->mChildren[i], nodeToBone, skin);
		AddChild(jnt);
	}
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

	/*
	if (useQuat) {
		//glm::mat4 animRot = glm::toMat4(currQuat);
		//glm::mat4 localTransform = preL * animRot;

		//// Print joint name
		//std::cout << "Joint: " << this->name << std::endl;

		//// Print preL matrix
		//std::cout << "preL:\n";
		//for (int i = 0; i < 4; ++i)
		//	std::cout << glm::to_string(glm::vec4(preL[0][i], preL[1][i], preL[2][i], preL[3][i])) << std::endl;

		//// Print animRot matrix
		//std::cout << "animRot:\n";
		//for (int i = 0; i < 4; ++i)
		//	std::cout << glm::to_string(glm::vec4(animRot[0][i], animRot[1][i], animRot[2][i], animRot[3][i])) << std::endl;

		//// Print currQuat values
		//std::cout << "currQuat: ("
		//	<< currQuat.w << ", "
		//	<< currQuat.x << ", "
		//	<< currQuat.y << ", "
		//	<< currQuat.z << ")\n" << std::endl;

		//W = parent * localTransform;

		glm::vec3 scale, translation, skew;
		glm::vec4 perspective;
		glm::quat dummyRot;
		glm::decompose(preL, scale, dummyRot, translation, skew, perspective);
		glm::mat4 localTransform = glm::translate(glm::mat4(1.0f), translation) *
			glm::toMat4(currQuat) *
			glm::scale(glm::mat4(1.0f), scale);
		W = parent * localTransform;
	}
	else {
		this->xDof->SetMinMax(rotxlimit.x, rotxlimit.y);
		this->yDof->SetMinMax(rotylimit.x, rotylimit.y);
		this->zDof->SetMinMax(rotzlimit.x, rotzlimit.y);
		L = glm::eulerAngleZYX(this->zDof->GetValue(), this->yDof->GetValue(), this->xDof->GetValue());
		L[3] = glm::vec4(offset, 1.0f);
		W = parent * preL * L;
	}
	*/

	this->xDof->SetMinMax(rotxlimit.x, rotxlimit.y);
	this->yDof->SetMinMax(rotylimit.x, rotylimit.y);
	this->zDof->SetMinMax(rotzlimit.x, rotzlimit.y);
	L = glm::eulerAngleZYX(this->zDof->GetValue(), this->yDof->GetValue(), this->xDof->GetValue());

	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;

	// Decompose preL
	glm::decompose(preL, scale, rotation, translation, skew, perspective);

	// Now build rotation matrix from your DOF angles
	glm::mat4 animRot = L;  // L is your rotation matrix from DOFs

	// Build scale matrix
	glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);

	// Build translation matrix
	glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), translation); //use offset here if you want to animate offset

	// Compose local transform: T * S * animRot
	glm::mat4 localTransform = translationMat * scaleMat * animRot;

	// Compose world transform
	//W = parent * localTransform;
	W = parent * localTransform;
	// std::cout << "after dof set before matrices" << std::endl;

//	this->xDof->SetValue(20.0f);
//	std::cout << "Inside Joint: xDof for " << this->name << " is " << this->xDof->GetValue() << std::endl;
	/*for (const auto& child : children) {
		child->Update(W);
	}*/

	//box->update(boxmin, boxmax, W);

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

glm::mat4 Joint::GetInvBindMatrix() {
	return invbind;
}

void Joint::AddChild(Joint * child) {
	this->children.push_back(child);
}

bool Joint::Load(Tokenizer & t) {
	t.GetToken(this->name);
	t.FindToken("{");
	int childCount = 0;
	draw = true;
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

glm::mat4 ConvertMatrix(const aiMatrix4x4& from) {
	glm::mat4 to;

	to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;

	return to;
}