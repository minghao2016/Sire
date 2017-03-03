/********************************************\
  *
  *  Sire - Molecular Simulation Framework
  *
  *  Copyright (C) 2017  Christopher Woods
  *
  *  This program is free software; you can redistribute it and/or modify
  *  it under the terms of the GNU General Public License as published by
  *  the Free Software Foundation; either version 2 of the License, or
  *  (at your option) any later version.
  *
  *  This program is distributed in the hope that it will be useful,
  *  but WITHOUT ANY WARRANTY; without even the implied warranty of
  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  *  GNU General Public License for more details.
  *
  *  You should have received a copy of the GNU General Public License
  *  along with this program; if not, write to the Free Software
  *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  *
  *  For full details of the license please see the COPYING file
  *  that should have come with this distribution.
  *
  *  You can contact the authors via the developer's mailing list
  *  at http://siremol.org
  *
\*********************************************/

#include <QFile>
#include <QTextStream>
#include <QHash>
#include <QElapsedTimer>
#include <QRegularExpression>

#include <tuple>

#include "amber2.h"

#include "SireMol/element.h"

#include "SireMol/atomcharges.h"
#include "SireMol/atommasses.h"
#include "SireMol/atomelements.h"
#include "SireMol/atomcoords.h"
#include "SireMol/atomvelocities.h"
#include "SireMol/connectivity.h"
#include "SireMol/selector.hpp"
#include "SireMol/mgname.h"

#include "SireMol/molecule.h"
#include "SireMol/moleditor.h"
#include "SireMol/reseditor.h"
#include "SireMol/atomeditor.h"
#include "SireMol/cgatomidx.h"
#include "SireMol/residuecutting.h"
#include "SireMol/atomcutting.h"
#include "SireMol/molidx.h"
#include "SireMol/atomidx.h"

#include "SireMol/amberparameters.h"

#include "SireCAS/trigfuncs.h"

#include "SireMM/ljparameter.h"
#include "SireMM/atomljs.h"
#include "SireMM/internalff.h"
#include "SireMM/cljnbpairs.h"
#include "SireMM/amberparams.h"

#include "SireVol/cartesian.h"
#include "SireVol/periodicbox.h"

#include "SireMaths/maths.h"
#include "SireUnits/units.h"

#include "SireBase/tempdir.h"
#include "SireBase/findexe.h"

#include "SireIO/errors.h"

#include "SireMove/internalmove.h"
#include "SireMove/flexibility.h"

#include "SireBase/parallel.h"
#include "SireBase/unittest.h"

#include "SireSystem/system.h"
#include "SireError/errors.h"

#include "SireStream/datastream.h"
#include "SireStream/shareddatastream.h"

#include <QDebug>

using namespace SireIO;
using namespace SireMM;
using namespace SireMol;
using namespace SireMove;
using namespace SireMaths;
using namespace SireCAS;
using namespace SireSystem;
using namespace SireUnits;
using namespace SireStream;
using namespace SireBase;

// The partial charges in the top file are not in electrons
static const double AMBERCHARGECONV = 18.2223;

static const double AMBER14COUL = 1.0 / 1.2 ;
static const double AMBER14LJ = 0.50 ;

/** Internal class used by AmberParm to hold format description */
class AmberFormat
{
public:
    AmberParm::FLAG_TYPE flag_type;
    int num_values;
    int field_width;
    int point_width;

    AmberFormat() : flag_type(AmberParm::UNKNOWN),
                    num_values(0), field_width(0), point_width(0)
    {}
    
    AmberFormat(AmberParm::FLAG_TYPE flag, int num,
                int field, int point=0)
            : flag_type(flag), num_values(num), field_width(field), point_width(point)
    {}
    
    AmberFormat(const QString &line)
    {
        QRegularExpression re("%FORMAT\\((\\d+)(\\w)(\\d+)\\.?(\\d+)?\\)");
        
        auto m = re.match(line);
        
        if (not m.hasMatch())
        {
            throw SireIO::parse_error( QObject::tr(
                    "Could not extract the format from the line '%1'. "
                    "Expected to read a line similar to '%FORMAT(5E16.8)'.")
                        .arg(line), CODELOC );
        }
        
        num_values = m.captured(1).toInt();
        field_width = m.captured(3).toInt();
        
        QString typ = m.captured(2).toLower();
        
        if (typ == "a")
            flag_type = AmberParm::STRING;
        else if (typ == "i")
            flag_type = AmberParm::INTEGER;
        else if (typ == "e")
            flag_type = AmberParm::FLOAT;
        else
            flag_type = AmberParm::UNKNOWN;
        
        if (not m.captured(4).isNull())
        {
            point_width = m.captured(4).toInt();
        }
    }
    
    ~AmberFormat()
    {}
    
    QString toString() const
    {
        switch(flag_type)
        {
            case AmberParm::STRING:
                return QString("AmberFormat( %1 x string[width = %2] )")
                            .arg(num_values).arg(field_width);
            case AmberParm::INTEGER:
                return QString("AmberFormat( %1 x integer[width = %2] )")
                            .arg(num_values).arg(field_width);
            case AmberParm::FLOAT:
                return QString("AmberFormat( %1 x float[width = %2, precision = %3] )")
                            .arg(num_values).arg(field_width).arg(point_width);
            default:
                return QString("AmberFormat( UNKNOWN )");
        }
    }
    
    /** The width of each field (number of columns) */
    int width() const
    {
        return field_width;
    }
    
    /** The maximum number of items per line */
    int numValues() const
    {
        return num_values;
    }
    
    /** The number of values after the decimal point for float values */
    int pointWidth() const
    {
        return point_width;
    }
    
    /** Return the number of values to read from the passed line */
    int numValues( const QString &line ) const
    {
        return qMin( num_values, line.length() / field_width );
    }
};

QVector<qint64> readIntData(const QVector<QString> &lines, AmberFormat format,
                            const QPair<qint64,qint64> &index,
                            double *total_score, QStringList *errors=0)
{
    QVector<qint64> data;

    double score = 0;

    //read in all of the lines...
    const int strt = index.first;
    const int end = index.first + index.second;
    
    const QString *l = lines.constData();
    
    data.reserve( (end-strt) * format.numValues() );
    
    for (int i=strt; i<end; ++i)
    {
        const QString &line = l[i];
        
        int nvalues = format.numValues(line);
        
        if (errors and nvalues < format.numValues() and i != end-1)
        {
            //one of the data lines has too little data
            errors->append( QObject::tr("Too few data values on line %1: "
                "Expected %2 values but only saw %3.")
                    .arg(i+1).arg(format.numValues()).arg(nvalues) );
        }
        
        double read_score = 0;
        
        for (int j=0; j<nvalues; ++j)
        {
            auto ref = line.midRef( j*format.width(), format.width() );
            
            if (not ref.isNull())
            {
                bool ok = true;
                qint64 value = ref.toLong(&ok);
                
                if (ok)
                {
                    data.append(value);
                    read_score += 1.0;
                }
                else if (errors)
                    errors->append( QObject::tr("Failed to convert the data "
                       "on line %1, column %2 (%3) into an integer!")
                            .arg(i+1).arg(j+1).arg(ref.toString()) );
            }
            else if (errors)
            {
                errors->append( QObject::tr("Failed to convert the data "
                       "on line %1, column %2 as the string is null!")
                            .arg(i+1).arg(j+1) );
            }
        }

        score += read_score / nvalues;
    }

    if (total_score)
        *total_score += score;

    return data;
}

QVector<double> readFloatData(const QVector<QString> &lines, AmberFormat format,
                              const QPair<qint64,qint64> &index,
                              double *total_score, QStringList *errors)
{
    QVector<double> data;

    double score = 0;

    //read in all of the lines...
    const int strt = index.first;
    const int end = index.first + index.second;
    
    data.reserve( (end-strt) * format.numValues() );
    
    const QString *l = lines.constData();
    
    for (int i=strt; i<end; ++i)
    {
        const QString &line = l[i];
        
        int nvalues = format.numValues(line);
        
        if (errors and nvalues < format.numValues() and i != end-1)
        {
            //one of the data lines has too little data
            errors->append( QObject::tr("Too few data values on line %1: "
                "Expected %2 values but only saw %3.")
                    .arg(i+1).arg(format.numValues()).arg(nvalues) );
        }
        
        double read_score = 0;
        
        for (int j=0; j<nvalues; ++j)
        {
            auto ref = line.midRef( j*format.width(), format.width() );
            
            if (not ref.isNull())
            {
                bool ok = true;
                double value = ref.toDouble(&ok);
                
                if (ok)
                {
                    data.append(value);
                    read_score += 1;
                }
                else if (errors)
                    errors->append( QObject::tr("Failed to convert the data "
                       "on line %1, column %2 (%3) into an integer!")
                            .arg(i+1).arg(j+1).arg(ref.toString()) );
            }
            else if (errors)
            {
                errors->append( QObject::tr("Failed to convert the data "
                       "on line %1, column %2 as the string is null!")
                            .arg(i+1).arg(j+1) );
            }
        }
        
        score += read_score / nvalues;
    }

    if (total_score)
        *total_score += score;

    return data;
}

QVector<QString> readStringData(const QVector<QString> &lines, AmberFormat format,
                                const QPair<qint64,qint64> &index,
                                double *total_score, QStringList *errors)
{
    QVector<QString> data;

    double score = 0;

    //read in all of the lines...
    const int strt = index.first;
    const int end = index.first + index.second;
    
    data.reserve( (end-strt) * format.numValues() );
    
    const QString *l = lines.constData();
    
    for (int i=strt; i<end; ++i)
    {
        const QString &line = l[i];
        
        int nvalues = format.numValues(line);
        
        if (errors and nvalues < format.numValues() and i != end-1)
        {
            //one of the data lines has too little data
            errors->append( QObject::tr("Too few data values on line %1: "
                "Expected %2 values but only saw %3.")
                    .arg(i+1).arg(format.numValues()).arg(nvalues) );
        }
        
        double read_score = 0;
        
        for (int j=0; j<nvalues; ++j)
        {
            auto ref = line.midRef( j*format.width(), format.width() );
            
            if (not ref.isNull())
            {
                data.append( ref.toString() );
                read_score += 1;
            }
            else if (errors)
            {
                errors->append( QObject::tr("Failed to convert the data "
                       "on line %1, column %2 as the string is null!")
                            .arg(i+1).arg(j+1) );
            }
        }
        
        score += read_score / nvalues;
    }

    if (total_score)
        *total_score += score;

    return data;
}

//////////////
////////////// Implementation of AmberRst
//////////////

