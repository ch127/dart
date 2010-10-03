#ifndef ALIGNMENT_ENVELOPE_H
#define ALIGNMENT_ENVELOPE_H
#include<iostream>
#include<string>
#include<vector>
#include<map>

#include "util/sstring.h"
#include "seq/biosequence.h"

using namespace std;


class AlignmentEnvelope
{
 public:
  // constructor, initializer
  AlignmentEnvelope(void); 
  void build_index(sstring guide_alignment_filename, sstring gap_char = "-"); 
  // main data storage unit - nested maps
  map< string, map<int, map<string, int > > > coordinates; 
  int lookup(string seqName, int index, string otherSeqName);

  void show(ostream& out); 
 private:
  map<string, string> alignment;  
  sstring gap_char; 
  // convert from alignment coords to sequence coords
  int sequence_coordinate(int alignment_coordinate, string seqName); 
};

#endif
