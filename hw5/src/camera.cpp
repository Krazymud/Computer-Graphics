#include "camera.h"
Camera::Camera(GLfloat c_speed) {
	cameraSpeed = c_speed;
	cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
	cameraFront = glm::vec3(0.0f, 0.0f, -10.0f);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	Yaw = -90.0f;
	Pitch = 0.0f;
}

void Camera::moveForward() {
	cameraPos += cameraSpeed * cameraFront;
}

void Camera::moveBack() {
	cameraPos -= cameraSpeed * cameraFront;
}

void Camera::moveRight() {
	cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void Camera::moveLeft() {
	cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void Camera::move(GLfloat x, GLfloat y, GLboolean constrainPitch) {
	x *= 0.1f;
	y *= 0.1f;

	Yaw += x;
	Pitch += y;

	if (constrainPitch)
	{
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}

	updateViewMat();
}

void Camera::updateViewMat() {
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	cameraFront = glm::normalize(front);
	// Also re-calculate the Right and Up vector
	cameraUp = glm::normalize(glm::cross(glm::normalize(glm::cross(cameraFront, cameraUp)), cameraFront));
}

/*glm::mat4 Camera::calculateView(float yaw, float pitch, float roll, glm::vec3 pos) {
	glm::quat key_quat = glm::quat(glm::vec3(pitch, yaw, roll));

	camera_quat = key_quat * camera_quat;

}*/

glm::mat4 Camera::GetViewMatrix() {
	return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

void Camera::setCameraSpeed(GLfloat speed) {
	cameraSpeed = speed;
}

void Camera::rotate(GLfloat const pitch, GLfloat const yaw) {

}