/*
 For more information, please see: http://software.sci.utah.edu

 The MIT License

 Copyright (c) 2009 Scientific Computing and Imaging Institute,
 University of Utah.


 Permission is hereby granted, free of charge, to any person obtaining a
 copy of this software and associated documentation files (the "Software"),
 to deal in the Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS IN THE SOFTWARE.
 */

// Core includes
#include <Core/Graphics/ColorMap.h>

namespace Core 
{

ColorMap::ColorMap()
{
	// Test code -- default colormap -- red to blue
	this->colors_.push_back( Color( 1.0, 0.0, 0.0 ) );
	this->colors_.push_back( Color( 0.0, 0.0, 1.0 ) );
}

ColorMap::~ColorMap()
{
}

ColorMap::ColorMap( const std::vector< Color >& colors )
: colors_( colors )
{
}

// GET_COLORS
const std::vector< Color >& ColorMap::get_colors() const
{
	return this->colors_;
}

// SET_COLORS:
void ColorMap::set_colors( const std::vector< Color >& colors )
{
	this->colors_ = colors;
}

} // End namespace Core


