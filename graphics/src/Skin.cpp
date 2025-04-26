////////////////////////////////////////
// Skin.cpp
////////////////////////////////////////

#include <iostream>
#include "Skin.h"
#include "Tokenizer.h"
#include "Joint.h"
#include "Triangle.h"
#include "Window.h"
#include "Skeleton.h"

Skin::Skin(Skeleton* skinsskel) {

	std::vector<SkinWeights*> sw = {};
	std::vector<glm::mat4> bindMatrices = {};
	this->skely = skinsskel;
	this->oneglm = glm::mat4(1.0f);
}

void Skin::doSkinning() {
	this->doSkin = true;
}
bool Skin::Load(const char* file) {
	if (!doSkin) return false;

	Tokenizer token;
	token.Open(file);

	token.FindToken("positions");
	int positionCount = token.GetInt();
	token.FindToken("{");
	for (int i = 0; i < positionCount; i++) {
		glm::vec3 pos;
		pos.x = token.GetFloat();
		pos.y = token.GetFloat();
		pos.z = token.GetFloat();
		positions.push_back(pos);
	}


	token.FindToken("normals");
	int normalCount = token.GetInt();
	token.FindToken("{");
	for (int i = 0; i < normalCount; i++) {
		glm::vec3 norm;
		norm.x = token.GetFloat();
		norm.y = token.GetFloat();
		norm.z = token.GetFloat();
		normals.push_back(norm);
	}

	token.FindToken("skinweights");
	int skinweightCount = token.GetInt();
	token.FindToken("{");
	for (int i = 0; i < skinweightCount; i++) {
		SkinWeights* skinweight = new SkinWeights();
		int numattachments = token.GetInt();
		for (int i = 0; i < numattachments; i++) {
			skinweight->jointIDs.push_back(token.GetInt());
			skinweight->weights.push_back(token.GetFloat());
		}
		sw.push_back(skinweight);
	}


	token.FindToken("triangles");
	int triangleCount = token.GetInt();
	token.FindToken("{");
	for (int i = 0; i < triangleCount*3; i++) {
		triangles.push_back(token.GetInt());
	}

	token.FindToken("bindings");
	int bindingCount = token.GetInt();
	token.FindToken("{");
	for (int i = 0; i < bindingCount; i++) {
		token.FindToken("matrix");
		token.FindToken("{");
		glm::mat4 bind = glm::mat4(1.0f);
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 3; k++) {
//				std::cout << "hi" << std::endl;
				bind[j][k] = token.GetFloat();
			}
		}
		bindMatrices.push_back(bind);
	}

	// Finish
	token.Close();

	this->tri = new Triangle(positions, normals, triangles);
	return true;
}


void Skin::update() {
	if (!doSkin) return;

	if (!skely->doSkeleton) {
		tri->update(positions, normals, triangles, glm::mat4(1.0f));
		return;
	}
	//smooth skin algorithm
	//for each vertex
	std::vector<glm::mat4>  jointMatrices;
	std::vector<glm::vec3>  transformedPositions;
	std::vector<glm::vec3>  transformedNormals;
	for (int i = 0; i < this->skely->joints.size(); i++) {
		// std::cout << "joint matrix count: " << jointMatrices.size() << std::endl;
		jointMatrices.push_back(this->skely->joints[i]->GetWorldMatrix() * inverse(bindMatrices.at(i)));
	}
	
	for (int i = 0; i < positions.size(); i++) {
		glm::mat4 newPosMat = glm::mat4(0.0f);
		glm::mat4 newNormMat = glm::mat4(0.0f);
		SkinWeights* skinweight = sw.at(i);
		for (int j = 0; j < skinweight->jointIDs.size(); j++) {
			float weight = skinweight->weights.at(j);
			int jointId = skinweight->jointIDs.at(j);
			glm::mat4 jointMatrix = jointMatrices.at(jointId);
			newPosMat += (skinweight->weights.at(j) * jointMatrices.at(skinweight->jointIDs.at(j)));
			newNormMat += skinweight->weights.at(j) * jointMatrices.at(skinweight->jointIDs.at(j));
		}
		glm::vec4 newPos = newPosMat * glm::vec4(positions.at(i), 1.0);
		transformedPositions.push_back(glm::vec3(newPos.x, newPos.y, newPos.z));
		glm::vec4 newNorm = newNormMat * glm::vec4(normals.at(i), 0.0);
		newNorm = newNorm / glm::length(newNorm);
		transformedNormals.push_back(glm::vec3(newNorm.x, newNorm.y, newNorm.z));
	}
	
	tri->update(transformedPositions, transformedNormals, triangles, oneglm);

}// –(traverse tree& compute all joint matrices)


void Skin::draw(GLuint shader, bool shadow) {
	if (!doSkin) return;

	tri->draw(shader, shadow);
}