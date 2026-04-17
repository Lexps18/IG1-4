#include "objects3D.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <numbers>   // std::numbers::pi (C++20)

using namespace glm;

// -----------------------------------------------------------------------
// PRÁCTICA 3 — objetos existentes (sin cambios)
// -----------------------------------------------------------------------

// CUBO

Cube::Cube(GLdouble length)
{
	mMesh = Mesh::generateCube(length);
}

void Cube::render(const glm::mat4& modelViewMat) const
{
	if (mMesh == nullptr) return;

	mat4 aMat = modelViewMat * mModelMat;
	mShader->use();
	mShader->setUniform("color", mColor);
	upload(aMat);

	glEnable(GL_CULL_FACE);

	glCullFace(GL_FRONT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);   // caras frontales: wireframe
	mMesh->render();

	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);  // caras traseras: puntos
	mMesh->render();

	// Restaurar estado para no contaminar objetos siguientes
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
}

// BOX OUTLINE

BoxOutline::BoxOutline(GLdouble length)
	: mTexture2(nullptr)
{
	mMesh = Mesh::generateBoxOutlineTexCor(length);

	mTexture = new Texture();
	mTexture->load("..\\assets\\images\\papelE.png");

	mTexture2 = new Texture();
	mTexture2->load("..\\assets\\images\\container.jpg");
}

BoxOutline::~BoxOutline()
{
	delete mTexture2;
	mTexture2 = nullptr;
}

void BoxOutline::render(const glm::mat4& modelViewMat) const
{
	if (mMesh == nullptr) return;

	glDisable(GL_BLEND);
	mat4 aMat = modelViewMat * mModelMat;
	mShader->use();
	mShader->setUniform("modulate", mModulate);
	upload(aMat);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);

	glCullFace(GL_BACK);
	mTexture->bind();
	mMesh->render();
	mTexture->unbind();

	glCullFace(GL_FRONT);
	mTexture2->bind();
	mMesh->render();
	mTexture2->unbind();

	glDisable(GL_CULL_FACE);
}

// ESTRELLA

Star3D::Star3D(GLdouble re, GLuint np, GLdouble h)
{
	mMesh = Mesh::generateStar3DTexCor(re, np, h);
	mTexture = new Texture();
	mTexture->load("..\\assets\\images\\rueda.png");
}

void Star3D::render(const glm::mat4& modelViewMat) const
{
	if (mMesh != nullptr) {
		mShader->use();
		mShader->setUniform("modulate", mModulate);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if (mTexture != nullptr) mTexture->bind();

		glm::mat4 aMat1 = modelViewMat * mModelMat;
		upload(aMat1);
		mMesh->render();

		// Segunda estrella enfrentada (rotada 180° sobre X)
		glm::mat4 aMat2 = modelViewMat * mModelMat *
			glm::rotate(mat4(1.0f), radians(180.0f), vec3(1, 0, 0));
		upload(aMat2);
		mMesh->render();

		if (mTexture != nullptr) mTexture->unbind();
	}
}

void Star3D::update()
{
	mModelMat = glm::rotate(mModelMat, radians(1.0f), vec3(0, 0, 1));
	mModelMat = glm::rotate(mModelMat, radians(0.5f), vec3(0, 1, 0));
}

// GLASS PARAPET

GlassParapet::GlassParapet(GLdouble length)
{
	mMesh = Mesh::generateBoxOutlineTexCor(length);
	mTexture = new Texture();
	mTexture->load("..\\assets\\images\\windowC.png", 200);
}

void GlassParapet::render(const glm::mat4& modelViewMat) const
{
	if (mMesh != nullptr) {
		mat4 aMat = modelViewMat * mModelMat;
		mShader->use();
		mShader->setUniform("modulate", mModulate);
		upload(aMat);

		// El blending y glDepthMask los gestiona Scene4::render() antes
		// de llamar a este método, siguiendo el protocolo de los apuntes T4.
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);

		if (mTexture != nullptr) mTexture->bind();

		// Primera pasada: caras traseras (interior del cristal visto desde dentro)
		glCullFace(GL_FRONT);
		mMesh->render();

		// Segunda pasada: caras frontales (exterior del cristal visto desde fuera)
		// Al hacer dos pasadas en vez de ningún culling, cada píxel solo
		// recibe el color del cristal una vez → alpha correcto sin saturación.
		glCullFace(GL_BACK);
		mMesh->render();

		if (mTexture != nullptr) mTexture->unbind();

		glDisable(GL_CULL_FACE);
	}
}

// -----------------------------------------------------------------------
// PRÁCTICA 4 — figuras de revolución
// -----------------------------------------------------------------------

// TORO
// El perfil es un círculo de radio r centrado en (R, 0):
//   P(t) = (R + r·cos(t),  r·sin(t))   para t ∈ [0, 2π]
// Al girar ese perfil alrededor del eje Y se obtiene el toro.
Torus::Torus(GLdouble R, GLdouble r, GLuint nMuestras)
{
	std::vector<glm::vec2> perfil;
	perfil.reserve(nMuestras + 1);

	GLdouble step = 2.0 * std::numbers::pi / nMuestras;
	for (GLuint k = 0; k <= nMuestras; ++k) {
		GLdouble t = k * step;
		// Punto del perfil en el plano XY
		perfil.emplace_back(
			(float)(R + r * cos(t)),  // distancia al eje Y
			(float)(r * sin(t)));     // altura
	}

	mMesh = Mesh::generateByRevolution(perfil, nMuestras);
}

