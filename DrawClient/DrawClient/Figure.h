#pragma once
using namespace std;

enum FigureType 
{
	POLYLINE = 0,
	POLYGON = 1,
	RECTANGLE = 2,
	ELLIPSE = 3
};

struct Figure
{
	FigureType type;
	int verticesNumber;
	POINT* vertices;

//methods:
public:
	Figure(FigureType _type, int size);
	Figure(const Figure&);
	Figure& operator=(const Figure&);
	~Figure();
	void draw(HDC hdc);
};

