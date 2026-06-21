//Copyright SimBlocks LLC 2016-2026
#include "MathTypes.h"

using namespace sbio::math;

SLatLon::SLatLon()
{
}

SLatLon::SLatLon(Latitude latitude, Longitude longitude) : latitude(latitude), longitude(longitude)
{
}

SGeodeticCoordinates::SGeodeticCoordinates(Latitude latitude, Longitude longitude, HeightRelativeToWGS84Ellipsoid fAlt) : latitude(latitude), longitude(longitude), altitude(fAlt)
{
}

SGeodeticCoordinates::SGeodeticCoordinates(Latitude latitude, Longitude longitude, double fAlt) : SGeodeticCoordinates(latitude, longitude, HeightRelativeToWGS84Ellipsoid(fAlt))
{
}

SUTMCoordinates SUTMCoordinates::FromEastingNorthing(double easting, double northing)
{
  SUTMCoordinates coordinates;
  coordinates.easting = easting;
  coordinates.northing = northing;
  return coordinates;
}

SUTMExtent::SUTMExtent(const SUTMCoordinates& upperLeft, const SUTMCoordinates& lowerRight) : upperLeft(upperLeft), lowerRight(lowerRight)
{
}

SReferencePlaneCoordinateSystem::SReferencePlaneCoordinateSystem()
{
  rotation.setIdentity();
  inverseRotation.setIdentity();
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
