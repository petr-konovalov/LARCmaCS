#pragma once

#ifdef OLD_SSL_PROTO
namespace FieldConstantsRoboCup2009 {
  static const int line_width = 10;
  static const int field_length=6050;
  static const int field_width=4050;
  static const int boundary_width=250;
  static const int referee_width=425;
  static const int goal_width=700;
  static const int goal_depth=180;
  static const int goal_wall_width=20;
  static const int center_circle_radius=500;
  static const int defense_radius=500;
  static const int defense_stretch=350;
  static const int free_kick_from_defense_dist=200;
  static const int penalty_spot_from_field_line_dist=450;
  static const int penalty_line_from_spot_dist=400;
}
#else
namespace FieldConstantsRoboCup2018A {
    const double kFieldLength = 12000.0;
    const double kFieldWidth = 9000.0;
    const double kGoalWidth = 1200.0;
    const double kGoalDepth = 180.0;
    const double kBoundaryWidth = 250.0;
    const double kLineThickness = 10.0;
    const double kPenaltyAreaDepth = 1200.0;
    const double kPenaltyAreaWidth = 2400.0;
    const double kCenterCircleRadius = 500.0;
    const int kNumCamerasTotal = 8;
    const int kNumCamerasLocal = 4;
}
#endif
