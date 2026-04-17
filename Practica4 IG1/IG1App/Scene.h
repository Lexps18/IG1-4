#ifndef _H_Scene_H_
#define _H_Scene_H_

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Camera.h"
#include "Entity.h"

#include <vector>

class Scene
{
public:
	Scene() = default;
	virtual ~Scene();

	Scene(const Scene& s) = delete;
	Scene& operator=(const Scene& s) = delete;

	virtual void init();
	virtual void update();

	virtual void render(Camera const& cam) const;

	void load();
	void unload();

protected:
	void destroy();
	void setGL();
	void resetGL();

	std::vector<Abs_Entity*> gObjects;
};

// -----------------------------------------------------------------------
// Prácticas anteriores
// -----------------------------------------------------------------------

class Scene1 : public Scene {
public:
	void init() override;
};

class Scene2 : public Scene {
public:
	void init() override;
};

class Scene3 : public Scene {
public:
	void init() override;
};

class Scene4 : public Scene {
public:
	void init() override;
	void render(Camera const& cam) const override;
	void update() override;

	mutable bool mCaptureNext = false;
	void setCapture() { mCaptureNext = true; }
};

// -----------------------------------------------------------------------
// Práctica 4
// -----------------------------------------------------------------------

// Escena 5: Toro (malla por revolución)
class Scene5 : public Scene {
public:
	void init() override;
};

// Escena 6: Cubo indexado (IndexMesh con glDrawElements)
class Scene6 : public Scene {
public:
	void init() override;
};

// Escena 7: Droide (entidad compuesta con figuras de revolución)
class Scene7 : public Scene {
public:
	void init() override;
};

// Escena 8: Sistema planetario (nodo ficticio + animación continua)
class Scene8 : public Scene {
public:
	void init() override;
	void update() override;
private:
	// Punteros a los nodos ficticios para animarlos en update().
	// La memoria la gestiona gObjects (via CompoundEntity).
	CompoundEntity* mOrbitaPlaneta = nullptr;
	CompoundEntity* mOrbitaLuna = nullptr;
};

#endif //_H_Scene_H_