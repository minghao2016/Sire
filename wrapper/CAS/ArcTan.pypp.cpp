// This file has been generated by Py++.

// (C) Christopher Woods, GPL >= 2 License

#include "boost/python.hpp"
#include "ArcTan.pypp.hpp"

namespace bp = boost::python;

#include "SireMaths/errors.h"

#include "SireStream/datastream.h"

#include "complexvalues.h"

#include "exp.h"

#include "expression.h"

#include "identities.h"

#include "invtrigfuncs.h"

#include "trigfuncs.h"

#include "invtrigfuncs.h"

SireCAS::ArcTan __copy__(const SireCAS::ArcTan &other){ return SireCAS::ArcTan(other); }

#include "Qt/qdatastream.hpp"

#include "Helpers/str.hpp"

void register_ArcTan_class(){

    { //::SireCAS::ArcTan
        typedef bp::class_< SireCAS::ArcTan, bp::bases< SireCAS::SingleFunc, SireCAS::ExBase > > ArcTan_exposer_t;
        ArcTan_exposer_t ArcTan_exposer = ArcTan_exposer_t( "ArcTan", bp::init< >() );
        bp::scope ArcTan_scope( ArcTan_exposer );
        ArcTan_exposer.def( bp::init< SireCAS::Expression const & >(( bp::arg("ex") )) );
        ArcTan_exposer.def( bp::init< SireCAS::ArcTan const & >(( bp::arg("other") )) );
        { //::SireCAS::ArcTan::evaluate
        
            typedef double ( ::SireCAS::ArcTan::*evaluate_function_type)( ::SireCAS::Values const & ) const;
            evaluate_function_type evaluate_function_value( &::SireCAS::ArcTan::evaluate );
            
            ArcTan_exposer.def( 
                "evaluate"
                , evaluate_function_value
                , ( bp::arg("values") ) );
        
        }
        { //::SireCAS::ArcTan::evaluate
        
            typedef ::SireMaths::Complex ( ::SireCAS::ArcTan::*evaluate_function_type)( ::SireCAS::ComplexValues const & ) const;
            evaluate_function_type evaluate_function_value( &::SireCAS::ArcTan::evaluate );
            
            ArcTan_exposer.def( 
                "evaluate"
                , evaluate_function_value
                , ( bp::arg("values") ) );
        
        }
        ArcTan_exposer.def( bp::self == bp::other< SireCAS::ExBase >() );
        { //::SireCAS::ArcTan::typeName
        
            typedef char const * ( *typeName_function_type )(  );
            typeName_function_type typeName_function_value( &::SireCAS::ArcTan::typeName );
            
            ArcTan_exposer.def( 
                "typeName"
                , typeName_function_value );
        
        }
        { //::SireCAS::ArcTan::what
        
            typedef char const * ( ::SireCAS::ArcTan::*what_function_type)(  ) const;
            what_function_type what_function_value( &::SireCAS::ArcTan::what );
            
            ArcTan_exposer.def( 
                "what"
                , what_function_value );
        
        }
        ArcTan_exposer.staticmethod( "typeName" );
        ArcTan_exposer.def( "__copy__", &__copy__);
        ArcTan_exposer.def( "__deepcopy__", &__copy__);
        ArcTan_exposer.def( "clone", &__copy__);
        ArcTan_exposer.def( "__rlshift__", &__rlshift__QDataStream< ::SireCAS::ArcTan >,
                            bp::return_internal_reference<1, bp::with_custodian_and_ward<1,2> >() );
        ArcTan_exposer.def( "__rrshift__", &__rrshift__QDataStream< ::SireCAS::ArcTan >,
                            bp::return_internal_reference<1, bp::with_custodian_and_ward<1,2> >() );
        ArcTan_exposer.def( "__str__", &__str__< ::SireCAS::ArcTan > );
        ArcTan_exposer.def( "__repr__", &__str__< ::SireCAS::ArcTan > );
        ArcTan_exposer.def( "__hash__", &::SireCAS::ArcTan::hash );
    }

}
