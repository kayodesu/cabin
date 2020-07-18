package sunmisc;

import sun.misc.Unsafe;

/**
 * Status: Pass
 */
public class UnsafeMemoryTest {
    
    public static void main(String[] args) {
        Unsafe unsafe = UnsafeGetter.getUnsafe();
        final long address = unsafe.allocateMemory(8);

        unsafe.putAddress(address, address);
        System.out.println(address == unsafe.getAddress(address));

        unsafe.putByte(address, (byte)7);
        System.out.println((byte)7 == unsafe.getByte(address));

        unsafe.putByte(address, (byte)-7);
        System.out.println((byte)-7 == unsafe.getByte(address));

        unsafe.putShort(address, (short)500);
        System.out.println((short)500 == unsafe.getShort(address));

        unsafe.putShort(address, (short)-500);
        System.out.println((short)-500 == unsafe.getShort(address));

        unsafe.putChar(address, 'c');
        System.out.println('c' == unsafe.getChar(address));

        unsafe.putInt(address, 65536);
        System.out.println(65536 == unsafe.getInt(address));

        unsafe.putInt(address, -65536);
        System.out.println(-65536 == unsafe.getInt(address));

        unsafe.putLong(address, 9999999999L);
        System.out.println(9999999999L == unsafe.getLong(address));

        unsafe.putLong(address, -9999999999L);
        System.out.println(-9999999999L == unsafe.getLong(address));

        unsafe.putFloat(address, 3.14f);
        System.out.println(Float.compare(unsafe.getFloat(address), 3.14f) == 0 ?"Pass":"Fail");

        unsafe.putDouble(address, 2.71828);
        System.out.println(Double.compare(unsafe.getDouble(address), 2.71828) == 0 ?"Pass":"Fail");

        long newAddress = unsafe.reallocateMemory(address, 100);
        unsafe.freeMemory(newAddress);
    }
    
}
