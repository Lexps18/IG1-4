#include "Entity.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;


Abs_Entity::~Abs_Entity()
{
	delete mMesh;
	mMesh = nullptr;
}

void Abs_Entity::load()
{
	if (mMesh) mMesh->load();
}

void Abs_Entity::unload()
{
	if (mMesh) mMesh->unload();
}

void Abs_Entity::upload(const mat4& modelViewMat) const
{
	mShader->setUniform("modelView", modelViewMat);
}

EntityWithColors::EntityWithColors()
{
	mShader = Shader::get("vcolors");
}

void EntityWithColors::render(const mat4& modelViewMat) const
{
	if (mMesh != nullptr) {
		mat4 aMat = modelViewMat * mModelMat;
		mShader->use();
		upload(aMat);
		mMesh->render();
	}
}

SingleColorEntity::SingleColorEntity(glm::vec4 color)
	: mColor(color)
{
	mShader = Shader::get("simple");
}

void SingleColorEntity::render(const mat4& modelViewMat) const
{
	if (mMesh != nullptr) {
		mat4 aMat = modelViewMat * mModelMat;
		mShader->use();
		mShader->setUniform("color", mColor);
		upload(aMat);
		mMesh->render();
	}
}

RGBAxes::RGBAxes(GLdouble l)
{
	mMesh = Mesh::createRGBAxes(l);
}


EntityWithTexture::EntityWithTexture()
	: mTexture(nullptr)
{
	mShader = Shader::get("texture");
}

EntityWithTexture::~EntityWithTexture()
{
	delete mTexture;
	mTexture = nullptr;
}

void EntityWithTexture::render(const glm::mat4& modelViewMat) const
{
	if (mMesh != nullptr) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		mat4 aMat = modelViewMat * mModelMat;
		mShader->use();
		mShader->setUniform("modulate", mModulate);
		upload(aMat);
		mTexture->bind();
		mMesh->render();
		mTexture->unbind();
	}
}

// -----------------------------------------------------------------------
// CompoundEntity
// -----------------------------------------------------------------------

CompoundEntity::~CompoundEntity()
{
	// Libera todos los hijos (igual que Scene::destroy)
	for (Abs_Entity* e : gObjects) delete e;
	gObjects.clear();
}

void CompoundEntity::load()
{
	// CompoundEntity no tiene mMesh propia; delega en los hijos
	for (Abs_Entity* e : gObjects) e->load();
}

void CompoundEntity::unload()
{
	for (Abs_Entity* e : gObjects) e->unload();
}

void CompoundEntity::render(const glm::mat4& modelViewMat) const
{
	// Acumula la transformación propia y la pasa a cada hijo
	mat4 aMat = modelViewMat * mModelMat;
	for (Abs_Entity* e : gObjects)
		e->render(aMat);
}

void CompoundEntity::update()
{
	for (Abs_Entity* e : gObjects)
		e->update();
}