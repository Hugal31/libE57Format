// libE57Format testing Copyright © 2022 Andy Maloney <asmaloney@gmail.com>
// SPDX-License-Identifier: MIT

#include <array>
#include <fstream>

#include "gtest/gtest.h"

#include "E57SimpleWriter.h"

#include "Helpers.h"
#include "RandomNum.h"
#include "TestData.h"

namespace
{
   constexpr uint8_t cNumAxes = 3;
   constexpr uint8_t cNumCubeFaces = 6;

   using Point = std::array<float, cNumAxes>;
   using Cube = std::array<Point, 8>;

   constexpr auto cCubeCorners =
      Cube{ Point{ -0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, -0.5f }, { 0.5f, 0.5f, -0.5f }, { -0.5f, 0.5f, -0.5f },
            Point{ -0.5f, 0.5f, 0.5f },   { 0.5f, 0.5f, 0.5f },   { 0.5f, -0.5f, 0.5f }, { -0.5f, -0.5f, 0.5f } };

   constexpr auto cIndexSequence = std::make_index_sequence<3>{};

   template <class T, size_t... Is, size_t N>
   constexpr std::array<T, N> multiply( std::array<T, N> const &src, std::index_sequence<Is...>, T const &mul )
   {
      return std::array<T, N>{ { ( src[Is] * mul )... } };
   }

   // Call a function for each of the corner points for a cube centred on the origin, sized using cubeSize.
   void generateCubeCornerPoints( float inCubeSize, const std::function<void( const Point &inPoint )> &lambda )
   {
      for ( const auto &point : cCubeCorners )
      {
         const auto cSized = multiply( point, cIndexSequence, inCubeSize );
         lambda( cSized );
      }
   }

   // Create "inPointsPerFace" pseudo-random points per face for a cube centred on the origin,
   // sized using cubeSize, and call a function for each of them.
   // This gives us a cube with non-uniform points which can be useful for testing.
   // (Note that we set a seed in each test so the results will always be the same pseudo-random points.)
   void generateCubePoints( float inCubeSize, uint16_t inPointsPerFace,
                            const std::function<void( uint8_t inFace, const Point &inPoint )> &lambda )
   {
      Point point;

      const auto cHalfSize = inCubeSize / 2.0f;

      for ( uint8_t face = 0; face < cNumCubeFaces; ++face )
      {
         const uint8_t axis = face % cNumAxes;

         for ( uint16_t i = 0; i < inPointsPerFace; ++i )
         {
            point[axis] = ( face > 2 ) ? cHalfSize : -cHalfSize;
            point[( axis + 1 ) % cNumAxes] = Random::num() * inCubeSize - cHalfSize;
            point[( axis + 2 ) % cNumAxes] = Random::num() * inCubeSize - cHalfSize;

            lambda( face, point );
         }
      }
   }

   // Set the header to indicate we are using coloured cartesian points.
   // Just used to avoid a bunch of repetition.
   inline void setUsingColouredCartesianPoints( e57::Data3D &ioHeader )
   {
      ioHeader.pointFields.cartesianXField = true;
      ioHeader.pointFields.cartesianYField = true;
      ioHeader.pointFields.cartesianZField = true;

      ioHeader.pointFields.colorRedField = true;
      ioHeader.pointFields.colorGreenField = true;
      ioHeader.pointFields.colorBlueField = true;

      ioHeader.colorLimits.colorRedMaximum = 255;
      ioHeader.colorLimits.colorGreenMaximum = 255;
      ioHeader.colorLimits.colorBlueMaximum = 255;
   }

   // Fill in a point and its colour given an index, which face it is on, and the data.
   template <typename T>
   inline void fillColouredCartesianPoint( e57::Data3DPointsData_t<T> &ioPointsData, int64_t inIndex, uint8_t inFace,
                                           const Point &inPoint )
   {
      static_assert( std::is_floating_point<T>::value, "Floating point type required." );

      ioPointsData.cartesianX[inIndex] = inPoint[0];
      ioPointsData.cartesianY[inIndex] = inPoint[1];
      ioPointsData.cartesianZ[inIndex] = inPoint[2];

      switch ( inFace )
      {
         case 0:
         case 3:
         {
            ioPointsData.colorRed[inIndex] = 0;
            ioPointsData.colorGreen[inIndex] = 0;
            ioPointsData.colorBlue[inIndex] = 255;
            break;
         }
         case 1:
         case 4:
         {
            ioPointsData.colorRed[inIndex] = 0;
            ioPointsData.colorGreen[inIndex] = 255;
            ioPointsData.colorBlue[inIndex] = 0;
            break;
         }
         case 2:
         case 5:
         {
            ioPointsData.colorRed[inIndex] = 255;
            ioPointsData.colorGreen[inIndex] = 0;
            ioPointsData.colorBlue[inIndex] = 0;
            break;
         }
      }
   }
}

