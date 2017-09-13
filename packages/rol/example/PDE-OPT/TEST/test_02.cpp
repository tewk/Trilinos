// @HEADER
// ************************************************************************
//
//               Rapid Optimization Library (ROL) Package
//                 Copyright (2014) Sandia Corporation
//
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact lead developers:
//              Drew Kouri   (dpkouri@sandia.gov) and
//              Denis Ridzal (dridzal@sandia.gov)
//
// ************************************************************************
// @HEADER

/*! \file  test_02.cpp
    \brief Unit test for the mesh manager and the degree-of-freedom manager.
           Mesh type: BACKWARD-FACING STEP with QUAD CELLS and HGRAD SPACE.
*/

#include "ROL_Algorithm.hpp"
#include "ROL_BoundConstraint_SimOpt.hpp"
#include "ROL_Vector_SimOpt.hpp"

#include "Teuchos_oblackholestream.hpp"
#include "Teuchos_GlobalMPISession.hpp"
#include "Teuchos_XMLParameterListHelpers.hpp"

#include "Intrepid_HGRAD_QUAD_C1_FEM.hpp"
#include "Intrepid_HGRAD_QUAD_C2_FEM.hpp"

#include <iostream>
#include <algorithm>

#include "../TOOLS/meshmanager.hpp"
#include "../TOOLS/dofmanager.hpp"

typedef double RealT;

