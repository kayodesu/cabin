package sunmisc;

import sun.misc.Unsafe;

/**
 * Status: Pass
 */
public class UnsafeObjectTest {
    
    private static final Unsafe unsafe = UnsafeTest.getUnsafe();

    public static void booleanArray() {
        boolean[] arr = {false, true, false};

        long booleanArrBaseOffset = unsafe.arrayBaseOffset(boolean[].class);
        long booleanArrIndexScale = unsafe.arrayIndexScale(boolean[].class);
        long index1 = booleanArrBaseOffset + booleanArrIndexScale;

        System.out.println(unsafe.getBoolean(arr, index1));
        unsafe.putBoolean(arr, index1, false);
        System.out.println(!unsafe.getBoolean(arr, index1));
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

    public static void charArray() {
        char[] arr = {'x', 'y', 'z'};

        long charArrBaseOffset = unsafe.arrayBaseOffset(char[].class);
        long charArrIndexScale = unsafe.arrayIndexScale(char[].class);
        long index1 = charArrBaseOffset + charArrIndexScale;

        System.out.println('y' == unsafe.getChar(arr, index1));
        unsafe.putChar(arr, index1, 'a');
        System.out.println('a' == unsafe.getChar(arr, index1));
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

    public static void intArray() {
        int[] arr = {3, 4, 5};

        long intArrBaseOffset = unsafe.arrayBaseOffset(int[].class);
        long intArrIndexScale = unsafe.arrayIndexScale(int[].class);
        long index1 = intArrBaseOffset + intArrIndexScale;

        System.out.println(4 == unsafe.getInt(arr, index1));
        unsafe.putInt(arr, index1, 12345);
        System.out.println(12345 == unsafe.getInt(arr, index1));
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

    public static void floatArray() {
        float[] arr = {1.4f, 3.14f, 0f};

        long floatArrBaseOffset = unsafe.arrayBaseOffset(float[].class);
        long floatArrIndexScale = unsafe.arrayIndexScale(float[].class);
        long index1 = floatArrBaseOffset + floatArrIndexScale;

        System.out.println(Float.compare(3.14f, unsafe.getFloat(arr, index1)) == 0);
        unsafe.putFloat(arr, index1, 2.71828f);
        System.out.println(Float.compare(2.71828f, unsafe.getFloat(arr, index1)) == 0);
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

    public static void objectArray() {
        String[] arr = {"a", "b", "c"};

        long objectArrBaseOffset = unsafe.arrayBaseOffset(String[].class);
        long objectArrIndexScale = unsafe.arrayIndexScale(String[].class);
        long index1 = objectArrBaseOffset + objectArrIndexScale;

        System.out.println("b" == unsafe.getObject(arr, index1));
        unsafe.putObject(arr, index1, "hello");
        System.out.println("hello" == unsafe.getObject(arr, index1));
    }

    public static void main(String[] args) {
        booleanArray();
        byteArray();
        charArray();
        shortArray();
        intArray();
        longArray();
        floatArray();
        doubleArray();
        objectArray();
    }

}