static const RegisterMetaType<AmberRst> r_rst;

QDataStream SIREIO_EXPORT &operator<<(QDataStream &ds, const AmberRst &rst)
{
    writeHeader(ds, r_rst, 1);
    
    SharedDataStream sds(ds);
    
    sds << rst.ttle << rst.current_time
        << rst.coords << rst.vels
        << rst.box_dims << rst.box_angs
        << static_cast<const MoleculeParser&>(rst);
    
    return ds;
}

QDataStream SIREIO_EXPORT &operator>>(QDataStream &ds, AmberRst &rst)
{
    VersionID v = readHeader(ds, r_rst);
    
    if (v == 1)
    {
        SharedDataStream sds(ds);
        
        sds >> rst.ttle >> rst.current_time
            >> rst.coords >> rst.vels
            >> rst.box_dims >> rst.box_angs
            >> static_cast<MoleculeParser&>(rst);
    }
    else
        throw version_error(v, "1", r_rst, CODELOC);
    
    return ds;
}

Vector cubic_angs(90,90,90);

SIRE_REGISTER_PARSER( rst, AmberRst );
SIRE_REGISTER_PARSER( rst7, AmberRst );
SIRE_REGISTER_PARSER( crd, AmberRst );

/** Constructor */
AmberRst::AmberRst()
         : ConcreteProperty<AmberRst,MoleculeParser>(),
           current_time(0), box_dims(0), box_angs(cubic_angs)
{}

/** Private function used to read in the box data from the line with passed index */
void AmberRst::readBoxInfo(int boxidx)
{
    if (boxidx < 0 or boxidx >= lines().count())
        return;
    
    const QString &line = lines().constData()[boxidx];
    
    QStringList local_errors;
    
    if (line.length() >= 36)
    {
        //we can get the box dimensions
        bool x_ok = true;
        bool y_ok = true;
        bool z_ok = true;
        
        const double x = line.midRef(0,12).toDouble(&x_ok);
        const double y = line.midRef(12,12).toDouble(&y_ok);
        const double z = line.midRef(24,12).toDouble(&z_ok);
        
        if (not (x_ok and y_ok and z_ok))
        {
            local_errors.append( QObject::tr(
                "Cannot read the box dimensions "
                "there was a formatting issue with line number %1. (%2,%3,%4)")
                    .arg(boxidx+1).arg(x_ok).arg(y_ok).arg(z_ok) );
        }
        else
            box_dims = Vector(x,y,z);
    }

    if (line.length() >= 72)
    {
        //we can get the box dimensions
        bool x_ok = true;
        bool y_ok = true;
        bool z_ok = true;
        
        const double x = line.midRef(36,12).toDouble(&x_ok);
        const double y = line.midRef(48,12).toDouble(&y_ok);
        const double z = line.midRef(60,12).toDouble(&z_ok);
        
        if (not (x_ok and y_ok and z_ok))
        {
            local_errors.append( QObject::tr(
                "Cannot read the box angles "
                "there was a formatting issue with line number %1. (%2,%3,%4)")
                    .arg(boxidx+1).arg(x_ok).arg(y_ok).arg(z_ok) );
        }
        else
            box_angs = Vector(x,y,z);
    }
}

/** Construct by parsing the passed file */
AmberRst::AmberRst(const QString &filename, const PropertyMap &map)
         : ConcreteProperty<AmberRst,MoleculeParser>(filename, map),
           current_time(0), box_dims(0), box_angs(cubic_angs)
{
    if (lines().count() < 2)
        //there is nothing in the file
        return;
    
    double score = 0;
    
    // read in the title FORMAT(20A4)
    ttle = lines()[0].simplified();
    score += 1;
    
    // read in the number of atoms and time (two space-separated words)
    QStringList words = lines()[1].split(" ", QString::SkipEmptyParts);
    
    if (words.isEmpty())
        throw SireIO::parse_error( QObject::tr(
                "Could not read the number of atoms from the first word of the "
                "restart file '%1'. Please check that the file i ok.")
                    .arg(filename), CODELOC );
    
    bool ok = true;
    int natoms = words[0].toInt(&ok);
    
    if (not ok)
        throw SireIO::parse_error( QObject::tr(
                "Could not read the number of atoms from the first five columns of "
                "the restart file '%1'. Please check that the file is ok.")
                    .arg(filename), CODELOC );

    if (words.count() >= 2)
    {
        current_time = words[1].toDouble(&ok);
        
        if (not ok)
        {
            //we can't read the current time - this is annoying, but some
            //crd files don't contain this information - in this case,
            //the natoms information may also be misformed
            current_time = 0;
        }
    }
    score += 1;

    //now make sure that that the file is large enough!
    if (lines().count() < (2 + (natoms/2)))
    {
        throw SireIO::parse_error( QObject::tr(
                "There is a problem with this restart file. The number of atoms is "
                "%1, but the number of lines in the file is too small (%2). The number "
                "of lines needs to be at least %3 to give all of the information.")
                    .arg(natoms)
                    .arg(lines().count())
                    .arg(2 + (natoms/2)), CODELOC );
    }
 
    //now read in all of the coordinates
    QMutex mutex;
    coords = QVector<Vector>(natoms, Vector(0));
    Vector *coords_array = coords.data();
    QStringList global_errors;
    
    //get a pointer to the array of lines
    const QString *l = lines().constData();

    auto parse_coords = [&](int i, QStringList &errors)
    {
        //coordinates written as 6F12.7, two atoms per line
        const int linenum = 2 + (i / 2);
        const int column = (i % 2) * 36;

        //we have already validated that there are enough lines
        const QString &line = l[linenum];
        
        if (line.length() < column+36)
        {
            errors.append( QObject::tr(
                    "Cannot read the coordinates for the atom at index %1 as "
                    "the line at line number %2 is too short.")
                        .arg(i+1).arg(linenum+1) );
            
            return;
        }
        
        bool x_ok = true;
        bool y_ok = true;
        bool z_ok = true;
        
        const double x = line.midRef(column,12).toDouble(&x_ok);
        const double y = line.midRef(column+12,12).toDouble(&y_ok);
        const double z = line.midRef(column+24,12).toDouble(&z_ok);
        
        if (not (x_ok and y_ok and z_ok))
        {
            errors.append( QObject::tr(
                "Cannot read the coordinates for the atom at index %1 as "
                "there was a formatting issue with line number %2. (%3,%4,%5)")
                    .arg(i+1).arg(linenum+1).arg(x_ok).arg(y_ok).arg(z_ok) );
            return;
        }
        
        coords_array[i] = Vector(x,y,z);
    };
    
    if (usesParallel())
    {
        tbb::parallel_for( tbb::blocked_range<int>(0,natoms),
                           [&](tbb::blocked_range<int> r)
        {
            QStringList local_errors;
            
            for (int i=r.begin(); i<r.end(); ++i)
            {
                parse_coords(i, local_errors);
            }
            
            if (not local_errors.isEmpty())
            {
                QMutexLocker lkr(&mutex);
                global_errors += local_errors;
            }
        });
    }
    else
    {
        for (int i=0; i<natoms; ++i)
        {
            parse_coords(i, global_errors);
        }
    }
    
    if (not global_errors.isEmpty())
    {
        throw SireIO::parse_error( QObject::tr(
                "There were some problems reading in the coordinate data "
                "from the restart file %1.\n%2")
                    .arg(filename).arg(global_errors.join("\n")), CODELOC );
    }

    score += natoms/2;

    //now read in all of the velocities
    if (lines().count() < (2 + (natoms/2) + (natoms/2)))
    {
        //there are no velocities - see if there is periodic box information
        int boxidx = 2 + (natoms/2);
        
        if (boxidx < lines().count())
        {
            //there is - read in the box information
            this->readBoxInfo(boxidx);
            score += 1;
        }

        this->setScore(score);
        return;
    }

    vels = QVector<Vector>(natoms, Vector(0));
    Vector *vels_array = vels.data();

    auto parse_vels = [&](int i, QStringList &errors)
    {
        //coordinates written as 6F12.7, two atoms per line
        const int linenum = 2 + (i / 2) + (natoms / 2);
        const int column = (i % 2) * 36;

        //we have already validated that there are enough lines
        const QString &line = l[linenum];
        
        if (line.length() < column+36)
        {
            errors.append( QObject::tr(
                    "Cannot read the coordinates for the atom at index %1 as "
                    "the line at line number %2 is too short.")
                        .arg(i+1).arg(linenum+1) );
            
            return;
        }
        
        bool x_ok = true;
        bool y_ok = true;
        bool z_ok = true;
        
        const double x = line.midRef(column,12).toDouble(&x_ok);
        const double y = line.midRef(column+12,12).toDouble(&y_ok);
        const double z = line.midRef(column+24,12).toDouble(&z_ok);
        
        if (not (x_ok and y_ok and z_ok))
        {
            errors.append( QObject::tr(
                "Cannot read the velocities for the atom at index %1 as "
                "there was a formatting issue with line number %2. (%3,%4,%5)")
                    .arg(i+1).arg(linenum+1).arg(x_ok).arg(y_ok).arg(z_ok) );
            return;
        }
        
        // format is (units: Angstroms per 1/20.455 ps)
        vels_array[i] = Vector(x,y,z);
    };

    if (usesParallel())
    {
        tbb::parallel_for( tbb::blocked_range<int>(0,natoms),
                           [&](tbb::blocked_range<int> r)
        {
            QStringList local_errors;
            
            for (int i=r.begin(); i<r.end(); ++i)
            {
                parse_vels(i, local_errors);
            }
            
            if (not local_errors.isEmpty())
            {
                QMutexLocker lkr(&mutex);
                global_errors += local_errors;
            }
        });
    }
    else
    {
        for (int i=0; i<natoms; ++i)
        {
            parse_vels(i, global_errors);
        }
    }

    if (not global_errors.isEmpty())
    {
        throw SireIO::parse_error( QObject::tr(
                "There were some problems reading in the velocity data "
                "from the restart file %1.\n%2")
                    .arg(filename).arg(global_errors.join("\n")), CODELOC );
    }

    score += (natoms/2);

    //see if there is periodic box information
    int boxidx = 2 + (natoms/2) + (natoms/2);

    if (boxidx < lines().count())
    {
        //there is - read in the box information
        this->readBoxInfo(boxidx);
        score += 1;
    }

    this->setScore(score);
}

/** Construct by extracting the necessary data from the passed System */
AmberRst::AmberRst(const System &system, const PropertyMap &map)
         : ConcreteProperty<AmberRst,MoleculeParser>(),
           current_time(0), box_dims(0), box_angs(cubic_angs)
{
    //DO SOMETHING
}