int main(int argc, char *argv[]) {

  Teuchos::GlobalMPISession mpiSession(&argc, &argv);
  // This little trick lets us print to std::cout only if a (dummy) command-line argument is provided.
  int iprint     = argc - 1;
  Teuchos::RCP<std::ostream> outStream;
  Teuchos::oblackholestream bhs; // outputs nothing
  if (iprint > 0)
    outStream = Teuchos::rcp(&std::cout, false);
  else
    outStream = Teuchos::rcp(&bhs, false);

  int errorFlag  = 0;

  // *** Example body.
  try {

    /*** Read in XML input ***/
    std::string filename = "input_02.xml";
    Teuchos::RCP<Teuchos::ParameterList> parlist
      = Teuchos::rcp( new Teuchos::ParameterList() );
    Teuchos::updateParametersFromXmlFile( filename, parlist.ptr() );

    /*** Initialize mesh / degree-of-freedom manager. ***/
    MeshManager_BackwardFacingStepChannel<RealT> meshmgr(*parlist);
    Teuchos::RCP<Intrepid::FieldContainer<RealT> > nodesPtr = meshmgr.getNodes();
    Teuchos::RCP<Intrepid::FieldContainer<int> >   cellToNodeMapPtr = meshmgr.getCellToNodeMap();
    Teuchos::RCP<Intrepid::FieldContainer<int> >   cellToEdgeMapPtr = meshmgr.getCellToEdgeMap();
    Teuchos::RCP<std::vector<std::vector<std::vector<int> > > > sideSetsPtr = meshmgr.getSideSets(); 

    Intrepid::FieldContainer<RealT> &nodes = *nodesPtr;
    Intrepid::FieldContainer<int>   &cellToNodeMap = *cellToNodeMapPtr;
    Intrepid::FieldContainer<int>   &cellToEdgeMap = *cellToEdgeMapPtr;
    std::vector<std::vector<std::vector<int> > >  &sideSets = *sideSetsPtr;
    *outStream << "Number of nodes = " << meshmgr.getNumNodes() << std::endl << nodes;
    *outStream << "Number of cells = " << meshmgr.getNumCells() << std::endl << cellToNodeMap;
    *outStream << "Number of edges = " << meshmgr.getNumEdges() << std::endl << cellToEdgeMap;
    // Print mesh info to file.
    std::ofstream meshfile;
    meshfile.open("cells.txt");
    for (int i=0; i<cellToNodeMap.dimension(0); ++i) {
      meshfile << cellToNodeMap(i,0) << "  "
               << cellToNodeMap(i,1) << "  "
               << cellToNodeMap(i,2) << "  "
               << cellToNodeMap(i,3) << std::endl;
    }
    meshfile.close();
    meshfile.open("edges.txt");
    for (int i=0; i<cellToEdgeMap.dimension(0); ++i) {
      meshfile << cellToEdgeMap(i,0) << "  "
               << cellToEdgeMap(i,1) << "  "
               << cellToEdgeMap(i,2) << "  "
               << cellToEdgeMap(i,3) << std::endl;
    }
    meshfile.close();
    meshfile.open("nodes.txt");
    for (int i=0; i<nodes.dimension(0); ++i) {
      meshfile << nodes(i,0) << "  "
               << nodes(i,1) << std::endl;
    }
    meshfile.close();
    meshfile.open("sideset.txt");
    for (int i=0; i<static_cast<int>(sideSets.size()); ++i) {
      for (int j=0; j<static_cast<int>(sideSets[i].size()); ++j) {
        if (sideSets[i][j].size() > 0) {
          for (int k=0; k<static_cast<int>(sideSets[i][j].size()); ++k) {
            meshfile << sideSets[i][j][k] << std::endl;
          }
        }
        meshfile << std::endl << std::endl;
      }
    }
    meshfile.close();

    Teuchos::RCP<Intrepid::Basis_HGRAD_QUAD_C1_FEM<RealT, Intrepid::FieldContainer<RealT> > > basisPtrQ1 =
      Teuchos::rcp(new Intrepid::Basis_HGRAD_QUAD_C1_FEM<RealT, Intrepid::FieldContainer<RealT> >);

    Teuchos::RCP<Intrepid::Basis_HGRAD_QUAD_C2_FEM<RealT, Intrepid::FieldContainer<RealT> > > basisPtrQ2 =
      Teuchos::rcp(new Intrepid::Basis_HGRAD_QUAD_C2_FEM<RealT, Intrepid::FieldContainer<RealT> >);

    std::vector<Teuchos::RCP<Intrepid::Basis<RealT, Intrepid::FieldContainer<RealT> > > > basisPtrs(3, Teuchos::null);
    basisPtrs[0] = basisPtrQ2;
    basisPtrs[1] = basisPtrQ1;
    basisPtrs[2] = basisPtrQ2;

    Teuchos::RCP<MeshManager<RealT> > meshmgrPtr = Teuchos::rcpFromRef(meshmgr);

    DofManager<RealT> dofmgr(meshmgrPtr, basisPtrs);

    *outStream << "Number of node dofs = " << dofmgr.getNumNodeDofs() << std::endl << *(dofmgr.getNodeDofs());
    *outStream << "Number of edge dofs = " << dofmgr.getNumEdgeDofs() << std::endl << *(dofmgr.getEdgeDofs());
    *outStream << "Number of face dofs = " << dofmgr.getNumFaceDofs() << std::endl << *(dofmgr.getFaceDofs());
    *outStream << "Number of void dofs = " << dofmgr.getNumVoidDofs() << std::endl << *(dofmgr.getVoidDofs());
    *outStream << "Total number of dofs = " << dofmgr.getNumDofs() << std::endl << *(dofmgr.getCellDofs());

    std::vector<std::vector<int> > fieldPattern = dofmgr.getFieldPattern();
    for (int i=0; i<dofmgr.getNumFields(); ++i) {
      *outStream << "\nField " << i << " pattern:   ";
      for (int j=0; j<dofmgr.getLocalFieldSize(i); ++j) {
        *outStream << fieldPattern[i][j] << " ";
      }
      *outStream << std::endl;
    }

    for (int i=0; i<dofmgr.getNumFields(); ++i) {
      *outStream << "\nField  " << i << std::endl;
      *outStream << *(dofmgr.getFieldDofs(i));
    }

    bool correct = true;
    static const int checkDofs[] = {149, 152, 176, 173, 350, 366, 380, 364, 468};
    for (int i=0; i<dofmgr.getLocalFieldSize(2); ++i) {
      correct = correct && ( (*(dofmgr.getFieldDofs(2)))(43,i) == checkDofs[i] );
    }
    if (!correct) {
      errorFlag = -1;
    }

  }
  catch (std::logic_error err) {
    *outStream << err.what() << "\n";
    errorFlag = -1000;
  }; // end try

  if (errorFlag != 0)
    std::cout << "End Result: TEST FAILED\n";
  else
    std::cout << "End Result: TEST PASSED\n";

  return 0;
}