TEST( SimpleWriter, PathError )
{
   e57::WriterOptions options;
   options.guid = "File GUID";

   E57_ASSERT_THROW( e57::Writer( "./no-path/empty.e57", options ) );
}

TEST( SimpleWriter, Empty )
{
   e57::WriterOptions options;
   options.guid = "Empty File GUID";

   E57_ASSERT_NO_THROW( e57::Writer writer( "./empty.e57", options ) );
}

// Write a coloured cube of points using doubles.
TEST( SimpleWriter, ColouredCubeDouble )
{
   Random::seed( 42 );

   e57::WriterOptions options;
   options.guid = "Coloured Cube File GUID";

   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW( writer = new e57::Writer( "./ColouredCubeDouble.e57", options ) );

   constexpr uint16_t cNumPointsPerFace = 1280;
   constexpr uint16_t cNumPoints = cNumPointsPerFace * cNumCubeFaces;

   e57::Data3D header;
   header.guid = "Coloured Cube Double Scan Header GUID";
   header.description = "libE57Format test: cube of coloured points using doubles";
   header.pointCount = cNumPoints;

   // setting this to < 0.0 indicates we want to write doubles
   header.pointFields.pointRangeScaledInteger = -1.0;

   setUsingColouredCartesianPoints( header );

   e57::Data3DPointsData_d pointsData( header );

   // reset these so we can calculate them using min/max
   header.pointFields.pointRangeMinimum = e57::E57_DOUBLE_MAX;
   header.pointFields.pointRangeMaximum = e57::E57_DOUBLE_MIN;

   int64_t i = 0;
   auto writePointLambda = [&]( uint8_t inFace, const Point &inPoint ) {
      ASSERT_LE( i, cNumPoints );

      fillColouredCartesianPoint( pointsData, i, inFace, inPoint );

      header.pointFields.pointRangeMinimum = std::min( pointsData.cartesianX[i], header.pointFields.pointRangeMinimum );
      header.pointFields.pointRangeMinimum = std::min( pointsData.cartesianY[i], header.pointFields.pointRangeMinimum );
      header.pointFields.pointRangeMinimum = std::min( pointsData.cartesianZ[i], header.pointFields.pointRangeMinimum );

      header.pointFields.pointRangeMaximum = std::max( pointsData.cartesianX[i], header.pointFields.pointRangeMaximum );
      header.pointFields.pointRangeMaximum = std::max( pointsData.cartesianY[i], header.pointFields.pointRangeMaximum );
      header.pointFields.pointRangeMaximum = std::max( pointsData.cartesianZ[i], header.pointFields.pointRangeMaximum );

      ++i;
   };

   generateCubePoints( 1.0, cNumPointsPerFace, writePointLambda );

   const int64_t cScanIndex1 = writer->NewData3D( header );

   e57::CompressedVectorWriter dataWriter = writer->SetUpData3DPointsData( cScanIndex1, cNumPoints, pointsData );

   dataWriter.write( cNumPoints );
   dataWriter.close();

   delete writer;
}

// Write a coloured cube of points using floats.
TEST( SimpleWriter, ColouredCubeFloat )
{
   Random::seed( 42 );

   e57::WriterOptions options;
   options.guid = "Coloured Cube File GUID";

   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW( writer = new e57::Writer( "./ColouredCubeFloat.e57", options ) );

   constexpr uint16_t cNumPointsPerFace = 1280;
   constexpr uint16_t cNumPoints = cNumPointsPerFace * cNumCubeFaces;

   e57::Data3D header;
   header.guid = "Coloured Cube Float Scan Header GUID";
   header.description = "libE57Format test: cube of coloured points using floats";
   header.pointCount = cNumPoints;

   setUsingColouredCartesianPoints( header );

   e57::Data3DPointsData pointsData( header );

   // reset these so we can calculate them using min/max
   header.pointFields.pointRangeMinimum = e57::E57_FLOAT_MAX;
   header.pointFields.pointRangeMaximum = e57::E57_FLOAT_MIN;

   int64_t i = 0;
   auto writePointLambda = [&]( uint8_t inFace, const Point &inPoint ) {
      ASSERT_LE( i, cNumPoints );

      fillColouredCartesianPoint( pointsData, i, inFace, inPoint );

      header.pointFields.pointRangeMinimum =
         std::min( static_cast<double>( pointsData.cartesianX[i] ), header.pointFields.pointRangeMinimum );
      header.pointFields.pointRangeMinimum =
         std::min( static_cast<double>( pointsData.cartesianY[i] ), header.pointFields.pointRangeMinimum );
      header.pointFields.pointRangeMinimum =
         std::min( static_cast<double>( pointsData.cartesianZ[i] ), header.pointFields.pointRangeMinimum );

      header.pointFields.pointRangeMaximum =
         std::max( static_cast<double>( pointsData.cartesianX[i] ), header.pointFields.pointRangeMaximum );
      header.pointFields.pointRangeMaximum =
         std::max( static_cast<double>( pointsData.cartesianY[i] ), header.pointFields.pointRangeMaximum );
      header.pointFields.pointRangeMaximum =
         std::max( static_cast<double>( pointsData.cartesianZ[i] ), header.pointFields.pointRangeMaximum );

      ++i;
   };

   generateCubePoints( 1.0, cNumPointsPerFace, writePointLambda );

   const int64_t cScanIndex1 = writer->NewData3D( header );

   e57::CompressedVectorWriter dataWriter = writer->SetUpData3DPointsData( cScanIndex1, cNumPoints, pointsData );

   dataWriter.write( cNumPoints );
   dataWriter.close();

   delete writer;
}

