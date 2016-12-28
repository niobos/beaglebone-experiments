#!/usr/bin/env perl

use strict;
use warnings;

if( @ARGV == 0 ) {
  print STDERR "Usage: $0 /dev/pru_device\n";
  exit 64;
}

open my $fh, "+<", $ARGV[0] or die "Could not open {$ARGV[0]}: $!";

syswrite($fh, "0x00", 1);

my $buf;
sysread($fh, $buf, 4*5);

my @measurements = unpack "a5" x (length($buf)/5), $buf;

sub decode {
  my ($d) = @_;
  my @byte = unpack "C5", $d;
  my $sum = $byte[0] + $byte[1] + $byte[2] + $byte[3];
  if( ($sum & 0xff) != $byte[4] ) {
    return "checksum error: " . join(' ', map {sprintf "0x%02x", $_} @byte);
  }

  my $t = ($byte[2] << 8) | $byte[3];
  if( $t & 0x8000 ) {
    # 1's complement, according to https://github.com/adafruit/DHT-sensor-library/blob/master/DHT.cpp#L53
    $t = $t & 0x7fff;
    $t = -$t;
  }
  $t = $t/10;

  my $h = ($byte[0] << 8) | $byte[1];
  $h = $h/10;

  return sprintf "%3.1fC %3.1f%%", $t, $h;
}

print map { decode($_) . "\n" } @measurements;
