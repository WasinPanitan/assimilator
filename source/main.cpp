//Computational Fabrication Assignment #1
#include <algorithm>
#include <iostream>
#include <cmath>
#include <set>
#include <stdlib.h>
#include <vector>
#include "../include/CompFab.h"
#include "../include/Mesh.h"
#include <fstream>
#include "assimilator.hpp"

//Ray-Triangle Intersection
//Returns 1 if triangle and ray intersect, 0 otherwise
int rayTriangleIntersection(CompFab::Ray &ray, CompFab::Triangle &triangle)
{
    CompFab::Vec3 e1 = triangle.m_v2 - triangle.m_v1;
    CompFab::Vec3 e2 = triangle.m_v3 - triangle.m_v1;
    CompFab::Vec3 P, Q, T;
    float det, inv_det, u, v;
    float t;
    
    //Begin calculating determinant - also used to calculate u parameter
    P =  ray.m_direction%e2;
    
    //if determinant is near zero, ray lies in plane of triangle
    det = e1*P;
    
    //NOT CULLING
    if(det > -EPSILON && det < EPSILON)
        return 0;
    
    inv_det = 1.f / det;
    
    //calculate distance from V1 to ray origin
    T = ray.m_origin - triangle.m_v1;
    
    //Calculate u parameter and test bound
    u = T*P*inv_det;

    //The intersection lies outside of the triangle
    if(u < 0.f || u > 1.f) return 0;
    
    //Prepare to test v parameter
    Q = T%e1;
    
    //Calculate V parameter and test bound
    v = ray.m_direction*Q*inv_det;
    
    //The intersection lies outside of the triangle
    if(v < 0.f || u + v  > 1.f)
        return 0;
    
    t = e2*Q*inv_det;
    
    if(t > EPSILON) { //ray intersection
        return 1;
    }
    
    // No hit, no win
    return 0;

}

//Triangle list (global)
typedef std::vector<CompFab::Triangle> TriangleList;

TriangleList g_triangleList;
std::set<unsigned int> *triangleBucket;
CompFab::VoxelGrid *g_voxelGrid;


bool object3d[64][64][64];
bool projection1[64][64];
bool projection2[64][64];


unsigned int getBucketY(double y){
    return (int)(y/g_voxelGrid->m_spacing);
}

unsigned int getBucketZ(double z){
    return (int)(z/g_voxelGrid->m_spacing);
}

void addToBucket(unsigned int y,unsigned int z, unsigned int idx){
    triangleBucket[g_voxelGrid->m_dimY*z + y].insert(idx);
}


//Number of intersections with surface made by a ray originating at voxel and cast in direction.
int numSurfaceIntersections(CompFab::Vec3 &voxel, CompFab::Vec3 &direction)
{
    
    unsigned int hits = 0;

    //Cast ray in x-direction
    CompFab::Ray ray;
    ray.m_origin = voxel;
    ray.m_direction = direction;

    for(auto idx: triangleBucket[
        g_voxelGrid->m_dimY * getBucketZ(ray.m_origin.m_z) 
        + getBucketY(ray.m_origin.m_y)
    ])
    {
        hits += rayTriangleIntersection(ray,g_triangleList[idx]);
    }
    
    return hits;
}

bool loadMesh(char *filename, unsigned int dim)
{
    g_triangleList.clear();
    
    Mesh *tempMesh = new Mesh(filename, true);
    
    CompFab::Vec3 v1, v2, v3;

    //copy triangles to global list
    for(unsigned int tri =0; tri<tempMesh->t.size(); ++tri)
    {
        v1 = tempMesh->v[tempMesh->t[tri][0]];
        v2 = tempMesh->v[tempMesh->t[tri][1]];
        v3 = tempMesh->v[tempMesh->t[tri][2]];
        g_triangleList.push_back(CompFab::Triangle(v1,v2,v3));
    }

    //Create Voxel Grid
    CompFab::Vec3 bbMax, bbMin;
    BBox(*tempMesh, bbMin, bbMax);
    
    //Build Voxel Grid
    double bbX = bbMax[0] - bbMin[0];
    double bbY = bbMax[1] - bbMin[1];
    double bbZ = bbMax[2] - bbMin[2];
    double spacing;
    if(bbX > bbY && bbX > bbZ)
    {
        spacing = bbX/(double)(dim-2);
    } else if(bbY > bbX && bbY > bbZ) {
        spacing = bbY/(double)(dim-2);
    } else {
        spacing = bbZ/(double)(dim-2);
    }
    
    CompFab::Vec3 hspacing(0.5*spacing, 0.5*spacing, 0.5*spacing);
    
    g_voxelGrid = new CompFab::VoxelGrid(bbMin-hspacing, dim, dim, dim, spacing);

    delete tempMesh;
    
    return true;
}



