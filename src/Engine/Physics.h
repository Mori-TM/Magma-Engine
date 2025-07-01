/*
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
*/
//using namespace JPH;

void PhysicsInit()
{
    /*
    JPH::RegisterDefaultAllocator();
    JPH::Factory::sInstance = new JPH::Factory();
    JPH::RegisterTypes();

    // Create the physics system (number of bodies, contact constraints, etc.)
    const uint32_t cMaxBodies = 1024;
    const uint32_t cNumBodyMutexes = 0;  // Use a single mutex for simplicity
    const uint32_t cMaxBodyPairs = 1024;
    const uint32_t cMaxContactConstraints = 1024;

    JPH::TempAllocatorImpl temp_allocator(10 * 1024 * 1024);  // 10 MB temporary allocator
    JPH::JobSystemThreadPool job_system(cNumBodyMutexes - 1);

    JPH::PhysicsSystem physics_system;
    physics_system.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, nullptr, nullptr);

    // Create a simple box shape
    JPH::RefConst<JPH::Shape> box_shape = new JPH::BoxShape(JPH::Vec3(1.0f, 1.0f, 1.0f));  // A 1x1x1 box

    // Create the rigid body with initial settings
    JPH::BodyCreationSettings body_settings(box_shape, JPH::Vec3(0.0f, 10.0f, 0.0f), JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, JPH::Layers::MOVING);
    JPH::Body* body = physics_system.GetBodyInterface().CreateBody(body_settings);

    // Add the body to the world
    physics_system.GetBodyInterface().AddBody(body->GetID(), JPH::EActivation::Activate);

    // Simulate for a few seconds
    const float deltaTime = 1.0f / 60.0f;  // 60 FPS simulation

    for (int i = 0; i < 300; ++i)  // Simulate for 5 seconds (300 frames)
    {
        // Step the physics world
        physics_system.Update(deltaTime, 1, 1, &temp_allocator, &job_system);

        // Output the position of the body
        JPH::Vec3 position = body->GetPosition();
        std::cout << "Frame " << i << " - Body Position: (" << position.GetX() << ", " << position.GetY() << ", " << position.GetZ() << ")\n";
    }

    // Clean up
    delete JPH::Factory::sInstance;
    */
}