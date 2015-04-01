#!/usr/bin/perl 
#===============================================================================
#
#         FILE:  tscope-api-upload-file.pl
#
#       AUTHOR:  Michal J. Kubski, <mkubski@websense.com>
#      COMPANY:  Websense
#      CREATED:  30/10/12 10:36:36 GMT
#
# $Header: //code/release/ThreatScope/Production/2014_R4/src/tscope-api/tscope-api-upload-file.pl#1 $
#
#===============================================================================

use strict;
use warnings;

use File::Basename;
use File::Slurp;
use LWP;
use Data::Dumper;
use HTTP::Message;
use HTTP::Request;
use HTTP::Response;
use Getopt::Std;
use Sys::Hostname;
use Digest::SHA qw(sha1_hex);

our $api_content_type = "application/vnd.wbsn-sandbox+json;version=1.0";
our $version = 'tscope-api-upload-file/' . (do { (my $x = '$Id: //code/release/ThreatScope/Production/2014_R4/src/tscope-api/tscope-api-upload-file.pl#1 $') =~ s/.*#(\d+)\s*\$.*/$1/; $x;});

my $args = {};
getopts("a:u:f:h:k:r", $args);

my ($url, $archive, $fname, $hash, $key, $random) = ($args->{u}, $args->{a}, $args->{f}, $args->{h}, $args->{k}, $args->{r});

$key ||= "AP1K3Y02";

unless (defined($url) && defined($fname) && ($archive ? defined($hash) : 1)) {
    die <<EOF;
Upload a file:
    $0 [-r] -u url -f filename [-h filename-sha1] [-k API-KEY]
Upload an archive:
    $0 -u url -a archive-name -f filename -h filename-sha1 [-k API-KEY]
    
Note: -r adds a random string at the end of a file
EOF
}

my $file;
if (defined($archive)) {
    $file = read_file($archive);
} else {
    $file = read_file($fname);
    if (!defined($hash)) {
        if ($random) {
            $file .= int(rand(100000000));
        }
        $hash = sha1_hex($file);
    }
}


my $agent = "LWP-Test/0.1";
my $browser = new LWP::UserAgent;

$browser->agent($agent);

# This works on some LWP versions, on others it's not necessary
eval {
    $browser->ssl_opts('verify_hostname' => 0);
};

my $get_url = "$url/file/$hash";
#my $put_url = "$url/file/$hash?filename=".basename("$fname");
my $put_url = "$url";
my $post_url;


if (defined($archive)) {
    $post_url = "$url/file?filename=".basename("$archive");
}
 


# Now PUT (single file) or POST (archive) the data

my %headers = (
   # "X-API-Key" => $key,
    "Accept" => $api_content_type,
 #   "X-PUT-Token" => $token,
  #  "X-Product-Version", $version,
  #  "X-Node-Identity", hostname
);


my $request;
my $namea;
%headers = (%headers, ("Content-Length" => length($file)));
if (defined($archive)) {
    # POST archive
    %headers = (%headers, ("X-Files" => "$hash, $fname"));
    $request = HTTP::Request->new( "POST", $post_url, HTTP::Headers->new(%headers), $file);
} else {
    %headers = (%headers, ("X-Files" => "$hash, $fname"));
    $namea=basename($fname);
    %headers = (%headers, ("Filename" =>"$namea"));
    %headers = (%headers, ("sha1sum"  =>$hash));
    $request = HTTP::Request->new( "PUT", $put_url, HTTP::Headers->new(%headers), $file);
}
my $response = $browser->request($request);

#print $response->{_request}->method." ". $response->{_request}->{_uri}->as_string()."\n";
#print "HTTP Code: ".$response->code."\n";
print "Headers: ".Dumper($request->headers);
#print "Content: ".Dumper($response->content);

