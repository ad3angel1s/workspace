/*
 * Testable on QEMU:
 * qemu-system-riscv64 -machine virt -m 128M -gdb tcp::1234 -S -kernel build/hello-world/hello-world
 *
 * debuggable by running a gdb instance:
 * riscv64-unknown-elf-gdb --tui build/hello-world/hello-world
 *
 * and from there attach to the gdbserver:
 * target remote :1234
 *
 * Eclipse or a graphical front-end to GDB can be used as well.
 *
 * 1. Select a Debug Configuration with "C/C++Attach to Application" type
 * 2. Select the binary to be able to load the symbols in the binary field
 * 3. In the debugger choose gdbserver, point to the proper riscv64-*gdb instance
 * 4. On the connection panale, select TCP and the 1234 port for the server above
 * Controlling the debugging seems to be a bit broken but you can use the interface
 * to inspect variables/memories etc and still use the the shell with gdb commands
 * instead of clicking on the graphical items.
 *
 * Note that the debugger will start at 0x1000 where is the ROM code which QEMU
 * runs for the virt board which does not seem to be documented or available. It will
 * then do something and relinquish control of our application at the RAM starting
 * point which is at 0x80000000 which is where we have loaded our binary. Check the
 * value from the DTS and in the linker script
 *
 */

int main(void) {
    int a = 4;
    int b = 12;
    while (1) {
        int c = a + b;
    }
    return 0;
}
