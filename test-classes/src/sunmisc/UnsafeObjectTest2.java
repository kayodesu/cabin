package sunmisc;

import sun.misc.Unsafe;

import java.lang.reflect.Field;

/**
 * Status: Pass
 */
public class UnsafeObjectTest2 {
    
    private static final Unsafe unsafe = UnsafeTest.getUnsafe();
    
    private static boolean z;
    private static byte b;
    private static char c;
    private static short s;
    private static int i;
    private static long j;
    private static float f;
    private static double d;
    private static String str;

    public static void staticBooleanField() {
        BaseOffset bo = staticFieldBaseOffset("z");

        System.out.println(!unsafe.getBoolean(bo.base, bo.offset));
        unsafe.putBoolean(bo.base, bo.offset, true);
        System.out.println(unsafe.getBoolean(bo.base, bo.offset));
    }

    public static void staticByteField() {
        BaseOffset bo = staticFieldBaseOffset("b");

        System.out.println((byte)0 == unsafe.getByte(bo.base, bo.offset));
        unsafe.putByte(bo.base, bo.offset, (byte)17);
        System.out.println((byte)17 == unsafe.getByte(bo.base, bo.offset));
    }

    public static void staticCharField() {
        BaseOffset bo = staticFieldBaseOffset("c");

        System.out.println('\0' == unsafe.getChar(bo.base, bo.offset));
        unsafe.putChar(bo.base, bo.offset, 'x');
        System.out.println('x' == unsafe.getChar(bo.base, bo.offset));
    }

    public static void staticShortField() {
        BaseOffset bo = staticFieldBaseOffset("s");

        System.out.println((short)0 == unsafe.getShort(bo.base, bo.offset));
        unsafe.putShort(bo.base, bo.offset, (short)12345);
        System.out.println((short)12345 == unsafe.getShort(bo.base, bo.offset));
    }

    public static void staticIntField() {
        BaseOffset bo = staticFieldBaseOffset("i");

        System.out.println(0 == unsafe.getInt(bo.base, bo.offset));
        unsafe.putInt(bo.base, bo.offset, 12345);
        System.out.println(12345 == unsafe.getInt(bo.base, bo.offset));
    }

    public static void staticLongField() {
        BaseOffset bo = staticFieldBaseOffset("j");

        System.out.println(0L == unsafe.getLong(bo.base, bo.offset));
        unsafe.putLong(bo.base, bo.offset, 12345L);
        System.out.println(12345L == unsafe.getLong(bo.base, bo.offset));
    }

    public static void staticFloatField() {
        BaseOffset bo = staticFieldBaseOffset("f");

        System.out.println(Float.compare(0f, unsafe.getFloat(bo.base, bo.offset)) == 0);
        unsafe.putFloat(bo.base, bo.offset, 3.14f);
        System.out.println(Float.compare(3.14f, unsafe.getFloat(bo.base, bo.offset)) == 0);
    }

    public static void staticDoubleField() {
        BaseOffset bo = staticFieldBaseOffset("d");

        System.out.println(0 == unsafe.getDouble(bo.base, bo.offset));
        unsafe.putDouble(bo.base, bo.offset, 3.14);
        System.out.println(Double.compare(3.14, unsafe.getDouble(bo.base, bo.offset)) == 0);
    }
    
    public static void staticObjectField() {
        BaseOffset bo = staticFieldBaseOffset("str");

        System.out.println(null == unsafe.getObject(bo.base, bo.offset));
        unsafe.putObject(bo.base, bo.offset, "world");
        System.out.println("world" == unsafe.getObject(bo.base, bo.offset));
    }

    private static class BaseOffset {
        Object base;
        long offset;

        BaseOffset(Object base, long offset) {
            this.base = base;
            this.offset = offset;
        }
    }

    private static BaseOffset staticFieldBaseOffset(String fieldName) {
        try {
            Field f = UnsafeObjectTest2.class.getDeclaredField(fieldName);
            return new BaseOffset(unsafe.staticFieldBase(f), unsafe.staticFieldOffset(f));
        } catch (NoSuchFieldException e) {
            throw new RuntimeException(e);
        }
    }

    public static void main(String[] args) {
        staticBooleanField();
        staticByteField();
        staticCharField();
        staticShortField();
        staticIntField();
        staticLongField();
        staticFloatField();
        staticDoubleField();
        staticObjectField();
    }

}