/** Copy constructor */
AmberRst::AmberRst(const AmberRst &other)
         : ConcreteProperty<AmberRst,MoleculeParser>(other),
           ttle(other.ttle), current_time(other.current_time),
           coords(other.coords), vels(other.vels),
           box_dims(other.box_dims), box_angs(other.box_angs)
{}

/** Destructor */
AmberRst::~AmberRst()
{}

AmberRst& AmberRst::operator=(const AmberRst &other)
{
    if (this != &other)
    {
        ttle = other.ttle;
        current_time = other.current_time;
        coords = other.coords;
        vels = other.vels;
        box_dims = other.box_dims;
        box_angs = other.box_angs;
    
        MoleculeParser::operator=(other);
    }

    return *this;
}

bool AmberRst::operator==(const AmberRst &other) const
{
    return MoleculeParser::operator==(other);
}

bool AmberRst::operator!=(const AmberRst &other) const
{
    return MoleculeParser::operator!=(other);
}

const char* AmberRst::typeName()
{
    return QMetaType::typeName( qMetaTypeId<AmberRst>() );
}

const char* AmberRst::what() const
{
    return AmberRst::typeName();
}

QString AmberRst::toString() const
{
    if (coords.isEmpty())
    {
        return QObject::tr("AmberRst::null");
    }
    else if (vels.isEmpty())
    {
        return QObject::tr("AmberRst( title() = %1, nAtoms() = %2, hasVelocities() = false )")
                .arg(title()).arg(nAtoms());
    }
    else
    {
        return QObject::tr("AmberRst( title() = %1, nAtoms() = %2, hasVelocities() = true )")
                .arg(title()).arg(nAtoms());
    }
}

/** Parse from the passed file */
AmberRst AmberRst::parse(const QString &filename)
{
    return AmberRst(filename);
}

/** Internal function used to add the data from this parser into the passed System */
void AmberRst::addToSystem(System &system, const PropertyMap &map) const
{
    //first, we are going to work with the group of all molecules, which should
    //be called "all". We have to assume that the molecules are ordered in "all"
    //in the same order as they are in this restart file, with the data
    //in MolIdx/AtomIdx order (this should be the default for all parsers!)
    MoleculeGroup allmols = system[MGName("all")];

    const int nmols = allmols.nMolecules();

    QVector<int> atom_pointers(nmols+1, -1);
    
    int natoms = 0;
    
    for (int i=0; i<nmols; ++i)
    {
        atom_pointers[i] = natoms;
        const int nats = allmols[MolIdx(i)].data().info().nAtoms();
        natoms += nats;
    }
    
    atom_pointers[nmols] = natoms;
    
    if (natoms != this->nAtoms())
        throw SireIO::parse_error( QObject::tr(
                "Incompatibility between the files, as this restart file contains data "
                "for %1 atom(s), while the other file(s) have created a system with "
                "%2 atom(s)").arg(this->nAtoms()).arg(natoms), CODELOC );
    
    //next, copy the coordinates and optionally the velocities into the molecules
    QVector<Molecule> mols(nmols);
    Molecule *mols_array = mols.data();
    const Vector *coords_array = this->coordinates().constData();
    
    const PropertyName coords_property = map["coordinates"];
    
    if (this->hasVelocities())
    {
        const PropertyName vels_property = map["velocity"];
        const Vector *vels_array = this->velocities().constData();
    
        auto add_coords_and_vels = [&](int i)
        {
            const int atom_start_idx = atom_pointers.constData()[i];
            auto mol = allmols[MolIdx(i)].molecule();
            const auto molinfo = mol.data().info();
            
            //create space for the coordinates and velocities
            auto coords = QVector< QVector<Vector> >(molinfo.nCutGroups());
            auto vels = AtomVelocities(molinfo);

            for (int j=0; j<molinfo.nCutGroups(); ++j)
            {
                coords[j] = QVector<Vector>(molinfo.nAtoms(CGIdx(j)));
            }
            
            for (int j=0; j<mol.nAtoms(); ++j)
            {
                auto cgatomidx = molinfo.cgAtomIdx(AtomIdx(j));
                
                const int atom_idx = atom_start_idx + j;
                
                coords[cgatomidx.cutGroup()][cgatomidx.atom()] = coords_array[atom_idx];

                //velocity is Angstroms per 1/20.455 ps
                const auto vel_unit = (1.0 / 20.455) * angstrom / picosecond;
                
                const Vector &vel = vels_array[atom_idx];
                vels.set(cgatomidx, Velocity3D(vel.x() * vel_unit,
                                               vel.y() * vel_unit,
                                               vel.z() * vel_unit));
            }
            
            mols_array[i] = mol.edit()
                            .setProperty(vels_property, vels)
                            .setProperty(coords_property,
                                         AtomCoords(CoordGroupArray(coords)))
                            .commit();
        };
    
        if (usesParallel())
        {
            tbb::parallel_for( tbb::blocked_range<int>(0,nmols),
                               [&](tbb::blocked_range<int> r)
            {
                for (int i=r.begin(); i<r.end(); ++i)
                {
                    add_coords_and_vels(i);
                }
            });
        }
        else
        {
            for (int i=0; i<nmols; ++i)
            {
                add_coords_and_vels(i);
            }
        }
    }
    else
    {
        auto add_coords = [&](int i)
        {
            const int atom_start_idx = atom_pointers.constData()[i];
            auto mol = system[MolIdx(i)].molecule();
            const auto molinfo = mol.data().info();
            
            //create space for the coordinates
            auto coords = QVector< QVector<Vector> >(molinfo.nCutGroups());

            for (int j=0; j<molinfo.nCutGroups(); ++j)
            {
                coords[j] = QVector<Vector>(molinfo.nAtoms(CGIdx(j)));
            }
            
            for (int j=0; j<mol.nAtoms(); ++j)
            {
                auto cgatomidx = molinfo.cgAtomIdx(AtomIdx(j));
                
                const int atom_idx = atom_start_idx + j;
                
                coords[cgatomidx.cutGroup()][cgatomidx.atom()] = coords_array[atom_idx];
            }
            
            mols_array[i] = mol.edit()
                            .setProperty(coords_property,
                                         AtomCoords(CoordGroupArray(coords)))
                            .commit();
        };
    
        if (usesParallel())
        {
            tbb::parallel_for( tbb::blocked_range<int>(0,nmols),
                               [&](tbb::blocked_range<int> r)
            {
                for (int i=r.begin(); i<r.end(); ++i)
                {
                    add_coords(i);
                }
            });
        }
        else
        {
            for (int i=0; i<nmols; ++i)
            {
                add_coords(i);
            }
        }
    }
    
    system.update( Molecules(mols) );

    PropertyName space_property = map["space"];
    if (space_property.hasValue())
    {
        system.setProperty("space", space_property.value());
    }
    else if (box_dims != Vector(0) and space_property.hasSource())
    {
        if (box_angs != cubic_angs)
        {
            throw SireIO::parse_error( QObject::tr(
                    "Sire cannot currently support a non-cubic periodic box! %1")
                        .arg(box_angs.toString()), CODELOC );
        }
        
        
        system.setProperty( space_property.source(), SireVol::PeriodicBox(box_dims) );
    }
}

/** Return the title of the file */
QString AmberRst::title() const
{
    return ttle;
}

/** Return the current time of the simulation from which this restart
    file was written */
double AmberRst::time() const
{
    return current_time;
}

/** Return the number of atoms whose coordinates are contained in this restart file */
int AmberRst::nAtoms() const
{
    return coords.count();
}

/** Return whether or not this restart file also provides velocities */
bool AmberRst::hasVelocities() const
{
    return not vels.isEmpty();
}

/** Return the parsed coordinate data */
QVector<SireMaths::Vector> AmberRst::coordinates() const
{
    return coords;
}

/** Return the parsed coordinate data */
QVector<SireMaths::Vector> AmberRst::velocities() const
{
    return vels;
}

/** Return the parsed box dimensions */
SireMaths::Vector AmberRst::boxDimensions() const
{
    return box_dims;
}

/** Return the parsed box angles */
SireMaths::Vector AmberRst::boxAngles() const
{
    return box_angs;
}

//////////////
////////////// Implementation of AmberParm
//////////////

static const RegisterMetaType<AmberParm> r_parm;

SIRE_REGISTER_PARSER( prm, AmberParm );
SIRE_REGISTER_PARSER( prm7, AmberParm );
SIRE_REGISTER_PARSER( top, AmberParm );

/** Serialise to a binary datastream */
QDataStream SIREIO_EXPORT &operator<<(QDataStream &ds, const AmberParm &parm)
{
    writeHeader(ds, r_parm, 1);
    
    SharedDataStream sds(ds);
    
    sds << parm.flag_to_line
        << parm.int_data << parm.float_data << parm.string_data
        << static_cast<const MoleculeParser&>(parm);
    
    return ds;
}

/** Function called to rebuild the excluded atom lists */
void AmberParm::rebuildExcludedAtoms()
{
    const int nmols = this->nMolecules();
    const int natoms = this->nAtoms();
    
    if (nmols <= 0 or natoms <= 0)
        return;
    
    //number of excluded atoms per atom
    const auto nnb_per_atom = this->intData("NUMBER_EXCLUDED_ATOMS");
    //list of all excluded atoms
    const auto inb = this->intData("EXCLUDED_ATOMS_LIST");
    
    if (nnb_per_atom.count() != natoms)
    {
        throw SireIO::parse_error( QObject::tr(
            "The number of excluded atoms per atom array is not equal to the "
            "number of atoms! Should be %1, but is %2!")
                .arg(natoms).arg(nnb_per_atom.count()), CODELOC );
    }

    //get the number of atoms in each molecule
    const auto nats_per_mol = this->intData("ATOMS_PER_MOLECULE");
    SireBase::assert_equal( nats_per_mol.count(), nmols, CODELOC );
    
    excl_atoms = QVector< QVector< QVector<int> > >(nmols);
    int atomidx = 0;
    int nnbidx = 0;
    
    for (int i=0; i<nmols; ++i)
    {
        const int nats = nats_per_mol[i];
    
        QVector< QVector<int> > excl(nats);
        
        for (int j=0; j<nats; ++j)
        {
            const int nnb = nnb_per_atom[atomidx];
        
            QVector<int> ex(nnb);
            
            for (int k=0; k<nnb; ++k)
            {
                ex[k] = inb[nnbidx];
                nnbidx += 1;
            }
            
            atomidx += 1;
            
            excl[j] = ex;
        }
        
        excl_atoms[i] = excl;
    }
}

