// This file has been generated by Py++.

// (C) Christopher Woods, GPL >= 2 License


#include "boost/python.hpp"

#include "Helpers/clone_const_reference.hpp"

#include "Accumulator.pypp.hpp"

#include "Array2D_Matrix_.pypp.hpp"

#include "Array2D_NMatrix_.pypp.hpp"

#include "Array2D_SireMaths_AccumulatorPtr_.pypp.hpp"

#include "Array2D_Vector_.pypp.hpp"

#include "ArrayProperty_Vector_.pypp.hpp"

#include "Average.pypp.hpp"

#include "AverageAndStddev.pypp.hpp"

#include "AxisSet.pypp.hpp"

#include "BennettsFreeEnergyAverage.pypp.hpp"

#include "Complex.pypp.hpp"

#include "DistVector.pypp.hpp"

#include "ExpAverage.pypp.hpp"

#include "FreeEnergyAverage.pypp.hpp"

#include "Histogram.pypp.hpp"

#include "HistogramBin.pypp.hpp"

#include "HistogramValue.pypp.hpp"

#include "Line.pypp.hpp"

#include "Matrix.pypp.hpp"

#include "Median.pypp.hpp"

#include "MultiDouble.pypp.hpp"

#include "MultiFixed.pypp.hpp"

#include "MultiFloat.pypp.hpp"

#include "MultiInt.pypp.hpp"

#include "MultiUInt.pypp.hpp"

#include "MultiVector.pypp.hpp"

#include "N4Matrix.pypp.hpp"

#include "NMatrix.pypp.hpp"

#include "NVector.pypp.hpp"

#include "NullAccumulator.pypp.hpp"

#include "Plane.pypp.hpp"

#include "Quaternion.pypp.hpp"

#include "RanGenerator.pypp.hpp"

#include "RecordValues.pypp.hpp"

#include "Sphere.pypp.hpp"

#include "Torsion.pypp.hpp"

#include "Transform.pypp.hpp"

#include "Triangle.pypp.hpp"

#include "TrigArray2D_Matrix_.pypp.hpp"

#include "TrigArray2D_Vector_.pypp.hpp"

#include "TrigMatrix.pypp.hpp"

#include "Vector.pypp.hpp"

#include "VectorArrayProperty.pypp.hpp"

#include "VectorProperty.pypp.hpp"

#include "_Maths_free_functions.pypp.hpp"

namespace bp = boost::python;

#include "SireMaths_containers.h"

#include "SireMaths_registrars.h"

#include "SireMaths_properties.h"

#include "_Maths_global_variables.pyman.hpp"

BOOST_PYTHON_MODULE(_Maths){
    register_SireMaths_objects();

    register_SireMaths_containers();

    register_Array2D_SireMaths_AccumulatorPtr__class();

    register_Array2D_Matrix__class();

    register_Array2D_NMatrix__class();

    register_Array2D_Vector__class();

    register_ArrayProperty_Vector__class();

    register_TrigArray2D_Matrix__class();

    register_TrigArray2D_Vector__class();

    register_Accumulator_class();

    register_Average_class();

    register_AverageAndStddev_class();

    register_Vector_class();

    register_AxisSet_class();

    register_ExpAverage_class();

    register_FreeEnergyAverage_class();

    register_BennettsFreeEnergyAverage_class();

    register_Complex_class();

    register_DistVector_class();

    register_Histogram_class();

    register_HistogramBin_class();

    register_HistogramValue_class();

    register_Line_class();

    register_Matrix_class();

    register_Median_class();

    register_MultiDouble_class();

    register_MultiFixed_class();

    register_MultiFloat_class();

    register_MultiInt_class();

    register_MultiUInt_class();

    register_MultiVector_class();

    register_N4Matrix_class();

    register_NMatrix_class();

    register_NVector_class();

    register_NullAccumulator_class();

    register_Plane_class();

    register_Quaternion_class();

    register_RanGenerator_class();

    register_RecordValues_class();

    register_Sphere_class();

    register_Torsion_class();

    register_Transform_class();

    register_Triangle_class();

    register_TrigMatrix_class();

    register_VectorArrayProperty_class();

    register_VectorProperty_class();

    register_SireMaths_properties();

    register_man_global_variables();

    register_free_functions();
}

