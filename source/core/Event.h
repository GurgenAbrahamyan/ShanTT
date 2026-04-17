#pragma once


#include "../math_custom/Vector3.h"

#include <string.h>
class MaterialData;
class MeshData;
class Material;
class RenderMesh;
class ShaderData;
class Shader;
class Event {
};

class CreateObject : Event {
public:
    Vector3* position;

    CreateObject(Vector3 position) : position(&position) {}
};

class StopEngine : Event {

public:
    StopEngine() {}

};

class PressedKey : Event {

public:
    char key;
    PressedKey(char e) {
        key = e;
    }
};

class MouseDragged : Event {
public:
    double x, y;
    MouseDragged(double x, double y) : x(x), y(y){}


};

class CameraMode : Event {
public:
    bool key;
    CameraMode(bool e) {
        key = e;
    }
};

class UiMode : Event {
public:
    bool key;
    UiMode(bool e) {
        key = e;
    }


};

class InitModel : Event {

    //std::string path;

    InitModel(/*std::string path*/) {
      //  this->path = path;
	}

};

class InitMaterial : Event {
public:
    MaterialData* data;
	Material* result;
    InitMaterial(MaterialData* data) {
        this->data = data;
    }


};


class InitMesh : Event {
public:
    MeshData* data;
    RenderMesh* result;
    InitMesh(MeshData* data) {
        this->data = data;
    }


};

class InitShader : Event {
public:
    ShaderData* data;
    Shader* result;

    InitShader(ShaderData* data) {
        this->data = data;
    }


};

class GetDefaultShader : Event {
public:
    
    Shader* shader;

    GetDefaultShader() {
            
    
    }


};







