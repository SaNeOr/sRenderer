#pragma once
#include <tga/tgaimage.h>
#include <model/geometry.h>
#include <model/model.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>


using Line2i = Vec2i;
using Line3f = Vec3f;
using Triangle3i = Vec2i[3];

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);






namespace Renderer
{
	void Line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color);
	void Line(Vec2i t0, Vec2i t1, TGAImage& image, TGAColor color);

	void WireFrame(Vec3f* pts, TGAImage& image, TGAColor color = white);
	int CrossProduct(Line2i a, Line2i b);
	bool Inside(Vec2i p, Triangle3i tri);
	Vec3f Barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P);
	void Triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color);
	void Triangle(Vec3f* pts, float* zbuffer, TGAImage& image, TGAColor color, const int& width, const int& height);
	void Triangle(Vec3f* pts, float* zbuffer, TGAImage& image, TGAImage& diff, Vec2i* uv, const int& width, const int& height);
	Vec3f m2v(Matrix m);
	Matrix v2m(Vec3f v);
	Matrix ViewPort(int x, int y, int w, int h);
	Matrix LookAt(Vec3f eye, Vec3f center, Vec3f up);


	void LineGL(int x0, int y0, int x1, int y1, TGAColor color);
	void LineGL(Vec2i t0, Vec2i t1, TGAColor color);

	void WireFrameGL(Vec3f* pts, TGAColor color);



};

