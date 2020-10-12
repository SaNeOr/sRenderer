#include <fstream>
#include <iostream>
#include <tga/tgaimage.h>
#include <model/geometry.h>
#include <model/model.h>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

const Vec3f light_dir = Vec3f(0, 0, -1);

Model* model = NULL;
const int width = 1000;
const int height = 1000;

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

    model = new Model("assets/obj/african_head.obj");

    TGAImage image(width, height, TGAImage::RGB);

    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);

        //for (int j = 0; j < 3; j++) {
        //    //if (model->vert(face[j])) {
        //        Vec3f v0 = model->vert(face[j]);
        //        //std::cout << v0.x << " " << v0.y << " " << v0.z << std::endl;
        //        Vec3f v1 = model->vert(face[(j + 1) % 3]);
        //        int x0 = (v0.x + 1.) * width /  2.;
        //        int y0 = (v0.y + 1.) * height / 2. - height / 2.0f;
        //        int x1 = (v1.x + 1.) * width /  2.;
        //        int y1 = (v1.y + 1.) * height / 2. - height / 2.0f;
        //        Line(x0, y0, x1, y1, image, white);
        //    //}
        //}
        Vec2i screen_coords[3];
        Vec3f world_coords[3];
        for (int j = 0; j < 3; j++) {
            Vec3f v = model->vert(face[j]); 
            //std::cout << v.x << " " << v.y << " " << v.z << std::endl;
            int vx = (v.x + 1.0f) * width / 2.0f;
            int vy = (v.y + 1.0f) * height / 2.0f;//-height / 2.0f;
            screen_coords[j] = Vec2i(vx, vy);
            world_coords[j] = v;
        }

        Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);        //   fuck xor!
        n.normalize();

        float indensity = n * light_dir;
        if (indensity > 0) {
            Triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(indensity * 255, indensity * 255, indensity * 255, 255));
        }
    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("out/output.tga");

    delete model;
    return 0;
}