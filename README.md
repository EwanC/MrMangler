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

#### Examples
```
$ echo "foo(signed char a, float b, ...)" | ./MrMangler"
_Z3foofz
```

#### TOOD
* [ ] Tests
* [ ] Windows
