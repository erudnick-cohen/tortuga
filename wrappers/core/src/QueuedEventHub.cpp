/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  wrappers/core/src/QueuedEventHub.cpp
 */

// Library Includes
#include <boost/python.hpp>

// Project Includes
#include "core/include/EventHub.h"
#include "core/include/QueuedEventHub.h"
#include "core/include/EventPublisher.h"
#include "core/include/EventConnection.h"
#include "wrappers/core/include/EventFunctor.h"

namespace bp = boost::python;

void registerQueuedEventHubClass()
{
    bp::class_<ram::core::QueuedEventHub,
        bp::bases<ram::core::EventHub> >("QueuedEventHub",
            bp::init<ram::core::EventHubPtr, bp::optional<std::string> >(
            (bp::arg("eventHub"),
             bp::arg("name") = std::string("QueuedEventHub")) ))
        .def("publishEvents",
             &ram::core::QueuedEventHub::publishEvents)
        .def("waitAndPublishEvents",
             &ram::core::QueuedEventHub::waitAndPublishEvents);
}


