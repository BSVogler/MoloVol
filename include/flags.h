#ifndef FLAGS_H

#define FLAGS_H

// output flags
enum mvOUT : unsigned {
  mvOUT_NONE = 0,
  
  mvOUT_STRUCTURE = 1 << 1,
  mvOUT_RESOLUTION = 1 << 2,
  mvOUT_DEPTH = 1 << 3,
  mvOUT_RADIUS_S = 1 << 4,
  mvOUT_RADIUS_L = 1 << 5,
  mvOUT_INP = mvOUT_STRUCTURE | mvOUT_RESOLUTION | mvOUT_DEPTH | mvOUT_RADIUS_S | mvOUT_RADIUS_L,
  
  mvOUT_OPT_HETATM = 1 << 7,
  mvOUT_OPT_UNITCELL = 1 << 8,
  mvOUT_OPT_PROBEMODE = 1 << 9,
  mvOUT_OPT_SURFACE = 1 << 10,
  mvOUT_OPT = mvOUT_OPT_HETATM | mvOUT_OPT_UNITCELL | mvOUT_OPT_PROBEMODE | mvOUT_OPT_SURFACE,

  mvOUT_FORMULA = 1 << 12,
  mvOUT_TIME = 1 << 13,
  
  mvOUT_VOL_VDW = 1 << 15,
  mvOUT_VOL_INACCESSIBLE = 1 << 16,
  mvOUT_VOL_CORE_S = 1 << 17,
  mvOUT_VOL_SHELL_S = 1 << 18,
  mvOUT_VOL_CORE_L = 1 << 19,
  mvOUT_VOL_SHELL_L = 1 << 20,
  mvOUT_VOL = mvOUT_VOL_VDW | mvOUT_VOL_INACCESSIBLE | mvOUT_VOL_CORE_S | mvOUT_VOL_SHELL_S | mvOUT_VOL_CORE_L | mvOUT_VOL_SHELL_L,
  
  mvOUT_SURF_VDW = 1 << 22,
  mvOUT_SURF_MOL = 1 << 23,
  mvOUT_SURF_EXCLUDED_S = 1 << 24,
  mvOUT_SURF_ACCESSIBLE_S = 1 << 25,
  mvOUT_SURF = mvOUT_SURF_VDW | mvOUT_SURF_MOL | mvOUT_SURF_EXCLUDED_S | mvOUT_SURF_ACCESSIBLE_S,

  mvOUT_CAVITIES = 1 << 27,
  mvALL = 0xFFFFFFFF
};

#endif
