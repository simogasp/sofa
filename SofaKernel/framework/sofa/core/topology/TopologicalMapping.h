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
#ifndef SOFA_CORE_TOPOLOGY_TOPOLOGICALMAPPING_H
#define SOFA_CORE_TOPOLOGY_TOPOLOGICALMAPPING_H

#include <sofa/core/topology/BaseMeshTopology.h>

namespace sofa
{

namespace core
{

namespace topology
{

/**
*  \brief This Interface is a new kind of Mapping, called TopologicalMapping, which converts an INPUT TOPOLOGY to an OUTPUT TOPOLOGY (both topologies are of type BaseTopology)
*
* It first initializes the mesh of the output topology from the mesh of the input topology,
* and it creates the two Index Maps that maintain the correspondence between the indices of their common elements.
*
* Then, at each propagation of topological changes, it translates the topological change events that are propagated from the INPUT topology
* into specific actions that call element adding or element removal methods on the OUTPUT topology, and it updates the Index Maps.
*
* So, at each time step, the geometrical and adjacency information are consistent in both topologies.
*
*/
class TopologicalMapping : public virtual objectmodel::BaseObject
{
public:
    SOFA_ABSTRACT_CLASS(TopologicalMapping, objectmodel::BaseObject);

    /// Input Topology
    typedef BaseMeshTopology In;
    /// Output Topology
    typedef BaseMeshTopology Out;
protected:
    TopologicalMapping()
        : fromModel(initLink("input", "Input topology to map"))
        , toModel(initLink("output", "Output topology to map"))
    {}

    ~TopologicalMapping() override { }
public:
    /// Specify the input and output models.
    virtual void setTopologies(In* from, Out* to)
    {
        this->fromModel.set( from );
        this->toModel.set( to );
    };

    /// Set the path to the objects mapped in the scene graph
    void setPathInputObject(const std::string &o) {fromModel.setPath(o);}
    void setPathOutputObject(const std::string &o) {toModel.setPath(o);}

    /// Accessor to the INPUT topology of the TopologicalMapping :
    In* getFrom() {return fromModel.get();}

    /// Accessor to the OUTPUT topology of the TopologicalMapping :
    Out* getTo() {return toModel.get();}

    /// Method called at each topological changes propagation which comes from the INPUT topology to adapt the OUTPUT topology :
    virtual void updateTopologicalMappingTopDown() = 0;

    /// Method called at each topological changes propagation which comes from the OUTPUT topology to adapt the INPUT topology :
    virtual void updateTopologicalMappingBottomUp() {}

    /// Return true if this mapping is able to propagate topological changes from input to output model
    virtual bool propagateFromInputToOutputModel() { return true; }

    /// Return true if this mapping is able to propagate topological changes from output to input model
    virtual bool propagateFromOutputToInputModel() { return false; }

    /// return true if the output topology subdivide the input one. (the topology uses the Loc2GlobVec/Glob2LocMap/In2OutMap structs and share the same DOFs)
    virtual bool isTheOutputTopologySubdividingTheInputOne() { return true; }

    /// Accessor to index maps :
    const std::map<unsigned int, unsigned int>& getGlob2LocMap() { return Glob2LocMap;}
    //const sofa::helper::vector<unsigned int>& getLoc2GlobVec(){ return Loc2GlobVec.getValue();}

    Data <sofa::helper::vector<unsigned int> >& getLoc2GlobVec() {return Loc2GlobDataVec;}

    virtual unsigned int getGlobIndex(unsigned int ind)
    {
        if(ind< (Loc2GlobDataVec.getValue()).size())
        {
            return (Loc2GlobDataVec.getValue())[ind];
        }
        else
        {
            return 0;
        }
    }

    virtual unsigned int getFromIndex(unsigned int /*ind*/)
    {
        return 0;
    }

    void dumpGlob2LocMap()
    {
        std::map<unsigned int, unsigned int>::iterator itM;
        msg_info() << "## Log Glob2LocMap - size: " << Glob2LocMap.size() << " ##";
        for (itM = Glob2LocMap.begin(); itM != Glob2LocMap.end(); ++itM)
            msg_info() << (*itM).first << " - " << (*itM).second;

        msg_info() << "#################";
    }

