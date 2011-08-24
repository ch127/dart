#include "ecfg/single_chain_ecfg.h"
#include "ecfg/ecfgsexpr.h"

Single_chain_ECFG::Single_chain_ECFG (const Irrev_EM_matrix& matrix)
  : ECFG_scores (((Irrev_EM_matrix&)matrix).alphabet(), 1)
{
  const int chain_idx = matrix_set.add_matrix (1, Irrev);
  if (chain_idx != 0)
    THROWEXPR ("chain_idx should be zero");

  ((EM_matrix_params&) *matrix_set.chain[chain_idx].matrix) = matrix;

  state_info[0] = ECFG_emitl_state_info (chain_idx);

  transition (Start, 0) = transition (0, 0) = transition (0, End) = transition (Start, End) = ScoreOfProb1;

  if (CTAGGING(1,SINGLE_CHAIN_ECFG))
    {
      CL << "Single_chain_ECFG:\n";
      ECFG_builder::ecfg2stream (CL, alphabet, *this);
    }
}
