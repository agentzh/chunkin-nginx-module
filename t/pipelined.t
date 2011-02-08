# vi:filetype=

use lib 't/lib';

my $skip_all;

BEGIN { $skip_all = 1; }

use Test::Nginx::Socket::Chunkin $skip_all ?
    (skip_all => 'too experimental to run the tests properly :P')
    : ();

plan tests => repeat_each() * 2 * blocks();

no_diff;

run_tests();

__DATA__

=== TEST 1: sanity
--- config
    chunkin on;
    location /main {
        chunkin_keepalive on;
        client_body_buffer_size    4;
        echo_request_body;
    }
    location /main2 {
        chunkin_keepalive on;
        client_body_buffer_size    4;
        echo_request_body;
    }

--- more_headers
Transfer-Encoding: chunked
--- pipelined_requests eval
[
"POST /main
5\r
hello\r
0\r
\r
",
"POST /main2
6\r
,world\r
0\r
\r
"]
--- response_body: hello,world
--- ONLY



=== TEST 2: standard body read
--- config
    chunkin on;
    location /main {
        client_body_buffer_size    4;
        echo_read_request_body;
        echo_request_body;
    }
--- more_headers
Content-Length: 5
--- pipelined_requests eval
[
"POST /main
hello",
"POST /main
world"]
--- response_body: helloworld