/** Internal function used to return the start index for the bonds of each
    molecule, and the number of bonds in each molecule */
QVector< QVector<int> > indexBonds(const QVector<qint64> &bonds,
                                   const QVector<int> &atom_to_mol,
                                   const int nmols)
{
    QVector< QVector<int> > molbonds(nmols);

    for (int i=0; i<bonds.count(); i+=3)
    {
        //format is atom0-atom1-parameter
        int mol0 = atom_to_mol[bonds[i]/3];   //divide by three as index is
        int mol1 = atom_to_mol[bonds[i+1]/3]; //into the coordinate array
        
        if (mol0 != mol1)
            throw SireIO::parse_error( QObject::tr(
                    "Something went wrong as there is a bond between two different "
                    "molecules!"), CODELOC );
        
        molbonds[mol0].append(i);
    }
    
    return molbonds;
}

/** Internal function used to return the start index for the angles of each
    molecule, and the number of angles in each molecule */
QVector< QVector<int> > indexAngles(const QVector<qint64> &angs,
                                    const QVector<int> &atom_to_mol,
                                    const int nmols)
{
    QVector< QVector<int> > molangs(nmols);
    
    for (int i=0; i<angs.count(); i+=4)
    {
        //format is atom0-atom1-atom2-parameter
        int mol0 = atom_to_mol[angs[i]/3];   //divide by three as index is
        int mol1 = atom_to_mol[angs[i+1]/3]; //into the coordinate array
        int mol2 = atom_to_mol[angs[i+2]/3];
        
        if (mol0 != mol1 or mol0 != mol2)
            throw SireIO::parse_error( QObject::tr(
                    "Something went wrong as there is a angle between more than one different "
                    "molecule!"), CODELOC );
        
        molangs[mol0].append(i);
    }

    return molangs;
}

/** Internal function used to return the start index for the dihedrals of each
    molecule, and the number of dihedrals in each molecule */
QVector< QVector<int> > indexDihedrals(const QVector<qint64> &dihs,
                                       const QVector<int> &atom_to_mol,
                                       const int nmols)
{
    QVector< QVector<int> > moldihs(nmols);

    for (int i=0; i<dihs.count(); i+=5)
    {
        //format is atom0-atom1-atom2-atom3-parameter
        int mol0 = atom_to_mol[dihs[i]/3];   //divide by three as index is
        int mol1 = atom_to_mol[dihs[i+1]/3]; //into the coordinate array
        int mol2 = atom_to_mol[ std::abs(dihs[i+2]/3) ];
        int mol3 = atom_to_mol[ std::abs(dihs[i+3]/3) ];
        
        if (mol0 != mol1 or mol0 != mol2 or mol0 != mol3)
            throw SireIO::parse_error( QObject::tr(
                    "Something went wrong as there is a dihedral between more than one different "
                    "molecule!"), CODELOC );

        moldihs[mol0].append(i);
    }
    
    return moldihs;
}

/** Function called to rebuild the Bond Angle and Dihedral indicies */
void AmberParm::rebuildBADIndicies()
{
    const int nmols = this->nMolecules();
    const int natoms = this->nAtoms();
    
    if (nmols <= 0 or natoms <= 0)
        return;

    //get the lookup table to go from atom index to molecule index
    const auto atom_to_mol = this->getAtomIndexToMolIndex();
    
    //now index the connectivity - find the start index and number of bonds/angles/dihedrals
    //for each molecule
    if (usesParallel())
    {
        tbb::parallel_invoke
        (
            [&]()
            {
                bonds_inc_h = indexBonds(this->intData("BONDS_INC_HYDROGEN"),
                                         atom_to_mol, nmols);
            },
            [&]()
            {
                bonds_exc_h = indexBonds(this->intData("BONDS_WITHOUT_HYDROGEN"),
                                         atom_to_mol, nmols);
            },
            [&]()
            {
                angs_inc_h = indexAngles(this->intData("ANGLES_INC_HYDROGEN"),
                                         atom_to_mol, nmols);
            },
            [&]()
            {
                angs_exc_h = indexAngles(this->intData("ANGLES_WITHOUT_HYDROGEN"),
                                         atom_to_mol, nmols);
            },
            [&]()
            {
                dihs_inc_h = indexDihedrals(this->intData("DIHEDRALS_INC_HYDROGEN"),
                                            atom_to_mol, nmols);
            },
            [&]()
            {
                dihs_exc_h = indexDihedrals(this->intData("DIHEDRALS_WITHOUT_HYDROGEN"),
                                            atom_to_mol, nmols);
            }
        );
    }
    else
    {
        bonds_inc_h = indexBonds(this->intData("BONDS_INC_HYDROGEN"),
                                 atom_to_mol, nmols);
        bonds_exc_h = indexBonds(this->intData("BONDS_WITHOUT_HYDROGEN"),
                                 atom_to_mol, nmols);
        angs_inc_h = indexAngles(this->intData("ANGLES_INC_HYDROGEN"),
                                 atom_to_mol, nmols);
        angs_exc_h = indexAngles(this->intData("ANGLES_WITHOUT_HYDROGEN"),
                                 atom_to_mol, nmols);
        dihs_inc_h = indexDihedrals(this->intData("DIHEDRALS_INC_HYDROGEN"),
                                    atom_to_mol, nmols);
        dihs_exc_h = indexDihedrals(this->intData("DIHEDRALS_WITHOUT_HYDROGEN"),
                                    atom_to_mol, nmols);
    }
}

/** Function called to rebuild all of the LJ parameters */
void AmberParm::rebuildLJParameters()
{
    lj_data.clear();

    if (pointers.count() < 20)
        return;

    const int ntypes = pointers[1];  //number of distinct atom types
    
    if (ntypes <= 0)
        return;

    const int nphb = pointers[19];   //number of distinct 10-12 hydrogen bond pair types

    lj_data = QVector<LJParameter>(ntypes);
    auto lj_data_array = lj_data.data();
    
    auto acoeffs = float_data.value("LENNARD_JONES_ACOEF");
    auto bcoeffs = float_data.value("LENNARD_JONES_BCOEF");
    
    auto hbond_acoeffs = float_data.value("HBOND_ACOEF");
    auto hbond_bcoeffs = float_data.value("HBOND_BCOEF");
    
    auto nb_parm_index = int_data.value("NONBONDED_PARM_INDEX");
    
    if (acoeffs.count() != bcoeffs.count() or
        acoeffs.count() != (ntypes*(ntypes+1))/2)
    {
        throw SireIO::parse_error( QObject::tr(
                "Incorrect number of LJ coefficients for the number of specified "
                "atom types! Should be %1 for %2 types, but actually have "
                "%3 LJ A-coefficients, and %4 LJ B-coefficients")
                    .arg((ntypes*(ntypes+1))/2)
                    .arg(ntypes)
                    .arg(acoeffs.count())
                    .arg(bcoeffs.count()), CODELOC );
    }
    
    if (nb_parm_index.count() != ntypes*ntypes)
    {
        throw SireIO::parse_error( QObject::tr(
                "Incorrect number of non-bonded parameter indicies. There should "
                "be %1 indicies for %2 types, but actually have %3.")
                    .arg(ntypes*ntypes)
                    .arg(ntypes)
                    .arg(nb_parm_index.count()), CODELOC );
    }

    if (hbond_acoeffs.count() != nphb or
        hbond_bcoeffs.count() != nphb)
    {
        throw SireIO::parse_error( QObject::tr(
                "Incorrect number of HBond parameters. There should be "
                "%1 such parameters, but the number of HBond A coefficients is "
                "%2, and the number of B coefficients is %3.")
                    .arg(nphb)
                    .arg(hbond_acoeffs.count())
                    .arg(hbond_bcoeffs.count()), CODELOC );
    }

    auto acoeffs_array = acoeffs.constData();
    auto bcoeffs_array = bcoeffs.constData();
    
    auto build_lj = [&](int i)
    {
        //amber stores the A and B coefficients as the product of all
        //possible combinations. We need to find the values from the
        // LJ_i * LJ_i values
        int idx = nb_parm_index[ ntypes * i + i  ];
        
        if (idx < 0)
        {
            //this is a 10-12 parameter
            throw SireError::unsupported( QObject::tr(
                    "Sire does not yet support Amber Parm files that "
                    "use 10-12 HBond parameters."), CODELOC );
        }
        else
        {
            double acoeff = acoeffs_array[ idx - 1 ];
            double bcoeff = bcoeffs_array[ idx - 1 ];

            double sigma = 0;
            double epsilon = 0;

            // numeric imprecision means that any parameter with acoeff less
            // than 1e-10 is really equal to 0
            if (acoeff > 1e-10)
            {
                // convert a_coeff & b_coeff into angstroms and kcal/mol-1
                sigma = std::pow( acoeff / bcoeff ,  1/6. );
                epsilon = pow_2( bcoeff ) / (4*acoeff);
            }
            
            lj_data_array[i] = LJParameter(sigma * angstrom, epsilon * kcal_per_mol);
        }
    };
    
    if (usesParallel())
    {
        tbb::parallel_for( tbb::blocked_range<int>(0,ntypes),
                           [&](tbb::blocked_range<int> r)
        {
            for (int i=r.begin(); i<r.end(); ++i)
            {
                build_lj(i);
            }
        });
    }
    else
    {
        for (int i=0; i<ntypes; ++i)
        {
            build_lj(i);
        }
    }
}

/** Function called after loading the AmberParm from a binary stream
    to populate all of the calculated member data */
void AmberParm::rebuildAfterReload()
{
    pointers = this->intData("POINTERS");
    
    if (pointers.isEmpty())
    {
        this->operator=( AmberParm() );
    }
    else if (pointers.count() < 31)
    {
        throw SireIO::parse_error( QObject::tr(
            "There was no, or an insufficient 'POINTERS' section in the file! (%1)")
                .arg(pointers.count()), CODELOC );
    }

    if (usesParallel())
    {
        tbb::parallel_invoke
        (
            //now we have to build the LJ parameters (Amber stores them weirdly!)
            [&](){ this->rebuildLJParameters(); },
            //now we have to build the lookup indicies for the bonds, angles and dihedrals
            [&](){ this->rebuildBADIndicies(); },
            //now we have to build the excluded atom lists
            [&](){ this->rebuildExcludedAtoms(); }
        );
    }
    else
    {
        this->rebuildLJParameters();
        this->rebuildBADIndicies();
        this->rebuildExcludedAtoms();
    }
}

