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
#ifndef SOFA_COMPONENT_COLLISION_MOUSEINTERACTOR_H
#define SOFA_COMPONENT_COLLISION_MOUSEINTERACTOR_H
#include "config.h"

#include <SofaUserInteraction/InteractionPerformer.h>
#include <SofaUserInteraction/RayModel.h>
#include <sofa/core/collision/DetectionOutput.h>
#include <sofa/simulation/Node.h>


namespace sofa
{

namespace component
{

namespace collision
{

/** @brief Represents a point on an object.
 *
 * This requires not only the 3D location, but also the MechanicalState it is attached to.
 */
struct BodyPicked
{
    BodyPicked():body(nullptr), mstate(nullptr), dist(0) {}
    sofa::core::CollisionModel *body;
    sofa::core::behavior::BaseMechanicalState *mstate;
    unsigned int indexCollisionElement;
    defaulttype::Vector3 point;
    SReal dist;
    SReal rayLength;
    operator bool() { return mstate != nullptr; }
};

class SOFA_USER_INTERACTION_API BaseMouseInteractor : public core::BehaviorModel
{
public:
    SOFA_ABSTRACT_CLASS(BaseMouseInteractor, core::BehaviorModel);
    typedef sofa::component::collision::RayModel MouseCollisionModel;
    typedef helper::vector< InteractionPerformer* > VecPerformer;
protected:
    BaseMouseInteractor(): isAttached(false),distanceFromMouse(0) {}
public:
    void draw(const core::visual::VisualParams* vparams) override;

    void cleanup() override;


    //Interactions handling
    void addInteractionPerformer(InteractionPerformer *i);
    bool removeInteractionPerformer( InteractionPerformer *i);
    //Called at each time step: launch all the performers
    void updatePosition( SReal dt) override;
    //Propagate an event in case to all the performers
    void handleEvent(core::objectmodel::Event *e) override;


    virtual core::behavior::BaseMechanicalState *getMouseContainer()=0;

    bool isMouseAttached() const { return isAttached;}
    void setMouseAttached(bool b) {isAttached=b;}

    MouseCollisionModel *getMouseRayModel() {return mouseCollision;}
    void setMouseRayModel( MouseCollisionModel* model) {mouseCollision=model;}

    BodyPicked getBodyPicked() const {return lastPicked;}
    void setBodyPicked( BodyPicked picked) {lastPicked=picked;}

    SReal getDistanceFromMouse() const {return distanceFromMouse;}
    void setDistanceFromMouse(SReal d) {distanceFromMouse=d;}

protected:
    MouseCollisionModel  *mouseCollision;
    BodyPicked lastPicked;
    bool isAttached;
    SReal distanceFromMouse;

    VecPerformer performers;
};



/**
 *  \brief class to execute specific tasks of the Mouse
 *
 */
template <class DataTypes>
class MouseInteractor : public BaseMouseInteractor
{
public:
    SOFA_CLASS(SOFA_TEMPLATE(MouseInteractor,DataTypes),BaseMouseInteractor);

    typedef sofa::component::container::MechanicalObject< DataTypes > MouseContainer;
    typedef typename DataTypes::Coord Coord;
public:
    MouseInteractor():mouseInSofa(nullptr) {}
    ~MouseInteractor() override {}

    void init() override;

    core::behavior::BaseMechanicalState *getMouseContainer() override {return mouseInSofa;}


    virtual std::string getTemplateName() const override
    {
        return templateName(this);
    }
    static std::string templateName(const MouseInteractor<DataTypes>* = nullptr)
    {
        return DataTypes::Name();
    }
protected:
    MouseContainer       *mouseInSofa;
};

#if  !defined(SOFA_COMPONENT_COLLISION_MOUSEINTERACTOR_CPP)
extern template class SOFA_USER_INTERACTION_API MouseInteractor<defaulttype::Vec2Types>;
extern template class SOFA_USER_INTERACTION_API MouseInteractor<defaulttype::Vec3Types>;
extern template class SOFA_USER_INTERACTION_API MouseInteractor<defaulttype::Rigid3Types>;

#endif



} // namespace collision

} // namespace component

} // namespace sofa

#endif
