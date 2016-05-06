// This file has been generated by Py++.

// (C) Christopher Woods, GPL >= 2 License

#include "boost/python.hpp"
#include "MolNum.pypp.hpp"

namespace bp = boost::python;

#include "molnum.h"

#include "molnum.h"

#include "molid.h"

#include "molidx.h"

#include "molnum.h"

#include "molname.h"

#include "moleculegroup.h"

#include "moleculegroups.h"

#include "mover.hpp"

SireMol::MolNum __copy__(const SireMol::MolNum &other){ return SireMol::MolNum(other); }

#include "Qt/qdatastream.hpp"

#include "Helpers/str.hpp"

void register_MolNum_class(){

    { //::SireMol::MolNum
        typedef bp::class_< SireMol::MolNum, bp::bases< SireMol::MolID, SireID::ID, SireID::Number > > MolNum_exposer_t;
        MolNum_exposer_t MolNum_exposer = MolNum_exposer_t( "MolNum", bp::init< >() );
        bp::scope MolNum_scope( MolNum_exposer );
        MolNum_exposer.def( bp::init< quint32 >(( bp::arg("num") )) );
        MolNum_exposer.def( bp::init< SireMol::MolNum const & >(( bp::arg("other") )) );
        { //::SireMol::MolNum::getUniqueNumber
        
            typedef ::SireMol::MolNum ( *getUniqueNumber_function_type )(  );
            getUniqueNumber_function_type getUniqueNumber_function_value( &::SireMol::MolNum::getUniqueNumber );
            
            MolNum_exposer.def( 
                "getUniqueNumber"
                , getUniqueNumber_function_value );
        
        }
        { //::SireMol::MolNum::hash
        
            typedef ::uint ( ::SireMol::MolNum::*hash_function_type)(  ) const;
            hash_function_type hash_function_value( &::SireMol::MolNum::hash );
            
            MolNum_exposer.def( 
                "hash"
                , hash_function_value );
        
        }
        { //::SireMol::MolNum::isNull
        
            typedef bool ( ::SireMol::MolNum::*isNull_function_type)(  ) const;
            isNull_function_type isNull_function_value( &::SireMol::MolNum::isNull );
            
            MolNum_exposer.def( 
                "isNull"
                , isNull_function_value );
        
        }
        { //::SireMol::MolNum::map
        
            typedef ::QList< SireMol::MolNum > ( ::SireMol::MolNum::*map_function_type)( ::SireMol::Molecules const & ) const;
            map_function_type map_function_value( &::SireMol::MolNum::map );
            
            MolNum_exposer.def( 
                "map"
                , map_function_value
                , ( bp::arg("molecules") ) );
        
        }
        { //::SireMol::MolNum::map
        
            typedef ::QList< SireMol::MolNum > ( ::SireMol::MolNum::*map_function_type)( ::SireMol::MoleculeGroup const & ) const;
            map_function_type map_function_value( &::SireMol::MolNum::map );
            
            MolNum_exposer.def( 
                "map"
                , map_function_value
                , ( bp::arg("arg0") ) );
        
        }
        { //::SireMol::MolNum::map
        
            typedef ::QList< SireMol::MolNum > ( ::SireMol::MolNum::*map_function_type)( ::SireMol::MolGroupsBase const & ) const;
            map_function_type map_function_value( &::SireMol::MolNum::map );
            
            MolNum_exposer.def( 
                "map"
                , map_function_value
                , ( bp::arg("arg0") ) );
        
        }
        MolNum_exposer.def( bp::self != bp::self );
        MolNum_exposer.def( bp::self < bp::self );
        MolNum_exposer.def( bp::self <= bp::self );
        { //::SireMol::MolNum::operator=
        
            typedef ::SireMol::MolNum & ( ::SireMol::MolNum::*assign_function_type)( ::SireMol::MolNum const & ) ;
            assign_function_type assign_function_value( &::SireMol::MolNum::operator= );
            
            MolNum_exposer.def( 
                "assign"
                , assign_function_value
                , ( bp::arg("other") )
                , bp::return_self< >() );
        
        }
        MolNum_exposer.def( bp::self == bp::other< SireID::ID >() );
        MolNum_exposer.def( bp::self == bp::self );
        MolNum_exposer.def( bp::self > bp::self );
        MolNum_exposer.def( bp::self >= bp::self );
        { //::SireMol::MolNum::toString
        
            typedef ::QString ( ::SireMol::MolNum::*toString_function_type)(  ) const;
            toString_function_type toString_function_value( &::SireMol::MolNum::toString );
            
            MolNum_exposer.def( 
                "toString"
                , toString_function_value );
        
        }
        { //::SireMol::MolNum::typeName
        
            typedef char const * ( *typeName_function_type )(  );
            typeName_function_type typeName_function_value( &::SireMol::MolNum::typeName );
            
            MolNum_exposer.def( 
                "typeName"
                , typeName_function_value );
        
        }
        { //::SireMol::MolNum::what
        
            typedef char const * ( ::SireMol::MolNum::*what_function_type)(  ) const;
            what_function_type what_function_value( &::SireMol::MolNum::what );
            
            MolNum_exposer.def( 
                "what"
                , what_function_value );
        
        }
        MolNum_exposer.staticmethod( "getUniqueNumber" );
        MolNum_exposer.staticmethod( "typeName" );
        MolNum_exposer.def( "__copy__", &__copy__);
        MolNum_exposer.def( "__deepcopy__", &__copy__);
        MolNum_exposer.def( "clone", &__copy__);
        MolNum_exposer.def( "__rlshift__", &__rlshift__QDataStream< ::SireMol::MolNum >,
                            bp::return_internal_reference<1, bp::with_custodian_and_ward<1,2> >() );
        MolNum_exposer.def( "__rrshift__", &__rrshift__QDataStream< ::SireMol::MolNum >,
                            bp::return_internal_reference<1, bp::with_custodian_and_ward<1,2> >() );
        MolNum_exposer.def( "__str__", &__str__< ::SireMol::MolNum > );
        MolNum_exposer.def( "__repr__", &__str__< ::SireMol::MolNum > );
        MolNum_exposer.def( "__hash__", &::SireMol::MolNum::hash );
    }

}
