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
    int bestside[8] = {
        0,0,0,0,0,0,0,0
    };
    int bestorder[8] = {
        0,0,0,0,0,0,0,0
    };
    int mincost = -1u/2;
    bool **projection[8][4];
    int cost[8][4][8];
    int calculateCost() {
        int sum = 0;
        for(int i=0;i<8;i++) {
            sum += cost[i][side[i]][order[i]];
        }
        if(sum < mincost) {
            cout << sum << " " << mincost << "\n";
            for(int i=0;i<8;i++) {
                bestorder[i]=order[i];
                bestside[i]=side[i];
            }
            mincost = sum;
        }
    }
    int xorBm(int sec,int rot,int bmsec){
        int sum = 0;
        for(int i=0;i<8;i++) {
            for(int j=0;j<64;j++){
                sum += projection[sec][rot][i][j] != bm[bmsec*8+i][j];
            }
        }
        return sum;
    }
    int project2d(){
        for(int i=0;i<8;i++) {
            for(int j=0;j<4;j++) {
                projection[i][j] = new bool*[8];
                for(int k = 0;k < 8; ++k){
                    projection[i][j][k] = new bool[64];
                }
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
            for(int rot=0;rot < 4;rot++) {
                for(int bmsec=0;bmsec<8;bmsec++){
                    cost[i][rot][bmsec] = xorBm(i,rot,bmsec);
                }
            }
        }
    }
    void orderPerm(int idx) {
        if(idx==8){
            calculateCost();
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
    void sidePerm(int idx) {
        if(idx==8){
            orderPerm(0);
            return;
        }
        for(int i=0;i<4;i++) {
            side[idx]=i;
            sidePerm(idx+1);
        }
        // side[idx] = s;
        // orderPerm(0);
        // if(s==3)return sidePerm(idx+1,0);
        // return sidePerm(idx,s+1);
    }
    void run(CompFab::VoxelGrid* _vx,bool *_bm[64]){
        vx = _vx;
        bm = _bm;
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
        sidePerm(0);
        vector<int> ans;
        for(int i=0;i<8;i++) {
            ans.push_back(bestorder[i]);
            ans.push_back(bestside[i]);
            cout << bestorder[i] << "," << bestside[i] << "\n";
            delete sec[i];
        }
        for(int i=0;i<8;i++) {
            for(int j=0;j<8;j++) {
                if(bestorder[j] == i) {
                    for(int k=0;k<8;k++) {
                        for(int l=0;l<64;l++) {
                            cout << projection[j][bestside[j]][k][l] << " ";
                        }
                        cout << "\n";
                    }
                }

            }
        }
    }
}