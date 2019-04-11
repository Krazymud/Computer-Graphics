#include <iostream>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <thread>
#include "data.h"
#include "camera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
const char *glsl_version = "#version 330";
bool moving = false, firstMouse = true;
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
Camera m_camera(0.05);

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

														 // glfw window creation
														 // --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//style
	ImGui::StyleColorsClassic();
	//binding
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	ImVec4 clear_color = ImVec4(0.2f, 0.3f, 0.3f, 1.0f);


	// build and compile our shader program
	Shader shader("shader.vs", "shader.fs");

	// set up vertex data (and buffer(s)) and configure vertex attributes

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	//binding vertices
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//texture

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// render loop
	while (!glfwWindowShouldClose(window))
	{
		
		// input
		processInput(window);


		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		//start the imgui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//show a window
		static bool projection = false;
		static bool view_c = false;
		static bool fps = false;
		static float f = 0.0f;

		ImGui::Begin("Camera\n");
		ImGui::Checkbox("projection", &projection);
		ImGui::Checkbox("view_c", &view_c);
		ImGui::Checkbox("fps", &fps);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
		shader.use();

		int screenWidth, screenHeight;
		glfwGetWindowSize(window, &screenWidth, &screenHeight);
		glm::mat4 model;
		// model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 view;
		glm::mat4 proj;
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));
		
		if (projection || view_c || fps) {
			if (projection) {
				moving = false;
				model = glm::translate(model, glm::vec3(-1.5f, 0.5f, -1.5f));
				ImGui::Begin("Types of projection");
				static bool ortho = false, pers = false;
				static float left = -10.0f, right = 10.0f, bottom = -10.0f, top = 10.0f,
					z_near = 0.1f, z_far = 30.0f, fov = 45.0f, ratio = (float)screenWidth / (float)screenHeight;
				ImGui::Checkbox("orthographic", &ortho);
				ImGui::Checkbox("Perspective", &pers);
				if (ortho) {
					ImGui::Begin("Modify the parameters");
					ImGui::SliderFloat("left", &left, -100.0f, 0.0f);
					ImGui::SliderFloat("right", &right, 0.0f, 100.0f);
					ImGui::SliderFloat("bottom", &bottom, -100.0f, 0.0f);
					ImGui::SliderFloat("top", &top, 0.0f, 100.0f);
					ImGui::SliderFloat("near", &z_near, 0.1f, 10.0f);
					ImGui::SliderFloat("far", &z_far, 10.0f, 50.0f);
					if (ImGui::Button("Close Me")) {
						ortho = false;
					}
					ImGui::End();
					proj = glm::ortho(left, right, bottom, top, z_near, z_far);
				}
				if (pers) {
					ImGui::Begin("Modify the parameters");
					ImGui::SliderFloat("fov", &fov, 0.0f, 100.0f);
					ImGui::SliderFloat("rate", &ratio, 0.0f, 10.0f);
					ImGui::SliderFloat("near", &z_near, 0.1f, 10.0f);
					ImGui::SliderFloat("far", &z_far, 10.0f, 50.0f);
					if (ImGui::Button("Close Me")) {
						pers = false;
					}
					ImGui::End();
					proj = glm::perspective(glm::radians(fov), ratio, z_near, z_far);
				}
				if (ImGui::Button("Close Me")) {
					projection = false;
				}
				ImGui::End();
			}
			else if (view_c) {
				moving = false;
				proj = glm::perspective(glm::radians(45.0f), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
				static int start = 0;
				static bool flag = true;
				ImGui::Begin("View Changing");
				ImGui::RadioButton("Start", &start, 1);
				ImGui::RadioButton("Stop", &start, 0);
				if (start) {
					static float ini_time = glfwGetTime();
					if (flag == true) {
						ini_time = glfwGetTime();
						flag = false;
					}
					float curr_time = glfwGetTime();
					float radius = 10.0f, camX = sin(curr_time - ini_time) * radius,
						camZ = cos(curr_time - ini_time) * radius;
					view = glm::lookAt(glm::vec3(camX, 0.0f, camZ),
						glm::vec3(0.0f, 0.0f, 0.0f),
						glm::vec3(0.0f, 1.0f, 0.0f));
				}
				else {
					flag = true;
				}
				
				if (ImGui::Button("Close Me")) {
					view_c = false;
				}
				ImGui::End();
			}
			else if (fps) {
				moving = true;
				//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				proj = glm::perspective(glm::radians(45.0f), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
				view = m_camera.GetViewMatrix();
			}
			shader.setMat4("model", model);
			shader.setMat4("projection", proj);
			shader.setMat4("view", view);
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// render
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}

// process all input
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (moving == true) {
		m_camera.setCameraSpeed(2.5f * deltaTime);
		float cameraSpeed = 0.05f;
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			m_camera.moveForward();
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			m_camera.moveBack();
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			m_camera.moveLeft();
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			m_camera.moveRight();
		}
	}
}

//window size changed
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	m_camera.move(xoffset, yoffset, true);
}