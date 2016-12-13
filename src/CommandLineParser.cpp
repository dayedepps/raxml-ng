#include "CommandLineParser.hpp"

#include <getopt.h>

using namespace std;

static struct option long_options[] =
{
  {"help",               no_argument,       0, 0 },  /*  0 */
  {"version",            no_argument,       0, 0 },  /*  1 */
  {"evaluate",           no_argument,       0, 0 },  /*  2 */
  {"search",             no_argument,       0, 0 },  /*  3 */

  {"msa",                required_argument, 0, 0 },  /*  4 */
  {"tree",               required_argument, 0, 0 },  /*  5 */
  {"prefix",             required_argument, 0, 0 },  /*  6 */
  {"model",              required_argument, 0, 0 },  /*  7 */
  {"data-type",          required_argument, 0, 0 },  /*  8 */

  {"opt-model",          required_argument, 0, 0 },  /*  9 */
  {"opt-branches",       required_argument, 0, 0 },  /*  10 */
  {"prob-msa",           required_argument, 0, 0 },  /*  11 */
  {"pat-comp",           required_argument, 0, 0 },  /*  12 */
  {"tip-inner",          required_argument, 0, 0 },  /*  13 */
  {"brlen",              required_argument, 0, 0 },  /*  14 */
  {"spr-radius",         required_argument, 0, 0 },  /*  15 */
  {"spr-cutoff",         required_argument, 0, 0 },  /*  16 */
  {"lh-epsilon",         required_argument, 0, 0 },  /*  17 */

  {"seed",               required_argument, 0, 0 },  /*  18 */
  {"threads",            required_argument, 0, 0 },  /*  19 */
  {"simd",               required_argument, 0, 0 },  /*  20 */
  { 0, 0, 0, 0 }
};

