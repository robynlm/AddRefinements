#include "cctk.h"
#include "cctk_Arguments.h"
#include "cctk_Parameters.h"
#include "carpet.hh"

#ifdef HAVE_CARPET
using namespace Carpet;
#endif

#ifdef __cplusplus
  extern "C" {
#endif
    void AddRefinement_Change_num_levels(CCTK_ARGUMENTS);
#ifdef __cplusplus
  } /* extern "C" */
#endif


void AddRefinement_Change_num_levels(CCTK_ARGUMENTS) {
  #ifdef HAVE_CARPET
    DECLARE_CCTK_ARGUMENTS;
    DECLARE_CCTK_PARAMETERS;
    CCTK_REAL reduced_variable;
    static int addref_threshold_done_already[40] = {0};
    int min_active_n = num_levels[0] - 1;

    if (veryverbose) {
      CCTK_VInfo(CCTK_THORNSTRING, "old_num_levels[0] %d, num_levels[0] %d", old_num_levels[0], num_levels[0]);
    }
    if (old_num_levels[0]==num_levels[0]) {
      CCTK_ParameterSet("regrid_every", "CarpetRegrid2", "-1");
    }

    if (cctk_time >= addref_threshold[min_active_n] 
      && addref_threshold[min_active_n] > 0.0) {
      BEGIN_GLOBAL_MODE (cctkGH) {
      // Find the global maximum of tau
      const int Reduction_Handle = CCTK_ReductionHandle(reduction_operation);
      const CCTK_INT input_array_variable_indices = {CCTK_VarIndex("CosmoLapse::tau")};
      const int ierr = CCTK_Reduce(cctkGH,
                                    -1, // target processors; -1 -> all
                                    Reduction_Handle,
                                    1,  // number of output variables
                                    CCTK_VARIABLE_REAL,
                                    &reduced_variable,
                                    1,  // number of variables to be reduced
                                    input_array_variable_indices);
      } END_GLOBAL_MODE;
 
      if (verbose) {
        CCTK_VInfo(CCTK_THORNSTRING, "num_levels[0] is %d, comparing %f to %f", 
              num_levels[0], reduced_variable, addref_threshold[min_active_n]);
      } // end if (verbose)
      
      // Change num_levels based on threshold
      if (reduced_variable >= addref_threshold[min_active_n]) {
        num_levels[0] = min_active_n + 2;
        CCTK_ParameterSet("regrid_every", "CarpetRegrid2", "1");
        CCTK_VInfo(CCTK_THORNSTRING, "Incrementing num_levels[0] to %d", num_levels[0]);
        if (addref_xorigin[min_active_n] > 1e-10) {
          position_x[0] = addref_xorigin[min_active_n];
        }
        if (addref_yorigin[min_active_n] > 1e-10) {
          position_y[0] = addref_yorigin[min_active_n];
        }
        if (addref_zorigin[min_active_n] > 1e-10) {
          position_z[0] = addref_zorigin[min_active_n];
        }
      } // end if (reduced_variable >= addref_threshold[min_active_n])
    } // end if (cctk_time >= addref_threshold[min_active_n])
  # endif // HAVE_CARPET
} // end void AddRefinement_Change_num_levels
