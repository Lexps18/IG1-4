#include "Mesh.h"

#include <numbers>    // std::numbers::pi (C++20)
#include <cassert>
#include <limits>

using namespace std;
using namespace glm;

// Placeholder para indicar que el objeto GPU aún no existe
constexpr GLuint NONE = numeric_limits<GLuint>::max();

// -----------------------------------------------------------------------
// Mesh
// -----------------------------------------------------------------------

Mesh::Mesh()
	: mVAO(NONE)
	, mVBO(NONE)
	, mCBO(NONE)
	, mTCO(NONE)
{
}

Mesh::~Mesh()
{
	unload();
}

void
Mesh::draw() const
{
	glDrawArrays(mPrimitive, 0, size());
}

void
Mesh::load()
{
	assert(mVBO == NONE); // no cargar dos veces

	if (vVertices.size() > 0) {
		glGenBuffers(1, &mVBO);
		glGenVertexArrays(1, &mVAO);

		glBindVertexArray(mVAO);
		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBufferData(GL_ARRAY_BUFFER, vVertices.size() * sizeof(vec3), vVertices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), nullptr);
		glEnableVertexAttribArray(0);

		if (vColors.size() > 0) {
			glGenBuffers(1, &mCBO);
			glBindBuffer(GL_ARRAY_BUFFER, mCBO);
			glBufferData(GL_ARRAY_BUFFER, vColors.size() * sizeof(vec4), vColors.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vec4), nullptr);
			glEnableVertexAttribArray(1);
		}

		glGenBuffers(1, &mTCO);
		glBindBuffer(GL_ARRAY_BUFFER, mTCO);
		glBufferData(GL_ARRAY_BUFFER,
			vTexCoords.size() * sizeof(vec2),
			vTexCoords.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), nullptr);
		glEnableVertexAttribArray(2);
	}
}

void
Mesh::unload()
{
	if (mVAO != NONE) {
		glDeleteVertexArrays(1, &mVAO);
		glDeleteBuffers(1, &mVBO);
		mVAO = NONE;
		mVBO = NONE;

		if (mCBO != NONE) {
			glDeleteBuffers(1, &mCBO);
			mCBO = NONE;
		}

		if (mTCO != NONE) {
			glDeleteBuffers(1, &mTCO);
			mTCO = NONE;
		}
	}
}

void
Mesh::render() const
{
	assert(mVAO != NONE);
	glBindVertexArray(mVAO);
	draw();
}

// -----------------------------------------------------------------------
// Métodos estáticos de generación de mallas
// -----------------------------------------------------------------------

Mesh*
Mesh::createRGBAxes(GLdouble l)
{
	Mesh* mesh = new Mesh();

	mesh->mPrimitive = GL_LINES;
	mesh->mNumVertices = 6;
	mesh->vVertices.reserve(mesh->mNumVertices);

	mesh->vVertices.emplace_back(0.0, 0.0, 0.0);
	mesh->vVertices.emplace_back(l, 0.0, 0.0);
	mesh->vVertices.emplace_back(0.0, 0.0, 0.0);
	mesh->vVertices.emplace_back(0.0, l, 0.0);
	mesh->vVertices.emplace_back(0.0, 0.0, 0.0);
	mesh->vVertices.emplace_back(0.0, 0.0, l);

	mesh->vColors.reserve(mesh->mNumVertices);
	mesh->vColors.emplace_back(1.0, 0.0, 0.0, 1.0);
	mesh->vColors.emplace_back(1.0, 0.0, 0.0, 1.0);
	mesh->vColors.emplace_back(0.0, 1.0, 0.0, 1.0);
	mesh->vColors.emplace_back(0.0, 1.0, 0.0, 1.0);
	mesh->vColors.emplace_back(0.0, 0.0, 1.0, 1.0);
	mesh->vColors.emplace_back(0.0, 0.0, 1.0, 1.0);

	return mesh;
}

Mesh* Mesh::generateRegularPolygon(GLuint num, GLdouble r, GLuint primitiva)
{
	Mesh* mesh = new Mesh();
	mesh->mPrimitive = primitiva;
	mesh->mNumVertices = num;
	mesh->vVertices.reserve(mesh->mNumVertices);

	GLdouble angulo = 90.0;
	for (GLuint i = 0; i < num; i++) {
		GLdouble x = r * cos(radians(angulo));
		GLdouble y = r * sin(radians(angulo));
		mesh->vVertices.emplace_back(x, y, 0.0);
		angulo += 360.0 / num;
	}

	if (primitiva == GL_TRIANGLES) {
		mesh->vColors.push_back({ 1, 0, 0, 1 });
		mesh->vColors.push_back({ 0, 1, 0, 1 });
		mesh->vColors.push_back({ 0, 0, 1, 1 });
	}
	return mesh;
}

