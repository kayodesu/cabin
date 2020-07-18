package sunmisc;

import sun.misc.Unsafe;

import java.lang.reflect.Field;

/**
 * Status: Pass
 */
public class UnsafeObjectTest {
    
    private static final Unsafe unsafe = UnsafeGetter.getUnsafe();
    
    private boolean z;
    private byte b;
    private char c;
    private short s;
    private int i;
    private long j;
    private float f;
    private double d;
    private String str;

    public static void booleanArray() {
        boolean[] arr = {false, true, false};

        long booleanArrBaseOffset = unsafe.arrayBaseOffset(boolean[].class);
        long booleanArrIndexScale = unsafe.arrayIndexScale(boolean[].class);
        long index1 = booleanArrBaseOffset + booleanArrIndexScale;

        System.out.println(unsafe.getBoolean(arr, index1));
        unsafe.putBoolean(arr, index1, false);
        System.out.println(!unsafe.getBoolean(arr, index1));
    }

    public static void booleanField() {
        UnsafeObjectTest obj = new UnsafeObjectTest();
        long zOffset = objectFieldOffset("z");

        System.out.println(!unsafe.getBoolean(obj, zOffset));
        unsafe.putBoolean(obj, zOffset, true);
        System.out.println(unsafe.getBoolean(obj, zOffset));
    }

    public static void byteArray() {
        byte[] arr = {1, 3, 8};

        long byteArrBaseOffset = unsafe.arrayBaseOffset(byte[].class);
        long byteArrIndexScale = unsafe.arrayIndexScale(byte[].class);
        long index1 = byteArrBaseOffset + byteArrIndexScale;

        System.out.println((byte)3 == unsafe.getByte(arr, index1));
        unsafe.putByte(arr, index1, (byte)120);
        System.out.println((byte)120 == unsafe.getByte(arr, index1));
    }

    public static void byteField() {
        UnsafeObjectTest obj = new UnsafeObjectTest();
        long bOffset = objectFieldOffset("b");

        System.out.println((byte)0 == unsafe.getByte(obj, bOffset));
        unsafe.putByte(obj, bOffset, (byte)17);
        System.out.println((byte)17 == unsafe.getByte(obj, bOffset));
    }

    public static void charArray() {
        char[] arr = {'x', 'y', 'z'};

        long charArrBaseOffset = unsafe.arrayBaseOffset(char[].class);
        long charArrIndexScale = unsafe.arrayIndexScale(char[].class);
        long index1 = charArrBaseOffset + charArrIndexScale;

        System.out.println('y' == unsafe.getChar(arr, index1));
        unsafe.putChar(arr, index1, 'a');
        System.out.println('a' == unsafe.getChar(arr, index1));
    }

    public static void charField() {
        UnsafeObjectTest obj = new UnsafeObjectTest();
        long cOffset = objectFieldOffset("c");

        System.out.println('\0' == unsafe.getChar(obj, cOffset));
        unsafe.putChar(obj, cOffset, 'x');
        System.out.println('x' == unsafe.getChar(obj, cOffset));
    }

    public static void shortArray() {
        short[] arr = {3, 4, 5};

        long shortArrBaseOffset = unsafe.arrayBaseOffset(short[].class);
        long shortArrIndexScale = unsafe.arrayIndexScale(short[].class);
        long index1 = shortArrBaseOffset + shortArrIndexScale;

        System.out.println(4 == unsafe.getShort(arr, index1));
        unsafe.putShort(arr, index1, (short)12345);
        System.out.println((short)12345 == unsafe.getShort(arr, index1));
    }

    public static void shortField() {
        UnsafeObjectTest obj = new UnsafeObjectTest();
        long sOffset = objectFieldOffset("s");

        System.out.println((short)0 == unsafe.getShort(obj, sOffset));
        unsafe.putShort(obj, sOffset, (short)12345);
        System.out.println((short)12345 == unsafe.getShort(obj, sOffset));
    }

    public static void intArray() {
        int[] arr = {3, 4, 5};

        long intArrBaseOffset = unsafe.arrayBaseOffset(int[].class);
        long intArrIndexScale = unsafe.arrayIndexScale(int[].class);
        long index1 = intArrBaseOffset + intArrIndexScale;

        System.out.println(4 == unsafe.getInt(arr, index1));
        unsafe.putInt(arr, index1, 12345);
        System.out.println(12345 == unsafe.getInt(arr, index1));
    }

