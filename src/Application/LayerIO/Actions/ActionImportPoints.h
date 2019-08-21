/*
 For more information, please see: http://software.sci.utah.edu

 The MIT License

 Copyright (c) 2016 Scientific Computing and Imaging Institute,
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

#ifndef APPLICATION_LAYERIO_ACTIONS_ACTIONIMPORTPOINTS_H
#define APPLICATION_LAYERIO_ACTIONS_ACTIONIMPORTPOINTS_H

// Core includes
#include <Core/Action/Actions.h>
#include <Core/Interface/Interface.h>

#include <Core/Geometry/Point.h>
#include <Application/LayerIO/Actions/ActionImportVector.h>

namespace Seg3D
{
struct PointReader
{
  std::istream& PointReader::operator()(std::istream& i, Core::Point& p) const;
};


class ActionImportPoints : public ActionImportVector<Core::Point>
{
  CORE_ACTION(
    CORE_ACTION_TYPE("ImportPoints", "This action imports a list of points (x y z) to Seg3D.")
    CORE_ACTION_ARGUMENT("file_path", "Path to the file that the points will be read from.")
    CORE_ACTION_CHANGES_PROJECT_DATA()
    )

public:
  using Base = ActionImportVector<Core::Point>;
  ActionImportPoints(const std::string& file_path) :
    Base(file_path)
  {
    init_parameters();
  }
  ActionImportPoints()
  {
    init_parameters();
  }

  // DISPATCH:
  static void Dispatch(Core::ActionContextHandle context,
    const std::string& file_path)
  {
    auto action = Core::ActionHandle(new ActionImportPoints(file_path));

    Core::ActionDispatcher::PostAction(action, context);
  }
};

} // end namespace Seg3D

#endif