void saveVoxelsToObj(const char * outfile,CompFab::VoxelGrid* voxel)
{
 
    Mesh box;
    Mesh mout;
    int nx = voxel->m_dimX;
    int ny = voxel->m_dimY;
    int nz = voxel->m_dimZ;
    double spacing = voxel->m_spacing;
    
    CompFab::Vec3 hspacing(0.5*spacing, 0.5*spacing, 0.5*spacing);
    
    for (int ii = 0; ii < nx; ii++) {
        for (int jj = 0; jj < ny; jj++) {
            for (int kk = 0; kk < nz; kk++) {
                if(!voxel->isInside(ii,jj,kk)){
                    continue;
                }
                projection1[kk][ii] = true;
                projection2[kk][jj] = true;
                CompFab::Vec3 coord(0.5f + ((double)ii)*spacing, 0.5f + ((double)jj)*spacing, 0.5f+((double)kk)*spacing);
                CompFab::Vec3 box0 = coord - hspacing;
                CompFab::Vec3 box1 = coord + hspacing;
                makeCube(box, box0, box1);
                mout.append(box);
            }
        }
    }

    mout.save_obj(outfile);
}




bool** ReadBMP(char* filename)
{
    int i;
    FILE* f = fopen(filename, "rb");

    if(f == NULL)
        throw "Argument Exception";

    unsigned char info[54];
    fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

    // extract image height and width from header
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];
    int depth = *(int*)&info[28];

    std::cout << "Image: " << filename << std::endl;
    std::cout << "Width: " << width << std::endl;
    std::cout << "Height: " << height << std::endl;
    std::cout << "Depth: " << depth << std::endl;

    int row_padded = (width*3 + 3) & (~3);
    unsigned char* data = new unsigned char[row_padded];
    unsigned char tmp;

    bool** bitmap = new bool*[height];
    for(int i = 0;i < height; ++i){
        bitmap[i] = new bool[width];
    }

    for(int i = 0; i < height; i++)
    {
        fread(data, sizeof(unsigned char), row_padded, f);
        for(int j = 0; j < width*3; j += 3)
        {
            // Convert (B, G, R) to (R, G, B)
            tmp = data[j];
            data[j] = data[j+2];
            data[j+2] = tmp;

            if(((int)data[j]+(int)data[j+1]+(int)data[j+2])/3 < 128){
                bitmap[i][j/3] = true;
            }
            else{
                bitmap[i][j/3] = false;
            }
            //cout << "Height: " << i << " Width: " << j/3 << " R: "<< (int)data[j] << " G: " << (int)data[j+1]<< " B: " << (int)data[j+2]<< endl;
        }
    }

    fclose(f);
    return bitmap;
}

