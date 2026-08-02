#pragma once

struct SceneContext;

class ISystem {

  public: 
    virtual ~ISystem() = default;

    virtual void Initialize (SceneContext&) {};
    virtual void Update     (SceneContext&, float) {}
    virtual void FixedUpdate(SceneContext&, float) {}
    virtual void Shutdown   (SceneContext&) {};
};