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
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// settings
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;
const char *glsl_version = "#version 330";
bool moving = true, firstMouse = true;
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
Camera m_camera(glm::vec3(0.0f, 0.0f, 5.0f));

glm::vec3 lightPos(0.0f, 0.8f, 1.8f);


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
	//glfwSetCursorPosCallback(window, mouse_callback);
	//glfwSetScrollCallback(window, scroll_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glEnable(GL_DEPTH_TEST);

	//setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//style
	ImGui::StyleColorsClassic();
	//binding
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// build and compile our shader program
	Shader phongShader("phong.vs", "phong.fs");
	Shader gouraudShader("gouraud.vs", "gouraud.fs");
	Shader lightShader("light.vs", "light.fs");
	// set up vertex data (and buffer(s)) and configure vertex attributes

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	//binding vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// render loop
	while (!glfwWindowShouldClose(window))
	{
		
		// input
		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//start the imgui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//show a window
		static bool phong = false;
		static bool gouraud = false;
		static float f = 0.0f;

		ImGui::Begin("Lights and Shading\n");
		ImGui::Checkbox("Phong Shading", &phong);
		ImGui::Checkbox("Gouraud Shading", &gouraud);
		
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();

		int screenWidth, screenHeight;
		glfwGetWindowSize(window, &screenWidth, &screenHeight);
		glm::mat4 model;
		glm::mat4 view = m_camera.GetViewMatrix();
		glm::mat4 proj = glm::perspective(m_camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		

		if (phong) {
			static bool move = false;
			static float ambient = 0.1;
			static float specular = 0.5;
			static float diffuse = 1.0;
			static int reflecting = 32;
			
			ImGui::Begin("Phong Shading");
			ImGui::SetWindowSize(ImVec2(400, 170));
			ImGui::SliderFloat("ambient", &ambient, 0.0, 1.0);
			ImGui::SliderFloat("diffuse", &diffuse, 0.0, 1.0);
			ImGui::SliderFloat("specular", &specular, 0.0, 1.0);
			ImGui::SliderInt("reflecting", &reflecting, 2, 256);
			if (ImGui::Button("Move the light")) {
				move = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("Stop moving")) {
				move = false;
			}
			if (ImGui::Button("Close Me")) {
				phong = false;
			}
			ImGui::End();
			phongShader.use();
			phongShader.setColor("objectColor", 1.0f, 0.5f, 0.31f);
			phongShader.setColor("lightColor", 1.0f, 1.0f, 1.0f);
			phongShader.setVec3("lightPos", lightPos);
			phongShader.setVec3("viewPos", m_camera.Position);
			phongShader.setFloat("a", ambient);
			phongShader.setFloat("s", specular);
			phongShader.setFloat("d", diffuse);
			phongShader.setInt("r", reflecting);
			model = glm::rotate(model, (float)(glfwGetTime() * 0.7) * glm::radians(50.0f), glm::vec3(0.5f, 0.4f, 0.5f));
			phongShader.setMat4("model", model);
			phongShader.setMat4("projection", proj);
			phongShader.setMat4("view", view);
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			static bool flag = true;
			if (move) {
				static float ini_time = glfwGetTime();
				if (flag == true) {
					ini_time = glfwGetTime();
					flag = false;
				}
				float curr_time = glfwGetTime();
				float radius = 1.8f, X = sin((curr_time - ini_time) * 1.5) * radius,
					Z = cos((curr_time - ini_time) * 1.5) * radius;
				lightPos.x = X;
				lightPos.z = Z;
			}
			else {
				flag = true;
				lightPos = glm::vec3(0.0f, 0.8f, 1.8f);
			}
		}
		else if (gouraud) {
			static bool move = false;
			static float ambient = 0.1;
			static float specular = 0.5;
			static float diffuse = 1.0;
			static int reflecting = 32;
			ImGui::Begin("Gouraud Shading");
			ImGui::SetWindowSize(ImVec2(400, 170));
			ImGui::SliderFloat("ambient", &ambient, 0.0, 1.0);
			ImGui::SliderFloat("diffuse", &diffuse, 0.0, 1.0);
			ImGui::SliderFloat("specular", &specular, 0.0, 1.0);
			ImGui::SliderInt("reflecting", &reflecting, 2, 256);
			if (ImGui::Button("Move the light")) {
				move = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("Stop moving")) {
				move = false;
			}
			if (ImGui::Button("Close Me")) {
				gouraud = false;
			}
			ImGui::End();
			gouraudShader.use();
			gouraudShader.setColor("objectColor", 1.0f, 0.5f, 0.31f);
			gouraudShader.setColor("lightColor", 1.0f, 1.0f, 1.0f);
			gouraudShader.setVec3("lightPos", lightPos);
			gouraudShader.setVec3("viewPos", m_camera.Position);
			gouraudShader.setFloat("a", ambient);
			gouraudShader.setFloat("s", specular);
			gouraudShader.setFloat("d", diffuse);
			gouraudShader.setInt("r", reflecting);
			model = glm::rotate(model, (float)(glfwGetTime() * 0.7) * glm::radians(50.0f), glm::vec3(0.5f, 0.4f, 0.5f));
			gouraudShader.setMat4("model", model);
			gouraudShader.setMat4("projection", proj);
			gouraudShader.setMat4("view", view);
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			static bool flag = true;
			if (move) {
				static float ini_time = glfwGetTime();
				if (flag == true) {
					ini_time = glfwGetTime();
					flag = false;
				}
				float curr_time = glfwGetTime();
				float radius = 1.8f, X = sin((curr_time - ini_time) * 1.5) * radius,
					Z = cos((curr_time - ini_time) * 1.5) * radius;
				lightPos.x = X;
				lightPos.z = Z;
			}
			else {
				flag = true;
				lightPos = glm::vec3(0.0f, 0.8f, 1.8f);
			}
		}

		

		lightShader.use();
		lightShader.setMat4("projection", proj);
		lightShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		lightShader.setMat4("model", model);

		glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

		// render
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
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

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		m_camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		m_camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		m_camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		m_camera.ProcessKeyboard(RIGHT, deltaTime);
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

	m_camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	m_camera.ProcessMouseScroll(yoffset);
}