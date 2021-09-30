#include "cmf.h"

using cmf::print;
using cmf::strformat;
using cmf::strunformat;
using cmf::cell_t;
using cmf::ZFill;

#define DX_SEARCH 1e-4;

struct FilledRegion
{
    cmf::RefinementTreeNode* lb;
    int i, j, k;
}

int main(int argc, char** argv)
{
    std::string inFile = "input.ptl";
    cmf::ReadInput(inFile);
    cmf::globalSettings = cmf::GlobalSettings(cmf::mainInput["GlobalSettings"]);
    cmf::CreateParallelContext(&argc, &argv);
    PTL::PropertyTree localTree;
    localTree.Read(inFile);
    cmf::CartesianMeshInputInfo inputInfo(localTree["Domain"]);
    cmf::CartesianMesh domain(inputInfo);
    
    
    
    return 0;
}