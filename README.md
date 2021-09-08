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


## **Details**

Simulate network latency, delay, packet loss with clumsy on Windows 7/8/10:

![]()

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

## Usage

You have to run terminal as administrator
```
clumsy.exe <filter> <function> <value> <run time>
```

### Usage Example
This command will make 100ms lag for 30 seconds
```
clumsy.exe outbound lag 100 30
```

If you want to run clumsy forever, you must set the `run time` to 0.

## License

MIT
