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
#ifndef SOFA_COMPONENT_ENGINE_GROUPFILTERYOUNGMODULUS_H
#define SOFA_COMPONENT_ENGINE_GROUPFILTERYOUNGMODULUS_H
#include "config.h"



#include <sofa/defaulttype/Vec.h>
#include <sofa/core/DataEngine.h>
#include <sofa/core/objectmodel/BaseObject.h>
#include <sofa/core/loader/PrimitiveGroup.h>

namespace sofa
{

namespace component
{

namespace engine
{

/**
 * This class returns a vector of Young modulus, according a list of groups
 */

template <class DataTypes>
class GroupFilterYoungModulus : public core::DataEngine
{
public:
    SOFA_CLASS( SOFA_TEMPLATE(GroupFilterYoungModulus,DataTypes),core::DataEngine);

    typedef typename DataTypes::Real Real;

protected:

    GroupFilterYoungModulus();
    ~GroupFilterYoungModulus() override {}
public:
    void init() override;
    void reinit() override;
    void doUpdate() override;

    //Input
    Data<helper::vector<sofa::core::loader::PrimitiveGroup > > f_groups; ///< Groups
    Data<helper::vector<unsigned int> > f_primitives; ///< not mandatory
    Data<helper::vector<int > > f_elementsGroup; ///< Vector of groups (each element gives its group
    //Output
    Data<helper::vector<Real> > f_youngModulus; ///< Vector of young modulus for each primitive
    //Parameters
    Data<std::string> p_mapGroupModulus; ///< Mapping between groups and modulus
    Data<Real> p_defaultModulus; ///< Default value if the primitive is not in a group
    Data<helper::vector<Real> > p_groupMod; ///< list of young modulus for each group

    virtual std::string getTemplateName() const override
    {
        return templateName(this);
    }

    static std::string templateName(const GroupFilterYoungModulus<DataTypes>* = nullptr)
    {
        return DataTypes::Name();
    }

};

#if  !defined(SOFA_COMPONENT_ENGINE_GROUPFILTERYOUNGMODULUS_CPP)
extern template class SOFA_GENERAL_ENGINE_API GroupFilterYoungModulus<defaulttype::Vec3Types>;
 
#endif

} // namespace engine

} // namespace component

} // namespace sofa

#endif