Mesh* Mesh::generateRectangle(GLdouble w, GLdouble h)
{
	Mesh* mesh = new Mesh();
	mesh->mPrimitive = GL_TRIANGLE_STRIP;
	mesh->mNumVertices = 4;
	mesh->vVertices.reserve(4);

	GLdouble a = w / 2.0, b = h / 2.0;
	mesh->vVertices.emplace_back(-a, b, 0.0);
	mesh->vVertices.emplace_back(-a, -b, 0.0);
	mesh->vVertices.emplace_back(a, b, 0.0);
	mesh->vVertices.emplace_back(a, -b, 0.0);

	return mesh;
}

Mesh* Mesh::generateRGBRectangle(GLdouble w, GLdouble h)
{
	Mesh* mesh = generateRectangle(w, h);
	mesh->vColors.reserve(4);
	mesh->vColors.push_back({ 1, 0, 0, 1 });
	mesh->vColors.push_back({ 0, 1, 0, 1 });
	mesh->vColors.push_back({ 0, 1, 0, 1 });
	mesh->vColors.push_back({ 0, 0, 1, 1 });
	return mesh;
}

Mesh* Mesh::generateRectangleTexCor(GLdouble w, GLdouble h, GLuint rw, GLuint rh)
{
	Mesh* m = generateRectangle(w, h);
	m->vTexCoords.reserve(4);
	m->vTexCoords.emplace_back(0, rh);
	m->vTexCoords.emplace_back(0, 0);
	m->vTexCoords.emplace_back(rw, rh);
	m->vTexCoords.emplace_back(rw, 0);
	return m;
}

Mesh* Mesh::generateCube(GLdouble length)
{
	Mesh* mesh = new Mesh();
	mesh->mPrimitive = GL_TRIANGLES;
	mesh->mNumVertices = 36;
	mesh->vVertices.reserve(36);

	GLdouble h = length / 2.0;

	// Cara +Z
	mesh->vVertices.emplace_back(-h, -h, h);
	mesh->vVertices.emplace_back(h, -h, h);
	mesh->vVertices.emplace_back(h, h, h);
	mesh->vVertices.emplace_back(-h, -h, h);
	mesh->vVertices.emplace_back(h, h, h);
	mesh->vVertices.emplace_back(-h, h, h);
	// Cara -Z
	mesh->vVertices.emplace_back(h, -h, -h);
	mesh->vVertices.emplace_back(-h, -h, -h);
	mesh->vVertices.emplace_back(-h, h, -h);
	mesh->vVertices.emplace_back(h, -h, -h);
	mesh->vVertices.emplace_back(-h, h, -h);
	mesh->vVertices.emplace_back(h, h, -h);
	// Cara +X
	mesh->vVertices.emplace_back(h, -h, h);
	mesh->vVertices.emplace_back(h, -h, -h);
	mesh->vVertices.emplace_back(h, h, -h);
	mesh->vVertices.emplace_back(h, -h, h);
	mesh->vVertices.emplace_back(h, h, -h);
	mesh->vVertices.emplace_back(h, h, h);
	// Cara -X
	mesh->vVertices.emplace_back(-h, -h, -h);
	mesh->vVertices.emplace_back(-h, -h, h);
	mesh->vVertices.emplace_back(-h, h, h);
	mesh->vVertices.emplace_back(-h, -h, -h);
	mesh->vVertices.emplace_back(-h, h, h);
	mesh->vVertices.emplace_back(-h, h, -h);
	// Cara +Y
	mesh->vVertices.emplace_back(-h, h, h);
	mesh->vVertices.emplace_back(h, h, h);
	mesh->vVertices.emplace_back(h, h, -h);
	mesh->vVertices.emplace_back(-h, h, h);
	mesh->vVertices.emplace_back(h, h, -h);
	mesh->vVertices.emplace_back(-h, h, -h);
	// Cara -Y
	mesh->vVertices.emplace_back(-h, -h, -h);
	mesh->vVertices.emplace_back(h, -h, -h);
	mesh->vVertices.emplace_back(h, -h, h);
	mesh->vVertices.emplace_back(-h, -h, -h);
	mesh->vVertices.emplace_back(h, -h, h);
	mesh->vVertices.emplace_back(-h, -h, h);

	return mesh;
}

