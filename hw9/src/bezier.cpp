#include "bezier.h"
int fac(int n) {
	if (n == 1 || n == 0) {
		return 1;
	}
	for (int i = n - 1; i > 1; --i) {
		n *= i;
	}
	return n;
}

double comb(int n, int i) {
	return fac(n) / (fac(i) * fac(n - i));
}
void Bezier::AddPoints(Point p) {

	curve.push_back(p);
}

void Bezier::DelPoints() {
	curve.pop_back();
}

int Bezier::GetNum() {
	return curve.size();
}

void Bezier::DrawBezier() {
	int num_cp = curve.size() - 1;
	if (num_cp < 1) {
		return;
	}

	float x, y;
	int num=0;
	for (float t = 0; t <= 1; t = t + (1.0 / acc)) {
		x = 0; 
		y = 0;
		for (int i = 0; i <= num_cp; ++i) {
			x += comb(num_cp, i) * pow(t, i) * pow((1 - t), (num_cp - i)) * curve[i].x;
			y += comb(num_cp, i) * pow(t, i) * pow((1 - t), (num_cp - i)) * curve[i].y;
		}
		vertices[num] = x;
		vertices[num + 1] = y;
		vertices[num + 2] = 0.0;
		num += 3;
	}
	unsigned int VAO;
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_LINE_STRIP, 0, num / 3);

	glBindVertexArray(0);
}

void Bezier::DrawGrowingProcess(float t) {
	if (curve.size() < 3) {
		return;
	}
	int num = curve.size();
	for (int i = 0; i < num; ++i) {
		vertices[3 * i] = curve[i].x;
		vertices[3 * i + 1] = curve[i].y;
		vertices[3 * i + 2] = curve[i].z;
	}
	unsigned int VAO;
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	while (--num) {
		for (int i = 0; i < num; ++i) {
			vertices[3 * i] = (1 - t) * vertices[3 * i] + t * vertices[3 * i + 3];
			vertices[3 * i + 1] = (1 - t) * vertices[3 * i + 1] + t * vertices[3 * i + 4];
			vertices[3 * i + 2] = (1 - t) * vertices[3 * i + 2] + t * vertices[3 * i + 5];
		}
		
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glDrawArrays(GL_LINE_STRIP, 0, num);
		glDrawArrays(GL_POINTS, 0, num);
	}
}

void Bezier::DrawVertices() {
	if (curve.empty()) {
		return;
	}
	for (int i = 0; i < curve.size(); ++i) {
		vertices[3 * i] = curve[i].x;
		vertices[3 * i + 1] = curve[i].y;
		vertices[3 * i + 2] = curve[i].z;
	}
	unsigned int VAO; 
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_POINTS, 0, curve.size());
	glDrawArrays(GL_LINE_STRIP, 0, curve.size());

	glBindVertexArray(0);
}

vector<Point> Bezier::GenCurve() {
	return vector<Point>();
}