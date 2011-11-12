//////////////////////////////////////////////////////////////////////////
//  
//  Copyright (c) 2011, Image Engine Design Inc. All rights reserved.
//  Copyright (c) 2011, John Haddon. All rights reserved.
//  
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are
//  met:
//  
//      * Redistributions of source code must retain the above
//        copyright notice, this list of conditions and the following
//        disclaimer.
//  
//      * Redistributions in binary form must reproduce the above
//        copyright notice, this list of conditions and the following
//        disclaimer in the documentation and/or other materials provided with
//        the distribution.
//  
//      * Neither the name of John Haddon nor the names of
//        any other contributors to this software may be used to endorse or
//        promote products derived from this software without specific prior
//        written permission.
//  
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
//  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//  
//////////////////////////////////////////////////////////////////////////

#include "IECore/ParameterisedInterface.h"

#include "Gaffer/ParameterisedHolder.h"
#include "Gaffer/CompoundParameterHandler.h"
#include "Gaffer/TypedPlug.h"
#include "Gaffer/NumericPlug.h"

using namespace Gaffer;

template<typename BaseType>
const IECore::RunTimeTyped::TypeDescription<ParameterisedHolder<BaseType> > ParameterisedHolder<BaseType>::g_typeDescription;

template<typename BaseType>
ParameterisedHolder<BaseType>::ParameterisedHolder( const std::string &name )
	:	BaseType( name ), m_parameterised( 0 ), m_parameterHandler( 0 )
{
	BaseType::addChild( new StringPlug( "__className" ) );
	BaseType::addChild( new IntPlug( "__classVersion", Plug::In, -1 ) );
	BaseType::addChild( new StringPlug( "__searchPathEnvVar" ) );
}

template<typename BaseType>
ParameterisedHolder<BaseType>::~ParameterisedHolder()
{
}

template<typename BaseType>
void ParameterisedHolder<BaseType>::setParameterised( IECore::RunTimeTypedPtr parameterised, bool keepExistingValues )
{
	IECore::ParameterisedInterface *interface = dynamic_cast<IECore::ParameterisedInterface *>( parameterised.get() );
	if( !interface )
	{
		throw IECore::Exception( "Not a ParameterisedInterface derived type." );
	}
	
	m_parameterised = parameterised;
	m_parameterHandler = new CompoundParameterHandler( interface->parameters() );
	m_parameterHandler->setupPlug( this );
	if( !keepExistingValues )
	{
		m_parameterHandler->setPlugValue();
	}
}

template<typename BaseType>
void ParameterisedHolder<BaseType>::setParameterised( const std::string &className, int classVersion, const std::string &searchPathEnvVar, bool keepExistingValues )
{
	IECore::RunTimeTypedPtr p = loadClass( className, classVersion, searchPathEnvVar );

	GraphComponent::getChild<StringPlug>( "__className" )->setValue( className );
	GraphComponent::getChild<IntPlug>( "__classVersion" )->setValue( classVersion );
	GraphComponent::getChild<StringPlug>( "__searchPathEnvVar" )->setValue( searchPathEnvVar );

	setParameterised( p, keepExistingValues );
}

template<typename BaseType>
IECore::RunTimeTypedPtr ParameterisedHolder<BaseType>::getParameterised( std::string *className, int *classVersion, std::string *searchPathEnvVar ) const
{
	Node *node = const_cast<Node *>( static_cast<const Node *>( this ) );
	if( className )
	{
		*className = node->getChild<StringPlug>( "__className" )->getValue();
	}
	if( classVersion )
	{
		*classVersion = node->getChild<IntPlug>( "__classVersion" )->getValue();
	}
	if( searchPathEnvVar )
	{
		*searchPathEnvVar = node->getChild<StringPlug>( "__searchPathEnvVar" )->getValue();
	}
	return m_parameterised;
}

template<typename BaseType>
IECore::ParameterisedInterface *ParameterisedHolder<BaseType>::parameterisedInterface( std::string *className, int *classVersion, std::string *searchPathEnvVar )
{
	return dynamic_cast<IECore::ParameterisedInterface *>( getParameterised( className, classVersion, searchPathEnvVar ).get() );
}


template<typename BaseType>
CompoundParameterHandlerPtr ParameterisedHolder<BaseType>::parameterHandler()
{
	return m_parameterHandler;
}

template<typename BaseType>
ConstCompoundParameterHandlerPtr ParameterisedHolder<BaseType>::parameterHandler() const
{
	return m_parameterHandler;
}

template<typename BaseType>
void ParameterisedHolder<BaseType>::setParameterisedValues()
{
	if( m_parameterHandler )
	{
		m_parameterHandler->setParameterValue();
	}
}

template<typename BaseType>
void ParameterisedHolder<BaseType>::loadParameterised()
{
	std::string className = GraphComponent::getChild<StringPlug>( "__className" )->getValue();
	int classVersion = GraphComponent::getChild<IntPlug>( "__classVersion" )->getValue();
	std::string searchPathEnvVar = GraphComponent::getChild<StringPlug>( "__searchPathEnvVar" )->getValue();
	if( className.size() && searchPathEnvVar.size() )
	{
		setParameterised( className, classVersion, searchPathEnvVar, true );
	}
}

template<typename BaseType>
IECore::RunTimeTypedPtr ParameterisedHolder<BaseType>::loadClass( const std::string &className, int classVersion, const std::string &searchPathEnvVar ) const
{
	throw IECore::Exception( "Cannot load classes on a ParameterisedHolder not created in Python." );
}

//////////////////////////////////////////////////////////////////////////
// ParameterModificationContext
//////////////////////////////////////////////////////////////////////////

template<typename BaseType>
ParameterisedHolder<BaseType>::ParameterModificationContext::ParameterModificationContext( Ptr parameterisedHolder )
	:	m_parameterisedHolder( parameterisedHolder )
{
}

template<typename BaseType>
ParameterisedHolder<BaseType>::ParameterModificationContext::~ParameterModificationContext()
{
	if( m_parameterisedHolder->m_parameterHandler )
	{
		m_parameterisedHolder->m_parameterHandler->setupPlug( m_parameterisedHolder );
		m_parameterisedHolder->m_parameterHandler->setPlugValue();
	}
}

// specialisation

namespace Gaffer
{

IECORE_RUNTIMETYPED_DEFINETEMPLATESPECIALISATION( ParameterisedHolderNode, ParameterisedHolderNodeTypeId )

}

// explicit instantiation
template class ParameterisedHolder<Node>;
