# clumsy

__clumsy makes your network condition on Windows significantly worse, but in a managed and interactive manner.__

Leveraging the awesome [WinDivert](http://reqrypt.org/windivert.html), clumsy stops living network packets and capture them, lag/drop/tamper/.. the packets on demand, then send them away. Whether you want to track down weird bugs related to broken network, or evaluate your application on poor connections, clumsy will come in handy:

* No installation.
* No need for proxy setup or code change in your application.
* System wide network capturing means it works on any application.
* Works even if you're offline (ie, connecting from localhost to localhost).
* Your application keeps running, while clumsy can start and stop anytime.
* Interactive control how bad the network can be, with enough visual feedback to tell you what's going on.

See [this page](http://jagt.github.io/clumsy) for more info and build instructions.


## **This Fork**

This is a fork of the original [jagt/clumsy](https://github.com/jagt/clumsy) to make clumsy a **command line app**.

## **Build**

### **Prerequisites**

- Git
- GENie - for build generation
- An Editor or IDE - for debug builds and devlopment
- MSYS2 - for release builds

    **Clone the repo**
    ```
    git clone https://github.com/ibarhimcetin/clumsy.git
    ```

    **Change directory**
    ```
    cd clumsy
    ```

    **Generate Makefile**
    ```
    genie gmake
    ```

    **Build**
    ```
    make -C .\build
    ```
    *Note:* This command will create an x64 debug exe file. If you want another build, you can check the help section with
    ```
    make -C .\build help
    ```

    **Exe will be in here**
    ```
    .\bin\gmake\Debug\x64\clumsy.exe
    ```

## **Usage**

You have to run terminal as administrator
```
clumsy.exe -f <filter> -m <module> -v <value> <run time>
```

### **Usage Example**
This command will make 100ms lag for 30 seconds
```
clumsy.exe -f "outbound and !loopback" -m "lag" -v "100" "30"
```
`run time` is optional. If you want to run clumsy forever, do not set it.

### **Multi Module Usage Example**
Also, you can use more than one module at the same time. To do that, you can use a command like this
```
clumsy.exe -f "outbound" -m "lag" -v "320" -m "drop" -v "20" 120
```
The command above will make 320ms lag and drop with a 20% chance at the same time for 120 seconds.

***Important:*** Pay attention to the order of arguments.

## License

MIT
