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
#include <stb_image.h>

std::string getLastPathPart(const std::string& path);

Skin::Skin(Skeleton* skinsskel) {

	std::vector<SkinWeights*> sw = {};
	std::vector<glm::mat4> bindMatrices = {};
	this->skely = skinsskel;
	this->oneglm = glm::mat4(1.0f);
}

void Skin::Preload(int capacity) {
	sw.reserve(capacity);
	for (int i = 0; i < capacity; i++) {
		SkinWeights* weights = new SkinWeights();
		sw.push_back(weights);
	}
}

void Skin::doSkinning() {
	this->doSkin = true;
}

bool Skin::Load(aiMesh* mMesh, aiMaterial* mMaterial, int texindex) {

	triangles.reserve(mMesh->mNumFaces * 3);
	for (int j = 0; j < mMesh->mNumFaces; j++) {

		triangles.push_back(mMesh->mFaces[j].mIndices[0]);
		triangles.push_back(mMesh->mFaces[j].mIndices[1]);
		triangles.push_back(mMesh->mFaces[j].mIndices[2]);
	}

	positions.reserve(mMesh->mNumVertices);
	normals.reserve(mMesh->mNumVertices);
	for (int j = 0; j < mMesh->mNumVertices; j++) {
		glm::vec3 vert(mMesh->mVertices[j].x, mMesh->mVertices[j].y, mMesh->mVertices[j].z);
		positions.push_back(vert);
		glm::vec3 normal(mMesh->mNormals[j].x, mMesh->mNormals[j].y, mMesh->mNormals[j].z);
		normals.push_back(normal);
	}

	if (mMesh->HasTextureCoords(0)) {
		std::cout << "FOUND UVS" << std::endl;
		uvs.reserve(mMesh->mNumVertices);
		for (int t = 0; t < mMesh->mNumVertices; t++) {
			float x = mMesh->mTextureCoords[0][t].x;
			float y = mMesh->mTextureCoords[0][t].y;
			glm::vec2 uv(x, y);
			//std::cout << "UVS" << uv.x << " " << uv.y << std::endl;
			uvs.push_back(uv);
		}
	}
	else {
		std::cout << "DID NOT FIND UVS" << std::endl;
	}

	this->tri = new Triangle(&positions, &normals, &uvs, &triangles);
	tri->tex = false;

	aiColor4D diffuse;
	aiGetMaterialColor(mMaterial, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
	tri->color.x = diffuse.r;
	tri->color.y = diffuse.g;
	tri->color.z = diffuse.b;

	aiString texPath;
	if (mMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS || texindex > 0) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		const char* path = texindex == 1 ? "baked.png" : texPath.C_Str();
		std::string filename = getLastPathPart(std::string(path));
		std::string source = PROJECT_SOURCE_DIR;
		std::string  middle = "/assets/textures/";
		source = source + middle + filename;

		int width, height, nrChannels;
		std::cout << "texture source" << source.c_str() << std::endl;
		unsigned char* data = stbi_load(source.c_str(), &width, &height, &nrChannels, 0);

		if (data)
		{
			tri->tex = true;
			glGenTextures(1, &(tri->texture));
			glBindTexture(GL_TEXTURE_2D, tri->texture);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}

		stbi_image_free(data);

		//More Debug
		std::cout << "vertex 0: " << sw[0]->jointIDs.size();
		std::cout << "vertex 0: " << sw[0]->weights.size();


	}

	tri->create(&positions, &normals, &triangles, &uvs, oneglm);


	for (int i = 0; i < mMesh->mNumVertices; i++) {
		SkinWeights* skinweight = sw[i];
		float totalWeight = 0.0f;
		for (float w : skinweight->weights) totalWeight += w;

		if (totalWeight > 0.0f) {
			for (float& w : skinweight->weights) w /= totalWeight;
		}
		else {
			std::cerr << "Warning: vertex " << i << " has zero total weight!" << std::endl;
		}
	}
	
	return true;
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

	std::vector<glm::vec2> uvs;

	this->tri = new Triangle(&positions, &normals, &uvs, &triangles);


	return true;
}


void Skin::update() {
	if (!doSkin) return;

	if (!skely->doSkeleton) {
		tri->update(&positions, &normals, &uvs, &triangles, glm::mat4(1.0f));
		return;
	}
	//smooth skin algorithm
	//for each vertex
	std::vector<glm::mat4>  jointMatrices;
	std::vector<glm::vec3>  transformedPositions;
	std::vector<glm::vec3>  transformedNormals;
	if (bindMatrices.size() > 0) {
		for (int i = 0; i < this->skely->joints.size(); i++) {
			// std::cout << "joint matrix count: " << jointMatrices.size() << std::endl;
			jointMatrices.push_back(this->skely->joints[i]->GetWorldMatrix() * inverse(bindMatrices.at(i)));
		}
	}
	else {
		for (int i = 0; i < this->skely->joints.size(); i++) {
			// std::cout << "joint matrix count: " << jointMatrices.size() << std::endl;
			jointMatrices.push_back(this->skely->joints[i]->GetWorldMatrix() * this->skely->joints[i]->GetInvBindMatrix());
		}
	}

	for (int i = 0; i < positions.size(); i++) {
		glm::mat4 newPosMat = glm::mat4(0.0f);
		glm::mat4 newNormMat = glm::mat4(0.0f);
		SkinWeights* skinweight = sw.at(i);
		float sum = 0;
		for (int j = 0; j < skinweight->jointIDs.size(); j++) {
			float weight = skinweight->weights.at(j);
			sum += weight;
			int jointId = skinweight->jointIDs.at(j);
			glm::mat4 jointMatrix = jointMatrices.at(jointId);
			newPosMat += (skinweight->weights.at(j) * jointMatrix);
			glm::mat3 normalTransform = glm::transpose(glm::inverse(glm::mat3(jointMatrix)));
			newNormMat += glm::mat4(skinweight->weights.at(j) * normalTransform);
		}
		if (abs(sum - 1.0f) > 0.01) {
			//std::cout << "WEIGHTS DO NOT SUM TO 1: " << i << " " << sum << std::endl;
		}
		glm::vec4 newPos = newPosMat * glm::vec4(positions.at(i), 1.0);
		transformedPositions.push_back(glm::vec3(newPos.x, newPos.y, newPos.z));
		glm::vec4 newNorm = newNormMat * glm::vec4(normals.at(i), 0.0);
		newNorm = newNorm / glm::length(newNorm);
		transformedNormals.push_back(glm::vec3(newNorm.x, newNorm.y, newNorm.z));
	}
	
	tri->update(&transformedPositions, &transformedNormals, &uvs, &triangles, oneglm);

}// –(traverse tree& compute all joint matrices)


void Skin::draw(GLuint shader, bool shadow) {
	if (!doSkin) return;

	tri->draw(shader, shadow);
}