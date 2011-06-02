#!/usr/bin/perl -w
use strict;
use Gtk2 -init;
my $window = Gtk2::Window->new('toplevel');
$window->signal_connect(destroy => sub { Gtk2->main_quit });
my $button = Gtk2::Button->new('About');
my $about = Gtk2::AboutDialog->new;
$button->signal_connect(clicked => sub { $about->show });
$window->add($button);
$window->show_all;
Gtk2->main;
