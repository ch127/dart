#include "ontology/onto_sexpr.h"
#include "util/unixenv.h"

// path to default chain file, from dart root
#define DEFAULT_CHAIN_FILE "src/ontology/t/test5.tsm"

// default max number of steps during EM
#define MAX_EM_STEPS 99

// wrapper for Alphabet::tok2int
int tok2int (const Alphabet& alphabet, const sstring& s, const char* desc)
{
  if (!alphabet.contains_tok (s))
    THROWEXPR (desc << " token '" << s << "' not found in alphabet");

  return alphabet.token2int (s);
}


// main program
int main (int argc, char** argv)
{
  // initialise the options parser
  INIT_OPTS_LIST (opts, argc, argv, 0, "[options]",
		  "read a substitution matrix and exponentiate it\n");

  sstring default_chain_filename;
  default_chain_filename << Dart_Unix::get_DARTDIR() << '/' << DEFAULT_CHAIN_FILE;

  sstring chain_filename, write_filename;
  bool compute_evidence, compute_posteriors, compute_summaries, learn_params;
  int max_steps;

  opts.print_title ("Modeling options");

  opts.add ("r -read", chain_filename = default_chain_filename, "file to load model from");
  opts.add ("w -write", write_filename = "", "file to save model to", false);
  opts.add ("e -evidence", compute_evidence = false, "compute log-evidences");
  opts.add ("p -predict", compute_posteriors = false, "compute posterior probabilities over missing data");
  opts.add ("s -summarize", compute_summaries = false, "compute summary statistics");
  opts.add ("l -learn", learn_params = false, "learn parameters by EM");

  opts.newline();
  opts.add ("ms -max-steps", max_steps = MAX_EM_STEPS, "max number of steps during EM learning");

  // parse the command line & do stuff
  try
    {
      // parse command line
      if (!opts.parse()) { cerr << opts.short_help(); exit(1); }
    }
  catch (const Dart_exception& e)
    {
      cerr << opts.short_help();
      cerr << e.what();
      exit(1);
    }

  // do stuff
  try
    {
      // read chain file, get SExpr
      SExpr_file sexpr_file (chain_filename.c_str());
      SExpr& sexpr = sexpr_file.sexpr;

      // initialize guile & define the newick smob, the quick & hacky way (by calling xrate's init code)
      ECFG_Scheme_evaluator scheme;
      scheme.initialize();

      // init the Termx
      Termx term;
      Termx_builder::init_termx (term, sexpr);

      // do EM, if asked
      if (learn_params)
	{
	  Termx_trainer trainer (term, max_steps);
	  cout << ";; EM results\n";
	  cout << trainer.final_sexpr().to_parenthesized_string() << '\n';
	}

      // calculate evidences, if asked
      if (compute_evidence)
	{
	  Termx_log_evidence log_ev (term);
	  SExpr log_ev_sexpr = log_ev.map_reduce_sexpr();
	  cout << ";; log-evidence by family\n";
	  cout << log_ev_sexpr.to_parenthesized_string() << '\n';
	}

      // calculate posteriors, if asked
      if (compute_posteriors)
	{
	  Termx_prediction prediction (term);
	  SExpr prediction_sexpr = prediction.map_reduce_sexpr();
	  cout << ";; posteriors by family, gene and state\n";
	  cout << prediction_sexpr.to_parenthesized_string() << '\n';
	}

      // calculate summaries, if asked
      if (compute_summaries)
	{
	  cout << ";; summary statistics by parameter\n";
	  THROWEXPR ("Unimplemented");
	}

      // save
      if (write_filename.size())
	{
	  ofstream write_file (write_filename.c_str());
	  Termx_builder::termx2stream (write_file, term);
	}
    }
  catch (const Dart_exception& e)
    {
      cerr << e.what();
      exit(1);
    }

  return 0;
}
