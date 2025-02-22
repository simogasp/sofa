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
#ifndef SOFA_COMPONENT_FORCEFIELD_TRIANGULARFEMFORCEFIELDOPTIM_H
#define SOFA_COMPONENT_FORCEFIELD_TRIANGULARFEMFORCEFIELDOPTIM_H
#include "config.h"



#include <sofa/core/behavior/ForceField.h>
#include <sofa/core/topology/BaseMeshTopology.h>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/Mat.h>
#include <SofaBaseTopology/TopologyData.h>

#include <map>
#include <sofa/helper/map.h>

namespace sofa
{

namespace component
{

namespace forcefield
{

template<class DataTypes>
class TriangularFEMForceFieldOptim;

/// This class can be overridden if needed for additionnal storage within template specializations.
template<class DataTypes>
class TriangularFEMForceFieldOptimInternalData
{
public:
    typedef TriangularFEMForceFieldOptim<DataTypes> Main;
    void reinit(Main* /*m*/) {}
};


/** corotational triangle from
* @InProceedings{NPF05,
*   author       = "Nesme, Matthieu and Payan, Yohan and Faure, Fran\c{c}ois",
*   title        = "Efficient, Physically Plausible Finite Elements",
*   booktitle    = "Eurographics (short papers)",
*   month        = "august",
*   year         = "2005",
*   editor       = "J. Dingliana and F. Ganovelli",
*   keywords     = "animation, physical model, elasticity, finite elements",
*   url          = "http://www-evasion.imag.fr/Publications/2005/NPF05"
* }
*/

template<class DataTypes>
class TriangularFEMForceFieldOptim : public core::behavior::ForceField<DataTypes>
{
public:
    SOFA_CLASS(SOFA_TEMPLATE(TriangularFEMForceFieldOptim, DataTypes), SOFA_TEMPLATE(core::behavior::ForceField, DataTypes));

    typedef core::behavior::ForceField<DataTypes> Inherited;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::VecReal VecReal;
    typedef VecCoord Vector;
    typedef typename DataTypes::Coord    Coord   ;
    typedef typename DataTypes::Deriv    Deriv   ;
    typedef typename Coord::value_type   Real    ;

    typedef core::objectmodel::Data<VecCoord> DataVecCoord;
    typedef core::objectmodel::Data<VecDeriv> DataVecDeriv;

    typedef sofa::core::topology::BaseMeshTopology::index_type Index;
    typedef sofa::core::topology::BaseMeshTopology::Triangle Triangle;
    typedef sofa::core::topology::BaseMeshTopology::Triangle Element;
    typedef sofa::core::topology::BaseMeshTopology::SeqTriangles VecElement;
    typedef sofa::core::topology::BaseMeshTopology::TrianglesAroundVertex TrianglesAroundVertex;

    typedef sofa::helper::Quater<Real> Quat;

protected:
    typedef defaulttype::Mat<2, 3, Real > Transformation;				    ///< matrix for rigid transformations like rotations
    enum { DerivSize = DataTypes::deriv_total_size };
    typedef defaulttype::Mat<DerivSize, DerivSize, Real> MatBloc;

    typedef TriangularFEMForceFieldOptimInternalData<DataTypes> InternalData;
    InternalData data;

protected:
    /// ForceField API
    TriangularFEMForceFieldOptim();

    virtual ~TriangularFEMForceFieldOptim();
public:
    void init() override;
    void reinit() override;
    void addForce(const core::MechanicalParams* mparams, DataVecDeriv& f, const DataVecCoord& x, const DataVecDeriv& v) override;
    void addDForce(const core::MechanicalParams* mparams, DataVecDeriv& df, const DataVecDeriv& dx) override;
    void addKToMatrix(const core::MechanicalParams* mparams, const sofa::core::behavior::MultiMatrixAccessor* matrix) override;
    SReal getPotentialEnergy(const core::MechanicalParams* mparams, const DataVecCoord& x) const override;
    void getTrianglePrincipalStress(unsigned int i, Real& stressValue, Deriv& stressDirection);

