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
#ifndef SOFA_COMPONENT_MISC_EVALPOINTSDISTANCE_H
#define SOFA_COMPONENT_MISC_EVALPOINTSDISTANCE_H
#include "config.h"

#include <sofa/core/behavior/MechanicalState.h>

#include <sofa/core/objectmodel/BaseObject.h>
#include <sofa/core/objectmodel/Event.h>
#include <sofa/core/objectmodel/DataFileName.h>

#include <sofa/helper/vector.h>

#include <fstream>

namespace sofa
{

namespace component
{

namespace misc
{

/** Compute the distance between point/node positions in two objects
*/
template<class TDataTypes>
class EvalPointsDistance: public virtual sofa::core::objectmodel::BaseObject
{

public:
    SOFA_CLASS(SOFA_TEMPLATE(EvalPointsDistance,TDataTypes), sofa::core::objectmodel::BaseObject);

    typedef TDataTypes DataTypes;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename DataTypes::Real Real;
    typedef typename defaulttype::Vector3 Vec3;

    /// Rendering of lines between associated points (activation)
    Data < bool > f_draw;
    /// Suppress field for save as function
    Data < bool > isToPrint;
    /// Output file name    
    sofa::core::objectmodel::DataFileName f_filename;
    /// Period between outputs
    Data < double > f_period;
    /// Computed distances
    Data < helper::vector<Real> > dist;

    Data < double > distMean; ///< mean distance (OUTPUT)
    Data < double > distMin; ///< min distance (OUTPUT)
    Data < double > distMax; ///< max distance (OUTPUT)
    Data < double > distDev; ///< distance standard deviation (OUTPUT)

    Data < double > rdistMean; ///< mean relative distance (OUTPUT)
    Data < double > rdistMin; ///< min relative distance (OUTPUT)
    Data < double > rdistMax; ///< max relative distance (OUTPUT)
    Data < double > rdistDev; ///< relative distance standard deviation (OUTPUT)

    typedef SingleLink<EvalPointsDistance<DataTypes>,core::behavior::MechanicalState<DataTypes>,BaseLink::FLAG_STOREPATH|BaseLink::FLAG_STRONGLINK> LinkMState;
    /// First model mechanical state
    LinkMState mstate1;
    /// Second model mechanical state
    LinkMState mstate2;

protected:
    /** Default constructor
    */
    EvalPointsDistance();
    ~EvalPointsDistance() override;
public:
    /// Init the computation of the distances
    void init() override;
    /// Update
    void reinit() override;
    /// Reset the computation of the distances
    void reset() override { reinit(); }

    /** Distance computation */

    /// Get the nodes/points coordinates of the two objects and compute the distances
    virtual SReal eval();
    /// Compute the distances between the two objects
    virtual SReal doEval(const VecCoord& x1, const VecCoord& x2, const VecCoord& x0);


    void handleEvent(sofa::core::objectmodel::Event* event) override;
    void draw(const core::visual::VisualParams* vparams) override;
    virtual void doDraw(const core::visual::VisualParams* vparams, const VecCoord& x1, const VecCoord& x2);

    /// Retrieve the associated MechanicalState (First model)
    core::behavior::MechanicalState<DataTypes>* getMState1() { return mstate1.get(); }
    core::behavior::BaseMechanicalState* getMechModel1() { return mstate1.get(); }

    /// Retrieve the associated MechanicalState (Second model)
    core::behavior::MechanicalState<DataTypes>* getMState2() { return mstate2.get(); }
    core::behavior::BaseMechanicalState* getMechModel2() { return mstate2; }


    /// Pre-construction check method called by ObjectFactory.
    /// Check that DataTypes matches the MechanicalState.
    template<class T>
    static bool canCreate(T*& obj, core::objectmodel::BaseContext* context, core::objectmodel::BaseObjectDescription* arg)
    {
        std::string object1 = arg->getAttribute("object1","@./");
        std::string object2 = arg->getAttribute("object2","@./");
        if (!LinkMState::CheckPath(object1, context))
            return false;
        if (!LinkMState::CheckPath(object2, context))
            return false;

        return core::objectmodel::BaseObject::canCreate(obj, context, arg);
    }

    virtual std::string getTemplateName() const override
    {
        return templateName(this);
    }

    static std::string templateName(const EvalPointsDistance<DataTypes>* = nullptr)
    {
        return DataTypes::Name();
    }

    void setPathToMS1(const std::string &o) { mstate1.setPath(o); }
    void setPathToMS2(const std::string &o) { mstate2.setPath(o); }
    void setPeriod(const double& _dt)      { f_period.setValue(_dt); }

protected:
    /// output file
    std::ofstream* outfile;
    /// time value for the distance computations
    double lastTime;

    sofa::defaulttype::BoundingBox box1;
    sofa::defaulttype::BoundingBox box2;
};

#if  !defined(SOFA_COMPONENT_MISC_EVALPOINTSDISTANCE_CPP)
extern template class SOFA_VALIDATION_API EvalPointsDistance<defaulttype::Vec3Types>;
extern template class SOFA_VALIDATION_API EvalPointsDistance<defaulttype::Rigid3Types>;

#endif

} // namespace misc

} // namespace component

} // namespace sofa

#endif
