#include "stdafx.h"
#include "Figure.h"


Figure::Figure(FigureType _type, int size) : type(_type), verticesNumber(size)
{
	if (size)
		vertices = new POINT[verticesNumber];
	else
		vertices = nullptr;
}


Figure::~Figure()
{
	if (vertices != nullptr)
		delete[] vertices;
}

Figure::Figure(const Figure& f)
{
	verticesNumber = f.verticesNumber;
	type = f.type;
	if (verticesNumber)
		vertices = new POINT[verticesNumber];
	for (int i = 0; i < verticesNumber; i++)
		vertices[i] = f.vertices[i];
}

Figure& Figure::operator=(const Figure& f)
{
	if (vertices != nullptr)
		delete[] vertices;
	verticesNumber = f.verticesNumber;
	type = f.type;
	if (verticesNumber)
		vertices = new POINT[verticesNumber];
	for (int i = 0; i < verticesNumber; i++)
		vertices[i] = f.vertices[i];
	return *this;
}

void Figure::draw(HDC hdc)
{
	switch (type)
	{
	case POLYLINE:
		Polyline(hdc, vertices, verticesNumber);
		break;
	case POLYGON:
		BeginPath(hdc);
		Polyline(hdc, vertices, verticesNumber);
		CloseFigure(hdc);
		EndPath(hdc);
		SetPolyFillMode(hdc, WINDING);
		FillPath(hdc);
		break;
	case RECTANGLE:
		ASSERT(verticesNumber == 2);
		Rectangle(hdc, vertices[0].x, vertices[0].y, vertices[1].x, vertices[1].y);
		break;
	case ELLIPSE:
		ASSERT(verticesNumber == 2);
		Ellipse(hdc, vertices[0].x, vertices[0].y, vertices[1].x, vertices[1].y);
		break;
	}
}
