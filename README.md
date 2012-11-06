Application Accounting for EGI InSPIRE
======================================

What it does
------------
This software supplies a sensor for application accounting, with the
goal to account start and end time as well as the name of the launched
programs.


How it works
------------
### Client (library)
The client is a library that overwrites the libc `execve()` function
used to call a program. Amongst others, the Bourne Again Shell (bash)
uses this function after a fork to call a program. The client intercepts
the function and wraps the accounting logic around this call. The
information is then sent to the server.

### Server (daemon)
The server receives the sensor data from all clients and is meant to run
in the background. Upon receiving data, the server can then look up the
user in the girdmap file to translate the local user to a global
identifier.

### Communication
Communication between the Client and the Server is implemented using
[ZeroMQ](http://zero.mq). If the Client can not connect to the Server, a
background process is launched after the completion of the application
that sends the message once the Server is running again.


How to run it
-------------
The project uses CMake for a multiplatform build. Build requirements
are:
* CMake (~= 2.8.0)
* ZeroMQ (~= 2.2.0)
* LibYAML (~= 0.1.4)

To get started, type:
```sh
mkdir build
cd build
cmake ../src
make
```

Launch client (DYLD... are for OS X, ignore them if you want):
```sh
LD_PRELOAD=./acc_bash DYLD_INSERT_LIBRARIES=./acc_bash DYLD_FORCE_FLAT_NAMESPACE=y bash
```  

Lauch server (in a seperate terminal):
```sh
./server
```

Now if you launch programs on the bash you should see accounting records
being outputted by the server. Note that `echo` is not a program but a
bash buildin, therefore it does not create a record (nor does it launch
any application).

To Do
-----
* Init script for the server
* Daemonize server
* Lookup of users in a gridmap-file)
