#!/usr/bin/perl

use strict;
use warnings;
use File::Path qw(make_path remove_tree);  # Add the 'remove_tree' function here
use Cwd;
use POSIX ":sys_wait_h";

my $interrupted = 0;

$SIG{INT} = \&interrupt_handler;

sub interrupt_handler {
    $interrupted = 1;
    print "User interrupted the execution.\n";
    exit 0;
}

sub run_command {
    my ($command) = @_;
    print "\nRunning command: $command\n";

    my $output = `$command 2>&1`;
    my $exit_status = $? >> 8;
    print "$output";
    if ($exit_status != 0) {
        print "Command '$command' failed at line " . __LINE__ . ". Press Enter to exit...";
        <STDIN>;
        exit 1;
    }
}

sub exec_file {
    my ($file) = @_;
    print "\nRunning file: $file\n";

    if (-x $file) {
        system($file);
    } else {
        print "File '$file' not found or not executable. Skipping execution.\n";
    }
}

sub run_commands {
    my (@commands) = @_;
    foreach my $command (@commands) {
        last if $interrupted; # Check if Ctrl+C was pressed
        run_command($command);
    }
}

my $project_name = "power";
my $build_dir = "build";
my $generator = "Unix Makefiles";

if (@ARGV >= 2 && $ARGV[1] eq "vs") {
    $generator = "Visual Studio 17 2022";
}

if (@ARGV) {

    if ($ARGV[0] eq "clean") {

        print "Cleaning...\n";
        if (-d $build_dir) {
            remove_tree($build_dir);
        }
        exit 0;

    } elsif ($ARGV[0] eq "cmake") {

        unless (-d $build_dir) {
            make_path($build_dir) or die "Failed to create build directory: $!";
        }

        chdir $build_dir or die "Failed to change directory to $build_dir at line " . __LINE__ . ": $!";

        my @commands = (
            "cmake -G \"$generator\" ..",
            "make"
        );

        chdir $project_name or die "Failed to change directory to $project_name at line " . __LINE__ . ": $!";
        my $current_dir = getcwd();
        print "Current directory: $current_dir\n";

        run_commands(@commands);
        exec_file("./main")

    }elsif($ARGV[0] eq "make"){

        chdir $build_dir or die "Failed to change directory to $build_dir at line " . __LINE__ . ": $!";
        run_command("make");
        chdir $project_name or die "Failed to change directory to $project_name at line " . __LINE__ . ": $!";
        print "Current directory: " . getcwd() . "\n";
        exec_file("./main");

    }else {
        print "Invalid option. Please use 'clean' or 'cmake' as command-line arguments.\n";
        exit 1;
    }
} else {

    print "No command-line argument provided. Please use 'clean' or 'cmake'.\n";
    exit 1;

}

print "Press Enter to exit...";
<STDIN>;


