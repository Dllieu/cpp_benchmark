# perf
TODO

## strace

trace system calls and signals

```shell
strace -f -ff -e trace=open,stat,read,write gnome-calculator
```

## netstat

Check network configuration (i.e. check multicast group)

```shell
netstat -gn
```

## objdump

Check generated code, specially if assuming auto-vectorization

```shell
objdump -M intel -D {object_file.o}
```

## ulimit
limit system-wide resource use (e.g. avoid forkbomb / limit ram comsumption / ...)

```shell
ulimit -a
```

## sysctl
configure kernel parameters at runtime

```shell
# display all variables
sysctl -a
```

## VTune Amplifier XE 2016+
### Install GUI (windows)
- Install VTune Amplifier XE GUI on Windows (Analyzer)
 - You need admin right
 - Retrieve the package (VTune_Amplifier_XE_2016_update3_setup.exe)
 - Install all the certificates (should be automatic if firewall is open)
 - Run the setup

### Server (linux)
- Collector
 - Install VTune + drivers

### Basic Test
- After Installation, run VTune Amplifier GUI
- Create new project
- In Analysis Target, select **remote Linux (SSH)**
- Put the PID, the SSH information, and selection **Attach to Process**
- In **Advanced**, configure the VTune Amplifier installation directory
- Start the analysis

### Documentation
- Tuning Guides and Performance Analysis Papers
 - [Guides & Tutorials with Samples](https://software.intel.com/en-us/intel-vtune-amplifier-xe-support/training)
 - [Top-down analysis method](https://software.intel.com/en-us/top-down-microarchitecture-analysis-method-win)
 - [Metrics to checks when improving the performance, **processor specific**](https://software.intel.com/en-us/articles/processor-specific-performance-analysis-papers)
 - [Metrics Reference](https://software.intel.com/en-us/node/596788)
 
