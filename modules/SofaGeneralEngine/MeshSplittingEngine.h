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
#ifndef SOFA_COMPONENT_ENGINE_MeshSplittingEngine_H
#define SOFA_COMPONENT_ENGINE_MeshSplittingEngine_H
#include "config.h"

#include <sofa/core/DataEngine.h>
#include <sofa/core/objectmodel/BaseObject.h>
#include <sofa/defaulttype/Vec.h>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/core/topology/BaseMeshTopology.h>
#include <sofa/helper/vectorData.h>

namespace sofa
{

namespace component
{

namespace engine
{

/**
 * This class breaks a mesh in multiple parts, based on selected vertices or cells.
 * It provide a map to be used in subsetMultiMapping, and vertex positions for each parts.
 */
template <class DataTypes>
class MeshSplittingEngine : public core::DataEngine
{
public:
    typedef core::DataEngine Inherited;
    SOFA_CLASS(SOFA_TEMPLATE(MeshSplittingEngine,DataTypes),Inherited);

    typedef typename DataTypes::Real Real;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef VecCoord SeqPositions;
    typedef typename core::topology::BaseMeshTopology::Edge Edge;
    typedef typename core::topology::BaseMeshTopology::SeqEdges SeqEdges;
    typedef typename core::topology::BaseMeshTopology::Triangle Triangle;
    typedef typename core::topology::BaseMeshTopology::SeqTriangles SeqTriangles;
    typedef typename core::topology::BaseMeshTopology::Quad Quad;
    typedef typename core::topology::BaseMeshTopology::SeqQuads SeqQuads;
    typedef typename core::topology::BaseMeshTopology::Tetrahedron Tetrahedron;
    typedef typename core::topology::BaseMeshTopology::SeqTetrahedra SeqTetrahedra;
    typedef typename core::topology::BaseMeshTopology::Hexahedron Hexahedron;
    typedef typename core::topology::BaseMeshTopology::SeqHexahedra SeqHexahedra;
    typedef typename core::topology::BaseMeshTopology::PointID PointID;
    typedef typename core::topology::BaseMeshTopology::SetIndices SetIndices;

    /// inputs
    Data< SeqPositions > inputPosition;
    Data< SeqEdges > inputEdges; ///< input edges
    Data< SeqTriangles > inputTriangles; ///< input triangles
    Data< SeqQuads > inputQuads; ///< input quads
    Data< SeqTetrahedra > inputTets; ///< input tetrahedra
    Data< SeqHexahedra > inputHexa; ///< input hexahedra
    Data<unsigned int> nbInputs; ///< Number of input vectors
    helper::vectorData<SetIndices> indices;
    helper::vectorData<SetIndices> edgeIndices;
    helper::vectorData<SetIndices> triangleIndices;
    helper::vectorData<SetIndices> quadIndices;
    helper::vectorData<SetIndices> tetrahedronIndices;
    helper::vectorData<SetIndices> hexahedronIndices;

    /// outputs
    Data< helper::vector<unsigned int> > indexPairs;
    helper::vectorData<SeqPositions> position;

    virtual std::string getTemplateName() const    override { return templateName(this);    }
    static std::string templateName(const MeshSplittingEngine<DataTypes>* = nullptr) { return DataTypes::Name();    }

protected:

    MeshSplittingEngine();
    ~MeshSplittingEngine() override;

public:
    void init() override
    {
        addInput(&inputPosition);
        addInput(&inputEdges);
        addInput(&inputTriangles);
        addInput(&inputQuads);
        addInput(&inputTets);
        addInput(&inputHexa);
        addInput(&nbInputs);
        addOutput(&indexPairs);
        resizeData();

        setDirtyValue();
    }

    void reinit()    override { resizeData(); update();  }

    /// Parse the given description to assign values to this object's fields and potentially other parameters
    void parse ( sofa::core::objectmodel::BaseObjectDescription* arg ) override
    {
        const char* p = arg->getAttribute(nbInputs.getName().c_str());
        if (p) {
            std::string nbStr = p;
            nbInputs.read(nbStr);
            resizeData();
        }
        Inherit1::parse(arg);
    }

    /// Assign the field values stored in the given map of name -> value pairs
    void parseFields ( const std::map<std::string,std::string*>& str ) override
    {
        std::map<std::string,std::string*>::const_iterator it = str.find(nbInputs.getName());
        if (it != str.end() && it->second)
        {
            std::string nbStr = *it->second;
            nbInputs.read(nbStr);
            resizeData();
        }
    }


    void doUpdate() override;

protected:
    void resizeData()
    {
        indices.resize(nbInputs.getValue());
        edgeIndices.resize(nbInputs.getValue());
        triangleIndices.resize(nbInputs.getValue());
        quadIndices.resize(nbInputs.getValue());
        tetrahedronIndices.resize(nbInputs.getValue());
        hexahedronIndices.resize(nbInputs.getValue());
        position.resize(nbInputs.getValue()+1); // one more to store the remaining sub mesh
    }

};

#if  !defined(SOFA_COMPONENT_ENGINE_MeshSplittingEngine_CPP)
extern template class SOFA_GENERAL_ENGINE_API MeshSplittingEngine<defaulttype::Vec3Types>;
 
#endif

} // namespace engine

} // namespace component

} // namespace sofa

#endif
