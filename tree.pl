#!/usr/bin/perl -w
use strict;
use Gtk2 -init;
use Glib qw(TRUE FALSE);

use constant ID		=> 0;
use constant HEADER	=> 1;

my $start = qr@Conditional jump or move depends on uninitialised value\(s\)|[0-9]+ bytes in [0-9]+ blocks are definitely lost in loss record [0-9,]+ of [0-9,]+@;

sub create_model {
	my $model = Gtk2::TreeStore->new(qw/Glib::String Glib::String Glib::String/);

	my $id = 1;
	my $parent = undef;
	while (<>) {
		chomp;

		next unless /^==[0-9]+== (.*)$/;
		my $line = $1;

		if (!length $line) {
#			last if ($id > 10);
			$parent = undef;
		} elsif (defined $parent) {
			my $child_iter = $model->append($parent);
			$model->set($child_iter, ID, undef, HEADER, "$line", 2, "blebla");
		} elsif ($line =~ $start) {
			$parent = $model->append(undef);
			$model->set($parent, ID, $id++, HEADER, "$line", 2, "X");
		}
	}

	return $model;
}

sub add_columns {
   my $treeview = shift;
   my $model = $treeview->get_model;

   my $renderer = Gtk2::CellRendererText->new;
   $renderer->set(xalign => 0.0);

   my $col_offset = $treeview->insert_column_with_attributes 
   					(-1, "Id", $renderer,
					 text => ID);
   my $column = $treeview->get_column($col_offset - 1);
   $column->set_clickable(TRUE);

   $col_offset = $treeview->insert_column_with_attributes 
   					(-1, "Holiday", $renderer,
					 text => HEADER);
   $column = $treeview->get_column($col_offset - 1);
   $column->set_clickable(TRUE);

   $col_offset = $treeview->insert_column_with_attributes 
   					(-1, "bitch", $renderer,
					 text => 2);
   $column = $treeview->get_column($col_offset - 1);
   $column->set_clickable(FALSE);
}

my $window = Gtk2::Window->new('toplevel');
$window->set_title("Card planning sheet");
$window->signal_connect(destroy => sub { Gtk2->main_quit });

my $vbox = Gtk2::VBox->new(FALSE, 8);
$vbox->set_border_width(8);
$window->add ($vbox);

my $sw = Gtk2::ScrolledWindow->new(undef, undef);
$sw->set_shadow_type('etched-in');
$sw->set_policy('automatic', 'automatic');
$vbox->pack_start($sw, TRUE, TRUE, 0);

my $model = create_model();
my $treeview = Gtk2::TreeView->new($model);
$treeview->set_rules_hint(TRUE);
$treeview->get_selection->set_mode('multiple');

add_columns($treeview);

$sw->add($treeview);

$treeview->signal_connect(realize => sub { $_[0]->expand_all; 1 });
$window->set_default_size(650, 400);

my $hbox = Gtk2::HBox->new(FALSE, 8);

my $button = Gtk2::Button->new('_Expand');
$button->signal_connect(clicked => sub { $treeview->expand_all; 1 });
$hbox->pack_start($button, FALSE, FALSE, 0);

$button = Gtk2::Button->new('_Collapse');
$button->signal_connect(clicked => sub { $treeview->collapse_all; 1 });
$hbox->pack_start($button, FALSE, FALSE, 0);

$button = Gtk2::Button->new('_Quit');
$button->signal_connect(clicked => sub { Gtk2->main_quit });
$hbox->pack_start($button, FALSE, FALSE, 0);

$vbox->pack_start($hbox, FALSE, FALSE, 0);

$window->show_all;
Gtk2->main;

1;
