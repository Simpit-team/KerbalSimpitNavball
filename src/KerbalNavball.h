#include <Adafruit_TFTLCD.h>

// Basic color definitions
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0
#define WHITE    0xFFFF

// Specific colors
#define CENTRAL_MARKER_COLOR 0xF780
#define PROGRADE_MARKER_COLOR GREEN
#define NORMAL_MARKER_COLOR MAGENTA
#define RADIAL_MARKER_COLOR CYAN
#define TARGET_MARKER_COLOR MAGENTA
#define MANEUVER_MARKER_COLOR BLUE

// radius of all the central points of the markers, in pixels
#define MARKER_CENTER_RADIUS 2

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

class KerbalNavball {
public:
  KerbalNavball();

  // set the values for roll/pitch/yaw, in degs
  void set_rpy(float roll, float pitch, float yaw);

  // Set a target with those values
  void set_maneuver(float heading, float pitch);
  void unset_maneuver();

  // Set a maneuver with those values
  void set_target(float heading, float pitch);
  void unset_target();

  // Set speed orientation
  void set_speed_orientation(float prograde_heading, float prograde_pitch, float normal_heading, float normal_pitch, float radial_heading, float radial_pitch);
  void unset_speed_orientation();

  // Draw the navball on the given Adafruit_GFX
  void draw(Adafruit_GFX* tft);
  
private :
  //internally, store the values as int based on the discretisation value
  int _roll;
  int _pitch;
  int _yaw;

  float _heading_target;
  float _pitch_target;
  bool _is_target_set;

  float _heading_maneuver;
  float _pitch_maneuver;
  bool _is_maneuver_set;

  float _prograde_heading;
  float _prograde_pitch;
  float _normal_heading;
  float _normal_pitch;
  float _radial_heading;
  float _radial_pitch;
  bool _is_speed_orientation_set;

  // Store start time of the call to draw, to display the time needed to draw a frame
  unsigned long _start_draw_time;

  // number of point used for the angle discretisation. This is the amont of value used for the full 360°. Must be a mutliple of 4.
  static const int DISCRETISATION_SIZE = 360;
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
