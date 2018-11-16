#include <iostream>
#include <vector>
#include <stdint.h>
#include <algorithm>
#include <fstream>
#include <map>

using namespace std;

// Coordinate pair
struct Point {
    double x, y;
};

// MBR with bottom-left and top-right pair of coordinates
struct Mbr {
    Point mbr_points[2];
};

vector<Mbr> R, S;

void readMbrFile(string filename, vector<Mbr> &d) {
    std::ifstream mbr_file(filename);
    double x0, y0, x1, y1;
    char c = ',';
    if(!mbr_file) {
        cout << "Cannot open input file.\n";
        return;
    }
    while (mbr_file >> x0 >> c >> y0 >> c >> x1 >> c >> y1)
    {
        //cout << x0 << " " << y0 << " " << x1 << " " << y1 << endl;
        Mbr temp;
        temp.mbr_points[0] = {x0,y0};
        temp.mbr_points[1] = {x1,y1};
        d.push_back(temp);
    }
    mbr_file.close();
}
void CRadixSort(vector<double> X, vector<int> &sortIndex, vector<int> &rankIndex) {
    for (int i = 0 ; i != sortIndex.size() ; i++) {
        sortIndex[i] = i;
        rankIndex[i] = i;
    }
    cout << "\noriginal array:\n";
    for (int i = 0 ; i != X.size() ; i++) {
        cout << X[i] << "(" << sortIndex[i] << ") ";
    }
    sort(sortIndex.begin(), sortIndex.end(),
        [&](const double& a, const double& b) {
            return (X[a] < X[b]);
        }
    );
    cout << "\nsortIndex:";
    for (int i = 0 ; i != sortIndex.size() ; i++) {
        cout << sortIndex[i] << " ";
    }
    sort(rankIndex.begin(), rankIndex.end(),
        [&](const int& a, const int& b) {
            return (sortIndex[a] < sortIndex[b]);
        }
    );
    cout << "\nrankIndex:";
    for (int i = 0 ; i != rankIndex.size() ; i++) {
        cout << rankIndex[i] << " ";
    }
    cout << endl;
}

void sortBasedMbrFilter(vector<int> &sortIndex, vector<int> &rankIndex, vector<pair<int,int>> &C) {
    int r_size = R.size();
    for (int i = 0; i < r_size; i++) {
        for (int j = rankIndex[2*i]+1; j < rankIndex[2*i+1]; j++) {
            int index = sortIndex[j];
            int mbr_j = ((index/2)-r_size);
            if (index%2 == 0 && mbr_j >= 0) {
                //cout << " i:" << i << " j:" << j << " x_i0:" << X[sortIndex[rankIndex[2*i]]] << " x_j0:" << X[sortIndex[j]];
                double mbr_i_ly = R[i].mbr_points[0].y;
                double mbr_i_ry = R[i].mbr_points[1].y;
                double mbr_j_ly = S[mbr_j].mbr_points[0].y;
                double mbr_j_ry = S[mbr_j].mbr_points[1].y;
                //cout << "\nmbr_i_ly:" << mbr_i_ly << " mbr_i_ry:" << mbr_i_ry << " mbr_j_ly:" << mbr_j_ly << " mbr_j_ry:" << mbr_j_ry;

                /*
                A.X1 < B.X2:	true
                A.X2 > B.X1:	true
                A.Y1 < B.Y2:	true
                A.Y2 > B.Y1:	true
                Intersect:	    true
                */
                if (mbr_i_ly < mbr_j_ry && mbr_i_ry > mbr_j_ly) {
                    cout << "\n" << i << " overlaps with " << (index/2) << endl;
                    C.push_back(pair<int,int>(i, (index/2)));
                } else {
                    //cout << "\nNo overlap between " << i << " & " << (index/2) << endl;
                }
            }
        }
        cout << endl;
    }
}
int main() {
    readMbrFile("mbr_r.txt", R);
    readMbrFile("mbr_s.txt", S);

    // X = x-coordinates of R & S
    vector<double> X;
    for (auto mbr: R) {
        X.push_back(mbr.mbr_points[0].x);
        X.push_back(mbr.mbr_points[1].x);
    }
    for (auto mbr: S) {
        X.push_back(mbr.mbr_points[0].x);
        X.push_back(mbr.mbr_points[1].x);
    }
    vector<int> sortIndex(X.size(), 0);
    vector<int> rankIndex(X.size(), 0);

    CRadixSort(X, sortIndex, rankIndex);

    vector<pair<int,int>> C;
    sortBasedMbrFilter(sortIndex, rankIndex, C);

    return 0;
}