int main(int argc, char **argv)
{

    unsigned int dim = 64; //dimension of voxel grid (e.g. 32x32x32)

    //Load OBJ
    if(argc < 3)
    {
        std::cout<<"Usage: Voxelizer InputMeshFilename.obj InputImage.bmp \n";
        exit(0);
    }
    for(int i = 0;i <64; i++){
        for(int j = 0;j< 64;j++){
            projection1[i][j] = false;
            projection2[i][j] = false;
            for(int k = 0;k< 64;k++){
                object3d[i][j][k] = false;
            }
        }
    }
    std::cout<<"Load Mesh : "<<argv[1]<<"\n";
    loadMesh(argv[1], dim);
    bool** bitmap = ReadBMP(argv[2]);

    std::cout<<"Voxelizing with dimension " << dim << "\n";
    //Cast ray, check if voxel is inside or outside
    //even number of surface intersections = outside (OUT then IN then OUT)
    // odd number = inside (IN then OUT)
    CompFab::Vec3 voxelPos;
    CompFab::Vec3 direction(1.0,0.0,0.0);
    triangleBucket = new std::set<unsigned int>[g_voxelGrid->m_size/g_voxelGrid->m_dimX];
    printf("m_spacing: %lf\n",g_voxelGrid->m_spacing);
    int xy,xz,ny,nz;

    //BucketSort
    for(unsigned int tri=0; tri<g_triangleList.size(); ++tri) {
        xy = xz = 0;
        ny = nz = dim;  
        CompFab::Vec3 relative_v1 =  g_triangleList[tri].m_v1 - g_voxelGrid->m_lowerLeft;
        CompFab::Vec3 relative_v2 =  g_triangleList[tri].m_v2 - g_voxelGrid->m_lowerLeft;
        CompFab::Vec3 relative_v3 =  g_triangleList[tri].m_v3 - g_voxelGrid->m_lowerLeft;
        xy = std::max(getBucketY(relative_v1.m_y),std::max(getBucketY(relative_v2.m_y),getBucketY(relative_v3.m_y)));
        ny = std::min(getBucketY(relative_v1.m_y),std::min(getBucketY(relative_v2.m_y),getBucketY(relative_v3.m_y)));
        xz = std::max(getBucketZ(relative_v1.m_z),std::max(getBucketZ(relative_v2.m_z),getBucketZ(relative_v3.m_z)));
        nz = std::min(getBucketZ(relative_v1.m_z),std::min(getBucketZ(relative_v2.m_z),getBucketZ(relative_v3.m_z)));
        for(unsigned int bucketZ = nz; bucketZ < xz; ++bucketZ){
            for(unsigned int bucketY = ny; bucketY < xy; ++bucketY){
                //printf("%d %d\n",bucketY,bucketZ);
                addToBucket(bucketY, bucketZ, tri);
            }
        }
    }
    int sum = 0;
    for(int i=0;i<g_voxelGrid->m_size/g_voxelGrid->m_dimX;++i){
        sum += triangleBucket[i].size();
    }
    //printf("%d\n",sum);
    for(unsigned int voxelX = 0; voxelX < g_voxelGrid->m_dimX; ++voxelX) {
            //printf("slice %d\n",voxelX);
            for(unsigned int voxelY = 0; voxelY < g_voxelGrid->m_dimY; ++voxelY) {
                    for(unsigned int voxelZ = 0; voxelZ < g_voxelGrid->m_dimZ; ++voxelZ) {
                        voxelPos = g_voxelGrid->m_lowerLeft + (CompFab::Vec3(voxelX,voxelY,voxelZ) * g_voxelGrid->m_spacing);
                        if((numSurfaceIntersections(voxelPos, direction) % 2) == 0)
                        {
                            g_voxelGrid->isInside(voxelX, voxelY, voxelZ) = false;
                        } else {
                            object3d[voxelX][voxelY][voxelZ] = true;
                            g_voxelGrid->isInside(voxelX, voxelY, voxelZ) = true;

                        }
                    }
            }
    }
    delete[] triangleBucket;
    
    
    //Write out voxel data as obj
    saveVoxelsToObj("output.obj",g_voxelGrid);
    for(int i = 0;i <64; i++){
        for(int k = 0;k< 64;k++){
            if(projection1[i][k]){
                std::cout << "1 ";
            }else
                std::cout << "0 ";
        }
        std::cout << "\n";
    }
    // projection1 is projection of 3d object to plane

    std::cout << "\n";
    std::cout << "\n";
    for(int i = 0;i <64; i++){
        for(int k = 0;k< 64;k++){
            if(projection2[i][k]){
                std::cout << "1 ";
            }else
                std::cout << "0 ";
        }
        std::cout << "\n";
    }
    // projection2 is projection of 3d object to plane on another axis

    std::cout << "\n";
    std::cout << "\n";
    for(int i = 0;i <64; i++){
        for(int k = 0;k< 64;k++){
            if(bitmap[i][k]){
                std::cout << "1 ";
            }else
                std::cout << "0 ";
        }
        std::cout << "\n";
    }
    //bitmap is translation from picture bmp to 2d array

    assimilator::run(g_voxelGrid,bitmap);
    delete g_voxelGrid;

}