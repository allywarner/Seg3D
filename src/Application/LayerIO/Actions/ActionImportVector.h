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

#ifndef APPLICATION_LAYERIO_ACTIONS_ACTIONIMPORTVECTOR_H
#define APPLICATION_LAYERIO_ACTIONS_ACTIONIMPORTVECTOR_H

// Core includes
#include <Core/Action/Actions.h>
#include <Core/Interface/Interface.h>
#include <Core/Utils/Exception.h>

#include <Core/Geometry/Point.h>

#include <fstream>

namespace Seg3D
{

template <class T>
class ActionImportVector : public Core::Action
{
public:
  using Vector = std::vector<T>;

  ActionImportVector(const std::string& file_path) :
    file_path_(file_path)
  {
  }

  ActionImportVector()
  {
  }

  // -- Functions that describe action --
public:
  // VALIDATE:
  // Each action needs to be validated just before it is posted. This way we
  // enforce that every action that hits the main post_action signal will be
  // a valid action to execute.
  virtual bool validate( Core::ActionContextHandle& context ) override
  {
    if (this->file_path_.empty())
    {
      context->report_error(std::string("File path is empty."));
      return false;
    }

    return true; // validated
  }

  static std::vector<T> read_file(std::string& filename)
  {
    std::vector<T> import_vector;
    std::ifstream input_file;
    input_file.open(filename);
    if (input_file)
    {
      //what precision? give variable
      input_file.precision(10);
      std::copy(std::istream_iterator<T>(input_file),
        std::istream_iterator<T>(),
        std::back_inserter(import_vector));
    }
    else
    {
      CORE_THROW_RUNTIMEERROR("Invalid filepath.");
    }
 

    return import_vector;
  }

  // RUN:
  // Each action needs to have this piece implemented. It spells out how the
  // action is run. It returns whether the action was successful or not.
  virtual bool run( Core::ActionContextHandle& context, Core::ActionResultHandle& result) override
  {
    std::string message("Importing vector...");
    Core::ActionProgressHandle progress(new Core::ActionProgress(message));
    progress->begin_progress_reporting();
   
    try
    {
      auto import_vector = read_file(this->file_path_);

      result.reset(new Core::ActionResult(import_vector));

    }
    catch (...)
    {
      context->report_error("Could not open and read from file: " + this->file_path_);
      return false;
    }

    progress->end_progress_reporting();

    return true;
  }

  void set_file_path( const std::string& file_path ) { file_path_ = file_path; }

  // -- Action parameters --
protected:
  void init_parameters()
  {
    //TODO figure out
    this->add_parameter( this->file_path_ );
  }
private:
  // Where the layer should be imported from
  std::string file_path_;
};

} // end namespace Seg3D

#endif
