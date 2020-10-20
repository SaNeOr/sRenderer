#include <fstream>
#include <iostream>
#include <algorithm>

#include <tga/tgaimage.h>
#include <model/geometry.h>
#include <model/model.h>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

const Vec3f light_dir = Vec3f(0, 0, -1);
Vec3f camera = Vec3f(0, 0, 3);

Model* model = NULL;
const int width = 800;
const int height = 800;
const int depth = 255;

void Line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    
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

void Line(Vec2i t0, Vec2i t1, TGAImage& image, TGAColor color) {
    Line(t0.x, t0.y, t1.x, t1.y, image, color);
}



void WireFrame(Vec3f* pts, TGAImage& image, TGAColor color = white) {
	Vec2i a = Vec2i(pts[0].x, pts[0].y);
	Vec2i b = Vec2i(pts[1].x, pts[1].y);
	Vec2i c = Vec2i(pts[2].x, pts[2].y);

    Line(a, b, image, color);
    Line(b, c, image, color);
    Line(a, c, image, color);
}


// math
//  translate matrix
//  scale matrix
//  rotate matrix
//

//  Scan Convert Line:
//#define L2L_SCAN 
#define POINT_IN_TRIANGLE
//      Method1: Line-by-line Scan

#ifdef L2L_SCANE
void Triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color) {
    // sort by y-coordinates;
    if (t0.y > t1.y) std::swap(t0, t1);
    if (t1.y > t2.y) std::swap(t1, t2);
    if (t0.y > t1.y) std::swap(t0, t1);

    int total_height = t2.y - t0.y;
    int segment_height = std::max(1, t1.y - t0.y);


    for (int y = t0.y; y <= t1.y; y++) {
        float alpha = (float)(y - t0.y) / total_height;
        float beta = (float)(y - t0.y) / segment_height;

        Vec2i A = t0 + (t2 - t0) * alpha;
        Vec2i B = t0 + (t1 - t0) * beta;

        if (A.x > B.x) std::swap(A, B);
        for (int j = A.x; j <= B.x; j++) {
            image.set(j, y, color);
        }
    }

    segment_height = std::max(t2.y - t1.y,1);
    for (int y = t1.y; y <= t2.y; y++) {
        float alpha = (float)(y - t0.y) / total_height;
        float beta = (float)(y - t1.y) / segment_height;

        Vec2i A = t0 + (t2 - t0) * alpha;
        Vec2i B = t1 + (t2 - t1) * beta;

        if (A.x > B.x) std::swap(A, B);
        for (int j = A.x; j <= B.x; j++) {
            image.set(j, y, color);
        }
    }

}
#endif // L2L_SCANE

//      Method2: Point In Triangle
///*  TODO
using Line2i = Vec2i;
using Line3f = Vec3f;
using Triangle3i = Vec2i[3];

//template <int T>
//using Tx = Vec2i[T];


int CrossProduct(Line2i a, Line2i b) {
    return a.x * b.y - a.y * b.x;
}



bool Inside(Vec2i p, Triangle3i tri) {
    //Line2i PA = Line2i(p.x - tri[0].x, p.y - tri[0].y);
    Line2i  A = tri[1] - tri[2],  B = tri[0] - tri[1],  C = tri[2] - tri[0];
    Line2i PA = p      - tri[2], PB = p      - tri[1], PC = p      - tri[0];

    
    int d0 = CrossProduct(PA, A);
    int d1 = CrossProduct(PB, B);
    int d2 = CrossProduct(PC, C);

    if (d0 >= 0 && d1 >= 0 && d2 >= 0) return true;
    if (d0 <= 0 && d1 <= 0 && d2 <= 0) return true;

    return false;
}

#ifdef POINT_IN_TRIANGLE
void Triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color){
    // sort by y-coordinates; (ASC
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

            if (Inside(p, Triangle3i{ t0, t1, t2})) {
                image.set(p.x, p.y, color);
            }

        }
    }
}


#endif // POINT_IN_TRIANGLE

//*/