Mesh* Mesh::generateBoxOutline(GLdouble length)
{
	Mesh* mesh = new Mesh();
	mesh->mPrimitive = GL_TRIANGLE_STRIP;
	mesh->mNumVertices = 10;
	mesh->vVertices.reserve(10);

	GLdouble h = length / 2.0;

	mesh->vVertices.push_back(glm::vec3(-h, h, h));
	mesh->vVertices.push_back(glm::vec3(-h, -h, h));
	mesh->vVertices.push_back(glm::vec3(h, h, h));
	mesh->vVertices.push_back(glm::vec3(h, -h, h));
	mesh->vVertices.push_back(glm::vec3(h, h, -h));
	mesh->vVertices.push_back(glm::vec3(h, -h, -h));
	mesh->vVertices.push_back(glm::vec3(-h, h, -h));
	mesh->vVertices.push_back(glm::vec3(-h, -h, -h));
	mesh->vVertices.push_back(glm::vec3(-h, h, h));
	mesh->vVertices.push_back(glm::vec3(-h, -h, h));

	return mesh;
}

Mesh* Mesh::generateBoxOutlineTexCor(GLdouble length)
{
	Mesh* m = generateBoxOutline(length);
	m->vTexCoords.reserve(10);

	m->vTexCoords.push_back(glm::vec2(0.0, 1.0));
	m->vTexCoords.push_back(glm::vec2(0.0, 0.0));
	m->vTexCoords.push_back(glm::vec2(1.0, 1.0));
	m->vTexCoords.push_back(glm::vec2(1.0, 0.0));
	m->vTexCoords.push_back(glm::vec2(2.0, 1.0));
	m->vTexCoords.push_back(glm::vec2(2.0, 0.0));
	m->vTexCoords.push_back(glm::vec2(3.0, 1.0));
	m->vTexCoords.push_back(glm::vec2(3.0, 0.0));
	m->vTexCoords.push_back(glm::vec2(4.0, 1.0));
	m->vTexCoords.push_back(glm::vec2(4.0, 0.0));

	return m;
}

Mesh* Mesh::generateRGBCubeTriangles(GLdouble length)
{
	Mesh* mesh = generateCube(length);
	mesh->vColors.reserve(36);

	// 4 caras rojas (24 vértices)
	for (int i = 0; i < 24; ++i) mesh->vColors.push_back({ 1, 0, 0, 1 });
	// 1 cara azul (6 vértices)
	for (int i = 0; i < 6; ++i) mesh->vColors.push_back({ 0, 0, 1, 1 });
	// 1 cara verde (6 vértices)
	for (int i = 0; i < 6; ++i) mesh->vColors.push_back({ 0, 1, 0, 1 });

	return mesh;
}

Mesh* Mesh::generateStar3D(GLdouble re, GLuint np, GLdouble h)
{
	Mesh* mesh = new Mesh();
	mesh->mPrimitive = GL_TRIANGLE_FAN;

	int totalVertices = 2 * np;
	mesh->mNumVertices = 2 + totalVertices;
	mesh->vVertices.reserve(mesh->mNumVertices);

	mesh->vVertices.push_back(glm::vec3(0, 0, 0));

	GLdouble ri = re / 2.0;
	GLdouble angleStep = 360.0 / totalVertices;

	for (int i = 0; i <= totalVertices; i++) {
		GLdouble r = (i % 2 == 0) ? re : ri;
		GLdouble angle = glm::radians(90.0 + i * angleStep);
		mesh->vVertices.push_back(glm::vec3(r * cos(angle), r * sin(angle), h));
	}

	return mesh;
}

Mesh* Mesh::generateStar3DTexCor(GLdouble re, GLuint np, GLdouble h)
{
	Mesh* m = generateStar3D(re, np, h);
	m->vTexCoords.reserve(m->mNumVertices);

	m->vTexCoords.push_back(glm::vec2(0.5f, 0.5f));

	int total = 2 * np;
	GLdouble step = 360.0 / total;

	for (int i = 0; i <= total; i++) {
		GLdouble r = (i % 2 == 0) ? 0.5 : 0.25;
		GLdouble angle = glm::radians(90.0 + i * step);
		m->vTexCoords.push_back(glm::vec2(
			0.5f + (GLfloat)(r * cos(angle)),
			0.5f + (GLfloat)(r * sin(angle))));
	}

	return m;
}

