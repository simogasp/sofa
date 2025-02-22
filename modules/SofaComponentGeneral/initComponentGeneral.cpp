/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, development version     *
*                (c) 2006-2019 INRIA, USTL, UJF, CNRS, MGH                    *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#include <sofa/helper/system/config.h>
#include <SofaComponentGeneral/initComponentGeneral.h>

#include <SofaGeneralAnimationLoop/initGeneralAnimationLoop.h>
#include <SofaGeneralDeformable/initGeneralDeformable.h>
#include <SofaGeneralExplicitOdeSolver/initGeneralExplicitODESolver.h>
#include <SofaGeneralImplicitOdeSolver/initGeneralImplicitODESolver.h>
#include <SofaGeneralLinearSolver/initGeneralLinearSolver.h>
#include <SofaGeneralLoader/initGeneralLoader.h>
#include <SofaGeneralMeshCollision/initGeneralMeshCollision.h>
#include <SofaGeneralObjectInteraction/initGeneralObjectInteraction.h>
#include <SofaGeneralRigid/initGeneralRigid.h>
#include <SofaGeneralSimpleFem/initGeneralSimpleFEM.h>
#include <SofaGeneralTopology/initGeneralTopology.h>
#include <SofaGeneralVisual/initGeneralVisual.h>
#include <SofaValidation/initValidation.h>
#include <SofaGeneralEngine/initGeneralEngine.h>
#include <SofaGraphComponent/initGraphComponent.h>
#include <SofaTopologyMapping/initTopologyMapping.h>
#include <SofaBoundaryCondition/initBoundaryCondition.h>
#include <SofaUserInteraction/initUserInteraction.h>
#include <SofaConstraint/initConstraint.h>

#if SOFAGENERAL_HAVE_SOFADENSESOLVER
#include <SofaDenseSolver/initDenseSolver.h>
#endif

#include <SofaEigen2Solver/initEigen2Solver.h>

namespace sofa
{

namespace component
{


void initComponentGeneral()
{
    static bool first = true;
    if (first)
    {
        first = false;
    }


    initGeneralAnimationLoop();
    initGeneralDeformable();
    initGeneralExplicitODESolver();
    initGeneralImplicitODESolver();
    initGeneralLinearSolver();
    initGeneralLoader();
    initGeneralMeshCollision();
    initGeneralObjectInteraction();
    initGeneralRigid();
    initGeneralSimpleFEM();
    initGeneralTopology();
    initGeneralVisual();
    initValidation();
    initGeneralEngine();
    initGraphComponent();
    initTopologyMapping();
    initBoundaryCondition();
    initUserInteraction();
    initConstraint();
#if SOFAGENERAL_HAVE_SOFADENSESOLVER
    initDenseSolver();
#endif
}


} // namespace component

} // namespace sofa
