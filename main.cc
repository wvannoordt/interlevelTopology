#include "cmf.h"

using cmf::print;
using cmf::strformat;
using cmf::strunformat;
using cmf::cell_t;
using cmf::ZFill;

struct FilledRegion
{
    cmf::RefinementTreeNode* lb;
    int i, j, k;
};

class DomainTopology
{
    public:
        DomainTopology(cmf::CartesianMesh* mesh_in)
        {
            mesh = mesh_in;
        }
        void Output(std::string filename)
        {
            std::ofstream myfile;
            myfile.open(filename);
            int numTerminalBlocks = 0;
            for (auto lb: *mesh)
            {
                if (lb->IsTerminal()) numTerminalBlocks++;
            }
            int pointsPerBlock = 5*5;
            int cellsPerBlock = 4*4;
            if (cmf::Is3D()) pointsPerBlock*= 5;
            if (cmf::Is3D()) cellsPerBlock*= 4;
            int numPoints = pointsPerBlock*numTerminalBlocks;
            myfile << strformat("# vtk DataFile Version 3.0\ncmf\nASCII\nDATASET UNSTRUCTURED_GRID\nPOINTS {} float\n", numPoints);
            double dx[5] = {0.0, 0.1, 0.5, 0.9, 1.0};
            for (auto lb: *mesh)
            {
                if (lb->IsTerminal())
                {
                    auto bnds = lb->GetBlockBounds();
                    for (int n = 0; n < pointsPerBlock; n++)
                    {
                        cmf::Vec3<int> ijk = 0;
                        ijk[0] = n%5;
                        ijk[1] = ((n-ijk[0])/5)%5;
                        ijk[2] = ((n-ijk[0]-5*ijk[1])/25)%5;
                        cmf::Vec3<double> xyz = 0;
                        for (int d = 0; d < cmf::Dim(); d++)
                        {
                            xyz[d] = bnds[2*d]+dx[ijk[d]]*(bnds[2*d+1] - bnds[2*d])
                        }
                        myfile << strformat("{} {} {}\n", xyz[0], xyz[1], xyz[2]);
                    }
                }
            }
            ptPerCell = 4;
            if (cmf::Is3D()) ptPerCell = 8;
            myfile << strformat("CELLS {} {}\n", numTerminalBlocks*cellsPerBlock, ptPerCell*numTerminalBlocks*cellsPerBlock);
            int base = 0;
            for (auto lb: *mesh)
            {
                if (lb->IsTerminal())
                {
                    for (int n = 0; n < cellsPerBlock; n++)
                    {
                        cmf::Vec3<int> ijk = 0;
                        ijk[0] = n%4;
                        ijk[1] = ((n-ijk[0])/4)%4;
                        ijk[2] = ((n-ijk[0]-4*ijk[1])/16)%4;
                        myfile << ptPerCell << " ";
                        for (int f = 0; f < ptPerCell; f++)
                        {
                            
                            if (f < ptPerCell-1) myfile << " ";
                        }
                    }
                    base += pointsPerBlock;
                }
            }
            myfile.close();
        }
    private:
        cmf::CartesianMesh* mesh;
};

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