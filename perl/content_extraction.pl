#/usr/local/bin/perl

use strict;
use warnings;

use Encode;
use HTML::Tidy;

use CEviaTD;

main();

sub main {
    if (@ARGV < 2) {
        print STDERR "Usage: $0 input_html output_html\n";
        exit 1;
    }
    my $input_html  = shift @ARGV;
    my $output_html = shift @ARGV;

    my $encoding = 'utf8';

    my $input_content = "";
    open(my $ifh, '<', $input_html) or die $!;
    while (my $line = <$ifh>) {
        $input_content .= $line;
    }
    close($ifh);

    my $tidy = HTML::Tidy->new({
            doctype          => 'omit',
            indent           => 0,
            numeric_entities => 1,
            output_xhtml     => 1,
            tidy_mark        => 0,
            wrap             => 0,
    });
    my $trans_input = $tidy->clean(decode($encoding, $input_content));

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

    open(my $ofh, '>', $output_html) or die $!;
    print {$ofh} $trans_output;
    close($ofh);
}