//Vec3f Barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P) {
//    //  P = A + u * (C - A) + v * (B - A)
//    //  P = (1-u-v)A + vB + uC;
//
//    //  u = ((v1.v1)(v2.v0) - (v1.v0)(v2.v1)) / ((v0.v0)(v1.v1) - (v0.v1)(v1.v0))
//    //  v = ((v0.v0)(v2.v1) - (v0.v1)(v2.v0)) / ((v0.v0)(v1.v1) - (v0.v1)(v1.v0))
//    Line3f v0(C - A), v1 = (B - A), v2 = (P-A);
//    float dot00 = v0 * v0;
//    float dot01 = v0 * v1;
//    float dot02 = v0 * v2;
//    float dot11 = v1 * v1;

//    float dot12 = v1 * v2;
//
//
//    float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
//    float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
//    float v = (dot00 * dot12 - dot01 * dot02) * invDenom;
//    
//    return Vec3f((1.0f - u - v), v, u);
//}

Vec3f Barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P) {
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

void Triangle(Vec3f* pts, float* zbuffer, TGAImage& image, TGAColor color) {
    if (pts[0].y > pts[1].y) {std::swap(pts[0], pts[1]);}
    if (pts[0].y > pts[1].y) {std::swap(pts[0], pts[1]);}
    if (pts[1].y > pts[2].y) {std::swap(pts[1], pts[2]);}

    Vec2f boundmin = Vec2f(
        std::max(0.0f, std::min(pts[0].x, std::min(pts[1].x, pts[2].x)) ),
        std::max(0.0f, std::min(pts[0].y, std::min(pts[1].y, pts[2].y)))
    );

    Vec2f boundmax = Vec2f{
        std::min((float)width, std::max(pts[0].x, std::max(pts[1].x, pts[2].x))),
        std::min((float)height,std::max(pts[0].y, std::max(pts[1].y, pts[2].y)))
    };


    Vec3f p;
    for (p.x = boundmin.x; p.x <= boundmax.x; p.x++) {
        for (p.y = boundmin.y; p.y <= boundmax.y ; p.y++) {
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



void Triangle(Vec3f* pts, float* zbuffer, TGAImage& image,  TGAImage& diff, Vec2i* uv) {
    // sort by y-coordinates; (ASC      pts[0] < pts[1] < pts[2]
    if (pts[0].y == pts[1].y && pts[0].y == pts[2].y) return;
    if (pts[0].y > pts[1].y) {std::swap(pts[0], pts[1]); std::swap(uv[0],uv[1]);}
    if (pts[0].y > pts[1].y) {std::swap(pts[0], pts[1]); std::swap(uv[0],uv[1]);}
    if (pts[1].y > pts[2].y) {std::swap(pts[1], pts[2]); std::swap(uv[1],uv[2]);}

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
                    image.set(p.x,p.y, diff.get(X, Y));
                }
            }
        }
    }
}


//void Triangle(Vec3f* pts, float* zbuffer, TGAImage& image, TGAColor color) {
//    Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
//    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
//    Vec2f clamp(image.get_width() - 1, image.get_height() - 1);
//    for (int i = 0; i < 3; i++) {
//        for (int j = 0; j < 2; j++) {
//            bboxmin[j] = std::max(0.0f, std::min(bboxmin[j], pts[i][j]));
//            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
//        }
//    }
//    Vec3f P;
//    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
//        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
//            Vec3f bc_screen = Barycentric(pts[0], pts[1], pts[2], P);
//            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
//            P.z = 0;
//            for (int i = 0; i < 3; i++) P.z += pts[i][2] * bc_screen[i];
//            if (zbuffer[int(P.x + P.y * width)] < P.z) {
//                zbuffer[int(P.x + P.y * width)] = P.z;
//                image.set(P.x, P.y, color);
//            }
//        }
//    }
//}

Vec3f m2v(Matrix m) {
    return Vec3f(m[0][0] / m[3][0], m[1][0] / m[3][0], m[2][0]/ m[3][0]);
}

Matrix v2m(Vec3f v) {
    Matrix m(4, 1);     //  [0,0,0,0]
    m[0][0] = v.x;
    m[1][0] = v.y;
    m[2][0] = v.z;
    m[3][0] = 1.0f;

    return m;
}

