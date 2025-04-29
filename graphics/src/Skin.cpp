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
	color = glm::vec3(0);
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

bool Skin::Load(aiMesh* mMesh, aiMaterial* mMaterial) {

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
		uvs.reserve(mMesh->mNumVertices);
		for (int t = 0; t < mMesh->mNumVertices; t++) {
			float x = mMesh->mTextureCoords[0][t].x;
			float y = mMesh->mTextureCoords[0][t].y;
			glm::vec2 uv(x, y);
			uvs.push_back(uv);
		}
	}

	this->tri = new Triangle(positions, normals, triangles);
	tri->tex = false;

	aiColor4D diffuse;
	aiGetMaterialColor(mMaterial, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
	color.x = diffuse.r;
	color.y = diffuse.g;
	color.z = diffuse.b;

	aiString texPath;
	if (mMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		const char* path = texPath.C_Str();
		std::string filename = getLastPathPart(std::string(path));
		std::string source = PROJECT_SOURCE_DIR;
		std::string  middle = "/assets/textures/";
		source = source + middle + filename;

		int width, height, nrChannels;
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


void Skin::draw(const glm::mat4& viewProjMtx, GLuint shader) {
	if (!doSkin) return;

	tri->draw(viewProjMtx, shader);
}