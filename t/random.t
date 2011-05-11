# vi:filetype=

use lib 't/lib';
use Test::Nginx::Socket::Chunkin;

#worker_connections(1024);
master_process_enabled(1);

our $data;

repeat_each(500);

plan tests => repeat_each() * 2 * blocks();

no_diff;

run_tests();

__DATA__

=== TEST 1: single chunk
--- config
    chunkin on;
    location /ar.do {
        client_max_body_size       1m;
        client_body_buffer_size    512;
        echo_request_body;
    }
--- more_headers
Transfer-Encoding: chunked
--- request eval
$::data = '';
my $count = (int rand 32766) + 1;
for (my $i = 0; $i < $count; $i++) {
    my $c = chr int rand 128;
    $::data .= $c;
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
--- timeout: 10



=== TEST 2: single chunk
--- config
    chunkin on;
    location /ar.do {
        client_max_body_size       1m;
        client_body_buffer_size    1k;
        echo_request_body;
    }
--- more_headers
Transfer-Encoding: chunked
--- request eval
$::data = '';
my $count = 32766;
for (my $i = 0; $i < $count; $i++) {
    my $c = chr int rand 256;
    $::data .= $c;
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
--- timeout: 10