    void draw(const core::visual::VisualParams* vparams) override;

    // parse method attribute (for compatibility with non-optimized version)
    void parse ( sofa::core::objectmodel::BaseObjectDescription* arg ) override;

    /// Class to store FEM information on each triangle, for topology modification handling
    class TriangleInfo
    {
    public:
        //Index ia, ib, ic;
        Real bx, cx, cy, ss_factor;
        Transformation init_frame; // Mat<2,3,Real>

        TriangleInfo() { }

        /// Output stream
        inline friend std::ostream& operator<< ( std::ostream& os, const TriangleInfo& ti )
        {
            return os << "bx= " << ti.bx << " cx= " << ti.cx << " cy= " << ti.cy << " ss_factor= " << ti.ss_factor << " init_frame= " << ti.init_frame << " END";
        }

        /// Input stream
        inline friend std::istream& operator>> ( std::istream& in, TriangleInfo& ti )
        {
            std::string str;
            while (in >> str)
            {
                if (str == "END") break;
                else if (str == "bx=") in >> ti.bx;
                else if (str == "cx=") in >> ti.cx;
                else if (str == "cy=") in >> ti.cy;
                else if (str == "ss_factor=") in >> ti.ss_factor;
                else if (str == "init_frame=") in >> ti.init_frame;
                else if (!str.empty() && str[str.length()-1]=='=') in >> str; // unknown value
            }
            return in;
        }
    };

    Real gamma, mu;
    class TriangleState
    {
    public:
        Transformation frame; // Mat<2,3,Real>
        Deriv stress;

        TriangleState() { }

        /// Output stream
        inline friend std::ostream& operator<< ( std::ostream& os, const TriangleState& ti )
        {
            return os << "frame= " << ti.frame << " stress= " << ti.stress << " END";
        }

        /// Input stream
        inline friend std::istream& operator>> ( std::istream& in, TriangleState& ti )
        {
            std::string str;
            while (in >> str)
            {
                if (str == "END") break;
                else if (str == "frame=") in >> ti.frame;
                else if (str == "stress=") in >> ti.stress;
                else if (!str.empty() && str[str.length()-1]=='=') in >> str; // unknown value
            }
            return in;
        }
    };
    /// Class to store FEM information on each edge, for topology modification handling
    class EdgeInfo
    {
    public:
        bool fracturable;

        EdgeInfo()
            : fracturable(false) { }

        /// Output stream
        inline friend std::ostream& operator<< ( std::ostream& os, const EdgeInfo& /*ei*/ )
        {
            return os;
        }

        /// Input stream
        inline friend std::istream& operator>> ( std::istream& in, EdgeInfo& /*ei*/ )
        {
            return in;
        }
    };

    /// Class to store FEM information on each vertex, for topology modification handling
    class VertexInfo
    {
    public:
        VertexInfo()
        /*:sumEigenValues(0.0)*/ {}

        /// Output stream
        inline friend std::ostream& operator<< ( std::ostream& os, const VertexInfo& /*vi*/)
        {
            return os;
        }
        /// Input stream
        inline friend std::istream& operator>> ( std::istream& in, VertexInfo& /*vi*/)
        {
            return in;
        }
    };

    /// Topology Data
    typedef typename VecCoord::template rebind<TriangleInfo>::other VecTriangleInfo;
    typedef typename VecCoord::template rebind<TriangleState>::other VecTriangleState;
    typedef typename VecCoord::template rebind<VertexInfo>::other VecVertexInfo;
    typedef typename VecCoord::template rebind<EdgeInfo>::other VecEdgeInfo;
    topology::TriangleData<VecTriangleInfo> d_triangleInfo; ///< Internal triangle data (persistent)
    topology::TriangleData<VecTriangleState> d_triangleState; ///< Internal triangle data (time-dependent)
    topology::PointData<VecVertexInfo> d_vertexInfo; ///< Internal point data
    topology::EdgeData<VecEdgeInfo> d_edgeInfo; ///< Internal edge data


