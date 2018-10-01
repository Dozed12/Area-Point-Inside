#include <SFML/Graphics.hpp>

#include "AreaAlgorithm.h"

//Generate random concave polygon
std::vector<vertex> randomPolygon(double centerX, double centerY,unsigned int div, unsigned int radius, int maxUpDelta, int maxDownDelta) {
	
	std::vector<vertex> vertices;

	double deltaAngle = 2 * PI / div;
	unsigned int id = 0;
	for (double angle = 0; angle < PI * 2; angle += deltaAngle) {
		double delta = rand() % ((maxUpDelta - maxDownDelta) + 1) + maxDownDelta;
		vertices.push_back(vertex((centerX + cos(angle) * (radius + delta)),
								(centerY + sin(angle) * (radius + delta)),
								id));
		id++;
	}

	return vertices;
}

//Draw polygon
void drawPoly(std::vector<vertex> polygon, unsigned int n, sf::RenderWindow* window) {
	sf::ConvexShape shape;
	shape.setPointCount(n);
	for (unsigned int i = 0; i < n; i++)
	{
		shape.setPoint(i, sf::Vector2f(polygon[i].x, polygon[i].y));
	}
	window->draw(shape);
}

//Raycast Algorithm
int pnpoly(std::vector<vertex> vertices, vertex target)
{
	int i, j, c = 0;
	for (i = 0, j = vertices.size() - 1; i < vertices.size(); j = i++) {
		if (((vertices[i].y>target.y) != (vertices[j].y>target.y)) &&
			(target.x < (vertices[j].x - vertices[i].x) * (target.y - vertices[i].y) / (vertices[j].y - vertices[i].y) + vertices[i].x))
			c = !c;
	}
	return c;
}

//Winding number algorithm
int Is_Left(const vertex &p0,
	const vertex &p1,
	const vertex &point)
{
	return ((p1.x - p0.x) * (point.y - p0.y) -
		(point.x - p0.x) * (p1.y - p0.y));
}
bool Is_Inside(const vertex              &point,
	const std::vector<vertex> &points_list)
{
	// The winding number counter.
	int winding_number = 0;

	// Loop through all edges of the polygon.
	typedef std::vector<vertex>::size_type size_type;

	size_type size = points_list.size();

	for (size_type i = 0; i < size; ++i)             // Edge from point1 to points_list[i+1]
	{
		vertex point1(points_list[i]);
		vertex point2;

		// Wrap?
		if (i == (size - 1))
		{
			point2 = points_list[0];
		}
		else
		{
			point2 = points_list[i + 1];
		}

		if (point1.y <= point.y)                                    // start y <= point.y
		{
			if (point2.y > point.y)                                 // An upward crossing
			{
				if (Is_Left(point1, point2, point) > 0)             // Point left of edge
				{
					++winding_number;                               // Have a valid up intersect
				}
			}
		}
		else
		{
			// start y > point.y (no test needed)
			if (point2.y <= point.y)                                // A downward crossing
			{
				if (Is_Left(point1, point2, point) < 0)             // Point right of edge
				{
					--winding_number;                               // Have a valid down intersect
				}
			}
		}
	}

	return (winding_number != 0);
}

int main()
{

	//srand((unsigned int)time(0));

	sf::RenderWindow window(sf::VideoMode(800, 800), "Area Algorithm for point inside polygon");

	unsigned int nVertices = 50;

	std::vector<vertex> polygon = randomPolygon(200, 200, nVertices, 150, 50, -50);

	//Stress test Area Algorithm
	unsigned int total = 400 * 400;
	clock_t start = clock();
	unsigned int pointsCounted = 0;
	for (size_t i = 0; i < 400; i++)
	{
		for (size_t j = 0; j < 400; j++)
		{
			if (areaPointInside(polygon, vertex(i, j)))
				pointsCounted++;
		}
	}
	clock_t stop = clock();
	printf("Area Algorithm counted %d/%d points inside in: %f ms\n", pointsCounted, total, (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC);

	//Stress test Raycast Algorithm
	total = 400 * 400;
	start = clock();
	pointsCounted = 0;
	for (size_t i = 0; i < 400; i++)
	{
		for (size_t j = 0; j < 400; j++)
		{
			if (pnpoly(polygon, vertex(i, j)))
				pointsCounted++;
		}
	}
	stop = clock();
	printf("Raycast Algorithm counted %d/%d points inside in: %f ms\n", pointsCounted, total, (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC);

	//Stress test Winding Number Algorithm
	total = 400 * 400;
	start = clock();
	pointsCounted = 0;
	for (size_t i = 0; i < 400; i++)
	{
		for (size_t j = 0; j < 400; j++)
		{
			if (Is_Inside(vertex(i, j), polygon))
				pointsCounted++;
		}
	}
	stop = clock();
	printf("Winding Algorithm counted %d/%d points inside in: %f ms\n", pointsCounted, total, (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC);

	while (window.isOpen())
	{

		window.clear();
		drawPoly(polygon, nVertices, &window);
		window.display();

		sf::Event event;
		while (window.waitEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

	}

	return 0;
}