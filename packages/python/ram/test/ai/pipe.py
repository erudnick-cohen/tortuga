# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/ai/pipe.py

# Python Imports
import unittest

# Project Imports
import ram.ai.pipe as pipe
import ext.core as core
import ext.vision as vision
import ext.math as math

import ram.motion as motion
import ram.motion.pipe

import ram.test.ai.support as aisupport

class PipeTest(aisupport.AITestCase):
    def injectPipeEvent(self, x, y, angle):
        self.injectEvent(vision.EventType.PIPE_FOUND, vision.PipeEvent,0,0,0, 
                         x = x, y = y, angle = angle)

class TestStart(PipeTest):
    def setUp(self):
        PipeTest.setUp(self)
        self.machine.start(pipe.Start)
    
    def testStart(self):
        """Make sure we are diving with no detector on"""
        self.assertFalse(self.visionSystem.pipeLineDetector)
        self.assertCurrentMotion(motion.basic.RateChangeDepth)
        
    def testFinish(self):
        self.injectEvent(motion.basic.Motion.FINISHED)
        self.assertCurrentState(pipe.Searching)
        
class TestSearching(PipeTest):
    def setUp(self):
        PipeTest.setUp(self)
        self.machine.start(pipe.Searching)        
    
    def testStart(self):
        """Make sure we have the detector on when starting"""
        self.assert_(self.visionSystem.pipeLineDetector)
        self.assertCurrentMotion(motion.search.ForwardZigZag)
                
    def testLightFound(self):
        # Now change states
        self.injectEvent(vision.EventType.PIPE_FOUND, vision.PipeEvent, 0, 
                         0, 0)
        self.assertCurrentState(pipe.Seeking)
        
        # Leave and make sure its still on
        self.assert_(self.visionSystem.pipeLineDetector)
        
def pipeFoundHelper(self):
    # Standard directions
    self.injectPipeEvent(x = 0.5, y = -0.5, angle = math.Degree(15.0))
        
    self.assertLessThan(self.controller.speed, 0)
    self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
    self.assertGreaterThan(self.controller.yawChange, 0)
    
    # Now lets test biasing with vehicle at -22.5, and pipe 22.5 to the right
    # we will do both +90, and -90 (west and east) bias direction.  Absolute
    # pipe orientation is -45 degrees.
    self.vehicle.orientation = math.Quaternion(math.Degree(-22.5),
                                               math.Vector3.UNIT_Z)
    self.controller.setDesiredOrientation(self.vehicle.orientation)
    
    cstate = self.machine.currentState()
    cstate._biasDirection = math.Degree(-90)
    self.injectPipeEvent(x = 0, y = -0, angle = math.Degree(-22.5))
    self.assertLessThan(self.controller.yawChange, 0)
    
    cstate._biasDirection = math.Degree(90)
    self.injectPipeEvent(x = 0, y = -0, angle = math.Degree(-22.5))
    self.assertGreaterThan(self.controller.yawChange, 0)
    
    # Now the same maneuvers without bias, to ensure it moves the opposite way
    cstate._biasDirection = None
    
    self.injectPipeEvent(x = 0, y = -0, angle = math.Degree(-22.5))
    self.assertLessThan(self.controller.yawChange, 0)
    
    # Now the same maneuvers with the bias in the right direction
    cstate._biasDirection = math.Degree(-45)
    
    self.injectPipeEvent(x = 0, y = -0, angle = math.Degree(-22.5))
    self.assertLessThan(self.controller.yawChange, 0)
    
    # Now lets test biasing with vehicle at 22.5, and pipe 22.5 to the left
    # we will do both +90, and -90 (west and east) bias direction.  Absolute
    # pipe orientation is 45 degrees.
    self.vehicle.orientation = math.Quaternion(math.Degree(22.5),
                                               math.Vector3.UNIT_Z)
    self.controller.setDesiredOrientation(self.vehicle.orientation)
    
    cstate._biasDirection = math.Degree(-90)
    self.injectPipeEvent(x = 0, y = -0, angle = math.Degree(22.5))
    self.assertLessThan(self.controller.yawChange, 0)
    
    cstate._biasDirection = math.Degree(90)
    self.injectPipeEvent(x = 0, y = -0, angle = math.Degree(22.5))
    self.assertGreaterThan(self.controller.yawChange, 0)
    
    # Now the same maneuvers without bias, to ensure it moves the opposite way
    cstate._biasDirection = None
    
    self.injectPipeEvent(x = 0, y = -0, angle = math.Degree(22.5))
    self.assertGreaterThan(self.controller.yawChange, 0)
        
    # Now the same maneuvers with the bias in the right direction
    cstate._biasDirection = math.Degree(45)
    
    self.injectPipeEvent(x = 0, y = -0, angle = math.Degree(22.5))
    self.assertGreaterThan(self.controller.yawChange, 0)
        
