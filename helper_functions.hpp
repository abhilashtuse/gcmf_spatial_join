#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <list>
#include <shapefil.h>
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

// Read MBR text file and fill MBR values into vector
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

// Custom radix sort to generate sortIndex and rankIndex vectors
void CRadixSort(vector<double> X, vector<int> &sortIndex, vector<int> &rankIndex) {
    for (int i = 0 ; i != sortIndex.size() ; i++) {
        sortIndex[i] = i;
        rankIndex[i] = i;
    }
    // cout << "\noriginal array:\n";
    // for (int i = 0 ; i != X.size() ; i++) {
    //     cout << X[i] << "(" << sortIndex[i] << ") ";
    // }
    sort(sortIndex.begin(), sortIndex.end(),
        [&](const double& a, const double& b) {
            return (X[a] < X[b]);
        }
    );
    // cout << "\nsortIndex:";
    // for (int i = 0 ; i != sortIndex.size() ; i++) {
    //     cout << sortIndex[i] << " ";
    // }
    sort(rankIndex.begin(), rankIndex.end(),
        [&](const int& a, const int& b) {
            return (sortIndex[a] < sortIndex[b]);
        }
    );
    // cout << "\nrankIndex:";
    // for (int i = 0 ; i != rankIndex.size() ; i++) {
    //     cout << rankIndex[i] << " ";
    // }
    // cout << endl;
}

// Read shape file and fill R and S MBR vectors.
// 1st half of the MBR's are stored into R and remaining into S vector.
void readShapeFile(string path) {
    SHPHandle _handle = SHPOpen(path.c_str(), "rb");
    if (_handle == NULL) {
        cout << "Cannot open input file:" << path << endl;
        return;
    }
    vector<SHPObject*> shapeObjects;
    int numEntities = 0;
    int shapeType = 0;
    double minBound[4], maxBound[4];

    SHPGetInfo(_handle, &numEntities, &shapeType, minBound, maxBound);
    cout << "numEntities:" << numEntities << " type:" << shapeType;
    for (int i = 0; i < numEntities; i++) {
        SHPObject *shapeObject = SHPReadObject(_handle, i);
        if (shapeObject->nSHPType == SHPT_POLYGON)
            shapeObjects.push_back(shapeObject);
    }

    vector<list<Point>> pointsLists;
    // Iterate over all of the shape objects in the shape file
    int totalShapes = shapeObjects.size();
    for (unsigned int i = 0; i < totalShapes; i++) {
        Mbr temp;
        temp.mbr_points[0] = {shapeObjects[i]->dfXMin,shapeObjects[i]->dfYMin};
        temp.mbr_points[1] = {shapeObjects[i]->dfXMax,shapeObjects[i]->dfYMax};
        if (i < totalShapes / 2)
            R.push_back(temp);
        else
            S.push_back(temp);
    }
    SHPClose(_handle);
}
