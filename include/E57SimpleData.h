/*
 * E57Simple - public header of E57 Simple API for reading/writing .e57 files.
 *
 * Copyright (c) 2010 Stan Coleby (scoleby@intelisum.com)
 * Copyright (c) 2020 PTC Inc.
 * Copyright (c) 2022 Andy Maloney <asmaloney@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#pragma once

//! @file
//! @brief Data structures for E57 Simple API

#include "E57Format.h"

namespace e57
{
   //! Indicates to use FloatNode instead of ScaledIntegerNode in fields that can use both.
   constexpr double E57_NOT_SCALED_USE_FLOAT = 0.0;

   //! Indicates to use ScaledIntegerNode instead of FloatNode in fields that can use both.
   constexpr double E57_NOT_SCALED_USE_INTEGER = -1.0;

   //! @cond documentNonPublic   The following isn't part of the API, and isn't documented.
   class ReaderImpl;
   class WriterImpl;
   //! @endcond

   //! @brief Defines a rigid body translation in Cartesian coordinates.
   struct E57_DLL Translation
   {
      double x = 0.0; //!< The X coordinate of the translation (in meters)
      double y = 0.0; //!< The Y coordinate of the translation (in meters)
      double z = 0.0; //!< The Z coordinate of the translation (in meters)

      bool operator==( const Translation &rhs ) const
      {
         return ( x == rhs.x ) && ( y == rhs.y ) && ( z == rhs.z );
      }

      bool operator!=( const Translation &rhs ) const
      {
         return !operator==( rhs );
      }

      static Translation identity()
      {
         return {};
      }
   };

   //! @brief Represents a rigid body rotation.
   struct E57_DLL Quaternion
   {
      double w = 1.0; //!< The real part of the quaternion (shall be nonnegative)
      double x = 0.0; //!< The i coefficient of the quaternion
      double y = 0.0; //!< The j coefficient of the quaternion
      double z = 0.0; //!< The k coefficient of the quaternion

      bool operator==( const Quaternion &rhs ) const
      {
         return ( w == rhs.w ) && ( x == rhs.x ) && ( y == rhs.y ) && ( z == rhs.z );
      }

      bool operator!=( const Quaternion &rhs ) const
      {
         return !operator==( rhs );
      }

      static Quaternion identity()
      {
         return {};
      }
   };

   //! @brief Defines a rigid body transform in cartesian coordinates.
   struct E57_DLL RigidBodyTransform
   {
      Quaternion rotation;     //!< A unit quaternion representing the rotation, R, of the transform
      Translation translation; //!< The translation point vector, t, of the transform

      bool operator==( const RigidBodyTransform &rhs ) const
      {
         return ( rotation == rhs.rotation ) && ( translation == rhs.translation );
      }

      bool operator!=( const RigidBodyTransform &rhs ) const
      {
         return !operator==( rhs );
      }

      static RigidBodyTransform identity()
      {
         return { Quaternion::identity(), Translation::identity() };
      }
   };

   //! @brief Specifies an axis-aligned box in local cartesian coordinates.
   struct E57_DLL CartesianBounds
   {
      double xMinimum = -E57_DOUBLE_MAX; //!< The minimum extent of the bounding box in the X direction
      double xMaximum = E57_DOUBLE_MAX;  //!< The maximum extent of the bounding box in the X direction
      double yMinimum = -E57_DOUBLE_MAX; //!< The minimum extent of the bounding box in the Y direction
      double yMaximum = E57_DOUBLE_MAX;  //!< The maximum extent of the bounding box in the Y direction
      double zMinimum = -E57_DOUBLE_MAX; //!< The minimum extent of the bounding box in the Z direction
      double zMaximum = E57_DOUBLE_MAX;  //!< The maximum extent of the bounding box in the Z direction

      bool operator==( const CartesianBounds &rhs ) const
      {
         return ( xMinimum == rhs.xMinimum ) && ( xMaximum == rhs.xMaximum ) && ( yMinimum == rhs.yMinimum ) &&
                ( yMaximum == rhs.yMaximum ) && ( zMinimum == rhs.zMinimum ) && ( zMaximum == rhs.zMaximum );
      }

      bool operator!=( const CartesianBounds &rhs ) const
      {
         return !operator==( rhs );
      }
   };

   //! @brief Stores the bounds of some data in spherical coordinates.
   struct E57_DLL SphericalBounds
   {
      SphericalBounds();       // constructor in the cpp to avoid exposing M_PI
      double rangeMinimum;     //!< The minimum extent of the bounding region in the r direction
      double rangeMaximum;     //!< The maximum extent of the bounding region in the r direction
      double elevationMinimum; //!< The minimum extent of the bounding region from the horizontal plane
      double elevationMaximum; //!< The maximum extent of the bounding region from the horizontal plane
      double azimuthStart; //!< The starting azimuth angle defining the extent of the bounding region around the z axis
      double azimuthEnd;   //!< The ending azimuth angle defining the extent of the bounding region around the z axis

      bool operator==( const SphericalBounds &rhs ) const
      {
         return ( rangeMinimum == rhs.rangeMinimum ) && ( rangeMaximum == rhs.rangeMaximum ) &&
                ( elevationMinimum == rhs.elevationMinimum ) && ( elevationMaximum == rhs.elevationMaximum ) &&
                ( azimuthStart == rhs.azimuthStart ) && ( azimuthEnd == rhs.azimuthEnd );
      }

      bool operator!=( const SphericalBounds &rhs ) const
      {
         return !operator==( rhs );
      }
   };

   //! @brief Stores the minimum and maximum of rowIndex, columnIndex, and returnIndex fields for a set of points.
   struct E57_DLL IndexBounds
   {
      int64_t rowMinimum = 0; //!< The minimum rowIndex value of any point represented by this IndexBounds object
      int64_t rowMaximum = 0; //!< The maximum rowIndex value of any point represented by this IndexBounds object

      int64_t columnMinimum = 0; //!< The minimum columnIndex value of any point represented by this IndexBounds object
      int64_t columnMaximum = 0; //!< The maximum columnIndex value of any point represented by this IndexBounds object

      int64_t returnMinimum = 0; //!< The minimum returnIndex value of any point represented by this IndexBounds object
      int64_t returnMaximum = 0; //!< The maximum returnIndex value of any point represented by this IndexBounds object

      bool operator==( const IndexBounds &rhs ) const
      {
         return ( rowMinimum == rhs.rowMinimum ) && ( rowMaximum == rhs.rowMaximum ) &&
                ( columnMinimum == rhs.columnMinimum ) && ( columnMaximum == rhs.columnMaximum ) &&
                ( returnMinimum == rhs.returnMinimum ) && ( returnMaximum == rhs.returnMaximum );
      }

      bool operator!=( const IndexBounds &rhs ) const
      {
         return !operator==( rhs );
      }
   };

   //! @brief Specifies the limits for the value of signal intensity that a sensor is capable of producing
   struct E57_DLL IntensityLimits
   {
      double intensityMinimum = 0.0; //!< The minimum producible intensity value. Unit is unspecified.
      double intensityMaximum = 0.0; //!< The maximum producible intensity value. Unit is unspecified.

      bool operator==( const IntensityLimits &rhs ) const
      {
         return ( intensityMinimum == rhs.intensityMinimum ) && ( intensityMaximum == rhs.intensityMaximum );
      }

      bool operator!=( const IntensityLimits &rhs ) const
      {
         return !operator==( rhs );
      }
   };

   //! @brief Specifies the limits for the value of red, green, and blue color that a sensor is capable of producing.
   struct E57_DLL ColorLimits
   {
      double colorRedMinimum = 0.0; //!< The minimum producible red color value. Unit is unspecified.
      double colorRedMaximum = 0.0; //!< The maximum producible red color value. Unit is unspecified.

      double colorGreenMinimum = 0.0; //!< The minimum producible green color value. Unit is unspecified.
      double colorGreenMaximum = 0.0; //!< The maximum producible green color value. Unit is unspecified.

      double colorBlueMinimum = 0.0; //!< The minimum producible blue color value. Unit is unspecified.
      double colorBlueMaximum = 0.0; //!< The maximum producible blue color value. Unit is unspecified.

      bool operator==( const ColorLimits &rhs ) const
      {
         return ( colorRedMinimum == rhs.colorRedMinimum ) && ( colorRedMaximum == rhs.colorRedMaximum ) &&
                ( colorGreenMinimum == rhs.colorGreenMinimum ) && ( colorGreenMaximum == rhs.colorGreenMaximum ) &&
                ( colorBlueMinimum == rhs.colorBlueMinimum ) && ( colorBlueMaximum == rhs.colorBlueMaximum );
      }

      bool operator!=( const ColorLimits &rhs ) const
      {
         return !operator==( rhs );
      }
   };

   //! @brief Encodes date and time.
   //! @details The date and time is encoded using a single floating point number, stored as an E57 Float element which
   //! is based on the Global Positioning System (GPS) time scale.
   struct E57_DLL DateTime
   {
      //! The time, in seconds, since GPS time was zero. This time specification may include fractions of a second.
      double dateTimeValue = 0.0;

      //! This element should be present, and its value set to 1 if, and only if, the time stored in the
      //! dateTimeValue element is obtained from an atomic clock time source. Shall be either 0 or 1.
      int32_t isAtomicClockReferenced = 0;

      bool operator==( const DateTime &rhs ) const
      {
         return ( dateTimeValue == rhs.dateTimeValue ) && ( isAtomicClockReferenced == rhs.isAtomicClockReferenced );
      }

      bool operator!=( const DateTime &rhs ) const
      {
         return !operator==( rhs );
      }
   };

   //! @brief Stores the top-level information for the XML section of the file.
   struct E57_DLL E57Root
   {
      ustring formatName;        //!< Must contain the string "ASTM E57 3D Imaging Data File"
      ustring guid;              //!< A globally unique identification string for the current version of the file
      uint32_t versionMajor = 1; //!< Major version number (should be 1)
      uint32_t versionMinor = 0; //!< Minor version number (should be 0)
      ustring e57LibraryVersion; //!< The version identifier for the E57 file format library that wrote the file.
      DateTime creationDateTime; //!< Date/time that the file was created
      int64_t data3DSize = 0;    //!< Size of the Data3D vector for storing 3D imaging data
      int64_t images2DSize = 0;  //!< Size of the Images2D vector for storing 2D images from a camera or similar device.
      ustring coordinateMetadata; //!< Information describing the Coordinate Reference System to be used for the file
   };

   //! @brief Stores information about a single group of points in a row or column
   struct E57_DLL LineGroupRecord
   {
      //! The value of the identifying element of all members in this group. Shall be in the interval [0, 2^63).
      int64_t idElementValue = 0;

      //! The record number of the first point in the continuous interval. Shall be in the interval [0, 2^63).
      int64_t startPointIndex = 0;

      //! The number of PointRecords in the group. Shall be in the interval [1, 2^63). May be zero.
      int64_t pointCount = 0;

      //! The bounding box (in Cartesian coordinates) of all points in the group (in the local coordinate system of the
      //! points).
      CartesianBounds cartesianBounds;

      //! The bounding region (in spherical coordinates) of all the points in the group (in the local coordinate system
      //! of the points).
      SphericalBounds sphericalBounds;
   };

   //! @brief Stores a set of point groups organized by the rowIndex or columnIndex attribute of the PointRecord
   struct E57_DLL GroupingByLine
   {
      //! The name of the PointRecord element that identifies which group the point is in. The value of this string must
      //! be "rowIndex" or "columnIndex".
      ustring idElementName;

      int64_t groupsSize = 0;     //!< Size of the groups compressedVector of LineGroupRecord structures
      int64_t pointCountSize = 0; //!< The size value for the LineGroupRecord::pointCount.
   };

   //! @brief Supports the division of points within an Data3D into logical groupings
   struct E57_DLL PointGroupingSchemes
   {
      GroupingByLine groupingByLine; //!< Grouping information by row or column index
   };

   //! @brief Used to interrogate if standardized fields are available
   struct E57_DLL PointStandardizedFieldsAvailable
   {
      bool cartesianXField = false;            //!< Indicates that the PointRecord cartesianX field is active
      bool cartesianYField = false;            //!< Indicates that the PointRecord cartesianY field is active
      bool cartesianZField = false;            //!< Indicates that the PointRecord cartesianZ field is active
      bool cartesianInvalidStateField = false; //!< Indicates that the PointRecord cartesianInvalidState field is active

      bool sphericalRangeField = false;        //!< Indicates that the PointRecord sphericalRange field is active
      bool sphericalAzimuthField = false;      //!< Indicates that the PointRecord sphericalAzimuth field is active
      bool sphericalElevationField = false;    //!< Indicates that the PointRecord sphericalElevation field is active
      bool sphericalInvalidStateField = false; //!< Indicates that the PointRecord sphericalInvalidState field is active

      //! Indicates that the PointRecord cartesian and range fields should be configured with this minimum value e.g.
      //! E57_FLOAT_MIN or E57_DOUBLE_MIN. If using a ScaledIntegerNode then this needs to be a minimum range value.
      double pointRangeMinimum = E57_DOUBLE_MIN;

      //! Indicates that the PointRecord cartesian and range fields should be configured with this maximum value e.g.
      //! E57_FLOAT_MAX or E57_DOUBLE_MAX. If using a ScaledIntegerNode then this needs to be a maximum range value.
      double pointRangeMaximum = E57_DOUBLE_MAX;

      //! @brief Controls the type of Node used for the PointRecord cartesian and range fields
      //! @details The value determines which type of Node to use and whether to use floats or doubles.
      //! Value | Node Type
      //! -- | --
      //! &lt; 0.0 | FloatNode using doubles
      //! == 0.0 (e57::E57_NOT_SCALED_USE_FLOAT) | FloatNode using floats (@em default)
      //! &gt; 0.0 | ScaledIntegerNode with the value as the scale setting
      double pointRangeScaledInteger = E57_NOT_SCALED_USE_FLOAT;

      //! Indicates that the PointRecord angle fields should be configured with this minimum value E57_FLOAT_MIN or
      //! E57_DOUBLE_MIN. If using a ScaledIntegerNode then this needs to be a minimum angle value.
      double angleMinimum = E57_DOUBLE_MIN;

      //! Indicates that the PointRecord angle fields should be configured with this maximum value e.g. E57_FLOAT_MAX or
      //! E57_DOUBLE_MAX. If using a ScaledIntegerNode then this needs to be a maximum angle value.
      double angleMaximum = E57_DOUBLE_MAX;

      //! @brief Controls the type of Node used for the PointRecord angle fields
      //! @details The value determines which type of Node to use and whether to use floats or doubles.
      //! Value | Node Type
      //! -- | --
      //! &lt; 0.0 | FloatNode using doubles
      //! == 0.0 (e57::E57_NOT_SCALED_USE_FLOAT) | FloatNode using floats (@em default)
      //! &gt; 0.0 | ScaledIntegerNode with the value as the scale setting
      double angleScaledInteger = E57_NOT_SCALED_USE_FLOAT;

      bool rowIndexField = false; //!< Indicates that the PointRecord @a rowIndex field is active

      //! Indicates that the PointRecord @a rowIndex fields should be configured with this maximum value where the
      //! minimum will be set to 0.
      uint32_t rowIndexMaximum = E57_UINT32_MAX;

      bool columnIndexField = false; //!< Indicates that the PointRecord @a columnIndex field is active

      //! Indicates that the PointRecord @a columnIndex fields should be configured with this maximum value where the
      //! minimum will be set to 0.
      uint32_t columnIndexMaximum = E57_UINT32_MAX;

      bool returnIndexField = false;         //!< Indicates that the PointRecord @a returnIndex field is active
      bool returnCountField = false;         //!< Indicates that the PointRecord @a returnCount field is active
      uint8_t returnMaximum = E57_UINT8_MAX; //!< Indicates that the PointRecord return fields should be configured
                                             //!< with this maximum value where the minimum will be set to 0.

      bool timeStampField = false;          //!< Indicates that the PointRecord @a timeStamp field is active
      bool isTimeStampInvalidField = false; //!< Indicates that the PointRecord @a isTimeStampInvalid field is active

      //! Indicates that the PointRecord @a timeStamp fields should be configured with this minimum value e.g.
      //! E57_UINT32_MIN, E57_DOUBLE_MIN or E57_DOUBLE_MIN. If using a ScaledIntegerNode then this needs to be a minimum
      //! time value.
      double timeMinimum = E57_DOUBLE_MIN;

      //! Indicates that the PointRecord @a timeStamp fields should be configured with this maximum value. e.g.
      //! E57_UINT32_MAX, E57_DOUBLE_MAX or E57_DOUBLE_MAX.
      double timeMaximum = E57_DOUBLE_MAX;

      //! @brief Controls the type of Node used for the PointRecord @a timeStamp fields
      //! @details The value determines which type of Node to use and whether to use floats or doubles.
      //! Value  | Node Type
      //! -- | --
      //! &lt; 0.0 | IntegerNode
      //! == 0.0 (e57::E57_NOT_SCALED_USE_FLOAT) | FloatNode using floats if (::timeMaximum == E57_FLOAT_MAX)
      //! == 0.0 | FloatNode using doubles if (::timeMaximum == E57_DOUBLE_MAX) (@em default)
      //! &gt; 0.0 | ScaledIntegerNode with the value as the scale setting
      double timeScaledInteger = E57_NOT_SCALED_USE_FLOAT;

      bool intensityField = false;          //!< Indicates that the PointRecord @a intensity field is active
      bool isIntensityInvalidField = false; //!< Indicates that the PointRecord @a isIntensityInvalid field is active

      //! @brief Controls the type of Node used for the PointRecord @a intensity fields
      //! @details The value determines which type of Node to use.
      //! Value | Node Type
      //! -- | --
      //! &lt; 0.0 | IntegerNode
      //! == 0.0 (e57::E57_NOT_SCALED_USE_FLOAT) | FloatNode using floats (@em default)
      //! &gt; 0.0 | ScaledIntegerNode with the value as the scale setting
      double intensityScaledInteger = E57_NOT_SCALED_USE_INTEGER;

      bool colorRedField = false;       //!< Indicates that the PointRecord @a colorRed field is active
      bool colorGreenField = false;     //!< Indicates that the PointRecord @a colorGreen field is active
      bool colorBlueField = false;      //!< Indicates that the PointRecord @a colorBlue field is active
      bool isColorInvalidField = false; //!< Indicates that the PointRecord @a isColorInvalid field is active

      bool normalXField = false; //!< Indicates that the PointRecord @a nor:normalX field is active
      bool normalYField = false; //!< Indicates that the PointRecord @a nor:normalY field is active
      bool normalZField = false; //!< Indicates that the PointRecord @a nor:normalZ field is active
   };

   //! @brief Stores the top-level information for a single lidar scan
   struct E57_DLL Data3D
   {
      ustring name; //!< A user-defined name for the Data3D.
      ustring guid; //!< A globally unique identification string for the current version of the Data3D object

      //! A vector of globally unique identification Strings from which the points in this Data3D originated.
      std::vector<ustring> originalGuids;

      ustring description; //!< A user-defined description of the Image

      ustring sensorVendor; //!< The name of the manufacturer for the sensor used to collect the points in this Data3D.
      ustring sensorModel;  //!< The model name or number for the sensor.
      ustring sensorSerialNumber;    //!< The serial number for the sensor.
      ustring sensorHardwareVersion; //!< The version number for the sensor hardware at the time of data collection.
      ustring sensorSoftwareVersion; //!< The version number for the software used for the data collection.
      ustring sensorFirmwareVersion; //!< The version number for the firmware installed in the sensor at the time of
                                     //!< data collection.

      //! The ambient temperature, measured at the sensor, at the time of data collection (in degrees Celsius).
      float temperature = E57_FLOAT_MAX;

      //! The percentage relative humidity, measured at the sensor, at the time of data collection. Shall be in the
      //! interval [0, 100].
      float relativeHumidity = E57_FLOAT_MAX;

      //! The atmospheric pressure, measured at the sensor, at the time of data collection (in Pascals). Shall be
      //! positive.
      float atmosphericPressure = E57_FLOAT_MAX;

      DateTime acquisitionStart; //!< The start date and time that the data was acquired.
      DateTime acquisitionEnd;   //!< The end date and time that the data was acquired.

      //! A rigid body transform that describes the coordinate frame of the 3D imaging system origin in the file-level
      //! coordinate system.
      RigidBodyTransform pose;

      IndexBounds indexBounds; //!< The bounds of the row, column, and return number of all the points in this Data3D.

      //! The bounding region (in cartesian coordinates) of all the points in this Data3D (in the local coordinate
      //! system of the points).
      CartesianBounds cartesianBounds;

      //! The bounding region (in spherical coordinates) of all the points in this Data3D (in the local coordinate
      //! system of the points).
      SphericalBounds sphericalBounds;

      //! The limits for the value of signal intensity that the sensor is capable of producing.
      IntensityLimits intensityLimits;

      //! The limits for the value of red, green, and blue color that the sensor is capable of producing.
      ColorLimits colorLimits;

      PointGroupingSchemes pointGroupingSchemes;    //!< The defined schemes that group points in different ways
      PointStandardizedFieldsAvailable pointFields; //!< The active fields used in the WritePoints function.

      //! The number of points in the Data3D.
      int64_t pointCount = 0;
   };

   //! @brief Stores pointers to user-provided buffers
   template <typename COORDTYPE = float> struct E57_DLL Data3DPointsData_t
   {
      static_assert( std::is_floating_point<COORDTYPE>::value, "Floating point type required." );

      //! @brief Default constructor does not manage any memory or adjust min/max for floats.
      Data3DPointsData_t() = default;

      //! @brief Constructor which allocates buffers for all valid fields in the given Data3D header
      //! This constructor will also adjust the min/max fields in the data3D pointFields if we are using floats.
      //! @param [in] data3D Completed header which indicates the fields we are using
      explicit Data3DPointsData_t( e57::Data3D &data3D );

      //! @brief Destructor will delete any memory allocated using the Data3DPointsData_t( const e57::Data3D & )
      //! constructor
      ~Data3DPointsData_t();

      //! Pointer to a buffer with the X coordinate (in meters) of the point in Cartesian coordinates
      COORDTYPE *cartesianX = nullptr;

      //! Pointer to a buffer with the Y coordinate (in meters) of the point in Cartesian coordinates
      COORDTYPE *cartesianY = nullptr;

      //! Pointer to a buffer with the Z coordinate (in meters) of the point in Cartesian coordinates
      COORDTYPE *cartesianZ = nullptr;

      //! Value = 0 if the point is considered valid, 1 otherwise
      int8_t *cartesianInvalidState = nullptr;

      //! Pointer to a buffer with the Point response intensity. Unit is unspecified.
      float *intensity = nullptr;

      //! Value = 0 if the intensity is considered valid, 1 otherwise
      int8_t *isIntensityInvalid = nullptr;

      uint16_t *colorRed = nullptr;     //!< Pointer to a buffer with the Red color coefficient. Unit is unspecified
      uint16_t *colorGreen = nullptr;   //!< Pointer to a buffer with the Green color coefficient. Unit is unspecified
      uint16_t *colorBlue = nullptr;    //!< Pointer to a buffer with the Blue color coefficient. Unit is unspecified
      int8_t *isColorInvalid = nullptr; //!< Value = 0 if the color is considered valid, 1 otherwise

      //! Pointer to a buffer with the range (in meters) of points in spherical coordinates.
      COORDTYPE *sphericalRange = nullptr;

      //! Pointer to a buffer with the Azimuth angle (in radians) of point in spherical coordinates
      COORDTYPE *sphericalAzimuth = nullptr;

      //! Pointer to a buffer with the Elevation angle (in radians) of point in spherical coordinates
      COORDTYPE *sphericalElevation = nullptr;

      //!< Value = 0 if the range is considered valid, 1 otherwise
      int8_t *sphericalInvalidState = nullptr;

      //! Pointer to a buffer with the row number of point (zero based). This is useful for data that is stored in a
      //! regular grid.  Shall be in the interval (0, 2^31).
      int32_t *rowIndex = nullptr;

      //! Pointer to a buffer with the column number of point (zero based). This is useful for data that is stored in a
      //! regular grid. Shall be in the interval (0, 2^31).
      int32_t *columnIndex = nullptr;

      //! Pointer to a buffer with the number of this return (zero based). That is, 0 is the first return, 1 is the
      //! second, and so on. Shall be in the interval (0, returnCount). Only for multi-return sensors.
      int8_t *returnIndex = nullptr;

      //! Pointer to a buffer with the total number of returns for the pulse that this corresponds to. Shall be in the
      //! interval (0, 2^7). Only for multi-return sensors.
      int8_t *returnCount = nullptr;

      //! Pointer to a buffer with the time (in seconds) since the start time for the data, which is given by
      //! acquisitionStart in the parent Data3D Structure.
      double *timeStamp = nullptr;

      //!< Value = 0 if the timeStamp is considered valid, 1 otherwise
      int8_t *isTimeStampInvalid = nullptr;

      // E57_EXT_surface_normals
      float *normalX = nullptr; //!< The X component of a surface normal vector (E57_EXT_surface_normals extension).
      float *normalY = nullptr; //!< The Y component of a surface normal vector (E57_EXT_surface_normals extension).
      float *normalZ = nullptr; //!< The Z component of a surface normal vector (E57_EXT_surface_normals extension).

   private:
      //! Keeps track of whether we used the Data3D constructor or not so we can free our memory.
      bool _selfAllocated = false;
   };

   using Data3DPointsData = Data3DPointsData_t<float>;
   using Data3DPointsData_d = Data3DPointsData_t<double>;

   extern template Data3DPointsData_t<float>::Data3DPointsData_t( Data3D &data3D );
   extern template Data3DPointsData_t<double>::Data3DPointsData_t( Data3D &data3D );

   extern template Data3DPointsData_t<float>::~Data3DPointsData_t();
   extern template Data3DPointsData_t<double>::~Data3DPointsData_t();

   //! @brief Stores an image that is to be used only as a visual reference.
   struct E57_DLL VisualReferenceRepresentation
   {
      int64_t jpegImageSize = 0; //!< Size of JPEG format image data in BlobNode.
      int64_t pngImageSize = 0;  //!< Size of PNG format image data in BlobNode.
      int64_t imageMaskSize = 0; //!< Size of PNG format image mask in BlobNode.
      int32_t imageWidth = 0;    //!< The image width (in pixels). Shall be positive.
      int32_t imageHeight = 0;   //!< The image height (in pixels). Shall be positive.

      bool operator==( const VisualReferenceRepresentation &rhs ) const
      {
         return ( jpegImageSize == rhs.jpegImageSize ) && ( pngImageSize == rhs.pngImageSize ) &&
                ( imageMaskSize == rhs.imageMaskSize ) && ( imageWidth == rhs.imageWidth ) &&
                ( imageHeight == rhs.imageHeight );
      }

      bool operator!=( const VisualReferenceRepresentation &rhs ) const
      {
         return !operator==( rhs );
      }
   };

   //! @brief Stores an image that is mapped from 3D using the pinhole camera projection model.
   struct E57_DLL PinholeRepresentation
   {
      int64_t jpegImageSize = 0; //!< Size of JPEG format image data in BlobNode.
      int64_t pngImageSize = 0;  //!< Size of PNG format image data in BlobNode.
      int64_t imageMaskSize = 0; //!< Size of PNG format image mask in BlobNode.
      int32_t imageWidth = 0;    //!< The image width (in pixels). Shall be positive.
      int32_t imageHeight = 0;   //!< The image height (in pixels). Shall be positive.
      double focalLength = 0.0;  //!< The camera's focal length (in meters). Shall be positive.
      double pixelWidth = 0.0;   //!< The width of the pixels in the camera (in meters). Shall be positive.
      double pixelHeight = 0.0;  //!< The height of the pixels in the camera (in meters). Shall be positive.

      //! The X coordinate in the image of the principal point, (in pixels). The principal point is the intersection of
      //! the z axis of the camera coordinate frame with the image plane.
      double principalPointX = 0.0;
      double principalPointY = 0.0; //!< The Y coordinate in the image of the principal point (in pixels).

      bool operator==( const PinholeRepresentation &rhs ) const
      {
         return ( jpegImageSize == rhs.jpegImageSize ) && ( pngImageSize == rhs.pngImageSize ) &&
                ( imageMaskSize == rhs.imageMaskSize ) && ( imageWidth == rhs.imageWidth ) &&
                ( imageHeight == rhs.imageHeight ) && ( focalLength == rhs.focalLength ) &&
                ( pixelWidth == rhs.pixelWidth ) && ( pixelHeight == rhs.pixelHeight ) &&
                ( principalPointX == rhs.principalPointX ) && ( principalPointY == rhs.principalPointY );
      }

      bool operator!=( const PinholeRepresentation &rhs ) const
      {
         return !operator==( rhs );
      }
   };

   //! @brief Stores an image that is mapped from 3D using a spherical projection model
   struct E57_DLL SphericalRepresentation
   {
      int64_t jpegImageSize = 0; //!< Size of JPEG format image data in BlobNode.
      int64_t pngImageSize = 0;  //!< Size of PNG format image data in BlobNode.
      int64_t imageMaskSize = 0; //!< Size of PNG format image mask in BlobNode.
      int32_t imageWidth = 0;    //!< The image width (in pixels). Shall be positive
      int32_t imageHeight = 0;   //!< The image height (in pixels). Shall be positive
      double pixelWidth = 0.0;   //!< The width of a pixel in the image (in radians). Shall be positive
      double pixelHeight = 0.0;  //!< The height of a pixel in the image (in radians). Shall be positive.

      bool operator==( const SphericalRepresentation &rhs ) const
      {
         return ( jpegImageSize == rhs.jpegImageSize ) && ( pngImageSize == rhs.pngImageSize ) &&
                ( imageMaskSize == rhs.imageMaskSize ) && ( imageWidth == rhs.imageWidth ) &&
                ( imageHeight == rhs.imageHeight ) && ( pixelWidth == rhs.pixelWidth ) &&
                ( pixelHeight == rhs.pixelHeight );
      }

      bool operator!=( const SphericalRepresentation &rhs ) const
      {
         return !operator==( rhs );
      }
   };

   //! @brief Stores an image that is mapped from 3D using a cylindrical projection model.
   struct E57_DLL CylindricalRepresentation
   {
      int64_t jpegImageSize = 0; //!< Size of JPEG format image data in Blob.
      int64_t pngImageSize = 0;  //!< Size of PNG format image data in Blob.
      int64_t imageMaskSize = 0; //!< Size of PNG format image mask in Blob.
      int32_t imageWidth = 0;    //!< The image width (in pixels). Shall be positive
      int32_t imageHeight = 0;   //!< The image height (in pixels). Shall be positive
      double pixelWidth = 0.0;   //!< The width of a pixel in the image (in radians). Shall be positive.
      double pixelHeight = 0.0;  //!< The height of a pixel in the image (in meters). Shall be positive.

      //! The closest distance from the cylindrical image surface to the center of projection (that is, the radius of
      //! the cylinder) (in meters). Shall be non-negative.
      double radius = 0.0;

      //! The Y coordinate in the image of the principal point (in pixels). This is the intersection of the z = 0 plane
      //! with the image.
      double principalPointY = 0.0;

      bool operator==( const CylindricalRepresentation &rhs ) const
      {
         return ( jpegImageSize == rhs.jpegImageSize ) && ( pngImageSize == rhs.pngImageSize ) &&
                ( imageMaskSize == rhs.imageMaskSize ) && ( imageWidth == rhs.imageWidth ) &&
                ( imageHeight == rhs.imageHeight ) && ( pixelWidth == rhs.pixelWidth ) &&
                ( pixelHeight == rhs.pixelHeight ) && ( radius == rhs.radius ) &&
                ( principalPointY == rhs.principalPointY );
      }

      bool operator!=( const CylindricalRepresentation &rhs ) const
      {
         return !operator==( rhs );
      }
   };

   //! @brief Stores an image from a camera
   struct E57_DLL Image2D
   {
      ustring name;        //!< A user-defined name for the Image2D.
      ustring guid;        //!< A globally unique identification string for the current version of the Image2D object
      ustring description; //!< A user-defined description of the Image2D
      DateTime acquisitionDateTime; //!< The date and time that the image was taken

      //! The globally unique identification string (guid element) for the Data3D that was being acquired when the
      //! picture was taken
      ustring associatedData3DGuid;

      ustring sensorVendor; //!< The name of the manufacturer for the sensor used to collect the points in this Data3D.
      ustring sensorModel;  //!< The model name or number for the sensor.
      ustring sensorSerialNumber; //!< The serial number for the sensor.

      //! A rigid body transform that describes the coordinate frame of the camera in the file-level coordinate system
      RigidBodyTransform pose;

      //! Representation for an image that does not define any camera projection model.
      //! The image is to be used for visual reference only
      VisualReferenceRepresentation visualReferenceRepresentation;

      //! Representation for an image using the pinhole camera projection model.
      PinholeRepresentation pinholeRepresentation;

      //! Representation for an image using the spherical camera projection model.
      SphericalRepresentation sphericalRepresentation;

      //! Representation for an image using the cylindrical camera projection model.
      CylindricalRepresentation cylindricalRepresentation;
   };

   //! @brief Identifies the format representation for the image data
   enum Image2DType
   {
      E57_NO_IMAGE = 0,      //!< No image data
      E57_JPEG_IMAGE = 1,    //!< JPEG format image data.
      E57_PNG_IMAGE = 2,     //!< PNG format image data.
      E57_PNG_IMAGE_MASK = 3 //!< PNG format image mask.
   };

   //! @brief Identifies the representation for the image data
   enum Image2DProjection
   {
      E57_NO_PROJECTION = 0, //!< No representation for the image data is present
      E57_VISUAL = 1,        //!< VisualReferenceRepresentation for the image data
      E57_PINHOLE = 2,       //!< PinholeRepresentation for the image data
      E57_SPHERICAL = 3,     //!< SphericalRepresentation for the image data
      E57_CYLINDRICAL = 4    //!< CylindricalRepresentation for the image data
   };
} // end namespace e57
