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
#include <sofa/core/objectmodel/BaseObject.h>
#include <sofa/core/objectmodel/BaseContext.h>
#include <sofa/core/objectmodel/BaseNode.h>
#include <sofa/core/objectmodel/Event.h>
#include <sofa/core/objectmodel/KeypressedEvent.h>
#include <sofa/core/topology/Topology.h>
#include <sofa/helper/TagFactory.h>
#include <iostream>


namespace sofa
{

namespace core
{

namespace objectmodel
{

BaseObject::BaseObject()
    : Base()
    , f_listening(initData( &f_listening, false, "listening", "if true, handle the events, otherwise ignore the events"))
    , l_context(initLink("context","Graph Node containing this object (or BaseContext::getDefault() if no graph is used"))
    , l_slaves(initLink("slaves","Sub-objects used internally by this object"))
    , l_master(initLink("master","nullptr for regular objects, or master object for which this object is one sub-objects"))
{
    l_context.setValidator(&sofa::core::objectmodel::BaseObject::changeContextLink);
    l_context.set(BaseContext::getDefault());
    l_slaves.setValidator(&sofa::core::objectmodel::BaseObject::changeSlavesLink);
    f_listening.setAutoLink(false);
}

BaseObject::~BaseObject()
{
    assert(l_master.get() == nullptr); // an object that is still a slave should not be able to be deleted, as at least one smart pointer points to it
    for(VecSlaves::const_iterator iSlaves = l_slaves.begin(); iSlaves != l_slaves.end(); ++iSlaves)
    {
        (*iSlaves)->l_master.reset();
    }
}

// This method insures that context is never nullptr (using BaseContext::getDefault() instead)
// and that all slaves of an object share its context
void BaseObject::changeContextLink(BaseContext* before, BaseContext*& after)
{
    if (!after) after = BaseContext::getDefault();
    if (before == after) return;
    for (unsigned int i = 0; i < l_slaves.size(); ++i) l_slaves.get(i)->l_context.set(after);
    if (after != BaseContext::getDefault())
    {
        // update links
        updateLinks(false);
    }
}

/// This method insures that slaves objects have master and context links set correctly
void BaseObject::changeSlavesLink(BaseObject::SPtr ptr, unsigned int /*index*/, bool add)
{
    if (!ptr) return;
    if (add) { ptr->l_master.set(this); ptr->l_context.set(getContext()); }
    else     { ptr->l_master.reset(); ptr->l_context.reset(); }
}

void BaseObject::parse( BaseObjectDescription* arg )
{
    if (arg->getAttribute("src"))
    {
        std::string valueString(arg->getAttribute("src"));

        if (valueString[0] != '@')
        {
            msg_error() <<"'src' attribute value should be a link using '@'";
        }
        else
        {
            if(valueString.size() == 1) // ignore '@' alone
            {
                msg_warning() << "'src=@' does nothing.";
            }
            else
            {
                std::vector< std::string > attributeList;
                arg->getAttributeList(attributeList);
                setSrc(valueString, &attributeList);
            }

        }
        arg->removeAttribute("src");
    }
    Base::parse(arg);
}

void BaseObject::setSrc(const std::string &valueString, std::vector< std::string > *attributeList)
{
    BaseObject* loader = nullptr;

    std::size_t posAt = valueString.rfind('@');
    if (posAt == std::string::npos) posAt = 0;
    std::string objectName;

    objectName = valueString.substr(posAt+1);
    loader = getContext()->get<BaseObject>(objectName);
    if (!loader)
    {
        msg_error() << "Source object \"" << valueString << "\" NOT FOUND.";
        return;
    }
    setSrc(valueString, loader, attributeList);
}

void BaseObject::setSrc(const std::string &valueString, const BaseObject *loader, std::vector< std::string > *attributeList)
{
    BaseObject* obj = this;

    std::multimap < std::string, BaseData*> dataLoaderMap(loader->m_aliasData);
    std::multimap < std::string, BaseData*>::iterator it_map;

    if (attributeList != nullptr)
    {
        for (unsigned int j = 0; j<attributeList->size(); ++j)
        {
            it_map = dataLoaderMap.find ((*attributeList)[j]);
            if (it_map != dataLoaderMap.end())
                dataLoaderMap.erase (it_map);
        }
    }

    // -- Temporary patch, using exceptions. TODO: use a flag to set Data not to be automatically linked. --
    //{
    it_map = dataLoaderMap.find ("type");
    if (it_map != dataLoaderMap.end())
        dataLoaderMap.erase (it_map);

    it_map = dataLoaderMap.find ("filename");
    if (it_map != dataLoaderMap.end())
        dataLoaderMap.erase (it_map);
    //}


    for (it_map = dataLoaderMap.begin(); it_map != dataLoaderMap.end(); ++it_map)
    {
        BaseData* data = obj->findData( (*it_map).first );
        if (data != nullptr)
        {
            if (!(*it_map).second->isAutoLink())
            {
                msg_info() << "Disabling autolink for Data '" << data->getName() << "'";
            }
            else
            {
                std::string linkPath = valueString+"."+(*it_map).first;
                data->setParent( (*it_map).second, linkPath);
            }
        }
    }
}

void* BaseObject::findLinkDestClass(const BaseClass* destType, const std::string& path, const BaseLink* link)
{
    if (this->getContext() == BaseContext::getDefault())
        return nullptr;
    else
        return this->getContext()->findLinkDestClass(destType, path, link);
}


const BaseContext* BaseObject::getContext() const
{
    return l_context.get();
}

BaseContext* BaseObject::getContext()
{
    return l_context.get();
}

const BaseObject* BaseObject::getMaster() const
{
    return l_master.get();
}

BaseObject* BaseObject::getMaster()
{
    return l_master.get();
}

const BaseObject::VecSlaves& BaseObject::getSlaves() const
{
    return l_slaves.getValue();
}

BaseObject* BaseObject::getSlave(const std::string& name) const
{
    for(VecSlaves::const_iterator iSlaves = l_slaves.begin(); iSlaves != l_slaves.end(); ++iSlaves)
    {
        if ((*iSlaves)->getName() == name)
            return iSlaves->get();
    }
    return nullptr;
}

void BaseObject::addSlave(BaseObject::SPtr s)
{
    BaseObject::SPtr previous = s->getMaster();
    if (previous == this) return;
    if (previous)
        previous->l_slaves.remove(s);
    l_slaves.add(s);
    if (previous)
        this->getContext()->notifyMoveSlave(previous.get(), this, s.get());
    else
        this->getContext()->notifyAddSlave(this, s.get());
}

void BaseObject::removeSlave(BaseObject::SPtr s)
{
    if (l_slaves.remove(s))
    {
        this->getContext()->notifyRemoveSlave(this, s.get());
    }
}

/// Copy the source aspect to the destination aspect for each Data in the component.
void BaseObject::copyAspect(int destAspect, int srcAspect)
{
    Base::copyAspect(destAspect, srcAspect);
    // copyAspect is no longer recursive to slave objects
}

/// Release memory allocated for the specified aspect.
void BaseObject::releaseAspect(int aspect)
{
    Base::releaseAspect(aspect);
    // releaseAspect is no longer recursive to slave objects
}

void BaseObject::init()
{
	for(VecData::const_iterator iData = this->m_vecData.begin(); iData != this->m_vecData.end(); ++iData)
	{
		if ((*iData)->isRequired() && !(*iData)->isSet())
		{
        msg_warning() << "Required data \"" << (*iData)->getName() << "\" has not been set. (Current value is " << (*iData)->getValueString() << ")" ;
		}
	}
}

void BaseObject::bwdInit()
{
}

void BaseObject::reinit()
{
}

void BaseObject::updateInternal()
{
    const auto& mapTrackedData = m_internalDataTracker.getMapTrackedData();
    for( auto const& it : mapTrackedData )
    {
        it.first->updateIfDirty();
    }

    if(m_internalDataTracker.hasChanged())
    {
        doUpdateInternal();
        m_internalDataTracker.clean();
    }
}

void BaseObject::trackInternalData(const objectmodel::BaseData& data)
{
    m_internalDataTracker.trackData(data);
}

bool BaseObject::hasDataChanged(const objectmodel::BaseData& data)
{
    bool dataFoundinTracker = false;
    const auto& mapTrackedData = m_internalDataTracker.getMapTrackedData();
    const std::string & dataName = data.getName();

    for( auto const& it : mapTrackedData )
    {
        if(it.first->getName()==dataName)
        {
            dataFoundinTracker=true;
            break;
        }
    }
    if(!dataFoundinTracker)
    {
        msg_error()<< "Data " << dataName << " is not tracked";
        return false;
    }

    return m_internalDataTracker.hasChanged(data);
}

void BaseObject::doUpdateInternal()
{ }

void BaseObject::storeResetState()
{ }

void BaseObject::reset()
{ }

void BaseObject::cleanup()
{ }

void BaseObject::handleEvent( Event* /*e*/ )
{ }

void BaseObject::handleTopologyChange(core::topology::Topology* t)
{
    if (t == this->getContext()->getTopology())
    {
        handleTopologyChange();
    }
}

SReal BaseObject::getTime() const
{
    return getContext()->getTime();
}

std::string BaseObject::getPathName() const {

    const BaseContext* context = this->getContext();
    std::string result = "";
    if( context )
    {
        const BaseNode* node = context->toBaseNode();
        if( node ) {
            result += node->getPathName();;
            if (node->getPathName() != "/")
                result += "/";
        }

    }
    result += getName();
    return result;
}

} // namespace objectmodel

} // namespace core

} // namespace sofa

