/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/src/modules/BasicDVLEstimationModule.cpp
 */

// STD Includes
#include <iostream>

// Library Includes
#include <log4cpp/Category.hh>

// Project Includes
#include "vehicle/include/Events.h"
#include "estimation/include/modules/BasicDVLEstimationModule.h"
#include "math/include/Helpers.h"
#include "math/include/Quaternion.h"
#include "math/include/Matrix2.h"
#include "vehicle/include/device/IVelocitySensor.h"

static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("StEstDVL"));

namespace ram {
namespace estimation {

BasicDVLEstimationModule::BasicDVLEstimationModule(core::ConfigNode config,
                                                   core::EventHubPtr eventHub,
                                                   EstimatedStatePtr estState) :
    EstimationModule(eventHub, "BasicDVLEstimationModule",estState,
                     vehicle::device::IVelocitySensor::RAW_UPDATE)
{
    // initialization from config values should be done here
    LOGGER.info("%Vel_b[2] Vel_n[2] Pos_n[2] Timestamp");
}

void BasicDVLEstimationModule::update(core::EventPtr event)
{

    vehicle::RawDVLDataEventPtr ievent =
        boost::dynamic_pointer_cast<vehicle::RawDVLDataEvent>(event);

    // return if the cast failed and let people know about it.
    if(!ievent){
        LOGGER.warn("BasicDVLEstimationModule: update: Invalid Event Type");
        return;
    }

    RawDVLData state = ievent->rawDVLData;
    double timestep = ievent->timestep;

    // we want to work with a vector3 so we can rotate in 3 dimensions
    math::Vector3 vel3_b(ievent->velocity_b[0], ievent->velocity_b[1], 0);

    math::Quaternion orientation = m_estimatedState->getEstimatedOrientation();

    // rotate the body frame velocity to the inertial frame
    // this is valid for any orientation, dont need to be level
    math::Vector3 vel3_n = orientation.UnitInverse() * vel3_b;

    // for now we only care about the in plane velocity measurements
    math::Vector2 vel2_n(vel3_n[0], vel3_n[1]);

    // grab the old position estimate
    math::Vector2 oldPos = m_estimatedState->getEstimatedPosition();
    math::Vector2 oldVel = m_estimatedState->getEstimatedVelocity();
    
    // trapezoidal integration for new position
    math::Vector2 pos_n = oldPos + timestep * (vel2_n + oldVel) / 2;

    // store the new estimates
    m_estimatedState->setEstimatedVelocity(vel2_n);
    m_estimatedState->setEstimatedPosition(pos_n);

    // log the estimates
    LOGGER.infoStream() << orientation[0] << " "
                        << orientation[1] << " "
                        << orientation[2] << " "
                        << orientation[3] << " "
                        << vel3_b[0] << " "
                        << vel3_b[1] << " "
                        << vel3_b[2] << " "
                        << vel3_n[0] << " "
                        << vel3_n[1] << " "
                        << vel3_n[2] << " "
                        << pos_n[0] << " "
                        << pos_n[1] << " ";
}

} // namespace estimation
} // namespace ram