    void dumpLoc2GlobVec()
    {
        const sofa::helper::vector <unsigned int>& buffer = Loc2GlobDataVec.getValue();
        msg_info() << "## Log Loc2GlobDataVec - size: " << buffer.size() << " ##";
        for (unsigned int i=0; i<buffer.size(); ++i)
            msg_info() << i << " - " << buffer[i];

        msg_info() << "#################";
    }

    /// Method to check the topology mapping maps regarding the upper topology
    virtual bool checkTopologies() {return false;}

    /** return all the from indices in the 'In' topology corresponding to the index in the 'Out' topology.
    *   This function is used instead of  the previous one when the function isTheOutputTopologySubdividingTheInputOne() returns false.
    */
    virtual void getFromIndex( helper::vector<unsigned int>& /*fromIndices*/, const unsigned int /*toIndex*/) const {}

    const std::map<unsigned int, sofa::helper::vector<unsigned int> >& getIn2OutMap() { return In2OutMap;}

    /// Pre-construction check method called by ObjectFactory.
    ///
    /// This implementation reads the "input" and "output" attributes and checks
    /// that the corresponding objects exist, and are not the same object.
    template<class T>
    static bool canCreate ( T*& obj, core::objectmodel::BaseContext* context, core::objectmodel::BaseObjectDescription* arg )
    {
        In* stin = nullptr;
        Out* stout = nullptr;

        std::string inPath, outPath;

        if (arg->getAttribute("input"))
            inPath = arg->getAttribute("input");
        else
            inPath = "@../";

        context->findLinkDest(stin, inPath, nullptr);

        if (arg->getAttribute("output"))
            outPath = arg->getAttribute("output");
        else
            outPath = "@./";

        context->findLinkDest(stout, outPath, nullptr);

        if (stin == nullptr)
        {
            context->serr << "Creation of " << className(obj) << " topology mapping failed as input topology model is missing or invalid." << context->sendl;
            return false;
        }

        if (stout == nullptr)
        {
            context->serr << "Creation of " << className(obj) <<" topology mapping failed as output topology model is missing or invalid." << context->sendl;
            return false;
        }

        if (static_cast<BaseObject*>(stin) == static_cast<BaseObject*>(stout))
        {
            // we should refuse to create mappings with the same input and output model, which may happen if a State object is missing in the child node
            context->serr <<  "Creation of " << className(obj) << " topology mapping failed as the same object \"" << stin->getName() << "\" is linked as input and output." << context->sendl;
            return false;
        }

        return BaseObject::canCreate(obj, context, arg);
    }

    /// Construction method called by ObjectFactory.
    ///
    /// This implementation read the object1 and object2 attributes to
    /// find the input and output topologies of this mapping.
    template<class T>
    static typename T::SPtr create (T*, core::objectmodel::BaseContext* context, core::objectmodel::BaseObjectDescription* arg )
    {
        typename T::SPtr obj = sofa::core::objectmodel::New<T>();

        if (context)
            context->addObject(obj);

        if (arg)
        {
            std::string inPath, outPath;
            if (arg->getAttribute("input"))
                inPath = arg->getAttribute("input");
            else
                inPath = "@../";

            if (arg->getAttribute("output"))
                outPath = arg->getAttribute("output");
            else
                outPath = "@./";

            obj->fromModel.setPath( inPath );
            obj->toModel.setPath( outPath );

            obj->parse(arg);
        }

        return obj;
    }

protected:
    /// Input source BaseTopology
    SingleLink<TopologicalMapping, In, BaseLink::FLAG_STOREPATH|BaseLink::FLAG_STRONGLINK> fromModel;
    //In* fromModel;
    /// Output target BaseTopology
    SingleLink<TopologicalMapping, Out, BaseLink::FLAG_STOREPATH|BaseLink::FLAG_STRONGLINK> toModel;
    //Out* toModel;

    // Two index maps :

    // Array which gives for each index (local index) of an element in the OUTPUT topology
    // the corresponding index (global index) of the same element in the INPUT topology :
    Data <sofa::helper::vector <unsigned int> > Loc2GlobDataVec;

    // Map which gives for each index (global index) of an element in the INPUT topology
    // the corresponding index (local index) of the same element in the OUTPUT topology :
    std::map<unsigned int, unsigned int> Glob2LocMap;   //TODO put it in Data => Data allow map

    std::map<unsigned int, sofa::helper::vector<unsigned int> > In2OutMap;
};

} // namespace topology

} // namespace core

} // namespace sofa

#endif
