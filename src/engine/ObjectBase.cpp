#include <engine/ObjectBase.h>
#include <cassert>
ObjectBase::ObjectBase(Shader *shader, Shader *depthShader)
    : m_mainShader(shader), m_depthShader(depthShader) { assert(m_mainShader != nullptr); }
void ObjectBase::onUpdate(float deltaTime) {}
void ObjectBase::onRenderShadowMap() {}
void ObjectBase::onRender() {}
void ObjectBase::onImGuiRender() {}
Shader *ObjectBase::mainShader() { return m_mainShader; }
Shader *ObjectBase::depthShader() { return m_depthShader; }
ObjectBase::~ObjectBase() {}
