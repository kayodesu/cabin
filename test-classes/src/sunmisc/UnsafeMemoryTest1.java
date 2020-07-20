package sunmisc;

import sun.misc.Unsafe;

import java.lang.reflect.Field;

/**
 * Status: Fail
 */
public class UnsafeMemoryTest1 {
    static Unsafe unsafe = UnsafeTest.getUnsafe();

    int i = 10;
    int j = 20;
    
    public static void main(String[] args) {
        UnsafeMemoryTest1 test = new UnsafeMemoryTest1();

        System.out.println(test.i == 10);
        // set 'i' is 0
        unsafe.setMemory(test, objectFieldOffset("i"), 4 /* sizeof(int) */, (byte) 0);
        System.out.println(test.i == 0);

        System.out.println(test.j == 20);
        // copy 'i' to 'j'
        unsafe.copyMemory(test, objectFieldOffset("i"), test, objectFieldOffset("j"), 4 /* sizeof(int) */);
        System.out.println(test.j == 0);
    }

    private static long objectFieldOffset(String fieldName) {
        try {
            Field f = UnsafeObjectTest1.class.getDeclaredField(fieldName);
            return UnsafeTest.getUnsafe().objectFieldOffset(f);
        } catch (NoSuchFieldException e) {
            throw new RuntimeException(e);
        }
    }
}
