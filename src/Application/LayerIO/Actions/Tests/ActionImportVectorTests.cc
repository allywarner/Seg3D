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

#include <gtest/gtest.h>

#include <fstream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>

#include <Application/LayerIO/Actions/ActionImportPoints.h>

#include <Core/Action/ActionContext.h>
#include <Core/Geometry/Point.h>

#include <Testing/Utils/FilesystemPaths.h>

using namespace Core;
using namespace Seg3D;
using namespace Testing::Utils;
using namespace ::testing;

class ActionImportPointsTests : public Test
{
protected:
  virtual void SetUp()
  {
    auto path = testInputDir() / "test" / "import_points" / "import_points_test.txt";
    file_path = path.string();
    points.clear();
    context.reset( new ActionContext() );
    result.reset( new ActionResult() );
  }

  void SinglePointVector()
  {
    points.clear();
    points.push_back( Point(0, 0, 0) );
  }

  std::string dummy_path;
  std::string file_path;
  std::vector< Core::Point > points;
  ActionContextHandle context;
  ActionResultHandle result;
};

TEST(PointReadTest, simple_file)
{
  auto path = testInputDir()/"test"/"import_points"/"import_points_test.txt";

  std::ifstream input_file;
  std::vector<Core::Point> vector;

  input_file.open(path.string());
  input_file.precision(10);
  std::copy(std::istream_iterator<Core::Point>(input_file),
            std::istream_iterator<Core::Point>(),
            std::back_inserter(vector));

  //std::cout << std::setprecision(10);
  //std::copy(vector.begin(),vector.end(),
  //           std::ostream_iterator<Core::Point>(std::cout, "\n"));
  
  std::vector<Point> expected
  {
    {31.000000, 27.673913, 24.155594},
    {31.000000, 42.804348, 41.625125},
    {31.000000, 42.152174, 28.718531},
    {31.000000, 23.500000, 32.108142}
  };

  ASSERT_EQ(expected, vector);

}

TEST_F(ActionImportPointsTests, EmptyAction)
{
  // factory instantiation
  ActionImportPoints action;
}

TEST_F(ActionImportPointsTests, ValidateEmptyParams)
{
  ActionImportPoints action;
  ASSERT_FALSE( action.validate(context) );
}

TEST_F(ActionImportPointsTests, ValidateEmptyFilePath)
{
  ActionImportPoints action;
  action.set_file_path( "" );
  ASSERT_FALSE( action.validate(context) );
}

TEST_F(ActionImportPointsTests, RunInvalidPath)
{
  ActionImportPoints action( " " );
  ASSERT_TRUE( action.validate(context) );
  ASSERT_FALSE( action.run( context, result ) );
}


TEST_F(ActionImportPointsTests, RunValidPath)
{
  auto path = testInputDir()/"test"/"import_points"/"import_points_test.txt";

  ActionImportPoints action( path.string() );
  ASSERT_TRUE( action.validate(context) );
  ASSERT_TRUE( action.run( context, result ) );

  //TODO: Get value out of the action and compare to 'expected'
  //std::ifstream inputfile;
  //inputfile.open(file_path.c_str() );
  //double x = -1.0, y = -1.0, z = -1.0;
  //inputfile >> x >> y >> z;
  //ASSERT_DOUBLE_EQ(x, 0);
  //ASSERT_DOUBLE_EQ(y, 0);
  //ASSERT_DOUBLE_EQ(z, 0);
} 
