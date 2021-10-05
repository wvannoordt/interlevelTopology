#include "cmf.h"
#include "DomainTopology.h"

void Refine(cmf::CartesianMesh& mesh)
{
    cmf::Vec3<double> x1(0.48, 0.48, 0.0);
    cmf::Vec3<double> x2(0.48, 0.58, 0.0);
    
    std::vector<cmf::RefinementTreeNode*> nodes;
    std::vector<char> refs;
    
    nodes.push_back(mesh.Blocks()->GetNodeAt(x1));
    refs.push_back(1);
    
    nodes.push_back(mesh.Blocks()->GetNodeAt(x2));
    refs.push_back(2);
    
    mesh.Blocks()->RefineNodes(nodes, refs);
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
    
    Refine(domain);
    
    DomainTopology topo(&domain);
    
    topo.Output("topology.vtk");
    return 0;
}