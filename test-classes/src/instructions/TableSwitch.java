package instructions;

/**
 * Status: Pass
 */
public class TableSwitch {
    private static void test(int i) {
        switch (i) {
            case 3: System.out.println("3"); break;
            case 4: System.out.println("4"); break;
            case 5: System.out.println("5"); break;
            default: System.out.println("default");
        }
    }
    
    public static void main(String[] args) {
        for (int i = 1; i < 7; i++)
            test(i);
    }
}
