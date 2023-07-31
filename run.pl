#!/usr/bin/perl

use strict;
use warnings;
use File::Path qw(make_path remove_tree);  # Add the 'remove_tree' function here
use Cwd;

my $error_message = "Failed to create build directory: ";
my $project_name = "power";
my $build_dir = "build";

sub run_command {
    my ($command) = @_;
    my $exit_status = system($command);
    if ($exit_status != 0) {
        print "Command '$command' failed at line " . __LINE__ . ". Press Enter to exit...";
        <STDIN>;
        exit 1;
    }
    return $exit_status;
}

if (@ARGV) {
    if ($ARGV[0] eq "clean") {
        print "Cleaning...\n";
        if (-d $build_dir) {
            remove_tree($build_dir) or die $error_message;
        }
        exit 0;
    } elsif ($ARGV[0] eq "cmake") {
        my $generator = "Unix Makefiles";
        if (@ARGV >= 2 && $ARGV[1] eq "vs") {
            $generator = "\"Visual Studio 17 2022\"";
        }

        unless (-d $build_dir) {
            make_path($build_dir) or die $error_message;
        }

        chdir $build_dir or die "Failed to change directory to $build_dir at line " . __LINE__ . ": $!";

        run_command("cmake -G $generator ..");

        run_command("make");

        chdir $project_name or die "Failed to change directory to $project_name at line " . __LINE__ . ": $!";
        my $current_dir = getcwd();
        print "Current directory: $current_dir\n";

        run_command("./main.exe");
    } elsif ($ARGV[0] eq "run") {
        chdir $build_dir or die "Failed to change directory to $build_dir at line " . __LINE__ . ": $!";
        chdir $project_name or die "Failed to change directory to $project_name at line " . __LINE__ . ": $!";
        my $current_dir = getcwd();
        print "Current directory: $current_dir\n";
        run_command("./main.exe");
    } else {
        print "Invalid option. Please use 'clean', 'cmake', or 'run' as command-line arguments.\n";
        exit 1;
    }
} else {
    print "No command-line argument provided. Please use 'clean', 'cmake', or 'run'.\n";
    exit 1;
}

print "Press Enter to exit...";
<STDIN>;
