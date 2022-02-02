use strict;
use warnings;
use CGI;

my $cgi = CGI->new;
my $str = $cgi->param("string");

print "Content-type: text/html\r\n\r\n";

print "<html><h1>This text was printed with perl</h1></html><br>";
print $str;
