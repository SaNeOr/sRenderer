#include <fstream>
#include <iostream>
#include <algorithm>

#include <tga/tgaimage.h>
#include <model/geometry.h>
#include <model/model.h>
#include <algorithm>
#include <Renderer.h>

Vec3f eye(0, -1, 1);
Vec3f center(0, 0,0);

const Vec3f light_dir = center - eye;

Model* model = NULL;
const int width = 800;
const int height = 800;
//const int depth = 255;


int main(int argc, char** argv) {
  
    Matrix projectionMat = Matrix::identity(4);
    projectionMat[3][3] =  (eye-center).norm();
    
                                //startX, startY       // viewport width height
    Matrix viewportTransform = Renderer::ViewPort(0, 0, width, height);
    Matrix modelViewMat = Renderer::LookAt(eye, center, Vec3f(0, 1, 0));


    TGAImage diff;//  = new TGAImage("assets/texture/african_head_diffuse.tga");
    diff.read_tga_file("assets/texture/jwj_diffuse.tga");
    diff.flip_vertically();

    model = new Model("assets/obj/jwj.obj");
    TGAImage image(width, height, TGAImage::RGB);

    float* zbuffer = new float[width * height];
    for (int i = width * height; i--; zbuffer[i] = -std::numeric_limits<float>::max());



    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);

        Vec2i screen_coords[3];
        Vec3f world_coords[3];
        Vec3f pts[3];

        for (int j = 0; j < 3; j++) {
            Vec3f v = model->vert(face[j]); 
            world_coords[j] = v;


            //pts[j] = World2screen(v);
            pts[j] = Renderer::m2v(  (viewportTransform * projectionMat * modelViewMat * Renderer::v2m(v)));

			pts[j].x = int(pts[j].x);
			pts[j].y = int(pts[j].y);
            
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
            Renderer::Triangle(pts, zbuffer, image, diff, uv, width, height);
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
			}
		}
		zbimage.flip_vertically(); // i want to have the origin at the left bottom corner of the image
		zbimage.write_tga_file("out/zbuffer.tga");
	}

    delete model;
    return 0;
}