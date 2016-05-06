// This file has been generated by Py++.

// (C) Christopher Woods, GPL >= 2 License

#include "boost/python.hpp"
#include "ArcSinh.pypp.hpp"

namespace bp = boost::python;

#include "SireMaths/errors.h"

#include "SireStream/datastream.h"

#include "complexvalues.h"

#include "exp.h"

#include "expression.h"

#include "hyperbolicfuncs.h"

#include "identities.h"

#include "invhyperbolicfuncs.h"

#include "invtrigfuncs.h"

#include "trigfuncs.h"

#include "invhyperbolicfuncs.h"

SireCAS::ArcSinh __copy__(const SireCAS::ArcSinh &other){ return SireCAS::ArcSinh(other); }

#include "Qt/qdatastream.hpp"

#include "Helpers/str.hpp"

void register_ArcSinh_class(){

    { //::SireCAS::ArcSinh
        typedef bp::class_< SireCAS::ArcSinh, bp::bases< SireCAS::SingleFunc, SireCAS::ExBase > > ArcSinh_exposer_t;
        ArcSinh_exposer_t ArcSinh_exposer = ArcSinh_exposer_t( "ArcSinh", bp::init< >() );
        bp::scope ArcSinh_scope( ArcSinh_exposer );
        ArcSinh_exposer.def( bp::init< SireCAS::Expression const & >(( bp::arg("ex") )) );
        ArcSinh_exposer.def( bp::init< SireCAS::ArcSinh const & >(( bp::arg("other") )) );
        { //::SireCAS::ArcSinh::evaluate
        
            typedef double ( ::SireCAS::ArcSinh::*evaluate_function_type)( ::SireCAS::Values const & ) const;
            evaluate_function_type evaluate_function_value( &::SireCAS::ArcSinh::evaluate );
            
            ArcSinh_exposer.def( 
                "evaluate"
                , evaluate_function_value
                , ( bp::arg("values") ) );
        
        }
        { //::SireCAS::ArcSinh::evaluate
        
            typedef ::SireMaths::Complex ( ::SireCAS::ArcSinh::*evaluate_function_type)( ::SireCAS::ComplexValues const & ) const;
            evaluate_function_type evaluate_function_value( &::SireCAS::ArcSinh::evaluate );
            
            ArcSinh_exposer.def( 
                "evaluate"
                , evaluate_function_value
                , ( bp::arg("values") ) );
        
        }
        ArcSinh_exposer.def( bp::self == bp::other< SireCAS::ExBase >() );
        { //::SireCAS::ArcSinh::typeName
        
            typedef char const * ( *typeName_function_type )(  );
            typeName_function_type typeName_function_value( &::SireCAS::ArcSinh::typeName );
            
            ArcSinh_exposer.def( 
                "typeName"
                , typeName_function_value );
        
        }
        { //::SireCAS::ArcSinh::what
        
            typedef char const * ( ::SireCAS::ArcSinh::*what_function_type)(  ) const;
            what_function_type what_function_value( &::SireCAS::ArcSinh::what );
            
            ArcSinh_exposer.def( 
                "what"
                , what_function_value );
        
        }
        ArcSinh_exposer.staticmethod( "typeName" );
        ArcSinh_exposer.def( "__copy__", &__copy__);
        ArcSinh_exposer.def( "__deepcopy__", &__copy__);
        ArcSinh_exposer.def( "clone", &__copy__);
        ArcSinh_exposer.def( "__rlshift__", &__rlshift__QDataStream< ::SireCAS::ArcSinh >,
                            bp::return_internal_reference<1, bp::with_custodian_and_ward<1,2> >() );
        ArcSinh_exposer.def( "__rrshift__", &__rrshift__QDataStream< ::SireCAS::ArcSinh >,
                            bp::return_internal_reference<1, bp::with_custodian_and_ward<1,2> >() );
        ArcSinh_exposer.def( "__str__", &__str__< ::SireCAS::ArcSinh > );
        ArcSinh_exposer.def( "__repr__", &__str__< ::SireCAS::ArcSinh > );
        ArcSinh_exposer.def( "__hash__", &::SireCAS::ArcSinh::hash );
    }

}
