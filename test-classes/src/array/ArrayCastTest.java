package array;

/**
 * Status: Pass
 */
public class ArrayCastTest {
    public static void main(String[] args) {
        try {
            int[][] arr = new int[2][3];
            Object[] o = (Object[]) arr; // OK

            int[][][] arr1 = new int[2][3][4];
            Object[] o1 = (Object[]) arr1; // OK
            Object[][] o2 = (Object[][]) arr1; // OK
        } catch (Exception e) {
            System.out.println("Fail");
            return;
        }
        System.out.println("Pass");
    }
}
