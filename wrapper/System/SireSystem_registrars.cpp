//WARNING - AUTOGENERATED FILE - CONTENTS WILL BE OVERWRITTEN!
#include <Python.h>

#include "SireSystem_registrars.h"

#include "systemmonitors.h"
#include "monitorname.h"
#include "spacewrapper.h"
#include "monitorproperty.h"
#include "sysidentifier.h"
#include "system.h"
#include "monitoridx.h"
#include "energymonitor.h"
#include "monitoridentifier.h"
#include "constraints.h"
#include "sysidx.h"
#include "dihedralcomponent.h"
#include "monitormonitor.h"
#include "sysname.h"
#include "closemols.h"
#include "freeenergymonitor.h"
#include "checkpoint.h"
#include "anglecomponent.h"
#include "monitorcomponents.h"
#include "systemmonitor.h"
#include "monitorcomponent.h"
#include "idassigner.h"
#include "polarisecharges.h"
#include "perturbationconstraint.h"
#include "identityconstraint.h"
#include "constraint.h"
#include "volmapmonitor.h"
#include "distancecomponent.h"

#include "Helpers/objectregistry.hpp"

void register_SireSystem_objects()
{

    ObjectRegistry::registerConverterFor< SireSystem::SystemMonitors >();
    ObjectRegistry::registerConverterFor< SireSystem::MonitorName >();
    ObjectRegistry::registerConverterFor< SireSystem::SpaceWrapper >();
    ObjectRegistry::registerConverterFor< SireSystem::MonitorProperty >();
    ObjectRegistry::registerConverterFor< SireID::Specify<SireSystem::SysID> >();
    ObjectRegistry::registerConverterFor< SireID::IDAndSet<SireSystem::SysID> >();
    ObjectRegistry::registerConverterFor< SireID::IDOrSet<SireSystem::SysID> >();
    ObjectRegistry::registerConverterFor< SireSystem::SysIdentifier >();
    ObjectRegistry::registerConverterFor< SireSystem::System >();
    ObjectRegistry::registerConverterFor< SireSystem::MonitorIdx >();
    ObjectRegistry::registerConverterFor< SireSystem::EnergyMonitor >();
    ObjectRegistry::registerConverterFor< SireID::Specify<SireSystem::MonitorID> >();
    ObjectRegistry::registerConverterFor< SireID::IDAndSet<SireSystem::MonitorID> >();
    ObjectRegistry::registerConverterFor< SireID::IDOrSet<SireSystem::MonitorID> >();
    ObjectRegistry::registerConverterFor< SireSystem::MonitorIdentifier >();
    ObjectRegistry::registerConverterFor< SireSystem::Constraints >();
    ObjectRegistry::registerConverterFor< SireSystem::SysIdx >();
    ObjectRegistry::registerConverterFor< SireSystem::DihedralComponent >();
    ObjectRegistry::registerConverterFor< SireSystem::MonitorMonitor >();
    ObjectRegistry::registerConverterFor< SireSystem::SysName >();
    ObjectRegistry::registerConverterFor< SireSystem::CloseMols >();
    ObjectRegistry::registerConverterFor< SireSystem::FreeEnergyMonitor >();
    ObjectRegistry::registerConverterFor< SireSystem::AssignerGroup >();
    ObjectRegistry::registerConverterFor< SireSystem::CheckPoint >();
    ObjectRegistry::registerConverterFor< SireSystem::AngleComponent >();
    ObjectRegistry::registerConverterFor< SireSystem::MonitorComponents >();
    ObjectRegistry::registerConverterFor< SireSystem::NullMonitor >();
    ObjectRegistry::registerConverterFor< SireSystem::MonitorComponent >();
    ObjectRegistry::registerConverterFor< SireSystem::IDAssigner >();
    ObjectRegistry::registerConverterFor< SireSystem::PolariseCharges >();
    ObjectRegistry::registerConverterFor< SireSystem::PolariseChargesFF >();
    ObjectRegistry::registerConverterFor< SireSystem::PerturbationConstraint >();
    ObjectRegistry::registerConverterFor< SireSystem::IdentityConstraint >();
    ObjectRegistry::registerConverterFor< SireSystem::NullConstraint >();
    ObjectRegistry::registerConverterFor< SireSystem::PropertyConstraint >();
    ObjectRegistry::registerConverterFor< SireSystem::ComponentConstraint >();
    ObjectRegistry::registerConverterFor< SireSystem::WindowedComponent >();
    ObjectRegistry::registerConverterFor< SireSystem::VolMapMonitor >();
    ObjectRegistry::registerConverterFor< SireSystem::DistanceComponent >();
    ObjectRegistry::registerConverterFor< SireSystem::DoubleDistanceComponent >();
    ObjectRegistry::registerConverterFor< SireSystem::TripleDistanceComponent >();

}

