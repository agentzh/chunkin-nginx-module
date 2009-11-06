# vi:filetype=perl

use lib 'lib';
use Test::Nginx::LWP;

plan tests => 2 * blocks();

run_tests();

__DATA__

=== TEST 1: off & POST
--- config
    chunkin off;
    location /main {
        echo hi;
    }
--- request
POST /main
--- chunked_body eval
["hello", "world"]
--- error_code: 411
--- response_body_like: 411 Length Required


=== TEST 2: default (off) & POST
--- config
    location /main {
    }
--- request
POST /main
--- chunked_body eval
["hello", "world"]
--- error_code: 411
--- response_body_like: 411 Length Required



=== TEST 3: off & GET
--- config
    chunkin off;
    location /main {
        echo hi;
    }
--- request
GET /main
--- response_body
hi
--- error_code: 200



=== TEST 4: on & GET
--- config
    chunkin on;
    location /main {
        echo hi;
    }
--- request
GET /main
--- response_body
hi
--- error_code: 200



=== TEST 5: on & POST
--- config
    chunkin on;
    location /main {
        echo $request_method;
        #echo $echo_request_body;
    }
--- request
POST /main
--- chunked_body eval
["hello", "world"]
--- error_code: 200
--- response_body
POST

