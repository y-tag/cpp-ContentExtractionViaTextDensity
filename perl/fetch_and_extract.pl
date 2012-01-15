#/usr/local/bin/perl

use strict;
use warnings;

use Encode;
use LWP::Simple;
use HTML::Tidy;

use CEviaTD;

main();

sub main {
    if (@ARGV < 1) {
        print STDERR "Usage: $0 url\n";
        exit 1;
    }
    my $url = shift @ARGV;

    my $encoding = 'utf8';

    my $input_content = get($url); # encode is utf8
    die "Fail to fetch content: $url" unless $input_content;

    my $tidy = HTML::Tidy->new({
            doctype          => 'omit',
            indent           => 0,
            numeric_entities => 1,
            output_xhtml     => 1,
            tidy_mark        => 0,
            wrap             => 0,
    });
    my $trans_input = $tidy->clean($input_content);

    my $extractor = CEviaTD::CEviaTD->new();
    my $ret = $extractor->Parse($trans_input);

    if ($ret < 0) {
        print STDERR "Failt to extract content\n";
        exit 1;
    }

    my $extracted_xml  = $extractor->GetExtractedXml();
    my $extracted_text = $extractor->GetExtractedText();

    my $trans_output = encode($encoding, $tidy->clean(decode($encoding, $extracted_xml)));

    print $extracted_text, "\n";
}
