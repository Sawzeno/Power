#!/usr/local/bin/perl

use strict;
use warnings;
use File::Path qw(make_path remove_tree);  
use Cwd;
use POSIX ":sys_wait_h";
use POSIX qw(sysconf _SC_PAGESIZE);
use Time::HiRes qw(gettimeofday tv_interval);
use List::Util qw(min);
use Proc::ProcessTable;
use Proc::ProcessTable::Process;
use Fcntl qw(F_GETFL F_SETFL O_NONBLOCK);

my $interrupted = 0;
my $page_size = sysconf(_SC_PAGESIZE);
my @child_pids;

$SIG{INT} = \&interrupt_handler;

sub interrupt_handler {
    $interrupted = 1;
    print "User interrupted the execution.\n";
    # Terminate all child processes
    foreach my $pid (@child_pids) {
        kill 'INT', $pid;
    }

    # Wait for child processes to terminate
    foreach my $pid (@child_pids) {
        waitpid($pid, 0);
        print "Process with PID $pid terminated.\n";
    }

    exit 1;
}

sub run_command {
    my ($command) = @_;
    print "\nRunning command: $command\n";

    my $output = "";
    open(my $output_fh, '-|', $command) or die "Failed to run command: $!";
    while (my $line = <$output_fh>) {
        last if $interrupted; # Check if Ctrl+C was pressed
        $output .= $line;
        print $line; # Optional: If you want to print the output line by line
    }
    close($output_fh);

    my $exit_status = $? >> 8;
    if ($exit_status != 0) {
        print "Command '$command' failed at line " . __LINE__ . ". Press Enter to exit...";
        <STDIN>;
        exit 1;
    }

    return $output;
}

sub error_handler {
    my @pids = @_;

    # Kill all the subprocesses and the parent process
    foreach my $pid (@pids) {
        kill 'INT', $pid;
    }

    die "User interrupted the execution.\n";
}

sub monitor_proc {
    my ($pid, $start_time) = @_;
    my $update_interval = 1; # Update interval in seconds
    my $output = "";
    my $process_running = 1;
    my $parent_process = $$; # Get the PID of the parent process
    my $page_size = sysconf(_SC_PAGESIZE); # Get the system's page size in bytes

    print "Parent PID: $parent_process, Child PID: $pid\n";

    while ($process_running) {
        my $process_table = Proc::ProcessTable->new();
        my $process_info;
        foreach my $p (@{$process_table->table}) {
            if ($p->pid == $parent_process) {
                $process_info = $p;
                last;
            }
        }
    
        last unless defined $process_info; # Check if the parent process is still running
    
        my $ps_output = `ps -p $pid -o %cpu,%mem,command`;
        my ($cpu, $mem, $command) = $ps_output =~ /(\d+\.\d+)\s+(\d+\.\d+)\s+(.*)/;
        my $cpu_percentage = defined $cpu ? $cpu : 0;
        my $mem_usage = $process_info->rss * $page_size / 1024; # Memory usage in KB
        my $cpu_time = $process_info->pctcpu;
        my $mem_percentage = $process_info->pctmem;
    
        last unless $cpu_time; # Check if the child process is still running
    
        my $end_time = [gettimeofday];
        my $elapsed_time = tv_interval($start_time, $end_time);
    
        printf("\rElapsed time: %0.2f seconds, CPU time: %0.2f%%, Memory usage: %0.2f KB, CPU percentage: %0.2f%%, Memory percentage: %0.2f%%, Command: %s",
            $elapsed_time, $cpu_time, $mem_usage, $cpu_percentage, $mem_percentage, $command);
    
        $output .= $ps_output;
        $process_running = kill 0, $pid; # Check if the process is still running
        last if waitpid($pid, WNOHANG) != 0; # Check if the child process has finished
        sleep $update_interval;
    }

    print "\n"; # Newline to end the line properly after the loop

    return $output;
}

sub exec_file {
    my ($file) = @_;
    print "\n Running file: $file\n";

    my $start_time = [gettimeofday];
    my $pid = fork();
    die "Failed to fork: $!" unless defined $pid;

    if ($pid == 0) { # Child process
        my $child_pid = $$;
        eval {
            local $SIG{INT} = sub { die "User interrupted the execution.\n" };
            exec($file);
            die "Failed to execute $file: $!";
        };

        # If an exception occurred, kill the parent process
        error_handler($child_pid, $$) if $@;
    } else { # Parent process
        my $parent_pid = $$;
        my $process_table = Proc::ProcessTable->new();

        monitor_proc($pid, $start_time, $process_table);
    }
}

sub run_commands {
    my (@commands) = @_;
    foreach my $command (@commands) {
        last if $interrupted;
        run_command($command);
        last if $interrupted;
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

        run_commands(@commands);
        chdir $project_name or die "Failed to change directory to $project_name at line " . __LINE__ . ": $!";
        my $current_dir = getcwd();
        print "Current directory: $current_dir\n";

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
