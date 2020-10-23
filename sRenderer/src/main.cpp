#include <fstream>
#include <iostream>
#include <algorithm>

#include <tga/tgaimage.h>
#include <model/geometry.h>
#include <model/model.h>
#include <algorithm>
#include <Renderer.h>
#include <stdio.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

Vec3f eye(0, -1, 1);
Vec3f center(0, 0,0);

const Vec3f light_dir = center - eye;

Model* model = NULL;
const int width = 800;
const int height = 800;
//const int depth = 255;


int main(int argc, char** argv) {


    /// /////=====================================================/////////
#pragma region {
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);     // widht / 2 height /2
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);


    /* Initialize the library */
    if (!glfwInit())
        return -1;


    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    const char* glsl_version = "#version 330";


    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
#pragma endregion

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    bool show_demo_window = true;
    /* Loop until the user closes the window */

    Matrix projectionMat = Matrix::identity(4);
    projectionMat[3][3] = (eye - center).norm();

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


    float t = -5.0f;
    while (!glfwWindowShouldClose(window))
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);


        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)


            ImGui::SliderFloat("fuck t", &t, -480.0f, 480.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

#pragma region ====


        // Rendering
        ImGui::Render();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);

        //clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBegin(GL_POINTS);
#pragma endregion
        //===============My Renderer


        //Renderer::LineGL(t,0, 0, 480, red);
        

        for (int i = 0; i < model->nfaces(); i++) {
            std::vector<int> face = model->face(i);

            Vec2i screen_coords[3];
            Vec3f world_coords[3];
            Vec3f pts[3];

            for (int j = 0; j < 3; j++) {
                Vec3f v = model->vert(face[j]);
                world_coords[j] = v;


                //pts[j] = World2screen(v);
                pts[j] = Renderer::m2v((viewportTransform * projectionMat * modelViewMat * Renderer::v2m(v)));

                pts[j].x = int(pts[j].x);
                pts[j].y = int(pts[j].y);

            }

            Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);        //   fuck xor!
            n.normalize();

            float indensity = n * light_dir;
            if (indensity > 0)
            {
                Vec2i uv[3];
                for (int k = 0; k < 3; k++) {
                    uv[k] = model->uv(i, k);
                }

                Renderer::WireFrameGL(pts, white);
                //Triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(indensity * 255, indensity * 255, indensity * 255, 255));
                //Triangle(pts, zbuffer, image, TGAColor(indensity * 255, indensity * 255, indensity * 255, 255));
                //Renderer::Triangle(pts, zbuffer, image, diff, uv, width, height);
            }

        }

        //image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
        //zbimage.flip_vertically(); // i want to have the origin at the left bottom corner of the image
         
        //===============//

#pragma region }
        glEnd();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();

    }
    glfwTerminate();
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
#pragma endregion


    delete model;



 //   Matrix projectionMat = Matrix::identity(4);
 //   projectionMat[3][3] =  (eye-center).norm();
 //   
 //                               //startX, startY       // viewport width height
 //   Matrix viewportTransform = Renderer::ViewPort(0, 0, width, height);
 //   Matrix modelViewMat = Renderer::LookAt(eye, center, Vec3f(0, 1, 0));


 //   TGAImage diff;//  = new TGAImage("assets/texture/african_head_diffuse.tga");
 //   diff.read_tga_file("assets/texture/jwj_diffuse.tga");
 //   diff.flip_vertically();

 //   model = new Model("assets/obj/jwj.obj");
 //   TGAImage image(width, height, TGAImage::RGB);

 //   float* zbuffer = new float[width * height];
 //   for (int i = width * height; i--; zbuffer[i] = -std::numeric_limits<float>::max());



 //   for (int i = 0; i < model->nfaces(); i++) {
 //       std::vector<int> face = model->face(i);

 //       Vec2i screen_coords[3];
 //       Vec3f world_coords[3];
 //       Vec3f pts[3];

 //       for (int j = 0; j < 3; j++) {
 //           Vec3f v = model->vert(face[j]); 
 //           world_coords[j] = v;


 //           //pts[j] = World2screen(v);
 //           pts[j] = Renderer::m2v(  (viewportTransform * projectionMat * modelViewMat * Renderer::v2m(v)));

	//		pts[j].x = int(pts[j].x);
	//		pts[j].y = int(pts[j].y);
 //           
 //       }

 //       Vec3f n =  (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);        //   fuck xor!
 //       n.normalize();

 //       float indensity = n * light_dir;
 //       if (indensity > 0) 
 //       {
	//		Vec2i uv[3];
	//		for (int k = 0; k < 3; k++){
	//		    uv[k] = model->uv(i, k);
 //           }
 //          
 //           Renderer::WireFrame(pts, image);
 //           //Triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(indensity * 255, indensity * 255, indensity * 255, 255));
 //           //Triangle(pts, zbuffer, image, TGAColor(indensity * 255, indensity * 255, indensity * 255, 255));
 //           //Renderer::Triangle(pts, zbuffer, image, diff, uv, width, height);
 //       }


 //   }

 //   image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
 //   image.write_tga_file("out/output.tga");


	//{ // dump z-buffer
	//	TGAImage zbimage(width, height, TGAImage::GRAYSCALE);
	//	for (int i = 0; i < width; i++) {
	//		for (int j = 0; j < height; j++) {

 //               if(zbuffer[i + j * width] >= 0)
	//			    zbimage.set(i, j, TGAColor(zbuffer[i + j * width] * 255, 1));
	//		}
	//	}
	//	zbimage.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	//	zbimage.write_tga_file("out/zbuffer.tga");
	//}

    //delete model;
    return 0;
}