/** Read from a binary datastream */
QDataStream SIREIO_EXPORT &operator>>(QDataStream &ds, AmberParm &parm)
{
    VersionID v = readHeader(ds, r_parm);
    
    if (v == 1)
    {
        SharedDataStream sds(ds);
        
        parm = AmberParm();
        
        sds >> parm.flag_to_line
            >> parm.int_data >> parm.float_data >> parm.string_data
            >> static_cast<MoleculeParser&>(parm);
        
        parm.rebuildAfterReload();
    }
    else
        throw version_error(v, "1", r_parm, CODELOC);
    
    return ds;
}

/** Constructor */
AmberParm::AmberParm() : ConcreteProperty<AmberParm,MoleculeParser>()
{
    for (int i=0; i<32; ++i)
    {
        pointers.append(0);
    }
}

AmberParm::FLAG_TYPE flagType(const QStringList &lines, const QPair<qint64,qint64> &index)
{
    AmberFormat f(lines[index.first+1]);
    return f.flag_type;
}

/** Return the flag type for the data associated with the passed flag.
    This returns UNKNOWN if this is not known */
AmberParm::FLAG_TYPE AmberParm::flagType(const QString &flag) const
{
    if (flag_to_line.contains(flag))
    {
        auto index = flag_to_line.value(flag);
    
        if (index.first > 0)
            return AmberFormat(lines()[index.first-1]).flag_type;
    }

    return AmberParm::UNKNOWN;
}

/** Return the integer data for the passed flag. This returns an empty
    list if there is no data associated with this flag. This raises
    an invalid_cast error if data exists, but it is the wrong type */
QVector<qint64> AmberParm::intData(const QString &flag) const
{
    auto it = int_data.constFind(flag);
    
    if (it != int_data.constEnd())
    {
        return it.value();
    }
    
    if (flag_to_line.contains(flag))
    {
        if (float_data.contains(flag))
            throw SireError::invalid_cast( QObject::tr(
                "Cannot convert the float data for flag '%1' to integer data!")
                    .arg(flag), CODELOC );
        else if (string_data.contains(flag))
            throw SireError::invalid_cast( QObject::tr(
                "Cannot convert the string data for flag '%1' to integer data!")
                    .arg(flag), CODELOC );
        else
            throw SireError::invalid_cast( QObject::tr(
                "Cannot convert the data for flag '%1' to integer data!")
                    .arg(flag), CODELOC );
    }

    return QVector<qint64>();
}

/** Return the float data for the passed flag. This returns an empty
    list if there is no data associated with this flag. This raises
    an invalid_cast error if data exists, but it is the wrong type */
QVector<double> AmberParm::floatData(const QString &flag) const
{
    auto it = float_data.constFind(flag);
    
    if (it != float_data.constEnd())
    {
        return it.value();
    }
    
    if (flag_to_line.contains(flag))
    {
        if (int_data.contains(flag))
            throw SireError::invalid_cast( QObject::tr(
                "Cannot convert the integer data for flag '%1' to float data!")
                    .arg(flag), CODELOC );
        else if (string_data.contains(flag))
            throw SireError::invalid_cast( QObject::tr(
                "Cannot convert the string data for flag '%1' to float data!")
                    .arg(flag), CODELOC );
        else
            throw SireError::invalid_cast( QObject::tr(
                "Cannot convert the data for flag '%1' to float data!")
                    .arg(flag), CODELOC );
    }

    return QVector<double>();
}

/** Return the string data for the passed flag. This returns an empty
    list if there is no data associated with this flag. This raises
    an invalid_cast error if data exists, but it is the wrong type */
QVector<QString> AmberParm::stringData(const QString &flag) const
{
    auto it = string_data.constFind(flag);
    
    if (it != string_data.constEnd())
    {
        return it.value();
    }
    
    if (flag_to_line.contains(flag))
    {
        if (float_data.contains(flag))
            throw SireError::invalid_cast( QObject::tr(
                "Cannot convert the float data for flag '%1' to string data!")
                    .arg(flag), CODELOC );
        else if (int_data.contains(flag))
            throw SireError::invalid_cast( QObject::tr(
                "Cannot convert the integer data for flag '%1' to string data!")
                    .arg(flag), CODELOC );
        else
            throw SireError::invalid_cast( QObject::tr(
                "Cannot convert the data for flag '%1' to string data!")
                    .arg(flag), CODELOC );
    }

    return QVector<QString>();
}

/** This function finds all atoms that are bonded to the atom at index 'atom_idx'
    (which is in molecule with index 'mol_idx', populating the hashe
    'atom_to_mol' (the molecule containing the passed atom). This uses the bonding information
    in 'bonded_atoms', which is the list of all atoms that are bonded to each atom */
static void findBondedAtoms(int atom_idx, int mol_idx,
                            const QHash<int, int> &bonded_atoms,
                            QHash<int, int> &atom_to_mol,
                            QSet<int> &atoms_in_mol)
{
    for ( auto bonded_atom : bonded_atoms.values(atom_idx) )
    {
        if (not atoms_in_mol.contains(bonded_atom))
        {
            //we have not walked along this atom before
            atom_to_mol[bonded_atom] = mol_idx;
            atoms_in_mol.insert(bonded_atom);
            findBondedAtoms( bonded_atom, mol_idx, bonded_atoms, atom_to_mol, atoms_in_mol );
        }
    }
}

/** This function uses the bond information in 'bonds_inc_h' and 'bonds_exc_h'
    to divide the passed atoms into molecules. This returns an array of
    the number of atoms in each molecule (same format as ATOMS_PER_MOLECULE) */
static QVector<qint64> discoverMolecules(const QVector<qint64> &bonds_inc_h,
                                         const QVector<qint64> &bonds_exc_h,
                                         int natoms)
{
    //first, create a hash showing which atoms are bonded to each other
    
    //NOTE: the atom numbers in the following arrays that describe bonds
    //are coordinate array indexes for runtime speed. The true atom number
    //equals the absolute value of the number divided by three, plus one.
    //
    //%FORMAT(10I8)  (IBH(i),JBH(i),ICBH(i), i=1,NBONH)
    //  IBH    : atom involved in bond "i", bond contains hydrogen
    //  JBH    : atom involved in bond "i", bond contains hydrogen
    //  ICBH   : index into parameter arrays RK and REQ
    QHash<int, int> bonded_atoms;

    for ( int j = 0 ; j < bonds_exc_h.count() ; j = j + 3 )
    {
        int atom0 = bonds_exc_h[ j ] / 3 + 1;
        int atom1 = bonds_exc_h[ j + 1 ] / 3 + 1;
        bonded_atoms.insertMulti(atom0, atom1);
        bonded_atoms.insertMulti(atom1, atom0);
    }

    for ( int j = 0 ; j < bonds_inc_h.count() ; j = j + 3 )
    {
        int atom0 = bonds_inc_h[ j ] / 3 + 1 ;
        int atom1 = bonds_inc_h[ j + 1 ] / 3 + 1 ;
        bonded_atoms.insertMulti(atom0, atom1);
        bonded_atoms.insertMulti(atom1, atom0);
    }

    // Then recursively walk along each atom to find all the atoms that
    // are in the same molecule
    int nmols = 0;
    
    QHash<int, int> atom_to_mol;

    QList<qint64> atoms_per_mol;

    for ( int i = 1 ; i <= natoms ; i++ )
    {
        if (not atom_to_mol.contains(i))
        {
            QSet<int> atoms_in_mol;
        
            nmols += 1;
            atom_to_mol[ i ] = nmols;
            atoms_in_mol.insert(i);

            // Recursive walk
            findBondedAtoms(i, nmols, bonded_atoms, atom_to_mol, atoms_in_mol);
            
            //this has now found all of the atoms in this molecule. Add the
            //number of atoms in the molecule to atoms_per_mol
            atoms_per_mol.append( atoms_in_mol.count() );
        }
    }
    
    return atoms_per_mol.toVector();
}

/** Process all of the flags */
double AmberParm::processAllFlags()
{
    QMutex int_mutex, float_mutex, string_mutex;
    
    double score = 0;
    
    const QStringList flags = flag_to_line.keys();
    
    QStringList global_errors;
    
    auto process_flag = [&](int i, QStringList &errors, double &scr)
    {
        const QString &flag = flags[i];
        const QPair<qint64,qint64> index = flag_to_line.value(flag);
        
        //the format for the data is on the preceeding line
        const AmberFormat format(lines()[index.first-1]);
        
        switch(format.flag_type)
        {
            case INTEGER:
            {
                QVector<qint64> data = readIntData(lines(), format, index,
                                                   &scr, &errors);
                QMutexLocker lkr(&int_mutex);
                int_data.insert(flag, data);
                break;
            }
            case FLOAT:
            {
                QVector<double> data = readFloatData(lines(), format, index,
                                                     &scr, &errors);
                QMutexLocker lkr(&float_mutex);
                float_data.insert(flag, data);
                break;
            }
            case STRING:
            {
                QVector<QString> data = readStringData(lines(), format, index,
                                                       &scr, &errors);
                QMutexLocker lkr(&string_mutex);
                string_data.insert(flag, data);
                break;
            }
            default:
                break;
        }
    };
    
    if (usesParallel())
    {
        tbb::parallel_for( tbb::blocked_range<int>(0,flags.count()),
                           [&](tbb::blocked_range<int> r)
        {
            QStringList local_errors;
            double local_score = 0;
        
            for (int i=r.begin(); i<r.end(); ++i)
            {
                process_flag(i, local_errors, local_score);
            }

            QMutexLocker lkr(&int_mutex);
            score += local_score;
            
            if (not local_errors.isEmpty())
            {
                global_errors += local_errors;
            }
        });
    }
    else
    {
        for (int i=0; i<flags.count(); ++i)
        {
            process_flag(i, global_errors, score);
        }
    }
    
    if (not global_errors.isEmpty())
    {
        throw SireIO::parse_error( QObject::tr(
            "There were errors parsing the file:%1")
                .arg(global_errors.join("\n")), CODELOC );
    }
    
    //now have to get the general pointers array
    pointers = this->intData("POINTERS");
    
    if (pointers.isEmpty())
    {
        this->operator=( AmberParm() );
        return 0;
    }
    else if (pointers.count() < 31)
    {
        throw SireIO::parse_error( QObject::tr(
            "There was no, or an insufficient 'POINTERS' section in the file! (%1)")
                .arg(pointers.count()), CODELOC );
    }
    
    //now we have to work out how the atoms divide into molecules
    const auto atoms_per_mol = this->intData("ATOMS_PER_MOLECULE");
    
    if (atoms_per_mol.count() == 0)
    {
        auto atoms_per_mol = discoverMolecules(int_data.value("BONDS_INC_HYDROGEN"),
                                               int_data.value("BONDS_WITHOUT_HYDROGEN"),
                                               nAtoms());
 
        if (atoms_per_mol.count() == 0)
        {
            atoms_per_mol = QVector<qint64>(1, nAtoms());
        }
 
        int_data.insert("ATOMS_PER_MOLECULE", atoms_per_mol);
    }

    //build everything that can be derived from this information
    this->rebuildAfterReload();
    
    return score;
}

