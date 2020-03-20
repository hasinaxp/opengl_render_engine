#pragma once
#include "../api.h"
#include "transform.h"


namespace sp {

	const float cnst_default_cam_speed = 10.0f;
	const glm::vec3 cnst_default_cam_position = glm::vec3(0.0f, 0.3f, 3.0f);
	const glm::vec3 cnst_default_cam_position_2d = glm::vec3(0.0f, 0.0f, 1.0f);
	const float cnst_default_cam_angle = cnst_pi * 75 / 180;


	//class responsible for camera control in 3d world or 2d world
	class SP_API Camera
	{
	private:
		Transform _transform;
		bool _invert_x;
		bool _invert_y;
		float _speed;
		float _sencitivity_x;
		float _sencitivity_y;
		glm::mat4 _view_matrix;
		glm::mat4 _projection_matrix;


	public:
		//use orthogonal projection for 2d camera and perspective projection for 3d camera
		Camera(glm::mat4 projectionMatrix, glm::vec3 position = cnst_default_cam_position, float speed = cnst_default_cam_speed);

		void invert_x(bool invert); // invert x mouse control of camera
		void invert_y(bool invert); // invert y mouse control of camera

		// move camera in the world 
		void moveForword(float dt);
		void moveBackword(float dt);
		void moveLeft(float dt);
		void moveRight(float dt);
		void moveUp(float dt);
		void moveDown(float dt);

		//rotate camera
		void rotateCam(float x, float y);

		//generate projection matrix
		static glm::mat4 genPerspectiveProjectionMatrix(float fov = cnst_default_cam_angle, int width = 640, int height = 480, float near = 0.01f, float far = 400.0f);
		static glm::mat4 genOrthoProjectionMatrix(float left, float right, float top, float bottom, float near = 10.0f, float far = -100.0f);
		static glm::mat4 genOrthoMatrix(float scaleX, float scaleY, bool originCenter = false, float near = 10.0f, float far = -100.0f);


		// get camera's different values
		Transform& getTransform() { return _transform; }
		glm::mat4 getViewMatrix() const { return _view_matrix; }
		glm::mat4& getViewMatrixRef() { return _view_matrix; }
		glm::mat4 getProjectionMatrix() const { return _projection_matrix; };
		glm::mat4& getProjectionMatrixRef() { return _projection_matrix; };
		glm::mat4 getViewProjectionMatrix() const { return   _projection_matrix * _view_matrix; };
		
		//set camera movement constants
		float getSpeed() const { return _speed; }
		float getSencitivity_x() const { return _sencitivity_x; }
		float getSencitivity_y() const { return _sencitivity_y; }
		void setSpeed(float speed) { _speed = speed; }
		void setSencitivity_x(float s) { _sencitivity_x = s; }
		void setSencitivity_y(float s) { _sencitivity_y = s; }
		
		//change camera's projection
		void setProjectionMatrix(glm::mat4 mat) { _projection_matrix = mat; }
	private:
		virtual void calculateMatrices();
	};
	


};