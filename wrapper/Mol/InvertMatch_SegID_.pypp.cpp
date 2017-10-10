// This file has been generated by Py++.

// (C) Christopher Woods, GPL >= 2 License

#include "boost/python.hpp"
#include "InvertMatch_SegID_.pypp.hpp"

namespace bp = boost::python;

#include "SireMol/errors.h"

#include "SireStream/datastream.h"

#include "atom.h"

#include "chain.h"

#include "cutgroup.h"

#include "editor.hpp"

#include "groupatomids.h"

#include "groupgroupids.h"

#include "moleculegroup.h"

#include "moleculegroups.h"

#include "molecules.h"

#include "molinfo.h"

#include "mover.hpp"

#include "partialmolecule.h"

#include "residue.h"

#include "segid.h"

#include "segidentifier.h"

#include "segment.h"

#include "selector.hpp"

#include "tostring.h"

#include "segid.h"

SireID::InvertMatch<SireMol::SegID> __copy__(const SireID::InvertMatch<SireMol::SegID> &other){ return SireID::InvertMatch<SireMol::SegID>(other); }

#include "Qt/qdatastream.hpp"

#include "Helpers/str.hpp"

void register_InvertMatch_SegID__class(){

    { //::SireID::InvertMatch< SireMol::SegID >
        typedef bp::class_< SireID::InvertMatch< SireMol::SegID >, bp::bases< SireMol::SegID, SireID::ID > > InvertMatch_SegID__exposer_t;
        InvertMatch_SegID__exposer_t InvertMatch_SegID__exposer = InvertMatch_SegID__exposer_t( "InvertMatch_SegID_", "", bp::init< >("") );
        bp::scope InvertMatch_SegID__scope( InvertMatch_SegID__exposer );
        InvertMatch_SegID__exposer.def( bp::init< SireMol::SegID const & >(( bp::arg("id") ), "") );
        InvertMatch_SegID__exposer.def( bp::init< SireID::InvertMatch< SireMol::SegID > const & >(( bp::arg("other") ), "") );
        { //::SireID::InvertMatch< SireMol::SegID >::hash
        
            typedef SireID::InvertMatch< SireMol::SegID > exported_class_t;
            typedef ::uint ( ::SireID::InvertMatch< SireMol::SegID >::*hash_function_type)(  ) const;
            hash_function_type hash_function_value( &::SireID::InvertMatch< SireMol::SegID >::hash );
            
            InvertMatch_SegID__exposer.def( 
                "hash"
                , hash_function_value
                , "" );
        
        }
        { //::SireID::InvertMatch< SireMol::SegID >::isNull
        
            typedef SireID::InvertMatch< SireMol::SegID > exported_class_t;
            typedef bool ( ::SireID::InvertMatch< SireMol::SegID >::*isNull_function_type)(  ) const;
            isNull_function_type isNull_function_value( &::SireID::InvertMatch< SireMol::SegID >::isNull );
            
            InvertMatch_SegID__exposer.def( 
                "isNull"
                , isNull_function_value
                , "" );
        
        }
        { //::SireID::InvertMatch< SireMol::SegID >::map
        
            typedef SireID::InvertMatch< SireMol::SegID > exported_class_t;
            typedef ::QList< SireMol::SegIdx > ( ::SireID::InvertMatch< SireMol::SegID >::*map_function_type)( ::SireMol::SegID::SearchObject const & ) const;
            map_function_type map_function_value( &::SireID::InvertMatch< SireMol::SegID >::map );
            
            InvertMatch_SegID__exposer.def( 
                "map"
                , map_function_value
                , ( bp::arg("obj") )
                , "" );
        
        }
        InvertMatch_SegID__exposer.def( bp::self != bp::self );
        InvertMatch_SegID__exposer.def( bp::self != bp::other< SireID::ID >() );
        { //::SireID::InvertMatch< SireMol::SegID >::operator=
        
            typedef SireID::InvertMatch< SireMol::SegID > exported_class_t;
            typedef ::SireID::InvertMatch< SireMol::SegID > & ( ::SireID::InvertMatch< SireMol::SegID >::*assign_function_type)( ::SireID::InvertMatch< SireMol::SegID > const & ) ;
            assign_function_type assign_function_value( &::SireID::InvertMatch< SireMol::SegID >::operator= );
            
            InvertMatch_SegID__exposer.def( 
                "assign"
                , assign_function_value
                , ( bp::arg("other") )
                , bp::return_self< >()
                , "" );
        
        }
        InvertMatch_SegID__exposer.def( bp::self == bp::self );
        InvertMatch_SegID__exposer.def( bp::self == bp::other< SireID::ID >() );
        { //::SireID::InvertMatch< SireMol::SegID >::toString
        
            typedef SireID::InvertMatch< SireMol::SegID > exported_class_t;
            typedef ::QString ( ::SireID::InvertMatch< SireMol::SegID >::*toString_function_type)(  ) const;
            toString_function_type toString_function_value( &::SireID::InvertMatch< SireMol::SegID >::toString );
            
            InvertMatch_SegID__exposer.def( 
                "toString"
                , toString_function_value
                , "" );
        
        }
        { //::SireID::InvertMatch< SireMol::SegID >::typeName
        
            typedef SireID::InvertMatch< SireMol::SegID > exported_class_t;
            typedef char const * ( *typeName_function_type )(  );
            typeName_function_type typeName_function_value( &::SireID::InvertMatch< SireMol::SegID >::typeName );
            
            InvertMatch_SegID__exposer.def( 
                "typeName"
                , typeName_function_value
                , "" );
        
        }
        { //::SireID::InvertMatch< SireMol::SegID >::what
        
            typedef SireID::InvertMatch< SireMol::SegID > exported_class_t;
            typedef char const * ( ::SireID::InvertMatch< SireMol::SegID >::*what_function_type)(  ) const;
            what_function_type what_function_value( &::SireID::InvertMatch< SireMol::SegID >::what );
            
            InvertMatch_SegID__exposer.def( 
                "what"
                , what_function_value
                , "" );
        
        }
        InvertMatch_SegID__exposer.staticmethod( "typeName" );
        InvertMatch_SegID__exposer.def( "__copy__", &__copy__);
        InvertMatch_SegID__exposer.def( "__deepcopy__", &__copy__);
        InvertMatch_SegID__exposer.def( "clone", &__copy__);
        InvertMatch_SegID__exposer.def( "__rlshift__", &__rlshift__QDataStream< ::SireID::InvertMatch<SireMol::SegID> >,
                            bp::return_internal_reference<1, bp::with_custodian_and_ward<1,2> >() );
        InvertMatch_SegID__exposer.def( "__rrshift__", &__rrshift__QDataStream< ::SireID::InvertMatch<SireMol::SegID> >,
                            bp::return_internal_reference<1, bp::with_custodian_and_ward<1,2> >() );
        InvertMatch_SegID__exposer.def( "__str__", &__str__< ::SireID::InvertMatch<SireMol::SegID> > );
        InvertMatch_SegID__exposer.def( "__repr__", &__str__< ::SireID::InvertMatch<SireMol::SegID> > );
        InvertMatch_SegID__exposer.def( "__hash__", &::SireID::InvertMatch<SireMol::SegID>::hash );
    }

}