Matrix ViewPort(int x, int y, int w, int h) {
    Matrix m = Matrix::identity(4);
    
    m[0][0] = w / 2.0f;
    m[1][1] = h / 2.0f;
    m[2][2] = depth / 2.0f;


    m[0][3] = x + w / 2.0f;
    m[1][3] = y + h / 2.0f;
    m[2][3] = depth / 2.0f;

    return m;
}


Vec3f World2screen(Vec3f v) {
    return Vec3f(v.x * width / 2.0f + v.x + width/ 2.0f, 
       v.y * height / 2.0f + v.y + height / 2.0f - height / 2.0f ,
        v.z);
    //return Vec3f(int((v.x + 1.0f) * width / 2.0f), int((v.y + 1.0f) * height / 2.0f - height / 2.0f), v.z);
}

int main(int argc, char** argv) {
     //TGAImage image(500, 500, TGAImage::RGB);
     //Vec2i t0[3] = { Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80) };
     //Vec2i t1[3] = { Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180) };
     //Vec2i t2[3] = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };
     //Triangle(t0[0], t0[1], t0[2], image, red);
     //Triangle(t1[0], t1[1], t1[2], image, white);
     //Triangle(t2[0], t2[1], t2[2], image, green);

     //for (int i = 0; i < 100; i+= 5) {
     //    for (int j = 0; j < 100; j+= 5) {
     //       image.set(i, j, white);
     //    }
     //}
     
     //Line(10, 15, 13, 40, image, red);
     
    //model = new Model("assets/obj/ddy.obj");

    Matrix projection = Matrix::identity(4.0f);
    projection[3][2] = -1.0f /camera.z;
                                
                                //startX, startY       // viewport width height
    Matrix viewProj = ViewPort(0, -height / 2, width, height);

    TGAImage diff;//  = new TGAImage("assets/texture/african_head_diffuse.tga");
    diff.read_tga_file("assets/texture/jwj_diffuse.tga");
    diff.flip_vertically();

    model = new Model("assets/obj/jwj.obj");

    TGAImage image(width, height, TGAImage::RGB);

    float* zbuffer = new float[width * height];
    for (int i = width * height; i--; zbuffer[i] = -std::numeric_limits<float>::max());

    //TGAImage image(width, 16, TGAImage::RGB);
    //int ybuffer[width];
    //std::fill(ybuffer, ybuffer + width, std::numeric_limits<int>::min());


    // screen line
    //Line(Vec2i(10, 10), Vec2i(790, 10), image, white);


    //for (int i = 0; i < model->nfaces(); i++) {
    //    std::vector<int> face = model->face(i);
    //    Vec3f pts[3];
    //    for (int i = 0; i < 3; i++) pts[i] = World2screen(model->vert(face[i]));
    //    Triangle(pts, zbuffer, image, TGAColor(rand() % 255, rand() % 255, rand() % 255, 255));
    //}


    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);

        Vec2i screen_coords[3];
        Vec3f world_coords[3];
        Vec3f pts[3];

        for (int j = 0; j < 3; j++) {
            Vec3f v = model->vert(face[j]); 
            world_coords[j] = v;


            pts[j] = World2screen(v);
            //pts[j] =  m2v(viewProj *  v2m(v));
            
        }

        Vec3f n =  (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);        //   fuck xor!
        n.normalize();

        float indensity = n * light_dir;
        if (indensity > 0) 
        {
			Vec2i uv[3];
			for (int k = 0; k < 3; k++){
			    uv[k] = model->uv(i, k);
            }

           
            //WireFrame(pts, image);
            //Triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(indensity * 255, indensity * 255, indensity * 255, 255));
            //Triangle(pts, zbuffer, image, TGAColor(indensity * 255, indensity * 255, indensity * 255, 255));
            Triangle(pts, zbuffer, image, diff, uv);
        }


    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("out/output.tga");


	{ // dump z-buffer
		TGAImage zbimage(width, height, TGAImage::GRAYSCALE);
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {

                if(zbuffer[i + j * width] >= 0)
				    zbimage.set(i, j, TGAColor(zbuffer[i + j * width] * 255, 1));

                //std::cout << zbuffer[i + j * width] << " ";
			}
		}
		zbimage.flip_vertically(); // i want to have the origin at the left bottom corner of the image
		zbimage.write_tga_file("out/zbuffer.tga");
	}

    delete model;
    return 0;
}