    public static void intField() {
        UnsafeObjectTest obj = new UnsafeObjectTest();
        long iOffset = objectFieldOffset("i");

        System.out.println(0 == unsafe.getInt(obj, iOffset));
        unsafe.putInt(obj, iOffset, 12345);
        System.out.println(12345 == unsafe.getInt(obj, iOffset));
    }

    public static void longArray() {
        long[] arr = {3, 4, 5};

        long longArrBaseOffset = unsafe.arrayBaseOffset(long[].class);
        long longArrIndexScale = unsafe.arrayIndexScale(long[].class);
        long index1 = longArrBaseOffset + longArrIndexScale;

        System.out.println(4L == unsafe.getLong(arr, index1));
        unsafe.putLong(arr, index1, 12345L);
        System.out.println(12345L == unsafe.getLong(arr, index1));
    }

    public static void longField() {
        UnsafeObjectTest obj = new UnsafeObjectTest();
        long jOffset = objectFieldOffset("j");

        System.out.println(0L == unsafe.getLong(obj, jOffset));
        unsafe.putLong(obj, jOffset, 12345L);
        System.out.println(12345L == unsafe.getLong(obj, jOffset));
    }

    public static void floatArray() {
        float[] arr = {1.4f, 3.14f, 0f};

        long floatArrBaseOffset = unsafe.arrayBaseOffset(float[].class);
        long floatArrIndexScale = unsafe.arrayIndexScale(float[].class);
        long index1 = floatArrBaseOffset + floatArrIndexScale;

        System.out.println(Float.compare(3.14f, unsafe.getFloat(arr, index1)) == 0);
        unsafe.putFloat(arr, index1, 2.71828f);
        System.out.println(Float.compare(2.71828f, unsafe.getFloat(arr, index1)) == 0);
    }

    public static void floatField() {
        UnsafeObjectTest obj = new UnsafeObjectTest();
        long fOffset = objectFieldOffset("f");

        System.out.println(Float.compare(0f, unsafe.getFloat(obj, fOffset)) == 0);
        unsafe.putFloat(obj, fOffset, 3.14f);
        System.out.println(Float.compare(3.14f, unsafe.getFloat(obj, fOffset)) == 0);
    }

    public static void doubleArray() {
        double[] arr = {1.4, 3.14, 0d};

        long doubleArrBaseOffset = unsafe.arrayBaseOffset(double[].class);
        long doubleArrIndexScale = unsafe.arrayIndexScale(double[].class);
        long index1 = doubleArrBaseOffset + doubleArrIndexScale;

        System.out.println(Double.compare(3.14, unsafe.getDouble(arr, index1)) == 0);
        unsafe.putDouble(arr, index1, 2.71828);
        System.out.println(Double.compare(2.71828, unsafe.getDouble(arr, index1)) == 0);
    }

    public static void doubleField() {
        UnsafeObjectTest obj = new UnsafeObjectTest();
        long dOffset = objectFieldOffset("d");

        System.out.println(0 == unsafe.getDouble(obj, dOffset));
        unsafe.putDouble(obj, dOffset, 3.14);
        System.out.println(Double.compare(3.14, unsafe.getDouble(obj, dOffset)) == 0);
    }

    public static void objectArray() {
        String[] arr = {"a", "b", "c"};

        long objectArrBaseOffset = unsafe.arrayBaseOffset(String[].class);
        long objectArrIndexScale = unsafe.arrayIndexScale(String[].class);
        long index1 = objectArrBaseOffset + objectArrIndexScale;

        System.out.println("b" == unsafe.getObject(arr, index1));
        unsafe.putObject(arr, index1, "hello");
        System.out.println("hello" == unsafe.getObject(arr, index1));
    }
    
    public static void objectField() {
        UnsafeObjectTest obj = new UnsafeObjectTest();
        long strOffset = objectFieldOffset("str");

        System.out.println(null == unsafe.getObject(obj, strOffset));
        unsafe.putObject(obj, strOffset, "world");
        System.out.println("world" == unsafe.getObject(obj, strOffset));
    }

    private static long objectFieldOffset(String fieldName) {
        try {
            Field f = UnsafeObjectTest.class.getDeclaredField(fieldName);
            return unsafe.objectFieldOffset(f);
        } catch (NoSuchFieldException e) {
            throw new RuntimeException(e);
        }
    }

    public static void main(String[] args) {
        booleanArray();
        booleanField();

        byteArray();
        byteField();

        charArray();
        charField();

        shortArray();
        shortField();

        intArray();
        intField();

        longArray();
        longField();

        floatArray();
        floatField();

        doubleArray();
        doubleField();

        objectArray();
        objectField();
    }

}
