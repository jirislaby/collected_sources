#!/usr/bin/perl -w
use strict;
use warnings;

my $shift = 4;
my $nesting = 13;

srand;

sub a($) {
	my ($m) = @_;
	if ($m > $nesting) {
		printf "%*sfor (;;) ;\n", $m * $shift, "";
		return;
	}
	printf "%*sif (t($m)) {\n", $m * $shift, "";
	a($m + 1);
	printf "%*s} else {\n", $m * $shift, "";
	a($m + 1);
	printf "%*s}\n", $m * $shift, "";
}

print "void main(void)\n{\n\tchar *a;\n\t a = kmalloc(0, 0);\n";
a(1);
print "*a = 0;\n}";
