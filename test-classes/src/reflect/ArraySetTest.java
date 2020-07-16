package reflect;

import java.lang.reflect.Array;

/**
 * Status: Pass
 */
public class ArraySetTest {
    public static void main(String[] args) {
        setNullArray();
        setNonArray();
        setArrayTypeMismatch();
        setArrayBadIndex();
        setObjectArray();
        setPrimitiveArray();
        setNullValue();
        setWrongTypeValue();
    }

    public static void setNullArray() {
        try {
            Object x = null;
            Array.set(x, 3, "a");
        } catch (NullPointerException e) {
            System.out.println(e.getMessage() == null);
        }
    }

    public static void setNonArray() {
        try {
            String str = "abc";
            Array.set(str, 1, "a");
        } catch (IllegalArgumentException e) {
            System.out.println("Argument is not an array".equals(e.getMessage()));
        }
    }

    public static void setArrayTypeMismatch() {
        try {
            int[] arr = {1, 2, 3};
            Array.set(arr, 0, "beyond");
        } catch (IllegalArgumentException e) {
            System.out.println("argument type mismatch".equals(e.getMessage()));
        }
    }

    public static void setArrayBadIndex() {
        try {
            int[] arr = {1, 2, 3};
            Array.set(arr, -1, 4);
        } catch (ArrayIndexOutOfBoundsException e) {
            System.out.println("Pass");
        }
    }

    public static void setObjectArray() {
        String[] arr = {"beyond"};
        Array.set(arr, 0, "5457");
        System.out.println("5457" == Array.get(arr, 0));
    }

    public static void setPrimitiveArray() {
        Array.set(new boolean[]{true}, 0, false);
        Array.set(new byte[]{2}, 0, (byte) 3);
        Array.set(new char[]{'a'}, 0, 'b');
        Array.set(new short[]{2}, 0, (short) 3);
        Array.set(new int[]{2}, 0, 3);
        Array.set(new long[]{2}, 0, 3L);
        Array.set(new float[]{3.14f}, 0, 2.71f);
        Array.set(new double[]{2.71}, 0, 3.14);

        int[] arr = {5, 4, 5, 7};
        Array.set(arr, 0, 0);
        System.out.println(0 == (Integer) Array.get(arr, 0));
    }

    public static void setNullValue() {
        try {
            int[] arr = {1, 2, 3};
            Array.set(arr, 0, null);
        } catch (IllegalArgumentException e) {
            System.out.println("Pass");
        }

        Object[] arr = {1, 2, 3};
        Array.set(arr, 0, null); // OK
    }

    public static void setWrongTypeValue() {
        try {
            char[] aaa = { 'c' };
            Array.set(aaa, 0, new Integer(3));
        } catch (IllegalArgumentException e) {
            System.out.println("Pass");
        }
    }
}
