#include "bresenham.h"

vector<Point> bresenham(Point A, Point B) {
	vector<Point> points;
	bool steep = abs(B.y - A.y) > abs(B.x - A.x);
	if (steep) {
		swap(A.x, A.y);
		swap(B.x, B.y);
	}
	if (A.x > B.x) {
		swap(A.x, B.x);
		swap(A.y, B.y);
	}
	int deltax = B.x - A.x,
		deltay = abs(B.y - A.y),
		error = deltax / 2,
		ystep, y = A.y;
	if (A.y < B.y) {
		ystep = 1;
	}
	else {
		ystep = -1;
	}
	for (int x = A.x; x < B.x; ++x) {
		if (steep) {
			points.push_back(Point(y, x));
		}
		else {
			points.push_back(Point(x, y));
		}
		error = error - deltay;
		if (error < 0) {
			y += ystep;
			error += deltax;
		}
	}
	return points;
}

vector<Point> drawTriangle(Point A, Point B, Point C) {
	vector<Point> points;
	vector<vector<Point>> edges;
	edges.push_back(bresenham(A, B));
	edges.push_back(bresenham(A, C));
	edges.push_back(bresenham(B, C));
	for (auto e : edges) {
		for (auto p : e) {
			points.push_back(p);
		}
	}
	return points;
}

vector<Point> getPoints(Point center, Point p) {
	vector<Point> res;
	res.push_back(Point(center.x + p.x, center.y + p.y));
	res.push_back(Point(center.x - p.x, center.y + p.y));
	res.push_back(Point(center.x + p.x, center.y - p.y));
	res.push_back(Point(center.x - p.x, center.y - p.y));
	res.push_back(Point(center.x + p.y, center.y + p.x));
	res.push_back(Point(center.x - p.y, center.y + p.x));
	res.push_back(Point(center.x + p.y, center.y - p.x));
	res.push_back(Point(center.x - p.y, center.y - p.x));
	return res;
}

vector<Point> drawCircle(Point center, int radius) {
	vector<Point> points;
	int x = 0,
		y = radius,
		d = 3 - radius - radius;
	vector<Point> tmp = getPoints(center, Point(x, y));
	for (auto p : tmp) {
		points.push_back(p);
	}
	while (y >= x) {
		++x;
		if (d > 0) {
			--y;
			d = d + 4 * (x - y) + 10;
		}
		else {
			d = d + 4 * x + 6;
		}
		tmp = getPoints(center, Point(x, y));
		for (auto p : tmp) {
			points.push_back(p);
		}
	}
	return points;
}

vector<Point> rasterize(Point A, Point B, Point C) {
	vector<Point> points;
	float k1 = (float)(B.y - C.y) / (B.x - C.x),
		b1 = B.y - k1 * B.x,
		k2 = (float)(A.y - C.y) / (A.x - C.x),
		b2 = A.y - k2 * A.x,
		k3 = (float)(A.y - B.y) / (A.x - B.x),
		b3 = A.y - k3 * A.x;
	int x_min, y_min, x_max, y_max;
	x_min = min(A.x, min(B.x, C.x));
	x_max = max(A.x, max(B.x, C.x));
	y_min = min(A.y, min(B.y, C.y));
	y_max = max(A.y, max(B.y, C.y));
	for (int i = x_min; i <= x_max; ++i) {
		for (int j = y_min; j <= y_max; ++j) {
			float u = (k1 * i - j + b1) * (k1 * A.x - A.y + b1),
				v = (k2 * i - j + b2) * (k2 * B.x - B.y + b2),
				w = (k3 * i - j + b3) * (k3 * C.x - C.y + b3);
			if (!(u < 0.0f || v < 0.0f || w < 0.0f)) {
				points.push_back(Point(i, j));
			}
		}
	}
	return points;
}

float *creatVertices(vector<Point> points) {
	int point_num = points.size();
	int total = point_num * 3;
	float *vertices = new float[total];
	for (int i = 0; i < point_num; ++i) {
		vertices[i * 3 + 0] = (float)points[i].x / (float)640;
		vertices[i * 3 + 1] = (float)points[i].y / (float)360;
		vertices[i * 3 + 2] = 0.0f;
	}
	return vertices;
}