// ESFERA
// Perfil: semicircunferencia de radio r desde +π/2 hasta -π/2 (de arriba a abajo)
//   P(t) = (r·cos(t),  r·sin(t))
// Al girar genera una esfera completa.
Sphere::Sphere(GLdouble r, GLuint nMuestras)
{
	std::vector<glm::vec2> perfil;
	perfil.reserve(nMuestras + 1);

	GLdouble step = std::numbers::pi / nMuestras; // de π/2 a -π/2 en nMuestras pasos
	for (GLuint k = 0; k <= nMuestras; ++k) {
		GLdouble t = std::numbers::pi / 2.0 - k * step; // empieza arriba (+π/2)
		perfil.emplace_back(
			(float)(r * cos(t)),  // x ≥ 0 siempre (perfil en el semiplano derecho)
			(float)(r * sin(t))); // y va de r a -r
	}

	mMesh = Mesh::generateByRevolution(perfil, nMuestras);
}

// CILINDRO
// Perfil: segmento vertical a distancia r del eje, de +h/2 a -h/2
Cylinder::Cylinder(GLdouble r, GLdouble h, GLuint nMuestras)
{
	std::vector<glm::vec2> perfil = {
		{ (float)r,  (float)(h / 2.0) },
		{ (float)r, -(float)(h / 2.0) }
	};
	mMesh = Mesh::generateByRevolution(perfil, nMuestras);
}

// CONO TRUNCADO
// Perfil: segmento oblicuo de (rTop, +h/2) a (rBot, -h/2)
TruncatedCone::TruncatedCone(GLdouble rTop, GLdouble rBot, GLdouble h, GLuint nMuestras)
{
	std::vector<glm::vec2> perfil = {
		{ (float)rTop,  (float)(h / 2.0) },
		{ (float)rBot, -(float)(h / 2.0) }
	};
	mMesh = Mesh::generateByRevolution(perfil, nMuestras);
}

// -----------------------------------------------------------------------
// PRÁCTICA 4 — cubo indexado (Escena 6)
// -----------------------------------------------------------------------

// IndexCube usa IndexMesh con glDrawElements: solo 8 vértices reales en GPU
IndexCube::IndexCube(GLdouble length)
{
	mMesh = IndexMesh::generateIndexCube(length);
}

// -----------------------------------------------------------------------
// PRÁCTICA 4 — Droide (Escena 7)
// -----------------------------------------------------------------------

// El droide es una CompoundEntity formada por:
//   - Cuerpo:   cilindro central gris
//   - Cabeza:   esfera amarilla encima del cuerpo
//   - Brazo L:  cilindro pequeño a la izquierda
//   - Brazo R:  cilindro pequeño a la derecha
//   - Pierna L: cono truncado a la izquierda
//   - Pierna R: cono truncado a la derecha
//
// Cada pieza se posiciona con translate sobre su mModelMat (identidad).
// El conjunto se coloca en el origen; la escena puede moverlo luego.
Droide::Droide()
{
	// --- Cuerpo ---
	auto* body = new Cylinder(50.0, 120.0, 20);
	body->setColor({ 0.55f, 0.55f, 0.55f, 1.0f }); // gris metálico
	addEntity(body);

	// --- Cabeza (esfera sobre el cuerpo) ---
	auto* head = new Sphere(40.0, 20);
	head->setColor({ 0.85f, 0.85f, 0.20f, 1.0f }); // amarillo
	// El cuerpo llega hasta y=+60; la cabeza se sitúa encima (+60 + radio=40 → centro en y=100)
	head->setModelMat(translate(mat4(1.0f), vec3(0.0f, 100.0f, 0.0f)));
	addEntity(head);

	// --- Brazo izquierdo ---
	auto* armL = new Cylinder(12.0, 90.0, 12);
	armL->setColor({ 0.25f, 0.35f, 0.80f, 1.0f }); // azul
	armL->setModelMat(translate(mat4(1.0f), vec3(-70.0f, 15.0f, 0.0f)));
	addEntity(armL);

	// --- Brazo derecho ---
	auto* armR = new Cylinder(12.0, 90.0, 12);
	armR->setColor({ 0.25f, 0.35f, 0.80f, 1.0f });
	armR->setModelMat(translate(mat4(1.0f), vec3(70.0f, 15.0f, 0.0f)));
	addEntity(armR);

	// --- Pierna izquierda (cono truncado: más ancho arriba que abajo) ---
	auto* legL = new TruncatedCone(22.0, 12.0, 110.0, 12);
	legL->setColor({ 0.55f, 0.20f, 0.20f, 1.0f }); // rojo oscuro
	// El cuerpo llega hasta y=-60; la pierna cuelga hacia abajo
	legL->setModelMat(translate(mat4(1.0f), vec3(-28.0f, -115.0f, 0.0f)));
	addEntity(legL);

	// --- Pierna derecha ---
	auto* legR = new TruncatedCone(22.0, 12.0, 110.0, 12);
	legR->setColor({ 0.55f, 0.20f, 0.20f, 1.0f });
	legR->setModelMat(translate(mat4(1.0f), vec3(28.0f, -115.0f, 0.0f)));
	addEntity(legR);
}