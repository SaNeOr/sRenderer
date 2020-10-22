#include "Renderer.h"

void Renderer::Line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
{

	bool steep = false;
	if (std::abs(x1 - x0) < std::abs(y1 - y0)) {
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}

	if (x1 < x0) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}


	int dx = x1 - x0;
	int dy = y1 - y0;

	float derror = std::abs(dy / float(dx));
	float error = 0.0f;

	int y = y0;
	for (float x = x0; x <= x1; x++) {   //  dx > dy
		if (!steep) {
			image.set(x, y, color);
		}
		else {
			image.set(y, x, color);
		}

		error += derror;
		if (error > 0.5f) {
			y += (y1 > y0 ? 1.0f : -1.0f);
			error -= 1.0f;
		}
	}
}

void Renderer::Line(Vec2i t0, Vec2i t1, TGAImage& image, TGAColor color)
{
	Line(t0.x, t0.y, t1.x, t1.y, image, color);
}

void Renderer::WireFrame(Vec3f* pts, TGAImage& image, TGAColor color)
{
	Vec2i a = Vec2i(pts[0].x, pts[0].y);
	Vec2i b = Vec2i(pts[1].x, pts[1].y);
	Vec2i c = Vec2i(pts[2].x, pts[2].y);

	Line(a, b, image, color);
	Line(b, c, image, color);
	Line(a, c, image, color);
}

int Renderer::CrossProduct(Line2i a, Line2i b)
{
	return a.x * b.y - a.y * b.x;

}

bool Renderer::Inside(Vec2i p, Triangle3i tri)
{
	//Line2i PA = Line2i(p.x - tri[0].x, p.y - tri[0].y);
	Line2i  A = tri[1] - tri[2], B = tri[0] - tri[1], C = tri[2] - tri[0];
	Line2i PA = p - tri[2], PB = p - tri[1], PC = p - tri[0];


	int d0 = CrossProduct(PA, A);
	int d1 = CrossProduct(PB, B);
	int d2 = CrossProduct(PC, C);

	if (d0 >= 0 && d1 >= 0 && d2 >= 0) return true;
	if (d0 <= 0 && d1 <= 0 && d2 <= 0) return true;

	return false;
}

Vec3f Renderer::Barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P)
{
	Vec3f s[2];
	for (int i = 2; i--; ) {
		s[i][0] = C[i] - A[i];
		s[i][1] = B[i] - A[i];
		s[i][2] = A[i] - P[i];
	}
	Vec3f u = (s[0] ^ s[1]);
	if (std::abs(u[2]) > 1e-2) // don't forget that u[2] is integer. If it is zero then triangle ABC is degenerate
		return Vec3f(1.f - (u.y + u.x) / u.z, u.y / u.z, u.x / u.z);
	return Vec3f(-1, 1, 1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}

void Renderer::Triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color)
{
	if (t0.y > t1.y) std::swap(t0, t1);
	if (t1.y > t2.y) std::swap(t1, t2);
	if (t0.y > t1.y) std::swap(t0, t1);


	int boundX = std::min(t0.x, std::min(t1.x, t2.x));
	int boundWith = std::max(t0.x, std::max(t1.x, t2.x)) - boundX;

	int boundY = t0.y;
	int bountHeight = t2.y - t0.y;

	// Draw grids
	//for (int x = boundX; x <= boundX + boundWith; x+=5) {
	//    for (int y = boundY; y <= boundY + bountHeight; y+=5) {
	//        image.set(x, y, white);
	//    }
	//}

	for (int x = boundX; x <= boundX + boundWith; x++) {
		for (int y = boundY; y <= boundY + bountHeight; y++) {
			Vec2i p = Vec2i(x, y);

			if (Inside(p, Triangle3i{ t0, t1, t2 })) {
				image.set(p.x, p.y, color);
			}

		}
	}
}

