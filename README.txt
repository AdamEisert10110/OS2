Adam Eisert, 5247161

Black Box Serial

Serial	Parallel
3055	1022
2034	1526
2556	2044
4079	2187
3876	2544
4675	7066
3564	1301
8710	1268
2441	1684
6031	2748
----------------
4102	2339

Something interesting occured with this project.
When using pthread_join() at the end of the loop which found each file, to search for the keywords,
the average increase was 
4102/2339 = 1.75x paralled speed up of the code, when searching through linux-master. However, there was a memory leak
of around 4 blocks. In order to fix this, threads were joined more often, but this caused a massive slowdown,
and times were

4102/9037 = 0.4x, this was the parallel slowdown compared to serial. Dont know why. Couldnt figure it out.
I think it was something to do with pthread_join(), where threads were not fully joined. 
In order to increase speed, some memory was leaked, in order to maintain memory, speeds were slow.

In addition, when run back to back, sometimes, the same test would fail and succeed.
No changes to code. Sometimes, tests would fail.

My computer doesnt have as many cores. Its probably worse with multiple threads. 

System:    Host: LAPTOP-T8CG3687 Kernel: 4.4.0-19041-Microsoft x86_64 bits: 64 compiler: gcc v: 5.4.0 Console: tty 2
           Distro: Ubuntu 20.04.3 LTS (Focal Fossa)
Machine:   Message: No machine data: try newer kernel. Is dmidecode installed? Try -M --dmidecode.
Battery:   ID-1: battery charge: 73% condition: N/A model: N/A status: Discharging
CPU:       Topology: Quad Core model: Intel Core i7-8650U bits: 64 type: MT MCP arch: Kaby Lake rev: A
           L2 cache: 256 KiB
           flags: avx avx2 lm nx pae sse sse2 sse3 sse4_1 sse4_2 ssse3 vmx bogomips: 33792
           Speed: 2112 MHz min/max: N/A Core speeds (MHz): 1: 2112 2: 2112 3: 2112 4: 2112 5: 2112 6: 2112 7: 2112
           8: 2112
Graphics:  Message: Device data requires root.
           Display: server: X.org 1.20.13 driver: tty: 120x30
           Message: Advanced graphics data unavailable in console. Try -G --display
Audio:     Message: Device data requires root.
Network:   Message: Device data requires root.
           IF-ID-1: eth0 state: N/A speed: N/A duplex: N/A mac: 98:29:a6:73:96:2f
           IF-ID-2: eth1 state: N/A speed: N/A duplex: N/A mac: 00:ff:64:a8:2e:d1
           IF-ID-3: wifi0 state: N/A speed: N/A duplex: N/A mac: 0c:54:15:98:6d:68
           IF-ID-4: wifi1 state: N/A speed: N/A duplex: N/A mac: 0c:54:15:98:6d:69
           IF-ID-5: wifi2 state: N/A speed: N/A duplex: N/A mac: 0e:54:15:98:6d:68
Drives:    Local Storage: total: N/A used: 154.41 GiB
Sensors:   Message: No sensors data was found. Is sensors configured?
Info:      Processes: 5 Uptime: 6d 3h 41m Memory: 15.84 GiB used: 7.03 GiB (44.4%) Init: N/A Compilers: gcc: 9.3.0
           Shell: bash v: 5.0.17 inxi: 3.0.38