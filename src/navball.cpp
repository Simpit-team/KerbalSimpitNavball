#include "navball.h"

float Navball::_sin_table[DISCRETISATION_SIZE/4 + 1];

Navball::Navball(){
  for(int i = 0; i <= DISCRETISATION_SIZE/4; i++){
    _sin_table[i] = sin(i*2*3.14/DISCRETISATION_SIZE);
  }
}

void Navball::draw(Adafruit_GFX* tft){
  // Erase previous navball
  tft->fillRect(0, 0, SIZE, SIZE, WHITE);
  tft->drawCircle(SIZE/2, SIZE/2, SIZE/2, BLACK);

  // Draw the 'horizontal' lines, including equator
  for(int lat = - 90 + LAT_INC; lat < 90; lat += LAT_INC){
    // Set the color based on the line
    int color = GREEN;
    if(lat < 0) color = RED;
    if(lat > 0) color = BLUE;
    
    for(int lon = -180; lon < 180; lon += 360/NBR_LON){
      Point2D startXY = getXY(lat, lon);
      Point2D endXY = getXY(lat, lon + 360/NBR_LON);
      if(startXY.visible && endXY.visible){
        tft->drawLine(startXY.x, startXY.y, endXY.x, endXY.y, color);
      }
    }
  }

  // Draw the 'vertical' lines
  for(int lon = - 180 + 45; lon <= 180; lon += 45){
    // Set the color based on the line
    int color = BLACK;
    
    for(int lat = -90; lat < 90; lat += 180/NBR_LON){
      Point2D startXY = getXY(lat, lon);
      Point2D endXY = getXY(lat + 180/NBR_LON, lon);
      if(startXY.visible && endXY.visible){
        tft->drawLine(startXY.x, startXY.y, endXY.x, endXY.y, color);
      }
    }
  }
}

void Navball::set_rpy(float roll, float pitch, float yaw){
  _roll =  discretize_angle(roll);
  _pitch = discretize_angle(pitch);
  _yaw =   discretize_angle(yaw);
}

float Navball::sin_d(int disc_angle){
  // ensure the angle is positive
  if(disc_angle < 0) return -sin_d(-disc_angle);
  // ensure the angle is below 360°
  disc_angle = disc_angle % DISCRETISATION_SIZE;
  // ensure the angle is below 180°
  if(disc_angle >= DISCRETISATION_SIZE/2) return -sin_d(disc_angle - DISCRETISATION_SIZE/2);
  
  if(disc_angle <= DISCRETISATION_SIZE/4){
    return _sin_table[disc_angle];
  } else {
    return _sin_table[DISCRETISATION_SIZE/2 - disc_angle];
  }
}

float Navball::cos_d(int disc_angle){
  // Use the fact that cos(a) = sin(a + 90°)
  return sin_d(disc_angle + DISCRETISATION_SIZE/4);
}

Point2D Navball::getXY(float lat, float lon){
  // Discretize the coordinate
  LatLon pt_polar = {discretize_angle(lat), discretize_angle(lon)};

  // Get the original 3D coordinate, without any rotation
  // X is the axis to the right, Y to the top, Z is coming to the viewer
  Point3D pt1 = {sin_d(pt_polar.lon)*cos_d(pt_polar.lat), sin_d(pt_polar.lat), cos_d(pt_polar.lon)*cos_d(pt_polar.lat)};

  // Apply yaw rotation
  Point3D pt2 = {pt1.x*cos_d(_yaw)+pt1.z*sin_d(_yaw), pt1.y, -pt1.x*sin_d(_yaw)+pt1.z*cos_d(_yaw)};

  // Apply pitch rotation
  Point3D pt3 = {pt2.x, pt2.y*cos_d(_pitch)+pt2.z*sin_d(_pitch), -pt2.y*sin_d(_pitch)+pt2.z*cos_d(_pitch)};

  // Apply roll rotation
  Point3D pt4 = {pt3.x*cos_d(_roll)+pt3.y*sin_d(_roll), -pt3.x*sin_d(_roll)+pt3.y*cos_d(_roll), pt3.z};

  // Convert the coordinate from a sphere of size one to screen coordinate
  return Point2D{(pt4.x+ 1)*SIZE/2, (pt4.y+ 1)*SIZE/2, pt4.z >= 0};
}
