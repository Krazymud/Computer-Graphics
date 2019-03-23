#ifndef BRESENHAM_H
#define BRESEMHAM_H
#include <vector>
#include <algorithm>
using namespace std;
struct Point {
	int x;
	int y;
	Point(int x_, int y_) {
		x = x_;
		y = y_;
	}
};

vector<Point> bresenham(Point, Point);
vector<Point> drawTriangle(Point, Point, Point);
vector<Point> drawCircle(Point, int);
vector<Point> rasterize(Point, Point, Point);
vector<Point> getPoints(Point, Point);
float *creatVertices(vector<Point> v);

#endif
