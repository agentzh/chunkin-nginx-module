use strict;
use lib '.';
use inc::Module::Install;

name            ('Test-Nginx-LWP');
license         ('bsd');
perl_version    ('5.006001');
repository      ('http://github.com/agentzh/chunkin-nginx-module');
all_from        ('lib/Test/Nginx/LWP.pm');

use_test_base();
auto_install();

#tests('t/*.t t/*/*.t');

WriteAll();

