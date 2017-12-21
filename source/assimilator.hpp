#include <iostream>

using namespace std;

namespace assimilator{
    CompFab::VoxelGrid* vx;
    bool **bm;
    int side[8] = {
        0,0,0,0,0,0,0,0
    };
    int order[8] = {
        0,0,0,0,0,0,0,0
    };
    int mark[8] = {
        0,0,0,0,0,0,0,0
    };
    bool **projection[8][4];
    int cost[8][4][8][4];
    int calculateCost() {
        for(int i=0;i<8;i++) {
            for(int i=0;i<4;i++) {

            }
        }
    }
    int xorBm(int sec,int rot,int bmsec){
        cout << sec << " " << rot << " " << bmsec << "\n";

        return 0;
    }
    int project2d(){
        for(int i=0;i<8;i++) {
            for(int j=0;j<4;j++) {
                projection[i][j] = new bool*[8];
                for(int k = 0;k < 8; ++k){
                    projection[i][j][k] = new bool[64];
                }
                // for(int k = 0;k < 8; ++k){
                //     int err = xorBm(i,j,k);
                // }
            }
            for(int j=0;j<4;j++) {
                for(int x = 0;x < 8; ++x){
                    for(int y = 0;y < 64; ++y){
                        projection[i][j][x][y] = false;
                    }
                }
            }
            for(int j=0;j<8;j++) {
                for(int x = 0;x < 64; ++x){
                    for(int y = 0;y < 64;y++) {
                        if(!vx->isInside(x,y,i*8+j)){
                            continue;
                        }
                        projection[i][0][j][x] = true;
                        projection[i][1][j][y] = true;
                        projection[i][2][j][63-x] = true;
                        projection[i][3][j][63-y] = true;
                    }
                }
            }
            for(int j=0;j<4;j++) {
                for(int k=0;k<8;k++) {
                    for(int l=0;l<64;l++) {
                        cout << projection[i][j][k][l] << " ";
                    }
                    cout << "\n";
                }
                cout << "\n";
            }
        }
    }
    void orderPerm(int idx) {
        if(idx==8){
            return;
        };
        for(int i=0;i<8;i++){
            if(!mark[i]) {
                mark[i] = true;
                order[idx] = i;
                orderPerm(idx + 1);
                mark[i] = false;
            }
        }
    }
    void sidePerm(int idx,int side) {
        if(idx==8)return;
        orderPerm(0);
        if(side==3)return sidePerm(idx+1,0);
        return sidePerm(idx,side+1);
    }
    void run(CompFab::VoxelGrid* _vx,bool *_bm[64]){
        vx = _vx;
        CompFab::VoxelGrid* sec[8];
        for(int i=0;i<8;i++) {
            sec[i] = new CompFab::VoxelGrid(
                vx -> m_lowerLeft,
                vx -> m_dimX, 
                vx -> m_dimY, 
                vx -> m_dimZ,
                vx -> m_spacing
            );
        }
        project2d();
        sidePerm(0,0);
        
        for(int i=0;i<8;i++) {
            delete sec[i];
        }
    }
}