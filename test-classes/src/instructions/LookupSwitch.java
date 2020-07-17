package instructions;

/**
 * Status: Pass
 */
public class LookupSwitch {
    private static void test(int i) {
        switch (i) {
            case -100: System.out.println("-100"); break;
            case 0: System.out.println("0"); break;
            case 3: System.out.println("3"); break;
            case -200: System.out.println("-200"); break;
            case 5: System.out.println("5"); break;
            default: System.out.println("default: " + i);
        }
    }
    
    public static void main(String[] args) {
        test(-100);
        test(-200);

        for (int i = 0; i < 7; i++)
            test(i);
    }
}
