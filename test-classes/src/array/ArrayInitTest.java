package array;

import java.util.Arrays;

/**
 * Status: Pass
 */
public class ArrayInitTest {
    public static void main(String[] args) {
        // The refernces inside the array are 
        // automatically initialized to null.
        System.out.println(Arrays.toString(new Object[5]));
        
        // The primitives inside the array are 
        // automatically initialized to zero.
        System.out.println(Arrays.toString(new int[5]));
    }
}
