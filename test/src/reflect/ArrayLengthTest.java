package reflect;

import java.lang.reflect.Array;

/**
 * Status: Pass
 */
public class ArrayLengthTest {
    public static void main(String[] args) {
        nullArrayLength();
        nonArrayLength();
        arrayLength();
    }

    public static void nullArrayLength() {
        try {
            Object x = null;
            Array.getLength(x);
        } catch (NullPointerException e) {
            System.out.println("Pass");
        }
    }

    public static void nonArrayLength() {
        try {
            Array.getLength("A");
        } catch (IllegalArgumentException e) {
            System.out.println("Pass");
        }
    }

    public static void arrayLength() {
        System.out.println(Array.getLength(new int[0]) == 0);
        System.out.println(Array.getLength(new int[3]) == 3);
    }
}
