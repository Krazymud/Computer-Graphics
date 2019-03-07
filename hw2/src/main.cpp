#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <thread>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;
const char *glsl_version = "#version 330";

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
	ImVec4 tri_color_1 = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	ImVec4 tri_color_2 = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
	ImVec4 tri_color_3 = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
	ImVec4 tri_color_4 = ImVec4(1.0f, 0.5f, 0.2f, 1.0f);

	// build and compile our shader program
	// vertex shader
	Shader shader1("shader1.vs", "shader1.fs");
	Shader shader2("shader2.vs", "shader2.fs");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	float firstTri[] = {
		//location			//color
		0.0f,  1.0f, 0.0f,	tri_color_1.x, tri_color_1.y, tri_color_1.z,
		-0.5f, 0.0f, 0.0f,	tri_color_2.x, tri_color_2.y, tri_color_2.z,
		0.5f, 0.0f, 0.0f,	tri_color_3.x, tri_color_3.y, tri_color_3.z
	};
	float secondTri[] = {
		//location
		-0.5f, 0.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.0f,
		1.0f, -1.0f, 0.0f
	};
	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 4
	};
	float p1 = 0.5 / 3, p2 = 1.0 / 3,
		p3 = 0.5 / 4, p4 = 1.0 / 4;
	float lines[] = {
		//location
		0.0f, 0.0f, 0.0f,
		-p3 * 3, -p4, 0.0f,
		-p1, -p2 * 2, 0.0f,
		p1, -p2 * 2, 0.0f,
		p3 * 3, -p4, 0.0f
	};
	unsigned int lindices[] = {
		0, 2,
		2, 4,
		4, 1,
		1, 3,
		3, 0
	};
	unsigned int VBOs[3], VAOs[3], EBOs[2];
	glGenVertexArrays(3, VAOs);
	glGenBuffers(3, VBOs);
	glGenBuffers(2, EBOs);
	// bind the first triangle
	glBindVertexArray(VAOs[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(firstTri), firstTri, GL_STATIC_DRAW);
	//location
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//bind the second triangle(s)
	glBindVertexArray(VAOs[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(secondTri), secondTri, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//bind the lines
	glBindVertexArray(VAOs[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lines), lines, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(lindices), lindices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// render loop
	while (!glfwWindowShouldClose(window))
	{
		this_thread::sleep_for(chrono::milliseconds(29));
		// input
		processInput(window);

		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);

		//start the imgui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//show a window
		static float f = 0.0f;
		static int counter = 0;
		ImGui::Begin("Change the color\n");
		ImGui::ColorEdit3("color1", (float*)&tri_color_1);
		ImGui::ColorEdit3("color2", (float*)&tri_color_2);
		ImGui::ColorEdit3("color3", (float*)&tri_color_3);
		ImGui::ColorEdit3("color4", (float*)&tri_color_4);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();

		firstTri[3] = tri_color_1.x;
		firstTri[4] = tri_color_1.y;
		firstTri[5] = tri_color_1.z;
		firstTri[9] = tri_color_2.x;
		firstTri[10] = tri_color_2.y;
		firstTri[11] = tri_color_2.z;
		firstTri[15] = tri_color_3.x;
		firstTri[16] = tri_color_3.y;
		firstTri[17] = tri_color_3.z;

		// draw the triangle
		shader1.use();
		glBindVertexArray(VAOs[0]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(firstTri), firstTri, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		shader2.use();
		shader2.setColor("outColor", tri_color_4.x, tri_color_4.y, tri_color_4.z);
		glBindVertexArray(VAOs[1]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(VAOs[2]);
		glDrawElements(GL_LINES, 10, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glLineWidth(2.0f);
		


		// render
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(2, VAOs);
	glDeleteBuffers(2, VBOs);
	glDeleteBuffers(2, EBOs);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}

// process all input
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

//window size changed
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}