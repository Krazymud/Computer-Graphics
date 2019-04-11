#ifndef CAMERA_H
#define CAMERA_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

class Camera {
public:
	Camera(GLfloat c_speed);
	void moveForward();
	void moveBack();
	void moveRight();
	void moveLeft();
	void move(GLfloat x, GLfloat y, GLboolean constrainPitch);
	void rotate(GLfloat const pitch, GLfloat const yaw);
	void setCameraSpeed(GLfloat speed);
	void updateViewMat();
	glm::mat4 GetViewMatrix();
	glm::mat4 calculateView(float yaw, float pitch, float roll, glm::vec3 pos);
	
private:
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	GLfloat Yaw, Pitch;
	GLfloat cameraSpeed;
	glm::quat MyQuaternion;
};

#endif