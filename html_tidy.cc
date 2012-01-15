// Copyright (c) 2011 Fei Sun. All Rights Reserved.
// Author: Ofey.sunfei@gmail.com (Fei Sun)
//         tagami.yukihiro@gmail.com (Yukihiro TAGAMI)
// License: GPLv3 <http://www.gnu.org/copyleft/gpl.html>

#include "html_tidy.h"

#include <stdio.h>
#include <errno.h>

#include "tidy.h"
#include "buffio.h"

int HTMLTidy::tidy(const std::string &input, const std::string &in_enc,
                   std::string *output) {
  if (output == NULL) { return -1; }

  TidyBuffer outbuf;
  TidyBuffer errbuf;
  TidyOptionId optid;
  int rc = -1;
  Bool ok;

  TidyDoc tdoc = tidyCreate(); // Initialize "document"
  tidyBufInit(&outbuf);
  tidyBufInit(&errbuf);

  tidySetInCharEncoding(tdoc,  in_enc.c_str());
  tidySetOutCharEncoding(tdoc, in_enc.c_str());

  optid = tidyOptGetIdForName("doctype");
  tidyOptSetValue(tdoc, optid, "omit");
  optid = tidyOptGetIdForName("output-xhtml");
  tidyOptSetBool(tdoc, optid, yes);
  optid = tidyOptGetIdForName("wrap");
  tidyOptSetInt(tdoc, optid, 0);

  ok = tidyOptSetBool(tdoc, TidyXhtmlOut, yes);        // Convert to XHTML
  if (ok) { rc = tidySetErrorBuffer(tdoc, &errbuf); }  // Capture diagnostics
  if (rc >= 0) { rc = tidyParseString(tdoc, input.c_str()); }  // Parse the input
  if (rc >= 0) { rc = tidyCleanAndRepair(tdoc); }      // Tidy it up!
  if (rc >= 0) { rc = tidyRunDiagnostics(tdoc); }      // Kvetch
  if (rc > 1)  { rc = (tidyOptSetBool(tdoc, TidyForceOutput, yes) ? rc : -1 ); }  // If error, force output.  
  if (rc >= 0) { rc = tidySaveBuffer(tdoc, &outbuf); } // Pretty Print
  if (rc >= 0) {
    char tmp[outbuf.size + 1];
    memcpy(tmp, outbuf.bp, outbuf.size);
    tmp[outbuf.size] = '\0';
    *output = tmp;
  } else {
    printf("A severe error (\%d) occurred.\\n", rc);
  }

  tidyBufFree(&outbuf);
  tidyBufFree(&errbuf);
  tidyRelease(tdoc);
  return rc;
}
