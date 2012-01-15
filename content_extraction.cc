// License: GPLv3 <http://www.gnu.org/copyleft/gpl.html>

#include "ce_via_td.h"
#include "html_tidy.h"

#include <cstdio>

#include <iostream>
#include <fstream>

int main(int argc, char** argv) {
  if (argc < 3) {
    fprintf(stderr, "Usage %s input_html output_html\n", argv[0]);
    exit(1);
  }

  std::string input_html  = argv[1];
  std::string output_html = argv[2];

  std::ifstream fin(input_html.c_str(), std::ios::in);
  if (! fin.is_open()) {
    fprintf(stderr, "Fail to open input_html: %s\n", input_html.c_str());
    return 1;
  }
  
  std::string buff;
  std::string input_data;
  while (getline(fin, buff)) {
    input_data += buff;
  }

  std::string trans_input;
  HTMLTidy::tidy(input_data, "utf8", &trans_input);

  CEviaTD extractor;
  int ret = extractor.Parse(trans_input.c_str());
  if (ret != 1) {
    fprintf(stderr, "Fail to parse\n");
    return 1;
  }

  std::string output;
  HTMLTidy::tidy(extractor.GetExtractedXml(), "utf8", &output); 

  std::ofstream fout(output_html.c_str(), std::ios::out);
  if (! fout.is_open()) {
    fprintf(stderr, "Fail to open output_html: %s\n", output_html.c_str());
    return 1;
  }
  fout << output.c_str();
  fout.close();

  //fprintf(stdout, "%s\n", output.c_str());
  fprintf(stdout, "%s\n", extractor.GetExtractedText());

  return 0;
}
