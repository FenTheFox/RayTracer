//
//  stringops.h
//  sqlite-test
//
//  Created by Timm Allman on 10/22/14.
//  Copyright (c) 2014 UMass. All rights reserved.
//
#pragma once

std::string trim (std::string, std::string = " \t");

std::string substring (std::string, size_t, size_t = 0);