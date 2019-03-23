#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <thread>
#include "bresenham.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
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


	// build and compile our shader program
	// vertex shader
	Shader shader("shader.vs", "shader.fs");

	// set up vertex data (and buffer(s)) and configure vertex attributes

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	//binding vertices

	bool ras = false;
	bool draw_circle = false;
	//location
	vector<Point> ras_tri = rasterize(Point(0, 180), Point(230, -180), Point(-230, -180));
	float *ras_vertices = creatVertices(ras_tri);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// render loop
	while (!glfwWindowShouldClose(window))
	{
		
		// input
		processInput(window);

		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);

		this_thread::sleep_for(chrono::milliseconds(10));
		//start the imgui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//show a window
		static float f = 0.0f;
		ImGui::Begin("Bresenham\n");
		ImGui::Text("Need to rasterize? (DEFAULT: no)");
		ImGui::Checkbox("Rasterize", &ras);
		ImGui::Text("Circle or Triangle? (DEFAULT: triangle)");
		ImGui::Checkbox("Draw circle", &draw_circle);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();

		int width, height;
		glfwGetWindowSize(window, &width, &height);

		if (draw_circle) {
			ImGui::Begin("Drawing circle\n");
			ImGui::Text("Modify the radius of the circle.");
			ImGui::SliderFloat("radius", &f, 0.0f, 300.0f);
			if (ImGui::Button("Close")) {
				draw_circle = false;
			}
			ImGui::End();
			
		}
		vector<Point> test;
		// drawing
		if (draw_circle) {
			test = drawCircle(Point(0, 0), static_cast<int>(f));
		}
		else {
			if (ras) {
				test = ras_tri;
			}
			else {
				test = drawTriangle(Point(0, 180), Point(230, -180), Point(-230, -180));
			}
		}
		int total = test.size() * 3;
		float *vertices;
		if (!ras) {
			vertices = creatVertices(test);
		}
		else {
			vertices = ras_vertices;
		}
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * total, vertices, GL_STREAM_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		shader.use();
		glDrawArrays(GL_POINTS, 0, test.size());
		if (!ras) {
			delete vertices;
		}

		// render
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	delete(ras_vertices);
	glBindVertexArray(0);
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
}

//window size changed
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}