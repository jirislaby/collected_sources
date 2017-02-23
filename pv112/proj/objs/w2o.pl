#!/usr/bin/perl
use strict;

my $st = 0;
my @pole = ();

print "static struct triangle XXX[] = {\n";

while (<>) {
	chop;
	chop;
	if ($_ eq "-") {
		$st = 1;
		next;
	} elsif ($_ eq "--") {
		$st = 0;
		@pole = ();
		next;
	}
	if ($st eq 0) {
		push @pole, $_;
	} else {
		my @crd = split;
		print "{ ";
		for (my $a = 0; $a < 3; $a++) {
			my @pos = split / /, $pole[$crd[$a]];
			printf "{ %4.1f, %4.1f, %4.1f }%s", $pos[0], $pos[1],
				$pos[2], $a == 2 ? " " : ", ";
		}
		print "},\n";
	}
};

print "};\n";
