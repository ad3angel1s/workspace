/*
 * Testable on QEMU:
 * qemu-system-riscv64 -machine virt -m 128M -gdb tcp::1234 -S -kernel build/hello-world/hello-world
 *
 * debuggable by running a gdb instance:
 * riscv64-unknown-elf-gdb --tui build/hello-world/hello-world
 *
 * and from there attach to the gdbserver:
 * target remote :1234
 */

int main(void) {
    int a = 4;
    int b = 12;
    while (1) {
        int c = a + b;
    }
    return 0;
}
