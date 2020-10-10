#include <fstream>
#include <iostream>
#include <tga/tgaimage.h>
#include <model/geometry.h>
#include <model/model.h>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

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


int main(int argc, char** argv) {
     //TGAImage image(100, 100, TGAImage::RGB);

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
        for (int j = 0; j < 3; j++) {
            //if (model->vert(face[j])) {
                Vec3f v0 = model->vert(face[j]);
                //std::cout << v0.x << " " << v0.y << " " << v0.z << std::endl;
                Vec3f v1 = model->vert(face[(j + 1) % 3]);
                int x0 = (v0.x + 1.) * width /  2.;
                int y0 = (v0.y + 1.) * height / 2. - height / 2.0f;
                int x1 = (v1.x + 1.) * width /  2.;
                int y1 = (v1.y + 1.) * height / 2. - height / 2.0f;
                Line(x0, y0, x1, y1, image, white);
            //}
        }
    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("out/output.tga");

    delete model;
    return 0;
}