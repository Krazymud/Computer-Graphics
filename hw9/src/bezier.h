#ifndef BEZIER_H
#define BEZIER_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include <vector>
using namespace std;


struct Point {
	float x, y, z;
	Point(float xpos, float ypos) {
		x = xpos;
		y = ypos;
		z = 0.0f;
	}
};



class Bezier {
private:
	int acc = 200;	//accuracy in drawing bezier
	float vertices[1000];
	vector<Point> curve;
public:
	int GetNum();
	void AddPoints(Point);
	void DelPoints();
	void DrawVertices();
	void DrawBezier();
	void DrawGrowingProcess(float);
	vector<Point> GenCurve();

};

#endif
