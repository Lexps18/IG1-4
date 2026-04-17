#include "Scene.h"
#include "RGBObjects.h"
#include "objects2D.h"
#include "objects3D.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

// -----------------------------------------------------------------------
// Scene base
// -----------------------------------------------------------------------

void Scene::init()
{
	setGL();
	gObjects.push_back(new RGBAxes(400.0));
}

Scene::~Scene()
{
	destroy();
	resetGL();
}

void Scene::destroy()
{
	for (Abs_Entity* el : gObjects) delete el;
	gObjects.clear();
}

void Scene::load()
{
	for (Abs_Entity* obj : gObjects) obj->load();
}

void Scene::unload()
{
	for (Abs_Entity* obj : gObjects) obj->unload();
}

void Scene::setGL()
{
	glClearColor(0.6f, 0.7f, 0.8f, 1.0f);
	glEnable(GL_DEPTH_TEST);
}

void Scene::resetGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glDisable(GL_DEPTH_TEST);
}

void Scene::render(Camera const& cam) const
{
	cam.upload();
	for (Abs_Entity* el : gObjects)
		el->render(cam.viewMat());
}

void Scene::update()
{
	for (Abs_Entity* obj : gObjects)
		obj->update();
}

// -----------------------------------------------------------------------
// Escenas de prácticas anteriores (sin cambios)
// -----------------------------------------------------------------------

void Scene1::init()
{
	setGL();
	gObjects.push_back(new RGBAxes(400.0));

	RegularPolygon* hexagono = new RegularPolygon(6, 200.0);
	hexagono->setColor(glm::vec4(255.0f, 0.0f, 255.0f, 1.0f));
	gObjects.push_back(hexagono);

	RegularPolygon* circulo = new RegularPolygon(50, 200.0);
	circulo->setColor(glm::vec4(255.0f, 255.0f, 0.0f, 1.0f));
	gObjects.push_back(circulo);
}

void Scene2::init()
{
	setGL();
	const float r = 200.0f;

	gObjects.push_back(new RGBAxes(400.0));

	RGBTriangle* triangulo = new RGBTriangle(70.0, r);
	triangulo->setModelMat(translate(mat4(1.0f), vec3(r, 0.0f, 0.0f)));
	gObjects.push_back(triangulo);

	gObjects.push_back(new RGBRectangle(280.0, 280.0));

	RegularPolygon* circulo = new RegularPolygon(50, r);
	circulo->setColor(glm::vec4(255.0f, 255.0f, 0.0f, 1.0f));
	gObjects.push_back(circulo);
}

void Scene3::init()
{
	setGL();
	gObjects.push_back(new RGBAxes(400.0));
	gObjects.push_back(new RGBCube(200.0));
}

