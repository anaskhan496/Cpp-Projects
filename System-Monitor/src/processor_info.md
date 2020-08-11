 
    Jump to content
    Email for accessibility support

Project: System Monitor

1. Introduction
2. htop
3. Starter Code
4. Project Structure
5. Build Tools
6. System Class
7. System Data
8. LinuxParser Namespace
9. String Parsing
10. Processor Class
11. Processor Data
12. Process Class
13. Process Data
14. Goal
15. Project Workspace

    16. Project: System Monitor

KnowledgeSearch project Q&A

    Student HubChat with peers and mentors

Toggle Sidebar
Processor Data
Processor Data

Linux stores processor utilization data within the /proc/stat file.
`cat /proc/stat`

This data is more complex than most of the other data necessary to complete this project.

For example, /proc/stat contains aggregate processor information (on the "cpu" line) and individual processor information (on the "cpu0", "cpu1", etc. lines). Indeed, htop displays utilization information for each individual processor.

For this project, however, you only need to display aggregate CPU information, which you can find on the "cpu" line of /proc/stat.

If you would like to add individual processor information to your system monitor project, go for it!
Data

/proc/stat contains 10 integer values for each processor. The Linux source code documents each of these numbers:

    The very first "cpu" line aggregates the numbers in all of the other "cpuN" lines. These numbers identify the amount of time the CPU has spent performing different kinds of work. Time units are in USER_HZ (typically hundredths of a second). The meanings of the columns are as follows, from left to right:

        user: normal processes executing in user mode
        nice: niced processes executing in user mode
        system: processes executing in kernel mode
        idle: twiddling thumbs
        iowait: In a word, iowait stands for waiting for I/O to complete. But there are several problems:
            Cpu will not wait for I/O to complete, iowait is the time that a task is waiting for I/O to complete. When cpu goes into idle state for outstanding task io, another task will be scheduled on this CPU.
            In a multi-core CPU, the task waiting for I/O to complete is not running on any CPU, so the iowait of each CPU is difficult to calculate.
            The value of iowait field in /proc/stat will decrease in certain conditions. So, the iowait is not reliable by reading from /proc/stat.
        irq: servicing interrupts
        softirq: servicing softirqs
        steal: involuntary wait
        guest: running a normal guest
        guest_nice: running a niced guest

Even once you know what each of these numbers represents, it's still a challenge to determine exactly how to use these figures to calculate processor utilization. This guide and this StackOverflow post are helpful.
Measurement Interval

Once you've parsed /proc/stat and calculated the processor utilization, you've got what you need for this project. Congratulations!

However, when you run your system monitor, you might notice that the process utilization seems very stable. Too stable.

That's because the processor data in /proc/stat is measured since boot. If the system has been up for a long time, a temporary interval of even extreme system utilization is unlikely to change the long-term average statistics very much. This means that the processor could be red-lining right now but the system monitor might still show a relatively underutilized processor, if the processor has spent most of the time since boot in an idle state.

You might want to update the system monitor to report the current utilization of the processor, rather than the long-term average utilization since boot. You would need to measure the difference in system utilization between two points in time relatively close to the present. A formula like:

Δ active time units / Δ total time units

Consider this a bonus challenge that is not required to pass the project.

// Guest time is already accounted in usertime
usertime = usertime - guest;                     # As you see here, it subtracts guest from user time
nicetime = nicetime - guestnice;                 # and guest_nice from nice time
// Fields existing on kernels >= 2.6
// (and RHEL's patched kernel 2.4...)
idlealltime = idletime + ioWait;                 # ioWait is added in the idleTime
systemalltime = systemtime + irq + softIrq;
virtalltime = guest + guestnice;
totaltime = usertime + nicetime + systemalltime + idlealltime + steal + virtalltime;

   user    nice   system  idle      iowait irq   softirq  steal  guest  guest_nice
cpu  74608   2520   24433   1117073   6176   4054  0        0      0      0

PrevIdle = previdle + previowait
Idle = idle + iowait

PrevNonIdle = prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal
NonIdle = user + nice + system + irq + softirq + steal

PrevTotal = PrevIdle + PrevNonIdle
Total = Idle + NonIdle

# differentiate: actual value minus the previous one
totald = Total - PrevTotal
idled = Idle - PrevIdle

CPU_Percentage = (totald - idled)/totald