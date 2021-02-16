package array;

/**
 * Status: Pass
 */
public class LongArrayTest {

    public static void main(String[] args) {
        int len = 123456789;
        int[] arr = new int[len];
        for (int i = 0; i < len; i++) {
            arr[i] = i;
        }

        for (int i = 0; i < len; i++) {
            if (arr[i] != i) {
                System.out.println("Fail");
                return;
            }
        }

        System.out.println("Pass");
    }
}