void Scene4::init()
{
	setGL();

	gObjects.push_back(new RGBAxes(400.0));
	gObjects.push_back(new Ground(400.0, 400.0));

	BoxOutline* caja = new BoxOutline(100.0);
	caja->setModelMat(translate(mat4(1.0f), vec3(-100.0f, 50.0f, -100.0f)));
	gObjects.push_back(caja);

	Star3D* estrella = new Star3D(40.0, 8, 40.0);
	estrella->setModelMat(translate(mat4(1.0f), vec3(-100.0f, 50.0f, -100.0f)));
	gObjects.push_back(estrella);

	GlassParapet* cristalera = new GlassParapet(400.0);
	cristalera->setModelMat(scale(mat4(1.0f), vec3(1.0f, 0.5f, 1.0f)));
	gObjects.push_back(cristalera);

	Photo* foto = new Photo(100.0, 75.0);
	mat4 fotoMat = translate(mat4(1.0f), vec3(0.0f, 1.0f, 0.0f));
	fotoMat = fotoMat * rotate(mat4(1.0f), radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
	foto->setModelMat(fotoMat);
	gObjects.push_back(foto);
}

void Scene4::render(Camera const& cam) const
{
	cam.upload();

	// PASO 1: opacos con depth test completo (lee Y escribe z-buffer).
	// Todos los objetos excepto la cristalera (penúltimo) y la foto (último).
	for (size_t i = 0; i + 2 < gObjects.size(); ++i)
		gObjects[i]->render(cam.viewMat());

	// PASO 2: captura de la foto antes de renderizar traslúcidos,
	// para que la imagen no capture el vidrio encima de todo.
	if (mCaptureNext) {
		glFlush();
		gObjects.back()->update(); // foto = último objeto
		mCaptureNext = false;
	}

	// PASO 3: traslúcidos — z-buffer en modo solo lectura (T4 Blending p.16).
	// Así los traslúcidos mezclan su color con lo que hay detrás,
	// pero no tapan los opacos que vengan detrás de ellos.
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	gObjects[gObjects.size() - 2]->render(cam.viewMat()); // cristalera

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	// PASO 4: foto siempre al final (encima de todo)
	gObjects.back()->render(cam.viewMat());
}

void Scene4::update()
{
	for (size_t i = 0; i + 1 < gObjects.size(); i++)
		gObjects[i]->update();
}

// -----------------------------------------------------------------------
// Escena 5: Toro
// -----------------------------------------------------------------------
// Muestra un toro generado por revolución del perfil circular.
// R=200 → radio mayor (distancia del centro del tubo al eje Y)
// r=60  → radio menor (radio del propio tubo)
// 30 muestras → suficiente resolución sin excesivo coste
void Scene5::init()
{
	setGL();
	gObjects.push_back(new RGBAxes(400.0));

	auto* toro = new Torus(200.0, 60.0, 30);
	toro->setColor({ 0.9f, 0.4f, 0.1f, 1.0f }); // naranja
	gObjects.push_back(toro);
}

// -----------------------------------------------------------------------
// Escena 6: Cubo indexado
// -----------------------------------------------------------------------
// Usa IndexMesh (glDrawElements): solo 8 vértices en GPU, 36 índices.
// Se renderiza igual que un Cube normal pero con menos datos en memoria.
void Scene6::init()
{
	setGL();
	gObjects.push_back(new RGBAxes(400.0));

	auto* cubo = new IndexCube(200.0);
	cubo->setColor({ 0.2f, 0.6f, 1.0f, 1.0f }); // azul claro
	gObjects.push_back(cubo);
}

// -----------------------------------------------------------------------
// Escena 7: Droide
// -----------------------------------------------------------------------
// Entidad compuesta (CompoundEntity) con partes creadas por revolución.
// El Droide encapsula toda la jerarquía en su constructor.
void Scene7::init()
{
	setGL();
	gObjects.push_back(new RGBAxes(400.0));

	auto* droide = new Droide();
	gObjects.push_back(droide);
}

// -----------------------------------------------------------------------
// Escena 8: Sistema planetario con nodo ficticio
// -----------------------------------------------------------------------
// Estructura jerárquica:
//   Sol (Sphere estática en el origen)
//   mOrbitaPlaneta (CompoundEntity — nodo ficticio que rota)
//     └── Planeta (Sphere desplazada +300 en X respecto al nodo)
//     └── mOrbitaLuna (CompoundEntity — nodo ficticio que rota)
//           └── Luna (Sphere desplazada +100 en X respecto al nodo)
//
// update() rota mOrbitaPlaneta (órbita del planeta alrededor del sol) y
// mOrbitaLuna (órbita de la luna alrededor del planeta) a distinta velocidad.
void Scene8::init()
{
	setGL();
	gObjects.push_back(new RGBAxes(600.0));

	// Sol: esfera amarilla fija en el origen
	auto* sol = new Sphere(80.0, 30);
	sol->setColor({ 1.0f, 0.9f, 0.1f, 1.0f });
	gObjects.push_back(sol);

	// Nodo ficticio del planeta: se añade a gObjects para que Scene lo
	// libere automáticamente en destroy()
	mOrbitaPlaneta = new CompoundEntity();
	gObjects.push_back(mOrbitaPlaneta);

	// Planeta: desplazado del nodo 300 unidades en X
	auto* planeta = new Sphere(40.0, 20);
	planeta->setColor({ 0.2f, 0.4f, 1.0f, 1.0f }); // azul
	planeta->setModelMat(translate(mat4(1.0f), vec3(300.0f, 0.0f, 0.0f)));
	mOrbitaPlaneta->addEntity(planeta);

	// Nodo ficticio de la luna: hijo de mOrbitaPlaneta, desplazado igual
	// que el planeta para que gire a su alrededor
	mOrbitaLuna = new CompoundEntity();
	mOrbitaLuna->setModelMat(translate(mat4(1.0f), vec3(300.0f, 0.0f, 0.0f)));
	mOrbitaPlaneta->addEntity(mOrbitaLuna);

	// Luna: desplazada +100 en X desde el nodo de la luna
	auto* luna = new Sphere(15.0, 15);
	luna->setColor({ 0.8f, 0.8f, 0.8f, 1.0f }); // gris
	luna->setModelMat(translate(mat4(1.0f), vec3(100.0f, 0.0f, 0.0f)));
	mOrbitaLuna->addEntity(luna);
}

// El planeta orbita lento (1°/frame) y la luna más rápido (4°/frame)
void Scene8::update()
{
	// Nodo planeta: rota alrededor del sol sobre el eje Y global
	mOrbitaPlaneta->setModelMat(
		rotate(mOrbitaPlaneta->getModelMat(),
			radians(1.0f), vec3(0.0f, 1.0f, 0.0f)));

	// Nodo luna: rota alrededor del planeta sobre el eje Y
	mOrbitaLuna->setModelMat(
		rotate(mOrbitaLuna->getModelMat(),
			radians(4.0f), vec3(0.0f, 1.0f, 0.0f)));
}