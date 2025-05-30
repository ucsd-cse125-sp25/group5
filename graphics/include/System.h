#pragma once

#include <vector>
#include <Particle.h>

class System {
public:
	void InitSimpleParticleSystem();
	void InitColoredTrail(glm::vec3 pos, glm::vec3 color);
	void InitParticleExplosion(glm::vec3 pos);
	void InitDamageEffect(glm::vec3 pos);
	void CreateParticle(float lifetime);
	void Update(float deltaTime);
	void UpdatePos(glm::vec3 newPos);
	void Draw(GLuint shader);
	float creationrate;
	float particlelifetime;
	float particlelifetimevar;
	float particleElasticity;
	float ground;
	float gravity;
	float airdensity;
	float friction;
	float particleradius;
	int decayMode;
	glm::vec3 initpos;
	glm::vec3 initposvar;
	glm::vec3 initvel;
	glm::vec3 initvelvar;
	glm::vec3 particlecolor;
	float ctime;
	

private:
	bool spawn;
	std::vector<Particle*> particles;
};