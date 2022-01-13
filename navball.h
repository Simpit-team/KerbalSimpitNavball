#include <Adafruit_TFTLCD.h>

#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define WHITE   0xFFFF
#define GREEN   0x07E0

// In deg discretized
struct LatLon{
  int lat;
  int lon;
};

struct Point3D{
  float x;
  float y;
  float z;
};

struct Point2D{
  float x;
  float y;
  bool visible; // True if the point is not hidden behind the navball
};

class Navball {
public:
  Navball();

  // set the values for roll/pitch/yaw, in degs
  void set_rpy(float roll, float pitch, float yaw);

  // Draw the navball on the given Adafruit_GFX
  void draw(Adafruit_GFX* tft);
  
private :
  //internally, store the values as int based on the discretisation value
  int _roll;
  int _pitch;
  int _yaw;

  // number of point used for the angle discretisation. This is the amont of value used for the full 360°. Must be a mutliple of 4.
  static const int DISCRETISATION_SIZE = 1000;
  const int NBR_LON = 36; // increment in longitude for drawing them
  const int LAT_INC = 15; // increment in latitude for drawing them
  const int SIZE = 300; // in pixels, diameter of the navball

  // Discretized values for sin/cos to avoid computing them on the fly. Available between [0, 90]. 
  // Use sin_d and cos_d to compute a sin/cos value.
  static float _sin_table[DISCRETISATION_SIZE/4 + 1];

  // Compute a sin value from a discretized angle, by looking it up in _sin_table
  float sin_d(int disc_angle);
  // Compute a cos value from a discretized angle, by looking it up in _sin_table
  float cos_d(int disc_angle);

  // Convert an angle (in deg) to an discretized value, between 0 and DISCRETISATION_SIZE. Assume the angle is above -360.
  inline int discretize_angle(float angle){ return ((int)((angle  * DISCRETISATION_SIZE)/360) + DISCRETISATION_SIZE) % DISCRETISATION_SIZE; }

  // Return the screen position of a point defined by its lon/lat coordinate (taking into account vessel roll/pitch/yaw).
  Point2D getXY(float lat, float lon);
};
