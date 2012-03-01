# vi:filetype=

use lib 't/lib';
use Test::Nginx::LWP;

plan tests => repeat_each() * 2 * blocks();

no_diff;

run_tests();

__DATA__

=== TEST 1: exceeding the default max chunks per buf setting (512)
--- config
    location /main {
        client_body_buffer_size 1m;
        echo_read_request_body;
        echo_request_body;
    }
--- request
POST /main
--- chunked_body eval
[split //, 'a' x 1024]
--- response_body eval
'a' x 1024



=== TEST 2: NOT exceeding the custom max chunks per buf setting
--- config
    location /main {
        client_body_buffer_size 1m;
        chunkin_max_chunks_per_buf 1024;
        echo_read_request_body;
        echo_request_body;
    }
--- request
POST /main
--- chunked_body eval
[split //, 'a' x 1024]
--- response_body eval
'a' x 1024



=== TEST 3: JUST exceeding the custom max chunks per buf setting
--- config
    location /main {
        client_body_buffer_size 1m;
        chunkin_max_chunks_per_buf 1024;
        echo_read_request_body;
        echo_request_body;
    }
--- request
POST /main
--- chunked_body eval
[split //, 'a' x 1025]
--- response_body eval
'a' x 1025

