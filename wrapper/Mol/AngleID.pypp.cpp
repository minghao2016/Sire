// This file has been generated by Py++.

// (C) Christopher Woods, GPL >= 2 License

#include "boost/python.hpp"
#include "Helpers/clone_const_reference.hpp"
#include "AngleID.pypp.hpp"

namespace bp = boost::python;

#include "SireBase/property.h"

#include "SireMaths/triangle.h"

#include "SireMaths/vector.h"

#include "SireStream/datastream.h"

#include "SireStream/shareddatastream.h"

#include "angleid.h"

#include "atomcoords.h"

#include "moleculedata.h"

#include "moleculeinfodata.h"

#include "angleid.h"

SireMol::AngleID __copy__(const SireMol::AngleID &other){ return SireMol::AngleID(other); }

#include "Qt/qdatastream.hpp"

#include "Helpers/str.hpp"

void register_AngleID_class(){

    { //::SireMol::AngleID
        typedef bp::class_< SireMol::AngleID, bp::bases< SireID::ID > > AngleID_exposer_t;
        AngleID_exposer_t AngleID_exposer = AngleID_exposer_t( "AngleID", bp::init< >() );
        bp::scope AngleID_scope( AngleID_exposer );
        AngleID_exposer.def( bp::init< SireMol::AtomID const &, SireMol::AtomID const &, SireMol::AtomID const & >(( bp::arg("atom0"), bp::arg("atom1"), bp::arg("atom2") )) );
        AngleID_exposer.def( bp::init< SireMol::AngleID const & >(( bp::arg("other") )) );
        { //::SireMol::AngleID::atom0
        
            typedef ::SireMol::AtomID const & ( ::SireMol::AngleID::*atom0_function_type)(  ) const;
            atom0_function_type atom0_function_value( &::SireMol::AngleID::atom0 );
            
            AngleID_exposer.def( 
                "atom0"
                , atom0_function_value
                , bp::return_value_policy<bp::clone_const_reference>() );
        
        }
        { //::SireMol::AngleID::atom1
        
            typedef ::SireMol::AtomID const & ( ::SireMol::AngleID::*atom1_function_type)(  ) const;
            atom1_function_type atom1_function_value( &::SireMol::AngleID::atom1 );
            
            AngleID_exposer.def( 
                "atom1"
                , atom1_function_value
                , bp::return_value_policy<bp::clone_const_reference>() );
        
        }
        { //::SireMol::AngleID::atom2
        
            typedef ::SireMol::AtomID const & ( ::SireMol::AngleID::*atom2_function_type)(  ) const;
            atom2_function_type atom2_function_value( &::SireMol::AngleID::atom2 );
            
            AngleID_exposer.def( 
                "atom2"
                , atom2_function_value
                , bp::return_value_policy<bp::clone_const_reference>() );
        
        }
        { //::SireMol::AngleID::hash
        
            typedef ::uint ( ::SireMol::AngleID::*hash_function_type)(  ) const;
            hash_function_type hash_function_value( &::SireMol::AngleID::hash );
            
            AngleID_exposer.def( 
                "hash"
                , hash_function_value );
        
        }
        { //::SireMol::AngleID::isNull
        
            typedef bool ( ::SireMol::AngleID::*isNull_function_type)(  ) const;
            isNull_function_type isNull_function_value( &::SireMol::AngleID::isNull );
            
            AngleID_exposer.def( 
                "isNull"
                , isNull_function_value );
        
        }
        { //::SireMol::AngleID::map
        
            typedef ::boost::tuples::tuple< SireMol::AtomIdx, SireMol::AtomIdx, SireMol::AtomIdx, boost::tuples::null_type, boost::tuples::null_type, boost::tuples::null_type, boost::tuples::null_type, boost::tuples::null_type, boost::tuples::null_type, boost::tuples::null_type > ( ::SireMol::AngleID::*map_function_type)( ::SireMol::MoleculeInfoData const & ) const;
            map_function_type map_function_value( &::SireMol::AngleID::map );
            
            AngleID_exposer.def( 
                "map"
                , map_function_value
                , ( bp::arg("molinfo") ) );
        
        }
        { //::SireMol::AngleID::map
        
            typedef ::boost::tuples::tuple< SireMol::AtomIdx, SireMol::AtomIdx, SireMol::AtomIdx, boost::tuples::null_type, boost::tuples::null_type, boost::tuples::null_type, boost::tuples::null_type, boost::tuples::null_type, boost::tuples::null_type, boost::tuples::null_type > ( ::SireMol::AngleID::*map_function_type)( ::SireMol::MoleculeInfoData const &,::SireMol::MoleculeInfoData const &,::SireMol::MoleculeInfoData const & ) const;
            map_function_type map_function_value( &::SireMol::AngleID::map );
            
            AngleID_exposer.def( 
                "map"
                , map_function_value
                , ( bp::arg("mol0info"), bp::arg("mol1info"), bp::arg("mol2info") ) );
        
        }
        { //::SireMol::AngleID::mirror
        
            typedef ::SireMol::AngleID ( ::SireMol::AngleID::*mirror_function_type)(  ) const;
            mirror_function_type mirror_function_value( &::SireMol::AngleID::mirror );
            
            AngleID_exposer.def( 
                "mirror"
                , mirror_function_value );
        
        }
        AngleID_exposer.def( bp::self != bp::self );
        { //::SireMol::AngleID::operator=
        
            typedef ::SireMol::AngleID & ( ::SireMol::AngleID::*assign_function_type)( ::SireMol::AngleID const & ) ;
            assign_function_type assign_function_value( &::SireMol::AngleID::operator= );
            
            AngleID_exposer.def( 
                "assign"
                , assign_function_value
                , ( bp::arg("other") )
                , bp::return_self< >() );
        
        }
        AngleID_exposer.def( bp::self == bp::other< SireID::ID >() );
        AngleID_exposer.def( bp::self == bp::self );
        { //::SireMol::AngleID::size
        
            typedef ::SireUnits::Dimension::Angle ( ::SireMol::AngleID::*size_function_type)( ::SireMol::MoleculeData const &,::SireBase::PropertyMap const & ) const;
            size_function_type size_function_value( &::SireMol::AngleID::size );
            
            AngleID_exposer.def( 
                "size"
                , size_function_value
                , ( bp::arg("moldata"), bp::arg("map")=SireBase::PropertyMap() ) );
        
        }
        { //::SireMol::AngleID::size
        
            typedef ::SireUnits::Dimension::Angle ( ::SireMol::AngleID::*size_function_type)( ::SireMol::MoleculeData const &,::SireMol::MoleculeData const &,::SireMol::MoleculeData const &,::SireBase::PropertyMap const & ) const;
            size_function_type size_function_value( &::SireMol::AngleID::size );
            
            AngleID_exposer.def( 
                "size"
                , size_function_value
                , ( bp::arg("mol0data"), bp::arg("mol1data"), bp::arg("mol2data"), bp::arg("map")=SireBase::PropertyMap() ) );
        
        }
        { //::SireMol::AngleID::size
        
            typedef ::SireUnits::Dimension::Angle ( ::SireMol::AngleID::*size_function_type)( ::SireMol::MoleculeData const &,::SireBase::PropertyMap const &,::SireMol::MoleculeData const &,::SireBase::PropertyMap const &,::SireMol::MoleculeData const &,::SireBase::PropertyMap const & ) const;
            size_function_type size_function_value( &::SireMol::AngleID::size );
            
            AngleID_exposer.def( 
                "size"
                , size_function_value
                , ( bp::arg("mol0data"), bp::arg("map0"), bp::arg("mol1data"), bp::arg("map1"), bp::arg("mol2data"), bp::arg("map2") ) );
        
        }
        { //::SireMol::AngleID::toString
        
            typedef ::QString ( ::SireMol::AngleID::*toString_function_type)(  ) const;
            toString_function_type toString_function_value( &::SireMol::AngleID::toString );
            
            AngleID_exposer.def( 
                "toString"
                , toString_function_value );
        
        }
        { //::SireMol::AngleID::triangle
        
            typedef ::SireMaths::Triangle ( ::SireMol::AngleID::*triangle_function_type)( ::SireMol::MoleculeData const &,::SireBase::PropertyMap const & ) const;
            triangle_function_type triangle_function_value( &::SireMol::AngleID::triangle );
            
            AngleID_exposer.def( 
                "triangle"
                , triangle_function_value
                , ( bp::arg("moldata"), bp::arg("map")=SireBase::PropertyMap() ) );
        
        }
        { //::SireMol::AngleID::triangle
        
            typedef ::SireMaths::Triangle ( ::SireMol::AngleID::*triangle_function_type)( ::SireMol::MoleculeData const &,::SireMol::MoleculeData const &,::SireMol::MoleculeData const &,::SireBase::PropertyMap const & ) const;
            triangle_function_type triangle_function_value( &::SireMol::AngleID::triangle );
            
            AngleID_exposer.def( 
                "triangle"
                , triangle_function_value
                , ( bp::arg("mol0data"), bp::arg("mol1data"), bp::arg("mol2data"), bp::arg("map")=SireBase::PropertyMap() ) );
        
        }
        { //::SireMol::AngleID::triangle
        
            typedef ::SireMaths::Triangle ( ::SireMol::AngleID::*triangle_function_type)( ::SireMol::MoleculeData const &,::SireBase::PropertyMap const &,::SireMol::MoleculeData const &,::SireBase::PropertyMap const &,::SireMol::MoleculeData const &,::SireBase::PropertyMap const & ) const;
            triangle_function_type triangle_function_value( &::SireMol::AngleID::triangle );
            
            AngleID_exposer.def( 
                "triangle"
                , triangle_function_value
                , ( bp::arg("mol0data"), bp::arg("map0"), bp::arg("mol1data"), bp::arg("map1"), bp::arg("mol2data"), bp::arg("map2") ) );
        
        }
        { //::SireMol::AngleID::typeName
        
            typedef char const * ( *typeName_function_type )(  );
            typeName_function_type typeName_function_value( &::SireMol::AngleID::typeName );
            
            AngleID_exposer.def( 
                "typeName"
                , typeName_function_value );
        
        }
        { //::SireMol::AngleID::vector
        
            typedef ::SireMaths::Vector ( ::SireMol::AngleID::*vector_function_type)( ::SireMol::MoleculeData const &,::SireBase::PropertyMap const & ) const;
            vector_function_type vector_function_value( &::SireMol::AngleID::vector );
            
            AngleID_exposer.def( 
                "vector"
                , vector_function_value
                , ( bp::arg("moldata"), bp::arg("map")=SireBase::PropertyMap() ) );
        
        }
        { //::SireMol::AngleID::vector
        
            typedef ::SireMaths::Vector ( ::SireMol::AngleID::*vector_function_type)( ::SireMol::MoleculeData const &,::SireMol::MoleculeData const &,::SireMol::MoleculeData const &,::SireBase::PropertyMap const & ) const;
            vector_function_type vector_function_value( &::SireMol::AngleID::vector );
            
            AngleID_exposer.def( 
                "vector"
                , vector_function_value
                , ( bp::arg("mol0data"), bp::arg("mol1data"), bp::arg("mol2data"), bp::arg("map")=SireBase::PropertyMap() ) );
        
        }
        { //::SireMol::AngleID::vector
        
            typedef ::SireMaths::Vector ( ::SireMol::AngleID::*vector_function_type)( ::SireMol::MoleculeData const &,::SireBase::PropertyMap const &,::SireMol::MoleculeData const &,::SireBase::PropertyMap const &,::SireMol::MoleculeData const &,::SireBase::PropertyMap const & ) const;
            vector_function_type vector_function_value( &::SireMol::AngleID::vector );
            
            AngleID_exposer.def( 
                "vector"
                , vector_function_value
                , ( bp::arg("mol0data"), bp::arg("map0"), bp::arg("mol1data"), bp::arg("map1"), bp::arg("mol2data"), bp::arg("map2") ) );
        
        }
        { //::SireMol::AngleID::what
        
            typedef char const * ( ::SireMol::AngleID::*what_function_type)(  ) const;
            what_function_type what_function_value( &::SireMol::AngleID::what );
            
            AngleID_exposer.def( 
                "what"
                , what_function_value );
        
        }
        AngleID_exposer.staticmethod( "typeName" );
        AngleID_exposer.def( "__copy__", &__copy__);
        AngleID_exposer.def( "__deepcopy__", &__copy__);
        AngleID_exposer.def( "clone", &__copy__);
        AngleID_exposer.def( "__rlshift__", &__rlshift__QDataStream< ::SireMol::AngleID >,
                            bp::return_internal_reference<1, bp::with_custodian_and_ward<1,2> >() );
        AngleID_exposer.def( "__rrshift__", &__rrshift__QDataStream< ::SireMol::AngleID >,
                            bp::return_internal_reference<1, bp::with_custodian_and_ward<1,2> >() );
        AngleID_exposer.def( "__str__", &__str__< ::SireMol::AngleID > );
        AngleID_exposer.def( "__repr__", &__str__< ::SireMol::AngleID > );
        AngleID_exposer.def( "__hash__", &::SireMol::AngleID::hash );
    }

}
