#pragma once
#include "Entity.h"

// -----------------------------------------------------------------------
// PRÁCTICA 3 — objetos existentes
// -----------------------------------------------------------------------

class Cube : public SingleColorEntity
{
public:
	explicit Cube(GLdouble length);
	void render(const glm::mat4& modelViewMat) const override;
};

class BoxOutline : public EntityWithTexture
{
protected:
	Texture* mTexture2;
public:
	explicit BoxOutline(GLdouble length);
	virtual ~BoxOutline() override;
	void render(const glm::mat4& modelViewMat) const override;
};

class Star3D : public EntityWithTexture
{
public:
	explicit Star3D(GLdouble re, GLuint np, GLdouble h);
	void render(const glm::mat4& modelViewMat) const override;
	void update() override;
};

class GlassParapet : public EntityWithTexture
{
public:
	explicit GlassParapet(GLdouble length);
	void render(const glm::mat4& modelViewMat) const override;
};

// -----------------------------------------------------------------------
// PRÁCTICA 4 — figuras de revolución (Escena 5 y base de Escena 7)
// -----------------------------------------------------------------------

// Toro: perfil circular de radio r centrado a distancia R del eje Y
class Torus : public SingleColorEntity
{
public:
	// R = radio mayor (centro del tubo al eje), r = radio menor (radio del tubo)
	// nMuestras = número de subdivisiones tanto del perfil como de la revolución
	Torus(GLdouble R, GLdouble r, GLuint nMuestras);
};

// Esfera de radio r con nMuestras paralelos y meridianos
class Sphere : public SingleColorEntity
{
public:
	Sphere(GLdouble r, GLuint nMuestras);
};

// Cilindro de radio r y altura h
class Cylinder : public SingleColorEntity
{
public:
	Cylinder(GLdouble r, GLdouble h, GLuint nMuestras);
};

// Cono truncado: radio superior rTop, radio inferior rBot, altura h
class TruncatedCone : public SingleColorEntity
{
public:
	TruncatedCone(GLdouble rTop, GLdouble rBot, GLdouble h, GLuint nMuestras);
};

// -----------------------------------------------------------------------
// PRÁCTICA 4 — cubo indexado (Escena 6)
// -----------------------------------------------------------------------

// Cubo renderizado con malla indexada (IndexMesh + glDrawElements)
class IndexCube : public SingleColorEntity
{
public:
	explicit IndexCube(GLdouble length);
};

// -----------------------------------------------------------------------
// PRÁCTICA 4 — entidad compuesta: Droide (Escena 7)
// -----------------------------------------------------------------------

// Droide: entidad compuesta (cabeza + cuerpo + brazos + piernas)
// Hereda de CompoundEntity para aprovechar su render/load/destroy
class Droide : public CompoundEntity
{
public:
	Droide();
};