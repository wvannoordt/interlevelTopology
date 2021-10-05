#include "cmf.h"
using cmf::print;
using cmf::strformat;
using cmf::strunformat;
using cmf::cell_t;
using cmf::ZFill;
#ifndef DOMAIN_TOPOLOGY_H

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
            deltaXGuard = 1e-2;
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
            for (auto lb: *mesh)
            {
                if (lb->IsTerminal())
                {
                    auto bnds = lb->GetBlockBounds();
                    for (int n = 0; n < pointsPerBlock; n++)
                    {
                        double dx[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
                        cmf::Vec3<int> ijk = 0;
                        ijk[0] = n%5;
                        ijk[1] = ((n-ijk[0])/5)%5;
                        ijk[2] = ((n-ijk[0]-5*ijk[1])/25)%5;
                        cmf::Vec3<double> xyz = 0;
                        for (int d = 0; d < cmf::Dim(); d++)
                        {
                            dx[0] = 0.0;
                            dx[1] = deltaXGuard;
                            dx[2] = 0.5*(bnds[2*d+1]-bnds[2*d]);
                            dx[4] = 2*dx[2];
                            dx[3] = dx[4] - deltaXGuard;
                            xyz[d] = bnds[2*d]+dx[ijk[d]];
                        }
                        myfile << strformat("{} {} {}\n", xyz[0], xyz[1], xyz[2]);
                    }
                }
            }
            int ptPerCell = 4;
            if (cmf::Is3D()) ptPerCell = 8;
            myfile << strformat("CELLS {} {}\n", numTerminalBlocks*cellsPerBlock, (1+ptPerCell)*numTerminalBlocks*cellsPerBlock);
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
                            cmf::Vec3<int> dijk = 0;
                            dijk[0] = f%2;
                            dijk[1] = ((f-dijk[0])/2)%2;
                            dijk[2] = ((f-dijk[0]-2*dijk[1])/4)%2;
                            cmf::Vec3<int> realIjk = ijk + dijk;
                            int num = base + realIjk[0]+realIjk[1]*5+realIjk[2]*5*5;
                            myfile << num;
                            if (f < ptPerCell-1) myfile << " ";
                        }
                        myfile << "\n";
                    }
                    base += pointsPerBlock;
                }
            }
            myfile << "CELL_TYPES " << numTerminalBlocks*cellsPerBlock << "\n";
            for (auto lb: *mesh)
            {
                if (lb->IsTerminal())
                {
                    for (int n = 0; n < cellsPerBlock; n++) myfile << (cmf::Is3D()?11:8) << "\n";
                }
            }
            myfile.close();
        }
    private:
        cmf::CartesianMesh* mesh;
        double deltaXGuard;
};

#endif