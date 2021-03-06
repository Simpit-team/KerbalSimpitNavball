#include "KerbalNavball.h"

float KerbalNavball::_sin_table[DISCRETISATION_SIZE/4 + 1];

KerbalNavball::KerbalNavball(){
  for(int i = 0; i <= DISCRETISATION_SIZE/4; i++){
    _sin_table[i] = sin(i*2*3.14/DISCRETISATION_SIZE);
  }

  _is_target_set = false;
  _is_maneuver_set = false;
  _is_speed_orientation_set = false;
}

void KerbalNavball::draw(Adafruit_GFX* tft){
  _start_draw_time = millis();
  
  // Erase previous navball
  tft->fillRect(0, 0, SIZE, SIZE, WHITE);

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Draw the ball itself
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
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

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Draw the graduations
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////

  tft->setTextColor(BLACK);
  tft->setTextSize(1);

  Point2D posXY = getXY(0, 0);
  if(posXY.visible){
	  tft->setCursor(posXY.x, posXY.y);
	  tft->print("N");
  }

  posXY = getXY(0, 45);
  if(posXY.visible){
	  tft->setCursor(posXY.x, posXY.y);
	  tft->print("45");
  }

  posXY = getXY(0, 90);
  if(posXY.visible){
	  tft->setCursor(posXY.x, posXY.y);
	  tft->print("E");
  }

  posXY = getXY(0, 135);
  if(posXY.visible){
	  tft->setCursor(posXY.x, posXY.y);
	  tft->print("135");
  }

  posXY = getXY(0, 180);
  if(posXY.visible){
	  tft->setCursor(posXY.x, posXY.y);
	  tft->print("S");
  }

  posXY = getXY(0, 225);
  if(posXY.visible){
	  tft->setCursor(posXY.x, posXY.y);
	  tft->print("225");
  }

  posXY = getXY(0, 270);
  if(posXY.visible){
	  tft->setCursor(posXY.x, posXY.y);
	  tft->print("W");
  }

  posXY = getXY(0, 315);
  if(posXY.visible){
	  tft->setCursor(posXY.x, posXY.y);
	  tft->print("315");
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Draw central marker
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////

  // Basic size of the markers, approximately the radius of each marker
  const int MARKER_SIZE = SIZE/20;

  // Central marker is bigger than other markers, scale to be 8*MARKER_SIZE long and 2*MARKER_SIZE in height
  tft->drawFastHLine(SIZE/2 - 4*MARKER_SIZE, SIZE/2, 2*MARKER_SIZE, CENTRAL_MARKER_COLOR);
  tft->drawLine(SIZE/2 - 2*MARKER_SIZE, SIZE/2, SIZE/2, SIZE/2 + 2*MARKER_SIZE, CENTRAL_MARKER_COLOR);
  tft->drawLine(SIZE/2 + 2*MARKER_SIZE, SIZE/2, SIZE/2, SIZE/2 + 2*MARKER_SIZE, CENTRAL_MARKER_COLOR);
  tft->drawFastHLine(SIZE/2 + 2*MARKER_SIZE, SIZE/2, 2*MARKER_SIZE, CENTRAL_MARKER_COLOR);
  tft->fillCircle(SIZE/2, SIZE/2, MARKER_CENTER_RADIUS, CENTRAL_MARKER_COLOR);


  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Draw prograde, normal, radial marker
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////

  if(_is_speed_orientation_set){

    posXY = getXY(_prograde_pitch, _prograde_heading);
    if(posXY.visible){
      // Draw prograde marker
      tft->fillCircle(posXY.x, posXY.y, MARKER_CENTER_RADIUS, PROGRADE_MARKER_COLOR);
      tft->drawCircle(posXY.x, posXY.y, MARKER_SIZE/2, PROGRADE_MARKER_COLOR);
      tft->drawFastVLine(posXY.x, posXY.y - MARKER_SIZE, MARKER_SIZE/2, PROGRADE_MARKER_COLOR);
      tft->drawFastHLine(posXY.x + MARKER_SIZE/2, posXY.y, MARKER_SIZE/2, PROGRADE_MARKER_COLOR);
      tft->drawFastHLine(posXY.x - MARKER_SIZE, posXY.y, MARKER_SIZE/2, PROGRADE_MARKER_COLOR);
    } else {
      // Draw retrograde marker
      posXY = getXY(_prograde_pitch + 180, _prograde_heading);
      tft->drawCircle(posXY.x, posXY.y, MARKER_SIZE/2, PROGRADE_MARKER_COLOR);
      tft->drawLine(posXY.x - 0.7*MARKER_SIZE/2, posXY.y + 0.7*MARKER_SIZE/2, posXY.x + 0.7*MARKER_SIZE/2, posXY.y - 0.7*MARKER_SIZE/2, PROGRADE_MARKER_COLOR);
      tft->drawLine(posXY.x + 0.7*MARKER_SIZE/2, posXY.y + 0.7*MARKER_SIZE/2, posXY.x - 0.7*MARKER_SIZE/2, posXY.y - 0.7*MARKER_SIZE/2, PROGRADE_MARKER_COLOR);
      tft->drawFastVLine(posXY.x, posXY.y - MARKER_SIZE, MARKER_SIZE/2, PROGRADE_MARKER_COLOR);
      tft->drawFastHLine(posXY.x + MARKER_SIZE/2, posXY.y, MARKER_SIZE/2, PROGRADE_MARKER_COLOR);
      tft->drawFastHLine(posXY.x - MARKER_SIZE, posXY.y, MARKER_SIZE/2, PROGRADE_MARKER_COLOR);
    }

    posXY = getXY(_normal_pitch, _normal_heading);
    if(posXY.visible){
      // Draw normal marker
      tft->fillCircle(posXY.x, posXY.y, MARKER_CENTER_RADIUS, NORMAL_MARKER_COLOR);
      tft->drawLine(posXY.x, posXY.y - MARKER_SIZE, posXY.x + 0.7*MARKER_SIZE, posXY.y + 0.7*MARKER_SIZE, NORMAL_MARKER_COLOR);
      tft->drawLine(posXY.x, posXY.y - MARKER_SIZE, posXY.x - 0.7*MARKER_SIZE, posXY.y + 0.7*MARKER_SIZE, NORMAL_MARKER_COLOR);
      tft->drawLine(posXY.x + 0.7*MARKER_SIZE, posXY.y + 0.7*MARKER_SIZE, posXY.x - 0.7*MARKER_SIZE, posXY.y + 0.7*MARKER_SIZE, NORMAL_MARKER_COLOR);
    } else {
      // Draw anti-normal marker
      posXY = getXY(_normal_pitch + 180, _normal_heading);
      tft->fillCircle(posXY.x, posXY.y, MARKER_CENTER_RADIUS, NORMAL_MARKER_COLOR);
      tft->drawLine(posXY.x, posXY.y + MARKER_SIZE, posXY.x + 0.7*MARKER_SIZE, posXY.y - 0.7*MARKER_SIZE, NORMAL_MARKER_COLOR);
      tft->drawLine(posXY.x, posXY.y + MARKER_SIZE, posXY.x - 0.7*MARKER_SIZE, posXY.y - 0.7*MARKER_SIZE, NORMAL_MARKER_COLOR);
      tft->drawLine(posXY.x + 0.7*MARKER_SIZE, posXY.y - 0.7*MARKER_SIZE, posXY.x - 0.7*MARKER_SIZE, posXY.y - 0.7*MARKER_SIZE, NORMAL_MARKER_COLOR);
      tft->drawLine(posXY.x + 0.3*MARKER_SIZE, posXY.y + 0.3*MARKER_SIZE, posXY.x + MARKER_SIZE, posXY.y + MARKER_SIZE, NORMAL_MARKER_COLOR);
      tft->drawLine(posXY.x - 0.3*MARKER_SIZE, posXY.y + 0.3*MARKER_SIZE, posXY.x - MARKER_SIZE, posXY.y + MARKER_SIZE, NORMAL_MARKER_COLOR);
      tft->drawLine(posXY.x, posXY.y - 0.7*MARKER_SIZE, posXY.x, posXY.y - MARKER_SIZE, NORMAL_MARKER_COLOR);
    }

    posXY = getXY(_radial_pitch, _radial_heading);
    if(posXY.visible){
      // Draw radial in marker
      tft->drawCircle(posXY.x, posXY.y, 2*MARKER_SIZE/3, RADIAL_MARKER_COLOR);
      tft->drawLine(posXY.x + 0.7*2*MARKER_SIZE/3, posXY.y + 0.7*2*MARKER_SIZE/3, posXY.x + 2*MARKER_SIZE/3, posXY.y + 2*MARKER_SIZE/3, RADIAL_MARKER_COLOR);
      tft->drawLine(posXY.x + 0.7*2*MARKER_SIZE/3, posXY.y - 0.7*2*MARKER_SIZE/3, posXY.x + 2*MARKER_SIZE/3, posXY.y - 2*MARKER_SIZE/3, RADIAL_MARKER_COLOR);
      tft->drawLine(posXY.x - 0.7*2*MARKER_SIZE/3, posXY.y + 0.7*2*MARKER_SIZE/3, posXY.x - 2*MARKER_SIZE/3, posXY.y + 2*MARKER_SIZE/3, RADIAL_MARKER_COLOR);
      tft->drawLine(posXY.x - 0.7*2*MARKER_SIZE/3, posXY.y - 0.7*2*MARKER_SIZE/3, posXY.x - 2*MARKER_SIZE/3, posXY.y - 2*MARKER_SIZE/3, RADIAL_MARKER_COLOR);
    } else {
      // Draw radial out marker
      posXY = getXY(_radial_pitch + 180, _radial_heading);
      tft->drawCircle(posXY.x, posXY.y, 2*MARKER_SIZE/3, RADIAL_MARKER_COLOR);
      tft->drawLine(posXY.x + 0.7*2*MARKER_SIZE/3, posXY.y + 0.7*2*MARKER_SIZE/3, posXY.x + 0.3*2*MARKER_SIZE/3, posXY.y + 0.3*2*MARKER_SIZE/3, RADIAL_MARKER_COLOR);
      tft->drawLine(posXY.x + 0.7*2*MARKER_SIZE/3, posXY.y - 0.7*2*MARKER_SIZE/3, posXY.x + 0.3*2*MARKER_SIZE/3, posXY.y - 0.3*2*MARKER_SIZE/3, RADIAL_MARKER_COLOR);
      tft->drawLine(posXY.x - 0.7*2*MARKER_SIZE/3, posXY.y + 0.7*2*MARKER_SIZE/3, posXY.x - 0.3*2*MARKER_SIZE/3, posXY.y + 0.3*2*MARKER_SIZE/3, RADIAL_MARKER_COLOR);
      tft->drawLine(posXY.x - 0.7*2*MARKER_SIZE/3, posXY.y - 0.7*2*MARKER_SIZE/3, posXY.x - 0.3*2*MARKER_SIZE/3, posXY.y - 0.3*2*MARKER_SIZE/3, RADIAL_MARKER_COLOR);
    }
  }
  
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Draw target marker
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////

  if(_is_target_set){
    posXY = getXY(_pitch_target, _heading_target);
    if(posXY.visible){
      // Draw marker to target
      tft->fillCircle(posXY.x, posXY.y, MARKER_CENTER_RADIUS, TARGET_MARKER_COLOR);
      tft->drawCircle(posXY.x, posXY.y, MARKER_SIZE, TARGET_MARKER_COLOR);
    } else {
      // Draw marker opposite to target
      posXY = getXY(_pitch_target + 180, _heading_target);
      tft->fillCircle(posXY.x, posXY.y, MARKER_CENTER_RADIUS, TARGET_MARKER_COLOR);
      tft->drawFastVLine(posXY.x, posXY.y + MARKER_SIZE/3, 2*MARKER_SIZE/3, TARGET_MARKER_COLOR);
      tft->drawLine(posXY.x + 0.3*MARKER_SIZE, posXY.y - 0.3*MARKER_SIZE, posXY.x + 0.7*MARKER_SIZE, posXY.y - 0.7*MARKER_SIZE, TARGET_MARKER_COLOR);
      tft->drawLine(posXY.x - 0.3*MARKER_SIZE, posXY.y - 0.3*MARKER_SIZE, posXY.x - 0.7*MARKER_SIZE, posXY.y - 0.7*MARKER_SIZE, TARGET_MARKER_COLOR);
    }
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Draw maneuver marker
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////

  if(_is_maneuver_set){
    posXY = getXY(_pitch_maneuver, _heading_maneuver);
    if(posXY.visible){
      tft->fillCircle(posXY.x, posXY.y, MARKER_CENTER_RADIUS, MANEUVER_MARKER_COLOR);
      tft->drawFastVLine(posXY.x, posXY.y - MARKER_SIZE, 2*MARKER_SIZE/3, MANEUVER_MARKER_COLOR);
      tft->drawLine(posXY.x - 0.3*MARKER_SIZE, posXY.y + 0.3*MARKER_SIZE, posXY.x - 0.7*MARKER_SIZE, posXY.y + 0.7*MARKER_SIZE, MANEUVER_MARKER_COLOR);
      tft->drawLine(posXY.x + 0.3*MARKER_SIZE, posXY.y + 0.3*MARKER_SIZE, posXY.x + 0.7*MARKER_SIZE, posXY.y + 0.7*MARKER_SIZE, MANEUVER_MARKER_COLOR);
    }
  }
  

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Draw indicators (values)
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  tft->setTextColor(BLACK);
  tft->setCursor(0, 0);
  // Values are contraints between 0-360 for yaw and -90 to 90 for pitch
  if(_pitch <= DISCRETISATION_SIZE/4){
    tft->println("Heading : " + String(_yaw*360/DISCRETISATION_SIZE));
    tft->println("Pitch : " + String(_pitch*360/DISCRETISATION_SIZE));
  } else if(_pitch > DISCRETISATION_SIZE/4 && _pitch <= DISCRETISATION_SIZE/2){
    tft->println("Heading : " + String(((_yaw + DISCRETISATION_SIZE/2)%DISCRETISATION_SIZE)*360/DISCRETISATION_SIZE));
    tft->println("Pitch : " + String(((DISCRETISATION_SIZE/2 - _pitch)%DISCRETISATION_SIZE)*360/DISCRETISATION_SIZE));
  } else if(_pitch > DISCRETISATION_SIZE/2 && _pitch <= 3*DISCRETISATION_SIZE/4){
    tft->println("Heading : " + String(((_yaw + DISCRETISATION_SIZE/2)%DISCRETISATION_SIZE)*360/DISCRETISATION_SIZE));
    tft->println("Pitch : " + String(((DISCRETISATION_SIZE/2 - _pitch)%DISCRETISATION_SIZE)*360/DISCRETISATION_SIZE));
  } else {
    tft->println("Heading : " + String(_yaw*360/DISCRETISATION_SIZE));
    tft->println("Pitch : " + String((_pitch - DISCRETISATION_SIZE)*360/DISCRETISATION_SIZE));
  }
  tft->println("Time : " + String(millis() - _start_draw_time) + "ms");
}

void KerbalNavball::set_rpy(float roll, float pitch, float yaw){
  _roll =  discretize_angle(roll);
  _pitch = discretize_angle(pitch);
  _yaw =   discretize_angle(yaw);
}

void KerbalNavball::set_target(float heading, float pitch){
  _is_target_set = true;
  _heading_target = heading;
  _pitch_target = pitch;
}
  
void KerbalNavball::unset_target(){
  _is_target_set = false;
  _heading_target = 0;
  _pitch_target = 0;
}

void KerbalNavball::set_maneuver(float heading, float pitch){
  _is_maneuver_set = true;
  _heading_maneuver = heading;
  _pitch_maneuver = pitch;
}
  
void KerbalNavball::unset_maneuver(){
  _is_maneuver_set = false;
  _heading_maneuver = 0;
  _pitch_maneuver = 0;
}

void KerbalNavball::set_speed_orientation(float prograde_heading, float prograde_pitch, float normal_heading, float normal_pitch, float radial_heading, float radial_pitch){
  _is_speed_orientation_set = true;
  _prograde_heading = prograde_heading;
  _prograde_pitch = prograde_pitch;
  _normal_heading = normal_heading;
  _normal_pitch = normal_pitch;
  _radial_heading = radial_heading;
  _radial_pitch = radial_pitch;
}

void KerbalNavball::unset_speed_orientation(){
  _is_speed_orientation_set = false;
  _prograde_heading = 0;
  _prograde_pitch = 0;
  _normal_heading = 0;
  _normal_pitch = 0;
  _radial_heading = 0;
  _radial_pitch = 0;
}

float KerbalNavball::sin_d(int disc_angle){
  // ensure the angle is positive
  if(disc_angle < 0) return -sin_d(-disc_angle);
  // ensure the angle is below 360??
  disc_angle = disc_angle % DISCRETISATION_SIZE;
  // ensure the angle is below 180??
  if(disc_angle >= DISCRETISATION_SIZE/2) return -sin_d(disc_angle - DISCRETISATION_SIZE/2);
  
  if(disc_angle <= DISCRETISATION_SIZE/4){
    return _sin_table[disc_angle];
  } else {
    return _sin_table[DISCRETISATION_SIZE/2 - disc_angle];
  }
}

float KerbalNavball::cos_d(int disc_angle){
  // Use the fact that cos(a) = sin(a + 90??)
  return sin_d(disc_angle + DISCRETISATION_SIZE/4);
}

Point2D KerbalNavball::getXY(float lat, float lon){
  // Discretize the coordinate
  LatLon pt_polar = {discretize_angle(lat), discretize_angle(lon)};

  // Get the original 3D coordinate, without any rotation
  // X is the axis to the right, Y to the top, Z is coming to the viewer
  Point3D pt1 = {sin_d(pt_polar.lon)*cos_d(pt_polar.lat), sin_d(pt_polar.lat), cos_d(pt_polar.lon)*cos_d(pt_polar.lat)};

  // Apply yaw rotation
  Point3D pt2 = {pt1.x*cos_d(_yaw)+pt1.z*sin_d(-_yaw), pt1.y, -pt1.x*sin_d(-_yaw)+pt1.z*cos_d(_yaw)};

  // Apply pitch rotation
  Point3D pt3 = {pt2.x, pt2.y*cos_d(_pitch)+pt2.z*sin_d(_pitch), -pt2.y*sin_d(_pitch)+pt2.z*cos_d(_pitch)};

  // Apply roll rotation
  Point3D pt4 = {pt3.x*cos_d(_roll)+pt3.y*sin_d(_roll), -pt3.x*sin_d(_roll)+pt3.y*cos_d(_roll), pt3.z};

  // Convert the coordinate from a sphere of size one to screen coordinate
  return Point2D{(pt4.x+ 1)*SIZE/2, (pt4.y+ 1)*SIZE/2, pt4.z >= 0};
}
