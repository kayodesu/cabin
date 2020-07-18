package sunmisc;

import sun.misc.Unsafe;

import java.lang.reflect.Field;

/**
 * Status: Pass
 */
public class UnsafeObjectTest1 {
    
    private static final Unsafe unsafe = UnsafeTest.getUnsafe();
    
    private boolean z;
    private byte b;
    private char c;
    private short s;
    private int i;
    private long j;
    private float f;
    private double d;
    private String str;

    public static void booleanField() {
        UnsafeObjectTest1 obj = new UnsafeObjectTest1();
        long zOffset = objectFieldOffset("z");

        System.out.println(!unsafe.getBoolean(obj, zOffset));
        unsafe.putBoolean(obj, zOffset, true);
        System.out.println(unsafe.getBoolean(obj, zOffset));
    }

    public static void byteField() {
        UnsafeObjectTest1 obj = new UnsafeObjectTest1();
        long bOffset = objectFieldOffset("b");

        System.out.println((byte)0 == unsafe.getByte(obj, bOffset));
        unsafe.putByte(obj, bOffset, (byte)17);
        System.out.println((byte)17 == unsafe.getByte(obj, bOffset));
    }

    public static void charField() {
        UnsafeObjectTest1 obj = new UnsafeObjectTest1();
        long cOffset = objectFieldOffset("c");

        System.out.println('\0' == unsafe.getChar(obj, cOffset));
        unsafe.putChar(obj, cOffset, 'x');
        System.out.println('x' == unsafe.getChar(obj, cOffset));
    }

    public static void shortField() {
        UnsafeObjectTest1 obj = new UnsafeObjectTest1();
        long sOffset = objectFieldOffset("s");

        System.out.println((short)0 == unsafe.getShort(obj, sOffset));
        unsafe.putShort(obj, sOffset, (short)12345);
        System.out.println((short)12345 == unsafe.getShort(obj, sOffset));
    }

    public static void intField() {
        UnsafeObjectTest1 obj = new UnsafeObjectTest1();
        long iOffset = objectFieldOffset("i");

        System.out.println(0 == unsafe.getInt(obj, iOffset));
        unsafe.putInt(obj, iOffset, 12345);
        System.out.println(12345 == unsafe.getInt(obj, iOffset));
    }

    public static void longField() {
        UnsafeObjectTest1 obj = new UnsafeObjectTest1();
        long jOffset = objectFieldOffset("j");

        System.out.println(0L == unsafe.getLong(obj, jOffset));
        unsafe.putLong(obj, jOffset, 12345L);
        System.out.println(12345L == unsafe.getLong(obj, jOffset));
    }

    public static void floatField() {
        UnsafeObjectTest1 obj = new UnsafeObjectTest1();
        long fOffset = objectFieldOffset("f");

        System.out.println(Float.compare(0f, unsafe.getFloat(obj, fOffset)) == 0);
        unsafe.putFloat(obj, fOffset, 3.14f);
        System.out.println(Float.compare(3.14f, unsafe.getFloat(obj, fOffset)) == 0);
    }

    public static void doubleField() {
        UnsafeObjectTest1 obj = new UnsafeObjectTest1();
        long dOffset = objectFieldOffset("d");

        System.out.println(0 == unsafe.getDouble(obj, dOffset));
        unsafe.putDouble(obj, dOffset, 3.14);
        System.out.println(Double.compare(3.14, unsafe.getDouble(obj, dOffset)) == 0);
    }
    
    public static void objectField() {
        UnsafeObjectTest1 obj = new UnsafeObjectTest1();
        long strOffset = objectFieldOffset("str");

        System.out.println(null == unsafe.getObject(obj, strOffset));
        unsafe.putObject(obj, strOffset, "world");
        System.out.println("world" == unsafe.getObject(obj, strOffset));
    }

    private static long objectFieldOffset(String fieldName) {
        try {
            Field f = UnsafeObjectTest1.class.getDeclaredField(fieldName);
            return unsafe.objectFieldOffset(f);
        } catch (NoSuchFieldException e) {
            throw new RuntimeException(e);
        }
    }

    public static void main(String[] args) {
        booleanField();
        byteField();
        charField();
        shortField();
        intField();
        longField();
        floatField();
        doubleField();
        objectField();
    }

}