/** Construct by reading from the file called 'filename' */
AmberParm::AmberParm(const QString &filename, const PropertyMap &map)
          : ConcreteProperty<AmberParm,MoleculeParser>(filename, map)
{
    double score = 0;

    // as we are reading, look out for any FLAGs, so that we
    // can record their locations
    QString last_flag = QString::null;

    const int nlines = lines().count();
    const QString *lines_array = lines().constData();

    for (int i=0; i<nlines; ++i)
    {
        const QString &line = lines_array[i];

        if (line.length() > 0 and line[0] == '%')
        {
            //this is a control line
            if (line.startsWith("%FLAG"))
            {
                //this is a new flag - close any open old flag
                if (not last_flag.isNull())
                {
                    if (flag_to_line.contains(last_flag))
                    {
                        flag_to_line[last_flag].second = i - flag_to_line[last_flag].first;
                    }
                    
                    last_flag = QString::null;
                }

                //find the new flag
                QStringList words = line.split(" ", QString::SkipEmptyParts);
                
                QString flag = words[1];
                
                if (flag_to_line.contains(flag))
                    throw SireError::file_error( QObject::tr(
                        "The file '%1' does not look like a valid Amber Parm7 file, "
                        "as the FLAG '%2' is duplicated! (on lines %3 and %4)")
                            .arg(filename).arg(flag)
                            .arg(flag_to_line[flag].first)
                            .arg(i), CODELOC );
                
                //skip the FLAG line, and the FORMAT line that must come immediately after
                flag_to_line.insert( flag, QPair<qint64,qint64>(i+2,-1) );
                last_flag = flag;
                score += 1;
            }
        }
    }
    
    if (not last_flag.isNull())
    {
        flag_to_line[last_flag].second = nlines - flag_to_line[last_flag].first;
        last_flag = QString::null;
    }

    //now process all of the flag data
    score += this->processAllFlags();

    //finally, make sure that we have been constructed sane
    this->assertSane();
    
    this->setScore(score);
}

/** Construct by converting from the passed system, using the passed property
    map to find the right properties */
AmberParm::AmberParm(const System &system, const PropertyMap &map)
{
    this->operator=(AmberParm());
}

/** Copy constructor */
AmberParm::AmberParm(const AmberParm &other)
           : ConcreteProperty<AmberParm,MoleculeParser>(other),
             flag_to_line(other.flag_to_line),
             int_data(other.int_data), float_data(other.float_data),
             string_data(other.string_data), lj_data(other.lj_data),
             bonds_inc_h(other.bonds_inc_h), bonds_exc_h(other.bonds_exc_h),
             angs_inc_h(other.angs_inc_h), angs_exc_h(other.angs_exc_h),
             dihs_inc_h(other.dihs_inc_h), dihs_exc_h(other.dihs_exc_h),
             excl_atoms(other.excl_atoms), pointers(other.pointers)
{}

/** Destructor */
AmberParm::~AmberParm()
{}

/** Copy assignment operator */
AmberParm& AmberParm::operator=(const AmberParm &other)
{
    if (this != &other)
    {
        flag_to_line = other.flag_to_line;
        int_data = other.int_data;
        float_data = other.float_data;
        string_data = other.string_data;
        lj_data = other.lj_data;
        bonds_inc_h = other.bonds_inc_h;
        bonds_exc_h = other.bonds_exc_h;
        angs_inc_h = other.angs_inc_h;
        angs_exc_h = other.angs_exc_h;
        dihs_inc_h = other.dihs_inc_h;
        dihs_exc_h = other.dihs_exc_h;
        excl_atoms = other.excl_atoms;
        pointers = other.pointers;
        MoleculeParser::operator=(other);
    }
    
    return *this;
}

/** Comparison operator */
bool AmberParm::operator==(const AmberParm &other) const
{
    return MoleculeParser::operator==(other);
}

/** Comparison operator */
bool AmberParm::operator!=(const AmberParm &other) const
{
    return not operator==(other);
}

const char* AmberParm::typeName()
{
    return QMetaType::typeName( qMetaTypeId<AmberParm>() );
}

const char* AmberParm::what() const
{
    return AmberParm::typeName();
}

/** Return a string representation of this object */
QString AmberParm::toString() const
{
    if (lines().isEmpty())
        return QObject::tr("AmberParm::null");
    else
    {
        return QObject::tr("AmberParm( title() = '%1', nMolecules() = %2, "
                           "nResidues() = %3, nAtoms() = %4 )")
                    .arg(title()).arg(nMolecules()).arg(nResidues()).arg(nAtoms());
    }
}

/** Return the title of the parameter file */
QString AmberParm::title() const
{
    return QStringList( string_data.value("TITLE").toList() ).join("").simplified();
}

/** Return the total number of atoms in the file */
int AmberParm::nAtoms() const
{
    if (pointers.count() > 0)
        return pointers[0];
    else
        return 0;
}

/** Return the total number of atoms in the ith molecule in the file */
int AmberParm::nAtoms(int idx) const
{
    const QVector< QPair<int,int> > mol_idxs = this->moleculeIndicies();
    idx = Index(idx).map(mol_idxs.count());
    return mol_idxs[idx].second;
}

/** Return the number of distinct atom types */
int AmberParm::nTypes() const
{
    if (pointers.count() > 1)
        return pointers[1];
    else
        return 0;
}

/** Return the total number of bonds */
int AmberParm::nBonds() const
{
    return nBondsWithHydrogen() + nBondsNoHydrogen();
}

/** Return the number of bonds including hydrogen */
int AmberParm::nBondsWithHydrogen() const
{
    if (pointers.count() > 2)
        return pointers[2];
    else
        return 0;
}

/** Return the number of bonds no containing hydrogen */
int AmberParm::nBondsNoHydrogen() const
{
    if (pointers.count() > 3)
        return pointers[3];
    else
        return 0;
}

/** Return the number of angles */
int AmberParm::nAngles() const
{
    return nAnglesWithHydrogen() + nAnglesNoHydrogen();
}

/** Return the number of angles containing hydrogen */
int AmberParm::nAnglesWithHydrogen() const
{
    if (pointers.count() > 4)
        return pointers[4];
    else
        return 0;
}

/** Return the number of angles without hydrogen */
int AmberParm::nAnglesNoHydrogen() const
{
    if (pointers.count() > 5)
        return pointers[5];
    else
        return 0;
}

/** Return the number of dihedrals */
int AmberParm::nDihedrals() const
{
    return nDihedralsWithHydrogen() + nDihedralsNoHydrogen();
}

/** Return the number of dihedrals containing hydrogen */
int AmberParm::nDihedralsWithHydrogen() const
{
    if (pointers.count() > 6)
        return pointers[6];
    else
        return 0;
}

/** Return the number of dihedrals without hydrogen */
int AmberParm::nDihedralsNoHydrogen() const
{
    if (pointers.count() > 7)
        return pointers[7];
    else
        return 0;
}

/** Return the number of excluded atoms */
int AmberParm::nExcluded() const
{
    if (pointers.count() > 9)
        return pointers[9];
    else
        return 0;
}

/** Return the number of residues */
int AmberParm::nResidues() const
{
    if (pointers.count() > 10)
        return pointers[10];
    else
        return 0;
}

/** Return the number of molecules in the file */
int AmberParm::nMolecules() const
{
    QVector<qint64> atoms_per_mol = int_data.value("ATOMS_PER_MOLECULE");
    
    if (atoms_per_mol.isEmpty())
    {
        if (nAtoms() == 0)
            return 0;
        else
            return 1;
    }
    else
        return atoms_per_mol.count();
}

/** Return an array that maps from the index of each atom to the index
    of each molecule. Note that both the atom index and molecule index
    are 0-indexed */
QVector<int> AmberParm::getAtomIndexToMolIndex() const
{
    const int natoms = this->nAtoms();

    if (natoms <= 0)
        return QVector<int>();

    QVector< QPair<int,int> > molidxs = this->moleculeIndicies();

    if (molidxs.isEmpty())
        return QVector<int>();

    const int nmols = molidxs.count();

    QVector<int> atom_to_mol(natoms);
    int *atom_to_mol_array = atom_to_mol.data();
    auto molidxs_array = molidxs.constData();

    if (usesParallel())
    {
        tbb::parallel_for( tbb::blocked_range<int>(0,nmols),
                           [&](tbb::blocked_range<int> r)
        {
            for (int i=r.begin(); i<r.end(); ++i)
            {
                const int start_idx = molidxs_array[i].first;
                const int end_idx = start_idx + molidxs_array[i].second;
                
                for (int j=start_idx; j<end_idx; ++j)
                {
                    atom_to_mol_array[j] = i;
                }
            }
        });
    }
    else
    {
        for (int i=0; i<nmols; ++i)
        {
            const int start_idx = molidxs_array[i].first;
            const int end_idx = start_idx + molidxs_array[i].second;
            
            for (int j=start_idx; j<end_idx; ++j)
            {
                atom_to_mol_array[j] = i;
            }
        }
    }
    
    return atom_to_mol;
}

/** Return the first index of the atom in each molecule, together with
    the number of atoms in that molecule */
QVector< QPair<int,int> > AmberParm::moleculeIndicies() const
{
    QVector< QPair<int,int> > idxs;
    
    QVector<qint64> atoms_per_mol = int_data.value("ATOMS_PER_MOLECULE");

    idxs.reserve(atoms_per_mol.count());
    
    if (atoms_per_mol.isEmpty())
    {
        if (nAtoms() > 0)
        {
            idxs.append( QPair<int,int>(0,nAtoms()) );
        }
    }
    else
    {
        qint64 nats = 0;
    
        for (auto a : atoms_per_mol)
        {
            if (a > 0)
            {
                idxs.append( QPair<int,int>(nats, a) );
                nats += a;
            }
        }
    }
    
    return idxs;
}

/** Return an AmberParm object parsed from the passed file */
AmberParm AmberParm::parse(const QString &filename, const PropertyMap &map)
{
    return AmberParm(filename);
}

/** Return the lines that correspond to the passed flag. This returns an
    empty list of there are no lines associated with the passed flag */
