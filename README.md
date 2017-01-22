<pre>
___  ___    ___  ___                  _           
|  \/  |    |  \/  |                 | |          
| .  . |_ __| .  . | __ _ _ __   __ _| | ___ _ __ 
| |\/| | '__| |\/| |/ _` | '_ \ / _` | |/ _ \ '__|
| |  | | |  | |  | | (_| | | | | (_| | |  __/ |   
\_|  |_/_|  \_|  |_/\__,_|_| |_|\__, |_|\___|_|   
                                 __/ |            
                                |___/             
</pre>

###  Author: Ewan Crawford
#### Email: ewan.cr@gmail.com
#### License: MIT

Small and quick mangler for function signatures.
Aims to be cross platform to support itanium and windows
mangling. As a result does the mangling itself rather than
calling library function like `__cxa_demangle`.

### Test status
[![Build Status](https://travis-ci.org/EwanC/MrMangler.svg)](https://travis-ci.org/EwanC/MrMangler) Master

### TODO

* Windows Struct & Union back references
* doxy-comment & Format
* Clean up test scripts

### Status
The project is still in early stages on development, so
is not expected to compile on platforms other than Linux.

Right now C language mangling with some C++ features like
references is the target. To see exactly what's supported
look at the file [test/inputs.txt](test/inputs.txt), containing the inputs
tested against.

### Examples
```bash
$ echo "foo(signed char a, float b, ...)" | ./MrMangler
_Z3fooafz

$ echo "bar(const uint32_t *const)" | ./MrMangler
_Z3barKPKj

$ echo "super_duper_function(struct my_struct **)" | ./MrMangler
_Z20super_duper_functionPP9my_struct

$ echo "AliceBob(signed char& packet)" | ./MrMangler
_Z8AliceBobRa

$ echo "int foo(float, bool (**)(char, int))" | ./MrMangler
_Z3foofPPFbciE

$ echo "const int16_t func_1(void)" | ./MrMangler -w
?func_1@@YA?BFXZ
```
