#pragma once

#include <math.h>
#include <limits>
#include <vector>
#include <algorithm> 

#define PI 3.14159265359

struct vector
{
	double x;
	double y;

	vector() {}
	vector(double nx, double ny) {
		x = nx;
		y = ny;
	}

	//Magnitude
	double mag() {
		return sqrt(x * x + y * y);
	}

	//Multiply by a scalar
	void mult(double s) {
		x *= s;
		y *= s;
	}

	//Adds 2 vectors and returns the resulting vector
	static vector add(vector a, vector b) {
		vector r(a.x + b.x, a.y + b.y);
		return r;
	}

	//Rotate vector
	void rotate(double angle) {
		double tmpX = x;
		double tmpY = y;
		x = tmpX * cos(angle) - tmpY * sin(angle);
		y = tmpX * sin(angle) + y * cos(angle);
	}
};

struct vertex
{
	double x;
	double y;
	unsigned int id;
	vertex() {}
	vertex(double nx, double ny, unsigned int nID) {
		x = nx;
		y = ny;
		id = nID;
	}
	vertex(double nx, double ny) {
		x = nx;
		y = ny;
	}
	vertex(const vertex& a) : x(a.x), y(a.y), id(a.id) { }
	void operator=(const vertex &V) {
		x = V.x;
		y = V.y;
		id = V.id;
	}
	friend bool operator==(const vertex& a, const vertex& b)
	{
		return a.id == b.id;
	}
};

bool areaPointInside(std::vector<vertex> polygon, vertex target);
double area(std::vector<vertex> polygon);
std::vector<unsigned int> pickEdge(std::vector<vertex> polygon, vertex target);
std::vector<vertex> breakEdge(unsigned int forwardVertexID, std::vector<vertex> polygon, vertex target);
bool intersection(vertex start1, vertex end1, vertex start2, vertex end2);
double dist(vertex a, vertex b);
int sign(double val);