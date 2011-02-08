package Test::Nginx::Socket::Chunkin;

use lib 'lib';
use lib 'inc';
use Test::Nginx::Socket -Base;

config_preamble(<<'_EOC_');
        error_page 411 = @chunkin_error;
        location @chunkin_error {
            chunkin_resume;
        }
_EOC_

1;
