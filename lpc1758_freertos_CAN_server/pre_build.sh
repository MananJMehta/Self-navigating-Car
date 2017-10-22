#!/bin/bash

# 
# This file executes from the build directory by the Eclipse "Pre Build" step
#

#python ../_can_dbc/dbc_parse.py -i ../../common_dbc/243.dbc -s MASTER > ../_can_dbc/generated_can.h
python ../_can_dbc/dbc_parse.py -i ../_can_dbc/243.dbc -s MASTER > ../_can_dbc/generated_can.h