// Write a cube of points using scaled ints.
TEST( SimpleWriter, ColouredCubeScaledInt )
{
   Random::seed( 42 );

   e57::WriterOptions options;
   options.guid = "Coloured Cube Scaled Int File GUID";

   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW( writer = new e57::Writer( "./ColouredCubeScaledInt.e57", options ) );

   constexpr uint16_t cNumPointsPerFace = 1280;
   constexpr uint16_t cNumPoints = cNumPointsPerFace * cNumCubeFaces;

   e57::Data3D header;
   header.guid = "Cube Scaled Int Scan Header GUID";
   header.description = "libE57Format test: cube of coloured points using scaled integers";
   header.pointCount = cNumPoints;

   // setting this to > 0.0 indicates we want to write scaled ints and to use this as the scale
   header.pointFields.pointRangeScaledInteger = 0.001;

   setUsingColouredCartesianPoints( header );

   e57::Data3DPointsData_d pointsData( header );

   // reset these so we can calculate them using min/max
   header.pointFields.pointRangeMinimum = e57::E57_DOUBLE_MAX;
   header.pointFields.pointRangeMaximum = e57::E57_DOUBLE_MIN;

   int64_t i = 0;
   auto writePointLambda = [&]( uint8_t inFace, const Point &inPoint ) {
      ASSERT_LE( i, cNumPoints );

      fillColouredCartesianPoint( pointsData, i, inFace, inPoint );

      header.pointFields.pointRangeMinimum = std::min( pointsData.cartesianX[i], header.pointFields.pointRangeMinimum );
      header.pointFields.pointRangeMinimum = std::min( pointsData.cartesianY[i], header.pointFields.pointRangeMinimum );
      header.pointFields.pointRangeMinimum = std::min( pointsData.cartesianZ[i], header.pointFields.pointRangeMinimum );

      header.pointFields.pointRangeMaximum = std::max( pointsData.cartesianX[i], header.pointFields.pointRangeMaximum );
      header.pointFields.pointRangeMaximum = std::max( pointsData.cartesianY[i], header.pointFields.pointRangeMaximum );
      header.pointFields.pointRangeMaximum = std::max( pointsData.cartesianZ[i], header.pointFields.pointRangeMaximum );

      ++i;
   };

   generateCubePoints( 1.0, cNumPointsPerFace, writePointLambda );

   const int64_t cScanIndex1 = writer->NewData3D( header );

   e57::CompressedVectorWriter dataWriter = writer->SetUpData3DPointsData( cScanIndex1, cNumPoints, pointsData );

   dataWriter.write( cNumPoints );
   dataWriter.close();

   delete writer;
}

TEST( SimpleWriter, MultipleScans )
{
   e57::WriterOptions options;
   options.guid = "Multiple Scans File GUID";

   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW( writer = new e57::Writer( "./MultipleScans.e57", options ) );

   constexpr int cNumPoints = 8;

   e57::Data3D header;
   header.pointCount = cNumPoints;
   header.pointFields.cartesianXField = true;
   header.pointFields.cartesianYField = true;
   header.pointFields.cartesianZField = true;

   e57::Data3DPointsData pointsData( header );

   // scan 1
   header.guid = "Multiple Scans Scan 1 Header GUID";

   const int64_t cScanIndex1 = writer->NewData3D( header );

   int64_t i = 0;
   auto writePointLambda = [&]( const Point &point ) {
      pointsData.cartesianX[i] = point[0];
      pointsData.cartesianY[i] = point[1];
      pointsData.cartesianZ[i] = point[2];
      ++i;
   };

   generateCubeCornerPoints( 1.0, writePointLambda );

   e57::CompressedVectorWriter dataWriter = writer->SetUpData3DPointsData( cScanIndex1, cNumPoints, pointsData );

   dataWriter.write( cNumPoints );
   dataWriter.close();

   // scan 2
   header.guid = "Multiple Scans Scan 2 Header GUID";

   const int64_t cScanIndex2 = writer->NewData3D( header );

   i = 0;
   generateCubeCornerPoints( 0.5, writePointLambda );

   dataWriter = writer->SetUpData3DPointsData( cScanIndex2, cNumPoints, pointsData );

   dataWriter.write( cNumPoints );
   dataWriter.close();

   delete writer;
}

