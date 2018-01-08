// This file has been generated by Py++.

// (C) Christopher Woods, GPL >= 2 License

#include "boost/python.hpp"
#include "Helpers/clone_const_reference.hpp"
#include "WindowedComponent.pypp.hpp"

namespace bp = boost::python;

#include "SireBase/numberproperty.h"

#include "SireBase/propertylist.h"

#include "SireError/errors.h"

#include "SireMaths/maths.h"

#include "SireStream/datastream.h"

#include "SireStream/shareddatastream.h"

#include "SireSystem/errors.h"

#include "constraint.h"

#include "delta.h"

#include "system.h"

#include <QDebug>

#include "constraint.h"

SireSystem::WindowedComponent __copy__(const SireSystem::WindowedComponent &other){ return SireSystem::WindowedComponent(other); }

#include "Qt/qdatastream.hpp"

#include "Helpers/str.hpp"

void register_WindowedComponent_class(){

    { //::SireSystem::WindowedComponent
        typedef bp::class_< SireSystem::WindowedComponent, bp::bases< SireSystem::Constraint, SireBase::Property > > WindowedComponent_exposer_t;
        WindowedComponent_exposer_t WindowedComponent_exposer = WindowedComponent_exposer_t( "WindowedComponent", "This constraint is used to constrain a component to adopt one of the values\nfrom a set - this is used to implement FEP windows, where lambda_forwards\ncan be constrained to be the next lambda value along\n\nAuthor: Christopher Woods\n", bp::init< >("Null constructor") );
        bp::scope WindowedComponent_scope( WindowedComponent_exposer );
        WindowedComponent_exposer.def( bp::init< SireCAS::Symbol const &, SireCAS::Symbol const &, QVector< double > const &, bp::optional< int > >(( bp::arg("component"), bp::arg("reference"), bp::arg("values"), bp::arg("step_size")=(int)(1) ), "Construct a WindowedConstraint that constrains the component represented\nby the symbol component to lie step_size windows above (or below if\nstep_size is negative) the window in which the component represented\nby the symbol reference resides - where values contains all of\nthe values of the windows, in the order that you have arranged\nthem. Whilst this will not sort window_values, it will remove\nall duplicate values") );
        WindowedComponent_exposer.def( bp::init< SireSystem::WindowedComponent const & >(( bp::arg("other") ), "Copy constructor") );
        { //::SireSystem::WindowedComponent::component
        
            typedef ::SireCAS::Symbol const & ( ::SireSystem::WindowedComponent::*component_function_type)(  ) const;
            component_function_type component_function_value( &::SireSystem::WindowedComponent::component );
            
            WindowedComponent_exposer.def( 
                "component"
                , component_function_value
                , bp::return_value_policy<bp::clone_const_reference>()
                , "Return the symbol representing the component being constrained" );
        
        }
        WindowedComponent_exposer.def( bp::self != bp::self );
        { //::SireSystem::WindowedComponent::operator=
        
            typedef ::SireSystem::WindowedComponent & ( ::SireSystem::WindowedComponent::*assign_function_type)( ::SireSystem::WindowedComponent const & ) ;
            assign_function_type assign_function_value( &::SireSystem::WindowedComponent::operator= );
            
            WindowedComponent_exposer.def( 
                "assign"
                , assign_function_value
                , ( bp::arg("other") )
                , bp::return_self< >()
                , "" );
        
        }
        WindowedComponent_exposer.def( bp::self == bp::self );
        { //::SireSystem::WindowedComponent::referenceComponent
        
            typedef ::SireCAS::Symbol const & ( ::SireSystem::WindowedComponent::*referenceComponent_function_type)(  ) const;
            referenceComponent_function_type referenceComponent_function_value( &::SireSystem::WindowedComponent::referenceComponent );
            
            WindowedComponent_exposer.def( 
                "referenceComponent"
                , referenceComponent_function_value
                , bp::return_value_policy<bp::clone_const_reference>()
                , "Return the symbol representing the reference component" );
        
        }
        { //::SireSystem::WindowedComponent::stepSize
        
            typedef int ( ::SireSystem::WindowedComponent::*stepSize_function_type)(  ) const;
            stepSize_function_type stepSize_function_value( &::SireSystem::WindowedComponent::stepSize );
            
            WindowedComponent_exposer.def( 
                "stepSize"
                , stepSize_function_value
                , "Return the step size for this windows - this is the number of\nwindows above (or below if step_size is negative) for this\nwindow compared to the window containing the reference component" );
        
        }
        { //::SireSystem::WindowedComponent::toString
        
            typedef ::QString ( ::SireSystem::WindowedComponent::*toString_function_type)(  ) const;
            toString_function_type toString_function_value( &::SireSystem::WindowedComponent::toString );
            
            WindowedComponent_exposer.def( 
                "toString"
                , toString_function_value
                , "Return a string representation of this constraint" );
        
        }
        { //::SireSystem::WindowedComponent::typeName
        
            typedef char const * ( *typeName_function_type )(  );
            typeName_function_type typeName_function_value( &::SireSystem::WindowedComponent::typeName );
            
            WindowedComponent_exposer.def( 
                "typeName"
                , typeName_function_value
                , "" );
        
        }
        { //::SireSystem::WindowedComponent::windowValues
        
            typedef ::QVector< double > const & ( ::SireSystem::WindowedComponent::*windowValues_function_type)(  ) const;
            windowValues_function_type windowValues_function_value( &::SireSystem::WindowedComponent::windowValues );
            
            WindowedComponent_exposer.def( 
                "windowValues"
                , windowValues_function_value
                , bp::return_value_policy< bp::copy_const_reference >()
                , "Return the values of all of the windows" );
        
        }
        WindowedComponent_exposer.staticmethod( "typeName" );
        WindowedComponent_exposer.def( "__copy__", &__copy__);
        WindowedComponent_exposer.def( "__deepcopy__", &__copy__);
        WindowedComponent_exposer.def( "clone", &__copy__);
        WindowedComponent_exposer.def( "__rlshift__", &__rlshift__QDataStream< ::SireSystem::WindowedComponent >,
                            bp::return_internal_reference<1, bp::with_custodian_and_ward<1,2> >() );
        WindowedComponent_exposer.def( "__rrshift__", &__rrshift__QDataStream< ::SireSystem::WindowedComponent >,
                            bp::return_internal_reference<1, bp::with_custodian_and_ward<1,2> >() );
        WindowedComponent_exposer.def( "__str__", &__str__< ::SireSystem::WindowedComponent > );
        WindowedComponent_exposer.def( "__repr__", &__str__< ::SireSystem::WindowedComponent > );
    }

}