class TestSeeking(PipeTest):
    def setUp(self):
        PipeTest.setUp(self)
        self.machine.start(pipe.Seeking)
    
    def testStart(self):
        self.assertCurrentMotion(motion.pipe.Hover)
        
    def testPipeFound(self):
        """Make sure new found events move the vehicle"""
        pipeFoundHelper(self)
    
    def testPipeLost(self):
        """Make sure losing the light goes back to search"""
        self.injectEvent(vision.EventType.PIPE_LOST)
        self.assertCurrentState(pipe.Searching)
        
class TestCentering(PipeTest):
    def setUp(self):
        PipeTest.setUp(self)
        self.machine.start(pipe.Centering)
        
    def testStart(self):
        """Make sure the motion and the timer are setup properly"""
        self.machine.stop()
        self.machine.start(pipe.Centering)
        
        self.assertCurrentMotion(motion.pipe.Hover)
        
        self.releaseTimer(pipe.Centering.SETTLED)
        self.assertCurrentState(pipe.AlongPipe)
        
    def testPipeLost(self):
        """Make sure we search when we lose the pipe"""
        self.injectEvent(vision.EventType.PIPE_LOST)
        self.assertCurrentState(pipe.Searching)
    
    def testPipeFound(self):
        """Make sure the loop back works"""
        pipeFoundHelper(self)
    
    def testSettled(self):
        """Make sure we move on after settling"""
        self.injectEvent(pipe.Centering.SETTLED)
        self.assertCurrentState(pipe.AlongPipe)

class TestAlongPipe(PipeTest):
    def setUp(self):
        PipeTest.setUp(self)
        self.machine.start(pipe.AlongPipe)
        
    def testStart(self):
        self.assertCurrentMotion(motion.pipe.Follow)
        
    def testPipeFound(self):
        """Make sure it has an effect on the vehicle, and throws event"""
        
        # The outside angle case
        self.injectPipeEvent(x = 0.5, y = -0.5, angle = math.Degree(15.0))
        
        # Goes forward even when pipe behind vehicle
        self.assertGreaterThan(self.controller.speed, 0)
        self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        self.assertEqual(0, self.controller.yawChange)
        self.controller.speed = 0
        self.controller.sidewaysSpeed = 0
        
        # The inside range case
        self.injectPipeEvent(x = 0.4, y = -0.4, angle = math.Degree(15.0))
        self.assertGreaterThan(self.controller.speed, 0)
        self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        self.assertGreaterThan(self.controller.yawChange, 0)
        self.controller.speed = 0
        self.controller.sidewaysSpeed = 0
        
    
        # Test the transition to a new pipe (ie, now pipes in front)
        self.called = False
        def handler(event):
            self.called = True
        self.eventHub.subscribeToType(pipe.AlongPipe.FOUND_NEW_PIPE, handler)
        
        self.injectPipeEvent(x = 0.4, y = 0.4, angle = math.Degree(15.0))
        
        self.assert_(self.called)
        
    def testPipeLost(self):
        """Make sure it goes to between pipes, when pipe is lost"""
        self.injectEvent(vision.EventType.PIPE_LOST)
        self.assertCurrentState(pipe.BetweenPipes)

class TestBetweenPipes(PipeTest):
    def testStart(self):
        # Make sure we start driving forward
        self.machine.start(pipe.BetweenPipes)
        self.assert_(self.controller.speed > 0)
        
        # Now make sure we stop
        self.releaseTimer(pipe.BetweenPipes.LOST_PATH)
        self.assertEqual(0, self.controller.speed)
        
        # Make sure we hit the end state
        self.assert_(self.machine.complete)
    
    def testPipeFound(self):
        """If we find a pipe, seek it"""
        self.machine.start(pipe.BetweenPipes)
        self.injectEvent(vision.EventType.PIPE_FOUND)
        self.assertCurrentState(pipe.Seeking)
