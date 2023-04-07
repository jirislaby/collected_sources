#!/usr/bin/perl -w
use strict;
use Digest::MD5 qw(md5_hex);
use Data::Dumper;
use threads;

my $hash = "c10bcecb7bf37b1ce7c1afbe0ec66a0b";

sub compute($) {
	foreach my $try (@{$_[0]}) {
		my $md5 = md5_hex($try);
#		print "trying $try: $md5\n";
		if ($md5 eq $hash) {
			print "got it: $try\n";
			exit 1;
		}
	}
}

sub getvariants($) {
	my %subs = (
		'a' => [ '@', '4' ],
		'b' => [ '8' ],
		'e' => [ '3' ],
		'i' => [ '!' ],
		'l' => [ '1' ],
		'o' => [ '0' ],
		's' => [ '$', '5' ],
		't' => [ '7' ],
	);

	my @all = @_;
	for (my $i = 0; $i < length; $i++) {
		my $letter = lc substr $_, $i, 1;
		my @to_add;
		foreach my $sub (@{$subs{$letter}}) {
			foreach my $word (@all) {
				my $c = $word;
				substr($c, $i, 1) = $sub;
				push @to_add, $c;
			}
		}
		push @all, @to_add;
	}
	return \@all;
}

my $iter = 0;
my $counter = 0;
while (<>) {
	chomp;
	my $all = getvariants($_);

	if (!($iter % 1000)) {
		print "trying $iter. word from dict, tried $counter combinations\n";
	}
	if (0 && !($iter % 10000)) {
		print "$_: ", Dumper($all);
	}

	$counter += scalar @{$all};
	compute($all);
	$iter++;
}

1;
