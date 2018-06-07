#ifndef HEPVis_SbStyles_h
#define HEPVis_SbStyles_h

// If adding MarkerType, have to update 
//   SoMarkerSet, SbStyle, SoStyle, SbPainterPS, SoPainterAction.
typedef enum {
  SbMarkerDot = 0,
  SbMarkerPlus = 1,
  SbMarkerAsterisk,
  SbMarkerCross,
  SbMarkerStar,
  SbMarkerCircleLine,
  SbMarkerCircleFilled,
  SbMarkerTriangleUpLine,
  SbMarkerTriangleUpFilled,
  SbMarkerTriangleDownLine,
  SbMarkerTriangleDownFilled,
  SbMarkerDavidStarLine,
  SbMarkerDavidStarFilled,
  SbMarkerSwissCrossLine,
  SbMarkerSwissCrossFilled,
  SbMarkerDiamondLine,
  SbMarkerDiamondFilled,
  SbMarkerSquareLine,
  SbMarkerSquareFilled,
  SbMarkerPentaStarLine,
  SbMarkerPentaStarFilled
} SbMarkerStyle;

typedef enum {
  SbAreaSolid = 0,
  SbAreaHatched,
  SbAreaChecker,
  SbAreaEdged
} SbAreaStyle;

typedef enum {
  SbProjectionNone,
  SbProjectionRZ,
  SbProjectionPHIZ,
  SbProjectionZR,
  SbProjectionZPHI
} SbProjectionType;

typedef enum {
  SbPaintingUniform,
  SbPaintingByValue,
  SbPaintingByLevel,
  SbPaintingGreyScale,
  SbPaintingVioletToRed,
  SbPaintingGreyScaleInverse
} SbPaintingPolicy;

typedef enum {
  SbHatchingNone,
  SbHatchingRight,
  SbHatchingLeft,
  SbHatchingLeftAndRight
} SbHatchingPolicy;

typedef unsigned short SbLinePattern;

#define SbLinePattern_solid  0xFFFF
#define SbLinePattern_dashed 0x00FF
//#define SbLinePattern_dotted 0x0101
#define SbLinePattern_dotted 0x1111
#define SbLinePattern_dash_dotted 0x1C47

#define SbLinePattern_s_solid  "0xFFFF"
#define SbLinePattern_s_dashed "0x00FF"
//#define SbLinePattern_s_dotted "0x0101"
#define SbLinePattern_s_dotted "0x1111"
#define SbLinePattern_s_dash_dotted "0x1C47"

#define SbModeling_boxes "boxes"
#define SbModeling_wire_boxes "wire_boxes"
#define SbModeling_lines "lines"
#define SbModeling_segments "segments"
#define SbModeling_points "points"
#define SbModeling_top_lines "top_lines"
#define SbModeling_texts "texts"
#define SbModeling_curve "curve"
#define SbModeling_filled_curve "filled_curve"
#define SbModeling_bar_chart "bar_chart"
#define SbModeling_solid "solid"
#define SbModeling_wire_frame "wire_frame"
#define SbModeling_reduced_wire_frame "reduced_wire_frame"
#define SbModeling_polygon "polygon"
#define SbModeling_none "none"

#define SbEncoding_none "none"
#define SbEncoding_PAW "PAW"
#define SbEncoding_Latex "Latex"

#define SbTickModeling_none  "none"
#define SbTickModeling_hippo "hippo"
#define SbTickModeling_hplot "hplot"

#define SbPainting_uniform       "uniform"
#define SbPainting_by_value      "by_value"
#define SbPainting_by_level      "by_level"
#define SbPainting_grey_scale    "grey_scale"
#define SbPainting_violet_to_red "violet_to_red"

// SbFont_ROOT_<code>
// From TAttText.cxx :
//*-*      1 : times-medium-i-normal      "Times New Roman"      1           4
//*-*      2 : times-bold-r-normal        "Times New Roman"      0           7
//*-*      3 : times-bold-i-normal        "Times New Roman"      1           7
//*-*      4 : helvetica-medium-r-normal  "Arial"                0           4
//*-*      5 : helvetica-medium-o-normal  "Arial"                1           4
//*-*      6 : helvetica-bold-r-normal    "Arial"                0           7
//*-*      7 : helvetica-bold-o-normal    "Arial"                1           7
//*-*      8 : courier-medium-r-normal    "Courier New"          0           4
//*-*      9 : courier-medium-o-normal    "Courier New"          1           4
//*-*     10 : courier-bold-r-normal      "Courier New"          0           7
//*-*     11 : courier-bold-o-normal      "Courier New"          1           7
//*-*     12 : symbol-medium-r-normal     "Symbol"               0           6
//*-*     13 : times-medium-r-normal      "Times New Roman"      0           4
//*-*     14 :                            "Wingdings"            0           4

#define SbFont_ROOT_12  "TTF/timesi"
#define SbFont_ROOT_22  "TTF/timesbd"
#define SbFont_ROOT_32  "TTF/timesbi"
#define SbFont_ROOT_42  "TTF/arial"
#define SbFont_ROOT_52  "TTF/ariali"
#define SbFont_ROOT_62  "TTF/arialbd"
#define SbFont_ROOT_72  "TTF/arialbi"
#define SbFont_ROOT_82  "TTF/cour"
#define SbFont_ROOT_92  "TTF/couri"
#define SbFont_ROOT_102 "TTF/courbd"
#define SbFont_ROOT_112 "TTF/courbi"
#define SbFont_ROOT_122 "TTF/symbol"
#define SbFont_ROOT_132 "TTF/times"
#define SbFont_ROOT_142 "TTF/wingding"

#define SbFont_Hershey "Hershey"
#define SbFont_times "times"

#define SbLightModel_base_color "base_color"
#define SbLightModel_phong      "phong"

#endif



