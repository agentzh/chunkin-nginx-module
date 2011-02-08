# vi:filetype=

use lib 't/lib';
use Test::Nginx::Socket::Chunkin;

plan tests => repeat_each() * 2 * blocks();

no_diff;

run_tests();

__DATA__

=== TEST 1: bad chunk size in the 2nd chunk
--- config
    chunkin on;
    location /main {
        client_body_timeout 1;
        echo_read_request_body;
        echo_request_body;
    }
--- more_headers
Transfer-Encoding: chunked
User-Agent: Java Browser
--- timeout: 2
--- request eval
"POST /main
0\r
"
--- response_body:
--- error_code:



=== TEST 2: bad chunk size in the 2nd chunk (using standard client body reader)
--- config
    chunkin on;
    location /main {
        client_body_timeout 1;
        echo_read_request_body;
        echo_request_body;
    }
--- more_headers
Content-Length: 5
--- timeout: 3
--- request eval
"POST /main
he"
--- response_body:
--- error_code:
--- SKIP

