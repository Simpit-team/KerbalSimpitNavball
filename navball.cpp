#include "navball.h"

float Navball::_sin_table[DISCRETISATION_SIZE];
float Navball::_cos_table[DISCRETISATION_SIZE];

Navball::Navball(){
  for(int i = 0; i < DISCRETISATION_SIZE; i++){
    _sin_table[i] = sin(i*2*3.14/DISCRETISATION_SIZE);
    _cos_table[i] = cos(i*2*3.14/DISCRETISATION_SIZE);
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

Point2D Navball::getXY(float lat, float lon){
  // Discretize the coordinate
  LatLon pt_polar = {discretize_angle(lat), discretize_angle(lon)};

  // Get the original 3D coordinate, without any rotation
  // X is the axis to the right, Y to the top, Z is coming to the viewer
  Point3D pt1 = {_sin_table[pt_polar.lon]*_cos_table[pt_polar.lat], _sin_table[pt_polar.lat], _cos_table[pt_polar.lon]*_cos_table[pt_polar.lat]};

  // Apply yaw rotation
  Point3D pt2 = {pt1.x*_cos_table[_yaw]+pt1.z*_sin_table[_yaw], pt1.y, -pt1.x*_sin_table[_yaw]+pt1.z*_cos_table[_yaw]};

  // Apply pitch rotation
  Point3D pt3 = {pt2.x, pt2.y*_cos_table[_pitch]+pt2.z*_sin_table[_pitch], -pt2.y*_sin_table[_pitch]+pt2.z*_cos_table[_pitch]};

  // Apply roll rotation
  Point3D pt4 = {pt3.x*_cos_table[_roll]+pt3.y*_sin_table[_roll], -pt3.x*_sin_table[_roll]+pt3.y*_cos_table[_roll], pt3.z};

  // Convert the coordinate from a sphere of size one to screen coordinate
  return Point2D{(pt4.x+ 1)*SIZE/2, (pt4.y+ 1)*SIZE/2, pt4.z >= 0};
}
