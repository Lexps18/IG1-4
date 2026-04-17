#include "Shader.h"
#include "Camera.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_access.hpp>

using namespace glm;

Camera::Camera(Viewport* vp)
	: mViewMat(1.0)
	, mProjMat(1.0)
	, xRight(vp->width() / 2.0)
	, xLeft(-xRight)
	, yTop(vp->height() / 2.0)
	, yBot(-yTop)
	, mViewPort(vp)
{
	setPM();
	setAxes();
}

void Camera::uploadVM() const
{
	Shader::setUniform4All("modelView", mViewMat);
}

void Camera::setVM()
{
	mViewMat = lookAt(mEye, mLook, mUp); // glm::lookAt defines the view matrix
	setAxes();
}

void Camera::set2D()
{
	mEye = { 0, 0, 500 };
	mLook = { 0, 0, 0 };
	mUp = { 0, 1, 0 };
	mRadio = 500.0;
	mAng = 90.0;

	setVM();
}

void Camera::set3D()
{
	mEye = { 500, 500, 500 };
	mLook = { 0, 10, 0 };
	mUp = { 0, 1, 0 };
	mRadio = 707.1;
	mAng = 45.0;

	setVM();
}

void Camera::setCenital()
{
	//colocamos el ojo en el eje Y
	mEye = { 0, 500, 0 };
	mLook = { 0, 0, 0 };
	mUp = { 0, 0, -1 };

	mRadio = 500.0;
	mAng = 90.0;

	setVM();
}

void Camera::pitch(GLfloat a)
{
	mViewMat = rotate(mViewMat, radians(a), vec3(1.0f, 0.0f, 0.0f));
	// glm::rotate returns mViewMat * rotationMatrix
	setAxes();
}
void Camera::yaw(GLfloat a)
{
	mViewMat = rotate(mViewMat, radians(a), vec3(0.0f, 1.0f, 0.0f));
	// glm::rotate returns mViewMat * rotationMatrix
	setAxes();
}
void Camera::roll(GLfloat a)
{
	mViewMat = rotate(mViewMat, radians(a), vec3(0.0f, 0.0f, 1.0f));
	// glm::rotate returns mViewMat * rotationMatrix
	setAxes();
}

void Camera::setSize(GLdouble xw, GLdouble yh)
{
	xRight = xw / 2.0;
	xLeft = -xRight;
	yTop = yh / 2.0;
	yBot = -yTop;
	setPM();
}

void Camera::setScale(GLdouble s)
{
	mScaleFact -= s;
	if (mScaleFact < 0.0) mScaleFact = 0.01;
	setPM();
}

void Camera::setPM()
{
	if (bOrto) {
		// Proyección ortogonal: los límites se escalan directamente
		mProjMat = ortho(
			(float)(xLeft * mScaleFact),
			(float)(xRight * mScaleFact),
			(float)(yBot * mScaleFact),
			(float)(yTop * mScaleFact),
			(float)mNearVal,
			(float)mFarVal);
	}
	else {
		// Proyección perspectiva con frustum.
		// La "ventana de vista" (xLeft..xRight, yBot..yTop) está definida a la
		// distancia |eye - look|. Para trasladarla al plano cercano (mNearVal)
		// se aplica la proporción de triángulos semejantes: factor = near / dist.
		GLdouble dist = length(mEye - mLook);       // distancia ojo → look
		GLdouble ratio = mNearVal / dist;             // escala al plano cercano

		mProjMat = frustum(
			(float)(xLeft * mScaleFact * ratio),
			(float)(xRight * mScaleFact * ratio),
			(float)(yBot * mScaleFact * ratio),
			(float)(yTop * mScaleFact * ratio),
			(float)mNearVal,
			(float)mFarVal);
	}
}

void Camera::uploadPM() const
{
	Shader::setUniform4All("projection", mProjMat);
}

void Camera::upload() const
{
	mViewPort->upload();
	uploadVM();
	uploadPM();
}

void Camera::setAxes() {
	mRight = row(mViewMat, 0);
	mUpward = row(mViewMat, 1);
	mFront = -row(mViewMat, 2);
}

void Camera::moveLR(GLfloat cs) {
	mEye += mRight * (float)cs;
	mLook += mRight * (float)cs;
	setVM();
}
void Camera::moveFB(GLfloat cs) {
	mEye += mFront * (float)cs;
	mLook += mFront * (float)cs;
	setVM();
}
void Camera::moveUD(GLfloat cs) {
	mEye += mUpward * (float)cs;
	mLook += mUpward * (float)cs;
	setVM();
}

void Camera::changePrj() {
	bOrto = !bOrto; // alterna entre ortogonal y perspectiva
	setPM();
}

void Camera::pitchReal(GLfloat cs)
{
	// Rota el vector de vista (look - eye) sobre el eje derecho de la cámara
	glm::vec3 focus = mLook - mEye;
	glm::mat4 rot = rotate(mat4(1.0f), radians(cs), mRight);
	focus = rot * vec4(focus, 0.0f);
	mUp = rot * vec4(mUp, 0.0f); // mUp también rota sobre mRight
	mLook = mEye + focus;
	setVM();
}

void Camera::yawReal(GLfloat cs)
{
	// Rota el vector de vista sobre el eje vertical de la cámara
	glm::vec3 focus = mLook - mEye;
	glm::mat4 rot = rotate(mat4(1.0f), radians(cs), mUpward);
	focus = rot * vec4(focus, 0.0f);
	mUp = rot * vec4(mUp, 0.0f); // mUp también rota sobre mUpward
	mLook = mEye + focus;
	setVM();
}

void Camera::rollReal(GLfloat cs)
{
	// Inclina la cámara rotando mUp sobre el eje de visión
	mUp = rotate(mat4(1.0f), radians(cs), mFront) * vec4(mUp, 0.0f);
	setVM();
}

void Camera::orbit(GLdouble incAng, GLdouble incY) {
	mAng += incAng;
	mEye.x = mLook.x + (float)(cos(radians(mAng)) * mRadio);
	mEye.z = mLook.z - (float)(sin(radians(mAng)) * mRadio);
	mEye.y += (float)incY;
	mUp = { 0.0f, 1.0f, 0.0f }; // orbit es global sobre Y → resetear mUp
	setVM();
}