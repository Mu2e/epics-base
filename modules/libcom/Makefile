#*************************************************************************
# Copyright (c) 2002 The University of Chicago, as Operator of Argonne
#     National Laboratory.
# Copyright (c) 2002 The Regents of the University of California, as
#     Operator of Los Alamos National Laboratory.
# EPICS BASE is distributed subject to a Software License Agreement found
# in file LICENSE that is included with this distribution.
#*************************************************************************

TOP = ../..
include $(TOP)/configure/CONFIG

DIRS += src

DIRS += RTEMS
RTEMS_DEPEND_DIRS = src

DIRS += vxWorks
vxWorks_DEPEND_DIRS = src

DIRS += test
test_DEPEND_DIRS = RTEMS vxWorks

include $(TOP)/configure/RULES_DIRS
