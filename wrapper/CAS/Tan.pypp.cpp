// This file has been generated by Py++.

// (C) Christopher Woods, GPL >= 2 License

#include "boost/python.hpp"
#include "Tan.pypp.hpp"

namespace bp = boost::python;

#include "SireStream/datastream.h"

#include "complexvalues.h"

#include "exp.h"

#include "expression.h"

#include "identities.h"

#include "trigfuncs.h"

#include "trigfuncs.h"

SireCAS::Tan __copy__(const SireCAS::Tan &other){ return SireCAS::Tan(other); }

#include "Qt/qdatastream.hpp"

#include "Helpers/str.hpp"

void register_Tan_class(){

    { //::SireCAS::Tan
        typedef bp::class_< SireCAS::Tan, bp::bases< SireCAS::SingleFunc, SireCAS::ExBase > > Tan_exposer_t;
        Tan_exposer_t Tan_exposer = Tan_exposer_t( "Tan", bp::init< >() );
        bp::scope Tan_scope( Tan_exposer );
        Tan_exposer.def( bp::init< SireCAS::Expression const & >(( bp::arg("ex") )) );
        Tan_exposer.def( bp::init< SireCAS::Tan const & >(( bp::arg("other") )) );
        { //::SireCAS::Tan::evaluate
        
            typedef double ( ::SireCAS::Tan::*evaluate_function_type)( ::SireCAS::Values const & ) const;
            evaluate_function_type evaluate_function_value( &::SireCAS::Tan::evaluate );
            
            Tan_exposer.def( 
                "evaluate"
                , evaluate_function_value
                , ( bp::arg("values") ) );
        
        }
        { //::SireCAS::Tan::evaluate
        
            typedef ::SireMaths::Complex ( ::SireCAS::Tan::*evaluate_function_type)( ::SireCAS::ComplexValues const & ) const;
            evaluate_function_type evaluate_function_value( &::SireCAS::Tan::evaluate );
            
            Tan_exposer.def( 
                "evaluate"
                , evaluate_function_value
                , ( bp::arg("values") ) );
        
        }
        Tan_exposer.def( bp::self == bp::other< SireCAS::ExBase >() );
        { //::SireCAS::Tan::typeName
        
            typedef char const * ( *typeName_function_type )(  );
            typeName_function_type typeName_function_value( &::SireCAS::Tan::typeName );
            
            Tan_exposer.def( 
                "typeName"
                , typeName_function_value );
        
        }
        { //::SireCAS::Tan::what
        
            typedef char const * ( ::SireCAS::Tan::*what_function_type)(  ) const;
            what_function_type what_function_value( &::SireCAS::Tan::what );
            
            Tan_exposer.def( 
                "what"
                , what_function_value );
        
        }
        Tan_exposer.staticmethod( "typeName" );
        Tan_exposer.def( "__copy__", &__copy__);
        Tan_exposer.def( "__deepcopy__", &__copy__);
        Tan_exposer.def( "clone", &__copy__);
        Tan_exposer.def( "__rlshift__", &__rlshift__QDataStream< ::SireCAS::Tan >,
                            bp::return_internal_reference<1, bp::with_custodian_and_ward<1,2> >() );
        Tan_exposer.def( "__rrshift__", &__rrshift__QDataStream< ::SireCAS::Tan >,
                            bp::return_internal_reference<1, bp::with_custodian_and_ward<1,2> >() );
        Tan_exposer.def( "__str__", &__str__< ::SireCAS::Tan > );
        Tan_exposer.def( "__repr__", &__str__< ::SireCAS::Tan > );
        Tan_exposer.def( "__hash__", &::SireCAS::Tan::hash );
    }

}