QVector<QString> AmberParm::linesForFlag(const QString &flag) const
{
    auto it = flag_to_line.constFind(flag);
    
    if (it != flag_to_line.constEnd())
    {
        const int start = it->first;
        const int count = it->second;
        
        SireBase::assert_true( start >= 0 and start < lines().count(), CODELOC );
        SireBase::assert_true( count > 0 and start+count < lines().count(), CODELOC );

        return lines().mid(start,count);
    }
    else
        return QVector<QString>();
}

/** Return all of the flags that are held in this file */
QStringList AmberParm::flags() const
{
    return flag_to_line.keys();
}

template<class FUNC, class Exception>
void live_test(FUNC function, QList<boost::shared_ptr<Exception>> &errors)
{
    try
    {
        return function();
    }
    catch(const Exception &e)
    {
        errors.append( boost::shared_ptr<Exception>(e.clone()) );
    }
}

/** Run through all of the data that has been read and perform a series
    of tests that will see if the prm7 data is sane. If any test fails,
    then an exception will be thrown */
void AmberParm::assertSane() const
{
    QList<boost::shared_ptr<SireError::exception>> errors;
    
    int natoms = this->nAtoms();
    
    live_test( [&](){ SireBase::assert_equal(this->stringData("ATOM_NAME").count(),
                                             natoms, CODELOC);}, errors );
    
    live_test( [&](){ SireBase::assert_equal(this->floatData("CHARGE").count(),
                                             natoms, CODELOC);}, errors );
    
    live_test( [&](){ SireBase::assert_equal(this->intData("ATOMIC_NUMBER").count(),
                                             natoms, CODELOC);}, errors );
    
    live_test( [&](){ SireBase::assert_equal(this->floatData("MASS").count(),
                                             natoms, CODELOC);}, errors );
    
    live_test( [&](){ SireBase::assert_equal(this->intData("ATOM_TYPE_INDEX").count(),
                                             natoms, CODELOC);}, errors );
    
    if (not errors.isEmpty())
    {
        for (auto error : errors)
        {
            qDebug() << error->toString();
        }
    
        throw SireIO::parse_error( QObject::tr(
            "Sanity tests failed for the loaded Amber prm7 format file"), CODELOC );
    }
}

/** Internal function used to get the molecule structure that starts at index 'start_idx'
    in the file, and that has 'natoms' atoms */
MolStructureEditor AmberParm::getMolStructure(int start_idx, int natoms,
                                              const PropertyName &cutting) const
{
    if (natoms == 0)
    {
        throw SireError::program_bug( QObject::tr(
                "Strange - there are no atoms in this molecule? %1 : %2")
                    .arg(start_idx).arg(natoms), CODELOC );
    }

    //first step is to build the structure of the molecule - i.e.
    //the layout of cutgroups, residues and atoms
    MolStructureEditor mol;

    const auto atom_names = this->stringData("ATOM_NAME");

    //locate the residue pointers for this molecule - note that the
    //residue pointers are 1-indexed (i.e. from atom 1 to atom N)
    const auto res_pointers = this->intData("RESIDUE_POINTER");
    const auto res_names = this->stringData("RESIDUE_LABEL");

    int res_start_idx = -1;
    int res_end_idx = -1;

    for (int i=0; i<res_pointers.count(); ++i)
    {
        if (res_pointers[i] == start_idx+1)
        {
            res_start_idx = i;
        }
        else if (res_pointers[i] == (start_idx+natoms+1))
        {
            res_end_idx = i;
            break;
        }
    }

    if (res_start_idx == -1)
    {
        throw SireIO::parse_error( QObject::tr(
                "Could not find the residues that belong to the molecule with "
                "atom indicies %1 to %2 (%3).")
                    .arg(start_idx+1).arg(start_idx+natoms)
                    .arg(res_start_idx), CODELOC );
    }

    if (res_end_idx == -1)
    {
        res_end_idx = res_pointers.count();
    }

    const int nres = res_end_idx - res_start_idx;

    for (int i=1; i<=nres; ++i)
    {
        int res_idx = res_start_idx + i - 1;  // 1-index versus 0-index
        int res_num = res_idx + 1;
    
        auto res = mol.add( ResNum(res_num) );
        res.rename( ResName( res_names[res_idx].trimmed() ) );
        
        int res_start_atom = res_pointers[res_idx];
        int res_end_atom;
        
        if (res_idx+1 == res_pointers.count())
        {
            res_end_atom = res_start_atom + natoms - 1;
        }
        else
        {
            res_end_atom = res_pointers[res_idx+1] - 1; // 1 lower than first index
                                                        // of atom in next residue
        }
        
        //by default we will use one CutGroup per molecule - this
        //may be changed later by the cutting system.
        auto cutgroup = mol.add( CGName(QString::number(i)) );
        
        for (int j=res_start_atom; j<=res_end_atom; ++j)
        {
            auto atom = cutgroup.add( AtomNum(j) );
            atom.rename( AtomName(atom_names[j-1].trimmed()) );  // 0-index versus 1-index
            atom.reparent( ResNum(res_num) );
        }
    }
    
    if (cutting.hasValue())
    {
        const CuttingFunction &cutfunc = cutting.value().asA<CuttingFunction>();
        
        if (not cutfunc.isA<ResidueCutting>())
        {
            mol = cutfunc(mol);
        }
    }
    
    return mol;
}

/** Return the AmberParams for the ith molecule */
AmberParams AmberParm::getAmberParams(int molidx, const MoleculeInfoData &molinfo) const
{
    //function to assign all of the atom parameters
    auto assign_atoms = [&]()
    {
        //assign all of the atomic properties
        const auto charge_array = this->floatData("CHARGE").constData();
        const auto mass_array = this->floatData("MASS").constData();
        const auto atomic_num_array = this->intData("ATOMIC_NUMBER").constData();
        const auto amber_type_array = this->intData("ATOM_TYPE_INDEX").constData();
        const auto ambertype_array = this->stringData("AMBER_ATOM_TYPE").constData();
        
        const int natoms = molinfo.nAtoms();
        
        AmberParams params(molinfo);
        
        for (int i=0; i<natoms; ++i)
        {
            const int atom_num = molinfo.number(AtomIdx(i)).value();
            auto cgatomidx = molinfo.cgAtomIdx(AtomIdx(i));
            
            const int atom_idx = atom_num - 1;  // 1-index versus 0-index
            
            params.add( AtomIdx(i),
                        (charge_array[atom_idx] / AMBERCHARGECONV) * mod_electron,
                        mass_array[atom_idx] * g_per_mol,
                        Element(int(atomic_num_array[atom_idx])),
                        lj_data[ amber_type_array[atom_idx] - 1 ],
                        ambertype_array[atom_idx].trimmed() );
        }
        
        return params;
    };

    //function to read all of the bonding information
    auto assign_bonds = [&]()
    {
        AmberParams params(molinfo);
        
        const auto k_array = this->floatData("BOND_FORCE_CONSTANT").constData();
        const auto r0_array = this->floatData("BOND_EQUIL_VALUE").constData();
    
        auto func = [&](const QVector<int> &idxs, const QVector<qint64> &bonds)
        {
            for (int idx : idxs)
            {
                const AtomNum atom0( bonds[ idx ] / 3 + 1 );
                const AtomNum atom1( bonds[ idx + 1 ] / 3 + 1 );

                const int param_idx = bonds[ idx + 2 ] - 1; // 1 indexed to 0 indexed
                
                const double k = k_array[param_idx];
                const double r0 = r0_array[param_idx];
                
                params.add( BondID(atom0,atom1), k, r0 );
            }
        };

        func( bonds_inc_h[molidx], this->intData("BONDS_INC_HYDROGEN") );
        func( bonds_exc_h[molidx], this->intData("BONDS_WITHOUT_HYDROGEN") );
        
        return params;
    };
    
    //function to read in the angle information
    auto assign_angles = [&]()
    {
        AmberParams params(molinfo);

        const auto k_array = this->floatData("ANGLE_FORCE_CONSTANT").constData();
        const auto t0_array = this->floatData("ANGLE_EQUIL_VALUE").constData();
        
        auto func = [&](const QVector<int> &idxs, const QVector<qint64> &angles)
        {
            for (int idx : idxs)
            {
                const AtomNum atom0( angles[ idx ] / 3 + 1 );
                const AtomNum atom1( angles[ idx + 1 ] / 3 + 1 );
                const AtomNum atom2( angles[ idx + 2 ] / 3 + 1 );

                const int param_idx = angles[ idx + 3 ] - 1;  //1 indexed to 0 indexed
                
                const double k = k_array[param_idx];
                const double t0 = t0_array[param_idx];
                
                params.add( AngleID(atom0,atom1,atom2), k, t0 );
            }
        };

        func( angs_inc_h[molidx], this->intData("ANGLES_INC_HYDROGEN") );
        func( angs_exc_h[molidx], this->intData("ANGLES_WITHOUT_HYDROGEN") );

        return params;
    };

    //function to read in the dihedral+improper information and nbpairs information
    auto assign_dihedrals = [&]()
    {
        const auto k_array = this->floatData("DIHEDRAL_FORCE_CONSTANT").constData();
        const auto per_array = this->floatData("DIHEDRAL_PERIODICITY").constData();
        const auto pha_array = this->floatData("DIHEDRAL_PHASE").constData();

        const auto sceefactor = this->floatData("SCEE_SCALE_FACTOR");
        const auto scnbfactor = this->floatData("SCNB_SCALE_FACTOR");

        AmberParams params(molinfo);

        auto func = [&](const QVector<int> &idxs, const QVector<qint64> &dihedrals)
        {
            for (int idx : idxs)
            {
                const AtomNum atom0( dihedrals[ idx ] / 3 + 1 );
                const AtomNum atom1( dihedrals[ idx + 1 ] / 3 + 1 );
                const AtomNum atom2( std::abs(dihedrals[ idx + 2 ] / 3) + 1 );
                const AtomNum atom3( std::abs(dihedrals[ idx + 3 ] / 3) + 1 );

                const bool ignored = dihedrals[ idx + 2 ] < 0;  // negative implies ignored
                const bool improper = dihedrals[ idx + 3 ] < 0; // negative implies improper
                
                const int param_index = dihedrals[ idx + 4 ] - 1; // 1 indexed to 0
                
                double k = k_array[param_index]; // kcal_per_mol
                double per = per_array[param_index]; // radians
                double phase = pha_array[param_index];

                if (improper)
                {
                    params.add(ImproperID(atom0,atom1,atom2,atom3), k, per, phase);
                }
                else
                {
                    params.add(DihedralID(atom0,atom1,atom2,atom3), k, per, phase);

                    if (not ignored)
                    {
                        // this is a 1-4 pair that has a scale factor
                    
                        // Assume default values for 14 scaling factors
                        // Note that these are NOT inversed after reading from input
                        double sclee14 = 1.0/AMBER14COUL;
                        double sclnb14 = 1.0/AMBER14LJ;
                        
                        if (not sceefactor.isEmpty())
                            sclee14 = sceefactor[param_index];
                        
                        if (not scnbfactor.isEmpty())
                            sclnb14 = scnbfactor[param_index];
                    
                        if (sclee14 < 0.00001)
                        {
                            throw SireError::program_bug( QObject::tr(
                                "A 1,4 pair has a coulombic scaling factor of 0.0 in the top file "
                                "which would mean an infinite energy!"),
                                    CODELOC );
                        }
                        if (sclnb14 < 0.00001)
                        {
                            throw SireError::program_bug( QObject::tr(
                                "A 1,4 pair has a LJ scaling factor of 0.0 in the top file "
                                "which would mean an infinite energy!"),
                                    CODELOC );
                        }

                        //invert the scale factors
                        sclee14 = 1.0/sclee14;
                        sclnb14 = 1.0/sclnb14;

                        //save them into the parameter space
                        params.addNB14( BondID(atom0,atom3), sclee14, sclnb14 );
                    }
                }
            }
        };

        func( dihs_inc_h[molidx], this->intData("DIHEDRALS_INC_HYDROGEN") );
        func( dihs_exc_h[molidx], this->intData("DIHEDRALS_WITHOUT_HYDROGEN") );

        return params;
    };
    
    //function to read in the excluded atoms information
    auto assign_excluded = [&]()
    {
        CLJNBPairs nbpairs;
        
        if (molinfo.nAtoms() <= 3)
        {
            //everything is bonded, so scale factor is 0
            nbpairs = CLJNBPairs(molinfo, CLJScaleFactor(0,0));
        }
        else
        {
            //default is that atoms are not bonded (so scale factor is 1)
            nbpairs = CLJNBPairs(molinfo, CLJScaleFactor(1.0,1.0));
            
            const int natoms = molinfo.nAtoms();

            //get the set of excluded atoms for each atom of this molecule
            const auto excluded_atoms = excl_atoms[molidx];
            SireBase::assert_equal( excluded_atoms.count(), natoms, CODELOC );

            for (int i=0; i<natoms; ++i)
            {
                const AtomIdx atom0(i);
                const CGAtomIdx cgatom0 = molinfo.cgAtomIdx(atom0);
            
                // an atom is bonded to itself
                nbpairs.set(cgatom0, cgatom0, CLJScaleFactor(0,0));

                // Are there any excluded atoms of atom0?
                for (const auto excluded_atom : excluded_atoms[i])
                {
                    if (excluded_atom > 0)
                    {
                        const AtomNum atomnum1(excluded_atom);
                        const CGAtomIdx cgatom1 = molinfo.cgAtomIdx(atomnum1);
                        nbpairs.set( cgatom0, cgatom1, CLJScaleFactor(0,0) );
                    }
                }
            }
        }
        
        AmberParams params(molinfo);
        params.setExcludedAtoms(nbpairs);
        
        return params;
    };
    
    AmberParams params(molinfo);
    
    //assign all of the parameters
    if (usesParallel())
    {
        AmberParams atoms, bonds, angles, dihedrals, excluded;
    
        tbb::parallel_invoke
        (
            [&](){ atoms = assign_atoms(); },
            [&](){ bonds = assign_bonds(); },
            [&](){ angles = assign_angles(); },
            [&](){ dihedrals = assign_dihedrals(); },
            [&](){ excluded = assign_excluded(); }
        );
        
        params += atoms;
        params += bonds;
        params += angles;
        params += dihedrals;
        params += excluded;
    }
    else
    {
        params += assign_atoms();
        params += assign_bonds();
        params += assign_angles();
        params += assign_dihedrals();
        params += assign_excluded();
    }
    
    return params;
}

