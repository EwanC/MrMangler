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

#### Status
The project is still in early stages on development, so
is not expected to compile on platforms other than Linux.

Currently builtin types are only supported until such a
time as the project becomes stable.

#### Examples
```
$ echo "foo(signed char a, float b, ...)" | ./MrMangler
_Z3fooafz
```

#### TODO
* [ ] Tests
* [ ] Windows mangling
* [ ] Build on host platforms other than linux