void Renderer::Triangle(Vec3f* pts, float* zbuffer, TGAImage& image, TGAColor color, const int& width, const int& height)
{
	if (pts[0].y > pts[1].y) { std::swap(pts[0], pts[1]); }
	if (pts[0].y > pts[1].y) { std::swap(pts[0], pts[1]); }
	if (pts[1].y > pts[2].y) { std::swap(pts[1], pts[2]); }

	Vec2f boundmin = Vec2f(
		std::max(0.0f, std::min(pts[0].x, std::min(pts[1].x, pts[2].x))),
		std::max(0.0f, std::min(pts[0].y, std::min(pts[1].y, pts[2].y)))
	);

	Vec2f boundmax = Vec2f{
		std::min((float)width, std::max(pts[0].x, std::max(pts[1].x, pts[2].x))),
		std::min((float)height,std::max(pts[0].y, std::max(pts[1].y, pts[2].y)))
	};


	Vec3f p;
	for (p.x = boundmin.x; p.x <= boundmax.x; p.x++) {
		for (p.y = boundmin.y; p.y <= boundmax.y; p.y++) {
			Vec3f bc_screen = Barycentric(pts[0], pts[1], pts[2], p);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
			p.z = 0;
			for (int i = 0; i < 3; i++) p.z += pts[i].z * bc_screen[i];     //  p = uA + vB + wC => p.z = uA.z + vB.z + wC.z;
			if (zbuffer[int(p.x + p.y * width)] < p.z) {
				zbuffer[int(p.x + p.y * width)] = p.z;
				image.set(p.x, p.y, color);
			}
		}
	}
}

void Renderer::Triangle(Vec3f* pts, float* zbuffer, TGAImage& image, TGAImage& diff, Vec2i* uv, const int& width, const int& height)
{
	// sort by y-coordinates; (ASC      pts[0] < pts[1] < pts[2]
	if (pts[0].y == pts[1].y && pts[0].y == pts[2].y) return;
	if (pts[0].y > pts[1].y) { std::swap(pts[0], pts[1]); std::swap(uv[0], uv[1]); }
	if (pts[0].y > pts[1].y) { std::swap(pts[0], pts[1]); std::swap(uv[0], uv[1]); }
	if (pts[1].y > pts[2].y) { std::swap(pts[1], pts[2]); std::swap(uv[1], uv[2]); }

	Vec2f boundmin = Vec2f(
		std::max(0.0f, std::min(pts[0].x, std::min(pts[1].x, pts[2].x))),
		std::max(0.0f, std::min(pts[0].y, std::min(pts[1].y, pts[2].y)))
	);

	Vec2f boundmax = Vec2f{
		std::min((float)width, std::max(pts[0].x, std::max(pts[1].x, pts[2].x))),
		std::min((float)height,std::max(pts[0].y, std::max(pts[1].y, pts[2].y)))
	};

	Vec3f p;
	for (p.x = boundmin.x; p.x <= boundmax.x; p.x++) {
		for (p.y = boundmin.y; p.y <= boundmax.y; p.y++) {
			Vec3f bc_screen = Barycentric(pts[0], pts[1], pts[2], p);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
			p.z = 0;
			for (int i = 0; i < 3; i++) p.z += pts[i].z * bc_screen[i];     //  p = uA + vB + wC => p.z = uA.z + vB.z + wC.z;
			if (zbuffer[int(p.x + p.y * width)] < p.z)
			{
				zbuffer[int(p.x + p.y * width)] = p.z;

				int X = 0, Y = 0;
				for (int i = 0; i < 3; i++) {
					X += uv[i].x * bc_screen[i];
					Y += uv[i].y * bc_screen[i];
				}
				for (int i = 0; i < 3; i++) {
					image.set(p.x, p.y, diff.get(X, Y));
				}
			}
		}
	}
}

Vec3f Renderer::m2v(Matrix m)
{
	return Vec3f(m[0][0] / m[3][0], m[1][0] / m[3][0], m[2][0] / m[3][0]);
}

Matrix Renderer::v2m(Vec3f v)
{
	Matrix m(4, 1);     //  [0,0,0,0]
	m[0][0] = v.x;
	m[1][0] = v.y;
	m[2][0] = v.z;
	m[3][0] = 1.0f;

	return m;
}

Matrix Renderer::ViewPort(int x, int y, int w, int h)
{
	Matrix m = Matrix::identity(4);

	m[0][0] = w / 2.0f;
	m[1][1] = h / 2.0f;
	//m[2][2] = 1;
	//m[3][2] = 1.0f;

	m[0][3] = x + w / 2.0f;
	m[1][3] = y + h / 2.0f;
	//m[2][3] = 1.0f;

	return m;
}

Matrix Renderer::LookAt(Vec3f eye, Vec3f center, Vec3f up)
{
	Vec3f D = (eye - center).normalize();
	Vec3f R = (up ^ D).normalize();
	Vec3f U = (D ^ R).normalize();
	Matrix res = Matrix::identity(4);
	Matrix eyePos = Matrix::identity(4);
	for (int i = 0; i < 3; i++) {
		res[0][i] = R[i];
		res[1][i] = U[i];
		res[2][i] = D[i];
		//res[i][3] = -center[i];
		eyePos[i][3] = eye[i];
	}

	return res * eyePos;
}
