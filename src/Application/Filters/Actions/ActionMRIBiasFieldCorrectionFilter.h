/*
 For more information, please see: http://software.sci.utah.edu
 
 The MIT License
 
 Copyright (c) 2018 Scientific Computing and Imaging Institute,
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

#ifndef APPLICATION_FILTERS_ACTIONS_ACTIONMRIBIASFIELDCORRECTIONFILTER_H
#define APPLICATION_FILTERS_ACTIONS_ACTIONMRIBIASFIELDCORRECTIONFILTER_H

#include <Core/Action/Actions.h>
#include <Core/Interface/Interface.h>
#include <Application/Layer/Layer.h>
#include <Application/Layer/LayerAction.h>
#include <Application/Layer/LayerManager.h>

namespace Seg3D
{

class ActionMRIBiasFieldCorrectionFilter : public LayerAction
{
  
CORE_ACTION(
  CORE_ACTION_TYPE( "MRIBiasFieldCorrectionFilter", "ITK filter that corrects nonuniformity in MRI images." )
  CORE_ACTION_ARGUMENT( "layerid", "The layerid on which this filter needs to be run." )
  CORE_ACTION_ARGUMENT( "mask", "The layerid of the mask that needs to be applied." )
  CORE_ACTION_OPTIONAL_ARGUMENT( "replace", "true", "Replace the old layer (true), or add an new layer (false)" )
  CORE_ACTION_OPTIONAL_ARGUMENT( "invert_mask", "false", "Whether the mask needs to be inverted." )
  //CORE_ACTION_OPTIONAL_ARGUMENT( "preserve_data_format", "true", "ITK filters run in floating point percision,"
                               // " this option will convert the result back into the original format." )
  //CORE_ACTION_OPTIONAL_ARGUMENT( "radius", "2.0", "The filter radius." )
  CORE_ACTION_OPTIONAL_ARGUMENT( "sandbox", "-1", "The sandbox in which to run the action." )
  CORE_ACTION_ARGUMENT_IS_NONPERSISTENT( "sandbox" )	
  CORE_ACTION_CHANGES_PROJECT_DATA()
  CORE_ACTION_IS_UNDOABLE()
)
  
  // -- Constructor/Destructor --
public:
  ActionMRIBiasFieldCorrectionFilter()
  {
    this->add_layer_id( this->target_layer_ );
    this->add_layer_id( this->mask_layer_ );
    this->add_parameter( this->replace_ );
	this->add_parameter(this->invert_mask_);
	this->add_parameter(this->sandbox_);
    //this->add_parameter( this->preserve_data_format_ );
    //this->add_parameter( this->radius_ );
  }
  
  // -- Functions that describe action --
public:
  virtual bool validate( Core::ActionContextHandle& context );
  virtual bool run( Core::ActionContextHandle& context, Core::ActionResultHandle& result );
  
  // -- Action parameters --
private:
  
  std::string target_layer_;
  std::string mask_layer_;
  bool replace_;
  bool invert_mask_;
  std::string replace_with_;
  //bool preserve_data_format_;
  //int radius_;
  SandboxID sandbox_;
  
  // -- Dispatch this action from the interface --
public:
  // DISPATCH:
  // Create and dispatch action that inserts the new layer 
    static void Dispatch( Core::ActionContextHandle context, std::string target_layer, std::string mask_layer, bool replace, bool invert_mask, std::string replace_with); //,bool preserve_data_format, double radius );
  
};
	
} // end namespace Seg3D

#endif