#include "transform.h"

#define GLM_ENABLE_EXPERIMENTAL

#include "../deps/glm/gtc/quaternion.hpp"
#include "../deps/glm/gtx/quaternion.hpp"
#include "../deps/glm/gtc/matrix_transform.hpp"
#include "../deps/glm/gtx/matrix_decompose.hpp"


namespace sp {


	glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest)
	{
		start = normalize(start);
		dest = normalize(dest);

		float cosTheta = dot(start, dest);
		glm::vec3 rotationAxis;

		if (cosTheta < -1 + 0.001f)
		{
			// special case when vectors in opposite directions :
			// there is no "ideal" rotation axis
			// So guess one; any will do as long as it's perpendicular to start
			// This implementation favors a rotation around the Up axis,
			// since it's often what you want to do.
			rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
			if (glm::length2(rotationAxis) < 0.01) // bad luck, they were parallel, try again!
				rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);

			rotationAxis = normalize(rotationAxis);
			return angleAxis(glm::radians(180.0f), rotationAxis);
		}

		// Implementation from Stan Melax's Game Programming Gems 1 article
		rotationAxis = cross(start, dest);

		float s = sqrt((1 + cosTheta) * 2);
		float invs = 1 / s;

		return glm::quat(
			s * 0.5f,
			rotationAxis.x * invs,
			rotationAxis.y * invs,
			rotationAxis.z * invs);
	}

	Transform::Transform(glm::vec3 position, glm::vec3 scale, std::function<void()> callback)
		: _position(position),
		_scale(scale),
		_orientation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
		_callback(callback)
	{
		computeValues();
	}

	glm::vec3 Transform::getOrientationEular()
	{
		return glm::eulerAngles(_orientation);
	}

	void Transform::setPosition(glm::vec3 pos)
	{
		_position = pos;
		computeValues();
	}

	void Transform::setPosition(float x, float y, float z)
	{
		_position = glm::vec3(x, y, z);
		computeValues();
	}

	void Transform::setScale(glm::vec3 scale)
	{
		_scale = scale;
		computeValues();
	}

	void Transform::setScale(float scale)
	{
		_scale = glm::vec3(scale);
		computeValues();
	}

	void Transform::setOrientation(glm::quat orientation)
	{
		_orientation = orientation;
		computeValues();
	}

	void Transform::setOrientationEular(float yaw, float pitch, float roll)
	{
		glm::vec3 euler(pitch, yaw, roll);
		_orientation = glm::quat(euler);
		computeValues();
	}

	void Transform::setOrientationAxisAngle(glm::vec3 axis, float angle)
	{
		_orientation = glm::angleAxis(angle, axis);
		computeValues();
	}

	void Transform::setModelMatrix(glm::mat4 m)
	{
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(m, _scale, _orientation, _position, skew, perspective);
		computeValues();
	}

	void Transform::translate(glm::vec3 dp)
	{
		_position += dp;
		computeValues();
	}

	void Transform::translate(float x, float y, float z)
	{
		_position += glm::vec3(x, y, z);
		computeValues();
	}

	void Transform::translateForword(float distance)
	{
		_position += _front * distance;
		computeValues();
	}

	void Transform::translateDirection(glm::vec3 direction, float distance)
	{
		direction = glm::normalize(direction);
		_position += direction * distance;
		computeValues();
	}

	void Transform::rotate(float angle, glm::vec3 axis)
	{
		axis = glm::normalize(axis);
		glm::quat angl = glm::rotate(glm::mat4(1.0f), angle, axis);
		_orientation = angl * _orientation;
		computeValues();
	}

	void Transform::rotate(float yaw, float pitch, float z)
	{
		glm::vec3 euler(pitch, yaw, z);
		glm::quat angle = glm::quat(euler);
		_orientation = angle * _orientation;
		computeValues();
	}

	void Transform::rotateTo(float yaw, float pitch, float roll, float amount)
	{
		glm::vec3 euler(pitch, yaw, roll);
		glm::quat angle = glm::quat(euler);
		_orientation = glm::mix(_orientation, angle, amount);
		computeValues();
	}

	void Transform::rotateTo(float angle, glm::vec3 axis, float amount)
	{
		axis = glm::normalize(axis);
		glm::quat angl = glm::angleAxis(angle, axis);
		_orientation = glm::mix(_orientation, angl, amount);
		computeValues();
	}

	void Transform::rotateTo(glm::quat quat, float amount)
	{
		_orientation = glm::mix(_orientation, quat, amount);
		computeValues();
	}

	void Transform::lookAt(glm::vec3 point)
	{
		glm::vec3 direction = glm::normalize(point - _position);
		_orientation = glm::quatLookAtLH(direction, cnst_direction_world_up);
		computeValues();
	}

	void Transform::computeValues()
	{
		glm::mat4 position = glm::mat4(1.0),
			scale = glm::mat4(1.0);

		position = glm::translate(position, _position);
		glm::mat4 rotation = glm::toMat4(_orientation);
		scale = glm::scale(scale, _scale);
		_modelMatrix = position * rotation * scale;

		_front = _orientation * cnst_direction_world_forword;
		_front = glm::normalize(_front);
		_right = glm::normalize(glm::cross(_front, cnst_direction_world_up));
		_up = glm::normalize(glm::cross(_right, _front));
		if (_callback != nullptr)
			_callback();
	}





};