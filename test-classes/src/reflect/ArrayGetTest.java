package reflect;

import java.lang.reflect.Array;

/**
 * Status: Fail
 */
public class ArrayGetTest {

    public static void main(String[] args) {
        getNullArray();
        getNonArray();
        getArrayBadIndex();
        getObjectArray();
        getPrimitiveArray();
    }

    public static void getNullArray() {
        try {
            Object x = null;
            Array.get(x, 3);
        } catch (NullPointerException e) {
            System.out.println("Pass");
        }
    }

    public static void getNonArray() {
        try {
            String str = "abc";
            Array.get(str, 1);
        } catch (IllegalArgumentException e) {
            System.out.println("Pass");
        }
    }

    public static void getArrayBadIndex() {
        try {
            int[] arr = {1, 2, 3};
            Array.get(arr, -1);
        } catch (ArrayIndexOutOfBoundsException e) {
            System.out.println("Pass");
        }
    }

    public static void getObjectArray() {
        String[] arr = {"a", "b", "c"};
        System.out.println("c" == Array.get(arr, 2) ? "Pass" : "Fail");
    }

    public static void getPrimitiveArray() {
        System.out.println((Boolean) Array.get(new boolean[]{true}, 0));
        System.out.println((byte) 2 == (Byte) Array.get(new byte[]{2}, 0));
        System.out.println('a' == (Character) Array.get(new char[]{'a'}, 0));
        System.out.println((short) 2 == (Short) Array.get(new short[]{2}, 0));
        System.out.println(2 == (Integer) Array.get(new int[]{2}, 0));
        System.out.println(2L == (Long) Array.get(new long[]{2}, 0));
        System.out.println(3.14f == (Float) Array.get(new float[]{3.14f}, 0));
        System.out.println(2.71 == (Double) Array.get(new double[]{2.71}, 0));
    }

}
