#pragma once
#include "../api.h"
#include "../deps/glm/gtc/quaternion.hpp"
#include "../deps/glm/glm.hpp"
#include <functional>

#define sp_convert_transfrom_callback(x) std::bind(&x, this)


namespace sp {



	//constants for world direction
	const glm::vec3 cnst_direction_world_up = glm::vec3(0.0f, 1.0f, 0.0f);
	const glm::vec3 cnst_direction_world_left = glm::vec3(1.0f, 0.0f, 0.0f);
	const glm::vec3 cnst_direction_world_forword = glm::vec3(0.0f, 0.0f, 1.0f);
	const float cnst_pi = 3.1415926536f;

	//responsible for repersenting 3d transformation
	//i.e. position rotation scale
	//angle mode is radian by default
	class SP_API Transform 
	{
	private:
		glm::vec3 _position;
		glm::vec3 _scale;
		glm::quat _orientation;
		glm::mat4 _modelMatrix;
		glm::vec3 _front;
		glm::vec3 _right;
		glm::vec3 _up;
		std::function<void()> _callback;

	public:
		//note:	callback is a void function pointer to be called each time the transform is modified
		Transform(glm::vec3 position = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f), std::function<void()> callback = nullptr);


		//retrive different informations regarding transform
		glm::vec3 getPosition() const { return _position; }
		glm::vec3 getScale() const { return _scale; }
		glm::quat getOrientation() const { return _orientation; }
		glm::mat4 getModelMatrix() const { return _modelMatrix; }
		glm::vec3 getDirectionFront() const { return _front; }
		glm::vec3 getDirectionRight() const { return _right; }
		glm::vec3 getDirectionUp() const { return _up; }
		glm::vec3 getOrientationEular();

		glm::vec3& getPositonRef() { return _position; }
		glm::quat& getOrientationRef() { return _orientation; }
		glm::mat4& getModelMatrixRef() { return _modelMatrix; }
		glm::vec3& getDirectionFrontRef() { return _front; }
		glm::vec3& getDirectionRightRef() { return _right; }
		glm::vec3& getDirectionUpRef() { return _up; }

		//set values of transform directly
		void setPosition(glm::vec3 pos);
		void setPosition(float x, float y, float z);
		void setScale(glm::vec3 scale);
		void setScale(float scale);
		void setOrientation(glm::quat orientation);
		void setOrientationEular(float yaw, float pitch, float roll);
		void setOrientationAxisAngle(glm::vec3 axis, float angle);
		void setModelMatrix(glm::mat4 m);

		// set the callback function that will be called whenever there is a change in transform
		void setCallback(std::function<void()> callback) { _callback = callback; }

		//mode from current position
		void translate(glm::vec3 dp);
		void translate(float x, float y, float z);
		void translateForword(float distance);
		void translateDirection(glm::vec3 direction, float distance);

		//change rotation from current rotation
		void rotate(float angle = 0, glm::vec3 axis = glm::vec3(0.0f, 1.0f, 0.0f));
		void rotate(float yaw, float pitch, float z = 0.0f);
		void rotateTo(float yaw, float pitch = 0.0f, float roll = 0.0f, float amount = 1.0f);
		void rotateTo(float angle = 0.0f, glm::vec3 axis = glm::vec3(0.0f, 1.0f, 0.0f), float amount = 1.0f);
		void rotateTo(glm::quat quat, float amount = 1.0f);


		// make the model transformation face towords a point
		void lookAt(glm::vec3 point);

	private:
		void computeValues();
	};



};