// -----------------------------------------------------------------------
// generateByRevolution
// Rota el perfil 2D (en el plano XY) nMuestras veces alrededor del eje Y,
// generando triángulos entre muestras adyacentes. Los triángulos degenerados
// (vértice en el eje Y, x==0) se descartan para evitar polígonos planos.
// -----------------------------------------------------------------------
Mesh* Mesh::generateByRevolution(const vector<vec2>& perfil, int nMuestras)
{
	Mesh* mesh = new Mesh;
	mesh->mPrimitive = GL_TRIANGLES;

	// Genera nMuestras+1 copias del perfil rotadas (la última coincide con la primera)
	vector<vector<vec3>> vs(nMuestras + 1);
	GLdouble theta1 = 2.0 * numbers::pi / nMuestras;

	for (int i = 0; i <= nMuestras; ++i) {
		GLdouble c = cos(i * theta1), s = sin(i * theta1);
		vs[i].reserve(perfil.size());
		for (auto p : perfil)
			// Rotación Ry(θi): (x,y) → (x·cos, y, -x·sin)
			vs[i].emplace_back(p.x * c, p.y, -p.x * s);
	}

	// Conecta muestras adyacentes i y i+1 con dos triángulos por franja
	for (int i = 0; i < nMuestras; ++i) {
		for (int j = 0; j < (int)perfil.size() - 1; ++j) {
			//      Pi,j+1 ---- Pi+1,j+1
			//        |    \       |
			//      Pi,j  ---- Pi+1,j

			// Triángulo inferior (Pi,j — Pi,j+1 — Pi+1,j)
			// Se descarta si el vértice inferior está sobre el eje Y (x==0 → cara plana)
			if (perfil[j].x != 0.0f) {
				mesh->vVertices.push_back(vs[i][j]);
				mesh->vVertices.push_back(vs[i][j + 1]);
				mesh->vVertices.push_back(vs[i + 1][j]);
			}

			// Triángulo superior (Pi,j+1 — Pi+1,j+1 — Pi+1,j)
			// Se descarta si el vértice superior está sobre el eje Y
			if (perfil[j + 1].x != 0.0f) {
				mesh->vVertices.push_back(vs[i][j + 1]);
				mesh->vVertices.push_back(vs[i + 1][j + 1]);
				mesh->vVertices.push_back(vs[i + 1][j]);
			}
		}
	}

	mesh->mNumVertices = static_cast<GLuint>(mesh->vVertices.size());
	return mesh;
}

// -----------------------------------------------------------------------
// IndexMesh
// -----------------------------------------------------------------------

IndexMesh::~IndexMesh()
{
	// El EBO se libera antes que el VAO (que libera Mesh::unload)
	if (mEBO != 0) {
		glDeleteBuffers(1, &mEBO);
		mEBO = 0;
	}
}

// Sube vértices (via Mesh::load) y además el EBO con los índices
void IndexMesh::load()
{
	Mesh::load(); // sube VAO + VBOs de posición / color / textura

	// Añadir el EBO al VAO que acaba de crear Mesh::load
	glBindVertexArray(mVAO);
	glGenBuffers(1, &mEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		vIndices.size() * sizeof(GLuint),
		vIndices.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);
}

// Renderiza usando glDrawElements en lugar de glDrawArrays
void IndexMesh::draw() const
{
	glDrawElements(mPrimitive,
		static_cast<GLsizei>(vIndices.size()),
		GL_UNSIGNED_INT,
		nullptr);
}

// Cubo con 8 vértices únicos y 36 índices (12 triángulos, 2 por cara)
IndexMesh* IndexMesh::generateIndexCube(GLdouble length)
{
	auto* mesh = new IndexMesh;
	mesh->mPrimitive = GL_TRIANGLES;

	GLdouble h = length / 2.0;

	// Los 8 vértices únicos del cubo centrado en el origen
	mesh->vVertices = {
		{ (float)h, (float)h, (float)-h },  // 0
		{ (float)h, (float)-h, (float)-h },  // 1
		{ (float)h, (float)h, (float)h },  // 2
		{ (float)h, (float)-h, (float)h },  // 3
		{ (float)-h, (float)h, (float)h },  // 4
		{ (float)-h, (float)-h, (float)h },  // 5
		{ (float)-h, (float)h, (float)-h },  // 6
		{ (float)-h, (float)-h, (float)-h }   // 7
	};

	// 36 índices: 12 triángulos, en sentido antihorario visto desde fuera
	mesh->vIndices = {
		0, 1, 2,  2, 1, 3,   // cara +X
		2, 3, 4,  4, 3, 5,   // cara +Z
		4, 5, 6,  6, 5, 7,   // cara -X
		6, 7, 0,  0, 7, 1,   // cara -Z
		4, 6, 2,  2, 6, 0,   // cara +Y (techo)
		1, 7, 3,  3, 7, 5    // cara -Y (suelo)
	};

	// mNumVertices = nº de vértices únicos (para poder usar Mesh::load)
	mesh->mNumVertices = static_cast<GLuint>(mesh->vVertices.size());
	return mesh;
}