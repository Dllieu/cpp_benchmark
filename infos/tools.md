#perf
TODO

#objdump

Check generated code, specially if assuming auto-vectorization

```shell
objdump -M intel -D {object_file.o}
```

#ulimit
limit system-wide resource use (e.g. avoid forkbomb / limit ram comsumption / ...)

```shell
ulimit -a
```

#sysctl
configure kernel parameters at runtime

```shell
#display all variables
sysctl -a
```

