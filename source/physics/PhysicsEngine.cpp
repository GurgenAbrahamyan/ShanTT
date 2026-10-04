#include "PhysicsEngine.h"
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <thread>

void PhysicsEngine::Init()
{
    JPH::RegisterDefaultAllocator();
    JPH::Factory::sInstance = new JPH::Factory();
    JPH::RegisterTypes();

    m_tempAllocator = std::make_unique<JPH::TempAllocatorImpl>(10 * 1024 * 1024); // 10 MB scratch

    unsigned int numThreads = std::max(1u, std::thread::hardware_concurrency() - 1);
    m_jobSystem = std::make_unique<JPH::JobSystemThreadPool>(
        JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, numThreads);

    m_physicsSystem = std::make_unique<JPH::PhysicsSystem>();

    const JPH::uint cMaxBodies             = 1024;
    const JPH::uint cNumBodyMutexes        = 0; 
    const JPH::uint cMaxBodyPairs          = 1024;
    const JPH::uint cMaxContactConstraints = 1024;

    m_physicsSystem->Init(
        cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints,
        m_broadPhaseLayerInterface,
        m_objectVsBroadPhaseLayerFilter,
        m_objectLayerPairFilter);

    m_physicsSystem->SetGravity(JPH::Vec3(0.0f, 0.0f, 0.0f));
    render = std::make_unique<JoltDebugRenderer>();
}

void PhysicsEngine::Update(float dt)
{
    const int collisionSteps = 1;
    m_physicsSystem->Update(dt, collisionSteps, m_tempAllocator.get(), m_jobSystem.get());
}

void PhysicsEngine::Shutdown()
{
    m_physicsSystem.reset();
    m_jobSystem.reset();
    m_tempAllocator.reset();

    JPH::UnregisterTypes();
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;
}
