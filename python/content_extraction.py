#!/usr/bin/python

import sys
import tidy

import CEviaTD;

if len(sys.argv) <= 2:
    print >> sys.stderr, "Usage: %s input_html output_html" % sys.argv[0]
    sys.exit(1)

input_html  = sys.argv[1]
output_html = sys.argv[2]

encoding = 'utf8'

input_content = ""
for line in open(input_html):
    input_content += line

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
    print >> sys.sterr, "Fail to extract content"
    sys.exit(1)

extracted_xml  = extractor.GetExtractedXml()
extracted_text = extractor.GetExtractedText()

trans_output = tidy.parseString(extracted_xml, **tidy_options)

print extracted_text

f = open(output_html, "w")
f.write(extracted_xml)
f.close()


