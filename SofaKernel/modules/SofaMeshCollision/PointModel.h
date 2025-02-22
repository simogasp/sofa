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
#ifndef SOFA_COMPONENT_COLLISION_POINTMODEL_H
#define SOFA_COMPONENT_COLLISION_POINTMODEL_H
#include "config.h"

#include <sofa/core/CollisionModel.h>
#include <SofaMeshCollision/LocalMinDistanceFilter.h>
#include <SofaBaseMechanics/MechanicalObject.h>
#include <sofa/core/topology/BaseMeshTopology.h>
#include <sofa/defaulttype/VecTypes.h>
#include <vector>

namespace sofa
{

namespace component
{

namespace collision
{

template<class DataTypes>
class PointCollisionModel;

class PointLocalMinDistanceFilter;

template<class TDataTypes>
class TPoint : public core::TCollisionElementIterator<PointCollisionModel<TDataTypes> >
{
public:
    typedef TDataTypes DataTypes;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef PointCollisionModel<DataTypes> ParentModel;

    TPoint(ParentModel* model, int index);
    TPoint() {}

    explicit TPoint(const core::CollisionElementIterator& i);

    const Coord& p() const;
    const Coord& pFree() const;
    const Deriv& v() const;
    Deriv n() const;

    /// Return true if the element stores a free position vector
    bool hasFreePosition() const;

    bool testLMD(const sofa::defaulttype::Vector3 &, double &, double &);

    bool activated(core::CollisionModel *cm = nullptr) const;
};

class PointActiver
{
public:
    PointActiver() {}
    virtual ~PointActiver() {}
    virtual bool activePoint(int /*index*/, core::CollisionModel * /*cm*/ = nullptr) {return true;}
	static PointActiver* getDefaultActiver() { static PointActiver defaultActiver; return &defaultActiver; }
};

template<class TDataTypes>
class SOFA_MESH_COLLISION_API PointCollisionModel : public core::CollisionModel
{
public:
    SOFA_CLASS(SOFA_TEMPLATE(PointCollisionModel, TDataTypes), core::CollisionModel);

    typedef TDataTypes DataTypes;
    typedef DataTypes InDataTypes;
    typedef PointCollisionModel<DataTypes> ParentModel;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef TPoint<DataTypes> Element;
    typedef helper::vector<unsigned int> VecIndex;

    friend class TPoint<DataTypes>;
protected:
    PointCollisionModel();
public:
    void init() override;

    // -- CollisionModel interface

    void resize(int size) override;

    void computeBoundingTree(int maxDepth=0) override;

    void computeContinuousBoundingTree(double dt, int maxDepth=0) override;

    void draw(const core::visual::VisualParams*,int index) override;
    void draw(const core::visual::VisualParams* vparams) override;

    bool canCollideWithElement(int index, CollisionModel* model2, int index2) override;

    core::behavior::MechanicalState<DataTypes>* getMechanicalState() { return mstate; }

    Deriv getNormal(int index){ return (normals.size()) ? normals[index] : Deriv();}

    PointLocalMinDistanceFilter *getFilter() const;

    void setFilter(PointLocalMinDistanceFilter * /*lmdFilter*/);

    const Deriv& velocity(int index) const;

    Data<bool> bothSide; ///< to activate collision on both side of the point model (when surface normals are defined on these points)

    /// Pre-construction check method called by ObjectFactory.
    /// Check that DataTypes matches the MechanicalState.
    template<class T>
    static bool canCreate(T*& obj, core::objectmodel::BaseContext* context, core::objectmodel::BaseObjectDescription* arg)
    {
        if (dynamic_cast<core::behavior::MechanicalState<DataTypes>*>(context->getMechanicalState()) == nullptr)
            return false;
        return BaseObject::canCreate(obj, context, arg);
    }

    virtual std::string getTemplateName() const override
    {
        return templateName(this);
    }

    static std::string templateName(const PointCollisionModel<DataTypes>* = nullptr)
    {
        return DataTypes::Name();
    }


    void computeBBox(const core::ExecParams* params, bool onlyVisible) override;
    void updateNormals();

protected:

    core::behavior::MechanicalState<DataTypes>* mstate;

    Data<bool> computeNormals; ///< activate computation of normal vectors (required for some collision detection algorithms)

    Data<std::string> PointActiverPath; ///< path of a component PointActiver that activate or deactivate collision point during execution

    VecDeriv normals;

    PointLocalMinDistanceFilter *m_lmdFilter;
    EmptyFilter m_emptyFilter;

    Data<bool> m_displayFreePosition; ///< Display Collision Model Points free position(in green)    

    PointActiver *myActiver;
};

template <class TDataTypes> using TPointModel [[deprecated("The TPointModel is now deprecated please use PointCollisionModel instead.")]] = PointCollisionModel<TDataTypes>;

template<class DataTypes>
inline TPoint<DataTypes>::TPoint(ParentModel* model, int index)
    : core::TCollisionElementIterator<ParentModel>(model, index)
{

}

template<class DataTypes>
inline TPoint<DataTypes>::TPoint(const core::CollisionElementIterator& i)
    : core::TCollisionElementIterator<ParentModel>(static_cast<ParentModel*>(i.getCollisionModel()), i.getIndex())
{

}

template<class DataTypes>
inline const typename DataTypes::Coord& TPoint<DataTypes>::p() const { return this->model->mstate->read(core::ConstVecCoordId::position())->getValue()[this->index]; }

template<class DataTypes>
inline const typename DataTypes::Coord& TPoint<DataTypes>::pFree() const
{
    if (hasFreePosition())
        return this->model->mstate->read(core::ConstVecCoordId::freePosition())->getValue()[this->index];
    else
        return p();
}

template<class DataTypes>
inline const typename DataTypes::Deriv& TPoint<DataTypes>::v() const { return this->model->mstate->read(core::ConstVecDerivId::velocity())->getValue()[this->index]; }

template<class DataTypes>
inline const typename DataTypes::Deriv& PointCollisionModel<DataTypes>::velocity(int index) const { return mstate->read(core::ConstVecDerivId::velocity())->getValue()[index]; }

template<class DataTypes>
inline typename DataTypes::Deriv TPoint<DataTypes>::n() const { return ((unsigned)this->index<this->model->normals.size()) ? this->model->normals[this->index] : Deriv(); }

template<class DataTypes>
inline bool TPoint<DataTypes>::hasFreePosition() const { return this->model->mstate->read(core::ConstVecCoordId::freePosition())->isSet(); }

template<class DataTypes>
inline bool TPoint<DataTypes>::activated(core::CollisionModel *cm) const
{
    return this->model->myActiver->activePoint(this->index, cm);
}

typedef PointCollisionModel<sofa::defaulttype::Vec3Types> PointModel;
typedef TPoint<sofa::defaulttype::Vec3Types> Point;

#if  !defined(SOFA_COMPONENT_COLLISION_POINTMODEL_CPP)
extern template class SOFA_MESH_COLLISION_API PointCollisionModel<defaulttype::Vec3Types>;

#endif

//bool Point::testLMD(const Vector3 &PQ, double &coneFactor, double &coneExtension);

} // namespace collision

} // namespace component

} // namespace sofa

#endif
