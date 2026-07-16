

#include "../particles/Particle.h"
//#include "../../math_custom/GeneralFunctions.h"
#include "../../math_custom/Vector3.h"

class GeneralIntegrators {
public:
    static void EulerStep(ParticleSystem* p, float DeltaT) {
        int dim = p->getNumParticles() * 6;

        float* deriv = new float[dim];
        float* state = new float[dim];

        ParticleDerivative(p, deriv); 

        ScaleVector(deriv, DeltaT, dim);
        p->getState(state);
        AddVectors(state, deriv, state, dim);
        p->setState(state);

        p->simulationTime += DeltaT;

        delete[] deriv;
        delete[] state;
    }

    static void RK4Step(ParticleSystem* p, float dt) {
        int n = p->getNumParticles();
        int dim = n * 6;

        float* y0 = new float[dim];
        float* k1 = new float[dim];
        float* k2 = new float[dim];
        float* k3 = new float[dim];
        float* k4 = new float[dim];
        float* tempState = new float[dim];

        
        p->getState(y0);

        
        ParticleDerivative(p, k1);

        
        ScaleVector(k1, dt * 0.5f, dim);
        AddVectors(y0, k1, tempState, dim);
        p->setState(tempState);
        ParticleDerivative(p, k2);

       
        ScaleVector(k2, dt * 0.5f / (dt * 0.5f == 0 ? 1 : 1), dim);
        AddVectors(y0, k2, tempState, dim);
        p->setState(tempState);
        ParticleDerivative(p, k3);

        
        ScaleVector(k3, dt, dim);
        AddVectors(y0, k3, tempState, dim);
        p->setState(tempState);
        ParticleDerivative(p, k4);

        
        for (int i = 0; i < dim; i++) {
            tempState[i] = y0[i] + (k1[i] + 2 * k2[i] + 2 * k3[i] + k4[i]) / 6.0f;
        }

        p->setState(tempState);
        p->simulationTime += dt;

        delete[] y0;
        delete[] k1;
        delete[] k2;
        delete[] k3;
        delete[] k4;
        delete[] tempState;
    }


private:
   static int ParticleDerivative(ParticleSystem* p, float* dst) {
      

        for (int i = 0; i < p->getNumParticles(); i++) {
           
            *(dst++) = p->particles[i]->velocity.x;
            *(dst++) = p->particles[i]->velocity.y;
            *(dst++) = p->particles[i]->velocity.z;

           
            *(dst++) = p->particles[i]->forceAccumulator.getX() / p->particles[i]->mass;
            *(dst++) = p->particles[i]->forceAccumulator.getY() / p->particles[i]->mass;
            *(dst++) = p->particles[i]->forceAccumulator.getZ() / p->particles[i]->mass;
        }

        return 0;
    }

    static void ScaleVector(float* v, float scale, int dim) {
        for (int i = 0; i < dim; i++) {
            v[i] *= scale;
        }
    }

    static void AddVectors(float* a, float* b, float* dst, int dim) {
        for (int i = 0; i < dim; i++) {
            dst[i] = a[i] + b[i];
        }
    }
};