    class TFEMFFOTriangleInfoHandler : public topology::TopologyDataHandler<Triangle,VecTriangleInfo >
    {
    public:
        TFEMFFOTriangleInfoHandler(TriangularFEMForceFieldOptim<DataTypes>* _ff, topology::TriangleData<VecTriangleInfo >* _data) : topology::TopologyDataHandler<Triangle, VecTriangleInfo >(_data), ff(_ff) {}

        void applyCreateFunction(unsigned int triangleIndex, TriangleInfo& ,
                const Triangle & t,
                const sofa::helper::vector< unsigned int > &,
                const sofa::helper::vector< double > &);

    protected:
        TriangularFEMForceFieldOptim<DataTypes>* ff;
    };
    void initTriangleInfo(unsigned int triangleIndex, TriangleInfo& ti, const Triangle t, const VecCoord& x0);
    void initTriangleState(unsigned int triangleIndex, TriangleState& ti, const Triangle t, const VecCoord& x);

    void computeTriangleRotation(Transformation& result, Coord eab, Coord eac);
    void computeTriangleRotation(Transformation& result, Coord a, Coord b, Coord c)
    {
        computeTriangleRotation(result,b-a,c-a);
    }
    void computeTriangleRotation(Transformation& result, VecCoord& x0, Triangle t)
    {
        computeTriangleRotation(result,x0[t[0]], x0[t[1]], x0[t[2]]);
    }

    class TFEMFFOTriangleStateHandler : public topology::TopologyDataHandler<Triangle,VecTriangleState >
    {
    public:
        TFEMFFOTriangleStateHandler(TriangularFEMForceFieldOptim<DataTypes>* _ff, topology::TriangleData<VecTriangleState >* _data) : topology::TopologyDataHandler<Triangle, VecTriangleState >(_data), ff(_ff) {}

        void applyCreateFunction(unsigned int triangleIndex, TriangleState& ,
                const Triangle & t,
                const sofa::helper::vector< unsigned int > &,
                const sofa::helper::vector< double > &);

    protected:
        TriangularFEMForceFieldOptim<DataTypes>* ff;
    };

    sofa::core::topology::BaseMeshTopology* _topology;
    template<class MatrixWriter>
    void addKToMatrixT(const core::MechanicalParams* mparams, MatrixWriter m);

    void getTriangleVonMisesStress(unsigned int i, Real& stressValue);
    void getTrianglePrincipalStress(unsigned int i, Real& stressValue, Deriv& stressDirection, Real& stressValue2, Deriv& stressDirection2);

public:

    /// Forcefield intern paramaters
    Data<Real> d_poisson;
    Data<Real> d_young; ///< Young modulus in Hooke's law
    Data<Real> d_damping; ///< Ratio damping/stiffness
    Data<Real> d_restScale; ///< Scale factor applied to rest positions (to simulate pre-stretched materials)

    /// Display parameters
    Data<bool> d_showStressValue;
    Data<bool> d_showStressVector; ///< Flag activating rendering of stress directions within each triangle
    Data<Real> d_showStressMaxValue; ///< Max value for rendering of stress values


    TFEMFFOTriangleInfoHandler* triangleInfoHandler;
    TFEMFFOTriangleStateHandler* triangleStateHandler;

protected:
    Real drawPrevMaxStress;

};


#if  !defined(SOFA_COMPONENT_FORCEFIELD_TRIANGULARFEMFORCEFIELDOPTIM_CPP)

extern template class SOFA_GENERAL_SIMPLE_FEM_API TriangularFEMForceFieldOptim<defaulttype::Vec3Types>;


#endif //  !defined(SOFA_COMPONENT_FORCEFIELD_TRIANGULARFEMFORCEFIELDOPTIM_CPP)

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif // SOFA_COMPONENT_FORCEFIELD_TRIANGULARFEMFORCEFIELDOPTIM_H
