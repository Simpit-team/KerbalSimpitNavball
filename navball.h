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

  // Draw the navball on the given tft
  void draw(Adafruit_TFTLCD* tft);
  
private :
  //internally, store the values as int based on the discretisation value
  int _roll;
  int _pitch;
  int _yaw;

  static const int DISCRETISATION_SIZE = 1000;
  const int NBR_LON = 36; // increment in longitude for drawing them
  const int LAT_INC = 15; // increment in latitude for drawing them
  const int SIZE = 300; // in pixels, diameter of the navball

  //discretized values for sin/cos to avoid computing them on the fly
  static float _sin_table[DISCRETISATION_SIZE];
  static float _cos_table[DISCRETISATION_SIZE]; //TODO reuse sin table, just shifted ?

  // Convert an angle (in deg) to an discretized value, between 0 and DISCRETISATION_SIZE. Assume the angle is above -360.
  inline int discretize_angle(float angle){ return ((int)((angle  * DISCRETISATION_SIZE)/360) + DISCRETISATION_SIZE) % DISCRETISATION_SIZE; }

  // Return the screen position of a point defined by its lon/lat coordinate (taking into account vessel roll/pitch/yaw).
  Point2D getXY(float lat, float lon);
};
