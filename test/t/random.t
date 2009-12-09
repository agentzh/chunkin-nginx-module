# vi:filetype=perl

use lib 'lib';
use Test::Nginx::Socket;

our $data;

$Test::Nginx::Socket::RepeatEach = 1000;

plan tests => $Test::Nginx::Socket::RepeatEach * 2 * blocks();

no_diff;

run_tests();

__DATA__

=== TEST 1: single chunk
--- config
    chunkin on;
    location /ar.do {
        client_max_body_size       1m;
        echo_request_body;
    }
--- more_headers
Transfer-Encoding: chunked
--- request eval
$::data = '';
my $count = (int rand 32766) + 1;
for (my $i = 0; $i < $count; $i++) {
    $::data .= chr int rand 256;
}
#warn $::data;
my $s = "POST /ar.do
".
sprintf("%x\r\n", length $::data).
$::data
."\r
0\r
\r
";
open my $out, '>/tmp/out.txt' or die $!;
print $out $s;
close $out;
$s
--- response_body eval
$::data

