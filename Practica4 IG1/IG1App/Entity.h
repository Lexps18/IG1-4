#ifndef _H_Entities_H_
#define _H_Entities_H_

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

#include <vector>

class Abs_Entity // abstract class
{
public:
	Abs_Entity()
		: mModelMat(1.0)
		, mShader(nullptr) {
	};
	virtual ~Abs_Entity();

	Abs_Entity(const Abs_Entity& e) = delete;
	Abs_Entity& operator=(const Abs_Entity& e) = delete;

	virtual void render(const glm::mat4& modelViewMat) const = 0;
	virtual void update() {};

	glm::mat4 const& modelMat() const { return mModelMat; };
	void setModelMat(glm::mat4 const& aMat) { mModelMat = aMat; };

	// Alias públicos usados por las escenas (getModelMat / setModelMat)
	glm::mat4 const& getModelMat() const { return mModelMat; };

	virtual void load();
	virtual void unload();

protected:
	Mesh* mMesh = nullptr;
	glm::mat4 mModelMat;
	Shader* mShader;

	virtual void upload(const glm::mat4& mModelViewMat) const;
};

// -----------------------------------------------------------------------
class EntityWithColors : public Abs_Entity
{
public:
	explicit EntityWithColors();
	void render(const glm::mat4& modelViewMat) const override;
};

// -----------------------------------------------------------------------
class EntityWithTexture : public Abs_Entity
{
protected:
	Texture* mTexture;
	bool mModulate = false;
public:
	explicit EntityWithTexture();
	virtual ~EntityWithTexture() override;
	void render(const glm::mat4& modelViewMat) const override;
};

// -----------------------------------------------------------------------
class SingleColorEntity : public Abs_Entity
{
protected:
	glm::vec4 mColor;
public:
	explicit SingleColorEntity(glm::vec4 color = glm::vec4(1.0f));

	glm::vec4 getColor() { return mColor; };
	void setColor(const glm::vec4& color) { mColor = color; }
	void render(const glm::mat4& modelViewMat) const override;
};

// -----------------------------------------------------------------------
class RGBAxes : public EntityWithColors
{
public:
	explicit RGBAxes(GLdouble l);
};

// -----------------------------------------------------------------------
// CompoundEntity: entidad compuesta por un vector de sub-entidades.
// Su render() acumula su propia mModelMat y la pasa a los hijos.
// La destructora libera la memoria de todos los hijos.
// -----------------------------------------------------------------------
class CompoundEntity : public Abs_Entity
{
public:
	CompoundEntity() = default;
	virtual ~CompoundEntity();

	// load/unload delegan en los hijos (Abs_Entity::load llama a mMesh,
	// pero CompoundEntity no tiene mMesh; sobreescribimos para delegar)
	void load()   override;
	void unload() override;

	void addEntity(Abs_Entity* ae) { gObjects.push_back(ae); }

	void render(const glm::mat4& modelViewMat) const override;
	void update() override;

protected:
	std::vector<Abs_Entity*> gObjects;
};

#endif //_H_Entities_H_