// https://github.com/asmaloney/libE57Format/issues/26
TEST( SimpleWriter, ChineseFileName )
{
   e57::WriterOptions options;
   options.guid = "Chinese File Name File GUID";

   E57_ASSERT_NO_THROW( e57::Writer writer( "./测试点云.e57", options ) );
}

// https://github.com/asmaloney/libE57Format/issues/69
TEST( SimpleWriter, WriteUmlautFileName )
{
   e57::WriterOptions options;
   options.guid = "Umlaut File Name File GUID";

   E57_ASSERT_NO_THROW( e57::Writer writer( "./test filename äöü.e57", options ) );
}

TEST( SimpleWriter, CartesianPoints )
{
   e57::WriterOptions options;
   options.guid = "Cartesian Points File GUID";

   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW( writer = new e57::Writer( "./CartesianPoints-1025.e57", options ) );

   constexpr int64_t cNumPoints = 1025;

   e57::Data3D header;
   header.guid = "Cartesian Points Header GUID";
   header.pointCount = cNumPoints;
   header.pointFields.cartesianXField = true;
   header.pointFields.cartesianYField = true;
   header.pointFields.cartesianZField = true;

   const int64_t scanIndex = writer->NewData3D( header );

   e57::Data3DPointsData pointsData( header );

   for ( int64_t i = 0; i < cNumPoints; ++i )
   {
      auto floati = static_cast<float>( i );
      pointsData.cartesianX[i] = floati;
      pointsData.cartesianY[i] = floati;
      pointsData.cartesianZ[i] = floati;
   }

   e57::CompressedVectorWriter dataWriter = writer->SetUpData3DPointsData( scanIndex, cNumPoints, pointsData );

   dataWriter.write( cNumPoints );
   dataWriter.close();

   delete writer;
}

TEST( SimpleWriter, ColouredCartesianPoints )
{
   e57::WriterOptions options;
   options.guid = "Coloured Cartesian Points File GUID";

   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW( writer = new e57::Writer( "./ColouredCartesianPoints-1025.e57", options ) );

   constexpr int64_t cNumPoints = 1025;

   e57::Data3D header;
   header.guid = "Coloured Cartesian Points Header GUID";
   header.pointCount = cNumPoints;

   setUsingColouredCartesianPoints( header );

   const int64_t scanIndex = writer->NewData3D( header );

   e57::Data3DPointsData pointsData( header );

   for ( int64_t i = 0; i < cNumPoints; ++i )
   {
      auto floati = static_cast<float>( i );
      pointsData.cartesianX[i] = floati;
      pointsData.cartesianY[i] = floati;
      pointsData.cartesianZ[i] = floati;

      pointsData.colorRed[i] = 0;
      pointsData.colorGreen[i] = 0;
      pointsData.colorBlue[i] = 255;
   }

   e57::CompressedVectorWriter dataWriter = writer->SetUpData3DPointsData( scanIndex, cNumPoints, pointsData );

   dataWriter.write( cNumPoints );
   dataWriter.close();

   delete writer;
}

TEST( SimpleWriterData, VisualRefImage )
{
   e57::WriterOptions options;
   options.guid = "Visual Reference Image File GUID";

   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW( writer = new e57::Writer( "./VisualRefImage.e57", options ) );

   std::ifstream image( TestData::Path() + "/images/image.jpg", std::ifstream::ate | std::ifstream::binary );

   ASSERT_EQ( image.rdstate(), std::ios_base::goodbit );

   const int64_t cImageSize = image.tellg();

   image.clear();
   image.seekg( 0 );

   auto imageBuffer = new char[cImageSize];

   image.read( imageBuffer, cImageSize );

   ASSERT_EQ( image.rdstate(), std::ios_base::goodbit );

   e57::Image2D image2DHeader;
   image2DHeader.name = "JPEG Image Test";
   image2DHeader.guid = "Visual Reference Image - JPEG Image GUID";
   image2DHeader.description = "JPEG image test";
   image2DHeader.visualReferenceRepresentation.imageWidth = 225;
   image2DHeader.visualReferenceRepresentation.imageHeight = 300;
   image2DHeader.visualReferenceRepresentation.jpegImageSize = cImageSize;

   int64_t imageIndex = writer->NewImage2D( image2DHeader );

   writer->WriteImage2DData( imageIndex, e57::E57_JPEG_IMAGE, e57::E57_VISUAL, imageBuffer, 0, cImageSize );

   delete[] imageBuffer;

   delete writer;
}
