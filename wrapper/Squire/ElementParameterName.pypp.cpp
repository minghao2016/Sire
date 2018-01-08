// This file has been generated by Py++.

// (C) Christopher Woods, GPL >= 2 License

#include "boost/python.hpp"
#include "ElementParameterName.pypp.hpp"

namespace bp = boost::python;

#include "SireBase/errors.h"

#include "SireBase/propertylist.h"

#include "SireError/errors.h"

#include "SireFF/errors.h"

#include "SireFF/ff.h"

#include "SireMol/atomelements.h"

#include "SireStream/datastream.h"

#include "SireStream/shareddatastream.h"

#include "qmpotential.h"

#include "qmprogram.h"

#include "qmpotential.h"

Squire::ElementParameterName __copy__(const Squire::ElementParameterName &other){ return Squire::ElementParameterName(other); }

const char* pvt_get_name(const Squire::ElementParameterName&){ return "Squire::ElementParameterName";}

void register_ElementParameterName_class(){

    { //::Squire::ElementParameterName
        typedef bp::class_< Squire::ElementParameterName > ElementParameterName_exposer_t;
        ElementParameterName_exposer_t ElementParameterName_exposer = ElementParameterName_exposer_t( "ElementParameterName", "This class provides the default name of the\nproperty that contains the element parameters", bp::init< >("") );
        bp::scope ElementParameterName_scope( ElementParameterName_exposer );
        { //::Squire::ElementParameterName::element
        
            typedef ::QString const & ( ::Squire::ElementParameterName::*element_function_type)(  ) const;
            element_function_type element_function_value( &::Squire::ElementParameterName::element );
            
            ElementParameterName_exposer.def( 
                "element"
                , element_function_value
                , bp::return_value_policy< bp::copy_const_reference >()
                , "" );
        
        }
        ElementParameterName_exposer.def( "__copy__", &__copy__);
        ElementParameterName_exposer.def( "__deepcopy__", &__copy__);
        ElementParameterName_exposer.def( "clone", &__copy__);
        ElementParameterName_exposer.def( "__str__", &pvt_get_name);
        ElementParameterName_exposer.def( "__repr__", &pvt_get_name);
    }

}
