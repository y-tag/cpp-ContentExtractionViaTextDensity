#!/usr/bin/python

import re
import sys
import urllib2
import chardet
import tidy

import CEviaTD;

if len(sys.argv) <= 1:
    print >> sys.stderr, "Usage: %s url" % sys.argv[0]
    sys.exit(1)

url = sys.argv[1]

encoding = 'utf8'

input_content = urllib2.urlopen(url).read()
detected_encoding = chardet.detect(input_content)['encoding']

input_content = unicode(input_content, detected_encoding).encode(encoding)

tidy_options = {'char_encoding'    : encoding,
                'doctype'          : 'omit',
                'indent'           : 0,
                'numeric_entities' : 1,
                'output_xhtml'     : 1,
                'tidy_mark'        : 0,
                'wrap'             : 0}

trans_input = tidy.parseString(input_content, **tidy_options)

if len(str(trans_input)) == 0:
    print >> sys.stderr, "Fail to tidy html"
    print [e for e in trans_input.errors if e.severity == 'E']
    sys.exit(1)

trans_input = str(trans_input)

extractor = CEviaTD.CEviaTD();
ret = extractor.Parse(trans_input)

if ret < 0:
    print >> sys.stderr, "Fail to extract content"
    sys.exit(1)

extracted_xml  = extractor.GetExtractedXml()
extracted_text = extractor.GetExtractedText()

trans_output = tidy.parseString(extracted_xml, **tidy_options)

print extracted_text

