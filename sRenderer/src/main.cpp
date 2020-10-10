#include <fstream>
#include <iostream>
#include <tga/tgaimage.h>
#include <model/geometry.h>
#include <model/model.h>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

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
//      Method1: Line-by-line Scan
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

//      Method2: Point In Triangle
/*  TODO
void Triange(){}
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

    model = new Model("assets/obj/jwj.obj");

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

        Vec2i vertices[3];
        for (int j = 0; j < 3; j++) {
            Vec3f v = model->vert(face[j]); 
            //std::cout << v.x << " " << v.y << " " << v.z << std::endl;
            int vx = (v.x + 1.0f) * width / 2.0f;
            int vy = (v.y + 1.0f) * height / 2.0f - height / 2.0f;
            vertices[j] = Vec2i(vx, vy);
        }
        Triangle(vertices[0], vertices[1], vertices[2], image, TGAColor(rand() % 255, rand() % 255, rand() % 255, 255));

    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("out/output.tga");

    delete model;
    return 0;
}