void CommandLineParser::parse_options(int argc, char** argv, Options &opts)
{
  /* if no command specified, default to --search (or --help if no args were given) */
  opts.command = (argc > 1) ? Command::search : Command::help;
  opts.start_tree = StartingTree::random;
  opts.random_seed = (long)time(NULL);

  /* compress alignment patterns by default */
  opts.use_pattern_compression = true;
  /* use tip-inner case optimization by default */
  opts.use_tip_inner = true;

  /* optimize model and branch lengths */
  opts.optimize_model = true;
  opts.optimize_brlen = true;

  /* data type: default autodetect */
//  useropt->data_type = RAXML_DATATYPE_AUTO;

  /* initialize LH epsilon with default value */
  opts.lh_epsilon = DEF_LH_EPSILON;

  /* default: autodetect best SPR radius */
  opts.spr_radius = -1;
  opts.spr_cutoff = 1.0;

  /* default: scaled branch lengths */
  opts.brlen_linkage = PLLMOD_TREE_BRLEN_SCALED;

  /* use 2 threads per default */
  opts.num_threads = 2;

  opts.model_file = "GTR+G";

  // autodetect CPU instruction set and use respective SIMD kernels
  opts.simd_arch = sysutil_simd_autodetect();

  int option_index = 0;
  int c;
  int num_commands = 0;

  /* getopt_long_only() uses this global variable to track progress;
   * need this re-initialization to make function re-enterable... */
  optind = 0;

  while ((c = getopt_long_only(argc, argv, "", long_options, &option_index)) == 0)
  {
    switch (option_index)
    {
      case 0:
        opts.command = Command::help;
        num_commands++;
        break;

      case 1:
        opts.command = Command::version;
        num_commands++;
        break;

      case 2:
        opts.command = Command::evaluate;
        num_commands++;
        break;

      case 3:
        opts.command = Command::search;
        num_commands++;
        break;

      case 4: /* alignment file */
        opts.msa_file = optarg;
        break;

      case 5: /* starting tree */
        if (strcasecmp(optarg, "rand") == 0 || strcasecmp(optarg, "random") == 0)
          opts.start_tree = StartingTree::random;
        else if (strcasecmp(optarg, "pars") == 0 || strcasecmp(optarg, "parsimony") == 0)
          opts.start_tree = StartingTree::parsimony;
        else
        {
          opts.start_tree = StartingTree::user;
          opts.tree_file = optarg;
        }
        break;

      case 6: /* set prefix for output files */
        opts.outfile_prefix = optarg;
        break;

      case 7:  /* model */
        opts.model_file = optarg;
        break;

      case 8:  /* data-type */
//        if (strcasecmp(optarg, "dna") == 0)
//          opts.data_type = RAXML_DATATYPE_DNA;
//        else if (strcasecmp(optarg, "aa") == 0)
//          opts.data_type = RAXML_DATATYPE_AA;
//        else if (strcasecmp(optarg, "binary") == 0)
//          opts.data_type = RAXML_DATATYPE_BINARY;
//        else if (strcasecmp(optarg, "diploid10") == 0)
//          opts.data_type = RAXML_DATATYPE_DIPLOID10;
//        else if (strcasecmp(optarg, "multi") == 1)
//          opts.data_type = RAXML_DATATYPE_MULTI;
//        else if (strcasecmp(optarg, "auto") == 0)
//          opts.data_type = RAXML_DATATYPE_AUTO;
//        else
//          sysutil_fatal("\nUnknown data type: %s\n\n", optarg);
        break;

      case 9: /* optimize model */
        opts.optimize_model = !optarg || (strcasecmp(optarg, "off") != 0);
        break;

      case 10: /* optimize branches */
        opts.optimize_brlen = !optarg || (strcasecmp(optarg, "off") != 0);
        break;

      case 11:  /* prob-msa = use probabilitic MSA */
        if (!optarg || (strcasecmp(optarg, "off") != 0))
        {
          opts.use_prob_msa = true;
          opts.use_pattern_compression = false;
          opts.use_tip_inner = false;
        }
        break;

      case 12: /* disable pattern compression */
        opts.use_pattern_compression = !optarg || (strcasecmp(optarg, "off") != 0);
        break;

      case 13: /* disable tip-inner optimization */
        opts.use_tip_inner = !optarg || (strcasecmp(optarg, "off") != 0);
        break;

      case 14: /* branch length linkage mode */
        if (strcasecmp(optarg, "scaled") == 0)
          opts.brlen_linkage = PLLMOD_TREE_BRLEN_SCALED;
        else if (strcasecmp(optarg, "linked") == 0)
          opts.brlen_linkage = PLLMOD_TREE_BRLEN_LINKED;
        else if (strcasecmp(optarg, "unlinked") == 0)
        {
          opts.brlen_linkage = PLLMOD_TREE_BRLEN_UNLINKED;
          throw OptionException("Unlinked branch lengths not supported yet!");
        }
        else
          throw InvalidOptionValueException("Unknown branch linkage mode: %s", optarg);
        break;

      case 15:  /* spr-radius = maximum radius for fast SPRs */
        if (sscanf(optarg, "%u", &opts.spr_radius) != 1)
        {
          throw InvalidOptionValueException("Invalid SPR radius: %s, please provide a positive integer!",
                        optarg);
        }
        break;
      case 16:  /* spr-cutoff = relative LH cutoff to discard subtrees */
        if (strcasecmp(optarg, "off") == 0)
        {
          opts.spr_cutoff = 0.;
        }
        else if (sscanf(optarg, "%lf", &opts.spr_cutoff) != 1)
        {
          throw InvalidOptionValueException("Invalid SPR cutoff: %s, please provide a real number!",
                        optarg);
        }
        break;

      case 17: /* LH epsilon */
        if(sscanf(optarg, "%lf", &opts.lh_epsilon) != 1 || opts.lh_epsilon < 0.)
          throw InvalidOptionValueException("Invalid LH epsilon parameter value: %s, please provide a positive real number.", optarg);
        break;

      case 18: /* random seed */
        opts.random_seed = atol(optarg);
        break;

      case 19:  /* number of threads */
        if (sscanf(optarg, "%u", &opts.num_threads) != 1 || opts.num_threads == 0)
        {
          throw InvalidOptionValueException("Invalid number of threads: %s, please provide a positive integer number!",
                        optarg);
        }
        break;
      case 20: /* SIMD instruction set */
        if (strcasecmp(optarg, "none") == 0 || strcasecmp(optarg, "scalar") == 0)
        {
          opts.simd_arch = PLL_ATTRIB_ARCH_CPU;
        }
        else if (strcasecmp(optarg, "sse3") == 0 || strcasecmp(optarg, "sse") == 0)
        {
          opts.simd_arch = PLL_ATTRIB_ARCH_SSE;
        }
        else if (strcasecmp(optarg, "avx") == 0)
        {
          opts.simd_arch = PLL_ATTRIB_ARCH_AVX;
        }
        else if (strcasecmp(optarg, "avx2") == 0)
        {
          opts.simd_arch = PLL_ATTRIB_ARCH_AVX2;
        }
        else
        {
          sysutil_fatal("\nUnknown SIMD instruction set: %s!\n\n", optarg);
        }
        break;
      default:
        throw  OptionException("Internal error in option parsing");
    }
  }

  if (c != -1)
    exit(EXIT_FAILURE);

  /* if more than one independent command, fail */
  if (num_commands > 1)
    throw OptionException("More than one command specified");

  /* check for mandatory options for each command */
  if (opts.command == Command::evaluate || opts.command == Command::search)
  {
    if (opts.msa_file.empty())
      throw OptionException("Mandatory switch --msa");
  }

  if (opts.command == Command::evaluate)
  {
    if (opts.tree_file.empty())
      throw OptionException("Mandatory switch --tree");
  }
}

