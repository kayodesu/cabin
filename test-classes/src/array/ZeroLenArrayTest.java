package array;

/**
 * Status: Pass
 */
public class ZeroLenArrayTest {

    public static void main(String[] args) {
        int[] arr = new int[0];
        System.out.println(arr.getClass().getName());
        System.out.println(arr.length); // 0
    }
}