/** Internal function used to get the molecule structure that starts at index 'start_idx'
    in the file, and that has 'natoms' atoms */
MolEditor AmberParm::getMolecule(int molidx, int start_idx, int natoms,
                                 const PropertyMap &map) const
{
    if (natoms == 0)
    {
        throw SireError::program_bug( QObject::tr(
            "Strange - there are no atoms in molecule at index %1? %2 : %3")
                .arg(molidx).arg(start_idx).arg(natoms), CODELOC );
    }

    //first, construct the layout of the molecule (sorting of atoms into residues and cutgroups)
    auto mol = this->getMolStructure(start_idx, natoms, map["cutting"]).commit().edit();
    
    //get the info object that can map between AtomNum to AtomIdx etc.
    const auto molinfo = mol.info();

    auto amber_params = this->getAmberParams(molidx, molinfo);

    mol.setProperty(map["charge"], amber_params.charges());
    mol.setProperty(map["LJ"], amber_params.ljs());
    mol.setProperty(map["mass"], amber_params.masses());
    mol.setProperty(map["element"], amber_params.elements());
    mol.setProperty(map["ambertype"], amber_params.amberTypes());
    mol.setProperty(map["connectivity"], amber_params.connectivity());
    mol.setProperty(map["bond"],
                    amber_params.bondFunctions(InternalPotential::symbols().bond().r()));
    mol.setProperty(map["angle"],
                    amber_params.angleFunctions(InternalPotential::symbols().angle().theta()));
    mol.setProperty(map["dihedral"],
                    amber_params.dihedralFunctions(InternalPotential::symbols().dihedral().phi()));
    mol.setProperty(map["improper"],
                    amber_params.improperFunctions(InternalPotential::symbols().dihedral().phi()));
    mol.setProperty(map["intrascale"], amber_params.cljScaleFactors());

    mol.setProperty(map["amberparameters"], amber_params);

    return mol;
}

/** Return the ith molecule that is described by this AmberParm file. Note
    that this molecule won't have any coordinate data, as this is not
    provided in this file */
Molecule AmberParm::getMolecule(int molidx, const PropertyMap &map) const
{
    const QVector< QPair<int,int> > mol_idxs = this->moleculeIndicies();
    molidx = Index(molidx).map(mol_idxs.count());

    return this->getMolecule( molidx, mol_idxs[molidx].first,
                                      mol_idxs[molidx].second, map ).commit();
}

/** Return the ith molecule that is described by this AmberParm file, getting
    the coordinate (and possibly velocity) data from the passed AmberRst file */
Molecule AmberParm::getMolecule(int molidx, const AmberRst &rst,
                                const PropertyMap &map) const
{
    if (rst.nAtoms() != this->nAtoms())
    {
        //these two files are likely to be incompatible!
        throw SireIO::parse_error( QObject::tr(
                "The passed Amber Parm and Restart files are incompatible as they "
                "contain data for different numbers of atoms "
                "(%1 in the Parm file and %2 in the Restart)")
                    .arg(this->nAtoms()).arg(rst.nAtoms()), CODELOC );
    }

    const QVector< QPair<int,int> > mol_idxs = this->moleculeIndicies();
    molidx = Index(molidx).map(mol_idxs.count());

    auto mol = this->getMolecule( molidx, mol_idxs[molidx].first,
                                          mol_idxs[molidx].second, map);
    
    const auto molinfo = mol.info();

    //create space for the coordinates
    auto coords = QVector< QVector<Vector> >(molinfo.nCutGroups());
    
    for (int i=0; i<molinfo.nCutGroups(); ++i)
    {
        coords[i] = QVector<Vector>(molinfo.nAtoms(CGIdx(i)));
    }
    
    const Vector *coords_array = rst.coordinates().constData();

    if (rst.hasVelocities())
    {
        auto vels = AtomVelocities(molinfo);
        const Vector *vels_array = rst.velocities().constData();
        
        for (int i=0; i<mol.nAtoms(); ++i)
        {
            const int atom_num = molinfo.number(AtomIdx(i)).value();
            auto cgatomidx = molinfo.cgAtomIdx(AtomIdx(i));
            
            const int atom_idx = atom_num - 1;  // 1-index versus 0-index
            
            coords[cgatomidx.cutGroup()][cgatomidx.atom()] = coords_array[atom_idx];

            //velocity is Angstroms per 1/20.455 ps
            const auto vel_unit = (1.0 / 20.455) * angstrom / picosecond;
            
            const Vector &vel = vels_array[atom_idx];
            vels.set(cgatomidx, Velocity3D(vel.x() * vel_unit,
                                           vel.y() * vel_unit,
                                           vel.z() * vel_unit));
        }
        
        return mol.setProperty(map["velocity"], vels)
                  .setProperty(map["coordinates"], AtomCoords(CoordGroupArray(coords)))
                  .commit();
    }
    else
    {
        for (int i=0; i<mol.nAtoms(); ++i)
        {
            const int atom_num = molinfo.number(AtomIdx(i)).value();
            auto cgatomidx = molinfo.cgAtomIdx(AtomIdx(i));
            
            const int atom_idx = atom_num - 1;  // 1-index versus 0-index
            
            coords[cgatomidx.cutGroup()][cgatomidx.atom()] = coords_array[atom_idx];
        }
        
        return mol.setProperty(map["coordinates"], AtomCoords(CoordGroupArray(coords)))
                  .commit();
    }
}

/** Return the System that is described by this AmberParm file. Note that
    the molecules in this system don't have any coordinates (as these aren't
    provided by the file */
System AmberParm::startSystem(const PropertyMap &map) const
{
    const QVector< QPair<int,int> > mol_idxs = this->moleculeIndicies();

    const int nmols = mol_idxs.count();
    
    if (nmols == 0)
        return System();
    
    QVector<Molecule> mols(nmols);
    Molecule *mols_array = mols.data();
    
    if (usesParallel())
    {
        tbb::parallel_for( tbb::blocked_range<int>(0,nmols),
                           [&](tbb::blocked_range<int> r)
        {
            //create and populate all of the molecules
            for (int i=r.begin(); i<r.end(); ++i)
            {
                mols_array[i] = this->getMolecule(i, mol_idxs[i].first, mol_idxs[i].second, map);
            }
        });
    }
    else
    {
        for (int i=0; i<nmols; ++i)
        {
            mols_array[i] = this->getMolecule(i, mol_idxs[i].first, mol_idxs[i].second, map);
        }
    }
    
    MoleculeGroup molgroup("all");
    
    for (auto mol : mols)
    {
        molgroup.add(mol);
    }
    
    System system( this->title() );
    system.add(molgroup);
    
    return system;
}