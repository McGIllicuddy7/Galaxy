#ifdef __linux
#include <raylib/raylib.h>
#include <raylib/raymath.h>
#else
#include <raylib.h>
#include <raymath.h>
#endif
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#define NUM_STARS 32000
void update_positions(float * thetas, float * radii,float * zs, Matrix * buffer, int num_stars){
    for(int i =0; i<num_stars; i++){
        float radius = radii[i];
        float theta = thetas[i];
        float dtheta =  2/sqrt(radius+1);
        theta +=dtheta*GetFrameTime();
        thetas[i] = theta;
        buffer[i] = MatrixTranslate(radius*cos(theta),radius*sin(theta), zs[i]);
    }
}

int main(void){
 
    Matrix stars_trans[NUM_STARS] = {};
    float stars_thetas[NUM_STARS] = {};
    float stars_radii[NUM_STARS] = {};
    float stars_z[NUM_STARS] = {};
    unsigned int occupied[NUM_STARS/32] = {};
    int true_count = 0;
    int false_count = 0;
    srand(time(0));
    for(int i =0; i<NUM_STARS; i++){
        float radius = (rand()%3200+(1));
        radius *= radius;
        radius /= 13653;
        float theta = rand()%(int)(2*PI*1000);
        float z= rand()%100+800;
        theta = theta/1000;
        stars_thetas[i] = theta;
        stars_radii[i] = radius;
        stars_z[i] = z;
        stars_trans[i] = MatrixTranslate(radius*cos(theta),radius*sin(theta),z);
        int idx = i/32;
        int offset = i%32;
        int v_act = (rand()%10) == 0;
        if(v_act){
            true_count++;
        } else{
            false_count++;
        }
        int value = v_act <<(31-offset);
        occupied[idx]|= value;
    }
    printf("true count: %d. false count:%d", true_count, false_count);
    SetTraceLogLevel(LOG_ERROR);
    InitWindow(1000, 1000,"Hello Sailor");
    SetTargetFPS(61);
    Material m = LoadMaterialDefault();
    m.shader = LoadShader("shaders/instance.vs", "shaders/instance.fs");
    m.shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(m.shader, "mvp");
    m.shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(m.shader, "viewPos");
    m.shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocationAttrib(m.shader, "instanceTransform");
    int buffer = GetShaderLocation(m.shader, "occupied");
    SetShaderValueV(m.shader, buffer, occupied, SHADER_UNIFORM_INT, NUM_STARS/32);
    m.maps[MATERIAL_MAP_DIFFUSE].color = RED;
    Mesh mesh  = GenMeshSphere(3, 5, 5);
    Camera3D cam = {.fovy = 80, .position = {0,0,-100}, .projection = CAMERA_PERSPECTIVE, .target = {0,0,-1}, .up = {0,1,0}};
    Matrix trans = MatrixTranslate(0,0, 100);
    m.maps[MATERIAL_MAP_DIFFUSE].color =WHITE;
    while (!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(cam);
        DrawMeshInstanced(mesh, m, stars_trans, NUM_STARS);
        EndMode3D();
        DrawFPS(940, 60);
        update_positions(stars_thetas, stars_radii, stars_z,stars_trans, NUM_STARS);
        EndDrawing();
    } 
    return 0;
}