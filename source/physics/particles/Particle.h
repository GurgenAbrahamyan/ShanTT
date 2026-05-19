#pragma once

#include "../../math_custom/Vector3.h"
#include <vector>

class Particle {
public:
    float mass;
    Vector3 position;
    Vector3 velocity;
    Vector3 forceAccumulator;

    std::vector<float*> positionPointers;

    Particle(float m, const Vector3& pos)
        : mass(m), position(pos), forceAccumulator(0, 0, 0)
		, velocity(0, 0, 0) {}

   
    void clearForces() {
        forceAccumulator = Vector3(0, 0, 0);
    }

    void addForce(const Vector3& force) {
        forceAccumulator = forceAccumulator+ force;
    }

    Vector3 getAcceleration() const {
        return forceAccumulator * 1.0f / mass;
    }
};

class ParticleSystem {
public:
    std::vector<Particle*> particles;
    float simulationTime;
    std::vector<float*> positionPointers;

    ParticleSystem(float mass) : simulationTime(0.0f) {
      
    }

    void addParticle(Particle* p) {
        particles.push_back(p);

    }

    void clearForces() {
        for (Particle* p : particles) {
            p->clearForces();
        }
    }

    void getState(float* dst) {

        for (Particle* p : particles) {
            *(dst++) = p->position.getX();
            *(dst++) = p->position.getY();
            *(dst++) = p->position.getZ();
            *(dst++) = p->velocity.getX();
            *(dst++) = p->velocity.getY();
            *(dst++) = p->velocity.getZ();
        }
    }

    void setState(float* dst) {
        size_t index = 0;
        for (Particle* p : particles) {
            p->position = Vector3(dst[index], dst[index + 1], dst[index + 2]);
            p->velocity = Vector3(dst[index + 3], dst[index + 4], dst[index + 5]);
            index += 6;
        }
    }

    int getNumParticles() {
        return static_cast<int>(particles.size());
    }


    std::vector<Particle*> getParticles() const {
        return particles;
    }
    ~ParticleSystem() {
        for (auto* p : particles) delete p;
    }
    float* getPositionArrayForMesh() { 
        contiguousPositions.resize(particles.size() * 3); 
        for (size_t i = 0; i < positionPointers.size(); ++i) 
        contiguousPositions[i] = *(positionPointers[i]); 
     
     return contiguousPositions.data(); } 

private:
    std::vector<float> contiguousPositions;
};

 



