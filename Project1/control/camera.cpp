#include "camera.h"
#include "../deps/glm/gtc/matrix_transform.hpp"
#include <iostream>
namespace sp{

	Camera::Camera(glm::mat4 projectionMatrix, glm::vec3 position, float speed)
		:_transform(position),
		_invert_x(true),
		_invert_y(false),
		_speed(speed),
		_sencitivity_x(0.005),
		_sencitivity_y(0.003),
		_projection_matrix(projectionMatrix)
	{

		_transform.setCallback(sp_convert_transfrom_callback(Camera::calculateMatrices));
		_transform.lookAt({ 0, 0, 0 });
	}

	void Camera::invert_x(bool invert)
	{
		_invert_x = invert;
	}

	void Camera::invert_y(bool invert)
	{
		_invert_y = invert;
	}



	void Camera::moveForword(float dt)
	{
		_transform.translate(_transform.getDirectionFront() * _speed * dt);
	}

	void Camera::moveBackword(float dt)
	{
		_transform.translate(-(_transform.getDirectionFront()) * _speed * dt);
	}

	void Camera::moveLeft(float dt)
	{
		_transform.translate(-(_transform.getDirectionRight()) * _speed * dt);
	}

	void Camera::moveRight(float dt)
	{
		_transform.translate(_transform.getDirectionRight() * _speed * dt);
	}

	void Camera::moveUp(float dt)
	{
		_transform.translate(cnst_direction_world_up * _speed * dt);
	}

	void Camera::moveDown(float dt)
	{
		_transform.translate(-cnst_direction_world_up * _speed * dt);
	}

	void Camera::rotateCam(float x, float y)
	{
		_transform.rotate(x * (_invert_x? 1.0f : -1.0f) * _sencitivity_x, y * (_invert_y? 1.0f : -1.0f) * _sencitivity_y);
	}

	glm::mat4 Camera::genPerspectiveProjectionMatrix(float fov, int width, int height, float near, float far)
	{
		return glm::perspective(fov, (float)width / height, near, far);
	}

	glm::mat4 Camera::genOrthoProjectionMatrix(float left, float right, float top, float bottom, float near, float far)
	{
		return glm::ortho(left, right, bottom, top, near, far);
	}

	glm::mat4 Camera::genOrthoMatrix(float scaleX, float scaleY, bool originCenter, float, float)
	{
		float left = originCenter ? (-scaleX / 2) : 0;
		float right = originCenter ? (scaleX / 2) : scaleX;
		float top = originCenter ? (scaleY / 2) : 0;
		float bottom = originCenter ? (-scaleY / 2) : scaleY;
		return genOrthoProjectionMatrix(left, right, top, bottom);
	}

	void Camera::calculateMatrices()
	{
		_view_matrix = glm::lookAt(_transform.getPosition(), _transform.getPosition() + _transform.getDirectionFront(), _transform.getDirectionUp());
	}

};