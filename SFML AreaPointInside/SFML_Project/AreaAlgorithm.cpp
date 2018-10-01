
#include "AreaAlgorithm.h"
#include <stdio.h>

/*
1 - Calculate area
2 - Prune vertices that arent visible to point
3 - Find closest unpruned vertice
4 - Calculate bisection at that vertice
5 - Find which side of the bisection the point is on
6 - Break edge of the side and add the point as a vertex
7 - Calculate new area
8 - Compare areas and reach conclusion if point is inside or outside
*/

/*
Main algorithm function

returns true if inside
returns false if outside
*/
bool areaPointInside(std::vector<vertex> polygon, vertex target) {
	double originalArea = area(polygon);

	std::vector<unsigned int> edgeToBreak = pickEdge(polygon, target);

	std::vector<vertex> nPolygon = breakEdge(edgeToBreak[1], polygon, target);

	double newArea = area(nPolygon);

	return originalArea >= newArea;
}

/*
Calculates area of polygon using Shoelace formula
https://www.geeksforgeeks.org/area-of-a-polygon-with-given-n-ordered-vertices/

returns area of polygon
*/
double area(std::vector<vertex> polygon) {

	double area = 0.0;

	for (int i = 0; i < polygon.size(); ++i)
	{
		int j = (i + 1) % polygon.size();
		area += 0.5 * (polygon[i].x * polygon[j].y - polygon[j].x * polygon[i].y);
	}

	return (area);
}

/*
Chooses the edge to be broken

returns an array with the 2 vertex IDs of the edge
*/
std::vector<unsigned int> pickEdge(std::vector<vertex> polygon, vertex target) {

	//Copy of original
	std::vector<vertex> tmpPoly = polygon;

	//Identiyfy vertices to prune
	std::vector<unsigned int> IDsToPrune;
	for (unsigned int i = 0; i < tmpPoly.size(); i++)
	{
		int intersections = 0;
		//Check intersection with each edge of the polygon
		for (unsigned int j = 0; j < tmpPoly.size() - 1; j++)
		{
			intersections += intersection(tmpPoly[i], target, polygon[j], polygon[j + 1]);
			//Already 2 intersections, don't need to test more
			if (intersections > 2)
				break;
		}
		//Intersection between edge of last and first vertex
		if (intersections <= 2)
			intersections += intersection(tmpPoly[i], target, polygon[polygon.size() - 1], polygon[0]);
		//Too many intersections, add to prune list
		if (intersections > 2) {
			IDsToPrune.push_back(tmpPoly[i].id);
		}
	}

	//Prune temporary vertices
	for (int i = IDsToPrune.size()-1; i >= 0; i--)
		tmpPoly.erase(tmpPoly.begin() + IDsToPrune[i]);

	//Choose closest of remaining vertices
	double minDistToVertex = DBL_MAX;
	unsigned int closestVertexId = 0;
	for (unsigned int i = 0; i < tmpPoly.size(); i++)
	{
		double distToVertex = dist(target, tmpPoly[i]);
		if (minDistToVertex > distToVertex) {
			closestVertexId = tmpPoly[i].id;
			minDistToVertex = distToVertex;
		}
	}

	//Get IDs of the two vertices connected to chosen vertex
	unsigned int prevID;
	unsigned int nextID;

	nextID = closestVertexId + 1;
	if (closestVertexId == polygon.size() - 1)
		nextID = 0;

	prevID = closestVertexId - 1;
	if (closestVertexId == 0)
		prevID = polygon.size() - 1;

	//Get coordinates of these vertices
	vertex vert = polygon[closestVertexId];
	vertex prev = polygon[prevID];
	vertex next = polygon[nextID];

	//Find bisector vector
	vector prevVec(vert.x - prev.x, vert.y - prev.y);
	vector nextVec(next.x - vert.x, next.y - vert.y);
	double magPrev = prevVec.mag();
	double magNext = nextVec.mag();
	prevVec.mult(magNext);
	nextVec.mult(magPrev);
	vector bisector = vector::add(prevVec, nextVec);
	bisector.rotate(-PI / 2);

	//Find what side of bisector the point is
	//https://stackoverflow.com/questions/1560492/how-to-tell-whether-a-point-is-to-the-right-or-left-side-of-a-line
	double pointLineX = vert.x + bisector.x;
	double pointLineY = vert.y + bisector.y;
	double eval = (pointLineX - vert.x) * (target.y - vert.y) - (pointLineY - vert.y) * (target.x - vert.x);
	int position = sign(eval);

	//Prepare and return solution
	std::vector<unsigned int> vertices(2);

	if (position == 1) {
		vertices[0] = closestVertexId;
		vertices[1] = nextID;
	}
	else {
		vertices[0] = prevID;
		vertices[1] = closestVertexId;
	}

	return vertices;

}

/*
Breaks the edge and adds the new point

returns a new polygon as an array of 2 arrays with both coordinates
*/
std::vector<vertex> breakEdge(unsigned int forwardVertexID, std::vector<vertex> polygon, vertex target) {

	//TODO Everything seems fine until here

	std::vector<vertex> nPolygon = polygon;

	vertex nVert(target.x, target.y, forwardVertexID);

	nPolygon.insert(nPolygon.begin() + forwardVertexID, nVert);

	return nPolygon;

}

/*
Detect intersection of 2 line segments

return true if intercepted
return false if not intercepted
*/
bool intersection(vertex start1, vertex end1, vertex start2, vertex end2) {

	double ax = end1.x - start1.x;
	double ay = end1.y - start1.y;

	double bx = start2.x - end2.x;
	double by = start2.y - end2.y;

	double dx = start2.x - start1.x;
	double dy = start2.y - start1.y;

	double det = ax * by - ay * bx;

	if (det == 0) return false;

	double r = (dx * by - dy * bx) / det;
	double s = (ax * dy - ay * dx) / det;

	return !(r < 0 || r > 1 || s < 0 || s > 1);

}

/*
Distance between 2 points

returns eucledian distance between 2 points
*/
double dist(vertex a, vertex b) {
	return sqrt((b.x - a.x)*(b.x - a.x) + (b.y - a.y)*(b.y - a.y));
}

/*
Evaluates the sign of the argument

returns 1 if positive
returns 0 if 0
returns -1 if negative
*/
int sign(double val) {
	return (val > 0) - (val < 0);
}