void CommandLineParser::print_help()
{
  LOG_INFO << "Usage: raxml-ng [OPTIONS]\n";

  LOG_INFO << "\n"
            "Commands (mutually exclusive):\n"
            "  --help                                    display help information.\n"
            "  --version                                 display version information.\n"
            "  --evaluate                                evaluate the likelihood of a tree.\n"
            "  --search                                  ML tree search.\n\n"
            "Input and output options:\n"
            "  --tree        FILENAME | rand | pars      starting tree: rand(om), pars(imony) or user-specified (newick file)\n"
            "  --msa         FILENAME                    alignment in FASTA, PHYLIP, VCF or CATG format.\n"
            "  --prefix STRING                           prefix for output files (default: MSA file name)\n"
            "  --data-type   dna | aa | multi<X> | auto  data type: DNA, AA, MULTI-state with X states or AUTO-detect (default)\n\n"
            "General options:\n"
            "  --seed                                    seed for pseudo-random number generator (default: current time)\n"
            "  --pat-comp on | off                       alignment pattern compression (default: ON)\n"
            "  --tip-inner on | off                      tip-inner case optimization (default: ON)\n\n"
            "Model options:\n"
            "  --model        <name>+G[n]+<Freqs> | FILE  Model specification OR partition file (default: GTR+G4)\n"
            "  --brlen        linked | scaled | unlinked  Branch length linkage between partitions (default: scaled)\n"
            "  --opt-model    on | off                    ML optimization of all model parameters (default: ON)\n"
            "  --opt-branches on | off                    ML optimization of all branch lengths (default: ON)\n"
            "  --prob-msa     on | off                    use probabilistic alignment (works with CATG and VCF)\n"
            "  --lh-epsilon   VALUE                       LogLikelihood epsilon for optimization/tree search (default: 0.1)\n\n"
            "Topology search options:\n"
            "  --spr-radius  VALUE                       SPR re-insertion radius for fast iterations (default: AUTO)\n"
            "  --spr-cutoff  VALUE | off                 Relative LH cutoff for descending into subtrees (default: 1.0)\n";
}
