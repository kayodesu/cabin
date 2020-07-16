package instructions;

/**
 * Status: Pass
 */
public class NewMultiArrayTest {
    
    public static void main(String[] args) {
        int[][][] x = new int[2][3][5];
        System.out.println(x.length); // 2
        System.out.println(x[0].length); // 3
        System.out.println(x[1][2].length); // 5
        
        x[1][2][3] = 7;
        System.out.println(x[1][2][3]); // 7
    }
    
}
