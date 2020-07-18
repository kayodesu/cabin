package sunmisc;

import sun.misc.Unsafe;

import java.lang.reflect.Field;

public class UnsafeTest {
    private int non_static_int;
    private static int static_int = -100;

    public static void main(String[] args) throws NoSuchFieldException {
        Unsafe unsafe = UnsafeGetter.getUnsafe();
        Field f = UnsafeTest.class.getDeclaredField("static_int");

        Object o = unsafe.staticFieldBase(f);
        System.out.println(o);

        long l = unsafe.staticFieldOffset(f);
        System.out.println(l);

        System.out.println(unsafe.getInt(unsafe.staticFieldBase(f), unsafe.staticFieldOffset(f)));
    }
}
